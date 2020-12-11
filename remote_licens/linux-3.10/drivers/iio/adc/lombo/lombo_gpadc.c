/*
 * lombo_gpadc.c - Lombo gpadc driver module
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
 */

#include <linux/device.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/iio/iio.h>
#include <linux/clk.h>

#include <mach/debug.h>

#ifdef CONFIG_ARCH_LOMBO_N9
#include "./csp/n9/gpadc_csp.h"
#else
#include "./csp/n7/gpadc_csp.h"
#endif /* CONFIG_ARCH_LOMBO_N9 */

#define DRIVER_NAME		"lombo-gpadc"

struct lombo_gpadc_dev {
	void __iomem		*base;
	int			irq;
	struct clk		*clk_gate;
	struct clk		*clk_reset;
	struct clk		*clk_gpadc;
	struct clk		*clk_parent;
};

static void gpadc_module_set_en(void *base, bool en)
{
	/* GPADC init */
	csp_gpadc_set_sample_rate(base, GPADC_SRC_512HZ);
	csp_gpadc_set_avg_en(base, en);
	csp_gpadc_set_int_en(base, 1, en);

	/* enable gpadc channel */
	csp_aux_set_th_data(base, AUXIN_CHAN_0, AUXIN_T_VALUE);
	csp_aux_set_th_data(base, AUXIN_CHAN_1, AUXIN_T_VALUE);
	csp_auxin_set_en(base, AUXIN_CHAN_0, en);
	csp_auxin_set_en(base, AUXIN_CHAN_1, en);

	csp_gpadc_set_en(base, en);
}

static void gpadc_init(void *base)
{
	gpadc_module_set_en(base, true);
}

static void gpadc_deinit(void *base)
{
	gpadc_module_set_en(base, false);
}

static int lombo_gpadc_read_chan(struct lombo_gpadc_dev *gpadc_dev,
				struct iio_chan_spec const *chan)
{
	u32 val;
	void __iomem *base;
	int ch;

	ch = chan->channel;
	base = gpadc_dev->base;

	if (chan->type == IIO_VOLTAGE) {
		switch (ch) {
		case AUXIN_CHAN_0:
		case AUXIN_CHAN_1:
			val = csp_auxin_get_data(base, ch);
			break;
		default:
			pr_warn("unknown IIO_VOLTAGE channel: %d\n", ch);
			return -1;
		}
#ifdef CONFIG_ARCH_LOMBO_N9
	} else if (chan->type == IIO_TEMP) {
		switch (ch) {
		case SENSOR_CHAN_0:
		case SENSOR_CHAN_1:
		case SENSOR_CHAN_2:
			val = csp_sensor_get_data(base, ch);
			break;
		default:
			pr_warn("unknown IIO_TEMP channel: %d\n", ch);
			return -1;
		}
#endif
	} else {
		pr_warn("unknown chan type: %d\n", chan->type);
		return -1;
	}

	return (int)val;
}

static int lombo_gpadc_read_raw(struct iio_dev *indio_dev,
			      struct iio_chan_spec const *chan,
			      int *val, int *val2, long mask)
{
	struct lombo_gpadc_dev *gpadc_dev = iio_priv(indio_dev);

	pr_info("chan channel: %d, name: %s, type: %d\n",
			chan->channel, chan->datasheet_name, chan->type);

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		*val = lombo_gpadc_read_chan(gpadc_dev, chan);
		if (*val < 0)
			return *val;

		return IIO_VAL_INT;

	default:
		pr_warn("%s unknown mask(%ld)\n", __func__, mask);
		break;
	}

	return -EINVAL;
}


#define LOMBO_GPADC_CHANNEL(_channel, _name, _type)		\
	{							\
		.type = _type,					\
		.indexed = 1,					\
		.channel = _channel,				\
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),	\
		.datasheet_name = _name,			\
	}


/* Demo */
static const struct iio_chan_spec lombo_adc_channels[] = {
	LOMBO_GPADC_CHANNEL(AUXIN_CHAN_0, "aux0", IIO_VOLTAGE),
	LOMBO_GPADC_CHANNEL(AUXIN_CHAN_1, "aux1", IIO_VOLTAGE),
#ifdef CONFIG_ARCH_LOMBO_N9
	LOMBO_GPADC_CHANNEL(SENSOR_CHAN_0, "sensor0", IIO_TEMP),
	LOMBO_GPADC_CHANNEL(SENSOR_CHAN_1, "sensor1", IIO_TEMP),
	LOMBO_GPADC_CHANNEL(SENSOR_CHAN_2, "sensor2", IIO_TEMP),
#endif
};

