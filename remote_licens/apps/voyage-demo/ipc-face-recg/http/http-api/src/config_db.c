/*
 * api/config_db.c - all functions for RESTful.
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
#include <sys/time.h>

#include <unistd.h>
#include <sys/types.h>

#include "cJSON.h"
#include "config_db.h"

#include "record_auto_delete.h"
#include "time_conv.h"

#include "recognition.h"
#include "person.h"
#include "common_record_info.h"
#include "db_cloud_platform.h"
#include "db_manager_info.h"
#include "jpeg_dec.h"
#include "system_volume.h"
#include "system_screen.h"
#include "light.h"
#include "net_wired.h"
#include "system_info.h"
#include "db_device_hw_param.h"
#include "system_develop_sel.h"
#include "db_device_status_info.h"
#include "ip_config.h"
#include "net_wifi.h"
#include "sqlite3.h"
#include "restful_api.h"
#include "base64.h"
#include "sys_http.h"
#include "app_lang.h"
#include "db_config.h"

#define LOG_TAG "config_db.c"
#include <log/log.h>

/**
 * set_recg_cfg - Update face recognition's configuration parameter
 * @p_info: a pointer of save recognition parameters
 *
 * @return 0-successful, other-failed
 */
int set_recg_cfg(recognition_para_config *p_info)
{
	int ret = -1;
	recognition_para_t p_recg;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return -1;
	}
	if (p_info->threshold_score < 0 || 
		p_info->threshold_score > 1 ||
		(p_info->live_detect != 0 && p_info->live_detect != 1)) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return -1;
	}

	/** set recognition para */
	p_recg.threshold_score = p_info->threshold_score;
	p_recg.live_detect = p_info->live_detect;

	ret = recognition_set_param(&p_recg);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return -2;
	}

	return 0;
}

/**
 * get_recg_cfg - Query face recognition's configuration parameter
 * @p_info: a pointer of save recognition parameters
 *
 * @return 0-successful, other-failed
 */
int get_recg_cfg(recognition_para_config *p_info)
{
	recognition_para_t p_recg;
	int ret;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return -1;
	}

	/** get recognition para */
	ret = recognition_get_param(&p_recg);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return -1;
	}

	p_info->threshold_score = p_recg.threshold_score;
	p_info->live_detect = p_recg.live_detect;

	return 0;
}

/**
 * get_device_hwpara - Query device hardware information
 * @p_info: a pointer of device_hw_param_config
 *
 * @return 0-successful, other-failed
 */
