/*
 * face_svc_main.h
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

#ifndef _FACE_SVC_MAIN_H_
#define _FACE_SVC_MAIN_H_

#include <pthread.h>

#include <nna_api.h>
#include <nir_api.h>

#include "face_svc.h"
#include "queue.h"
#include "list.h"
#include "face_svc_living.h"
#include "face_svc_conf.h"

#define NIR_FACE_DEBUG 1
#define DUMP_NIR_WINDOW 0

#define DEFAULT_CAMERAS_TS_DIFF 66666

#define SAVE_UNMATCH (1 << 0)
#define SAVE_MATCH_NO_LIVING (1 << 1)
#define SAVE_MATCH_LIVING (1 << 2)
#define SAVE_NIR_NO_LIVING (1 << 3)
#define SAVE_NIR_LIVING (1 << 4)

#define RGBNIR_MAP_CHECK  0x5B
#define RGBNIR_MAP_OFFSET  0x1024
#define FACE_SET_BIT(x, y) 		((x) |= (1u << (y) ))
#define FACE_GET_BIT(x, y) 		(((x) >> (y)) & 1)
#define FACE_CLEAR_BIT(x, y)	((x) &= ~(1u << (y)))
/*300ms living recg frame rate is 5 (200ms)*/
#define FACE_REPEAT_RECG_TIMEOUT_OFFSET 300
#define FACE_HEAD_POSE_ERROR			1
#define FACE_HEAD_POSE_RIGHT			2

typedef struct face_map_params {
	int check;
	double x_map[5];
	double y_map[5];
} face_map_params_t;

inline int get_face_save(int save, int mask)
{
	ALOGD_IF(0, "save: %d, mask: %d, &: %d", save, mask, save & mask);
	return save & mask;
}

inline void set_face_save(int *save, int mask)
{
	ALOGD_IF(0, "save: %d, mask: %d, &: %d", *save, mask, *save | mask);
	*save = *save | mask;
}

typedef struct _tracking_face {
	list_head_t node;
	face_frame_t *frame;
	face_frame_t *nir_frame;
	int id;
	int occur;
	int max_no_recg_face; /*max area no recg face*/
	int max_no_living_face; /*max area no living face */
	int recg;
	int recg_check_count;
	int quality_score;
	int quality_debonce;
	int nir_no_face;
	int living;
	int living_score;
	int living_last_score;
	int living_count;
	int living_score_acc;
	int unmatch_count;
	int living_check_count;
	int living_low_score_cnt;
	int face_mean_color[2];
	int rect_mean_rgb;
	int fake_yolo_score;
	int fake_yolo_score_acc;
	int rgb_mid_recg[2];	/* recg face mid position in rgb_frame,  in order to judge motion blur*/
	long long timeout_time;	/* add new face time + face_svc_conf->svc.timeout */
	int timeout;		/* face is timeout flag */
	int face_refuse;
	int face_pass;
	int face_state;
	int last_face_state;
	int detect_precondition;
	long long time;		/* save face refuse or pass frame time */
	long long repeat_detect_timeout_time;		/* detect time for repeat control */
	int save;
	nna_feature_map_t map;
	nna_face_t nna_rect;
	face_rect_t rect;
	face_feat_t feat;
	record_img_t img;	/* face save img path */
	long long add_timestamp; /* add face timestamp */
	long long recg_timestamp; /* recg finish timestamp */
	long long living_timestamp; /* living finish timestamp */
	int is_already_record;

	/* temp measure */
	int temp;
	face_temp_status_e temp_status;
	face_temp_status_e last_temp_status;
	face_temp_measure_t temp_measure;
} tracking_face_t;

typedef struct _cal_params
{
	float xScaleMean;
	int xOffsetMean;
	float kX;
	float bX;
	int xDetaMin;
	int xDetaMax;
	float yScaleMean;
	int yOffsetMean;
	float kY;
	float bY;
	int yDetaMin;
	int yDetaMax;
} cal_params_t;

