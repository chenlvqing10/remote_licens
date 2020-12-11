/*
 * web_biz_api.c - all functions for RESTful.
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

#include "mongoose.h"
#include "cJSON.h"
#include "time_conv.h"
#include "person.h"
#include "file_ops.h"
#include "time_conv.h"
#include "face.h"
#include "web_biz_api.h"
#include "ui_control.h"
#include "db_sqlite_middle.h"
#include "lombo_system_sound.h"
#include "ws_control.h"
#include "update_svc.h"
#include "open_door_svc.h"
#include "base64.h"
#include "http_client.h"
#include "ui_control.h"
#include "wifi_station.h"
#include "db_all.h"

#define LOG_TAG "web_biz_api"
#include <log/log.h>

#define UPGRADE_PKG_PATH		"/data/"
#define UPGRADE_PKG_FILE		"/data/EUPGRADE_temp.pkg"
#define TMP_LOGO_PIC_FILE		"/data/upload_logo.jpg"
#define TMP_SNAP_FACE_FILE		"/data/snap_face.jpg"
#define ADVERTS_PICS_PATH		"/data/advert"
#define SYSTEM_LOG_FILE		"/data/log/system.log"
#define MEMBER_PICS_PATH		"/data/member"

#define REG_PIC_DEFAULT_PATH		"/tmp/reg_default.jpg"

#define M_SIZE	64

#define CHUNKED_HEAD_FMT	"HTTP/1.1 %d\r\nTransfer-Encoding: chunked\r\n\r\n"
#define CHUNKED_ANS_FMT		"{ \"message\": \"%s\",\"result\":%d }"

#define PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret)	do { \
	if (!nc || !p || MG_EV_HTTP_REQUEST != ev) \
		return; \
	struct http_message *_http = (struct http_message *)p; \
	if (!_http) { \
		ALOGW("_http is null\n"); \
		ret = -2; \
		break; \
	} \
	root = cJSON_Parse(_http->body.p); \
	if (!root) { \
		ALOGE("cJSON_Parse root failed\n"); \
		ret = -2; \
		break; \
	} \
	pwd = cJSON_GetObjectItem(root, "password"); \
	if (!pwd) { \
		ALOGE("cJSON_Parse passwod failed\n"); \
		ret = -3; \
		break; \
	} \
	ret = is_pwd_matching(pwd->valuestring); \
} while (0)

#define UPDATA_DB(arr, ret)	do { \
	ret = db_device_hw_param_update(&arr); \
	if (SQLITE_OK != ret) { \
		ALOGE("%s:%d >> updata db failed", __func__, __LINE__); \
		return -1; \
	} \
	ret = 0; \
} while (0)

#define UPDATA_HANDLER(arr, ret, handler)	do { \
	ret = db_device_hw_param_update(&arr); \
	if (SQLITE_OK != ret) { \
		ALOGE("%s:%d >> updata db failed", __func__, __LINE__); \
		return -1; \
	} \
	ret = handler(); \
	if (ret) { \
		ALOGE("%s:%d >> %s failed", __func__, __LINE__, #handler); \
		return -2; \
	} \
	ret = 0; \
} while (0)

#define obj_create(name)  cJSON *_obj_##name = cJSON_CreateObject(); \
							if (!_obj_##name) {ALOGW("cjson create %s failed", #name ); ret = -100; }

#define obj_format(name, buf) if (_obj_##name) {\
									 buf = cJSON_PrintUnformatted(_obj_##name); \
									 if (!buf) {ALOGW("cjson format %s failed", #name ); ret = -101;} \
								}
#define obj_get(name) _obj_##name
#define obj_ref(name, ref)  cJSON *_obj_##name = (ref);

#define obj_get_string(name, field, str) do {cJSON * obj = cJSON_GetObjectItem(obj_get(name), field); \
												(str) = (obj)?obj->valuestring:0;}while(0)

#define obj_get_num(name, field, num) do {cJSON * obj = cJSON_GetObjectItem(obj_get(name), field); \
												(num) = (obj)?obj->valuedouble:0;}while(0)



	/*delete all sub object*/
#define obj_free(name)  if (_obj_##name)cJSON_Delete(_obj_##name);

#define obj_add_string(name, field, str)  do{if (_obj_##name)cJSON_AddStringToObject(_obj_##name, field, str);}while(0)

#define obj_add_num(name, field, num)  do{if (_obj_##name)cJSON_AddNumberToObject(_obj_##name, field, num);}while(0)

#define obj_add_item(name, field, item) do{if (_obj_##name)cJSON_AddItemToObject(_obj_##name, field, item); }while(0)


#define array_create(name)  cJSON *_arr_##name = cJSON_CreateArray(); \
							if (!_arr_##name) {ALOGW("cjson create %s failed", #name ); ret = -100;}

#define array_format(name, buf) if (_arr_##name) {\
															 buf = cJSON_PrintUnformatted(_arr_##name); \
															 if (!buf) {ALOGW("cjson format %s failed", #name ); ret = -101;} \
														}

	/*delete all sub object*/
#define array_free(name)  if (_arr_##name)cJSON_Delete(_arr_##name);


#define array_add_string(name,  str) do{if (_arr_##name) {cJSON str_ = cJSON_CreateString(str); \
											cJSON_AddItemToArray(_arr_##name, str_);} }while(0)

#define array_add_int(name,  num)    do{if (_arr_##name) {cJSON num_ = cJSON_CreateNumber(num); \
											cJSON_AddItemToArray(_arr_##name, num_);} }while(0)

#define array_add_item(name, item)  do{ if (_arr_##name) {cJSON_AddItemToArray(_arr_##name, item);} }while(0)


#define send_resp(err_msg)  	if (ret == 0) { mg_printf(nc, CHUNKED_HEAD_FMT, 200);\
									if (buf){mg_printf_http_chunk(nc, "%s", buf);}\
									else {mg_printf_http_chunk(nc, "{}");}\
									mg_send_http_chunk(nc, "", 0); ALOGD("%s done ",__func__ );\
									}else\
									{ans_handle_err(nc, err_msg, ret); ALOGW("%s error:%s ", __func__, err_msg );}\
									if (buf) {free(buf); buf = 0;};

extern int ota_start(void);
extern int net_get_info(char *ip, int i_size,
  char *mac, int m_size);
extern int get_eth_para(net_config_para_t *p);
extern int sync_set_eth_para(net_config_para_t *p);
extern int web_set_wifi_state(int enable);
extern int sync_get_scan_list(SCAN_INFO *p_ret);
extern void get_wifi_para(wifi_config_para_t *p);
extern void get_current_ssid(char *ssid);
extern int sync_connect_network(char *p_ssid, char *p_pwd,
	int security_type);
extern int sync_set_wifi_enabled(char on);
extern int web_connect_wifi(char *p_ssid, char *p_pwd,
	int security_type, int *failed_reason);
extern void save_wifi_config();
web_handler_cb m_cb;

static int cb_flag = 0;

void web_handle_cb(web_handler_cb cb)
{
	m_cb = cb;
}

/**
* truncate_file: clean file
* path: file path
*
* Returns 0: success, -1: failure.
*/
static int truncate_file(const char *path)
{
	int fd = 0;
	fd = open(path, O_WRONLY|O_TRUNC, 0666);
	if (fd <= 0) {
		ALOGE("open file error.\n");
		return -1;
	}

	ftruncate(fd, 0);
	lseek(fd, 0, SEEK_SET);

	fsync(fd);
	close(fd);
	return 0;
}

/**
* is_pwd_matching: is passwork matching
* pic: jpg data
*
* Returns 0: matching, -1: failure.
*/
static int is_pwd_matching(char *password)
{
	char pwd[M_SIZE] = {0};
	char md5_pwd[M_SIZE] = {0};
	int ret = 0;

	if (NULL == password || strlen(password) == 0) {
		ALOGW("post password is null!\n");
		return -1;
	}

	if (strlen(pwd) == 0) {
		ret = ui_control_get_password(pwd, sizeof(pwd));
		if (ret) {
			ALOGE("Get password failed!\n");
			return -1;
		}
	}

	ret = get_md5_string(password, md5_pwd, sizeof(md5_pwd));

	if (ret == 0 && strcmp(pwd, md5_pwd) == 0)
		return 0;
	else {
		ALOGW("password is error!\n");
		return -1;
	}
}

/**
* ans_handle_ok: answer server handle ok
*
*/
static void ans_handle_ok(struct mg_connection *nc,
	const char *msg, int code)
{
	ALOGD("%s: answer ok: %s[%d]", __func__, msg, code);
	mg_printf(nc, CHUNKED_HEAD_FMT, 200);
	mg_printf_http_chunk(nc, CHUNKED_ANS_FMT, msg, code);
	mg_send_http_chunk(nc, "", 0);
}

/**
* ans_handle_err: answer server handle fail
*
*/
static void ans_handle_err(struct mg_connection *nc,
	const char *msg, int err_code)
{
	ALOGW("%s: answer error: %s[%d]", __func__, msg, err_code);
	mg_printf(nc, CHUNKED_HEAD_FMT, 400);
	mg_printf_http_chunk(nc, CHUNKED_ANS_FMT, msg, err_code);
	mg_send_http_chunk(nc, "", 0);
}

static int reset_screen_display()
{
	int ret = 0;
	screen_bright_t scr_bright;
	screen_off_status_t scr_status;

	ALOGD("%s: reset screen display.", __func__);
	memset(&scr_bright, 0, sizeof(screen_bright_t));
	memset(&scr_status, 0, sizeof(screen_off_status_t));

	screen_svc_deinit();

	ret = ui_device_get_screen_param(&scr_bright, &scr_status);
	if (ret) {
		ALOGE("ui_device_get_screen_param failed");
		return -1;
	}
	ret = screen_svc_init(&scr_bright, &scr_status);
	if (ret) {
		ALOGE("init screen svc failed");
		return -1;
	}
	return 0;
}

static int reset_fill_light()
{
	int ret = 0;
	fill_light_param_t fl_param;

	ALOGD("%s: reset fill light.", __func__);\
	memset(&fl_param, 0, sizeof(fill_light_param_t));
	filling_light_deinit();
	ret = filling_light_init(&fl_param);
	if (ret) {
	  ALOGE("init fill light failed");
	  return -1;
	}
	return 0;
}

static int web_set_dev_params_imp(cJSON *data)
{
	int ret = 0;
	cJSON *tmp = NULL;
	device_hw_param_t arr;

	memset(&arr, 0, sizeof(device_hw_param_t));
	ret = db_device_hw_param_query(&arr);
	if (ret) {
		ALOGE("%s: get device hw params failed!", __func__);
		return -1;
	}

	cJSON_ArrayForEach(tmp, data) {
		/* support device controler areas */
		if (strcmp("volume", tmp->string) == 0) {
			ALOGD("set volume :%d", tmp->valueint);
			arr.volume = tmp->valueint;
			UPDATA_DB(arr, ret);
			lombo_set_master_volume(arr.volume);

		} else if (strcmp("auto_display_brightness", tmp->string) == 0) {
			ALOGD("set auto_display_brightness :%d", tmp->valueint);
			arr.auto_display_brightness = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_screen_display);
		} else if (strcmp("display_brightness", tmp->string) == 0) {
			ALOGD("set display_brightness :%d", tmp->valueint);
			arr.display_brightness = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_screen_display);
		} else if (strcmp("auto_screen_off", tmp->string) == 0) {
			ALOGD("set auto_screen_off :%d", tmp->valueint);
			arr.auto_screen_off = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_screen_display);
		} else if (strcmp("auto_screen_off_timeout", tmp->string) == 0) {
			ALOGD("set auto_screen_off_timeout :%d", tmp->valueint);
			arr.auto_screen_off_timeout = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_screen_display);
		} else if (strcmp("auto_screen_saver_off", tmp->string) == 0) {
			ALOGD("set auto_screen_saver_off :%d", tmp->valueint);
			arr.auto_screen_saver_off = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_screen_display);
		} else if (strcmp("auto_screen_saver_timeout", tmp->string) == 0) {
			ALOGD("set auto_screen_saver_timeout :%d", tmp->valueint);
			arr.auto_screen_saver_timeout = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_screen_display);

		} else if (strcmp("infrared_light_set", tmp->string) == 0) {
			ALOGD("set infrared_light_set :%d", tmp->valueint);
			arr.infrared_light_set = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_fill_light);
		} else if (strcmp("light_sensitive", tmp->string) == 0) {
			ALOGD("set light_sensitive :%d", tmp->valueint);
			arr.light_sensitive = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_fill_light);
		} else if (strcmp("light_high_brightness", tmp->string) == 0) {
			ALOGD("set light_high_brightness :%d", tmp->valueint);
			arr.light_high_brightness = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_fill_light);
		} else if (strcmp("light_low_brightness", tmp->string) == 0) {
			ALOGD("set light_low_brightness :%d", tmp->valueint);
			arr.light_low_brightness = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_fill_light);
		} else if (strcmp("light_det_interval", tmp->string) == 0) {
			ALOGD("set light_det_interval :%d", tmp->valueint);
			arr.light_det_interval = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_fill_light);
		} else if (strcmp("light_delay_time", tmp->string) == 0) {
			ALOGD("set light_delay_time :%d", tmp->valueint);
			arr.light_delay_time = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_fill_light);
		} else if (strcmp("gpio", tmp->string) == 0) {
			ALOGD("set gpio :%d", tmp->valueint);
			arr.gpio = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_fill_light);
		} else if (strcmp("human_induction", tmp->string) == 0) {
			/* unused this attribute */
			ALOGD("set human_induction :%d", tmp->valueint);
			arr.human_induction = tmp->valueint;
			UPDATA_HANDLER(arr, ret, reset_fill_light);

		} else if (strcmp("device_name", tmp->string) == 0) {
			device_system_info_t dev_info = {0};
			ALOGD("set device_name :%s", tmp->valuestring);
			memcpy(arr.device_name, tmp->valuestring, sizeof(arr.device_name));
			if (get_sys_info(&dev_info) != 0) {
				ALOGE("Get sys info failed!\n");
				return -1;
			}
			memcpy(dev_info.device_name, arr.device_name, sizeof(arr.device_name));
			ret = db_device_system_info_update(&dev_info);
			if (SQLITE_OK != ret) {
				ALOGE("%s:%d >> updata db failed", __func__, __LINE__);
				return -1;
			}
		}
	}

	return ret;
}

