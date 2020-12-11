/*
 * lb_omx_debug.h - debug api used by lombo omx.
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

#ifndef __LB_OSCL_DEBUG_LINUX_H__
#define __LB_OSCL_DEBUG_LINUX_H__

#include <stdio.h>
#include <assert.h>

#define DBG_VERBOSE	0
#define DBG_LOG		1
#define DBG_INFO	2
#define DBG_WARNING	3
#define DBG_ERROR	4
#define DBG_FATAL	5

#ifndef DBG_LEVEL
#define DBG_LEVEL	DBG_WARNING
#endif

#ifndef LOG_TAG
#define LOG_TAG		"OpenMax"
#endif

#define MDA_FMT		"[M][%s:%d] "

#define VIRGO_LOG	1

#ifndef VIRGO_LOG /* printf */

#if (DBG_LEVEL == DBG_VERBOSE)
#define OSCL_LOGV(fmt, ...) printf("[V/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGD(fmt, ...) printf("[D/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGI(fmt, ...) printf("[I/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGW(fmt, ...) printf("[W/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGE(fmt, ...) printf("[E/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#elif (DBG_LEVEL == DBG_LOG)
#define OSCL_LOGV(fmt, ...)
#define OSCL_LOGD(fmt, ...) printf("[D/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGI(fmt, ...) printf("[I/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGW(fmt, ...) printf("[W/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGE(fmt, ...) printf("[E/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#elif (DBG_LEVEL == DBG_INFO)
#define OSCL_LOGV(fmt, ...)
#define OSCL_LOGD(fmt, ...)
#define OSCL_LOGI(fmt, ...) printf("[I/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGW(fmt, ...) printf("[W/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGE(fmt, ...) printf("[E/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#elif (DBG_LEVEL == DBG_WARNING)
#define OSCL_LOGV(fmt, ...)
#define OSCL_LOGD(fmt, ...)
#define OSCL_LOGI(fmt, ...)
#define OSCL_LOGW(fmt, ...) printf("[W/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGE(fmt, ...) printf("[E/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#elif (DBG_LEVEL == DBG_ERROR)
#define OSCL_LOGV(fmt, ...)
#define OSCL_LOGD(fmt, ...)
#define OSCL_LOGI(fmt, ...)
#define OSCL_LOGW(fmt, ...)
#define OSCL_LOGE(fmt, ...) printf("[E/"LOG_TAG"]"MDA_FMT fmt "\n", \
				__func__, __LINE__, ##__VA_ARGS__)
#else
#error "unknown error level"
#endif

