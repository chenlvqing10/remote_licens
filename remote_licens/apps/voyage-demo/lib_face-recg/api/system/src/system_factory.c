/*
 * system_factory.c - Restore factory value function interface
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>

#include "file_ops.h"
#include "system_factory.h"

#define LOG_TAG "SYSTEM_FACTORY"
#include <log/log.h>

static factory_restore_ctx_t restore_ctx;

/* Set thread completion status falg */
static int sys_factory_set_done_status(thread_done_e is_done)
{
	restore_ctx.done = is_done;

	return 0;
}

/* Set thread result code */
static int sys_factory_set_result_code(restore_result_code_t result_code)
{
	restore_ctx.result_code = result_code;

	return 0;
}

/* get status */
int sys_factory_get_ctx(factory_restore_ctx_t *p_ctx)
{
	assert(p_ctx != NULL);

	memcpy(p_ctx, &restore_ctx, sizeof(factory_restore_ctx_t));

	return 0;
}

/* Init status */
int sys_factory_init_ctx(void)
{
	memset(&restore_ctx, 0, sizeof(factory_restore_ctx_t));
	restore_ctx.done = EE_THREAD_NOT_RUN;

	return 0;
}

/**
 * sys_factory_start_routine - Restore factory value thread processing function
 * @arg	Parameters passed in by pthread_create
 *
 * Returns a pointer.
 */
static void *sys_factory_start_routine(void *arg)
{
	int ret = 0;

	sys_factory_set_done_status(EE_THREAD_RUNNING);

	/* Step0: Synchronize file system data into physical media */
	file_system_sync();

	/*Step1: kill logcat */
	system("pkill logcat");

	/* Step2: umount emmc -f */
	ret = system("/bin/umount /data");
	if (ret != 0) {
		ALOGE("umount error: ret=%d", ret);
		sys_factory_set_result_code(RESTORE_ERR);
		goto RESTORE_FACTORY_ERR;
	}

	/* Step3: Format emmc : -F */
	ret = system("/sbin/mke2fs -T ext4 -q /dev/mmcblk0p1");
	if (ret != 0) {
		ALOGE("mke2fs error: ret=%d\n", ret);
		sys_factory_set_result_code(RESTORE_ERR);
		goto RESTORE_FACTORY_ERR;
	}

	/* Step4: Reboot system */
	ret = system("/sbin/reboot");
	if (ret != 0) {
		ALOGE("reboot error: ret=%d\n", ret);
		sys_factory_set_result_code(RESTORE_ERR);
		goto RESTORE_FACTORY_ERR;
	}

	sys_factory_set_result_code(RESTORE_OK);

RESTORE_FACTORY_ERR:
	sys_factory_set_done_status(EE_THREAD_DONE);

	return NULL;
}

/**
 * system_factory_thread - Restore factory default entry function
 * None
 *
 * Returns 0 if success, otherwise -1.
 */
int system_factory_thread()
{
	sys_factory_init_ctx();

	pthread_create(&(restore_ctx.tid), NULL, sys_factory_start_routine, NULL);
	pthread_detach(restore_ctx.tid);

	return 0;
}

