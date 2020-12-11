/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * ir cut Control Core driver code for LomboTech
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
#include <linux/of_gpio.h>
#include <linux/kthread.h>
#include <mach/debug.h>
#include <mach/csp.h>
#include <linux/clk.h>
#include <linux/of_address.h>

#include "ccc.h"
#include "ir_cut.h"

#define CHECK_TIME_S		1
#define FALSE_TRIGGERING_S	3
#define AUXIN1_DATA_BIT		(0x00000FFF)
#define AUXIN1_THRESHOLD_DATA	(0x200)

#define GPADC_CLK_GATE		"gpadc_apb_gate"
#define GPADC_CLK_REST		"gpadc_apb_reset"
#define GPADC_MODULE_CLK	"gpadc_clk"
#define GPADC_CLK_PARENT0	"gpadc_parent0"
#define GPADC_CLK_PARENT1	"gpadc_parent1"

#define GPAEN_OFFSET	(0x00000010)
#define GPAC_OFFSET	(0x00000014)
#define AIN1DAT_OFFSET	(0x00000040)

struct ir_cut {
	int		ir_cut_pin0;
	int		ir_cut_pin1;
	bool		ir_filter;
	void __iomem	*base;

	struct clk	*clk_gate;
	struct clk	*clk_reset;
	struct clk	*clk_gpadc;
	struct clk	*clk_parent0;
	struct clk	*clk_parent1;

	struct task_struct *ir_task;
	struct mutex lock;
};

struct ir_cut g_ir_cut;

static void csp_gpadc_init(void *base_addr)
{
	u32 value;

	/* gpadc control register */
	value = READREG32((void *)(base_addr + GPAC_OFFSET));
	value |= (0x1 << 8); /* src */
	value |= (0x1 << 7); /* sdae */
	value |= (0x1 << 1); /* auxin1e */
	WRITEREG32((void *)(base_addr + GPAC_OFFSET), value);

	/* enable gpdac */
	value = READREG32((void *)(base_addr + GPAEN_OFFSET));
	value |= (0x1 << 0);
	WRITEREG32((void *)(base_addr + GPAEN_OFFSET), value);
}

static u32 csp_read_auxin1_threshold_data(void *base_addr)
{
	u32 value;

	value = READREG32((void *)(base_addr + AIN1DAT_OFFSET));
	value = (AUXIN1_DATA_BIT) & value;
	if (value)
		return value;

	return -1;
}

static int ir_filter_on(struct ir_cut *g_ircut)
{
	int ret;
	struct ir_cut *ir = g_ircut;

	ret = gpio_direction_output(ir->ir_cut_pin0, 1);
	if (ret < 0)
		return ret;
	ret = gpio_direction_output(ir->ir_cut_pin1, 0);
	if (ret < 0)
		return ret;

	sensor_msleep(50);

	ret = gpio_direction_output(ir->ir_cut_pin0, 0);
	if (ret < 0)
		return ret;

	return ret;
}

static int ir_filter_off(struct ir_cut *g_ircut)
{
	int ret;
	struct ir_cut *ir = g_ircut;

	ret = gpio_direction_output(ir->ir_cut_pin0, 0);
	if (ret < 0)
		return ret;
	ret = gpio_direction_output(ir->ir_cut_pin1, 1);
	if (ret < 0)
		return ret;

	sensor_msleep(50);

	ret = gpio_direction_output(ir->ir_cut_pin1, 0);
	if (ret < 0)
		return ret;

	return ret;
}

static int ir_cut_request_pins(struct ir_cut *g_ircut,
				 struct cccore *ccc,
				 struct device_node *np)
{
	enum of_gpio_flags flags;
	int ret = 0;
	struct ir_cut *ir = g_ircut;

	ir->ir_cut_pin0 = of_get_named_gpio_flags(np,
					"ir_cut_pin0", 0, &flags);
	ir->ir_cut_pin1 = of_get_named_gpio_flags(np,
					"ir_cut_pin1", 0, &flags);

