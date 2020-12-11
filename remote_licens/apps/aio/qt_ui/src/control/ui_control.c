/*
* ui_control.c - define interface function of ui_control
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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <stdlib.h>
#include <string.h>

#define LOG_TAG "ui_control"
#include <log/log.h>

#include <pthread.h>
#include <sys/reboot.h>
#include <sys/statfs.h>
#include <sys/prctl.h>
#include <errno.h>

#include "mongoose.h"
#include "ws_svc_main.h"
#include "reset_svc.h"
#include "update_svc.h"
#include "factory_test.h"
#include "screen_svc.h"
#include "system_private.h"
#include "system_info.h"
#include "ws_bt_wifi.h"
#include "ws_http.h"
#include "web_biz_api.h"
#include "ui_control.h"
#include "dd_md5_i.h"
#include "wiegand_write.h"
#include "light_svc.h"
#include "screen_svc.h"
#include "display.h"
#include "led.h"
#include "system_volume.h"
#include "time_conv.h"
#include "open_door_svc.h"
#include "wiegand_svc.h"
#include "alarm_svc.h"
#include "reboot_svc.h"
#include "time_conv.h"
#include "time_ntp.h"
#include "file_ops.h"

#define SN_OFFSET 0
#define SECRETNO_OFFSET 0x200
#define DEVICEID_OFFSET 0x400
#define QRCODE_OFFSET 0x600

#define RGBNIR_MAP_CHECK 0x5B
#define RGBNIR_MAP_OFFSET 0x1024

#define SN_BUFFER_SIZE 10

#define TRY_TEST_COUNT 4
#define TRY_TEST_TIME 1

#define OPEN_DOOR_DELAY_TIME 5
#define WIEGAND_DATA_SIZE 26

#define MAX_BUF_LEN 256
#define PATH_TIMEZONE "/etc/timezone"
#define REGISTER_PIC_SAVE_PATH		"/data/face_register"
#define USBUPGRADE_PKG_PATH "/mnt/usb/"

/* mode will change */
static enum_mode_t s_mode;

extern long long get_systime_us(void);

/**
 * @brief  start batch import
 * @note   NULL
 * @retval 0 success 1 failed
 */
int ui_batch_import_start()
{
	return 0;
}

/**
 * @brief  fatory default
 * @note   NULL
 * @retval 0 success 1 failed
 */
int ui_factory_default(void)
{
	return reset_device_immed();
}

/**
 * @brief  set default flag
 * @note   NULL
 * @retval None
 */
void ui_factory_set_flag(void)
{
	set_reset_flag();
}

/**
 * update_device_start - upgrade device firmware.
 *
 * Returns 0  if success, other if failure.
 */
int ui_update_device_start(void)
{
	int ret = -1;
	ret = update_device_copy_pkg(USBUPGRADE_PKG_PATH); /* Returns 0  if find, -1 if failure, 1 not find ,2 in_operation*/
	if(ret == 0)
		return update_device_start();
	return ret;
}

/**
 * @brief  set eth param
 * @note   NULL
 * @param  *p_info: device_eth_param_t
 * @retval 0 success 1 failed
 */
int ui_device_eth_param_update(net_config_para_t *p_info)
{
	device_net_param_t net_info;
	if (!p_info) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}
	memset(&net_info, 0, sizeof(device_net_param_t));
	db_device_net_param_query(&net_info);
	net_info.enable_eth = p_info->enable;
	net_info.enable_eth_dhcp = p_info->dhcp;
	strncpy(net_info.eth_ip, p_info->eth_ip, 32);
	strncpy(net_info.eth_mask, p_info->eth_mask, 32);
	strncpy(net_info.eth_gateway, p_info->eth_gateway, 32);
	strncpy(net_info.eth_dns, p_info->eth_dns, 32);
	db_device_net_param_update(&net_info);
	return 0;
}

/**
 * @brief  get eth param
 * @note   NULL
 * @param  *p_info: device_eth_param_t
 * @retval 0 success 1 failed
 */
int ui_device_eth_param_query(net_config_para_t *p_info)
{
	device_net_param_t net_info;
	/*verify parameters*/
	if (!p_info) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}
	/*parameter init*/
	memset(&net_info, 0x00, sizeof(net_info));
	/*read device net parameter*/
	db_device_net_param_query(&net_info);
	p_info->enable = net_info.enable_eth;
	p_info->dhcp = net_info.enable_eth_dhcp;
	strncpy(p_info->eth_ip, net_info.eth_ip, 32);
	strncpy(p_info->eth_mask, net_info.eth_mask, 32);
	strncpy(p_info->eth_gateway, net_info.eth_gateway, 32);
	strncpy(p_info->eth_dns, net_info.eth_dns, 32);
	return 0;
}

#if 0
int ui_device_eth_param_query(device_eth_param_t *p_info)
{
	net_config_para_t param;
	memset(&param, 0, sizeof(net_config_para_t));
	net_wired_get_config_para(&param);

	if (param.dhcp == 1) {
		net_wired_get_ip_para(&param);
	}

	p_info->enable_eth_dhcp = param.dhcp;
	strncpy(p_info->eth_ip, param.eth_ip, sizeof(param.eth_ip));
	strncpy(p_info->eth_mask, param.eth_mask, sizeof(param.eth_mask));
	strncpy(p_info->eth_gateway, param.eth_gateway, sizeof(param.eth_gateway));
	strncpy(p_info->eth_dns, param.eth_dns, sizeof(param.eth_dns));

	return 0;
}
#endif
/**
 * @brief  set wifi param
 * @note   NULL
 * @param  *p_info:
 * @retval 0 success other failed
 */
int ui_device_wifi_param_update(wifi_config_para_t *p_info)
{
	device_net_param_t net_info;
	if (!p_info) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}
	memset(&net_info, 0, sizeof(device_net_param_t));
	db_device_net_param_query(&net_info);
	net_info.enable_wifi = p_info->enable_wifi;
	net_info.enable_hotspot = p_info->enable_hotspot;
	memcpy(net_info.hotspot_name, p_info->hotspot_name, WIFI_NAME_LEN);
	memcpy(net_info.hotspot_password, p_info->hotspot_password, WIFI_PWD_LEN);
	db_device_net_param_update(&net_info);
	return 0;
}

/**
 * @brief  get wifi param
 * @note   NULL
 * @param  *p_info:
 * @retval 0 success other failed
 */
int ui_device_wifi_param_query(wifi_config_para_t *p_info)
{
	device_net_param_t net_info;
	if (!p_info) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}
	memset(&net_info, 0, sizeof(device_net_param_t));
	db_device_net_param_query(&net_info);
	p_info->enable_wifi = net_info.enable_wifi;
	p_info->enable_hotspot = net_info.enable_hotspot;
	memcpy(p_info->hotspot_name, net_info.hotspot_name, WIFI_NAME_LEN);
	memcpy(p_info->hotspot_password, net_info.hotspot_password, WIFI_PWD_LEN);
	return 0;
}

/**
 * @brief  get fill light param
 * @note   NULL
 * @param  *p_info:
 * @retval 0 success other failed
 */
int ui_device_get_fill_light_param(fill_light_param_t *p_info)
{
	int ret;
	device_hw_param_t info;

	if (NULL == p_info)
		return -1;

	memset(&info, 0, sizeof(device_hw_param_t));

	ret = db_device_hw_param_query(&info);
	if (ret == 0) {
		p_info->infrared_light_set = info.infrared_light_set;
		p_info->light_sensitive = info.light_sensitive;
		p_info->light_high_brightness = info.light_high_brightness;
		p_info->light_low_brightness = info.light_low_brightness;
		p_info->light_det_interval = info.light_det_interval;
		p_info->light_delay_time = info.light_delay_time;
		p_info->gpio = info.gpio;
	}

	return 0;
}

