/*
 * record_export.h - Export punch record to excel file.
 * information from Excel.
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

#ifndef _RECORD_EXPORT_H
#define _RECORD_EXPORT_H

#include "xlsxwriter.h"
#include "record_info.h"
#include "thread_ops.h"

typedef enum tag_record_export_error_type_e {
	RECORD_EXPORT_OK = 0,		/* Record export success */
	RECORD_EXPORT_WRITE_ERR,	/* Record data write failed */
	RECORD_EXPORT_NO_UDISK_ERR,	/* Haven't udisk */
	RECORD_EXPORT_UDISK_FULL_ERR,	/* U disk will be full */
	RECORD_EXPORT_USER_EXIT_ERR,	/* During the export process, the user clicks the
					 * return button to exit.
					 */
} record_export_error_type_e;

typedef struct tag_export_query_condition {
	char user_name[64];	/* The name of the person who needs to export */
	int rec_pass;		/* Whether the face recoganition is passed */
	double rec_start_time;	/* The start time of the export record(ms) */
	double rec_end_time;	/* The end time of the export record(ms) */
} export_query_condition_t;

/* Status of record export */
typedef struct tag_record_export_status_t {
	int sum_count;
	int current_count;
	int fail_count;
} record_export_status_t;

/* A running thread */
typedef struct tag_record_export_ctx_t {
	pthread_t tid;			/* Thread ID */
	thread_done_e done;		/* Set to true when thread finishes */
	pthread_mutex_t mutex;		/* Mutex for resource synchronization */
	record_export_status_t status;  /*  Status of record export */
	int is_proc_exit;		/* Whether the record export ends early */
	record_export_error_type_e result_code;	/* Result returned by the thread */
} record_export_ctx_t;

/* Set the exit flag of imports */
int record_export_set_proc_exit_flag(int is_exit);

/* Init record export contex */
int record_export_init_ctx(void);

/* Get record export contex */
int record_export_get_ctx(record_export_ctx_t *p_ctx);

/**
 * record_export_thread - Use thread to process the record export function
 * in the backgroud.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_export_thread(export_query_condition_t *query_condition);

#endif /* _RECORD_EXPORT_H */

