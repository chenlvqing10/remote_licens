/*
 * api/web_biz_api.c - all functions for RESTful.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is lb_mem_free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the lb_mem_free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the lb_mem_free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>

#include "cJSON.h"
#include "sys_http.h"

#include "record_auto_delete.h"
#include "time_conv.h"
#include "recognition.h"

#include "person.h"
#include "common_record_info.h"
#include "db_cloud_platform.h"
#include "db_manager_info.h"
#include "jpeg_dec.h"
#include "config_db.h"
#include "record_db.h"
#include "face_db.h"
#include "lb_os_http.h"
#include "sys_info.h"

#define LOG_TAG "web_biz_api.c"
#include <log/log.h>

#include "restful_api.h"
#include "base64.h"
#include "http.h"
#include "system_admin.h"

person_t *pst_person;
person_reg_param_t *reg_param;
user_list_t *s_user_list; /* user list of logined */
/*
#define CHECK_MALLOC
*/
#ifdef WS_GOAHEAD
#include "goahead.h"
#endif
#ifdef WS_MG
#include "mongoose.h"
#endif

#ifdef WS_GOAHEAD
/**
 * lb_web_login
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_login(Webs *wp, char *prefix, char *dir, int flags)
{
	char *pStr1;
	char *pStr2;
	pStr1 = websGetVar(wp, "username", "");
	pStr2 = websGetVar(wp, "password", "");

	if (strcmp(pStr1, "xiaowei") == 0 && strcmp(pStr2, "123456") == 0) {
		websFlush(wp, 0);
		websSetStatus(wp, 200);
		websHeader(wp);
		websWrite(wp, "{ \"result\": \"ok\" }");
		websDone(wp);
	} else {
		websFlush(wp, 0);
		websSetStatus(wp, 199);
		websHeader(wp);
		websWrite(wp, "{ \"result\": \"fail\" }");
		websDone(wp);
	}

	return;
}

/**
 * lb_web_recg_cfg
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_recg_cfg(Webs *wp, char *prefix, char *dir, int flags)
{
	websFlush(wp, 0);
	websSetStatus(wp, 200);
	websHeader(wp);
	websWrite(wp, "{ \"result\": \"ok\" }");
	websDone(wp);

	return;
}

/**
 * lb_web_pic_validate
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_pic_validate(Webs *wp, char *prefix, char *dir, int flags)
{
	websFlush(wp, 0);
	websSetStatus(wp, 200);
	websHeader(wp);
	websWrite(wp, "{ \"result\": \"ok\" }");
	websDone(wp);

	return;
}

/**
 * lb_web_search_member
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_search_member(Webs *wp, char *prefix, char *dir, int flags)
{
	websFlush(wp, 0);
	websSetStatus(wp, 200);
	websHeader(wp);
	websWrite(wp, "{ \"result\": \"ok\" }");
	websDone(wp);

	return;
}

/**
 * lb_web_search_rec
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_search_rec(Webs *wp, char *prefix, char *dir, int flags)
{
	websFlush(wp, 0);
	websSetStatus(wp, 200);
	websHeader(wp);
	websWrite(wp, "{ \"result\": \"ok\" }");
	websDone(wp);

	return;
}

/**
 * lb_web_export_rec
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
void lb_web_export_rec(Webs *wp, char *prefix, char *dir, int flags)
{
	websFlush(wp, 0);
	websSetStatus(wp, 200);
	websHeader(wp);
	websWrite(wp, "{ \"result\": \"ok\" }");
	websDone(wp);

	return;
}

#endif

#ifdef WS_MG

int create_json(cJSON **root, cJSON **arr)
{
	*root = cJSON_CreateObject();
	if (!root) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		return -1;
	}
	*arr = cJSON_CreateArray();
	if (!arr) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		return -2;
	}

	return 0;
}

/**
 * add_user_to_list: add user to list
 * @user: user of logined
 *
 * Returns 0 means success, or other value means failure.
 */
int add_user_to_list(user_list_t *user)
{
	user_list_t *temp;
	if (!user)
		return -1;

	if (!s_user_list)
		s_user_list = user;
	else {
		temp = s_user_list;
		while (temp->next)
			temp = temp->next;
		temp->next = user;
	}

	return 0;
}

/**
 * find_user_from_list: find user from list
 * @user_name: user name
 * @session_id: session id
 *
 * Returns NULL means no find user, or other value means user's object.
 */
user_list_t *find_user_from_list(const char *user_name, const char *session_id)
{
	int is_find = 0;
	user_list_t *temp;

	if (!user_name || !session_id)
		return NULL;

	temp = s_user_list;
	while (temp) {
		if (!strcmp(temp->user_name, user_name)
			&& !strcmp(temp->session_id, session_id)) {
			is_find = 1;
			break;
		}
		temp = temp->next;
	}

	if (is_find)
		return temp;

	return NULL;
}

/**
 * del_user_from_list: delete user from list
 * @user: user of logined
 *
 * Returns 0 means success, or other value means failure.
 */
int del_user_from_list(user_list_t *user)
{
	int is_find = 0;
	user_list_t *tmp, *temp;

	if (!user)
		return -1;

	tmp = NULL;
	temp = s_user_list;
	while (temp) {
		if (temp == user) {
			if (tmp)
				tmp->next = temp->next;
			else
				s_user_list = temp->next;

			is_find = 1;
			break;
		}

		tmp = temp;
		temp = temp->next;
	}

	if (is_find) {
		lb_mem_free(user);
		return 0;
	}

	return -2;
}

/**
 * restful_send_ok_result: send success message to client
 * @nc: client connection's object
 * @json: the result JSON object
 *
 * Returns none.
 */
void restful_send_ok_result(struct mg_connection *nc, cJSON *json, cJSON *arr, cJSON *obj)
{
	char *json_str = NULL;

	if (NULL == nc || !json)
		return;

	mg_printf(nc, "%s", "HTTP/1.1 200\r\nTransfer-Encoding: chunked\r\n\r\n");

	if (obj)
		cJSON_AddItemToArray(arr, obj);

	cJSON_AddItemToObject(json, "data", arr);
	cJSON_AddNumberToObject(json, "success", STATU_OK);
	cJSON_AddStringToObject(json, "msg", "OK");
	json_str = cJSON_PrintUnformatted(json);
	ALOGD("[%d ----> %s]", __LINE__, json_str);
	if (json_str)
		mg_printf_http_chunk(nc, json_str);
	mg_send_http_chunk(nc, "", 0);

	if (json)
		cJSON_Delete(json);
	if (json_str)
		lb_mem_free(json_str);
}

/**
 * restful_send_ok_result: send success message to client
 * @nc: client connection's object
 * @json: the result JSON object
 *
 * Returns none.
 */
