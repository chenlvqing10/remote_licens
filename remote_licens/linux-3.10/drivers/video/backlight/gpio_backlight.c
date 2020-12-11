/*
 * gpio_backlight.c - Simple GPIO-controlled backlight
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/of_gpio.h>

#define GPIO_ACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 0 : 1)
#define GPIO_INACTIVE(flag) (((flag) & OF_GPIO_ACTIVE_LOW) ? 1 : 0)

struct gpio_backlight {
	struct device *dev;
	int bl_gpio;
	enum of_gpio_flags bl_gpio_flag;
};

static int gpio_backlight_update_status(
	struct backlight_device *bl)
{
	struct gpio_backlight *gbl = bl_get_data(bl);
	int brightness = bl->props.brightness;

	if (bl->props.power != FB_BLANK_UNBLANK ||
	    bl->props.fb_blank != FB_BLANK_UNBLANK ||
	    bl->props.state & (BL_CORE_SUSPENDED | BL_CORE_FBBLANK))
		brightness = 0;

	gpio_direction_output(gbl->bl_gpio, brightness);

	return 0;
}

static int gpio_backlight_get_brightness(
	struct backlight_device *bl)
{
	return bl->props.brightness;
}

static const struct backlight_ops backlight_backlight_ops = {
	.update_status	= gpio_backlight_update_status,
	.get_brightness	= gpio_backlight_get_brightness,
};

static int gpio_backlight_probe_dt(struct platform_device *pdev,
				   struct gpio_backlight *gbl)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	enum of_gpio_flags flags;
	int err;

	gbl->bl_gpio = of_get_named_gpio_flags(np, "bl_gpio", 0, &flags);
	gbl->bl_gpio_flag = flags;
	if (!gpio_is_valid(gbl->bl_gpio)) {
		pr_err("bl gpio %d invalid\n", gbl->bl_gpio);
		gbl->bl_gpio = 0;
	} else {
		err = devm_gpio_request(dev, gbl->bl_gpio, "bl_gpio");
		if (err < 0) {
			pr_err("bl gpio %d request failed\n", gbl->bl_gpio);
			gbl->bl_gpio = 0;
		}
	}
	pr_debug("bl_gpio: %d\nbl_gpio_flag: %d\n",
		gbl->bl_gpio, gbl->bl_gpio_flag);

	return 0;
}

static int gpio_backlight_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct backlight_properties props;
	struct backlight_device *bl;
	struct gpio_backlight *gbl;
	struct device_node *np = pdev->dev.of_node;
	int ret;

	if (!np) {
		dev_err(&pdev->dev,
			"failed to device tree node.\n");
		return -ENODEV;
	}

	gbl = devm_kzalloc(&pdev->dev, sizeof(*gbl), GFP_KERNEL);
	if (gbl == NULL)
		return -ENOMEM;

	gbl->dev = &pdev->dev;

	if (np) {
		ret = gpio_backlight_probe_dt(pdev, gbl);
		if (ret < 0) {
			dev_err(&pdev->dev, "failed to request GPIO\n");
			return ret;
		}

	}

	memset(&props, 0, sizeof(props));
	props.type = BACKLIGHT_RAW;
	props.max_brightness = 1;
	bl = backlight_device_register("gpio-backlight",
					dev, gbl,
					&backlight_backlight_ops,
					&props);
	if (IS_ERR(bl)) {
		dev_err(&pdev->dev, "failed to register backlight\n");
		return PTR_ERR(bl);
	}

	bl->props.brightness = GPIO_ACTIVE(gbl->bl_gpio_flag);
	backlight_update_status(bl);

	platform_set_drvdata(pdev, bl);

	return 0;
}

static int gpio_backlight_remove(struct platform_device *pdev)
{
	struct backlight_device *bd = platform_get_drvdata(pdev);

	backlight_device_unregister(bd);

	return 0;
}

static struct of_device_id gpio_backlight_of_match[] = {
	{
		.compatible = "gpio-backlight"
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, gpio_backlight_of_match);

static struct platform_driver gpio_backlight_driver = {
	.driver		= {
		.name		= "gpio-backlight",
		.of_match_table = gpio_backlight_of_match,
	},
	.probe		= gpio_backlight_probe,
	.remove		= gpio_backlight_remove,
};

static int __init gpio_backlight_init(void)
{
	int err;

	pr_debug("%s,%d\n", __FILE__, __LINE__);
	err = platform_driver_register(&gpio_backlight_driver);
	if (err < 0)
		return err;

	return 0;
}
fs_initcall(gpio_backlight_init);

static void __exit gpio_backlight_exit(void)
{
	platform_driver_unregister(&gpio_backlight_driver);
}
module_exit(gpio_backlight_exit);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("GPIO-based Backlight Driver");
MODULE_LICENSE("GPL");