int ui_device_get_fill_rtsp_video_param(
	device_video_param_t *p_info) {
	int ret;
	device_video_param_t info;

	if (NULL == p_info)
		return -1;

	memset(&info, 0, sizeof(device_video_param_t));

	ret = db_device_video_param_query(&info);
	if (ret == 0) {
		p_info->rtsp_video_set = info.rtsp_video_set;
		p_info->video_type_set = info.video_type_set;
	}

	return 0;

}


/**
 * @brief  get screen param
 * @note   NULL
 * @param  *p_info:
 * @retval 0 success other failed
 */
int ui_device_get_screen_param(screen_bright_t *p_bright,
			screen_off_status_t *p_status)
{
	int ret;
	device_hw_param_t info;

	if ((NULL == p_bright) || (NULL == p_status))
		return -1;

	memset(&info, 0, sizeof(device_hw_param_t));

	ret = db_device_hw_param_query(&info);
	if (ret == 0) {
		p_bright->auto_display_brightness = info.auto_display_brightness;
		p_bright->display_brightness = info.display_brightness;
		p_status->auto_screen_off = info.auto_screen_off;
		p_status->auto_screen_off_timeout = info.auto_screen_off_timeout;
		p_status->screen_saver_off = info.auto_screen_saver_off;
		p_status->screen_saver_off_timeout = info.auto_screen_saver_timeout;
	}

	return 0;
}

/**
 * @brief  get access param
 * @note   NULL
 * @param  *p_info:
 * @retval 0 success other failed
 */
int ui_device_get_access_param(access_param_t *p_info)
{
	int ret;
	int delay_time;
	device_hw_param_t info;

	if (NULL == p_info)
		return -1;

	memset(&info, 0, sizeof(device_hw_param_t));

	ret = db_device_hw_param_query(&info);
	if (ret == 0) {
		p_info->wiegand_protocol = info.wiegand_protocol;
		p_info->door_sensor = info.door_sensor;
		p_info->door_sensor_delay = info.door_sensor_delay;
		p_info->alarm_switch = info.alarm_switch;
		p_info->access_type = info.access_type;
	}
	delay_time = db_delay_time_query();
	ALOGD("%s[%d] db_delay_time_query() delay_time %d",
			__func__, __LINE__, delay_time);
	if (delay_time == -1)
		delay_time = OPEN_DOOR_DELAY_TIME;
	p_info->relay_delay_time = delay_time;
	ALOGD("%s[%d] db_delay_time_query() delay_time %d",
			__func__, __LINE__, p_info->relay_delay_time);

	return 0;
}

/**
 * @brief  generate bind code by dynamic mode
 * @note   NULL
 * @param  *path:
 * @retval 0 success other failed
 */
int ui_get_qrcode_path(char *path)
{
	if (NULL == path)
		return -1;
#if ATTENDANCE_MODE > 0
	return ws_svc_app_qrcode_bind(path);
#else
	return -1;
#endif
}

/**
 * @brief  get mode
 * @note   NULL
 * @param  *mode:  0 factory mode  1 wechat mode
 * @retval 0 success  other failed
 */
int ui_get_ui_mode(int *mode)
{
	mode_ui_param_t param;
	db_mode_ui_param_query(&param);
	*mode = param.mode;

	return 0;
}

/**
 * @brief  change ui mode thread
 * @note   NULL
 * @param  arg: a pointer of thread
 * @retval NULL
 */
static void *change_ui_mode_thread(void *arg)
{
	mode_ui_param_t param;
	prctl(PR_SET_NAME, "ui_mode");

	db_mode_ui_param_query(&param);
	if (s_mode != param.mode) {
		param.mode = s_mode;
		db_mode_ui_param_update(&param);
		ALOGD("%s[%d] db_mode_ui_param_update() ok",
				__func__, __LINE__);
#if ATTENDANCE_MODE > 0
		/* stop ws svc */
		ws_svc_app_deinit();
		ALOGD("%s[%d] ws_svc_app_deinit() ok",
				__func__, __LINE__);
#endif
		/* delete db with wbchat */
		db_perm_version_update(0);
		ALOGD("%s[%d] db_perm_version_update() ok",
				__func__, __LINE__);
		db_websocket_admin_info_del();
		ALOGD("%s[%d] db_websocket_admin_info_del() ok",
				__func__, __LINE__);
		db_websocket_person_info_del(TYPE_PERSON);
		ALOGD("%s[%d] db_websocket_person_info_del() ok",
				__func__, __LINE__);
		int ret = system("/sbin/reboot");
		if (ret < 0) {
			ALOGE("%s[%d] system(/sbin/reboot) error: %s",
				__func__, __LINE__, strerror(errno));
			ret = reboot(RB_AUTOBOOT);
			if (ret)
				ALOGE("%s[%d] reboot() error %s",
					__func__, __LINE__, strerror(errno));
			return NULL;
		}

		if (WIFEXITED(ret))
			ALOGD("%s[%d] normal termination, exit status=%d",
				__func__, __LINE__,
				WEXITSTATUS(ret));
		else if (WIFSIGNALED(ret)) {
			ALOGD("%s[%d] abnormal termination, signal number=%d",
				__func__, __LINE__,
				WTERMSIG(ret));
			ret = reboot(RB_AUTOBOOT);
			if (ret)
				ALOGE("%s[%d] reboot() error %s",
					__func__, __LINE__, strerror(errno));
		} else if (WIFSTOPPED(ret)) {
			ALOGD("%s[%d] process stopped, signal number=%d",
				__func__, __LINE__,
				WSTOPSIG(ret));
			ret = reboot(RB_AUTOBOOT);
			if (ret)
				ALOGE("%s[%d] reboot() error %s",
					__func__, __LINE__, strerror(errno));
		}
	}

	return NULL;
}

/**
 * @brief  start change ui mode thread
 * @note   NULL
 * @param  mode: 0 factory mode  1 wechat mode
 * @retval none
 */
static void start_change_ui_mode(enum_mode_t mode)
{
	pthread_t tid;

	/* Start change ui mode thread */
	s_mode = mode;
	pthread_create(&tid, NULL, change_ui_mode_thread, NULL);
	pthread_detach(tid);
}

/**
 * @brief  set mode
 * @note   NULL
 * @param  mode: 0 factory mode  1 wechat mode
 * @retval 0 success  other failed
 */
int ui_set_ui_mode(enum_mode_t mode)
{
#if 1
	start_change_ui_mode(mode);
#else
	mode_ui_param_t param;
	db_mode_ui_param_query(&param);
	if (mode != param.mode) {
		param.mode = mode;
		db_mode_ui_param_update(&param);
#if ATTENDANCE_MODE > 0
		/* stop ws svc */
		ws_svc_app_deinit();
#endif
		/* delete db with wbchat */
		db_perm_version_update(0);
		db_websocket_admin_info_del();
		db_websocket_person_info_del(TYPE_PERSON);
		system("/sbin/reboot");
	}
#endif
	return 0;
}

/**
 * @brief  set rotate
 * @note   abort function
 * @param  *rotate:
 * @retval  0
 */
int ui_get_ui_rotate(int *rotate)
{
	mode_ui_param_t param;

	db_mode_ui_param_query(&param);
	*rotate = param.rotate;

	return 0;
}
/**
 * @brief  get system info
 * @note   NULL
 * @param  *p_info:
 * @retval 0 success other failed
 */
int get_sys_info(device_system_info_t *p_info)
{
	build_info_t build_info;
	device_system_info_t sys_info;

	if (NULL == p_info)
		return -1;

	memset(&sys_info, 0, sizeof(device_system_info_t));
	db_device_system_info_query(&sys_info);
	memset(&build_info, 0, sizeof(build_info_t));
	system_info_get_build_info(&build_info);

	if (strlen(sys_info.device_name) == 0)
		memcpy(sys_info.device_name,
			build_info.device_name,
			sizeof(sys_info.device_name));

	memcpy(sys_info.firmware_version,
		   build_info.system_version,
		   sizeof(sys_info.firmware_version));

	time_second_to_date_str(atof(build_info.build_date_utc),
		sys_info.face_recg_alg_version, 32);
	memcpy(p_info, &sys_info, sizeof(device_system_info_t));
	return 0;
}