int restful_send_ok_result_thread(cJSON *json, int status, http_thread_info *thread_info)
{
	char *json_str = NULL;
	int len = 0, ret = 0;

	if (!json || !thread_info)
		return -1;

	if (!status) {
		cJSON_AddNumberToObject(json, "success", STATU_OK);
		cJSON_AddStringToObject(json, "msg", "OK");
		cJSON_AddStringToObject(json, "sssion_id", thread_info->session_id);
	} else {
		cJSON_AddNumberToObject(json, "success", status);
		cJSON_AddStringToObject(json, "msg", get_err(status));
		cJSON_AddStringToObject(json, "sssion_id", thread_info->session_id);
	}

	json_str = cJSON_PrintUnformatted(json);
	if (json_str)
		len = strlen(json_str);
	printf("url %s\n json %s\n", thread_info->notify_url, json_str);
	ret = send_request(thread_info->notify_url, json_str, len, NULL);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

Exit:
	if (json)
		cJSON_Delete(json);
	if (json_str)
		lb_mem_free(json_str);
	if (thread_info)
		lb_mem_free(thread_info);

	return ret;
}

/**
 * restful_send_fail_result: send failure message to client
 * @nc: client connection's object
 * @json: the result JSON object
 * @arr: error information item
 * @reason: reason of failure
 *
 * Returns none.
 */
void send_fail_result(struct mg_connection *nc, cJSON *json, cJSON *arr, int status)
{
	char *json_str;

	mg_printf(nc, "%s", "HTTP/1.1 206\r\nTransfer-Encoding: chunked\r\n\r\n");

	cJSON_AddItemToObject(json, "data", arr);
	cJSON_AddNumberToObject(json, "success", STATU_FAIL);
	cJSON_AddStringToObject(json, "msg", get_err(status));
	json_str = cJSON_PrintUnformatted(json);
	if (json_str)
		mg_printf_http_chunk(nc, json_str);

	mg_send_http_chunk(nc, "", 0);

	if (json)
		cJSON_Delete(json);
	if (json_str)
		lb_mem_free(json_str);
}


/**
 * check_user_is_validate: check user is validate
 * @u_name: user name
 * @u_password: user password
 *
 * Returns 0 means success, other means failure.
 */
int check_user_is_validate(const char *u_name, const char *u_password)
{
	int ret = -1;
	char bs_username[64] = {0};
	char bs_password[64] = {0};
	ee_queue_t head;
	manager_info_t *q = NULL;

	if (!u_name || !u_password)
		return -1;

	memset(bs_username, 0, sizeof(bs_username));
	memset(bs_password, 0, sizeof(bs_password));
	/* find user from system admin db */
	ret = system_admin_get_info(bs_username, sizeof(bs_username),
		bs_password, sizeof(bs_password));
	if (!ret && !strcmp(u_name, bs_username) && !strcmp(u_password, bs_password))
		return 0;

	memset(&head, 0, sizeof(head));
	/* find user from manager db */
	ret = db_manager_info_query_begin((char *)u_name, &head);
	ALOGD("[check_user_is_validate] u_name %s ret %d\n", u_name, ret);
	if (ret >= 0) {
		ret = -1;
		ee_queue_foreach(q, &head, manager_info_t, queue) {
			if (!strcmp(u_name, q->user_name)
				&& !strcmp(u_password, q->user_password)) {
				ret = 0;
				break;
			}
		}
	}
	db_manager_info_query_end(&head);

	return ret;
}

/**
 * lb_web_login: login
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_login(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char password[64] = {0};
	char session_id[32];
	int ret = -1;

	user_list_t *user;
	cJSON *root, *arr, *obj = NULL;

	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "user_password", password,
			sizeof(password));
		break;

	default:
		break;
	}

	if (chk_str_val(username) || chk_str_val(password)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}

	ret = check_user_is_validate(username, password);
	if (!ret) {
		sprintf(session_id, "%06d", rand() % 1000000);
		user = (user_list_t *)lb_mem_alloc_ext(sizeof(user_list_t),
			__func__, __LINE__);
		if (user == NULL) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, STATU_FAIL);
			goto Exit;
		}

		memset(user, 0, sizeof(user_list_t));
		strcpy(user->user_name, username);
		strcpy(user->session_id, session_id);
		user->begin_time = time(NULL);
		add_user_to_list(user);

		obj = cJSON_CreateObject();
		if (obj)
			cJSON_AddStringToObject(obj, "session_id", session_id);
		restful_send_ok_result(nc, root, arr, obj);
	} else
		send_fail_result(nc, root, arr, USER_NO_VALIDATE);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_logout: logout
 * @nc: client connection's object
 * @ev: event
 * @p: http message content.
 *
 * Returns none.
 */
void lb_web_logout(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	int ret = -1;

	user_list_t *user;
	cJSON *root, *arr, *obj = NULL;

	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		break;

	default:
		break;
	}

	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	user = find_user_from_list(username, session_id);
	ret = del_user_from_list(user);
	if (ret)
		ALOGE("[%s : %d : ret %d]\n", __func__, __LINE__, ret);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_recg_cfg: get/set face recognition's configuration parameter
 * @nc: client connection's object
 * @ev: event
 * @p: http message content.
 *
 * Returns none.
 */
