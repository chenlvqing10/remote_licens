/*
 * record_lookup.c - Processing interface for querying punch records.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <stdint.h>

#include "ee_queue.h"
#include "db_person_info.h"
#include "db_punch_record.h"
#include "time_conv.h"
#include "record_info.h"
#include "record_lookup.h"

/**
 * record_lookup_proc - Punch record lookup function.
 * @user_name: A pointer to username string.
 * @rec_pass: Punch pass flag
 * @rec_start_time: Record the start time of punching(ms).
 * @rec_end_time: Record the end time of punching(ms).
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_lookup_proc(char *user_name, int rec_pass, double rec_start_time,
	double rec_end_time)
{
	int count = 0;
	ee_queue_t head;
	person_info_t person;
	punch_record_t *q = NULL;
	full_punch_rec_info_t info;

	assert(user_name != NULL);

	/* Check the number of punch records */
	count = db_punch_record_count(user_name, rec_pass, rec_start_time, rec_end_time);

	/* Query the punch record from the database */
	db_punch_record_query_begin(user_name, rec_pass, rec_start_time, rec_end_time,
		0, count, &head);
	ee_queue_foreach(q, &head, punch_record_t, queue) {
		memset(&info, 0, sizeof(info));

		printf("id:%d match_score:%f fk_per_id:%d\n",
			q->id, q->match_score, q->fk_per_id);

		/* Find the person information corresponding to the punch
		 * record
		 */
		db_person_info_query_by_id(q->fk_per_id, &person);

		/* Employee mumber(E.g SW200) */
		snprintf(info.employee_number, sizeof(info.employee_number),
			"%s", person.employee_number);

		/* Name */
		/* When the record is rejected, the name of the person is not displayed */
		if (rec_pass != 0)
			snprintf(info.name, sizeof(info.name), "%s", person.name);

		/* Gender(E.g "Man/Women/..." */
		snprintf(info.gender, sizeof(info.gender), "%s", person.gender);

		/* ID card number */
		snprintf(info.id_card_number, sizeof(info.id_card_number), "%s",
			person.id_card_number);

		/* Department name(E.g "Software Lab001") */
		snprintf(info.department_name, sizeof(info.department_name), "%s",
			person.department_name);

		/* Register photo name.(Including jpg suffix) */
		snprintf(info.regist_photo_name, sizeof(info.regist_photo_name),
			"%s", person.regist_photo_name);

		/* Face recognition score */
		info.match_score = q->match_score;

		/* The address where punched */
		snprintf(info.address, sizeof(info.address), "%s", q->address);

		/* Punch time(ms) */
		info.punch_time = q->punch_time;

		/* The name of the large picture of the punch record */
		snprintf(info.punch_live_large_pic_name,
			sizeof(info.punch_live_large_pic_name), "%s",
			q->punch_live_large_pic_name);

		/* The name of the large picture of the punch record */
		snprintf(info.punch_live_small_pic_name,
			sizeof(info.punch_live_small_pic_name), "%s",
			q->punch_live_small_pic_name);

		/* Punch pass. */
		info.punch_pass = q->punch_pass;
	}
	db_punch_record_query_end(&head);

	return 0;
}

/**
 * record_lookup_get_full_info - Get complete information on punch records.
 * @rec: A pointer to pumch record.
 * @full_rec: Out, Punch record complete information.
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_lookup_get_full_info(punch_record_t *rec, full_punch_rec_info_t *full_rec)
{
	person_info_t person;

	assert((rec != NULL) && (full_rec != NULL));

	memset(full_rec, 0, sizeof(full_punch_rec_info_t));

	/* Find the person information corresponding to the punch
	 * record
	 */
	db_person_info_query_by_id(rec->fk_per_id, &person);

	/* Employee mumber(E.g SW200) */
	snprintf(full_rec->employee_number, sizeof(full_rec->employee_number),
		"%s", person.employee_number);

	/* Name */
	/* When the record is rejected, the name of the person is not displayed */
	if (rec->punch_pass != 0)
		snprintf(full_rec->name, sizeof(full_rec->name), "%s", person.name);

	/* Gender(E.g "Man/Women/..." */
	snprintf(full_rec->gender, sizeof(full_rec->gender), "%s", person.gender);

	/* ID card number */
	snprintf(full_rec->id_card_number, sizeof(full_rec->id_card_number), "%s",
		person.id_card_number);

	/* Department name(E.g "Software Lab001") */
	snprintf(full_rec->department_name, sizeof(full_rec->department_name), "%s",
		person.department_name);

	/* Register photo name.(Including jpg suffix) */
	snprintf(full_rec->regist_photo_name, sizeof(full_rec->regist_photo_name), "%s",
		person.regist_photo_name);

	/* Face recognition score */
	full_rec->match_score = rec->match_score;

	/* The address where punched */
	snprintf(full_rec->address, sizeof(full_rec->address), "%s", rec->address);

	/* Punch time(ms) */
	full_rec->punch_time = rec->punch_time;

	/* The name of the large picture of the punch record */
	snprintf(full_rec->punch_live_large_pic_name,
		sizeof(full_rec->punch_live_large_pic_name), "%s",
		rec->punch_live_large_pic_name);

	/* The name of the small picture of the punch record */
	snprintf(full_rec->punch_live_small_pic_name,
		sizeof(full_rec->punch_live_small_pic_name), "%s",
		rec->punch_live_small_pic_name);

	/* Punch pass */
	full_rec->punch_pass = rec->punch_pass;

	return 0;
}

/**
 * record_lookup_proc_begin - Query record information by name
 * @user_name: IN, A pointer to username string.
 * @punch_pass Punch pass flag
 * @start_time: Record the start time of punching(ms).
 * @end_time: Record the end time of punching(ms).
 * @person_info_queue_head: OUT, Point to the list header that stores person information.
 *
 * NOTE1: After the record information is used, it needs to be
 *	called record_lookup_proc_end
 * NOTE2: If user_name is an empty string, Query all punch record information.
 *
 * Returns the number of record that match the search condition.
 */
int record_lookup_proc_begin(char *user_name, int punch_pass, double start_time,
	double end_time, ee_queue_t *record_info_queue_head)
{
	int max_count = 0;
	int record_count = 0;

	assert((user_name != NULL) && (record_info_queue_head != NULL));

	/* Step1: Get the number of punch records */
	max_count = db_punch_record_count(user_name, punch_pass, start_time, end_time);

	/* Step2: Find punch records from the database */
	record_count = db_punch_record_query_begin(user_name, punch_pass, start_time,
		end_time, 0, max_count, record_info_queue_head);

	return record_count;
}

/**
 * record_lookup_proc_end - When the end of the use of the record information, need to
 * release the allocated resources.
 * @record_info_queue_head: IN, Point to the list header that stores record information.
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_lookup_proc_end(ee_queue_t *record_info_queue_head)
{
	int ret = -1;

	assert(record_info_queue_head != NULL);

	ret = db_punch_record_query_end(record_info_queue_head);

	return ret;
}

