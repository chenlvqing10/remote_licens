/*
 * time_conv.c - Time conversion
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

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <stdint.h>
#include <sys/time.h>

#undef LOG_TAG
#define LOG_TAG "TIME_CONV"
#include "log/log.h"

#include "time_conv.h"

long long time_get_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

/**
 * time_str_to_second - Convert time string to seconds.
 * @str_time A pointer to char time
 * @str_format the format of char time
 * @sec Out, time seconds
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_str_to_second(const char *str_time, const char *str_format,
	long *out_sec)
{
	int ret = 0;
	time_t t = 0;
	struct tm tmp_0, tmp_1;
	assert((str_time != NULL) && (out_sec != NULL));

	/* Note: must be initialized, otherwise tm_isdst will be an arbitrary value */
	memset(&tmp_0, 0, sizeof(struct tm));
	memset(&tmp_1, 0, sizeof(struct tm));
	/*don not use daylight time*/
	tmp_0.tm_isdst = -1;
	strptime(str_time, str_format, &tmp_0);
	memcpy(&tmp_1, &tmp_0, sizeof(struct tm));
	t = mktime(&tmp_1);
	*out_sec = (long)t;
	if (t > 0 && tmp_1.tm_year == tmp_0.tm_year &&
		tmp_1.tm_mon == tmp_0.tm_mon &&
		tmp_1.tm_mday == tmp_0.tm_mday &&
		tmp_1.tm_hour == tmp_0.tm_hour &&
		tmp_1.tm_min == tmp_0.tm_min &&
		tmp_1.tm_sec == tmp_0.tm_sec) {
		ret = 0;
	} else {
		ALOGE("mktime error\n");
		ret = -1;
	}
	return ret;
}

/**
 * time_second_to_str - Convert seconds to time strings
 * @sec time seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_second_to_str(double sec, char *out_str_time,
	unsigned int max_out_str_len)
{
	time_t t;
	struct tm *tmp;
	size_t s = 0;

	assert(out_str_time != NULL);

	t = sec;
	tmp = localtime(&t);
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}

	s = strftime(out_str_time, max_out_str_len, "%Y/%m/%d %H:%M:%S", tmp);
	if (0 == s) {
		ALOGE("strftime returned 0\n");

		return -1;
	}

	return 0;
}

/**
 * time_min_to_str - Convert seconds to time strings
 * @sec time seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_min_to_str(double sec, char *out_str_time,
		unsigned int max_out_str_len)
{
	time_t t;
	struct tm *tmp;
	size_t s = 0;

	assert(out_str_time != NULL);

	t = sec;
	tmp = localtime(&t);
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}

	s = strftime(out_str_time, max_out_str_len, "%Y/%m/%d %H:%M", tmp);
	if (0 == s) {
		ALOGE("strftime returned 0\n");

		return -1;
	}

	return 0;
}

/**
 * time_second_to_file_name_str - Convert seconds to a filename string
 * @sec time seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_second_to_file_name_str(double sec, char *out_str_time,
	unsigned int max_out_str_len)
{
	time_t t;
	struct tm *tmp;
	size_t s = 0;

	assert(out_str_time != NULL);

	t = sec;
	tmp = localtime(&t);
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}

	s = strftime(out_str_time, max_out_str_len, "%Y%m%d-%H%M%S", tmp);
	if (0 == s) {
		ALOGE("strftime returned 0\n");

		return -1;
	}

	return 0;
}

/**
 * time_second_to_date_str - Convert seconds to date strings
 * @sec time seconds
 * @out_date_str Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_second_to_date_str(double sec, char *out_date_str,
	unsigned int max_out_str_len)
{
	time_t t;
	struct tm *tmp;
	size_t s = 0;

	assert(out_date_str != NULL);

	t = sec;
	tmp = localtime(&t);
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}

	s = strftime(out_date_str, max_out_str_len, "%Y/%m/%d", tmp);
	if (0 == s) {
		ALOGE("strftime returned 0\n");

		return -1;
	}

	return 0;
}

/**
 * time_today_start_str - Get today's start time
 * @out_start_time_str Out, Store today's start time string
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_get_today_start_time_str(char *out_start_time_str,
	unsigned int max_out_str_len)
{
	time_t t;
	struct tm *tmp;
	size_t s = 0;

	assert(out_start_time_str != NULL);

	t = time(NULL);
	tmp = localtime(&t);
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}

	s = strftime(out_start_time_str, max_out_str_len, "%Y/%m/%d 00:00:00", tmp);
	if (0 == s) {
		ALOGE("strftime returned 0\n");

		return -1;
	}

	return 0;
}

/**
 * time_get_today_end_time_str - Get today's end time
 * @out_end_time_str Out, Store today's end time string
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_get_today_end_time_str(char *out_end_time_str, unsigned int max_out_str_len)
{
	time_t t;
	struct tm *tmp;
	size_t s = 0;

	assert(out_end_time_str != NULL);

	t = time(NULL);
	tmp = localtime(&t);
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}

	s = strftime(out_end_time_str, max_out_str_len, "%Y/%m/%d 23:59:59", tmp);
	if (0 == s) {
		ALOGE("strftime returned 0\n");

		return -1;
	}

	return 0;
}

/**
 * time_get_current_sec - Get current time
 * @s OUT, The time as the number of seconds since the Epoch,
 *	1970-01-01 00:00:00 +0000 (UTC)
 *
 * Returns 0 if success, otherwise not 0.
 */
