/*
 * face_recognition_param_test.c - For testing face recognition param functions.
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

#include "face_recognition_param.h"
#include "db_persistence.h"

int main(int argc, char *argv[])
{
	face_recognition_param_t param;

	/* Configure the database */
	db_config();

	/* Update the cloud flatform information */
	face_recognition_param_t info = {
		.identify_closest_distance = 1.5,
		.identify_furthest_distance = 3.2,
		.identify_threshold_score = 65.42,
		.nir_live_detect = 0,
	};
	db_face_recognition_param_update(&info);

	/*Query the cloud platform information */
	db_face_recognition_param_query(&param);
	face_recognition_param_print(&param);

	return 0;
}

#endif /* DB_TEST_DEMO */

