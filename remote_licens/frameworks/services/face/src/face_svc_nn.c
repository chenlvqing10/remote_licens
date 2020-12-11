/*
 * nn.c
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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define LOG_TAG "face_svc"
#include <log/log.h>

#include "face_svc_main.h"
#include "face_svc_nn.h"
#include "face_svc_isp.h"
#include "face_svc_living.h"
#include "temp_measure.h"

static face_svc_t *svc = NULL;
static nna_fn_t *nna_hdl = NULL;
static nna_facedet_cfg_t nna_cfg;

/*static*/ char *strorient(int orient)
{
	if (orient == FACE_FRONT) {
		return "front";
	} else if (orient == FACE_LEFT_30) {
		return "left_30";
	} else if (orient == FACE_LEFT_60) {
		return "left_60";
	} else if (orient == FACE_LEFT_90) {
		return "left_90";
	} else if (orient == FACE_RIGHT_30) {
		return "right_30";
	} else if (orient == FACE_RIGHT_60) {
		return "right_60";
	} else if (orient == FACE_RIGHT_90) {
		return "right_90";
	} else if (orient == FACE_UP_30) {
		return "up_30";
	} else if (orient == FACE_DOWN_30) {
		return "down_30";
	} else if (orient == FACE_ROTATE_LEFT_30) {
		return "rotate_left_30";
	} else if (orient == FACE_ROTATE_RIGHT_30) {
		return "rotate_right_30";
	} else {
		return "unknown";
	}
}

/*static*/ int check_face_orient(int orient)
{
	if (orient == FACE_FRONT ||
		orient == FACE_LEFT_30 || orient == FACE_RIGHT_30 ||
		orient == FACE_ROTATE_LEFT_30 || orient == FACE_ROTATE_RIGHT_30)
		return 0;

	return -1;
}

static int check_head_pose(tracking_face_t *face, float yaw, float pitch, float roll)
{
	int offset = 0;
	/*after enter right pose. eg: threshold is [-15,15]; if wrong or default, threshhold is [-10,10]*/
	if (face->detect_precondition == FACE_HEAD_POSE_RIGHT){
		offset = -5;
	}
	if (yaw < -(face_svc_conf->svc.head_pose_yaw - offset) ||
		yaw > (face_svc_conf->svc.head_pose_yaw - offset)) {
		return -1;
	}

	if (pitch < -(face_svc_conf->svc.head_pose_pitch - offset) ||
		pitch > (face_svc_conf->svc.head_pose_pitch - offset)) {
		return -1;
	}

	if (roll < -(face_svc_conf->svc.head_pose_roll - offset) ||
		roll > (face_svc_conf->svc.head_pose_roll - offset)) {
		return -1;
	}

	return 0;
}

static int check_capture_head_pose(face_capture_param_t *capture_param,
	float yaw, float pitch, float roll)
{
	if(capture_param == NULL)
		return -1;

	if (yaw < -capture_param->face.head_pose_yaw ||
		yaw > capture_param->face.head_pose_yaw) {
		return -1;
	}

	if (pitch < -capture_param->face.head_pose_pitch ||
		pitch > capture_param->face.head_pose_pitch) {
		return -1;
	}

	if (roll < -capture_param->face.head_pose_roll ||
		roll > capture_param->face.head_pose_roll) {
		return -1;
	}

	return 0;
}

static int check_capture_face_limit(face_capture_param_t *capture_param,
	face_rect_t *rect)
{
	int face_max, face_min, face_width, face_height;

	if (capture_param == NULL)
		return -1;

	face_max = capture_param->face.max;
	face_min = capture_param->face.min;
	face_width = rect->rect[2] - rect->rect[0];
	face_height = rect->rect[3] - rect->rect[1];
	if (face_width > face_max || face_height > face_max) {
		ALOGE("face size(%dx%d) > face max(%d)",
			face_width, face_height, face_min);
		return -1;
	}
	if (face_width < face_min || face_height < face_min) {
		ALOGE("face size(%dx%d) < face min(%d)",
			face_width, face_height, face_min);
		return -1;
	}
	if (rect->rect[0] < capture_param->face.x0 ||
		rect->rect[1] < capture_param->face.y0 ||
		rect->rect[2] > capture_param->face.x1 ||
		rect->rect[3] > capture_param->face.y1) {
		ALOGE("face position [%d, %d, %d ,%d], limit x0:%d y0:%d x1:%d y1:%d ",
			rect->rect[0],rect->rect[1], rect->rect[2], rect->rect[3],
			capture_param->face.x0, capture_param->face.y0,
			capture_param->face.x1, capture_param->face.y1);
		return -1;
	}

	return 0;
}

static int check_recg_rect(face_rect_t* rect)
{
	if (rect->rect[0] > face_svc_conf->svc.recg_rect_left &&
		rect->rect[1] > face_svc_conf->svc.recg_rect_top &&
		rect->rect[2] < face_svc_conf->svc.recg_rect_right - 1 &&
		rect->rect[3] < face_svc_conf->svc.recg_rect_bottom - 1)
		return 0;

	return -1;
}

/*static*/ int check_temp_measure_rect(face_rect_t* rect)
{
	if (rect->rect[0] > face_svc_conf->svc.temp_rect_left &&
		rect->rect[1] > face_svc_conf->svc.temp_rect_top &&
		rect->rect[2] < face_svc_conf->svc.temp_rect_right - 1 &&
		rect->rect[3] < face_svc_conf->svc.temp_rect_bottom - 1)
		return 0;

	return -1;
}

static int check_recg_face(face_attr_t* attr)
{
	if (face_svc_conf->svc.recg_male != -1)
		if (face_svc_conf->svc.recg_male != attr->male)
			return -1;
	if (face_svc_conf->svc.recg_wear_glasses != -1)
		if (face_svc_conf->svc.recg_wear_glasses != attr->wear_glasses)
			return -1;
	if (face_svc_conf->svc.recg_wear_hat != -1)
		if (face_svc_conf->svc.recg_wear_hat != attr->wear_hat)
			return -1;
	if (face_svc_conf->svc.recg_left_eye_cover != -1)
		if (face_svc_conf->svc.recg_left_eye_cover != attr->left_eye_cover)
			return -1;
	if (face_svc_conf->svc.recg_right_eye_cover != -1)
		if (face_svc_conf->svc.recg_right_eye_cover != attr->right_eye_cover)
			return -1;
	if (face_svc_conf->svc.recg_mouse_cover != -1)
		if (face_svc_conf->svc.recg_mouse_cover != attr->mouse_cover)
			return -1;
	if (face_svc_conf->svc.recg_mask != -1)
		if (face_svc_conf->svc.recg_mask != attr->mask)
			return -1;

	return 0;
}

static void render_position_map(face_rect_t* rect)
{
	//int ratio = 0;

	//ratio = face_svc_conf->svc.rgb_buffer_width * 1000 / face_svc_conf->media.rgb_preview_width;
	//if (ratio == face_svc_conf->svc.rgb_buffer_height * 1000 / face_svc_conf->media.rgb_preview_height) {
	if (face_svc_conf->svc.ratio_similar) {
		rect->rect_render[0] = face_svc_conf->media.rgb_preview_x +
			rect->rect_smooth[0] * face_svc_conf->media.rgb_preview_width /
			face_svc_conf->svc.rgb_buffer_width;
		rect->rect_render[1] = face_svc_conf->media.rgb_preview_y +
			rect->rect_smooth[1] * face_svc_conf->media.rgb_preview_height /
			face_svc_conf->svc.rgb_buffer_height;
		rect->rect_render[2] = face_svc_conf->media.rgb_preview_x +
			rect->rect_smooth[2] * face_svc_conf->media.rgb_preview_width /
			face_svc_conf->svc.rgb_buffer_width;
		rect->rect_render[3] = face_svc_conf->media.rgb_preview_y +
			rect->rect_smooth[3] * face_svc_conf->media.rgb_preview_height /
			face_svc_conf->svc.rgb_buffer_height;
	} else {
		rect->rect_render[0] = face_svc_conf->media.rgb_preview_y +
			rect->rect_smooth[0] * face_svc_conf->media.rgb_preview_height /
			face_svc_conf->svc.rgb_buffer_width;
		rect->rect_render[1] = face_svc_conf->media.rgb_preview_x +
			rect->rect_smooth[1] * face_svc_conf->media.rgb_preview_width /
			face_svc_conf->svc.rgb_buffer_height;
		rect->rect_render[2] = face_svc_conf->media.rgb_preview_y +
			rect->rect_smooth[2] * face_svc_conf->media.rgb_preview_height /
			face_svc_conf->svc.rgb_buffer_width;
		rect->rect_render[3] = face_svc_conf->media.rgb_preview_x +
			rect->rect_smooth[3] * face_svc_conf->media.rgb_preview_width /
			face_svc_conf->svc.rgb_buffer_height;
	}
}

