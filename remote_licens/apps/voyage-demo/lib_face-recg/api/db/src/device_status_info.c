/*
 * device_status_info.c - Interface for setting device status parameter functions
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
#include "db_device_status_info.h"
#include "db_config.h"

#define LOG_TAG "API_DB"
#include <log/log.h>

/**
 * device_status_info_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void device_status_info_print(device_status_param_t *p_info)
{
	assert(p_info != NULL);

	ALOGD("[%s:%d] device_status_info:\n"
		"company_name:%s\n"
		"company_logo:%s\n"
		"device_address:%s\n"
		"language:%d\n"
		"punch_mode:%d\n"
		"debug_switch:%d\n",
		__func__,
		__LINE__,
		p_info->company_name,
		p_info->company_logo,
		p_info->device_address,
		p_info->language,
		p_info->punch_mode,
		p_info->debug_switch);
}

/**
 * db_device_status_param_update - Update device status information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_status_param_update(device_status_param_t *p_info)
{
	int ret = 0;

	assert(p_info != NULL);

	ret += db_config_set_string(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_COMPANYNAME,
		p_info->company_name);
	ret += db_config_set_string(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_COMPANYLOGO,
		p_info->company_logo);
	ret += db_config_set_string(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_DEVICEADDRESS,
		p_info->device_address);
	ret += db_config_set_int(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_LANGUAGE,
		p_info->language);
	ret += db_config_set_int(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_PUNCHMODE,
		p_info->punch_mode);
	ret += db_config_set_int(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_DEBUGSWITCH,
		p_info->debug_switch);

	return ret;
}

/**
 * db_device_status_param_query - Query device status information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns number of data queried.
 */
int db_device_status_param_query(device_status_param_t *p_info)
{
	assert(p_info != NULL);

	memset(p_info, 0, sizeof(device_status_param_t));

	db_config_get_string(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_COMPANYNAME,
		p_info->company_name,
		sizeof(p_info->company_name),
		DBCONF_DEVICESTATUSPARAM_COMPANYNAME_VALUE);
	db_config_get_string(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_COMPANYLOGO,
		p_info->company_logo,
		sizeof(p_info->company_logo),
		DBCONF_DEVICESTATUSPARAM_COMPANYLOGO_VALUE);
	db_config_get_string(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_DEVICEADDRESS,
		p_info->device_address,
		sizeof(p_info->device_address),
		DBCONF_DEVICESTATUSPARAM_DEVICEADDRESS_VALUE);
	p_info->language = db_config_get_int(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_LANGUAGE,
		atoi(DBCONF_DEVICESTATUSPARAM_LANGUAGE_VALUE));
	p_info->punch_mode = db_config_get_int(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_PUNCHMODE,
		atoi(DBCONF_DEVICESTATUSPARAM_PUNCHMODE_VALUE));
	p_info->debug_switch = db_config_get_int(DBCONF_DEVICESTATUSPARAM,
		DBCONF_DEVICESTATUSPARAM_DEBUGSWITCH,
		atoi(DBCONF_DEVICESTATUSPARAM_DEBUGSWITCH_VALUE));

	return 1;
}

