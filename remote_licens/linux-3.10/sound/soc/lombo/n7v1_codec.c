/*
 * n7v1_codec.c - Common code for LomboTech SoCs
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
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/regmap.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <sound/core.h>
#include <sound/jack.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <trace/events/asoc.h>
#include <mach/csp.h>
#include <mach/debug.h>
#include <mach/common.h>

#include "n7v1_codec.h"
#include "csp/ac/csp_ac.h"
#undef  MOD_NAME
#define MOD_NAME "N7V1_AC"

struct ac_priv {
	/* todo: maybe we should add some fields in the future */
	void __iomem *regbase;
	int ac_shutdown; /* gpio for ac */
	int pa_enable_value;
	int pa_disable_value;
};

/* sample_rate supportted by this AC driver */
#define LOMBO_AC_RATES \
	(SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 | SNDRV_PCM_RATE_16000 | \
	SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 | \
	SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

/* sample_bits supportted by this AC driver */
#define LOMBO_AC_FORMAT \
	(SNDRV_PCM_FORMAT_U8 | SNDRV_PCM_FORMAT_S16_LE | \
	SNDRV_PCM_FORMAT_S24_LE | SNDRV_PCM_FORMAT_S32_LE)

/* func to write data to ac regs */
static int n7_ac_write(struct snd_soc_codec  *codec, unsigned int reg,
			unsigned int value)
{
	struct ac_priv *priv = snd_soc_codec_get_drvdata(codec);
	writel(value, priv->regbase + reg);
	return 0;
}

/* func to read data from ac regs */
static unsigned int n7_ac_read(struct snd_soc_codec *codec, unsigned int reg)
{
	struct ac_priv *priv = snd_soc_codec_get_drvdata(codec);
	return readl(priv->regbase + reg);
}

static const DECLARE_TLV_DB_MINMAX(playback_tlv, -12600, 0);
static const DECLARE_TLV_DB_MINMAX(inputpga_tlv, -600, 2100);
static const DECLARE_TLV_DB_MINMAX(record_tlv, 3000, -9600);

/* define codec controls */
static const struct snd_kcontrol_new n7ac_snd_controls[] = {
	SOC_DOUBLE("Input PGA Mute",		N7V1_AC_MUTEIN, 2, 3, 1, 0),
	SOC_DOUBLE("Record Digital Mute",	N7V1_AC_MUTEIN, 0, 1, 1, 0),
	SOC_SINGLE("Playback Digital Mute",	N7V1_AC_DMUTEOUT, 0, 1, 0),
	SOC_SINGLE("Playback Analog Mute",	N7V1_AC_AMUTEOUT, 2, 1, 0),

	SOC_DOUBLE_R_TLV("Record Master Volume", N7V1_AC_VRECL, N7V1_AC_VRECR,
		 0, 84, 0, record_tlv),
	SOC_DOUBLE_R_TLV("Input PGA Volume", N7V1_AC_VPGAL, N7V1_AC_VPGAR,
		 0, 19, 0, inputpga_tlv),
	SOC_SINGLE_TLV("Playback Master Volume", N7V1_AC_VPLAYL,
		 0, 84, 0, playback_tlv),
};

/* Left Output Mixer */
static const struct snd_kcontrol_new left_mixer_controls[] = {
	SOC_DAPM_SINGLE("Left DAC OUT Switch",  N7V1_AC_MPLAYL, 0, 1, 0),
	SOC_DAPM_SINGLE("Left Loopout Switch",  N7V1_AC_MPLAYL, 1, 1, 0),
};


/* define DAPM widgets */
static const struct snd_soc_dapm_widget n7ac_dapm_widgets[] = {
	/* ADC DAPM WIDGETS */
	SND_SOC_DAPM_ADC("ADCL", "Capture", N7V1_AC_PWRIN, 0, 0),
	SND_SOC_DAPM_ADC("ADCR", "Capture", N7V1_AC_PWRIN, 1, 0),

	/* PGA DAPM WIDGETS */
	SND_SOC_DAPM_PGA("PGAL",  N7V1_AC_PWRIN, 2, 0, NULL, 0),
	SND_SOC_DAPM_PGA("PGAR",  N7V1_AC_PWRIN, 3, 0, NULL, 0),

	/* DAC DAPM WIDGETS */
	SND_SOC_DAPM_DAC("DACL", "Playback", N7V1_AC_PWROUT, 0, 0),

	/* LINE OUT DRIVER */
	SND_SOC_DAPM_OUT_DRV("LINELDRV", N7V1_AC_PWROUT, 7, 0, NULL, 0),

	/* MICBIAS WIDGET */
	/* SND_SOC_DAPM_MICBIAS("MICBIAS", N7V1_AC_PWRM, 2, 0), */

	/* MIXER */
	SND_SOC_DAPM_MIXER("Left Output Mixer", SND_SOC_NOPM, 0, 0,
	left_mixer_controls,
	ARRAY_SIZE(left_mixer_controls)),

	/* OUTPUT WIDGETS */
	SND_SOC_DAPM_OUTPUT("SPKL"),
	SND_SOC_DAPM_INPUT("MICN"),
	SND_SOC_DAPM_INPUT("MICP"),
};

/* audio route map */
static const struct snd_soc_dapm_route n7ac_dapm_routes[] = {
	/* INPUT Routes */
	{"PGAL", NULL, "MICN"},
	{"PGAR", NULL, "MICP"},
	{"ADCL", NULL, "PGAL"},
	{"ADCR", NULL, "PGAR"},

	/* OUTPUT Routes */
	{"Left Output Mixer", "Left DAC OUT Switch", "DACL"},
	{"Left Output Mixer", "Left Loopout Switch", "PGAL"},

	{"LINELDRV", NULL, "Left Output Mixer"},

	{"SPKL", NULL, "LINELDRV"},
};

/* func to set fmt, including m/s fmt, i2s data fmt */
static int n7ac_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct ac_priv *priv = snd_soc_codec_get_drvdata(codec);

	/* set AC I2S MASTER/SLAVE MODE */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		csp_ac_set_ms_mode(priv->regbase, I2S_MASTER);
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		csp_ac_set_ms_mode(priv->regbase, I2S_SLAVE);
		break;
	default:
		return -EINVAL;
	}

	/* set I2S DATA FORMAT */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		csp_ac_set_i2sfmt(priv->regbase, I2S_STANDARD);
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		csp_ac_set_i2sfmt(priv->regbase, I2S_LSB);
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		csp_ac_set_i2sfmt(priv->regbase, I2S_MSB);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* func to set hw_params, including bitwith, sample rates */
static int n7ac_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params,
	struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct ac_priv *priv = snd_soc_codec_get_drvdata(codec);

	/* set dacfs/adcfs according to samplerates */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		csp_ac_set_playback_fs(priv->regbase, params_rate(params));
	else
		csp_ac_set_record_fs(priv->regbase, params_rate(params));
	return 0;
}