/**/
static int check_recg_repeat(tracking_face_t* face)
{
	long long cur = get_systime_us();
	int res = -1;

	if (face){
		if (face_svc_conf->svc.recg_repeat == 1){
			if (cur >= face->repeat_detect_timeout_time){
				ALOGD("face[%d] repeat timeout!!! recg=%d to=%lld rto=%lld", face->id, face->recg,
					face->timeout_time, face->repeat_detect_timeout_time);
				res = 0;
			}
		}
	}

	return res;
}


static void face_reset_face(tracking_face_t* face)
{
	if (face){
		face->living = 0;
		face->feat.living_body = 0;
		face->living_score = 0;
		face->living_last_score = 0;
		face->living_score_acc = 0;
		//face->living_check_count = 0;
		face->recg = 0;
		face->recg_check_count =0;
		face->timeout = 0;
		face->face_refuse = 0;
		face->face_pass = 0;
		tracking_reset_recg_timeout(svc, face->id);

		ALOGD("face_state face_reset_face");
		//face->face_error = 0;
	}
}

/*static*/ void face_set_exposure(nna_faces_t *rects)
{
	int i;
	viss_isp_rects_t rgb_rects;
	viss_isp_rects_t nir_rects;
	IRECTC in;
	IRECTC out;

	memset(&rgb_rects, 0, sizeof(viss_isp_rects_t));
	memset(&nir_rects, 0, sizeof(viss_isp_rects_t));

	if (!rects->num) {
		goto exit;
	}

	rgb_rects.rects = malloc(sizeof(viss_isp_rect_t) * rects->num);
	if (!rgb_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		goto exit;
	}
	rgb_rects.cnt = rects->num;

	nir_rects.rects = malloc(sizeof(viss_isp_rect_t) * rects->num);
	if (!nir_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		goto exit;
	}
	nir_rects.cnt = rects->num;

	for (i = 0; i < rects->num; i++) {
		nna_face_t *face_rect = &rects->face_rects[i];
		viss_isp_rect_t *rgb_rect = &rgb_rects.rects[i];
		viss_isp_rect_t *nir_rect = &nir_rects.rects[i];

		/* rgb rect */
		rgb_rect->left = face_rect->rect[0] + face_svc_conf->svc.rgb_x_margin;
		rgb_rect->top = face_rect->rect[1] + face_svc_conf->svc.rgb_y_margin;
		rgb_rect->right = face_rect->rect[2] + face_svc_conf->svc.rgb_x_margin;
		rgb_rect->bottom = face_rect->rect[3] + face_svc_conf->svc.rgb_y_margin;

		/* nir rect */
		in.x = face_rect->rect[0] + face_svc_conf->svc.rgb_x_margin;
		in.y = face_rect->rect[1] + face_svc_conf->svc.rgb_y_margin;
		in.width = face_rect->rect[2] - face_rect->rect[0];
		in.height = face_rect->rect[3] - face_rect->rect[1];
		rect_rgb_to_nir(svc, &in, &out);

		nir_rect->left = out.x;
		nir_rect->top = out.y;
		nir_rect->right = nir_rect->left + out.width;
		nir_rect->bottom = nir_rect->top + out.height;
	}

exit:
	face_set_ae_para(FACE_CAMERA_RGB, &rgb_rects);
	if (face_svc_conf->svc.living_check && !face_svc_conf->svc.living_check_mode)
		face_set_ae_para(FACE_CAMERA_NIR, &nir_rects);

	if (rgb_rects.rects)
		free(rgb_rects.rects);
	if (nir_rects.rects)
		free(nir_rects.rects);
}

void face_set_exposure_null(nna_faces_t *rects)
{
	viss_isp_rects_t rgb_rects;

	if (rects->num) {
		return;
	}

	memset(&rgb_rects, 0, sizeof(viss_isp_rects_t));
	face_set_ae_para(FACE_CAMERA_RGB, &rgb_rects);
	if (face_svc_conf->svc.living_check && !face_svc_conf->svc.living_check_mode) {
		viss_isp_rects_t nir_rects;
		memset(&nir_rects, 0, sizeof(viss_isp_rects_t));
		face_set_ae_para(FACE_CAMERA_NIR, &nir_rects);
	}
}


/* should lock */
/* priority exposure no recg/living max rect face */
static void face_set_exposure2(face_svc_t *svc, nna_faces_t *rects)
{
	int i;
	viss_isp_rects_t rgb_rects;
	viss_isp_rects_t nir_rects;
	IRECTC in;
	IRECTC out;
	tracking_face_t *face = NULL;
	nna_face_t *face_rect = NULL;
	int max_area, max_id;

	memset(&rgb_rects, 0, sizeof(viss_isp_rects_t));
	memset(&nir_rects, 0, sizeof(viss_isp_rects_t));

	if (!rects->num) {
		goto exit;
	}

	rgb_rects.rects = malloc(sizeof(viss_isp_rect_t));
	if (!rgb_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		goto exit;
	}
	rgb_rects.cnt = 1;

	nir_rects.rects = malloc(sizeof(viss_isp_rect_t));
	if (!nir_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		goto exit;
	}
	nir_rects.cnt = 1;

	/* rgb rect start*/
	max_area = 0;
	max_id = 0;
	for (i = 0; i < rects->num; i++) {
		face_rect = &rects->face_rects[i];
		face = tracking_get(svc, face_rect->id);
		if(!face)
			continue;
		if(!face->recg /* FIXME */) {
			int width = face_rect->rect[2] - face_rect->rect[0];
			int height = face_rect->rect[3] - face_rect->rect[1];
			int area = width * height;
			if (area > max_area) {
				max_area = area;
				max_id = i;
			}
		}
	}
	viss_isp_rect_t *rgb_rect = &rgb_rects.rects[0];
	face_rect = &rects->face_rects[max_id];
	rgb_rect->left = face_rect->rect[0] + face_svc_conf->svc.rgb_x_margin;
	rgb_rect->top = face_rect->rect[1] + face_svc_conf->svc.rgb_y_margin;
	rgb_rect->right = face_rect->rect[2] + face_svc_conf->svc.rgb_x_margin;
	rgb_rect->bottom = face_rect->rect[3] + face_svc_conf->svc.rgb_y_margin;
	/* rgb rect end*/

	/* nir rect start*/
	max_area = 0;
	max_id = 0;
	for (i = 0; i < rects->num; i++) {
		face_rect = &rects->face_rects[i];
		face = tracking_get(svc, face_rect->id);
		if(!face)
			continue;
		if(!face->living /* FIXME */) {
			int width = face_rect->rect[2] - face_rect->rect[0];
			int height = face_rect->rect[3] - face_rect->rect[1];
			int area = width * height;
			if (area > max_area) {
				max_area = area;
				max_id = i;
			}
		}
	}
	viss_isp_rect_t *nir_rect = &nir_rects.rects[0];
	face_rect = &rects->face_rects[max_id];
	in.x = face_rect->rect[0] + face_svc_conf->svc.rgb_x_margin;
	in.y = face_rect->rect[1] + face_svc_conf->svc.rgb_y_margin;
	in.width = face_rect->rect[2] - face_rect->rect[0];
	in.height = face_rect->rect[3] - face_rect->rect[1];
	rect_rgb_to_nir(svc, &in, &out);

	nir_rect->left = out.x;
	nir_rect->top = out.y;
	nir_rect->right = nir_rect->left + out.width;
	nir_rect->bottom = nir_rect->top + out.height;
	/* nir rect end*/

exit:
	face_set_ae_para(FACE_CAMERA_RGB, &rgb_rects);
	if (face_svc_conf->svc.living_check && !face_svc_conf->svc.living_check_mode)
		face_set_ae_para(FACE_CAMERA_NIR, &nir_rects);

	if (rgb_rects.rects)
		free(rgb_rects.rects);
	if (nir_rects.rects)
		free(nir_rects.rects);
}

