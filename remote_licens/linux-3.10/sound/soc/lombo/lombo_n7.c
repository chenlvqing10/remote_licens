/*
 * lombo_n7.c - Common code for LomboTech SoCs
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

#include <linux/module.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

#include <asm/mach-types.h>
#include <mach/debug.h>

#include "csp/i2s/csp_i2s.h"

#undef  MOD_NAME
#define MOD_NAME "N7_I2S"
#define AUDIO_PLL_48000S		(24571000U)
#define AUDIO_PLL_44100S		(22588200U)
#define DEFAULT_WSS			(32)

struct i2s_div {
	int rate;
	int bclk_div;
	int mclk_div;
};

/* we use 256fs here, so:
 * mclk_div = clk_rate/(256*sample_rate) - 1 */
static struct i2s_div div_table[] = {
	{8000,   47, 11},
	{16000,  23, 5},
	{24000,  15, 3},
	{32000,  11, 2},
	{48000,  7,  1},
	{96000,  3,  0},
	{11025,  31, 7},
	{22050,  15, 3},
	{44100,  7,  1},
	{88200,  3,  0},
};
struct i2s_div *find_div(int rate)
{
	int i, size;
	size = ARRAY_SIZE(div_table);
	for (i = 0; i < size; i++) {
		if (rate == div_table[i].rate)
			return &div_table[i];
	}
	return NULL;
}
static int lombo_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	int ret;
	int clk_rate, clk_id;
	struct i2s_div *clk_div;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	int sample_rate = params_rate(params);
	PRT_DBG("samle rate %d\n", sample_rate);
	switch (sample_rate) {
	case 8000:
	case 16000:
	case 24000:
	case 32000:
	case 48000:
	case 96000:
	case 192000:
		clk_id	 = 0;
		clk_rate = AUDIO_PLL_48000S;
		break;
	case 11025:
	case 22050:
	case 44100:
	case 88200:
		clk_id	 = 1;
		clk_rate = AUDIO_PLL_44100S;
		break;
	default:
		return -EINVAL;
	}
	/* set the clk rate */
	ret = snd_soc_dai_set_sysclk(cpu_dai, clk_id,
		clk_rate, SND_SOC_CLOCK_OUT);
	if (ret < 0) {
		dev_err(cpu_dai->dev, "set sysclk error\n");
		return ret;
	}
	/* calculate the divider */
	clk_div = find_div(sample_rate);
	if (!clk_div) {
		PRT_ERR("can't fine div for rate %d\n", sample_rate);
		return -1;
	}

	/* set codec m/s mode, i2s data fmt */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S
					 | SND_SOC_DAIFMT_NB_NF
					 | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	/* set i2s m/s mode, i2s data fmt */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S
					 | SND_SOC_DAIFMT_NB_NF
					 | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, LOMBO_DIV_MCLK,
		clk_div->mclk_div);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, LOMBO_DIV_BCLK,
		clk_div->bclk_div);
	if (ret < 0)
		return ret;
	return 0;
}

/*
 * n7 DAI operations.
 */
static struct snd_soc_ops lombo_soc_ops = {
	.hw_params = lombo_hw_params,
};

/* define dai_link */
static struct snd_soc_dai_link n7_dai_link = {
	.name		= "n7-ac",
	.stream_name	= "audio",
	.codec_name	= "4010800.ac",
	.codec_dai_name = "n7ac-dai",
	.cpu_dai_name	= "4010000.i2s",
	.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS,
	.ops		= &lombo_soc_ops,
	.platform_name	= "4010000.i2s",
};

/* define sound card */
static struct snd_soc_card snd_soc_n7 = {
	.name		= "n7-card",
	.owner		= THIS_MODULE,
	.dai_link	= &n7_dai_link,
	.num_links	= 1,
};

static struct platform_device *lombo_snd_device;

/*
 * func to init sound card, it will register a platfrom device named
 * soc-audio, which will  be matched with the platform driver in
 * the alsa core
 */
static int __init lombo_audio_init(void)
{
	int ret;

	lombo_snd_device = platform_device_alloc("soc-audio", -1);
	if (!lombo_snd_device)
		return -ENOMEM;

	platform_set_drvdata(lombo_snd_device, &snd_soc_n7);
	ret = platform_device_add(lombo_snd_device);

	if (ret)
		platform_device_put(lombo_snd_device);

	return ret;
}
module_init(lombo_audio_init);

static void __exit lombo_audio_exit(void)
{
	platform_device_unregister(lombo_snd_device);
}
module_exit(lombo_audio_exit);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("ALSA SoC n7");
MODULE_LICENSE("GPL");

