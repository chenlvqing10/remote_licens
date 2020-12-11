/*
 * pwm.h - pwm head file
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

#ifndef __PWM_H___
#define __PWM_H___

#include <linux/pwm.h>
#include <linux/platform_device.h>

/* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME	"PWM"

/**
 * struct extra_pwm_ops - extra operations for lombo PWM controller
 * @xxx: any operations for lombo pwm
 */
struct extra_pwm_ops {
	int (*xxx)(struct pwm_chip *chip, struct pwm_device *pwm);
};

struct lombo_pwm_chip {
	struct pwm_chip chip;
	const struct extra_pwm_ops *ops;
	void __iomem *reg;
	int irq; /* pwm irq in linux */
};

/**
 * struct pwm_data - per-pwm private data, for chip operations
 * @polarity: pwm's polarity. p or n output polarity? to realize...
 * @duty_ns: pwm's duty in ns
 * @period_ns: pwm's period in ns
 */
struct pwm_data {
	enum pwm_polarity polarity;
	int duty_ns;
	int period_ns;
	int period_cnt;
	int duty_cnt;
};

int pwm_irq_init(struct platform_device *pdev, void __iomem *reg);
void pwm_irq_exit(struct platform_device *pdev, void __iomem *reg);

#endif /* __PWM_H___ */

