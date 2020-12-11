/*
* ui_control.h - define interface function of ui_control
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
#ifndef API_CONTROL_INC_UI_CONTROL_H_
#define API_CONTROL_INC_UI_CONTROL_H_

#include "db_all.h"
#include "face.h"
#include "person_import.h"
#include "person_reg_check.h"
#include "face_svc_isp.h"
#include "light_svc.h"
#include "screen_svc.h"
#include "reboot_svc.h"
#include "checkin_export_svc.h"
#include "relay.h"
#include "rs.h"
#include "live_video.h"

#define MAX_FIELD_LEN 32
#define KEY_POINT_NUM 10
#define NORM_NUM 2
#define MAX_BUFFER_SIZE 256
#define CALIBRATION_NUM 5

/*timezone param*/
typedef enum {
	Beijing = 0,
	Tokyo,
	New_York,
	London,
	Paris,
	Berlin
} enum_timezone_t;

/* user parameter */
typedef struct ui_user_information {
	char	employee_number[MAX_FIELD_LEN];
	char	employee_name[MAX_FIELD_LEN];
	char	password[MAX_FIELD_LEN];
	char	face_feat[FACE_FEAT_SIZE];
	float	key_points[KEY_POINT_NUM];
	float	face_norm[NORM_NUM];
	float	score;
	char	authorization;
	char	save_frame_path[FILENAME_MAX];
} user_information_param_t;

/* algorithm parameter */
typedef struct tag_algorithm_system_info {
	char			recg_enable;
	char			infrared_image_enable;
	float			face_threshold_value;
	char			living_nir;
	float			living_detect_threshold_score;
	char			mask_detect_enable;
	char			mask_enable;
	float			mask_threshold_value;
	int			recg_timeout;
	char			repeat_recg_enable;
	int			recg_interval;
	int			recg_distance;

	/* bit flag: 0,paly_name; 1,play_grettings; */
	int			voice_mode;

	/* 0: please pass; 1: welcome; 2: time greetings */
	char			grettings;

	/* bit flag: 0,paly_prosthesis; 1,play_stranger; */
	int			stranger_voice;
	char			qr_code_recg;
	char			body_temperature_detect;
	float			temperature_compensation;
	float			alarm_temperature;
	int			exposure_values;
	char			WDR;
} algorithm_system_info_t;

/* access parameter */
typedef struct tag_access_param_t {
	/* wiegand protocol, 26 or 34 */
	char			wiegand_protocol;

	/* Door sensor type:
	 * Options:
	 *    0: Disabled
	 *    1: Enabled
	 *    2: Not detction, default
	 */
	char			door_sensor;
	char			door_sensor_delay;
	char			alarm_switch;
	char			relay_delay_time;
	char 			volum;
	char			brightness;
	char 			language_choose;
	char 			light;
	char			access_time;
	char			access_type;
} access_param_t;

/* system parameter */
typedef struct tag_system_param_t {
	enum_timezone_t zone_area;
	char 			volum;
	char			brightness;
	char			light_enable;
	char 			language_choose;
	char 			rtsp_video;
	char 			video_type;
	char			light;
} system_param_t;

/* factory test status */
typedef struct {
	unsigned int lcd:2;
	unsigned int touch_lcd:2;
	unsigned int camera:2;
	unsigned int wifi:2;
	unsigned int bluetooth:2;
	unsigned int rtc:2;
	unsigned int wired:2;
	unsigned int sensor_illuminance:2;
	unsigned int light:2;
	unsigned int light_infrared:2;
	unsigned int speaker:2;
	unsigned int relay:2;
	unsigned int rs485:2;
	unsigned int wiegand:2;
	unsigned int alarm:2;
} test_status_t;

typedef union {
	test_status_t status;
	int db_status;
} un_test_status_t;

/* face feature parameter */
typedef struct {
	char face_feature[FACE_FEAT_SIZE];
	float key_points[KEY_POINT_NUM];
	float face_norm[NORM_NUM];
	float score;
	char save_frame_path[FILENAME_MAX];
} ui_feat_local_t;

/* debug switch for developer */
typedef struct {
	unsigned int debug_for_time:1;
	unsigned int debug_for_isp:1;
	unsigned int debug_for_record:1;
	unsigned int debug_for_status:1;
} debug_switch_t;

typedef union {
	int debug_data;
	debug_switch_t debug_switch;
} un_debug_switch_t;

/* camera calibration param */
typedef struct _ui_map {
	double x_map[CALIBRATION_NUM];
	double y_map[CALIBRATION_NUM];
} ui_map_t;

typedef struct ui_map_params {
	int check;
	double x_map[CALIBRATION_NUM];
	double y_map[CALIBRATION_NUM];
} ui_map_params_t;

typedef struct _systemtime {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} systemtime;

typedef struct _memory {
	int total;
	int free;
	int used;
} memory;

/*person register*/
int ui_insert_user(user_information_param_t *p_info);

int ui_batch_import_start(void);

int ui_factory_default(void);

void ui_factory_set_flag(void);

int ui_update_device_start(void);

int ui_device_eth_param_query(net_config_para_t *p_info);

