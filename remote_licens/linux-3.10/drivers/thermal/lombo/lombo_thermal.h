/*
 * lombo_thermal.h - Generic definitions for LomboTech thermal
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

#ifndef __LOMBO_THERMAL_H__
#define __LOMBO_THERMAL_H__

#include <mach/debug.h>

 /* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME "THM"

#if defined(CONFIG_ARCH_LOMBO_N7)
#include "csp/n7/sensor_operate.h"
#elif defined(CONFIG_ARCH_LOMBO_N9)
#include "csp/n9/sensor_operate.h"
#endif

struct lombo_thermal_zone {
	struct thermal_zone_device *therm_dev;
	void __iomem *thermal_base;
	struct mutex th_lock;
	struct lombo_thsens_platform_data *trip_tab;
	enum thermal_device_mode mode;
	int id;
	int reg_val;
};

#endif /* __LOMBO_THERMAL_H__ */

