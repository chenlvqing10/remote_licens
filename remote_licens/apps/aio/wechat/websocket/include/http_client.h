/*
 * http_client.h - all function for http web client.
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

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "mongoose.h"

typedef void (*req_cb)(int state, const char *req, int req_len,
	const char *ans, int ans_len);

struct http_hdl {
	int exit;
	req_cb cb;
};

/**
 * http_client_init - initialize the http webclient.
 * @_mgr: the mg_mgr.
 * @_hdl: the http_hdl.
 *
 * Returns NONE.
 */
void http_client_init(struct mg_mgr *_mgr, struct http_hdl *_hdl);

/**
 * http_client_send_Req - webclient send request to server
 * @_mgr: the mg_mgr.
 * @url: server url.
 * @post_data: the date would been send to server.
 *
 * Returns 0 means success, or other value means failure.
 */
int http_client_send_Req(struct mg_mgr *_mgr,
	const char *url, const char *post_data);

/**
 * http_client_poll - webclient procedure poll
 * @_mgr: the mg_mgr.
 *
 * Returns NONE.
 */
void http_client_poll(struct mg_mgr *_mgr);

/**
 * http_client_close - close http webclient.
 * @_mgr: the mg_mgr.
 *
 * Returns NONE.
 */
void http_client_close(struct mg_mgr *_mgr);


#endif