int ui_device_eth_param_update(net_config_para_t *p_info);

int ui_device_wifi_param_update(wifi_config_para_t *p_info);

int ui_device_wifi_param_query(wifi_config_para_t *p_info);

int ui_device_get_fill_light_param(fill_light_param_t *p_info);
int ui_device_get_fill_rtsp_video_param(device_video_param_t *p_info);


int ui_device_get_screen_param(screen_bright_t *p_bright,
			screen_off_status_t *p_status);

int ui_device_get_access_param(access_param_t *p_info);

int ui_control_server_running(void);

int ui_control_set_display_brightness(int brightness);

int ui_control_set_infrared_light(char light);

int ui_control_set_rtsp_video(char rtsp_video);

int ui_control_set_video_type(char video_type);

int ui_control_set_volume(int volume);

int ui_control_get_volume(int *volume);

int ui_control_set_time(systemtime *date);

int ui_control_set_timezone(enum_timezone_t zone);

int ui_control_get_main_setting_param(main_setting_param_t *main_setting_param);

int ui_control_set_main_setting_param(main_setting_param_t *main_setting_param);

int ui_control_get_reboot_time(auto_reboot_param_t *p_info, data_time_t *p_reboot_time);

int ui_control_get_auto_reboot_param(auto_reboot_param_t *auto_reboot_param);

int ui_control_set_auto_reboot_param(auto_reboot_param_t *auto_reboot_param);

enum_timezone_t ui_control_get_timezone();

int ui_control_get_system_info(system_param_t *sys_info);

/* save qrcode to file */
int ui_get_qrcode_path(char *path);

int ui_get_ui_mode(int *mode);

int ui_set_ui_mode(enum_mode_t mode);

int ui_get_ui_rotate(int *rotate);

/* system info */
int get_sys_info(device_system_info_t *p_info);

int ui_get_algorithm_system_info(algorithm_system_info_t *p_info);

int ui_set_algorithm_system_info(algorithm_system_info_t *p_info);

faces_t *ui_get_faces();

void ui_put_faces(faces_t *faces);

void ui_face_set_login_mode(void);

/*0: no, 1: yes */
int ui_face_webchat_is_active(void);

/*take a photo and calc feats*/
int ui_control_calc_feat_from_photo(ui_feat_local_t *feat, int sec);

int ui_control_test_light(void);

int ui_control_test_light_infrared(void);

int ui_control_test_speaker(void);

int ui_control_test_wifi(void);

int ui_control_test_sensor_illuminance(void);

int ui_control_test_get_sensor_illuminance_value(int *value);

int ui_control_test_rtc(void);

int ui_control_test_camera(void);

int ui_control_test_eth(void);

int ui_control_test_gmac(void);

int ui_control_test_bluetooth(void);

int ui_control_test_usb(void);

int ui_control_sta_mem(memory *memory);

int ui_control_set_lang(int lang);

int ui_control_get_lang(device_lang_param_t *info);

int ui_control_get_time_param(device_time_param_t *info);

/* batch import */
int ui_control_person_import(void);
int ui_control_exit_person_import(void);

int ui_control_get_peron_import_status(person_import_ctx_t *p_info);

/* batch recognition */
int ui_control_person_recg(void);
int ui_control_exit_person_recg(void);

int ui_control_get_person_recg_status(person_reg_check_ctx_t *p_info);

int ui_control_get_debug_switch(un_debug_switch_t *debug);

int ui_control_set_debug_switch(un_debug_switch_t *debug);

int ui_control_get_isp_data(struct viss_isp_exif *exif_rgb,
							struct viss_isp_exif *exif_nir);

int ui_control_record_start(void);

int ui_control_record_stop(void);

void ui_control_wakeup_screen(void);

int ui_control_face_pause(void);

int ui_control_face_resume(void);

int ui_control_camera_nir_start_preview(
					int x, int y, int w, int h, int rotate);

int ui_control_camera_nir_stop_preview(void);

/*add person success sound*/
int ui_control_add_person_sound(void);

/*get websocket link status*/
int ui_control_websocket_is_link(void);

int ui_control_snapshot(int cam_id, char *file_name);

/* camera calibration param */
int ui_control_get_ord_map_params(ui_map_t *ord_map_params);

int ui_control_get_mod_map_params(ui_map_t *ord_map_params,
			ui_map_t *mod_map_params);

int ui_control_save_mod_map_param(ui_map_t *mod_map_params);

int ui_control_take_mod_pic(int x, int y, int w, int h);

int ui_control_cal_mod_map(map_params_t *map);

/*get websocket link status
  1 is link
  0 not link
  */
int ui_control_ws_is_link(void);

int ui_control_wifi_is_link(void);

int ui_control_get_sn(char *sn, int buf_len);

int ui_control_get_current_mode(mode_ui_param_t *mode);

int ui_control_person_info_query_begin(ee_queue_t *head, int type);

int ui_control_person_info_query_end(ee_queue_t *head, int type);

int ui_control_person_info_get_count(void);

int ui_control_person_info_get_begin(ee_queue_t *head,
	int index, int count);

