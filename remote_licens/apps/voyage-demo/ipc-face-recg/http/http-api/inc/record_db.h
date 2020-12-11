/*
 * record_db.h - export some interface function of punch record
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#ifndef _RECORD_DB_H
#define _RECORD_DB_H

typedef struct query_punch_info {
	/* employee_number */
	char employee_number[32];

	/** employee number */
	char name[32];

	/* Role:
	 * Options:
	 *	0: Whitelist
	 *	1: Blacklist
	 *	2: Commonlist
	 */
	char role;

	/* Access right
	 * Options:
	 *	0: Free access
	 *	1: Temporary access mode 1
	 *	2: Temporary access mode 2
	 */
	char access_right;

	/** start punch time YYYY/MM/DD HH:MM:SS */
	double start_punch_time;

	/** end punch time YYYY/MM/DD HH:MM:SS */
	double end_punch_time;

	/** punch pass or unpass
	 *	0:punch pass
	 *	1:punch unpass
	 */
	char punch_pass;

	/** where to start the query */
	int start_id;

	/** the nember you want to display */
	int record_count;

	char gender[8];
	char nationa[32];
	char id_card_number[32];
	char department_name[128];

	/** punch address */
	char address[32];

	/** punch time */
	char punch_time[32];
	int id;
	/** match score */
	float match_score;
} query_punch_record_info;

typedef struct punch_info {
	int id;

	/* employee_number */
	char employee_number[32];

	/** employee number */
	char name[32];

	char gender[8];
	char nationa[32];
	char id_card_number[32];
	char department_name[128];

	/** punch address */
	char address[32];

	/** punch time */
	char punch_time[32];

	/** match score */
	float match_score;

	char punch_live_large_pic_name[128];
	char punch_live_small_pic_name[128];

	char punch_live_large_pic_name_path[128];
	char punch_live_small_pic_name_path[128];

	char punch_pass;

	/** total punch record */
	int record_total;
} punch_record_info;

#define DEL_NUM		5

/**
 * total_punch_record: total punch record
 * @person: person info of querying
 * @record: punch info of querying
 * @start_time: start punch time
 * @end_time: end punch time
 *
 * @return 0-successful, other-failed
 */
int total_punch_record(person_t *person, punch_record_t *record,
	double start_time, double end_time);

/**
 * pack_punch_data: pach punch data
 * @q_person: a point of struc person_info_t
 * @q_record: a point of struc punch_record_t
 * @punch_info: a point of struc punch_record_info
 * @record_total: record total
 *
 * @return 0-successful, other-failed
 */
int pack_punch_data(punch_record_t *punch_record,
	punch_record_info *punch_info, int record_total);

/**
 * query_punch_data: query punch record
 * @person: a point of struc person_t
 * @record: a point of struc punch_record_t
 * @p_info: a point of struc query_punch_record_info
 * @punch_info: a point of struc punch_record_info
 *
 * Returns none.
 */
int query_punch_data(person_t *person, punch_record_t *record,
	query_punch_record_info *p_info, punch_record_info *punch_info);

/**
 * search_punch_record - Query punch record
 * @p_info: a pointer of save punch record
 *
 * @return 0-successful, other-failed
 */
int search_punch_record(query_punch_record_info *p_info,
	punch_record_info *punch_info);

/**
 * delete_all_punch_record: delete all punch record
 *
 * Returns 0 means success, other means failure.
 */
int delete_all_punch_record();

/**
 * delete_punch_record: delete punch record by id or employee_numb ...
 * persion: person' information
 *
 * Returns 0 means success, other means failure.
 */
int delete_punch_record(query_punch_record_info *p_info);

int time_str_to_sec(const char *str_time, double *out_sec);
int time_sec_to_str(double sec, char *out_str_time, unsigned int max_out_str_len);

#endif