/* should lock */
/* priority exposure no recg/living max rect face */
static void face_set_exposure3(face_svc_t *svc, nna_faces_t *rects)
{
	int i;
	viss_isp_rects_t rgb_rects;
	viss_isp_rects_t nir_rects;
	IRECTC in;
	IRECTC out;
	tracking_face_t *face = NULL;
	tracking_face_t *max_face = NULL;
	nna_face_t *face_rect = NULL;
	int max_area, max_id;

	memset(&rgb_rects, 0, sizeof(viss_isp_rects_t));
	memset(&nir_rects, 0, sizeof(viss_isp_rects_t));

	if (!rects->num) {
		goto exit;
	}

	rgb_rects.rects = malloc(sizeof(viss_isp_rect_t));
	if (!rgb_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		goto exit;
	}
	rgb_rects.cnt = 1;

	nir_rects.rects = malloc(sizeof(viss_isp_rect_t));
	if (!nir_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		goto exit;
	}
	nir_rects.cnt = 1;

	/* rgb rect start*/
	/* find no recg max_area face recg*/
	max_area = 0;
	max_id = 0;
	max_face = NULL;
	for (i = 0; i < rects->num; i++) {
		/* rgb rect */
		face_rect = &rects->face_rects[i];
		face = tracking_get(svc, face_rect->id);
		if(!face)
			continue;
		if(!face->recg /* FIXME */) {
			int width = face_rect->rect[2] - face_rect->rect[0];
			int height = face_rect->rect[3] - face_rect->rect[1];
			int area = width * height;
			if (area > max_area) {
				max_area = area;
				max_face = face;
				max_id = i;
			} else {
				face->max_no_recg_face = 0;
			}
		}
	}
	if (max_face)
		max_face->max_no_recg_face = 1; /* max not recg face */

	/* find max_area face exposure */
	max_area = 0;
	max_id = 0;
	for (i = 0; i < rects->num; i++) {
		face_rect = &rects->face_rects[i];
		face = tracking_get(svc, face_rect->id);
		if(!face)
			continue;
		int width = face_rect->rect[2] - face_rect->rect[0];
		int height = face_rect->rect[3] - face_rect->rect[1];
		int area = width * height;
		if (area > max_area) {
			max_area = area;
			max_id = i;
		}
	}
	viss_isp_rect_t *rgb_rect = &rgb_rects.rects[0];
	face_rect = &rects->face_rects[max_id];
	rgb_rect->left = face_rect->rect[0] + face_svc_conf->svc.rgb_x_margin;
	rgb_rect->top = face_rect->rect[1] + face_svc_conf->svc.rgb_y_margin;
	rgb_rect->right = face_rect->rect[2] + face_svc_conf->svc.rgb_x_margin;
	rgb_rect->bottom = face_rect->rect[3] + face_svc_conf->svc.rgb_y_margin;
	/* rgb rect end*/

	/* nir rect start*/
	max_area = 0;
	max_id = 0;
	max_face = NULL;
	for (i = 0; i < rects->num; i++) {
		face_rect = &rects->face_rects[i];
		face = tracking_get(svc, face_rect->id);
		if(!face)
			continue;
		if(!face->living /* FIXME */) {
			int width = face_rect->rect[2] - face_rect->rect[0];
			int height = face_rect->rect[3] - face_rect->rect[1];
			int area = width * height;
			if (area > max_area) {
				max_area = area;
				max_face = face;
				max_id = i;
			} else {
				face->max_no_living_face = 0;
			}
		}
	}
	if (max_face)
		max_face->max_no_living_face = 1; /* max not living face */

	viss_isp_rect_t *nir_rect = &nir_rects.rects[0];
	face_rect = &rects->face_rects[max_id];
	in.x = face_rect->rect[0] + face_svc_conf->svc.rgb_x_margin;
	in.y = face_rect->rect[1] + face_svc_conf->svc.rgb_y_margin;
	in.width = face_rect->rect[2] - face_rect->rect[0];
	in.height = face_rect->rect[3] - face_rect->rect[1];
	rect_rgb_to_nir(svc, &in, &out);

	nir_rect->left = out.x;
	nir_rect->top = out.y;
	nir_rect->right = nir_rect->left + out.width;
	nir_rect->bottom = nir_rect->top + out.height;
	/* nir rect end*/

exit:
	face_set_ae_para(FACE_CAMERA_RGB, &rgb_rects);
	if (face_svc_conf->svc.living_check && !face_svc_conf->svc.living_check_mode)
		face_set_ae_para(FACE_CAMERA_NIR, &nir_rects);

	if (rgb_rects.rects)
		free(rgb_rects.rects);
	if (nir_rects.rects)
		free(nir_rects.rects);
}

