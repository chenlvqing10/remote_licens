/*
 * face.c - define interface function of face
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

#include <errno.h>
#include <sys/prctl.h>
#include <sys/queue.h>
#include <string.h>
#include <dirent.h>
#define LOG_TAG	"api-face"
#include <log/log.h>

#include "face.h"
#include "face_svc.h"
#include "file_ops.h"
#include "lombo_malloc.h"
#include "face_svc_isp.h"
#include "face_svc_media.h"
#include "db_all.h"
#include "screen_svc.h"
#include "light_svc.h"
#include "lombo_system_sound.h"
#include "jpeg_dec.h"
#include "time_conv.h"
#include "ws_svc_main.h"
#include "video_stream.h"
#include "audio_stream.h"
#include "video_recorder.h"
#include "storage.h"
#include "time_conv.h"
#include "system_private.h"
#include "sound_svc.h"
#include "open_door_svc.h"
#include "test_log_svc.h"
#include "http_main.h"
#include "wiegand_svc.h"

#define FACE_DEBUG 0

/*#define FACE_RECORD_PHOTO_PATH	"/data/face_record/"*/
#define FACE_RECORD_PATH    "/data/face_record"
#define FACE_CAPTURE_PATH   "/data/face_capture"
#define FACE_RECORD_FREE_THRESHOLD  300
#define MASK_FILTER	10

#if ATTENDANCE_MODE == 2
/* do nothings */
#else
#define OPEN_LOCK
#define WG_OUT
#endif

#define PASS_SOUND_PATH  "/usr/share/aio/res/sound/pass.wav"
#define CAPTURE_OK_SOUND_PATH  "/usr/share/aio/res/sound/capture_ok.wav"

/*sound for websocket*/
#define BIND_SOUND_PATH  "/usr/share/aio/res/sound/bind.wav"
#define UNBIND_SOUND_PATH  "/usr/share/aio/res/sound/unbind.wav"
#define NET_LINK_SOUND_PATH  "/usr/share/aio/res/sound/net_link.wav"
#define NET_DISLINK_SOUND_PATH  "/usr/share/aio/res/sound/net_dislink.wav"

#define RGBNIR_MAP_CHECK  0x5B
#define RGBNIR_MAP_OFFSET  0x1024

#define MAX_CHECKIN_LIMIT_COUNT 100000
#define MMC_DATA_DISK_PATH "/data"
#define MAX_LIMIT_USE_RATIO_PERCENT 90

#define SET_BIT(x, y) (x |= (1u << y))
#define GET_BIT(x, y) (x >> y & 1)
#define CLK_BIT(x, y)	(x &= ~(1u << y))

typedef enum {
	EMPLOYEE = 0,
	VISITOR,
	STRANGER,
} user_type_em;

typedef enum {
	PLAY_NAME = 1,
	PLAY_TEMP,
	PLAY_RET,
	PLAY_MASK,
	PLAY_CONTACT_MANAGER,
	PLAY_REGISTER,
	PLAY_GRETTING,
	PLAY_STRANGER,
	PLAY_PROSTHESIS,
} enum_play_t;

typedef enum {
	DISPLAY_NAME = 1,
	DISPLAY_STRANGER,
	DISPLAY_TEMP_PREPARING,
	DISPLAY_TEMP_MEASURING,
	DISPLAY_POSE_ERROR,
} enum_display_t;

typedef struct face_map_params {
	int check;
	double x_map[5];
	double y_map[5];
} face_map_params_t;

typedef struct {
	user_id_t *user;
	int num;
} admins_t;

typedef int (*face_task_func)(void *);

typedef struct face_task {
	struct list_head node;
	face_task_func func;
	void *args;
} face_task_t;

typedef struct _api_face {

	void *svc_hdl;
	face_cb cb;
	note_cb note_cb;
	qrcode_cb qr_cb;
	struct list_head face_list;
	pthread_mutex_t face_lock;
	int face_num;
	int cb_flag;

	/*for set photo area*/
	int point_lt[2];
	int point_rb[2];

	pthread_mutex_t db_lock;
	face_db_t *face_db;
	int db_version;
	face_recognition_param_t recg_param;

	/* Face task thread */
	pthread_mutex_t task_lock;
	pthread_cond_t task_cond;
	pthread_t face_task_thrd;
	struct list_head task_list;

	/*check in data*/
	pthread_mutex_t checkin_lock;
	pthread_cond_t checkin_cond;
	pthread_t checkin_thrd;
	pthread_t storage_thrd;

	/* face record */
	int record_enable;
	int record_running;
	long long record_start_ts;
	long long record_delay;
	face_rec_cb record_cb;
	int record_width;
	int record_height;

	/*login mode*/
	int login_mode;
	admins_t admins;

	/*attendance mode*/
	attendance_param_t attendance_info;

	/* checkin mode */
	checkin_param_t checkin_info;

	/* test_log_enable */
	int test_log_enable;

	/* access time */
	char access_time[128];
	char access_type;

	/* language */
	device_lang_param_t lang_t;

	display_control_t display_ctl;

	/* 0: face identify mode; 1: face register mode */
	int face_register;

	userid_t uid;
	int uid_max;

	/* platformEnable */
	int platform_enable;
	/* platformIp */
	char platform_ip[64];
	long long platform_hearbeat;
} api_face_t;

typedef struct _vs_record {
	int width;
	int height;
	void *astream;
	void *vstream;
	void *recorder;
} vs_record;

api_face_t *api_face;

/**
 * get_face_handle: get face service handle
 *
 * return face service handle
 */
void *get_face_handle(void)
{
	return api_face->svc_hdl;
}

int face_photo_set_area(int x0, int y0, int x1, int y1)
{
	if (api_face) {
		ALOGD("%s:x0=%d  y0=%d x1=%d y1=%d\n", __func__,
			  x0, y0, x1, y1);
		api_face->point_lt[0] = x0;
		api_face->point_lt[1] = y0;
		api_face->point_rb[0] = x1;
		api_face->point_rb[1] = y1;
		return 0;
	}
	return -1;
}
/**
 * face_play_sound: play sound
 * @args: audio file name
 *
 * return 0 if success, otherwise return -1
 */
int face_play_sound(void *args)
{
	int ret;

	ret = lombo_system_sound_play((const char *)args);
	if (ret)
		ALOGE("failed to play sound:%s", (const char *)args);

	return ret;
}

/**
 * face_checkin_insert: insert checkin record
 * @args: checkin record parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_checkin_insert(void *args)
{
	int ret = 0;

	web_socket_checkin_info_t *person = NULL;
	if (NULL == args) {
		ALOGE("face_checkin_insert args null\n");
		return ret;
	}
	person = (web_socket_checkin_info_t *)args;
	ALOGD("%s:%d >> %s %d\n", __func__, __LINE__,
		person->user_id, person->timestamp);
	ret = db_websocket_checkin_info_insert(person);
	if (0 >= ret) {
		ALOGW("%s:%d >> insert checkin failed!", __func__, __LINE__);
		file_remove(person->face_path);
		file_remove(person->frame_path);
	}
	free(person);
	person = NULL;
	pthread_mutex_lock(&api_face->checkin_lock);
	pthread_cond_signal(&api_face->checkin_cond);
	pthread_mutex_unlock(&api_face->checkin_lock);

	return ret;
}

/**
 * face_task_add: add func to task list
 * @func: a pointer of function
 * @args: checkin record parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_task_add(face_task_func func, void *args)
{
	face_task_t *task;

	task = (face_task_t *)malloc(sizeof(face_task_t));
	if (task) {
		task->func = func;
		task->args = args;
		pthread_mutex_lock(&api_face->task_lock);
		list_add(&task->node, &api_face->task_list);
		pthread_cond_signal(&api_face->task_cond);
		pthread_mutex_unlock(&api_face->task_lock);
		return 0;
	}

	return -1;
}

/**
 * face_task_thread: process task list thread
 * @args: thread parameter
 *
 * return NULL
 */
static void *face_task_thread(void *args)
{
	face_task_t *task;
	int ret;

	ALOGD("face task thread");

	prctl(PR_SET_NAME, "face-task");

	while (1) {
		pthread_mutex_lock(&api_face->task_lock);
		if (list_empty(&api_face->task_list))
			pthread_cond_wait(&api_face->task_cond, &api_face->task_lock);
		task = list_first_entry(&api_face->task_list, face_task_t, node);
		list_del(&task->node);
		pthread_mutex_unlock(&api_face->task_lock);

		if (!task) {
			ALOGE("task == NULL, it should not happen");
			continue;
		}

		ret = task->func(task->args);
		if (ret)
			ALOGE("do task failed");

		free(task);
	}

	return NULL;
}

/**
 * face_task_init: initialize task list thread
 *
 * return 0 if success, otherwise return -1
 */
