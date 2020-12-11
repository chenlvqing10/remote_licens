/*
 * config_db.h - export some interface function of operate config db
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

#ifndef _CONFIG_DB_H
#define _CONFIG_DB_H

#define STR_MODE_NUMBER		"lombo"
#define STR_FIRMWARE_VERSION	"v1.0.0.0"
#define STR_PRODUCT_SN		"1234"

typedef struct face_recognition_para {
	/* Identify threshold score
	 * Below this threshold
	 * the face is considered to be a failure(Percentage)
	 */
	float threshold_score;

	/* Nir live detect
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char live_detect;
} recognition_para_config;

typedef struct device_hardware_param {
	/* Volume. Set the volume of the microphone and speaker(range 0~100) */
	int volume;

	/* Automatically adjust screen brightness control
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char auto_display_brightness;

	/* Set the screen brightness value (range 0~100).
	 * When auto_display_brightness function is enabled, Current setting
	 * no effect.
	 */
	int display_brightness;

	/* Infrared light control:
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 *	2: Automatic
	 */
	char infrared_light_set;

	/* Light sensitive control:
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char light_sensitive;

	/* Human induction control:
	 * Options:
	 *	0: Disabled
	 *	1: Enabled
	 */
	char human_induction;
} device_hw_param_config;

typedef struct device_hardware_param_screeninfo {
	/* Automatically adjust screen brightness control
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	int auto_display_brightness;

	/* Set the screen brightness value (range 0~100).
	 * When auto_display_brightness function is enabled, Current setting
	 * no effect.
	 */
	int display_brightness;
} device_hw_param_config_screeninfo;

typedef struct device_hardware_param_light_info {
	/* Automatically fill light enable or disable
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char auto_adapt_enable;

	/* Set the fill switch enable or disable
	 * When auto_adapt_enable is enabled, Current setting
	 * no effect.
	 *	0: Disable
	 *	1: Enable
	 */
	char switch_enable;
} device_hw_param_config_light_info;

typedef struct cloud_platform_info {
	/* AI server URL address。Store the face features library in the cloud.*/
	char ai_server_url[256];

	/* Attendance cloud platform URL address。Manage through the cloud
	 * (Visitor, Approval，Attendance, Personnel management,etc)
	 */
	char attendance_url[256];

	/* FOTA upgrade URL address。periodically detect this server */
	char fota_url[256];

	/* URL address of upload punch record to this server */
	char cloud_upload_rec_url[256];

	/* URL address of import members from this server */
	char cloud_import_mems_url[256];

	/* URL address of delete all members from this server */
	char cloud_import_delete_all_url[256];

	/* URL address of notify ipaddr changed to this server */
	char cloud_ipaddr_changed_url[256];

	/* URL address of import members result to this server */
	char cloud_import_mems_ret_url[256];

} cloud_platform_config_info;

typedef struct dev_time_info {
	/** the current time of divice */
	char curtime[40];

	/** time zone of the divice */
	char timezone;
} dev_time_config_info;

typedef struct develop_sel_para_info {
	/* debug switch
	 * Options:
	 *	0: close
	 *	1: open
	 */
	char debug_sw;
} develop_sel_para_config_info;

typedef struct device_status_info {
	/* Company Name. Information displayed in the UI main window */
	char company_name[32];

	/* Company Logo file name。 */
	char company_logo[32];

	/* Device installation address */
	char device_address[256];

	/* Multi-language choice
	 * Options:
	 *	0: English
	 *	1: Chinese
	 */
	int language;

	/* Punch mode
	 * Options:
	 *	0: 1:N
	 *	1: 1:1 + Card reader
	 *	2: 1:1 + Resident Identity Card
	 *	3: Card reader
	 *	4: Resident Identity Card
	 */
	int punch_mode;

	/* Debug switch
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	int debug_switch;
} device_status_config_info;

typedef struct net_para_info {
	/*net enable*/
	char enable;

	/* Eth priority
	 * Options:
	 *	0: Eth
	 *	1: WiFi
	 */
	char priority;

	/* DHCP
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char dhcp;

	/* Eth ip address */
	char eth_ip[32];

	/* Eth subnet mask */
	char eth_mask[32];

	/* Eth gateway */
	char eth_gateway[32];

	/* Eth DNS */
	char eth_dns[32];
} net_para_config_info;

typedef struct wifi_para_info {
	/* wifi enable */
	int enable_wifi;
	/* Hotspot Enable */
	int enable_hotspot;

	/* Hotspot name */
	char hotspot_name[32];

	/* Hotspot password */
	char hotspot_password[9];
} wifi_para_config_info;

typedef struct sys_base_info {
	/* mode number */
	char mode_number[32];

	/* firmware version */
	char firmware_version[32];

	/* kernel version */
	char kernel_version[256];

	/* product sn */
	char product_sn[32];
} sys_base_config_info;

typedef struct sys_build_info {
	char build_date[64];
	char build_user[64];
	char build_host[64];
	char product_name[64];
} sys_build_config_info;

/**
 * set_recg_cfg: set face recognition's configuration parameter
 * @recg_cfg_para: a pointer of save recognition parameters
 *
 * @return 0-successful, other-failed
 */
int set_recg_cfg(recognition_para_config *recg_cfg_para);

/**
 * get_recg_cfg: get face recognition's configuration parameter
 * @recg_cfg_para: a pointer of save recognition parameters
 *
 * @return 0-successful, other-failed
 */
int get_recg_cfg(recognition_para_config *recg_cfg_para);

/**
 * get_device_hwpara - Query device hardware information
 * @recg_cfg_para: a pointer of device_hw_param_config
 *
 * @return 0-successful, other-failed
 */
