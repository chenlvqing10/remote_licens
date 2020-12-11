/*
 * common.h - test driver common head file
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

#ifndef __COMMON_H
#define __COMMON_H

#include <mach/debug.h>

enum {
	CLK_TEST,
	PWM_TEST,
	VC_TEST,
	MEMCTRL_TEST,
	DMA_TEST,
	TEST_CNT,
};

#define DEV_NAME	"lombo_drv_test"

struct drv_test_dev {
	struct device dev;
	struct task_struct *test_task;
	unsigned long test_bitmap;	/* bitmap of the drivers to be tested */
};

#define to_drv_test_dev(x) container_of((x), struct drv_test_dev, dev);

#define	DEF_TEST_FUNC(name, bit)					\
	ssize_t name##_show(struct device *dev,				\
			struct device_attribute *attr,			\
			char *buf)					\
	{								\
		int ret;						\
		struct drv_test_dev *pdev = to_drv_test_dev(dev);	\
									\
		if (!test_and_set_bit((bit), &pdev->test_bitmap))	\
			ret = sprintf(buf, #name" starting now...\n");	\
		else							\
			ret = sprintf(buf, #name" already running\n");	\
		wake_up_process(pdev->test_task);			\
		return (ssize_t)ret;					\
	}

/*
 * the below definitions are for module clocks
 */
#ifndef CONFIG_CLK_DRVIER_TEST
#define clk_test()
#else
void clk_test(void);
#endif

#ifndef CONFIG_PWM_DRVIER_TEST
#define pwm_test()
#else
void pwm_test(void);
#endif

#ifndef CONFIG_VC_DRVIER_TEST
#define vc_test()
#else
void vc_test(void);
#endif

#ifndef CONFIG_MEMCTRL_DRVIER_TEST
#define memctrl_test()
#else
void memctrl_test(void);
#endif

#ifndef CONFIG_DMA_DRVIER_TEST
#define dma_test()
#else
void dma_test(void);
#endif

#endif /* __COMMON_H */

