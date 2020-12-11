/*
 * checkin_export_svc.h - Export checkin record to excel file.
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

#ifndef _CHECKIN_EXPORT_SVC_H
#define _CHECKIN_EXPORT_SVC_H

#include <pthread.h>

#include "thread_ops.h"
#include "ee_queue.h"

#define MAX_USER_ID_NUM 64

/* get checkin count */
typedef int (*func_get_checkin_count)();

/* get checkin record information */
typedef int (*func_get_checkin_rec_info)(
			int index, int count, ee_queue_t *head);

/* export checkin record information */
typedef struct tag_checkin_rec_info {
	char user_id[MAX_USER_ID_NUM];
	char name[MAX_USER_ID_NUM];
	unsigned int checkin_time;
	ee_queue_t queue;
} checkin_rec_info_t;

typedef enum tag_record_export_error_type_e {
	RECORD_EXPORT_OK = 0,		/* Record export success */
	RECORD_EXPORT_WRITE_ERR,	/* Record data write failed */
	RECORD_EXPORT_NO_UDISK_ERR,	/* Haven't udisk */
	RECORD_EXPORT_UDISK_FULL_ERR,	/* U disk will be full */
	RECORD_EXPORT_USER_EXIT_ERR,	/* During the export process,
					 * the user clicks the
					 * return button to exit.
					 */
} record_export_error_type_e;

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
 * record_export_start_service - start record export service
 * @func_count: a function pointer of func_get_checkin_count
 * @func_rec_info: a function pointer of func_get_checkin_rec_info
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_export_start_service(
		func_get_checkin_count func_count,
		func_get_checkin_rec_info func_rec_info);

/**
 * record_export_stop_service - stop record export service
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int record_export_stop_service(void);

#endif /* _CHECKIN_EXPORT_SVC_H */

