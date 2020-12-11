/*
* ws_svc_main.cpp - define interface function of websocket  by cpp
*
* Copyright (C) 2019, LomboTech Co.Ltd.
* Author: lomboswer <lomboswer@lombotech.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License alon
* with this program; if not, write to the Free Software Foundation, Inc.
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#define LOG_TAG "ws_svc_main"
#include <log/log.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>

#include "ws_control.h"
#include "ws_svc.h"
#include "cJSON.h"
#include "ws_core_log.h"
#include "qrencode_svc.h"
#include "ws_svc_main.h"
#include "md5.h"
#include "file_ops.h"
#include "system_info.h"
#include "face.h"
#include "screen_size.h"


#define MAX_FACE_NUM 2
#define MAX_CONTACT_NUM 199
#define USERINFO_NUM_PER_TRANSLATE 100
#define VISITOR_NUM_PER_TRANSLATE 100
/*req len in bin for wechat*/
#define REQ_ID_LEN 128
#define NONCE_LEN 6
#define MAX_DIFF_SECOND 5
#define PACKET_PATH "/cache/package.efw"
#define VERSION_LENGTH 32
#define LOG_PATH "/data/log/system.log"
#define OTA_PATH "/data/ota"
#define LOG_INTER_SEC ((3600*3))
/*errcode of wechat*/
#define ERROR_CODE_REGISTERED 600002
#define ERROR_CODE_INVALID_ACTIVECODE 600003
#define ERROR_CODE_NOT_AUTHOR 600004
#define ERROR_CODE_KEEP_ALIVE_REGISTERED 600012
#define ERROR_CODE_NOT_REGISTERED 600014
#define ERROR_CODE_NOT_ACTIVE 600016
#define ERROR_CODE_NOT_LOGIN 600020
#define ERROR_CODE_SUBSCRIBED 600045
#define ERROR_CODE_ACTIVED 600053
#define ERROR_CODE_HAS_SUBSCRIBED 600054
typedef enum {
	RECG_OK = 0,
	RECG_PIC_NO_HAVE,
	RECG_PIC_DECODE_FAIL,
	RECG_PIC_NO_DETECT_FACE,
	RECG_PIC_MULTIPLE_FACE,
	RECG_FACE_SIZE_ERROR,
	RECG_PIC_QUALITY_TOO_LOW,
} e_recg_error_code;

typedef struct tag_person_reg_error_info_t {
	e_recg_error_code error_code;
	const char *error_msg;
} recg_error_info_t;

recg_error_info_t st_recg_info[] = {
	{RECG_OK,			"OK"},
	{RECG_PIC_NO_HAVE,		"No picture"},
	{RECG_PIC_DECODE_FAIL,		"Image decoding failed"},
	{RECG_PIC_NO_DETECT_FACE,	"Unable to detect face"},
	{RECG_PIC_MULTIPLE_FACE,	"Checked multiple faces"},
	{RECG_FACE_SIZE_ERROR,		"Checked face size"},
	{RECG_PIC_QUALITY_TOO_LOW,	"Face quality is too low"}
};

struct req_id_t {
	char *id;
	ws_recv_cb fn;
};

struct push_cmd_t {
	char *cmd;
	ws_recv_cb fn;
};

/*register status*/
typedef enum {
	STATUS_DEFAULT = 0,
	STATUS_SYNC,
	STATUS_SECRET_NO,
	STATUS_REGISTER,
	STATUS_ACTIVE,
	STATUS_SUBSCRIBE,
	STATUS_SUCCESS,
	STATUS_IDLE
} register_status;

/*picture recg status*/
typedef enum {
	user_status_default = 0,
	user_status_down_photo,
	user_status_down_photo_cb,
	user_status_calc_feats,
	user_status_upload,
	user_status_upload_cb
} user_status_t;

/*face feats*/
typedef union {
	_feat_local_t feat;
	char fa_data[FA_DATA_SIZE];
} _feat_data_t;

/*recg with picture*/
typedef struct us_data {
	struct list_head node;
	int id;
	int photo_size;
	char oper_id[128];
	char media_id[128];
	int media_type;
	_feat_data_t data;
	char path[128];
	char face_feat_base64[2048];
	char userid[64];
	char req_id[32];
	int user_type;
	int index;
	int error;
	user_status_t status;
} user_data_t;

typedef struct {
	int id;
	int photo_size;
	char media_id[128];
	char media_type[16];
	_feat_data_t data;
	char path[128];
	char face_feat_base64[2048];
} user_face_t;

typedef struct {
	char oper_id[128];
	char userid[64];
	int user_type;
	user_face_t face[MAX_FACE_NUM];
	int face_count;
	int index;
	int error;
} user_faces_t;
/**
 * firmware info for ota
 */
typedef struct {
	int is_download;
	char media_id[128];
	int size;
	char md5sum[64];
	char download_url[256];
	char firmware_version[VERSION_LENGTH];
	pthread_t id;
} firmware_info_t;
/**
 * logfile info
 */
#pragma pack(1)
typedef struct {
	int format_verison;
	int cmd_len;
	char cmd[10];
	int req_id_len;
	char req_id[13];
	int data_len;
} log_file_info_t;
#pragma pack()

typedef struct {
	websocket_client_param_t param;
	/*exit flag*/
	int websocket_exit;
	/*for register */
	register_status status;
	register_status status_last;
	int register_thread_is_run;
	pthread_t register_thread_id;
	/*checkint */
	pthread_mutex_t checkin_lock;
	int checkin_send;
	ee_queue_t *h_checkin;
	ee_queue_t head_checkin;
	time_t  last_time;
	/*face feats*/
	pthread_mutex_t face_down_lock;
	pthread_cond_t face_down_cond;
	user_faces_t faces;
	int face_thread_is_run;
	/*for take photo register*/
	int face_photo_thread;
	int face_photo_exit_flag;
	/*face feats for mul person*/
	struct list_head user_data_list;
	unsigned int user_num;
	pthread_mutex_t user_data_lock;
	pthread_t user_data_thread_id;
	/*for photo local*/
	user_faces_t face_photo;

	/*visitor*/
	int visitor_packet_count;
	ee_queue_t head_visitor_person;
	ee_queue_t *h_visitor_person;
	ee_queue_t head_visitor_rule;
	ee_queue_t *h_visitor_rule;

	/*userinfo*/
	int userinfo_packet_count;
	int userinfo_perm_version;
	ee_queue_t head_userinfo_person;
	ee_queue_t *h_userinfo_person;
	ee_queue_t head_userinfo_rule;
	ee_queue_t *h_userinfo_rule;
	int userinfo_recving;

	/*userinfo ids*/
	char **userid_ids;
	unsigned int total_user_num_ids;
	unsigned int current_index_ids;
	ee_queue_t head_userinfo_person_ids;
	ee_queue_t *h_userinfo_person_ids;
	ee_queue_t head_userinfo_rule_ids;
	ee_queue_t *h_userinfo_rule_ids;
	unsigned int version_ids;
	int userinfo_send_ids;

	/*links status*/
	int link_status;
	int link_wechat;
	int link_discount;

	/*call back for signal*/
	ws_svc_call_back cb_fun;
	/*health status*/
	int health_status;
	/*update firmware*/
	firmware_info_t firmware_info;
	/*thread for log push*/
	pthread_t log_thread_id;
	int log_run;

} websocket_client_data_t;

static websocket_client_data_t *websocket_data;

/**
 * @brief  create dir
 * @note   NULL
 * @retval 0 success
 */
static int ws_app_create_dir(const char *path)
{
	if (access(path, R_OK | W_OK | X_OK) == 0)
		return 0;

	return mkdir(path, 0755);
}
/**
 * @brief  get perm version
 * @note   NULL
 * @retval 0 success
 */
int ws_app_get_perm_version(void)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_perm_version");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_perm_version");

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  get userinfo by page
 * @note   limit max is 200
 * @param  offset:
 * @param  limit:
 * @retval 0 success
 */
int ws_app_get_userinfo_by_page(int offset, int limit)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;

	if (websocket_data->userinfo_recving && !offset) {
		ALOGE("%s: userinfo_recving =%d\n", __func__,
			  websocket_data->userinfo_recving);
		return -1;
	}
	if (websocket_data->userinfo_recving == 0)
		websocket_data->userinfo_recving = 1;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_userinfo_by_page");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(
		headers,
		"req_id",
		"lb_get_userinfo_by_page");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddNumberToObject(body, "offset", offset);
	cJSON_AddNumberToObject(body, "limit", limit);

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}


/**
 * @brief by page cb
 * @note  is the last  insert all data to db
 * otherwise storage the data to queue and get next page
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_get_userinfo_by_page_cb(unsigned char *data,
			size_t len, unsigned char flags) {

	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *user_info_array = NULL;
	cJSON *user_info = NULL;
#if 0
	cJSON *pass_rule_array = NULL;
	cJSON *pass_rule = NULL;
	web_socket_rule_info_t *rule_info = NULL;
	int size_rule = 0;
	char *rule = NULL;
	/*
	int id = 0;
	int effect_time = 0;
	*/
#endif
	cJSON *fa_array = NULL;
	cJSON *fa = NULL;

	websocket_person_info_t *person_info = NULL;

	int size = 0;
	int size_fa = 0;
	char *userid = NULL;
	char *name = NULL;
	int openvid = 0;
	int user_type = 0;
	char *datax = NULL;
	if (websocket_data->userinfo_packet_count == 0) {
#if 0
		if (ee_queue_empty(h) == 0 || ee_queue_empty(h_rule) == 0) {
			ws_control_userinfo_free(h, h_rule);
		}
#endif
		ee_queue_init(websocket_data->h_userinfo_person);
		ee_queue_init(websocket_data->h_userinfo_rule);
	}
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		ret = -1;
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		ret = -1;
		goto error;
	}
	if (websocket_data->userinfo_packet_count == 0) {
		websocket_data->userinfo_perm_version =
			cJSON_GetObjectItem(body, "perm_version")->valueint;
	} else {
		if (websocket_data->userinfo_perm_version !=
			cJSON_GetObjectItem(
				body,
				"perm_version")->valueint) {
			ret = -2;
			goto error;
		}
	}
	user_info_array = cJSON_GetObjectItem(body, "userinfo");
	if (user_info_array == NULL) {
		ret = -1;
		goto error;
	}
	size = cJSON_GetArraySize(user_info_array);
	for (int i = 0; i < size; i++) {
		user_info = cJSON_GetArrayItem(user_info_array, i);
		/* userid */
		userid = cJSON_GetObjectItem(user_info, "userid")->valuestring;

		/* openvid */
		openvid = cJSON_GetObjectItem(user_info, "openvid")->valueint;
		/* name */
		name = cJSON_GetObjectItem(user_info, "name")->valuestring;
		/* user_type */
		user_type = cJSON_GetObjectItem(user_info, "user_type")->valueint;
		/* fa */
		fa_array = cJSON_GetObjectItem(user_info, "fa_list");
		if (fa_array == NULL) {
			ret = -1;
			goto error;
		}
		/* malloc */
		person_info = calloc(sizeof(websocket_person_info_t), 1);
		if (person_info == NULL) {
			ret = -1;
			goto error;
		}
		strncpy(person_info->user_id, userid, 64);
		strncpy(person_info->name, name, 32);
		person_info->openvid = openvid;
		person_info->user_type = user_type;

		size_fa = cJSON_GetArraySize(fa_array);
		for (int j = 0; j < size_fa; j++) {
			fa = cJSON_GetArrayItem(fa_array, j);
			person_info->face_id = cJSON_GetObjectItem(fa, "id")->valueint;
			datax = cJSON_GetObjectItem(fa, "data")->valuestring;
			ws_control_fadata_2_algdata(datax, person_info);
		}
		ee_queue_insert_tail(websocket_data->h_userinfo_person,
							 &(person_info->queue));
#if 0
		/* rule arry */
		pass_rule_array = cJSON_GetObjectItem(user_info, "pass_rule_list");
		if (NULL == pass_rule_array) {
			ret = -1;
			goto error;
		}
		size_rule = cJSON_GetArraySize(pass_rule_array);
		for (int k = 0;
			 k < size_rule;
			 k++) {
			rule_info = calloc(sizeof(web_socket_rule_info_t), 1);
			if (rule_info == NULL) {
				ret = -1;
				goto error;
			}
			strncpy(rule_info->user_id, userid, 64);
			pass_rule = cJSON_GetArrayItem(pass_rule_array, k);
			rule_info->rule_id = cJSON_GetObjectItem(pass_rule, "id")->valueint;
			rule = cJSON_GetObjectItem(pass_rule, "rule")->valuestring;
			strncpy(rule_info->pass_rule, rule, 64);
			rule_info->effect_time = cJSON_GetObjectItem(pass_rule,
			"effect_time")->valueint;
			/* insert rule */
			ee_queue_insert_tail(h_rule, &(rule_info->queue));
		}
#endif
	}

	if (cJSON_GetObjectItem(body, "is_last")->valueint == 1) {
		websocket_data->userinfo_recving = 0;
		websocket_data->userinfo_packet_count = 0;
		ws_control_userinfo_update(websocket_data->h_userinfo_person,
		websocket_data->h_userinfo_rule);
		ws_control_userinfo_free(websocket_data->h_userinfo_person,
		websocket_data->h_userinfo_rule);
		ws_control_set_perm_version(
				websocket_data->userinfo_perm_version);
	} else {
		websocket_data->userinfo_packet_count++;
		ws_app_get_userinfo_by_page(USERINFO_NUM_PER_TRANSLATE *
		websocket_data->userinfo_packet_count,
		USERINFO_NUM_PER_TRANSLATE);
	}

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	if (ret == -1) {
		websocket_data->userinfo_packet_count = 0;
		ws_control_userinfo_free(websocket_data->h_userinfo_person,
		websocket_data->h_userinfo_rule);
	} else if (ret == -2) {
		/*  perm version diffrent */
		websocket_data->userinfo_packet_count = 0;
		ws_control_userinfo_free(websocket_data->h_userinfo_person,
		websocket_data->h_userinfo_rule);
		websocket_data->userinfo_recving = 0;
		ws_app_get_userinfo_by_page(0, USERINFO_NUM_PER_TRANSLATE);
	}

	return ret;
}

/**
 * @brief
 * @note
 * @param  **user:
 * @param  count:
 * @retval
 */
