/*
 * lombo_i2s.c - Common code for LomboTech SoCs
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
#include <linux/gpio.h>
#include <linux/module.h>

#include <sound/core.h>
#include <sound/soc.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/dmaengine_pcm.h>
#include <mach/debug.h>
#include <mach/csp.h>
#include <mach/common.h>

#include "csp/i2s/csp_i2s.h"
#include "lombo_i2s.h"
#include "lombo_mock_dma.h"

#undef  MOD_NAME
#define MOD_NAME "N7_I2S"

#define I2S_ON			1
#define I2S_OFF			0

#define CHANNELS		2
#define DMA_BUFFER_MAX		(1024*128)
#define PERIOD_BYTES_MIN	PAGE_SIZE
#define PERIOD_BYTES_MAX	(PAGE_SIZE*2)
#define PERIOD_MIN		1
#define PERIOD_MAX		64
#define PREALLOC_PCM_BUFFER	(1024*128)


/* smaple rates supportted ty I2S */
#define LOMBO_I2S_RATES \
	(SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 | SNDRV_PCM_RATE_16000 | \
	SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 | \
	SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

/* smaple_bits supportted ty I2S */
#define LOMBO_I2S_FORMAT \
	(SNDRV_PCM_FORMAT_U8 | SNDRV_PCM_FORMAT_S16_LE | \
	SNDRV_PCM_FORMAT_S24_LE | SNDRV_PCM_FORMAT_S32_LE)

/* define pcm dma params */
static const struct snd_pcm_hardware lombo_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_INTERLEAVED |
					SNDRV_PCM_INFO_BLOCK_TRANSFER |
					SNDRV_PCM_INFO_MMAP |
					SNDRV_PCM_INFO_MMAP_VALID,
	.formats		= LOMBO_I2S_FORMAT,
	.channels_min		= CHANNELS,
	.channels_max		= CHANNELS,
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


struct lombo_i2s_info {
	struct device	*dev;
	void __iomem	*regs; /* base addr of i2s in the io space */
	int		irq;
	int		use_cpu;
	int		clk_rate;
	int		parent_id;
	struct clk	*i2s_clk;	/* i2s clk */
	struct clk	*parent_clk[2];
	struct clk	*i2s_apb_gate;
	struct clk	*i2s_apb_reset;
	struct snd_dmaengine_dai_dma_data capture_dma_data;
	struct snd_dmaengine_dai_dma_data playback_dma_data;
};

/* get the i2s_info in the dev associated with the dai */
static inline struct lombo_i2s_info *to_i2s_info(struct snd_soc_dai *dai)
{
	return snd_soc_dai_get_drvdata(dai);
}

/* func to control the i2s tx */
static void lombo_snd_txctrl(struct lombo_i2s_info *i2s_info, int on)
{
	PRT_DBG("Entered\n");
	if (on)
		csp_i2s_tx_open(i2s_info->regs);
	else
		csp_i2s_tx_close(i2s_info->regs);
}

/* func to control the i2s rx */
static void lombo_snd_rxctrl(struct lombo_i2s_info *i2s_info, int on)
{
	PRT_DBG("Entered\n");
	if (on)
		csp_i2s_rx_open(i2s_info->regs);
	else
		csp_i2s_rx_close(i2s_info->regs);
}

/* set i2s m/s mode, i2s data fmt */
static int lombo_i2s_set_format(struct snd_soc_dai *cpu_dai,
		unsigned int fmt)
{
	struct lombo_i2s_info *i2s_info = to_i2s_info(cpu_dai);
	PRT_DBG("Entered\n");
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	/* SND_SOC_DAIFMT_CBM_CFM means master for codec,
	 * so it's slave for i2s
	 */
	case SND_SOC_DAIFMT_CBM_CFM:
		csp_i2s_set_ms_mode(i2s_info->regs, I2S_SLAVE);
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		csp_i2s_set_ms_mode(i2s_info->regs, I2S_MASTER);
		break;
	default:
		return -EINVAL;
	}
	/* set i2s data fmt */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_LEFT_J:
		csp_i2s_set_format(i2s_info->regs, I2S_MSB);
		break;
	case SND_SOC_DAIFMT_I2S:
		csp_i2s_set_format(i2s_info->regs, I2S_STANDARD);
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		csp_i2s_set_format(i2s_info->regs, I2S_LSB);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

/* set I2S data bitwidth & dma data bitwidth */
static int lombo_i2s_hw_params(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *params,
				 struct snd_soc_dai *dai)
{
	struct snd_dmaengine_dai_dma_data *dma_data;
	struct lombo_i2s_info *i2s_info;

	i2s_info = to_i2s_info(dai);
	dma_data = snd_soc_dai_get_dma_data(dai, substream);
	/* set i2s wlen, dma data width according to bitwidth */
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_U8:
		csp_i2s_set_wlen(i2s_info->regs, 8, substream->stream);
		dma_data->addr_width = 2;
		break;
	case SNDRV_PCM_FORMAT_S16_LE:
		csp_i2s_set_wlen(i2s_info->regs, 16, substream->stream);
		dma_data->addr_width = 2;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		csp_i2s_set_wlen(i2s_info->regs, 24, substream->stream);
		dma_data->addr_width = 4;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		csp_i2s_set_wlen(i2s_info->regs, 32, substream->stream);
		dma_data->addr_width = 4;
		break;
	default:
		return -EINVAL;
	}
	PRT_DBG("dma_data->addr_width %d\n", dma_data->addr_width);
	return 0;
}