int get_device_hwpara(device_hw_param_config *hw_para);

/**
 * set_device_hwpara - Update device hardware information
 * @recg_cfg_para: a pointer of device_hw_param_config
 *
 * @return 0-successful, other-failed
 */
int set_device_hwpara(device_hw_param_config *hw_para);

/**
 * get_device_hwpara_volume - Query device hardware volume
 * @recg_cfg_para: a pointer of saving volume value
 *
 * @return 0-successful, other-failed
 */
int get_device_hwpara_volume(int *volume);

/**
 * set_device_hwpara_volume - Update device hardware volume
 * @recg_cfg_para: the current volume value
 *
 * @return 0-successful, other-failed
 */
int set_device_hwpara_volume(int volume);

/**
 * get_device_hwpara_screeninfo - Query device hardware screeninfo
 * @recg_cfg_para: a pointer of saving volume value
 *
 * @return 0-successful, other-failed
 */
int get_device_hwpara_screeninfo(device_hw_param_config_screeninfo *p_info);

/**
 * set_device_hwpara_screeninfo - Update device hardware screeninfo
 * @recg_cfg_para: the current volume value
 *
 * @return 0-successful, other-failed
 */
int set_device_hwpara_screeninfo(device_hw_param_config_screeninfo *p_info);

/**
 * get_light_info - Query device hardware light info
 * @recg_cfg_para: a pointer of saving volume value
 *
 * @return 0-successful, other-failed
 */
int get_light_info(device_hw_param_config_light_info *p_info);

/**
 * set_light_info - Update device hardware light info
 * @recg_cfg_para: a pointer of saving volume value
 *
 * @return 0-successful, other-failed
 */
int set_light_info(device_hw_param_config_light_info *p_info);

/**
 * get_cloud_platform_info - Query cloud platform info
 * @recg_cfg_para: a pointer of saving cloud platform info
 *
 * @return 0-successful, other-failed
 */
int get_cloud_platform_info(cloud_platform_config_info *p_info);

/**
 * set_cloud_platform_info - Update cloud platform info
 * @recg_cfg_para: a pointer of saving cloud platform info
 *
 * @return 0-successful, other-failed
 */
int set_cloud_platform_info(cloud_platform_config_info *p_info);

/**
 * get_dev_time - Query device time
 * @recg_cfg_para: a pointer of saving device time info
 *
 * @return 0-successful, other-failed
 */
int get_dev_time(dev_time_config_info *p_info);

/**
 * set_dev_time - Update device time
 * @recg_cfg_para: a pointer of saving device time info
 *
 * @return 0-successful, other-failed
 */
int set_dev_time(dev_time_config_info *p_info);

/**
 * get_dev_time - Query develop select
 * @p_info: a pointer of develop select info
 *
 * @return 0-successful, other-failed
 */
int get_develop_select(develop_sel_para_config_info *p_info);

/**
 * set_dev_time - Update develop select
 * @p_info: a pointer of develop select info
 *
 * @return 0-successful, other-failed
 */
int set_develop_select(develop_sel_para_config_info *p_info);

/**
 * get_device_status - Query device status
 * @p_info: a pointer of device status
 *
 * @return 0-successful, other-failed
 */
int get_device_status(device_status_config_info *p_info);

/**
 * set_device_status - Update device status
 * @p_info: a pointer of device status
 *
 * @return 0-successful, other-failed
 */
int set_device_status(device_status_config_info *p_info);

/**
 * get_eth_param - Query eth param
 * @p_info: a pointer of eth param
 *
 * @return 0-successful, other-failed
 */
int get_eth_param(net_para_config_info *p_info);

/**
 * set_eth_param - Update eth param
 * @p_info: a pointer of eth param
 *
 * @return 0-successful, other-failed
 */
int set_eth_param(net_para_config_info *p_info);

/**
 * get_wifi_param - Query wifi param
 * @p_info: a pointer of wifi param
 *
 * @return 0-successful, other-failed
 */
int get_wifi_param(wifi_para_config_info *p_info);

/**
 * set_wifi_param - Update wifi param
 * @p_info: a pointer of wifi param
 *
 * @return 0-successful, other-failed
 */
int set_wifi_param(wifi_para_config_info *p_info);

/**
 * get_sys_kernel_info - Query system base info
 * @p_info: a pointer of system base info
 *
 * @return 0-successful, other-failed
 */
int get_sys_kernel_info(sys_base_config_info *p_info);

/**
 * get_sys_build_info - Query system build info
 * @p_info: a pointer of system build info
 *
 * @return 0-successful, other-failed
 */
int get_sys_build_info(sys_build_config_info *p_info);

/**
 * get_cloud_ipaddr_changed_url - Query cloud_ipaddr_changed_url
 * @param: a pointer of cloud_ipaddr_changed_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_ipaddr_changed_url(char *param, int param_len);

/**
 * get_cloud_upload_rec_url - Query upload_rec_url
 * @param: a pointer of upload_rec_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_upload_rec_url(char *param, int param_len);

/**
 * get_cloud_import_mems_url - Query cloud_import_mems_url
 * @param: a pointer of cloud_import_mems_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_import_mems_url(char *param, int param_len);

/**
 * get_cloud_import_delete_all_url - Query cloud_import_delete_all_url
 * @param: a pointer of cloud_import_delete_all_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_import_delete_all_url(char *param, int param_len);

/**
 * get_cloud_import_mems_ret_url - Query cloud_import_mems_ret_url
 * @param: a pointer of cloud_import_mems_ret_url
 * @len: length of param buffer
 *
 * @return 0-successful, other-failed
 */
int get_cloud_import_mems_ret_url(char *param, int param_len);

#endif

