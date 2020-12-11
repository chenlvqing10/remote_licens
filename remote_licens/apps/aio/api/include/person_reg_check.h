/*
 * person_reg_check.h - Define interface for test personnel registration.
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

#ifndef _PERSON_REG_CHECK_H
#define _PERSON_REG_CHECK_H

#include "person.h"
#include "thread_ops.h"

#define PERSON_REG_CHECK_FILE_PATH		"/mnt/usb/RECG_TEST/person.xls"

/** Written result to the Excel file */
#define EXCEL_REG_CHECK_RESULT_FILE_PREFIX	"person_recg_result"

/* Directory for storing pictures of registrants */
#define PERSON_REG_CHECK_PIC_FOLDER_NAME	"DCIM"

/* import status */
typedef struct tag_reg_check_status_t {
	int sum_count;
	int current_count;
	int fail_count;
} reg_check_status_t;

/* A running thread */
typedef struct tag_person_reg_check_ctx_t {
	pthread_t tid;			/* Thread ID */
	thread_done_e done;		/* Set to true when thread finishes */
	pthread_mutex_t mutex;		/* Mutex for resource synchronization */
	reg_check_status_t status;	/*  Status of person registration */
	void *p_out;			/* Result returned by the thread */
	int is_proc_exit; /* Whether the recognition process ends early */
	person_recg_param_t recg_param;
} person_reg_check_ctx_t;

typedef struct tag_reg_check_info {
	/* person information */
	person_t person_reg_info;

	/* Expected results of the registration test */
	char reg_expected_result[64];

} reg_check_info_t;

/**
 * Set the exit flag of recognitions
 * @is_exit: flag of exit
 *
 * return 0
 */
int excel_reg_check_set_proc_exit_flag(int is_exit);

/* Init status */
int person_reg_check_init_ctx(void);

/* get status */
int person_reg_check_get_ctx(person_reg_check_ctx_t *p_ctx);

/**
 * person_reg_check_thread - Use thread to process the person reg check function
 * in the backgroud.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_reg_check_thread();

#endif /* _PERSON_REG_CHECK_H */

