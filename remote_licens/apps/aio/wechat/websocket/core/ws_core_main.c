/*
 * ws_core_main.c - main interface of the websocket_test testcase.
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
#include <sys/prctl.h>

#include "ws_core_main.h"
#include "ws_core_log.h"

#define LOG_TAG "ws_core"
#include <log/log.h>

static int s_done = 0; /* s_done = 1 websocked disconnected */
static ws_svc_t *ws_svc = NULL;
static net_status s_linked = NET_NOT_LINKED;
static int s_run;

/**
 * @brief  mongoose webscoket status proc
 * @note   process status example:connect,recv,close;
 * @param  *nc: the information of connect
 * @param  ev: cmd type
 * @param  *ev_data: point to different data as ev different
 * @retval None
 */
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
	struct websocket_message *wm = (struct websocket_message *)ev_data;
	(void) nc;

	switch (ev) {
	case MG_EV_CONNECT: {
		ALOGD("MG_EV_CONNECT\n");
		int status = *((int *) ev_data);
		if (status != 0) {
			ALOGE("-- Connection error: %d\n", status);
			ws_svc->status = WS_ERROR;
			if (ws_svc->nc_error_cb)
				ws_svc->nc_error_cb(ws_svc->status);
			/* need reset connect */
			s_done = 1;
		}
		break;
	}

	case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
		ALOGD("MG_EV_WEBSOCKET_HANDSHAKE_DONE\n");
		struct http_message *hm = (struct http_message *)ev_data;
		if (hm->resp_code == 101) {
			ALOGD("Connected\n");
			/* ws_trace("message=%.*s\n", (int)hm->message.len, hm->message.p); */
			ws_svc->status = WS_SUCCESS;
			if (ws_svc->nc_success_cb)
				ws_svc->nc_success_cb(ws_svc->status);

			/* succes connected, start timer */
			mg_set_timer(nc, mg_time() + 1);
		} else {
			ALOGE("-- Connection failed! HTTP code %d\n", hm->resp_code);
			/* Connection will be close after this. */
			s_done = 1;
		}
		break;
	}

	case MG_EV_POLL: {
		if (ws_svc->status == WS_SUCCESS) {
			ws_data_t *ws_data = NULL;
			pthread_mutex_lock(&ws_svc->ws_send_lock);
			if (!empty_queue(&ws_svc->ws_send_queue)) {
				ws_data = (ws_data_t *)de_queue(&ws_svc->ws_send_queue);
			}
			pthread_mutex_unlock(&ws_svc->ws_send_lock);
			if (ws_data) {
				ALOGD("send data=%s, len=%d\n", ws_data->data, ws_data->len);
				mg_send_websocket_frame(nc, ws_data->flags,
					ws_data->data, ws_data->len);

				free(ws_data->data);
				ws_data->data = NULL;
				free(ws_data);
				ws_data = NULL;
			}
		}
		break;
	}

	case MG_EV_WEBSOCKET_FRAME: {
		ALOGD("frame data=%.*s len = %d flags = %d\n", (int)wm->size, wm->data, (int)wm->size,
			wm->flags & 0x0f);
		if (ws_svc->nc_recv_cb)
			ws_svc->nc_recv_cb(wm->data, wm->size, wm->flags & 0x0f);
		break;
	}

	case MG_EV_WEBSOCKET_CONTROL_FRAME: {
		/* 3probe = pong */
		/* ws_trace("conrtol frame data=%.*s\n", (int)wm->size, wm->data); */
		break;
	}

	case MG_EV_TIMER: {
		/* reset timer */
		mg_set_timer(nc, mg_time() + 1);
		ws_svc->mg_ev_timer++;
		/* ws_info("mg_ev_timer=%d\n", ws_svc->mg_ev_timer); */

		if (ws_svc->status == WS_SUCCESS) {
			ws_timer_t *ws_timer = NULL;
			ws_timer_t *ws_timer_tmp = NULL;
			pthread_mutex_lock(&ws_svc->ws_timer_lock);
			list_for_each_entry_safe(ws_timer, ws_timer_tmp,
				&ws_svc->ws_timer_list, node) {
				if (!(ws_svc->mg_ev_timer % ws_timer->sec)) {
					ALOGD("timer data=%s, len=%d\n", ws_timer->data, ws_timer->len);
					mg_send_websocket_frame(nc, ws_timer->flags,
						ws_timer->data, ws_timer->len);
				}
			}
			pthread_mutex_unlock(&ws_svc->ws_timer_lock);
		}
		break;
	}

	case MG_EV_CLOSE: {
		ALOGD("MG_EV_CLOSE\n");
		ws_svc->status = WS_REST;
		if (ws_svc->nc_reset_cb)
			ws_svc->nc_reset_cb(ws_svc->status);

		/* need reset connect */
		s_done = 1;
		break;
	}
	}
}