/**
 * @brief  get algorithm info
 * @note   NULL
 * @param  *p_info:
 * @retval 0
 */
int ui_get_algorithm_system_info(algorithm_system_info_t *p_info)
{
	face_recognition_param_t param;

	face_get_recg_param(&param);
	ALOGE("%s: score = %f liv_check = %d mask[%d:%f]\n", __func__,
		  param.identify_threshold_score, param.nir_live_detect,
		  param.mask_enable, param.mask_threshold_value);

	p_info->recg_enable = param.recg_enable;
	p_info->infrared_image_enable = param.infrared_image_enable;
	p_info->face_threshold_value = param.identify_threshold_score;
	p_info->living_nir = param.nir_live_detect;
	p_info->living_detect_threshold_score = param.living_detect_threshold_score;
	p_info->mask_detect_enable = param.mask_detect_enable;
	p_info->mask_enable = param.mask_enable;
	p_info->mask_threshold_value = param.mask_threshold_value;
	p_info->recg_timeout = param.recg_timeout;
	p_info->repeat_recg_enable = param.repeat_recg_enable;
	p_info->recg_interval = param.recg_interval;
	p_info->recg_distance = param.recg_distance;
	p_info->voice_mode = param.voice_mode;
	p_info->grettings = param.grettings;
	p_info->stranger_voice = param.stranger_voice;
	p_info->qr_code_recg = param.qr_code_recg;
	p_info->body_temperature_detect = param.body_temperature_detect;
	p_info->temperature_compensation = param.temperature_compensation;
	p_info->alarm_temperature = param.alarm_temperature;
	p_info->exposure_values = param.exposure_values;

	return 0;
}
/**
 * @brief  set alg info
 * @note   NULL
 * @param  *p_info:
 * @retval 0
 */
int ui_set_algorithm_system_info(algorithm_system_info_t *p_info)
{
	face_recognition_param_t param;

	face_get_recg_param(&param);

	param.recg_enable = p_info->recg_enable;
	param.infrared_image_enable = p_info->infrared_image_enable;
	param.identify_threshold_score = p_info->face_threshold_value;
	param.nir_live_detect = p_info->living_nir;
	param.living_detect_threshold_score = p_info->living_detect_threshold_score;
	param.mask_detect_enable = p_info->mask_detect_enable;
	param.mask_enable = p_info->mask_enable;
	param.mask_threshold_value = p_info->mask_threshold_value;
	param.recg_timeout = p_info->recg_timeout;
	param.repeat_recg_enable = p_info->repeat_recg_enable;
	param.recg_interval = p_info->recg_interval;
	param.recg_distance = p_info->recg_distance;
	param.voice_mode = p_info->voice_mode;
	param.grettings = p_info->grettings;
	param.stranger_voice = p_info->stranger_voice;
	param.qr_code_recg = p_info->qr_code_recg;
	param.body_temperature_detect = p_info->body_temperature_detect;
	param.temperature_compensation = p_info->temperature_compensation;
	param.alarm_temperature = p_info->alarm_temperature;
	param.exposure_values = p_info->exposure_values;

	face_set_recg_param(&param, 0);

	return 0;
}
/**
 * @brief  get faces list
 * @note   NULL
 * @retval return list when success
 */
faces_t *ui_get_faces(void)
{
	return faces_get();
}

/**
 * @brief  free faces list
 * @note   NULL
 * @param  *faces:
 * @retval None
 */
void ui_put_faces(faces_t *faces)
{
	faces_put(faces);
}

/**
 * @brief  set login mode
 * @note   abort interface
 * @retval None
 */
void ui_face_set_login_mode(void)
{
	face_set_login_mode();
}

/**
 * @brief  get wechat status
 * @note   NULL
 * @retval 0 not active 1 active
 */
int ui_face_webchat_is_active(void)
{
	int ret = 0;
	websocket_client_param_t *config = NULL;

	config = calloc(sizeof(websocket_client_param_t), 1);

	if (NULL == config)
		return -1;

	ret = db_websocket_client_param_query(config);
	ALOGE("%s secret = %s\n", __func__, config->secret);

	if (0 == strcmp(config->secret, "123456")) {
		ALOGE("%s is no active\n", __func__);
		ret = 0;
	} else {
		ALOGE("%s is active\n", __func__);
		ret = 1;
	}

	if (config)
		free(config);

	return ret;
}

/**
 * @brief  get wechat sn
 * @note   NULL
 * @param  *sn:
 * @param  buf_len:
 * @retval 0 success other failed
 */
int ui_control_get_sn(char *sn, int buf_len)
{
	int ret = 0;

	/* if ((NULL == sn) || (SN_BUFFER_SIZE >= buf_len)) */
	if ((NULL == sn) || (MAX_BUF_LEN < buf_len))
		return -1;

	/* ret = system_get_private_data(SN_OFFSET, sn, SN_BUFFER_SIZE); */
	ret = system_get_private_data(SN_OFFSET, sn, buf_len);
	if (ret) {
		ALOGE("get SN failed");
		return -1;
	}
	/* sn[SN_BUFFER_SIZE] = 0; */
	ALOGD("SN:%s", sn);

	return ret;
}

/**
 * @brief  calc feat from photo
 * @note   NULL
 * @param  *feat: face feats
 * @param  sec: timeout for take photo
 * @retval 0 success other failed
 */
int ui_control_calc_feat_from_photo(ui_feat_local_t *feat, int sec)
{
	int ret = 0;
	face_info_t info;
	face_capture_param_t cap;

	memset(&info, 0, sizeof(face_info_t));
	memset(&cap, 0, sizeof(face_capture_param_t));

	create_dir(REGISTER_PIC_SAVE_PATH);
	cap.timeout_sec = sec;
#if ATTENDANCE_MODE == 2
	cap.save_frame = 0;
	snprintf(cap.save_frame_path, sizeof(cap.save_frame_path),
		"%s","");
#else
	cap.save_frame = 1;
	snprintf(cap.save_frame_path, sizeof(cap.save_frame_path),
		"%s/frame_%lld.jpg",REGISTER_PIC_SAVE_PATH,
		get_systime_us());
#endif
	/* ret = face_photo_2_feat(&info, sec); */
	ret = face_capture_2_feat(&info, &cap);
	if (ret == 0 && feat) {
		memcpy(feat->face_feature, info.feat.values, FACE_FEAT_SIZE);
		memcpy(feat->key_points, info.rect.kpts, sizeof(info.rect.kpts));
		memcpy(feat->face_norm, info.feat.norm, sizeof(info.feat.norm));
		memcpy(feat->save_frame_path, cap.save_frame_path,
			sizeof(cap.save_frame_path));
		feat->score = info.feat.score;
		ALOGD("%s sizeof(rect.kpts) %d sizeof(feat.norm) %d score %f",
			__func__, sizeof(info.rect.kpts), sizeof(info.feat.norm),
			info.feat.score);
	}

	return ret;
}
/**
 * @brief  free user queue
 * @note   NULL
 * @param  *head:  queue head
 * @retval 0 success other failed
 */
static int ui_user_free(ee_queue_t *head)
{
	if (NULL == head) {
		ALOGE("%s:head is null\n", __func__);
		return -1;
	}

	ee_queue_t *h = head, *pos, *n;
	websocket_person_info_t *p = NULL;
	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, websocket_person_info_t, queue);
		free(p);
	}

	ee_queue_init(h);

	return 0;
}

