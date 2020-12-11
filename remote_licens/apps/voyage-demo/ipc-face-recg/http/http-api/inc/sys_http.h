/*
 * sys_http.h - export some interface function of sys http function
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _SYS_HTTP_H
#define _SYS_HTTP_H

#include "wifi_station.h"
#include "cJSON.h"
#include "ee_queue.h"

#define BSSID_MAX_LENGTH 17
#define SSID_MAX_LENGTH 32
#define CLOUDSERVERCONF "/data/cloud_server.ini"
#define ONE_PACK_MAX_RECORD_NUM		(20)
#define REG_PIC_DEFAULT_PATH		"/tmp/reg_default.jpg"

typedef struct wifi_scan_list {
	int channel;
	int rssi;
	int wifi_list_num;
	SECURITY security;
	char bssid[BSSID_MAX_LENGTH+1];
	char ssid[SSID_MAX_LENGTH+1];
} wifi_scan_sys_list;

typedef struct wifi_info {
	char wifi_name[64];
	char wifi_password[64];
	int security;
} wifi_sys_info;

wifi_scan_sys_list *scan_wifi_list(int *ret);
int get_pic_quality(char *data, float *quality);
int reboot_http();
int take_photo_http(char **base64_str);
int get_face_feature_http(char *file_data,
	char **b64_feature, char **b64_key_points, float *norm);
int factory_default();
int write_tmp_fw_pack_bin_file(int index, const char *data, int len);
int write_fw_file_to_flash(int total, const char *md5_str);
int get_wifi_name(char *wifiname);
int connect_wifi(wifi_sys_info *p_info);
cJSON *read_param_from_file(const char *file_name);
int get_ini_value(const char *field_name, char *field_value, int value_len);
int set_ini_value(const char *field_name, const char *field_value);
int chk_str_val(char *str);

#endif
