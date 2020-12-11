/*
 * api/sys_http.c - all functions for http sys function
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
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/reboot.h>

#include "cJSON.h"
#include "config_db.h"

#define LOG_TAG "sys_http.c"
#include <log/log.h>

#include "person.h"
#include "jpeg_dec.h"
#include "face_svc.h"
#include "sys_http.h"
#include "net_wifi.h"
#include "system_factory.h"
#include "config.h"
#include "face.h"
#include "lb_os_http.h"
#include "base64.h"
#include "sys_info.h"
#include "take_photo.h"

/**
 * get_jpg_quality: get image quality
 * data: image data
 * len: data len
 * quality: quality of image
 *
 * Returns 0 means success, other means failure.
 */
int get_jpg_quality(const char *data, int len, float *quality)
{
#if 0
	struct jpeg_dec_output	output;
	face_frame_t		frame;
	int			ret = PERSON_REG_OK;
	int			face_num = 0;
	face_info_t		*info = NULL;
	int			w_stride = 0;
	int			h_stride = 0;

	/** para check */
	if (NULL == data)
		return -1;

	w_stride = config_get_int("register", "jpeg_w_stride", 800);
	h_stride = config_get_int("register", "jpeg_h_stride", 1024);
	ret = jpeg_dec_data((char *)data, len, w_stride, h_stride,
			JPEG_SCALE_AUTO, &output);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = PERSON_INFO_IMAGE_DECODE_FAIL;
		goto Exit;
	}

	info = (face_info_t *)malloc(sizeof(face_info_t));
	if (!info) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = PERSON_INFO_INTERNAL_ERROR;
		goto Exit;
	}

	/* get face info */
	frame.width = output.stride_width;
	frame.height = output.stride_height;
	frame.viraddr = output.virt_addr;
	frame.phyaddr = output.phy_addr;
	frame.size = output.size;
	face_num = face_recg_block((void *)(get_face_handle()), &frame, 1, info);
	if (face_num < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = PERSON_INFO_INTERNAL_ERROR;
		goto Exit;
	}

	/* check face num */
	if (face_num == 0) {
		ret = PERSON_INFO_NO_DETECT_FACE;
		ALOGE("[%s : %d : ret = %d]\n", __func__, __LINE__, ret);
		goto Exit;
	} else if (face_num > 1) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = PERSON_INFO_MULTIPLE_FACE;
		goto Exit;
	}
	*quality = info->rect.quality_score;

Exit:
	jpeg_dec_end(&output);
	if (info) {
		lb_mem_free(info);
		info = NULL;
	}
	return ret;
#else
	*quality = 0;
	return 0;
#endif
}

int get_pic_quality(char *data, float *quality)
{
	int ret = 0, jpg_len = 0;
	char *jpg_data;

	/** para check */
	if (data == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	ret = base64_decode(data, strlen(data),
		&jpg_data, &jpg_len);
	if (!ret && jpg_data && (jpg_len > 0)) {
		ret = get_jpg_quality(jpg_data, jpg_len, quality);
		if (ret) {
			ALOGE("[%s : %d : ret = %d]\n", __func__, __LINE__, ret);
			goto Exit;
		}
		lb_mem_free(jpg_data);
	}

Exit:
	return ret;
}

int reboot_http()
{
	int ret = 0;

	ret = reboot(RB_AUTOBOOT);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
	}

	return ret;
}

int take_photo_http(char **base64_str)
{
	int ret = 0, rlen, base64_len;
	char *pbin = NULL;

	ret = take_photo((unsigned char **)&pbin, &rlen);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	ret = base64_encode(pbin, rlen, base64_str, &base64_len);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

Exit:
	camera_free_mem_output(pbin);

	return ret;
}

int get_face_feature_http(char *file_data,
	char **b64_feature, char **b64_key_points, float *norm)
{
	int ret = 0, jpg_len, b64_kpts_len, b64_feat_len;
	char key_points[64];
	char feature[1200];
	char *jpg_data;

	/** para check */
	if (file_data == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	ret = base64_decode(file_data, strlen(file_data), &jpg_data, &jpg_len);
	if (ret || !jpg_data || !(jpg_len > 0)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

	ret = person_get_feats_from_data(jpg_data, jpg_len, 0, feature);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -3;
		goto Exit;
	}

	memcpy(key_points, feature+FACE_FEAT_SIZE, 10*sizeof(float));
	memcpy(norm, feature+FACE_FEAT_SIZE+10*sizeof(float), sizeof(float)*2);
	ret = base64_encode(feature, FACE_FEAT_SIZE, b64_feature, &b64_feat_len);
	ret |= base64_encode(key_points, 10*sizeof(float),
		b64_key_points, &b64_kpts_len);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -4;
		goto Exit;
	}

Exit:
	if (jpg_data)
		lb_mem_free(jpg_data);
	return ret;
}

