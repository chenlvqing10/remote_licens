/*
 * lombo_audio_dai.c - Common code for LomboTech SoCs
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>

#include <sound/core.h>
#include <sound/soc.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/dmaengine_pcm.h>
#include <mach/debug.h>
#include <mach/csp.h>
#include <mach/common.h>

#undef  MOD_NAME
#define MOD_NAME "LOMBO_AUDIO_DAI"

#define CHANNELS_MIN		1
#define CHANNELS_MAX		2
#define DMA_BUFFER_MAX		(1024*64)
#define PERIOD_BYTES_MIN	32
#define PERIOD_BYTES_MAX	(PAGE_SIZE*2)
#define PERIOD_MIN		1
#define PERIOD_MAX		64
#define PREALLOC_PCM_BUFFER	(1024*64)

#define LOMBO_RXFIFO_ADDR	0x04010818
#define LOMBO_TXFIFO_ADDR	0x0401081C

/* smaple rates supportted ty I2S */
#define LOMBO_AUDIO_RATES \
	(SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 | SNDRV_PCM_RATE_16000 | \
	SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 | \
	SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

/* smaple_bits supportted ty I2S */
#define LOMBO_AUDIO_FORMAT \
	(SNDRV_PCM_FORMAT_U8 | SNDRV_PCM_FORMAT_S16_LE | \
	SNDRV_PCM_FORMAT_S24_LE | SNDRV_PCM_FORMAT_S32_LE)


struct lombo_audio_info {
	struct device	*dev;
	struct snd_dmaengine_dai_dma_data capture_dma_data;
	struct snd_dmaengine_dai_dma_data playback_dma_data;
};

/* define pcm dma params */
static const struct snd_pcm_hardware lombo_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_INTERLEAVED |
					SNDRV_PCM_INFO_BLOCK_TRANSFER |
					SNDRV_PCM_INFO_MMAP |
					SNDRV_PCM_INFO_MMAP_VALID,
	/* max dma buffer bytes */
	.buffer_bytes_max	= DMA_BUFFER_MAX,
	/* min bytes for one period */
	.period_bytes_min	= PERIOD_BYTES_MIN,
	/* max bytes for one period */
	.period_bytes_max	= PERIOD_BYTES_MAX,
	.periods_min		= PERIOD_MIN,		/* max periods */
	.periods_max		= PERIOD_MAX,		/* min periods */
};

static int dma_slave_config(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params, struct dma_slave_config *slave_config)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_dmaengine_dai_dma_data *dma_data;
	int ret;
	dma_data = snd_soc_dai_get_dma_data(rtd->cpu_dai, substream);
	ret = snd_hwparams_to_dma_slave_config(substream, params, slave_config);
	if (ret)
		return ret;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		slave_config->dst_addr = dma_data->addr;
		slave_config->dst_maxburst = dma_data->maxburst;
		slave_config->src_maxburst = dma_data->maxburst;
	} else {
		slave_config->src_addr = dma_data->addr;
		slave_config->src_maxburst = dma_data->maxburst;
		slave_config->dst_maxburst = dma_data->maxburst;
	}
	if (dma_data->addr_width != DMA_SLAVE_BUSWIDTH_UNDEFINED) {
		slave_config->dst_addr_width = dma_data->addr_width;
		slave_config->src_addr_width = dma_data->addr_width;
	}
	slave_config->slave_id = dma_data->slave_id;
	return 0;
}
/* define damengine config */
/* todo: maybe we need to implement compat_request_channel callback func
 * to request a dma channel */
static const struct snd_dmaengine_pcm_config lombo_dmaengine_pcm_config = {
	.prepare_slave_config	= dma_slave_config,
	.prealloc_buffer_size	= PREALLOC_PCM_BUFFER,
	.pcm_hardware		= &lombo_pcm_hardware,
};

/* set I2S data bitwidth & dma data bitwidth */
static int lombo_audio_hw_params(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *params,
				 struct snd_soc_dai *dai)
{
	struct snd_dmaengine_dai_dma_data *dma_data;
	struct lombo_audio_info *info;

