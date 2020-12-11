/*
 * ws_svc.h - main interface of the websocket_test testcase.
 *
 * Copyright (C) 2016 - 2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef __WS_SVC__
#define __WS_SVC__

#include "mongoose.h"
#include "list.h"
#include "queue.h"

#define SERVER_URL_LEN 1024
#define PROTOCOL_LEN 64

typedef int(*ws_recv_cb)(unsigned char *data, size_t len, unsigned char flags);
typedef int(*ws_stats_cb)(int stats);
typedef int(*ws_done_cb)(void);

typedef enum {
	NET_NOT_LINKED = 0,
	NET_LINKED,
} net_status;

typedef enum {
	WS_ERROR = 0,
	WS_SUCCESS,
	WS_REST,
} ws_status;

typedef struct _ws_svc_t {
	char server_url[SERVER_URL_LEN];
	char protocol[PROTOCOL_LEN];

	ws_status status;

	ws_recv_cb nc_recv_cb;
	ws_stats_cb nc_success_cb;
	ws_stats_cb nc_reset_cb;
	ws_stats_cb nc_error_cb;

	pthread_t ws_svc_thrd;
	head_t ws_send_queue;
	pthread_mutex_t ws_send_lock;

	unsigned int mg_ev_timer;
	struct list_head ws_timer_list;
	pthread_mutex_t ws_timer_lock;

	pthread_mutex_t net_link_lock;
	pthread_cond_t net_link_cond;
} ws_svc_t;


typedef struct _ws_data_t {
	unsigned char *data;
	size_t len;
	unsigned char flags;
} ws_data_t;

typedef struct _ws_timer_t {
	struct list_head node;

	int sec;
	unsigned char *data;
	size_t len;
	unsigned char flags;
} ws_timer_t;

void *ws_svc_init(char *server_url, size_t slen, char *protocol, size_t plen);
void ws_svc_deinit(void);

void ws_svc_set_net_linked(void);
void ws_svc_set_net_not_linked(void);
void ws_svc_set_net_reconnect(void);

int ws_register_recv_cb(ws_recv_cb cb);
int ws_register_timer(int sec, unsigned char *data, size_t len, unsigned char flags);
int ws_register_timer_text(int sec, unsigned char *data, size_t len);
int ws_register_timer_binary(int sec, unsigned char *data, size_t len);

int ws_register_nc_success_cb(ws_stats_cb cb);
int ws_register_nc_reset_cb(ws_stats_cb cb);
int ws_register_nc_error_cb(ws_stats_cb cb);

int ws_send(unsigned char *data, size_t len, unsigned char flags);
int ws_send_text(unsigned char *data, size_t len);
int ws_send_binary(unsigned char *data, size_t len);

#endif /* __WS_SVC__ */