void lb_web_set_recg_cfg(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char buf[64] = {0};
	char username[64] = {0};
	char session_id[64] = {0};
	int ret = -1, lvflg = 0;
	float tmpthv;

	cJSON *root, *arr, *obj = NULL;
	recognition_para_config recg_para;

	/** get data */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "threshold", buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%f", &tmpthv);

		mg_get_http_var(&p_http_msg->body, "livingcheck", buf, sizeof(buf));
		if (!strcmp(buf, "1"))
			lvflg = 1;
		else
			lvflg = 0;
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (tmpthv < 0 || tmpthv > 1)
		tmpthv = 0.5;

	/** set recg config */
	recg_para.live_detect = lvflg;
	recg_para.threshold_score = tmpthv;
	ret = set_recg_cfg(&recg_para);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_recg_cfg: get face recognition's configuration parameter
 * @nc: client connection's object
 * @ev: event
 * @p: http message content.
 *
 * Returns none.
 */
void lb_web_get_recg_cfg(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	int ret = -1;

	cJSON *root, *arr, *obj = NULL;
	recognition_para_config recg_para;

	/** get data */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	/** get recg config */
	memset(&recg_para, 0, sizeof(recognition_para_config));
	ret = get_recg_cfg(&recg_para);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	cJSON_AddNumberToObject(obj, "threshold", recg_para.threshold_score);
	cJSON_AddNumberToObject(obj, "livingcheck", recg_para.live_detect);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

void *add_member_thread(void *arg)
{
	cJSON *root, *arr, *obj = NULL;
	int ret = 0;
	http_thread_info *thread_info = (http_thread_info *)arg;
	face_number_info *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	ret = add_member((face_number_info *)p_info);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		obj = cJSON_CreateObject();
		cJSON_AddStringToObject(obj, "name", p_info->name);
		goto Exit;
	}

Exit:
	cJSON_AddItemToObject(root, "data", arr);
	restful_send_ok_result_thread(root, ret, thread_info);

	if (p_info->file_data)
		lb_mem_free(p_info->file_data);
	if (p_info)
		lb_mem_free(p_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_add_member_info(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, add_member_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_add_member: add member
 * @nc: client connection's object
 * @ev: event
 * @p: http message content.
 *
 * Returns none.
 */
void lb_web_add_member(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64];
	char session_id[64];
	char buf[64] = {0};
	char notify_url[64] = {0};
	char *file_data = NULL;
	int len, ret = 0, add_flag = 1;

	cJSON *root, *arr, *obj = NULL;
	face_number_info *p_info;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	p_info = (face_number_info *)lb_mem_alloc_ext(sizeof(face_number_info),
		__func__, __LINE__);
	if (p_info == NULL || thread_info == NULL || (NULL == nc) || (NULL == p)
		|| create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "employee_number",
			p_info->employee_number, sizeof(p_info->employee_number));
		mg_get_http_var(&p_http_msg->body, "name",
			p_info->name, sizeof(p_info->name));
		mg_get_http_var(&p_http_msg->body, "gender",
			p_info->gender, sizeof(p_info->gender));
		mg_get_http_var(&p_http_msg->body, "nationa",
			p_info->nationa, sizeof(p_info->nationa));
		mg_get_http_var(&p_http_msg->body, "id_card_number",
			p_info->id_card_number, sizeof(p_info->id_card_number));
		mg_get_http_var(&p_http_msg->body, "mobile",
			p_info->mobile, sizeof(p_info->mobile));
		mg_get_http_var(&p_http_msg->body, "notify_url",
			notify_url, sizeof(notify_url));
		if (strlen(notify_url) > 0)
			add_flag = 1;
		else
			add_flag = 0;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "access_right", buf, sizeof(buf));
		if (!strcmp(buf, "0"))
			p_info->access_right = 0;
		else if (!strcmp(buf, "1"))
			p_info->access_right = 1;
		else if (!strcmp(buf, "2"))
			p_info->access_right = 2;
		else
			p_info->access_right = -1;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "temporay_access_start_time",
			buf, sizeof(buf));
		time_str_to_sec(buf, &p_info->temporary_access_start_time);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "temporay_access_end_time",
			buf, sizeof(buf));
		time_str_to_sec(buf, &p_info->temporary_access_end_time);

		len = p_http_msg->body.len;
		file_data = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (file_data == NULL) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, STATU_FAIL);
			goto Exit;
		}
		mg_get_http_var(&p_http_msg->body, "file", file_data, len);
		p_info->file_data = file_data;
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (chk_str_val(p_info->employee_number) || chk_str_val(p_info->name) ||
		chk_str_val(p_info->gender) || chk_str_val(p_info->nationa) ||
		chk_str_val(p_info->id_card_number) || chk_str_val(p_info->mobile)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}

	if (!add_flag) {
		ret = add_member((face_number_info *)p_info);
		printf("add_member(%s) ret %d\n", p_info->employee_number, ret);
		if (ret) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, ret);
			goto Exit;
		} else
			restful_send_ok_result(nc, root, arr, obj);
		goto Exit;
	} else {
		/** synchronous return */
		restful_send_ok_result(nc, root, arr, obj);

		/** begin add member thread */
		thread_info->data = p_info;
		strcpy(thread_info->notify_url, notify_url);
		strcpy(thread_info->session_id, session_id);
		proc_add_member_info(thread_info);
	}
	return;

Exit:
	if (p_info)
		lb_mem_free(p_info);
	if (file_data)
		lb_mem_free(file_data);
	if (thread_info)
		lb_mem_free(thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

void *search_member_thread(void *arg)
{
	cJSON *root, *arr, *obj = NULL;
	int ret = 0, i;
	face_number_pack_info p_pack_info[ONE_PACK_MAX_RECORD_NUM];
	http_thread_info *thread_info = (http_thread_info *)arg;
	face_number_info *p_info = thread_info->data;

	memset(&p_pack_info, 0, ONE_PACK_MAX_RECORD_NUM * sizeof(face_number_pack_info));

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	/** process data */
	ret = search_member(p_info, p_pack_info);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = STATU_FAIL;
		goto Exit;
	}

	/** pack data and send */
	for (i = 0; i < p_info->record_count; i++) {
		obj = cJSON_CreateObject();
		if (!obj) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			ret = ERR_INIT;
			goto Exit;
		}
		cJSON_AddNumberToObject(obj, "id",
				p_pack_info[i].id);
		cJSON_AddStringToObject(obj, "employee_number",
				p_pack_info[i].employee_number);
		cJSON_AddStringToObject(obj, "name",
				p_pack_info[i].name);
		cJSON_AddStringToObject(obj, "gender",
				p_pack_info[i].gender);
		cJSON_AddStringToObject(obj, "nationa",
				p_pack_info[i].nationa);
		cJSON_AddStringToObject(obj, "id_card_number",
				p_pack_info[i].id_card_number);
		cJSON_AddStringToObject(obj, "mobile",
				p_pack_info[i].mobile);
		cJSON_AddStringToObject(obj, "department_name",
				p_pack_info[i].department_name);
		cJSON_AddStringToObject(obj, "temporary_access_start_time",
				p_pack_info[i].temporary_access_start_time);
		cJSON_AddStringToObject(obj, "temporary_access_end_time",
				p_pack_info[i].temporary_access_end_time);
		cJSON_AddItemToArray(arr, obj);
	}
	i--;
	cJSON_AddItemToObject(root, "data", arr);
	cJSON_AddNumberToObject(root, "start_id", p_info->start_id);
	cJSON_AddNumberToObject(root, "record_count", p_info->record_count);
	cJSON_AddNumberToObject(root, "record_total", p_info->record_total);
	cJSON_AddNumberToObject(root, "end_id", p_pack_info[i].id);

