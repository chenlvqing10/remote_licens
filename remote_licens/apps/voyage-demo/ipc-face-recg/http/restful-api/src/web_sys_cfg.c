/*
 * api/sys_cfg.c - all functions for system configurations
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the lb_mem_free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if.h>

#define LOG_TAG "web_sys_cfg.c"
#include <log/log.h>
#include "sys_cfg.h"
#include "cJSON.h"
#include "system_factory.h"
#include "system_info.h"
#include "system_develop_sel.h"
#include "db_device_hw_param.h"
#include "db_device_status_info.h"
#include "db_manager_info.h"
#include "net_wired.h"
#include "net_wifi.h"
#include "light.h"
#include "system_screen.h"
#include "config_db.h"
#include "face_db.h"
#include "sys_http.h"
#include "restful_api.h"
#include "face_db.h"
#include "lb_os_http.h"
#include "app_lang.h"
#include "sys_info.h"
#include "net_utils.h"
#include "system_upgrade.h"

#include "web_biz_api.h"
#include "base64.h"

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
 * form_ip_setting - get the version information.
 * @wp:
 *
 * Returns 0 means success, or other value means failure.
 */
void form_ip_setting(Webs *wp)
{
	const char *pValStr;
	pValStr = websGetVar(wp, "ipaddr", "");
	printf("IP address: %s\n", pValStr);
}

/**
 * brightness_on_change - procedure when brightness is changed.
 * @wp:
 * @prefix:
 * @dir:
 * @flags:
 *
 * Returns 0 means success, or other value means failure.
 */
int brightness_on_change(Webs *wp, char *prefix, char *dir, int flags)
{
	char *pParamPos = NULL;
	char *pParamEnd = NULL;
	char *pParam = NULL;
	const char *pstrUrl = websGetUrl(wp);

	if (NULL == prefix) {
		printf("\n[%s]There?is?no?prefix?exist!\n", prefix);
		return -1;
	}

	pParamPos = strstr(pstrUrl, prefix);
	printf("%s\n", pParamPos);
	pParamPos += strlen(prefix);
	pParamEnd = strchr(pParamPos, '/');
	pParam = (char *)lb_mem_alloc_ext(pParamEnd-pParamPos+1, __func__, __LINE__);
	if (NULL == pParam) {
		printf("Not enough memory!\n");
		return -1;
	}
	memset(pParam, 0, pParamEnd-pParamPos+1);
	memcpy(pParam, pParamPos, pParamEnd-pParamPos);
	websSetStatus(wp, 200);
	websHeader(wp);
	websWrite(wp, "Current Brightness:?%s?!\n", pParam);
	websFooter(wp);
	websDone(wp);

	return 0;
}
#endif

#ifdef WS_MG
/**
 * form_ip_setting - get the version information.
 * @nc:
 * @ev:
 * @p:
 *
 * Returns 0 means success, or other value means failure.
 */
void form_ip_setting(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *p_http_msg = (struct http_message *)p;
	char ipaddr[20] = {0};

	if ((NULL == nc) || (NULL == p)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		return;
	}

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "ipaddr", ipaddr, sizeof(ipaddr));
		printf("IP address::%s\n", ipaddr);
		break;

	default:
		break;
	}
}

/**
 * lb_web_factory_default: reset factory default
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns None.
 */
void lb_web_factory_default(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
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

	/** factory default */
	ret = factory_default();
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	return;
}

