/*
* db_all.h - define interface function of db
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
#ifndef __DB_ALL_H
#define __DB_ALL_H

#include "ee_queue.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

#define WIFI_NAME_LEN	64
#define WIFI_PWD_LEN	64

/*
* defined the max of persons insert the db
* NOTE: If change the count, you should modify the datebase limit
*  at the some time.
*/
#if ATTENDANCE_MODE == 0
#define MAX_FEAT_CNT	20000
#else
#define MAX_FEAT_CNT	5000
#endif

/* eth ip parameter */
typedef struct tag_net_para {
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
} net_config_para_t;

/* wifi config parameter */
typedef struct {
	/* wifi enable */
	char enable_wifi;

	/* Hotspot Enable */
	char enable_hotspot;

	/* Hotspot name */
	char hotspot_name[WIFI_NAME_LEN];

	/* Hotspot password */
	char hotspot_password[WIFI_PWD_LEN];
} wifi_config_para_t;

/*hal*/
typedef struct tag_device_hw_param {
	/* Volume. Set the volume of the microphone and speaker(range 0~100) */
	char volume;

	/* Automatically adjust screen brightness control
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 */
	char auto_display_brightness;

	/* Set the screen brightness value (range 0~100).
	 * When auto_display_brightness function is enabled, Current setting
	 * no effect.
	 */
	char display_brightness;
	char auto_screen_off;
	int auto_screen_off_timeout;
	char auto_screen_saver_off;
	int auto_screen_saver_timeout;

	/* Infrared light control:
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 *    2: Automatic
	 */
	char infrared_light_set;

	/* Light sensitive control:
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 */
	char light_sensitive;
	int light_high_brightness;
	int light_low_brightness;
	char light_det_interval;
	char light_delay_time;
	int gpio;

	/* Human induction control:
	 * Options:
	 *    0: Disabled
	 *    1: Enabled
	 */
	char human_induction;

	char device_name[128];

	/* Wiegand protocol, 26 or 34 */
	char wiegand_protocol;

	/* Door sensor type:
	 * Options:
	 *    0: Disabled
	 *    1: Enabled
	 *    2: Not detction, default
	 */
	char door_sensor;

	/* Door sensor delay, 1~255s */
	char door_sensor_delay;

	/* Alarm switch control:
	 * Options:
	 *    0: Disabled
	 *    1: Enabled
	 */
	char alarm_switch;
	char access_time[128];
	char access_type;
} device_hw_param_t;