/**
 * @brief  set websocket net link
 * @note   the websocket begin connect server after call this funciton
 * @retval None
 */
void ws_svc_set_net_linked(void)
{
	if (ws_svc) {
		pthread_mutex_lock(&ws_svc->net_link_lock);
		s_linked = NET_LINKED;
		pthread_cond_signal(&ws_svc->net_link_cond);
		pthread_mutex_unlock(&ws_svc->net_link_lock);
	}
}

/**
 * @brief  set websocket dislink
 * @note   stop reconnect server when call this function
 * @retval None
 */
void ws_svc_set_net_not_linked(void)
{
	if (ws_svc) {
		pthread_mutex_lock(&ws_svc->net_link_lock);
		s_linked = NET_NOT_LINKED;
		s_done = 1;
		ws_svc->status = WS_ERROR;
		if (ws_svc->nc_error_cb)
			ws_svc->nc_error_cb(ws_svc->status);
		pthread_cond_signal(&ws_svc->net_link_cond);
		pthread_mutex_unlock(&ws_svc->net_link_lock);
	}
}

/**
 * @brief  set net reconnect
 * @note   websocket will reconnect server when call this function
 * @retval None
 */
void ws_svc_set_net_reconnect(void)
{
	if (ws_svc) {
		pthread_mutex_lock(&ws_svc->net_link_lock);
		s_linked = NET_LINKED;
		s_done = 1;
		ws_svc->status = WS_ERROR;
		if (ws_svc->nc_error_cb)
			ws_svc->nc_error_cb(ws_svc->status);
		pthread_cond_signal(&ws_svc->net_link_cond);
		pthread_mutex_unlock(&ws_svc->net_link_lock);
	} else {
		ALOGE("%s: ws_svc is not init\n", __func__);
	}
}

/**
 * @brief  the main thread for control the websocket logic
 * @note   NULL
 * @param  *arg:
 * @retval None
 */
static void *ws_svc_thread(void *arg)
{
	ws_svc_t *svc = (ws_svc_t *)arg;
	if (svc == NULL) {
		ALOGE("stop ws_svc\n");
		return NULL;
	}

	prctl(PR_SET_NAME, "websocket");

	s_run = 1;
	//log_init();
	while (s_run) {
		struct mg_mgr mgr;
		struct mg_connection *nc;

		pthread_mutex_lock(&ws_svc->net_link_lock);
		if (s_linked != NET_LINKED)
			pthread_cond_wait(&ws_svc->net_link_cond, &ws_svc->net_link_lock);
		pthread_mutex_unlock(&ws_svc->net_link_lock);

		mg_mgr_init(&mgr, NULL);

		nc = mg_connect_ws(&mgr, ev_handler,
			svc->server_url, svc->protocol, NULL);
		if (nc == NULL) {
			ALOGE("Invalid address\n");
		}

		s_done = 0;
		while (!s_done) {
			mg_mgr_poll(&mgr, 100);
		}
		mg_mgr_free(&mgr);
		ws_clean_send_queue();
	}
	ALOGD("%s: exit\n", __func__);
	return NULL;
}

/**
 * @brief  svc init
 * @note   alloc resources for websokcet
 * @param  *server_url:  url need linked
 * @param  slen: length of url
 * @param  *protocol: support protocol type
 * @param  plen: length of protocol
 * @retval None
 */