static int upload_file_local(struct mg_connection *nc, int ev,
	struct mg_http_multipart_part *mp, char *file)
{
	static FILE *flip = NULL;
	int ret = 0;

	switch (ev) {
	case MG_EV_HTTP_MULTIPART_REQUEST:
	{
		struct mg_str query_string;
		char pwd[M_SIZE] = {0};
		const char *s = mp->file_name;
		const char *end = s;
		/* ALOGD("upload_file_local >> file_name:%s", mp->file_name); */
		while (*end != '\n') end++;
		s = mg_skip(s, end, "?", &query_string);
		s = mg_skip(s, end, " ", &query_string);
		mg_get_http_var(&query_string, "password", pwd, sizeof(pwd));
		ALOGD("upload_file_local >> pwd:%s", pwd);
		ret = is_pwd_matching(pwd);
		if (ret)
			return ret;
	}
		break;

	case MG_EV_HTTP_PART_BEGIN:
		if (0 == strcmp(mp->var_name, "file") && !flip) {
			if (access(file, 0) == 0)
				truncate_file(file);
			flip = fopen(file, "wb");
			if (!flip)
				ALOGE("[%s]open %s failed\n", __func__, file);
		}
		break;

	case MG_EV_HTTP_PART_DATA:
		if (0 == strcmp(mp->var_name, "file") && flip) {
			ret = fwrite(mp->data.p, 1, mp->data.len, flip);
			if (ret != mp->data.len) {
				ALOGE("write file(%s) failed, size:%d != write_count:%d\n",
					file, mp->data.len, ret);
				ret = -1;
			} else {
				ret = 0;
			}
		}
		break;

	case MG_EV_HTTP_PART_END:
		if (0 == strcmp(mp->var_name, "file") && flip) {
			fflush(flip);
			fclose(flip);
		}
		break;

	case MG_EV_HTTP_MULTIPART_REQUEST_END:
		if(flip){
			flip = NULL;
		} else {
			ret = -1;
		}
		break;

	default:
		ALOGW("%s:%d >> do not found ev=%d",
			__func__, __LINE__, ev);
		break;
	}
	return ret;
}

static int web_add_member_imp(person_t *pst_person,
	person_reg_param_t *reg_param) {
	int ret;
	img_info_t st_img;
	char e_msg[128] = {0};
	char detail_info[64] = {0};
	e_person_reg_code e_code = PERSON_REG_OK;

	/* to config the args of face recg */
	reg_param->check_face_size = 0;
	reg_param->face_max = 512;
	reg_param->face_min = 120;
	reg_param->quality = 0;
	reg_param->threshold = 0.5f;
	reg_param->refresh_db_each_time = 0;
	reg_param->jpeg_width_stride = 800;
	reg_param->jpeg_height_stride = 1024;
	reg_param->batch_reg = 0;

	memset(&st_img, 0, sizeof(img_info_t));
	strcpy(st_img.pic_path, pst_person->regist_photo_name);
	memset(detail_info, 0, sizeof(detail_info));
	ret = person_reg_proc(pst_person, &st_img, detail_info,
		sizeof(detail_info), reg_param, &e_code, 1);

	if (ret) {
		memset(e_msg, 0, sizeof(e_msg));
		person_reg_fail_reason(e_code, e_msg, sizeof(e_msg));
		ALOGD("[%s]person_reg_proc fail.msg[%d]=%s\n", __func__,
			e_code, e_msg);
		ret = -3;
	} else {
		ALOGD("[%s]person_reg_proc ok.\n", __func__);
		ret = face_db_update();
		if (ret) {
			ALOGW("[%s:%d]update db failed!", __func__, __LINE__);
			ret = -4;
		}
	}

	remove_pic_by_path(st_img.pic_path);
	if (m_cb)
		m_cb(WEB_REGISTER, ret);
	return ret;
}

