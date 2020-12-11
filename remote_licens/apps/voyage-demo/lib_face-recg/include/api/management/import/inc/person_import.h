/*
 * person_import.h - Define interface for reading personnel registration
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

#ifndef _PERSON_IMPORT_H
#define _PERSON_IMPORT_H

#include "thread_ops.h"
#include "person.h"

#define PERSON_IMPORT_EXCEL_FILE_PATH	"/mnt/usb/REG/person.xls"

/** When the registration fails, the failure information is written to the Excel file */
#define EXCEL_REG_FAIL_FILE_NAME	"person_reg_error"

/* Directory for storing pictures of registrants */
#define PERSON_IMPORT_PIC_FOLDER_NAME	"DCIM"

typedef enum tag_person_import_error_type_e {
	PERSON_IMPORT_OK = 0,		/* person import success */
	PERSON_IMPORT_NO_UDISK_ERR,	/* Haven't udisk */
	PERSON_IMPORT_READ_ERR,		/* person import data read failed */
	PERSON_IMPORT_WRITE_ERR,	/* person import data write failed */
	PERSON_IMPORT_USER_EXIT_ERR,	/* During the import process, the user clicks the
					 * return button to exit.
					 */
} person_import_error_type_e;

/* Status of person registration */
typedef struct tag_person_import_status_t {
	int sum_count;
	int current_count;
	int fail_count;
} person_import_status_t;

/* A running thread */
typedef struct tag_person_import_ctx_t {
	pthread_t tid;			/* Thread ID */
	thread_done_e done;		/* Set to true when thread finishes */
	pthread_mutex_t mutex;		/* Mutex for resource synchronization */
	person_import_status_t status;  /*  Status of person registration */
	int is_proc_exit;		/* Whether the import process ends early */
	person_import_error_type_e result_code;	/* Result returned by the thread */
	person_reg_param_t reg_param;
	e_person_reg_code person_reg_result_code;
} person_import_ctx_t;

/* Init status */
int person_import_init_ctx(void);

/* get status */
int person_import_get_ctx(person_import_ctx_t *p_ctx);

/* Set the exit flag of imports */
int excel_person_info_set_proc_exit_flag(int is_exit);

/**
 * person_import_thread - Use thread to process the person import function
 * in the backgroud.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_import_thread();

#endif /* _PERSON_IMPORT_H */