int ui_control_local_person_get_begin(ee_queue_t *head,
	int index, int count);
int ui_control_local_person_get_end(ee_queue_t *head);

int ui_control_person_info_get_end(ee_queue_t *head);

/*  set time config */
int ui_control_set_auto_time(char auto_time);

int ui_control_set_auto_timezone(char auto_timezone);

/* delete person by user_id */
int ui_control_person_info_del_by_user_id(char *user_id);

/* clean person in table */
int ui_control_person_info_del(int type);

/* get delay time of relay open door */
int ui_control_get_delay_time(void);

/* set delay time of relay open door */
int ui_control_set_delay_time(int delay_time);

/**
 * @brief  get wiegand protocol
 *
 * Returns wiegand data size contain even bit and odd bit.
 */
int ui_control_get_wiegand_protocol(void);

/**
 * @brief  set wiegand protocol
 * @wiegand_protocol: wiegand protocol
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_wiegand_protocol(int wiegand_protocol);

/**
 * @brief  set door sensor type
 * @wiegand_protocol: door sensor type
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_door_sensor_type(int door_sensor_type);

/**
 * @brief  set door sensor delay
 * @wiegand_protocol: door sensor delay time
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_door_sensor_delay(int delay_time);

/**
 * @brief  set alarm switch
 * @alarm_switch: alarm switch
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_alarm_switch(int alarm_switch);

/**
 * @brief  get access type
 * @access_time: access type
 *
 * Returns  access type.
 */
int ui_control_get_access_type(void);

/**
 * @brief  set alarm switch
 * @alarm_switch: alarm switch
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_access_type(int access_type);

/**
 * @brief  get access time
 * @access_time: access time
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_get_access_time(char *access_time);

/**
 * @brief  set access time
 * @access_time: access time
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_access_time(char *access_time);

/**
 * ui_control_get_development_status - get development status.
 *
 * Returns development status.
 */
int ui_control_get_development_status(void);

/**
 * @brief  get checkin info
 * @p_info: checkin info
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_get_checkin_info(checkin_param_t *p_info);

/**
 * @brief  set checkin info
 * @p_info: checkin info
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_set_checkin_info(checkin_param_t *p_info);

/**
 * ui_control_get_password - get password of enter main menu.
 * @password: A pointer of password.
 * @pwd_len: length of password buffer
 *
 * Returns  0 success, -1: failure.
 */
int ui_control_get_password(char *password, int pwd_len);

/**
 * ui_control_set_password - set password of enter main menu.
 * @password: password of enter main menu.
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_set_password(const char *password);

/**
 * get_md5_string - get the md5 of src.
 * @src: source chars.
 * @dst: dst chars.
 * @len: the size of dst.
 *
 * Returns 0 if success, otherwise -1.
 */
int get_md5_string(const char *src, char *dst, int len);

/**
 * ui_control_export_rec_get_count - get total count of export record.
 * @password: password of enter main menu.
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_export_rec_get_count(void);

int ui_control_export_by_time_get_count(char *user_id,
	long start, long end);

/**
 * ui_control_export_rec_query_begin - query export record.
 * @index: index of total record.
 * @count: count of current record.
 * @head: a pointer of query result queue
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_export_rec_query_begin(int index, int count, ee_queue_t *head);

int ui_control_export_rec_query_begin_by_id_time(
	int index, int count, ee_queue_t *head, char *user_id,
	long start, long end);

/**
 * ui_control_export_rec_query_end - free query result.
 * @head: a pointer of query result queue.
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_export_rec_query_end(ee_queue_t *head);

/**
 * ui_control_get_export_result - get export result.
 * @p_ctx: a pointer of export result.
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_get_export_result(record_export_ctx_t *p_ctx);

/*
* ui_control_relay_ctrl - set relay status
* @state: relay status
*
* return 0 if success, otherwise return -1
*/
int ui_control_relay_ctrl(RELAY_STATE state);

/**
 * ui_control_wg_write - Write data to device
 * @data_size: IN, wiegand data size
 * @cardid: IN, cardid
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_wg_write(int data_size, uint64_t cardid);

/**
 * ui_control_rs_open - open serial port
 * @dev_name: device name
 * @open_mode: open mode(1/2/3:read only/write only/read and write)
 * @speed: boud rate
 * @bits: data bits
 * @even: even/odd bit
 * @stop: stop bit
 * @_trans_data_notify: a pointer of callback
 * @context: object of context
 *
 * Returns object of device.
 */
void *ui_control_rs_open(const char *dev_name, int open_mode,
				int speed, int bits, char even, int stop,
				cb_trans_data_notify _trans_data_notify,
				void *context);

/**
 * ui_control_rs_close - close serial port
 * @handle: object of device
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_rs_close(void *handle);

/**
 * ui_control_rs_send_data: send data to serial port
 * @handle: object of device
 * @data: data of send
 * &data_len: length of data
 *
 * Returns 0 if success, otherwise -1.
 */
int ui_control_rs_send_data(void *handle, const char *data, int data_len);

#endif /* API_CONTROL_INC_UI_CONTROL_H_ */
