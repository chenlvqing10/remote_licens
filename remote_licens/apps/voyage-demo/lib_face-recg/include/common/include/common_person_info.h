/*
 * person_info.h - Interface for management personnel registration
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

#ifndef __PERSON_INFO_H
#define __PERSON_INFO_H

#include "ee_queue.h"

#define FACE_FEATURES_DATA_LENGTH	1024

typedef struct tag_person_info {
	int id;

	/* Employee mumber(E.g SW200) */
	char employee_number[32];

	/* Name */
	char name[32];

	/* Gender(E.g "Man/Women/..." */
	char gender[8];

	/* Nationa */
	char nationa[32];

	/* ID card number */
	char id_card_number[32];

	/* Telephone number */
	char mobile[20];

	/* Department name(E.g "Software Lab001") */
	char department_name[128];

	/* Access right
	 * Options:
	 *	0: Free access
	 *	1: Temporary access mode 1
	 *	2: Temporary access mode 2
	 */
	int access_right;

	/* When the permission is temporary access permission mode 1,
	 * Allowed start time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	double temporary_access_start_time;

	/* When the permission is temporary access permission mode 1,
	 * Allowed end time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	double temporary_access_end_time;

	/* Allowed number of passes
	 * When the permission is temporary access permission mode 2
	 */
	int temporary_access_times;

	/* Access card number.
	 * Effective when the punch mode is "Card reader"
	 */
	char access_card_number[32];

	/* Remarks: Used to fill in the description information */
	char remarks[128];

	/* Register photo name.(Including jpg suffix) */
	char regist_photo_name[128];

	/* Face feature value. Used to store facial feature values */
	unsigned char face_features[FACE_FEATURES_DATA_LENGTH];

	/* face key points */
	char face_key_points[256];

	/* norm */
	double face_norm[2];

	/* Bidirectional queue */
	ee_queue_t queue;
} person_info_t;

#endif /* __PERSON_INFO_H */