int face_task_init(void)
{
	if (NULL == api_face) {
		ALOGE("api_face == NULL");
		return -1;
	}

	INIT_LIST_HEAD(&api_face->task_list);
	if (pthread_mutex_init(&api_face->task_lock, NULL)) {
		ALOGE("init sound lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	if (pthread_cond_init(&api_face->task_cond, NULL)) {
		ALOGE("init sound cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	if (pthread_create(&api_face->face_task_thrd, NULL,
				face_task_thread, (void *)api_face)) {
		ALOGE("create face sound play thread failed, errno: %d (%s)",
			errno, strerror(errno));
		return -1;
	}

	return 0;
}

/* set login mode */
void face_set_login_mode(void)
{
	if (api_face->login_mode == 0)
		api_face->login_mode = 1;
}

/**
 * checkin_thread: check checkin record thread
 * @args: thread parameter
 *
 * return NULL
 */
 #include "web_biz_api.h"
static void *checkin_thread(void *args)
{
	struct timeval now;
	struct timespec time_spec;
	prctl(PR_SET_NAME, "checkin_thread");

	while (1) {
		/* Check record in db every 10 second */
		gettimeofday(&now, NULL);
		time_spec.tv_sec = now.tv_sec + 10;
		time_spec.tv_nsec = now.tv_usec * 1000;
		pthread_mutex_lock(&api_face->checkin_lock);
		pthread_cond_timedwait(&api_face->checkin_cond,
					&api_face->checkin_lock, &time_spec);
		pthread_mutex_unlock(&api_face->checkin_lock);
#if ATTENDANCE_MODE > 0
		ws_app_checkin();
#else
		long long living = time_get_us() - api_face->platform_hearbeat;
		if (api_face->platform_enable && living < 3000000)
			http_send_checkin(api_face->platform_ip);
#endif
	}

	return NULL;
}

/**
 * storage_thread: check storage thread
 * @args: thread parameter
 *
 * return NULL
 */
static void *storage_thread(void *args)
{
	int count = 0;
	int ret = 0;
	prctl(PR_SET_NAME, "storage_thread");

	while (1) {
		/* Check storage in db every 10 second */
		count = db_websocket_checkin_info_get_count();
		ret = count - MAX_CHECKIN_LIMIT_COUNT;
		if (ret > 0) {
			ALOGD("checkin info is out of limit count");
			db_websocket_checkin_info_del_by_last(ret);
			file_system_sync();
		}
		ret = get_disk_use_ratio_round_up_percent(MMC_DATA_DISK_PATH);
		ALOGD("[%s:%d]percent=%d, free_percent=%d\n", __func__, __LINE__,
			ret, (100-ret));
		while (ret >= MAX_LIMIT_USE_RATIO_PERCENT) {
			count = db_websocket_checkin_info_get_count();
			if (count - 100 > 500) {
				db_websocket_checkin_info_del_by_last(100);
				file_system_sync();
			} else if (count > 500) {
				db_websocket_checkin_info_del_by_last(count - 500);
				file_system_sync();
				break;
			} else
				break;
			ret = get_disk_use_ratio_round_up_percent(MMC_DATA_DISK_PATH);
			ALOGD("[%s:%d]percent=%d, free_percent=%d\n", __func__, __LINE__,
				ret, (100-ret));
			sleep(1);
		}
		sleep(10);
	}

	return NULL;
}

/**
 * face_db_get_name: get name by index from face memory list
 * @index: index of list
 * @name: a pointer of name
 * @name_size: length of name buffer
 *
 * return 0 if success, otherwise return -1
 */
int face_db_get_name(int index, char *name, int name_size)
{
	face_db_t *face_db;

	if (!name || (NULL == api_face)) {
		ALOGE("api_face == NULL");
		return -1;
	}

	pthread_mutex_lock(&api_face->db_lock);
	face_db = api_face->face_db;
	if (!face_db) {
		pthread_mutex_unlock(&api_face->db_lock);
		return -1;
	}

	if (index >= face_db->count) {
		pthread_mutex_unlock(&api_face->db_lock);
		return -1;
	}

	strncpy(name, face_db->name[index].name, name_size);
	pthread_mutex_unlock(&api_face->db_lock);

	return 0;
}

void mask_filer(face_t *face, face_rect_t *p_rect)
{
#if 0
	if (p_rect->attr.mask) {
		face->mask_filter = 0;
		face->cur_mask = 1;
	} else {
		if (face->mask_filter < MASK_FILTER)
			face->mask_filter++;
		else
			face->cur_mask = 0;
	}
#endif
	face->cur_mask = p_rect->attr.mask;
}

int get_cur_gretting(void)
{
	int ret = 0;
	time_t seconds_time = 0;
	struct tm *ptm = NULL;

	if (api_face->recg_param.grettings == 0)
		ret = (int) RECG_PASS;
	else if (api_face->recg_param.grettings == 1) {
		if (api_face->access_type == 0)
			ret = (int)RECG_WELCOME;
		else
			ret = (int)RECG_WELCOME_AGAIN;
	} else {
		time(&seconds_time);
		ptm = localtime(&seconds_time);
		if (NULL == ptm)
			return -1;
		if (ptm->tm_hour >= 0 && ptm->tm_hour < 8)
			ret = (int) RECG_MORNING;
		else if (ptm->tm_hour >= 8 && ptm->tm_hour < 11)
			ret = (int) RECG_BEFORE_NOON;
		else if (ptm->tm_hour >= 11 && ptm->tm_hour < 13)
			ret = (int) RECG_NOON;
		else if (ptm->tm_hour >= 13 && ptm->tm_hour < 18)
			ret = (int) RECG_AFTER_NOON;
		else if (ptm->tm_hour >= 18 && ptm->tm_hour < 24)
			ret = (int) RECG_EVENING;
	}

	return ret;
}

#ifdef OPEN_LOCK
/**
 * is_access_time: check if access time
 *
 * return 1 if success, otherwise return 0
 */
static int is_access_time()
{
	char current_time[64];
	char s_current_time[16];
	char access_time_str[128];
	char *tmp_period = NULL;
	int ret = 0;

	memset(current_time, 0, 64);
	memset(s_current_time, 0, 16);
	memset(access_time_str, 0, 128);
	strncpy(access_time_str, api_face->access_time, 128);

	if (strcmp(access_time_str, "0/0/0") == 0)
		return 1;

	time_get_cur_time(current_time);
	strncpy(s_current_time, current_time, 5);

	tmp_period = strtok(access_time_str, "--/");
	while (tmp_period != NULL) {
		if (strcmp(tmp_period, "0") == 0) {
			tmp_period = strtok(NULL, "--/");
			continue;
		}

		if (strcmp(s_current_time, tmp_period) > 0) {
			tmp_period = strtok(NULL, "--/");
			if (strcmp(s_current_time, tmp_period) < 0) {
				ret = 1;
				break;
			}
		}
		tmp_period = strtok(NULL, "--/");
	}

	return ret;
}
#endif

void face_temp_measuring(face_t *face, face_rect_t *p_rect)
{
	if (p_rect->temp_status == TEMP_STATUS_PREPARING) {
		face->ui_notes[0] = 0;
		face->ui_notes[1] = RECG_TEMP_PREPARING;
		ALOGD("[%s][%d] temp preparing", __func__, __LINE__);
		face->last_temp_state = TEMP_STATUS_PREPARING;
		if (api_face->note_cb)
			api_face->note_cb(1, NULL, 0, 0, face->ui_notes, 2);
	} else if (p_rect->temp_status == TEMP_STATUS_MEASURING) {
		face->ui_notes[0] = 0;
		face->ui_notes[1] = RECG_TEMP_MEASURING;
		ALOGD("[%s][%d] temp measuring", __func__, __LINE__);
		face->last_temp_state = TEMP_STATUS_MEASURING;
		if (api_face->note_cb)
			api_face->note_cb(1, NULL, 0, 0, face->ui_notes, 2);
	}
}

int is_same_result(face_t *face, face_rect_t *p_rect)
{
	if (GET_BIT(p_rect->face_event, FACE_STATE_TEMP_STATUS_DONE) ||
			GET_BIT(p_rect->face_event, FACE_STATE_RECG_MASK) ||
			GET_BIT(p_rect->face_event, FACE_STATE_DETECT_PASS))
		return 1;

	/* stranger */
	if (api_face->recg_param.recg_enable &&
			GET_BIT(p_rect->face_state, FACE_STATE_RECG_REFUSE)) {
		memset(face->name, 0 ,PERSON_NAME_MAX);
		strcpy(face->name, STRANGER_STR);
	}

	ALOGD("last_living:%d, cur_living:%d, last_living_timeout:%d, cur_living_timeout:%d,"
		"*%s* *%s*, 0x%x, 0x%x",
		GET_BIT(face->last_face_state, FACE_STATE_LIVING_BODY),
		GET_BIT(p_rect->face_state, FACE_STATE_LIVING_BODY),
		GET_BIT(face->last_face_state, FACE_STATE_LIVING_RECG_TIMEOUT),
		GET_BIT(p_rect->face_state, FACE_STATE_LIVING_RECG_TIMEOUT),
		face->last_recg_name,
		face->name,
		face->last_face_state,
		p_rect->face_state);

	if (GET_BIT(face->last_face_state, FACE_STATE_LIVING_BODY) ==
			GET_BIT(p_rect->face_state, FACE_STATE_LIVING_BODY) &&
			GET_BIT(face->last_face_state, FACE_STATE_LIVING_RECG_TIMEOUT) ==
			GET_BIT(p_rect->face_state, FACE_STATE_LIVING_RECG_TIMEOUT) &&
			strcmp(face->last_recg_name, face->name) == 0)
		return 0;

	return 1;
}

void face_living_pass(face_t *face, face_rect_t *p_rect)
{
	int flag  = 0;

	if(!is_same_result(face, p_rect))
		return;

	/* stranger */
	if (api_face->recg_param.recg_enable &&
		GET_BIT(p_rect->face_state, FACE_STATE_RECG_REFUSE)) {
		if (GET_BIT(api_face->recg_param.stranger_voice, BIT_STRANGER)) {
			ALOGD("[%s][%d] stranger", __func__, __LINE__);
			tts_data_t tts;
			memset(&tts, 0 ,sizeof(tts_data_t));
			tts.format = TTS_FORMAT_UTF8;
			tts.greet_param.language_type = api_face->lang_t.lang;
			tts.greet_id = (int)RECG_STRANGER - 1;
			tts.greet_param.pos = GREETING_END;
			sound_play_tts(&tts);
		}
		flag++;
		memset(face->name, 0, PERSON_NAME_MAX);
		strcpy(face->name, STRANGER_STR);
	}

	/* name */
	if (api_face->recg_param.recg_enable &&
		GET_BIT(p_rect->face_state, FACE_STATE_RECG_PASS)) {
		if (GET_BIT(api_face->recg_param.voice_mode, BIT_NAME) &&
			api_face->display_ctl.name_display && !api_face->display_ctl.name_format) {
			ALOGD("[%s][%d] name: %s id: %d", __func__, __LINE__, face->name, face->id);
			tts_data_t tts;
			memset(&tts, 0 ,sizeof(tts_data_t));
			tts.string = face->name;
			tts.len = strlen(face->name);
			tts.format = TTS_FORMAT_UTF8;
			tts.greet_param.language_type = api_face->lang_t.lang;
			tts.greet_id = 0;
			tts.greet_param.pos = GREETING_CLOSE;
			sound_play_tts(&tts);
		}
		if (api_face->display_ctl.name_display)
			flag++;
	}

	/* temperature */
	if (api_face->recg_param.body_temperature_detect) {
		if (p_rect->temp_status == TEMP_STATUS_DONE) {
			p_rect->temp_measure.value = p_rect->temp_measure.value +
					api_face->recg_param.temperature_compensation;
			if (p_rect->temp_measure.value >=
					api_face->recg_param.alarm_temperature ||
					p_rect->temp_measure.value <= 34.0f) {
				ALOGD("[%s][%d] temperature abnormal", __func__, __LINE__);
				tts_data_t tts;
				memset(&tts, 0 ,sizeof(tts_data_t));
				tts.format = TTS_FORMAT_UTF8;
				tts.greet_param.language_type = api_face->lang_t.lang;
				tts.greet_id = RECG_TEMP_ABNORMAL - 1;
				tts.greet_param.pos = GREETING_END;
				sound_play_tts(&tts);

				flag++;
				face->ui_notes[0] = RECG_TEMP_ABNORMAL;
				face->ui_notes[1] = 0;
			} else {
				ALOGD("[%s][%d] temperature normal", __func__, __LINE__);
				tts_data_t tts;
				memset(&tts, 0 ,sizeof(tts_data_t));
				tts.format = TTS_FORMAT_UTF8;
				tts.greet_param.language_type = api_face->lang_t.lang;
				tts.greet_id = RECG_TEMP_NORMAL - 1;
				tts.greet_param.pos = GREETING_END;
				sound_play_tts(&tts);

				flag++;
				face->ui_notes[0] = RECG_TEMP_NORMAL;
				face->ui_notes[1] = 0;
			}
		}
	}

	/* no mask */
	if (api_face->recg_param.mask_detect_enable && !face->cur_mask) {
		ALOGD("[%s][%d] please wear the mask %d name %s id %d",
			__func__, __LINE__, face->cur_mask, face->name, face->id);
		tts_data_t tts;
		memset(&tts, 0 ,sizeof(tts_data_t));
		tts.format = TTS_FORMAT_UTF8;
		tts.greet_param.language_type = api_face->lang_t.lang;
		tts.greet_id = RECG_WEAR_MASK - 1;
		tts.greet_param.pos = GREETING_END;
		sound_play_tts(&tts);

		flag++;
		face->ui_notes[1] = RECG_WEAR_MASK;
	} else {
		/* contact manager */
		if (RECG_TEMP_ABNORMAL == face->ui_notes[0]) {
			// Todo: play contact manager

			flag++;
			face->ui_notes[1] = RECG_CONTACT_MANAGER;
		} else if (!api_face->recg_param.body_temperature_detect ||
			p_rect->temp_status == TEMP_STATUS_DONE) {
			if (api_face->recg_param.recg_enable && GET_BIT(p_rect->face_state, FACE_STATE_RECG_REFUSE)) {
				ALOGD("[%s][%d] please register name %s id %d",
					__func__, __LINE__, face->name, face->id);
				tts_data_t tts;
				memset(&tts, 0 ,sizeof(tts_data_t));
				tts.format = TTS_FORMAT_UTF8;
				tts.greet_param.language_type = api_face->lang_t.lang;
				tts.greet_id = RECG_REGISTER - 1;
				tts.greet_param.pos = GREETING_END;
				sound_play_tts(&tts);

				flag++;
				face->ui_notes[1] = RECG_REGISTER;
			} else {
				if (!api_face->recg_param.recg_enable || GET_BIT(p_rect->face_state, FACE_STATE_RECG_PASS)) {
					SET_BIT(face->sound_play, PLAY_GRETTING);

					if (GET_BIT(api_face->recg_param.voice_mode, BIT_GRETTING)) {
						ALOGD("[%s][%d] please pass name %s id %d",
							__func__, __LINE__, face->name, face->id);
						tts_data_t tts;
						memset(&tts, 0 ,sizeof(tts_data_t));
						tts.format = TTS_FORMAT_UTF8;
						tts.greet_param.language_type = api_face->lang_t.lang;
						tts.greet_id = get_cur_gretting() - 1;
						tts.greet_param.pos = GREETING_END;
						sound_play_tts(&tts);
					}

					flag++;
					face->ui_notes[1] = get_cur_gretting();
				}
			}
		}
	}

	if (flag > 0 && api_face->note_cb) {
		api_face->note_cb(GET_BIT(face->sound_play, PLAY_GRETTING),
		face->name, PERSON_NAME_MAX, p_rect->temp_measure.value,
		face->ui_notes, 2);
	}
}

void face_obj_pass(face_t *face, face_rect_t *p_rect)
{
	int flag  = 0;

	if(!is_same_result(face, p_rect))
		return;

	/* stranger */
	if (api_face->recg_param.recg_enable &&
		GET_BIT(p_rect->face_state, FACE_STATE_RECG_REFUSE)) {
		ALOGD("[%s][%d] stranger", __func__, __LINE__);
		flag++;
		memset(face->name, 0 ,PERSON_NAME_MAX);
		strcpy(face->name, STRANGER_STR);
	}

	/* name */
	if (api_face->recg_param.recg_enable &&
		GET_BIT(p_rect->face_state, FACE_STATE_RECG_PASS)) {
		ALOGD("[%s][%d] name", __func__, __LINE__);
		if (api_face->display_ctl.name_display)
			flag++;
	}

	/* living fail */
	if (GET_BIT(api_face->recg_param.stranger_voice, BIT_PROSTHESIS)) {
		ALOGD("[%s][%d] prosthesis", __func__, __LINE__);
		tts_data_t tts;
		memset(&tts, 0 ,sizeof(tts_data_t));
		tts.format = TTS_FORMAT_UTF8;
		tts.greet_id = (int)RECG_PROSTHESIS - 1;
		tts.greet_param.language_type = api_face->lang_t.lang;
		tts.greet_param.pos = GREETING_END;
		sound_play_tts(&tts);
	}
	flag++;
	face->ui_notes[0] = 0;
	face->ui_notes[1] = RECG_PROSTHESIS;

	if (flag > 0 && api_face->note_cb)
		api_face->note_cb(0, face->name, PERSON_NAME_MAX, 0,
		face->ui_notes, 2);
}

/**
 * face_update: update face info
 * @api_face: a pointer of face service
 * @f_rects: a pointer of face rects
 * @frame: face frame
 *
 * return 0 if success, otherwise return -1
 */
static void face_update(api_face_t *api_face, face_rects_t *f_rects,
						face_frame_t *frame)
{
	face_t *face, *face_tmp;
	int i;

	/* face register mode */
	if (api_face->face_register)
		return;

	/* no face is detected, clean face list */
	if (f_rects->cnt == 0) {
		if (api_face->face_num && api_face->note_cb)
			api_face->note_cb(2, NULL, 0, 0, NULL, 0);
        sound_stop_all();
		pthread_mutex_lock(&api_face->face_lock);
		list_for_each_entry_safe(face, face_tmp, &api_face->face_list, node) {
			ALOGD_IF(FACE_DEBUG, "clean face[%d]", face->id);
			list_del(&face->node);
			free(face);
			api_face->face_num--;
		}
		pthread_mutex_unlock(&api_face->face_lock);
		return;
	}
	/* detete the face which was dismissed */
	pthread_mutex_lock(&api_face->face_lock);
	list_for_each_entry_safe(face, face_tmp, &api_face->face_list, node) {
		for (i = 0; i < f_rects->cnt; i++) {
			if (f_rects->p_rect[i].id == face->id)
				break;
		}

		/* face was dismissed */
		if (i >= f_rects->cnt) {
			ALOGD_IF(FACE_DEBUG, "delete face[%d]", face->id);
			list_del(&face->node);
			free(face);
			api_face->face_num--;
		}
	}
	pthread_mutex_unlock(&api_face->face_lock);

	/* add new face to list */
	pthread_mutex_lock(&api_face->face_lock);

	for (i = 0; i < f_rects->cnt; i++) {
		face = NULL;
		list_for_each_entry(face_tmp, &api_face->face_list, node) {
			if (f_rects->p_rect[i].id == face_tmp->id) {
				ALOGD_IF(FACE_DEBUG, "find face[%d]", face_tmp->id);
				face = face_tmp;
				break;
			}
		}

		/* face not found */
		if (face == NULL) {
			ALOGD_IF(FACE_DEBUG, "face[%d] not found, add face",
					 f_rects->p_rect[i].id);
			face = malloc(sizeof(face_t));

			if (!face) {
				ALOGD_IF(FACE_DEBUG, "alloc face_t failed");
				pthread_mutex_unlock(&api_face->face_lock);
				return;
			}
			memset(face, 0, sizeof(face_t));
			face->id = f_rects->p_rect[i].id;
			list_add(&face->node, &api_face->face_list);
			api_face->face_num++;

			/* for debug */
			face->det_ts_begin = time_get_us() -
				(frame->det_end - frame->det_begin);
		}
		/* update face info */
		face->quality = f_rects->p_rect[i].quality_score;
		memcpy(face->kpts, f_rects->p_rect[i].kpts, sizeof(face->kpts));
		memcpy(face->rect, f_rects->p_rect[i].rect, sizeof(face->rect));
		memcpy(face->rect_smooth, f_rects->p_rect[i].rect_smooth,
			sizeof(face->rect_smooth));
		/* memset(face->nir_rect, 0, sizeof(face->nir_rect)); */

#if ATTENDANCE_MODE == 0
		mask_filer(face, &f_rects->p_rect[i]);

		if (f_rects->p_rect[i].face_event) {
			ALOGD("api face_event: 0x%x", f_rects->p_rect[i].face_event);

			if (api_face->recg_param.mask_detect_enable &&
				GET_BIT(f_rects->p_rect[i].face_event, FACE_STATE_RECG_MASK)) {
				CLK_BIT(face->sound_play, PLAY_GRETTING);
				ALOGD("[%s][%d] CLK_BIT last_mask %d cur_mask %d",
					__func__, __LINE__, face->mask, face->cur_mask);
			}

			if (api_face->recg_param.body_temperature_detect &&
				face->last_temp_state != f_rects->p_rect[i].temp_status) {
				CLK_BIT(face->recg_display, DISPLAY_TEMP_PREPARING);
				CLK_BIT(face->recg_display, DISPLAY_TEMP_MEASURING);
				ALOGD("[%s][%d] CLK_BIT last_temp %d cur_temp %d",
					__func__, __LINE__, face->last_temp_state, f_rects->p_rect[i].temp_status);
			}

			if (GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_DETECT_TIMEOUT)) {
				ALOGD("[%s][%d] pose error", __func__, __LINE__);

				face->ui_notes[0] = 0;
				face->ui_notes[1] = RECG_POSE_ERROR;
				if (api_face->note_cb)
					api_face->note_cb(1, NULL, 0, 0, face->ui_notes, 2);
			} else {
				if (api_face->recg_param.body_temperature_detect &&
					f_rects->p_rect[i].temp_status != TEMP_STATUS_DONE) {
					face_temp_measuring(face, &f_rects->p_rect[i]);
				} else {
					ALOGD("api face_state: 0x%x", f_rects->p_rect[i].face_state);

					if ((!api_face->recg_param.nir_live_detect ||
						GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_LIVING_BODY) ||
						GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_LIVING_RECG_TIMEOUT)) &&
						(!api_face->recg_param.recg_enable ||
						GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_RECG_REFUSE) ||
						GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_RECG_PASS))) {

						if (!api_face->recg_param.nir_live_detect ||
							GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_LIVING_BODY))
							face_living_pass(face, &f_rects->p_rect[i]);
						else if (GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_LIVING_RECG_TIMEOUT))
							face_obj_pass(face, &f_rects->p_rect[i]);

					}
				}
			}
			face->mask = face->cur_mask;
			face->last_temp_state = f_rects->p_rect[i].temp_status;
			if (GET_BIT(f_rects->p_rect[i].face_event, FACE_STATE_LIVING_BODY) ||
					GET_BIT(f_rects->p_rect[i].face_event, FACE_STATE_LIVING_RECG_TIMEOUT)) {
				CLK_BIT(face->last_face_state,FACE_STATE_LIVING_BODY);
				CLK_BIT(face->last_face_state,FACE_STATE_LIVING_RECG_TIMEOUT);
				face->last_face_state |= GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_LIVING_BODY)
					<< FACE_STATE_LIVING_BODY;
				face->last_face_state |= GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_LIVING_RECG_TIMEOUT)
					<< FACE_STATE_LIVING_RECG_TIMEOUT;
			}
			if (GET_BIT(f_rects->p_rect[i].face_event, FACE_STATE_RECG_PASS) ||
					GET_BIT(f_rects->p_rect[i].face_event, FACE_STATE_RECG_REFUSE)) {
				CLK_BIT(face->last_face_state,FACE_STATE_RECG_PASS);
				CLK_BIT(face->last_face_state,FACE_STATE_RECG_REFUSE);
				face->last_face_state |= GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_RECG_PASS)
					<< FACE_STATE_RECG_PASS;
				face->last_face_state |= GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_RECG_REFUSE)
					<< FACE_STATE_RECG_REFUSE;
			}
			strcpy(face->last_recg_name, face->name);
			if (GET_BIT(f_rects->p_rect[i].face_state, FACE_STATE_DETECT_TIMEOUT)) {
				face->last_face_state = 0;
				strcpy(face->name, "");
				strcpy(face->last_recg_name, "");
			}
			ALOGD("last_face_state: 0x%x, face_state: 0x%x, last_recg_name: *%s*",
				face->last_face_state, f_rects->p_rect[i].face_state,face->last_recg_name);
		}

