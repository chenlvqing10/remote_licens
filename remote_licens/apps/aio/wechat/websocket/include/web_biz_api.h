/*
 * web_biz_api.h - all functions for RESTful.
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


#include "mongoose.h"
#include "http_main.h"


#if 0
typedef struct tag_device_params {
	char name[256];
	char location[256];
	char inout; /* 0: door out, 1: door in, 3: door normal */
	char upblack; /* 0: normal, 1: upload */
	char uploadImage; /* 0: upload, 1: normal */
	char imageQuality; /* [0-5] */
	char pwd[32]; /* 6 bit password */
	char whitevalue; /* [70-100]: 88 */
	char mapvalue; /* [30-100]: 80 */
	char recogSpaceTime; /* [10-255] seconds */
	char delayvalue; /* default 3s */
	char banner[256];
	char bannerSpaceTime; /* [1-255] seconds */
	char bannerEnable; /* 0: enable, 1: disable */
	char imageSaveEnable; /* 0: enable, 1: disable */
	char detectVoiceEnable; /* 0: enable, 1: disable */
	char lightLevelPercent; /* [0-100] */
	char lightType; /* 0:close alway; 1:open alway; 2: time limit; 3: smart mode */
	char lightTimeStart[16]; /* default 20:00 */
	char lightTimeEnd[16]; /* default 06:00 */
	char systemVol; /* [0-7] */
	char detectRange; /* [0-5] ->[~, 0.5m, 1m, 2m, 3m, 4m] */
	char platformEnable; /* 0: open, 1: close */
	char platformIp[32];
	char wgOutType; /* 0:wg26, 1:wg34, 2:wg66 */
	char livenessEnable; /* 0: enable, 1: disable */
	char livenessValue; /* [0-10] */
	char resultNameEncrypt; /* 0: enable, 1: disable */
	char recogModeDB; /* 0: disable face recg, 1: enable face recg */
	char recogModeIC; /* 0: disable ic, 1: enable ic,  2: enable ic and face recg */
	char recogModeID; /* 0: disable id, 1: enable id,  2: enable id and recg white list */
	char recogRelay; /* 0: output, 1: idel */
	char resultFormat[256]; /* default: '{%s}, welcome' */
	char resultVoiceFormat[256]; /* default: '{%s}, welcome' */
	char rebootTime[32]; /* restart dev time, form: DDHHmm */
	char voiceSetting; /* 0:default, 1:name, 2:custom */
	char resultSetting; /* 0:default, 1: custom */
	char hearBeatEnable; /* 0: enable, 1: disable */
	char hearBeatIp[32];
	/** **/
	char tempEnable; /* 0: disable, 1: enable */
	char tempBroadcast; /* 0: disable, 1: enable */
	char tempHighRing; /* 0: disable, 1: enable */
	char tempRingStop; /* 0: disable, 1: enable */
	char tempNonResidentEnable; /* 0: disable, 1: enable */
	char tempCheckMode; /* 0: disable, 1: enable */
	char tempCheckMax;
	char tempCheckMin;
	char tempNormalMax;
	char masksEnable; /* 0: disable, 1: enable */
	char noMasksRing; /* 0: disable, 1: enable */
	char noMasksStop; /* 0: disable, 1: enable */
}device_params_t;
#endif

 /**
 * @brief send checkin data
 * @note  read checkin data from db and send to server
 * max 199 once
 * @retval
 */
int client_send_checkin(const char *url);
int httpclient_send_sync(const char *url,
	unsigned char *data, size_t len);


void web_handle_cb(web_handler_cb cb);

void lb_web_login(struct mg_connection *nc, int ev, void *p);
void lb_web_set_dev_hearbeat(struct mg_connection *nc, int ev, void *p);
void lb_web_get_dev_params(struct mg_connection *nc, int ev, void *p);
void lb_web_set_dev_params(struct mg_connection *nc, int ev, void *p);
void lb_web_restore_dev_params(struct mg_connection *nc, int ev, void *p);
void lb_web_get_dev_time(struct mg_connection *nc, int ev, void *p);
void lb_web_set_dev_time(struct mg_connection *nc, int ev, void *p);
void lb_web_upload_upgrade_file(struct mg_connection *nc, int ev, void *p);
void lb_web_set_dev_reboot(struct mg_connection *nc, int ev, void *p);
void lb_web_set_dev_remoteopnen(struct mg_connection *nc, int ev, void *p);
void lb_web_get_dev_version(struct mg_connection *nc, int ev, void *p);
void lb_web_get_snap_picture(struct mg_connection *nc, int ev, void *p);
void lb_web_set_dev_logo(struct mg_connection *nc, int ev, void *p);
void lb_web_restore_dev_logo(struct mg_connection *nc, int ev, void *p);
void lb_web_set_dev_network(struct mg_connection *nc, int ev, void *p);
void lb_web_get_snap_face(struct mg_connection *nc, int ev, void *p);
void lb_web_del_advert_pngs(struct mg_connection *nc, int ev, void *p);
void lb_web_add_advert_png(struct mg_connection *nc, int ev, void *p);
void lb_web_get_dev_log(struct mg_connection *nc, int ev, void *p);
void lb_web_del_dev_log(struct mg_connection *nc, int ev, void *p);
void lb_web_add_white_list(struct mg_connection *nc, int ev, void *p);
void lb_web_del_white_list(struct mg_connection *nc, int ev, void *p);
void lb_web_remove_white_lists(struct mg_connection *nc, int ev, void *p);
void lb_web_get_white_list(struct mg_connection *nc, int ev, void *p);
void lb_web_get_white_member(struct mg_connection *nc, int ev, void *p);
void lb_web_set_identify_callback(struct mg_connection *nc, int ev, void *p);
void lb_web_set_access_groups(struct mg_connection *nc, int ev, void *p);
void lb_web_set_special_groups(struct mg_connection *nc, int ev, void *p);
void lb_web_ip_setting(struct mg_connection *nc, int ev, void *p);
void lb_web_set_wifi_state(struct mg_connection *nc, int ev, void *p);
void lb_web_get_scan_wifi_results(struct mg_connection *nc, int ev, void *p);
void lb_web_connect_wifi(struct mg_connection *nc, int ev, void *p);
void lb_web_factory_default(struct mg_connection *nc, int ev, void *p);
void lb_web_alg_setting(struct mg_connection *nc, int ev, void *p);
void lb_web_upgrade(struct mg_connection *nc, int ev, void *p);
void lb_web_get_wifi_state(struct mg_connection *nc, int ev, void *p);

void lb_web_recv_test(struct mg_connection *nc, int ev, void *p);


#endif /* _WEB_BIZ_API_H_ */