static const struct iio_info lombo_iio_info = {
	.read_raw = lombo_gpadc_read_raw,
};

static int lombo_gpadc_probe(struct platform_device *pdev)
{
	int ret;
	struct lombo_gpadc_dev *gpadc_dev;
	struct iio_dev *indio_dev;
	struct resource *res;

	indio_dev = iio_device_alloc(sizeof(*gpadc_dev));
	if (!indio_dev)
		return -ENOMEM;

	platform_set_drvdata(pdev, indio_dev);
	gpadc_dev = iio_priv(indio_dev);

	indio_dev->name = dev_name(&pdev->dev);
	indio_dev->dev.parent = &pdev->dev;
	indio_dev->info = &lombo_iio_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = lombo_adc_channels;
	indio_dev->num_channels = ARRAY_SIZE(lombo_adc_channels);

	/* io resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	gpadc_dev->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(gpadc_dev->base)) {
		dev_err(&pdev->dev, "err ioremap invalid\n");
		ret = PTR_ERR(gpadc_dev->base);
		goto error_free;
	}

	gpadc_dev->clk_gate = devm_clk_get(&pdev->dev, "apb_gpadc_gate");
	gpadc_dev->clk_reset = devm_clk_get(&pdev->dev, "apb_gpadc_reset");
	gpadc_dev->clk_gpadc = devm_clk_get(&pdev->dev, "gpadc_clk");
	gpadc_dev->clk_parent = devm_clk_get(&pdev->dev, "osc24m");
	if (IS_ERR(gpadc_dev->clk_gate) || IS_ERR(gpadc_dev->clk_reset)
		|| IS_ERR(gpadc_dev->clk_gpadc)
		|| IS_ERR(gpadc_dev->clk_parent)) {
		dev_err(&pdev->dev, "get adc key gate/reset/module clk failed\n");
		return -EINVAL;
	}

	ret = clk_set_parent(gpadc_dev->clk_gpadc, gpadc_dev->clk_parent);
	if (ret) {
		dev_err(&pdev->dev, "set parent failed\n");
		return -EINVAL;
	}

	ret = clk_prepare_enable(gpadc_dev->clk_reset);
	if (ret) {
		dev_err(&pdev->dev, "enable adc key reset assert failed\n");
		return ret;
	}

	ret = clk_prepare_enable(gpadc_dev->clk_gate);
	if (ret) {
		clk_disable_unprepare(gpadc_dev->clk_reset);
		dev_err(&pdev->dev, "enable adc key gate clk failed\n");
		return ret;
	}

	ret = clk_prepare_enable(gpadc_dev->clk_gpadc);
	if (ret) {
		clk_disable_unprepare(gpadc_dev->clk_gate);
		clk_disable_unprepare(gpadc_dev->clk_reset);
		dev_err(&pdev->dev, "enable adc key clk failed\n");
		return ret;
	}

	gpadc_init(gpadc_dev->base);

	ret = iio_device_register(indio_dev);
	if (ret) {
		dev_err(&pdev->dev, "unable to register device\n");
		PRT_ERR("register gpadc as a iio device failed\n");
		goto err_out;
	}

	return 0;

err_out:
	gpadc_deinit(gpadc_dev->base);
error_free:
	iio_device_free(indio_dev);

	return ret;
}


static int lombo_gpadc_remove(struct platform_device *pdev)
{
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);
	struct lombo_gpadc_dev *gpadc_dev = iio_priv(indio_dev);

	gpadc_deinit(gpadc_dev->base);

	iio_device_unregister(indio_dev);
	iio_device_free(indio_dev);

	return 0;
}


#ifdef CONFIG_OF
static const struct of_device_id lombo_gpadc_dt_match[] = {
	{ .compatible = "lombo,n7-gpadc" },
	{ .compatible = "lombotech,n9-gpadc" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_ir_dt_match);
#endif

static struct platform_driver lombo_gpadc_driver = {
	.probe		= lombo_gpadc_probe,
	.remove		= lombo_gpadc_remove,
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(lombo_gpadc_dt_match),
	#endif
	},
};
module_platform_driver(lombo_gpadc_driver);

MODULE_DESCRIPTION("LomboTech gpadc driver");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_LICENSE("GPL");
