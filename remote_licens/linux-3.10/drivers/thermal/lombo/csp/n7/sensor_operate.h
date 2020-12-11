/*
 * sensor_operate.h - CSP define of Sensor
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

#ifndef __SENSOR_OPERATE_H
#define __SENSOR_OPERATE_H

#include <linux/io.h>
#include <mach/csp.h>
#include <mach/common.h>
#include <linux/platform_data/lombo_thermal.h>

#define GPADC_BUS_CLOCK_GATING		(VA_PRCM_APB_GAT1)
#define GPADC_BUS_RESET			(VA_PRCM_APB_RST1)

#ifdef CONFIG_ARCH_LOMBO_N7V1
#define GPADC_CLK_CTRL			(VA_PRCM_GPADC_CLK_CTRL)
#endif

#define GPADC_ENABLE			(VA_GPADC_GPAEN)
#define GPADC_SENSOR_CTRL		(VA_GPADC_GSC)
#define GPADC_SENSOR_DATA0		(VA_GPADC_S0DAT)

int csp_sensor_get_temp(void *base, int id);
void csp_sensor_hw_init(void *base);
#endif