/**
 * @brief  insert new user
 * @note   NULL
 * @param  *p_info: user info
 * @retval 0 success other failed
 */
int ui_insert_user(user_information_param_t *p_info)
{
	websocket_person_info_t *person_info = NULL;
	ee_queue_t *h = NULL;
	ee_queue_t head;
	float score;
	int ret = -1;

	h = &head;

	if (NULL == p_info) {
		ALOGE("p_info is null\n");
		return ret;
	}

	ret = face_get_identify_threshold_score(&score);
	if ((0 == ret) && (p_info->score >= score)) {
		ret = -3;
		return ret;
	}

	person_info = calloc(sizeof(websocket_person_info_t), 1);
	if (person_info == NULL) {
		ret = -2;
		return ret;
	}

	strncpy(person_info->user_id, p_info->employee_number,
		sizeof(p_info->employee_number));
	strncpy(person_info->name, p_info->employee_name,
		sizeof(p_info->employee_name));
	memset(person_info->access_card_number, 0 ,
		sizeof(person_info->access_card_number));
	memcpy(person_info->face_feature, p_info->face_feat, FACE_FEAT_SIZE);
	memcpy(person_info->key_points, p_info->key_points,
		sizeof(p_info->key_points));
	memcpy(person_info->face_norm, p_info->face_norm,
		sizeof(p_info->face_norm));
	memcpy(person_info->pic_large, p_info->save_frame_path,
		sizeof(p_info->save_frame_path));
	memset(person_info->access_card_number, 0,
		sizeof(person_info->access_card_number));

	ALOGD("%s sizeof(p_info->key_points) %d sizeof(p_info->face_norm) %d",
		  __func__, sizeof(p_info->key_points), sizeof(p_info->face_norm));
	ee_queue_init(h);
	ee_queue_insert_tail(h, &(person_info->queue));
	ret = db_websocket_person_info_insert(h);
	if (ret == 0) {
		face_db_add(person_info->user_id);
		ret = 0;
	} else {
		file_remove(person_info->pic_large);
		file_remove(person_info->pic_small);
		ret = -1;
	}
	ui_user_free(h);

	return ret;
}

 /**
 * @brief  is the face module had been start
 * @note   NULL
 * @retval 0 started -1 otherwise
 */
int ui_control_server_running(void)
{
	return face_is_runnig();
}

/**
 * @brief  set display brightness
 * @note   NULL
 * @param  brightness:1-100
 * @retval 0 success other failed
 */
int ui_control_set_display_brightness(int brightness)
{
	int ret = -1;
	screen_bright_t scr_bright;
	device_hw_param_t info;

	memset(&scr_bright, 0, sizeof(screen_bright_t));
	memset(&info, 0, sizeof(device_hw_param_t));
	ret = db_device_hw_param_query(&info);
	if (ret) {
		ALOGE("%s: read device hw para failed(%d)\n", __func__, ret);
		return -1;
	}
	scr_bright.auto_display_brightness = info.auto_display_brightness;
	scr_bright.display_brightness = brightness;
	system_screen_set_bright(scr_bright);
	info.display_brightness = brightness;
	ret = db_device_hw_param_update(&info);
	if (ret) {
		ALOGE("%s: update device hw para failed(%d)\n", __func__, ret);
		return -1;
	}
	ALOGD("info.display_brightness = %d\n",info.display_brightness);
	ret = display_set_bright_to_device(info.display_brightness);
	return ret;
}

/**
 * @brief  set infrared light
 * @note   NULL
 * @param  light:0 or 1 or 2
 * @retval 0 success other failed
 */
int ui_control_set_infrared_light(char light)
{
	int ret = -1;
	device_hw_param_t info;

	memset(&info, 0, sizeof(device_hw_param_t));
	ret = db_device_hw_param_query(&info);
	if (ret) {
		ALOGE("%s: read device hw para failed(%d)\n", __func__, ret);
		return -1;
	}
	if(light == 0)
		info.infrared_light_set = light;
	else
		info.infrared_light_set = 2;
	ret = db_device_hw_param_update(&info);
	if (ret) {
		ALOGE("%s: update device hw para failed(%d)\n", __func__, ret);
		return -1;
	}
	ALOGD("info.infrared_light_set = %d\n",info.infrared_light_set);

	if (light == 2) {
		/*white_led_ctrl(FILL_LIGHT_OPEN);*/
		light_enable_auto1();
	} else if (light == 0){
		/*white_led_ctrl(FILL_LIGHT_CLOSE);*/
		light_disable_auto();
	}


	return ret;
}

/**
 * @brief  set rtsp video
 * @note   NULL
 * @param  light:0 or 1
 * @retval 0 success other failed
 */

int ui_control_set_rtsp_video(char rtsp_video) {

	int ret = -1;
	device_video_param_t info;
	memset(&info, 0, sizeof(device_video_param_t));
	ret = db_device_video_param_query(&info);
	if (ret) {
		ALOGE("%s: read device video param failed(%d)\n", __func__, ret);
		return -1;
	}
	info.rtsp_video_set = rtsp_video;
	ret = db_device_video_param_update(&info);
	if (ret) {
		ALOGE("%s: update device video para failed(%d)\n", __func__, ret);
		return -1;
	}
	ALOGD("info.rtsp_video_set = %d\n",info.rtsp_video_set);

	if (rtsp_video == 1) {
		/*TODO:8.13 RTSP_VIDEO OPEN*/
		return 0;
	}

	else if (rtsp_video == 0){
		/*TODO:8.13 RTSP_VIDEO CLOSE*/
		return 0;

	}

	return ret;

}

/**
 * @brief  set video type
 * @note   NULL
 * @param  type:0 or 1 or 2
 * @retval 0 success other failed
 */

int ui_control_set_video_type(char video_type) {
	int ret = -1;
	device_video_param_t info;
	memset(&info, 0, sizeof(device_video_param_t));
	ret = db_device_video_param_query(&info);
	if (ret) {
		ALOGE("%s: read device video type param failed(%d)\n", __func__, ret);
		return -1;
	}

	info.video_type_set = video_type;
	ret = db_device_video_param_update(&info);
	if (ret) {
		ALOGE("%s: update device video type para failed(%d)\n", __func__, ret);
		return -1;
	}
	ALOGD("info.video_type_set = %d\n",info.video_type_set);

	if (video_type == 0) {
		/*TODO:8.13 VIDEO_TYPE 720P*/
		return 0;
	}

	else if (video_type == 1){
		/*TODO:8.13 VIDEO_TYPE 480P*/
		return 0;

	}

	else {
		/*TODO:8.13 VIDEO_TYPE 480P AND 720P*/
		return 0;
		}

	return ret;
}


/**
 * @brief  set volume
 * @note   NULL
 * @param  volume:0-100
 * @retval 0 success other failed
 */
int ui_control_set_volume(int volume)
{
	int ret = -1;
	ret = system_hw_volume_set_value(volume);
	if (ret) {
		ALOGE("%s: save volume to db failed", __func__);
	}

	return ret;
}


int ui_control_get_volume(int *volume)
{
	int ret = -1;
	ret = system_hw_volume_get_value(volume);
	if (ret) {
		ALOGE("%s: save volume to db failed", __func__);
	}

	return ret;
}
/**
 * @brief  set time
 * @note   NULL
 * @param  date: time struct
 * @retval 0 success other failed
 */
int ui_control_set_time(systemtime *date)
{
	int ret = -1;
	char info[100];
	if (date == NULL) {
		ALOGE("%s: time is null", __func__);
		return -1;
	}
	sprintf(info, "%04d/%02d/%02d %02d:%02d:%02d\n", date->year,
			date->month, date->day, date->hour, date->min, date->sec);
	ret = time_set_current_time(info);

	return ret;
}