wifi_scan_sys_list *scan_wifi_list(int *ret)
{
	int i;
	wifi_config_para_t wifi_param;
	wifi_event_para_t *wifi_list;
	wifi_scan_sys_list *wifi_scan_list;

	/** para check */

	/** get wifi status */
	memset(&wifi_param, 0, sizeof(wifi_config_para_t));
	*ret = net_wifi_get_para(&wifi_param);
	if (*ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		*ret = -2;
		goto Exit;
	}
	if (!wifi_param.enable_wifi) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		*ret = -3;
		goto Exit;
	}

	/** get wifi scan list */
	*ret = net_wifi_start_scan();
	if (*ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		*ret = -4;
		goto Exit;
	}
	wifi_list = net_wifi_get_events();
	if (!wifi_list || !(wifi_list->result.num)) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		*ret = -5;
		goto Exit;
	}

	/** return wifi scan list data */
	wifi_scan_list = (wifi_scan_sys_list *)calloc(wifi_list->result.num,
		sizeof(wifi_scan_sys_list));
	for (i = 0; i < wifi_list->result.num; i++) {
		wifi_scan_list[i].channel = wifi_list->result.info[i].channel;
		wifi_scan_list[i].rssi = wifi_list->result.info[i].rssi;
		wifi_scan_list[i].wifi_list_num = wifi_list->result.num;
		wifi_scan_list[i].security = wifi_list->result.info[i].security;
		strcpy(wifi_scan_list[i].bssid, wifi_list->result.info[i].bssid);
		strcpy(wifi_scan_list[i].ssid, wifi_list->result.info[i].ssid);
	}

	return wifi_scan_list;
Exit:
	return NULL;
}

int factory_default()
{
	int ret = 0;
	factory_restore_ctx_t ctx;

	memset(&ctx, 0, sizeof(ctx));
	ctx.done = EE_THREAD_NOT_RUN;
	ctx.result_code = RESTORE_ERR;

	ALOGD("start restore factory...\n");
	system("killall logcat");
	sleep(3);
	system_factory_thread();
	while (1) {
		sys_factory_get_ctx(&ctx);
		if (ctx.done == EE_THREAD_DONE)
			break;
		usleep(1000);
	}
	ALOGD("complete restore factory.\n");

	if (ctx.result_code != RESTORE_OK) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

	system("logcat -v time -r2048 -n 5 -f '/data/log/`date +%Y%m%d-%H-%M-%S`.log' &");

Exit:
	return ret;
}

/**
 * write_tmp_fw_pack_bin_file: write fw packet to temp file
 * @index: packet index
 * @data: fw data
 * @len: data length
 *
 * Returns 0 means success, other means failure.
 */
int write_tmp_fw_pack_bin_file(int index, const char *data, int len)
{
	char file_name[256];
	FILE *fp;

	if (!data || !len)
		return -1;

	sprintf(file_name, "/data/fw_%d.bin", index);
	fp = fopen(file_name, "wb");
	if (!fp)
		return -1;

	fwrite(data, 1, len, fp);
	fclose(fp);

	return 0;
}

/**
 * write_fw_file_to_flash: write fw file to flash
 * @total: toal of file
 * @md5_str: md5 string
 *
 * Returns 0 means success, other means failure.
 */
int write_fw_file_to_flash(int total, const char *md5_str)
{
	char src_name[256];
	char buffer[1400];
	char line[100];
	FILE *fp, *fp_dst, *file;
	int i, len, ret, offset;

	/*
	if (!md5_str)
		return -1;
	*/

	fp_dst = fopen("/data/firmware.bin", "wb");
	if (!fp_dst)
		return -1;

	for (i = 0; i < total; i++) {
		sprintf(src_name, "/data/fw_%d.bin", i);
		fp = fopen(src_name, "rb");
		if (!fp) {
			fclose(fp_dst);
			remove("/data/firmware.bin");
			return -1;
		}

		fseek(fp, 0, SEEK_END);
		len = ftell(fp);

		offset = 0;
		fseek(fp, 0, SEEK_SET);
		while (offset < len) {
			ret = fread(buffer, 1, sizeof(buffer), fp);
			if (ret > 0) {
				fwrite(buffer, 1, ret, fp_dst);
				offset += ret;
			} else {
				ALOGE("%d [file %d]fread(off %d tl %d) ret %d\n",
					__LINE__, i, offset, total, ret);
				break;
			}
		}
		fclose(fp);
	}
	fclose(fp_dst);

	for (i = 0; i < total; i++) {
		sprintf(src_name, "/data/fw_%d.bin", i);
		remove(src_name);
	}

	/** get md5_str */
	file = popen("md5sum /data/firmware.bin", "r");
	if (file != NULL) {
		while (fgets(line, 100, file) != NULL)
			ALOGD("[%s : %d : %s]\n", __func__, __LINE__, line);
	} else {
		pclose(file);
		return 1;
	}

	if (strstr(line, md5_str) == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		pclose(file);
		return 1;
	}

	pclose(file);

	return 0;
}