#if DUMP_NIR_WINDOW
static void face_set_nir_exposure(face_frame_t *nir_frame, nna_face_t *face_rect)
#else
static void face_set_nir_exposure(nna_face_t *face_rect)
#endif
{
	viss_isp_rects_t nir_rects;
	viss_isp_rect_t *nir_rect;
	float x_scale_rate;
	float y_scale_rate;
	int width, height;

	if (!face_rect) {
		return;
	}

	nir_rects.rects = malloc(sizeof(viss_isp_rect_t));
	if (!nir_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	nir_rects.cnt = 1;

	x_scale_rate = (float)face_svc_conf->svc.nir_buffer_width / face_svc_conf->svc.rgb_buffer_width;
	y_scale_rate = (float)face_svc_conf->svc.nir_buffer_height / face_svc_conf->svc.rgb_buffer_height;

	nir_rect = &nir_rects.rects[0];
	nir_rect->left = (float)face_rect->rect[0] * x_scale_rate;
	nir_rect->top = (float)face_rect->rect[1] * y_scale_rate;
	nir_rect->right = (float)face_rect->rect[2] * x_scale_rate;
	nir_rect->bottom = (float)face_rect->rect[3] * y_scale_rate;

	width = nir_rect->right - nir_rect->left;
	height = nir_rect->bottom - nir_rect->top;

#if 0
	nir_rect->left += width / 4;
	if (nir_rect->top + 2*height < face_svc_conf->svc.nir_buffer_height) {
		nir_rect->top += height;
	}
	nir_rect->right -= width / 4;
	nir_rect->bottom = face_svc_conf->svc.nir_buffer_height;
#else
	nir_rect->bottom = face_svc_conf->svc.nir_buffer_height;
#endif

	width = nir_rect->right - nir_rect->left;
	height = nir_rect->bottom - nir_rect->top;

	if (width < NIR_WINDOW_MIN) {
		nir_rect->left -= (NIR_WINDOW_MIN - width) / 2;
		nir_rect->right += (NIR_WINDOW_MIN - width) / 2;
	}

	if (height < NIR_WINDOW_MIN) {
		nir_rect->top -= (NIR_WINDOW_MIN - height) / 2;
		nir_rect->bottom += (NIR_WINDOW_MIN - height) / 2;
	}

	ALOGD_IF(NIR_FACE_DEBUG, "%s nir [%d, %d, %d, %d]", __func__,
			nir_rect->left, nir_rect->top, nir_rect->right, nir_rect->bottom);

#if DUMP_NIR_WINDOW
	if (nir_frame) {
		static int i = 0;
		char path[FILENAME_MAX];
		face_rect_t rect;
		rect.rect[0] = nir_rect->left;
		rect.rect[1] = nir_rect->top;
		rect.rect[2] = nir_rect->right;
		rect.rect[3] = nir_rect->bottom;
		snprintf(path, FILENAME_MAX, "/data/nir-window-%ld-%d.jpg", face_rect->id, i);
		dump_face(path, nir_frame, &rect, 0, 0);
		i++;
	}
#endif

	face_set_ae_para(FACE_CAMERA_NIR, &nir_rects);
	if (nir_rects.rects)
		free(nir_rects.rects);
}

#if DUMP_NIR_WINDOW
void face_set_exposure4(face_svc_t *svc, face_frame_t *nir_frame, nna_faces_t *rects)
#else
void face_set_exposure4(face_svc_t *svc, nna_faces_t *rects)
#endif
{
	int i, max_area, max_id;
	viss_isp_rects_t rgb_rects;
	nna_face_t face_rect;
	viss_isp_rect_t *rgb_rect;
	tracking_face_t *face;

	memset(&rgb_rects, 0, sizeof(viss_isp_rects_t));
	memset(&face_rect, 0, sizeof(nna_face_t));

	if (!rects->num) {
		goto exit;
	}

	rgb_rects.rects = malloc(sizeof(viss_isp_rect_t));
	if (!rgb_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		goto exit;
	}
	rgb_rects.cnt = 1;

	max_area = 0;
	max_id = 0;
	for (i = 0; i < rects->num; i++) {
		nna_face_t *tmp_face_rect = &rects->face_rects[i];
		int width = tmp_face_rect->rect[2] - tmp_face_rect->rect[0];
		int height = tmp_face_rect->rect[3] - tmp_face_rect->rect[1];
		int area = width * height;
		if (area > max_area) {
			max_area = area;
			max_id = i;
		}
	}

	/* rgb rect */
	memcpy(&face_rect, &rects->face_rects[max_id], sizeof(nna_face_t));
	rgb_rect = &rgb_rects.rects[0];
	rgb_rect->left = face_rect.rect[0] + face_svc_conf->svc.rgb_x_margin;
	rgb_rect->top = face_rect.rect[1] + face_svc_conf->svc.rgb_y_margin;
	rgb_rect->right = face_rect.rect[2] + face_svc_conf->svc.rgb_x_margin;
	rgb_rect->bottom = face_rect.rect[3] + face_svc_conf->svc.rgb_y_margin;

exit:
	face_set_ae_para(FACE_CAMERA_RGB, &rgb_rects);
	if (face_svc_conf->svc.living_check && !face_svc_conf->svc.living_check_mode) {
		if (!rgb_rects.cnt) {
			viss_isp_rects_t nir_rects;
			memset(&nir_rects, 0, sizeof(viss_isp_rects_t));
			face_set_ae_para(FACE_CAMERA_NIR, &nir_rects);
		} else {
			face = tracking_get(svc, face_rect.id);
			if (face->nir_no_face) {
#if DUMP_NIR_WINDOW
				face_set_nir_exposure(nir_frame, &face_rect);
#else
				face_set_nir_exposure(&face_rect);
#endif
			}
		}
	}

	if (rgb_rects.rects)
		free(rgb_rects.rects);
}

void det_cb_update_user_event(face_rect_t *p_rect)
{
	if (p_rect){
		tracking_face_t *face = tracking_get(svc, p_rect->id);
		if (face){
			if (face->face_refuse)
				FACE_SET_BIT(face->face_state, FACE_STATE_RECG_REFUSE);
			else{
				FACE_CLEAR_BIT(face->face_state, FACE_STATE_RECG_REFUSE);
			}
			if (face->face_pass)
				FACE_SET_BIT(face->face_state, FACE_STATE_RECG_PASS);
			else{
				FACE_CLEAR_BIT(face->face_state, FACE_STATE_RECG_PASS);
			}
			if (face->feat.living_body)
				FACE_SET_BIT(face->face_state, FACE_STATE_LIVING_BODY);
			else{
				FACE_CLEAR_BIT(face->face_state, FACE_STATE_LIVING_BODY);
			}

			if (face->detect_precondition == 2){
				FACE_SET_BIT(face->face_state, FACE_STATE_DETECT_PASS);
				FACE_CLEAR_BIT(face->face_state, FACE_STATE_DETECT_TIMEOUT);
			}else if (face->detect_precondition == 1){
				FACE_SET_BIT(face->face_state, FACE_STATE_DETECT_TIMEOUT);
				FACE_CLEAR_BIT(face->face_state, FACE_STATE_DETECT_PASS);
			}

			if (p_rect->attr.mask)
				FACE_SET_BIT(face->face_state, FACE_STATE_RECG_MASK);
			else{
				FACE_CLEAR_BIT(face->face_state, FACE_STATE_RECG_MASK);
			}



			/*to app*/
			p_rect->face_event = 0;
			if ((0 == FACE_GET_BIT(face->last_face_state, FACE_STATE_DETECT_TIMEOUT)) &&
				(1 == FACE_GET_BIT(face->face_state, FACE_STATE_DETECT_TIMEOUT))){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_DETECT_TIMEOUT);
			}

			if ((0 == FACE_GET_BIT(face->last_face_state, FACE_STATE_DETECT_PASS)) &&
				(1 == FACE_GET_BIT(face->face_state, FACE_STATE_DETECT_PASS))){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_DETECT_PASS);
			}

			if ((0 == FACE_GET_BIT(face->last_face_state, FACE_STATE_LIVING_RECG_TIMEOUT)) &&
				(1 == FACE_GET_BIT(face->face_state, FACE_STATE_LIVING_RECG_TIMEOUT))){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_LIVING_RECG_TIMEOUT);
			}
			if ((0 == FACE_GET_BIT(face->last_face_state, FACE_STATE_RECG_REFUSE)) &&
				(1 == FACE_GET_BIT(face->face_state, FACE_STATE_RECG_REFUSE))){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_RECG_REFUSE);
			}
			if ((0 == FACE_GET_BIT(face->last_face_state, FACE_STATE_RECG_PASS)) &&
				(1 == FACE_GET_BIT(face->face_state, FACE_STATE_RECG_PASS))){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_RECG_PASS);
			}
			if ((0 == FACE_GET_BIT(face->last_face_state, FACE_STATE_LIVING_BODY)) &&
				(1 == FACE_GET_BIT(face->face_state, FACE_STATE_LIVING_BODY))){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_LIVING_BODY);
			}

			if ((TEMP_STATUS_PREPARING != face->last_temp_status) &&
				(TEMP_STATUS_PREPARING == face->temp_status)){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_TEMP_STATUS_PREPARING);
			}
			if ((TEMP_STATUS_MEASURING != face->last_temp_status) &&
				(TEMP_STATUS_MEASURING == face->temp_status)){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_TEMP_STATUS_MEASURING);
			}
			if ((TEMP_STATUS_DONE != face->last_temp_status) &&
				(TEMP_STATUS_DONE == face->temp_status)){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_TEMP_STATUS_DONE);
			}

			if ((FACE_GET_BIT(face->face_state, FACE_STATE_RECG_MASK)
				!= FACE_GET_BIT(face->last_face_state, FACE_STATE_RECG_MASK))){
					FACE_SET_BIT(p_rect->face_event, FACE_STATE_RECG_MASK);
			}


			p_rect->face_state = face->face_state;
			if (p_rect->face_event)
				ALOGD("to app face[%d] face_state=0x%x last_face_state=0x%x temp_status=%d last_temp_status=%d"
						"face_event=0x%x %s %s %s %s %s %s %s %s %s %s",
					p_rect->id,
					face->face_state,
					face->last_face_state,
					face->temp_status,
					face->last_temp_status,
					p_rect->face_event,
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_DETECT_TIMEOUT)?"FACE_STATE_DETECT_TIMEOUT":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_DETECT_PASS)?"FACE_STATE_DETECT_PASS":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_LIVING_RECG_TIMEOUT)?"FACE_STATE_LIVING_RECG_TIMEOUT":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_RECG_REFUSE)?"FACE_STATE_RECG_REFUSE":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_RECG_PASS)?"FACE_STATE_RECG_PASS":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_TEMP_STATUS_PREPARING)?"FACE_STATE_TEMP_STATUS_PREPARING":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_TEMP_STATUS_MEASURING)?"FACE_STATE_TEMP_STATUS_MEASURING":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_TEMP_STATUS_DONE)?"FACE_STATE_TEMP_STATUS_DONE":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_RECG_MASK)?"FACE_STATE_RECG_MASK":"",
					FACE_GET_BIT(p_rect->face_event, FACE_STATE_LIVING_BODY)?"FACE_STATE_LIVING_BODY":"");

			face->last_face_state = face->face_state;
			face->last_temp_status = face->temp_status;
		}
	}
}

void det_cb_recg_repeat_control(tracking_face_t *face)
{
	/*repeat check when recg or living has results*/
	int actual = 0;
	if (face /*&& ((face_svc_conf->svc.recg_enable && !face_svc_conf->svc.living_check &&
							(face->face_refuse || face->face_pass)) ||
					(!face_svc_conf->svc.recg_enable && face_svc_conf->svc.living_check
							&& (face->timeout || face->living)) ||
					(face_svc_conf->svc.recg_enable && face_svc_conf->svc.living_check &&
							(face->face_refuse || face->face_pass) && (face->timeout || face->living)))*/
			&& (0 == check_recg_repeat(face))){
		actual = (face_svc_conf->svc.recg_repeat_interval > (face_svc_conf->svc.det_timeout))?
				face_svc_conf->svc.recg_repeat_interval:face_svc_conf->svc.det_timeout;
		actual += FACE_REPEAT_RECG_TIMEOUT_OFFSET;
		face->repeat_detect_timeout_time = get_systime_us() + actual*1000;
		face_reset_face(face);
	}
}

