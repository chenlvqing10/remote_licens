/*
 * time_ntp.h - Time conversion
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

#ifndef _TIME_NTP_H
#define _TIME_NTP_H

/**
 * ntp_cal_time - get ntp server time and sets the system time
 *
 * Returns 0 if valid, otherwise not 0.
 */
#if 0
int ntp_cal_time(void);

int exit_ntp_cal_time(void);
#endif
int ntp_time_init(int flag);

int ntp_time_deinit(void);

int ntp_start(void);

int ntp_stop(void);

int ntp_checktime(void);
#endif /* _TIME_NTP_H */