int get_device_hwpara(device_hw_param_config *p_info)
{
	int ret = -1;
	device_hw_param_t hw_para;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get recognition para */
	ret = db_device_hw_param_query(&hw_para);
	if (!ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	p_info->volume = hw_para.volume;
	p_info->auto_display_brightness = hw_para.auto_display_brightness;
	p_info->display_brightness = hw_para.display_brightness;
	p_info->infrared_light_set = hw_para.infrared_light_set;
	p_info->light_sensitive = hw_para.light_sensitive;
	p_info->human_induction = hw_para.human_induction;

	return 0;
}

/**
 * set_device_hwpara - Update device hardware information
 * @p_info: a pointer of device_hw_param_config
 *
 * @return 0-successful, other-failed
 */
int set_device_hwpara(device_hw_param_config *p_info)
{
	int ret = -1;
	device_hw_param_t hw_para;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get old para */
	ret = db_device_hw_param_query(&hw_para);
	if (!ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** set recognition para */
	hw_para.volume = p_info->volume;
	hw_para.auto_display_brightness = p_info->auto_display_brightness;
	hw_para.display_brightness = p_info->display_brightness;
	hw_para.infrared_light_set = p_info->infrared_light_set;
	hw_para.light_sensitive = p_info->light_sensitive;
	hw_para.human_induction = p_info->human_induction;

	ret = db_device_hw_param_update(&hw_para);
	if (ret != SQLITE_OK) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * get_device_hwpara_volume - Query device hardware volume
 * @p_info: a pointer of saving volume value
 *
 * @return 0-successful, other-failed
 */
int get_device_hwpara_volume(int *volume)
{
	int ret = -1;
	ret = system_volume_get_value(volume);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * set_device_hwpara_volume - Update device hardware volume
 * @p_info: the current volume value
 *
 * @return 0-successful, other-failed
 */
int set_device_hwpara_volume(int volume)
{
	int ret = -1;

	ret = system_volume_set_value(volume);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * get_device_hwpara_screeninfo - Query device hardware screeninfo
 * @p_info: a pointer of saving volume value
 *
 * @return 0-successful, other-failed
 */
int get_device_hwpara_screeninfo(device_hw_param_config_screeninfo *p_info)
{
	int ret = -1;
	screen_bright_t st_bright;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	ret = system_screen_get_bright(&st_bright);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	p_info->auto_display_brightness = st_bright.auto_display_brightness;
	p_info->display_brightness = st_bright.display_brightness;

	return 0;
}

/**
 * set_device_hwpara_screeninfo - Update device hardware screeninfo
 * @p_info: the current volume value
 *
 * @return 0-successful, other-failed
 */
int set_device_hwpara_screeninfo(device_hw_param_config_screeninfo *p_info)
{
	int ret = -1;
	screen_bright_t st_bright;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** set device hw para */
	st_bright.auto_display_brightness = p_info->auto_display_brightness;
	st_bright.display_brightness = p_info->display_brightness;

	ret = system_screen_set_bright(st_bright);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * get_light_info - Query device hardware light info
 * @p_info: a pointer of saving volume value
 *
 * @return 0-successful, other-failed
 */
int get_light_info(device_hw_param_config_light_info *p_info)
{
	int ret = -1;
	int light_type;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get light_type */
	ret = system_filling_light_get_para(&light_type);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	switch (light_type) {
	case 2:
		p_info->auto_adapt_enable = 1;
		p_info->switch_enable = 0;
		break;
	case 1:
		p_info->auto_adapt_enable = 0;
		p_info->switch_enable = 1;
		break;
	case 0:
		p_info->auto_adapt_enable = 0;
		p_info->switch_enable = 0;
		break;
	default:
		p_info->auto_adapt_enable = 0;
		p_info->switch_enable = 0;
		break;
	}

	return 0;
}

/**
 * set_light_info - Update device hardware light info
 * @p_info: a pointer of saving volume value
 *
 * @return 0-successful, other-failed
 */
int set_light_info(device_hw_param_config_light_info *p_info)
{
	int ret = -1;
	int light_type;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	ret = 0;
	if (p_info->auto_adapt_enable) {
		ret |= system_filling_light_set_para(FILL_LIGHT_CLOSE);
		light_type = 2;
	} else {
		ret |= system_filling_light_set_para(FILL_LIGHT_AUTO_DIS);
		if (p_info->switch_enable)
			light_type = 1;
		else
			light_type = 0;
	}
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	ret = system_filling_light_set_para(light_type);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * get_cloud_platform_info - Query cloud platform info
 * @p_info: a pointer of saving cloud platform info
 *
 * @return 0-successful, other-failed
 */
int get_cloud_platform_info(cloud_platform_config_info *p_info)
{
	int ret = -1;
	char cloud_upload_rec_url[256];
	char cloud_import_mems_url[256];
	char cloud_import_delete_all_url[256];
	char cloud_ipaddr_changed_url[256];
	char cloud_import_mems_ret_url[256];
	cloud_platform_t cldparam;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get cloud_platform_info */

	memset(cloud_upload_rec_url, 0, sizeof(cloud_upload_rec_url));
	memset(cloud_import_mems_url, 0, sizeof(cloud_import_mems_url));
	memset(cloud_import_delete_all_url, 0,
			sizeof(cloud_import_delete_all_url));
	memset(cloud_ipaddr_changed_url, 0,
			sizeof(cloud_ipaddr_changed_url));
#if 0
	get_ini_value("cloud_upload_rec_url", cloud_upload_rec_url,
			sizeof(cloud_upload_rec_url));
	get_ini_value("cloud_import_mems_url", cloud_import_mems_url,
			sizeof(cloud_import_mems_url));
	get_ini_value("cloud_import_delete_all_url",
			cloud_import_delete_all_url,
			sizeof(cloud_import_delete_all_url));
	get_ini_value("cloud_ipaddr_changed_url",
			cloud_ipaddr_changed_url,
			sizeof(cloud_ipaddr_changed_url));
	get_ini_value("cloud_import_mems_ret_url",
			cloud_import_mems_ret_url,
			sizeof(cloud_import_mems_ret_url));
#else
	db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_UPLOAD_RET_URL,
			cloud_upload_rec_url,
			sizeof(cloud_upload_rec_url),
			DBCONF_CLOUDPLATFORM_UPLOAD_RET_URL_VALUE);
	db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_URL,
			cloud_import_mems_url,
			sizeof(cloud_import_mems_url),
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_URL_VALUE);
	db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_DEL_ALL_URL,
			cloud_import_delete_all_url,
			sizeof(cloud_import_delete_all_url),
			DBCONF_CLOUDPLATFORM_IMPORT_DEL_ALL_URL_VALUE);
	db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_CHANGE_URL,
			cloud_ipaddr_changed_url,
			sizeof(cloud_ipaddr_changed_url),
			DBCONF_CLOUDPLATFORM_CHANGE_URL_VALUE);
	db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_RET_URL,
			cloud_import_mems_ret_url,
			sizeof(cloud_import_mems_ret_url),
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_RET_URL_VALUE);
#endif
	ret = db_cloud_platform_query(&cldparam);
	if (!ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	strcpy(p_info->ai_server_url, cldparam.ai_server_url);
	strcpy(p_info->attendance_url, cldparam.attendance_url);
	strcpy(p_info->fota_url, cldparam.fota_url);
	strcpy(p_info->cloud_upload_rec_url, cloud_upload_rec_url);
	strcpy(p_info->cloud_import_mems_url, cloud_import_mems_url);
	strcpy(p_info->cloud_import_delete_all_url,
		cloud_import_delete_all_url);
	strcpy(p_info->cloud_ipaddr_changed_url,
		cloud_ipaddr_changed_url);
	strcpy(p_info->cloud_import_mems_ret_url,
		cloud_import_mems_ret_url);

	return 0;
}

/**
 * set_cloud_platform_info - Update cloud platform info
 * @p_info: a pointer of saving cloud platform info
 *
 * @return 0-successful, other-failed
 */
int set_cloud_platform_info(cloud_platform_config_info *p_info)
{
	int ret = -1;
	cloud_platform_t cldparam;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** set cloud_platform_info */
	/** first get old cloud_platform_info */
	ret = db_cloud_platform_query(&cldparam);
	if (!ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** then set new cloud_platform_info */
	ret = 0;
#if 0
	if (strlen(p_info->cloud_upload_rec_url) > 0)
		ret |= set_ini_value("cloud_upload_rec_url",
			p_info->cloud_upload_rec_url);
	if (strlen(p_info->cloud_import_mems_url) > 0)
		ret |= set_ini_value("cloud_import_mems_url",
			p_info->cloud_import_mems_url);
	if (strlen(p_info->cloud_import_delete_all_url) > 0)
		ret |= set_ini_value("cloud_import_delete_all_url",
			p_info->cloud_import_delete_all_url);
	if (strlen(p_info->cloud_ipaddr_changed_url) > 0)
		ret |= set_ini_value("cloud_ipaddr_changed_url",
			p_info->cloud_ipaddr_changed_url);
	if (strlen(p_info->cloud_import_mems_ret_url) > 0)
		ret |= set_ini_value("cloud_import_mems_ret_url",
			p_info->cloud_import_mems_ret_url);
#else
	if (strlen(p_info->cloud_upload_rec_url) > 0)
		ret |= db_config_set_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_UPLOAD_RET_URL,
			p_info->cloud_upload_rec_url);
	if (strlen(p_info->cloud_import_mems_url) > 0)
		ret |= db_config_set_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_URL,
			p_info->cloud_import_mems_url);
	if (strlen(p_info->cloud_import_delete_all_url) > 0)
		ret |= db_config_set_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_DEL_ALL_URL,
			p_info->cloud_import_delete_all_url);
	if (strlen(p_info->cloud_ipaddr_changed_url) > 0)
		ret |= db_config_set_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_CHANGE_URL,
			p_info->cloud_ipaddr_changed_url);
	if (strlen(p_info->cloud_import_mems_ret_url) > 0)
		ret |= db_config_set_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_RET_URL,
			p_info->cloud_import_mems_ret_url);
