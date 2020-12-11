/*
 * device_status_info_test.h - For testing device status functions.
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

#define DB_TEST_DEMO
#ifdef DB_TEST_DEMO

#include "db_device_status_info.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	device_status_param_t devices_status_info;

	/* Configure the database */
	db_config();

	/* Update the cloud flatform information */
	device_status_param_t info = {
		.company_name = "lombo",
		.company_logo = "logo.jpg",
		.device_address = "ZhuHai",
		.language = 1,
		.punch_mode = 0,
		.debug_switch = 1,
	};
	db_device_status_param_update(&info);

	/*Query the device status information */
	db_device_status_param_query(&devices_status_info);
	device_status_info_print(&devices_status_info);

	return 0;
}

#endif /* DB_TEST_DEMO */

