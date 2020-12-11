/*
 * common.c - test driver common operations
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

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/err.h>

#include "common.h"

/* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME	"COMMON"

struct drv_test_dev *test_dev;

/**
 * wait_for_test_event - wait test event
 * @pdev: test device handle
 *
 * When user cat the test sysfs node, the test event in bitmap will be set
 *
 * Return 0 is wait event success, -1 otherwise
 */
int wait_for_test_event(struct drv_test_dev *pdev)
{
	set_current_state(TASK_INTERRUPTIBLE);

	while (!kthread_should_stop()) {
		if (pdev->test_bitmap) { /* test pending */
			__set_current_state(TASK_RUNNING);
			return 0;
		}
		schedule();
		set_current_state(TASK_INTERRUPTIBLE);
	}
	__set_current_state(TASK_RUNNING);
	return -1;
}

/**
 * drv_test_thread - the common test thread
 * @arg: thread argument
 *
 * The thread will always wait for test event, if event coming,
 * then run the test program. When the test program run over,
 * it will continue to wait
 *
 * Return 0
 */
int drv_test_thread(void *arg)
{
	struct drv_test_dev *pdev = arg;

	while (!wait_for_test_event(pdev)) {
		/* test clk drv */
		if (test_and_clear_bit(CLK_TEST, &pdev->test_bitmap))
			clk_test();
		/* test pwm drv */
		if (test_and_clear_bit(PWM_TEST, &pdev->test_bitmap))
			pwm_test();
		/* test vc drv */
		if (test_and_clear_bit(VC_TEST, &pdev->test_bitmap))
			vc_test();
		/* test memctrl drv */
		if (test_and_clear_bit(MEMCTRL_TEST, &pdev->test_bitmap))
			memctrl_test();
		/* dma test driver*/
		if (test_and_clear_bit(DMA_TEST, &pdev->test_bitmap))
			dma_test();
		/* add your  test driver here... */
	}

	return 0;
}

/* the sysfs node function */
DEF_TEST_FUNC(clk_test, CLK_TEST)
DEF_TEST_FUNC(pwm_test, PWM_TEST)
DEF_TEST_FUNC(vc_test,	VC_TEST)
DEF_TEST_FUNC(memctrl_test, MEMCTRL_TEST)
DEF_TEST_FUNC(dma_test, DMA_TEST)

/* the sysfs attrs node */
struct device_attribute drv_test_attrs[] = {
	__ATTR(clk_test, S_IRUGO, clk_test_show, NULL),
	__ATTR(pwm_test, S_IRUGO, pwm_test_show, NULL),
	__ATTR(vc_test,	 S_IRUGO, vc_test_show,  NULL),
	__ATTR(memctrl_test, S_IRUGO, memctrl_test_show, NULL),
	__ATTR(dma_test, S_IRUGO, dma_test_show, NULL),
	/* add your  own attrs here... */
	__ATTR_NULL
};

void drv_test_release(struct device *dev)
{
	if (dev->parent)
		put_device(dev->parent);
}

/* the sysfs class defination */
struct class drv_test_class = {
	.name		= DEV_NAME,
	.owner		= THIS_MODULE,
	.dev_release	= drv_test_release,
	.dev_attrs	= drv_test_attrs,
};

__init int test_init(void)
{
	struct drv_test_dev *pdev = NULL;
	int ret = 0;

	pdev = kzalloc(sizeof(*pdev), GFP_KERNEL);
	if (!pdev) {
		PRT_ERR("alloc dev failed\n");
		return -ENOMEM;
	}

	/* create test thread */
	pdev->test_task = kthread_run(drv_test_thread, pdev, DEV_NAME);
	if (IS_ERR(pdev->test_task)) {
		PRT_ERR("kthread_run failed\n");
		goto err_kfree;
	}

	/* register test class */
	ret = class_register(&drv_test_class);
	if (ret) {
		PRT_ERR("drv_test_class register failed\n");
		goto err_kthread_stop;
	}

	/* init test devcie */
	pdev->dev.class = &drv_test_class;
	pdev->dev.parent = NULL;
	dev_set_name(&pdev->dev, DEV_NAME);

	/* register test devcie */
	ret = device_register(&pdev->dev);
	if (ret) {
		PRT_ERR("drv_test_dev register failed\n");
		goto err_class_unregister;
	}

	test_dev = pdev; /* saved for free in test_exit */
	return 0;

err_class_unregister:
	class_unregister(&drv_test_class);
err_kthread_stop:
	kthread_stop(pdev->test_task);
	pdev->test_task = NULL;
err_kfree:
	kfree(pdev);
	return ret;
}
module_init(test_init);

void __exit test_exit(void)
{
	struct drv_test_dev *pdev = test_dev;

	device_unregister(&pdev->dev);
	class_unregister(&drv_test_class);
	kthread_stop(pdev->test_task);
	pdev->test_task = NULL;
	kfree(pdev);

	test_dev = NULL; /* reset to NULL */
}
module_exit(test_exit);

MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("LomboTech N7 Test Driver");
MODULE_LICENSE("GPL");
