/*
* gui/lbui/lbal/osal/lb_os_mem.c - all functions to allocate and free memory.
*
* Copyright (C) 2016-2018, LomboTech Co.Ltd.
* Author: lomboswer <lomboswer@lombotech.com>
*
* Common code for LomboTech Socs
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
*/

/*----------------------------------------------*
 * header files                           *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lb_os_http.h"

#define LOG_TAG "lb_os_http"
#include <log/log.h>

/*----------------------------------------------*
 * defines                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * structs                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * extern functions                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * functions                                    *
 *----------------------------------------------*/
#define lb_mem_http

#ifdef lb_mem_http
#define MAX_NUM   800
typedef struct lb_tag_points {
	void	*addr;
	int	size;
	int	line;
	char	func[30];
	int	is_empty;
} lb_points_t;

lb_points_t	lb_points[MAX_NUM];

void add_point(void *addr, int size, int line, const char *func)
{
	int i = 0;

	for (i = 0; i < MAX_NUM; i++) {
		if (0 == lb_points[i].is_empty) {
			lb_points[i].addr = addr;
			lb_points[i].size = size;
			lb_points[i].line = line;
			lb_points[i].is_empty = 1;
			if (NULL != func)
				memcpy(lb_points[i].func, func, strlen(func));
			break;
		}
	}
}

void del_point(void *addr)
{
	int i = 0;

	for (i = 0; i < MAX_NUM; i++) {
		if (addr == lb_points[i].addr) {
			lb_points[i].addr = NULL;
			lb_points[i].size = 0;
			lb_points[i].line = 0;
			lb_points[i].is_empty = 0;
			memset(lb_points[i].func, 0, 30);
			break;
		}
	}
}

void lb_mem_dump(void)
{
	int i = 0;
	for (i = 0; i < MAX_NUM; i++) {
		if (0 != lb_points[i].is_empty) {
			ALOGD("i : %d", i);
			ALOGD("  line : %d", lb_points[i].line);
			ALOGD(" addr : 0x%x", (unsigned int)lb_points[i].addr);
			ALOGD(" size : %d", lb_points[i].size);
			if (NULL != lb_points[i].func)
				ALOGD(" func : %s", lb_points[i].func);
			ALOGD("  is_empty : %d\n", lb_points[i].is_empty);
		}
	}
}

#endif
void *lb_mem_alloc_ext(int size, const char *func, int lines)
{
	void *ret;

	if (size <= 0)
		return NULL;
	ret = calloc(size, 1);

	#ifdef lb_mem_http
	add_point(ret, size, lines, func);
	#endif
	return ret;
}

void lb_mem_free(void *p)
{
	if (NULL == p)
		return;

	#ifdef lb_mem_http
	del_point(p);
	#endif
	free(p);
}

