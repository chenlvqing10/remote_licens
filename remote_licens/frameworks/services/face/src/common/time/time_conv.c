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

LOCAL long long time_get_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)(tv.tv_sec) * 1000000 + tv.tv_usec;
}

LOCAL long long time_get_ms(void)
{
	struct timeval tv;
	long long t = 0;

	gettimeofday(&tv, NULL);

	t = (long long)(tv.tv_sec) * 1000 + (tv.tv_usec / 1000);

	return t;
}

/**
 * time_str_to_second - Convert time string to seconds.
 * @str_time A pointer to char time
 * @sec Out, time seconds
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_str_to_second(const char *str_time, long *out_sec)
{
	int ret = 0;
	time_t t = 0;
	struct tm *tmp_time = NULL;

	assert((str_time != NULL) && (out_sec != NULL));

	tmp_time = (struct tm *)malloc(sizeof(struct tm));
	if (NULL == tmp_time) {
		ALOGE("[%s:%d] malloc fail\n", __func__, __LINE__);
		return -1;
	}

	/* Note: must be initialized, otherwise tm_isdst will be an arbitrary value */
	memset(tmp_time, 0, sizeof(struct tm));
	strptime(str_time, "%Y/%m/%d %H:%M:%S", tmp_time);
	t = mktime(tmp_time);
	if (t != 0) {
		*out_sec = (long)t;
		ret = 0;
	} else {
		ret = -1;
		ALOGE("mktime error\n");
	}

	if (tmp_time) {
		free(tmp_time);
		tmp_time = NULL;
	}

	return ret;
}

/**
 * time_second_to_hms - Convert seconds to hour,minutes,second
 * @sec time seconds
 * @p_hour OUT, Hour
 * @p_minutes OUT, Minutes
 * @p_second OUT, Second.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_second_to_hms(int sec, int *p_hour, int *p_minutes, int *p_second)
{
	assert((p_hour != NULL) && (p_minutes != NULL) && (p_second != NULL));

	*p_hour = sec / 3600;
	*p_minutes = (sec % 3600) / 60;
	*p_second = (sec % 3600) % 60;

	return 0;
}

/**
 * time_hms_to_second - Convert hour,minutes,second to seconds
 * @hour IN, Hour
 * @minutes IN, Minutes
 * @second IN, Second.
 * @sec OUT, Time seconds
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_hms_to_second(int hour, int minutes, int second, int *sec)
{
	assert(sec != NULL);

	*sec = hour * 3600 + minutes * 60 + second;

	return 0;
}

/**
 * time_second_to_str - Convert seconds to time strings
 * @sec time seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_second_to_str(double sec, char *out_str_time,
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
 * time_second_to_str - Convert seconds to time strings
 * @ms time m seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_ms_to_str(long long ms, char *out_str_time,
		unsigned int max_out_str_len)
{
	time_t t;
	struct tm *tmp;
	size_t s = 0;

	assert(out_str_time != NULL);

	t = (time_t)(ms / 1000);
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
 * time_second_to_file_name_str - Convert seconds to a filename string
 * @sec time seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_second_to_file_name_str(double sec, char *out_str_time,
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
LOCAL int time_second_to_date_str(double sec, char *out_date_str,
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
LOCAL int time_get_today_start_time_str(char *out_start_time_str,
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
LOCAL int time_get_today_end_time_str(char *out_end_time_str, unsigned int max_out_str_len)
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
LOCAL int time_get_current_sec(time_t *s)
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
LOCAL int time_set_current_time(char *str_time)
{
	int ret = 0;
	long seconds = 0;
	struct timeval tv;

	ret = time_str_to_second(str_time, &seconds);
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

	return 0;
}

/**
 * time_get_cur_date get current date information
 * @param p_date the pointer of saving date information
 *
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_date(char *p_date)
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
		ptm->tm_year+1900, ptm->tm_mon + 1, ptm->tm_mday);

	return 0;
}

/**
 * time_get_cur_date_time get date time information
 * @param p_date_time the pointer of saving date and time information
 *
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_date_time(char *p_date_time)
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
		ptm->tm_year+1900, ptm->tm_mon + 1,
		ptm->tm_mday, ptm->tm_hour,
		ptm->tm_min/*, ptm->tm_sec:%02d*/);

	return 0;
}

/**
 * time_get_cur_time get current time information
 * @param p_date the pointer of saving date information
 *
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_time(char *p_time)
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
 * time_get_cur_time_ret_tm Get the current time and return the tm format
 * @ptm OUT, Store the current time in tm format
 *
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_time_ret_tm(struct tm *cur_tm)
{
	int ret = -1;
	time_t seconds_time = 0;
	struct tm *ptm = NULL;

	assert(cur_tm != NULL);

	time(&seconds_time);
	ptm = localtime(&seconds_time);
	if (ptm != NULL) {
		memcpy(cur_tm, ptm, sizeof(struct tm));
		ret = 0;
	}

	return ret;
}

/**
 * time_get_cur_date_week get date week information
 * @param p_date_time the pointer of saving date and time information
 *
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_date_week(char *p_date, int *week)
{
	time_t seconds_time = 0;
	struct tm *ptm = NULL;
	if (NULL == p_date)
		return -1;
	time(&seconds_time);
	ptm = localtime(&seconds_time);
	if (NULL == ptm)
		return -1;
	sprintf(p_date, "%d-%02d-%02d", ptm->tm_year+1900, ptm->tm_mon + 1, ptm->tm_mday);
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
LOCAL double get_before_n_day_timestamp(double *t, int nday)
{
	double t_before;
	double t_nday;

	if (nday < 0)
		return -1;

	t_nday = nday * SECONDS_OF_ONE_DAY;
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
LOCAL int time_get_cur_time_ret_str(char *out_str_time, unsigned int max_out_str_len)
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

/**
 * time_get_cur_time_ret_long_str - Get the current time string.
 * @out_str_time Out, Returns the current time string.
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_get_cur_time_ret_long_str(char *out_str_time, unsigned int max_out_str_len)
{
	size_t len;
	struct tm *tmp;
	struct timeval tv;
	struct timezone tz;
	char str[64];

	assert(out_str_time != NULL);

	gettimeofday(&tv, &tz);

	tmp = localtime(&(tv.tv_sec));
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}

	memset(str, 0, sizeof(str));
	len = strftime(str, sizeof(str), "%Y%m%d%H%M%S", tmp);
	if (0 == len) {
		ALOGE("[%s:%d] strftime returned 0\n", __func__, __LINE__);
		return -1;
	}

	snprintf(&str[len], sizeof(str) - len, "%06ld", tv.tv_usec);

	snprintf(out_str_time, max_out_str_len, "%s", str);

	return 0;
}

/**
 * get_after_n_day_timestamp - get after n day timestamp second unit.
 * @t: A pointer to timestamp.
 * @nday: day count.
 *
 * Returns 0 if success, otherwise not 0.
 */
LOCAL double get_after_n_day_timestamp(double *t, int nday)
{
	double t_after;
	double t_nday;

	if (nday < 0)
		return -1;

	t_nday = nday * SECONDS_OF_ONE_DAY;
	t_after = (*t) + t_nday;

	return t_after;
}