#endif
	if (strlen(p_info->ai_server_url) > 0)
		strcpy(cldparam.ai_server_url, p_info->ai_server_url);
	if (strlen(p_info->attendance_url) > 0)
		strcpy(cldparam.attendance_url, p_info->attendance_url);
	if (strlen(p_info->fota_url) > 0)
		strcpy(cldparam.fota_url, p_info->fota_url);
	ret = db_cloud_platform_update(&cldparam);
	if (ret != SQLITE_OK) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * get_dev_time - Query device time
 * @p_info: a pointer of saving device time info
 *
 * @return 0-successful, other-failed
 */
int get_dev_time(dev_time_config_info *p_info)
{
	int ret = -1;
	char curtime[40] = {0};
	struct timeval tv;
	struct timezone tz;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get dev_time_info */
	/** get curtime */
	ret = time_get_cur_date_time(curtime);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	/** get time zone */
	gettimeofday(&tv, &tz);

	strcpy(p_info->curtime, curtime);
	p_info->timezone = tz.tz_dsttime;

	return 0;
}

/**
 * set_dev_time - Update device time
 * @p_info: a pointer of saving device time info
 *
 * @return 0-successful, other-failed
 */
int set_dev_time(dev_time_config_info *p_info)
{
	int ret = -1;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	ALOGD("[%s : %d : %s]\n", __func__, __LINE__, p_info->curtime);

	/** set dev_time_info */
	ret = time_set_current_time(p_info->curtime);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	ALOGD("time_set_current_time [%d]\n", __LINE__);
	/** set time zone */

	return 0;
}

