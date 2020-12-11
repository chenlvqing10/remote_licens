/*
 * webserver/inc/ws_http.h - all function for http web server.
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

#ifndef _WS_HTTP_H_
#define _WS_HTTP_H_

typedef enum _WS_ENDPOINT_TYPE_E_ {
	WS_EP_ASP,
	WS_EP_FORM,
	WS_EP_URL,
	WS_EP_MAX
} ws_ep_type;

int ws_http_init(const char *port, const char *doc_root);
int ws_http_register_endpoint(char *endpoint, ws_ep_type type, void *fn);
void ws_http_poll(void);
void ws_http_proc(void *param);
int ws_http_close(void);

#endif

