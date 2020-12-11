/*
 * cloud_platform_test.h - For testing cloud platform functions.
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

#include "cloud_platform.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	cloud_platform_t cloud_info;

	/* Configure the database */
	db_config();

	/* Update the cloud flatform information */
	cloud_platform_t info = {
		.ai_server_url = "ai-server.com",
		.attendance_url = "attendance-server.com",
		.fota_url = "fota-server.com",
	};
	db_cloud_platform_update(&info);

	/*Query the cloud platform information */
	db_cloud_platform_query(&cloud_info);
	cloud_platform_print(&cloud_info);

	return 0;
}

#endif /* DB_TEST_DEMO */