/**
 * lb_web_fw_upg: firmware upgrade
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_fw_upg(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	struct http_message *p_http_msg = (struct http_message *)p;
	char buf[128];
	char username[32];
	char session_id[32];
	char md5_str[128];
	char src_name[256];
	static int s_firmware_file_count;
	char *b64_file, *bin_data;
	int index, total, bin_len, len, ret = -1, i;

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

		if (!find_user_from_list(username, session_id)) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
			return;
		}

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "file_index", buf,
			sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &index);

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "file_count", buf,
			sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &total);

		memset(md5_str, 0, sizeof(md5_str));
		mg_get_http_var(&p_http_msg->body, "md5_check_code", md5_str,
			sizeof(md5_str));

		len = p_http_msg->body.len;
		b64_file = (char *)lb_mem_alloc_ext(len, __func__, __LINE__);
		if (!b64_file) {
			ALOGE("malloc(b64_file len %d) failure, errno %d\n", len, errno);
			send_fail_result(nc, root, arr, STATU_FAIL);
			return;
		}

		memset(b64_file, 0, len);
		mg_get_http_var(&p_http_msg->body, "firmwarefile", b64_file, len);
		ret = base64_decode(b64_file, strlen(b64_file), &bin_data, &bin_len);
		if (!ret) {
			if (!index)
				s_firmware_file_count = 0;
			ret = write_tmp_fw_pack_bin_file(index, bin_data, bin_len);
			s_firmware_file_count++;
			lb_mem_free(bin_data);
		} else
			goto Exit;
		lb_mem_free(b64_file);
		break;

	default:
		break;
	}

	ALOGD("[s_firmware_file_count = %d  : total = %d ]", s_firmware_file_count, total);
	if (s_firmware_file_count == total) {
		ret = write_fw_file_to_flash(total, md5_str);
		if (ret) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			goto Exit;
		} else {
			for (i = 0; i < total; i++) {
				sprintf(src_name, "/data/fw_%d.bin", i);
				remove(src_name);
			}

			system("mv /data/firmware.bin /cache/package.efw");
			write_upgrade_flag_misc();
			system("sync");
			usleep(1000 * 1000);
			reboot_http();
		}
	}

	restful_send_ok_result(nc, root, arr, obj);

	return;

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	for (i = 0; i < total; i++) {
		sprintf(src_name, "/data/fw_%d.bin", i);
		remove(src_name);
	}
	send_fail_result(nc, root, arr, STATU_FAIL);
	return;
}

/**
 * lb_web_sys_info: get system information
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_sys_info(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	int ret = 0;

	cJSON *root, *arr, *obj = NULL;
	sys_build_config_info build_info;

	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&build_info, 0, sizeof(build_info));

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


	ret = get_sys_build_info(&build_info);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}
	cJSON_AddStringToObject(obj, "build_date", build_info.build_date);
	cJSON_AddStringToObject(obj, "build_author", build_info.build_user);
	cJSON_AddStringToObject(obj, "build_host", build_info.build_host);
	cJSON_AddStringToObject(obj, "model", build_info.product_name);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_sys_admin: set system adminstrator password
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_sys_admin(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char oldword[32] = {0};
	char newword[32] = {0};
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
		mg_get_http_var(&p_http_msg->body, "old_password", oldword,
			sizeof(oldword));
		mg_get_http_var(&p_http_msg->body, "new_password", newword,
			sizeof(newword));
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
	if (chk_str_val(oldword) || chk_str_val(newword)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}

	/** dispose data */
	ret = update_admin_pwd(oldword, newword);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_dev_time: get device time
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_dev_time(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	int ret = -1;

	dev_time_config_info time_info;
	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&time_info, 0, sizeof(time_info));

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

	/** get dev time */
	ret = get_dev_time(&time_info);
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
	cJSON_AddStringToObject(obj, "dev_time", time_info.curtime);
	cJSON_AddNumberToObject(obj, "time_zone", time_info.timezone);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_dev_time: device time setting
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_dev_time(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char timezone[8];
	int ret = -1;

	dev_time_config_info time_info;
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
		mg_get_http_var(&p_http_msg->body, "dev_time", time_info.curtime,
			sizeof(time_info.curtime));
		mg_get_http_var(&p_http_msg->body, "time_zone", timezone,
			sizeof(timezone));
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

	/** set dev time */
	ret = set_dev_time(&time_info);
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
 * lb_web_restart_device: reboot device
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_restart_device(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char flag[32] = {0};
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
		mg_get_http_var(&p_http_msg->body, "flag", flag, sizeof(flag));
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

	if (!strcmp(flag, "1")) {
		ret = reboot_http();
		if (ret) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			send_fail_result(nc, root, arr, STATU_FAIL);
			goto Exit;
		}
	}

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_device_hwparam: get hardware param
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_device_hwparam(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};

	cJSON *arr, *root, *obj = NULL;
	device_hw_param_config hw_param;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&hw_param, 0, sizeof(hw_param));

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

	/** get device hardware param */
	memset(&hw_param, 0, sizeof(device_hw_param_config));
	if (get_device_hwpara(&hw_param) < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (obj == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	cJSON_AddNumberToObject(obj, "volume", hw_param.volume);
	cJSON_AddNumberToObject(obj, "auto_display_brightness",
		hw_param.auto_display_brightness);
	cJSON_AddNumberToObject(obj, "display_brightness", hw_param.display_brightness);
	cJSON_AddNumberToObject(obj, "infrared_light_set", hw_param.infrared_light_set);
	cJSON_AddNumberToObject(obj, "light_sensitive", hw_param.light_sensitive);
	cJSON_AddNumberToObject(obj, "human_induction", hw_param.human_induction);

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_device_hwparam: set hardware param
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_device_hwparam(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	char buf[128];
	int ret = -1;

	cJSON *arr, *root, *obj = NULL;
	device_hw_param_config hw_param;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&hw_param, 0, sizeof(hw_param));

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "auto_display_brightness", buf,
			sizeof(buf));
		if (!strcmp(buf, "1"))
			hw_param.auto_display_brightness = 1;
		else if (!strcmp(buf, "2"))
			hw_param.auto_display_brightness = 2;
		else
			hw_param.auto_display_brightness = 0;

		mg_get_http_var(&p_http_msg->body, "infrared_light_set", buf,
			sizeof(buf));
		if (!strcmp(buf, "1"))
			hw_param.infrared_light_set = 1;
		else if (!strcmp(buf, "2"))
			hw_param.infrared_light_set = 2;
		else
			hw_param.infrared_light_set = 0;

		mg_get_http_var(&p_http_msg->body, "light_sensitive", buf,
			sizeof(buf));
		if (!strcmp(buf, "1"))
			hw_param.light_sensitive = 1;
		else
			hw_param.light_sensitive = 0;

		mg_get_http_var(&p_http_msg->body, "human_induction", buf,
			sizeof(buf));
		if (!strcmp(buf, "1"))
			hw_param.human_induction = 1;
		else
			hw_param.human_induction = 0;

		mg_get_http_var(&p_http_msg->body, "volume", buf,
			sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &hw_param.volume);

		mg_get_http_var(&p_http_msg->body, "display_brightness", buf,
			sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &hw_param.display_brightness);
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
	if (hw_param.volume < 0 || hw_param.volume > 100)
		hw_param.volume = 50;
	if (hw_param.display_brightness < 0 || hw_param.display_brightness > 100)
		hw_param.display_brightness = 50;

	/** set device hardware param */
	ret = set_device_hwpara(&hw_param);
	if (ret) {
		ALOGE("[%s:%d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_volume: get volume
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_volume(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	int ret = -1, volume;

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

	/** get volume */
	ret = get_device_hwpara_volume(&volume);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (obj == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	cJSON_AddNumberToObject(obj, "volume", volume);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_volume: set volume
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_volume(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	char volume[8] = {0};
	int volume_flag;

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
		mg_get_http_var(&p_http_msg->body, "volume", volume, sizeof(volume));
		if (strlen(volume) > 0)
			sscanf(volume, "%d", &volume_flag);
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
	if (volume_flag < 0 || volume_flag > 100)
		volume_flag = 50;

	/** set volume */
	if (set_device_hwpara_volume(volume_flag)) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data */
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_screeninfo: get screen info
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_screeninfo(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	int ret = -1;

	cJSON *root, *arr, *obj = NULL;
	device_hw_param_config_screeninfo sc_bright;

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

	/** get screen info */
	memset(&sc_bright, 0, sizeof(device_hw_param_config_screeninfo));
	ret = get_device_hwpara_screeninfo(&sc_bright);
	if (ret < 0) {
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
	cJSON_AddNumberToObject(obj, "auto_display_brightness",
		sc_bright.auto_display_brightness);
	cJSON_AddNumberToObject(obj, "display_brightness",
		sc_bright.display_brightness);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_screeninfo: set screen info
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_screeninfo(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	ALOGD("[cdeshi --> %d]", __LINE__);
	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	char buf[64] = {0};
	int ret = -1;

	cJSON *root, *arr, *obj = NULL;
	device_hw_param_config_screeninfo sc_bright;

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

		mg_get_http_var(&p_http_msg->body, "auto_display_brightness",
			buf, sizeof(buf));
		if (!strcmp(buf, "1"))
			sc_bright.auto_display_brightness = 1;
		else
			sc_bright.auto_display_brightness = 0;

		mg_get_http_var(&p_http_msg->body, "display_brightness",
			buf, sizeof(buf));
		if (strlen(buf) > 0)
			sscanf(buf, "%d", &sc_bright.display_brightness);

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}
	if (sc_bright.display_brightness < 0 || sc_bright.display_brightness > 100)
		sc_bright.display_brightness = 50;

	/** set device screeninfo */
	ret = set_device_hwpara_screeninfo(&sc_bright);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_light_info: get light info
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_light_info(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	int ret = -1;

	cJSON *root, *arr, *obj = NULL;
	device_hw_param_config_light_info light_info;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&light_info, 0, sizeof(device_hw_param_config_light_info));

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

	/** get light info */
	ret = get_light_info(&light_info);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}
	cJSON_AddNumberToObject(obj, "auto_adapt_enable", light_info.auto_adapt_enable);
	cJSON_AddNumberToObject(obj, "switch_enable", light_info.switch_enable);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_light_info: set light info
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_light_info(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char buf[128];
	int ret = -1;

	device_hw_param_config_light_info light_info;
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

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "auto_adapt_enable", buf,
			sizeof(buf));
		if (!strcmp(buf, "1"))
			light_info.auto_adapt_enable = 1;
		else
			light_info.auto_adapt_enable = 0;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "switch_enable", buf,
			sizeof(buf));
		if (!strcmp(buf, "1"))
			light_info.switch_enable = 1;
		else
			light_info.switch_enable = 0;
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

	/** set light info */
	ret = set_light_info(&light_info);
	if (ret == -3) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, AUTO_LIGHT_ALREAD_ENABLE);
		goto Exit;
	} else if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data */
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

void *add_manger_thread(void *arg)
{
	cJSON *root, *arr = NULL;
	int ret = 0;
	http_thread_info *thread_info = (http_thread_info *)arg;
	manger_face_info *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	/** dispose data */
	ret = add_admin(p_info->username2, p_info->password);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = STATU_FAIL;
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

int proc_add_manger(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, add_manger_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_add_manager: add manager
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_add_manager(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char notify_url[64] = {0};
	cJSON *root, *arr, *obj = NULL;
	manger_face_info *p_info;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	p_info = (manger_face_info *)lb_mem_alloc_ext(sizeof(manger_face_info),
		__func__, __LINE__);
	if ((p_info == NULL) || (thread_info == NULL) || (NULL == nc) || (NULL == p)
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

		mg_get_http_var(&p_http_msg->body, "user_name2", p_info->username2,
			sizeof(p_info->username2));
		mg_get_http_var(&p_http_msg->body, "user_password", p_info->password,
			sizeof(p_info->password));
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

	/** begin add manget thread */
	thread_info->data = p_info;
	strcpy(thread_info->notify_url, notify_url);
	strcpy(thread_info->session_id, session_id);
	proc_add_manger(thread_info);
	return;

Exit:
	if (p_info)
		lb_mem_free(p_info);
	if (thread_info)
		lb_mem_free(thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

void *search_manger_thread(void *arg)
{
	cJSON *root, *arr, *obj = NULL;
	int ret = 0;
	http_thread_info *thread_info = (http_thread_info *)arg;
	manger_face_info *p_info = thread_info->data;

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	ret = search_manger(p_info);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = STATU_FAIL;
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}
	cJSON_AddNumberToObject(obj, "id", p_info->id);
	cJSON_AddStringToObject(obj, "user_name2", p_info->username2);
	cJSON_AddStringToObject(obj, "user_password", p_info->password);
	cJSON_AddItemToArray(arr, obj);

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

int proc_search_manger_info(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, search_manger_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_search_manager: search manager
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_search_manager(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char id_str[32];
	char username2[32];
	char notify_url[64] = {0};

	cJSON *root, *arr, *obj = NULL;
	manger_face_info *p_info;
	http_thread_info *thread_info;

	/** para init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	p_info = (manger_face_info *)lb_mem_alloc_ext(sizeof(manger_face_info),
		__func__, __LINE__);
	if (p_info == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	} 
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
		mg_get_http_var(&p_http_msg->body, "notify_url",
			notify_url, sizeof(notify_url));
		memset(username2, 0, sizeof(username2));
		mg_get_http_var(&p_http_msg->body, "user_name2", p_info->username,
			sizeof(username2));

		memset(id_str, 0, sizeof(id_str));
		mg_get_http_var(&p_http_msg->body, "id", id_str,
			sizeof(id_str));
		if (strlen(id_str) > 0)
			sscanf(id_str, "%d", &p_info->id);
		break;

	default:
		break;
	}

	ALOGD("[%s : %d]\n", p_info->username, p_info->id);
	/** para check */
	if (create_json(&root, &arr) || chk_str_val(p_info->username)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** check user */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

	/** begin add manget thread */
	thread_info->data = p_info;
	strcpy(thread_info->notify_url, notify_url);
	strcpy(thread_info->session_id, session_id);
	proc_search_manger_info(thread_info);
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
 * lb_web_update_manager: update manager
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_update_manager(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	struct http_message *p_http_msg = (struct http_message *)p;

	char username[32] = {0};
	char session_id[32] = {0};
	char username2[32] = {0};
	char password[32] = {0};

	int ret = -1;
	cJSON *root, *arr, *obj = NULL;
	manger_face_info p_info;

	/** init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&p_info, 0, sizeof(p_info));

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "user_name2", username2,
			sizeof(username2));
		mg_get_http_var(&p_http_msg->body, "user_password", password,
			sizeof(password));

		/** wu yong */
		#if 0
		memset(id_str, 0, sizeof(id_str));
		mg_get_http_var(&p_http_msg->body, "id", id_str, sizeof(id_str));
		if (strlen(id_str) > 0)
			sscanf(id_str, "%d", &id);
		#endif
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
	if (chk_str_val(username2) || chk_str_val(password)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** get old manget information */
	ret = search_manger(&p_info);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = STATU_FAIL;
		goto Exit;
	}
	ALOGD("[---> username2 %s]", username2);
	ALOGD("[---> password %s]", password);
	if (strlen(username2) == 0)
		strcpy(username2, p_info.username2);
	if (strlen(password) == 0)
		strcpy(password, p_info.password);

	/** update number */
	ret = update_number(username2, password);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_delete_manager: delete manager
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_delete_manager(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char id_str[32];
	char username2[32];

	cJSON *root, *arr, *obj = NULL;
	int id, ret = -1;

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
		memset(username2, 0, sizeof(username2));
		mg_get_http_var(&p_http_msg->body, "user_name2", username2,
			sizeof(username2));

		/** wu yong */
		memset(id_str, 0, sizeof(id_str));
		mg_get_http_var(&p_http_msg->body, "id", id_str, sizeof(id_str));
		if (strlen(id_str) > 0)
			sscanf(id_str, "%d", &id);
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
	if (id < 0)
		id = 0;

	/** delete manger */
	ret = delete_number(username2);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_develop_select: get delete manager
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_develop_select(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	int ret = -1;

	develop_sel_para_config_info sel_info;
	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&sel_info, 0, sizeof(develop_sel_para_config_info));

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

	/** get develop select */
	ret = get_develop_select(&sel_info);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	cJSON_AddNumberToObject(obj, "debug_sw", sel_info.debug_sw);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_develop_select: set delete manager
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_develop_select(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char buf[128];
	int debug_sw = 0, ret = -1;

	develop_sel_para_config_info sel_info;
	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&sel_info, 0, sizeof(develop_sel_para_config_info));

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "debug_sw", buf, sizeof(buf));
		if (!strcmp(buf, "1"))
			debug_sw = 1;
		else
			debug_sw = 0;
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

	/** set develop select */
	sel_info.debug_sw = debug_sw;
	ret = set_develop_select(&sel_info);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
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
 * lb_web_get_device_status: get device status
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_device_status(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	int ret = -1;

	device_status_config_info device_info;
	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&device_info, 0, sizeof(device_status_config_info));

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

	/** get device status */
	ret = get_device_status(&device_info);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	/** pack data and send */
	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}
	cJSON_AddStringToObject(obj, "company_name", device_info.company_name);
	cJSON_AddStringToObject(obj, "company_logo", device_info.company_logo);
	cJSON_AddStringToObject(obj, "device_address", device_info.device_address);
	cJSON_AddNumberToObject(obj, "language", device_info.language);
	cJSON_AddNumberToObject(obj, "punch_mode", device_info.punch_mode);
	cJSON_AddNumberToObject(obj, "debug_switch", device_info.debug_switch);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_device_status: set device status
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_device_status(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char buf[128];
	int ret = -1;

	device_status_config_info device_info;
	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&device_info, 0, sizeof(device_status_config_info));

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		mg_get_http_var(&p_http_msg->body, "company_name",
			device_info.company_name,
			sizeof(device_info.company_name));
		mg_get_http_var(&p_http_msg->body, "company_logo",
			device_info.company_logo,
			sizeof(device_info.company_logo));
		mg_get_http_var(&p_http_msg->body, "device_address",
			device_info.device_address,
			sizeof(device_info.device_address));

		mg_get_http_var(&p_http_msg->body, "language", buf, sizeof(buf));
		if (strcmp(buf, "1") == 0)
			device_info.language = 1;
		else
			device_info.language = 0;

		mg_get_http_var(&p_http_msg->body, "punch_mode", buf, sizeof(buf));
		if (strcmp(buf, "1") == 0)
			device_info.punch_mode = 1;
		else
			device_info.punch_mode = 0;

		mg_get_http_var(&p_http_msg->body, "debug_switch", buf, sizeof(buf));
		if (strcmp(buf, "1") == 0)
			device_info.debug_switch = 1;
		else
			device_info.debug_switch = 0;

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

	/** set device indo */
	ret = set_device_status(&device_info);
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
 * lb_web_get_ethparam: get eth param
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_ethparam(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	char buf[64] = {0};

	cJSON *root, *arr, *obj = NULL;
	net_para_config_info net_info;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&net_info, 0, sizeof(net_para_config_info));

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

	/** get param */
	if (get_eth_param(&net_info)) {
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
	cJSON_AddNumberToObject(obj, "enable_eth", net_info.enable);
	cJSON_AddNumberToObject(obj, "enable_eth_priority", net_info.priority);
	cJSON_AddNumberToObject(obj, "enable_eth_dhcp", net_info.dhcp);
	sprintf(buf, "%d.%d.%d.%d", net_info.eth_ip[0],
				net_info.eth_ip[1],
				net_info.eth_ip[2],
				net_info.eth_ip[3]);
	cJSON_AddStringToObject(obj, "eth_ip", buf);
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d.%d.%d.%d", net_info.eth_mask[0],
				net_info.eth_mask[1],
				net_info.eth_mask[2],
				net_info.eth_mask[3]);
	cJSON_AddStringToObject(obj, "eth_mask", buf);
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d.%d.%d.%d", net_info.eth_gateway[0],
				net_info.eth_gateway[1],
				net_info.eth_gateway[2],
				net_info.eth_gateway[3]);
	cJSON_AddStringToObject(obj, "eth_gateway", buf);
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d.%d.%d.%d", net_info.eth_dns[0],
				net_info.eth_dns[1],
				net_info.eth_dns[2],
				net_info.eth_dns[3]);
	cJSON_AddStringToObject(obj, "eth_dns", buf);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_ethparam: set eth param
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_ethparam(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64] = {0};
	char session_id[64] = {0};
	char buf[64] = {0};
	int ret = -1;

	cJSON *root, *arr, *obj = NULL;
	net_para_config_info net_cfg;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&net_cfg, 0 ,sizeof(net_cfg));

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		/** int type */
		mg_get_http_var(&p_http_msg->body, "enable_eth",
			buf, sizeof(buf));
		if (!strcmp(buf, "1"))
			net_cfg.enable = 1;
		else
			net_cfg.enable = 0;

		mg_get_http_var(&p_http_msg->body, "enable_eth_priority",
			buf, sizeof(buf));
		if (!strcmp(buf, "1"))
			net_cfg.priority = 1;
		else
			net_cfg.priority = 0;

		mg_get_http_var(&p_http_msg->body, "enable_eth_dhcp",
			buf, sizeof(buf));
		if (!strcmp(buf, "1"))
			net_cfg.dhcp = 1;
		else
			net_cfg.dhcp = 0;

		/** char type */
		mg_get_http_var(&p_http_msg->body, "eth_ip", buf, sizeof(buf));
		if (strlen(buf) > 0)
			strcpy(net_cfg.eth_ip, buf);
		mg_get_http_var(&p_http_msg->body, "eth_mask", buf, sizeof(buf));
		if (strlen(buf) > 0)
			strcpy(net_cfg.eth_mask, buf);
		mg_get_http_var(&p_http_msg->body, "eth_gateway", buf, sizeof(buf));
		if (strlen(buf) > 0)
			strcpy(net_cfg.eth_gateway, buf);
		mg_get_http_var(&p_http_msg->body, "eth_dns", buf, sizeof(buf));
		if (strlen(buf) > 0)
			strcpy(net_cfg.eth_dns, buf);
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
	if (chk_str_val(net_cfg.eth_ip) || chk_str_val(net_cfg.eth_mask) ||
		chk_str_val(net_cfg.eth_gateway) ||
		chk_str_val(net_cfg.eth_dns)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}

	/** set param */
	ret = set_eth_param(&net_cfg);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_sys_baseinfo: get sys base information
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_sys_baseinfo(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	int ret = -1;

	sys_base_config_info kernel_info;
	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&kernel_info, 0, sizeof(sys_base_config_info));

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

	/** get system kernel info */
	ret = get_sys_kernel_info(&kernel_info);
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
	cJSON_AddStringToObject(obj, "mode_number", kernel_info.mode_number);
	cJSON_AddStringToObject(obj, "firmware_version", kernel_info.firmware_version);
	cJSON_AddStringToObject(obj, "kernel_version", kernel_info.kernel_version);
	cJSON_AddStringToObject(obj, "product_sn", kernel_info.product_sn);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_get_wifi_param: get wifi param
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_wifi_param(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	int ret = -1;

	wifi_para_config_info wifi_param;
	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&wifi_param, 0, sizeof(wifi_config_para_t));

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

	/** get wifi param */
	ret = get_wifi_param(&wifi_param);
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
	cJSON_AddNumberToObject(obj, "enable_wifi", wifi_param.enable_wifi);
	cJSON_AddNumberToObject(obj, "enable_hotspot", wifi_param.enable_hotspot);
	cJSON_AddStringToObject(obj, "hotspot_name", wifi_param.hotspot_name);
	cJSON_AddStringToObject(obj, "hotspot_password", wifi_param.hotspot_password);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

/**
 * lb_web_set_wifi_param: set wifi param
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_set_wifi_param(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char buf[128];
	int ret = -1;

	wifi_para_config_info wifi_param;
	cJSON *root, *arr, *obj = NULL;

	/** para init */
	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	memset(&wifi_param, 0, sizeof(wifi_config_para_t));

	/** get data */
	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "enable_wifi", buf,
			sizeof(buf));
		if (!strcmp(buf, "1"))
			wifi_param.enable_wifi = 1;
		else 
			wifi_param.enable_wifi = 0;

		memset(buf, 0, sizeof(buf));
		mg_get_http_var(&p_http_msg->body, "enable_hotspot", buf,
			sizeof(buf));
		if (!strcmp(buf, "1"))
			wifi_param.enable_hotspot = 1;
		else 
			wifi_param.enable_hotspot = 0;

		mg_get_http_var(&p_http_msg->body, "hotspot_name",
			wifi_param.hotspot_name, sizeof(wifi_param.hotspot_name));
		mg_get_http_var(&p_http_msg->body, "hotspot_password",
			wifi_param.hotspot_password, sizeof(wifi_param.hotspot_password));

		mg_get_http_var(&p_http_msg->body, "security_type", buf, sizeof(buf));
		if (!strcmp(buf, "0"))
			wifi_param.enable_hotspot = 0;
		else if (!strcmp(buf, "2"))
			wifi_param.enable_hotspot = 2;
		else
			wifi_param.enable_hotspot = 1;
		break;

	default:
		break;
	}

	/** para check */
	if (chk_str_val(wifi_param.hotspot_name) ||
		chk_str_val(wifi_param.hotspot_password)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_STR_VAL);
		goto Exit;
	}

	/** check user */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		goto Exit;
	}

	/** set wifi param */
	ret = set_wifi_param(&wifi_param);
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

void *scan_wifi_thread(void *arg)
{
	cJSON *root, *arr, *obj = NULL;
	int ret = 0, i;
	http_thread_info *thread_info = (http_thread_info *)arg;
	wifi_scan_sys_list *wifi_list;

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	/** scan wifi list thread */
	wifi_list = scan_wifi_list(&ret);
	if (wifi_list == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		goto Exit;
	}

	ALOGD("[%d : %d]\n", __LINE__, wifi_list->wifi_list_num);
	/** pack data and send */
	for (i = 0; i < wifi_list->wifi_list_num; i++) {
		obj = cJSON_CreateObject();
		if (!obj) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			ret = ERR_INIT;
			goto Exit;
		}
		cJSON_AddNumberToObject(obj, "channel", wifi_list[i].wifi_list_num);
		cJSON_AddNumberToObject(obj, "rssi", wifi_list[i].rssi);
		cJSON_AddNumberToObject(obj, "security", wifi_list[i].security);
		cJSON_AddStringToObject(obj, "bssid", wifi_list[i].bssid);
		cJSON_AddStringToObject(obj, "ssid", wifi_list[i].ssid);
		cJSON_AddItemToArray(arr, obj);
	}
Exit:
	if (0 != ret)
		ret = -1;
	cJSON_AddItemToObject(root, "data", arr);
	restful_send_ok_result_thread(root, ret, thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_scan_wifi_info(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, scan_wifi_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_scan_wifi_list: get wifi list of scan
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_scan_wifi_list(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char notify_url[64] = {0};

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

	/** begin scan wifi list thread */
	thread_info->data = NULL;
	strcpy(thread_info->notify_url, notify_url);
	strcpy(thread_info->session_id, session_id);
	proc_scan_wifi_info(thread_info);
	return;

Exit:
	if (thread_info)
		lb_mem_free(thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
}

/**
 * lb_web_scan_device: scan device
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_scan_device(struct mg_connection *nc, int ev, void *p)
{
	char result[1400];

	if ((NULL == nc) || (NULL == p)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		return;
	}

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		break;

	default:
		break;
	}

	mg_printf(nc, "%s",
		"HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
	sprintf(result,
		"%s",
		"{\"data\":[],\"success\":\"0\",\"msg\":\"OK\"}");
	mg_send_http_chunk(nc, result, strlen(result));
	mg_send_http_chunk(nc, "", 0);
}

/**
 * lb_web_getwifiname: get connecting wifi name
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_getwifiname(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char username[64];
	char session_id[64];
	char wifi_name[64];
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

	ret = get_wifi_name(wifi_name);
	if (ret == -3) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, WIFI_NOT_ENABLE);
		goto Exit;
	} else {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	obj = cJSON_CreateObject();
	if (!obj) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}
	cJSON_AddStringToObject(obj, "connectname", wifi_name);
	restful_send_ok_result(nc, root, arr, obj);

Exit:
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	return;
}

void *connect_wifi_thread(void *arg)
{
	cJSON *root, *arr = NULL;
	int ret = 0;
	http_thread_info *thread_info = (http_thread_info *)arg;
	wifi_sys_info *data = thread_info->data;

	if (create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	ret = connect_wifi(data);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = STATU_FAIL;
		goto Exit;
	}

Exit:
	if (data)
		lb_mem_free(data);

	cJSON_AddItemToObject(root, "data", arr);
	restful_send_ok_result_thread(root, ret, thread_info);

	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif
	pthread_exit(0);
}

int proc_connect_wifi(http_thread_info *p_info)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, connect_wifi_thread, p_info);
	pthread_detach(thread_id);
	return 0;
}

/**
 * lb_web_connectwifi: connect wifi
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_connectwifi(struct mg_connection *nc, int ev, void *p)
{
	#ifdef CHECK_MALLOC
	lb_mem_dump();
	#endif

	struct http_message *p_http_msg = (struct http_message *)p;
	char buf[64];
	char username[64];
	char session_id[64];
	char notify_url[64] = {0};

	cJSON *root, *arr, *obj = NULL;
	wifi_sys_info *data = NULL;
	http_thread_info *thread_info = NULL;

	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		goto Exit;
	}

	/** pata init */
	thread_info = (http_thread_info *)lb_mem_alloc_ext(sizeof(http_thread_info),
		__func__, __LINE__);
	data = (wifi_sys_info *)lb_mem_alloc_ext(sizeof(wifi_sys_info),
		__func__, __LINE__);
	if (data == NULL || thread_info == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
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

		mg_get_http_var(&p_http_msg->body, "wifi_name", data->wifi_name,
			sizeof(data->wifi_name));
		mg_get_http_var(&p_http_msg->body, "wifi_password", data->wifi_password,
			sizeof(data->wifi_password));
		mg_get_http_var(&p_http_msg->body, "security", buf, sizeof(buf));

		if (!strcmp(buf, "0"))
			data->security = 0;
		else if (!strcmp(buf, "2"))
			data->security = 2;
		else
			data->security = 1;
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
	if (chk_str_val(data->wifi_name) || chk_str_val(data->wifi_password)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, STATU_FAIL);
		goto Exit;
	}

	restful_send_ok_result(nc, root, arr, obj);

	/** begin connect wifi thread */
	thread_info->data = data;
	strcpy(thread_info->notify_url, notify_url);
	strcpy(thread_info->session_id, session_id);
	proc_connect_wifi(thread_info);
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
 * get_macaddr: get mac address
 * @is_wifi: network card type(0/1: eth0/wifi)
 * @macaddr: mac address
 *
 * Returns 0 means success, other means failure.
 */
int get_macaddr(int is_wifi, char *macaddr)
{
	int ret = 0;
	char dev_name[40];
	unsigned char maddr[6];

	if (!macaddr)
		return -1;

	if (is_wifi)
		strcpy(dev_name, "wlan0");
	else
		strcpy(dev_name, "eth0");

	memset(maddr, 0, sizeof(maddr));
	ret = net_dev_get_mac_addr(dev_name, maddr);
	if (ret) {
		ALOGE("[%s: %d] net_dev_get_mac_addr(%s) failed, error:%d",
			__func__, __LINE__,
			dev_name, ret);
		return -1;
	}

	sprintf(macaddr, 
		"%02X:%02X:%02X:%02X:%02X:%02X",
		maddr[0],
		maddr[1],
		maddr[2],
		maddr[3],
		maddr[4],
		maddr[5]);

	return 0;
}


/**
 * lb_web_get_macaddr
 * @nc: client connection's object
 * @ev: event
 * @p: http message content
 *
 * Returns none.
 */
void lb_web_get_macaddr(struct mg_connection *nc, int ev, void *p)
{
	struct http_message *p_http_msg = (struct http_message *)p;
	char username[32];
	char session_id[32];
	char opts[4];
	char macaddr[20];

	int is_wifi = 0;
	int ret = -1;

	cJSON *root, *arr, *obj;

	if ((NULL == nc) || (NULL == p) || create_json(&root, &arr)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_INIT);
		return;
	}

	switch (ev) {
	case MG_EV_HTTP_REQUEST:
		mg_get_http_var(&p_http_msg->body, "user_name", username,
			sizeof(username));
		mg_get_http_var(&p_http_msg->body, "session_id", session_id,
			sizeof(session_id));
		mg_get_http_var(&p_http_msg->body, "ops", opts, sizeof(opts));
		is_wifi = atoi(opts);
		break;

	default:
		break;
	}

	/** para check */
	if (!find_user_from_list(username, session_id)) {
		ALOGE("[%s:%d] user check failure\n",
			__func__, __LINE__);
		send_fail_result(nc, root, arr, ERR_NOT_LOGIN);
		return;
	}
	
	memset(macaddr, 0, sizeof(macaddr));
	ret = get_macaddr(is_wifi, macaddr);
	if (!ret) {
		obj = cJSON_CreateObject();
		if (obj) {
			cJSON_AddStringToObject(obj, "macaddr", macaddr);
			restful_send_ok_result(nc, root, arr, obj);
			return;
		}
	}
	send_fail_result(nc, root, arr, STATU_FAIL);

	return;
}

#endif

