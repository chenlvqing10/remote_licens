/*
 * face.c - drawing face track box of face detect
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define LOG_TAG		"api-face"
#include <log/log.h>

#include "face.h"
#include "face_svc.h"

#include <sys/queue.h>
#include "file_ops.h"
#include "db_person_info.h"
#include "db_persistence.h"
#include "common_record_info.h"
#include "db_punch_record.h"
#include "time_conv.h"
#include "recognition.h"
#include "system_develop_sel.h"
#include "config.h"
#include "lombo_sound_play.h"
#include "record_auto_delete.h"
#include "light.h"
#include "system_screen.h"
#include "person.h"
#include "lombo_malloc.h"
#include "face_svc_isp.h"
#include "display.h"
#include "face_svc_media.h"
#include "video_recorder.h"
#include "system_private.h"

#define FACE_DEBUG	0

#define FACE_RECORD_PHOTO_PATH	"/data/face_record/"

#define PASS_SOUND_PATH	"/usr/share/face-recg/res/sound/boot.wav"

#define RGBNIR_MAP_CHECK  0x5B
#define RGBNIR_MAP_OFFSET  0x1024

typedef struct face_map_params {
	int check;
	double x_map[5];
	double y_map[5];
} face_map_params_t;

typedef struct _name {
	char name[PERSON_NAME_MAX];
} name_t;

typedef struct _employee_number {
	char num[PERSON_EMPLOYEE_NUM_MAX];
} employee_number_t;

typedef struct tag_id {
	int id;
} person_id_t;

typedef struct face_db {
	int count;
	person_id_t *id;
	employee_number_t *num;
	name_t *name;
	feat_t *feat;
	float *norm;
	kpts_t *kpts;
} face_db_t;

typedef struct _api_face {
	void *svc_hdl;
	face_cb cb;
	struct list_head face_list;
	pthread_mutex_t face_lock;

	pthread_mutex_t db_lock;
	int face_num;
	face_db_t *face_db;
	recognition_para_t recg_param;
	develop_sel_para_t develop_param;
	char debug_switch;

	/* config */
	int nir_box_debug;
	int recg_time_debug;
	int recg_timeout;
	int face_sound_play;

	/* sound play thread */
	pthread_mutex_t sound_lock;
	pthread_cond_t sound_cond;
	pthread_t sound_play_thrd;

	/* face capture */
	pthread_mutex_t capture_lock;
	pthread_cond_t capture_cond;
	int capture_frame_ready;
	int capture_run;
	face_frame_t capture_frame;
	char capture_feat[FACE_FEAT_SIZE + 200];
	float yaw;
	float pitch;
	float roll;
} api_face_t;

api_face_t *api_face;

static void face_update(api_face_t *api_face, face_rects_t *f_rects,
					face_frame_t *frame)
{
	face_t *face, *face_tmp;
	int i;

	/* no face is detected, clean face list */
	if (f_rects->cnt == 0) {
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
		memcpy(face->head_pose, f_rects->p_rect[i].head_pose, sizeof(face->head_pose));
		memcpy(&face->attr, &f_rects->p_rect[i].attr, sizeof(face->attr));
		memset(face->nir_rect, 0, sizeof(face->nir_rect));
	}
	pthread_mutex_unlock(&api_face->face_lock);
}

