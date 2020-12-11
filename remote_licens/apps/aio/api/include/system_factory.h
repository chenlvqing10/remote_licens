/*
 * system_factory.h - Restore factory value function interface
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

#ifndef _SYSTEM_FACTORY_H
#define _SYSTEM_FACTORY_H

#include <pthread.h>
#include "thread_ops.h"

typedef enum tag_restore_result_code {
	RESTORE_OK = 0,
	RESTORE_ERR,
} restore_result_code_t;

/* A running thread */
typedef struct tag_factory_restore_ctx_t {
	pthread_t tid;			/* Thread ID */
	thread_done_e done;		/* Set to true when thread finishes */
	pthread_mutex_t mutex;		/* Mutex for resource synchronization */
	restore_result_code_t result_code; /* Result returned by the thread */
} factory_restore_ctx_t;

/* get status */
int sys_factory_get_ctx(factory_restore_ctx_t *p_ctx);

/* Init status */
int sys_factory_init_ctx(void);

/**
 * system_factory_thread - Restore factory in
 * a thread and system will reboot in 1 second
 *
 * Returns 0 if success, otherwise -1.
 */
int system_factory_thread(void);

/**
 * system_factory - Restore factory immedialy.
 *
 * Returns 0 if success, otherwise -1.
 */
int system_factory(void);

#endif /* _SYSTEM_FACTORY_H */
