/*
 * reboot_svc.c - Interface to reboot
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

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <sys/reboot.h>

#define LOG_TAG "reboot_svc"
#include <log/log.h>

#include "reboot_svc.h"

/**
 * information structure of system reboot
 */
typedef struct tag_sys_reboot_service {
	int is_stop;
	pthread_t reboot_thrd;
	data_time_t reboot_time;
} sys_reboot_service_t;

static sys_reboot_service_t s_reboot_service;

static int get_week(int y, int m, int d)
{
	int week = 0;
	if (m == 1 || m == 2) {
		m += 12;
		y--;
	}
	week = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;

	return week;
}

int time_get_cur_sec()
{
	time_t seconds_time = 0;
	struct tm *ptm = NULL;

	time(&seconds_time);
	ptm = localtime(&seconds_time);
	if (NULL == ptm) {
		ALOGE("[%s][%d] get cur time error", __func__, __LINE__);
		return -1;
	}

	return ptm->tm_sec;
}

int is_time_point(data_time_t *reboot_time)
{
	int week = 0;
	time_t seconds_time = 0;
	struct tm *ptm = NULL;
	time(&seconds_time);
	ptm = localtime(&seconds_time);
	if (NULL == ptm)
		return -1;

	if (reboot_time->day > 0) {
		if (reboot_time->day == ptm->tm_mday &&
				reboot_time->hour == ptm->tm_hour &&
				reboot_time->min == ptm->tm_min)
			return 1;
	} else if (reboot_time->week >= 0) {
		week = get_week(ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
		if (reboot_time->week == week && reboot_time->hour == ptm->tm_hour &&
				reboot_time->min == ptm->tm_min)
			return 1;
	} else if (reboot_time->hour == ptm->tm_hour &&
			reboot_time->min == ptm->tm_min)
		return 1;

	return 0;
}

void *reboot_thread(void *param)
{
	int ret;
	sys_reboot_service_t *service =
		(sys_reboot_service_t *)param;

	while (!service->is_stop) {
		usleep(1000 * 1000);
		if (time_get_cur_sec() != 0) {
			continue;
		}

		if (is_time_point(&s_reboot_service.reboot_time)) {
			ALOGD("%s[%d] system reboot begin", __func__, __LINE__);
			ret = system("/sbin/reboot");
			if (ret < 0) {
				ALOGE("%s[%d] system(/sbin/reboot) error: %s",
					__func__, __LINE__, strerror(errno));
				ret = reboot(RB_AUTOBOOT);
				if (ret)
					ALOGE("%s[%d] reboot() error %s",
						__func__, __LINE__, strerror(errno));
			}
		}

		/* usleep(60 * 1000 * 1000); */
	}

	return NULL;
}

/**
 * reboot_start_service - start service of reboot
 * @delay_time: delay time of open dooor
 *
 * Returns 0 if success, otherwise not 0.
 */
int reboot_start_service(data_time_t *reboot_time)
{
	memcpy(&s_reboot_service.reboot_time, reboot_time, sizeof(data_time_t));
	s_reboot_service.is_stop = 0;

	if (pthread_create(&s_reboot_service.reboot_thrd, NULL,
				reboot_thread, &s_reboot_service)) {
		ALOGE("reboot_thread thread failed, errno: %d (%s)\n",
					errno, strerror(errno));
		return -1;
	}

	return 0;
}

/**
 * reboot_stop_service - stop service of reboot
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int reboot_stop_service(void)
{
	s_reboot_service.is_stop = 1;
	pthread_join(s_reboot_service.reboot_thrd, NULL);

	return 0;
}

/**
 * set_reboot__time - set reboot time
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_reboot_time(data_time_t *reboot_time)
{
	memcpy(&s_reboot_service.reboot_time, reboot_time, sizeof(data_time_t));

	return 0;
}

