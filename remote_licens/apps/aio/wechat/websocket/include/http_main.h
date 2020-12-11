/*
 * http_main.h - define interface function of http server main.
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
#ifndef HTTP_MAIN_H_
#define HTTP_MAIN_H_

#include <pthread.h>

typedef struct web_thread {
	pthread_t tid;
	/* 0: stop, other:running */
	char isRunnig;
} web_thread_t;

typedef enum {
    WEB_LOGIN,
    WEB_MAIN_PAGE,
    WEB_REGISTER,
    WEB_PLATFORM_CHANGE,
    WEB_PLATFORM_HEARBEAT,

    WEB_ID_MAX
} WEB_ID;

typedef void (*web_handler_cb)(WEB_ID event, int value);

/**
 * http_send_checkin - send checkin data to remote.
 *
 */
void http_send_checkin(const char *url);

/**
 * http_set_web_handle_cb - set http webserver callback.
 *
 */
void http_set_web_handle_cb(web_handler_cb cb);

/**
 * http_stop_web_server - stop http webserver.
 *
 * Returns 0 success,other failed.
 */
int http_stop_web_server(void);

/**
 * http_start_web_server - start http webserver.
 *
 * Returns 0 success,other failed.
 */
int http_start_web_server(void);



#endif /* HTTP_MAIN_H_ */