/**
 * @brief  set auto get time
 * @note   NULL
 * @param  auto_time:0 or 1
 * @retval 0 success other failed
 */
int ui_control_set_auto_time(char auto_time)
{
	int ret = -1;
	device_time_param_t info;
	if(auto_time == 1){
		ntp_start();
	}else {
		ntp_stop();
	}
	memset(&info, 0, sizeof(device_time_param_t));
	ret = db_device_time_param_query(&info);
	if (ret) {
		ALOGE("%s: db_device_time_param_query fail",__func__);
		return -1;
	}

	info.auto_set_time = auto_time;
	ret = db_device_time_param_update(&info);
	if (ret) {
		ALOGE("%s: db_device_time_param_update fail",__func__);
		return -1;
	}
	return 0;
}

/**
 * @brief  set auto get timezone
 * @note   NULL
 * @param  auto_timezone:0 or 1
 * @retval 0 success other failed
 */
int ui_control_set_auto_timezone(char auto_timezone)
{
	int ret = -1;
	device_time_param_t info;

	memset(&info, 0, sizeof(device_time_param_t));
	ret = db_device_time_param_query(&info);
	if (ret) {
		ALOGE("%s: db_device_time_param_query fail",__func__);
		return -1;
	}

	info.auto_set_timezone = auto_timezone;
	ret = db_device_time_param_update(&info);
	if (ret) {
		ALOGE("%s: db_device_time_param_update fail",__func__);
		return -1;
	}
	return 0;
}

/**
 * @brief  set auto get timezone
 * @note   NULL
 * @param  auto_timezone:0 or 1
 * @retval 0 success other failed
 */
int ui_control_get_time_param(device_time_param_t *info)
{
	int ret = -1;

	if (info == NULL) {
		ALOGE("%s: info is null",__func__);
		return -1;
	}
	memset(info, 0, sizeof(device_time_param_t));
	ret = db_device_time_param_query(info);
	if (ret) {
		ALOGE("%s: db_device_time_param_query fail",__func__);
		return -1;
	}

	return 0;
}

/**
 * @brief  statistics on memory usage
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_sta_mem(memory *memory)
{
    struct statfs diskInfo;

	statfs("/data", &diskInfo);
	unsigned long long blocksize = diskInfo.f_bsize;
	unsigned long long totalsize = blocksize * diskInfo.f_blocks;
	unsigned long long freesize = blocksize * diskInfo.f_bavail;
	unsigned long long usedsize = blocksize * (diskInfo.f_blocks - diskInfo.f_bavail);

	memory->total = totalsize >> 20;
	memory->free = freesize >> 20;
	memory->used = usedsize >> 20;

	ALOGD("memory->total=%d\n", memory->total);
	ALOGD("memory->free=%d\n", memory->free);
	ALOGD("memory->used=%d\n", memory->used);

	return 0;
}

enum_timezone_t ui_control_get_timezone()
{
	int ret = -1;
	device_time_param_t p_info;

	memset(&p_info, 0, sizeof(device_time_param_t));
	db_device_time_param_query(&p_info);

	ret = db_device_time_param_query(&p_info);
	if (ret == -1) {
		ALOGE("db_device_time_param_query error\n");
		return -1;
	}

	if ((strncmp(p_info.timezone, "Asia/Shanghai",
		strlen("Asia/Shanghai"))) == 0) {
		return Beijing;
	}
	else if ((strncmp(p_info.timezone, "Asia/Tokyo",
		strlen("Asia/Tokyo"))) == 0) {
		return Tokyo;
	}
	else if ((strncmp(p_info.timezone, "America/New_York",
		strlen("America/New_York"))) == 0) {
		return New_York;
	}
	else if ((strncmp(p_info.timezone, "Europe/London",
		strlen("Europe/London"))) == 0) {
		return London;
	}
	else if ((strncmp(p_info.timezone, "Europe/Paris",
		strlen("Europe/Paris"))) == 0) {
		return Paris;
	}
	else if ((strncmp(p_info.timezone, "Europe/Berlin",
		strlen("Europe/Berlin"))) == 0) {
		return Berlin;
	}
	return -1;
}


int ui_control_set_timezone(enum_timezone_t zone)
{
	int ret = -1;
	char str_time[256] = {0};
	long seconds = 0;
	struct timeval tv;
	time_t t;
	struct tm *tmp;

	switch (zone) {
		case Beijing:
			setenv("TZ", "Asia/Shanghai", 1);
			break;
		case Tokyo:
			setenv("TZ", "Asia/Tokyo", 1);
			break;
		case New_York:
			setenv("TZ", "America/New_York", 1);
			break;
		case London:
			setenv("TZ", "Europe/London", 1);
			break;
		case Paris:
			setenv("TZ", "Europe/Paris", 1);
			break;
		case Berlin:
			setenv("TZ", "Europe/Berlin", 1);
			break;

		default:
			break;
	}

	t = time(NULL);
	tmp = localtime(&t);
	if (NULL == tmp) {
		ALOGE("localtime error\n");
		return -1;
	}
	sprintf(str_time, "%d/%02d/%02d %02d:%02d:%02d",
		tmp->tm_year, tmp->tm_mon,
		tmp->tm_mday, tmp->tm_hour,
		tmp->tm_min, tmp->tm_sec);
	ret = time_str_to_second(str_time, "%Y/%m/%d %H:%M:%S", &seconds);
	if (0 == ret) {
		tv.tv_sec = seconds;
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);
	}
	return ret;
}

/**
 * @brief  set language
 * @note   NULL
 * @param  lang:0 or 1
 * @retval 0 success other failed
 */
int ui_control_set_lang(int lang)
{
	int ret = -1;
	device_lang_param_t info;

	memset(&info, 0, sizeof(device_lang_param_t));
	ret = db_device_lang_param_query(&info);
	if (ret) {
		ALOGE("%s: db_device_lang_param_query fail",__func__);
		return -1;
	}

	info.lang = lang;
	ret = db_device_lang_param_update(&info);
	if (ret) {
		ALOGE("%s: db_device_lang_param_update fail",__func__);
		return -1;
	}

	set_language(&info);

	return 0;
}

/**
 * @brief  get language
 * @note   NULL
 * @param  lang:0 or 1
 * @retval 0 success other failed
 */
int ui_control_get_lang(device_lang_param_t *info)
{
	int ret = -1;

	if (info == NULL) {
		ALOGE("%s: info is null",__func__);
		return -1;
	}
	memset(info, 0, sizeof(device_lang_param_t));
	ret = db_device_lang_param_query(info);
	if (ret) {
		ALOGE("%s: db_device_lang_param_query fail",__func__);
		return -1;
	}
	return 0;
}

int ui_control_get_system_info(system_param_t *sys_info)
{
	int ret;

	if (sys_info == NULL)
		return -1;

	device_hw_param_t info;
	device_lang_param_t info_lang;
	device_video_param_t info_video;

	memset(&info, 0, sizeof(device_hw_param_t));
	ret = db_device_hw_param_query(&info);
	if (ret) {
		ALOGE("%s: read device hw para failed(%d)\n", __func__, ret);
		return -1;
	}

	memset(&info_lang, 0, sizeof(device_lang_param_t));
	ret = db_device_lang_param_query(&info_lang);
	if (ret) {
		ALOGE("%s: db_device_lang_param_query fail",__func__);
		return -1;
	}

	memset(&info, 0, sizeof(device_video_param_t));
	ret = db_device_video_param_query(&info_video);
	sys_info->zone_area = ui_control_get_timezone();
	ui_control_get_volume((int *)&sys_info->volum);
	sys_info->brightness = info.display_brightness;
	sys_info->light_enable = info.auto_display_brightness;
	sys_info->language_choose = info_lang.lang;
	sys_info->rtsp_video = info_video.rtsp_video_set;
	sys_info->video_type = info_video.video_type_set;
	sys_info->light = info.infrared_light_set;

	return 0;

}