int ws_app_get_userinfo_by_ids_v1(char **user, int count)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *array = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_userinfo_by_ids");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_userinfo_by_ids");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	array = cJSON_CreateStringArray((const char **)user, count);
	if (NULL == array) {
		ALOGE("array not exist\n");
		goto error;
	}
	cJSON_AddItemToObject(body, "userids", array);

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_get_userinfo_by_ids_cb_v1(
	unsigned char *data,
	size_t len,
	unsigned char flags)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *user_info_array = NULL;
	cJSON *user_info = NULL;

	cJSON *fa_array = NULL;
	cJSON *fa = NULL;
	websocket_person_info_t *person_info = NULL;
	int size = 0;
	int size_fa = 0;
	char *userid = NULL;
	char *name = NULL;
	int openvid = 0;
	int user_type = 0;
	char *datax = NULL;
	unsigned int number = 0;
	if (websocket_data->current_index_ids == 1) {
		ee_queue_init(websocket_data->h_userinfo_person_ids);
		ee_queue_init(websocket_data->h_userinfo_rule_ids);
	}
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		ret = -1;
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		ret = -1;
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		ret = -1;
		goto error;
	}
	user_info_array = cJSON_GetObjectItem(body, "userinfo");
	if (user_info_array == NULL) {
		ret = -1;
		goto error;
	}
	size = cJSON_GetArraySize(user_info_array);
	for (int i = 0; i < size; i++) {
		user_info = cJSON_GetArrayItem(user_info_array, i);
		/* userid */
		userid = cJSON_GetObjectItem(user_info, "userid")->valuestring;

		/* openvid */
		openvid = cJSON_GetObjectItem(user_info, "openvid")->valueint;
		/*  name*/
		name = cJSON_GetObjectItem(user_info, "name")->valuestring;

		/* user_type */
		user_type = cJSON_GetObjectItem(user_info, "user_type")->valueint;
		/* fa */
		fa_array = cJSON_GetObjectItem(user_info, "fa_list");
		if (fa_array == NULL) {
			ret = -1;
			goto error;
		}
		/* malloc */
		person_info = calloc(sizeof(websocket_person_info_t), 1);
		if (person_info == NULL) {
			ret = -1;
			goto error;
		}
		strncpy(person_info->user_id, userid, 64);
		strncpy(person_info->name, name, 32);
		person_info->openvid = openvid;
		person_info->user_type = user_type;

		size_fa = cJSON_GetArraySize(fa_array);
		for (int j = 0; j < size_fa; j++) {
			fa = cJSON_GetArrayItem(fa_array, j);
			person_info->face_id = cJSON_GetObjectItem(fa, "id")->valueint;
			datax = cJSON_GetObjectItem(fa, "data")->valuestring;
			ws_control_fadata_2_algdata(datax, person_info);
		}

		ee_queue_insert_tail(
			websocket_data->h_userinfo_person_ids,
			&(person_info->queue));
#if 0
		/* rule arry */
		pass_rule_array = cJSON_GetObjectItem(user_info, "pass_rule_list");
		if (NULL == pass_rule_array) {
			ret = -1;
			goto error;
		}
		size_rule = cJSON_GetArraySize(pass_rule_array);
		for (int k = 0;
			 k < size_rule;
			 k++) {
			rule_info = calloc(sizeof(web_socket_rule_info_t), 1);
			if (rule_info == NULL) {
				ret = -1;
				goto error;
			}
			strncpy(rule_info->user_id, userid, 64);
			pass_rule = cJSON_GetArrayItem(pass_rule_array, k);
			rule_info->rule_id = cJSON_GetObjectItem(pass_rule, "id")->valuein
			rule = cJSON_GetObjectItem(pass_rule, "rule")->valuestring;
			strncpy(rule_info->pass_rule, rule, 64);
			/*
			rule_info->effect_time = cJSON_GetObjectItem(pass_rule, "effect_ti
			insert rule
			*/
			ee_queue_insert_tail(h_rule, &(rule_info->queue));
		}
#endif
	}
	/*is complete*/
	if (websocket_data->current_index_ids * USERINFO_NUM_PER_TRANSLATE >=
			websocket_data->total_user_num_ids) {
		ws_control_userinfo_update_ids(
			websocket_data->h_userinfo_person_ids,
			websocket_data->h_userinfo_rule_ids);
		ws_control_userinfo_free(
			websocket_data->h_userinfo_person_ids,
			websocket_data->h_userinfo_rule_ids);

		ws_control_set_perm_version(websocket_data->version_ids);

		/*free */
		if (websocket_data->userid_ids) {
			for (int i = 0; i < websocket_data->total_user_num_ids; i++) {
				if (websocket_data->userid_ids[i])
					free(websocket_data->userid_ids[i]);
			}
			free(websocket_data->userid_ids);
			websocket_data->total_user_num_ids = 0;
			websocket_data->current_index_ids = 0;
		}
	} else {
		number = websocket_data->total_user_num_ids -
			USERINFO_NUM_PER_TRANSLATE * websocket_data->current_index_ids;
		if (number >= USERINFO_NUM_PER_TRANSLATE) {
			ws_app_get_userinfo_by_ids_v1(
				&websocket_data->userid_ids[
				USERINFO_NUM_PER_TRANSLATE *
				websocket_data->current_index_ids],
				USERINFO_NUM_PER_TRANSLATE);
		} else {
			ws_app_get_userinfo_by_ids_v1(
				&websocket_data->userid_ids[
				USERINFO_NUM_PER_TRANSLATE *
				websocket_data->current_index_ids],
				number);
		}
		websocket_data->current_index_ids++;
	}

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	if (ret == -1) {
		ws_control_userinfo_free(
			websocket_data->h_userinfo_person_ids,
			websocket_data->h_userinfo_rule_ids);
		if (websocket_data->userid_ids) {
			for (int i = 0; i < websocket_data->total_user_num_ids; i++) {
				if (websocket_data->userid_ids[i])
					free(websocket_data->userid_ids[i]);
			}
			free(websocket_data->userid_ids);
			websocket_data->total_user_num_ids = 0;
			websocket_data->current_index_ids = 0;
		}
	}

	return ret;
}

/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_push_change_contact_v1(
	unsigned char *data,
	size_t len,
	unsigned char flags)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *create_user_array = NULL;
	cJSON *update_user_array = NULL;
	cJSON *delete_user_array = NULL;
	cJSON *userid = NULL;

	int size = 0;
	int version = 0;
	char *userid_ids = NULL;
	char *userid_char = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	version = cJSON_GetObjectItem(body, "perm_version")->valueint;

	if (version - ws_control_get_perm_version() >= 2) {
		/**/
		ws_app_get_userinfo_by_page(0, USERINFO_NUM_PER_TRANSLATE);
		goto error;
	}
	websocket_data->version_ids = version;
	create_user_array = cJSON_GetObjectItem(body, "create_user");
	if (create_user_array != NULL) {
		size = cJSON_GetArraySize(create_user_array);
		websocket_data->userid_ids = malloc(sizeof(char *) * size);
		if (NULL == websocket_data->userid_ids) {
			ALOGE("userid_ids is NULL\n");
			goto error;
		}
		memset(websocket_data->userid_ids, 0, sizeof(char *) * size);
		for (int i = 0; i < size; i++) {
			userid = cJSON_GetArrayItem(create_user_array, i);
			userid_ids = cJSON_GetObjectItem(userid, "userid")->valuestring;
			websocket_data->userid_ids[i] = malloc(strlen(userid_ids) + 1);
			if (NULL == userid_char) {
				ret = -2;
				ALOGE("userid_char is NULL\n");
				goto error;
			}
			memset(websocket_data->userid_ids[i],
				   0, strlen(userid_ids) + 1);
			strncpy(websocket_data->userid_ids[i],
					userid_ids, strlen(userid_ids));
			websocket_data->total_user_num_ids++;
		}
	}
	update_user_array = cJSON_GetObjectItem(body, "update_user");
	if (update_user_array != NULL) {
		size = cJSON_GetArraySize(update_user_array);
		for (int i = 0; i < size; i++) {
			userid = cJSON_GetArrayItem(update_user_array, i);
			userid_ids = cJSON_GetObjectItem(userid, "userid")->valuestring;
			websocket_data->userid_ids[i] = malloc(strlen(userid_ids) + 1);
			if (NULL == userid_char) {
				ret = -2;
				ALOGE("userid_char is NULL\n");
				goto error;
			}
			memset(websocket_data->userid_ids[i],
				   0, strlen(userid_ids) + 1);
			strncpy(websocket_data->userid_ids[i],
					userid_ids, strlen(userid_ids));
			websocket_data->total_user_num_ids++;
		}
	}
	delete_user_array = cJSON_GetObjectItem(body, "delete_user");
	if (delete_user_array != NULL) {
		size = cJSON_GetArraySize(delete_user_array);
		for (int i = 0; i < size; i++) {
			userid = cJSON_GetArrayItem(delete_user_array, i);
			ws_control_userinfo_delete_by_id(
				cJSON_GetObjectItem(userid, "userid")->valuestring);
		}
	}
	/* send get_userinfo_by_ids command */
	if (websocket_data->total_user_num_ids) {
		websocket_data->current_index_ids++;
		if (websocket_data->total_user_num_ids >= USERINFO_NUM_PER_TRANSLATE) {
			ws_app_get_userinfo_by_ids_v1(
				websocket_data->userid_ids,
				USERINFO_NUM_PER_TRANSLATE);
		} else {
			ws_app_get_userinfo_by_ids_v1(
				websocket_data->userid_ids,
				websocket_data->total_user_num_ids);
		}
	}

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	if (ret == -2) {
		/*free */
		if (websocket_data->userid_ids) {
			for (int i = 0; i < websocket_data->total_user_num_ids; i++) {
				if (websocket_data->userid_ids[i])
					free(websocket_data->userid_ids[i]);
			}
			free(websocket_data->userid_ids);
			websocket_data->total_user_num_ids = 0;
			websocket_data->current_index_ids = 0;
		}
	}
	return ret;
}
/**
 * @brief  get userinfo by ids
 * @note   called by push contact  cmd
 * @param  **user:
 * @param  count:
 * @retval 0 success
 */
int ws_app_get_userinfo_by_ids(char **user, int count)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *array = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_userinfo_by_ids");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_userinfo_by_ids");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	array = cJSON_CreateStringArray((const char **)user, count);
	if (NULL == array) {
		goto error;
	}
	cJSON_AddItemToObject(body, "userids", array);

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  get perm version cb
 * @note   call get userinfo by page when version = 0
 * or version D-value >= 1
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_get_perm_version_cb(unsigned char *data,
			size_t len, unsigned char flags)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;

	/* char *req_id = NULL; */
	int errcode = -1;
	char *errmsg = NULL;
	int version = 0;
	int local_version = 0;

	ALOGD("get_perm_version enter\n");

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	/* req_id = cJSON_GetObjectItem(headers, "req_id")->valuestring; */
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	version = cJSON_GetObjectItem(body, "perm_version")->valueint;

	local_version = ws_control_get_perm_version();
	ALOGD("version = %d local_version = %d,%d\n",
		  version, local_version, version - local_version);
	if (version - local_version >= 1 || local_version == 0)
		ws_app_get_userinfo_by_page(0, USERINFO_NUM_PER_TRANSLATE);
	ALOGD("get_perm_version exit\n");

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}


/**
 * @brief get userinfo by ids cb
 * @note  pefer to get userinfo by page
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_get_userinfo_by_ids_cb(unsigned char *data,
			size_t len, unsigned char flags) {

	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *user_info_array = NULL;
	cJSON *user_info = NULL;
#if 0
	cJSON *pass_rule_array = NULL;
	cJSON *pass_rule = NULL;
	web_socket_rule_info_t *rule_info = NULL;
	int size_rule = 0;
	char *rule = NULL;
	/*
	int id = 0;
	int effect_time = 0;
	*/
#endif
	cJSON *fa_array = NULL;
	cJSON *fa = NULL;
	websocket_person_info_t *person_info = NULL;
	int size = 0;
	int size_fa = 0;
	char *userid = NULL;
	char *name = NULL;
	int openvid = 0;
	int user_type = 0;
	char *datax = NULL;

	ee_queue_t head;
	ee_queue_t *h = &head;

	ee_queue_t head1;
	ee_queue_t *h_rule = &head1;

	ee_queue_init(h);
	ee_queue_init(h_rule);
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	user_info_array = cJSON_GetObjectItem(body, "userinfo");
	if (user_info_array == NULL) {
		goto error;
	}
	size = cJSON_GetArraySize(user_info_array);
	for (int i = 0; i < size; i++) {
		user_info = cJSON_GetArrayItem(user_info_array, i);
		/* userid */
		userid = cJSON_GetObjectItem(user_info, "userid")->valuestring;

		/* openvid */
		openvid = cJSON_GetObjectItem(user_info, "openvid")->valueint;
		//  name
		name = cJSON_GetObjectItem(user_info, "name")->valuestring;

		/* user_type */
		user_type = cJSON_GetObjectItem(user_info, "user_type")->valueint;
		/* fa */
		fa_array = cJSON_GetObjectItem(user_info, "fa_list");
		if (fa_array == NULL) {
			ret = -1;
			goto error;
		}
		/* malloc */
		person_info = calloc(sizeof(websocket_person_info_t), 1);
		if (person_info == NULL) {
			ret = -1;
			goto error;
		}
		strncpy(person_info->user_id, userid, 64);
		strncpy(person_info->name, name, 32);
		person_info->openvid = openvid;
		person_info->user_type = user_type;

		size_fa = cJSON_GetArraySize(fa_array);
		for (int j = 0; j < size_fa; j++) {
			fa = cJSON_GetArrayItem(fa_array, j);
			person_info->face_id = cJSON_GetObjectItem(fa, "id")->valueint;
			datax = cJSON_GetObjectItem(fa, "data")->valuestring;
			ws_control_fadata_2_algdata(datax, person_info);
		}

		ee_queue_insert_tail(h, &(person_info->queue));
#if 0
		/* rule arry */
		pass_rule_array = cJSON_GetObjectItem(user_info, "pass_rule_list");
		if (NULL == pass_rule_array) {
			ret = -1;
			goto error;
		}
		size_rule = cJSON_GetArraySize(pass_rule_array);
		for (int k = 0;
			 k < size_rule;
			 k++) {
			rule_info = calloc(sizeof(web_socket_rule_info_t), 1);
			if (rule_info == NULL) {
				ret = -1;
				goto error;
			}
			strncpy(rule_info->user_id, userid, 64);
			pass_rule = cJSON_GetArrayItem(pass_rule_array, k);
			rule_info->rule_id = cJSON_GetObjectItem(pass_rule, "id")->valuein
			rule = cJSON_GetObjectItem(pass_rule, "rule")->valuestring;
			strncpy(rule_info->pass_rule, rule, 64);
			/*
			rule_info->effect_time = cJSON_GetObjectItem(pass_rule, "effect_ti
			insert rule
			*/
			ee_queue_insert_tail(h_rule, &(rule_info->queue));
		}
#endif
	}
	ws_control_userinfo_update_ids(h, h_rule);
	ws_control_userinfo_free(h, h_rule);
	/*update version*/
	ws_control_set_perm_version(
				websocket_data->userinfo_perm_version);
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	if (ret == -1) {
		ws_control_userinfo_free(h, h_rule);
	}

	return ret;
}

/**
 * @brief push change contact
 * @note   wechat push this cmd when userinfo change
 * for example:recg face add person and del person  and update person;
 * if delete person proc immediately; if update and add person ,will call
 * get userinfo by ids command;
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 if success
 */