static void det_cb_event(nna_feature_map_t *map, nna_facecb_info_t *det_info)
{
	node_t *e;
	rgb_det_buf_t *det_buf = NULL;
	face_frame_t *frame;
	face_frame_t *nir_frame = NULL;
	int cnt, return_cnt;
	int max_track_faces;
	int i, j;
	int max_area, max_id;
	rgb_recg_buf_t *recg_buf;
	face_rects_t cb_face_rects;
	face_info_t cb_face_info;
	tracking_face_t *face = NULL;
	temp_measure_t *measure = NULL;

	if (face_svc_conf->svc.dump_det_fps) {
		static long long start = 0;
		long long end = get_systime_us();
		static int fps = 0;

		fps++;
		if (end - start > 1000000) {
			ALOGI("face det frame %d fps", fps);
			start = get_systime_us();
			fps = 0;
		}
	}

	/* iterate rgb_det_queue and find the frame and map */
	pthread_mutex_lock(&svc->rgb_det_lock);
	TAILQ_FOREACH(e, &svc->rgb_det_queue, nodes) {
		det_buf = e->buf;
		if (&det_buf->map == map) {
			queue_remove(&svc->rgb_det_queue, e);
			break;
		}
	}
	pthread_mutex_unlock(&svc->rgb_det_lock);

	frame = det_buf->frame;
	nir_frame = det_buf->nir_frame;
	frame->det_end = get_systime_us();
	ALOGI_IF(face_svc_conf->debug.profile, "frame %p detecting toke %lld (us)",
		frame, frame->det_end - frame->det_begin);

	if (frame->type == FACE_FRAME_TYPE_RGB && face_svc_conf->svc.face_exposure == 1) {
		face_set_exposure(&det_info->rects);
	}

	if (frame->type == FACE_FRAME_TYPE_RGB && face_svc_conf->svc.face_exposure > 0) {
		face_set_exposure_null(&det_info->rects);
	}

	cnt = det_info->rects.num;
	if (cnt > 0) {
		max_track_faces = face_svc_conf->svc.max_track_faces;

		if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK) {
			pthread_mutex_lock(&svc->face_lock);
			if (svc->face_cnt == 1)
				max_track_faces = 1;

			svc->face_ret = cnt;
			return_cnt = cnt < svc->face_cnt ? cnt : svc->face_cnt;

			/* update face_index and face_cnt for multiple faces */
			svc->face_cnt = return_cnt;
			svc->face_index = 0;
			pthread_mutex_unlock(&svc->face_lock);
		} else if(frame->type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK) {
			max_track_faces = 1;
			return_cnt = 1;
		}
		else {
			return_cnt = cnt < face_svc_conf->svc.max_track_faces ?
				cnt : face_svc_conf->svc.max_track_faces;
		}

		cb_face_rects.cnt = return_cnt;
		cb_face_rects.p_rect = malloc(sizeof(face_rect_t) * return_cnt);
		if (!cb_face_rects.p_rect) {
			ALOGE("%s malloc face_rect_t failed, errno: %d (%s)",
				__func__, errno, strerror(errno));
			if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK) {
				pthread_mutex_lock(&svc->face_lock);
				svc->face_ret = FACE_SVC_ERR;
				pthread_mutex_unlock(&svc->face_lock);
			}
			goto free_res;
		}
		memset(cb_face_rects.p_rect, 0, sizeof(face_rect_t) * return_cnt);

		/* update tracking faces */
		if (frame->type == FACE_FRAME_TYPE_RGB) {
			pthread_mutex_lock(&svc->tracking_lock);
			tracking_update(svc, &det_info->rects);
			pthread_mutex_unlock(&svc->tracking_lock);

			if (face_svc_conf->svc.temp_measure_enable) {
				temp_measure_update(&det_info->rects);
				temp_measure_start();
			}
		}

		/* exposure */
		if (frame->type == FACE_FRAME_TYPE_RGB && face_svc_conf->svc.face_exposure == 2) {
			pthread_mutex_lock(&svc->tracking_lock);
			face_set_exposure2(svc, &det_info->rects);
			pthread_mutex_unlock(&svc->tracking_lock);
		}

		if (frame->type == FACE_FRAME_TYPE_RGB && face_svc_conf->svc.face_exposure == 3) {
			pthread_mutex_lock(&svc->tracking_lock);
			face_set_exposure3(svc, &det_info->rects);
			pthread_mutex_unlock(&svc->tracking_lock);
		}

		if(frame->type == FACE_FRAME_TYPE_RGB && face_svc_conf->svc.face_exposure == 4) {
			pthread_mutex_lock(&svc->tracking_lock);
#if DUMP_NIR_WINDOW
			face_set_exposure4(svc, nir_frame, &det_info->rects);
#else
			face_set_exposure4(svc, &det_info->rects);
#endif
			pthread_mutex_unlock(&svc->tracking_lock);
		}


		max_area = 0;
		max_id = 0;
		for (i = 0, j = 0; i < cnt && j < return_cnt; i++) {
			nna_face_t *face_rect = &det_info->rects.face_rects[i];
			face_rect_t *p_rect = &cb_face_rects.p_rect[j];
			recg_buf = NULL;

			ALOGD_IF(face_svc_conf->debug.debug_log, "frame: %p type: %s, face[%d]: [%d, %d, %d, %d], smooth: [%d, %d, %d, %d], quality score: %d, head_pose: [%f, %f, %f], mask: %d",
				frame, strtype(frame->type), face_rect->id,
				face_rect->rect[0], face_rect->rect[1],
				face_rect->rect[2], face_rect->rect[3],
				face_rect->rect_smooth[0], face_rect->rect_smooth[1],
				face_rect->rect_smooth[2], face_rect->rect_smooth[3],
				face_rect->quality_score,
				face_rect->head_pose[0],
				face_rect->head_pose[1],
				face_rect->head_pose[2],
				face_rect->attr.mask);

			if (max_track_faces == 1) {
				int width = face_rect->rect[2] - face_rect->rect[0];
				int height = face_rect->rect[3] - face_rect->rect[1];
				int area = width * height;
				if (area > max_area) {
					max_area = area;
					max_id = i;
				}

				if (i + 1 < cnt)
					continue;

				ALOGD_IF(face_svc_conf->debug.debug_log, "frame: %p face[%d] is the max face, area: %d, id: %d",
					frame, face_rect->id, max_area, max_id);
				face_rect = &det_info->rects.face_rects[max_id];
			}

			/* for app used */
			memcpy(p_rect->rect, face_rect->rect, sizeof(p_rect->rect));
			memcpy(p_rect->rect_smooth, face_rect->rect_smooth, sizeof(p_rect->rect_smooth));
			render_position_map(p_rect);
			p_rect->score = face_rect->score;
			p_rect->id = face_rect->id;
			memcpy(p_rect->kpts, face_rect->kpts, sizeof(p_rect->kpts));
			p_rect->quality_score = face_rect->quality_score;
			memcpy(p_rect->head_pose, face_rect->head_pose, sizeof(p_rect->head_pose));
			p_rect->attr.male = face_rect->attr.male;
			p_rect->attr.wear_glasses = face_rect->attr.wear_glasses;
			p_rect->attr.wear_hat = face_rect->attr.wear_hat;
			p_rect->attr.left_eye_cover = face_rect->attr.left_eye_cover;
			p_rect->attr.right_eye_cover = face_rect->attr.right_eye_cover;
			p_rect->attr.mouse_cover = face_rect->attr.mouse_cover;
			p_rect->attr.mask = face_rect->attr.mask;
			p_rect->yolo_score = face_rect->yolo_score;
			p_rect->yolo_sequence_score =  face_rect->yolo_sequence_score;
			struct viss_isp_exif exif;
			get_rgb_sensor_exif(0, &exif);
			p_rect->exposure_time = exif.exposure_time;
			p_rect->shutter_speed = exif.shutter_speed;
			p_rect->aperture = exif.aperture;
			p_rect->brightness = exif.brightness;
			p_rect->exposure_bias = exif.exposure_bias;
			p_rect->iso_speed = exif.iso_speed;
			p_rect->flash = exif.flash;
			p_rect->illu_id = exif.illu_id;
			p_rect->back_score = exif.back_score;

			struct viss_isp_exif nir_exif;
			get_nir_sensor_exif(0, &nir_exif);
			p_rect->nir_exposure_time = nir_exif.exposure_time;
			p_rect->nir_iso_speed = nir_exif.iso_speed;
			p_rect->nir_avg_lum = nir_exif.res[2];

			det_cb_update_user_event(p_rect);

			/* continue if frame from det api or recognise disable */
			if (frame->type == FACE_FRAME_TYPE_API_DET /* ||
					!face_svc_conf->svc.recg_enable*/) {
				j++;
				continue;
			}

			/* process others frame type */


			if (frame->type == FACE_FRAME_TYPE_RGB) {
				/* process rgb frame */
				pthread_mutex_lock(&svc->tracking_lock);
				face = tracking_get(svc, face_rect->id);

				pthread_mutex_lock(&svc->face_feat_lock);
				if (face && svc->feat_update) {
					svc->feat_update = 0;
					face->recg = 0;
					face->living = 0;
				}
				pthread_mutex_unlock(&svc->face_feat_lock);

				if (face)
					det_cb_recg_repeat_control(face);

				/* process face temp measure */
				if (face_svc_conf->svc.temp_measure_enable) {
					if (!face->temp) {
						if (check_temp_measure_rect(p_rect)) {
							face->temp_status = TEMP_STATUS_PREPARING;
							temp_measure_reset(face->id);
						} else {
							face->temp_status = TEMP_STATUS_MEASURING;
							measure = temp_measure_get(face->id);
							if (measure) {
								face->temp = 1;
								face->temp_status = TEMP_STATUS_DONE;
								face->temp_measure.value = measure->value;
								int ii = 0;
								for (ii = 0; ii < face_svc_conf->svc.temp_read_times; ii++) {
									ALOGV("temp_value[%d] = %f\n", ii, measure->temp_value[ii]);
								}
							}
						}
					}
					p_rect->temp = face->temp;
					p_rect->temp_status = face->temp_status;
					p_rect->temp_measure.value = face->temp_measure.value;
				}

				if (!face) {
					ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
					j++;
					pthread_mutex_unlock(&svc->tracking_lock);
					continue;
				} else if (face->recg && !face_svc_conf->svc.living_check) {
					ALOGI_IF(face_svc_conf->debug.debug_log, "frame: %p face[%d] already recognised", frame, face_rect->id);
					j++;
					/* if face is recg/living return frame to camera faster */
					if (face->frame) {
						free_frame(&face->frame);
						face->frame = NULL;
					}
					//det_cb_recg_repeat_control(face);

					pthread_mutex_unlock(&svc->tracking_lock);
					continue;
				} else if (face->recg && face->living) {
					ALOGI_IF(face_svc_conf->debug.debug_log, "frame: %p face[%d] already recognised and living checked", frame, face_rect->id);
					j++;
					/* if face is recg/living return frame to camera faster */
					if (face->frame) {
						free_frame(&face->frame);
						face->frame = NULL;
					}
					if (face->nir_frame) {
						free_frame(&face->nir_frame);
						face->nir_frame = NULL;
					}

					//det_cb_recg_repeat_control(face);
					pthread_mutex_unlock(&svc->tracking_lock);
					continue;
				} else  if (!face->recg) {

					if (check_head_pose(face, p_rect->head_pose[0], p_rect->head_pose[1], p_rect->head_pose[2])) {
						face->detect_precondition = FACE_HEAD_POSE_ERROR;
					} else {
						face->detect_precondition = FACE_HEAD_POSE_RIGHT;
					}

					if (check_recg_rect(p_rect) || check_recg_face(&p_rect->attr) ||
							(!p_rect->attr.mask &&
							check_head_pose(face, p_rect->head_pose[0], p_rect->head_pose[1], p_rect->head_pose[2]))) {

						face->time = frame->time_stamp;
						long long cur_time = get_systime_us();
						if (face_svc_conf->svc.det_timeout && cur_time > face->timeout_time
							/*&& face->face_refuse == 0*/ && face->frame) {
							ALOGD_IF(face_svc_conf->debug.debug_log,
								"%s svc_timeout %dsec timeout_time %lld cur_time = %lld", __func__,
								face_svc_conf->svc.det_timeout, face->timeout_time, cur_time);
							face->timeout = 1;
							//face->face_refuse = 1;
							//face->face_pass = 0;
							tracking_reset_recg_timeout(svc, face_rect->id);
							save_record_unqualified(svc, face, frame, p_rect, unqualified_frame);
						}

						j++;
						pthread_mutex_unlock(&svc->tracking_lock);
						continue;
					}

					if (!face->frame || face_rect->quality_score >= face->quality_score || !face_svc_conf->svc.recg_enable) {

						/* select the better quality frame */
						if (face->frame)
							free_frame(&face->frame);
						if (face->nir_frame) {
							free_frame(&face->nir_frame);
							face->nir_frame = NULL;
						}

						face->frame = frame;
						retain_frame(frame);

						if (nir_frame) {
							face->nir_frame = nir_frame;
							retain_frame(nir_frame);
						}

						face->quality_score = face_rect->quality_score;
						memcpy(&face->map, map, sizeof(nna_feature_map_t));
						memcpy(&face->nna_rect, face_rect, sizeof(nna_face_t));
						memcpy(&face->rect, p_rect, sizeof(face_rect_t));
					}

					ALOGD_IF(face_svc_conf->debug.profile, "quality debonce %d", face->quality_debonce);
					/*when recg is enable ,select the best picture*/
					if (face_svc_conf->svc.recg_enable && ( face->quality_debonce++ % face_svc_conf->svc.quality_debonce &&
						(face->quality_debonce <= face_svc_conf->svc.min_quality_debonce ||
						face->quality_score < face_svc_conf->svc.quality_threshold ||
						face_svc_conf->svc.quality_threshold == 0))) {

						j++;
						pthread_mutex_unlock(&svc->tracking_lock);
						continue;
					}
				}else{
					//det_cb_recg_repeat_control(face);
				}

				pthread_mutex_unlock(&svc->tracking_lock);
			} /* FACE_FRAME_TYPE_RGB */
			else if (frame->type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK) {
				if (p_rect->quality_score < svc->capture_param->face.quality ||
						check_capture_face_limit(svc->capture_param, p_rect) ||
						check_capture_head_pose(svc->capture_param,
						p_rect->head_pose[0], p_rect->head_pose[1], p_rect->head_pose[2])) {
					j++;
					continue;
				}
			}

			recg_buf = malloc(sizeof(rgb_recg_buf_t));
			if (!recg_buf) {
				ALOGE("%s malloc rgb_recg_buf_t failed, errno: %d (%s)",
					__func__, errno, strerror(errno));

				pthread_mutex_lock(&svc->tracking_lock);
				if (face->frame)
					free_frame(&face->frame);
				if (face->nir_frame)
					free_frame(&face->nir_frame);
				face->frame = NULL;
				face->nir_frame = NULL;
				face->quality_score = 0;
				face->quality_debonce = 1;
				pthread_mutex_unlock(&svc->tracking_lock);

				j++;
				continue;
			}
			memset(recg_buf, 0, sizeof(rgb_recg_buf_t));
			recg_buf->ref++;

			if (frame->type == FACE_FRAME_TYPE_RGB) {
				/* process rgb frame */
				pthread_mutex_lock(&svc->tracking_lock);
				if (!face->recg) {
					if (face_svc_conf->svc.living_check && face->living)
						recg_buf->face_info.feat.living_body = face->feat.living_body;

					/* quality debonce finish */
					recg_buf->frame = face->frame;
					recg_buf->nir_frame = face->nir_frame;

					memcpy(&recg_buf->face_rib.ftmap, &face->map, sizeof(nna_feature_map_t));
					memcpy(&recg_buf->face_rib.rois, &face->nna_rect, sizeof(nna_face_t));
					memcpy(&recg_buf->face_info.rect, &face->rect, sizeof(face_rect_t));
				} else {
					recg_buf->frame = frame;
					recg_buf->nir_frame = nir_frame;
					memcpy(&recg_buf->face_rib.ftmap, map, sizeof(nna_feature_map_t));
					memcpy(&recg_buf->face_rib.rois, face_rect, sizeof(nna_face_t));
					memcpy(&recg_buf->face_info.rect, p_rect, sizeof(face_rect_t));
					memcpy(&recg_buf->face_info.feat, &face->feat, sizeof(face_feat_t));
				}

				if (face_svc_conf->svc.recg_enable && !face->recg && check_recg_permit(svc, face, frame) ) {
					ALOGD_IF(face_svc_conf->debug.profile, "frame: %p face[%d] quality_debonce: %d to do recg",
						recg_buf->frame, face_rect->id, face->quality_debonce);
					retain_frame(recg_buf->frame);
					recg_buf->ref++;

					free_frame(&face->frame);
					face->frame = NULL;
					face->recg_check_count++;

					/* restore face */
					face->quality_score = 0;
					face->quality_debonce = 1;

					frame->recg_check_cnt++;
					recg_buf->face_info.recg_begin = get_systime_us();
					pthread_mutex_lock(&svc->rgb_recg_lock);
					en_queue(&svc->rgb_recg_queue, recg_buf);
					pthread_mutex_unlock(&svc->rgb_recg_lock);

					nn_face_feat(&recg_buf->face_rib);
				}
				if (face_svc_conf->svc.living_check && !face->living &&
						recg_buf->nir_frame && check_living_permit(svc, face, frame)) {
					ALOGD_IF(face_svc_conf->debug.profile, "frame: %p face[%d] to do living",
						recg_buf->frame, face_rect->id);
					retain_frame(recg_buf->frame);
					retain_frame(recg_buf->nir_frame);
					recg_buf->ref++;

					if (face->nir_frame) {
						free_frame(&face->nir_frame);
						face->nir_frame = NULL;
					}

					/* restore face */
					face->quality_score = 0;
					face->quality_debonce = 1;

					frame->living_check_cnt++;
					pthread_mutex_lock(&svc->rgb_living_lock);
					face->living_check_count++;
					en_queue(&svc->rgb_living_queue, recg_buf);
					pthread_cond_signal(&svc->rgb_living_cond);
					pthread_mutex_unlock(&svc->rgb_living_lock);
				} else if (face_svc_conf->svc.living_check && !face->living &&
						face_svc_conf->svc.living_check_mode && check_living_permit(svc, face, frame)) {
					retain_frame(recg_buf->frame);
					recg_buf->ref++;

					frame->living_check_cnt++;
					pthread_mutex_lock(&svc->rgb_living_lock);
					face->living_check_count++;
					en_queue(&svc->rgb_living_queue, recg_buf);
					pthread_cond_signal(&svc->rgb_living_cond);
					pthread_mutex_unlock(&svc->rgb_living_lock);
				}

				/* if face is recg / living return frame to camera faster */
				if (face->recg) {
					if (face->frame) {
						free_frame(&face->frame);
						face->frame = NULL;
					}
				}
				if (face_svc_conf->svc.living_check && face->living) {
					if (face->nir_frame) {
						free_frame(&face->nir_frame);
						face->nir_frame = NULL;
					}
				}
				pthread_mutex_unlock(&svc->tracking_lock);
			} else if (frame->type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK) {
					/* process api frame */
					recg_buf->frame = frame;
					memcpy(&recg_buf->face_rib.ftmap, map, sizeof(nna_feature_map_t));
					memcpy(&recg_buf->face_rib.rois, face_rect, sizeof(nna_face_t));
					memcpy(&recg_buf->face_info.rect, p_rect, sizeof(face_rect_t));
					retain_frame(recg_buf->frame);
					recg_buf->ref++;

					recg_buf->face_info.recg_begin = get_systime_us();
					pthread_mutex_lock(&svc->rgb_recg_lock);
					en_queue(&svc->rgb_recg_queue, recg_buf);
					pthread_mutex_unlock(&svc->rgb_recg_lock);

					nn_face_feat(&recg_buf->face_rib);

				} else {
				/* process api frame */
				recg_buf->frame = frame;
				memcpy(&recg_buf->face_rib.ftmap, map, sizeof(nna_feature_map_t));
				memcpy(&recg_buf->face_rib.rois, face_rect, sizeof(nna_face_t));
				memcpy(&recg_buf->face_info.rect, p_rect, sizeof(face_rect_t));

				recg_buf->ref++;

				recg_buf->face_info.recg_begin = get_systime_us();
				pthread_mutex_lock(&svc->rgb_recg_lock);
				en_queue(&svc->rgb_recg_queue, recg_buf);
				pthread_mutex_unlock(&svc->rgb_recg_lock);

				nn_face_feat(&recg_buf->face_rib);
			}
			pthread_mutex_lock(&svc->tracking_lock);
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
			pthread_mutex_unlock(&svc->tracking_lock);

			j++;
		} /* end of for cnt */


		/* invoke det callback */
		if (frame->type == FACE_FRAME_TYPE_RGB) {

			if (svc->det_cb)
				svc->det_cb(frame, &cb_face_rects);


			pthread_mutex_lock(&svc->tracking_lock);
			free_frame(&frame);
			if (nir_frame)
				free_frame(&nir_frame);
			pthread_mutex_unlock(&svc->tracking_lock);
		} else if (frame->type == FACE_FRAME_TYPE_API_DET && svc->det_cb) {
			svc->det_cb(frame, &cb_face_rects);
		} else if (frame->type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK) {
			if (svc->det_cb)
				svc->det_cb(frame, &cb_face_rects);
			pthread_mutex_lock(&svc->face_capture_lock);
			free_frame(&frame);
			pthread_mutex_unlock(&svc->face_capture_lock);
		}

		free(cb_face_rects.p_rect);
		cb_face_rects.p_rect = NULL;

		if (det_buf)
			free(det_buf);

		return;
	} else {
		/* clean tracking list face */
		if (frame->type == FACE_FRAME_TYPE_RGB) {
			pthread_mutex_lock(&svc->tracking_lock);
			tracking_clean(svc);
			pthread_mutex_unlock(&svc->tracking_lock);
		}

		if (frame->type == FACE_FRAME_TYPE_RGB &&
			face_svc_conf->svc.temp_measure_enable) {
			temp_measure_stop();
		}
		if (frame->type == FACE_FRAME_TYPE_RGB &&
			face_svc_conf->svc.dump_no_faces) {
			face_svc_conf->svc.no_faces++;
			if (!(face_svc_conf->svc.no_faces % face_svc_conf->svc.dump_no_faces_step)) {
				pthread_mutex_lock(&svc->tracking_lock);
				save_frame(svc, frame, no_face);
				pthread_mutex_unlock(&svc->tracking_lock);
			}
		} else if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK) {
			pthread_mutex_lock(&svc->face_lock);
			svc->face_ret = 0;
			pthread_mutex_unlock(&svc->face_lock);
		}
	}