	if (!gpio_is_valid(ir->ir_cut_pin0)) {
		PRT_INFO("no ir_cut0 pin(%d)\n",
			ir->ir_cut_pin0);
		ir->ir_cut_pin0 = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ir->ir_cut_pin0,
					"ir_cut_pin0");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (ir_cut_pin0)\n",
				ir->ir_cut_pin0);
			ir->ir_cut_pin0 = -1;
			ir->ir_cut_pin1 = -1;
			return -EBUSY;
		}
	}

	if (!gpio_is_valid(ir->ir_cut_pin1)) {
		PRT_INFO("no ir_cut1 pin(%d)\n",
			ir->ir_cut_pin1);
		ir->ir_cut_pin1 = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ir->ir_cut_pin1,
					"ir_cut_pin1");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (ir_cut_pin1)\n",
				ir->ir_cut_pin1);
			ir->ir_cut_pin0 = -1;
			ir->ir_cut_pin1 = -1;
			return -EBUSY;
		}
	}

	return 0;
}

static int ir_filter_init(struct ir_cut *g_ircut)
{
	u32 value;
	int ret;
	struct ir_cut *ir = g_ircut;

	value = csp_read_auxin1_threshold_data(ir->base);
	if (value < 0)
		return value;

	if ((AUXIN1_THRESHOLD_DATA < value)) {
		mutex_lock(&ir->lock);

		ret = ir_filter_off(ir);
		if (ret < 0)
			PRT_ERR("ir filter off failed!\n");
		else
			PRT_INFO("ir filter off!\n");

		ir->ir_filter = false;

		mutex_unlock(&ir->lock);
	} else {
		mutex_lock(&ir->lock);

		ret = ir_filter_on(ir);
		if (ret < 0)
			PRT_ERR("ir filter on failed!\n");
		else
			PRT_INFO("ir filter on!\n");

		ir->ir_filter = true;

		mutex_unlock(&ir->lock);
	}

	return ret;
}

static int threadfunc(void *data)
{
	int value;
	int ret = -1;
	struct ir_cut *ir = (struct ir_cut *)data;

	struct timeval old_time;
	struct timeval now_time;
	u32 time_used;

	sensor_msleep(2000);
	do_gettimeofday(&old_time);

	ret = ir_filter_init(ir);
	if (ret < 0)
		PRT_ERR("ir filter init failed!\n");

	while (!kthread_should_stop()) {

		set_current_state(TASK_INTERRUPTIBLE);

		do_gettimeofday(&now_time);
		time_used = now_time.tv_sec - old_time.tv_sec;
		if (time_used < FALSE_TRIGGERING_S)
			goto sc_time;

		value = csp_read_auxin1_threshold_data(ir->base);

		if ((AUXIN1_THRESHOLD_DATA < value)) {
			sensor_msleep(100);

			value = csp_read_auxin1_threshold_data(ir->base);
			if (AUXIN1_THRESHOLD_DATA > value)
				goto sc_time;

			if (ir->ir_filter) {
				mutex_lock(&ir->lock);

				ret = ir_filter_off(ir);
				if (ret < 0)
					PRT_ERR("ir filter off failed!\n");
				else
					PRT_INFO("ir filter off!\n");

				old_time = now_time;
				ir->ir_filter = false;

				mutex_unlock(&ir->lock);
			}
		} else {
			sensor_msleep(100);

			value = csp_read_auxin1_threshold_data(ir->base);
			if (AUXIN1_THRESHOLD_DATA < value)
				goto sc_time;

			if (!ir->ir_filter) {
				mutex_lock(&ir->lock);

				ret = ir_filter_on(ir);
				if (ret < 0)
					PRT_ERR("ir filter on failed!\n");
				else
					PRT_INFO("ir filter on!\n");

				old_time = now_time;
				ir->ir_filter = true;

				mutex_unlock(&ir->lock);
			}
		}

sc_time:
		schedule_timeout(CHECK_TIME_S*HZ);
	}
	return 0;
}

