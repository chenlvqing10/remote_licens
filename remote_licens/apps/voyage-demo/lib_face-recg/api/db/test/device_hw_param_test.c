/*
 * device_hw_param_test.h - For testing device hardware functions.
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

#include "device_hw_param.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	device_hw_param_t device_hw_info;

	/* Configure the database */
	db_config();

	/* Update the cloud flatform information */
	device_hw_param_t info = {
		.volume = 50,
		.auto_display_brightness = 1,
		.display_brightness = 50,
		.infrared_light_set = 0,
		.light_sensitive = 1,
		.human_induction = 1,
		.auto_screen_off = 0,
		.auto_screen_off_timeout = 15
	};
	db_device_hw_param_update(&info);

	/*Query the device hardware information */
	db_device_hw_param_query(&device_hw_info);
	device_hw_param_print(&device_hw_info);

	return 0;
}

#endif /* DB_TEST_DEMO */