int ws_app_push_change_contact(unsigned char *data,
			size_t len, unsigned char flags)
{
	int ret = -1;
	int current_version = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *create_user_array = NULL;
	cJSON *update_user_array = NULL;
	cJSON *delete_user_array = NULL;
	cJSON *userid = NULL;
	int size = 0;
	int version = 0;
	int count = 0;
	char *user[MAX_CONTACT_NUM] = { NULL };

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}

	version = cJSON_GetObjectItem(body, "perm_version")->valueint;
	current_version = ws_control_get_perm_version();
	ALOGE("%s: v=%d cur_v=%d diff =%d\n",
			  __func__,
			  version,
			  current_version,
			  version - current_version);
	/**/
	if (websocket_data->userinfo_recving) {
		ALOGE("%s: is recving\n", __func__);
		goto error;
	}
	if (version - current_version >= 2) {
		/*  */
		ws_app_get_userinfo_by_page(0, USERINFO_NUM_PER_TRANSLATE);
		goto error;
	}
	if (version - current_version <= 0) {
		ALOGE("%s:version not right\n", __func__);
		goto error;
	}
	create_user_array = cJSON_GetObjectItem(body, "create_user");
	if (create_user_array != NULL) {
		size = cJSON_GetArraySize(create_user_array);
		for (int i = 0; i < size; i++) {
			userid = cJSON_GetArrayItem(create_user_array, i);
			user[count++] = cJSON_GetObjectItem(userid, "userid")->valuestring;
			if (count >= MAX_CONTACT_NUM) {
				ws_app_get_userinfo_by_page(0,
					 USERINFO_NUM_PER_TRANSLATE);
				goto error;
			}
		}
	}
	update_user_array = cJSON_GetObjectItem(body, "update_user");
	if (update_user_array != NULL) {
		size = cJSON_GetArraySize(update_user_array);
		for (int i = 0; i < size; i++) {
			userid = cJSON_GetArrayItem(update_user_array, i);
			user[count++] = cJSON_GetObjectItem(userid, "userid")->valuestring;
			if (count >= MAX_CONTACT_NUM) {
				ws_app_get_userinfo_by_page(0,
					 USERINFO_NUM_PER_TRANSLATE);
				goto error;
			}
		}
	}
	delete_user_array = cJSON_GetObjectItem(body, "delete_user");
	if (delete_user_array != NULL) {
		size = cJSON_GetArraySize(delete_user_array);
		for (int i = 0; i < size; i++) {
			userid = cJSON_GetArrayItem(delete_user_array, i);
			ws_control_userinfo_delete_by_id(
				cJSON_GetObjectItem(userid, "userid")->valuestring);
		}
	}

	/* send get_userinfo_by_ids command */
	if (count == 0)
		ws_control_set_perm_version(version);
	if (count != 0) {
		websocket_data->userinfo_perm_version = version;
		ws_app_get_userinfo_by_ids(user, count);
	}
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	return ret;
}

/**
 * @brief  get visitor list
 * @note   not support by wechat
 * @param  offset:
 * @param  limit:
 * @retval 0 success
 */
int ws_app_get_visitor_list(int offset, int limit)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_visitor_list");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_visitor_list");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);

	cJSON_AddNumberToObject(root, "offset", offset);
	cJSON_AddNumberToObject(root, "limit", limit);

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  visitor list cb
 * @note   trigger by get visitor
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_get_visitor_list_cb(unsigned char *data,
				size_t len, unsigned char flags)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *user_info_array = NULL;
	cJSON *user_info = NULL;
#if 0
	cJSON *pass_rule_array = NULL;
	cJSON *pass_rule = NULL;
	web_socket_rule_info_t *rule_info = NULL;
	int size_rule = 0;
	char *rule = NULL;
	/*
	int id = 0;
	int effect_time = 0;
	*/
#endif
	cJSON *fa_array = NULL;
	cJSON *fa = NULL;

	websocket_person_info_t *person_info = NULL;

	int size = 0;
	int size_fa = 0;
	char *userid = NULL;
	char *name = NULL;
	int openvid = 0;
	int user_type = 0;
	char *datax = NULL;

	if (websocket_data->visitor_packet_count == 0) {
		if (ee_queue_empty(websocket_data->h_visitor_person) == 0 ||
			ee_queue_empty(websocket_data->h_visitor_rule) == 0) {
			ws_control_visitor_free(websocket_data->h_visitor_person,
					websocket_data->h_visitor_rule);
		}
		ee_queue_init(websocket_data->h_visitor_person);
		ee_queue_init(websocket_data->h_visitor_rule);
	}
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		ret = -1;
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		ret = -1;
		goto error;
	}
	user_info_array = cJSON_GetObjectItem(body, "userinfo");
	if (user_info_array == NULL) {
		ret = -1;
		goto error;
	}
	size = cJSON_GetArraySize(user_info_array);
	for (int i = 0; i < size; i++) {
		user_info = cJSON_GetArrayItem(user_info_array, i);
		/* userid */
		userid = cJSON_GetObjectItem(user_info, "userid")->valuestring;

		/* openvid */
		openvid = cJSON_GetObjectItem(user_info, "openvid")->valueint;
		/* name */
		name = cJSON_GetObjectItem(user_info, "name")->valuestring;

		/* user_type */
		user_type = cJSON_GetObjectItem(user_info, "user_type")->valueint;
		/* fa */
		fa_array = cJSON_GetObjectItem(user_info, "fa_list");
		if (fa_array == NULL) {
			ret = -1;
			goto error;
		}
		/* malloc */
		person_info = calloc(sizeof(websocket_person_info_t), 1);
		if (person_info == NULL) {
			ret = -1;
			goto error;
		}
		strncpy(person_info->user_id, userid, 64);
		strncpy(person_info->name, name, 32);
		person_info->openvid = openvid;
		person_info->user_type = user_type;

		size_fa = cJSON_GetArraySize(fa_array);
		for (int j = 0; j < size_fa; j++) {
			fa = cJSON_GetArrayItem(fa_array, j);
			person_info->face_id = cJSON_GetObjectItem(fa, "id")->valueint;
			datax = cJSON_GetObjectItem(fa, "data")->valuestring;
			ws_control_fadata_2_algdata(datax, person_info);
		}
		ee_queue_insert_tail(websocket_data->h_visitor_person,
							 &person_info->queue);
#if 0
		/* rule arry */
		pass_rule_array = cJSON_GetObjectItem(user_info, "pass_rule_list");
		if (NULL == pass_rule_array) {
			ret = -1;
			goto error;
		}
		size_rule = cJSON_GetArraySize(pass_rule_array);
		for (int k = 0;
			 k < size_rule;
			 k++) {
			rule_info = calloc(sizeof(web_socket_rule_info_t), 1);
			if (rule_info == NULL) {
				ret = -1;
				goto error;
			}
			strncpy(rule_info->user_id, userid, 64);
			pass_rule = cJSON_GetArrayItem(pass_rule_array, k);
			rule_info->rule_id = cJSON_GetObjectItem(pass_rule, "id")->valuein
			rule = cJSON_GetObjectItem(pass_rule, "rule")->valuestring;
			strncpy(rule_info->pass_rule, rule, 64);
			/*
			rule_info->effect_time = cJSON_GetObjectItem(pass_rule, "effect_ti
			insert rule
			*/
			ee_queue_insert_tail(h_rule, &(rule_info->queue));
		}
#endif
	}

	if (cJSON_GetObjectItem(body, "is_last")->valueint == 1) {
		websocket_data->visitor_packet_count = 0;
		ws_control_visitor_update(websocket_data->h_visitor_person,
websocket_data->h_visitor_rule);
		ws_control_visitor_free(websocket_data->h_visitor_person,
websocket_data->h_visitor_rule);

	} else {
		websocket_data->visitor_packet_count++;
		ws_app_get_visitor_list(VISITOR_NUM_PER_TRANSLATE *
websocket_data->visitor_packet_count, VISITOR_NUM_PER_TRANSLATE);
	}
error:

	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	if (ret < 0) {
		websocket_data->visitor_packet_count = 0;
		ws_control_visitor_free(websocket_data->h_visitor_person,
				websocket_data->h_visitor_rule);
	}

	return ret;
}
/**
 * @brief  visitor list change
 * @note   trigger when visitor list change  in wechat
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_push_visitor_list_change(
	unsigned char *data,
	size_t len,
	unsigned char flags)
{
	ws_app_get_visitor_list(0, VISITOR_NUM_PER_TRANSLATE);

	return 0;
}

/*face feats*/

/**
 * @brief upload userinfo
 * @note is abort
 * @retval 0 success
 */
static int ws_app_upload_userinfo(void)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *userinfo_array = NULL;
	cJSON *userinfo = NULL;
	cJSON *fa_array = NULL;
	cJSON *fa = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "upload_userinfo");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_upload_userinfo");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "oper_id",
							websocket_data->faces.oper_id);
	cJSON_AddNumberToObject(body, "errcode", websocket_data->faces.error);
	pthread_mutex_lock(&websocket_data->face_down_lock);
	if (websocket_data->faces.error == 0) {
		cJSON_AddStringToObject(body, "errmsg", "ok");
		userinfo_array = cJSON_CreateArray();
		if (NULL == userinfo_array) {
			ALOGE("malloc userinfo failed, errno: %d (%s)\n",
				  errno, strerror(errno));
			goto error;
		}
		cJSON_AddItemToObject(body, "userinfo", userinfo_array);
		userinfo = cJSON_CreateObject();
		if (NULL == userinfo) {
			ALOGE("malloc userinfo failed, errno: %d (%s)\n",
				  errno, strerror(errno));
			goto error;
		}
		cJSON_AddItemToArray(userinfo_array, userinfo);

		cJSON_AddStringToObject(userinfo, "userid",
				websocket_data->faces.userid);
		cJSON_AddNumberToObject(userinfo, "user_type",
				websocket_data->faces.user_type);

		fa_array = cJSON_CreateArray();
		if (NULL == fa_array) {
			ALOGE("malloc fa_array failed, errno: %d (%s)\n",
				  errno, strerror(errno));
			goto error;
		}
		cJSON_AddItemToObject(userinfo, "fa_list", fa_array);

		for (int i = 0; i < websocket_data->faces.face_count; i++) {
			fa = cJSON_CreateObject();
			if (NULL == fa) {
				ALOGE("malloc fa failed, errno: %d (%s)\n",
						errno, strerror(errno));
				goto error;
			}
			cJSON_AddItemToArray(fa_array, fa);

			cJSON_AddNumberToObject(fa, "id",
						websocket_data->faces.face[i].id);
			cJSON_AddStringToObject(fa, "data",
				websocket_data->faces.face[i].face_feat_base64);
		}
	} else {
		cJSON_AddStringToObject(body, "errmsg", st_recg_info[1].error_msg);
	}
	pthread_mutex_unlock(&websocket_data->face_down_lock);

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  upload userinfo
 * @note   call after calc picture feats
 * @param  *user_data:
 * @retval 0 success
 */
static int ws_app_upload_userinfo_v1(user_data_t *user_data)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *userinfo_array = NULL;
	cJSON *userinfo = NULL;
	cJSON *fa_array = NULL;
	cJSON *fa = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "upload_userinfo");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_upload_userinfo");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "oper_id", user_data->oper_id);
	cJSON_AddNumberToObject(body, "errcode", user_data->error);

	userinfo_array = cJSON_CreateArray();
	if (NULL == userinfo_array) {
		ALOGE("malloc userinfo failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(body, "userinfo", userinfo_array);
	userinfo = cJSON_CreateObject();
	if (NULL == userinfo) {
		ALOGE("malloc userinfo failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -2;
		goto error;
	}
	cJSON_AddItemToArray(userinfo_array, userinfo);
	cJSON_AddStringToObject(userinfo, "userid", user_data->userid);
	cJSON_AddNumberToObject(userinfo, "user_type", user_data->user_type);
	fa_array = cJSON_CreateArray();
	if (NULL == fa_array) {
		ALOGE("malloc fa_array failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -3;
		goto error;
	}
	cJSON_AddItemToObject(userinfo, "fa_list", fa_array);

	if (user_data->error == 0) {

		cJSON_AddStringToObject(body, "errmsg", "ok");

		fa = cJSON_CreateObject();
		if (NULL == fa) {
			ALOGE("malloc fa failed, errno: %d (%s)\n",
				  errno, strerror(errno));
			ret = -1;
			goto error;
		}
		cJSON_AddItemToArray(fa_array, fa);
		cJSON_AddNumberToObject(fa, "id", user_data->id);
		cJSON_AddStringToObject(fa, "data", user_data->face_feat_base64);
	} else {
		if (user_data->error > 0 && user_data->error < 7)
			cJSON_AddStringToObject(body, "errmsg",
				st_recg_info[user_data->error].error_msg);
		else
			cJSON_AddStringToObject(body, "errmsg", "unknown error");
	}

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:

	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief upload userinfo  cb
 * @note  delete data form list and update userinfo in db
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_upload_userinfo_cb(unsigned char *data,
				size_t len, unsigned char flags)
{
	cJSON *root = NULL;
	cJSON *headers = NULL;
	/*
	int ret = 0;
	cJSON *body = NULL;
	cJSON *userinfo_array = NULL;
	cJSON *userinfo = NULL;
	char *userid = NULL;
	char *name = NULL;
	char *req_id = NULL;
	int size = 0;
	*/
	int errcode = -1;
	char *errmsg = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	/* req_id = cJSON_GetObjectItem(headers, "req_id")->valuestring; */
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode || websocket_data->faces.error != 0) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}

	ws_control_userinfo_update_one(websocket_data->faces.userid,
			websocket_data->faces.user_type,
			websocket_data->faces.face[0].data.fa_data);
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return 0;
}

/**
 * @brief download photo
 * @note  trigger when push user_face
 * @param  *media_id:
 * @retval 0 success
 */
static int ws_app_download_photo_v1(char *media_id)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "download_photo");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", media_id);
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "media_id", media_id);
	cJSON_AddNumberToObject(body, "format_version", 1);
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:

	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief upload_userinfo_cb
 * @note   delete userinfo from list and update db
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_upload_userinfo_cb_v1(unsigned char *data,
							  size_t len, unsigned char flags)
{
	user_data_t *da, *da_tmp;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	int errcode = -1;
	char *errmsg = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	/*update db*/
	pthread_mutex_lock(&websocket_data->user_data_lock);
	list_for_each_entry_safe(da, da_tmp, &websocket_data->user_data_list, node)
	{
			/*calc status*/
			if (da->status  == user_status_upload_cb) {
				if (da->error == 0)
					ws_control_userinfo_update_one(
						da->userid,
						da->user_type,
						da->data.fa_data);
				list_del(&da->node);
				free(da);
				websocket_data->user_num--;
			}
	}
	pthread_mutex_unlock(&websocket_data->user_data_lock);

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return 0;
}

/**
 * @brief download_photo
 * @note abort
 * @param  *media_id:
 * @param  *media_type:
 * @retval
 */