#ifdef OPEN_LOCK
		if (!face->open_lock && GET_BIT(face->sound_play, PLAY_GRETTING)) {
			if (is_access_time()) {
				face->open_lock = 1;
				open_door();
				ALOGD("open_door()");
			} else {
				ALOGD("non-traffic time");
			}
		}
#endif
#ifdef WG_OUT
		if (!face->wg_out && GET_BIT(face->sound_play, PLAY_GRETTING)) {
			face->wg_out = 1;
			wiegand_write_svc(atoll(face->userid));
		}
#endif
		CLK_BIT(face->sound_play, PLAY_GRETTING);
#endif
	}

	pthread_mutex_unlock(&api_face->face_lock);
}

/**
 * is_admin: check if admin
 * @userid: current user
 *
 * return 0 if success, otherwise return -1
 */
static int is_admin(char *userid)
{
	int i = 0;

	if (0 == api_face->admins.num)
		api_face->login_mode = 2;
	for (i = 0; i < api_face->admins.num; i++) {
		if (0 == strcmp(userid, api_face->admins.user[i].userid)) {

			api_face->login_mode = 2;
			break;
		}
	}

	return 0;
}

/**
 * set_access_time: set access time
 *
 * return 0 if success, otherwise return -1
 */
int set_access_time(char *access_time)
{
	strncpy(api_face->access_time, access_time, 128);

	return 0;
}

/**
 * set_access_type: set access type
 *
 * return 0 if success, otherwise return -1
 */
int set_access_type(int access_type)
{
	api_face->access_type = access_type;

	return 0;
}

/**
 * set_language: set language
 *
 * return 0 if success, otherwise return -1
 */
int set_language(device_lang_param_t *info)
{
	memcpy(&api_face->lang_t, info, sizeof(device_lang_param_t));

	return 0;
}

/**
 * set_display_control: set display control
 *
 * return 0 if success, otherwise return -1
 */
int set_display_control(display_control_t *p_info)
{
	if (p_info == NULL)
		return -1;

	memcpy(&api_face->display_ctl, p_info, sizeof(display_control_t));

	return 0;
}

/**
 * face_get_identify_threshold_score: get identify threshold score
 * @score: a pointer of identify threshold score
 *
 * return 0 if success, otherwise return -1
 */
int face_get_identify_threshold_score(float *score)
{
	if ((NULL == score) || (NULL == api_face))
		return -1;

	*score = api_face->recg_param.identify_threshold_score;

	return 0;
}

/**
 * faces_set: copy face info
 * @api_face: a pointer of face service
 * @frame: face frame
 * @f_info: face rect info
 *
 * return 0 if success, otherwise return -1
 */
