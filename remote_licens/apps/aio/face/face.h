/*
 * face.h - define interface function of face
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
#ifndef _FACE1_H
#define _FACE1_H

#include "db_all.h"
#include "ee_list.h"
#include "face_svc.h"

#define PERSON_NAME_MAX     32
#define PERSON_USERID_MAX   64
#define TEMP_LEFT		150
#define TEMP_TOP		210
#define TEMP_RIGHT		650
#define TEMP_BOTTOM		810

#define STRANGER_STR	" "

typedef enum
{
	RECG_EVENING = 1,	/* 1 */
	RECG_PUNCH_SUCCESSFUL,	/* 2 */
	RECG_WELCOME,	/* 3 */
	RECG_STRANGER,	/* 4 */
	RECG_PROSTHESIS,	/* 5 */
	RECG_TEMP_NORMAL,	/* 6 */
	RECG_TEMP_ABNORMAL,	/* 7 */
	RECG_PASS,	/* 8 */
	RECG_REGISTER,	/* 9 */
	RECG_CONTACT_MANAGER,	/* 10 */
	RECG_WEAR_MASK,	/* 11 */
	RECG_WELCOME_AGAIN,	/* 12 */
	RECG_MORNING,	/* 13 */
	RECG_BEFORE_NOON,	/* 14 */
	RECG_NOON,	/* 15 */
	RECG_AFTER_NOON,	/* 16 */
	RECG_PUNCH_IN_SUCCESSFUL,	/* 17 */
	RECG_HARD_WORK,	/* 18 */
	RECG_PUNCH_OUT_SUCCESSFUL,	/* 19 */
	RECG_PUNCH_REPEATEDLY,	/* 20 */

	RECG_TEMP_PREPARING,	/* 21 */
	RECG_TEMP_MEASURING,	/* 22 */
	RECG_POSE_ERROR,	/* 23 */
} RECG_NOTE;

typedef enum {
	BIT_PROSTHESIS = 0,	/* 0 */
	BIT_STRANGER,	/* 1 */
} stranger_emum;

typedef enum {
	BIT_NAME = 0,	/* 0 */
	BIT_GRETTING,	/* 1 */
} voice_mode_emum;

typedef struct display_control {
	/* Option:
	 *	0: ui don't show name and tts don't play name
	 *	1: ui show name and tts play name
	 */
	int name_display;

	/* Option:
	 *	0: ui show name and tts play name
	 *	1: ui show lase name and tts don't play name
	 */
	int name_format;
} display_control_t;

typedef struct face {
	/* private */
	struct list_head node;

	/* same id for the same person */
	int id;

	/* face position [left, top, right, bottom] */
	int rect[4];

	/* the face position after track and smooth */
	int rect_smooth[4];

	int nir_rect[4];

	/* face quality */
	int quality;

	/* face feature point */
	float kpts[10];

	/* score of feature comparision */
	float score;
	/*comprision pass if 1*/
	int recg;
	int mask;
	int mask_filter;
	int cur_mask;
	int ui_notes[2];
	int max_id;
	int last_temp_state;

	int sound_play;
	int recg_display;
	int open_lock;
	int pass;
	int checkin;
	int wg_out;
	/* living face */
	int living;
	char userid[64];
	char name[PERSON_NAME_MAX];
	char last_recg_name[PERSON_NAME_MAX];
	int last_face_state;
	/* debug info */
	long long det_ts;
	long long recg_ts;
	long long cmp_ts;
	long long living_ts;

	/* for calculate the time of recg and live check */
	long long det_ts_begin;
	long long recg_ts_end;
	long long living_ts_end;
} face_t;

typedef struct faces {
	int face_num;
	face_t *faces;
} faces_t;

typedef struct _user_id {
	char userid[PERSON_USERID_MAX];
} user_id_t;

typedef struct _name {
	char name[PERSON_NAME_MAX];
} name_t;

typedef struct _user_type {
	int user_type;
} user_type_t;

typedef struct userids {
	user_id_t *uids;
	int cnt;
} userid_t;

/*sound for websocket*/
typedef enum {
	sound_bind = 0,
	sound_unbind,
	sound_netlink,
	sound_netdislink
} _websocket_sound_t;

