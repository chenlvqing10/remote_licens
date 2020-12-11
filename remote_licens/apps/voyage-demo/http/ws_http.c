/*
 * webserver/src/ws_http.c - all function for http web server.
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

#include "mongoose.h"
#include "ws_http.h"
#include <stdio.h>

static struct mg_serve_http_opts g_http_server_opts;
static struct mg_mgr g_mgr;
static struct mg_connection *g_nc;
static void ev_handler(struct mg_connection *nc, int ev, void *p);

/**
 * ws_http_init - initialize the http webserver.
 * @port: port number.
 * @doc_root: the root folder where the root page is saved.
 *
 * Returns 0 means success, or other value means failure.
 */
int ws_http_init(char *port, char *doc_root)
{
	if ((NULL == port) || (NULL == doc_root))
		return -1;

	printf("port=%s, doc_root=%s\n", port, doc_root);
	mg_mgr_init(&g_mgr, NULL);
	g_nc = mg_bind(&g_mgr, port, ev_handler);
	if (g_nc == NULL)
		return -2;

	mg_set_protocol_http_websocket(g_nc);
	g_http_server_opts.document_root = (char *)calloc(1, strlen(doc_root)+1);
	if (NULL == g_http_server_opts.document_root)
		return -3;
	memcpy((void *)g_http_server_opts.document_root, doc_root, strlen(doc_root));
	g_http_server_opts.enable_directory_listing = "yes";

	printf("ws_http_init ok.\n");
	return 0;
}

int ws_http_register_endpoint(char *endpoint, ws_ep_type type, void *fn)
{
	int err = 0;

	mg_register_http_endpoint(g_nc, endpoint, fn);

	return err;
}

/**
 * ws_http_proc - http webserver procedure which is used as thread function.
 *
 * Returns NONE.
 */
void ws_http_proc(void *param)
{
	while (1)
		mg_mgr_poll(&g_mgr, 1000);

}

/**
 * ws_http_close - close http webserver.
 *
 * Returns 0 means success.
 */
int ws_http_close(void)
{
	mg_mgr_free(&g_mgr);

	return 0;
}

/**
 * ev_handler - event handler function.
 * @nc: pointer of the connection.
 * @ev: event.
 * @p: http message.
 *
 * Returns 0 means success, or other value means failure.
 */
static void ev_handler(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *hm = (struct http_message *)p;

	switch (ev) {
	case MG_EV_HTTP_REQUEST:

	mg_serve_http(nc, hm, g_http_server_opts);
	break;

	default:
	break;
	}
}

