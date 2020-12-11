/*
 * debug.h - debug operations
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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

/*
 * Normally we strip Debug Log from release builds.
 * You can modify this (for example with "#define LOG_NDEBUG 0"
 * at the top of your source file) to change that behavior.
 */
#define LOG_NDEBUG 0

/*
 * This is the local tag used for the following simplified
 * logging macros.  You can change this preprocessor definition
 * before using the other macros to change the tag.
 */
#ifndef LOG_TAG
#define LOG_TAG               "wifi_softap"
#endif

#include <log/log.h>

#define logv(fmt, arg...) \
	ALOGV("<line[%04d] %s()> " fmt, __LINE__, __func__, ##arg);
#ifndef logd
#if LOG_NDEBUG
#define logd(...)   ((void)0)
#else
#define logd(fmt, arg...) \
	ALOG(LOG_DEBUG, LOG_TAG, "<line[%04d] %s()> " \
		fmt, __LINE__, __func__, ##arg);
#endif
#endif

#ifndef logi
#if LOG_NDEBUG
#define logi(...) \
	ALOG(LOG_INFO, LOG_TAG, __VA_ARGS__);
#else
#define logi(fmt, arg...) \
	ALOG(LOG_INFO, LOG_TAG, "<line[%04d] %s()> " \
		fmt, __LINE__, __func__, ##arg);
#endif
#endif

#ifndef logw
#if LOG_NDEBUG
#define logw(...) \
	ALOG(LOG_WARN, LOG_TAG, __VA_ARGS__);
#else
#define logw(fmt, arg...) \
	ALOG(LOG_WARN, LOG_TAG, "<line[%04d] %s()> " \
		fmt, __LINE__, __func__, ##arg);
#endif
#endif

#ifndef loge
#if LOG_NDEBUG
#define loge(...) \
	ALOG(LOG_ERROR, LOG_TAG, __VA_ARGS__);
#else
#define loge(fmt, arg...) \
	ALOG(LOG_ERROR, LOG_TAG, "<line[%04d] %s()> " \
		fmt, __LINE__, __func__, ##arg);
#endif
#endif

#endif