int faces_set(api_face_t *api_face, face_frame_t *frame, face_info_t *f_info)
{
	face_t *face;
	int max_id = 0;
	float threshold_score;

#if ATTENDANCE_MODE != 0
	tts_data_t tts;
#endif

#if ATTENDANCE_MODE == 2
	int type = 0;
	struct timeval tv;
	web_socket_checkin_info_t *person = NULL;
#endif

	pthread_mutex_lock(&api_face->face_lock);
	list_for_each_entry(face, &api_face->face_list, node) {
		if (face->id == f_info->rect.id) {
			face->max_id = f_info->feat.max_id;
			face->living = f_info->feat.living_body;
			face->score = f_info->feat.score;

			memcpy(face->nir_rect, f_info->nir_rect.rect,
				   sizeof(f_info->nir_rect.rect));
			face->nir_rect[0] = f_info->nir_rect.rect[0] / 2;
			face->nir_rect[1] = f_info->nir_rect.rect[1] / 2;
			face->nir_rect[2] = f_info->nir_rect.rect[2] / 2;
			face->nir_rect[3] = f_info->nir_rect.rect[3] / 2;

			if (api_face->login_mode == 1)
				is_admin(face->userid);
			face->det_ts = (frame->det_end - frame->det_begin) / 1000;
			face->recg_ts = (f_info->recg_end - f_info->recg_begin) / 1000;
			face->cmp_ts = (f_info->cmp_end - f_info->cmp_begin) / 1000;
			face->living_ts = (f_info->living_end -
							   f_info->living_begin) / 1000;

			if (api_face->recg_param.mask_enable && f_info->rect.attr.mask)
				threshold_score = api_face->recg_param.mask_threshold_value;
			else
				threshold_score = api_face->recg_param.identify_threshold_score;

			if (f_info->feat.score >= threshold_score) {
				face->recg = 1;
				/*name set*/
				pthread_mutex_lock(&api_face->db_lock);
				ALOGD("db_version = %d feat_version = %d\n",
						api_face->db_version,
						f_info->feat.feat_version);
				if (face->max_id >= 0 && face->max_id <
						api_face->face_db->count && api_face->db_version
						== f_info->feat.feat_version) {
					max_id = face->max_id;
					memcpy(face->name,
						&api_face->face_db->name[max_id],
						sizeof(face->name));
					memcpy(face->userid,
						&api_face->face_db->userid[max_id],
						sizeof(face->userid));
					ALOGD("[%s:%d] userid = %s name = %s faceid %d\n", __func__, __LINE__,
						face->userid,
						face->name, face->id);
				}
				pthread_mutex_unlock(&api_face->db_lock);

#if ATTENDANCE_MODE == 1
				/*sound play*/
				if (!face->sound_play &&
					(!api_face->recg_param.nir_live_detect ||
					 f_info->feat.living_body)) {
					face->sound_play = 1;

					int step = api_face->attendance_info.avoid_attendance_step;
					int index = f_info->feat.max_id;
					if (frame->time_stamp !=
						api_face->face_db->last_ts[index] &&
						frame->time_stamp -
						api_face->face_db->last_ts[index] <=
						step * 1000000) {
						ALOGD("Avoid attendance record tts play");
						memset(&tts, 0 ,sizeof(tts_data_t));
						tts.format = TTS_FORMAT_UTF8;
						tts.string = face->name;
						tts.len = strlen(face->name);
						tts.greet_id = (int)RECG_PUNCH_REPEATEDLY - 1;
						tts.greet_param.language_type = api_face->lang_t.lang;
						tts.greet_param.pos = GREETING_END;
						sound_play_tts(&tts);
					} else {
						/* sound_play_file( PASS_SOUND_PATH); */
						memset(&tts, 0 ,sizeof(tts_data_t));
						tts.format = TTS_FORMAT_UTF8;
						tts.string = face->name;
						tts.len = strlen(face->name);
						tts.greet_id = (int)RECG_PUNCH_SUCCESSFUL - 1;
						tts.greet_param.language_type = api_face->lang_t.lang;
						tts.greet_param.pos = GREETING_END;
						sound_play_tts(&tts);
						api_face->face_db->last_ts[index] = frame->time_stamp;
					}
				}
#elif ATTENDANCE_MODE == 2
				/*sound play*/
				if (!face->sound_play &&
					(!api_face->recg_param.nir_live_detect ||
					 f_info->feat.living_body)) {
					face->sound_play = 1;

					memset(&tts, 0 ,sizeof(tts_data_t));
					tts.format = TTS_FORMAT_UTF8;
					tts.string = face->name;
					tts.len = strlen(face->name);
					tts.greet_id = (int)RECG_PUNCH_SUCCESSFUL - 1;
					tts.greet_param.language_type = api_face->lang_t.lang;
					tts.greet_param.pos = GREETING_END;
					sound_play_tts(&tts);

					/*checkin data*/
					if (!face->checkin &&
						(!api_face->recg_param.nir_live_detect ||
						 f_info->feat.living_body)) {
						face->checkin = 1;
						person =
							calloc(
								sizeof(web_socket_checkin_info_t),
								1);
						if (NULL != person) {
							strncpy(person->user_id,
									face->userid,
									64);
							person->user_type = type;
							/*time((time_t *)&person->timestamp);*/
							gettimeofday(&tv, NULL);
							person->timestamp = tv.tv_sec;
							person->upload_time = tv.tv_usec;
							strncpy(person->name,
									face->name,
									PERSON_NAME_MAX);
							person->temperature = 0;
							/**/
							face_task_add(face_checkin_insert, person);
						}
					}
				}

#elif ATTENDANCE_MODE == 3
				/*sound play*/
				if (!face->sound_play &&
					(!api_face->recg_param.nir_live_detect ||
					 f_info->feat.living_body)) {
					face->sound_play = 1;

					memset(&tts, 0 ,sizeof(tts_data_t));
					tts.format = TTS_FORMAT_UTF8;
					tts.string = face->name;
					tts.len = strlen(face->name);
					tts.greet_param.pos = GREETING_END;
					tts.greet_param.language_type = api_face->lang_t.lang;
					tts.greet_id = (int)RECG_PUNCH_SUCCESSFUL - 1;
					sound_play_tts(&tts);
				}
#endif

#if ATTENDANCE_MODE != 0
#ifdef OPEN_LOCK
				if (!face->open_lock &&
					(!api_face->recg_param.nir_live_detect ||
					 f_info->feat.living_body)) {
					if (is_access_time()) {
						face->open_lock = 1;
						open_door();
						ALOGD("open_door()");
					} else {
						ALOGD("non-traffic time");
					}
				}
#endif

#ifdef WG_OUT
				if (!face->wg_out &&
					(!api_face->recg_param.nir_live_detect ||
					f_info->feat.living_body)) {
					face->wg_out = 1;
					wiegand_write_svc(atoll(face->userid));
				}
#endif
#endif
			}

			/* for debug: calculate recg time */
			if (face->score >= threshold_score
				&& face->recg_ts_end == 0)
				face->recg_ts_end = time_get_us();
			if (face->living == 1 && face->living_ts_end == 0)
				face->living_ts_end = time_get_us();
			break;
		}
	}
	pthread_mutex_unlock(&api_face->face_lock);

	return 0;
}

/**
 * _faces_get: get face data from face module
 * @api_face: a pointer of face service
 *
 * return face data
 */
faces_t *_faces_get(api_face_t *api_face)
{
	faces_t *faces = NULL;
	face_t *face;
	int i;

	pthread_mutex_lock(&api_face->face_lock);

	faces = malloc(sizeof(faces_t));

	if (!faces) {
		ALOGE("alloc faces_t failed");
		return NULL;
	}
	memset(faces, 0, sizeof(faces_t));

	if (api_face->face_num == 0) {
		ALOGD_IF(FACE_DEBUG, "face list is empty");
		pthread_mutex_unlock(&api_face->face_lock);
		return faces;
	}
	faces->face_num = api_face->face_num;
	faces->faces = malloc(sizeof(face_t) * faces->face_num);

	if (!faces->faces) {
		ALOGE("alloc faces failed");
		faces->face_num = 0;
		pthread_mutex_unlock(&api_face->face_lock);
		return faces;
	}
	i = 0;
	list_for_each_entry(face, &api_face->face_list, node) {
		memcpy(&faces->faces[i], face, sizeof(face_t));
		i++;
	}

	pthread_mutex_unlock(&api_face->face_lock);

	return faces;
}

/**
 * faces_get: get face data from face module
 *
 * return face data
 */
faces_t *faces_get(void)
{
	return _faces_get(api_face);
}

/**
 * faces_put: free face data to face module
 * @faces: face data
 *
 * return none
 */
void faces_put(faces_t *faces)
{
	if (faces) {
		if (faces->faces)
			free(faces->faces);
		free(faces);
	}
}

/**
 * api_face_det_cb: process result of face detect
 * @frame: face frame
 * @f_rects: face rects
 *
 * return NULL
 */
void *api_face_det_cb(face_frame_t *frame, face_rects_t *f_rects)
{
	if (!f_rects)
		return NULL;
	face_update(api_face, f_rects, frame);
	if (f_rects->cnt > 0) {
		screen_count_reset();
		screen_saver_count_reset();
		filling_light_reset_count();
		api_face->cb_flag = 0;
		if (api_face->cb) {
			api_face->cb(NULL, api_face->login_mode);
		}
	} else {
		if (api_face->cb_flag == 0 || api_face->cb_flag == -1) {
			if (api_face->cb) {
				api_face->cb(NULL, api_face->login_mode);
			}
			api_face->cb_flag = 1;
		}
	}

	return NULL;
}

/**
 * api_face_recg_cb: process result of face recognition
 * @frame: face frame
 * @f_rects: face rects
 *
 * return NULL
 */
void *api_face_recg_cb(face_frame_t *frame, face_info_t *f_info)
{
	if (!f_info)
		return NULL;
	faces_set(api_face, frame, f_info);

	return NULL;
}

/**
 * face_save_record: save record result of face recognition
 * @frame: face frame
 * @f_rects: face rects
 *
 * return NULL
 */