/* -------------- For AX debug ----------------- */
typedef enum {
	FACE_RECORD_EVENT_START,
	FACE_RECORD_EVENT_STOP,
	FACE_RECORD_EVENT_STORAGE_FULL,
} face_record_event_e;

typedef struct face_db {

	int count;
	user_id_t *userid;
	name_t *name;
	feat_t *feat;
	user_type_t *type;
	float *norm;
	kpts_t *kpts;
	long long *last_ts;

} face_db_t;

/**
 * note_cb: notify for ui with recg;
 *
 */
typedef void (*note_cb)(char ret, char *name, int name_sz, float temperature, int *note, int note_sz);

typedef void (*face_cb)(faces_t *, int login_info);

typedef void (*qrcode_cb)(char *data);

/**
 * face_init: initialize face module
 *
 * return 0 if success, otherwise return -1
 */
int face_init(void);

/**
 * face_is_runnig: is the face module had been start
 *
 * return 0 if started, otherwise return -1
 */
int face_is_runnig(void);

/**
 * face_start: start face module
 *
 * return 0 if success, otherwise return -1
 */
int face_start(void);

/**
 * face_set_qrcode_cb: set qrcode function
 * @cb: a pointer of qrcode callback function
 *
 * return 0 if success, otherwise return -1
 */
int face_set_qrcode_cb(qrcode_cb cb);

/**
 * face_set_note_cb: set face recg function
 * @cb: a pointer of face recg callback function
 *
 * return 0 if success, otherwise return -1
 */
int face_set_note_cb(note_cb cb);

/**
 * face_set_cb: set face callback function
 * @cb: a pointer of face callback function
 *
 * return 0 if success, otherwise return -1
 */
int face_set_cb(face_cb cb);

/**
 * face_db_add: notify face module for employee table add
 *
 * return 0 if success, otherwise return -1
 */
int face_db_add(char *user_id);

/**
 * face_db_delete: notify face module for employee table delete
 *
 * return 0 if success, otherwise return -1
 */
int face_db_delete(char *user_id);


/**
 * face_db_update: notify face module for employee table update
 *
 * return 0 if success, otherwise return -1
 */
int face_db_update(void);

/**
 * face_db_update_admin: load admin list from admin table
 *
 * return 0 if success, otherwise return -1
 */
int face_db_update_admin(void);

/**
 * face_db_get_name: get name by index from face memory list
 * @index: index of list
 * @name: a pointer of name
 * @name_size: length of name buffer
 *
 * return 0 if success, otherwise return -1
 */
int face_db_get_name(int index, char *name, int name_size);

/**
 * face_set_recg_param: set face recognition parameter
 * @param: face recognition parameter
 * @param: init flag
 *
 * return 0 if success, otherwise return -1
 */
int face_set_recg_param(face_recognition_param_t *param, int init_flag);

/**
 * face_get_recg_param: get face recognition parameter
 * for dabasebase(sysinfo table)
 * @param: a pointer of face recognition parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_recg_param(face_recognition_param_t *param);

/**
 * face_photo_2_feat: snapshot face picture, and get feature value
 * from face picture.
 * @info_f: a pointer of face feature value parameter
 * @sec: timeout time
 *
 * return 0 if success, otherwise return -1
 */
int face_photo_set_area(int x0, int y0, int x1, int y1);
int face_photo_2_feat(face_info_t *info_f, int sec);
int face_stop_photo_2_feat(void);

/**
 * face_capture_2_feat: snapshot face picture, and get feature value
 * from face picture.
 * @info_f: a pointer of face feature value parameter
 * @cap: parameter of photo
 *
 * return 0 if success, otherwise return -1
 */
int face_capture_2_feat(face_info_t *info_f,
	face_capture_param_t *cap);

/**
 * face_picture_2_feat: get feature value from picture.
 * @path: picture file name
 * @info_f: a pointer of face feature value parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_picture_2_feat(char *path, face_info_t *info_f);

/**
 * face_deinit: deinitialize face module
 *
 * return 0 if success, otherwise return -1
 */
void face_deinit(void);

/**
 * face_stop: stop face module
 *
 * return 0 if success, otherwise return -1
 */
int face_stop(void);

/**
 * face_pause: pause face serivce
 *
 * return 0 if success, otherwise return -1
 */
int face_pause(void);

/**
 * face_resume: resume face serivce
 *
 * return 0 if success, otherwise return -1
 */
