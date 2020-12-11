/*
 * api/sys_cfg.h - all functions for RESTful.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _SYS_CFG_H_
#define _SYS_CFG_H_

typedef struct wifi_hot {
	char hotname[128];
	struct wifi_hot *next;
} wifi_hot;

int sys_get_wifi_hot(wifi_hot *hot, int *count);
int sys_set_screen_bright(char *bright);

#endif