/* func to trigger tramsmit */
static int lombo_i2s_trigger(struct snd_pcm_substream *substream, int cmd,
			       struct snd_soc_dai *dai)
{
	int ret = 0;
	struct lombo_i2s_info *i2s_info = to_i2s_info(dai);

	PRT_DBG("entered\n");

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		PRT_DBG("trigger start\n");
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			lombo_snd_rxctrl(i2s_info, I2S_ON);
		else
			lombo_snd_txctrl(i2s_info, I2S_ON);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		PRT_DBG("trigger stop\n");
		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
			lombo_snd_rxctrl(i2s_info, I2S_OFF);
		else
			lombo_snd_txctrl(i2s_info, I2S_OFF);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

/* set i2s clk divider */
static int lombo_i2s_set_clkdiv(struct snd_soc_dai *cpu_dai,
	int div_id, int div)
{
	struct lombo_i2s_info *i2s_info = to_i2s_info(cpu_dai);
	PRT_DBG("set clk div id %d, div %d\n", div_id, div);
	csp_i2s_set_div(i2s_info->regs, div_id, div);
	return 0;
}


/* set the i2s clk and clk rate */
static int lombo_i2s_set_sysclk(struct snd_soc_dai *cpu_dai,
	int clk_id, unsigned int freq, int dir)
{
	int ret = 0;
	struct lombo_i2s_info *i2s_info = to_i2s_info(cpu_dai);

	i2s_info->parent_id = clk_id;
	ret = clk_set_parent(i2s_info->i2s_clk, i2s_info->parent_clk[clk_id]);
	if (ret)
		dev_err(i2s_info->dev, "Fail to set clk parent %d\n", ret);

	ret = clk_set_rate(i2s_info->i2s_clk, freq);
	if (ret)
		dev_err(i2s_info->dev, "Fail to set clk %d\n", ret);
	return ret;
}

/* i2s dai probe func */
static int lombo_i2s_dai_probe(struct snd_soc_dai *dai)
{
	struct lombo_i2s_info *i2s_info = to_i2s_info(dai);

	if (i2s_info == NULL) {
		PRT_ERR("i2s_info null error !!\n");
		return -1;
	}
	/* set dma params */
	dai->capture_dma_data = &i2s_info->capture_dma_data;
	dai->playback_dma_data = &i2s_info->playback_dma_data;

	/* enable i2s module */
	csp_i2s_enable(i2s_info->regs);

	/* set the wss to default value 32 */
	csp_i2s_set_wss(i2s_info->regs, 32);

	return 0;
}

/* define dai ops */
static const struct snd_soc_dai_ops lombo_i2s_dai_ops = {
	.trigger	= lombo_i2s_trigger,
	.hw_params	= lombo_i2s_hw_params,
	.set_fmt	= lombo_i2s_set_format,
	.set_clkdiv	= lombo_i2s_set_clkdiv,
	.set_sysclk	= lombo_i2s_set_sysclk,
};

/* define I2S dai driver */
static struct snd_soc_dai_driver lombo_i2s_dai = {
	.probe		= lombo_i2s_dai_probe,
	.playback = {
		.channels_min = CHANNELS,
		.channels_max = CHANNELS,
		.rates = LOMBO_I2S_RATES,
		.formats = LOMBO_I2S_FORMAT,},
	.capture = {
		.channels_min = CHANNELS,
		.channels_max = CHANNELS,
		.rates = LOMBO_I2S_RATES,
		.formats = LOMBO_I2S_FORMAT,},
	.ops = &lombo_i2s_dai_ops,
};

static const struct snd_soc_component_driver lombo_i2s_component = {
	.name		= "lombo-i2s",
};

/* i2s platform driver probe func */
static int lombo_i2s_dev_probe(struct platform_device *pdev)
{
	int ret = 0;
	u32 p1, p2;
	struct lombo_i2s_info *i2s_info;
	struct resource *res;

	PRT_DBG("Entered\n");

	i2s_info = devm_kzalloc(&pdev->dev, sizeof(*i2s_info), GFP_KERNEL);
	if (!i2s_info) {
		dev_err(&pdev->dev, "alloc lombo_i2s_info failed !!\n");
		return -ENOMEM;
	}

	/* get platform device resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "can't get io resources\n");
		return -ENOENT;
	}

	/* ioremap the registers */
	i2s_info->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(i2s_info->regs))
		return PTR_ERR(i2s_info->regs);

