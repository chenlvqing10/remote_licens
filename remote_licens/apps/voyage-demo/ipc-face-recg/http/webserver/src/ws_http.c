/*
 * webserver/src/ws_http.c - all function for http web server.
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

#ifdef WS_MG
#include "mongoose.h"
#endif
#ifdef WS_GOAHEAD
#include "goahead.h"
#endif

#define LOG_TAG		"ws_http"
#include <log/log.h>

#include "ws_http.h"
#include <stdio.h>

#ifdef WS_MG
static struct mg_serve_http_opts g_http_server_opts;
static struct mg_mgr g_mgr;
static struct mg_connection *g_nc;
static void ev_handler(struct mg_connection *nc, int ev, void *p);
#endif
#ifdef WS_GOAHEAD
static int finished = 0;
static void initPlatform(void);
static void sigHandler(int signo);
#endif

/**
 * ws_http_init - initialize the http webserver.
 * @port: port number.
 * @doc_root: the root folder where the root page is saved.
 *
 * Returns 0 means success, or other value means failure.
 */
int ws_http_init(char *port, char *doc_root)
{
	ALOGD("------------------>%s\n", __func__);
	if ((NULL == port)||(NULL == doc_root))
		return -1;
#ifdef WS_MG
	ALOGD("------------------>WS_MG\n");
	mg_mgr_init(&g_mgr, NULL);
	g_nc = mg_bind(&g_mgr, port, ev_handler);
	if (g_nc == NULL)
		return -2;

	mg_set_protocol_http_websocket(g_nc);
	g_http_server_opts.document_root = (char *)calloc(1, strlen(doc_root) + 1);
	if (NULL == g_http_server_opts.document_root)
		return -3;
	memcpy((void *)g_http_server_opts.document_root, doc_root, strlen(doc_root));
	g_http_server_opts.enable_directory_listing = "yes";
#endif
#ifdef WS_GOAHEAD
	ALOGD("------------------>WS_GOAHEAD\n");
	char *route;
	char *auth;
#if 0
	char *endpoints;
	char *tok;
#endif
	char endpoint[20] = {0};

	route = "route.txt";
	auth = "auth.txt";

	initPlatform();
	if (websOpen(doc_root, route) < 0) {
		error("Cannot initialize server. Exiting.");
		return -2;
	}
#if ME_GOAHEAD_AUTH
	ALOGD("------------------>ME_GOAHEAD_AUTH\n");
	if (websLoad(auth) < 0) {
		error("Cannot load %s", auth);
		return -3;
	}
#endif
	sprintf(endpoint,"http://*:%s",port);
	if (websListen(endpoint) < 0)
		return -4;
#if 0
        endpoints = sclone(ME_GOAHEAD_LISTEN);
        for (endpoint = stok(endpoints, ", \t", &tok); endpoint; endpoint = stok(NULL, ", \t,", &tok)) {
#if !ME_COM_SSL
		if (strstr(endpoint, "https"))
			continue;
#endif
		if (websListen(endpoint) < 0) {
			wfree(endpoints);
			return -4;
		}
        }
        wfree(endpoints);
#endif
	/*websServiceEvents(&finished);*/
	/*websClose();*/
#endif
	return 0;
}

/**
 * ws_http_register_endpoint: register http message process function.
 * endpoint: message flag
 * type: event type
 * fn: pointer of process function.
 *
 * Returns 0 if success, otherwise not 0.
 */
int ws_http_register_endpoint(char *endpoint, ws_ep_type type, void *fn)
{
	int err = 0;

	#ifdef WS_GOAHEAD
	switch (type){
	case WS_EP_ASP:
		err = websAspDefine(endpoint, fn);
		break;

	case WS_EP_FORM:
		err = websDefineAction(endpoint, fn);
		break;

	case WS_EP_URL:
		err = websUrlHandlerDefine(endpoint, NULL, 0, fn, 0);
		break;

	default:
		break;
	}
	#endif
	#ifdef WS_MG
	mg_register_http_endpoint(g_nc, endpoint, fn);
	#endif
	return err;
}

/**
 * ws_http_proc - http webserver procedure which is used as thread function.
 *
 * Returns NONE.
 */
void ws_http_proc(void *param)
{
#ifdef WS_MG
	while (1)
		mg_mgr_poll(&g_mgr, 1000);
#endif
#ifdef WS_GOAHEAD
	int	delay, nextEvent;
	while (finished == 0) {
		if (socketSelect(-1, delay)) {
		    socketProcess();
		}
#if ME_GOAHEAD_CGI
		delay = websCgiPoll();
#else
		delay = MAXINT;
#endif
		nextEvent = websRunEvents();
		delay = min(delay, nextEvent);
	}
	websClose();
#endif
}

/**
 * ws_http_close - close http webserver.
 *
 * Returns 0 means success.
 */
int ws_http_close(void)
{
#ifdef WS_MG
	mg_mgr_free(&g_mgr);
#endif
#ifdef WS_GOAHEAD
	websClose();
#endif
	return 0;
}

#ifdef WS_MG
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
	#if 0
	if (mg_vcmp(&hm->uri, "/action/formIPSetting") == 0) {
		char ipaddr[20] = {0};
		mg_get_http_var(&hm->body, "ipaddr", ipaddr, sizeof(ipaddr));
		printf("IP address::%s\n", ipaddr);
	} else {
		mg_serve_http(nc, hm, g_http_server_opts);
	}
	#else
	mg_serve_http(nc, hm, g_http_server_opts);
	#endif
	break;

	default:
	break;
	}
}
#endif

#ifdef WS_GOAHEAD
/**
 * initPlatform - Initalize platform, process the SIGTERM signal.
 *
 * Returns NONE.
 */
static void initPlatform(void)
{
#if ME_UNIX_LIKE
	signal(SIGTERM, sigHandler);
	#ifdef SIGPIPE
	signal(SIGPIPE, SIG_IGN);
	#endif
#endif
}

#if ME_UNIX_LIKE
/**
 * sigHandler - signal handler function.
 * signo: signal id.
 *
 * Returns NONE.
 */
static void sigHandler(int signo)
{
	finished = 1;
}
#endif
#endif