static int ws_app_download_photo(char *media_id, char *media_type)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "download_photo");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_download_photo");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "media_id", media_id);
	cJSON_AddNumberToObject(body, "format_version", 1);
	/*
	cJSON_AddNumberToObject(body, "offset", offset);
	cJSON_AddNumberToObject(body, "limit", limit);
	*/

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief download_photo_cb
 * @note  abort
 * @param  *data:
 * @param  len:
 * @retval 0 success
 */
int ws_app_download_photo_cb(unsigned char *data, size_t len)
{
	int index = 0;

	ALOGD("ws_app_download_photo_cb enter\n");
	pthread_mutex_lock(&websocket_data->face_down_lock);
	index = websocket_data->faces.index;
	snprintf(websocket_data->faces.face[index].path,
			 sizeof(websocket_data->faces.face[index].path),
			 "/tmp/%s.jpg", websocket_data->faces.face[index].media_id);
	ALOGD("recg %s\n", websocket_data->faces.face[index].path);
	ws_control_save_file(websocket_data->faces.face[index].path,
						 (char *)data, len);
	pthread_cond_signal(&websocket_data->face_down_cond);
	pthread_mutex_unlock(&websocket_data->face_down_lock);
	ALOGD("ws_app_download_photo_cb exit\n");

	return 0;
}

/**
 * @brief  calc picture feats thread
 * @note   abort
 * @param  *arg:
 * @retval None
 */
static void *thread_user_face(void *arg)
{
	int ret = 0;
	struct timeval now;
	struct timespec out_time;
	prctl(PR_SET_NAME, "user_face");

	ALOGD("thread_user_face enter \n");
	for (int i = 0; i < websocket_data->faces.face_count; i++) {
		websocket_data->faces.index = i;
		ws_app_download_photo(websocket_data->faces.face[i].media_id,
				websocket_data->faces.face[i].media_type);
		/*  wait for signal */
		gettimeofday(&now, NULL);
		out_time.tv_sec = now.tv_sec + 20;
		out_time.tv_nsec = now.tv_usec * 1000;
		pthread_mutex_lock(&websocket_data->face_down_lock);
		ret = pthread_cond_timedwait(&websocket_data->face_down_cond,
					&websocket_data->face_down_lock,
					&out_time);
		if (ret == 0) {
			/*  calc feats */
			ret = ws_control_calc_feat_from_picture(
				&websocket_data->faces.face[i].data.feat,
				websocket_data->faces.face[i].path);
			if (ret == 0) {
				ret = ws_control_algdata_2_fadata(websocket_data->faces.
					face[i].data.fa_data,
					websocket_data->faces.face[i].face_feat_base64);
			}
			websocket_data->faces.error = ret;
			/* delete file */
			ws_control_delete_file(websocket_data->faces.face[i].path);
		}
		pthread_mutex_unlock(&websocket_data->face_down_lock);
	}
	ALOGD("thread_user_face exit\n");
	ws_app_upload_userinfo();
	websocket_data->face_thread_is_run = 0;

	return NULL;
}

/**
 * @brief calc picture feats thread
 * @note  read data from list and calc feats
 * and trigger upload user operation
 * @retval None
 */
static void *thread_user_face_v1()
{
	int ret = 0;
	int flag = 0;
	user_data_t *data, *data_tmp;
	prctl(PR_SET_NAME, "face_v1");
#if 0
	struct timeval now;
	struct timespec out_time;
#endif

	websocket_data->face_thread_is_run = 1;
	while (websocket_data->face_thread_is_run) {
		pthread_mutex_lock(&websocket_data->user_data_lock);
		if (websocket_data->user_num == 0) {
			pthread_mutex_unlock(&websocket_data->user_data_lock);
			usleep(10);
			continue;
		}
		list_for_each_entry_safe(data, data_tmp,
				&websocket_data->user_data_list,
				node)
		{
			/*calc status*/
			if (data->status  == user_status_calc_feats) {
				list_del(&data->node);
				websocket_data->user_num--;
				flag = 1;
				break;
			}
		}
		pthread_mutex_unlock(&websocket_data->user_data_lock);
		/*calc fetures*/
		if (flag) {
			flag = 0;
			ret = ws_control_calc_feat_from_picture(
				&data->data.feat,
				data->path);
			if (ret == 0) {
				ret = ws_control_algdata_2_fadata(data->data.fa_data,
				data->face_feat_base64);
			}
			data->error = ret;
			/*  delete file */
			ws_control_delete_file(data->path);
			/*add to list*/
			pthread_mutex_lock(&websocket_data->user_data_lock);
			list_add(&data->node, &websocket_data->user_data_list);
			websocket_data->user_num++;
			/*upload*/
			ws_app_upload_userinfo_v1(data);
			data->status = user_status_upload_cb;
			pthread_mutex_unlock(&websocket_data->user_data_lock);
		}
		usleep(10*1000);
	}
	/*free*/
	pthread_mutex_lock(&websocket_data->user_data_lock);
	list_for_each_entry_safe(data, data_tmp,
							 &websocket_data->user_data_list,
							 node)
	{
		list_del(&data->node);
		free(data);
		websocket_data->user_num--;
	}
	pthread_mutex_unlock(&websocket_data->user_data_lock);

	return NULL;
}

/**
 * @brief  user_face
 * @note   abort
 * @retval
 */
static int ws_app_user_face()
{
	pthread_t face_down_thrd;
	ALOGD("ws_app_user_face enter\n");

	if (websocket_data->face_thread_is_run == 1)
		return 0;
	websocket_data->face_thread_is_run = 1;
	pthread_create(&face_down_thrd, NULL, thread_user_face, NULL);
	pthread_detach(face_down_thrd);
	ALOGD("ws_app_user_face exit\n");

	return 0;
}

/**
 * @brief  push user face
 * @note   trigger recg face by picture
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_push_user_face_v1(unsigned char *data,
						  size_t len, unsigned char flags)
{
	cJSON *root = NULL;
	cJSON *body = NULL;
	cJSON *face_info_array = NULL;
	cJSON *face_info = NULL;
	int size = 0;
	user_data_t *user_data = NULL;

	user_data = calloc(sizeof(user_data_t), 1);
	if (NULL == user_data) {
		ALOGE("calloc user_data failed\n");
		goto error;
	}
	ALOGD("ws_app_push_user_face enter\n");
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	strncpy(user_data->oper_id,
			cJSON_GetObjectItem(body, "oper_id")->valuestring, 128);
	strncpy(user_data->userid,
			cJSON_GetObjectItem(body, "userid")->valuestring, 64);
	user_data->user_type =
		cJSON_GetObjectItem(body, "user_type")->valueint;
	face_info_array = cJSON_GetObjectItem(body, "face_info");
	if (NULL == face_info_array)
		goto error;
	size = cJSON_GetArraySize(face_info_array);
	if (size >= MAX_FACE_NUM)
		size = MAX_FACE_NUM;
	for (int i = 0; i < size; i++) {
		face_info = cJSON_GetArrayItem(face_info_array, i);
		if (NULL == face_info)
			goto error;
		user_data->id =
			cJSON_GetObjectItem(face_info, "id")->valueint;
		user_data->photo_size =
			cJSON_GetObjectItem(face_info, "photo_size")->valueint;
		strncpy(user_data->media_id,
cJSON_GetObjectItem(face_info, "media_id")->valuestring, 128);
		if (strcmp("image/jpeg",
		cJSON_GetObjectItem(face_info, "media_type")->valuestring))
			user_data->media_type = 1; /*jpg*/
		else
			user_data->media_type = 2; /*png*/
	}
	user_data->status = user_status_down_photo;
	/*add user data to list*/
	pthread_mutex_lock(&websocket_data->user_data_lock);
	list_add(&user_data->node, &websocket_data->user_data_list);
	websocket_data->user_num++;
	ws_app_download_photo_v1(user_data->media_id);
	user_data->status = user_status_down_photo_cb;
	pthread_mutex_unlock(&websocket_data->user_data_lock);

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	ALOGD("ws_app_push_user_face exit\n");

	return 0;
}

/**
 * @brief  push_user_face
 * @note   abort
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_push_user_face(unsigned char *data,
				size_t len, unsigned char flags)
{
	cJSON *root = NULL;
	cJSON *body = NULL;
	cJSON *face_info_array = NULL;
	cJSON *face_info = NULL;
	int size = 0;

	memset(&websocket_data->faces, 0, sizeof(user_faces_t));
	ALOGD("ws_app_push_user_face enter\n");
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	strncpy(websocket_data->faces.oper_id,
			cJSON_GetObjectItem(body, "oper_id")->valuestring, 128);
	strncpy(websocket_data->faces.userid,
			cJSON_GetObjectItem(body, "userid")->valuestring, 64);
	websocket_data->faces.user_type =
		cJSON_GetObjectItem(body, "user_type")->valueint;
	face_info_array = cJSON_GetObjectItem(body, "face_info");
	if (NULL == face_info_array) {
		goto error;
	}
	size = cJSON_GetArraySize(face_info_array);
	if (size >= MAX_FACE_NUM) {
		size = MAX_FACE_NUM;
	}
	for (int i = 0; i < size; i++) {
		face_info = cJSON_GetArrayItem(face_info_array, i);
		if (NULL == face_info) {
			goto error;
		}
		websocket_data->faces.face[i].id =
			cJSON_GetObjectItem(face_info, "id")->valueint;
		websocket_data->faces.face[i].photo_size =
			cJSON_GetObjectItem(face_info, "photo_size")->valueint;
		strncpy(websocket_data->faces.face[i].media_id,
cJSON_GetObjectItem(face_info, "media_id")->valuestring, 128);
		strncpy(websocket_data->faces.face[i].media_type,
cJSON_GetObjectItem(face_info, "media_type")->valuestring, 64);
		websocket_data->faces.face_count++;
	}
	ws_app_user_face();
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	ALOGD("ws_app_push_user_face exit\n");

	return 0;
}

/**
 * @brief upload_userinfo_photo_error
 * @note  recg face by take photo when camera is used
 * will call this interface
 * @param  *oper_id:
 * @param  *userid:
 * @param  error:
 * @retval 0 success
 */
static int ws_app_upload_userinfo_photo_error(
	char *oper_id,
	char *userid,
	int error)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *userinfo_array = NULL;
	cJSON *userinfo = NULL;
	cJSON *fa_array = NULL;
#if 0
	cJSON *fa = NULL;