Exit:
	if (p_info)
		lb_mem_free(p_info);

	restful_send_ok_result_thread(root, ret, thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_search_member_info(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, search_member_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_search_member: search member
 * @nc: client connection's object
 * @ev: event
 * @p: http message content.
 *
 * Returns none.
 */
void lb_web_search_member(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char notify_url[64] = {0};
	char buf[128] = {0};

	cJSON *root, *arr, *obj = NULL;
	face_number_info *p_info;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	p_info = (face_number_info *)lb_mem_alloc_ext(sizeof(face_number_info),
		__func__, __LINE__);
	if (p_info == NULL || thread_info == NULL || (NULL == nc) || (NULL == p)
		|| create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name",
			username, sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id",
			session_id, sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "employee_number",
			p_info->employee_number,
			sizeof(p_info->employee_number));
		mg_get_http_var(&p_http_msg->body, "name", p_info->name,
			sizeof(p_info->name));

		mg_get_http_var(&p_http_msg->body, "notify_url",
			notify_url, sizeof(notify_url));

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "record_offset",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &p_info->start_id);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "record_count",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &p_info->record_count);
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (chk_str_val(p_info->employee_number) || chk_str_val(p_info->name)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}
	if (p_info->start_id < 0 || p_info->record_count < 0) {
		p_info->start_id = 0;
		p_info->record_count = 0;
	}
	if (p_info->record_count > 20)
		p_info->record_count = 20;

	/** synchronous return */
	restful_send_ok_result(nc, root, arr, obj);

	/** begin search member thread */
	thread_info->data = p_info;
	strcpy(thread_info->notify_url, notify_url);
	strcpy(thread_info->session_id, session_id);
	proc_search_member_info(thread_info);
	return;

Exit:
	if (p_info)
		lb_mem_free(p_info);
	if (thread_info)
		lb_mem_free(thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * lb_web_update_member: update member
 * @nc: client connection's object
 * @ev: event
 * @p: http message content.
 *
 * Returns none.
 */
void lb_web_update_member(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char buf[128] = {0};
	int ret = -1, len;
	char username[32];
	char session_id[32];
	char *face_features = NULL, *face_key_points = NULL;
	cJSON *root, *arr, *obj = NULL;
	face_number_info number_info;

	/** para init */
	memset(&number_info, 0, sizeof(face_number_info));
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get pata data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "employee_number",
			number_info.employee_number,
			sizeof(number_info.employee_number));
		mg_get_http_var(&p_http_msg->body, "name",
			number_info.name,
			sizeof(number_info.name));
		mg_get_http_var(&p_http_msg->body, "gender",
			number_info.gender,
			sizeof(number_info.gender));
		mg_get_http_var(&p_http_msg->body, "nationa",
			number_info.nationa,
			sizeof(number_info.nationa));
		mg_get_http_var(&p_http_msg->body, "id_card_number",
			number_info.id_card_number,
			sizeof(number_info.id_card_number));
		mg_get_http_var(&p_http_msg->body, "mobile",
			number_info.mobile,
			sizeof(number_info.mobile));
		mg_get_http_var(&p_http_msg->body, "access_card_number",
			number_info.access_card_number,
			sizeof(number_info.access_card_number));

		mg_get_http_var(&p_http_msg->body, "access_right",
			buf, sizeof(buf));
		if (!strcmp(buf, "0"))
			number_info.access_right = 0;
		else if (!strcmp(buf, "1"))
			number_info.access_right = 1;
		else if (!strcmp(buf, "2"))
			number_info.access_right = 2;
		else
			number_info.access_right = -1;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "face_norm", buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%lf", &number_info.face_norm[0]);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "face_norm_1", buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%lf", &number_info.face_norm[1]);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "temporay_access_start_time",
			buf, sizeof(buf));
		time_str_to_sec(buf, &number_info.temporary_access_start_time);
		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "temporay_access_end_time",
			buf, sizeof(buf));
		time_str_to_sec(buf, &number_info.temporary_access_end_time);

		len = p_http_msg->body.len;
		face_features = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (face_features == NULL) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, STATU_FAIL);
			goto Exit;
		}
		face_key_points = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (face_key_points == NULL) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, STATU_FAIL);
			goto Exit;
		}
		mg_get_http_var(&p_http_msg->body, "face_features",
			face_features, len);
		number_info.face_features = face_features;
		mg_get_http_var(&p_http_msg->body, "face_key_points",
			face_key_points, len);
		number_info.face_key_points = face_key_points;
		break;
	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (chk_str_val(number_info.employee_number) || chk_str_val(number_info.name) ||
		chk_str_val(number_info.gender) ||
		chk_str_val(number_info.nationa) || chk_str_val(number_info.mobile) ||
		chk_str_val(number_info.id_card_number) ||
		chk_str_val(number_info.access_card_number)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}

	/** dispose data */
	ret = update_member(&number_info);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** send message */
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	if (face_features)
		lb_mem_free(face_features);
	if (face_key_points)
		lb_mem_free(face_key_points);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * lb_web_total_member: total member
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_total_member(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	ALOGD("[%s ----> %d]", __func__, __LINE__);

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	int ret = -1, total;

	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	/** get total number */
	ret = total_number(&total);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack and send */
	obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(obj, "total", total);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

void *del_all_member_thread(void *arg)
{
	int ret = 0;
	cJSON *root, *arr = NULL;
	http_thread_info *thread_info = (http_thread_info *)arg;
	char *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ret = -1;
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

	ret = delete_all_memmbers();
	if (ret) {
		ret = -2;
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

Exit:
	if (p_info)
		lb_mem_free(p_info);

	cJSON_AddItemToObject(root, "data", arr);
	restful_send_ok_result_thread(root, ret, thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

void *del_member_thread(void *arg)
{
	int ret = 0;
	cJSON *root, *arr = NULL;
	http_thread_info *thread_info = (http_thread_info *)arg;
	char *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ret = -1;
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

	ret = delete_member(p_info, arr);
	if (ret) {
		ret = -2;
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

Exit:
	if (p_info)
		lb_mem_free(p_info);

	cJSON_AddItemToObject(root, "data", arr);
	restful_send_ok_result_thread(root, ret, thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_delete_all_member(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, del_all_member_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

int proc_delete_member(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, del_member_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_delete_member: delete member
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_delete_member(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char buf[64] = {0};
	char username[64] = {0};
	char session_id[64] = {0};
	char notify_url[64] = {0};
	char *data = NULL;
	int batch = 0, len;

	cJSON *root, *arr, *obj = NULL;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	if (thread_info == NULL || (NULL == nc) || (NULL == p) ||
		create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		mg_get_http_var(&p_http_msg->body, "notify_url", notify_url,
			sizeof(notify_url));

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "batch",
			buf, sizeof(buf));
		if (!strcmp(buf, "1"))
			batch = 1;
		else
			batch = 0;

		len = p_http_msg->body.len;
		data = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (data == NULL) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, STATU_FAIL);
			goto Exit;
		}
		memset(data, 0, len);
		mg_get_http_var(&p_http_msg->body, "data", data, len);
		break;
	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	/** synchronous return */
	restful_send_ok_result(nc, root, arr, obj);

	/** begin delete members thread */
	if (batch == 1) {
		thread_info->data = data;
		strcpy(thread_info->notify_url, notify_url);
		strcpy(thread_info->session_id, session_id);
		proc_delete_all_member(thread_info);
	} else if (batch == 0) {
		thread_info->data = data;
		strcpy(thread_info->notify_url, notify_url);
		strcpy(thread_info->session_id, session_id);
		proc_delete_member(thread_info);
	}
	return;
Exit:
	if (data)
		lb_mem_free(data);
	if (thread_info)
		lb_mem_free(thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * fill_record_to_json: fill json object of search punch record
 * @obj: json object
 * @q_person: person information
 * @q_record: punch record
 *
 * Returns none.
 */
int fill_record_to_json(cJSON *obj, punch_record_info punch_info)
{
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		return -1;
	}

	cJSON_AddNumberToObject(obj, "id", punch_info.id);
	cJSON_AddNumberToObject(obj, "match_score", punch_info.match_score);
	cJSON_AddStringToObject(obj, "address", punch_info.address);
	cJSON_AddStringToObject(obj, "punch_time", punch_info.punch_time);
	cJSON_AddStringToObject(obj, "punch_live_large_pic_name",
		punch_info.punch_live_large_pic_name);
	cJSON_AddStringToObject(obj, "punch_live_small_pic_name",
		punch_info.punch_live_small_pic_name);
	cJSON_AddNumberToObject(obj, "punch_pass", punch_info.punch_pass);

	return 0;
}

void *search_rec_thread(void *arg)
{
	cJSON *root, *arr, *obj = NULL;
	int ret = 0, i;
	punch_record_info punch_info[25];
	http_thread_info *thread_info = (http_thread_info *)arg;
	query_punch_record_info *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	/** dispose data */
	ret = search_punch_record(p_info, punch_info);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = STATU_FAIL;
		goto Exit;
	}

	/** pack data and send */
	ALOGD("[punch_info[0].record_total : %d]\n", punch_info[0].record_total);
	/** pack data and send */
	for (i = 0; i < p_info->record_count; i++) {
		obj = cJSON_CreateObject();
		if (!obj) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			ret = ERR_INIT;
			goto Exit;
		}
		fill_record_to_json(obj, punch_info[i]);
		cJSON_AddItemToArray(arr, obj);
	}
	i--;
	cJSON_AddItemToObject(root, "data", arr);
	cJSON_AddNumberToObject(root, "start_id", p_info->start_id);
	cJSON_AddNumberToObject(root, "record_count", p_info->record_count);
	cJSON_AddNumberToObject(root, "record_total", punch_info[0].record_total);
	cJSON_AddNumberToObject(root, "end_id", punch_info[i].id);

Exit:
	if (p_info)
		lb_mem_free(p_info);

	restful_send_ok_result_thread(root, ret, thread_info);
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_search_rec_info(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, search_rec_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_search_rec: search punch record
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_search_rec(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	struct http_message *p_http_msg = (struct http_message *)p;
	char buf[128] = {0};
	char username[64] = {0};
	char session_id[64] = {0};
	char notify_url[64] = {0};
	int length;

	cJSON *root, *arr, *obj = NULL;
	query_punch_record_info *p_info;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	length = sizeof(query_punch_record_info);
	p_info = (query_punch_record_info *)lb_mem_alloc_ext(length,
		__func__, __LINE__);
	if (p_info == NULL || thread_info == NULL || (NULL == nc) || (NULL == p)
		|| create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name",
			username, sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id",
			session_id, sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "employee_number",
			p_info->employee_number,
			sizeof(p_info->employee_number));
		mg_get_http_var(&p_http_msg->body, "name",
			p_info->name, sizeof(p_info->name));
		mg_get_http_var(&p_http_msg->body, "notify_url",
			notify_url, sizeof(notify_url));

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "start_punch_time",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			time_str_to_sec(buf, &p_info->start_punch_time);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "end_punch_time",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			time_str_to_sec(buf, &p_info->end_punch_time);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "punch_pass",
			buf, sizeof(buf));
		if (!strcmp(buf, "0"))
			p_info->punch_pass = 0;
		else if (!strcmp(buf, "1"))
			p_info->punch_pass = 1;
		else
			p_info->punch_pass = -1;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "record_offset",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &p_info->start_id);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "record_count",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &p_info->record_count);

		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (chk_str_val(p_info->employee_number) || chk_str_val(p_info->name)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}
	if (p_info->record_count <= 0)
		p_info->record_count = 1;
	if (p_info->record_count > 20)
		p_info->record_count = 20;
	if (p_info->start_id < 0)
		p_info->start_id = 0;

	/** synchronous return */
	restful_send_ok_result(nc, root, arr, obj);

	/** begin add member thread */
	thread_info->data = p_info;
	strcpy(thread_info->notify_url, notify_url);
	strcpy(thread_info->session_id, session_id);
	proc_search_rec_info(thread_info);
	return;

Exit:
	if (p_info)
		lb_mem_free(p_info);
	if (thread_info)
		lb_mem_free(thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * get_file_bin_data: read bin data from file
 * @file_path: filename
 * @pbin: bin data
 * @len: len of bin data
 *
 * Returns 0 means success, or other value means failure.
 */
int get_file_bin_data(char *file_path, unsigned char **pbin, int *len)
{
	FILE *fp = NULL;
	int file_len = 0, read_count = 0, tmp_count = 0, ret = 0, loop_count = 0;

	if (!file_path)
		return -1;
	if (strlen(file_path) < 0)
		return -1;

	ALOGD("[get_file_bin_data] file_path=%s\n", file_path);
	fp = fopen(file_path, "rb");
	if (NULL == fp) {
		ALOGE("fopen %s fail.\n", file_path);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	if (file_len < 0) {
		ALOGE("get file length error.\n");
		ret = -1;
		goto Exit;
	}
	fseek(fp, 0, SEEK_SET);

	*pbin = lb_mem_alloc_ext(file_len, __func__, __LINE__);
	if (NULL == pbin) {
		ALOGE("lb_mem_alloc_ext 1, error.\n");
		ret = -1;
		goto Exit;
	}

	ALOGD("[get_file_bin_data] file_len = %d\n", file_len);
	while (1) {
		tmp_count = fread(*pbin+read_count, 1, 1024, fp);
		read_count += tmp_count;
		if (read_count >= file_len)
			break;
		loop_count++;
		if (loop_count > 10240000) {
			ALOGE("TIMEOUT.\n");
			ret = -1;
			if (*pbin) {
				lb_mem_free(*pbin);
				*pbin = NULL;
			}
			*len = read_count;
			goto Exit;
		}
	}
	*len = read_count;
	ALOGD("file_len=%d, read_count=%d\n", file_len, read_count);
	if (read_count < file_len) {
		ret = -1;
		if (*pbin) {
			lb_mem_free(*pbin);
			*pbin = NULL;
		}
		goto Exit;
	}
Exit:
	if (fp)
		fclose(fp);
	return ret;
}

/**
 * get_file_path_name: get file name from file' url
 * @file_path: file url
 * @file_name: file name
 *
 * Returns 0 means success, or other value means failure.
 */
int get_file_path_name(char *file_path, char **file_name)
{
	char *p = file_path;

	ALOGD("[get_file_path_name]file_path=%s\n", file_path);
	if (NULL == file_path)
		return -1;
	if (strlen(file_path) < 0)
		return -1;
	while ((p = strstr(p, "/"))) {
		*file_name = p + 1;
		p = *file_name;
	}
	ALOGD("[get_file_path_name]picname=%s\n", *file_name);
	return 0;
}

/**
 * fill_export_record_to_json: fill json object of export punch record
 * @obj: json object
 * @q_person: person information
 * @q_record: punch record
 *
 * Returns none.
 */
void fill_export_record_to_json(cJSON *obj, punch_record_info punch_info)
{
	char *pbin = NULL;
	char *base64_str = NULL;
	int rlen = 0, ret = -1;
	int base64_len = 0;

	if (!obj)
		return;

	ALOGD("[%d : fill_export_record_to_json]", __LINE__);
	cJSON_AddNumberToObject(obj, "id", punch_info.id);
	cJSON_AddNumberToObject(obj, "match_score", punch_info.match_score);
	cJSON_AddStringToObject(obj, "address", punch_info.address);
	cJSON_AddStringToObject(obj, "punch_time", punch_info.punch_time);
	cJSON_AddStringToObject(obj, "punch_live_large_pic_name",
		punch_info.punch_live_large_pic_name);
	cJSON_AddStringToObject(obj, "punch_live_small_pic_name",
		punch_info.punch_live_small_pic_name);
	cJSON_AddNumberToObject(obj, "punch_pass", punch_info.punch_pass);

	ALOGD("[%d : fill_export_record_to_json]", __LINE__);
	ret = get_file_bin_data(punch_info.punch_live_large_pic_name,
			(unsigned char **)&pbin, &rlen);
	if (0 == ret && rlen > 0) {
		ret = base64_encode(pbin, rlen, &base64_str, &base64_len);
		if (ret)
			ALOGE("[lb_web_export_rec] base64_encode error!\n");
		cJSON_AddNumberToObject(obj, "base64_pic_len", base64_len);
		cJSON_AddStringToObject(obj, "base64_pic", base64_str);
	}
	ALOGD("[%d : fill_export_record_to_json]", __LINE__);
	if (pbin) {
		lb_mem_free(pbin);
		pbin = NULL;
	}
	if (base64_str) {
		lb_mem_free(base64_str);
		base64_str = NULL;
	}
}

void *export_rec_thread(void *arg)
{
	cJSON *root, *arr, *obj = NULL;
	int ret = 0, i;
	punch_record_info punch_info[25];
	http_thread_info *thread_info = (http_thread_info *)arg;
	query_punch_record_info *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}
	/** dispose data */
	ret = search_punch_record(p_info, punch_info);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = STATU_FAIL;
		goto Exit;
	}

	/** pack data and send */
	ALOGD("[punch_info[0].record_total : %d]\n", punch_info[0].record_total);
	/** pack data and send */
	for (i = 0; i < p_info->record_count; i++) {
		obj = cJSON_CreateObject();
		if (!obj) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			ret = ERR_INIT;
			goto Exit;
		}
		fill_export_record_to_json(obj, punch_info[i]);
		cJSON_AddItemToArray(arr, obj);
	}
	i--;
	cJSON_AddItemToObject(root, "data", arr);
	cJSON_AddNumberToObject(root, "start_id", p_info->start_id);
	cJSON_AddNumberToObject(root, "record_count", p_info->record_count);
	cJSON_AddNumberToObject(root, "record_total", punch_info[0].record_total);
	cJSON_AddNumberToObject(root, "end_id", punch_info[i].id);

Exit:
	if (p_info)
		lb_mem_free(p_info);

	restful_send_ok_result_thread(root, ret, thread_info);
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_export_rec_info(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, export_rec_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_export_rec: export punch record
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_export_rec(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char buf[128] = {0};
	char username[32];
	char session_id[32];
	char notify_url[64] = {0};
	int length;

	cJSON *root, *arr, *obj = NULL;
	query_punch_record_info *p_info;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	length = sizeof(query_punch_record_info);
	p_info = (query_punch_record_info *)lb_mem_alloc_ext(length,
		__func__, __LINE__);
	if (p_info == NULL || thread_info == NULL || (NULL == nc) || (NULL == p)
		|| create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name",
			username, sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id",
			session_id, sizeof(session_id));
		mg_get_http_var(&p_http_msg->body, "notify_url",
			notify_url, sizeof(notify_url));

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "start_punch_time",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			time_str_to_sec(buf, &p_info->start_punch_time);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "end_punch_time",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			time_str_to_sec(buf, &p_info->end_punch_time);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "punch_pass",
			buf, sizeof(buf));
		if (!strcmp(buf, "1"))
			p_info->punch_pass = 1;
		else
			p_info->punch_pass = 0;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "record_offset",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &p_info->start_id);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "record_count",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &p_info->record_count);
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (p_info->record_count <= 0)
		p_info->record_count = 1;
	if (p_info->record_count > 20)
		p_info->record_count = 20;
	if (p_info->start_id < 0)
		p_info->start_id = 0;

	/** synchronous return */
	restful_send_ok_result(nc, root, arr, obj);

	/** begin export record thread */
	thread_info->data = p_info;
	strcpy(thread_info->notify_url, notify_url);
	strcpy(thread_info->session_id, session_id);
	proc_export_rec_info(thread_info);
	return;

Exit:
	if (p_info)
		lb_mem_free(p_info);
	if (thread_info)
		lb_mem_free(thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

void *del_all_record_thread(void *arg)
{
	cJSON *root, *arr = NULL;
	int ret = 0;
	http_thread_info *thread_info = (http_thread_info *)arg;
	query_punch_record_info *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ret = ERR_INIT;
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

	ret = delete_all_punch_record();
	if (ret) {
		ret = STATU_FAIL;
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

Exit:
	if (p_info)
		lb_mem_free(p_info);

	cJSON_AddItemToObject(root, "data", arr);
	restful_send_ok_result_thread(root, ret, thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

void *del_record_thread(void *arg)
{
	cJSON *root, *arr = NULL;
	int ret = 0;
	http_thread_info *thread_info = (http_thread_info *)arg;
	query_punch_record_info *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ret = ERR_INIT;
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

	ret = delete_punch_record(p_info);
	if (ret) {
		ret = STATU_FAIL;
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

Exit:
	if (p_info)
		lb_mem_free(p_info);

	cJSON_AddItemToObject(root, "data", arr);
	restful_send_ok_result_thread(root, ret, thread_info);
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_delete_all_record(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, del_all_record_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

int proc_delete_record(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, del_record_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_export_rec: export punch record
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_delete_rec(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char buf[128] = {0};
	char username[32];
	char session_id[32];
	char notify_url[64] = {0};
	int batch = 0, length;

	cJSON *root, *arr, *obj = NULL;
	query_punch_record_info *p_info;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	length = sizeof(query_punch_record_info);
	p_info = (query_punch_record_info *)lb_mem_alloc_ext(length,
		__func__, __LINE__);
	if (p_info == NULL || thread_info == NULL || (NULL == nc) || (NULL == p)
		|| create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name",
			username, sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id",
			session_id, sizeof(session_id));
		mg_get_http_var(&p_http_msg->body, "notify_url", notify_url,
			sizeof(notify_url));
		mg_get_http_var(&p_http_msg->body, "employee_number",
			p_info->employee_number,
			sizeof(p_info->employee_number));
		mg_get_http_var(&p_http_msg->body, "name", p_info->name,
			sizeof(p_info->name));
		mg_get_http_var(&p_http_msg->body, "gender", p_info->gender,
			sizeof(p_info->gender));
		mg_get_http_var(&p_http_msg->body, "nationa", p_info->nationa,
			sizeof(p_info->nationa));
		mg_get_http_var(&p_http_msg->body, "id_card_number",
			p_info->id_card_number,
			sizeof(p_info->id_card_number));
		mg_get_http_var(&p_http_msg->body, "department_name",
			p_info->department_name,
			sizeof(p_info->department_name));
		mg_get_http_var(&p_http_msg->body, "address",
			p_info->address,
			sizeof(p_info->address));
		mg_get_http_var(&p_http_msg->body, "punch_time",
			p_info->punch_time,
			sizeof(p_info->punch_time));

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "id", buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &p_info->id);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "match_score",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%f", &p_info->match_score);
		else
			sscanf("-1", "%f", &p_info->match_score);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "punch_pass",
			buf, sizeof(buf));
		if (!strcmp(buf, "0"))
			p_info->punch_pass = 0;
		else if (!strcmp(buf, "1"))
			p_info->punch_pass = 1;
		else
			p_info->punch_pass = -1;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "batch", buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &batch);
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (chk_str_val(p_info->employee_number) || chk_str_val(p_info->name) ||
		chk_str_val(p_info->gender) || chk_str_val(p_info->nationa) ||
		chk_str_val(p_info->id_card_number) || chk_str_val(p_info->address) ||
		chk_str_val(p_info->department_name) ||
		chk_str_val(p_info->punch_time)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}
	if (p_info->match_score < 0)
		p_info->match_score = 0;
	if (p_info->id < 0)
		p_info->id = 0;
	if (batch != 0 && batch != 1)
		batch = 0;

	/** synchronous return */
	restful_send_ok_result(nc, root, arr, obj);

	/** begin del record thread */
	if (batch == 1) {
		thread_info->data = p_info;
		strcpy(thread_info->notify_url, notify_url);
		strcpy(thread_info->session_id, session_id);
		proc_delete_all_record(thread_info);
	} else if (batch == 0) {
		thread_info->data = p_info;
		strcpy(thread_info->notify_url, notify_url);
		strcpy(thread_info->session_id, session_id);
		proc_delete_record(thread_info);
	}
	return;

Exit:
	if (p_info)
		lb_mem_free(p_info);
	if (thread_info)
		lb_mem_free(thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * lb_web_get_cloud_platform_param : get cloud platform param
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_cloud_platform_param(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	int ret = -1;

	cJSON *root, *arr, *obj = NULL;
	cloud_platform_config_info cldparam;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get datas */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	/** get cloud platform info */
	memset(&cldparam, 0, sizeof(cloud_platform_config_info));
	ret = get_cloud_platform_info(&cldparam);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	cJSON_AddStringToObject(obj, "ai_server_url", cldparam.ai_server_url);
	cJSON_AddStringToObject(obj, "attendance_url", cldparam.attendance_url);
	cJSON_AddStringToObject(obj, "fota_url", cldparam.fota_url);
	cJSON_AddStringToObject(obj, "cloud_upload_rec_url",
		cldparam.cloud_upload_rec_url);
	cJSON_AddStringToObject(obj, "cloud_import_mems_url",
		cldparam.cloud_import_mems_url);
	cJSON_AddStringToObject(obj, "cloud_import_delete_all_url",
		cldparam.cloud_import_delete_all_url);
	cJSON_AddStringToObject(obj, "cloud_ipaddr_changed_url",
		cldparam.cloud_ipaddr_changed_url);
	cJSON_AddStringToObject(obj, "cloud_import_mems_ret_url",
		cldparam.cloud_import_mems_ret_url);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_cloud_platform_param : set cloud platform param
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_cloud_platform_param(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char ai_server_url[256] = {0};
	char attendance_url[256] = {0};
	char fota_url[256] = {0};
	char cloud_upload_rec_url[256] = {0};
	char cloud_import_mems_url[256] = {0};
	char cloud_import_delete_all_url[256] = {0};
	char cloud_ipaddr_changed_url[256] = {0};
	char cloud_import_mems_ret_url[256] = {0};
	int ret = -1;

	cJSON *root, *arr, *obj = NULL;
	cloud_platform_config_info cldparam;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get datas */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "ai_server_url",
			ai_server_url,
			sizeof(ai_server_url));
		mg_get_http_var(&p_http_msg->body, "attendance_url",
			attendance_url,
			sizeof(attendance_url));
		mg_get_http_var(&p_http_msg->body, "fota_url",
			fota_url,
			sizeof(fota_url));
		mg_get_http_var(&p_http_msg->body, "cloud_upload_rec_url",
			cloud_upload_rec_url,
			sizeof(cloud_upload_rec_url));
		mg_get_http_var(&p_http_msg->body,
			"cloud_import_mems_url",
			cloud_import_mems_url,
			sizeof(cloud_import_mems_url));
		mg_get_http_var(&p_http_msg->body,
			"cloud_import_delete_all_url",
			cloud_import_delete_all_url,
			sizeof(cloud_import_delete_all_url));
		mg_get_http_var(&p_http_msg->body,
			"cloud_ipaddr_changed_url",
			cloud_ipaddr_changed_url,
			sizeof(cloud_ipaddr_changed_url));
		mg_get_http_var(&p_http_msg->body,
			"cloud_import_mems_ret_url",
			cloud_import_mems_ret_url,
			sizeof(cloud_import_mems_ret_url));
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (chk_str_val(ai_server_url) || chk_str_val(attendance_url) ||
		chk_str_val(fota_url) || chk_str_val(cloud_upload_rec_url) ||
		chk_str_val(cloud_import_mems_url)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}

	/** get old cloud platform info */
	memset(&cldparam, 0, sizeof(cloud_platform_config_info));
	ret = get_cloud_platform_info(&cldparam);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	if (strlen(ai_server_url) > 0)
		strcpy(cldparam.ai_server_url, ai_server_url);
	if (strlen(attendance_url) > 0)
		strcpy(cldparam.attendance_url, attendance_url);
	if (strlen(fota_url) > 0)
		strcpy(cldparam.fota_url, fota_url);
	if (strlen(cloud_upload_rec_url) > 0)
		strcpy(cldparam.cloud_upload_rec_url, cloud_upload_rec_url);
	if (strlen(cloud_import_mems_url) > 0)
		strcpy(cldparam.cloud_import_mems_url, cloud_import_mems_url);
	if (strlen(cloud_import_delete_all_url) > 0)
		strcpy(cldparam.cloud_import_delete_all_url,
			cloud_import_delete_all_url);
	if (strlen(cloud_ipaddr_changed_url) > 0)
		strcpy(cldparam.cloud_ipaddr_changed_url,
			cloud_ipaddr_changed_url);
	if (strlen(cloud_import_mems_ret_url) > 0)
		strcpy(cldparam.cloud_import_mems_ret_url,
			cloud_import_mems_ret_url);

	/** set new cloud platform */
	ret = set_cloud_platform_info(&cldparam);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** synchronous return */
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_pic_quality: get picture quality
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_pic_quality(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64];
	char session_id[64];
	char *file_data = NULL;
	float quality = 0.0;
	int len = 0, ret = 0;

	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		len = p_http_msg->body.len;
		file_data = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (file_data == NULL) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, ERR_INIT);
			goto Exit;
		}

		memset(file_data, 0, len);
		mg_get_http_var(&p_http_msg->body, "file",
			file_data, len);
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	ret = get_pic_quality(file_data, &quality);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ret);
		goto Exit;
	}

	obj = cJSON_CreateObject();
	if (obj)
		cJSON_AddNumberToObject(obj, "face_quality", quality);
	restful_send_ok_result(nc, root, arr, obj);

Exit:

	if (file_data)
		lb_mem_free(file_data);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * lb_web_get_feature: get face's feature
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_feature(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64];
	char session_id[64];
	char *file_data = NULL;
	char *b64_feature = NULL;
	char *b64_key_points = NULL;
	int len = 0, ret = -1;
	float norm[2];

	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		len = p_http_msg->body.len;
		file_data = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (file_data == NULL) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, STATU_FAIL);
			goto Exit;
		}

		memset(file_data, 0, len);
		mg_get_http_var(&p_http_msg->body, "file", file_data, len);
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	/** get face feature */
	ret = get_face_feature_http(file_data, &b64_feature, &b64_key_points, norm);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ret);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	cJSON_AddStringToObject(obj, "face_features", b64_feature);
	cJSON_AddStringToObject(obj, "face_key_points", b64_key_points);
	cJSON_AddNumberToObject(obj, "face_norm", norm[0]);
	cJSON_AddNumberToObject(obj, "face_norm_1", norm[1]);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	if (b64_feature)
		lb_mem_free(b64_feature);
	if (b64_key_points)
		lb_mem_free(b64_key_points);
	if (file_data)
		lb_mem_free(file_data);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * lb_web_snapshot: snapshot
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_snapshot(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64];
	char session_id[64];
	char *base64_data = NULL;
	int ret = -1;

	cJSON *root, *arr, *obj = NULL;

	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	/** take photo */
	ret = take_photo_http(&base64_data);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	cJSON_AddStringToObject(obj, "file", base64_data);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	if (base64_data)
		lb_mem_free(base64_data);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * lb_web_add_member_by_feature: add memmber by face's feature
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_add_member_by_feature(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64];
	char session_id[64];
	char buf[64] = {0};
	char *file_data;
	int len, ret = 0;

	cJSON *root, *arr, *obj = NULL;
	face_number_info p_info;

	/** para init */
	memset(&p_info, 0, sizeof(face_number_info));
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "employee_number",
			p_info.employee_number, sizeof(p_info.employee_number));
		mg_get_http_var(&p_http_msg->body, "name",
			p_info.name, sizeof(p_info.name));
		mg_get_http_var(&p_http_msg->body, "gender",
			p_info.gender, sizeof(p_info.gender));
		mg_get_http_var(&p_http_msg->body, "nationa",
			p_info.nationa, sizeof(p_info.nationa));
		mg_get_http_var(&p_http_msg->body, "id_card_number",
			p_info.id_card_number, sizeof(p_info.id_card_number));
		mg_get_http_var(&p_http_msg->body, "mobile",
			p_info.mobile, sizeof(p_info.mobile));

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "access_right",
			buf, sizeof(buf));
		if (!strcmp(buf, "0"))
			p_info.access_right = 0;
		else if (!strcmp(buf, "1"))
			p_info.access_right = 1;
		else if (!strcmp(buf, "2"))
			p_info.access_right = 2;
		else
			p_info.access_right = -1;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body,
			"temporay_access_start_time",
			buf, sizeof(buf));
		time_str_to_sec(buf, &p_info.temporary_access_start_time);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "temporay_access_end_time",
			buf, sizeof(buf));
		time_str_to_sec(buf, &p_info.temporary_access_end_time);

		len = p_http_msg->body.len;
		file_data = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (file_data == NULL) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			goto Exit;
		}
		mg_get_http_var(&p_http_msg->body, "file",
			file_data, len);
		p_info.file_data = file_data;

		break;
	default:
		break;
	}

	/** para check */
	if (chk_str_val(p_info.employee_number) || chk_str_val(p_info.name) ||
		chk_str_val(p_info.gender) || chk_str_val(p_info.nationa) ||
		chk_str_val(p_info.id_card_number) || chk_str_val(p_info.mobile)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}

	/** add member */
	ret = add_member(&p_info);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ret);
		goto Exit;
	}

	/** pack data and send */
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

