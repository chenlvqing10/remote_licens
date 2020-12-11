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
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>

#include "file_ops.h"
#include "system_factory.h"

#define LOG_TAG "SYSTEM_FACTORY"
#include <log/log.h>

#define MISC_PARTITION "/dev/mmcblk0p4"
#define MISC_FACTORY_OFFSET 512

#define RESET_FACTORY_MAGIC_HEAD 0x2054ff55
#define CMD_RESET_FACTORY_DEFAULT 0x1

typedef struct tag_factory_info {
	unsigned int magic;
	unsigned int cmd;
	unsigned int rsv;
	unsigned int status;
} factory_info_t;

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
 * sys_factory_face_partition - Format face partitions
 * @None
 *
 * Returns 0 if success, otherwise -1.
 */
static int sys_factory_face_partition()
{
	int ret = 0;

	/* Step1: umount emmc -f */
	ret = system("/bin/umount /face");
	if (ret != 0) {
		ALOGE("umount error: ret=%d", ret);
		goto RESTORE_FACTORY_ERR;
	}

	/* Step2: Format emmc : -F */
	ret = system("/sbin/mke2fs -T ext4 -q /dev/mmcblk0p6");
	if (ret != 0) {
		ALOGE("mke2fs error: ret=%d\n", ret);
		goto RESTORE_FACTORY_ERR;
	}

	return 0;

RESTORE_FACTORY_ERR:
	return -1;
}

/**
 * sys_factory_data_partition - Format data partitions
 * @None
 *
 * Returns 0 if success, otherwise -1.
 */
static int sys_factory_data_partition()
{
	int ret = 0;

	/*Step1: kill logcat */
	system("pkill logcat");

	/* Step2: umount emmc -f */
	ret = system("/bin/umount /data");
	if (ret != 0) {
		ALOGE("umount error: ret=%d", ret);
		goto RESTORE_FACTORY_ERR;
	}

	/* Step3: Format emmc : -F */
	ret = system("/sbin/mke2fs -T ext4 -q /dev/mmcblk0p1");
	if (ret != 0) {
		ALOGE("mke2fs error: ret=%d\n", ret);
		goto RESTORE_FACTORY_ERR;
	}

	return 0;

RESTORE_FACTORY_ERR:
	return -1;
}

/**
 * sys_factory_start_routine - Restore factory value thread processing function
 * @arg	Parameters passed in by pthread_create
 *
 * Returns a pointer.
 */
void *sys_factory_start_routine(void *arg)
{
	int ret = 0;

	sys_factory_set_done_status(EE_THREAD_RUNNING);

	/* Step0: Synchronize file system data into physical media */
	file_system_sync();

	/* Step1 format data partition */
	ret = sys_factory_data_partition();
	if (ret != 0) {
		ALOGE("umount error: ret=%d", ret);
		sys_factory_set_result_code(RESTORE_ERR);
		goto RESTORE_FACTORY_ERR;
	}

	/* Step2 format face partition */
	ret = sys_factory_face_partition();
	if (ret != 0) {
		ALOGE("umount error: ret=%d", ret);
		sys_factory_set_result_code(RESTORE_ERR);
		goto RESTORE_FACTORY_ERR;
	}

	/* Step3: Reboot system */
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

static int sys_factory_set_factory_flag(void)
{
	int fd, count;
	factory_info_t factory_info;

	memset(&factory_info, 0, sizeof(factory_info_t));
	factory_info.magic = RESET_FACTORY_MAGIC_HEAD;
	factory_info.cmd = CMD_RESET_FACTORY_DEFAULT;

	ALOGD("open %s!\n", MISC_PARTITION);
	fd = open(MISC_PARTITION, O_RDWR);
	if (fd < 0) {
		ALOGE("error:open %s failed!\n", MISC_PARTITION);
		return -1;
	}

	lseek(fd, MISC_FACTORY_OFFSET, SEEK_SET);
	count = write(fd, &factory_info, sizeof(factory_info_t));
	if (count != sizeof(factory_info_t)) {
		ALOGE("write failed, count = %d", count);
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

/**
 * sys_factory_set_flag_routine - Restore factory value thread processing function
 * @arg	Parameters passed in by pthread_create
 *
 * Returns a pointer.
 */
static void *sys_factory_set_flag_routine(void *arg)
{
	int ret = 0;

	ALOGD("[%s:%d] thread id =%lx\n", __func__, __LINE__, pthread_self());
	prctl(PR_SET_NAME, "factory-reset");

	sys_factory_set_done_status(EE_THREAD_RUNNING);

	/* Step0: set factory flag in misc partition */
	ret = sys_factory_set_factory_flag();
	if (ret != 0) {
		ALOGE("set factory flag failed");
		sys_factory_set_result_code(RESTORE_ERR);
		goto RESTORE_FACTORY_ERR;
	}

	usleep(1000000);

	/* Step1: Reboot system */
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
int system_factory_thread(void)
{
	sys_factory_init_ctx();

	pthread_create(&(restore_ctx.tid), NULL,
		sys_factory_set_flag_routine, NULL);
	pthread_detach(restore_ctx.tid);

	return 0;
}

int system_factory(void)
{
	int ret = 0;

	ALOGD("factory reset");

	/* Step0: set factory flag in misc partition */
	ret = sys_factory_set_factory_flag();
	if (ret != 0) {
		ALOGE("set factory flag failed");
		return -1;
	}

	/* Step1: Reboot system */
	ret = system("/sbin/reboot");
	if (ret != 0) {
		ALOGE("reboot error: ret=%d\n", ret);
		return -1;
	}

	return 0;
}

