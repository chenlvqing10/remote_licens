/*
 * Lombo hw_md6 driver
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
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <asm-generic/errno-base.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#define OBJECT_MOVE	0
#define OBJECT_STOP	1

#define IRQ_NUM		"irq_num"

#define TIMER_TEST	0

struct hw_md6_dev {
	int gpio_num;
	int irq;
	struct input_dev *dev_in;
};

#if TIMER_TEST
static struct timer_list inputdev_timer;

static void inputdev_timer_timeout(unsigned long data)
{
	static int pin_val;
	struct hw_md6_dev *hm_dev = (struct hw_md6_dev *)data;
	struct input_dev *dev_in = hm_dev->dev_in;

	mod_timer(&inputdev_timer, jiffies + HZ/2);
	pin_val++;
	if (pin_val % 2 == 0) {
		input_event(dev_in, EV_MSC, MSC_RAW, OBJECT_MOVE);
		input_sync(dev_in);
	} else {
		input_event(dev_in, EV_MSC, MSC_RAW, OBJECT_STOP);
		input_sync(dev_in);
	}
}
#endif

static irqreturn_t hw_md6_irq_handler(int irq, void *dev_id)
{
	int value;
	struct hw_md6_dev *hm_dev = (struct hw_md6_dev *)dev_id;
	struct input_dev *dev_in = hm_dev->dev_in;

	#if TIMER_TEST
	return IRQ_HANDLED;
	#endif

	disable_irq_nosync(hm_dev->irq);
	value = gpio_get_value(hm_dev->gpio_num);

	/*
	printk(KERN_DEBUG "hw_md6_irq: ionum:%d type:%d code:%d val:%d\n",
		hm_dev->gpio_num, EV_MSC, MSC_RAW, value);
	*/

	if (value == 0) {
		input_event(dev_in, EV_MSC, MSC_RAW, OBJECT_MOVE);
		input_sync(dev_in);
	} else {
		input_event(dev_in, EV_MSC, MSC_RAW, OBJECT_STOP);
		input_sync(dev_in);
	}

	enable_irq(hm_dev->irq);

	return IRQ_HANDLED;
}

static int __init hw_md6_event_init(struct platform_device *pdev,
	struct hw_md6_dev *hm_dev)
{
	int ret;
	struct input_dev *dev_in;

	dev_info(&pdev->dev, "hw_md6_event_init++\n");
	dev_in = input_allocate_device();
	if (!dev_in) {
		dev_err(&pdev->dev, "line:%d, input_allocate_device err\n",
			__LINE__);
		return -ENOMEM;
	}

	set_bit(EV_MSC, dev_in->evbit);
	set_bit(MSC_RAW, dev_in->mscbit);

	ret = input_register_device(dev_in);
	if (ret != 0) {
		dev_err(&pdev->dev, "line:%d, input_register_device err\n",
			__LINE__);
		return -ENOMEM;
	}

#if TIMER_TEST
	init_timer(&inputdev_timer);
	inputdev_timer.expires = 0;
	inputdev_timer.data = (unsigned long) hm_dev;
	inputdev_timer.function = inputdev_timer_timeout;
	add_timer(&inputdev_timer);
	mod_timer(&inputdev_timer, jiffies+HZ/100);
#endif

	hm_dev->dev_in = dev_in;

	dev_info(&pdev->dev, "hw_md6_event_init--\n");
	return 0;
}

static void hw_md6_event_exit(struct platform_device *pdev,
	struct hw_md6_dev *hm_dev)
{
	struct input_dev *dev_in = hm_dev->dev_in;

	dev_info(&pdev->dev, "hw_md6_event_exit++\n");
	input_unregister_device(dev_in);
	input_free_device(dev_in);
#if TIMER_TEST
	del_timer(&inputdev_timer);
#endif
	dev_info(&pdev->dev, "hw_md6_event_exit--\n");
}

static int lombo_hw_md6_probe(struct platform_device *pdev)
{
	int ret;
	struct hw_md6_dev *hm_dev;
	struct device_node *np = pdev->dev.of_node;

	dev_info(&pdev->dev, "hw_md6_probe enter\n");
	hm_dev = devm_kzalloc(&pdev->dev, sizeof(*hm_dev), GFP_KERNEL);
	if (!hm_dev) {
		dev_err(&pdev->dev, "alloc hw_md6_dev failed\n");
		return -ENOMEM;
	}

	/* event init function must be called before request_irq function */
	hw_md6_event_init(pdev, hm_dev);

	hm_dev->gpio_num = of_get_named_gpio(np, IRQ_NUM, 0);
	if (hm_dev->gpio_num < 0) {
		dev_info(&pdev->dev, "%s property not found %d\n",
					IRQ_NUM, __LINE__);
		hm_dev->gpio_num = 0;
	}

	dev_info(&pdev->dev, "irq gpio: %d\n", hm_dev->gpio_num);

	ret = devm_gpio_request_one(&pdev->dev, hm_dev->gpio_num,
				    GPIOF_IN, "hw_md6-irq");
	if (ret < 0) {
		dev_err(&pdev->dev, "request %d fail %d\n",
				hm_dev->gpio_num, __LINE__);
		return ret;
	}

	hm_dev->irq = gpio_to_irq(hm_dev->gpio_num);
	if (hm_dev->irq < 0) {
		dev_err(&pdev->dev, "get irq %d fail %d\n",
				hm_dev->irq, __LINE__);
		return -EINVAL;
	}

	ret = request_irq(hm_dev->irq, hw_md6_irq_handler,
			  IRQ_TYPE_EDGE_BOTH, "hw_md6_irq_handler",
						hm_dev);
	if (unlikely(ret)) {
		dev_err(&pdev->dev, "request irq %d fail %d\n",
				hm_dev->irq, __LINE__);
		return ret;
	}

	platform_set_drvdata(pdev, hm_dev);

	return 0;
}

static int lombo_hw_md6_remove(struct platform_device *pdev)
{
	struct hw_md6_dev *hm_dev = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "lombo_hw_md6_remove enter\n");
	free_irq(hm_dev->irq, hm_dev);
	devm_gpio_free(&pdev->dev, hm_dev->gpio_num);
	hw_md6_event_exit(pdev, hm_dev);
	devm_kfree(&pdev->dev, hm_dev);

	return 0;
}

static const struct of_device_id lombo_hw_md6_match[] = {
	{ .compatible = "lombo,hw-md6" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_hw_md6_match);

static struct platform_driver lombo_hw_md6_driver = {
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "lombo-hw-md6",
		.of_match_table	= lombo_hw_md6_match,
	},
	.probe		= lombo_hw_md6_probe,
	.remove		= lombo_hw_md6_remove,
};

module_platform_driver(lombo_hw_md6_driver);

MODULE_AUTHOR("lomboswer@lombotech.com>");
MODULE_DESCRIPTION("System Power Control for Lombo SoCs");
MODULE_LICENSE("GPL");
