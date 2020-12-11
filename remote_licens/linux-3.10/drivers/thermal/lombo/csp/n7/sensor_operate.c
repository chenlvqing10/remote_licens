/*
 * sensor_operate.c - Sensor CSP
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
#include "sensor_operate.h"

int csp_sensor_get_temp(void *base, int id)
{
	int temp_int = 0;
	float temp = 0.0f;
	float slope = 0.0f;
	int value = 0;
	int offset = 0;
	u32 p1 = 0, p2 = 0, ret = 0;

	ret = lombo_func2(&p1, &p2);
	if (ret == 0) {
		if (p1 == 1) {
			offset = 349;
			slope = 0.06459;
		} else {
			offset = 349;
			slope = 0.06562;
		}
	} else {
		BUG();
	}

	value = (readl((void *)(0xF1203800 + 0x18)) >> 8) & 0x3FF;
	if (value)
		offset = value;

#ifdef CONFIG_ARCH_LOMBO_N7V1_EVB_SIP
	if (id == 0)
		slope = 0.06325;
	else if (id == 1)
		slope = 0.06375;
	else if (id == 2)
		slope = 0.0636;
#endif
	value = readl(base);
	BUG_ON(value == 0);

	temp = (150 + slope * (offset - value)) * 1000;
	temp_int = (int)temp;

	return temp_int;
}

void csp_sensor_hw_init(void *base)
{
	u32 value = 0;

	/* enable bus clock gating */
	value = readl((void *)GPADC_BUS_CLOCK_GATING);
	value |= (0x1 << 6);
	writel(value, (void *)GPADC_BUS_CLOCK_GATING);

	/* enable bus reset */
	value = readl((void *)GPADC_BUS_RESET);
	value |= (0x1 << 6);
	writel(value, (void *)GPADC_BUS_RESET);

#ifdef CONFIG_ARCH_LOMBO_N7V1
	/* gpadc clock config */
	value = 0;
	value |= (0x1 << 0);
	value |= (0x1 << 31);
	writel(value, (void *)GPADC_CLK_CTRL);
#endif

	/* enable gpadc */
	value = readl((void *)GPADC_ENABLE);
	value |= (0x1 << 0);
	writel(value, (void *)GPADC_ENABLE);

	value = readl((void *)GPADC_SENSOR_CTRL);
	value |= (0x1 << 1); /* enable data average */
	value |= (0x1 << 0); /* enable sensor */
	writel(value, (void *)GPADC_SENSOR_CTRL);
}

