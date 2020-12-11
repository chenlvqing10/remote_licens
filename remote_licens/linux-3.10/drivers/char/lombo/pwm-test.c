/*
 * pwm-test.c - pwm test driver
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

#include <linux/device.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/pwm.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/io.h>

#include "common.h"

/* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME	"PWM_TEST"	/* for print message */

#define PWM_TEST_NODE	"pwm_test"	/* pwm test node name in dts */
#define PWM_DUTY_TEST	300000		/* duty in ns for test */
#define PWM_PEROID_TEST	1000000		/* peroid in ns for test */

/**
 * pwm_irq_handle - pwm irq handle function
 * @irq: linux irq number
 * @dev_id: device id parament in request_irq
 *
 * return IRQ_HANDLED if irq is successfully handled,
 * IRQ_NONE if interrupt was not from this device
 */
irqreturn_t pwm_irq_handle(int irq, void *dev_id)
{
	PRT_INFO("enter\n");
	return IRQ_HANDLED;
}

/**
 * pwm_test - pwm drv test function
 */
void pwm_irq_test(struct device_node *pnode)
{
	unsigned int irq = 0;

	PRT_INFO("start\n");

	/* get pwm irq */
	irq = irq_of_parse_and_map(pnode, 0);
	if (!irq) {
		PRT_ERR("irq_of_parse_and_map failed\n");
		return;
	}
	PRT_INFO("irq_of_parse_and_map success, ret irq %d\n", (int)irq);

	/* request pwm irq */
	if (request_irq(irq, pwm_irq_handle, 0,	"pwm-irq", NULL)) {
		PRT_ERR("request irq %d failed!\n", (int)irq);
		return;
	}
	PRT_INFO("request irq %d success\n", (int)irq);

	/* do something... */

	/* free pwm irq */
	free_irq(irq, NULL);

	PRT_INFO("end\n");
}

int _pwm_gpio_cfg(struct device_node *pnode)
{
	struct platform_device *pdev;
	struct pinctrl *ptl = NULL;
	struct pinctrl_state *pins_test = NULL;
	int ret = 0;

	pdev = of_find_device_by_node(pnode);
	if (IS_ERR_OR_NULL(pdev)) {
		PRT_ERR("get platform_device from node fail\n");
		ret = -EINVAL;
		goto end;
	}

	ptl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(ptl)) {
		PRT_ERR("get pinctrl fail\n");
		ret = -EINVAL;
		goto end;
	}

	pins_test = pinctrl_lookup_state(ptl, "default");
	if (IS_ERR(pins_test)) {
		PRT_ERR("get default state fail\n");
		ret = -EINVAL;
		goto end;
	}

	pinctrl_select_state(ptl, pins_test);

	PRT_INFO("success!\n");
end:
	return ret;
}

/**
 * pwm_test - pwm drv test function
 */
void pwm_test(void)
{
	struct device_node *pnode = NULL;
	struct pwm_device *pdev = NULL;
	unsigned int period = 0;

	PRT_INFO("start\n");

	/* get pwm test device node */
	pnode = of_find_node_by_name(NULL, PWM_TEST_NODE);
	if (!pnode) {
		PRT_ERR("get %s node failed\n", PWM_TEST_NODE);
		return;
	}
	PRT_INFO("get %s node success, ret 0x%08x\n",
		PWM_TEST_NODE, (int)pnode);

	/* config pwm gpio */
	if (_pwm_gpio_cfg(pnode)) {
		PRT_ERR("config gpio failed\n");
		return;
	}
	PRT_INFO("config %s gpio success\n", PWM_TEST_NODE);

	/* request the pwm(configured in dts), get the pwm_device */
	pdev = of_pwm_get(pnode, NULL);
	if (IS_ERR(pdev)) {
		PRT_ERR("get pwm device failed, ret 0x%08x\n", (int)pdev);
		return;
	}
	PRT_INFO("get pwm device success, hwpwm is %d\n", (int)pdev->hwpwm);

	/* get orginal period(configured in dts) */
	period = pwm_get_period(pdev);
	PRT_INFO("get org period %d\n", (int)period);

	/* enable pwm */
	if (pwm_enable(pdev)) {
		PRT_ERR("pwm %d enable failed\n", (int)pdev->hwpwm);
		return;
	}
	PRT_INFO("pwm %d enable success\n", (int)pdev->hwpwm);

	/* disable pwm */
	pwm_disable(pdev);
	PRT_INFO("pwm %d disabled\n", (int)pdev->hwpwm);

	/* set pwm polarity */
	if (pwm_set_polarity(pdev, PWM_POLARITY_NORMAL)) {
		PRT_ERR("pwm %d set polarity to normal failed\n",
			(int)pdev->hwpwm);
		return;
	}
	PRT_INFO("pwm %d set polarity to normal success\n", (int)pdev->hwpwm);

	/* pwm duty and period config */
	if (pwm_config(pdev, PWM_DUTY_TEST, PWM_PEROID_TEST)) {
		PRT_ERR("pwm %d config failed, duty %d,peroid %d\n",
			(int)pdev->hwpwm, PWM_DUTY_TEST, PWM_PEROID_TEST);
		return;
	}
	PRT_INFO("pwm %d config success, duty %d, peroid %d\n",
		(int)pdev->hwpwm, PWM_DUTY_TEST, PWM_PEROID_TEST);

	/* test pwm irq */
	pwm_irq_test(pnode);

	/* free pwm device */
	pwm_put(pdev);

	PRT_INFO("%s(%d) end\n", __func__, __LINE__);
}

