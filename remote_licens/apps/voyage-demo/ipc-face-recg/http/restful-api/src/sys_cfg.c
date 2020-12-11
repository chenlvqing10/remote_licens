/*
 * api/web_biz_api.c - all functions for RESTful.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "sys_cfg.h"
#include "lb_os_http.h"

/**
 * sys_get_wifi_hot: get wifi hot list (for test)
 * hot: pointer of wifi hot list
 * count: count of wifi hot list
 *
 * Returns 0 means success.
 */
int sys_get_wifi_hot(wifi_hot *hot, int *count)
{
	int iRet = 0;
	wifi_hot *tmp = hot;

	memcpy(tmp->hotname, "TP-LINK_001", strlen("TP-LINK_001"));

	tmp->next = lb_mem_alloc_ext(sizeof(wifi_hot), __func__, __LINE__);
	tmp = tmp->next;
	memcpy(tmp->hotname, "TP-LINK_002", strlen("TP-LINK_002"));

	tmp->next = lb_mem_alloc_ext(sizeof(wifi_hot), __func__, __LINE__);
	tmp = tmp->next;
	memcpy(tmp->hotname, "TP-LINK_003", strlen("TP-LINK_003"));

	*count = 3;

	return iRet;
}

/**
 * sys_set_screen_bright: set screen bright (for test)
 * bright: bright value
 *
 * Returns 0 means success, other means failure.
 */
int sys_set_screen_bright(char *bright)
{
	int ret = -1;
	char cbr = atoi(bright);

	sscanf(bright, "%c", &cbr);
	return ret;
}

