/*
 * device_hw_param.c - Interface for setting device hardware parameter functions
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "db_sqlite_middle.h"
#include "db_device_hw_param.h"
#include "db_config.h"

#define LOG_TAG "API_DB"
#include <log/log.h>

/**
 * device_hw_param_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void device_hw_param_print(device_hw_param_t *p_info)
{
	assert(p_info != NULL);

	ALOGD("[%s:%d] device_hw_param:\n"
		"volume:%d\n"
		"auto_display_brightness:%d\n"
		"display_brightness:%d\n"
		"infrared_light_set:%d\n"
		"light_sensitive:%d\n"
		"human_induction:%d\n"
		"auto_screen_off:%d\n"
		"auto_screen_off_timeout:%d\n",
		__func__,
		__LINE__,
		p_info->volume,
		p_info->auto_display_brightness,
		p_info->display_brightness,
		p_info->infrared_light_set,
		p_info->light_sensitive,
		p_info->human_induction,
		p_info->auto_screen_off,
		p_info->auto_screen_off_timeout);
}

/**
 * db_device_hw_param_update - Update device hardware information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_hw_param_update(device_hw_param_t *p_info)
{
	int ret = 0;

	assert(p_info != NULL);

	ret += db_config_set_int(DBCONF_DEVICEHWPARAM, DBCONF_DEVICEHWPARAM_VOLUME,
		p_info->volume);
	ret += db_config_set_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_AUTODISPLAYBRIGHTNESS,
		p_info->auto_display_brightness);
	ret += db_config_set_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_DISPLAYBRIGHTNESS,
		p_info->display_brightness);
	ret += db_config_set_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_INFRAREDLIGHTSET,
		p_info->infrared_light_set);
	ret += db_config_set_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_LIGHTSENSITIVE,
		p_info->light_sensitive);
	ret += db_config_set_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_HUMANINDUCTION,
		p_info->human_induction);
	ret += db_config_set_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_AUTOSCREENOFF,
		p_info->auto_screen_off);
	ret += db_config_set_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_AUTOSCREENOFFTIMEOUT,
		p_info->auto_screen_off_timeout);

	return ret;
}

/**
 * db_device_hw_param_query - Query device hardware information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns number of data queried.
 */
int db_device_hw_param_query(device_hw_param_t *p_info)
{
	assert(p_info != NULL);

	memset(p_info, 0, sizeof(device_hw_param_t));

	p_info->volume = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_VOLUME,
		atoi(DBCONF_DEVICEHWPARAM_VOLUME_VALUE));
	p_info->auto_display_brightness = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_AUTODISPLAYBRIGHTNESS,
		atoi(DBCONF_DEVICEHWPARAM_AUTODISPLAYBRIGHTNESS_VALUE));
	p_info->display_brightness = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_DISPLAYBRIGHTNESS,
		atoi(DBCONF_DEVICEHWPARAM_DISPLAYBRIGHTNESS_VALUE));
	p_info->infrared_light_set = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_INFRAREDLIGHTSET,
		atoi(DBCONF_DEVICEHWPARAM_INFRAREDLIGHTSET_VALUE));
	p_info->light_sensitive = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_LIGHTSENSITIVE,
		atoi(DBCONF_DEVICEHWPARAM_LIGHTSENSITIVE_VALUE));
	p_info->human_induction = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_HUMANINDUCTION,
		atoi(DBCONF_DEVICEHWPARAM_HUMANINDUCTION_VALUE));
	p_info->auto_screen_off = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_AUTOSCREENOFF,
		atoi(DBCONF_DEVICEHWPARAM_AUTOSCREENOFF_VALUE));
	p_info->auto_screen_off_timeout = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_AUTOSCREENOFFTIMEOUT,
		atoi(DBCONF_DEVICEHWPARAM_AUTOSCREENOFFTIMEOUT_VALUE));
	p_info->screen_saver_off = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_SCREENSAVEROFF,
		atoi(DBCONF_DEVICEHWPARAM_SCREENSAVEROFF_VALUE));
	p_info->screen_saver_off_timeout = db_config_get_int(
		DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_SCREENSAVEROFFTIMEOUT,
		atoi(DBCONF_DEVICEHWPARAM_SCREENSAVEROFFTIMEOUT_VALUE));
	p_info->screen_saver_count = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_SCREENSAVERCOUNT,
		atoi(DBCONF_DEVICEHWPARAM_SCREENSAVERCOUNT_VALUE));
	p_info->screen_saver_time = db_config_get_int(DBCONF_DEVICEHWPARAM,
		DBCONF_DEVICEHWPARAM_SCREENSAVERTIME,
		atoi(DBCONF_DEVICEHWPARAM_SCREENSAVERTIME_VALUE));

	return 1;
}

