/*
 * system_time.h - Interface to Set system time
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

#ifndef _SYSTEM_TIME_H
#define _SYSTEM_TIME_H

#include <time.h>

/**
 * system_time_set_init_value - Set the initial value of the time
 * None
 *
 * When the device is powered on, if the time is earlier than 2000, the initial value of
 * the time is set to May 28, 2019
 *
 * Returns 0 if success, otherwise not 0.
 */
int system_time_set_init_value();

#endif /* _SYSTEM_TIME_H */
