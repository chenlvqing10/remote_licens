/*
 * manager_info_test.h - For testing management functions.
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

#include "manager_info.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	ee_queue_t head;

	/* Configure the database */
	db_config();

	/* Add a user */
	printf("Test add admin user ");
	manager_info_t user_a = {
		.user_name = "root",
		.user_password = "12345678",
	};
	db_manager_info_insert(&user_a);

	manager_info_t user_b = {
		.user_name = "yangwengang",
		.user_password = "12345678",
	};
	db_manager_info_insert(&user_b);

	/* Delete the admin user with the specified user name */
	db_manager_info_delete("root");

	/* Update the admin user information */
	manager_info_t user_c = {
		.user_name = "yangwengang",
		.user_password = "87654321",
	};
	db_manager_info_update("yangwengang", &user_c);

	/*Query the admin user information */
	db_manager_info_query_begin("yangwengang", &head);
	manager_info_print(&head);
	db_manager_info_query_end(&head);

	return 0;
}

#endif /* DB_TEST_DEMO */

