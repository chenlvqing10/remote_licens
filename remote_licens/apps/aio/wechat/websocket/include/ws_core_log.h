/*
 * ws_svc_log.h
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

#ifndef __WS_SVC_LOG__
#define __WS_SVC_LOG__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
#define NONE				 "\e[0m"
#define RED					 "\e[0;31m"
#define GREEN				 "\e[0;32m"
#define YELLOW				 "\e[1;33m"
*/

typedef enum level {
	INFO = 0,
	TRACE,
	WARN,
	ERROR
} level;

#define LOG_LEVEL_INFO			0x01
#define LOG_LEVEL_TRACE			0x02
#define LOG_LEVEL_WARN			0x03
#define LOG_LEVEL_ERROR			0x04
#define LOG_LEVEL_DISABLE		0x05

#define LOG_MODE_FILE			0x01
#define LOG_MODE_CONSOLE		0x02

#if 0
#define ws_log_init()			log_init()
#define ws_info(msg, ...)		(void)0
#define ws_trace(msg, ...)		(void)0
#define ws_warn(msg, ...)		(void)0
#define ws_error(msg, ...)		(void)0
#endif
#if 0
#define ws_log_init()			log_init()
#define ws_info(msg, ...)		log_info(NULL, 0, msg, ##__VA_ARGS__)
#define ws_trace(msg, ...)		log_trace(NULL, 0, msg, ##__VA_ARGS__)
#define ws_warn(msg, ...)		log_warn(NULL, 0, msg, ##__VA_ARGS__)
#define ws_error(msg, ...)		log_trace(NULL, 0, msg, ##__VA_ARGS__)
#endif
#if 1
#define ws_log_init()			log_init()
#define ws_info(msg, ...)		log_info(__FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#define ws_trace(msg, ...)		log_trace(__FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#define ws_warn(msg, ...)		log_warn(__FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#define ws_error(msg, ...)		log_error(__FUNCTION__, __LINE__, msg,  ##__VA_ARGS__)
#endif


void log_info(const char *func, int line, const char *fmt, ...);
void log_trace(const char *func, int line, const char *fmt, ...);
void log_warn(const char *func, int line, const char *fmt, ...);
void log_error(const char *func, int line, const char *fmt, ...);

void log_init(void);
void open_log_file(void);
void close_log_file(void);
void write_log(level error_level, void *buf, size_t size);
bool create_log_header(level error_level, const char *func, int line,
	char *level_name, char *log_data, int *header_size);

#endif /* __WS_SVC_LOG__ */