#endif
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "upload_userinfo");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_upload_userinfo_photo_e");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "oper_id", oper_id);
	cJSON_AddNumberToObject(body, "errcode", error);
	cJSON_AddStringToObject(body, "errmsg", "camera is using");
	/*add 1 userid*/
	userinfo_array = cJSON_CreateArray();
	if (NULL == userinfo_array) {
		ALOGE("malloc userinfo failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(body, "userinfo", userinfo_array);
	userinfo = cJSON_CreateObject();
	if (NULL == userinfo) {
		ALOGE("malloc userinfo failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToArray(userinfo_array, userinfo);
	cJSON_AddStringToObject(userinfo, "userid", userid);
	cJSON_AddNumberToObject(userinfo, "user_type", 0);
	fa_array = cJSON_CreateArray();
	if (NULL == fa_array) {
		ALOGE("malloc fa_array failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(userinfo, "fa_list", fa_array);
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));
error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}
/**
 * @brief  upload_userinfo_photo
 * @note   be called when recg face by take photo
 * @retval 0 success
 */
static int ws_app_upload_userinfo_photo(void)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *userinfo_array = NULL;
	cJSON *userinfo = NULL;
	cJSON *fa_array = NULL;
	cJSON *fa = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "upload_userinfo");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_upload_userinfo_photo");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "oper_id", websocket_data->face_photo.oper_id);
	cJSON_AddNumberToObject(body, "errcode", websocket_data->face_photo.error);
	userinfo_array = cJSON_CreateArray();
	if (NULL == userinfo_array) {
		ALOGE("malloc userinfo failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(body, "userinfo", userinfo_array);
	userinfo = cJSON_CreateObject();
	if (NULL == userinfo) {
		ALOGE("malloc userinfo failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToArray(userinfo_array, userinfo);
	cJSON_AddStringToObject(userinfo, "userid",
			websocket_data->face_photo.userid);
	cJSON_AddNumberToObject(userinfo, "user_type",
			websocket_data->face_photo.user_type);
	fa_array = cJSON_CreateArray();
	if (NULL == fa_array) {
		ALOGE("malloc fa_array failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(userinfo, "fa_list", fa_array);
	/*  pthread_mutex_lock(&face_down_lock); */
	if (websocket_data->face_photo.error == 0) {
		cJSON_AddStringToObject(body, "errmsg", st_recg_info[0].error_msg);
		for (int i = 0; i < websocket_data->face_photo.face_count; i++) {
			fa = cJSON_CreateObject();
			if (NULL == fa) {
				ALOGE("malloc fa failed, errno: %d (%s)\n",
					  errno, strerror(errno));
				goto error;
			}
			cJSON_AddItemToArray(fa_array, fa);
			cJSON_AddNumberToObject(
				fa,
				"id",
				websocket_data->face_photo.face[i].id);
			cJSON_AddStringToObject(
				fa,
				"data",
				websocket_data->face_photo.face[i].face_feat_base64);
		}
	} else {
		cJSON_AddStringToObject(body, "errmsg", st_recg_info[1].error_msg);
#if 0
		if (websocket_data->cb_fun)
			websocket_data->cb_fun(signal_exit_page);
		else
			ws_control_exit_page();
#endif
	}
	/*  pthread_mutex_unlock(&face_down_lock); */

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief userinfo_photo_cb
 * @note  update db when no error
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_upload_userinfo_photo_cb(
	unsigned char *data,
	size_t len,
	unsigned char flags)
{
	cJSON *root = NULL;
	cJSON *headers = NULL;
	int errcode = -1;
	char *errmsg = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	/*send signal for exit*/

	if (0 != errcode || websocket_data->face_photo.error != 0) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	ws_control_userinfo_update_one(
		websocket_data->face_photo.userid,
		websocket_data->face_photo.user_type,
		websocket_data->face_photo.face[0].data.fa_data);
error:

	/*websocket_data->face_photo_thread = 0;*/
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return 0;
}

/**
 * @brief take_photo_feat thread
 * @note  take a photo and calc feats
 * and send exit signal after calc feats
 * @param  *arg:
 * @retval None
 */
static void *thread_take_photo_feat(void *arg)
{
	int ret = 0;
	prctl(PR_SET_NAME, "take_feat");
	const int circle_r = 197;
    ret = face_photo_set_area(get_screen_width() / 2 - circle_r,
                                  get_screen_height() / 2 - circle_r,
                                  get_screen_width() / 2 + circle_r,
                                  get_screen_height() / 2 + circle_r);
    if (ret)
      ALOGE("set face input limit area fail!");
	ret = ws_control_calc_feat_from_photo(
		&websocket_data->face_photo.face[0].data.feat,
		30);
	if (ret == 0) {
		ret = ws_control_algdata_2_fadata(
			websocket_data->face_photo.face[0].data.fa_data,
			websocket_data->face_photo.face[0].face_feat_base64);
	}
	websocket_data->face_photo.error = ret;
	/*cancel take photo*/
	if (websocket_data->face_photo_exit_flag) {
		websocket_data->face_photo_exit_flag = 0;
		websocket_data->face_photo_thread = 0;
		return NULL;
	}
	ws_app_upload_userinfo_photo();
	usleep(100 * 1000);
	if (websocket_data->cb_fun)
		websocket_data->cb_fun(signal_exit_page);
	else
		ws_control_exit_page();
	websocket_data->face_photo_thread = 0;
	return 0;
}

/**
 * @brief  push_enter_page
 * @note   if camera is used call photo_err function
 * if camera not used create thread for take photo
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_push_enter_page(unsigned char *data,
				size_t len, unsigned char flags)
{
	pthread_t photo_thrd;
	cJSON *root = NULL;
	cJSON *body = NULL;
	char *oper_id = NULL;
	char *userid = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	oper_id = cJSON_GetObjectItem(body, "oper_id")->valuestring;
	userid = cJSON_GetObjectItem(body, "userid")->valuestring;
	if (websocket_data->face_photo_thread) {
		ALOGE("take photo is run\n");
		/*send error*/
		ws_app_upload_userinfo_photo_error(oper_id, userid, 101);
		goto error;
	}
	websocket_data->face_photo_thread = 1;
	if (websocket_data->cb_fun)
		websocket_data->cb_fun(signal_enter_page);
	else
		ws_control_enter_page(oper_id, userid);
	ALOGD("ws_app_user_face enter\n");

	memset(&websocket_data->face_photo, 0, sizeof(user_faces_t));

	websocket_data->face_photo.face_count = 1;
	strncpy(websocket_data->face_photo.oper_id, oper_id, 128);
	strncpy(websocket_data->face_photo.userid, userid, 128);

	pthread_create(&photo_thrd, NULL, thread_take_photo_feat, NULL);
	pthread_detach(photo_thrd);

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return 0;
}

/**
 * @brief  exit_page
 * @note   stop calc feats by take photo
 * when the cmd form the same operid otherwise ignore
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_push_exit_page(unsigned char *data,
				size_t len, unsigned char flags)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *body = NULL;
	char *oper_id = NULL;
#if 0
	char *userid = NULL;
#endif

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	oper_id = cJSON_GetObjectItem(body, "oper_id")->valuestring;
#if 0
	userid = cJSON_GetObjectItem(body, "userid")->valuestring;
#endif
	if (strcmp(oper_id, websocket_data->face_photo.oper_id) == 0) {
		websocket_data->face_photo_exit_flag = 1;
		ws_control_calc_feat_from_photo_stop();
		if (websocket_data->cb_fun)
			websocket_data->cb_fun(signal_exit_page);
		else
			ret = ws_control_exit_page();
	}
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/*app main*/
/**
 * @brief  get admin info
 * @note   NULL
 * @retval 0 success
 */
int ws_app_get_admin_info()
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_admin_info");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_admin_info");

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  admin_info_cb
 * @note   get admin info and storge to db
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_get_admin_info_cb(unsigned char *data,
				size_t len, unsigned char flags)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *userinfo_array = NULL;
	cJSON *userinfo = NULL;
	char *userid = NULL;
	char *name = NULL;
	int errcode = -1;
	char *errmsg = NULL;
	int size = 0;
	ee_queue_t head;
	websocket_admin_info_t *admin = NULL;
	ee_queue_t *h = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	userinfo_array = cJSON_GetObjectItem(body, "userinfo");
	if (NULL == userinfo_array) {
		goto error;
	}
	size = cJSON_GetArraySize(userinfo_array);

	h = &head;
	ee_queue_init(h);

	for (int i = 0; i < size; i++) {
		admin = calloc(sizeof(websocket_admin_info_t), 1);
		if (NULL == admin) {
			goto error;
		}
		userinfo = cJSON_GetArrayItem(userinfo_array, i);
		userid = cJSON_GetObjectItem(userinfo, "userid")->valuestring;
		strncpy(admin->user_id, userid, 64);
		name = cJSON_GetObjectItem(userinfo, "name")->valuestring;
		strncpy(admin->name, name, 32);
		ee_queue_insert_tail(h, &(admin->queue));
	}
	ws_control_admin_update(h);
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	ws_control_admin_free(h);

	return ret;
}

/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_push_bind_corp(unsigned char *data,
				size_t len, unsigned char flags)
{
	return 0;
}

/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_push_change_userid(unsigned char *data,
				size_t len, unsigned char flags)
{
	return 0;
}

/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_push_user_scan(unsigned char *data,
				size_t len, unsigned char flags)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *body = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	ret = ws_control_user_scan(
		cJSON_GetObjectItem(body, "userid")->valuestring,
		cJSON_GetObjectItem(body, "user_type")->valueint);
	if (ret == 1 && websocket_data->cb_fun)
		websocket_data->cb_fun(signal_login);

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return 0;
}

/*admin list*/
/**
 * @brief push_admin_list_change
 * @note  call get admin info
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_push_admin_list_change(
			unsigned char *data,
			size_t len,
			unsigned char flags)
{
	ws_app_get_admin_info();
	return 0;
}
/**
 *
 *
 * @author
 *
 * @param data
 * @param len
 * @param flags
 *
 * @return int
 */
int ws_app_upload_log_cb(unsigned char *data, size_t len, unsigned char flags)
{
	return 0;
}
/**
 *
 *
 * @author
 *
 * @param file
 *
 * @return int
 */
int ws_app_upload_log(char *file)
{
	int ret = 0;
	int file_size = 0;
	int head_size = 0;
	int size = 0;
	char *data = NULL;
	log_file_info_t info;

	if (NULL == file) {
		ALOGE("%s:file is null\n", __func__);
		return -1;
	}
	file_size = file_length(file);
	if (file_size == -1 || file_size >= 2 * 1024 * 1024) {
		ALOGE("%s:file size %d\n", __func__, file_size);
		return -1;
	}
	head_size = sizeof(log_file_info_t);
	size = head_size + file_size;
	data = calloc(size, 1);
	if (NULL == data) {
		ALOGE("%s:data is null\n", __func__);
		return -1;
	}
	/*head*/
	info.format_verison = 1;
	info.cmd_len = 10;
	memcpy(info.cmd, "upload_log", strlen("upload_log"));
	info.req_id_len = 13;
	memcpy(info.req_id, "lb_upload_log", strlen("lb_upload_log"));
	/*file read*/
	ret = file_read(file, &data[head_size], file_size);
	if (ret > 0) {
		/*send bin data*/
		info.data_len = ret;
		memcpy(data, &info, head_size);
		for(int i=0;i<head_size;i++){
			ALOGD("%02x\n", data[i]);
		}
		ret = ws_send_binary((unsigned char *)data, head_size + ret);
	}
	if (data) {
		free(data);
		data = NULL;
	}
	return ret;
}
/**
 *
 *
 * @brief
 *
 * @param arg
 *
 * @return void*
 */
static void* thread_push_log_by_3hour(void *arg)
{
	int counter = 0;
	websocket_data->log_run = 1;
	prctl(PR_SET_NAME, "push_log");

	while (websocket_data->log_run) {
		if (((counter++) % LOG_INTER_SEC == 0) &&
			websocket_data->link_wechat) {
			ALOGD("%s:push log\n", __func__);
			ws_app_upload_log(LOG_PATH);
		}
		usleep(1000 * 1000);
	}
	return NULL;
}
/**
 *
 *
 * @brief
 *
 * @param void
 */
void ws_app_log_thread_start(void)
{
	if (websocket_data) {
		if (websocket_data->log_run == 0)
			pthread_create(&websocket_data->log_thread_id, NULL,
						   thread_push_log_by_3hour, NULL);
	}
}
/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_push_fetch_log(unsigned char *data, size_t len, unsigned char flags)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *body = NULL;
	char *hint = NULL;
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	hint = cJSON_GetObjectItem(body, "hint")->valuestring;
	/*upload log*/
	ws_app_upload_log(hint);
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  ping wechat server
 * @note   this interface not need register to websocket server
 * @param  sec:
 * @retval 0 is success
 */
int ws_app_ping(int sec)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		return -1;
	}
	cJSON_AddStringToObject(root, "cmd", "ping");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_ping");
	request = cJSON_PrintUnformatted(root);
	ret = ws_register_timer_text(sec,
				(unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_ping_cb(unsigned char *data, size_t len, unsigned char flags)
{
	return 0;
}
/**
 * @brief  checktime to server  periodicity
 * @note   perfer to ping
 * @param  sec:
 * @retval
 */
int ws_app_register_get_timestamp(int sec)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_timestamp");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_timestamp");
	request = cJSON_PrintUnformatted(root);
	ret = ws_register_timer_text(
		sec,
		(unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief checktime once
 * @note  not need register to websocket server
 * @retval 0 success
 */
int ws_app_get_timestamp(void)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	char *request = NULL;

	ALOGD("ws_app_get_timestamp\n");
	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_timestamp");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_timestamp");
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief get_timestamp_cb
 * @note  when time diff value is > 5 seconds will
 * send qrcode signal for update bind qrcode and
 * set time to hwclock
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_get_timestamp_cb(unsigned char *data,
			size_t len, unsigned char flags)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	int errcode = -1;
	char *errmsg = NULL;
	int timestamp = 0;
	time_t tt = 0;
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	timestamp = cJSON_GetObjectItem(body, "timestamp")->valueint;
	ALOGD("timestamp = %d\n", timestamp);
	tt = time(NULL);
	ret = ws_control_set_timestamp(timestamp);
	if (websocket_data->cb_fun && abs(tt - timestamp) >= MAX_DIFF_SECOND)
		websocket_data->cb_fun(signal_qrcode);
	/*ws_control_qrcode();*/
	if (websocket_data->status == STATUS_IDLE &&
		websocket_data->status_last == STATUS_SYNC) {
		websocket_data->status = STATUS_SECRET_NO;
	}
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	return ret;
}
/**
 * @brief  change secret no with seed secret no
 * @note   only used in seed secret no
 * @param  *sn:
 * @param  *seed_secret_no:
 * @retval 0 success
 */
int ws_app_device_get_secret_no(char *sn, char *seed_secret_no)
{
	int ret = -1;
	int i = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;
	unsigned char sha[20] = { 0 };
	char device_signature[20 * 2 + 1] = { 0 };
	char str_magic[256] = { 0 };
	char str_timestamp[16] = { 0 };

	char str_secret_no[] = "secret_no";
	char nonce[NONCE_LEN + 1] = { 0 };
	ALOGD("ws_app_device_get_secret_no\n");

	ws_control_get_timestamp(str_timestamp, 16);
	ws_control_get_nonce(nonce, NONCE_LEN);

	char str[5][SORT_STR_LEN] = { 0 };
	strncpy(str[0], nonce, SORT_STR_LEN - 1);
	strncpy(str[1], str_timestamp, SORT_STR_LEN - 1);
	strncpy(str[2], sn, SORT_STR_LEN - 1);
	strncpy(str[3], seed_secret_no, SORT_STR_LEN - 1);
	strncpy(str[4], str_secret_no, SORT_STR_LEN - 1);

	ALOGD("sn             =%s\n", sn);
	ALOGD("seed_secret_no =%s\n", seed_secret_no);
	ALOGD("timestamp      =%s\n", str_timestamp);
	ALOGD("nonce          =%s\n", nonce);
	ALOGD("secret_no      =%s\n", str_secret_no);

	ws_control_sort_dictionary_order(str, 5);
	for (i = 0; i < 5; i++) strncat(str_magic, str[i], SORT_STR_LEN);

	ALOGD("str_magic      =%s\n", str_magic);
	const uint8_t *msgs[1] = { (const uint8_t *)str_magic };
	const size_t msg_lens[1] = { strlen(str_magic) };

	mg_hash_sha1_v(1, msgs, msg_lens, sha);

	for (i = 0; i < 20; i++) {
		sprintf(&device_signature[i * 2], "%02x", (unsigned char)sha[i]);
	}

	ALOGD("device_signature =%s\n", device_signature);

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_secret_no");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_secret_no");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "device_signature", device_signature);
	cJSON_AddStringToObject(body, "nonce", nonce);
	cJSON_AddStringToObject(body, "timestamp", str_timestamp);
	cJSON_AddStringToObject(body, "sn", sn);
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  get_secret_no_cb
 * @note   NULL
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_device_get_secret_no_cb(
	unsigned char *data,
	size_t len,
	unsigned char flags)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	int errcode = -1;
	char *errmsg = NULL;
	char *secret_no = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	secret_no = cJSON_GetObjectItem(body, "secret_no")->valuestring;
	ALOGD("secret_no = %s\n", secret_no);

	strncpy(websocket_data->param.secret_no, secret_no, 64);
	ws_control_set_param(&websocket_data->param);
	websocket_data->status = STATUS_REGISTER;
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  device_register
 * @note   register device to wechat
 * @param  *sn:
 * @param  *secret_no:
 * @retval 0 success
 */
int ws_app_device_register(char *sn, char *secret_no)
{
	int ret = -1;
	int i = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;
	unsigned char sha[20] = { 0 };
	char device_signature[20 * 2 + 1] = { 0 };
	char str_magic[256] = { 0 };
	char str_timestamp[16] = { 0 };

	char str_register[] = "register";
	char nonce[NONCE_LEN + 1] = { 0 };
	ALOGD("ws_app_device_register\n");

	ws_control_get_timestamp(str_timestamp, 16);
	ws_control_get_nonce(nonce, NONCE_LEN);

	char str[5][SORT_STR_LEN] = { 0 };
	strncpy(str[0], nonce, SORT_STR_LEN - 1);
	strncpy(str[1], str_timestamp, SORT_STR_LEN - 1);
	strncpy(str[2], sn, SORT_STR_LEN - 1);
	strncpy(str[3], secret_no, SORT_STR_LEN - 1);
	strncpy(str[4], str_register, SORT_STR_LEN - 1);

	ALOGD("sn             =%s\n", sn);
	ALOGD("seed_secret_no =%s\n", secret_no);
	ALOGD("timestamp      =%s\n", str_timestamp);
	ALOGD("nonce          =%s\n", nonce);
	ALOGD("register       =%s\n", str_register);

	ws_control_sort_dictionary_order(str, 5);
	for (i = 0; i < 5; i++) strncat(str_magic, str[i], SORT_STR_LEN);

	ALOGD("str_magic    =%s\n", str_magic);
	const uint8_t *msgs[1] = { (const uint8_t *)str_magic };
	const size_t msg_lens[1] = { strlen(str_magic) };

	mg_hash_sha1_v(1, msgs, msg_lens, sha);

	for (i = 0; i < 20; i++) {
		sprintf(&device_signature[i * 2], "%02x", (unsigned char)sha[i]);
	}

	ALOGD("device_signature =%s\n", device_signature);

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "register");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_register");
	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "device_signature", device_signature);
	cJSON_AddStringToObject(body, "nonce", nonce);
	cJSON_AddStringToObject(body, "timestamp", str_timestamp);
	cJSON_AddStringToObject(body, "sn", sn);
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief device_register_cb
 * @note  switch to active when no errcode
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_device_register_cb(unsigned char *data,
			size_t len, unsigned char flags)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	int errcode = -1;
	char *errmsg = NULL;
	char *active_code = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		if (ERROR_CODE_KEEP_ALIVE_REGISTERED == errcode ||
			ERROR_CODE_REGISTERED == errcode)
			websocket_data->status = STATUS_ACTIVE;
		else if (ERROR_CODE_ACTIVED == errcode)
			websocket_data->status = STATUS_SUBSCRIBE;
		else if (ERROR_CODE_HAS_SUBSCRIBED == errcode)
			websocket_data->status = STATUS_SUCCESS;
		else
			websocket_data->status = STATUS_DEFAULT;
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	active_code = cJSON_GetObjectItem(body, "active_code")->valuestring;
	ALOGD("active_code = %s\n", active_code);

	strncpy(websocket_data->param.active_code, active_code, 64);
	ws_control_set_param(&websocket_data->param);
	websocket_data->status = STATUS_ACTIVE;

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief device_active
 * @note  call after register success
 * @param  *active_code:
 * @retval 0 success
 */
int ws_app_device_active(char *active_code)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "active");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_active");

	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "active_code", active_code);

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief device_active_cb
 * @note  switch to subscribe crop status when no errcode
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_device_active_cb(unsigned char *data,
			size_t len, unsigned char flags)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	int errcode = -1;
	char *errmsg = NULL;
	char *secret = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		websocket_data->status = STATUS_DEFAULT;
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		goto error;
	}
	secret = cJSON_GetObjectItem(body, "secret")->valuestring;
	ALOGD("secret = %s\n", secret);
	if (strcmp(secret, websocket_data->param.secret)) {
		strncpy(websocket_data->param.secret, secret, 64);
		ws_control_set_param(&websocket_data->param);
		if (websocket_data->cb_fun)
			websocket_data->cb_fun(signal_bind);
		else
			ws_control_ws_bind();
	}
	websocket_data->status = STATUS_SUBSCRIBE;

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief subscribe_corp
 * @note  call after active success
 * @param  *secret:
 * @retval 0 success
 */