/*
*  record json: {"start_time":1538323200,"end_time":1565020799,"data":[{"userid":"123","user_type":0,"timestamp":1560270310},...]}
*/
int do_upload_record_revert(long start, long end)
{
	int ret = 0;
	ee_queue_t _head;
	ee_queue_t *h_checkin = NULL;
	web_socket_checkin_info_t *p = NULL;
	cJSON *root = NULL;
	cJSON *checkin_data = NULL;
	cJSON *checkin_item = NULL;
	char *request = NULL;
	char buf[32] = {0};

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}

	h_checkin = &_head;
	ret = ws_control_get_checkin_data_by_time(
		h_checkin, start, end);
	if (ret == 0) {
		ALOGI("%s:no checkin data\n", __func__);
		h_checkin = NULL;
		goto error;
	}
	ret = 0;
	checkin_data = cJSON_CreateArray();
	if (NULL == checkin_data) {
		ALOGE("malloc body failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddNumberToObject(root, "start_time", start);
	cJSON_AddNumberToObject(root, "end_time", end);
	cJSON_AddItemToObject(root, "data", checkin_data);
#if 0
	ui_control_get_sn(buf, sizeof(buf));
	cJSON_AddStringToObject(root, "dev_sn", buf);
#else
	char ip[32] = {0};
	net_get_info(ip, sizeof(ip), buf, sizeof(buf));
	cJSON_AddStringToObject(root, "mac_addr", buf);
#endif
	ee_queue_foreach(p, h_checkin,
					 web_socket_checkin_info_t, queue) {
		checkin_item = cJSON_CreateObject();
		if (NULL == checkin_item) {
			ALOGE("malloc body failed, errno: %d (%s)\n",
				  errno, strerror(errno));
			ret = -1;
			goto error;
		}
		cJSON_AddItemToArray(checkin_data, checkin_item);
		cJSON_AddStringToObject(checkin_item, "userid", p->user_id);
		cJSON_AddNumberToObject(checkin_item, "user_type", p->user_type);
		cJSON_AddNumberToObject(checkin_item, "timestamp", p->timestamp);
	}
	ws_control_put_checkin_data(h_checkin);
	h_checkin = NULL;

	request = cJSON_PrintUnformatted(root);
	if (!request) {
		ALOGW("%s >> get json format fail!\n", __func__);
		ret = -2;
		goto error;
	}
	ALOGD("%s:send json >> %s\n", __func__, request);

	//todo send remote server, eg:
	//ret = ws_send_text((unsigned char *)request, strlen(request));
	if (ret) {
		ALOGW("%s >> send json to remote server fail!\n", __func__);
		ret = -3;
		goto error;
	}
	cJSON_Delete(root);
	free(request);
	request = NULL;
	return ret;

error:
	if (request) {
		free(request);
		request = NULL;
	}

	if (h_checkin) {
		ws_control_put_checkin_data(h_checkin);
		h_checkin = NULL;
	}

	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	return ret;

}

void client_req_cb (int state, const char *req, int req_len,
	const char *ans, int ans_len) {
	if (0 == state) {
		ALOGD("http >> %s: send req[%s] return ans[%s]",
			__func__, req, ans);
		cb_flag = 1;
	} else if (-2 == state){
		ALOGW("http >> %s: send req[%s] exit", __func__, req);
		cb_flag = -1;
	} else {
		ALOGW("http >> %s: send req[%s] error", __func__, req);
		cb_flag = -2;
	}
}

int httpclient_send_sync(const char *url,
	unsigned char *data, size_t len) {
	int ret = 0;
	static int running = 0;
	struct mg_mgr *g_mgr = NULL;
	struct http_hdl *hdl = NULL;

	while(running != 0) {
		usleep(100 * 1000);
		ALOGD("%s: wait last ota post callback", __func__);
	}
	ALOGD("%s: start httpclient send", __func__);
	running = 1;

	g_mgr = calloc(sizeof(struct mg_mgr), 1);
	if (g_mgr == NULL) {
		ALOGE("calloc mg_mgr failed\n");
		ret = -1;
		goto error;
	}
	hdl = calloc(sizeof(struct http_hdl), 1);
	if (hdl == NULL) {
		ALOGE("calloc http_hdl failed\n");
		ret = -1;
		goto error;
	}
	hdl->cb = client_req_cb;
	http_client_init(g_mgr, hdl);
	http_client_send_Req(g_mgr, url,(const char *)data);
	while(cb_flag == 0)
		http_client_poll(g_mgr);
	if(1 != cb_flag)
		ret = -1;
	http_client_close(g_mgr);

error:
	if (g_mgr) {
		free(g_mgr);
		g_mgr = NULL;
	}
	if (hdl) {
		free(hdl);
		hdl = NULL;
	}
	cb_flag = 0;
	running = 0;
	ALOGD("%s: end httpclient send:%d", __func__, ret);
	return ret;
}

 /**
 * @brief send checkin data
 * @note  read checkin data from db and send to server
 * max 199 once
 * @retval
 * {"cmd":"checkin","dev_sn":"**********","data_len":3,"checkin_data":[{"userid":"123","user_type":0,"timestamp":1595745570,"name":"hym","temperature":36.23,"face_base64":"**"},{...}]}
 */
int client_send_checkin(const char *url)
{
#if 0
	 httpclient_send_sync("http://www.baidu.com/", NULL, 0);
	 return 0;
#endif
	int ret = 0, cnt = 0;
	cJSON *root = NULL;
	cJSON *checkin_data_array = NULL;
	cJSON *checkin_data = NULL;
	char *request = NULL;
	web_socket_checkin_info_t *p = NULL;
	ee_queue_t head_checkin;
	ee_queue_t *h_checkin = &head_checkin;
	char *file_base64 = NULL;
	char buf[32] = {0};

	ee_queue_init(h_checkin);
	cnt = ws_control_get_checkin_data(h_checkin);
	if (cnt == 0) {
		ALOGE("%s:no checkin data\n", __func__);
		return 0;
	}
	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
	cJSON_AddStringToObject(root, "cmd", "checkin");
	checkin_data_array = cJSON_CreateArray();
	if (NULL == checkin_data_array) {
		ALOGE("malloc checkin_data_array failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto error;
	}
#if 0
	ui_control_get_sn(buf, sizeof(buf));
	cJSON_AddStringToObject(root, "dev_sn", buf);
#else
	char ip[32] = {0};
	net_get_info(ip, sizeof(ip), buf, sizeof(buf));
	cJSON_AddStringToObject(root, "mac_addr", buf);
#endif
	cJSON_AddNumberToObject(root, "data_len", cnt);
	cJSON_AddItemToObject(root, "checkin_data", checkin_data_array);
	ee_queue_foreach(p, h_checkin,
					 web_socket_checkin_info_t, queue) {
		checkin_data = cJSON_CreateObject();
		if (NULL == checkin_data) {
			ALOGE("malloc body failed, errno: %d (%s)\n",
				  errno, strerror(errno));
			ret = -1;
			goto error;
		}
		cJSON_AddItemToArray(checkin_data_array, checkin_data);
		cJSON_AddStringToObject(checkin_data, "userid", p->user_id);
		cJSON_AddNumberToObject(checkin_data, "user_type", p->user_type);
		cJSON_AddNumberToObject(checkin_data, "timestamp", p->timestamp);
		cJSON_AddStringToObject(checkin_data, "name", p->name);
		cJSON_AddNumberToObject(checkin_data, "temperature", p->temperature);
		file_base64 = file2base(p->face_path);
		if (file_base64) {
			cJSON_AddStringToObject(checkin_data, "face_base64", file_base64);
			free_base64data(file_base64);
		} else {
			ALOGW("%s >> file2base[%s] errpr!\n", __func__, p->face_path);
			cJSON_AddStringToObject(checkin_data, "face_base64","");
		}
	}
	request = cJSON_PrintUnformatted(root);
	if (!request) {
		ALOGE("format json failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -3;
		goto error;
	}

	ret = httpclient_send_sync(url, (unsigned char *)request, strlen(request));
	if (0 == ret) {
		ee_queue_foreach(p, h_checkin,
						 web_socket_checkin_info_t, queue) {
			p->is_upload = 1;
		}
		ws_control_del_checkin_data(h_checkin);
	}

error:
	if (request) {
		free(request);
		request = NULL;
	}
	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	ws_control_put_checkin_data(h_checkin);

	return ret;
}

#if 0
/*
 * url: http://deviceAddress:port/handleCall
 * recv json data: {"password":"123456"}
 * return json data: {"message": "handle success","result":0}
 */
void lb_web_handle_call(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	/* todo some things*/

	/* return handler result */
	if (!ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "handle success", 0);
		return;
	} else {
		ALOGW("handle failed\n");
		ret = -3;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "handle fail", ret);
}

void lb_web_handle_by_js_obs(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	char password[M_SIZE] = {0};
	struct http_message *p_http_msg = (struct http_message *)p;

	if (!nc || !p || MG_EV_HTTP_REQUEST != ev)
		return;
	mg_get_http_var(&p_http_msg->body, "password", password,
		sizeof(password));
	ret = is_pwd_matching(password);
	if (ret) {
		ALOGW("passwork do not math!\n");
		goto err;
	}
	/* todo some things*/

	/* return handler result */
	if (!ret) {
		ans_handle_ok(nc, "handle success", 0);
		return;
	}
err:
	ans_handle_err(nc, "handle fail", ret);
}
#endif

/*
 * url: http://deviceAddress:port/deviceLogin
 * recv json data: {"username":"admin","password":"123456"}
 * return json data: {"message":"Login success","result":0}
 */
void lb_web_login(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *name = NULL;
	//device_system_info_t dev_info = {0};

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	name = cJSON_GetObjectItem(root, "username");
	if (!name) {
		ALOGE("cJSON_Parse username failed\n");
		ret = -3;
		goto err;
	}
	//if (get_sys_info(&dev_info) != 0) {
	//	ALOGE("Get sys info failed!\n");
	//	ret = -4;
	//	goto err;
	//}
	//if (strcmp(name->valuestring, dev_info.device_name) == 0) {
	if (strcmp(name->valuestring, "admin") == 0) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Login success", 0);
		return;
	} else {
		ALOGW("device_name error: %s!\n", name->valuestring);
		ret = -5;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Login fail", ret);
}

/*
 * url: http://deviceAddress:port/heartBeat
 * recv json data: {"password":"123456","currentTime":"adbuabsdr","ip":"192.168.10.100","personCount":10}
 * return json data: {"message":"Set dev hearbeat success","result":0}
 */
void lb_web_set_dev_hearbeat(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	if (m_cb)
		m_cb(WEB_PLATFORM_HEARBEAT, 1);

	/* return handler result */
	if (!ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Set dev hearbeat success", 0);
		return;
	} else {
		ALOGW("Set dev hearbeat failed\n");
		ret = -3;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set dev hearbeat fail", ret);
}

/*
 * url: http://deviceAddress:port/getDeviceParameter
 * recv json data: {"password":"123456"}
 * return json data: {"data":{"device_name":"E3","volume":80,...},"message":"Get params success","result":0}
 */
void lb_web_get_dev_params(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	char *buf;
	cJSON *root = NULL, *pwd = NULL;
	cJSON *obj = NULL, *data = NULL;
	device_hw_param_t arr;
	char id[32]= {0};

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -2;
		goto err;
	}
	data = cJSON_CreateObject();
	if (!data) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -2;
		goto err;
	}

	memset(&arr, 0, sizeof(device_hw_param_t));
	if (db_device_hw_param_query(&arr) == 0) {
		cJSON_AddStringToObject(data, "device_name", arr.device_name);
		cJSON_AddNumberToObject(data, "volume", arr.volume);
		cJSON_AddNumberToObject(data, "auto_display_brightness", arr.auto_display_brightness);
		cJSON_AddNumberToObject(data, "display_brightness", arr.display_brightness);
		cJSON_AddNumberToObject(data, "auto_screen_off", arr.auto_screen_off);
		cJSON_AddNumberToObject(data, "auto_screen_off_timeout", arr.auto_screen_off_timeout);
		cJSON_AddNumberToObject(data, "auto_screen_saver_off", arr.auto_screen_saver_off);
		cJSON_AddNumberToObject(data, "auto_screen_saver_timeout", arr.auto_screen_saver_timeout);
		cJSON_AddNumberToObject(data, "infrared_light_set", arr.infrared_light_set);
		cJSON_AddNumberToObject(data, "light_sensitive", arr.light_sensitive);
		cJSON_AddNumberToObject(data, "light_high_brightness", arr.light_high_brightness);
		cJSON_AddNumberToObject(data, "light_low_brightness", arr.light_low_brightness);
		cJSON_AddNumberToObject(data, "light_det_interval", arr.light_det_interval);
		cJSON_AddNumberToObject(data, "light_delay_time", arr.light_delay_time);
		cJSON_AddNumberToObject(data, "gpio", arr.gpio);
		cJSON_AddNumberToObject(data, "human_induction", arr.human_induction);
	}
	cJSON_AddItemToObject(obj, "data", data);
	cJSON_AddStringToObject(obj, "message", "Get params success");
#if 0
	ui_control_get_sn(id, sizeof(id));
	cJSON_AddStringToObject(obj, "dev_sn", id);
#else
	char ip[32] = {0};
	net_get_info(ip, sizeof(ip), id, sizeof(id));
	cJSON_AddStringToObject(obj, "mac_addr", id);
#endif
	cJSON_AddNumberToObject(obj, "result", 0);
	buf = cJSON_PrintUnformatted(obj);
	if (!buf) {
		ALOGW("%s >> get json format fail!\n", __func__);
		ret = -3;
		goto err;
	}
	cJSON_Delete(obj);
	cJSON_Delete(root);
	mg_printf(nc, CHUNKED_HEAD_FMT, 200);
	mg_printf_http_chunk(nc, "%s", buf);
	mg_send_http_chunk(nc, "", 0);
	free(buf);
	return;
err:
	if (obj)
		cJSON_Delete(obj);
	if (root)
		cJSON_Delete(root);
	if (-2 == ret) {
		mg_printf(nc, CHUNKED_HEAD_FMT, 501);
		mg_printf_http_chunk(nc, CHUNKED_ANS_FMT, "create json fail", ret);
		mg_send_http_chunk(nc, "", 0);
	} else if (-3 == ret) {
		mg_printf(nc, CHUNKED_HEAD_FMT, 501);
		mg_printf_http_chunk(nc, CHUNKED_ANS_FMT, "get json format fail", ret);
		mg_send_http_chunk(nc, "", 0);
	} else
		ans_handle_err(nc, "Get params fail", ret);
}

/*
 * url: http://deviceAddress:port/setDeviceParameter
 * recv json data: {"password":"123456","data":{"device_name":"AA","display_brightness":50}}
 * return json data: { "message": "Set param success","result":0 }
 */
void lb_web_set_dev_params(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *data = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	data = cJSON_GetObjectItem(root, "data");
	if (!data) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	ret = web_set_dev_params_imp(data);
	if (0 == ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Set param success", 0);
		return;
	} else {
		ALOGW("web_set_dev_params_imp failed\n");
		ret = -3;
	}
err:
	if(root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set param fail", ret);
}

/*
 * url: http://deviceAddress:port/restoreDeviceDefaultParameter
 * recv json data: {"password":"123456"}
 * return json data: { "message": "Restore device params success","result":0 }
 */
void lb_web_restore_dev_params(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	char *cmd = "sqlite3 /data/user.db \"delete from sysinfo; \
attach database '/etc/user.db' as source1; \
insert into sysinfo select * from source1.sysinfo;\"";

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	ALOGD("%s >> cmd: %s\n", __func__, cmd);

	ret = system(cmd);
	if (ret) {
		ALOGE("system restore sysinfo error\n");
		ret = -4;
		goto err;
	}
	cJSON_Delete(root);
	ans_handle_ok(nc, "Restore device params success", 0);
	return;

err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Restore device params fail", ret);
}

/*
 * url: http://deviceAddress:port/getDeviceTime
 * recv json data: {"password":"123456"}
 * return json data: {"data":{"time":"1970-01-01 09:39:01"},"message":"Get device time success","result":0}
 */
void lb_web_get_dev_time(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	char *buf;
	cJSON *root = NULL, *pwd = NULL;
	cJSON *obj = NULL, *data = NULL;
	time_t cur_time;
	struct tm *tm;
	struct tm now_time;
	char time_str[40] = {0};

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -2;
		goto err;
	}
	data = cJSON_CreateObject();
	if (!data) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -2;
		goto err;
	}
	cur_time = time(NULL);
	tm = localtime_r(&cur_time, &now_time);
	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);
	cJSON_AddStringToObject(data, "time", time_str);
	cJSON_AddItemToObject(obj, "data", data);
	cJSON_AddStringToObject(obj, "message", "Get device time success");
	cJSON_AddNumberToObject(obj, "result", 0);
	buf = cJSON_PrintUnformatted(obj);
	if (!buf) {
		ALOGW("%s >> get json format fail!\n", __func__);
		ret = -3;
		goto err;
	}
	cJSON_Delete(obj);
	cJSON_Delete(root);
	mg_printf(nc, CHUNKED_HEAD_FMT, 200);
	mg_printf_http_chunk(nc, "%s", buf);
	mg_send_http_chunk(nc, "", 0);
	free(buf);
	return;