free_res:
	if (frame->type == FACE_FRAME_TYPE_RGB) {
		memset(&cb_face_rects, 0, sizeof(face_rects_t));
		if (svc->det_cb)
			svc->det_cb(frame, &cb_face_rects);

		pthread_mutex_lock(&svc->tracking_lock);
		free_frame(&frame);
		if (nir_frame)
			free_frame(&nir_frame);
		pthread_mutex_unlock(&svc->tracking_lock);
	} else if (frame->type == FACE_FRAME_TYPE_API_DET && svc->det_cb) {
		memset(&cb_face_rects, 0, sizeof(face_rects_t));
		svc->det_cb(frame, &cb_face_rects);
	} else if (frame->type == FACE_FRAME_TYPE_API_RECG && svc->recg_cb) {
		memset(&cb_face_info, 0, sizeof(face_info_t));
		cb_face_info.error = FACE_SVC_ERR;
		svc->recg_cb(frame, &cb_face_info);
	} else if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK) {
		/* wake up block thread even if no face is detected */
		pthread_mutex_lock(&svc->face_lock);
		pthread_cond_signal(&svc->face_cond);
		pthread_mutex_unlock(&svc->face_lock);
	} else if (frame->type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK) {
		memset(&cb_face_rects, 0, sizeof(face_rects_t));
		if (svc->det_cb)
			svc->det_cb(frame, &cb_face_rects);
		pthread_mutex_lock(&svc->face_capture_lock);
		free_frame(&frame);
		pthread_mutex_unlock(&svc->face_capture_lock);
	}

	if (det_buf)
		free(det_buf);
}