int ui_control_get_main_setting_param(main_setting_param_t *main_setting_param)
{
	db_main_setting_param_query(main_setting_param);

	return 0;
}

int ui_control_set_main_setting_param(main_setting_param_t *p_info)
{
	int ret;
	main_setting_param_t param;
	memset(&param, 0, sizeof(main_setting_param_t));

	db_main_setting_param_query(&param);

	memcpy(param.company_name, p_info->company_name, 128);
	memcpy(param.device_name, p_info->device_name, 128);
	memcpy(param.face_border_color, p_info->face_border_color, 16);
	param.name_display = p_info->name_display;
	param.name_format = p_info->name_format;
	param.mac_display = p_info->mac_display;
	param.ip_display = p_info->ip_display;
	param.register_count_display = p_info->register_count_display;

	ret = db_main_setting_param_update(&param);
	if (ret)
		ALOGE("[%s][%d] main setting param update fail!", __func__, __LINE__);

	return 0;
}

int ui_control_get_auto_reboot_param(auto_reboot_param_t *auto_reboot_param)
{
	db_auto_reboot_param_query(auto_reboot_param);

	return 0;
}

int ui_control_get_reboot_time(auto_reboot_param_t *p_info, data_time_t *p_reboot_time)
{
	int i = 0;
	int reboot[2];
	char *tmp_str = NULL;
	auto_reboot_param_t param;
	memcpy(&param, p_info, sizeof(auto_reboot_param_t));

	tmp_str = strtok(param.reboot_time, ":");
	while (tmp_str != NULL) {
		if (i == 2)
			break;
		reboot[i] = atoi(tmp_str);
		ALOGD("[%s][%d] reboot time %d", __func__, __LINE__, reboot[i]);
		++i;
		tmp_str = strtok(NULL, ":");
	}

	p_reboot_time->hour = reboot[0];
	p_reboot_time->min = reboot[1];
	if (param.reboot_interval == 0) {
		p_reboot_time->day = 1;
		p_reboot_time->week = -1;
	} else if (param.reboot_interval == 1) {
		p_reboot_time->day = -1;
		p_reboot_time->week = 1;
	} else {
		p_reboot_time->day = -1;
		p_reboot_time->week = -1;
	}

	return 0;
}

int ui_control_set_auto_reboot_param(auto_reboot_param_t *p_info)
{
	int ret;
	auto_reboot_param_t param;
	data_time_t reboot_time;
	memset(&param, 0, sizeof(auto_reboot_param_t));

	db_auto_reboot_param_query(&param);

	param.reboot_interval = p_info->reboot_interval;
	memcpy(param.reboot_time, p_info->reboot_time, 16);

	ret = db_auto_reboot_param_update(&param);
	if (ret)
		ALOGE("[%s][%d] auto reboot param update fail!", __func__, __LINE__);

	ALOGD("[%s][%d] reboot time %s", __func__, __LINE__, param.reboot_time);

	ui_control_get_reboot_time(&param, &reboot_time);
	set_reboot_time(&reboot_time);

	return 0;
}

/**
 * @brief  test light
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_light(void)
{
	return factory_test_light(TRY_TEST_TIME, TRY_TEST_COUNT);
}

/**
 * @brief  test infrared light
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_light_infrared(void)
{
	return factory_test_light_infrared(TRY_TEST_TIME, TRY_TEST_COUNT);
}

/**
 * @brief  test speaker
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_speaker(void)
{
	return factory_test_speaker();
}

/**
 * @brief  test wifi
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_wifi(void)
{
	return factory_test_wifi();
}

/**
 * @brief  test sensor of illuminance
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_sensor_illuminance(void)
{
	return factory_test_illuminance();
}

int ui_control_test_get_sensor_illuminance_value(int *value)
{
	return factory_test_get_illuminance_value(value);
}

/**
 * @brief  test rtc
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_rtc(void)
{
	return factory_test_rtc();
}


/**
 * @brief  test camera
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_camera(void)
{
	return factory_test_camera();
}


/**
 * @brief  test wired
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_eth(void)
{
	return factory_test_eth();
}


/**
 * @brief  test gmac
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_gmac(void)
{
	return 0;
}


/**
 * @brief  test bluetooth
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_bluetooth(void)
{
	return factory_test_bluetooth();
}


/**
 * @brief  test usb
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_test_usb(void)
{
	return factory_test_usb();
}

/**
 * @brief  get debug switch status
 * @note   NULL
 * @param  *debug:
 * @retval 0 success other failed
 */
int ui_control_get_debug_switch(un_debug_switch_t *debug)
{
	int ret = 0;
	if (NULL == debug) {
		ALOGE("debug is null\n");
		return -1;
	}

	ret = db_debug_switch_query();
	if (ret == -1)
		return ret;

	debug->debug_data = ret;

	return 0;
}

/**
 * @brief  set debug switch
 * @note   NULL
 * @param  *debug:
 * @retval 0 success other failed
 */
int ui_control_set_debug_switch(un_debug_switch_t *debug)
{
	if (NULL == debug) {
		ALOGE("debug is null\n");
		return -1;
	}

	return db_debug_switch_update(debug->debug_data);
}

/**
 * @brief  start person import
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_person_import(void)
{
	return person_import_thread();
}

/**
 * @brief  exit person import
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_exit_person_import(void)
{
	return excel_person_info_set_proc_exit_flag(1);
}

/**
 * @brief  get info of person import progress
 * @note   NULL
 * @param  *p_info:
 * @retval 0 success other failed
 */
int ui_control_get_peron_import_status(person_import_ctx_t *p_info)
{
	if (NULL == p_info)
		return -1;

	return person_import_get_ctx(p_info);
}

/**
 * @brief  start reg thread
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_person_recg()
{
	return person_reg_check_thread();
}

/**
 * @brief  exit reg thread
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_exit_person_recg(void)
{
	return excel_reg_check_set_proc_exit_flag(1);
}

/**
 * @brief  get info of recg progress
 * @note   NULL
 * @param  *p_info:
 * @retval 0 success other failed
 */
int ui_control_get_person_recg_status(person_reg_check_ctx_t *p_info)
{
	if (NULL == p_info)
		return -1;

	return person_reg_check_get_ctx(p_info);
}

/**
 * @brief  get isp data
 * @note   NULL
 * @param  *exif_rgb: rgb isp
 * @param  *exif_nir: nir isp
 * @retval 0 success other failed
 */
int ui_control_get_isp_data(struct viss_isp_exif *exif_rgb,
							struct viss_isp_exif *exif_nir)
{
	if (NULL == exif_rgb || NULL == exif_nir) {
		ALOGE("exif is null\n");
		return -1;
	}

	get_rgb_sensor_exif(0, exif_rgb);
	get_nir_sensor_exif(0, exif_nir);

	return 0;
}

/**
 * @brief  start record
 * @note   NULL
 * @retval 0
 */
int ui_control_record_start(void)
{
	face_record_try();

	return 0;
}

/**
 * @brief  stop record
 * @note   NULL
 * @retval 0
 */
int ui_control_record_stop(void)
{
	face_record_stop_try();

	return 0;
}

extern void mouse_wakeup();
/**
 * @brief  wakeup screen
 * @note   NULL
 * @retval None
 */
void ui_control_wakeup_screen(void)
{
	screen_count_reset();
	screen_saver_count_reset();
	mouse_wakeup();
}

/**
 * @brief  pause face server
 * @note   NULL
 * @retval 0
 */
int ui_control_face_pause(void)
{
	if (face_pause_status() == 0)
		return face_pause();

	return 0;
}

/**
 * @brief  resume face server
 * @note   NULL
 * @retval 0
 */
int ui_control_face_resume(void)
{
	if (face_pause_status())
		return face_resume();

	return 0;
}

