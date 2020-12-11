/*
 * api/restful_api.c - all functions for RESTful.
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

#ifndef _RESTFUL_API_H_
#define _RESTFUL_API_H_

#include <time.h>

typedef struct _user_list {
	char	user_name[32]; /* user name */

	char	session_id[32]; /* session id */

	time_t	begin_time; /* time of user login success */

	struct _user_list *next; /* next */
} user_list_t;

typedef struct thread_info {
	void *data;
	char notify_url[64];
	char session_id[32];
} http_thread_info;


#ifdef WS_GOAHEAD
int restful_api_cal_sum(Webs *wp, char *prefix, char *dir, int flags);
#endif

const char *get_restful_status_str(int restfault_status);

#endif
