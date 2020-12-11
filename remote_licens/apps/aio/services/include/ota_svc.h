/*
 * ota_svc.h - resetting service background for monitoring reset key and
 *               provide resetting device api
 *
 *
 * Copyright (C) 2016-2020, LomboTech Co.Ltd.
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

 #ifndef __OTA_SVC_H__
 #define __OTA_SVC_H__


typedef enum {
	OTA_STATE_IDEL = 0,
	OTA_STATE_SEARCH, //1
	OTA_STATE_PROMPT, //2
	OTA_STATE_DOWNING, //3
	OTA_STATE_CHECKING, //4
	OTA_STATE_REBOOT
} ota_state;

 typedef void (*ota_change_cb)(void *ui_ota, ota_state state);

 void init_ota_url();

 void set_state_callback(void *ui_ota, ota_change_cb cb);

 void cancle_ota_http_get();
 int ota_http_start_update(struct download_handler_s *http_download);

 int ota_http_check_updateinfo(void);

 int get_ota_firmware_version(char *str, int s_size);

 int http_test(void);

 #endif /* __OTA_SVC_H__ */