int time_get_current_sec(time_t *s)
{
	time(s);

	return 0;
}

/**
 * time_set_current_time - Set system time to device
 * @str_time A string with a time format of: YYYY/MM/DD HH:MM:SS
 *
 * Returns 0 if success, otherwise not 0.
 */
int time_set_current_time(char *str_time)
{
	int ret = 0;
	long seconds = 0;
	struct timeval tv;

	ret = time_str_to_second(str_time, "%Y/%m/%d %H:%M:%S", &seconds);
	if (0 == ret) {
		tv.tv_sec = seconds;
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);

		/* Save time to hardware clock
		 *
		 * After the kernel or file system has just started, it reads the time
		 * from the hardware clock, and then according to the current time zone,
		 * plus the time zone, so when setting the time, it is necessary to set
		 * the hardware clock to UTC time, and the corresponding setting is:
		 * hwclock-w-u
		 */
		system("hwclock -w -u");
	}

	return ret;
}

/**
 * get current date information
 * @param p_date the pointer of saving date information
 * @return 0-successful; -1-failed
 */
int time_get_cur_date(char *p_date)
{
	time_t seconds_time = 0;
	struct tm *ptm = NULL;
	if (NULL == p_date)
		return -1;
	time(&seconds_time);
	ptm = localtime(&seconds_time);
	if (NULL == ptm)
		return -1;
	sprintf(p_date, "%d%02d%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);

	return 0;
}

/**
 * get date time information
 * @param p_date_time the pointer of saving date and time information
 * @return 0-successful; -1-failed
 */
int time_get_cur_date_time(char *p_date_time)
{
	time_t seconds_time = 0;
	struct tm *ptm = NULL;
	if (NULL == p_date_time)
		return -1;
	time(&seconds_time);
	ptm = localtime(&seconds_time);
	if (NULL == ptm)
		return -1;
	sprintf(p_date_time, "%d/%02d/%02d %02d:%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1,
		ptm->tm_mday, ptm->tm_hour,
		ptm->tm_min/*, ptm->tm_sec:%02d*/);

	return 0;
}

/**
 * get current time information
 * @param p_date the pointer of saving date information
 * @return 0-successful; -1-failed
 */
int time_get_cur_time(char *p_time)
{
	time_t seconds_time = 0;
	struct tm *ptm = NULL;
	if (NULL == p_time)
		return -1;
	time(&seconds_time);
	ptm = localtime(&seconds_time);
	if (NULL == ptm)
		return -1;
	sprintf(p_time, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	return 0;
}

/**
 * get date week information
 * @param p_date_time the pointer of saving date and time information
 * @return 0-successful; -1-failed
 */
int time_get_cur_date_week(char *p_date, int *week)
{
	time_t seconds_time = 0;
	struct tm *ptm = NULL;
	if (NULL == p_date)
		return -1;
	time(&seconds_time);
	ptm = localtime(&seconds_time);
	if (NULL == ptm)
		return -1;
	sprintf(p_date, "%d-%02d-%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
	*week = ptm->tm_wday;

	return 0;
}

/**
 * get_before_n_day_timestamp - get before n day timestamp second unit.
 * @t: A pointer to timestamp.
 * @nday: day count.
 *
 * Returns 0 if success, otherwise not 0.
 */
double get_before_n_day_timestamp(double *t, int nday)
{
	double t_before;
	double t_nday;

	if (nday < 0)
		return -1;

	t_nday = nday * 86400;
	t_before = (*t) - t_nday;

	return t_before;
}

/**
 * time_get_cur_time_ret_str - Get the current time string.
 * @out_str_time Out, Returns the current time string.
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int time_get_cur_time_ret_str(char *out_str_time, unsigned int max_out_str_len)
{
	time_t t;
	size_t s;
	struct tm *tmp;

	assert(out_str_time != NULL);

	t = time(NULL);
	tmp = localtime(&t);
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}

	s = strftime(out_str_time, max_out_str_len, "%Y%m%d%H%M%S", tmp);
	if (0 == s) {
		ALOGE("[%s:%d] strftime returned 0\n", __func__, __LINE__);

		return -1;
	}

	return 0;
}