/*time param*/
typedef struct tag_device_time_param {
	/* auto set time:
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char auto_set_time;
	/* auto set timezone:
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char auto_set_timezone;

	char timezone[32];
} device_time_param_t;

/* auto reboot time param*/
typedef struct tag_auto_reboot_param {
	/* reboot_interval:
	 * Options:
	 *	0: month
	 *	1: week
	 *	2: day
	 */
	char reboot_interval;

	char reboot_time[16];
} auto_reboot_param_t;

typedef struct tag_device_lang_param {
	/*  set language:
	 * Options:
	 *	0: Chinese
	 *	1: English
	 */
	char lang;

} device_lang_param_t;

typedef struct tag_device_video_param {
	/*  set language:
	 * Options:
	 *	0: Close
	 *	1: Open
	 */
	char rtsp_video_set;
	char video_type_set;

} device_video_param_t;

/*attendance param*/
typedef struct tag_attendance_param {
	int saved_attendance_pic;
	int avoid_attendance_step;
	int access_records;
} attendance_param_t;

/* checkin param */
typedef struct tag_checkin_param {
	int save_large_pic;
	int save_small_pic;
	int save_stranger_record;
} checkin_param_t;

/*net param*/
typedef struct tag_device_net_param {
	/* Eth settings.
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 */
	char enable_eth;

	/* Eth priority
	 * Options:
	 *    0: Eth
	 *    1: WiFi
	 */
	char enable_eth_priority;

	/* DHCP
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 */
	char enable_eth_dhcp;

	/* Eth ip address */
	char eth_ip[256];

	/* Eth subnet mask */
	char eth_mask[256];

	/* Eth gateway */
	char eth_gateway[256];

	/* Eth DNS */
	char eth_dns[256];

	/* WiFi
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 */
	char enable_wifi;

	/* Hotspot
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 */
	char enable_hotspot;

	/* Hotspot name */
	char hotspot_name[WIFI_NAME_LEN];

	/* Hotspot password */
	char hotspot_password[WIFI_PWD_LEN];
} device_net_param_t;

/*face*/
typedef struct tag_face_recognition_param {
	/* The closest distance allowed by face recognition(Unit: m) */
	float identify_closest_distance;

	/* The farthest distance allowed by face recognition(Unit: m)*/
	float identify_furthest_distance;

	/* Identify threshold score
	 * Below this threshold, the face is considered to be a failure(Percentage)
	 */
	float identify_threshold_score;

	/* Nir live detect
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 */
	char nir_live_detect;

	/* Mask identify threshold score
	 * Below this threshold, the mask check is considered to be a failure(Percentage)
	 */
	float mask_threshold_value;

	/* mask check enable state
	 * Options:
	 *	  0: Disable
	 *	  1: Enable
	 */
	char mask_enable;

	char recg_enable;
	float living_detect_threshold_score;
	char mask_detect_enable;
	char infrared_image_enable;
	int recg_timeout;
	char repeat_recg_enable;
	int recg_interval;
	int recg_distance;
	int voice_mode;
	char grettings;
	int stranger_voice;
	char qr_code_recg;
	char body_temperature_detect;
	float temperature_compensation;
	float alarm_temperature;
	int exposure_values;
} face_recognition_param_t;

/*system info*/
typedef struct tag_device_system_info {
	char face_recg_alg_version[32];
	char firmware_version[32];
	char device_name[128];
} device_system_info_t;

/*platform info*/
typedef struct tag_device_platform_info {
	int platform_enable;
	char platform_ip[64];
} device_platform_info_t;

/* main ui settings parameter */
typedef struct tag_main_setting_param_t {
	char			company_name[128];
	/* char			company_logo[128]; */
	char			device_name[128];
	char			face_border_color[16];
	char			name_display;
	char			name_format;
	char			mac_display;
	char			ip_display;
	char			register_count_display;
} main_setting_param_t;

/*websocket client param*/
typedef struct tag_websocket_client_param {
	char sn[64];
	char seed_secret_no[64];
	char websocket_url[64];
	char device_id[64];
	char static_qrcode_url[128];
	char secret_no[64];
	char secret[64];
	char active_code[64];
	char firmware_verson[32];
	int perm_version;
} websocket_client_param_t;

/*mode+ui param*/
typedef enum {
#if 0
	MODE_FACTORY = 0,
	MODE_WECHAT,
#endif
	MODE_OTHER
} enum_mode_t;

typedef struct tag_mode_ui_param {

	int rotate;
	int mode;

} mode_ui_param_t;

/*check in*/
typedef struct tag_websocket_checkin_info {

	char user_id[64];
	int user_type;
	int timestamp;
	int is_upload;
	int upload_time;
	char face_path[128];
	char name[64];
	float temperature;
	char frame_path[128];
	ee_queue_t queue;
} web_socket_checkin_info_t;

/* export check in*/
typedef struct tag_websocket_export_checkin_info {
	char userid[64];
	char name[64];
	int check_in_time;
	ee_queue_t queue;
} web_socket_export_checkin_info_t;


typedef enum {
	TYPE_INT = 0,
	TYPE_FLT = 1,
	TYPE_STR
}query_type_t;

/*admin list*/
typedef struct tag_websocket_admin_info {
	char user_id[64];
	char name[32];
	ee_queue_t queue;
} websocket_admin_info_t;

/*person info */
typedef struct tag_websocket_person_info {

	char user_id[64];
	char name[32];
	char access_card_number[32];
	int user_type;
	long long openvid;
	char pic_large[FILENAME_MAX];
	char pic_small[FILENAME_MAX];
	int add_source;
	int add_time;
	int face_id;
	char face_feature[1024];
	float key_points[10];
	float face_norm[2];
	ee_queue_t queue;

} websocket_person_info_t;

typedef struct tag_local_person_info {
	char user_id[64];
	char name[32];
	char pic_large[FILENAME_MAX];
	char pic_small[FILENAME_MAX];
	char access_card_number[32];
	ee_queue_t queue;
} local_person_info_t;

typedef enum {
	TYPE_PERSON = 0,
	TYPE_VISITOR
} person_type_t;

/*rules*/
typedef struct tag_websocket_rule_info {
	char user_id[64];
	int rule_id;
	char pass_rule[64];
	int effect_time;
	ee_queue_t queue;
} web_socket_rule_info_t;

/**
 * ui_device_set_attendance_param - Query attendance parameter.
 * @p_info: A pointer to attendance_param_t.

 *
 * Returns 0 if success, otherwise failure.
 */
int ui_device_set_attendance_param(attendance_param_t* p_info);

/**
 * ui_device_get_attendance_param - Query attendance parameter.
 * @p_info: A pointer to attendance_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int ui_device_get_attendance_param(attendance_param_t* p_info);

/**
 * ui_device_set_checkin_param - Query checkin parameter.
 * @p_info: A pointer to checkin_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int ui_device_set_checkin_param(checkin_param_t* p_info);

/**
 * ui_device_get_checkin_param - Query checkin parameter.
 * @p_info: A pointer to checkin_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int ui_device_get_checkin_param(checkin_param_t* p_info);

/**
 * db_device_hw_param_update - Update device hardware information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_hw_param_update(device_hw_param_t *p_info);

/**
 * db_auto_reboot_param_update - Update auto reboot information.
 * @p_info: A pointer to auto_reboot_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_auto_reboot_param_update(auto_reboot_param_t* p_info);

/**
 * db_auto_reboot_param_query - Query auto reboot information.
 * @p_info: A pointer to auto_reboot_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_auto_reboot_param_query(auto_reboot_param_t* p_info);

/**
 * db_main_setting_param_update - Update main ui information.
 * @p_info: A pointer to main_setting_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_main_setting_param_update(main_setting_param_t* p_info);

/**
 * db_main_setting_param_query - Query main ui information.
 * @p_info: A pointer to main_setting_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_main_setting_param_query(main_setting_param_t* p_info);

/**
 * db_device_hw_param_query - Query device hardware information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_hw_param_query(device_hw_param_t *p_info);

/**
 * device_net_param_print - Display information in a queue for
 * debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void device_net_param_print(device_net_param_t *p_info);

/**
 * db_device_net_param_update - Update cloud platform information.
 * @p_info: A pointer to device_net_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_net_param_update(device_net_param_t *p_info);

/**
 * db_device_net_param_query - Query cloud platform information.
 * @p_info: A pointer to device_net_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_net_param_query(device_net_param_t *p_info);

/**
 * face_recognition_param_print - Display information in a queue for debugging
 * purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void face_recognition_param_print(face_recognition_param_t *p_info);

/**
 * db_face_recognition_param_update - Update face recognition param
 * information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_recognition_param_update(face_recognition_param_t *p_info);

/**
 * db_face_recognition_param_query - Query face recognition param.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_recognition_param_query(face_recognition_param_t *p_info);

/**
 * db_ui_get_value - get value of key information
 *
 * Returns 0 if success, otherwise failure.
 */
int db_ui_get_value(const char *key, char *value, int size);

/**
 * db_ui_set_value - set value of key information
 *
 * Returns 0 if success, otherwise failure.
 */
int db_ui_set_value(const char *key, char *value);

/**
 * db_device_system_info_update - Update system info parameter.
 * @p_info: A pointer to device_system_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_device_system_info_update(device_system_info_t *p_info);

/**
 * db_device_system_info_query - Query system info parameter.
 * @p_info: A pointer to device_system_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_device_system_info_query(device_system_info_t *p_info);

/**
 * db_device_platform_info_update - Update platform info parameter.
 * @p_info: A pointer to device_platform_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_device_platform_info_update(device_platform_info_t *p_info);

/**
 * db_device_platform_info_query - Query platform info parameter.
 * @p_info: A pointer to device_platform_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_device_platform_info_query(device_platform_info_t *p_info);

/**
 * db_mode_ui_param_update - Update ui mode parameter.
 * @p_info: A pointer to mode_ui_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_mode_ui_param_update(mode_ui_param_t *p_info);

/**
 * db_mode_ui_param_query - Query ui mode parameter.
 * @p_info: A pointer to mode_ui_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_mode_ui_param_query(mode_ui_param_t *p_info);

/**
 * db_websocket_client_param_update - Update websocket client parameter.
 * @p_info: A pointer to websocket_client_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_websocket_client_param_update(websocket_client_param_t *p_info);

/**
 * db_websocket_client_param_query - Query websocket client parameter.
 * @p_info: A pointer to websocket_client_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_websocket_client_param_query(websocket_client_param_t *p_info);

/**
 * db_perm_version_update - Update perm version parameter.
 * @version: perm version.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_perm_version_update(int version);

/**
 * db_perm_version_query - Query perm version parameter.
 * @version: perm version.
 *
 * Returns perm version.
 */
int db_perm_version_query(void);

/**
 * db_perm_factory_test_update - Update factory test flag value.
 * @bit_value: A 32bit value for restore factory test flag.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_factory_test_update(int bit_value);

/**
 * db_perm_factory_test_query - query factory test flag value.
 *
 * Returns factory test flag value. if error, this value will
 * be -1;
 */
int db_factory_test_query(void);

/**
 * db_debug_switch_update - Update debug mode switch value.
 * @mode: The debug mode value.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_debug_switch_update(int mode);

/**
 * db_debug_switch_query - query debug mode value.
 *
 * Returns debug switch value. if error, this value will
 * be -1;
 */
int db_debug_switch_query(void);

/**
 * db_delay_time_update - Update delay time value.
 * @mode: The relay time value.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_delay_time_update(int relay_time);

/**
 * db_delay_time_query - query delay time value.
 *
 * Returns relay time value. if error, this value will
 * be -1;
 */
int db_delay_time_query(void);

/**
 * db_wiegand_protocol_update - Update wiegand protocol value.
 * @wiegand_protocol: The wiegand protocol value.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_wiegand_protocol_update(int wiegand_protocol);

/**
 * db_wiegand_protocol_query - query wiegand protocol value.
 *
 * Returns wiegand data size value. if error, this value will
 * be -1;
 */
int db_wiegand_protocol_query(void);

/**
 * db_door_sensor_type_update - Update wiegand protocol value.
 * @door_sensor_type: The door sensor type.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_door_sensor_type_update(int door_sensor_type);

/**
 * db_door_sensor_type_query - query door sensor type.
 *
 * Returns door sensor type. if error, this value will
 * be -1;
 */
int db_door_sensor_type_query(void);

/**
 * db_door_sensor_delay_update - Update door sensor delay time.
 * @delay_time: The door sensor delay time.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_door_sensor_delay_update(int delay_time);

/**
 * db_door_sensor_delay_query - query door sensor delay.
 *
 * Returns door sensor delay. if error, this value will
 * be -1;
 */
int db_door_sensor_delay_query(void);

/**
 * db_alarm_switch_update - Update alarm switch.
 * @alarm_switch: The alarm switch.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_alarm_switch_update(int alarm_switch);

/**
 * db_access_type_query - query access type.
 *
 * Returns access type.
 */
int db_access_type_query();

/**
 * db_access_time_query - query access time.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_access_time_query(char *access_time);

/**
 * db_access_time_update - Update access time.
 * @access_time: The access time.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_access_time_update(char *access_time);

/**
 * db_query_sysinfo_by_key - Query sysinfo table by key.
 * @in key: query key.
 * @in type: query out type: int,float,string
 * @out value: query result pointer, need allocate by user
 *
 * Returns 0 if success, otherwise -1.
 */
int db_query_sysinfo_by_key(char *key, query_type_t type, void *value);

/**
 * db_websocket_admin_info_query_begin - query admin information.
 * @head: A pointer of admin info queue.
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_websocket_admin_info_query_begin(ee_queue_t *head);

/**
 * db_websocket_admin_info_query_end - free admin information.
 * @head: A pointer of admin info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_admin_info_query_end(ee_queue_t *head);

/**
 * db_websocket_admin_info_del - delete all admin information.
 * @head: A pointer of admin info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_admin_info_del();

/**
 * db_websocket_admin_info_insert - insert admin information.
 * @head: A pointer of admin info queue.
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_websocket_admin_info_insert(ee_queue_t *head);

/**
 * db_websocket_person_info_query_begin - query person information.
 * @head: A pointer of person info queue.
 * @type: table type: 0/1: employee/visitor;
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_websocket_person_info_query_begin(ee_queue_t *head, int type);

/**
 * db_person_info_mmc - query person information.
 * @type: table type: 0/1: employee/visitor;
 * @face_db: A pointer of admin info queue.
 * @index: the index of start.
 * @count: query the most count of person;
 *
 * Returns >= 0 admin info record count
 */
int db_person_info_mmc(int type, void *face_db,
  int feat_cnt, int *index, int count);

/**
 * db_person_info_query_begin - query person information.
 * @head: A pointer of admin info queue.
 * @type: table type: 0/1: employee/visitor;
 * @index: the index of start.
 * @count: query the most count of person;
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_person_info_query_begin(ee_queue_t* head,
	int type, int index, int count);

/**
 * db_websocket_person_info_query_end - free person information.
 * @head: A pointer of person info queue.
 * @type: table type: 0/1: employee/visitor;
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_person_info_query_end(ee_queue_t *head, int type);

/**
 * db_person_info_get_begin - query person information with user_id.
 * @head: A pointer of admin info queue.
 * @type: table type: 0/1: employee/visitor;
 * @user_id: the user id regiseter.
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_person_info_get_begin(ee_queue_t* head,
	int type, char *user_id);

/**
 * db_ui_person_info_exist - is the user id regiseter
 *
 * Returns  0 not exist, other exist..
 */
int db_ui_person_info_exist(char *user_id);
int db_access_card_number_exit(char* cardno);

/**
 * db_ui_person_info_get_count - get count of person information
 *
 * Returns count of person information.
 */
int db_ui_person_info_get_count(int type);

/**
 * db_ui_person_info_query_begin - query person information
 * by index, count parameter.
 * @head: A pointer of person info queue.
 * @index: index of record in table;
 * @count: count of record;
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_ui_person_info_query_begin(ee_queue_t *head, int index, int count);

/**
 * db_ui_person_txt_query_begin - query person information
 * by index, count parameter.
 * @head: A pointer of person info queue.
 * @index: index of record in table;
 * @count: count of record;
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_ui_local_person_query_begin(ee_queue_t *head, int index, int count);
int db_ui_local_person_query_search_begin(ee_queue_t *head,
  char *id, char *name, int index, int count);
int db_ui_local_person_query_end(ee_queue_t *head);
int db_ui_local_person_get_count(char *userid, char *name);

/**
 * db_ui_person_info_query_end - free person information.
 * @head: A pointer of person info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_ui_person_info_query_end(ee_queue_t *head);

/**
 * db_websocket_person_info_del_by_user_id - delete person information.
 * @user_id: user id.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_person_info_del_by_user_id(char *user_id);

/**
 * db_websocket_person_info_del - delete all person information from tale.
 * @type: table type(0/1:employee/vistor table).
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_person_info_del(int type);

/**
 * db_websocket_person_info_insert - insert person information.
 * @head: A pointer of person info queue.
 *
 * Returns 0 success, -1: failure.
 */
int db_websocket_person_info_insert(ee_queue_t *head);

/**
 * db_websocket_person_info_update_feat - update person information(feature).
 * @p_info: A pointer of websocket_person_info_t.
 *
 * Returns 0 success, -1: failure.
 */
int db_websocket_person_info_update_feat(websocket_person_info_t *p_info);

/**
 * db_websocket_person_info - get person information(feature).
 * @p_info: A pointer of websocket_person_info_t.
 *
 * Returns 0 success, -1: failure.
 */
int db_websocket_person_info(websocket_person_info_t* p_info);

/**
 * db_websocket_person_info_query_begin_by_name
 * Query employee infos by name
 * @user_name A string of user name
 * @head The pointer to result queue
 *
 * Returns -1 if errors, otherwise the nums of related records.
 */
int db_websocket_person_info_query_begin_by_name(
	char *user_name, ee_queue_t *head);

/**
 * db_websocket_rule_info_query_begin - query rule information
 * @head: A pointer of rule info queue.
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_websocket_rule_info_query_begin(ee_queue_t *head);

/**
 * db_websocket_rule_info_query_end - free rule information.
 * @head: A pointer of rule info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_rule_info_query_end(ee_queue_t *head);

/**
 * db_websocket_rule_info_del_by_user_id - delete rule information.
 * @user_id: user id.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_rule_info_del_by_user_id(char *user_id);

/**
 * db_websocket_rule_info_insert - add rule information.
 * @head: A pointer of rule info queue.
 * @type: type: 0/1: person/guest
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_rule_info_insert(ee_queue_t *head, int type);

/**
 * db_websocket_checkin_info_query_begin - query checkin information.
 * @head: A pointer of checkin info queue.
 *
 * Returns >= 0 record count, -1: failure.
 */
int db_websocket_checkin_info_query_begin(
  ee_queue_t *head, int count);

/**
 * db_websocket_checkin_info_query_by_id_time_begin - query checkin information.
 * @head: A pointer of checkin info queue.
 * @user_id: The user_id of checkin info.
 * @start: The start time of checkin info.
 * @end: The end time of checkin info.
 *
 * Returns >= 0 record count, -1: failure.
 */
int db_websocket_checkin_info_query_by_id_time_begin(
	ee_queue_t* head, char *user_id, long start, long end);

int db_websocket_checkin_info_count_by_id_time(
	char *user_id, long start, long end);

int db_websocket_checkin_query_limit_begin(
  ee_queue_t *head, char *user_id, long start, long end,
  int index, int count);

/**
 * db_websocket_checkin_info_query_by_time_begin - query checkin information.
 * @head: A pointer of checkin info queue.
 * @start: The start time of checkin info.
 * @end: The end time of checkin info.
 *
 * Returns >= 0 record count, -1: failure.
 */
int db_websocket_checkin_info_query_by_time_begin(
	ee_queue_t* head, long start, long end);

/**
 * db_websocket_checkin_info_query_end - free checkin information.
 * @head: A pointer of checkin info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_checkin_info_query_end(ee_queue_t *head);

//int db_websocket_checkin_info_del_by_user_id(char *user_id);
int db_websocket_checkin_info_del_by_day(int days);

int db_websocket_checkin_info_del_by_time(int timestamp);

int db_websocket_checkin_info_del_table();

int db_websocket_checkin_info_del_by_last(int count);

/**
 * db_websocket_checkin_info_get_count - get count of checkin information
 *
 * Returns count of checkin information.
 */
int db_websocket_checkin_info_get_count(void);

/**
 * db_websocket_checkin_info_insert - insert checkin information.
 * @p_info: A pointer of web_socket_checkin_info_t.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_checkin_info_insert(web_socket_checkin_info_t *p_info);

/**
 * db_websocket_checkin_info_del - delete checkin information.
 * @p_info: A pointer of web_socket_checkin_info_t.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_checkin_info_del(web_socket_checkin_info_t *p_info);

/**
 * db_ui_get_development_status - get development status.
 *
 * Returns development status.
 */
int db_ui_get_development_status(void);

/**
 * db_ui_get_password - get password of enter main menu.
 * @password: A pointer of password.
 * @pwd_len: length of password buffer
 *
 * Returns  0 success, -1: failure.
 */
int db_ui_get_password(char *password, int pwd_len);

/**
 * db_ui_set_password - set password of enter main menu.
 * @password: password of enter main menu.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_ui_set_password(const char *password);

/**
 * db_ui_export_rec_get_count - get total count of export record.
 * @password: password of enter main menu.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_ui_export_rec_get_count(void);

int db_ui_export_by_time_get_count(char *user_id,
	long start, long end);

/**
 * db_ui_export_rec_query_begin - query export record.
 * @index: index of total record.
 * @count: count of current record.
 * @head: a pointer of query result queue
 *
 * Returns 0 if success, otherwise -1.
 */
int db_ui_export_rec_query_begin(int index, int count, ee_queue_t *head);

int db_ui_export_rec_query_by_id_time_begin(
  int index, int count, ee_queue_t* head, char *user_id,
  long start, long end);

/**
 * db_ui_export_rec_query_end - free query result.
 * @head: a pointer of query result queue.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_ui_export_rec_query_end(ee_queue_t *head);

/**
 * db_device_time_param_update - update time config param
 * @p_info: iA pointer of device_time_param_t.
 * Returns 0 if success, otherwise -1.
 */
int db_device_time_param_update(device_time_param_t* p_info);

/**
 * db_device_time_param_query - query time config info.
 * @p_info: A pointer of device_time_param_t.
 * Returns 0 if success, otherwise -1.
 */
int db_device_time_param_query(device_time_param_t* p_info);

/**
 * db_device_lang_param_query - query time config info.
 * @p_info: A pointer of device_lang_param_t.
 * Returns 0 if success, otherwise -1.
 */
int db_device_lang_param_query(device_lang_param_t* p_info);

/**
 * db_device_lang_param_update - query time config info.
 * @p_info: A pointer of device_lang_param_t.
 * Returns 0 if success, otherwise -1.
 */
int db_device_lang_param_update(device_lang_param_t* p_info);

/**
 * db_device_rtsp_video_param_query - query time config info.
 * @p_info: A pointer of device_lang_param_t.
 * Returns 0 if success, otherwise -1.
 */
int db_device_video_param_query(device_video_param_t* p_info);

/**
 * db_device_lang_param_update - query time config info.
 * @p_info: A pointer of device_lang_param_t.
 * Returns 0 if success, otherwise -1.
 */
int db_device_video_param_update(device_video_param_t* p_info);

/**
 * db_all_init - database system parameter initialize.
 *
 * Returns none.
 */
void db_all_init();

void db_deinit();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DB_ALL_H */