err:
	if (obj)
		cJSON_Delete(obj);
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Get device time fail", ret);
}

/*
 * url: http://deviceAddress:port/setDeviceTime
 * recv json data: {"password":"123456","data":{"time":"2019-08-12 12:00:00"}}
 * return json data: { "message": "Set device time success","result":0 }
 */
void lb_web_set_dev_time(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *data = NULL;
	char *time = NULL;
	long tmp = 0;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	data = cJSON_GetObjectItem(root, "data");
	if (!data) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	time = cJSON_GetObjectItem(data, "time")->valuestring;
	if (0 == time_str_to_second(time, "%Y-%m-%d %H:%M:%S", &tmp)) {
		ALOGD("%s >> time:%s, tmp=%ld", __func__, time, tmp);
		ws_control_set_timestamp(tmp);
		cJSON_Delete(root);
		ans_handle_ok(nc, "Set device time success", 0);
		return;
	} else {
		ALOGW("time_str_to_second failed\n");
		ret = -3;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set device time fail", ret);
}

/*
 * url: http://deviceAddress:port/uploadUpgradeAPKFile?password=123456
 * Content-Type: multipart/form-data
 * return json data: { "message": "Upload upgrade file success","result":0 }
 */
void lb_web_upload_upgrade_file(struct mg_connection *nc, int ev, void *p)
{
	int ret;

	if (!nc || !p)
		return;

	ret = upload_file_local(nc, ev,
		(struct mg_http_multipart_part *)p, UPGRADE_PKG_FILE);
	if (MG_EV_HTTP_MULTIPART_REQUEST == ev) {
		if(ret){
			ans_handle_err(nc, "Upload upgrade file fail", -1);
			nc->flags |= MG_F_SEND_AND_CLOSE;
		}
	} else if (MG_EV_HTTP_MULTIPART_REQUEST_END == ev) {
		if(ret){
			ans_handle_err(nc, "Upload upgrade file fail", -2);
		} else {
			ans_handle_ok(nc, "Upload upgrade file success", 0);
			ret = update_device_copy_pkg(UPGRADE_PKG_PATH);
			if (ret == 0) {
				ret = update_device_start();
			}
		}
	}
}

/*
 * url: http://deviceAddress:port/setDeviceReboot
 * recv json data: {"password":"123456","data":{"type":"DelayReboot", "value":"5"}}
 * return json data: { "message": "Set device reboot success","result":0 }
 */
void lb_web_set_dev_reboot(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *data = NULL;
	char *type = NULL, *value = NULL;
	char cmd[64] = {0};

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	data = cJSON_GetObjectItem(root, "data");
	if (!data) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	type = cJSON_GetObjectItem(data, "type")->valuestring;
	value = cJSON_GetObjectItem(data, "value")->valuestring;
	if (strcmp(type, "DelayReboot") == 0) {
		snprintf(cmd, sizeof(cmd), "sleep %s && reboot &", value);
		ALOGD("system run cmd: %s\n", cmd);
		ret = system(cmd);
		if (ret) {
			ALOGE("system reboot error\n");
			ret = -3;
			goto err;
		}
		cJSON_Delete(root);
		ans_handle_ok(nc, "Set device reboot success", 0);
		return;
	} else if (strcmp(type, "TimingReboot") == 0) {
		//todo
		cJSON_Delete(root);
		ans_handle_ok(nc, "Set device reboot success", 0);
		return;
	} else {
		ALOGW("Set device reboot type no support: %s\n", type);
		ret = -3;
	}

err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set device reboot fail", ret);
}

/*
 * url: http://deviceAddress:port/setDeviceRemoteOpen
 * recv json data: {"password":"123456"}
 * return json data: { "message": "Set device remote open success","result":0 }
 */
void lb_web_set_dev_remoteopnen(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	open_door();
	cJSON_Delete(root);
	ans_handle_ok(nc, "Set device remote open success", 0);
	return;

err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set device remote open fail", ret);
}

/*
 * url: http://deviceAddress:port/getDeviceVersion
 * json data: {"password":"123456"}
 * return json data: { "data":{"face_recg_alg_version":"V0.0.0",...},"message": "Get device version success","result":0 }
 */
void lb_web_get_dev_version(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	char *buf;
	cJSON *root = NULL, *pwd = NULL;
	cJSON *obj = NULL, *data = NULL;
	device_system_info_t dev_info = {0};

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -2;
		goto err;
	}
	data = cJSON_CreateObject();
	if (!data) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -2;
		goto err;
	}

	if (get_sys_info(&dev_info) != 0) {
		ALOGE("Get sys info failed!\n");
		ret = -3;
		goto err;
	}
	//todo
	cJSON_AddStringToObject(data, "face_recg_alg_version",
		dev_info.face_recg_alg_version);
	cJSON_AddStringToObject(data, "firmware_version",
		dev_info.firmware_version);
	cJSON_AddStringToObject(data, "device_name",
		dev_info.device_name);

	cJSON_AddItemToObject(obj, "data", data);
	cJSON_AddStringToObject(obj, "message",
		"Get device version success");
	cJSON_AddNumberToObject(obj, "result", 0);
	buf = cJSON_PrintUnformatted(obj);
	if (!buf) {
		ALOGW("%s >> get json format fail!\n", __func__);
		ret = -4;
		goto err;
	}
	cJSON_Delete(obj);
	cJSON_Delete(root);
	mg_printf(nc, CHUNKED_HEAD_FMT, 200);
	mg_printf_http_chunk(nc, "%s", buf);
	mg_send_http_chunk(nc, "", 0);
	free(buf);
	return;