int get_wifi_name(char *wifiname)
{
	int ret = 0;
	wifi_config_para_t wifi_param;

	/** para check */
	if (wifiname == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	/** para init */
	memset(&wifi_param, 0, sizeof(wifi_config_para_t));

	ret = net_wifi_get_para(&wifi_param);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

	if (!wifi_param.enable_wifi) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -3;
		goto Exit;
	}

	ret = net_wifi_get_connect_ssid(wifiname);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -4;
		goto Exit;
	}

Exit:
	return ret;
}

int connect_wifi(wifi_sys_info *p_info)
{
	int ret = 0;
	wifi_config_para_t wifi_param;

	/** para check */
	if (p_info == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	/** para init */
	memset(&wifi_param, 0, sizeof(wifi_config_para_t));

	ret = net_wifi_get_para(&wifi_param);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

	if (!wifi_param.enable_wifi) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -3;
		goto Exit;
	}

	ret = sta_connect(p_info->wifi_name, p_info->wifi_password, p_info->security);
	if (ret == -1) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -4;
		goto Exit;
	}

Exit:
	return ret;
}

/**
 * read_param_from_file: get json object from ini file
 * @file_name: file name
 *
 * Returns 0 means failure, other means json object.
 */
cJSON *read_param_from_file(const char *file_name)
{
	FILE *fp;
	char *buffer;
	int length;
	cJSON *root;

	if (!file_name) {
		ALOGE("read_param_from_file() file_name is NULL\n");
		return NULL;
	}

	fp = fopen(file_name, "rb");
	if (!fp) {
		ALOGE("read_param_from_file() open(%s) failure\n", file_name);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	length = ftell(fp);

	buffer = (char *)malloc(length+1);
	if (!buffer) {
		ALOGE("read_param_from_file() malloc(%d) failure\n", length+1);
		fclose(fp);
		return NULL;
	}

	memset(buffer, 0, length+1);
	fseek(fp, 0, SEEK_SET);
	fread(buffer, 1, length, fp);
	fclose(fp);

	root = cJSON_Parse(buffer);
	lb_mem_free(buffer);

	return root;
}

/**
 * get_ini_value - get param value from ini file
 * @field_name: name of the parameter field
 * @field_value: value of the parameter field
 * @value_len: save the buffer length of the parameter value.
 *
 * Returns 0 if valid, otherwise not 0.
 */
int get_ini_value(const char *field_name, char *field_value, int value_len)
{
	int ret = -1;

	cJSON *root;
	cJSON *item;

	if ((NULL == field_name) || (NULL == field_value))
		return -1;

	root = read_param_from_file(CLOUDSERVERCONF);
	if (!root)
		return -1;

	item = cJSON_GetObjectItem(root, field_name);
	if (item) {
		if (value_len > strlen(item->valuestring)) {
			strcpy(field_value, item->valuestring);
			ret = 0;
		}
	}
	cJSON_Delete(root);

	return ret;
}

/**
 * set_ini_value - set param value to ini file
 * @field_name: name of the parameter field
 * @field_value: value of the parameter field
 *
 * Returns 0 if valid, otherwise not 0.
 */
int set_ini_value(const char *field_name, const char *field_value)
{
	int ret = -1;
	char *pjson;

	cJSON *root;
	cJSON *item;
	FILE *fp;

	if ((NULL == field_name) || (NULL == field_value))
		return -1;

	root = read_param_from_file(CLOUDSERVERCONF);
	if (!root) {
		root = cJSON_CreateObject();
		if (root) {
			cJSON_AddStringToObject(root, field_name,
				field_value);
			pjson = cJSON_PrintUnformatted(root);
			if (pjson) {
				fp = fopen(CLOUDSERVERCONF, "wb");
				if (fp) {
					fwrite(pjson, 1, strlen(pjson), fp);
					fclose(fp);
					ret = 0;
				}
				lb_mem_free(pjson);
			}
			cJSON_Delete(root);
		}
		return ret;
	}

	item = cJSON_GetObjectItem(root, field_name);
	if (item)
		cJSON_DeleteItemFromObject(root, field_name);

	cJSON_AddStringToObject(root, field_name, field_value);

	pjson = cJSON_PrintUnformatted(root);
	if (pjson) {
		fp = fopen(CLOUDSERVERCONF, "wb");
		if (fp) {
			fwrite(pjson, 1, strlen(pjson), fp);
			fclose(fp);
			ret = 0;
		}
		lb_mem_free(pjson);
	}
	cJSON_Delete(root);

	return ret;
}

/**
 * check_string_is_validate: check string validate
 * @str: string that check
 *
 * Returns none.
 */
int chk_str_val(char *str)
{
	int str_length = 0, i;

	if (!str)
		return -1;
	str_length = strlen(str);

	for (i = 0; i < str_length; i++) {
		if ((str[i] >= 33 && str[i] <= 47) ||
			(str[i] >= 58 && str[i] <= 64) ||
			(str[i] >= 91 && str[i] <= 96) ||
			(str[i] >= 123 && str[i] <= 126)) {
			if (str[i] == 46 || str[i] == 58)
				return 0;
			ALOGE("[%s : %d : -->%c]", __func__, __LINE__, str[i]);
			return -2;
		}
	}

	return 0;
}