typedef struct _face_svc
{
	void *media_hdl;

	void *rgb_stream;
	head_t rgb_frame_queue;
	pthread_mutex_t rgb_frame_lock;
	pthread_cond_t rgb_frame_cond;
	pthread_t face_det_thrd;

	void *rgb_mirror_stream;
	head_t rgb_mirror_frame_queue;
	pthread_mutex_t rgb_mirror_frame_lock;
	pthread_cond_t rgb_mirror_frame_cond;
	pthread_t face_rgb_mirror_dist_thrd;

	head_t rgb_det_queue;
	pthread_mutex_t rgb_det_lock;

	head_t rgb_recg_queue;
	pthread_mutex_t rgb_recg_lock;
	pthread_cond_t rgb_recg_cond;
	pthread_t face_recg_thrd;

	head_t rgb_cmp_queue;
	pthread_mutex_t rgb_cmp_lock;
	pthread_cond_t rgb_cmp_cond;
	pthread_t face_cmp_thrd;

	void *nir_stream;
	head_t nir_frame_queue;
	pthread_mutex_t nir_frame_lock;
	pthread_cond_t nir_frame_cond;

	/* pause face detect */
	int pause;
	int redet;
	pthread_mutex_t pause_lock;

	/* nir and living detection */
	void *nirdt_hdl;
	nir_fn_t *nir_living_hdl;
	nir_fn_t *rgb_color_living_hdl;
	nir_fn_t *rgb_living_hdl;
	nir_fn_t *nir_living_hdl_outdoors;
	nir_fn_t *nir_living_hdl_mask;

	/* face tracking */
	list_head_t tracking_list;
	pthread_mutex_t tracking_lock;

	/* living resource */
	head_t rgb_living_queue;
	pthread_mutex_t rgb_living_lock;
	pthread_cond_t rgb_living_cond;
	pthread_t face_living_thrd;

	/* face record */
	head_t record_queue;
	pthread_t face_record_thrd;
	pthread_mutex_t record_lock;
	pthread_cond_t record_cond;

	/* for blocking api */
	pthread_mutex_t block_api_lock;
	int face_cnt;
	face_info_t *face_info;
	int face_compare;
	int face_index;
	int face_ret;
	pthread_mutex_t face_lock;
	pthread_cond_t face_cond;

	/* capture switch, 0: off 1:on */
	int capture;
	face_capture_param_t *capture_param;
	face_info_t *capture_info;
	pthread_mutex_t face_capture_lock;
	pthread_cond_t face_capture_cond;

	/* zbar mode, 0: off 1: only zbar det 2: face/zbar all det */
	int zbar_mode;
	pthread_mutex_t zbar_lock;

	/* dark env nir det face */
	int nir_det;
	face_nir_det_cb nir_det_cb;
	pthread_t nir_det_thrd;
	pthread_mutex_t nir_det_lock;
	pthread_cond_t nir_det_cond;

	/* configuration */
	cal_params_t cal_params;

	/* configuration */
	map_params_t map_params;

	face_det_cb det_cb;
	face_recg_cb recg_cb;
	face_record_cb record_cb;
	nna_face_feature_t *face_feat;
	int feat_version;
	int feat_update;
	pthread_mutex_t face_feat_lock;
} face_svc_t;

typedef struct _rgb_det_buf {
	face_frame_t *frame;
	face_frame_t *nir_frame;

	nna_feature_map_t map;
} rgb_det_buf_t;

char *strtype(int type);
void free_rgb_det_buf(face_svc_t *svc, rgb_det_buf_t **pp);

typedef struct _rgb_recg_buf {
	face_frame_t *frame;
	face_frame_t *nir_frame;

	int ref;
	int type;

	nna_face_rib_t face_rib;

	face_info_t face_info;
} rgb_recg_buf_t;

typedef struct _face_record_buf {
	face_frame_t *frame;
	int cb; /* face record thread callback app record save flag */
	int save; /* face record thread save img flag */
	int delete; /* recg pass, but living not pass not timeout need delete recg img */
	int face;
	face_record_t face_record;
	int is_already_record;
} face_record_buf_t;

void save_frame(face_svc_t *svc, face_frame_t *frame, record_img_e type);
void save_record(face_svc_t *svc, tracking_face_t *face,
	rgb_recg_buf_t *recg_buf, record_img_e type);
void save_record_unqualified(face_svc_t *svc, tracking_face_t *face,
	face_frame_t *frame, face_rect_t *p_rect, record_img_e type);

void dump_face(char *path, face_frame_t *frame, face_rect_t *rect,
	int x_margin, int y_margin);

void face_svc_load_params(face_svc_t *svc);

void retain_frame(face_frame_t *frame);
void free_frame(face_frame_t **pp);
void free_frame_lock(face_frame_t **pp);

void free_rgb_recg_buf_no_cb(face_svc_t *svc, rgb_recg_buf_t **pp);
void free_rgb_recg_buf(face_svc_t *svc, rgb_recg_buf_t **pp);

long long get_systime_us(void);

void tracking_update(face_svc_t *svc, nna_faces_t *face_rects);
tracking_face_t *tracking_get(face_svc_t *svc, int id);
void tracking_clean(face_svc_t *svc);
void tracking_reset_recg_timeout(face_svc_t *svc, int id);
void tracking_set_recg(tracking_face_t *face, face_feat_t *feat);
void tracking_set_living(tracking_face_t *face, face_feat_t *feat);
int check_living_permit(face_svc_t *svc, tracking_face_t *face, face_frame_t *frame);
int check_recg_permit(face_svc_t *svc, tracking_face_t *face, face_frame_t *frame);
#if 0
int check_living_permit(face_svc_t *svc, tracking_face_t *face);
int check_recg_permit(face_svc_t *svc, tracking_face_t *face);
#endif
void rect_rgb_to_nir(face_svc_t *svc, IRECTC *in, IRECTC *out);

#endif /* _FACE_SVC_MAIN_H_ */