static void recg_cb_event(nna_face_rib_t *rib, nna_face_feature_t *feat_info)
{
	node_t *e;
	rgb_recg_buf_t *recg_buf;
	face_frame_t *frame;
	face_info_t *face_info;

	if (face_svc_conf->svc.dump_recg_fps) {
		static long long start = 0;
		long long end = get_systime_us();
		static int fps = 0;

		fps++;
		if (end - start > 1000000) {
			ALOGI("face recg frame %d fps", fps);
			start = get_systime_us();
			fps = 0;
		}
	}

	pthread_mutex_lock(&svc->rgb_recg_lock);
	TAILQ_FOREACH(e, &svc->rgb_recg_queue, nodes) {
		recg_buf = e->buf;
		if (&recg_buf->face_rib == rib) {
			queue_remove(&svc->rgb_recg_queue, e);
			break;
		}
	}
	pthread_mutex_unlock(&svc->rgb_recg_lock);

	frame = recg_buf->frame;
	face_info = &recg_buf->face_info;

	face_info->recg_end = get_systime_us();
	ALOGI_IF(face_svc_conf->debug.profile, "frame: %p face[%d] recognising toke %lld (us)",
		frame, face_info->rect.id, face_info->recg_end - face_info->recg_begin);

	/* seem always have feature */
	if (feat_info && feat_info->feature_size) {
		memcpy(face_info->feat.values,
			feat_info->feature_ref, FACE_FEAT_SIZE);

		if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK &&
				!svc->face_compare) {
			pthread_mutex_lock(&svc->face_lock);
			if (svc->face_index < svc->face_cnt) {
				face_info_t *info = &svc->face_info[svc->face_index];
				memcpy(info, face_info, sizeof(face_info_t));
			}
			if (++svc->face_index == svc->face_cnt) {
				pthread_cond_signal(&svc->face_cond);
			}
			pthread_mutex_unlock(&svc->face_lock);

			pthread_mutex_lock(&svc->tracking_lock);
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
			pthread_mutex_unlock(&svc->tracking_lock);
			return;
		}

		pthread_mutex_lock(&svc->rgb_cmp_lock);
		en_queue(&svc->rgb_cmp_queue, recg_buf);
		pthread_cond_signal(&svc->rgb_cmp_cond);
		pthread_mutex_unlock(&svc->rgb_cmp_lock);
	} else {
		ALOGI("%s unknown error", __func__);
		if (recg_buf->type  == FACE_FRAME_TYPE_LIVING) {
			pthread_mutex_lock(&svc->tracking_lock);
			free_frame(&frame);
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
			pthread_mutex_unlock(&svc->tracking_lock);
		} else if (frame->type == FACE_FRAME_TYPE_RGB) {
			pthread_mutex_lock(&svc->tracking_lock);
			face_info->error |= FACE_SVC_ERR;
			free_rgb_recg_buf(svc, &recg_buf);
			free_frame(&frame);
			pthread_mutex_unlock(&svc->tracking_lock);
		} else if (frame->type == FACE_FRAME_TYPE_API_RECG && svc->recg_cb) {
			face_info->error |= FACE_SVC_ERR;
			svc->recg_cb(frame, face_info);

			pthread_mutex_lock(&svc->tracking_lock);
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
			pthread_mutex_unlock(&svc->tracking_lock);
		} else if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK) {
			/* wake up block thread even if no face is detected */
			pthread_mutex_lock(&svc->face_lock);
			/* svc->face_ret = FACE_SVC_ERR; */
			svc->face_ret = 1; /* FIX ME ? have face but allway get feature fail */
			if (svc->face_index < svc->face_cnt) {
				face_info_t *info = &svc->face_info[svc->face_index];
				memcpy(info, face_info, sizeof(face_info_t));
			}
			if (++svc->face_index == svc->face_cnt) {
				pthread_cond_signal(&svc->face_cond);
			}
			pthread_mutex_unlock(&svc->face_lock);

			pthread_mutex_lock(&svc->tracking_lock);
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
			pthread_mutex_unlock(&svc->tracking_lock);
		} else if (frame->type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK) {
			pthread_mutex_lock(&svc->face_capture_lock);
			free_frame(&frame);
			pthread_mutex_unlock(&svc->face_capture_lock);

			pthread_mutex_lock(&svc->tracking_lock);
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
			pthread_mutex_unlock(&svc->tracking_lock);
		}
	}
}