#if 0
void *batch_import_members_thread(void *arg)
{
	cJSON *root, *arr, *obj = NULL;
	int ret = 0;
	char employee_number[64];
	http_thread_info *thread_info = (http_thread_info *)arg;
	char *data = thread_info->data;

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	obj = cJSON_CreateObject();
	ret = batch_import_members(data, obj);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		cJSON_AddStringToObject(obj, "employee_number", employee_number);
		cJSON_AddStringToObject(obj, "err_reason", get_err(ret));
		ret = STATU_FAIL;
		goto Exit;
	}

Exit:
	cJSON_AddItemToArray(arr, obj);
	cJSON_AddItemToObject(root, "data", arr);
	restful_send_ok_result_thread(root, ret, thread_info);

	if (data)
		lb_mem_free(data);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_batch_import_members(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, batch_import_members_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_batch_import_members: batch import members
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
/** / */
void lb_web_batch_import_members(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64];
	char session_id[64];
	char notify_url[64] = {0};
	char *data;
	int ret = 0, len;

	cJSON *root, *arr, *obj = NULL;
	user_list_t *user = NULL;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	if (thread_info == NULL || (NULL == nc) || (NULL == p)
		|| create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		mg_get_http_var(&p_http_msg->body, "notify_url", notify_url,
			sizeof(notify_url));

		len = p_http_msg->body.len;
		data = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (!data) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, ERR_INIT);
			goto Exit;
		}

		memset(data, 0, len);
		mg_get_http_var(&p_http_msg->body, "B66B", data, len);
		break;
	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	/** synchronous return */
	restful_send_ok_result(nc, root, arr, obj);

	/** begin batch import members thread */
	thread_info->data = data;
	strcpy(thread_info->notify_url, notify_url);
	strcpy(thread_info->session_id, session_id);
	proc_batch_import_members(thread_info);
	return;

Exit:
	if (data) {
		lb_mem_free(data);
		data = NULL;
	}

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}
#else
/**
 * lb_web_batch_import_members: batch import members
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
/** / */
void lb_web_batch_import_members(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64];
	char session_id[64];
	char notify_url[64] = {0};
	char *data = NULL;
	int ret = 0, len;

	cJSON *root, *arr, *obj = NULL;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	if (thread_info == NULL || (NULL == nc) || (NULL == p)
		|| create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		mg_get_http_var(&p_http_msg->body, "notify_url", notify_url,
			sizeof(notify_url));

		len = p_http_msg->body.len;
		data = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (!data) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, ERR_INIT);
			goto Exit;
		}

		memset(data, 0, len);
		mg_get_http_var(&p_http_msg->body, "B66B", data, len);
		break;
	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	ret = batch_import_members(data, arr);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = STATU_FAIL;
		send_fail_result(nc, root, arr, ret);
		goto Exit;
	}

	/** synchronous return */
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	if (data) {
		lb_mem_free(data);
		data = NULL;
	}

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}
#endif
#endif
