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

#ifndef _TIME_CONV_H
#define _TIME_CONV_H

#include <time.h>

#ifndef FACE_EXPORT
#define FACE_EXPORT
#if __GNUC__ >= 4 // it means the compiler is GCC version 4.0 or later
	#ifdef FACE_EXPORT
		#define PUBLIC __attribute__((visibility ("default")))
		#define LOCAL __attribute__((visibility("hidden")))
	#else
		#define PUBLIC
		#define LOCAL
	#endif
#else
	#error "##### requires gcc version >= 4.0 #####"
#endif
#endif

/* Conversion between s and ms */
#define TIME_BETWEEN_S_AND_MS_CONVER		1000

/* The number seconds of a day */
#define SECONDS_OF_ONE_DAY		86400

LOCAL long long time_get_us(void);

LOCAL long long time_get_ms(void);

/**
 * time_str_to_second - Convert time string to seconds.
 * @str_time A pointer to char time
 * @sec Out, time seconds
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_str_to_second(const char *str_time, long *out_sec);

/**
 * time_second_to_hms - Convert seconds to hour,minutes,second
 * @sec time seconds
 * @p_hour OUT, Hour
 * @p_minutes OUT, Minutes
 * @p_second OUT, Second.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_second_to_hms(int sec, int *p_hour, int *p_minutes, int *p_second);

/**
 * time_hms_to_second - Convert hour,minutes,second to seconds
 * @hour IN, Hour
 * @minutes IN, Minutes
 * @second IN, Second.
 * @sec OUT, Time seconds
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_hms_to_second(int hour, int minutes, int second, int *sec);

/**
 * time_second_to_str - Convert seconds to time strings
 * @sec time seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_second_to_str(double sec, char *out_str_time,
		unsigned int max_out_str_len);

/**
 * time_second_to_str - Convert seconds to time strings
 * @ms time m seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_ms_to_str(long long ms, char *out_str_time,
		unsigned int max_out_str_len);

/**
 * time_second_to_file_name_str - Convert seconds to a filename string
 * @sec time seconds
 * @out_str_time Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_second_to_file_name_str(double sec, char *out_str_time,
		unsigned int max_out_str_len);

/**
 * time_get_current_sec - Get current time
 * @s OUT, The time as the number of seconds since the Epoch,
 *	1970-01-01 00:00:00 +0000 (UTC)
 *
 * Returns 0 if success, otherwise not 0.
 */
LOCAL int time_get_current_sec(time_t *s);

/**
 * time_set_current_time - Set system time to device
 * @str_time A string with a time format of: YYYY/MM/DD HH:MM:SS
 *
 * Returns 0 if success, otherwise not 0.
 */
LOCAL int time_set_current_time(char *str_time);

/**
 * time_second_to_date_str - Convert seconds to date strings
 * @sec time seconds
 * @out_date_str Out, time strings
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_second_to_date_str(double sec, char *out_date_str,
		unsigned int max_out_str_len);

/**
 * time_today_start_str - Get today's start time
 * @out_start_time_str Out, Store today's start time string
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_get_today_start_time_str(char *out_start_time_str,
		unsigned int max_out_str_len);

/**
 * time_get_today_end_time_str - Get today's end time
 * @out_end_time_str Out, Store today's end time string
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_get_today_end_time_str(char *out_end_time_str,
		unsigned int max_out_str_len);

/**
 * get current date information
 * @param p_date the pointer of saving date information
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_date(char *p_date);

/**
 * get date time information
 * @param p_date_time the pointer of saving date and time information
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_date_time(char *p_date_time);

/**
 * get current time information
 * @param p_date the pointer of saving date information
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_time(char *p_time);

/**
 * time_get_cur_time_ret_tm Get the current time and return the tm format
 * @ptm OUT, Store the current time in tm format
 *
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_time_ret_tm(struct tm *cur_tm);

/**
 * get date week information
 * @param p_date_time the pointer of saving date and time information
 * @return 0-successful; -1-failed
 */
LOCAL int time_get_cur_date_week(char *p_date, int *week);

/**
 * time_get_cur_time_ret_str - Get the current time string.
 * @out_str_time Out, Returns the current time string.
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_get_cur_time_ret_str(char *out_str_time, unsigned int max_out_str_len);

/**
 * time_get_cur_time_ret_long_str - Get the current time string.
 * @out_str_time Out, Returns the current time string.
 * @max_out_str_len The result in the character array out_str_time of size max.
 *
 * Returns 0 if valid, otherwise not 0.
 */
LOCAL int time_get_cur_time_ret_long_str(char *out_str_time, unsigned int max_out_str_len);

/**
 * get_before_n_day_timestamp - get before n day timestamp second unit.
 * @t: A pointer to timestamp.
 * @nday: day count.
 *
 * Returns 0 if success, otherwise not 0.
 */
LOCAL double get_before_n_day_timestamp(double *t, int nday);

/**
 * get_after_n_day_timestamp - get after n day timestamp second unit.
 * @t: A pointer to timestamp.
 * @nday: day count.
 *
 * Returns 0 if success, otherwise not 0.
 */
LOCAL double get_after_n_day_timestamp(double *t, int nday);

#endif /* _TIME_CONV_H */

