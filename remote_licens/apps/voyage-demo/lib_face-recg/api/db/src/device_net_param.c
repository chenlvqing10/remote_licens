/*
 * device_net_param.c - Interface for setting device net parameter functions
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
#include "db_device_net_param.h"
#include "db_config.h"

#define LOG_TAG "API_DB"
#include <log/log.h>

/**
 * device_net_param_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void device_net_param_print(device_net_param_t *p_info)
{
	assert(p_info != NULL);

	ALOGD("[%s:%d] device_net_param:\n"
		"enable_eth=%d  enable_eth_priority=%d  enable_eth_dhcp=%d\n"
		"eth_ip=%s  eth_mask=%s  eth_gateway=%s  eth_dns=%s\n"
		"enable_wifi=%d  enable_hotspot=%d hotspot_name=%s\n"
		"hotspot_password=%s\n",
		__func__,
		__LINE__,
		p_info->enable_eth,
		p_info->enable_eth_priority,
		p_info->enable_eth_dhcp,
		p_info->eth_ip,
		p_info->eth_mask,
		p_info->eth_gateway,
		p_info->eth_dns,
		p_info->enable_wifi,
		p_info->enable_hotspot,
		p_info->hotspot_name,
		p_info->hotspot_password);
}

/**
 * db_device_net_param_update - Update cloud platform information.
 * @p_info: A pointer to device_net_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_net_param_update(device_net_param_t *p_info)
{
	int ret = 0;

	assert(p_info != NULL);

	ret += db_config_set_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEETH,
		p_info->enable_eth);
	ret += db_config_set_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEETHPRIORITY,
		p_info->enable_eth_priority);
	ret += db_config_set_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEETHDHCP,
		p_info->enable_eth_dhcp);
	ret += db_config_set_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ETHIP,
		p_info->eth_ip);
	ret += db_config_set_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ETHMASK,
		p_info->eth_mask);
	ret += db_config_set_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ETHGATEWAY,
		p_info->eth_gateway);
	ret += db_config_set_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ETHDNS,
		p_info->eth_dns);
	ret += db_config_set_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEWIFI,
		p_info->enable_wifi);
	ret += db_config_set_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEHOTSPOT,
		p_info->enable_hotspot);
	ret += db_config_set_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_HOTSPOTNAME,
		p_info->hotspot_name);
	ret += db_config_set_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_HOTSPOTPASSWORD,
		p_info->hotspot_password);

	return ret;
}

/**
 * db_device_net_param_query - Query cloud platform information.
 * @p_info: A pointer to device_net_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_net_param_query(device_net_param_t *p_info)
{
	assert(p_info != NULL);

	memset(p_info, 0, sizeof(device_net_param_t));

	p_info->enable_eth = db_config_get_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEETH,
		atoi(DBCONF_DEVICENETPARAM_ENABLEETH_VALUE));
	p_info->enable_eth_priority = db_config_get_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEETHPRIORITY,
		atoi(DBCONF_DEVICENETPARAM_ENABLEETHPRIORITY_VALUE));
	p_info->enable_eth_dhcp = db_config_get_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEETHDHCP,
		atoi(DBCONF_DEVICENETPARAM_ENABLEETHDHCP_VALUE));
	db_config_get_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ETHIP,
		p_info->eth_ip,
		sizeof(p_info->eth_ip),
		DBCONF_DEVICENETPARAM_ETHIP_VALUE);
	db_config_get_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ETHMASK,
		p_info->eth_mask,
		sizeof(p_info->eth_mask),
		DBCONF_DEVICENETPARAM_ETHMASK_VALUE);
	db_config_get_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ETHGATEWAY,
		p_info->eth_gateway,
		sizeof(p_info->eth_gateway),
		DBCONF_DEVICENETPARAM_ETHGATEWAY_VALUE);
	db_config_get_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ETHDNS,
		p_info->eth_dns,
		sizeof(p_info->eth_dns),
		DBCONF_DEVICENETPARAM_ETHDNS_VALUE);
	p_info->enable_wifi = db_config_get_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEWIFI,
		atoi(DBCONF_DEVICENETPARAM_ENABLEWIFI_VALUE));
	p_info->enable_hotspot = db_config_get_int(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_ENABLEHOTSPOT,
		atoi(DBCONF_DEVICENETPARAM_ENABLEHOTSPOT_VALUE));
	db_config_get_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_HOTSPOTNAME,
		p_info->hotspot_name,
		sizeof(p_info->hotspot_name),
		DBCONF_DEVICENETPARAM_HOTSPOTNAME_VALUE);
	db_config_get_string(DBCONF_DEVICENETPARAM,
		DBCONF_DEVICENETPARAM_HOTSPOTPASSWORD,
		p_info->hotspot_password,
		sizeof(p_info->hotspot_password),
		DBCONF_DEVICENETPARAM_HOTSPOTPASSWORD_VALUE);

	return 1;
}