err:
	if (obj)
		cJSON_Delete(obj);
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Get device version fail", ret);
}

/*
 * url: http://deviceAddress:port/getDeviceSnapPicture
 * recv json data: {"password":"123456"}
 * return json data: { "data":{"snapPicBase64":"/9j/4AAQSkZJRABDXFBU"},"message": "Get snap picture success","result":0 }
 */
void lb_web_get_snap_picture(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	char *buf;
	cJSON *root = NULL, *pwd = NULL;
	cJSON *obj = NULL, *data = NULL;
	char file_name[MAX_BUFFER_SIZE] = {0};
	char *file_base64 = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	snprintf(file_name, sizeof(file_name),
		 "/tmp/snap%lld_rgb.jpg", time_get_us());
	ret = ui_control_snapshot(0, file_name);
	if (ret) {
		ALOGW("%s >> ui_control_snapshot fail!\n", __func__);
		ret = -2;
		goto err;
	}
	file_base64 = file2base(file_name);
	if (!file_base64) {
		ALOGW("%s >> file2base fail!\n", __func__);
		ret = -3;
		goto err;
	}

	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -4;
		goto err;
	}
	data = cJSON_CreateObject();
	if (!data) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -4;
		goto err;
	}

	cJSON_AddStringToObject(data, "snapPicBase64", file_base64);
	cJSON_AddItemToObject(obj, "data", data);
	cJSON_AddStringToObject(obj, "message",
		"Get snap picture success");
	cJSON_AddNumberToObject(obj, "result", 0);
	buf = cJSON_PrintUnformatted(obj);
	if (!buf) {
		ALOGW("%s >> get json format fail!\n", __func__);
		ret = -5;
		goto err;
	}
	cJSON_Delete(obj);
	cJSON_Delete(root);
	file_remove(file_name);
	free_base64data(file_base64);
	mg_printf(nc, CHUNKED_HEAD_FMT, 200);
	mg_printf_http_chunk(nc, "%s", buf);
	mg_send_http_chunk(nc, "", 0);
	free(buf);
	return;
err:

	if (obj)
		cJSON_Delete(obj);
	if (root)
		cJSON_Delete(root);
	if (file_base64)
		free_base64data(file_base64);
	if (is_file_exist(file_name))
		file_remove(file_name);
	ans_handle_err(nc, "Get snap picture fail", ret);
}

/*
 * url: http://deviceAddress:port/setDeviceLogo?password=123456
 * Content-Type: multipart/form-data
 * return json data: {"message": "Set device logo success","result":0 }
 */
void lb_web_set_dev_logo(struct mg_connection *nc, int ev, void *p)
{
	int ret;

	if (!nc || !p)
		return;

	ret = upload_file_local(nc, ev,
		(struct mg_http_multipart_part *)p, TMP_LOGO_PIC_FILE);
	if (MG_EV_HTTP_MULTIPART_REQUEST == ev) {
		if(ret){
			ans_handle_err(nc, "Set device logo fail", -1);
			nc->flags |= MG_F_SEND_AND_CLOSE;
		}
	} else if (MG_EV_HTTP_MULTIPART_REQUEST_END == ev) {
		if(ret){
			ans_handle_err(nc, "Set device logo fail", -2);
		} else {
			ans_handle_ok(nc, "Set device logo success", 0);
			//todo
		}
	}
}

/*
 * url: http://deviceAddress:port/restoreDeviceLogo
 * recv json data: {"password":"123456"}
 * return json data: {"message": "Restore device logo success","result":0}
 */
void lb_web_restore_dev_logo(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	//todo

	/* return handler result */
	if (!ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Restore device logo success", 0);
		return;
	} else {
		ALOGW("Restore device logo failed\n");
		ret = -3;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Restore device logo fail", ret);
}

/*
 * url: http://deviceAddress:port/setDeviceNetwork
 * recv json data: {"password":"123456","data":{"isDhcp":false,"ip":"192.168.19.73","gateway":"192.168.19.254","mask":"255.255.255.0","dns":"192.168.16.1"}}
 * return json data: {"message": "Set device network success","result":0 }
 */
void lb_web_set_dev_network(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *data = NULL;
	net_config_para_t eth_info;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	memset(&eth_info, 0, sizeof(eth_info));
	data = cJSON_GetObjectItem(root, "data");
	if (!data) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	eth_info.dhcp =
		cJSON_GetObjectItem(data, "isDhcp")->valueint;
	strncpy(eth_info.eth_ip,
		cJSON_GetObjectItem(data, "ip")->valuestring,
		sizeof(eth_info.eth_ip));
	strncpy(eth_info.eth_gateway,
		cJSON_GetObjectItem(data, "gateway")->valuestring,
		sizeof(eth_info.eth_gateway));
	strncpy(eth_info.eth_mask,
		cJSON_GetObjectItem(data, "mask")->valuestring,
		sizeof(eth_info.eth_mask));
	strncpy(eth_info.eth_dns,
		cJSON_GetObjectItem(data, "dns")->valuestring,
		sizeof(eth_info.eth_dns));

	if (!eth_info.dhcp &&
		((INADDR_NONE == inet_addr(eth_info.eth_ip)) ||
		(INADDR_NONE == inet_addr(eth_info.eth_mask)) ||
		(INADDR_NONE == inet_addr(eth_info.eth_gateway)) ||
		(INADDR_NONE == inet_addr(eth_info.eth_dns)))) {
		ALOGE("inet_addr parse failed\n");
		ret = -3;
		goto err;
	}

	ret = sync_set_eth_para(&eth_info);
	if (ret) {
		ALOGE("ui_device_eth_param_update failed\n");
		ret = -4;
		goto err;
	} else {
		ui_device_eth_param_update(&eth_info);
	}
	cJSON_Delete(root);
	ans_handle_ok(nc, "Set device network success", 0);
	return;

err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set device network fail", ret);
}

/*
 * url: http://deviceAddress:port/getDeviceSnapFace
 * recv json data: {"password":"123456"}
 * return json data: { "data":{"snapPicBase64":"/9j/4AAQSkZJRABDXFBU"},"message": "Get snap face success","result":0 }
 */
void lb_web_get_snap_face(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	char *buf;
	cJSON *root = NULL, *pwd = NULL;
	cJSON *obj = NULL, *data = NULL;
	face_feat_opt_t opt;
	face_capture_param_t cap;
	char *file_base64 = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	if (face_pause_status()) {
		ret = -2;
		goto err;
	}

	memset(&opt, 0, sizeof(face_feat_opt_t));
	memset(&cap, 0, sizeof(face_capture_param_t));
	/* config the params of catch face areas */
	cap.timeout_sec = 10;
	cap.face.head_pose_yaw = 20;
	cap.face.head_pose_pitch = 20;
	cap.face.head_pose_roll = 30;
	cap.face.max = 512;
	cap.face.min = 45;
#if 0
	cap.face.x0 = 445;
	cap.face.y0 = 150;
	cap.face.x1 = 445 + 390;
	cap.face.y1 = 150 + 394;
#endif
	cap.face.x0 = 0;
	cap.face.y0 = 0;
	cap.face.x1 = 1024;
	cap.face.y1 = 800;

	cap.save_face = 1;
	snprintf(cap.save_face_path, sizeof(cap.save_face_path),
		"%s", TMP_SNAP_FACE_FILE);
	ret = face_get_feats_from_capture(get_face_handle(),
		&cap, &opt);
	if (ret) {
		ret = -3;
		goto err;
	}
	file_base64 = file2base(TMP_SNAP_FACE_FILE);
	if (!file_base64) {
		ALOGW("%s >> file2base fail!\n", __func__);
		ret = -4;
		goto err;
	}

	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -5;
		goto err;
	}
	data = cJSON_CreateObject();
	if (!data) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -5;
		goto err;
	}

	cJSON_AddStringToObject(data, "snapPicBase64", file_base64);
	cJSON_AddItemToObject(obj, "data", data);
	cJSON_AddStringToObject(obj, "message",
		"Get snap face success");
	cJSON_AddNumberToObject(obj, "result", 0);
	buf = cJSON_PrintUnformatted(obj);
	if (!buf) {
		ALOGW("%s >> get json format fail!\n", __func__);
		ret = -6;
		goto err;
	}

	cJSON_Delete(obj);
	cJSON_Delete(root);
	file_remove(TMP_SNAP_FACE_FILE);
	free_base64data(file_base64);
	mg_printf(nc, CHUNKED_HEAD_FMT, 200);
	mg_printf_http_chunk(nc, "%s", buf);
	mg_send_http_chunk(nc, "", 0);
	free(buf);
	return;

err:

	if (obj)
		cJSON_Delete(obj);
	if (root)
		cJSON_Delete(root);
	if (file_base64)
		free_base64data(file_base64);
	if (is_file_exist(TMP_SNAP_FACE_FILE))
		file_remove(TMP_SNAP_FACE_FILE);
	ans_handle_err(nc, "Get snap face fail", ret);
}

/*
 * url: http://deviceAddress:port/deleteAllDeviceAdvertPicture
 * recv json data: {"password":"123456"}
 * return json data: {"message": "Del advert pictures success","result":0}
 */
void lb_web_del_advert_pngs(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	DIR *d = NULL;
	struct dirent *dp = NULL;
	struct stat st;
	char tmp[256] = {0};

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	/* delete all files in the ADVERTS_PICS_PATH */
	d = opendir(ADVERTS_PICS_PATH);
	if (!d) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Del advert pictures success", 0);
		return;
	}
	while ((dp = readdir(d)) != NULL) {
		if ((!strncmp(dp->d_name, ".", 1)) || (!strncmp(dp->d_name, "..", 2)))
			continue;
		snprintf(tmp, sizeof(tmp) - 1, "%s/%s",
			ADVERTS_PICS_PATH, dp->d_name);
		stat(tmp, &st);
		if (!S_ISDIR(st.st_mode)) {
			remove(tmp);
		}
	}
	closedir(d);

	//todo

	/* return handler result */
	if (!ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Del advert pictures success", 0);
		return;
	} else {
		ALOGW("Del advert pictures failed\n");
		ret = -3;
	}

