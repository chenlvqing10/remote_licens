/*
 * ws_svc_main.c - main interface of the websocket_test testcase.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/kernel.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "ws_svc.h"

static int s_done = 0;
static int s_is_connected = 0;


static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
	struct websocket_message *wm = (struct websocket_message *)ev_data;
	(void) nc;

	switch (ev) {
	case MG_EV_CONNECT: {
		int status = *((int *) ev_data);
		if (status != 0) {
			printf("-- Connection error: %d\n", status);
		}
		break;
	}
	case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
		struct http_message *hm = (struct http_message *)ev_data;
		if (hm->resp_code == 101) {
			printf("-- Connected\n");
			s_is_connected = 1;
		} else {
			printf("-- Connection failed! HTTP code %d\n", hm->resp_code);
			/* Connection will be close after this. */
		}
		break;
	}
	case MG_EV_POLL: {
		char msg[500];
		int n = 0;
		fd_set read_set, write_set, err_set;
		struct timeval timeout = {0, 0};
		FD_ZERO(&read_set);
		FD_ZERO(&write_set);
		FD_ZERO(&err_set);
		FD_SET(0 /* stdin */, &read_set);
		if (select(1, &read_set, &write_set, &err_set, &timeout) == 1) {
			n = read(0, msg, sizeof(msg));
		}
		if (n <= 0) break;
		while (msg[n - 1] == '\r' || msg[n - 1] == '\n') n--;
		mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, msg, n);
		break;
	}
	case MG_EV_WEBSOCKET_FRAME: {
		printf("%.*s\n", (int) wm->size, wm->data);
		break;
	}
	case MG_EV_WEBSOCKET_CONTROL_FRAME: {
		printf("-- MG_EV_WEBSOCKET_CONTROL_FRAME\n");
		printf("%.*s\n", (int) wm->size, wm->data);
		break;
	}
	case MG_EV_CLOSE: {
		if (s_is_connected)
			printf("-- Disconnected\n");
		1
		/* s_done = 1; */
		break;
	}
	}
}

int main(void)
{
	struct mg_mgr mgr;
	struct mg_connection *nc;
	const char *chat_server_url = "wss://openhw.work.weixin.qq.com/";
	printf("[%s %d]\n", __func__, __LINE__);

	mg_mgr_init(&mgr, NULL);
	printf("[%s %d]\n", __func__, __LINE__);

	nc = mg_connect_ws(&mgr, ev_handler, chat_server_url, "ws_chat", NULL);
	printf("[%s %d]\n", __func__, __LINE__);
	if (nc == NULL) {
		fprintf(stderr, "Invalid address\n");
		return 1;
	}
	printf("[%s %d]\n", __func__, __LINE__);

	while (!s_done) {
		mg_mgr_poll(&mgr, 100);
	}
	mg_mgr_free(&mgr);

	return 0;
}

void ws_svc_deinit(void)
{
	s_done = 1;
}