/**
 * get_dev_time - Query develop select
 * @p_info: a pointer of develop select info
 *
 * @return 0-successful, other-failed
 */
int get_develop_select(develop_sel_para_config_info *p_info)
{
	int ret = -1;
	develop_sel_para_t param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** set dev_time_info */
	ret = system_develop_sel_get_para(&param);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	p_info->debug_sw = param.debug_sw;

	return 0;
}

/**
 * set_dev_time - Update develop select
 * @p_info: a pointer of develop select info
 *
 * @return 0-successful, other-failed
 */
int set_develop_select(develop_sel_para_config_info *p_info)
{
	int ret = -1;
	develop_sel_para_t param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** set dev_time_info */
	param.debug_sw = p_info->debug_sw;
	ret = system_develop_sel_set_para(&param);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * get_device_status - Query device status
 * @p_info: a pointer of device status
 *
 * @return 0-successful, other-failed
 */
int get_device_status(device_status_config_info *p_info)
{
	int ret = -1;
	device_status_param_t param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get device_status */
	ret = db_device_status_param_query(&param);
	if (ret < 0) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	strcpy(p_info->company_name, param.company_name);
	strcpy(p_info->company_logo, param.company_logo);
	strcpy(p_info->device_address, param.device_address);
	p_info->language = param.language;
	p_info->punch_mode = param.punch_mode;
	p_info->debug_switch = param.debug_switch;

	return 0;
}

/**
 * set_device_status - Update device status
 * @p_info: a pointer of device status
 *
 * @return 0-successful, other-failed
 */
int set_device_status(device_status_config_info *p_info)
{
	int ret = -1;
	device_status_param_t param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	if (p_info->debug_switch != 0 &&
		p_info->debug_switch != 1) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	if (p_info->punch_mode != 0 &&
		p_info->punch_mode != 1 &&
		p_info->punch_mode != 2 &&
		p_info->punch_mode != 3 &&
		p_info->punch_mode != 4) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	if (p_info->language != 0 &&
		p_info->language != 1) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** set device_status */
	memset(&param, 0, sizeof(device_status_param_t));
	strcpy(param.company_name, p_info->company_name);
	strcpy(param.company_logo, p_info->company_logo);
	strcpy(param.device_address, p_info->device_address);
	param.language = p_info->language;
	param.punch_mode = p_info->punch_mode;
	param.debug_switch = p_info->debug_switch;
	ret = db_device_status_param_update(&param);
	if (ret != SQLITE_OK) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	lang_deinit();
	lang_init();

	return 0;
}

/**
 * get_eth_param - Query eth param
 * @p_info: a pointer of eth param
 *
 * @return 0-successful, other-failed
 */
int get_eth_param(net_para_config_info *p_info)
{
	int ret = -1;
	net_config_para_t param;
	ip_static_config_t eth0_param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get device_status */
	ret = net_wired_get_config_para(&param);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	p_info->enable = param.enable;
	p_info->priority = param.priority;
	p_info->dhcp = param.dhcp;
	strcpy(p_info->eth_dns, param.eth_dns);
	if (param.dhcp) {
		net_dev_get_static_ip("eth0", &eth0_param);
		strcpy(p_info->eth_ip, (const char *)(eth0_param.ip_addr));
		strcpy(p_info->eth_mask, (const char *)(eth0_param.netmask));
		strcpy(p_info->eth_gateway, (const char *)(eth0_param.gateway));
	} else {
		strcpy(p_info->eth_ip, param.eth_ip);
		strcpy(p_info->eth_mask, param.eth_mask);
		strcpy(p_info->eth_gateway, param.eth_gateway);
	}
	return 0;
}

/**
 * set_eth_param - Update eth param
 * @p_info: a pointer of eth param
 *
 * @return 0-successful, other-failed
 */
int set_eth_param(net_para_config_info *p_info)
{
	int ret = -1;
	net_config_para_t param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	if (p_info->enable != 0 &&
		p_info->enable != 1) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	if (p_info->priority != 0 &&
		p_info->priority != 1) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}
	if (p_info->dhcp != 0 &&
		p_info->dhcp != 1) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get device_status */
	/** get old net para */
	ret = net_wired_get_config_para(&param);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	param.enable = p_info->enable;
	param.priority = p_info->priority;
	param.dhcp = p_info->dhcp;

	if (strlen(p_info->eth_ip) > 0)
		strcpy(param.eth_ip, p_info->eth_ip);
	if (strlen(p_info->eth_mask) > 0)
		strcpy(param.eth_mask, p_info->eth_mask);
	if (strlen(p_info->eth_gateway) > 0)
		strcpy(param.eth_gateway, p_info->eth_gateway);
	if (strlen(p_info->eth_dns) > 0)
		strcpy(param.eth_dns, p_info->eth_dns);

	/** set new para */
	ret = net_wired_set_config_para(&param);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * get_wifi_param - Query wifi param
 * @p_info: a pointer of wifi param
 *
 * @return 0-successful, other-failed
 */
int get_wifi_param(wifi_para_config_info *p_info)
{
	int ret = -1;
	wifi_config_para_t param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get device_status */
	ret = net_wifi_get_para(&param);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	p_info->enable_wifi = param.enable_wifi;
	p_info->enable_hotspot = param.enable_hotspot;
	strcpy(p_info->hotspot_name, param.hotspot_name);
	strcpy(p_info->hotspot_password, param.hotspot_password);

	return 0;
}

/**
 * set_wifi_param - Update wifi param
 * @p_info: a pointer of wifi param
 *
 * @return 0-successful, other-failed
 */
int set_wifi_param(wifi_para_config_info *p_info)
{
	int ret = -1;
	wifi_config_para_t param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** set new para */
	memset(&param, 0, sizeof(wifi_config_para_t));
	param.enable_wifi = p_info->enable_wifi;
	param.enable_hotspot = p_info->enable_hotspot;
	strcpy(param.hotspot_name, p_info->hotspot_name);
	strcpy(param.hotspot_password, p_info->hotspot_password);

	/** get device_status */
	ret = net_wifi_set_config_para(&param);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	return 0;
}

/**
 * get_sys_kernel_info - Query system kernel info
 * @p_info: a pointer of system kernel info
 *
 * @return 0-successful, other-failed
 */
int get_sys_kernel_info(sys_base_config_info *p_info)
{
	int ret = -1;
	char kernel_version[256] = {0};

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get device_status */
	ret = system_info_get_kernel_version(kernel_version, sizeof(kernel_version));
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	strcpy(p_info->mode_number, STR_MODE_NUMBER);
	strcpy(p_info->firmware_version, STR_FIRMWARE_VERSION);
	strcpy(p_info->kernel_version, kernel_version);
	strcpy(p_info->product_sn, STR_PRODUCT_SN);

	return 0;
}

/**
 * get_sys_build_info - Query system build info
 * @p_info: a pointer of system build info
 *
 * @return 0-successful, other-failed
 */
int get_sys_build_info(sys_build_config_info *p_info)
{
	int ret = -1;
	build_info_t param;

	/** check para */
	if (p_info == NULL) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	/** get device_status */
	ret = system_info_get_build_info(&param);
	if (ret) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		return ret;
	}

	strcpy(p_info->build_date, param.build_date);
	strcpy(p_info->build_user, param.build_user);
	strcpy(p_info->build_host, param.build_host);
	strcpy(p_info->product_name, param.product_name);

	return 0;
}