/**
 * @brief  start nir camera preview
 * @note
 * @param  x: top
 * @param  y: left
 * @param  w: width
 * @param  h: height
 * @param  rotate: 0 0;1 90;2 180;3 270;
 * @retval 0 success other failed
 */
int ui_control_camera_nir_start_preview(
	int x, int y, int w, int h, int rotate)
{
	return face_svc_start_preview(FACE_CAMERA_NIR, x, y, w, h, rotate, 1);
}

/**
 * @brief  stop nir camera preview
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_camera_nir_stop_preview(void)
{
	return face_svc_stop_preview(FACE_CAMERA_NIR);
}
/**
 * @brief  play sound when add person
 * @note   NULL
 * @retval 0 success other failed
 */
int ui_control_add_person_sound(void)
{
	return face_capture_ok_sound_play();
}

int ui_control_websocket_is_link(void)
{
#if ATTENDANCE_MODE > 0
	return ws_svc_app_ws_is_link();
#else
	return 0;
#endif
}

int ui_control_snapshot(int cam_id, char *file_name)
{
	return face_snapshot(cam_id, file_name);
}

int ui_control_get_ord_map_params(ui_map_t *ord_map_params)
{
	int ret;
	map_params_t ord_params;
	if (ord_map_params == NULL)
		return -1;

	memset(&ord_params, 0, sizeof(map_params_t));
	ret = face_get_ord_map_params(&ord_params);
	if (ret == 0)
		memcpy(ord_map_params, &ord_params, sizeof(map_params_t));

	return ret;
}

int ui_control_get_mod_map_params(ui_map_t *ord_map_params,
		ui_map_t *mod_map_params)
{
	int ret;
	map_params_t ord_params, mod_params;

	if ((ord_map_params == NULL) || (mod_map_params == NULL))
		return -1;

	memset(&ord_params, 0, sizeof(map_params_t));
	memset(&mod_params, 0, sizeof(map_params_t));
	memcpy(&ord_params, ord_map_params, sizeof(map_params_t));
	ret = face_get_mod_map_params(&ord_params, &mod_params);
	if (ret == 0)
		memcpy(mod_map_params, &mod_params, sizeof(map_params_t));

	return ret;
}

int ui_control_save_mod_map_param(ui_map_t *mod_map_params)
{
	int i = 0;
	ui_map_params_t ui_map_params;

	memset(&ui_map_params, 0, sizeof(ui_map_params_t));
	ui_map_params.check = RGBNIR_MAP_CHECK;
	for (i = 0; i < CALIBRATION_NUM; i++) {
		ui_map_params.x_map[i] = mod_map_params->x_map[i];
		ALOGV("x_map %f", ui_map_params.x_map[i]);
	}
	for (i = 0; i < CALIBRATION_NUM; i++) {
		ui_map_params.y_map[i] = mod_map_params->y_map[i];
		ALOGV("y_map %f", ui_map_params.y_map[i]);
	}

	return system_set_private_data(RGBNIR_MAP_OFFSET,
			 (char *)&ui_map_params, sizeof(ui_map_params_t));
}

int ui_control_take_mod_pic(int x, int y, int w, int h)
{
	return face_take_mod_pic(x, y, w, h);
}

int ui_control_cal_mod_map(map_params_t *map)
{
	return face_cal_mod_map(map);
}

/**
 * @brief  get ws link status
 * @note   NULL
 * @retval 0 not link 1 linked
 */
int ui_control_ws_is_link(void)
{
#if ATTENDANCE_MODE > 0
	return ws_svc_app_wechat_is_link();
#else
	return 0;
#endif
}

/**
 * @brief  wifi is connected
 * @note   NULL
 * @retval 0 not link  1 linked
 */
int ui_control_wifi_is_link(void)
{
	return bt_wifi_is_link();
}

/**
 * @brief  get current mode
 * @note   NULL
 * @param  *mode:
 * @retval 0 success other failed
 */
int ui_control_get_current_mode(mode_ui_param_t *mode)
{
	if (NULL == mode)
		return -1;

	return db_mode_ui_param_query(mode);
}

/**
 * @brief  query person info
 * @note   NULL
 * @param  *head:
 * @param  type:
 * @retval 0 success other failed
 */
int ui_control_person_info_query_begin(ee_queue_t *head, int type)
{
	return db_websocket_person_info_query_begin(head, type);
}

/**
 * @brief  free person queue
 * @note   NULL
 * @param  *head:
 * @param  type:
 * @retval 0 success other failed
 */
int ui_control_person_info_query_end(ee_queue_t *head, int type)
{
	return db_websocket_person_info_query_end(head, type);
}

/**
 * @brief  get count of person info
 * @note   NULL
 * @param  NULL
 * @retval count of person info
 */
int ui_control_person_info_get_count(void)
{
	return db_ui_person_info_get_count(TYPE_PERSON);
}

/**
 * @brief  get person info by limit
 * @note   NULL
 * @param  *head:
 * @param  index: record index
 * @param  count: record count
 * @retval 0 success other failed
 */
int ui_control_person_info_get_begin(ee_queue_t *head, int index, int count)
{
	return db_ui_person_info_query_begin(head, index, count);
}

/**
 * @brief  get person info by limit
 * @note   NULL
 * @param  *head:
 * @param  index: record index
 * @param  count: record count
 * @retval 0 success other failed
 */
int ui_control_local_person_get_begin(ee_queue_t *head, int index, int count)
{
	return db_ui_local_person_query_begin(head, index, count);
}

int ui_control_local_person_get_end(ee_queue_t *head)
{
	return db_ui_local_person_query_end(head);
}

/**
 * @brief  free person queue
 * @note   NULL
 * @param  *head:
 * @param  type:
 * @retval 0 success other failed
 */
int ui_control_person_info_get_end(ee_queue_t *head)
{
	return db_ui_person_info_query_end(head);
}

/**
 * @brief  del person info by userid
 * @note   NULL
 * @param  *user_id:
 * @retval 0 success other failed
 */
int ui_control_person_info_del_by_user_id(char *user_id)
{
	int ret = db_websocket_person_info_del_by_user_id(user_id);
	ALOGD("db_websocket_person_info_del_by_user_id(%s) ret %d", user_id, ret);
#if 0
	if (ret == 1)
		face_db_update();
#endif
	return ret;
}

/**
 * @brief  clean peron table
 * @note   NULL
 * @param  type:
 * @retval 0 success other failed
 */
int ui_control_person_info_del(int type)
{
	int ret = db_websocket_person_info_del(type);
	ALOGD("db_websocket_person_info_del(%d) ret %d", type, ret);
	if (ret > 0)
		face_db_update();

	return ret;
}

/**
 * @brief  get delay time of relay open door
 * @note   NULL
 * @retval delay time
 */
int ui_control_get_delay_time(void)
{
	int delay_time = db_delay_time_query();
	ALOGD("%s[%d] db_delay_time_query() delay_time %d",
			__func__, __LINE__, delay_time);
	if (delay_time == -1)
		delay_time = OPEN_DOOR_DELAY_TIME;

	return delay_time;
}

/**
 * @brief  set delay time of relay open door
 * @delay_time: delay time
 *
 * @retval 0 success other failed
 */
int ui_control_set_delay_time(int delay_time)
{
	int ret;

	ret = db_delay_time_update(delay_time);
	if (!ret)
		ret = set_open_door_delay_time(delay_time);

	return ret;
}

/**
 * @brief  get wiegand protocol
 * @note   NULL
 * @retval wiegand protocol, 26 or 34
 */
int ui_control_get_wiegand_protocol(void)
{
	int data_size = db_wiegand_protocol_query();
	ALOGD("%s[%d] data_size %d", __func__, __LINE__, data_size);
	if (data_size == -1)
		data_size = WIEGAND_DATA_SIZE;

	return data_size;
}