/* set clk divider */
static int n7ac_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
	int div_id, int div)
{
	/* do nothing now */
	return 0;
}

/* mute before stopping playback to minimize pop */
static int n7ac_digital_mute(struct snd_soc_dai *dai, int mute)
{
	int ret;
	struct snd_soc_codec *codec = dai->codec;
	struct ac_priv *priv = snd_soc_codec_get_drvdata(codec);

	/* mute or unmute the playback */
	if (mute) {
		if (priv->ac_shutdown >= 0) {
			ret = gpio_direction_output(priv->ac_shutdown,
					priv->pa_disable_value);
			if (ret < 0)
				dev_err(codec->dev, "set ac_shutdown to output err\n");
		}
	} else {
		if (priv->ac_shutdown >= 0) {
			ret = gpio_direction_output(priv->ac_shutdown,
					priv->pa_enable_value);
			if (ret < 0)
				dev_err(codec->dev, "set ac_shutdown to output err\n");
			usleep_range(100000, 110000);
		}
	}

	return 0;
}

/* overall power control */
static int n7ac_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	struct ac_priv *priv = snd_soc_codec_get_drvdata(codec);

	switch (level) {
	case SND_SOC_BIAS_ON:
		PRT_DBG("powr on\n");
		break;
	case SND_SOC_BIAS_PREPARE:
		if (codec->dapm.bias_level >= level)
			break;
		csp_ac_poweron(priv->regbase);
		PRT_DBG("powr prepare\n");
		break;
	case SND_SOC_BIAS_STANDBY:
		if (codec->dapm.bias_level <= level)
			break;
		csp_ac_poweroff(priv->regbase);
		PRT_DBG("powr standby\n");
		break;
	case SND_SOC_BIAS_OFF:
		csp_ac_poweroff(priv->regbase);
		PRT_DBG("powr off\n");
		break;
	default:
		break;
	}
	codec->dapm.bias_level = level;
	PRT_DBG("bias level %d\n", level);
	return 0;
}

/* suspend */
static int n7ac_suspend(struct snd_soc_codec *codec)
{
	n7ac_set_bias_level(codec, SND_SOC_BIAS_OFF);
	PRT_INFO("====enter=====\n");
	return 0;
}