	i2s_info->irq = platform_get_irq(pdev, 0);
	PRT_INFO("i2s irq %d\n", i2s_info->irq);

	/* set pcm dma addr & bustwidth */
	i2s_info->capture_dma_data.addr = res->start + csp_i2s_dmaaddr_in();
	i2s_info->playback_dma_data.addr = res->start + csp_i2s_dmaaddr_out();
	i2s_info->capture_dma_data.maxburst = 1;
	i2s_info->playback_dma_data.maxburst = 1;
	i2s_info->i2s_apb_gate = devm_clk_get(&pdev->dev, "i2s_apb_gate");
	if (IS_ERR(i2s_info->i2s_apb_gate)) {
		dev_err(&pdev->dev, "get i2s_apb_gate error\n");
		return PTR_ERR(i2s_info->i2s_apb_gate);
	}
	ret = clk_prepare_enable(i2s_info->i2s_apb_gate);
	if (ret) {
		dev_err(&pdev->dev, "enable i2s_apb_gate error\n");
		return ret;
	}
	i2s_info->i2s_apb_reset = devm_clk_get(&pdev->dev, "i2s_apb_reset");
	if (IS_ERR(i2s_info->i2s_apb_reset)) {
		dev_err(&pdev->dev, "get i2s_apb_reset error\n");
		return PTR_ERR(i2s_info->i2s_apb_reset);
	}
	ret = clk_prepare_enable(i2s_info->i2s_apb_reset);
	if (ret) {
		dev_err(&pdev->dev, "enable i2s_apb_reset error\n");
		return ret;
	}

	/* get i2s-clock and enable it */
	i2s_info->i2s_clk = devm_clk_get(&pdev->dev, "i2s_module_clk");
	if (IS_ERR(i2s_info->i2s_clk)) {
		dev_err(&pdev->dev, "get i2s-clk error\n");
		return PTR_ERR(i2s_info->i2s_clk);
	}
	ret = clk_prepare_enable(i2s_info->i2s_clk);
	if (ret) {
		dev_err(&pdev->dev, "enable i2s-clk error\n");
		return ret;
	}

	i2s_info->parent_clk[0] = clk_get(NULL, "audio_pll_div7");
	if (IS_ERR(i2s_info->parent_clk[0])) {
		dev_err(&pdev->dev, "get audio_pll_div7 error\n");
		return PTR_ERR(i2s_info->parent_clk[0]);
	}

	i2s_info->parent_clk[1] = clk_get(NULL, "audio_pll_div17");
	if (IS_ERR(i2s_info->parent_clk[1])) {
		dev_err(&pdev->dev, "get audio_pll_div17 error\n");
		return PTR_ERR(i2s_info->parent_clk[1]);
	}
	ret = clk_set_parent(i2s_info->i2s_clk, i2s_info->parent_clk[0]);
	if (ret)
		dev_err(i2s_info->dev, "Fail to set clk parent %d\n", ret);
	ret = clk_set_rate(i2s_info->i2s_clk, 24571000);
	if (ret)
		dev_err(i2s_info->dev, "Fail to set clk %d\n", ret);

	/* the i2s_info will be set to dev, and dai also have the pointer
	 * to dev, so we can't get i2s_info from dai later */
	dev_set_drvdata(&pdev->dev, i2s_info);

	/* register i2s dai */
	ret = snd_soc_register_component(&pdev->dev, &lombo_i2s_component,
					 &lombo_i2s_dai, 1);
	if (ret) {
		PRT_ERR("failed to register the dai\n");
		return ret;
	}