void *ws_svc_init(char *server_url, size_t slen,
	char *protocol, size_t plen)
{
	if (server_url == NULL || slen > SERVER_URL_LEN ||
		protocol == NULL || plen > PROTOCOL_LEN) {
		ALOGE("[%s] param\n", __func__);
		goto error;
	}

	ws_svc = (ws_svc_t *)malloc(sizeof(ws_svc_t));
	if (!ws_svc) {
		ALOGE("malloc ws_svc_init failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto error;
	}

	memset(ws_svc, 0, sizeof(ws_svc_t));
	memcpy(ws_svc->server_url, server_url, slen);
	memcpy(ws_svc->protocol, protocol, plen);

	TAILQ_INIT(&ws_svc->ws_send_queue);
	if (pthread_mutex_init(&ws_svc->ws_send_lock, NULL)) {
		ALOGE("init ws_send_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto error;
	}

	INIT_LIST_HEAD(&ws_svc->ws_timer_list);
	if (pthread_mutex_init(&ws_svc->ws_timer_lock, NULL)) {
		ALOGE("init ws_timer_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto error;
	}

	if (pthread_mutex_init(&ws_svc->net_link_lock, NULL)) {
		ALOGE("init net_link_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto error;
	}
	if (pthread_cond_init(&ws_svc->net_link_cond, NULL)) {
		ALOGE("init net_link_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto error;
	}

	if (pthread_create(&ws_svc->ws_svc_thrd, NULL,
			ws_svc_thread, (void *)ws_svc)) {
		ALOGE("create ws_svc_thread thread failed, "
			"errno: %d (%s)", errno, strerror(errno));
		goto error;
	}
	return ws_svc;

error:

	if (ws_svc) {
		free(ws_svc);
		ws_svc = NULL;
	}

	return NULL;
}

/**
 * @brief  clean send queue
 * @note   clean send queue when deinit websocket
 * @retval None
 */
void ws_clean_send_queue(void)
{
	ws_data_t *ws_data = NULL;
	pthread_mutex_lock(&ws_svc->ws_send_lock);
	while (!empty_queue(&ws_svc->ws_send_queue)) {
		ws_data = (ws_data_t *)de_queue(&ws_svc->ws_send_queue);
		if (ws_data) {
			free(ws_data->data);
			ws_data->data = NULL;
			free(ws_data);
			ws_data = NULL;
		}
	}
	pthread_mutex_unlock(&ws_svc->ws_send_lock);
}

/**
 * @brief  clean timer list
 * @note   clean timer list when deinit
 * @retval None
 */
void ws_clean_timer_list(void)
{
	ws_timer_t *ws_timer = NULL;
	ws_timer_t *ws_timer_tmp = NULL;

	pthread_mutex_lock(&ws_svc->ws_timer_lock);
	list_for_each_entry_safe(ws_timer, ws_timer_tmp,
		&ws_svc->ws_timer_list, node) {
		if (ws_timer) {
			list_del(&ws_timer->node);
			if (ws_timer) {
				free(ws_timer->data);
				ws_timer->data = NULL;
				free(ws_timer);
				ws_timer = NULL;
			}
		}
	}
	pthread_mutex_unlock(&ws_svc->ws_timer_lock);
}

/**
 * @brief  deinit websokcet
 * @note   free resources for websocket
 * @retval None
 */
void ws_svc_deinit(void)
{
	if (s_run) {
		s_run = 0;
		if (s_linked == NET_NOT_LINKED) {
			ALOGD("%s:signal send\n", __func__);
			ws_svc_set_net_linked();
		}
		s_done = 1;
		pthread_join(ws_svc->ws_svc_thrd, NULL);
		ALOGD("%s:exit\n", __func__);
	}

	if (ws_svc) {
		free(ws_svc);
		ws_svc = NULL;
	}
}

/**
 * @brief  send date to websocket server
 * @note   the real send operation in ev_hander
 * @param  *data: the data to send
 * @param  len: data length
 * @param  flags: data type
 * @retval 0 success
 */
int ws_send(unsigned char *data, size_t len, unsigned char flags)
{
	int ret = -1;
	ws_data_t *ws_data = NULL;

	if (data == NULL || len <= 0) {
		ALOGE("ws_send NULL data\n");
		return -1;
	}

	if (ws_svc->status != WS_SUCCESS) {
		ALOGE("ws status error\n");
		return -1;
	}

	ws_data = (ws_data_t *)malloc(sizeof(ws_data_t));
	if (ws_data == NULL) {
		ALOGE("malloc ws_data_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}
	memset(ws_data, 0, sizeof(ws_data_t));

	ws_data->data = (unsigned char *)malloc(len + 1);
	if (ws_data->data == NULL) {
		ret = -1;
		ALOGE("malloc ws_data data failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto error;
	}
	memset(ws_data->data, 0, len + 1);

	memcpy(ws_data->data, data, len);
	ws_data->len = len;
	ws_data->flags = flags;

	pthread_mutex_lock(&ws_svc->ws_send_lock);
	ret = en_queue(&ws_svc->ws_send_queue, ws_data);
	pthread_mutex_unlock(&ws_svc->ws_send_lock);
	if (ret) {
		ALOGE("ws_send en_queue failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto error;
	}

	return 0;

error:

	if (ws_data->data) {
		free(ws_data->data);
		ws_data->data = NULL;
	}

	if (ws_data) {
		free(ws_data);
		ws_data = NULL;
	}

	return ret;
}

/**
 * @brief  send text
 * @note   NULL
 * @param  *data:
 * @param  len:
 * @retval 0 success
 */
int ws_send_text(unsigned char *data, size_t len)
{
	return ws_send(data, len, WEBSOCKET_OP_TEXT);
}

/**
 * @brief  sent binary data
 * @note	NULL
 * @param  *data:
 * @param  len:
 * @retval 0 success
 */
int ws_send_binary(unsigned char *data, size_t len)
{
	return ws_send(data, len, WEBSOCKET_OP_BINARY);
}

/**
 * @brief register timer
 * @note  add send data by timer  trigger
 * @param  sec: send  interval units second
 * @param  *data:   success
 * @param  len: data length
 * @param  flags:  data type
 * @retval 0 success
 */
int ws_register_timer(int sec, unsigned char *data, size_t len, unsigned char flags)
{
	ALOGD("ws_register_timer\n");
	int ret = -1;
	ws_timer_t *ws_timer = NULL;

	if (data == NULL || len <= 0) {
		ALOGE("ws_register_timer NULL data\n");
		return -1;
	}

	ws_timer = (ws_timer_t *)malloc(sizeof(ws_timer_t));
	if (ws_timer == NULL) {
		ALOGE("malloc ws_timer_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}
	memset(ws_timer, 0, sizeof(ws_timer_t));

	ws_timer->data = (unsigned char *)malloc(len + 1);
	if (ws_timer->data == NULL) {
		ret = -1;
		ALOGE("malloc ws_timer data failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto error;
	}
	memset(ws_timer->data, 0, len + 1);

	memcpy(ws_timer->data, data, len);
	ws_timer->len = len;
	ws_timer->flags = flags;
	ws_timer->sec = sec;

	pthread_mutex_lock(&ws_svc->ws_timer_lock);
	list_add(&ws_timer->node, &ws_svc->ws_timer_list);
	pthread_mutex_unlock(&ws_svc->ws_timer_lock);
	return 0;

error:

	if (ws_timer->data) {
		free(ws_timer->data);
		ws_timer->data = NULL;
	}

	if (ws_timer) {
		free(ws_timer);
		ws_timer = NULL;
	}
	return ret;
}

/**
 * @brief  register send text data timer
 * @note   NULL
 * @param  sec:
 * @param  *data:
 * @param  len:
 * @retval 0 success
 */
int ws_register_timer_text(int sec, unsigned char *data, size_t len)
{
	return ws_register_timer(sec, data, len, WEBSOCKET_OP_TEXT);
}

/**
 * @brief  register send binary data timer
 * @note   NULL
 * @param  sec:
 * @param  *data:
 * @param  len:
 * @retval 0 success
 */
int ws_register_timer_binary(int sec, unsigned char *data, size_t len)
{
	return ws_register_timer(sec, data, len, WEBSOCKET_OP_BINARY);
}

/**
 * @brief register recv callback
 * @note  recv function will be call when received data
 * @param  cb:
 * @retval 0 success
 */
int ws_register_recv_cb(ws_recv_cb cb)
{
	if (cb == NULL) {
		ALOGE("ws_register_timer NULL cb\n");
		return -1;
	}
	if (ws_svc == NULL) {
		ALOGE("ws_svc NULL\n");
		return -1;
	}
	ws_svc->nc_recv_cb = cb;
	return 0;
}

/**
 * @brief register websocket connect  callback
 * @note  will be called  when connect server success
 * @param  cb:
 * @retval 0 success
 */
int ws_register_nc_success_cb(ws_stats_cb cb)
{
	if (cb == NULL) {
		ALOGE("ws_register_nc_success_cb NULL cb\n");
		return -1;
	}
	if (ws_svc == NULL) {
		ALOGE("ws_svc NULL\n");
		return -1;
	}
	ws_svc->nc_success_cb = cb;
	return 0;
}

/**
 * @brief  register  websocket disconnect callback
 * @note   will be called when websocket disconnect
 * @param  cb:
 * @retval 0 success
 */
int ws_register_nc_reset_cb(ws_stats_cb cb)
{
	if (cb == NULL) {
		ALOGE("ws_register_nc_reset_cb NULL cb\n");
		return -1;
	}
	if (ws_svc == NULL) {
		ALOGE("ws_svc NULL\n");
		return -1;
	}
	ws_svc->nc_reset_cb = cb;
	return 0;
}

/**
 * @brief  register errer callback
 * @note   will be called when websocket error
 * @param  cb:
 * @retval 0 success
 */
int ws_register_nc_error_cb(ws_stats_cb cb)
{
	if (cb == NULL) {
		ALOGE("ws_register_nc_error_cb NULL cb\n");
		return -1;
	}
	if (ws_svc == NULL) {
		ALOGE("ws_svc NULL\n");
		return -1;
	}
	ws_svc->nc_error_cb = cb;
	return 0;
}

