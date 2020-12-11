/*
 * ota_svc.c - all function for http web client.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>

#define LOG_TAG "ota_svc"
#include <log/log.h>

#include "cJSON.h"
#include "http_client.h"
#include "file_ops.h"
#include "file_http_download.h"
#include "update_svc.h"
#include "system_info.h"
#include "system_private.h"
#include "system_chip_id.h"

#include "ota_svc.h"
#include "db_all.h"

#define OTA_URL "http://ota.uz18.com:8999/get-firmware-info"
#define OTA_PATH "/data/ota"
#define SN_OFFSET 0

static int ota_cb_flag = 0;
static ota_change_cb ui_cb;
static void *ui;
static char ota_url[1024] = {0};

struct firmware_info_s {
	char firmware_version[32];
	char md5sum[64];
	int size;
	char download_url[128];
};

struct device_info_s {
	char name[64];
	char chip[64];
	char version[64];
	char sn[64];
	char mac[64];
};

struct device_info_s ota_dev_info = {0};
struct firmware_info_s firmware_info = {0};
int running = 0;

extern int net_get_info(char *ip, int i_size, char *mac, int m_size);

void init_ota_url()
{
	db_ui_get_value("ota_url", ota_url, 1024);
}

int get_ota_device_info(struct device_info_s *device_info)
{
	build_info_t build_info = {0};

	system_info_get_build_info(&build_info);
	memcpy(device_info->name, build_info.product_name, 64);
	memcpy(device_info->chip, build_info.product_chip, 64);
	memcpy(device_info->version, build_info.system_version, 64);
	system_get_private_data(SN_OFFSET, device_info->sn, 11);
	return 0;
}

int ota_http_parse_data(struct firmware_info_s *firmware_info, const char *ans, int ans_len)
{
	int ret = -1;
	cJSON *root = NULL;
	cJSON *tmp = NULL;

	root = cJSON_Parse((const char *)ans);
	if (NULL == root) {
		ALOGE("cJSON_Parse root failed\n");
		goto error;
	}

	tmp = cJSON_GetObjectItem(root, "firmware_version");
	if (NULL == tmp) {
		ALOGE("cJSON_GetObjectItem firmware_version failed\n");
		goto error;
	}
	snprintf(firmware_info->firmware_version,
		sizeof(firmware_info->firmware_version), "%s", tmp->valuestring);

	tmp = cJSON_GetObjectItem(root, "md5sum");
	if (NULL == tmp) {
		ALOGE("cJSON_GetObjectItem md5sum failed\n");
		goto error;
	}
	snprintf(firmware_info->md5sum, sizeof(firmware_info->md5sum),
		"%s", tmp->valuestring);

	tmp = cJSON_GetObjectItem(root, "size");
	if (NULL == tmp) {
		ALOGE("cJSON_GetObjectItem size failed\n");
		goto error;
	}
	firmware_info->size = tmp->valueint;

	tmp = cJSON_GetObjectItem(root, "download_url");
	if (NULL == tmp) {
		ALOGE("cJSON_GetObjectItem download_url failed\n");
		goto error;
	}
	snprintf(firmware_info->download_url,
		sizeof(firmware_info->download_url), "%s", tmp->valuestring);
	ret = 0;

error:

	if (root) {
		cJSON_Delete(root);
		root = NULL;
	}
	return ret;
}

bool ota_http_check_update(struct firmware_info_s *firmware_info, char* version)
{
	if (strcasecmp(firmware_info->firmware_version, version) != 0)
		return true;
	return false;
}

void download_finish_cb(void*data, int size)
{
	ALOGD("%s: data=%s\n", __func__, (char *)data);
	if (ui_cb)
		ui_cb(ui, OTA_STATE_CHECKING);
}

int ota_http_remove_useless_eupgrade(char *filename)
{
	DIR *dir;
	struct dirent *ptr;
	char filename_tmp[128];

	snprintf(filename_tmp, 128, "%s.tmp", filename);

	dir = opendir(OTA_PATH"/");
	if (dir == NULL) {
		ALOGE("%s:Open dir error!\n", __func__);
		return -1;
	}

	while ((ptr = readdir(dir)) != NULL) {
		if (strcmp(ptr->d_name, ".") == 0
			|| strcmp(ptr->d_name, "..") == 0)
			continue;
		else if (ptr->d_type == DT_REG) {
			if (strncmp(ptr->d_name, filename, strlen(filename)) == 0 ||
				strncmp(ptr->d_name, filename_tmp, strlen(filename_tmp)) == 0) {
				ALOGD("cmp d_name: %s\n", ptr->d_name);
			} else {
				ALOGD("remove d_name: %s\n", ptr->d_name);
				remove(ptr->d_name);
			}
		}
	}
	return 0;
}

int ota_http_start_update(struct download_handler_s *http_download)
{
	int ret = -1;
	char filename[64] = {0};
	char filepath[128] = {0};

	snprintf(filename, 64, "EUPGRADE-%s.pkg", firmware_info.md5sum);
	snprintf(filepath, 128, OTA_PATH"/%s", filename);

	if (is_dir_exist(OTA_PATH"/")) {
		ret = mk_mul_dirs(OTA_PATH"/");
		if (ret != 0) {
			ALOGE("%s:mk_mul_dirs error!\n", __func__);
			return -1;
		}
	}
	ota_http_remove_useless_eupgrade(filename);

	http_download->set_url(http_download, firmware_info.download_url, OTA_PATH, filename);
	http_download->set_auto_free_res(http_download);
	http_download->set_finish_cb(http_download, download_finish_cb, filepath, strlen(filepath));
	http_download->start(http_download);
	return 0;
}

void ota_http_req_cb (int state, const char *req, int req_len,
		const char *ans, int ans_len)
{
	int ret = -1;
	if (-1 == ota_cb_flag) {
		ALOGD("http >> %s: had been cancle exit", __func__);
		return;
	}
	if (0 == state) {
		ALOGD("http >> %s: send req[%s] return ans[%s]", __func__, req, ans);

		memset(&firmware_info, 0, sizeof(struct firmware_info_s));
		ret = ota_http_parse_data(&firmware_info, ans, ans_len);
		if (ret == 0) {
			ALOGD("version: %s\n", firmware_info.firmware_version);
			ALOGD("md5sum: %s\n", firmware_info.md5sum);
			ALOGD("size: %d\n", firmware_info.size);
			ALOGD("download_url: %s\n", firmware_info.download_url);
			if (ota_http_check_update(&firmware_info, ota_dev_info.version)) {
				if (ui_cb)
					ui_cb(ui, OTA_STATE_PROMPT);
			} else {
				if (ui_cb)
					ui_cb(ui, OTA_STATE_IDEL);
			}
			ota_cb_flag = 1;
		} else {
			ota_cb_flag = -1;
		}
	} else if (-2 == state) {
		ALOGW("http >> %s: send req[%s] exit", __func__, req);
		ota_cb_flag = -1;
	} else {
		ALOGW("http >> %s: send req[%s] error", __func__, req);
		ota_cb_flag = -2;
	}
}

int ota_http_get_sync(const char *url, unsigned char *data, size_t len)
{
	int ret = 0;
	struct mg_mgr *g_mgr = NULL;
	struct http_hdl *hdl = NULL;

	while(running != 0) {
		usleep(100 * 1000);
		ALOGD("%s: wait last ota post callback", __func__);
	}
	ALOGD("%s: start ota http seach >> %s", __func__, url);
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
	hdl->cb = ota_http_req_cb;
	http_client_init(g_mgr, hdl);
	http_client_send_Req(g_mgr, url,(const char *)data);
	while(ota_cb_flag == 0) {
		http_client_poll(g_mgr);
	}

	if(1 != ota_cb_flag)
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
	running = 0;
	ota_cb_flag = 0;
	ALOGD("%s: end ota http seach:%d", __func__, ret);
	return ret;

}

void cancle_ota_http_get()
{
	ALOGD("http >> %s: cancle exit", __func__);
	if (running)
		ota_cb_flag = -1;
}

void set_state_callback(void *ui_ota, ota_change_cb cb)
{
	ui = ui_ota;
	ui_cb = cb;
}

int ota_http_check_updateinfo(void)
{
	int ret = -1;
	cJSON *root = NULL;
	char *request = NULL;
	char ip[64] = {0};
	char mac[64] = {0};

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
				errno, strerror(errno));
		goto error;
	}

	memset(&ota_dev_info, 0, sizeof(struct device_info_s));
	if (get_ota_device_info(&ota_dev_info)!= 0) {
		ALOGE("Get dev_info failed!\n");
		ret = -3;
		goto error;
	}

	system_get_chip_id(ota_dev_info.chip, sizeof(ota_dev_info.chip));
	cJSON_AddStringToObject(root, "name", ota_dev_info.name);
	cJSON_AddStringToObject(root, "chip", ota_dev_info.chip);
	cJSON_AddStringToObject(root, "version",  ota_dev_info.version);
	cJSON_AddStringToObject(root, "sn", ota_dev_info.sn);
	net_get_info(ip, sizeof(ip), mac, sizeof(mac));
	cJSON_AddStringToObject(root, "mac", mac);
	request = cJSON_PrintUnformatted(root);
	if (strlen(ota_url) > 0)
		ret = ota_http_get_sync(ota_url,
			(unsigned char *)request, strlen(request));
	else
		ret = ota_http_get_sync(OTA_URL,
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

int get_ota_firmware_version(char *str, int s_size) {
	int len = sizeof(firmware_info.firmware_version);
	if (len > s_size)
		return -1;
	memcpy(str, firmware_info.firmware_version, len);
	return 0;
}

int http_test(void)
{
	int ret = -1;
	cJSON *root = NULL;
	char *request = NULL;

	root = cJSON_CreateObject();
	if (NULL == root) {
		ALOGE("malloc root failed, errno: %d (%s)\n",
				errno, strerror(errno));
		goto error;
	}

	cJSON_AddStringToObject(root, "name", "hyk");
	request = cJSON_PrintUnformatted(root);
	ret = ota_http_get_sync(OTA_URL, (unsigned char *)request, strlen(request));
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