static int nna_event(void *comp, void *data, unsigned int event, void *event_data)
{
	switch (event) {
		case CB_FACEDET_DONE:
		case CB_FACETRK_DONE:
		{
			det_cb_event(data, event_data);
			break;
		}
		case CB_FACEREC_DONE:
		{
			recg_cb_event(data, event_data);
			break;
		}
		default:
			ALOGE("unknown event");
	}

	return 0;
}

int nn_init(face_svc_t *p)
{
	int ret;
	int time;

	ALOGV("enter %s\n", __func__);

	if (!p) {
		ALOGE("%s invalid parameter", __func__);
		return -1;
	}
	svc = p;

	nna_hdl = (nna_fn_t *)lomboax_open(NULL);
	if (!nna_hdl) {
		ALOGE("%s ax open failed\n", __func__);
		return -1;
	}
	usleep(10);

	/* nna configuration */
	ALOGV("starting NNA_SET_FACE_CFG\n");
	nna_cfg.fdet_en = FACE_DETECT_ENABLED;
	nna_cfg.ftracking_en = FACE_TRACKING_ENABLED;
	nna_cfg.frec_en = face_svc_conf->svc.recg_enable;
	nna_cfg.fquality_en = face_svc_conf->svc.fquality_enable;
	nna_cfg.headpose_en = face_svc_conf->svc.headpose_enable;
	nna_cfg.faceattr_en = face_svc_conf->svc.faceattr_enable;
	nna_cfg.facemask_en = face_svc_conf->svc.mask_det_enable;
	nna_cfg.fmask_rec_en = face_svc_conf->svc.mask_recg_enable;
	nna_cfg.max_det_faces = face_svc_conf->svc.max_track_faces;
	nna_cfg.max_track_frames = 20;
	nna_cfg.visual_track_thresh = 50;
	nna_cfg.use_fast_track = face_svc_conf->svc.use_fast_track;
	//nna_cfg.det_mode = face_svc_conf->svc.nn_det_mode;
	nna_cfg.max_track_faces = face_svc_conf->svc.max_track_faces;
	nna_cfg.fdet_max_size = face_svc_conf->svc.max_face_pixel;
	nna_cfg.fdet_min_size = face_svc_conf->svc.min_face_pixel;
	nna_cfg.frame_width = face_svc_conf->svc.rgb_buffer_width;
	nna_cfg.frame_height = face_svc_conf->svc.rgb_buffer_height;
	nna_cfg.color_en = face_svc_conf->svc.living_color_enable;
	nna_cfg.yolo_en = face_svc_conf->svc.living_yolo_enable;
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
	if (ret) {
		ALOGE("%s starting NNA_SET_FACE_CFG failed, %d", __func__, ret);
		return -1;
	}

	ALOGV("starting NNA_SET_INIT\n");
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_INIT, 0);
	if (ret) {
		ALOGE("%s starting NNA_SET_INIT failed, %d", __func__, ret);
		return -1;
	}
	usleep(50);

	ALOGV("starting NNA_SET_CALLBACK\n");
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_CALLBACK, nna_event);
	if (ret) {
		ALOGE("%s starting NNA_SET_CALLBACK failed, %d", __func__, ret);
		return -1;
	}

	ALOGV("starting NNA_SET_FACEDET_RUN\n");
	time = 0;
	do {
		ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_RUN, 0);
		ALOGV("  return %d", ret);
		usleep(10000);
	} while (ret && ++time < 3);
	if (ret) {
		ALOGE("%s starting NNA_SET_FACEDET_RUN failed, %d", __func__, ret);
		return -1;
	}
	usleep(10000);

	ALOGV("starting NNA_SET_FACETRK_RUN\n");
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACETRK_RUN, 0);
	if (ret) {
		ALOGE("%s starting NNA_SET_FACETRK_RUN failed, %d", __func__, ret);
		return -1;
	}

	ALOGV("starting NNA_SET_FACEREC_RUN\n");
	if (nna_cfg.frec_en) {
		ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEREC_RUN, 0);
		if (ret) {
			ALOGE("%s starting NNA_SET_FACEREC_RUN failed, %d", __func__, ret);
			return -1;
		}
	}

	return ret;
}

int nn_face_det(nna_feature_map_t *map)
{
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_DATA, map);
}

int nn_face_feat(nna_face_rib_t *face_rib)
{
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEREC_DATA, face_rib);
}

int nn_face_feat_cmp(nna_face_feature_t *face_feat)
{
	return nna_hdl->nna_facematch(nna_hdl, face_feat);
}

int nn_set_mask_det_enable(int enable)
{
	nna_cfg.facemask_en = enable;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_set_mask_recg_enable(int enable)
{
	nna_cfg.fmask_rec_en = enable;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_set_max_track_faces(int count)
{
	nna_cfg.max_det_faces = count;
	nna_cfg.max_track_faces = count;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_set_fquality_enable(int enable)
{
	nna_cfg.fquality_en = enable;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_set_headpose_enable(int enable)
{
	nna_cfg.headpose_en = enable;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_set_faceattr_enable(int enable)
{
	nna_cfg.faceattr_en = enable;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_set_use_fast_trak(int use)
{
	nna_cfg.use_fast_track = use;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_set_det_mode(int mode)
{
	//nna_cfg.det_mode = mode;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_face_para_cfg(face_svc_conf_t *face_svc_conf)
{
	nna_cfg.max_det_faces = face_svc_conf->svc.max_track_faces;
	nna_cfg.max_track_faces = face_svc_conf->svc.max_track_faces;
	nna_cfg.fdet_max_size = face_svc_conf->svc.max_face_pixel;
	nna_cfg.fdet_min_size = face_svc_conf->svc.min_face_pixel;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_set_min_face_pixel(int min_face_pixel)
{
	nna_cfg.fdet_min_size = face_svc_conf->svc.min_face_pixel;
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
}

int nn_deinit(void)
{
	ALOGV("enter nn_deinit\n");
	nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_STOP, 0);
	nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEREC_STOP, 0);
	if (nna_hdl) {
		lomboax_close(nna_hdl);
		nna_hdl = NULL;
	}

	return 0;
}