err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Del advert pictures fail", ret);
}

/*
 * url: http://deviceAddress:port/addDeviceAdvertPicture?password=123456
 * Content-Type: multipart/form-data
 * return json data: {"message": "Add advert picture success","result":0}
 */
void lb_web_add_advert_png(struct mg_connection *nc, int ev, void *p)
{
	int ret;
	static char file[128] = {0};

	if (!nc || !p)
		return;
	struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *)p;
	if(MG_EV_HTTP_PART_BEGIN == ev) {
		create_dir(ADVERTS_PICS_PATH);
		snprintf(file, sizeof(file), "%s/%s",
			ADVERTS_PICS_PATH, mp->file_name);
	}

	ret = upload_file_local(nc, ev, mp, file);
	if (MG_EV_HTTP_MULTIPART_REQUEST == ev) {
		if(ret){
			ans_handle_err(nc, "Add advert picture fail", -1);
			nc->flags |= MG_F_SEND_AND_CLOSE;
		}
	} else if (MG_EV_HTTP_MULTIPART_REQUEST_END == ev) {
		if(ret){
			ans_handle_err(nc, "Add advert picture fail", -2);
		} else {
			ans_handle_ok(nc, "Add advert picture success", 0);
			//todo
		}
		memset(file, '\0', sizeof(file));
	}
}

/*
 * url: http://deviceAddress:port/getDeviceLog
 * recv json data: {"password":"123456","data":{"devType":"handle"}}
 * return json data: { "data":{"logcatBase64":"/9j/4AAQSkZJRABDXFBU"},"message": "Get device log success","result":0 }
 */
void lb_web_get_dev_log(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	cJSON *obj = NULL, *data = NULL;
	char *buf = NULL;
	char *devType = NULL;
	char *file_base64 = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	data = cJSON_GetObjectItem(root, "data");
	if (!data) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	devType = cJSON_GetObjectItem(data, "devType")->valuestring;
	if (strcmp(devType, "handle") == 0) {
		/* normal working log */
		if (is_file_exist(SYSTEM_LOG_FILE)) {
			file_base64 = file2base(SYSTEM_LOG_FILE);
			if (!file_base64) {
				ALOGW("%s >> file2base fail!\n", __func__);
				ret = -4;
				goto err;
			}
			obj = cJSON_CreateObject();
			if (!obj) {
				ALOGW("%s >> create json fail!\n", __func__);
				ret = -5;
				goto err;
			}
			data = cJSON_CreateObject();
			if (!data) {
				ALOGW("%s >> create json fail!\n", __func__);
				ret = -5;
				goto err;
			}
			cJSON_AddStringToObject(data, "logcatBase64", file_base64);
			cJSON_AddItemToObject(obj, "data", data);
			cJSON_AddStringToObject(obj, "message",
				"Get device log success");
			cJSON_AddNumberToObject(obj, "result", 0);
			buf = cJSON_PrintUnformatted(obj);
			if (!buf) {
				ALOGW("%s >> get json format fail!\n", __func__);
				ret = -6;
				goto err;
			}

			cJSON_Delete(obj);
			cJSON_Delete(root);
			free_base64data(file_base64);
			mg_printf(nc, CHUNKED_HEAD_FMT, 200);
			mg_printf_http_chunk(nc, "%s", buf);
			mg_send_http_chunk(nc, "", 0);
			free(buf);
			return;
		} else {
			ALOGE("file[%s] not exist\n", SYSTEM_LOG_FILE);
			ret = -3;
			goto err;
		}
	} else if (strcmp(devType, "crash") == 0) {
		/* crash status log */
		//todo

		/* return handler result */
		if (!ret) {
			cJSON_Delete(root);
			ans_handle_ok(nc, "Get device log success", 0);
			return;
		} else {
			ALOGW("handle failed\n");
			ret = -3;
		}
	} else {
		ALOGW("devType[%s] not support\n", devType);
		ret = -3;
		goto err;
	}

err:
	if (obj)
		cJSON_Delete(obj);
	if (root)
		cJSON_Delete(root);
	if (file_base64)
		free_base64data(file_base64);
	ans_handle_err(nc, "Get device log fail", ret);
}

/*
 * url: http://deviceAddress:port/deleteDeviceLog
 * recv json data: {"password":"123456","data":{"devType":"handle"}}
 * return json data: {"message": "Del device log success","result":0}
 */
void lb_web_del_dev_log(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *data = NULL;
	char *devType = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	data = cJSON_GetObjectItem(root, "data");
	if (!data) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	devType = cJSON_GetObjectItem(data, "devType")->valuestring;
	if (strcmp(devType, "handle") == 0) {
		/* normal working log */
		file_remove(SYSTEM_LOG_FILE);
	} else if (strcmp(devType, "crash") == 0) {
		/* crash status log */
		//todo

	} else {
		ALOGW("devType[%s] not support\n", devType);
		ret = -3;
	}

	if (!ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Del device log success", 0);
		return;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Del device log fail", ret);
}

#if 0
/*
 * url: http://deviceAddress:port/addDeviceWhiteList?password=123456
 * Content-Type: multipart/form-data
 * return json data: {"message": "Add white list success","result":0}
 */
void lb_web_add_white_list(struct mg_connection *nc, int ev, void *p)
{
	int ret;
	static person_t pst_person;
	static person_reg_param_t reg_param;
	if (!nc || !p)
		return;
	struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *)p;
	if(MG_EV_HTTP_PART_BEGIN == ev) {
		create_dir(MEMBER_PICS_PATH);
		memset(pst_person.regist_photo_name, 0,
			sizeof(pst_person.regist_photo_name));
		snprintf(pst_person.regist_photo_name,
			sizeof(pst_person.regist_photo_name),
			"%s/%s", MEMBER_PICS_PATH, mp->file_name);
	}

	ret = upload_file_local(nc, ev, mp, pst_person.regist_photo_name);
	if (MG_EV_HTTP_MULTIPART_REQUEST == ev) {
		if(ret){
			ans_handle_err(nc, "Add advert picture fail", -1);
			nc->flags |= MG_F_SEND_AND_CLOSE;
		}
		memset(&pst_person, 0, sizeof(person_t));
		memset(&reg_param, 0, sizeof(person_reg_param_t));
	} else if (MG_EV_HTTP_PART_DATA == ev) {
		if (0 == strcmp(mp->var_name, "employee_number")) {
			memset(pst_person.employee_number, 0,
				sizeof(pst_person.employee_number));
			memcpy(pst_person.employee_number, mp->data.p,
				mp->data.len);
			ALOGD("[%s]employee_number=%s\n", __func__,
				pst_person.employee_number);
		} else if (0 == strcmp(mp->var_name, "name")) {
			memset(pst_person.name, 0, sizeof(pst_person.name));
			memcpy(pst_person.name, mp->data.p, mp->data.len);
			ALOGD("[%s]name=%s\n", __func__, pst_person.name);
		}
	} else if (MG_EV_HTTP_MULTIPART_REQUEST_END == ev) {
		if(ret){
			ans_handle_err(nc, "Add white list fail", -2);
		} else {
			ret = web_add_member_imp(&pst_person, &reg_param);
			if (ret) {
				ans_handle_err(nc, "Add white list success", ret);
			} else {
				ans_handle_ok(nc, "Add white list success", 0);
			}
		}
		memset(&pst_person, 0, sizeof(person_t));
		memset(&reg_param, 0, sizeof(person_reg_param_t));
		return;
	}
}
#else

/*
 * url: http://deviceAddress:port/addDeviceWhiteList
 * recv json data: {"password":"123456","totalnum":2,"currentnum":1,"data":{"employee_number":"1234","name":"abc","register_base64":"/9j/4AAQSkZJRABDXFBU"}}
 * return json data: {"message": "Add white list success","result":0}
 */
