/*
 * api/web_biz_api.c - all functions for RESTful.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>

#include "biz_api.h"
#include "cJSON.h"
#include "mongoose.h"

#define LOG_TAG "web_biz_api"
#include <log/log.h>

/**
 * lb_web_recg_cfg
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_recg_cfg(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *p_http_msg = (struct http_message *)p;
	char threshold[64] = {0};
	char livingcheck[64] = {0};
	char ops[8] = {0};

	int ret = -1;

	float thv, tmpthv;

	int lvflg = 0;
	int opsflg = 0;

	cJSON *obj = NULL;
	char *pjson = NULL;

	if ((NULL == nc) || (NULL == p))
		return;
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "ops", ops, sizeof(ops));
		mg_get_http_var(&p_http_msg->body, "threshold", threshold,
			sizeof(threshold));
		mg_get_http_var(&p_http_msg->body, "livingcheck", livingcheck,
			sizeof(livingcheck));
		break;

	default:
		break;
	}

	sscanf(livingcheck, "%d", &lvflg);
	sscanf(ops, "%d", &opsflg);

	sscanf(threshold, "%f", &tmpthv);
	thv = tmpthv;

	if (opsflg == 1) {
		ret = recognition_set_param(thv, lvflg);
		if (ret) {
			ALOGE("[lombo_web_recg_cfg:%d]recognition_set_param error\n",
				__LINE__);
			mg_printf(nc, "%s",
				"HTTP/1.1 501\r\nTransfer-Encoding: chunked\r\n\r\n");
			mg_printf_http_chunk(nc, "{ \"result\": \'FAIL\'}");
			mg_send_http_chunk(nc, "", 0);
			return;
		} else {
			mg_printf(nc, "%s",
				"HTTP/1.1 200\r\nTransfer-Encoding: chunked\r\n\r\n");
			mg_printf_http_chunk(nc, "{ \"result\": \'OK\'}");
			mg_send_http_chunk(nc, "", 0);
		}

	} else if (opsflg == 0) {
		ret = recognition_get_param(&thv, &lvflg);
		if (ret) {
			ALOGE("[lombo_web_recg_cfg:%d]recognition_get_param error\n",
				__LINE__);
			mg_printf(nc, "%s",
				"HTTP/1.1 501\r\nTransfer-Encoding: chunked\r\n\r\n");
			mg_printf_http_chunk(nc, "{ \"result\": \'FAIL\'}");
			mg_send_http_chunk(nc, "", 0);
			goto Exit;
		}
		obj = cJSON_CreateObject();
		if (NULL == obj) {
			ret = -1;
			goto Exit;
		}
		memset(threshold, 0, sizeof(threshold));
		memset(livingcheck, 0, sizeof(livingcheck));
		snprintf(threshold, sizeof(threshold), "%f", thv);
		snprintf(livingcheck, sizeof(livingcheck), "%d", lvflg);
		cJSON_AddStringToObject(obj, "threshold", threshold);
		cJSON_AddStringToObject(obj, "livingcheck", livingcheck);
		pjson = cJSON_PrintUnformatted(obj);
		if (NULL == pjson)
			goto Exit;

		mg_printf(nc, "%s", "HTTP/1.1 200\r\nTransfer-Encoding: chunked\r\n\r\n");
		mg_send_http_chunk(nc, pjson, strlen(pjson));
		mg_send_http_chunk(nc, "", 0);

	}
Exit:
	if (obj)
		cJSON_Delete(obj);

	return;
}

/**
 * lb_web_bright_setting
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_bright_setting(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *p_http_msg = (struct http_message *)p;
	char bright[32] = {0};
	char ops[32] = {0};

	int ret = -1;
	int bri = 0;
	int opsflg = 0;

	cJSON *obj;
	char *pjson = NULL;

	if ((NULL == nc) || (NULL == p))
		return;
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "ops", ops, sizeof(ops));
		mg_get_http_var(&p_http_msg->body, "bright", bright, sizeof(bright));
		break;

	default:
		break;
	}

	sscanf(bright, "%d", &bri);
	sscanf(ops, "%d", &opsflg);

	if (1 == opsflg) {
		ret = display_set_bright_to_device(bri);
		ALOGD("[display_set_bright_to_device]ret=%d\n", ret);
		if (ret) {
			mg_printf(nc, "%s",
				"HTTP/1.1 501\r\nTransfer-Encoding: chunked\r\n\r\n");
			mg_printf_http_chunk(nc, "{ \"result\": \'FAIL\'}");
			mg_send_http_chunk(nc, "", 0);
			goto Exit;
		} else {
			mg_printf(nc, "%s",
				"HTTP/1.1 200\r\nTransfer-Encoding: chunked\r\n\r\n");
			mg_printf_http_chunk(nc, "{ \"result\": \'OK\'}");
			mg_send_http_chunk(nc, "", 0);
		}
	} else if (opsflg == 0) {
		ret = display_get_bright_from_device(&bri);
		if (ret) {
			mg_printf(nc, "%s",
				"HTTP/1.1 501\r\nTransfer-Encoding: chunked\r\n\r\n");
			mg_printf_http_chunk(nc, "{ \"result\": \'FAIL\'}");
			mg_send_http_chunk(nc, "", 0);
			goto Exit;
		}
		obj = cJSON_CreateObject();
		if (NULL == obj)
			goto Exit;
		cJSON_AddNumberToObject(obj, "bright", bri);
		pjson = cJSON_PrintUnformatted(obj);
		if (NULL == pjson)
			goto Exit;
		mg_printf(nc, "%s", "HTTP/1.1 200\r\nTransfer-Encoding: chunked\r\n\r\n");
		mg_send_http_chunk(nc, pjson, strlen(pjson));
		mg_send_http_chunk(nc, "", 0);
	}

Exit:
	if (pjson)
		cJSON_Delete(obj);

	return;
}

/**
 * lb_web_sys_info
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_sys_info(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	char *pjson = NULL;

	char build_date[64] = {0};
	char build_user[64] = {0};
	char build_host[64] = {0};
	char product_name[64] = {0};

	if ((NULL == nc) || (NULL == p))
		return;
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		break;

	default:
		break;
	}

	ret = system_info_get_build_info(build_date, build_user, build_host,
		product_name);

	cJSON *obj = cJSON_CreateObject();
	if (NULL == obj) {
		ret = -1;
		goto Exit;
	}

	cJSON_AddStringToObject(obj, "build_date", build_date);
	cJSON_AddStringToObject(obj, "build_author", build_user);
	cJSON_AddStringToObject(obj, "build_host", build_host);
	cJSON_AddStringToObject(obj, "model", product_name);

	pjson = cJSON_PrintUnformatted(obj);
Exit:
	if (ret == 0) {
		mg_printf(nc, "%s", "HTTP/1.1 200\r\nTransfer-Encoding: chunked\r\n\r\n");
		if (pjson)
			mg_send_http_chunk(nc, pjson, strlen(pjson));
		mg_send_http_chunk(nc, "", 0);
	} else {
		mg_printf(nc, "%s", "HTTP/1.1 501\r\nTransfer-Encoding: chunked\r\n\r\n");
		mg_printf_http_chunk(nc, "{ \"result\": \"fail\" }");
		mg_send_http_chunk(nc, "", 0);
	}

	if (obj != NULL)
		cJSON_Delete(obj);

	return;
}

