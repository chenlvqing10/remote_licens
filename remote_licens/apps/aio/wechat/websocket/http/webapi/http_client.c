/*
 * http_client.c - all function for http web client.
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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

#include "http_client.h"
#include <stdio.h>

#define LOG_TAG "http_client"
#include <log/log.h>

#define HEADERS "Content-Type: application/json\r\nAccept: */*\r\nCache-Control: no-cache\r\n"

static void ev_handler(struct mg_connection *nc, int ev, void *p);

/**
 * http_client_init - initialize the http webclient.
 * @port: port number.
 * @doc_root: the root folder where the root page is saved.
 *
 * Returns NONE.
 */
void http_client_init(struct mg_mgr *_mgr, struct http_hdl *_hdl)
{
	mg_mgr_init(_mgr, _hdl);
	ALOGD("http_client_init ok.\n");
}

/**
 * http_client_send_Req - webclient send request to server
 * @url: server url.
 * @post_data: the date would been send to server.
 *
 * Returns 0 means success, or other value means failure.
 */
int http_client_send_Req(struct mg_mgr *_mgr,
	const char *url, const char *post_data)
{
	struct mg_connection *nc = NULL;

	if (!_mgr) {
		ALOGE("mg_mgr had not inited.");
		return -1;
	}
	ALOGD("http >> %s:%d >> _mgr=%p send[%s] json:%s",
		__func__, __LINE__, _mgr, url, post_data);
#if 0
	const char *_data = "{\"password\":\"123456\",\"platformEnable\":1,\"platformIp\":\"http://192.168.19.97:80/recvTest\"}";
	mg_connect_http(p_mgr, ev_handler, "http://192.168.19.94:80/recvTest", NULL, _data);
#endif
	nc = mg_connect_http(_mgr, ev_handler, url, HEADERS, post_data);
	if (nc == NULL){
		ALOGE("http >> %s:%d >> send[%s] failed",
			__func__, __LINE__, url);
		return -1;
	}else{
		ALOGD("http >> %s:%d >> send[%s] using nc:%p",
			__func__, __LINE__, url, nc);
	}
	return 0;
}

/**
 * http_client_poll - webclient procedure poll
 *
 * Returns NONE.
 */
void http_client_poll(struct mg_mgr *_mgr)
{
	ALOGD("http >> %s:%d >> _mgr=%p ", __func__, __LINE__,  _mgr);
	if (_mgr) {
		mg_mgr_poll(_mgr, 1000);
	}
}

/**
 * http_client_close - close http webclient.
 *
 * Returns NONE.
 */
void http_client_close(struct mg_mgr *_mgr)
{
	ALOGD("http >> %s:%d >> _mgr=%p ", __func__, __LINE__,  _mgr);

	if (_mgr) {
		mg_mgr_free(_mgr);
	}
}

/**
 * ev_handler - event handler function.
 * @nc: pointer of the connection.
 * @ev: event.
 * @p: http message.
 *
 * Returns NONE.
 */
static void ev_handler(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *hm = (struct http_message *)p;
	int connect_status;
	req_cb cb;
	ALOGD("http >> %s:%d >> ev=%d nc=%p", __func__, __LINE__, ev, nc);

	switch (ev) {
	case MG_EV_CONNECT:
		connect_status = *(int *)p;
		if (connect_status != 0) {
			ALOGE("Error connecting to server,error code:%d", connect_status);
			if (nc->mgr->user_data) {
				cb = ((struct http_hdl *)nc->mgr->user_data)->cb;
				if (cb) {
					cb(-1, nc->send_mbuf.buf, nc->send_mbuf.len,
						"", 0);
				}
			}
		}
		break;
	case MG_EV_HTTP_REPLY:
		nc->flags |= MG_F_CLOSE_IMMEDIATELY;
		ALOGD("Server recv msg");

		if (nc->mgr->user_data) {
			cb = ((struct http_hdl *)nc->mgr->user_data)->cb;
			if (cb) {
				cb(0, "", 0, hm->body.p, hm->body.len);
#if 0
			cb(0, nc->send_mbuf.buf, nc->send_mbuf.len,
				hm->body.p, hm->body.len);
#endif
			}
		}
		break;
	case MG_EV_CLOSE:
		ALOGD("Server close connection! nc=%p", nc);
		if (nc->mgr->user_data) {
			cb = ((struct http_hdl *)nc->mgr->user_data)->cb;
			if (cb) {
				cb(-2, "", 0, "", 0);
			}
		}
		break;

	default:
		break;
	}
}

