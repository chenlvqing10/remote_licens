/*
 * punch_record_test.h - For testing punch record.
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

#include "db_punch_record.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	int count = 0;
	ee_queue_t head;

	/* Configure the database */
	db_config();

	/* Add a user */
	printf("test add punch record:\n");
	punch_record_t info_a = {
		.fk_per_id = 2,
		.match_score = 55.5,
		.address = "ZhuHai",
		.punch_time = 1557967746514,
		.punch_live_large_pic_name = "1-large.jpg",
		.punch_live_small_pic_name = "1-small.jpg",
		.punch_pass = 0,
	};
	db_punch_record_insert(&info_a);

	punch_record_t info_b = {
		.fk_per_id = 4,
		.match_score = 66.6,
		.address = "ShenZheng",
		.punch_time = 1557967745514,
		.punch_live_large_pic_name = "2-large.jpg",
		.punch_live_small_pic_name = "2-small.jpg",
		.punch_pass = 1,
	};
	db_punch_record_insert(&info_b);

	/* Delete the user with the specified user name */
	db_punch_record_delete("LiLei");

	/* Update the user information */
	punch_record_t user_c = {
		.fk_per_id = 6,
		.match_score = 88.8,
		.address = "ZhuHai",
		.punch_time = 1557967744514,
		.punch_live_large_pic_name = "3-large.jpg",
		.punch_live_small_pic_name = "3-small.jpg",
		.punch_pass = 0,
	};
	db_punch_record_update(1, &user_c);

	/* Get the number of punch record. */
	count = db_punch_record_count("", 1, 0, 1000);
	printf("Count:%d\n", count);

	/*Query the user information */
	db_punch_record_query_begin("HanMeiMei", 1, 0, 1000, 5, 10, &head);
	punch_record_print(&head);
	db_punch_record_query_end(&head);

	return 0;
}

#endif /* DB_TEST_DEMO */

