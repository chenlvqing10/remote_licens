/*
 * microwave.h - export some interface function of operate microwave device
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

#ifndef _MICROWAVE_H
#define _MICROWAVE_H

#include <stdlib.h>

#define OBJECT_MOVE		1
#define OBJECT_STOP		0

/* this may be change according system */
#define MICROWAVE_DEV_PATH "/dev/input/event0"

typedef void (*ev_state_cb)(int state);

void event_state_set_cb(ev_state_cb cb);

int microwave_init(void);
int lombo_microwave_start(void);
int lombo_microwave_stop(void);

#endif /* _MICROWAVE_H */