int face_resume(void);

/**
 * face_pause_status: get face service status
 *
 * return 1 pause, 0 running
 */
int face_pause_status(void);

/**
 * get_face_handle: get face service handle
 *
 * return face service handle
 */
void *get_face_handle(void);

/**
 * faces_get: get face data from face module
 *
 * return face data
 */
faces_t *faces_get(void);

/**
 * faces_put: free face data to face module
 * @faces: face data
 *
 * return none
 */
void faces_put(faces_t *faces);

/* set login mode */
void face_set_login_mode();

/**
 * face_play_sound: play sound
 * @args: audio file name
 *
 * return 0 if success, otherwise return -1
 */
int face_play_sound(void *args);

/**
 * face_capture_ok_sound_play: play capture ok sound (add to play list)
 *
 * return 0 if success, otherwise return -1
 */
int face_capture_ok_sound_play(void);

/**
 * face_capture_ok_sound_play_block: play capture ok sound (block)
 *
 * return 0 if success, otherwise return -1
 */
int face_capture_ok_sound_play_block(void);

/**
 * websocket_sound_play: play sound
 * @type: audio file type
 *        sound_bind = 0,
 *        sound_unbind,
 *        sound_netlink,
 *        sound_netdislink
 *
 * return 0 if success, otherwise return -1
 */
int websocket_sound_play(_websocket_sound_t type);

/* record callback function */
typedef void (*face_rec_cb)(face_record_event_e event, void *args);

/**
 * face_record_try: record start
 *
 * return none
 */
void face_record_try(void);

/**
 * face_record_stop_try: record stop
 *
 * return none
 */
void face_record_stop_try(void);

/**
 * face_snapshot: snapshot picture
 * @cam_id: camera id (0/1: rgb/nir)
 *
 * return 0 if success, otherwise return -1
 */
int face_snapshot(int cam_id, char *file_name);

/**
 * face_get_ord_map_params: get camera ord map parameter
 * @ord_map_params: a pointer of ord map parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_ord_map_params(map_params_t *ord_map_params);

/**
 * face_get_mod_map_params: get camera mod map parameter
 * @ord_map_params: ord map parameter
 * @mod_map_params: a pointer of mod map parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_mod_map_params(map_params_t *ord_map_params,
			map_params_t *mod_map_params);

/**
 * face_get_map_params: get camera map parameter
 * @map_params: a pointer of map parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_map_params(map_params_t *map_params);

int face_take_mod_pic(int x, int y, int w, int h);

int face_cal_mod_map(map_params_t *map);

/**
 * face_get_identify_threshold_score: get identify threshold score
 * @score: a pointer of identify threshold score
 *
 * return 0 if success, otherwise return -1
 */
int face_get_identify_threshold_score(float *score);

float face_recg_mask_threshold(void);

/**
 * face_get_test_log: get test log parameter
 * @test_log: a pointer of int parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_test_log_param(int *test_log);

/**
 * face_set_test_log: set test log parameter
 * @test_log: int parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_set_test_log_param(int test_log);

/**
 * set_dump_img_config: set attendance image parameter
 *
 */
void set_dump_img_config(void);

/**
 * set_gate_dump_img_config: set gate image parameter
 *
 */
void set_gate_dump_img_config(void);

/**
 * set_access_time: set access time
 *
 * return 0 if success, otherwise return -1
 */
int set_access_time(char *access_time);

/**
 * set_access_type: set access type
 *
 * return 0 if success, otherwise return -1
 */
int set_access_type(int access_type);

/**
 * set_language: set language
 *
 * return 0 if success, otherwise return -1
 */
int set_language(device_lang_param_t *info);

/**
 * set_display_control: set display control
 *
 * return 0 if success, otherwise return -1
 */
int set_display_control(display_control_t *p_info);

/**
 * face_set_platform_param: set platform parameter
 * @enable: a pointer of int parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_set_platform_param(int enable);

/**
 * face_set_platform_hearbeat: set platform hearbeat
 *
 */
void face_set_platform_hearbeat(void);

int face_get_recg_timeout(void);

/**
 * face_set_register_mode: set register mode parameter
 * @param: register mode parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_set_register_mode(int param);

int ui_get_userid_max();

#endif /* _FACE1_H */