/**
 * get_cloud_ipaddr_changed_url - Query cloud_ipaddr_changed_url
 * @param: a pointer of cloud_ipaddr_changed_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_ipaddr_changed_url(char *param, int param_len)
{
	if (NULL == param)
		return -1;

	return db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_CHANGE_URL,
			param,
			param_len,
			DBCONF_CLOUDPLATFORM_CHANGE_URL_VALUE);
}

/**
 * get_cloud_upload_rec_url - Query upload_rec_url
 * @param: a pointer of upload_rec_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_upload_rec_url(char *param, int param_len)
{
	if (NULL == param)
		return -1;

	return db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_UPLOAD_RET_URL,
			param,
			param_len,
			DBCONF_CLOUDPLATFORM_UPLOAD_RET_URL_VALUE);
}

/**
 * get_cloud_upload_rec_url - Query cloud_import_mems_url
 * @param: a pointer of cloud_import_mems_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_import_mems_url(char *param, int param_len)
{
	if (NULL == param)
		return -1;

	return db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_URL,
			param,
			param_len,
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_URL_VALUE);
}

/**
 * get_cloud_import_delete_all_url - Query cloud_import_delete_all_url
 * @param: a pointer of cloud_import_delete_all_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_import_delete_all_url(char *param, int param_len)
{
	if (NULL == param)
		return -1;

	return db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_DEL_ALL_URL,
			param,
			param_len,
			DBCONF_CLOUDPLATFORM_IMPORT_DEL_ALL_URL_VALUE);
}

/**
 * get_cloud_import_mems_ret_url - Query cloud_import_mems_ret_url
 * @param: a pointer of cloud_import_mems_ret_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_import_mems_ret_url(char *param, int param_len)
{
	if (NULL == param)
		return -1;

	return db_config_get_string(DBCONF_CLOUDPLATFORM,
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_RET_URL,
			param,
			param_len,
			DBCONF_CLOUDPLATFORM_IMPORT_MEMS_RET_URL_VALUE);
}

