/*
 * lombo_wireless_pm.h - Generic definitions of Lombo wireless pm driver
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#ifndef __LOMBO_WIRELESS_PM_H__
#define __LOMBO_WIRELESS_PM_H__

/**
 * enum wireless_type - type of wireless module.
 * @WL_WIFI: wifi.
 * @WL_BT: bluetooth.
 */
enum wireless_type {
	WL_WIFI = 0,
	WL_BT
};

/**
 * enum power_type - type of power on.
 * @PWR_ON: power on.
 * @PWR_OFF: power off.
 */
enum power_type {
	PWR_ON = 0,
	PWR_OFF
};

/**
 * enum startup_type - type of startup.
 * @ENABLED: enabled.
 * @DISABLED: disabled.
 */
enum startup_type {
	ENABLED = 0,
	DISABLED
};

/**
 * lombo_wireless_set_power - set power control pin state.
 * @wl_type: type of wireless module.
 * @pwr_type: type of power on.
 *
 * return 0 if success; otherwise failed.
 */
extern int lombo_wireless_set_power(enum wireless_type wl_type,
		enum power_type pwr_type);

/**
 * lombo_wireless_set_enable - set enabled state of wireless state.
 * @wl_type: type of wireless module.
 * @su_type: type of startup.
 *
 * return 0 if success; otherwise failed.
 */
extern int lombo_wireless_set_enable(enum wireless_type wl_type,
		enum startup_type su_type);

#endif /* __LOMBO_WIRELESS_PM_H__ */
