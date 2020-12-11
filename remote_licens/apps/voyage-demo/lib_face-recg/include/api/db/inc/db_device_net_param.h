/*
 * device_net_param.h - Interface for setting device net parameter functions
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

#ifndef __DEVICE_NET_PARAM_H
#define __DEVICE_NET_PARAM_H

#include "ee_queue.h"

typedef struct tag_device_net_param {
	/* Eth settings.
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char enable_eth;

	/* Eth priority
	 * Options:
	 *	0: Eth
	 *	1: WiFi
	 */
	char enable_eth_priority;

	/* DHCP
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char enable_eth_dhcp;

	/* Eth ip address */
	char eth_ip[256];

	/* Eth subnet mask */
	char eth_mask[256];

	/* Eth gateway */
	char eth_gateway[256];

	/* Eth DNS */
	char eth_dns[256];

	/* WiFi
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char enable_wifi;

	/* Hotspot
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char enable_hotspot;

	/* Hotspot name */
	char hotspot_name[32];

	/* Hotspot password */
	char hotspot_password[9];
} device_net_param_t;

/**
 * device_net_param_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void device_net_param_print(device_net_param_t *p_info);

/**
 * db_device_net_param_update - Update cloud platform information.
 * @p_info: A pointer to device_net_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_net_param_update(device_net_param_t *p_info);

/**
 * db_device_net_param_query - Query cloud platform information.
 * @p_info: A pointer to device_net_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_net_param_query(device_net_param_t *p_info);

#endif /* __DEVICE_NET_PARAM_H */

