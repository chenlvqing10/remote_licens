/******************************************************************************
 *
 * Copyright(c) 2013 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
/*
 * Description:
 *	This file can be applied to following platforms:
 *	CONFIG_PLATFORM_LOMBO_N7
 */
#include <drv_types.h>
#ifdef CONFIG_GPIO_WAKEUP
#include <linux/gpio.h>
#endif

#ifdef CONFIG_MMC
#if defined(CONFIG_PLATFORM_ARM_SUN50IW1P1)

#endif
#ifdef CONFIG_GPIO_WAKEUP

#endif
#endif /* CONFIG_MMC */
#include <mach/lombo_wireless_pm.h>
#include <mach/lombo_mmc.h>

/*
 * Return:
 *	0:	power on successfully
 *	others: power on failed
 */
int platform_wifi_power_on(void)
{
	int ret = 0;
	lombo_wireless_set_power(WL_WIFI, PWR_ON);
	mdelay(100);
	lombo_wireless_set_enable(WL_WIFI, DISABLED);
	mdelay(100);
	lombo_wireless_set_enable(WL_WIFI, ENABLED);
	mdelay(100);
	lbsdc_scan_card(1);

	return 0;
}

void platform_wifi_power_off(void)
{
	lbsdc_scan_card(0);
	mdelay(100);
	lombo_wireless_set_enable(WL_WIFI, DISABLED);
	mdelay(100);
	lombo_wireless_set_power(WL_WIFI, PWR_OFF);
}
