/*
 * device_net_param_test.h - For testing cloud platform functions.
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

#ifdef DB_TEST_DEMO

#include "device_net_param.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	device_net_param_t cloud_info;

	/* Configure the database */
	db_config();

	/* Update the cloud flatform information */
	device_net_param_t info = {
		.enable_eth = 1,
		.enable_eth_priority = 1,
		.enable_eth_dhcp = 0,
		.eth_ip = "192.168.1.11",
		.eth_mask = "255.255.255.0",
		.eth_gateway = "192.168.1.1",
		.eth_dns = "192.168.1.1",
		.enable_wifi = 1,
		.enable_hotspot = 1,
		.hotspot_name = "DuDu_WiFi",
		.hotspot_password = "12345678",
	};
	db_device_net_param_update(&info);

	/*Query the cloud platform information */
	db_device_net_param_query(&cloud_info);
	device_net_param_print(&cloud_info);

	return 0;
}

#endif /* DB_TEST_DEMO */

