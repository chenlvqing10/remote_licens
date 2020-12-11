/*
 * system_time.c - Interface to Set system time
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

#include <sys/time.h>

#include "time_conv.h"
#include "system_time.h"

/* If the system is earlier than this time, it will be set as the default time */
#define SYS_EARLIEST_TIME	"2000/01/01 00:00:00"

/* System default time */
#define SYS_DEFAULT_TIME	"2019/06/08 08:08:08"

/**
 * system_time_set_init_value - Set the initial value of the time
 * None
 *
 * When the device is powered on, if the time is earlier than 2000, the initial value of
 * the time is set to May 28, 2019
 *
 * Returns 0 if success, otherwise not 0.
 */
int system_time_set_init_value()
{
	int ret = 0;
	time_t s = 0;
	long sec_earliest_time = 0;

	time_get_current_sec(&s);

	ret = time_str_to_second(SYS_EARLIEST_TIME, &sec_earliest_time);
	if ((0 == ret) && (s < sec_earliest_time))
		time_set_current_time(SYS_DEFAULT_TIME);

	return 0;
}