	info = snd_soc_dai_get_drvdata(dai);
	dma_data = snd_soc_dai_get_dma_data(dai, substream);
	/* set i2s wlen, dma data width according to bitwidth */
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_U8:
	case SNDRV_PCM_FORMAT_S16_LE:
		dma_data->addr_width = 2;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S32_LE:
		dma_data->addr_width = 4;
		break;
	default:
		return -EINVAL;
	}
	PRT_DBG("dma_data->addr_width %d\n", dma_data->addr_width);
	return 0;
}


/* lombo dai probe func */
static int lombo_dai_probe(struct snd_soc_dai *dai)
{
	struct lombo_audio_info *info = snd_soc_dai_get_drvdata(dai);

	if (info == NULL) {
		PRT_ERR("audio_info null error !!\n");
		return -1;
	}
	/* set dma params */
	dai->capture_dma_data = &info->capture_dma_data;
	dai->playback_dma_data = &info->playback_dma_data;

	return 0;
}

static int lombo_dai_suspend(struct snd_soc_dai *cpu_dai)
{
	PRT_DBG("Entered\n");
	return 0;
}

static int lombo_dai_resume(struct snd_soc_dai *cpu_dai)
{
	PRT_DBG("Entered\n");
	return 0;
}

/* define dai ops */
static const struct snd_soc_dai_ops lombo_audio_dai_ops = {
	.hw_params	= lombo_audio_hw_params,
};

/* define lomb audio dai driver */
static struct snd_soc_dai_driver lombo_audio_dai = {
	.name		= "lombo_dai",
	.probe		= lombo_dai_probe,
	.suspend	= lombo_dai_suspend,
	.resume		= lombo_dai_resume,
	.playback = {
		.stream_name = "Playback",
		.channels_min = CHANNELS_MIN,
		.channels_max = CHANNELS_MAX,
		.rates = LOMBO_AUDIO_RATES,
		.formats = LOMBO_AUDIO_FORMAT,},
	.capture = {
		.stream_name = "Capture",
		.channels_min = CHANNELS_MIN,
		.channels_max = CHANNELS_MAX,
		.rates = LOMBO_AUDIO_RATES,
		.formats = LOMBO_AUDIO_FORMAT,},
	.ops = &lombo_audio_dai_ops,
};

static const struct snd_soc_component_driver lombo_dai_component = {
	.name		= "lombo_dai",
};

/* audio dai platform driver probe func */
static int lombo_audio_dev_probe(struct platform_device *pdev)
{
	int ret;
	struct lombo_audio_info *info;

	PRT_DBG("Entered\n");

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "alloc lombo_audio_info failed !!\n");
		return -ENOMEM;
	}
	dev_set_drvdata(&pdev->dev, info);

	/* set pcm dma addr & bustwidth */
	info->capture_dma_data.addr = LOMBO_RXFIFO_ADDR;
	info->playback_dma_data.addr = LOMBO_TXFIFO_ADDR;
	info->capture_dma_data.maxburst = 4;
	info->playback_dma_data.maxburst = 4;

	/* register audio dai */
	ret = snd_soc_register_component(&pdev->dev, &lombo_dai_component,
					 &lombo_audio_dai, 1);
	if (ret) {
		PRT_ERR("failed to register the dai\n");
		return ret;
	}

	/* register pcm platform driver */
	ret = snd_dmaengine_pcm_register(&pdev->dev,
		&lombo_dmaengine_pcm_config,
		SND_DMAENGINE_PCM_FLAG_COMPAT |
		SND_DMAENGINE_PCM_FLAG_NO_RESIDUE);
	if (ret) {
		PRT_ERR("failed to register the dma: %d !!\n", ret);
		goto err;
	}

	return 0;
err:
	snd_soc_unregister_component(&pdev->dev);
	return ret;

}

static int lombo_audio_dev_remove(struct platform_device *pdev)
{
	snd_dmaengine_pcm_unregister(&pdev->dev);
	snd_soc_unregister_component(&pdev->dev);

	return 0;
}

static const struct of_device_id lombo_audio_match[] = {
	{ .compatible = "lombo, n9-dai", },
	{ /* end */},
};

static struct platform_driver lombo_audio_driver = {
	.probe  = lombo_audio_dev_probe,
	.remove = lombo_audio_dev_remove,
	.driver = {
		.name = "lombo-dai",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(lombo_audio_match),
	},
};

module_platform_driver(lombo_audio_driver);

/* Module information */
MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("lombo audio SoC Interface");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:lombo-audio");

