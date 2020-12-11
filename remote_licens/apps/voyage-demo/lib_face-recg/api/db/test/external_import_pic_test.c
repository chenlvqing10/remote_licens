/*
 * external_import_pic_test.c - For testing external import pic functions.
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

#include "external_import_pic.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	ee_queue_t head;

	/* Configure the database */
	db_config();

	/* Add a user */
	printf("test add external import pic:\n");
	external_import_pic_t pic_a = {
		.pic_name = "pic1.jpg",
	};
	db_external_import_pic_insert(&pic_a);

	external_import_pic_t pic_b = {
		.pic_name = "pic2.jpg"
	};
	db_external_import_pic_insert(&pic_b);

	/* Delete the picture with the specified picture name */
	db_external_import_pic_delete("pic2.jpg");

	/* Update the user information */
	external_import_pic_t pic_c = {
		.pic_name = "old_pic1.jpg"
	};
	db_external_import_pic_update("pic1.jpg", &pic_c);

	/*Query the user information */
	db_external_import_pic_query_begin(&head);
	external_import_pic_print(&head);
	db_external_import_pic_query_end(&head);

	return 0;
}

#endif /* DB_TEST_DEMO */