/* resume */
static int n7ac_resume(struct snd_soc_codec *codec)
{
	n7ac_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	PRT_INFO("====enter=====\n");
	return 0;
}

/* define dai_ops for codec */
static const struct snd_soc_dai_ops n7ac_dai_ops = {
	.hw_params		= n7ac_hw_params,
	.set_clkdiv		= n7ac_set_dai_clkdiv,
	.set_fmt		= n7ac_set_dai_fmt,
	.digital_mute		= n7ac_digital_mute,
};


/* define dai driver */
static struct snd_soc_dai_driver n7ac_dai = {
	.name	  = "n7ac-dai",
	.playback = {
		.stream_name  = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates        = LOMBO_AC_RATES,
		.formats      = LOMBO_AC_FORMAT,
	},
	.capture = {
		.stream_name  = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates        = LOMBO_AC_RATES,
		.formats      = LOMBO_AC_FORMAT,
	},
	.ops	= &n7ac_dai_ops,
};

static int n7ac_probe(struct snd_soc_codec *codec)
{
	struct ac_priv *priv = snd_soc_codec_get_drvdata(codec);
	csp_ac_init(priv->regbase);
	return 0;
}

static struct snd_soc_codec_driver n7ac_soc_driver = {
	.probe			= n7ac_probe,
	.suspend		= n7ac_suspend,
	.resume			= n7ac_resume,
	.set_bias_level		= n7ac_set_bias_level,
	.write			= n7_ac_write,
	.read			= n7_ac_read,
	.controls		= n7ac_snd_controls,
	.num_controls		= ARRAY_SIZE(n7ac_snd_controls),
	.dapm_widgets		= n7ac_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(n7ac_dapm_widgets),
	.dapm_routes		= n7ac_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(n7ac_dapm_routes),
};

static int n7ac_codec_probe(struct platform_device *pdev)
{
	int ret;
	struct ac_priv *priv;
	struct resource *res;
	enum of_gpio_flags flag;

	pr_debug("Entered %s\n", __func__);

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (NULL == priv) {
		dev_err(&pdev->dev, "allock ac_priv error\n");
		return -ENOMEM;
	}

	/* get the resource associated with this platfrom device */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "can't get io resources\n");
		return -ENOENT;
	}

	/* ioremap the regs */
	priv->regbase = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(priv->regbase))
		return PTR_ERR(priv->regbase);

	/* get gpio ac_shutdown */
	priv->ac_shutdown = of_get_named_gpio_flags(pdev->dev.of_node,
						"ac_shutdown", 0, &flag);
	if (priv->ac_shutdown < 0)
		dev_warn(&pdev->dev, "can't find ac shutdown gpio\n");
	if (gpio_is_valid(priv->ac_shutdown)) {
		ret = devm_gpio_request(&pdev->dev,
			priv->ac_shutdown, "ac_shutdown");
		if (ret < 0) {
			dev_err(&pdev->dev,
				"request %d fail\n", priv->ac_shutdown);
			priv->ac_shutdown = -1;
		}
		priv->pa_enable_value = (flag == OF_GPIO_ACTIVE_LOW) ? 0 : 1;
		priv->pa_disable_value = (!priv->pa_enable_value) & 1;
		gpio_direction_output(priv->ac_shutdown,
				priv->pa_disable_value);
	}

	platform_set_drvdata(pdev, priv);

	/* register codec driver¡¢codec_dai */
	return snd_soc_register_codec(&pdev->dev,
			&n7ac_soc_driver, &n7ac_dai, 1);
}

static int n7ac_codec_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static struct of_device_id lombo_ac_match[] = {
	{ .compatible = "lombo, n7-ac", },
	{ /* end */},
};


static struct platform_driver n7ac_codec_driver = {
	.driver = {
		.name = "n7-ac",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(lombo_ac_match),
	},
	.probe	= n7ac_codec_probe,
	.remove	= n7ac_codec_remove,
};

static int __init lombo_codec1_init(void)
{
	int ret = 0;
	u32 p1 = 0, p2 = 0;

	lombo_func2(&p1, &p2);
	if (p1 == 2) {
		ret = platform_driver_register(&n7ac_codec_driver);
		PRT_INFO("------codec p1 %d -----\n", p1);
	}
	return 0;
}

module_init(lombo_codec1_init);

static void __exit lombo_codec1_exit(void)
{
	u32 p1 = 0, p2 = 0;

	lombo_func2(&p1, &p2);
	if (p1 == 2)
		platform_driver_unregister(&n7ac_codec_driver);
}

module_exit(lombo_codec1_exit);

/* Module information */
MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("n7 AC SoC Interface");
MODULE_LICENSE("GPL");