int ws_app_device_subscribe_corp(char *secret)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;
	build_info_t build_info;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "subscribe_corp");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_subscribe_corp");

	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "secret", secret);
	memset(&build_info, 0, sizeof(build_info_t));
	system_info_get_build_info(&build_info);
	cJSON_AddStringToObject(
		body,
		"firmware_version",
		build_info.system_version);

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief subscribe_corp_cb
 * @note switch to success when no err
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_device_subscribe_corp_cb(
	unsigned char *data,
	size_t len,
	unsigned char flags)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	int errcode = -1;
	char *errmsg = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (errcode == 0 || errcode == ERROR_CODE_SUBSCRIBED) {
		ALOGD("subscribe_corp finish\n");
		websocket_data->status = STATUS_SUCCESS;
	} else {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		websocket_data->status = STATUS_DEFAULT;
		goto error;
	}

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  generate dynamic bind qrcode
 * @note   NULL
 * @param  *sn:
 * @param  *secret:
 * @param  path:
 * @retval 0 success
 */
int ws_app_device_dynamic_bind_qrcode(char *sn, char *secret,
	char* path)
{
	int i = 0;
	unsigned char sha[20] = {0};
	char device_signature[20 * 2 + 1] = {0};
	char str_magic[128] = {0};
	char qr_encode_str[1024] = {0};
	char str_timestamp[16] = {0};
	char nonce[6 + 1] = {0};

	ws_control_get_timestamp(str_timestamp, 16);
	ws_control_get_nonce(nonce, 6);

	char url[] = "https://open.work.weixin.qq.com/connect/hardware";
	char type[] = "qrcode";
	char str[6][SORT_STR_LEN] = {0};
	strncpy(str[0], sn, SORT_STR_LEN - 1);
	strncpy(str[1], secret, SORT_STR_LEN - 1);
	strncpy(str[2], str_timestamp, SORT_STR_LEN - 1);
	strncpy(str[3], nonce, SORT_STR_LEN - 1);
	strncpy(str[4], type, SORT_STR_LEN - 1);

	ws_info("sn             =%s\n", sn);
	ws_info("secretNo       =%s\n", secret);
	ws_info("timestamp      =%s\n", str_timestamp);
	ws_info("nonce          =%s\n", nonce);
	ws_info("type           =%s\n", type);

	ws_control_sort_dictionary_order(str, 6);
	for (i = 0; i < 6; i++)
		strcat(str_magic, str[i]);

	ws_info("str_magic		=%s\n", str_magic);
	const uint8_t *msgs[1] = {(const uint8_t *) str_magic};
	const size_t msg_lens[1] = {strlen(str_magic)};

	mg_hash_sha1_v(1, msgs, msg_lens, sha);

	for(i = 0; i<20; i++)
		sprintf(&device_signature[i*2], "%02x", (unsigned char)sha[i]);

	ws_info("device_signature =%s\n", device_signature);

	sprintf(qr_encode_str, "%s?sn=%s&timestamp=%s&nonce=%s&signature=%s&type=%s",
		url, sn, str_timestamp, nonce, device_signature, type);

	ws_info("%s\n", qr_encode_str);

	return qrencode_create(qr_encode_str, path);
}

/**
 * @brief generate dynmic login qrcode
 * @note  NULL
 * @param  *sn:
 * @param  *secret_no:
 * @param  *path:
 * @retval 0 success
 */
int ws_app_device_dynamic_login_qrcode(char *sn, char *secret_no, char *path)
{
	int i = 0;
	unsigned char sha[20] = { 0 };
	char device_signature[20 * 2 + 1] = { 0 };
	char str_magic[128] = { 0 };
	char qr_encode_str[1024] = { 0 };
	char str_timestamp[16] = { 0 };
	char nonce[6 + 1] = { 0 };

	if (NULL == path || NULL == sn || NULL == secret_no) {
		ALOGE("sn or secret_no or path is null\n");
		return -1;
	}
	ws_control_get_timestamp(str_timestamp, 16);
	ws_control_get_nonce(nonce, 6);

	char url[] = "https://open.work.weixin.qq.com/connect/hardware";
	char type[] = "dynamic_login";
	char state[] = "qrcode_login";
	char str[6][SORT_STR_LEN] = { 0 };
	strncpy(str[0], sn, SORT_STR_LEN - 1);
	strncpy(str[1], secret_no, SORT_STR_LEN - 1);
	strncpy(str[2], str_timestamp, SORT_STR_LEN - 1);
	strncpy(str[3], nonce, SORT_STR_LEN - 1);
	strncpy(str[4], type, SORT_STR_LEN - 1);
	strncpy(str[5], state, SORT_STR_LEN - 1);

	ALOGD("sn             =%s\n", sn);
	ALOGD("secretNo       =%s\n", secret_no);
	ALOGD("timestamp      =%s\n", str_timestamp);
	ALOGD("nonce          =%s\n", nonce);
	ALOGD("type           =%s\n", type);
	ALOGD("state          =%s\n", state);

	ws_control_sort_dictionary_order(str, 6);
	for (i = 0; i < 6; i++) strncat(str_magic, str[i], SORT_STR_LEN);

	ALOGD("str_magic    =%s\n", str_magic);
	const uint8_t *msgs[1] = { (const uint8_t *)str_magic };
	const size_t msg_lens[1] = { strlen(str_magic) };

	mg_hash_sha1_v(1, msgs, msg_lens, sha);

	for (i = 0; i < 20; i++)
		sprintf(&device_signature[i * 2], "%02x", (unsigned char)sha[i]);

	ALOGD("device_signature =%s\n", device_signature);

	snprintf(qr_encode_str, sizeof(qr_encode_str),
	"%s?sn=%s&timestamp=%s&nonce=%s&signature=%s&type=dynamic_login&state=%s",
			 url, sn, str_timestamp, nonce, device_signature, state);


	ALOGD("%s\n", qr_encode_str);

	return qrencode_create(qr_encode_str, path);
}

/**
 * @brief
 * @note
 * @retval
 */
int ws_app_error_report(char *errmsg, int errcode)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;

	if (NULL == errmsg) {
		ALOGE("%s: errmsg is null\n", __func__);
		return -1;
	}
	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "error_report");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_error_report");

	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddNumberToObject(body, "errcode", errcode);
	cJSON_AddStringToObject(body, "errmsg", errmsg);
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	return ret;
}

/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_error_report_cb(unsigned char *data,
			size_t len, unsigned char flags)
{

	return 0;
}
/**
 * @brief
 * @note
 * @param  *user_id: empoyee id
 * @param  user_type: 0 empoyee(teacher) 2 student
 * @param  tmperature: 36.7
 * @param  status: 0 normal 1 high 2 low
 * @retval
 */
int ws_app_report_user_temperature(
	char *user_id,
	int user_type,
	float tmperature,
	int status)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;
	char buf[16]= {0};

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "report_user_temperature");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(
		headers,
		"req_id",
		"lb_report_user_temperature");

	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body,"userid",user_id);
	cJSON_AddNumberToObject(body,"user_type",user_type);
	sprintf(buf,"%f",tmperature);
	cJSON_AddStringToObject(body,"temperature",buf);
	cJSON_AddNumberToObject(body,"status",status);
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	return ret;
}
/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_report_user_temperature_cb(
	unsigned char *data,
	size_t len,
	unsigned char flags)
{
	int errcode = 0;
	cJSON *root = NULL;
	cJSON *body = NULL;
	char *errmsg  = NULL;
	int status = 0;
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if(errcode != 0) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if(NULL == body){
		ALOGE("cJSON_Parse body failed\n");
		goto error;
	}
	status = cJSON_GetObjectItem(body, "status")->valueint;
	websocket_data->health_status = status;
	/**/
	if(websocket_data->cb_fun)
		websocket_data->cb_fun(signal_health_status);
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return 0;
}
/**
 * @brief get body status
 * @note  get body status when after report temperature
 * @param status int 0 normal 1 high 2 low
 * @retval
 */
void ws_app_get_body_status(int *status)
{
	if(status)
		*status = websocket_data->health_status;
}

/**
 * @brief send checkin data
 * @note  read checkin data from db and send to server
 * max 199 once
 * @retval
 */
int ws_app_checkin(void)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	cJSON *checkin_data_array = NULL;
	cJSON *checkin_data = NULL;
	char *request = NULL;
	time_t cur_time;
	web_socket_checkin_info_t *p = NULL;

	if (NULL == websocket_data) {
		/* ALOGE("websocket_data is NULL\n"); */
		return -1;
	}
	if (websocket_data->websocket_exit)
		return 0;
	if (websocket_data->link_status == 0) {
		ALOGI("%s:link_status not link\n", __func__);
		return 0;
	}
	time(&cur_time);
	pthread_mutex_lock(&websocket_data->checkin_lock);
	if (websocket_data->checkin_send == 1 && cur_time -
		websocket_data->last_time <= 8) {
		pthread_mutex_unlock(&websocket_data->checkin_lock);
		ALOGE("%s:sending\n", __func__);
		return 0;
	}
	websocket_data->last_time = cur_time;
	/*hava data not send free queue*/
	if (websocket_data->checkin_send == 1)
		ws_control_put_checkin_data(websocket_data->h_checkin);
	ee_queue_init(websocket_data->h_checkin);
	ret = ws_control_get_checkin_data(websocket_data->h_checkin);
	if (ret == 0) {
		pthread_mutex_unlock(&websocket_data->checkin_lock);
		ALOGE("%s:no checkin data\n", __func__);
		return 0;
	}
	websocket_data->checkin_send = 1;
	pthread_mutex_unlock(&websocket_data->checkin_lock);
	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "checkin");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_checkin");

	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);

	checkin_data_array = cJSON_CreateArray();
	if (NULL == checkin_data_array) {
		ALOGE("malloc checkin_data_array failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddItemToObject(body, "checkin_data", checkin_data_array);
	pthread_mutex_lock(&websocket_data->checkin_lock);
	ee_queue_foreach(p, websocket_data->h_checkin,
					 web_socket_checkin_info_t, queue) {
		checkin_data = cJSON_CreateObject();
		if (NULL == checkin_data) {
			pthread_mutex_unlock(&websocket_data->checkin_lock);
			ALOGE("malloc body failed, errno: %d (%s)\n",
				  errno, strerror(errno));
			ret = -1;
			goto error;
		}
		ALOGD("%s %d\n", p->user_id, p->timestamp);
		cJSON_AddItemToArray(checkin_data_array, checkin_data);
		cJSON_AddStringToObject(checkin_data, "userid", p->user_id);
		cJSON_AddNumberToObject(checkin_data, "user_type", p->user_type);
		cJSON_AddNumberToObject(checkin_data, "timestamp", p->timestamp);
		p->is_upload = 1;
	}
	pthread_mutex_unlock(&websocket_data->checkin_lock);
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	if (ret == -1) {
		pthread_mutex_lock(&websocket_data->checkin_lock);
		ws_control_put_checkin_data(websocket_data->h_checkin);
		pthread_mutex_unlock(&websocket_data->checkin_lock);
	}

	return ret;
}

/**
 * @brief checkin cb
 * @note  delete record if translate is success
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_checkin_cb(unsigned char *data, size_t len, unsigned char flags)
{
	int size = 0;
	int errcode = 0;
	cJSON *root = NULL;
	cJSON *body = NULL;
	cJSON *fail_list_array = NULL;
	cJSON *fail_user = NULL;
	char *errmsg = NULL;
	char *userid = NULL;
	int timestamp = 0;
	web_socket_checkin_info_t *p = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	/*update list*/
	/*  ws_control_set_checkin_upload(h); */

	fail_list_array = cJSON_GetObjectItem(body, "fail_list");
	if (fail_list_array) {
		size = cJSON_GetArraySize(fail_list_array);
		for (int i = 0; i < size; i++) {
			fail_user = cJSON_GetArrayItem(fail_list_array, i);
			if (NULL == fail_user) {
				goto error;
			}
			userid = cJSON_GetObjectItem(fail_user, "userid")->valuestring;
			timestamp = cJSON_GetObjectItem(fail_user, "timestamp")->valueint;
			ALOGD("failed %s %d\n", userid, timestamp);
			pthread_mutex_lock(&websocket_data->checkin_lock);
			ee_queue_foreach(
				p,
				websocket_data->h_checkin,
				web_socket_checkin_info_t,
				queue) {
				ALOGD("in queue %s %d\n", userid, timestamp);
				if (p->timestamp == timestamp &&
					0 == strcmp(userid, p->user_id)) {
					p->is_upload = 0;
					break;
				}
			}
			pthread_mutex_unlock(&websocket_data->checkin_lock);
		}
	}
	pthread_mutex_lock(&websocket_data->checkin_lock);
	ws_control_del_checkin_data(websocket_data->h_checkin);
	pthread_mutex_unlock(&websocket_data->checkin_lock);

error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	pthread_mutex_lock(&websocket_data->checkin_lock);
	ws_control_put_checkin_data(websocket_data->h_checkin);
	websocket_data->checkin_send = 0;
	pthread_mutex_unlock(&websocket_data->checkin_lock);

	return 0;
}