static ssize_t ir_cut_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	u32 value;

	value = csp_read_auxin1_threshold_data(g_ir_cut.base);

	PRT_INFO("in1 data:%d\n", value);
	PRT_INFO("ir filter state:%d\n", g_ir_cut.ir_filter);

	return 0;
}

static ssize_t ir_cut_store(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	unsigned int state = 0;
	int ret = -1;

	sscanf(buf, "%du", &state);

	if (state == 0) {
		mutex_lock(&g_ir_cut.lock);

		ret = ir_filter_off(&g_ir_cut);
		if (ret < 0)
			PRT_ERR("ir filter off failed");
		else
			PRT_INFO("ir filter off!\n");

		g_ir_cut.ir_filter = false;

		mutex_unlock(&g_ir_cut.lock);
	} else if (state == 1) {
		mutex_lock(&g_ir_cut.lock);

		ret = ir_filter_on(&g_ir_cut);
		if (ret < 0)
			PRT_ERR("ir filter on failed");
		else
			PRT_INFO("ir filter on!\n");

		g_ir_cut.ir_filter = true;

		mutex_unlock(&g_ir_cut.lock);
	}
	return count;
}

static DEVICE_ATTR(ir_cut_node, 0666, ir_cut_show, ir_cut_store);

int ir_cut_init(struct platform_device *pdev)
{
	int ret = -1;
	struct device_node *np = NULL;
	struct cccore *ccc = NULL;

	ccc = dev_get_drvdata(&pdev->dev);
	np = pdev->dev.of_node;

	mutex_init(&g_ir_cut.lock);

	ret = ir_cut_request_pins(&g_ir_cut, ccc, np);
	if (ret < 0) {
		PRT_INFO("no ir cut pin!\n");
		return ret;
	}

	g_ir_cut.clk_gate = devm_clk_get(&pdev->dev, GPADC_CLK_GATE);
	if (IS_ERR_OR_NULL(g_ir_cut.clk_gate)) {
		PRT_INFO("failed to get apb gpadc clock gate!\n");
		return -1;
	}

	g_ir_cut.clk_reset = devm_clk_get(&pdev->dev, GPADC_CLK_REST);
	if (IS_ERR_OR_NULL(g_ir_cut.clk_reset)) {
		PRT_INFO("failed to get apb gpadc clock reset!\n");
		return -1;
	}

	g_ir_cut.clk_gpadc = devm_clk_get(&pdev->dev, GPADC_MODULE_CLK);
	if (IS_ERR_OR_NULL(g_ir_cut.clk_gpadc)) {
		PRT_INFO("failed to get gpadc module clock!\n");
		return -1;
	}

	clk_prepare_enable(g_ir_cut.clk_gate);
	clk_prepare_enable(g_ir_cut.clk_reset);
	clk_prepare_enable(g_ir_cut.clk_gpadc);

	g_ir_cut.base = of_iomap(np, 0);
	if (IS_ERR(g_ir_cut.base)) {
		PRT_INFO("failed to remap gpadc IO!\n");
		return -1;
	}

	csp_gpadc_init(g_ir_cut.base);

	device_create_file(ccc->dev, &dev_attr_ir_cut_node);

	g_ir_cut.ir_task = kthread_create(threadfunc, &g_ir_cut, "ir_cut_task");
	if (IS_ERR_OR_NULL(g_ir_cut.ir_task)) {
		PRT_ERR("create ir cut thread failed\n");
		g_ir_cut.ir_task = NULL;
		return -1;
	} else
		wake_up_process(g_ir_cut.ir_task);

	return ret;
}

void ir_cut_exit(struct cccore *ccc)
{
	if (gpio_is_valid(g_ir_cut.ir_cut_pin0))
		devm_gpio_free(ccc->dev, g_ir_cut.ir_cut_pin0);
	if (gpio_is_valid(g_ir_cut.ir_cut_pin1))
		devm_gpio_free(ccc->dev, g_ir_cut.ir_cut_pin1);

	if (g_ir_cut.ir_task) {
		kthread_stop(g_ir_cut.ir_task);
		g_ir_cut.ir_task = NULL;
	}
}
