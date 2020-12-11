/*
 * reboot_svc.h - Interface to reboot
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
 
#ifndef _REBOOT_SVC_H
#define _REBOOT_SVC_H

typedef struct data_time_str {
	int min;
	int hour;
	int day;
	int week;
} data_time_t;

/**
 * reboot_start_service - start service of reboot
 * @delay_time: delay time of open dooor
 *
 * Returns 0 if success, otherwise not 0.
 */
int reboot_start_service(data_time_t *reboot_time);

/**
 * reboot_stop_service - stop service of reboot
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int reboot_stop_service(void);

/**
 * set_reboot__time - set reboot time
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_reboot_time(data_time_t *reboot_time);

#endif /* _REBOOT_SVC_H */