	if (lombo_func2(&p1, &p2) == 0) {
		u32 dma_disabled = boot_get_dma_status();
		if (p1 == 1 && p2 == 0 && dma_disabled)
			i2s_info->use_cpu = 1;
		else
			i2s_info->use_cpu = 0;
		PRT_INFO("p1 %x, p2 %x, dma_disabled %d, usecpu %d\n",
			p1, p2, dma_disabled, i2s_info->use_cpu);
	}

	if (i2s_info->use_cpu) {
		struct mock_pcm_config config;
		config.i2s_base = i2s_info->regs;
		config.i2s_irq = i2s_info->irq;
		config.pcm_hardware = &lombo_pcm_hardware;
		config.prealloc_buffer_size = PREALLOC_PCM_BUFFER;
		ret = mock_dma_platform_register(&pdev->dev, &config);
		if (ret) {
			PRT_ERR("failed to register mock dma: %d !!\n", ret);
			goto err;
		}
	} else {
		/* register pcm platform driver */
		ret = snd_dmaengine_pcm_register(&pdev->dev,
			&lombo_dmaengine_pcm_config,
			SND_DMAENGINE_PCM_FLAG_COMPAT |
			SND_DMAENGINE_PCM_FLAG_NO_RESIDUE);
		if (ret) {
			PRT_ERR("failed to register the dma: %d !!\n", ret);
			goto err;
		}
	}

	return 0;
err:
	snd_soc_unregister_component(&pdev->dev);
	return ret;
}

static int lombo_i2s_dev_remove(struct platform_device *pdev)
{
	struct lombo_i2s_info *i2s_info = platform_get_drvdata(pdev);

	/* release the parent clk */
	clk_put(i2s_info->parent_clk[0]);
	clk_put(i2s_info->parent_clk[1]);
	if (i2s_info->use_cpu)
		mock_dma_platform_unregister(&pdev->dev);
	else
		snd_dmaengine_pcm_unregister(&pdev->dev);

	snd_soc_unregister_component(&pdev->dev);
	return 0;
}

#ifdef CONFIG_PM
static int lombo_i2s_suspend(struct device *_dev)
{
	struct lombo_i2s_info *i2s_info = dev_get_drvdata(_dev);
	struct clk *null_clk = NULL;

	PRT_DBG("====enter=====\n");
	i2s_info->clk_rate = clk_get_rate(i2s_info->i2s_clk);
	/* set i2s clk parent to null, and disable it */
	null_clk = clk_get(NULL, "null_clk");
	clk_set_parent(i2s_info->i2s_clk, null_clk);
	clk_put(null_clk);
	clk_disable_unprepare(i2s_info->i2s_clk);
	clk_disable_unprepare(i2s_info->i2s_apb_gate);
	clk_disable_unprepare(i2s_info->i2s_apb_reset);
	/* TODO: maybe we need to cache some regs here */

	return 0;
}

static int lombo_i2s_resume(struct device *_dev)
{
	int ret;
	struct lombo_i2s_info *i2s_info = dev_get_drvdata(_dev);

	PRT_DBG("====enter=====\n");
	ret = clk_prepare_enable(i2s_info->i2s_apb_gate);
	ret = clk_prepare_enable(i2s_info->i2s_apb_reset);
	ret = clk_set_parent(i2s_info->i2s_clk,
		i2s_info->parent_clk[i2s_info->parent_id]);
	ret = clk_prepare_enable(i2s_info->i2s_clk);
	clk_set_rate(i2s_info->i2s_clk, i2s_info->clk_rate);

	/* enable i2s module */
	csp_i2s_enable(i2s_info->regs);

	/* set the wss to default value 32 */
	csp_i2s_set_wss(i2s_info->regs, 32);

	return ret;
}

static const struct dev_pm_ops lombo_i2s_pmops = {
	.suspend = lombo_i2s_suspend,
	.resume = lombo_i2s_resume,
};
#else
#define lombo_i2s_pmops NULL
#endif

static struct of_device_id lombo_i2s_match[] = {
	{ .compatible = "lombo, n7-i2s", },
	{ /* end */},
};

static struct platform_driver lombo_i2s_driver = {
	.probe  = lombo_i2s_dev_probe,
	.remove = lombo_i2s_dev_remove,
	.driver = {
		.name = "lombo-i2s",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(lombo_i2s_match),
		.pm = &lombo_i2s_pmops,
	},
};

module_platform_driver(lombo_i2s_driver);

/* Module information */
MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("lombo I2S SoC Interface");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:lombo-i2s");