void lb_web_add_white_list(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *data = NULL;
	int totalnum = 0, currentnum = 0;
	char *png_base64 = NULL;
	person_t pst_person;
	person_reg_param_t reg_param;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	memset(&pst_person, 0, sizeof(person_t));
	memset(&reg_param, 0, sizeof(person_reg_param_t));
	create_dir(MEMBER_PICS_PATH);
	memset(pst_person.regist_photo_name, 0,
		sizeof(pst_person.regist_photo_name));
	snprintf(pst_person.regist_photo_name,
		sizeof(pst_person.regist_photo_name),
		"%s/register_%lld.jpg", MEMBER_PICS_PATH, time_get_us());

	totalnum = cJSON_GetObjectItem(root, "totalnum")->valueint;
	currentnum = cJSON_GetObjectItem(root, "currentnum")->valueint;
	ALOGD("%s: totalnum=%d, currentnum%d\n", __func__, totalnum, currentnum);
	data = cJSON_GetObjectItem(root, "data");
	memset(pst_person.employee_number, 0,
		sizeof(pst_person.employee_number));
	memcpy(pst_person.employee_number,
		cJSON_GetObjectItem(data, "employee_number")->valuestring,
		sizeof(pst_person.employee_number));
	memset(pst_person.name, 0, sizeof(pst_person.name));
	memcpy(pst_person.name,
		cJSON_GetObjectItem(data, "name")->valuestring,
		sizeof(pst_person.name));
	png_base64 = cJSON_GetObjectItem(data,
		"register_base64")->valuestring;
	base2file(png_base64, pst_person.regist_photo_name);
	ret = web_add_member_imp(&pst_person, &reg_param);
	if (!ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Add white list success", 0);
		return;
	} else {
		ALOGW("Add white list failed\n");
		ret = -3;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Add white list fail", ret);
}
#endif

/*
 * url: http://deviceAddress:port/deleteDeviceWhiteList
 * recv json data: {"password":"123456","data":{"employee_number":"1234"}}
 * send json data: {"message": "Del white list success","result":0}
 */
void lb_web_del_white_list(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *data = NULL;
	char *user_id = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	data = cJSON_GetObjectItem(root, "data");
	if (!data) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	user_id = cJSON_GetObjectItem(data, "employee_number")->valuestring;
	if (!user_id || strlen(user_id) == 0) {
		ALOGE("employee_number is null\n");
		ret = -3;
		goto err;
	}

	ws_control_userinfo_delete_by_id(user_id);
	cJSON_Delete(root);
	ans_handle_ok(nc, "Del white list success", 0);
	return;
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Del white list fail", ret);
}

/*
 * url: http://deviceAddress:port/deleteDeviceAllWhiteList
 * recv json data: {"password":"123456"}
 * send json data: {"message": "Remove white lists success","result":0}
 */
void lb_web_remove_white_lists(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	db_websocket_person_info_del(0);

	cJSON_Delete(root);
	ans_handle_ok(nc, "Remove white lists success", 0);
	return;
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Remove white lists fail", ret);
}

/*
 * url: http://deviceAddress:port/getAllDeviceIdWhiteList
 * recv json data: {"password":"123456"}
 * send json data: {"data":{"idNumList":["1234"]},"message": "Get white list success","result":0}
 */
void lb_web_get_white_list(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0, count = 0, cnt = 0;
	cJSON *root = NULL, *pwd = NULL;
	cJSON *obj = NULL, *data = NULL;
	cJSON *idNumList = NULL, *_id = NULL;
	ee_queue_t head_person;
	websocket_person_info_t *_p = NULL;
	char *buf;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -5;
		goto err;
	}
	data = cJSON_CreateObject();
	if (!data) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -5;
		goto err;
	}

	count = db_ui_person_info_get_count(TYPE_PERSON);
	ee_queue_init(&head_person);
	cnt = ui_control_person_info_get_begin(
				&head_person, 0, count);
	if (cnt > 0) {
		idNumList = cJSON_AddArrayToObject(data, "idNumList");
		ee_queue_foreach(_p, &head_person, websocket_person_info_t, queue) {
			_id = cJSON_CreateString(_p->user_id);
			cJSON_AddItemToArray(idNumList, _id);
		}
	}
	ui_control_person_info_get_end(&head_person);

	cJSON_AddItemToObject(obj, "data", data);
	cJSON_AddStringToObject(obj, "message",
		"Get white list success");
	cJSON_AddNumberToObject(obj, "result", 0);
	buf = cJSON_PrintUnformatted(obj);
	if (!buf) {
		ALOGW("%s >> get json format fail!\n", __func__);
		ret = -6;
		goto err;
	}

	cJSON_Delete(obj);
	cJSON_Delete(root);
	file_remove(TMP_SNAP_FACE_FILE);
	mg_printf(nc, CHUNKED_HEAD_FMT, 200);
	mg_printf_http_chunk(nc, "%s", buf);
	mg_send_http_chunk(nc, "", 0);
	free(buf);
	return;

err:
	if (obj)
		cJSON_Delete(obj);
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Get white list fail", ret);
}

/*
 * url: http://deviceAddress:port/getDeviceWhiteListDetailByIdNum
 * recv json data: {"password":"123456","data":{"employee_number":"1234"}}
 * send json data: {"data":{"employee_number":"1234","name":"abc","register_base64":"/9j/4AAQSkZJRABDXFBU"},"message": "Get white member success","result":0}
 */
void lb_web_get_white_member(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	cJSON *obj = NULL, *data = NULL;
	websocket_person_info_t p_info;
	char reg_jpg[64] = {0};
	char *file_base64 = NULL;
	char *buf;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	data = cJSON_GetObjectItem(root, "data");
	if (!data) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	memset(&p_info, 0, sizeof(websocket_person_info_t));
	memcpy(p_info.user_id,
		cJSON_GetObjectItem(data, "employee_number")->valuestring,
		sizeof(p_info.user_id));
	ret = db_websocket_person_info(&p_info);
	if (ret) {
		ALOGE("websocket_person_info_t had not found\n");
		ret = -3;
		goto err;
	}
	snprintf(reg_jpg, sizeof(reg_jpg), "%s",
		p_info.pic_large);

	file_base64 = file2base(reg_jpg);
	if (!file_base64) {
		ALOGW("%s >> file2base[%s] fail!\n", __func__, p_info.pic_large);
		ret = -4;
		goto err;
	}

	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -5;
		goto err;
	}
	data = cJSON_CreateObject();
	if (!data) {
		ALOGW("%s >> create json fail!\n", __func__);
		ret = -5;
		goto err;
	}

	cJSON_AddStringToObject(data, "employee_number",
		p_info.user_id);
	cJSON_AddStringToObject(data, "name", p_info.name);
	cJSON_AddStringToObject(data, "register_base64",
		file_base64);
	cJSON_AddItemToObject(obj, "data", data);
	cJSON_AddStringToObject(obj, "message",
		"Get white member success");
	cJSON_AddNumberToObject(obj, "result", 0);
	buf = cJSON_PrintUnformatted(obj);
	if (!buf) {
		ALOGW("%s >> get json format fail!\n", __func__);
		ret = -6;
		goto err;
	}

	cJSON_Delete(obj);
	cJSON_Delete(root);
	file_remove(TMP_SNAP_FACE_FILE);
	free_base64data(file_base64);
	mg_printf(nc, CHUNKED_HEAD_FMT, 200);
	mg_printf_http_chunk(nc, "%s", buf);
	mg_send_http_chunk(nc, "", 0);
	free(buf);
	return;

err:

	if (obj)
		cJSON_Delete(obj);
	if (root)
		cJSON_Delete(root);
	if (file_base64)
		free_base64data(file_base64);
	ans_handle_err(nc, "Get white member fail", ret);
}

/*
 * url: http://deviceAddress:port/setIdentifyCallBck
 * recv json data: {"password":"123456","platformEnable":1,"platformIp":"http://xx"}
 * send json data: {"message": "Set param success","result":0}
 */
void lb_web_set_identify_callback(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL, *tmp = NULL;
	device_platform_info_t dev_info = {0};

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;

	tmp = cJSON_GetObjectItem(root, "platformEnable");
	if (!tmp) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	dev_info.platform_enable = tmp->valueint;
	tmp = cJSON_GetObjectItem(root, "platformIp");
	if (!tmp) {
		ALOGE("cJSON_Parse data failed\n");
		ret = -2;
		goto err;
	}
	strncpy(dev_info.platform_ip, tmp->valuestring, sizeof(dev_info.platform_ip));
	ALOGD("set dev_info.platform_enable=%d, dev_info.platform_ip=%s",
		dev_info.platform_enable, dev_info.platform_ip);
	ret = db_device_platform_info_update(&dev_info);
	if (SQLITE_OK != ret) {
		ALOGE("%s:%d >> updata db failed", __func__, __LINE__);
		ret = -3;
		goto err;
	}
	if (m_cb) {
		m_cb(WEB_PLATFORM_CHANGE, dev_info.platform_enable);
	}

	if (0 == ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Set param success", 0);
		return;
	} else {
		ALOGW("lb_web_set_identify_callback failed\n");
		ret = -4;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set param fail", ret);
}

/*
 * url: http://deviceAddress:port/setDeviceTimeAccessGroups
 * recv json data: {"password":"123456","data":{"TimeGroup":[{"TimeGroupId":"1","TimeGroup":[{"WeekIndex":"0","Times":[{"start":"08:00","End":"12:00"},...]},...]}]}}
 * send json data: {"message": "Set access groups success","result":0}
 */
void lb_web_set_access_groups(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	//todo

	/* return handler result */
	if (!ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Set access groups success", 0);
		return;
	} else {
		ALOGW("Set access groups failed\n");
		ret = -3;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set access groups fail", ret);
}

/*
 * url: http://deviceAddress:port/setDeviceTimeAccessSpecialTimeGroups
 * recv json data: {"password":"123456","data":{"SpecialTimeGroup":[{"SpecialTimeGroupId":"1","SpecialTimeGroupData":[{"start":"08:00","End":"12:00"},...]},...]}}
 * send json data: {"message": "Set special groups success","result":0}
 */
void lb_web_set_special_groups(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret)
		goto err;
	//todo

	/* return handler result */
	if (!ret) {
		cJSON_Delete(root);
		ans_handle_ok(nc, "Set special group success", 0);
		return;
	} else {
		ALOGW("Set special group failed\n");
		ret = -3;
	}
err:
	if (root)
		cJSON_Delete(root);
	ans_handle_err(nc, "Set special group fail", ret);
}




void lb_web_ip_setting(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	char *buf = 0;

	net_config_para_t para = {0};

	int set = 0;
	char *ip = 0;
	char *mask = 0;
	char *gate = 0;
	char *dns = 0;
	int dhcp = 0;
	{
		struct http_message *_http = (struct http_message *)p;
		if (_http && _http->body.p)
		ALOGD("%s >> recv >> %s\n", __func__, _http->body.p);
	}
	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);
	if (ret){

	}else{
		obj_ref(input, root);
		obj_get_num(input, "set", set);

		if (set){
			obj_get_string(input, "ip", ip);
			obj_get_string(input, "mask", mask);
			obj_get_string(input, "gate", gate);
			obj_get_string(input, "dns", dns);
			obj_get_num(input, "is_dhcp", dhcp);

			if (!dhcp){
				memcpy(para.eth_ip, ip, strlen(ip));
				memcpy(para.eth_mask, mask, strlen(mask));
				memcpy(para.eth_gateway, gate, strlen(gate));
				memcpy(para.eth_dns, dns, strlen(dns));
				ALOGD("mask=%s eth_mask=%s", mask, para.eth_mask);
			} else {
				ui_device_eth_param_query(&para);
			}
			ALOGD("dhcp=%d", dhcp);
			para.dhcp = dhcp;
			para.enable = 1;
			ret = sync_set_eth_para(&para);
			if (ret) {
				ALOGE("%s >> set eth params failed!", __func__);
			} else {
				ret = ui_device_eth_param_update(&para);
				if (!ret) {
					ALOGD("%s >> save eth params success!", __func__);
				} else {
					ALOGE("%s >> save eth params failed!", __func__);
				}
			}
		}else{
			get_eth_para(&para);
			obj_create(ip_setting);
			obj_add_string(ip_setting, "ip", para.eth_ip);
			obj_add_string(ip_setting, "mask", para.eth_mask);
			obj_add_string(ip_setting, "gate", para.eth_gateway);
			obj_add_string(ip_setting, "dns", para.eth_dns);
			obj_add_num(ip_setting, "is_dhcp", para.dhcp);

			obj_format(ip_setting, buf);
			obj_free(ip_setting);
		}

	}

	send_resp("ip_setting failed");
	if (root)
		cJSON_Delete(root);
	return;
}