static void *face_save_record(face_frame_t *frame, face_record_t *record)
{
	int index = 0, ret = 0;
	face_info_t *face_info;
	web_socket_checkin_info_t checkin_data;
	char name[32] = {0};

	/* parameters check */
	if (!record) {
		ALOGE("%s: pointer is NULL\n", __func__);
		return NULL;
	}
	memset(&checkin_data, 0, sizeof(web_socket_checkin_info_t));
	face_info = &record->face_info;
	index = face_info->feat.max_id;

#if ATTENDANCE_MODE == 0
	pthread_mutex_lock(&api_face->db_lock);
	if (api_face->checkin_info.save_large_pic)
		strncpy(checkin_data.frame_path, record->img.path[record->type], 128);
	else
		strncpy(checkin_data.frame_path, "", 128);
	if (record->type == match_frame) {
		strncpy(checkin_data.face_path, record->img.path[match_face], 128);
	} else if (record->type == unmatch_frame) {
		strncpy(checkin_data.face_path, record->img.path[unmatch_face], 128);
	} else if (record->type == unqualified_frame) {
		strncpy(checkin_data.face_path, record->img.path[unqualified_face], 128);
	}
	if (!api_face->checkin_info.save_small_pic)
		strncpy(checkin_data.face_path, "", 128);
	checkin_data.temperature = 0;
	time((time_t *)&checkin_data.timestamp);

	if (record->face_pass) {
		strncpy(checkin_data.user_id, api_face->face_db->userid[index].userid, 64);
		checkin_data.user_type = api_face->face_db->type[index].user_type;
		strncpy(checkin_data.name, api_face->face_db->name[index].name, 32);
		ret = db_websocket_checkin_info_insert(&checkin_data);
		if (0 > ret) {
			ALOGW("%s:%d >> insert checkin failed!", __func__, __LINE__);
			file_remove(checkin_data.face_path);
			file_remove(checkin_data.frame_path);
		}
	} else if (record->face_refuse) {
		strncpy(checkin_data.user_id, "-1", 64);
		checkin_data.user_type = STRANGER;
		strncpy(checkin_data.name, "", 32);
		ret = db_websocket_checkin_info_insert(&checkin_data);
		if (0 > ret) {
			ALOGW("%s:%d >> insert checkin failed!", __func__, __LINE__);
			file_remove(checkin_data.face_path);
			file_remove(checkin_data.frame_path);
		}
	}
	pthread_mutex_unlock(&api_face->db_lock);

	api_face->face_db->last_ts[index] = record->time;
	ALOGD("last_ts=%lld",api_face->face_db->last_ts[index]);
#elif ATTENDANCE_MODE == 1
	int step = api_face->attendance_info.avoid_attendance_step;
	if (record->time != api_face->face_db->last_ts[index] &&
		record->time - api_face->face_db->last_ts[index] <=
		step * 1000000) {
		ALOGD("Avoid attendance record");
		remove(record->img.path[record->type]);
	} else {
		pthread_mutex_lock(&api_face->db_lock);
		strncpy(checkin_data.user_id, api_face->face_db->userid[index].userid, 64);
		checkin_data.user_type = api_face->face_db->type[index].user_type;
		strncpy(name, api_face->face_db->name[index].name, 32);
		strncpy(checkin_data.face_path, record->img.path[record->type], 128);
		strncpy(checkin_data.name, name, 32);
		checkin_data.temperature = 0;
		pthread_mutex_unlock(&api_face->db_lock);

		time((time_t *)&checkin_data.timestamp);
		ret = db_websocket_checkin_info_insert(&checkin_data);
		if (0 >= ret) {
			ALOGW("%s:%d >> insert checkin failed!", __func__, __LINE__);
			file_remove(checkin_data.face_path);
			file_remove(checkin_data.frame_path);
		}
		api_face->face_db->last_ts[index] = record->time;
		ALOGD("last_ts=%lld",api_face->face_db->last_ts[index]);
	}
#elif ATTENDANCE_MODE == 2
	pthread_mutex_lock(&api_face->db_lock);
	strncpy(checkin_data.user_id, api_face->face_db->userid[index].userid, 64);
	checkin_data.user_type = api_face->face_db->type[index].user_type;
	strncpy(name, api_face->face_db->name[index].name, 32);
	strncpy(checkin_data.face_path, "", 128);
	strncpy(checkin_data.name, name, 32);
	checkin_data.temperature = 0;
	pthread_mutex_unlock(&api_face->db_lock);
	time((time_t *)&checkin_data.timestamp);
	ret = db_websocket_checkin_info_insert(&checkin_data);
	if (0 >= ret) {
		ALOGW("%s:%d >> insert checkin failed!", __func__, __LINE__);
		file_remove(checkin_data.face_path);
		file_remove(checkin_data.frame_path);
	}
#else
	pthread_mutex_lock(&api_face->db_lock);
	strncpy(checkin_data.user_id, api_face->face_db->userid[index].userid, 64);
	checkin_data.user_type = api_face->face_db->type[index].user_type;
	strncpy(name, api_face->face_db->name[index].name, 32);
	strncpy(checkin_data.face_path, record->img.path[record->type], 128);
	strncpy(checkin_data.name, name, 32);
	checkin_data.temperature = 0;
	pthread_mutex_unlock(&api_face->db_lock);

	time((time_t *)&checkin_data.timestamp);
	ret = db_websocket_checkin_info_insert(&checkin_data);
	if (0 >= ret) {
		ALOGW("%s:%d >> insert checkin failed!", __func__, __LINE__);
		file_remove(checkin_data.face_path);
		file_remove(checkin_data.frame_path);
	}
#endif

	if (api_face->test_log_enable == 1) {
		test_log_record_default(&checkin_data, name, record);
	}

	pthread_mutex_lock(&api_face->checkin_lock);
	pthread_cond_signal(&api_face->checkin_cond);
	pthread_mutex_unlock(&api_face->checkin_lock);

	return NULL;
}

/**
 * face_checkin_init: initialize check checkin thread
 *
 * return 0 if success, otherwise return -1
 */