/**
 * @brief  set wiegand protocol
 * @data_size: wiegand data size
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_wiegand_protocol(int data_size)
{
	int ret;

	ret = db_wiegand_protocol_update(data_size);
	if (!ret)
		ret = wiegand_write_set_data_size(data_size);

	return ret;
}

/**
 * @brief  set door sensor type
 * @wiegand_protocol: door sensor type
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_door_sensor_type(int door_sensor_type)
{
	int ret;

	ret = db_door_sensor_type_update(door_sensor_type);
	if (!ret)
		ret = set_door_sensor_type(door_sensor_type);

	return ret;
}

/**
 * @brief  set door sensor delay
 * @wiegand_protocol: door sensor delay time
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_door_sensor_delay(int delay_time)
{
	int ret;

	ret = db_door_sensor_delay_update(delay_time);
	if (!ret)
		ret = set_alarm_delay_time(delay_time);

	return ret;
}

/**
 * @brief  set alarm switch
 * @alarm_switch: alarm switch
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_alarm_switch(int alarm_switch)
{
	int ret;

	ret = db_alarm_switch_update(alarm_switch);
	if (!ret)
		ret = set_alarm_switch(alarm_switch);

	return ret;
}

/**
 * @brief  get access type
 * @access_time: access type
 *
 * Returns  access type.
 */
int ui_control_get_access_type(void)
{
	return db_access_type_query();
}

/**
 * @brief  set access_type
 * @access_type: access type
 * Returns  0 success, -1: failure.
 */
int ui_control_set_access_type(int access_type)
{
	int ret = -1;
	device_hw_param_t info;

	memset(&info, 0, sizeof(device_hw_param_t));
	ret = db_device_hw_param_query(&info);
	if (ret) {
		ALOGE("%s: read device hw para failed(%d)\n", __func__, ret);
		return -1;
	}
	info.access_type = access_type;
	ret = db_device_hw_param_update(&info);
	if (ret) {
		ALOGE("%s: update device hw para failed(%d)\n", __func__, ret);
		return -1;
	}
	ALOGD("[%s %d] set access_type = %d\n", __func__, __LINE__, info.access_type);
	set_access_type(access_type);

	return 0;
}

/**
 * @brief  get access time
 * @access_time: access time
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_get_access_time(char *access_time)
{
	if (NULL == access_time)
		return -1;

	return db_access_time_query(access_time);
}

/**
 * @brief  set access time
 * @access_time: access time
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_access_time(char *access_time)
{
	int ret;

	ret = db_access_time_update(access_time);
	if (!ret)
		ret = set_access_time(access_time);

	return ret;
}

/**
 * ui_control_get_development_status - get development status.
 *
 * Returns development status.
 */
int ui_control_get_development_status(void)
{
	int development = db_ui_get_development_status();
	ALOGD("%s[%d] db_ui_get_development_status() ret %d",
			__func__, __LINE__, development);

	return development;
}

/**
 * @brief  get checkin info
 * @p_info: checkin info
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_get_checkin_info(checkin_param_t *p_info)
{
	if (NULL == p_info)
		return -1;

	return ui_device_get_checkin_param(p_info);
}

/**
 * @brief  set checkin info
 * @p_info: checkin info
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_checkin_info(checkin_param_t *p_info)
{
	int ret;

	ret = ui_device_set_checkin_param(p_info);
	if (ret) {
		ALOGE("%s: update checkin para failed(%d)\n", __func__, ret);
		return -1;
	}
	set_gate_dump_img_config();

	return ret;
}

/**
 * ui_control_get_password - get password of enter main menu.
 * @password: A pointer of password.
 * @pwd_len: length of password buffer
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_get_password(char *password, int pwd_len)
{
	return db_ui_get_password(password, pwd_len);
}

/**
 * ui_control_set_password - set password of enter main menu.
 * @password: password of enter main menu.
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_set_password(const char *password)
{
	return db_ui_set_password(password);
}

/**
 * get_md5_string - get the md5 of src.
 * @src: source chars.
 * @dst: dst chars.
 * @len: the size of dst.
 *
 * Returns 0 if success, otherwise -1.
 */
int get_md5_string(const char *src, char *dst, int len) {
  #define MD5_BUFFER_LEN 16
  #define MD5_BUFFER_LIMIT 32
  int i, len1;
  struct MD5Context md5;
  unsigned char decrypt[MD5_BUFFER_LEN];

  if ((NULL == src) || (NULL == dst) || (len < MD5_BUFFER_LIMIT)) {
    return -1;
  }

  MD5Init(&md5);
  MD5Update(&md5, (unsigned char *)src, strlen(src));
  MD5Final(decrypt, &md5);

  len1 = 0;
  for (i = 0; i < MD5_BUFFER_LEN; i++) {
    len1 += snprintf(dst+len1, len, "%02X", decrypt[i]);
  }

  return 0;
}

/**
 * ui_control_export_rec_get_count - get total count of export record.
 * @password: password of enter main menu.
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_export_rec_get_count(void)
{
	return db_ui_export_rec_get_count();
}

int ui_control_export_by_time_get_count(char *user_id,
	long start, long end)
{
	return db_ui_export_by_time_get_count(user_id, start, end);
}

/**
 * ui_control_export_rec_query_begin - query export record.
 * @index: index of total record.
 * @count: count of current record.
 * @head: a pointer of query result queue
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_export_rec_query_begin(int index,
	int count, ee_queue_t *head)
{
	return db_ui_export_rec_query_begin(index, count, head);
}

int ui_control_export_rec_query_begin_by_id_time(
	int index, int count, ee_queue_t *head, char *user_id,
	long start, long end)
{
	ALOGD("%s:%d >> %d, %d, user_id=%s, %ld, %ld",__func__, __LINE__,
		index, count, user_id, start, end);
	return db_ui_export_rec_query_by_id_time_begin(
		index, count, head, user_id, start, end);
}

/**
 * ui_control_export_rec_query_end - free query result.
 * @head: a pointer of query result queue.
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_export_rec_query_end(ee_queue_t *head)
{
	return db_ui_export_rec_query_end(head);
}

/**
 * ui_control_get_export_result - get export result.
 * @p_ctx: a pointer of export result.
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_get_export_result(record_export_ctx_t *p_ctx)
{
	return record_export_get_ctx(p_ctx);
}

/*
* ui_control_relay_ctrl - set relay status
* @state: relay status
*
* return 0 if success, otherwise return -1
*/
int ui_control_relay_ctrl(RELAY_STATE state)
{
	return relay_ctrl(state);
}

/**
 * ui_control_wg_write - Write data to device
 * @data_size: IN, wiegand data size
 * @cardid: IN, cardid
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_wg_write(int data_size, uint64_t cardid)
{
	return wg_write(data_size, cardid);
}

/**
 * ui_control_rs_open - open serial port
 * @dev_name: device name
 * @open_mode: open mode(1/2/3:read only/write only/read and write)
 * @speed: boud rate
 * @bits: data bits
 * @even: even/odd bit
 * @stop: stop bit
 * @_trans_data_notify: a pointer of callback
 *
 * Returns object of device.
 */
void *ui_control_rs_open(const char *dev_name, int open_mode,
				int speed, int bits, char even, int stop,
				cb_trans_data_notify _trans_data_notify,
				void *context)
{
	return rs_open(dev_name, open_mode,
				speed, bits, even, stop,
				_trans_data_notify, context);
}

/**
 * ui_control_rs_close - close serial port
 * @handle: object of device
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_rs_close(void *handle)
{
	return rs_close(handle);
}

/**
 * ui_control_rs_send_data: send data to serial port
 * @handle: object of device
 * @data: data of send
 * &data_len: length of data
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_rs_send_data(void *handle, const char *data, int data_len)
{
	return rs_send_data(handle, data, data_len);
}

