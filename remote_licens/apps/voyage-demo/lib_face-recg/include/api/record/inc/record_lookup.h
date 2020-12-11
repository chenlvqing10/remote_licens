/*
 * record_lookup.h - Processing interface for querying punch records.
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

#ifndef _RECORD_LOOKUP_H
#define _RECORD_LOOKUP_H

#include "record_info.h"
#include "common_record_info.h"

/** Number of punch records per page */
#define PUNCH_RECORD_COUNT_PER_PAGE	5

/**
 * record_lookup_get_full_info - Get complete information on punch records.
 * @rec: A pointer to pumch record.
 * @full_rec: Out, Punch record complete information.
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_lookup_get_full_info(punch_record_t *rec, full_punch_rec_info_t *full_rec);

/**
 * record_lookup_proc - Punch record lookup function.
 * @user_name: A pointer to username string.
 * @punch_pass: Punch pass flag
 * @start_time: Record the start time of punching(ms).
 * @end_time: Record the end time of punching(ms).
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_lookup_proc(char *user_name, int punch_pass, double start_time,
	double end_time);

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
	double end_time, ee_queue_t *record_info_queue_head);

/**
 * record_lookup_proc_end - When the end of the use of the record information, need to
 * release the allocated resources.
 * @record_info_queue_head: IN, Point to the list header that stores record information.
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_lookup_proc_end(ee_queue_t *record_info_queue_head);

#endif /* _RECORD_LOOKUP_H */