void lb_web_get_wifi_state(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	char *buf = 0;
	char connected_ssid[128] = {0};
	wifi_config_para_t para = {0};
	{
		struct http_message *_http = (struct http_message *)p;
		if (_http && _http->body.p)
		ALOGD("%s >> recv >> %s\n", __func__, _http->body.p);
	}

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);

	(void)buf;
	if (ret){

	}else{
		get_wifi_para(&para);
		get_current_ssid(connected_ssid);
		ALOGD("enable_wifi=%d connected_ssid=%s",
				para.enable_wifi,
				connected_ssid);
		obj_create(wifi_state);
		obj_add_num(wifi_state, "enable_wifi", para.enable_wifi);
		obj_add_num(wifi_state, "enable_hotspot", para.enable_hotspot);
		obj_add_string(wifi_state, "hotspot_name", para.hotspot_name);
		obj_add_string(wifi_state, "hotspot_password", para.hotspot_password);
		obj_add_string(wifi_state, "connected_ssid", connected_ssid);

		obj_format(wifi_state, buf);
		obj_free(wifi_state);
	}

	send_resp("get wifi state failed");
	if (root)
		cJSON_Delete(root);

	return;
}


void lb_web_set_wifi_state(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	char *buf = 0;
	int enable = 0;

	{
		struct http_message *_http = (struct http_message *)p;
		if (_http && _http->body.p)
		ALOGD("%s >> recv >> %s\n", __func__, _http->body.p);
	}

	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);

	(void)buf;
	if (ret){

	}else{
		obj_ref(wifi, root);
		obj_get_num(wifi, "is_enable", enable);
		ret = sync_set_wifi_enabled(enable);
		if (0 == ret) {
			save_wifi_config();
		}
	}

	send_resp("set wifi state failed");
	if (root)
		cJSON_Delete(root);

	return;
}

static const char *wifi_map_scurity(int sec)
{
	const char *sec_s = 0;
	switch (sec){
		case SECURITY_OPEN:
			sec_s = "OPEN";
			break;
		case SECURITY_WPA_PSK:
			sec_s = "WPA";
			break;
		case SECURITY_WPA2_PSK:
			sec_s = "WPA2";
			break;
		default:
			sec_s = "UNKNOWN";
			break;
	}
	return sec_s;
}


void lb_web_get_scan_wifi_results(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	char *buf = 0;

	SCAN_INFO *scan_res = NULL;
	HOTSPOT_INFO *info = 0;
	int i = 0;
	{
		ALOGD("%s start", __func__);
	}
	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);

	if (ret){
		/*do nothing*/
	}else{
		scan_res = calloc(sizeof(SCAN_INFO), 1);
		if (!scan_res) {
			ALOGE("%s: calloc scan_res is NULL\n", __func__);
			goto exit_rt;
		}
		/* the list max limit number*/
		scan_res->num = 50;
		scan_res->info = calloc(sizeof(HOTSPOT_INFO), scan_res->num);
		if (!scan_res->info) {
			ALOGE("%s: calloc result.info is NULL\n", __func__);
			free(scan_res);
			scan_res = NULL;
			goto exit_rt;
		}

		ret = sync_get_scan_list(scan_res);
		if (ret) {
			ALOGE("%s: get result is NULL\n", __func__);
			free(scan_res->info);
			scan_res->info = NULL;
			free(scan_res);
			scan_res = NULL;
			goto exit_rt;
		}
		array_create(scan);
		/*ALOGD("hotspot(%d) ", scan_res->num);*/
		for (i = 0; i < scan_res->num; i++){
			info = &scan_res->info[i];
			/*ALOGD("ssid=%s strength=%d enc=%s channel=%d ",
					info->ssid,
					info->rssi,
					wifi_map_scurity(info->security),
					info->channel);*/
			if (strlen(info->ssid) > 0){
				obj_create(hotspot);
				obj_add_string(hotspot, "ssid", info->ssid);
				obj_add_num(hotspot, "strength", info->rssi);
				obj_add_string(hotspot, "encryption", wifi_map_scurity(info->security));
				obj_add_num(hotspot, "channel", info->channel);
				array_add_item(scan, obj_get(hotspot));
			}
		}
		array_format(scan, buf);
		array_free(scan);
		if (scan_res) {
			if (scan_res->info) {
				free(scan_res->info);
				scan_res->info = NULL;
			}
			free(scan_res);
			scan_res = NULL;
		}
	}
exit_rt:
	send_resp("get scan result failed");
	if (root)
		cJSON_Delete(root);

	return;
}

void lb_web_connect_wifi(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	char *buf = 0;

	char *ssid = 0;
	char *wifi_pwd = 0;
	int encryption = 0;
	int failed = 0;
	char connected_ssid[128] = {0};
	{
		ALOGD("%s start", __func__);
	}
	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);

	if (ret){

	}else{
		obj_ref(con_wifi, root);
		obj_get_num(con_wifi, "encryption", encryption);
		obj_get_string(con_wifi, "ssid", ssid);
		obj_get_string(con_wifi, "pwd", wifi_pwd);
		failed = sync_connect_network(ssid, wifi_pwd, encryption);
		if (failed == 5) { //WIFI_STATION_CONNECTED
			get_current_ssid(connected_ssid);
		}
		obj_create(reason);
		obj_add_num(reason, "failed", failed);
		obj_add_string(reason, "connected_wifi", &connected_ssid[0]);
		obj_format(reason, buf);
		obj_free(reason);
	}
	if (root)
		cJSON_Delete(root);

	send_resp("connect wifi failed");
	return;
}



void lb_web_factory_default(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	char *buf = 0;


	{
		ALOGD("%s start", __func__);
	}
	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);

	if (ret){

	}else{
		ui_factory_default();
		/*force send ok*/
		ret = 0;
	}
	send_resp("factory default failed");
	if (root)
		cJSON_Delete(root);

	return;
}



void lb_web_alg_setting(struct mg_connection *nc, int ev, void *p)
{
	int ret = 0;
	cJSON *root = NULL, *pwd = NULL;
	char *buf = 0;

	float recg_threshold = 0;
	int living = 0;
	int set = 0;
	algorithm_system_info_t info = {0};
	{
		ALOGD("%s start", __func__);
	}
	PARSE_ROOT_PWD(nc, ev, p, root, pwd, ret);

	if (ret){

	}else{
		obj_ref(alg, root);
		obj_get_num(alg, "recg_threshold", recg_threshold);
		obj_get_num(alg, "living", living);
		obj_get_num(alg, "set", set);
		ALOGD("set=%d recg_threshold=%f living=%d",
			set,
			recg_threshold,
			living);
		if (set){
			ui_get_algorithm_system_info(&info);
			info.face_threshold_value = recg_threshold/100.0;
			info.living_nir = living;
			ui_set_algorithm_system_info(&info);
		}else{
			ui_get_algorithm_system_info(&info);
			obj_create(alg_cur);
			obj_add_num(alg_cur, "recg_threshold",
				(int)(info.face_threshold_value * 100));
			obj_add_num(alg_cur, "living",
				info.living_nir);
			obj_format(alg_cur, buf);
			obj_free(alg_cur);
		}
		/*force send ok*/
		ret = 0;
	}
	send_resp("algsetting failed");
	if (root)
		cJSON_Delete(root);

	return;
}

/*
 * url: http://deviceAddress:port/uploadUpgradeAPKFile?password=123456
 * Content-Type: multipart/form-data
 * return json data: { "message": "Upload upgrade file success","result":0 }
 */
void lb_web_upgrade(struct mg_connection *nc, int ev, void *p)
{
	int ret;
	static char fn[128] = {0};
	char to_target_name[128] = {0};
	if (!nc || !p)
		return;
	struct mg_http_multipart_part *mg_mp = p;


	ret = upload_file_local(nc, ev,
		(struct mg_http_multipart_part *)p, UPGRADE_PKG_FILE);
	//ALOGD("ev=%d mg_mp->file_name=%s", ev, mg_mp->file_name);
	if (MG_EV_HTTP_PART_DATA == ev){
		snprintf(fn, 128, "%s",
			mg_mp->file_name);
	}
	if (MG_EV_HTTP_MULTIPART_REQUEST == ev) {
		if(ret){
			ans_handle_err(nc, "Upload upgrade file fail", -1);
			nc->flags |= MG_F_SEND_AND_CLOSE;
		}else{

		}
	} else if (MG_EV_HTTP_MULTIPART_REQUEST_END == ev) {
		if(ret){
			ans_handle_err(nc, "Upload upgrade file fail", -2);
		} else {
			snprintf(to_target_name, 128, "mv %s %s%s && sync",
				UPGRADE_PKG_FILE,
				UPGRADE_PKG_PATH,
				fn);
			ALOGD("%s", to_target_name);
			system(to_target_name);
			memset(to_target_name, 0, 128);
			snprintf(to_target_name, 128, "rm -f %s%s && sync",
				UPGRADE_PKG_PATH,
				fn);

			//ret = update_device_by_path(to_target_name);
			ret = update_device_copy_pkg(UPGRADE_PKG_PATH);

			ALOGD("%s", to_target_name);
			system(to_target_name);

			if (ret == 0) {
				ret = update_device_start();
			}

			if (!ret){
				ans_handle_ok(nc, "Upload upgrade file success", 0);

			}else{
				update_device_cancel();
				ans_handle_ok(nc, "Upload upgrade file failed, incomplete file", -3);
			}
		}
	}


}



void lb_web_recv_test(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *_http = (struct http_message *)p;
	if (!_http) return;
	ALOGD("%s >> recv >> %s\n", __func__, _http->body.p);
	/* return handler result */
	ans_handle_ok(nc, "recv test success", 0);
}

