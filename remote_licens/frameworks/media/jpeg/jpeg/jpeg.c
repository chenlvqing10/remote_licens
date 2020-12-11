/*
 * jpeg.c
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "JPEG"
#include "log/log.h"

pthread_mutex_t global_jpeg_lock;
int jpeg_initialed = 0;

int jpeg_init(void)
{
	ALOGD("jpeg_init");
	if (pthread_mutex_init(&global_jpeg_lock, NULL)) {
		ALOGE("init global_jpeg_lock failed\n");
		return -1;
	}

	jpeg_initialed = 1;

	return 0;
}

void jpeg_lock(void)
{
	ALOGD("jpeg_lock");
	if (!jpeg_initialed)
		jpeg_init();
	pthread_mutex_lock(&global_jpeg_lock);
}

void jpeg_unlock(void)
{
	ALOGD("jpeg_unlock");
	pthread_mutex_unlock(&global_jpeg_lock);
}