int ws_app_register_get_firmware_info(int sec)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_firmware_info");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_firmware_info");
	request = cJSON_PrintUnformatted(root);
	ret = ws_register_timer_text(
		sec,
		(unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}
/**
 *
 *
 * @author
 *
 * @param void
 *
 * @return int
 */
int ws_app_get_firmware_info(void)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "get_firmware_info");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_get_firmware_info");
	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 *
 *
 * @author
 *
 * @param version
 */
void ws_app_get_firmware_version(char *version)
{
	if (version) {
		strncpy(
			version,
			websocket_data->firmware_info.firmware_version,
			VERSION_LENGTH);
	}
}
/**
 *
 *
 * @author
 *
 * @param arg
 *
 * @return void*
 */
static void *thread_download_firmware_wget(void *arg)
{
	int ret = 0;
	int status = 0;
	int counter = 0;
	char cmd[256] = { 0 };
	char tmp_path[256] = { 0 };
	prctl(PR_SET_NAME, "download_firmware");

	char *filename = NULL;
	if (is_file_exist(PACKET_PATH))
		remove(PACKET_PATH);
	sprintf(cmd, "wget -c -T 10 -q -P %s %s", OTA_PATH,
			websocket_data->firmware_info.download_url);
	ALOGD("%s:%s\n", __func__, websocket_data->firmware_info.download_url);
	filename = ws_control_get_file_name(websocket_data->firmware_info.download_url);
	sprintf(tmp_path, "%s/%s", OTA_PATH, filename);
	while (1) {
		if (counter++ >= 10) {
			ALOGE("%s:wget count =%d\n", __func__, counter);
			break;
		}
		status = system(cmd);
		/**/
		if (0 == status) {
			if (WIFEXITED(status)) {
				if (0 == WEXITSTATUS(status)) {
					/*md5 calc*/
					ret = check_file_md5(
						tmp_path,
						websocket_data->firmware_info.md5sum);
					/*success*/
					if (ret == 0) {
						/*move*/
						ret = file_move(tmp_path, PACKET_PATH);
						if (ret == 0) {
							ALOGD("%s:update success\n", __func__);
							if (websocket_data->cb_fun)
								websocket_data->cb_fun(signal_firmware_success);
							break;
						}
					} else {
						ALOGE("%s: md5 error\n", __func__);
						if (websocket_data->cb_fun)
							websocket_data->cb_fun(signal_firmware_continue);
					}
				} else {
					ALOGE("%s:cmd 1 error\n", __func__);
					if (websocket_data->cb_fun)
						websocket_data->cb_fun(signal_firmware_continue);
				}
			} else {
				ALOGE("%s:cmd 2 error\n", __func__);
				if (websocket_data->cb_fun)
					websocket_data->cb_fun(signal_firmware_continue);
			}
		} else {
			ALOGE("%s:cmd exec error\n", __func__);
			if (websocket_data->cb_fun)
				websocket_data->cb_fun(signal_firmware_continue);
		}
	}
	/**/
	if (counter >= 10) {
		if (websocket_data->cb_fun)
				websocket_data->cb_fun(signal_firmware_failed);
	}
	if (is_file_exist(tmp_path))
		remove(tmp_path);
	websocket_data->firmware_info.is_download = 0;

	return NULL;
}
/**
 *
 *
 * @author
 *
 * @param void
 */
void ws_app_start_download_firmware(void)
{
	if (websocket_data) {
		pthread_create(&websocket_data->firmware_info.id, NULL,
					   thread_download_firmware_wget, NULL);
		pthread_detach(websocket_data->firmware_info.id);
	}
}
/**
 *
 *
 * @author
 *
 * @param data
 * @param len
 * @param flags
 *
 * @return int
 */
int ws_app_get_firmware_info_cb(
	unsigned char *data,
	size_t len,
	unsigned char flags)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *firmware_version = NULL;
	char *md5sum = NULL;
	int size = 0;
	char *media_id = NULL;
	char *download_url = NULL;
	int errcode = -1;
	char *errmsg = NULL;
	build_info_t build_info;

	if (websocket_data->firmware_info.is_download) {
		ALOGE("%s is loading\n", __func__);
		return -1;
	}
	websocket_data->firmware_info.is_download = 1;
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		ret = -2;
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		ret = -2;
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		ret = -2;
		goto error;
	}
	body = cJSON_GetObjectItem(root, "body");
	if (NULL == body) {
		ALOGE("cJSON_GetObjectItem body failed\n");
		ret = -2;
		goto error;
	}
	firmware_version = cJSON_GetObjectItem(
		body,
		"firmware_version")->valuestring;
	memset(&build_info, 0, sizeof(build_info_t));
	system_info_get_build_info(&build_info);
	if (strcmp(firmware_version, build_info.system_version) <= 0) {
		ALOGE("%s:firmware_version is low %s\n",
			  __func__, build_info.system_version);
		ret = -2;
		goto error;
	}
	strncpy(websocket_data->firmware_info.firmware_version,
			firmware_version, VERSION_LENGTH);
	md5sum = cJSON_GetObjectItem(body, "md5sum")->valuestring;
	strncpy(websocket_data->firmware_info.md5sum,
			md5sum, 64);
	size = cJSON_GetObjectItem(body, "size")->valueint;
	websocket_data->firmware_info.size = size;
	if (size < 20*1024*1024) {
		media_id = cJSON_GetObjectItem(body, "media_id")->valuestring;
		strncpy(websocket_data->firmware_info.media_id,
				media_id, 128);
		/*start */
		websocket_data->firmware_info.is_download = 0;
	} else {
		download_url = cJSON_GetObjectItem(body, "download_url")->valuestring;
		strncpy(websocket_data->firmware_info.download_url,
			download_url, 256);
		/*start*/
		if (websocket_data->cb_fun)
			websocket_data->cb_fun(signal_new_firmware);
#if 0
		pthread_create(&websocket_data->firmware_info.id, NULL,
					   thread_download_firmware_wget, NULL);
		pthread_detach(websocket_data->firmware_info.id);
#endif

	}
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	if (ret == -2)
		websocket_data->firmware_info.is_download = 0;
	return ret;
}

/**
 * @brief
 * @note
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval
 */
int ws_app_push_update_firmware(unsigned char *data,
				size_t len, unsigned char flags)
{
	ws_app_get_firmware_info();
	return 0;
}
/**
 * @brief  selfhelp_notify
 * @note   for debug
 * @param  *data:
 * @retval 0 success
 */
int ws_app_selfhelp_notify(char *data)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *body = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "selfhelp_notify");
	headers = cJSON_CreateObject();
	if (NULL == headers) {
		ALOGE("malloc headers failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "headers", headers);
	cJSON_AddStringToObject(headers, "req_id", "lb_selfhelp_notify");

	body = cJSON_CreateObject();
	if (NULL == body) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		goto error;
	}
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(body, "data", data);

	request = cJSON_PrintUnformatted(root);
	ret = ws_send_text((unsigned char *)request, strlen(request));

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}

	return ret;
}

/**
 * @brief  selfhelp_notify_cb
 * @note   for debug
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_selfhelp_notify_cb(unsigned char *data,
			size_t len, unsigned char flags)
{
	int ret = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	int errcode = -1;
	char *errmsg = NULL;

	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		goto error;
	}
	errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
	errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
	if (0 != errcode) {
		ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
		goto error;
	}
	ALOGD("selfhelp_notify_cb finish\n");
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	return ret;
}

/**
 * @brief  device register and active thread
 * @note  maintain a status machine for register and active
 * sync->register->active->subscribe_core->success
 * @param  *arg:
 * @retval None
 */
static void *thread_register_and_active(void *arg)
{
	time_t tt;
	prctl(PR_SET_NAME, "register_thread");

	websocket_data->status = STATUS_DEFAULT;
	websocket_data->status_last = STATUS_DEFAULT;
	websocket_data->register_thread_is_run = 1;
	websocket_data->link_wechat = 0;
	ALOGD("thread_register_and_active enter\n");
	while (websocket_data->register_thread_is_run) {
		switch (websocket_data->status) {
		case STATUS_DEFAULT:
			websocket_data->status_last = websocket_data->status;
			websocket_data->status = STATUS_SYNC;
			break;

		case STATUS_SYNC:
			ws_app_get_timestamp();
			time(&tt);
			websocket_data->status_last = websocket_data->status;
			websocket_data->status = STATUS_IDLE;
			break;

		case STATUS_SECRET_NO:
			websocket_data->status_last = websocket_data->status;
			if (0 == strcmp("123456", websocket_data->param.secret_no)) {
				ws_app_device_get_secret_no(
					websocket_data->param.sn,
					websocket_data->param.seed_secret_no);
				websocket_data->status = STATUS_IDLE;
			} else {
				websocket_data->status = STATUS_REGISTER;
			}
			break;

		case STATUS_REGISTER:
			websocket_data->status_last = websocket_data->status;
			ws_app_device_register(
				websocket_data->param.sn,
				websocket_data->param.secret_no);
			time(&tt);
			websocket_data->status = STATUS_IDLE;
			break;

		case STATUS_ACTIVE:
			websocket_data->status_last = websocket_data->status;
			ws_app_device_active(websocket_data->param.active_code);
			time(&tt);
			websocket_data->status = STATUS_IDLE;
#if 0
			if (0 == strcmp("123456", websocket_data->param.secret)) {
				ws_app_device_active(websocket_data->param.active_code);
				websocket_data->status = STATUS_IDLE;
				time(&tt);
			} else {
				websocket_data->status = STATUS_SUBSCRIBE;
			}
#endif
			break;

		case STATUS_SUBSCRIBE:
			websocket_data->status_last = websocket_data->status;
			ws_app_device_subscribe_corp(websocket_data->param.secret);
			websocket_data->status = STATUS_IDLE;
			break;

		case STATUS_SUCCESS:
			websocket_data->link_wechat = 1;
			ws_app_get_admin_info();
			ws_app_get_perm_version();
			/*ws_app_get_firmware_info();*/
			ws_app_log_thread_start();
			websocket_data->register_thread_is_run = 0;
			break;

		case STATUS_IDLE:
			/*repeat active*/
			if (websocket_data->status_last == STATUS_ACTIVE &&
				(time(NULL) - tt >= 5)) {
				websocket_data->status = STATUS_ACTIVE;
			}
			/*repeat register*/
			if (websocket_data->status_last == STATUS_REGISTER &&
				(time(NULL) - tt >= 5)) {
				websocket_data->status = STATUS_REGISTER;
			}
			/*repeat checktime*/
			if (websocket_data->status_last == STATUS_SYNC &&
				(time(NULL) - tt >= 5)) {
				websocket_data->status = STATUS_SYNC;
			}
			break;

		}
		usleep(100 * 1000);
	}
	ALOGD("thread_register_and_active exit\n");

	return NULL;
}

/**
 * @brief  stop register thread
 * @note   NULL
 * @retval 0 success
 */
int ws_app_register_and_active_progess_stop(void)
{
	websocket_data->status = STATUS_DEFAULT;

	if (websocket_data->register_thread_is_run == 0)
		return 0;
	websocket_data->register_thread_is_run = 0;
	usleep(200*1000);
	return 0;
}

/**
 * @brief  start register thread
 * @note NULL
 * @retval 0 success
 */
int ws_app_register_and_avtive_progess_start(void)
{
	if (websocket_data->register_thread_is_run)
		return 0;
	pthread_create(&websocket_data->register_thread_id, NULL,
				   thread_register_and_active, NULL);
	pthread_detach(websocket_data->register_thread_id);

	return 0;
}
static void ws_app_error_proc(void)
{
	user_data_t *data = NULL;
	user_data_t *data_tmp = NULL;

	ALOGD("%s: exec\n", __func__);
#if 1
	/*checkin queue free*/
	pthread_mutex_lock(&websocket_data->checkin_lock);
	if (websocket_data->checkin_send) {
		ws_control_put_checkin_data(websocket_data->h_checkin);
		websocket_data->checkin_send = 0;
	}
	pthread_mutex_unlock(&websocket_data->checkin_lock);
	/*userinfo free*/
	websocket_data->userinfo_recving = 0;
	if (websocket_data->userinfo_packet_count) {
		ws_control_userinfo_free(
			websocket_data->h_userinfo_person,
			websocket_data->h_userinfo_rule);
		websocket_data->userinfo_packet_count = 0;
	}
	/*free*/
	pthread_mutex_lock(&websocket_data->user_data_lock);
	if (!list_empty_careful(&websocket_data->user_data_list)) {
		ALOGD("%s: free list\n", __func__);
		list_for_each_entry_safe(
			data,
			data_tmp,
			&websocket_data->user_data_list,
			node) {
			if (data) {
				list_del(&data->node);
				free(data);
				websocket_data->user_num--;
			}
		}
	}
	pthread_mutex_unlock(&websocket_data->user_data_lock);
#endif
	ALOGD("%s: exec end\n", __func__);
}
/**
 * @brief  unbind corp
 * @note   unbind operation and send unbind signal
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
int ws_app_push_unbind_corp(unsigned char *data, size_t len, unsigned char flags)
{
	ALOGD("ws_app_push_unbind_corp enter\n");
	strncpy(websocket_data->param.secret, "123456", 64);
	websocket_data->param.perm_version = 0;
	ws_control_set_param(&websocket_data->param);
	ws_control_unbind_operation();
	if (websocket_data->cb_fun)
		websocket_data->cb_fun(signal_unbind);
	else
		ws_control_ws_unbind();
	ALOGD("ws_app_push_unbind_corp exit\n");

#if 0
	if (websocket_data->register_thread_is_run)
		websocket_data->status = STATUS_DEFAULT;
	else
		ws_app_register_and_avtive_progess_start();
#endif

	return 0;
}
static int ws_app_push_restart_device(
	unsigned char *data,
	size_t len,
	unsigned char flag)
{
	system("reboot");
	return 0;
}
/*svc main*/
struct push_cmd_t push_cmds[] = {
	{ "push_change_contact", ws_app_push_change_contact },
	{ "push_enter_page", ws_app_push_enter_page },
	{ "push_exit_page", ws_app_push_exit_page },
	{ "push_bind_corp", ws_app_push_bind_corp },
	{ "push_change_userid", ws_app_push_change_userid },
	{ "push_update_firmware", ws_app_push_update_firmware },
	{ "push_admin_list_change", ws_app_push_admin_list_change },
	{ "push_user_face", ws_app_push_user_face_v1 },
	{ "push_visitor_list_change", ws_app_push_visitor_list_change },
	{ "push_unbind_corp", ws_app_push_unbind_corp },
	{ "push_user_scan", ws_app_push_user_scan },
	{ "push_fetch_log", ws_app_push_fetch_log },
	{ "push_restart_device", ws_app_push_restart_device }
};

