/*
 * api/web_biz_api.h - all functions for RESTful.
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

#ifndef _WEB_BIZ_API_H_
#define _WEB_BIZ_API_H_

#ifdef WS_GOAHEAD
#include "goahead.h"
#endif
#ifdef WS_MG
#include "mongoose.h"
#endif

#include "restful_api.h"
#include "cJSON.h"

#ifdef WS_GOAHEAD
void lb_web_login(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_recg_cfg(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_add_member(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_pic_validate(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_search_member(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_update_member(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_delete_member(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_search_rec(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_export_rec(Webs *wp, char *prefix, char *dir, int flags);
#endif

#ifdef WS_MG
void lb_web_login(struct mg_connection *nc, int ev, void *p);
void lb_web_logout(struct mg_connection *nc, int ev, void *p);
void lb_web_set_recg_cfg(struct mg_connection *nc, int ev, void *p);
void lb_web_get_recg_cfg(struct mg_connection *nc, int ev, void *p);
void lb_web_add_member(struct mg_connection *nc, int ev, void *p);
void lb_web_search_member(struct mg_connection *nc, int ev, void *p);
void lb_web_update_member(struct mg_connection *nc, int ev, void *p);
void lb_web_total_member(struct mg_connection *nc, int ev, void *p);
void lb_web_delete_member(struct mg_connection *nc, int ev, void *p);
void lb_web_search_rec(struct mg_connection *nc, int ev, void *p);
void lb_web_export_rec(struct mg_connection *nc, int ev, void *p);
void lb_web_delete_rec(struct mg_connection *nc, int ev, void *p);
void lb_web_get_cloud_platform_param(struct mg_connection *nc, int ev, void *p);
void lb_web_set_cloud_platform_param(struct mg_connection *nc, int ev, void *p);
void lb_web_get_pic_quality(struct mg_connection *nc, int ev, void *p);
void lb_web_get_feature(struct mg_connection *nc, int ev, void *p);
void lb_web_snapshot(struct mg_connection *nc, int ev, void *p);
void lb_web_add_member_by_feature(struct mg_connection *nc, int ev, void *p);
void lb_web_batch_import_members(struct mg_connection *nc, int ev, void *p);
void lb_web_getwifiname(struct mg_connection *nc, int ev, void *p);
void lb_web_connectwifi(struct mg_connection *nc, int ev, void *p);
#endif

/**
 * get_file_bin_data: read bin data from file
 * @file_path: filename
 * @pbin: bin data
 * @len: len of bin data
 *
 * Returns 0 means success, or other value means failure.
 */
int get_file_bin_data(char *file_path, unsigned char **pbin, int *len);

int create_json(cJSON **root, cJSON **arr);

/**
 * restful_send_fail_result: send failure message to client
 * @nc: client connection's object
 * @json: the result JSON object
 * @arr: error information item
 * @reason: reason of failure
 *
 * Returns none.
 */
void send_fail_result(struct mg_connection *nc, cJSON *json,
		cJSON *arr, int status);

/**
 * find_user_from_list: find user from list
 * @user_name: user name
 * @session_id: session id
 *
 * Returns NULL means no find user, or other value means user's object.
 */
user_list_t *find_user_from_list(const char *user_name,
		const char *session_id);

/**
 * restful_send_ok_result: send success message to client
 * @nc: client connection's object
 * @json: the result JSON object
 *
 * Returns none.
 */
void restful_send_ok_result(struct mg_connection *nc,
		cJSON *json, cJSON *arr, cJSON *obj);

/**
 * restful_send_ok_result: send success message to client
 * @nc: client connection's object
 * @json: the result JSON object
 *
 * Returns none.
 */
int restful_send_ok_result_thread(cJSON *json, int status,
		http_thread_info *thread_info);

#endif