int face_checkin_init(void)
{
	if (pthread_mutex_init(&api_face->checkin_lock, NULL)) {
		ALOGE("init sound lock failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		return -1;
	}
	if (pthread_cond_init(&api_face->checkin_cond, NULL)) {
		ALOGE("init sound cond failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		return -1;
	}
	if (pthread_create(&api_face->checkin_thrd, NULL,
					   checkin_thread, (void *)api_face)) {
		ALOGE("create face sound play thread failed, errno: %d (%s)",
			  errno, strerror(errno));
		return -1;
	}
	if (pthread_create(&api_face->storage_thrd, NULL,
					   storage_thread, (void *)api_face)) {
		ALOGE("create face storage thread failed, errno: %d (%s)",
			  errno, strerror(errno));
		return -1;
	}
	pthread_detach(api_face->storage_thrd);

	return 0;
}

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
int websocket_sound_play(_websocket_sound_t type)
{
	int ret = 0;

	switch (type) {
	case sound_bind:
		ret = sound_play_file(BIND_SOUND_PATH);
		break;
	case sound_unbind:
		ret = sound_play_file(UNBIND_SOUND_PATH);
		break;
	case sound_netlink:
		ret = sound_play_file(NET_LINK_SOUND_PATH);
		break;
	case sound_netdislink:
		ret = sound_play_file(NET_DISLINK_SOUND_PATH);
		break;
	}

	return ret;
}

/**
 * face_capture_ok_sound_play: play capture ok sound (add to play list)
 *
 * return 0 if success, otherwise return -1
 */
int face_capture_ok_sound_play(void)
{
	return sound_play_file(CAPTURE_OK_SOUND_PATH);
}

/**
 * face_capture_ok_sound_play_block: play capture ok sound (block)
 *
 * return 0 if success, otherwise return -1
 */
int face_capture_ok_sound_play_block(void)
{
	return face_play_sound(CAPTURE_OK_SOUND_PATH);
}

/**
 * face_db_free: free face db record
 * @face_db: a pointer of face db record
 *
 * return none
 */
void face_db_free(face_db_t *face_db)
{
	if (face_db) {
		if (face_db->userid)
			free(face_db->userid);
		if (face_db->name)
			free(face_db->name);
		if (face_db->feat)
			free(face_db->feat);
		if (face_db->kpts)
			free(face_db->kpts);
		if (face_db->norm)
			free(face_db->norm);
		if (face_db->type)
			free(face_db->type);
		free(face_db);
	}
}

/**
 * get_queue_data: add to memory face db list from queue
 * @h: a pointer of record queue
 * @face_db: memory face db list
 * @feat_cnt: feature count
 * @index: offset of face db list
 *
 * return 0 if success, otherwise return -1
 */
int get_queue_data(ee_queue_t *h, face_db_t *face_db, int feat_cnt, int index)
{
	int idx = index;
	websocket_person_info_t *p = NULL;

	ee_queue_foreach(p, h, websocket_person_info_t, queue) {
		if (idx < feat_cnt) {
#if ATTENDANCE_MODE != 2
			if (strlen(p->pic_large) <= 0 &&
				strlen(p->pic_small) <= 0) {
				ALOGW("userid[%s]='%s' pic_large is null, loading failed!!",
					p->user_id, p->name);
				continue;
			}
#endif
			memcpy(face_db->userid[idx].userid, p->user_id,
				sizeof(user_id_t));
			memcpy(face_db->feat[idx].values,
				p->face_feature, FACE_FEAT_SIZE);
			memcpy(face_db->name[idx].name,
				p->name, PERSON_NAME_MAX);
			memcpy(face_db->kpts[idx].kpts,
				p->key_points, sizeof(float) * 10);

			face_db->type[idx].user_type = p->user_type;
			face_db->norm[idx * 2] = (float)
				p->face_norm[0];
			face_db->norm[idx * 2 + 1] = (float)
				p->face_norm[1];
			idx++;
		}
	}
	api_face->uid.cnt = idx;

	return idx;
}

/**
 * face_db_update_admin: add to memory admin db list from database(admin table)
 *
 * return 0 if success, otherwise return -1
 */
int face_db_update_admin(void)
{
	ee_queue_t head_admin;
	ee_queue_t *h_admin = NULL;
	websocket_admin_info_t *p = NULL;
	int admin_cnt = 0;
	int idx = 0;
	user_id_t *user = NULL;

	h_admin = &head_admin;
	ee_queue_init(h_admin);

	admin_cnt = db_websocket_admin_info_query_begin(h_admin);

	if (0 == admin_cnt)

		goto error;

	user = malloc(admin_cnt * sizeof(user_id_t));

	if (user == NULL)

		goto error;

	ee_queue_foreach(p, h_admin, websocket_admin_info_t, queue) {
		if (idx < admin_cnt) {

			strncpy(user[idx].userid, p->user_id, 64);
			idx++;
		}
	}
	db_websocket_admin_info_query_end(h_admin);

	if (api_face->admins.user) {
		free(api_face->admins.user);
		api_face->admins.user = NULL;
	}
	api_face->admins.num = admin_cnt;
	api_face->admins.user = user;

	return 0;

error:
	db_websocket_admin_info_query_end(h_admin);
	return 0;
}

int _face_db_memset(void)
{
	if (api_face->face_db) {
		if (api_face->face_db->userid) {
			memset(api_face->face_db->userid, 0, sizeof(user_id_t) * MAX_FEAT_CNT);
		} else {
			ALOGE("[%s][%d]: err", __func__, __LINE__);
		}
		if (api_face->face_db->name) {
			memset(api_face->face_db->name, 0, sizeof(name_t) * MAX_FEAT_CNT);
		} else {
			ALOGE("[%s][%d]: err", __func__, __LINE__);
		}
		if (api_face->face_db->feat) {
			memset(api_face->face_db->feat, 0, sizeof(feat_t) * MAX_FEAT_CNT);
		} else {
			ALOGE("[%s][%d]: err", __func__, __LINE__);
		}
		if (api_face->face_db->norm) {
			memset(api_face->face_db->norm, 0, sizeof(float) * MAX_FEAT_CNT * 2);
		} else {
			ALOGE("[%s][%d]: err", __func__, __LINE__);
		}
		if (api_face->face_db->kpts) {
			memset(api_face->face_db->kpts, 0, sizeof(kpts_t) * MAX_FEAT_CNT);
		} else {
			ALOGE("[%s][%d]: err", __func__, __LINE__);
		}
		if (api_face->face_db->type) {
			memset(api_face->face_db->type, 0, sizeof(user_type_t) * MAX_FEAT_CNT);
		} else {
			ALOGE("[%s][%d]: err", __func__, __LINE__);
		}
	} else {
		ALOGE("[%s][%d]: err", __func__, __LINE__);
	}
	return 0;
}

int _face_db_malloc(void)
{
	face_db_t *face_db = NULL;

	face_db = calloc(sizeof(face_db_t), 1);
	if (!face_db) {
		ALOGE("%s: malloc face_db failed", __func__);
		goto free_res;
	}

	face_db->feat = malloc(sizeof(feat_t) * MAX_FEAT_CNT);
	if (!face_db->feat) {
		ALOGE("%s: malloc feat memery failed", __func__);
		goto free_res;
	}

	face_db->userid = malloc(sizeof(user_id_t) * MAX_FEAT_CNT);
	if (!face_db->userid) {
		ALOGE("%s: malloc employee_number_t failed", __func__);
		goto free_res;
	}

	face_db->type = malloc(sizeof(user_type_t) * MAX_FEAT_CNT);
	if (!face_db->type) {
		ALOGE("%s: malloc employee_number_t failed", __func__);
		goto free_res;
	}

	face_db->name = malloc(sizeof(name_t) * MAX_FEAT_CNT);
	if (!face_db->name) {
		ALOGE("%s: malloc feat memery failed", __func__);
		goto free_res;
	}

	face_db->norm = malloc(sizeof(float) * MAX_FEAT_CNT * 2);
	if (!face_db->norm) {
		ALOGE("%s: malloc norm failed", __func__);
		goto free_res;
	}

	face_db->kpts = malloc(sizeof(kpts_t) * MAX_FEAT_CNT);
	if (!face_db->kpts) {
		ALOGE("%s: malloc kpts failed", __func__);
		goto free_res;
	}

	face_db->last_ts = malloc(sizeof(long long) * MAX_FEAT_CNT);
	if (!face_db->last_ts) {
		ALOGE("%s: malloc last_ts failed", __func__);
		goto free_res;
	}
	memset(face_db->last_ts, 0, sizeof(long long) * MAX_FEAT_CNT);

	api_face->face_db = face_db;

	ALOGE("[%s][%d]: enter", __func__, __LINE__);
	return 0;

free_res:

	ALOGE("%s: fail\n", __func__);

	if (face_db)
		face_db_free(face_db);
	return -1;

}

/**
 * face_db_malloc: notify face module for employee table
 *
 * return 0 if success, otherwise return -1
 */
int face_db_malloc(void)
{
	feats_t feats;
	int ret = 0;

	/**************************************************
	 * Pre-alloc face db memory for following reason:
	 * 1. Improve efficiency of face update
	 * 2. Prevent memory fragment which will cause oom(out-of-memory)
	 *  when free memory is low.
	 **************************************************/
	pthread_mutex_lock(&api_face->db_lock);
	if (api_face->face_db) {
		/* clean face_svc feattures */
		memset(&feats, 0, sizeof(feats_t));
		ret = face_register_face_feats(api_face->svc_hdl, &feats);
		if (ret) {
			ALOGE("%s: face_register_face_feats fail\n", __func__);
		}
		ret = _face_db_memset();
		if (ret) {
			ALOGE("%s: _face_db_memset fail\n", __func__);
		}
	} else {
		ret = _face_db_malloc();
		if (ret) {
			ALOGE("%s: _face_db_malloc fail\n", __func__);
		}
		ret = _face_db_memset();
		if (ret) {
			ALOGE("%s: _face_db_memset fail\n", __func__);
		}
	}
	pthread_mutex_unlock(&api_face->db_lock);

	return ret;
}

/**
 * face_db_add: notify face module for employee table add
 *
 * return 0 if success, otherwise return -1
 */
int face_db_add(char *user_id) {
	int ret = -1;
	face_db_t *face_db = NULL;
	ee_queue_t head_person;
	ee_queue_t *h_person = NULL;
	websocket_person_info_t *p = NULL;
	h_person = &head_person;
	int person_cnt = 0;
	feats_t feats;

	person_cnt = db_person_info_get_begin(h_person, TYPE_PERSON, user_id);
	if (person_cnt <= 0) {
		ALOGE("%s:%d >> had not found userid[%s]!",
			__func__, __LINE__, user_id);
		return ret;
	}

	pthread_mutex_lock(&api_face->db_lock);
	face_db = api_face->face_db;
	if (face_db->count < MAX_FEAT_CNT) {
		get_queue_data(h_person, face_db, face_db->count+1, face_db->count);
		face_db->count++;
		ret = 0;
	} else {
		for (int i = 0; i < face_db->count; i++) {
			if (0 == strcmp(face_db->userid[i].userid, "-65535")) {
				/* clean face_svc feattures */
				memset(&feats, 0, sizeof(feats_t));
				ret = face_register_face_feats(api_face->svc_hdl, &feats);
				if (ret) {
					ALOGE("%s: face_register_face_feats fail\n", __func__);
					break;
				}
				ret = -1;
				ee_queue_foreach(p, h_person,
					websocket_person_info_t, queue) {
#if ATTENDANCE_MODE != 2
					if (strlen(p->pic_large) <= 0 &&
						strlen(p->pic_small) <= 0) {
						ALOGW("userid[%s]='%s' pic_large is null, loading failed!!",
							p->user_id, p->name);
						continue;
					}
#endif
					memcpy(face_db->userid[i].userid,
						p->user_id, sizeof(user_id_t));
					memcpy(face_db->feat[i].values,
						p->face_feature, FACE_FEAT_SIZE);
					memcpy(face_db->name[i].name,
						p->name, PERSON_NAME_MAX);
					memcpy(face_db->kpts[i].kpts,
						p->key_points, sizeof(float) * 10);

					face_db->type[i].user_type = p->user_type;
					face_db->norm[i * 2] = (float)p->face_norm[0];
					face_db->norm[i * 2 + 1] = (float)p->face_norm[1];
					ret = 0;
					break;
				}
				break;
			}
		}
	}
	if (0 == ret) {
		/* update face feattures */
		memset(&feats, 0, sizeof(feats_t));
		feats.cnt = face_db->count;
		feats.feat = face_db->feat;
		feats.kpts = face_db->kpts;
		feats.norm = face_db->norm;
		feats.version = ++api_face->db_version;
		ret = face_register_face_feats(api_face->svc_hdl, &feats);
	}
	db_websocket_person_info_query_end(h_person, TYPE_PERSON);
	pthread_mutex_unlock(&api_face->db_lock);
	return ret;
}

/**
 * face_db_delete: notify face module for employee table delete
 *
 * return 0 if success, otherwise return -1
 */
int face_db_delete(char *user_id)
{
	int ret = -1;
	face_db_t *face_db = NULL;
	feats_t feats;

	pthread_mutex_lock(&api_face->db_lock);
	face_db = api_face->face_db;
	for (int i = 0; i < face_db->count; i++) {
		if (0 == strcmp(face_db->userid[i].userid, user_id)) {
			/* clean face_svc feattures */
			memset(&feats, 0, sizeof(feats_t));
			ret = face_register_face_feats(api_face->svc_hdl, &feats);
			if (ret) {
				ALOGE("%s: face_register_face_feats fail\n", __func__);
			}
			/* clean face feattures */
			strncpy(face_db->userid[i].userid, "-65535", PERSON_USERID_MAX);
			memset(face_db->feat[i].values, 0, FACE_FEAT_SIZE);
			face_db->norm[i * 2] = 0;
			face_db->norm[i * 2 + 1] = 0;
			/* update face feattures */
			face_db->count--;
			feats.cnt = face_db->count;
			feats.feat = face_db->feat;
			feats.kpts = face_db->kpts;
			feats.norm = face_db->norm;
			feats.version = ++api_face->db_version;
			ret = face_register_face_feats(api_face->svc_hdl, &feats);
			break;
		}
	}
	pthread_mutex_unlock(&api_face->db_lock);
	return ret;
}

/**
 * face_db_update: notify face module for employee table update
 *
 * return 0 if success, otherwise return -1
 */
int face_db_update(void)
{
	feats_t feats;
	face_db_t *face_db = NULL;
	int feat_cnt = 0;
	int person_cnt = 0;
	int visitor_cnt = 0;
	int ret = 0;
	int db_ret = 0;
	int is_load_db_error = 0;
	int load_db_step = 0;
#if ATTENDANCE_MODE == 0
	load_db_step = 25000;
#else
	load_db_step = 500;
#endif

	ee_queue_t head_person;
	ee_queue_t *h_person = NULL;
	ee_queue_t head_visitor;
	ee_queue_t *h_visitor = NULL;
	file_system_sync();

	ret = face_db_malloc();
	if (ret) {
		ALOGE("%s: malloc face_db memery failed", __func__);
		return ret;
	}

	h_person = &head_person;
	h_visitor = &head_visitor;

	ee_queue_init(h_person);
	ee_queue_init(h_visitor);

	face_db = api_face->face_db;

	if (NULL == face_db) {
		ALOGE("%s: malloc face_db memery failed", __func__);
		return ret;
	}
	person_cnt = db_ui_person_info_get_count(TYPE_PERSON);
	visitor_cnt = db_ui_person_info_get_count(TYPE_VISITOR);

	feat_cnt = person_cnt + visitor_cnt;
	feat_cnt = (feat_cnt <= MAX_FEAT_CNT) ? feat_cnt : MAX_FEAT_CNT;
	pthread_mutex_lock(&api_face->db_lock);

	ALOGD("[%s][%d]: start load person,feat_cnt: %d\n",
		__func__, __LINE__, feat_cnt);
	if (feat_cnt > 0) {
		ret = 0;
		db_ret = 0;
		api_face->uid.cnt = 0;
		while(ret < person_cnt) {
			db_ret = db_person_info_mmc(TYPE_PERSON, (void *)face_db,
				feat_cnt, &api_face->uid.cnt, load_db_step);
			if (db_ret > 0) {
				ret += db_ret;
			} else {
				ALOGE("%s:%d >> There may be something wrong in the db! \n"
					"You can run this cmd to find the different of db: \n"
					"sqlite3 /data/user.db '.schema employee'",
				__func__, __LINE__);
				is_load_db_error = 1;
				break;
			}
		}
		face_db->count += ret;

		ret = 0;
		db_ret = 0;
		while(ret < visitor_cnt) {
			db_ret = db_person_info_mmc(TYPE_VISITOR, (void *)face_db,
				feat_cnt, &api_face->uid.cnt, load_db_step);
			if (db_ret > 0) {
				ret += db_ret;
			} else {
				ALOGE("%s:%d >> There may be something wrong in the db! \n"
					"You can run this cmd to find the different of db: \n"
					"sqlite3 /data/user.db '.schema employee'",
				__func__, __LINE__);
				is_load_db_error = 1;
				break;
			}
		}
		face_db->count += ret;
	}

	ALOGD("[%s][%d]: end load person! face_count: %d. %s",
		__func__, __LINE__, face_db->count,
		is_load_db_error?"some feature load error":"all db load ok");

	/* update face feattures */
	feats.cnt = face_db->count;
	feats.feat = face_db->feat;
	feats.kpts = face_db->kpts;
	feats.norm = face_db->norm;
	feats.version = ++api_face->db_version;
	ret = face_register_face_feats(api_face->svc_hdl, &feats);
	pthread_mutex_unlock(&api_face->db_lock);

	return ret;

}

int qr_scan_cb(void *arg, char *data)
{
	ALOGD("%s[%d] qr_code recg >> %s", __func__, __LINE__, data);
	if (api_face->qr_cb)
		api_face->qr_cb(data);
	return 0;
}

/**
 * face_set_recg_param: set face recognition parameter
 * @param: face recognition parameter
 * @param: init flag
 *
 * return 0 if success, otherwise return -1
 */
int face_set_recg_param(face_recognition_param_t *param, int init_flag)
{
	int ret = 0;
	face_config_t config;
	int mask_recg_enable;
	float mask_threshold_score;

	/* face recg score threshold and nir living detect */
	config.score_threshold = param->identify_threshold_score;
	config.living = param->nir_live_detect;
	ret = face_svc_set_config(api_face->svc_hdl, &config);

	/* face recg enable */
	if (init_flag || (api_face->recg_param.recg_enable != param->recg_enable)) {
		if (param->recg_enable) {
			ret = face_svc_recg_resume(api_face->svc_hdl);
			if (ret)
				ALOGE("face svc recg resume failed");
		} else {
			ret = face_svc_recg_pause(api_face->svc_hdl);
			if (ret)
				ALOGE("face svc recg pause failed");
		}
	}

	/* infrared image */
	if (init_flag || (api_face->recg_param.infrared_image_enable !=
		param->infrared_image_enable)) {
		if (param->infrared_image_enable) {
			ret = face_svc_resume_nir_preview(api_face->svc_hdl);
			if (ret)
				ALOGE("face svc resume nir preview failed");
		} else {
			ret = face_svc_pause_nir_preview(api_face->svc_hdl);
			if (ret)
				ALOGE("face svc pause nir preview failed");
		}
	}

	/* nir living detect score threshold */
	if (api_face->recg_param.living_detect_threshold_score !=
			(int)(param->living_detect_threshold_score * 100)) {
		ret = face_svc_set_living_score_threshold(
			(int)(param->living_detect_threshold_score * 100));
		if (ret)
			ALOGE("face svc set living score threshold failed");
	}

	/* mask recg */
	mask_recg_enable = face_svc_get_mask_recg_enable(api_face->svc_hdl);

	if (mask_recg_enable != param->mask_enable) {
		ret = face_svc_set_mask_recg_enable(api_face->svc_hdl, param->mask_enable);
		if (ret)
			ALOGE("face set mask recg enable failed");
	}

	/* mask recg score threshold */
	if (param->mask_enable) {
		mask_threshold_score = face_svc_get_mask_score_threshold(api_face->svc_hdl);
		if (mask_threshold_score != param->mask_threshold_value) {
			ret = face_svc_set_mask_score_threshold(api_face->svc_hdl,
					param->mask_threshold_value);
			if (ret)
				ALOGE("face set mask score threshold failed");
		}
	}

	/* recg timeout */
	if (init_flag || (api_face->recg_param.recg_timeout !=
		param->recg_timeout)) {
		api_face->recg_param.recg_timeout = param->recg_timeout;
		set_gate_dump_img_config();
	}

	/* recg repeat */
	if (init_flag || (api_face->recg_param.repeat_recg_enable !=
		param->repeat_recg_enable)) {
		ret = face_svc_set_recg_repeat(api_face->svc_hdl,
			param->repeat_recg_enable);
		if (ret)
			ALOGE("face svc set recg repeat failed");
	}

	/* recg repeat interval */
	if (init_flag || (api_face->recg_param.recg_interval !=
		param->recg_interval)) {
		ret = face_svc_set_recg_repeat_interval(api_face->svc_hdl,
			param->recg_interval);
		if (ret)
			ALOGE("face svc set recg repeat interval failed");
	}

	/* recg distance */
	if (init_flag || (api_face->recg_param.recg_distance !=
		param->recg_distance)) {
		ret = face_svc_set_det_distance(param->recg_distance);
		if (ret)
			ALOGE("face svc set min face pixel failed");
	}

	/* voice mode */
	/* grettings */
	/* stranger voice */

	/* qr code recg enable */
	if (init_flag || (api_face->recg_param.qr_code_recg !=
		param->qr_code_recg)) {
		if (param->qr_code_recg) {
			ALOGD("%s[%d] face_svc_start_qrmode", __func__, __LINE__);
			ret = face_svc_start_qrmode_set(api_face->svc_hdl, 2, qr_scan_cb, NULL);
			if (ret)
				ALOGE("face svc start qrmode failed");
		} else {
			ret = face_svc_stop_qrmode(api_face->svc_hdl);
			if (ret)
				ALOGE("face svc stop qrmode failed");
		}
	}

	/* body temperature detect enable */
	if (init_flag || (api_face->recg_param.body_temperature_detect !=
			param->body_temperature_detect)) {
		ret = face_svc_set_temp_measure(param->body_temperature_detect);
		if (ret)
			ALOGE("face svc set temp measure failed");
	}

	/* alarm temperature */

	/* exposure values */
	if (init_flag || (api_face->recg_param.exposure_values !=
		param->exposure_values)) {
		ret = face_svc_set_exp_bias(0, param->exposure_values);
		if (ret)
			ALOGE("face svc set exp bias failed");
	}

	memcpy(&api_face->recg_param, param, sizeof(face_recognition_param_t));
	db_face_recognition_param_update(param);

	return ret;
}

/**
 * face_get_recg_param: get face recognition parameter
 * for dabasebase(sysinfo table)
 * @param: a pointer of face recognition parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_recg_param(face_recognition_param_t *param)
{
	return db_face_recognition_param_query(param);
}

/**
 * face_capture_2_feat: snapshot face picture, and get feature value
 * from face picture.
 * @info_f: a pointer of face feature value parameter
 * @cap: parameter of photo
 *
 * return 0 if success, otherwise return -1
 */
int face_capture_2_feat(face_info_t *info_f,
	face_capture_param_t *cap)
{
	int ret = 0;
	face_feat_opt_t opt;

	if (NULL == info_f || NULL == cap)
		return -1;
	memset(&opt, 0, sizeof(face_feat_opt_t));
	cap->face.head_pose_yaw = 20;
	cap->face.head_pose_pitch = 20;
	cap->face.head_pose_roll = 30;
	cap->face.max = 512;
	cap->face.min = 45;

	if (api_face) {
		cap->face.x0 = api_face->point_lt[0];
		cap->face.y0 = api_face->point_lt[1];
		cap->face.x1 = api_face->point_rb[0];
		cap->face.y1 = api_face->point_rb[1];
		ALOGD("%s:x0=%d y0=%d x1=%d y1=%d\n", __func__,
			  cap->face.x0, cap->face.y0,
			  cap->face.x1, cap->face.y1);
	}
	ret = face_get_feats_from_capture(api_face->svc_hdl, cap, &opt);
	if (ret == 0)
		memcpy(info_f, &opt.info, sizeof(face_info_t));
	return ret;
}

/**
 * face_photo_2_feat: snapshot face picture, and get feature value
 * from face picture.
 * @info_f: a pointer of face feature value parameter
 * @sec: timeout time
 *
 * return 0 if success, otherwise return -1
 */
int face_photo_2_feat(face_info_t *info_f, int sec)
{
	int ret = 0;
	face_feat_opt_t opt;
	face_capture_param_t cap;

	if (NULL == info_f)
		return -1;
	memset(&opt, 0, sizeof(face_feat_opt_t));
	memset(&cap, 0, sizeof(face_capture_param_t));
	cap.timeout_sec = sec;
	cap.face.head_pose_yaw = 20;
	cap.face.head_pose_pitch = 20;
	cap.face.head_pose_roll = 30;
	cap.face.max = 512;
	cap.face.min = 45;

	if (api_face) {
		cap.face.x0 = api_face->point_lt[0];
		cap.face.y0 = api_face->point_lt[1];
		cap.face.x1 = api_face->point_rb[0];
		cap.face.y1 = api_face->point_rb[1];

		ALOGD("%s:x0=%d y0=%d x1=%d y1=%d\n", __func__,
			  cap.face.x0, cap.face.y0,
			  cap.face.x1, cap.face.y1);
	}
#if 0
	cap.face.x0 = 445;
	cap.face.y0 = 150;
	cap.face.x1 = 445 + 390;
	cap.face.y1 = 150 + 394;
#endif
	ret = face_get_feats_from_capture(api_face->svc_hdl, &cap, &opt);
	if (ret == 0)
		memcpy(info_f, &opt.info, sizeof(face_info_t));

	return ret;
}

int face_stop_photo_2_feat(void)
{
	return face_stop_feats_from_capture(api_face->svc_hdl);
}

/**
 * face_picture_2_feat: get feature value from picture.
 * @path: picture file name
 * @info_f: a pointer of face feature value parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_picture_2_feat(char *path, face_info_t *info_f)
{
	int ret = 0;
	face_image_file_t image_file;
	face_feat_opt_t opt;

	if (path == NULL || NULL == info_f)
		return -1;
	memset(&image_file, 0, sizeof(face_image_file_t));
	memset(&opt, 0, sizeof(face_feat_opt_t));
	strcpy(image_file.image_path, path);
	ret = face_get_feats_from_image_file(
		api_face->svc_hdl, &image_file, &opt);
	ALOGE("%s: face_picture_2_feat ret = %d\n", __func__, ret);
	if (ret == 0)
		memcpy(info_f, &opt.info, sizeof(face_info_t));

	return ret;
}

/**
 * face_start: start face module
 *
 * return 0 if success, otherwise return -1
 */
int face_start(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;
	ret = face_svc_start(api_face->svc_hdl);

	if (ret) {
		ALOGE("start face service failed\n");
		return -1;
	}

	return 0;
}

/**
 * face_stop: stop face module
 *
 * return 0 if success, otherwise return -1
 */
int face_stop(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;
	ret = face_svc_stop(api_face->svc_hdl);

	if (ret) {
		ALOGE("stop face service failed\n");
		return -1;
	}

	return 0;
}

/**
 * face_pause: pause face serivce
 *
 * return 0 if success, otherwise return -1
 */
int face_pause(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;
	ret = face_svc_pause(api_face->svc_hdl);

	if (ret) {
		ALOGE("stop face service failed\n");
		return -1;
	}

	return 0;
}

/**
 * face_resume: resume face serivce
 *
 * return 0 if success, otherwise return -1
 */
int face_resume(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;
	ret = face_svc_resume(api_face->svc_hdl);

	if (ret) {
		ALOGE("stop face service failed\n");
		return -1;
	}

	return 0;
}

/**
 * face_pause_status: get face service status
 *
 * return 1 pause, 0 running
 */
int face_pause_status(void)
{
	if (!api_face || !api_face->svc_hdl)
		return -1;
	return face_svc_pause_status(api_face->svc_hdl);
}

/**
 * face_set_qrcode_cb: set qrcode function
 * @cb: a pointer of qrcode callback function
 *
 * return 0 if success, otherwise return -1
 */
int face_set_qrcode_cb(qrcode_cb cb)
{
	if (!api_face)
		return -1;
	api_face->qr_cb = cb;

	return 0;
}

/**
 * face_set_note_cb: set face recg function
 * @cb: a pointer of face recg callback function
 *
 * return 0 if success, otherwise return -1
 */
int face_set_note_cb(note_cb cb)
{
	if (!api_face)
		return -1;
	api_face->note_cb = cb;

	return 0;
}

/**
 * face_set_cb: set face callback function
 * @cb: a pointer of face callback function
 *
 * return 0 if success, otherwise return -1
 */
int face_set_cb(face_cb cb)
{
	if (!api_face)
		return -1;
	api_face->cb = cb;

	return 0;
}

/*For AX debug*/

/**
 * get_rec_name: get record file name
 * @save_path: save path
 * @type: record type
 * @p_file_name: a pointer of save file name
 * @file_str_max_len: length of file name buffer
 *
 * return 0 if success, otherwise return -1
 */
static int get_rec_name(const char *save_path, const char *type,
			char *p_file_name, int file_str_max_len)
{
	DIR *dir;
	int ret;
	char str_date[64];
	char str_time[64];
	char dir_name[64] = {'\0'};

	/* Create folder */
	time_get_cur_date(str_date);
	snprintf(dir_name, sizeof(dir_name), "%s/%s",
			save_path, str_date);
	dir = opendir(dir_name);
	if (dir == NULL) {
		ret = mk_mul_dirs(dir_name);
		if (ret != 0) {
			ALOGE("Create multiple folder: %s fail\n", dir_name);
			return -1;
		}
	} else
		closedir(dir);

	/* Get current time string */
	time_get_cur_time_ret_str(str_time, sizeof(str_time));
	snprintf(p_file_name, file_str_max_len, "%s/%s-face-%s.mp4",
				dir_name, type, str_time);

	return 0;
}

/**
 * face_record_try: record start
 *
 * return none
 */
void face_record_try(void)
{
	char file_name[FILENAME_MAX];
	float free_size = 0.0;
	int ret;

	if (NULL == api_face) {
		ALOGE("api_face == NULL");
		return;
	}

	/* Detecting the available space for the /data partition */
	ret = get_disk_free("/data", &free_size);
	if (ret != 0) {
		ALOGE("[%s:%d] Get disk status fail %s\n",
				__func__, __LINE__, strerror(errno));
		return;
	}

	ALOGD("/data free_size: %fM", free_size);
	if (free_size < FACE_RECORD_FREE_THRESHOLD) {
		if (api_face->record_cb)
			api_face->record_cb(FACE_RECORD_EVENT_STORAGE_FULL,
					NULL);
		return;
	}

	/* Get a file name */
	memset(file_name, 0, sizeof(file_name));
	ret = get_rec_name(FACE_RECORD_PATH, "rgb",
				file_name, sizeof(file_name));
	if (ret != 0) {
		ALOGE("[%s:%d] Get pic name fail\n", __func__, __LINE__);
		return;
	}
	ALOGD("rgb record start file:%s", file_name);
	ret = face_svc_start_rec(FACE_CAMERA_RGB, NULL, file_name);
	if (ret) {
		ALOGE("record start failed");
		return;
	}

	memset(file_name, 0, sizeof(file_name));
	ret = get_rec_name(FACE_RECORD_PATH, "nir",
				file_name, sizeof(file_name));
	if (ret != 0) {
		ALOGE("[%s:%d] Get pic name fail\n",
					__func__, __LINE__);
		return;
	}
	ALOGD("nir record start file:%s", file_name);
	ret = face_svc_start_rec(FACE_CAMERA_NIR, NULL, file_name);
	if (ret) {
		ALOGE("record start failed");
		return;
	}

	if (api_face->record_cb)
		api_face->record_cb(FACE_RECORD_EVENT_START, NULL);
}

/**
 * face_record_stop_try: record stop
 *
 * return none
 */
void face_record_stop_try(void)
{
	int ret;

	if (NULL == api_face) {
		ALOGE("api_face == NULL");
		return;
	}

	ALOGD("record stop");
	ret = face_svc_stop_rec(FACE_CAMERA_RGB);
	if (ret) {
		ALOGE("rgb record stop failed");
		return;
	}

	ret = face_svc_stop_rec(FACE_CAMERA_NIR);
	if (ret) {
		ALOGE("nir record stop failed");
		return;
	}

	if (api_face->record_cb)
		api_face->record_cb(FACE_RECORD_EVENT_STOP, NULL);
}

/**
 * face_snapshot: snapshot picture
 * @cam_id: camera id (0/1: rgb/nir)
 *
 * return 0 if success, otherwise return -1
 */
int face_snapshot(int cam_id, char *file_name)
{
	return face_take_picture_file(cam_id, file_name);
}

/**
 * face_get_ord_map_params: get camera ord map parameter
 * @ord_map_params: a pointer of ord map parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_ord_map_params(map_params_t *ord_map_params)
{
	return face_svc_get_ord_map_params(ord_map_params);
}

/**
 * face_get_mod_map_params: get camera mod map parameter
 * @ord_map_params: ord map parameter
 * @mod_map_params: a pointer of mod map parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_mod_map_params(map_params_t *ord_map_params,
					map_params_t *mod_map_params)
{
	return face_svc_get_mod_map_params(NULL, mod_map_params);
}

/**
 * face_get_map_params: get camera map parameter
 * @map_params: a pointer of map parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_map_params(map_params_t *map_params)
{
	int i = 0;
	int ret = 0;
	face_map_params_t face_map_param;

	memset(&face_map_param, 0, sizeof(face_map_params_t));
	ret = system_get_private_data(RGBNIR_MAP_OFFSET, (char *)&face_map_param,
		sizeof(face_map_params_t));
	if (ret) {
		ALOGE("face get private face_map_param failed");
		return -1;
	}

	ALOGV("RGBNIR_MAP_CHECK = 0x%02x", face_map_param.check);
	if (face_map_param.check != RGBNIR_MAP_CHECK) {
		return -1;
	}

	for (i = 0; i < 5; i++) {
		map_params->x_map[i] = face_map_param.x_map[i];
		map_params->y_map[i] = face_map_param.y_map[i];
	}

	return 0;
}

int face_take_mod_pic(int x, int y, int w, int h)
{
	return face_svc_take_mod_pic(api_face->svc_hdl, x, y, w, h);
}

int face_cal_mod_map(map_params_t *map)
{
	return face_svc_cal_mod_map(api_face->svc_hdl, map);
}

/**
 * face_get_test_log: get test log parameter
 * @test_log: a pointer of int parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_get_test_log_param(int *test_log)
{
	*test_log = api_face->test_log_enable;

	return 0;
}

/**
 * face_set_test_log: set test log parameter
 * @test_log: a pointer of int parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_set_test_log_param(int test_log)
{
	api_face->test_log_enable = test_log;

	return 0;
}

int face_get_recg_timeout(void)
{
	return api_face->recg_param.recg_timeout;
}

/**
 * set_dump_img_config: set attendance image parameter
 *
 */
void set_dump_img_config(void)
{
	dump_img_config_t config_t;
	memset(&config_t, 0, sizeof(dump_img_config_t));
	memset(&api_face->attendance_info, 0, sizeof(attendance_param_t));
	ui_device_get_attendance_param(&api_face->attendance_info);
	face_get_dump_img_config(&config_t);
	config_t.dump_save_img =
		api_face->attendance_info.saved_attendance_pic;
	config_t.dump_match_face = 1;
	face_set_dump_img_config(&config_t);
}

/**
 * set_gate_dump_img_config: set gate image parameter
 *
 */
void set_gate_dump_img_config(void)
{
	dump_img_config_t config_t;
	memset(&config_t, 0, sizeof(dump_img_config_t));
	memset(&api_face->checkin_info, 0, sizeof(checkin_param_t));
	ui_device_get_checkin_param(&api_face->checkin_info);
	face_get_dump_img_config(&config_t);

	config_t.dump_unqualified_frame = 0;
	config_t.dump_unqualified_face = 0;
	if (!api_face->checkin_info.save_large_pic && !api_face->checkin_info.save_small_pic) {
		config_t.dump_save_img = 0;
		config_t.dump_match_frame = 1;
		config_t.dump_match_face = 1;
		if (api_face->checkin_info.save_stranger_record) {
			config_t.dump_unmatch_frame = 1;
			config_t.dump_unmatch_face = 1;
		} else {
			config_t.dump_unmatch_frame = 0;
			config_t.dump_unmatch_face = 0;
		}
	} else {
		config_t.dump_save_img = 1;
		config_t.dump_match_frame = api_face->checkin_info.save_large_pic;
		config_t.dump_match_face = api_face->checkin_info.save_small_pic;
		if (api_face->checkin_info.save_stranger_record) {
			config_t.dump_unmatch_frame = api_face->checkin_info.save_large_pic;
			config_t.dump_unmatch_face = api_face->checkin_info.save_small_pic;
		} else {
			config_t.dump_unmatch_frame = 0;
			config_t.dump_unmatch_face = 0;
		}
	}
	config_t.det_timeout = api_face->recg_param.recg_timeout;

	face_set_dump_img_config(&config_t);
}

/**
 * face_set_platform_param: set platform parameter
 * @enable: a pointer of int parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_set_platform_param(int enable)
{
	device_platform_info_t platform_t;

	db_device_platform_info_query(&platform_t);
	ALOGD("%s:%d >> platform_enable=%d, platform_ip=%s",
		__func__, __LINE__, api_face->platform_enable,
		api_face->platform_ip);
	api_face->platform_enable = platform_t.platform_enable;
	memcpy(api_face->platform_ip, platform_t.platform_ip, 64);

	return 0;
}

/**
 * face_set_platform_hearbeat: set platform hearbeat
 *
 */
void face_set_platform_hearbeat(void)
{
	api_face->platform_hearbeat = time_get_us();
}

static int face_recg_param_init(void)
{
	face_recognition_param_t recg_param_init;

	memset(&recg_param_init, 0, sizeof(face_recognition_param_t));
	db_face_recognition_param_query(&recg_param_init);
	face_set_recg_param(&recg_param_init, 1);

	return 0;
}

/**
 * face_set_register_mode: set register mode parameter
 * @param: register mode parameter
 *
 * return 0 if success, otherwise return -1
 */
int face_set_register_mode(int param)
{
	api_face->face_register = param;

	return 0;
}

int ui_get_userid_max()
{
	int uid = 0;
	pthread_mutex_lock(&api_face->db_lock);
	api_face->uid.uids = api_face->face_db->userid;
	pthread_mutex_unlock(&api_face->db_lock);

	for (int i = 0; i < api_face->uid.cnt; i++) {
		uid = atoi(api_face->uid.uids[i].userid);
		if (uid > api_face->uid_max) {
			api_face->uid_max = uid;
		}
	}
	return api_face->uid_max;
}

/**
 * face_init: initialize face module
 *
 * return 0 if success, otherwise return -1
 */
int face_init(void)
{
	map_params_t map_params;
	int ret;
	device_platform_info_t platform_t;
	main_setting_param_t main_setting_t;

	/*int hdr_sw;*/
	api_face = calloc(sizeof(api_face_t), 1);

	if (!api_face) {
		ALOGE("alloc mem for api_face");
		return -1;
	}
	api_face->cb_flag = -1;
	api_face->uid_max = 0;
	/* For face manager */
	INIT_LIST_HEAD(&api_face->face_list);

	if (pthread_mutex_init(&api_face->face_lock, NULL)) {
		ALOGE("init face_lock failed");
		return -1;
	}
	if (pthread_mutex_init(&api_face->db_lock, NULL)) {
		ALOGE("init db_lock failed");
		return -1;
	}

	db_device_platform_info_query(&platform_t);
	api_face->platform_enable = platform_t.platform_enable;
	memcpy(api_face->platform_ip, platform_t.platform_ip, 64);
	api_face->platform_hearbeat = 0;

	/* Initialize face service */
	api_face->svc_hdl = face_svc_init();
	if (!api_face->svc_hdl) {
		ALOGE("face svc init failed");
		return -1;
	}
	/* Load and register face features to service */
	ret = face_db_update();
	if (ret) {
		ALOGE("face db init failed");
		return -1;
	}

	ret = face_db_update_admin();
	if (ret) {
		ALOGE("face db admin init failed");
		return -1;
	}

	ret = face_task_init();
	if (ret) {
		ALOGE("face task init failed");
		return -1;
	}

	ret =sound_svc_init();
	if (ret) {
		ALOGE("sound init failed");
		return -1;
	}

#if 1
	ret = face_checkin_init();
	if (ret) {
		ALOGE("checkin init failed");
		return -1;
	}
#endif

	/* access time init */
	memset(api_face->access_time, 0, 128);
	db_access_time_query(api_face->access_time);
	api_face->access_type = db_access_type_query();
	/* language init */
	memset(&api_face->lang_t, 0, sizeof(device_lang_param_t));
	db_device_lang_param_query(&api_face->lang_t);
	/* display control init */
	memset(&main_setting_t, 0, sizeof(main_setting_param_t));
	db_main_setting_param_query(&main_setting_t);
	api_face->display_ctl.name_display = main_setting_t.name_display;
	api_face->display_ctl.name_format = main_setting_t.name_format;

/*
	hdr_sw = isp_get_config();

	if (hdr_sw < 0)
	ALOGE("isp get config failed");

	ret = set_rgb_sensor_hdr(0, hdr_sw);

	if (ret)
	ALOGE("hdr fail!");

*/

	ret = face_recg_param_init();
	if (ret) {
		ALOGE("face recg param init failed");
		return -1;
	}

#if ATTENDANCE_MODE == 2
	/* do nothings */
#elif ATTENDANCE_MODE == 1
	set_dump_img_config();
#else
	set_gate_dump_img_config();
#endif

#if ATTENDANCE_MODE == 0
	face_svc_set_temp_measure_rect(api_face->svc_hdl,
			TEMP_LEFT, TEMP_TOP, TEMP_RIGHT, TEMP_BOTTOM);
#endif

	memset(&map_params, 0, sizeof(map_params_t));
	ret = face_get_map_params(&map_params);
	if (ret == 0) {
		face_svc_set_map_params(api_face->svc_hdl, &map_params);
	}

	/* Register face detect callback function */
	ret = face_register_det_cb(api_face->svc_hdl, api_face_det_cb);

	if (ret) {
		ALOGE("register face detect faild\n");
		return -1;
	}
	/* Register face feat detect callback function */
	ret = face_register_recg_cb(api_face->svc_hdl, api_face_recg_cb);

	if (ret) {
		ALOGE("register face feat detect faild\n");
		return -1;
	}
	/* Register punch record callback funciton */
	ret = face_register_record_cb(api_face->svc_hdl, face_save_record);
	file_system_sync();

	if (ret) {
		ALOGE("register record save cb faild\n");
		return -1;
	}
	/* Start face detect serve */
	ret = face_svc_start(api_face->svc_hdl);

	if (ret) {
		ALOGE("start face detect serve faild\n");
		return -1;
	}

	return 0;
}

/**
 * face_is_runnig: is the face module had been start
 *
 * return 0 if started, otherwise return -1
 */
int face_is_runnig(void)
{
	if (api_face && api_face->cb_flag != -1)
		return 0;
	else
		return -1;
}

/**
 * face_deinit: deinitialize face module
 *
 * return 0 if success, otherwise return -1
 */
void face_deinit(void)
{
	if (!api_face || !api_face->svc_hdl) {
		ALOGE("face deinit failed, svc_handle == NULL");
		return;
	}
	face_register_recg_cb(api_face->svc_hdl, NULL);
	face_register_det_cb(api_face->svc_hdl, NULL);
	face_register_record_cb(api_face->svc_hdl, NULL);

	/*stop face server*/
	face_svc_stop(api_face->svc_hdl);

	/*deinitial face server*/
	face_svc_deinit(api_face->svc_hdl);

	/* free db malloc memory */
	face_db_free(api_face->face_db);

	/* deinit sound svc */
	sound_svc_deinit();
}

