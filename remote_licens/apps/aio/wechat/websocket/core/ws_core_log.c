/*
 * ws_svc_log.c
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
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <iniparser.h>

#define LOG_TAG "websocket"
#include <log/log.h>

#include "ws_core_log.h"

#define LOG_DATA_SIZE 4096

static int log_fd = -1;
static char *log_file = "/data/log/ws.log";

static int log_level = LOG_LEVEL_INFO; /* LOG_LEVEL_DISABLE; */
static int log_mode = LOG_MODE_FILE; /* LOG_MODE_FILE; */

#define WS_CONF_PATH "/etc/websocket.conf"

void log_init(void)
{
	dictionary *conf;
	conf = iniparser_load(WS_CONF_PATH);
	if (conf) {
		log_level = iniparser_getint(conf, "log:log_level", LOG_LEVEL_DISABLE);
		log_mode = iniparser_getint(conf, "log:log_mode", LOG_MODE_FILE);
		iniparser_freedict(conf);
	} else {
		log_level = LOG_LEVEL_INFO;
		log_mode = LOG_MODE_FILE;
	}

	ALOGI("websocket log parameters");
	ALOGI("  log_level: %d", log_level);
	ALOGI("  log_mode: %d", log_mode);
}

void log_info(const char *func, int line, const char *fmt, ...)
{
	if (LOG_LEVEL_INFO < log_level)
		return;

	int size = 0;
	char log_data[LOG_DATA_SIZE] = {0};

	if (!create_log_header(INFO, func, line, "info", log_data, &size))
		return;

	va_list ap;
	va_start(ap, fmt);
	size += vsnprintf(log_data + size, LOG_DATA_SIZE - size, fmt, ap);
	va_end(ap);

	write_log(INFO, log_data, size);
}

void log_trace(const char *func, int line, const char *fmt, ...)
{
	if (LOG_LEVEL_TRACE < log_level)
		return;

	int size = 0;
	char log_data[LOG_DATA_SIZE] = {0};

	if (!create_log_header(TRACE, func, line, "trace", log_data, &size))
		return;

	va_list ap;
	va_start(ap, fmt);
	size += vsnprintf(log_data + size, LOG_DATA_SIZE - size, fmt, ap);
	va_end(ap);

	write_log(TRACE, log_data, size);
}

void log_warn(const char *func, int line, const char *fmt, ...)
{
	if (LOG_LEVEL_WARN < log_level)
		return;

	int size = 0;
	char log_data[LOG_DATA_SIZE] = {0};

	if (!create_log_header(WARN, func, line, "warn", log_data, &size))
		return;

	va_list ap;
	va_start(ap, fmt);
	size += vsnprintf(log_data + size, LOG_DATA_SIZE - size, fmt, ap);
	va_end(ap);

	write_log(WARN, log_data, size);
}

void log_error(const char *func, int line, const char *fmt, ...)
{
	if (LOG_LEVEL_ERROR < log_level)
		return;

	int size = 0;
	char log_data[LOG_DATA_SIZE] = {0};

	if (!create_log_header(ERROR, func, line, "error", log_data, &size))
		return;

	va_list ap;
	va_start(ap, fmt);
	size += vsnprintf(log_data + size, LOG_DATA_SIZE - size, fmt, ap);
	va_end(ap);

	write_log(ERROR, log_data, size);
}

bool create_log_header(level error_level, const char *func, int line,
	char *level_name, char *log_data, int *header_size)
{
	struct timeval tv;
	struct tm *tm;
	int log_header_size = -1;

	if (gettimeofday(&tv, NULL) == -1)
		return false;

	if ((tm = localtime(&tv.tv_sec)) == NULL)
		return false;

	if (error_level == ERROR) {
		/* add strerror() to error msg. */
		if (func) {
			if (errno != 0) {
				log_header_size = snprintf(log_data, LOG_DATA_SIZE,
						"[%d-%02d-%02d %02d:%02d:%02d.%03d][%s:%d][%s][%d][%d](%s) ",
						1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
						(int)(tv.tv_usec / 1000), func, line,
						level_name, getpid(), errno, strerror(errno));
			} else {
				log_header_size = snprintf(log_data, LOG_DATA_SIZE,
						"[%d-%02d-%02d %02d:%02d:%02d.%03d][%s:%d][%s][%d][%d] ",
						1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
						(int)(tv.tv_usec / 1000), func, line,
						level_name, getpid(), errno);
			}
		} else {
			if (errno != 0) {
				log_header_size = snprintf(log_data, LOG_DATA_SIZE,
						"[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%d](%s) ",
						1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
						(int)(tv.tv_usec / 1000),
						level_name, getpid(), errno, strerror(errno));
			} else {
				log_header_size = snprintf(log_data, LOG_DATA_SIZE,
						"[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%d] ",
						1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
						(int)(tv.tv_usec / 1000),
						level_name, getpid(), errno);
			}
		}
	} else {
		if (func) {
			log_header_size = snprintf(log_data, LOG_DATA_SIZE,
					"[%d-%02d-%02d %02d:%02d:%02d.%03d][%s:%d][%s][%d] ",
					1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
					(int)(tv.tv_usec / 1000), func, line,
					level_name, getpid());
		} else {
			log_header_size = snprintf(log_data, LOG_DATA_SIZE,
					"[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d] ",
					1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
					(int)(tv.tv_usec / 1000),
					level_name, getpid());
		}
	}

	if (log_header_size == -1)
		return false;

	*header_size = log_header_size;
	return true;
}

void open_log_file(void)
{
	/* file mode 644 */
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

	log_fd = open(log_file, O_RDWR | O_APPEND);
	if (log_fd == -1 && errno == ENOENT)
		log_fd = open(log_file, O_RDWR | O_CREAT | O_TRUNC, mode);
}

/*
#define NONE				 "\e[0m"
#define RED					 "\e[0;31m"
#define GREEN				 "\e[0;32m"
#define YELLOW				 "\e[1;33m"
*/
void write_log(level error_level, void *buf, size_t size)
{
	/* not deamon printf stdout */
	if (log_mode == LOG_MODE_CONSOLE) {
		if (error_level == ERROR) {
			ALOGE("\e[0;31m %s \e[0m", (char *)buf);
		} else if (error_level == WARN) {
			ALOGW("\e[1;33m %s \e[0m", (char *)buf);
		} else {
			ALOGD(" %s ", (char *)buf);
		}
		return;
	}

	if (log_fd < 0) {
		open_log_file();
	}

	write(log_fd, buf, size);
}

void close_log_file(void)
{
	close(log_fd);
}