#define _LBLOG(level, fmt, ...)						\
	do {								\
		switch (level) {					\
		case DBG_VERBOSE:					\
			printf("[V/"LOG_TAG"]"MDA_FMT fmt "\n",	\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		case DBG_LOG:						\
			printf("[D/"LOG_TAG"]"MDA_FMT fmt "\n",	\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		case DBG_INFO:						\
			printf("[I/"LOG_TAG"]"MDA_FMT fmt "\n",	\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		case DBG_WARNING:					\
			printf("[W/"LOG_TAG"]"MDA_FMT fmt "\n",	\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		case DBG_ERROR:						\
			printf("[E/"LOG_TAG"]"MDA_FMT fmt "\n",	\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		}							\
	} while (0)

#else /* virgo log */

#include <log/log.h>
#if (DBG_LEVEL == DBG_VERBOSE)
#define OSCL_LOGV(fmt, ...) ALOGV((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGD(fmt, ...) ALOGD((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGI(fmt, ...) ALOGI((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGW(fmt, ...) ALOGW((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGE(fmt, ...) ALOGE((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#elif (DBG_LEVEL == DBG_LOG)
#define OSCL_LOGV(fmt, ...)
#define OSCL_LOGD(fmt, ...) ALOGD((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGI(fmt, ...) ALOGI((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGW(fmt, ...) ALOGW((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGE(fmt, ...) ALOGE((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#elif (DBG_LEVEL == DBG_INFO)
#define OSCL_LOGV(fmt, ...)
#define OSCL_LOGD(fmt, ...)
#define OSCL_LOGI(fmt, ...) ALOGI((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGW(fmt, ...) ALOGW((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGE(fmt, ...) ALOGE((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#elif (DBG_LEVEL == DBG_WARNING)
#define OSCL_LOGV(fmt, ...)
#define OSCL_LOGD(fmt, ...)
#define OSCL_LOGI(fmt, ...)
#define OSCL_LOGW(fmt, ...) ALOGW((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#define OSCL_LOGE(fmt, ...) ALOGE((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#elif (DBG_LEVEL == DBG_ERROR)
#define OSCL_LOGV(fmt, ...)
#define OSCL_LOGD(fmt, ...)
#define OSCL_LOGI(fmt, ...)
#define OSCL_LOGW(fmt, ...)
#define OSCL_LOGE(fmt, ...) ALOGE((MDA_FMT fmt), __func__, __LINE__, ##__VA_ARGS__)
#else
#error "unknown error level"
#endif

#define _LBLOG(level, fmt, ...)						\
	do {								\
		switch (level) {					\
		case DBG_VERBOSE:					\
			ALOGV((MDA_FMT fmt),				\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		case DBG_LOG:						\
			ALOGD((MDA_FMT fmt),				\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		case DBG_INFO:						\
			ALOGI((MDA_FMT fmt),				\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		case DBG_WARNING:					\
			ALOGW((MDA_FMT fmt),				\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		case DBG_ERROR:						\
			ALOGE((MDA_FMT fmt),				\
				__func__, __LINE__, ##__VA_ARGS__);	\
			break;						\
		}							\
	} while (0)

#endif /* VIRGO_LOG */

#define OSCL_ASSERT(...) assert(__VA_ARGS__)
#define OSCL_PRINT(...) OSCL_LOGE(__VA_ARGS__)
#define OSCL_TRACE(...) OSCL_LOGV(__VA_ARGS__)

#define oscl_param_check_exit(C, V, S) do { \
	if (!(C)) {\
		ret = V;\
		if (S) \
			OSCL_LOGW("check fail: error: 0x%lx - %s", \
				(long)V, (S) != NULL ? (S) : "NULL");\
		else \
			OSCL_LOGW("check fail: error: 0x%x", (int)V); \
		goto EXIT; } \
	} while (0)

#define oscl_param_check(C, V, S) do { \
	if (!(C)) {\
		if (S) \
			OSCL_LOGW("check fail: error: 0x%lx - %s", \
				(long)V, (S) != NULL ? (S) : "NULL");\
		else \
			OSCL_LOGW("check fail: error: 0x%lx", (long)V); \
		return V; } \
	} while (0)



#define dbg_log_port(port, lvl, fmt, ...) \
	do { \
		if (port) { \
			if (lvl < port->log_level) \
				break; \
			_LBLOG(lvl, "[port:%s-%d] " fmt, \
				get_port_name(port),\
				port->port_param.nPortIndex, \
				##__VA_ARGS__); \
		} else \
			_LBLOG(lvl, "[port:unknown] " fmt, ##__VA_ARGS__); \
	} while (0)

#define dbg_log_comp(cmp_hdl, lvl, fmt, ...) \
	do { \
		lb_omx_component_t *_log_com = NULL; \
		if (cmp_hdl) \
			_log_com = get_lb_component(cmp_hdl); \
		if (_log_com) { \
			if (lvl < _log_com->log_level) \
				break; \
			_LBLOG(lvl, "[comp:%s] " fmt, _log_com->name, ##__VA_ARGS__); \
		} else \
			_LBLOG(lvl, "[comp:unknown] " fmt, ##__VA_ARGS__); \
	} while (0)


#define DEBUG_THREAD_START 1
#define DEBUG_THREAD_STOP 2
#define DEBUG_THREAD_EXIT 3
#define DEBUG_THREAD_MASK 0x7
#define DEBUG_MSG_SHT 0
#define DEBUG_BUF_MGNT_SHT 3
#define get_debug_state(flag, thread_sht)  (((flag)>>(thread_sht))&DEBUG_THREAD_MASK)
#define set_debug_state(flag, thread_sht, val)  \
	(((flag)&~((DEBUG_THREAD_MASK)<<thread_sht))|((val)<<(thread_sht)))

#endif /* __LB_OSCL_DEBUG_LINUX_H__ */