struct req_id_t req_ids[] = {
	{ "lb_ping", ws_app_ping_cb },
	{ "lb_get_timestamp", ws_app_get_timestamp_cb },
	{ "lb_get_secret_no", ws_app_device_get_secret_no_cb },
	{ "lb_register", ws_app_device_register_cb },
	{ "lb_active", ws_app_device_active_cb },
	{ "lb_subscribe_corp", ws_app_device_subscribe_corp_cb },
	{ "lb_get_perm_version", ws_app_get_perm_version_cb },
	{ "lb_get_userinfo_by_page", ws_app_get_userinfo_by_page_cb },
	{ "lb_selfhelp_notify", ws_app_selfhelp_notify_cb },
	{ "lb_get_userinfo_by_ids", ws_app_get_userinfo_by_ids_cb },
	{ "lb_upload_userinfo", ws_app_upload_userinfo_cb_v1 },
	{ "lb_upload_userinfo_photo", ws_app_upload_userinfo_photo_cb },
	{ "lb_upload_userinfo_photo_e", NULL},
	{ "lb_checkin", ws_app_checkin_cb },
	{ "lb_get_admin_info", ws_app_get_admin_info_cb },
	{ "lb_get_visitor_list", ws_app_get_visitor_list_cb },
	{ "lb_report_user_temperature", ws_app_report_user_temperature_cb },
	{ "lb_upload_log", ws_app_upload_log_cb },
	{ "lb_error_report", ws_app_error_report_cb },
	{ "lb_get_firmware_info", ws_app_get_firmware_info_cb },
	{ "lb_unknow", NULL },
};

/**
 * @brief  all binary data process interface
 * @note   NULL
 * @param  *data:
 * @param  len:
 * @retval
 */
static int ws_app_handle_bin(unsigned char *data, size_t len)
{

	char req_id[REQ_ID_LEN] = { 0 };
	user_data_t *da, *da_tmp;
	int req_id_len = *(int *)&data[4];
	if (req_id_len >= REQ_ID_LEN)
		return -1;
	memset(req_id, 0, REQ_ID_LEN);
	memcpy(req_id, &data[8], req_id_len);
	req_id[req_id_len] = 0;
	/*foreach list*/
	pthread_mutex_lock(&websocket_data->user_data_lock);
	list_for_each_entry_safe(da, da_tmp, &websocket_data->user_data_list,
node)
	{
		if (0 == strcmp(da->media_id, req_id)) {
			snprintf(da->path, sizeof(da->path), "/tmp/%s.jpg",
				da->media_id);
			ALOGD("recg %s\n", da->path);
			ws_control_save_file(da->path,
					(char *)&data[12 + req_id_len],
					len - 12 - req_id_len);
			/*calc status*/
			da->status = user_status_calc_feats;
			break;
		}
	}
	pthread_mutex_unlock(&websocket_data->user_data_lock);
#if 0
	if (0 == strcmp("lb_download_photo", req_id)) {
		ws_app_download_photo_cb(&data[12 + req_id_len], len - 12 - req_id_len);
	}
#endif

	return 0;
}

/**
 * @brief  all text data process enterence
 * @note   NULL
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
static int ws_app_handle_text(unsigned char *data, size_t len,
							unsigned char flags)
{
	int ret = -1;
	int i = 0;
	cJSON *root = NULL;
	cJSON *headers = NULL;
	cJSON *cmd = NULL;
	char *req_id = NULL;
	char *push_cmd = NULL;
	int errcode = -1;
	char *errmsg = NULL;

	ALOGD("ws_app_handle_text enter\n");
	root = cJSON_Parse((const char *)data);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}
	/*errcode process*/
	headers = cJSON_GetObjectItem(root, "headers");
	if (NULL == headers) {
		ALOGE("cJSON_GetObjectItem headers failed\n");
		/*err global*/
		errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
		/*device not register not active not login*/
		if (errcode == ERROR_CODE_NOT_LOGIN ||
			errcode == ERROR_CODE_NOT_REGISTERED ||
			errcode == ERROR_CODE_NOT_ACTIVE) {
			if (websocket_data->register_thread_is_run)
				websocket_data->status = STATUS_DEFAULT;
			else
				ws_app_register_and_avtive_progess_start();
		}
			goto error;
	}
	cmd = cJSON_GetObjectItem(root, "cmd");
	if (cmd) {
		ALOGD("ws_app_handle_text push\n");
		push_cmd = cmd->valuestring;
		for (i = 0; i < (sizeof(push_cmds)/sizeof(struct push_cmd_t)); i++) {
			if (0 == strcmp(push_cmds[i].cmd, push_cmd)) {
				ALOGD("%s\n", push_cmd);
				if (push_cmds[i].fn) {
					push_cmds[i].fn(data, len, flags);
				}
				goto error;
			}
		}
	} else {
		ALOGD("ws_app_handle_text cb\n");
		errcode = cJSON_GetObjectItem(root, "errcode")->valueint;
		errmsg = cJSON_GetObjectItem(root, "errmsg")->valuestring;
		if (0 != errcode) {
			ALOGE("errcode = %d, errmsg = %s\n", errcode, errmsg);
			switch (errcode) {
				/*not auther*/
			case ERROR_CODE_NOT_AUTHOR:
				if (strcmp("123456", websocket_data->param.secret)) {
					strncpy(websocket_data->param.secret,
					"123456", 64);
					websocket_data->param.perm_version = 0;
					ws_control_set_param(&websocket_data->param);
					ws_control_unbind_operation();
					if (websocket_data->cb_fun)
						websocket_data->cb_fun(signal_unbind);
					else
						ws_control_ws_unbind();
					if (websocket_data->register_thread_is_run)
						websocket_data->status = STATUS_DEFAULT;
					else
					ws_app_register_and_avtive_progess_start();
				}
				goto error;
				/*invalid active code*/
			case ERROR_CODE_INVALID_ACTIVECODE:
				if (websocket_data->register_thread_is_run)
					websocket_data->status = STATUS_REGISTER;
				goto error;
			/*other*/
			case ERROR_CODE_NOT_REGISTERED:
			case ERROR_CODE_NOT_ACTIVE:
			case ERROR_CODE_NOT_LOGIN:
				if (websocket_data->register_thread_is_run)
					websocket_data->status = STATUS_DEFAULT;
				else
					ws_app_register_and_avtive_progess_start();
				goto error;
			}
			/* goto error;*/
		}
		req_id = cJSON_GetObjectItem(headers, "req_id")->valuestring;
		for (i = 0; i < (sizeof(req_ids)/sizeof(struct req_id_t)); i++) {
			if (0 == strcmp(req_ids[i].id, req_id)) {
				ALOGD("%s\n", req_id);
				if (req_ids[i].fn) {
					req_ids[i].fn(data, len, flags);
				}
				goto error;
			}
		}
	}
error:
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	ALOGD("ws_app_handle_text exit\n");

	return ret;
}

/**
 * @brief  websocket callback function
 * @note   NULL
 * @param  *data:
 * @param  len:
 * @param  flags:
 * @retval 0 success
 */
static int recv_hander(unsigned char *data, size_t len, unsigned char flags)
{
	switch (flags) {
	case WEBSOCKET_OP_TEXT:
		ALOGD("recv_hander text\n");
		ws_app_handle_text(data, len, flags);
		break;
	case WEBSOCKET_OP_BINARY:
		ALOGD("recv_hander bin\n");
		ws_app_handle_bin(data, len);
		break;
	default:
		break;
	}

	return 0;
}

/**
 * @brief  websocket connect success callback
 * @note   send link signal
 * @param  status:
 * @retval 0 success
 */
static int success_hander(int status)
{
	ALOGD("success_hander\n");
	if (websocket_data->link_status == 0) {
		websocket_data->link_discount = 0;
		if (websocket_data->cb_fun)
			websocket_data->cb_fun(signal_link);
		else
			ws_control_ws_link();
		websocket_data->link_status = 1;
		ws_app_register_and_avtive_progess_start();
	}

	return 0;
}

/**
 * @brief  reset_hander
 * @note   send dis link signal
 * @param  status:
 * @retval 0 success
 */
static int reset_hander(int status)
{
	ALOGD("reset_hander\n");
	if (websocket_data->websocket_exit) {
		ALOGD("%s:websocket_exit\n", __func__);
		return 0;
	}
	ws_app_error_proc();
	if (websocket_data->link_status) {
		websocket_data->link_status = 0;
		websocket_data->link_wechat = 0;
		if (websocket_data->cb_fun)
			websocket_data->cb_fun(signal_dislink);
		else
			ws_control_ws_dis_link();
		ALOGD("error_hander\n");
		ws_app_register_and_active_progess_stop();
	}
	return 0;
}

/**
 * @brief
 * @note
 * @param  status:
 * @retval
 */
static int error_hander(int status)
{
	if (websocket_data->websocket_exit) {
		ALOGD("%s:websocket_exit\n", __func__);
		return 0;
	}
	ws_app_error_proc();
	return 0;
}

/**
 * @brief wechat websokcet init
 * @note  alloc resources
 * @retval 0 success
 */
int ws_svc_app_init(void)
{
	void *ws_svc = NULL;
	websocket_client_param_t *param = NULL;

	websocket_data = calloc(sizeof(websocket_client_data_t), 1);
	if (NULL == websocket_data) {
		ALOGE("websocket_data is calloc failed\n");
		return -1;
	}
	/*register init*/
	websocket_data->status = STATUS_DEFAULT;
	websocket_data->status_last = STATUS_DEFAULT;
	/*user data init*/
	if (pthread_mutex_init(&websocket_data->user_data_lock, NULL)) {
		ALOGE("init user_data_lock failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		return -1;
	}
	/*user data list init*/
	INIT_LIST_HEAD(&websocket_data->user_data_list);
	/*init task*/
	if (pthread_create(&websocket_data->user_data_thread_id, NULL,
				thread_user_face_v1, NULL)) {
		ALOGE("create thread_user_face_v1 thread failed, errno: %d (%s)",
			errno, strerror(errno));
		return -1;
	}
	/*face init*/
	if (pthread_mutex_init(&websocket_data->face_down_lock, NULL)) {
		ALOGE("init face_down lock failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		return -1;
	}
	if (pthread_cond_init(&websocket_data->face_down_cond, NULL)) {
		ALOGE("init face_down cond failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		return -1;
	}
	/*checkin*/
	websocket_data->h_checkin = &websocket_data->head_checkin;
	if (pthread_mutex_init(&websocket_data->checkin_lock, NULL)) {
		ALOGE("init checkin_lock lock failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		return -1;
	}
	/*userinfo*/
	websocket_data->h_userinfo_person = &websocket_data->head_userinfo_person;
	websocket_data->h_userinfo_rule = &websocket_data->head_userinfo_rule;
	websocket_data->h_userinfo_person_ids = &websocket_data->head_userinfo_person_ids;
	websocket_data->h_userinfo_rule_ids = &websocket_data->head_userinfo_rule_ids;
	/*visitor*/
	websocket_data->h_visitor_person = &websocket_data->head_visitor_person;
	websocket_data->h_visitor_rule = &websocket_data->head_visitor_rule;

	param = &websocket_data->param;
	ws_control_get_param(param);
	ALOGD("sn = %s\n", param->sn);
	ALOGD("secret_no = %s\n", param->secret_no);
	ALOGD("device_id = %s\n", param->device_id);
	ALOGD("qrcode = %s\n", param->static_qrcode_url);
	ALOGD("secret = %s\n", param->secret);
	ALOGD("web_url = %s\n", param->websocket_url);

	ws_svc = ws_svc_init(param->websocket_url, strlen(param->websocket_url),
						 "protocol", strlen("protocol"));
	if (!ws_svc)
		return -1;
	ws_register_recv_cb(recv_hander);
	ws_register_nc_success_cb(success_hander);
	ws_register_nc_reset_cb(reset_hander);
	ws_register_nc_error_cb(error_hander);

	ws_app_ping(30);
	ws_app_register_get_timestamp(3600 * 12);
	/*ws_app_register_get_firmware_info(3600 * 3);*/
	ws_app_create_dir(OTA_PATH);
	return 0;
}

/**
 * @brief  set websocket status
 * @note
 * @param  link:  0 link  1 not link
 * @retval 0 success
 */
int ws_svc_app_set_netlink(int link)
{
	if (link) {
		ws_svc_set_net_linked();
	} else {
		ws_svc_set_net_not_linked();
	}

	return 0;
}

/**
 * @brief  set websocket reconnect
 * @note
 * @retval None
 */
void ws_svc_app_net_reconnect(void)
{
	ws_svc_set_net_reconnect();
}

/**
 * @brief  deinit websocket
 * @note
 * @retval
 */
int ws_svc_app_deinit(void)
{
	if (websocket_data) {
		websocket_data->websocket_exit = 1;
		websocket_data->face_thread_is_run = 0;
		pthread_join(websocket_data->user_data_thread_id, NULL);
		ws_app_register_and_active_progess_stop();
		ws_svc_deinit();
		free(websocket_data);
		websocket_data = NULL;
	}

	return 0;
}

/**
 * @brief  generate qrcode for login
 * @note   generata login qrcode by dyncmic
 * @param  *path:
 * @retval
 */
int ws_svc_app_qrcode(char *path)
{
	if (NULL == websocket_data) {
		ALOGE("%s:websocket_data is null\n", __func__);
		return -1;
	}
	if (websocket_data->websocket_exit) {
		ALOGE("%s:websocket_exit is 1\n", __func__);
		return -1;
	}

	return ws_app_device_dynamic_login_qrcode(
		websocket_data->param.sn,
		websocket_data->param.secret_no,
		path);
}

/**
 * @brief  generate qrcode for bind
 * @note   generate qrcode for bind  dynamicly
 * @param  *path:
 * @retval
 */
int ws_svc_app_qrcode_bind(char *path)
{
	if (NULL == websocket_data) {
		ALOGE("%s:websocket_data is null\n", __func__);
		return -1;
	}
	if (websocket_data->websocket_exit) {
		ALOGE("%s:websocket_exit is 1\n", __func__);
		return -1;
	}
	return ws_app_device_dynamic_bind_qrcode(
		websocket_data->param.sn,
		websocket_data->param.secret_no,
		path);
	/*return qrencode_create(websocket_data->param.static_qrcode_url, path);*/
}

/**
 * @brief get websocket linkstatus
 * @note
 * @retval 0 notlink  1 link
 */
int ws_svc_app_ws_is_link(void)
{
	if (websocket_data)
		return websocket_data->link_status;

	return 0;
}

/**
 * @brief
 * @note
 * @param  cb:
 * @retval None
 */
void ws_svc_app_set_cb(ws_svc_call_back cb)
{
	if (websocket_data)
		websocket_data->cb_fun = cb;
}

/**
 * @brief get wechat link status
 * @note
 * @retval 0 notlink  1 linked
 */
int ws_svc_app_wechat_is_link(void)
{
	if (websocket_data)
		return websocket_data->link_status;

	return 0;
}

