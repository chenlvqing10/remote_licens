/*
 * record_info.h - Define business layer interface for punch record
 *    registration.
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
#ifndef _RECORD_INFO_H
#define _RECORD_INFO_H

#include "ee_queue.h"

typedef struct tag_punch_record_info {
	/* Employee mumber(E.g SW200) */
	char employee_number[32];

	/* Name */
	char name[32];

	/* Gender(E.g "Man/Women/..." */
	char gender[8];

	/* ID card number */
	char id_card_number[32];

	/* Department name(E.g "Software Lab001") */
	char department_name[128];

	/* Register photo name.(Including jpg suffix) */
	char regist_photo_name[128];

	/* Face recognition score */
	double match_score;

	/* The address where punched */
	char address[32];

	/* Punch time(ms) */
	double punch_time;

	/* The name of the large picture of the punch record */
	char punch_live_large_pic_name[128];

	/* The name of the small picture of the punch record */
	char punch_live_small_pic_name[128];

	/* Punch pass. Face recognition is below a preset threshold, or a nonliving
	 * record.
	 * Options:
	 *	0: recognition reject
	 *	1: recognition passed
	 */
	char punch_pass;

	/* Bidirectional queue */
	ee_queue_t queue;

} full_punch_rec_info_t;

typedef struct tag_punch_record_info_of_upload {
	int id;
	double match_score;
	/* The address where punched */
	char address[32];
	/* Punch time(ms) */
	double punch_time;
	/* The name of the large picture of the punch record */
	char punch_live_large_pic_name[128];
	/* The name of the small picture of the punch record */
	char punch_live_small_pic_name[128];
	/* Punch pass. Face recognition is below a preset threshold, or a nonliving
	 * record.
	 * Options:
	 *	0: recognition reject
	 *	1: recognition passed
	 */
	char punch_pass;

	/* Employee mumber(E.g SW200) */
	char employee_number[32];
	/* Name */
	char name[32];
	/* Gender(E.g "Man/Women/..." */
	char gender[8];
	char nationa[32];
	/* ID card number */
	char id_card_number[32];
	long mobile;
	/* Employee mumber(E.g SW200) */
	char department_name[32];
	int access_right;
	double temporary_access_start_time;
	double temporary_access_end_time;
	char access_card_number[32];

	/* Bidirectional queue */
	ee_queue_t queue;
} full_punch_rec_info_t_of_upload;

#endif /* _RECORD_INFO_H */