int faces_set(api_face_t *api_face, face_frame_t *frame, face_info_t *f_info)
{
	face_t *face;

	pthread_mutex_lock(&api_face->face_lock);
	list_for_each_entry(face, &api_face->face_list, node) {
		if (face->id == f_info->rect.id) {
			face->max_id = f_info->feat.max_id;
			face->living = f_info->feat.living_body;
			face->score = f_info->feat.score;
			/* ALOGD("nir x0:%d, y0:%d, x1:%d, y1:%d",
					f_info->nir_rect.rect[0],
					f_info->nir_rect.rect[1],
					f_info->nir_rect.rect[2],
					f_info->nir_rect.rect[3]); */
			memcpy(face->nir_rect, f_info->nir_rect.rect,
						sizeof(f_info->nir_rect.rect));
			face->nir_rect[0] = f_info->nir_rect.rect[0] / 2;
			face->nir_rect[1] = f_info->nir_rect.rect[1] / 2;
			face->nir_rect[2] = f_info->nir_rect.rect[2] / 2;
			face->nir_rect[3] = f_info->nir_rect.rect[3] / 2;
			pthread_mutex_lock(&api_face->db_lock);
			if (face->max_id >= 0 && face->max_id < api_face->face_db->count)
				memcpy(face->name, &api_face->face_db->name[face->max_id],
						sizeof(face->name));
			pthread_mutex_unlock(&api_face->db_lock);
			face->det_ts = (frame->det_end - frame->det_begin) / 1000;
			face->recg_ts = (f_info->recg_end - f_info->recg_begin) / 1000;
			face->cmp_ts = (f_info->cmp_end - f_info->cmp_begin) / 1000;
			face->living_ts = (f_info->living_end - f_info->living_begin)
						/ 1000;

			/* play sound if pass, play just once for the same id */
			if (f_info->feat.score >= api_face->recg_param.threshold_score &&
				!face->sound_play) {
				if (!api_face->recg_param.live_detect
					|| f_info->feat.living_body == 1) {
					face->sound_play = 1;
					pthread_mutex_lock(&api_face->sound_lock);
					pthread_cond_signal(&api_face->sound_cond);
					pthread_mutex_unlock(&api_face->sound_lock);
				}
			}

			/* for debug: calculate recg time */
			if (face->score >= api_face->recg_param.threshold_score
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

void faces_put(faces_t *faces)
{
	if (faces) {
		if (faces->faces)
			free(faces->faces);
		free(faces);
	}
}

faces_t *faces_get(void)
{
	return _faces_get(api_face);
}

static int face_check_head_pose(float yaw, float pitch, float roll)
{
	if (yaw < -api_face->yaw || yaw > api_face->yaw)
		return -1;

	if (pitch < -api_face->pitch || pitch > api_face->pitch)
		return -1;

	if (roll < -api_face->roll || roll > api_face->roll)
		return -1;

	return 0;
}

void *api_face_det_cb(face_frame_t *frame, face_rects_t *f_rects)
{
	face_rect_t *rect;
	int ret;

	if (!f_rects)
		return NULL;

	face_update(api_face, f_rects, frame);

	if (f_rects->cnt > 0) {
		display_screen_saver_off();
		screen_count_reset();
		screen_saver_count_reset();
		system_filling_light_reset_count();
	}

	if (f_rects->cnt == 1) {
		pthread_mutex_lock(&api_face->capture_lock);
		if (!api_face->capture_frame_ready && api_face->capture_run) {
			rect = &f_rects->p_rect[0];
			ret = face_check_head_pose(rect->head_pose[0], rect->head_pose[1],
							rect->head_pose[2]);
			if (!ret) {
				api_face->capture_frame.viraddr[0] =
					(void *)lombo_malloc_cached(frame->size[0] +
					frame->size[1] + frame->size[2]);
				if (api_face->capture_frame.viraddr[0]) {
					api_face->capture_frame.viraddr[1] =
						api_face->capture_frame.viraddr[0] + frame->size[0];
					api_face->capture_frame.viraddr[2] =
						api_face->capture_frame.viraddr[0] + frame->size[0] +
						frame->size[1];
					api_face->capture_frame.phyaddr[0] =
						lombo_vir_to_phy(api_face->capture_frame.viraddr[0]);
					api_face->capture_frame.phyaddr[1] =
						lombo_vir_to_phy(api_face->capture_frame.viraddr[1]);
					api_face->capture_frame.phyaddr[2] =
						lombo_vir_to_phy(api_face->capture_frame.viraddr[2]);
					memcpy(api_face->capture_frame.viraddr[0],
							frame->viraddr[0], frame->size[0]);
					memcpy(api_face->capture_frame.viraddr[1],
							frame->viraddr[1], frame->size[1]);
					memcpy(api_face->capture_frame.viraddr[2],
							frame->viraddr[2], frame->size[2]);
					api_face->capture_frame.width = frame->width;
					api_face->capture_frame.height = frame->height;
					api_face->capture_frame.size[0] = frame->size[0];
					api_face->capture_frame.size[1] = frame->size[1];
					api_face->capture_frame.size[2] = frame->size[2];
					api_face->capture_frame_ready = 1;
					pthread_cond_signal(&api_face->capture_cond);
				}
			}
		}
		pthread_mutex_unlock(&api_face->capture_lock);
	}

	if (api_face->cb) {
		faces_t *faces_temp = faces_get();

		api_face->cb(faces_temp);

		faces_put(faces_temp);
	}

	return NULL;
}

void *api_face_recg_cb(face_frame_t *frame, face_info_t *f_info)
{
	if (!f_info)
		return NULL;

	faces_set(api_face, frame, f_info);

	/* for nir detect debug */
	if (api_face->nir_box_debug && api_face->cb)
		api_face->cb(NULL);

#if 0/* for mis recg debug */

	#define REGISTER_PIC_SAVE_PATH		"/data/face_register"

	char pic_path[64];
	int ret;
	long long ts;
	char name[32];

	if (f_info->feat.score <= 0.5f)
		return NULL;

	ret = face_db_get_name(f_info->feat.max_id, name);
	if (ret) {
		ALOGE("face debug get name failed");
		return NULL;
	}

	if (!strcmp(name, "M004"))
		return NULL;

	/* if (f_info->feat.max_id != 63)
		return NULL; */
	ALOGD("mis recg max id = %d, name:%s", f_info->feat.max_id, name);

	ts = time_get_us();
	snprintf(pic_path, sizeof(pic_path), "%s/%s-%lld.yuv",
					REGISTER_PIC_SAVE_PATH, "mis-recg", ts);
	ALOGD("no face pic save path:%s", pic_path);
	ret = file_write(pic_path, frame->viraddr, frame->size);
	if (ret)
		ALOGE("file write failed\n");
	snprintf(pic_path, sizeof(pic_path), "%s/%s-%lld.yuv",
					REGISTER_PIC_SAVE_PATH, "mis-recg-y", ts);
	ALOGD("no face pic save path:%s", pic_path);
	ret = file_write(pic_path, frame->viraddr, frame->width * frame->height);
	if (ret)
		ALOGE("file write failed\n");
	snprintf(pic_path, sizeof(pic_path), "%s/%s-%lld.yuv",
					REGISTER_PIC_SAVE_PATH, "mis-recg-uv", ts);
	ALOGD("no face pic save path:%s", pic_path);
	ret = file_write(pic_path, frame->viraddr + frame->width * frame->height,
						frame->width * frame->height / 2);
	if (ret)
		ALOGE("file write failed\n");
#endif

	return NULL;
}

int face_start(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;

	ret = face_svc_start(api_face->svc_hdl);
	if (ret) {
		printf("start face service failed\n");
		return -1;
	}

	return 0;
}

int face_stop(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;

	ret = face_svc_stop(api_face->svc_hdl);
	if (ret) {
		printf("stop face service failed\n");
		return -1;
	}

	return 0;
}

int face_pause(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;
	ret = face_svc_pause(api_face->svc_hdl);
	if (ret) {
		printf("stop face service failed\n");
		return -1;
	}

	return 0;
}

int face_resume(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;
	ret = face_svc_resume(api_face->svc_hdl);
	if (ret) {
		printf("stop face service failed\n");
		return -1;
	}

	return 0;
}

int face_pause_status(void)
{
	if (!api_face || !api_face->svc_hdl)
		return -1;

	return face_svc_pause_status(api_face->svc_hdl);
}

int face_set_cb(face_cb cb)
{
	if (!api_face)
		return -1;

	api_face->cb = cb;

	return 0;
}

int face_get_info(face_frame_t *frame, int cnt, face_info_t *info)
{
	return face_recg_block(api_face->svc_hdl, frame, cnt, info);
}


int face_db_get_count(void)
{
	int count;

	if (!api_face)
		return -1;

	pthread_mutex_lock(&api_face->db_lock);
	count = api_face->face_db->count;
	pthread_mutex_unlock(&api_face->db_lock);

	return count;
}

void face_db_get_kpts(float *kpts, char *face_key_points)
{
	char *strtok_tmp_ptr;
	char *delim = ":", *kpt_str;
	int i = 0;

	kpt_str = strtok_r(face_key_points, delim, &strtok_tmp_ptr);
	while (kpt_str) {
		kpts[i++] = atof(kpt_str);
		kpt_str = strtok_r(NULL, delim, &strtok_tmp_ptr);
	}
}

face_db_t *face_db_load(void)
{
	person_info_t *p_tmp = NULL;
	face_db_t *face_db = NULL;
	ee_queue_t tmp_head;
	int idx, feat_cnt, ret;

	ALOGD("face load db\n");

	face_db = malloc(sizeof(face_db_t));
	if (!face_db) {
		ALOGE("alloc face_db_t failed");
		return NULL;
	}
	memset(face_db, 0x00, sizeof(face_db_t));

	feat_cnt = db_person_info_count();
	if (feat_cnt == 0) {
		ALOGD("%s: face db is empty", __func__);
		return face_db;
	}
	face_db->count = feat_cnt;
	ALOGD("%d person number in db", feat_cnt);

	face_db->feat = malloc(sizeof(feat_t) * feat_cnt);
	if (!face_db->feat) {
		ALOGE("%s: malloc feat memery failed", __func__);
		goto free_res;
	}

	face_db->num = malloc(sizeof(employee_number_t) * feat_cnt);
	if (!face_db->num) {
		ALOGE("%s: malloc employee_number_t failed", __func__);
		goto free_res;
	}

	face_db->name = malloc(sizeof(name_t) * feat_cnt);
	if (!face_db->name) {
		ALOGE("%s: malloc feat memery failed", __func__);
		goto free_res;
	}

	face_db->id = malloc(sizeof(person_id_t) * feat_cnt);
	if (!face_db->id) {
		ALOGE("%s: malloc feat memery failed", __func__);
		goto free_res;
	}

	/* 2 * 512 */
	face_db->norm = malloc(sizeof(float) * feat_cnt * 2);
	if (!face_db->norm) {
		ALOGE("%s: malloc norm failed", __func__);
		goto free_res;
	}

	face_db->kpts = malloc(sizeof(kpts_t) * feat_cnt);
	if (!face_db->kpts) {
		ALOGE("%s: malloc kpts failed", __func__);
		goto free_res;
	}

	ret = db_person_info_query_begin("", 0, feat_cnt, &tmp_head);
	if (!ret) {
		ALOGE("%s: query failed-%d", __func__, ret);
		db_person_info_query_end(&tmp_head);
		goto free_res;
	}

	if (ee_queue_empty(&tmp_head)) {
		db_person_info_query_end(&tmp_head);
		goto free_res;
	} else {
		idx = 0;
		ee_queue_foreach(p_tmp, &tmp_head, person_info_t, queue) {
			if (idx < feat_cnt) {
				memcpy(face_db->num[idx].num, p_tmp->employee_number,
					sizeof(employee_number_t));
				memcpy(face_db->feat[idx].values,
					p_tmp->face_features, FACE_FEAT_SIZE);
				memcpy(face_db->name[idx].name,
					p_tmp->name, PERSON_NAME_MAX);
				face_db->id[idx].id = p_tmp->id;
				face_db_get_kpts(face_db->kpts[idx].kpts,
							p_tmp->face_key_points);
				face_db->norm[idx * 2] = (float)p_tmp->face_norm[0];
				face_db->norm[idx * 2 + 1] = (float)p_tmp->face_norm[1];
				idx++;
			}
		}
	}
	db_person_info_query_end(&tmp_head);

	return face_db;

free_res:
	if (face_db) {
		if (face_db->id)
			free(face_db->id);
		if (face_db->name)
			free(face_db->name);
		if (face_db->feat)
			free(face_db->feat);
		free(face_db);
	}

	return NULL;
}

void face_db_free(face_db_t *face_db)
{
	if (face_db) {
		if (face_db->id)
			free(face_db->id);
		if (face_db->num)
			free(face_db->num);
		if (face_db->name)
			free(face_db->name);
		if (face_db->feat)
			free(face_db->feat);
		if (face_db->kpts)
			free(face_db->kpts);
		if (face_db->norm)
			free(face_db->norm);
		free(face_db);
	}
}

void face_clear_face_list()
{
	face_t *face, *face_tmp;
	pthread_mutex_lock(&api_face->face_lock);
	list_for_each_entry_safe(face, face_tmp, &api_face->face_list, node) {
		list_del(&face->node);
		free(face);
		api_face->face_num--;
	}
	pthread_mutex_unlock(&api_face->face_lock);
}

void face_clear_face_list_by_name(const char *name)
{
	face_t *face, *face_tmp;
	pthread_mutex_lock(&api_face->face_lock);
	list_for_each_entry_safe(face, face_tmp, &api_face->face_list, node) {
		if (strcmp(name, face->name) == 0) {
			list_del(&face->node);
			free(face);
			api_face->face_num--;
		}
	}
	pthread_mutex_unlock(&api_face->face_lock);
}

int face_db_update(void)
{
	face_db_t *face_db;
	feats_t feats;
	int ret;

	face_db = face_db_load();
	if (!face_db) {
		ALOGE("face db load failed");
		return -1;
	}

	pthread_mutex_lock(&api_face->db_lock);
	/* update face feattures */
	feats.cnt = face_db->count;
	feats.feat = face_db->feat;
	feats.kpts = face_db->kpts;
	feats.norm = face_db->norm;
	ret = face_register_face_feats(api_face->svc_hdl, &feats);
	if (ret) {
		pthread_mutex_unlock(&api_face->db_lock);
		face_db_free(face_db);
		return -1;
	}

	/* free previous db */
	if (api_face->face_db)
		face_db_free(api_face->face_db);
	api_face->face_db = face_db;
	pthread_mutex_unlock(&api_face->db_lock);

	return 0;
}

int face_db_init(void)
{
	face_db_t *face_db;
	feats_t feats;
	int ret;

	face_db = face_db_load();
	if (!face_db) {
		ALOGE("face db load failed");
		return -1;
	}

	pthread_mutex_lock(&api_face->db_lock);
	feats.cnt = face_db->count;
	feats.feat = face_db->feat;
	feats.kpts = face_db->kpts;
	feats.norm = face_db->norm;
	ret = face_register_face_feats(api_face->svc_hdl, &feats);
	if (ret) {
		pthread_mutex_unlock(&api_face->db_lock);
		face_db_free(face_db);
		return -1;
	}
	api_face->face_db = face_db;
	pthread_mutex_unlock(&api_face->db_lock);

	return 0;
}

int face_db_get_name(int index, char *name, int name_size)
{
	face_db_t *face_db;

	if (!name)
		return -1;

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

int face_db_get_employee_num(int index, char *num, int num_size)
{
	face_db_t *face_db;

	if (!num)
		return -1;

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

	strncpy(num, face_db->num[index].num, num_size);
	pthread_mutex_unlock(&api_face->db_lock);

	return 0;
}

int face_db_get_person_id(int index)
{
	face_db_t *face_db;
	int id;

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

	id = face_db->id[index].id;
	pthread_mutex_unlock(&api_face->db_lock);

	return id;
}

float face_recg_threshold(void)
{
	return api_face->recg_param.threshold_score;
}

int face_live_detect(void)
{
	return api_face->recg_param.live_detect;
}

void face_recg_para_change_cb(recognition_para_t *recg_param)
{
	face_config_t config;
	int ret;

	ALOGD("face_recg_para_change_cb");
	memcpy(&api_face->recg_param, recg_param, sizeof(recognition_para_t));

	config.score_threshold = recg_param->threshold_score;
	config.living = recg_param->live_detect;
	config.min_face_pixel = recg_param->min_face_pixel;
	config.max_face_pixel = recg_param->max_face_pixel;
	config.max_track_faces = recg_param->max_track_faces;
	ret = face_svc_set_config2(api_face->svc_hdl, &config);
	if (ret)
		ALOGE("face set config failed");
}

void face_isp_para_change_cb(isp_para_t *isp_param)
{
	int ret;

	ret = set_rgb_sensor_hdr(0, isp_param->hdr_sw);
	if (ret)
		ALOGE("hdr fail!");
}

int face_debug_switch(void)
{
	return (int)(api_face->develop_param.debug_sw);
}

int face_recg_time_debug(void)
{
	return api_face->recg_time_debug;
}

int face_recg_timeout(void)
{
	return api_face->recg_timeout;
}

void face_develop_sel_para_change_cb(develop_sel_para_t *develop_param)
{
	ALOGD("face_develop_sel_para_change_cb");
	memcpy(&api_face->develop_param, develop_param, sizeof(develop_sel_para_t));
}

static void *face_save_record(face_frame_t *frame, face_record_t *record)
{
	punch_record_t punch_record;
	face_info_t *face_info;
	int ret = 0, id = 0;

	ALOGD("face save record");

	/* parameters check */
	if (!record) {
		ALOGE("%s: pointer is NULL\n", __func__);
		return NULL;
	}

	/* get person id in db */
	face_info = &record->face_info;
	id = face_db_get_person_id(face_info->feat.max_id);
	if (id < 0) {
		ALOGE("%s: get person id failed\n", __func__);
		return NULL;
	}

	/* fill person info */
	memset(&punch_record, 0x00, sizeof(punch_record_t));
	punch_record.fk_per_id = id;
	punch_record.match_score = face_info->feat.score;
	if (record->face_pass) {
		punch_record.punch_pass = 1;
		if (record->type == match_frame) {
			strncpy(punch_record.punch_live_small_pic_name, record->img.path[match_face],
				sizeof(punch_record.punch_live_small_pic_name));
		}
	} else if (record->face_refuse) {
		punch_record.punch_pass = 0;
		if (record->type == match_frame) {
			strncpy(punch_record.punch_live_small_pic_name, record->img.path[match_face],
				sizeof(punch_record.punch_live_small_pic_name));
		} else if (record->type == unmatch_frame) {
			strncpy(punch_record.punch_live_small_pic_name, record->img.path[unmatch_face],
				sizeof(punch_record.punch_live_small_pic_name));
		}
	}
	strncpy(punch_record.address, "TODO", sizeof(punch_record.address));
	punch_record.punch_time = time_get_us() / 1000;
	strncpy(punch_record.punch_live_large_pic_name, record->img.path[record->type],
				sizeof(punch_record.punch_live_large_pic_name));
	ret = db_punch_record_insert(&punch_record);
	if (ret) {
		ALOGE("%s: failed to insert record to database\n", __func__);
		return NULL;
	}

	return NULL;
}

void *get_face_handle(void)
{
	return api_face->svc_hdl;
}

static void *face_sound_play_thread(void *args)
{
	int ret;

	while (1) {
		pthread_mutex_lock(&api_face->sound_lock);
		pthread_cond_wait(&api_face->sound_cond, &api_face->sound_lock);
		pthread_mutex_unlock(&api_face->sound_lock);

		ret = lombo_sound_play(PASS_SOUND_PATH);
		if (ret)
			ALOGE("failed to play sound:%s", PASS_SOUND_PATH);
	}

	return NULL;
}

int face_sound_init(void)
{
	if (pthread_mutex_init(&api_face->sound_lock, NULL)) {
		ALOGE("init sound lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	if (pthread_cond_init(&api_face->sound_cond, NULL)) {
		ALOGE("init sound cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	if (pthread_create(&api_face->sound_play_thrd, NULL,
				face_sound_play_thread, (void *)api_face)) {
		ALOGE("create face sound play thread failed, errno: %d (%s)",
			errno, strerror(errno));
		return -1;
	}

	return 0;
}

int face_get_config(void)
{
	recognition_para_t *recg_param;
	develop_sel_para_t *develop_param;
	int ret;

	api_face->nir_box_debug = config_get_int("face", "nir_box_debug", 0);
	api_face->recg_time_debug = config_get_int("face", "recg_time_debug", 0);
	api_face->recg_timeout = config_get_int("face", "recg_timeout", 3);
	api_face->face_sound_play = config_get_int("face", "face_sound_play", 1);

	/* get recognition parameter */
	recg_param = &api_face->recg_param;
	ret = recognition_get_param(recg_param);
	if (ret) {
		ALOGE("recognition get param failed");
		return -1;
	}
	ALOGD("recognition param, threshold:%f, live:%d", recg_param->threshold_score,
					recg_param->live_detect);
	/* register callbak of recognition parameter */
	recognition_register_change_cb(face_recg_para_change_cb);

	/* get developer parameter */
	develop_param = &api_face->develop_param;
	ret = system_develop_sel_get_para(develop_param);
	if (ret) {
		ALOGE("get develop debug switch failed");
		return -1;
	}
	ALOGD("develop param, debug switch:%c", develop_param->debug_sw);

	/* register callbak of developer parameter */
	develop_sel_register_change_cb(face_develop_sel_para_change_cb);

	return 0;
}

int isp_get_config(void)
{
	isp_para_t isp_para;
	int ret;
	memset(&isp_para, 0x00, sizeof(isp_para));
	ret = isp_get_param(&isp_para);
	if (ret) {
		ALOGE("isp get param failed");
		return -1;
	}
	ALOGD("isp param, hdr:%d", isp_para.hdr_sw);
	/* register callbak of isp parameter */
	isp_register_change_cb(face_isp_para_change_cb);

	return isp_para.hdr_sw;
}

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

int face_reset_recg(void)
{
	int ret = 0;

	if (!api_face || !api_face->svc_hdl)
		return -1;

	ret = face_svc_reset_recg(api_face->svc_hdl);
	if (ret) {
		printf("clean track into fail\n");
		return -1;
	}

	return ret;
}

int face_init(void)
{
	face_config_t config;
	int ret;
	int hdr_sw;
	map_params_t map_params;

	api_face = calloc(sizeof(api_face_t), 1);
	if (!api_face) {
		ALOGE("alloc mem for api_face");
		return -1;
	}

	/* For face manager */
	INIT_LIST_HEAD(&api_face->face_list);
	if (pthread_mutex_init(&api_face->face_lock, NULL)) {
		ALOGE("init face_lock failed");
		return -1;
	}

	if (pthread_mutex_init(&api_face->db_lock, NULL)) {
		ALOGE("init db lock failed");
		return -1;
	}

	if (pthread_mutex_init(&api_face->capture_lock, NULL)) {
		ALOGE("init capture_lock failed");
		return -1;
	}

	if (pthread_cond_init(&api_face->capture_cond, NULL)) {
		ALOGE("init db capture_cond failed");
		return -1;
	}

	/* Get recognition/developer/...  parameter*/
	ret = face_get_config();
	if (ret) {
		ALOGE("face get config failed");
		return -1;
	}

	if (api_face->face_sound_play) {
		ret = face_sound_init();
		if (ret) {
			ALOGE("face sound init failed");
			return -1;
		}
	}

	/* Initialize face service */
	api_face->svc_hdl = face_svc_init();
	if (!api_face->svc_hdl) {
		ALOGE("face svc init failed");
		return -1;
	}

	/* Load and register face features to service */
	ret = face_db_init();
	if (ret) {
		ALOGE("face db init failed");
		return -1;
	}

	hdr_sw = isp_get_config();
	if (hdr_sw < 0)
		ALOGE("isp get config failed");

	ret = set_rgb_sensor_hdr(0, hdr_sw);
	if (ret)
		ALOGE("hdr fail!");

	/* Config face service */
	config.score_threshold = api_face->recg_param.threshold_score;
	config.living = api_face->recg_param.live_detect;
	config.min_face_pixel = api_face->recg_param.min_face_pixel;
	config.max_face_pixel = api_face->recg_param.max_face_pixel;
	config.max_track_faces = api_face->recg_param.max_track_faces;
	ret = face_svc_set_config2(api_face->svc_hdl, &config);
	if (ret) {
		ALOGE("face set config failed");
		return -1;
	}

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

	/* Start thread of auto-delete-record */
	punch_record_auto_del_proc();

	return 0;
}

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
}
