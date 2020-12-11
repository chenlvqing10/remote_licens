/*
 * open_door_svc.h - Interface to open door
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

#ifndef _OPEN_DOOR_SVC_H
#define _OPEN_DOOR_SVC_H

/**
 * open_door_start_service - start service of open door check
 * if not open door.
 * @delay_time: delay time of open dooor
 *
 * Returns 0 if success, otherwise not 0.
 */
int open_door_start_service(int delay_time);

/**
 * open_door_stop_service - stop service of open door check
 * if not open door.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int open_door_stop_service(void);

/**
 * open_door - open door
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int open_door(void);

/**
 * set_open_door_delay_time - set open door delay time
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int set_open_door_delay_time(int delay_time);

/**
 * alarm_state_change - alarm state change
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
int alarm_state_change(int state);

#endif /* _OPEN_DOOR_SVC_H */

