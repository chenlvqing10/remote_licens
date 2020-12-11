/*
 * person_info_test.h - For testing user functions.
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

#include "db_person_info.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	int count = 0;
	ee_queue_t head;

	/* Configure the database */
	db_config();

	/* Add a user */
	printf("test add userinfo:\n");
	person_info_t user_a = {
		.employee_number = "SW800",
		.name = "Poly",
		.gender = "Man",
		.nationa = "汉",
		.id_card_number = "420527199801155332",
		.mobile = "1363295xxxx",
		.department_name = "Tech Software",
		.access_right = 0,
		.temporary_access_start_time = 0,
		.temporary_access_end_time = 0,
		.temporary_access_times = 0,
		.access_card_number = "",
		.regist_photo_name = "Poly.jpg",
	};
	db_person_info_insert(&user_a);

	person_info_t user_b = {
		.employee_number = "SD900",
		.name = "HanMeiMei",
		.gender = "Woman",
		.nationa = "维吾尔",
		.id_card_number = "420527200002155331",
		.mobile = "1583044xxxx",
		.department_name = "Tech Software",
		.access_right = 0,
		.temporary_access_start_time = 0,
		.temporary_access_end_time = 0,
		.temporary_access_times = 0,
		.access_card_number = "",
		.regist_photo_name = "HanMeiMei.jpg",
	};
	db_person_info_insert(&user_b);

	/* Delete the user with the specified user name */
	db_person_info_delete("Poly");

	/* Update the user information */
	person_info_t user_c = {
		.employee_number = "SD900",
		.name = "HanMeiMei",
		.gender = "男",
		.nationa = "维吾尔",
		.id_card_number = "420527200002155331",
		.mobile = "13488888888",
		.department_name = "Tech Software",
		.access_right = 0,
		.temporary_access_start_time = 0,
		.temporary_access_end_time = 0,
		.temporary_access_times = 0,
		.access_card_number = "",
		.regist_photo_name = "HanMeiMei.jpg",
	};
	db_person_info_update("HanMeiMei", &user_c);

	/* Get the number of users. */
	count = db_person_info_count();
	printf("Count: %d\n", count);

	/*Query the user information */
	db_person_info_query_begin("", 0, 2, &head);
	person_info_print(&head);
	db_person_info_query_end(&head);

	return 0;
}

#endif /* DB_TEST_DEMO */

