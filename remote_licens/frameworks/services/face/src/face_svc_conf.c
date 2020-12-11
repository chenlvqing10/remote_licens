/*
 * face_svc_conf.c
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
#include <iniparser.h>

#define LOG_TAG "face_svc"
#include <log/log.h>

#include "private.h"
#include "face_svc_main.h"
#include "face_svc_conf.h"

face_svc_conf_t *face_svc_conf;

static char conf_file_path[128];

void face_svc_conf_mutex_init(void)
{
	pthread_mutex_init(&face_svc_conf->conf_mutex, NULL);
	return;
}

void face_svc_conf_mutex_term(void)
{
	pthread_mutex_destroy(&face_svc_conf->conf_mutex);
	return;
}

void face_svc_conf_mutex_lock(void)
{
	pthread_mutex_lock(&face_svc_conf->conf_mutex);
	return;
}

void face_svc_conf_mutex_unlock(void)
{
	pthread_mutex_unlock(&face_svc_conf->conf_mutex);
	return;
}

char *face_svc_get_conf_file_path()
{
	if (strlen(conf_file_path) == 0)
		return CONF_PATH;
	else
		return conf_file_path;
}

void face_svc_set_conf_file_path(const char *path)
{
	strcpy(conf_file_path, path);

	conf_file_path[strlen(conf_file_path)] = '\0';
}

int face_svc_load_conf(void)
{
	face_svc_conf = (face_svc_conf_t *)malloc(sizeof(face_svc_conf_t));
	if (NULL == face_svc_conf) {
		ALOGE("%s %d face svc load conf malloc failed, errno:%s[%d].\n",
			__func__, __LINE__, strerror(errno), errno);
		return -1;
	}

	memset(face_svc_conf, 0, sizeof(face_svc_conf_t));

	face_svc_conf_mutex_init();

	dictionary *conf;

	/* debug conf */
	int debug_log = DEFAULT_DEBUG_LOG;
	int profile = DEFAULT_PROFILE;

	/* svc conf */
	int dump_save_img = DEFAULT_DUMP_SAVE_IMG;
	int dump_yuv = DEFAULT_DUMP_YUV;
	int dump_no_faces = DEFAULT_DUMP_NO_FACES;
	int no_faces = DEFAULT_NO_FACES;
	int dump_no_faces_step = DEFAULT_DUMP_NO_FACES_STEP;
	int dump_debug_unmatch_frame = DEFAULT_DUMP_DEBUG_UNMATCH_FRAME;
	int dump_debug_unmatch_face = DEFAULT_DUMP_DEBUG_UNMATCH_FACE;
	int dump_debug_nir_living = DEFAULT_DUMP_DEBUG_NIR_LIVING;
	int dump_debug_nir_no_face = DEFAULT_DUMP_DEBUG_NIR_NO_FACE;
	int dump_debug_nir_det_face = DEFAULT_DUMP_DEBUG_NIR_DET_FACE;
	int dump_debug_rgb_det_face = DEFAULT_DUMP_DEBUG_RGB_DET_FACE;
	int dump_debug_rgb_living = DEFAULT_DUMP_DEBUG_RGB_LIVING;
	int dump_debug_rgb_color_living = DEFAULT_DUMP_DEBUG_RGB_COLOR_LIVING;
	int dump_debug_rgb_yolo_living = DEFAULT_DUMP_DEBUG_RGB_YOLO_LIVING;
	int dump_debug_reg_img = DEFAULT_DUMP_DEBUG_REG_IMG;
	int dump_unqualified_frame = DEFAULT_DUMP_UNQUALIFIED_FRAME;
	int dump_unqualified_face = DEFAULT_DUMP_UNQUALIFIED_FACE;
	int dump_match_frame = DEFAULT_DUMP_MATCH_FRAME;
	int dump_match_face = DEFAULT_DUMP_MATCH_FACE;
	int dump_unmatch_frame = DEFAULT_DUMP_UNMATCH_FRAME;
	int dump_unmatch_face = DEFAULT_DUMP_UNMATCH_FACE;
	int det_timeout = DEFAULT_DET_TIMEOUT;
	int det_timeout_cond = DEFAULT_DET_TIMEOUT_COND;
	int det_timeout_2nd_record = DEFAULT_DET_TIMEOUT_2ND_RECORD;
	int dump_living = DEFAULT_DUMP_LIVING;
	int dump_fps = DEFAULT_DUMP_FPS;
	int dump_det_fps = DEFAULT_DUMP_DET_FPS;
	int dump_recg_fps = DEFAULT_DUMP_RECG_FPS;
	int dump_nir_living_fps = DEFAULT_DUMP_NIR_LIVING_FPS;
	int dump_rgb_living_fps = DEFAULT_DUMP_RGB_LIVING_FPS;
	int max_track_faces = DEFAULT_MAX_TRACK_FACES;
	int min_face_pixel = DEFAULT_MIN_FACE_PIXEL;
	int max_face_pixel = DEFAULT_MAX_FACE_PIXEL;
	int min_reg_face_pixel = DEFAULT_MIN_REG_FACE_PIXEL;
	int max_reg_img_size = DEFAULT_MAX_REG_IMG_SIZE;
	float pixel_distance_k = DEFAULT_PIXEL_DISTANCE_K;
	float pixel_distance_b = DEFAULT_PIXEL_DISTANCE_B;
	int rgb_buffer_width = DEFAULT_RGB_BUFFER_WIDTH;
	int rgb_buffer_height = DEFAULT_RGB_BUFFER_HEIGHT;
	int rgb_x_margin = DEFAULT_RGB_X_MARGIN;
	int rgb_y_margin = DEFAULT_RGB_Y_MARGIN;
	int nir_buffer_width = DEFAULT_NIR_BUFFER_WIDTH;
	int nir_buffer_height = DEFAULT_NIR_BUFFER_HEIGHT;
	int nir_x_expand = DEFAULT_NIR_X_EXPAND;
	int nir_y_expand = DEFAULT_NIR_Y_EXPAND;
	int det_step = DEFAULT_DET_STEP;
	int nn_det_queue_size = DEFAULT_NN_DET_QUEUE_SIZE;
	int quality_threshold = DEFAULT_QUALITY_THRESHOLD;
	int fquality_enable = DEFAULT_FQUALITY_ENABLE;
	int headpose_enable = DEFAULT_HEADPOSE_ENABLE;
	int faceattr_enable = DEFAULT_FACEATTR_ENABLE;
	int use_fast_track = DEFAULT_USE_FAST_TRACK;
	int nn_det_mode = DEFAULT_NN_DET_MODE;
	int quality_debonce = DEFAULT_QUALITY_DEBONCE;
	int min_quality_debonce = DEFAULT_MIN_QUALITY_DEBONCE;
	int recg_enable = DEFAULT_RECG_ENABLE;
	int recg_rect_left = DEFAULT_RECG_RECT_LEFT;
	int recg_rect_top = DEFAULT_RECG_RECT_TOP;
	int recg_rect_right = DEFAULT_RECG_RECT_RIGHT;
	int recg_rect_bottom = DEFAULT_RECG_RECT_BOTTOM;
	int recg_male = DEFAULT_RECG_MALE;
	int recg_wear_glasses = DEFAULT_RECG_WEAR_GLASSES;
	int recg_wear_hat = DEFAULT_RECG_WEAR_HAT;
	int recg_left_eye_cover = DEFAULT_RECG_LEFT_EYE_COVER;
	int recg_right_eye_cover = DEFAULT_RECG_RIGHT_EYE_COVER;
	int recg_mouse_cover = DEFAULT_RECG_MOUSE_COVER;
	int recg_mask = DEFAULT_RECG_MASK;
	int frame_recg_max = DEFAULT_FRAME_RECG_MAX;
	int recg_repeat = DEFAULT_RECG_REPEAT;
	int recg_repeat_interval = DEFAULT_RECG_REPEAT_INTERVAL;
	float score_threshold = DEFAULT_SCORE_THRESHOLD;
	int mask_det_enable = DEFAULT_MASK_DET_ENABLE;
	int mask_recg_enable = DEFAULT_MASK_RECG_ENABLE;
	float mask_score_threshold = DEFAULT_MASK_SCORE_THRESHOLD;
	int mask_living_enable = DEFAULT_MASK_LIVING_ENABLE;
	int confidence_threshold = DEFAULT_CONFIDENCE_THRESHOLD;
	int living_check = DEFAULT_LIVING_CHECK;
	int living_check_mode = DEFAULT_LIVING_CHECK_MODE;
	int living_check_without_uv = DEFAULT_LIVING_CHECK_WITHOUT_UV;
	int living_score_threshold = DEFAULT_LIVING_SCORE_THRESHOLD;
	int living_check_count = DEFAULT_LIVING_CHECK_COUNT;
	int living_yolo_enable = DEFAULT_LIVING_YOLO_ENABLE;
	int living_color_enable = DEFAULT_LIVING_COLOR_ENABLE;
	int living_confidence_thr = DEFAULT_LIVING_CONFIDENCE_THRESHOLD;
	int fake_yolo_score = DEFAULT_FAKE_YOLO_SCORE;
	int frame_living_max = DEFAULT_FRAME_LIVING_MAX;
	int face_exposure = DEFAULT_FACE_EXPOSURE;
	int feat_cmp_size = DEFAULT_FEAT_CMP_SIZE;
	int head_pose_yaw = DEFAULT_HEAD_POSE_YAW;
	int head_pose_pitch = DEFAULT_HEAD_POSE_PITCH;
	int head_pose_roll = DEFAULT_HEAD_POSE_ROLL;
	int cal_mode = DEFAULT_CAL_MODE;
	int device_id = DEFAULT_DEVICE_ID;
	int ratio_similar = DEFAULT_RATIO_SIMILAR;

	/* zbar */
	int zbar_enable = DEFAULT_ZBAR_ENABLE;
	int zbar_code_type = DEFAULT_ZBAR_CODE_TYPE;
	int zbar_det_x = DEFAULT_ZBAR_DET_X;
	int zbar_det_y = DEFAULT_ZBAR_DET_Y;
	int zbar_det_width = DEFAULT_ZBAR_DET_WIDTH;
	int zbar_det_height = DEFAULT_ZBAR_DET_HEIGHT;
	int zbar_timeout = DEFAULT_ZBAR_TIMEOUT;
	int zbar_debug = DEFAULT_ZBAR_DEBUG;

	/* rgb2nir */
	char *map_ord_path = DEFAULT_MAP_ORD_IMG_DIR;
	int map_ord_width = DEFAULT_MAP_ORD_WIDTH;
	int map_ord_height = DEFAULT_MAP_ORD_HEIGHT;
	double map_ord_grid_face = DEFAULT_MAP_ORD_GRID_FACE;
	char *map_mod_path = DEFAULT_MAP_MOD_IMG_DIR;
	int map_mod_width = DEFAULT_MAP_MOD_WIDTH;
	int map_mod_height = DEFAULT_MAP_MOD_HEIGHT;
	double map_mod_grid_face = DEFAULT_MAP_MOD_GRID_FACE;
	double map_lr = DEFAULT_MAP_LR;
	int map_lr_cnt_thr = DEFAULT_MAP_LR_CNT_THR;
	int map_img_flip = DEFAULT_MAP_IMG_FLIP;

	/* temp_measure */
	int temp_measure_enable = DEFAULT_TEMP_MEASURE_ENABLE;
	int temp_rect_left = DEFAULT_TEMP_RECT_LEFT;
	int temp_rect_top = DEFAULT_TEMP_RECT_TOP;
	int temp_rect_right = DEFAULT_TEMP_RECT_RIGHT;
	int temp_rect_bottom = DEFAULT_TEMP_RECT_BOTTOM;
	int temp_read_times = DEFAULT_TEMP_READ_TIMES;
	int temp_read_interval = DEFAULT_TEMP_READ_INTERVAL;
	int temp_measure_debug = DEFAULT_TEMP_MEASURE_ENABLE;

	/* media conf */
	int rgb_enable = DEFAULT_RGB_ENABLE;
	char *rgb_device = DEFAULT_RGB_DEVICE;
	int rgb_width = DEFAULT_RGB_WIDTH;
	int rgb_height = DEFAULT_RGB_HEIGHT;
	int rgb_preview = DEFAULT_RGB_PREVIEW;
	int rgb_preview_x = DEFAULT_RGB_PREVIEW_X;
	int rgb_preview_y = DEFAULT_RGB_PREVIEW_Y;
	int rgb_preview_width = DEFAULT_RGB_PREVIEW_WIDTH;
	int rgb_preview_height = DEFAULT_RGB_PREVIEW_HEIGHT;
	int rgb_preview_rotation = DEFAULT_RGB_PREVIEW_ROTATION;
	int rgb_rotation = DEFAULT_RGB_ROTATION;
	int rgb_mirror_enable = DEFAULT_RGB_MIRROR_ENABLE;
	char *rgb_mirror_device = DEFAULT_RGB_MIRROR_DEVICE;
	int rgb_mirror_width = DEFAULT_RGB_MIRROR_WIDTH;
	int rgb_mirror_height = DEFAULT_RGB_MIRROR_HEIGHT;
	int rgb_mirror_preview = DEFAULT_RGB_MIRROR_PREVIEW;
	int rgb_mirror_preview_x = DEFAULT_RGB_MIRROR_PREVIEW_X;
	int rgb_mirror_preview_y = DEFAULT_RGB_MIRROR_PREVIEW_Y;
	int rgb_mirror_preview_width = DEFAULT_RGB_MIRROR_PREVIEW_WIDTH;
	int rgb_mirror_preview_height = DEFAULT_RGB_MIRROR_PREVIEW_HEIGHT;
	int rgb_mirror_preview_rotation = DEFAULT_RGB_MIRROR_PREVIEW_ROTATION;
	int rgb_mirror_rotation = DEFAULT_RGB_MIRROR_ROTATION;
	int nir_enable = DEFAULT_NIR_ENABLE;
	char *nir_device = DEFAULT_NIR_DEVICE;
	int nir_width = DEFAULT_NIR_WIDTH;
	int nir_height = DEFAULT_NIR_HEIGHT;
	int nir_preview = DEFAULT_NIR_PREVIEW;
	int nir_preview_x = DEFAULT_NIR_PREVIEW_X;
	int nir_preview_y = DEFAULT_NIR_PREVIEW_Y;
	int nir_preview_width = DEFAULT_NIR_PREVIEW_WIDTH;
	int nir_preview_height = DEFAULT_NIR_PREVIEW_HEIGHT;
	int nir_preview_rotation = DEFAULT_NIR_PREVIEW_ROTATION;
	int nir_rotation = DEFAULT_NIR_ROTATION;

	/* isp conf */
	int isp_cnt = DEFAULT_ISP_CNT;

	(void)no_faces;
	conf = iniparser_load(face_svc_get_conf_file_path());
	if (conf) {
		/* debug conf */
		debug_log = iniparser_getint(conf, "log:debug", DEFAULT_DEBUG_LOG);
		profile = iniparser_getint(conf, "log:profile", DEFAULT_PROFILE);

		/* svc conf */
		dump_save_img = iniparser_getint(conf, "svc:dump_save_img", DEFAULT_DUMP_SAVE_IMG);
		dump_match_frame = iniparser_getint(conf, "svc:dump_match_frame", DEFAULT_DUMP_MATCH_FRAME);
		dump_match_face = iniparser_getint(conf, "svc:dump_match_face", DEFAULT_DUMP_MATCH_FACE);
		dump_unmatch_frame = iniparser_getint(conf, "svc:dump_unmatch_frame", DEFAULT_DUMP_UNMATCH_FRAME);
		dump_unmatch_face = iniparser_getint(conf, "svc:dump_unmatch_face", DEFAULT_DUMP_UNMATCH_FACE);
		dump_unqualified_frame = iniparser_getint(conf, "svc:dump_unqualified_frame", DEFAULT_DUMP_UNQUALIFIED_FRAME);
		dump_unqualified_face = iniparser_getint(conf, "svc:dump_unqualified_face", DEFAULT_DUMP_UNQUALIFIED_FACE);
		det_timeout = iniparser_getint(conf, "svc:det_timeout", DEFAULT_DET_TIMEOUT);
		det_timeout_cond = iniparser_getint(conf, "svc:det_timeout_cond", DEFAULT_DET_TIMEOUT_COND);
		det_timeout_2nd_record = iniparser_getint(conf, "svc:det_timeout_2nd_record", DEFAULT_DET_TIMEOUT_2ND_RECORD);
		max_track_faces = iniparser_getint(conf, "svc:max_track_faces", DEFAULT_MAX_TRACK_FACES);
		min_face_pixel = iniparser_getint(conf, "svc:min_face_pixel", DEFAULT_MIN_FACE_PIXEL);
		max_face_pixel = iniparser_getint(conf, "svc:max_face_pixel", DEFAULT_MAX_FACE_PIXEL);
		quality_debonce = iniparser_getint(conf, "svc:quality_debonce", DEFAULT_QUALITY_DEBONCE);
		min_quality_debonce = iniparser_getint(conf, "svc:min_quality_debonce", DEFAULT_MIN_QUALITY_DEBONCE);
		quality_threshold = iniparser_getint(conf, "svc:quality_threshold", DEFAULT_QUALITY_THRESHOLD);
		fquality_enable = iniparser_getint(conf, "svc:fquality_enable", DEFAULT_FQUALITY_ENABLE);
		headpose_enable = iniparser_getint(conf, "svc:headpose_enable", DEFAULT_HEADPOSE_ENABLE);
		faceattr_enable = iniparser_getint(conf, "svc:faceattr_enable", DEFAULT_FACEATTR_ENABLE);
		use_fast_track = iniparser_getint(conf, "svc:use_fast_track", DEFAULT_USE_FAST_TRACK);
		nn_det_mode = iniparser_getint(conf, "svc:nn_det_mode", DEFAULT_NN_DET_MODE);
		recg_enable = iniparser_getint(conf, "svc:recg_enable", DEFAULT_RECG_ENABLE);
		rgb_buffer_width = iniparser_getint(conf, "svc:rgb_buffer_width", DEFAULT_RGB_BUFFER_WIDTH);
		rgb_buffer_height = iniparser_getint(conf, "svc:rgb_buffer_height", DEFAULT_RGB_BUFFER_HEIGHT);
		rgb_x_margin = iniparser_getint(conf, "svc:rgb_x_margin", DEFAULT_RGB_X_MARGIN);
		rgb_y_margin = iniparser_getint(conf, "svc:rgb_y_margin", DEFAULT_RGB_Y_MARGIN);
		nir_buffer_width = iniparser_getint(conf, "svc:nir_buffer_width", DEFAULT_NIR_BUFFER_WIDTH);
		nir_buffer_height = iniparser_getint(conf, "svc:nir_buffer_height", DEFAULT_NIR_BUFFER_HEIGHT);
		nir_x_expand = iniparser_getint(conf, "svc:nir_x_expand", DEFAULT_NIR_X_EXPAND);
		nir_y_expand = iniparser_getint(conf, "svc:nir_y_expand", DEFAULT_NIR_Y_EXPAND);
		recg_rect_left = iniparser_getint(conf, "svc:recg_rect_left", DEFAULT_RECG_RECT_LEFT);
		recg_rect_top = iniparser_getint(conf, "svc:recg_rect_top", DEFAULT_RECG_RECT_TOP);
		recg_rect_right = iniparser_getint(conf, "svc:recg_rect_right", rgb_buffer_width);
		recg_rect_bottom = iniparser_getint(conf, "svc:recg_rect_bottom", rgb_buffer_height);
		recg_male = iniparser_getint(conf, "svc:recg_male", DEFAULT_RECG_MALE);
		recg_wear_glasses = iniparser_getint(conf, "svc:recg_wear_glasses", DEFAULT_RECG_WEAR_GLASSES);
		recg_wear_hat = iniparser_getint(conf, "svc:recg_wear_hat", DEFAULT_RECG_WEAR_HAT);
		recg_left_eye_cover = iniparser_getint(conf, "svc:recg_left_eye_cover", DEFAULT_RECG_LEFT_EYE_COVER);
		recg_right_eye_cover = iniparser_getint(conf, "svc:recg_right_eye_cover", DEFAULT_RECG_RIGHT_EYE_COVER);
		recg_mouse_cover = iniparser_getint(conf, "svc:recg_mouse_cover", DEFAULT_RECG_MOUSE_COVER);
		recg_mask = iniparser_getint(conf, "svc:recg_mask", DEFAULT_RECG_MASK);
		recg_repeat = iniparser_getint(conf, "svc:recg_repeat", recg_repeat);
		recg_repeat_interval = iniparser_getint(conf, "svc:recg_repeat_interval", recg_repeat_interval);


		score_threshold = iniparser_getdouble(conf, "svc:score_threshold", DEFAULT_SCORE_THRESHOLD);
		mask_det_enable = iniparser_getint(conf, "svc:mask_det_enable", DEFAULT_MASK_DET_ENABLE);
		mask_recg_enable = iniparser_getint(conf, "svc:mask_recg_enable", DEFAULT_MASK_RECG_ENABLE);
		mask_score_threshold = iniparser_getdouble(conf, "svc:mask_score_threshold", DEFAULT_MASK_SCORE_THRESHOLD);
		mask_living_enable = iniparser_getint(conf, "svc:mask_living_enable", DEFAULT_MASK_LIVING_ENABLE);
		confidence_threshold = iniparser_getdouble(conf, "svc:confidence_threshold", DEFAULT_CONFIDENCE_THRESHOLD);
		living_check = iniparser_getint(conf, "svc:living_check", DEFAULT_LIVING_CHECK);
		living_check_mode = iniparser_getint(conf, "svc:living_check_mode", DEFAULT_LIVING_CHECK_MODE);
		living_check_without_uv = iniparser_getint(conf, "svc:living_check_without_uv", DEFAULT_LIVING_CHECK_WITHOUT_UV);
		living_score_threshold = iniparser_getint(conf, "svc:living_score_threshold", DEFAULT_LIVING_SCORE_THRESHOLD);

		face_exposure = iniparser_getint(conf, "svc:face_exposure", DEFAULT_FACE_EXPOSURE);
		head_pose_yaw = iniparser_getint(conf, "svc:head_pose_yaw", DEFAULT_HEAD_POSE_YAW);
		head_pose_pitch = iniparser_getint(conf, "svc:head_pose_pitch", DEFAULT_HEAD_POSE_PITCH);
		head_pose_roll = iniparser_getint(conf, "svc:head_pose_roll", DEFAULT_HEAD_POSE_ROLL);
		cal_mode = iniparser_getint(conf, "svc:cal_mode", DEFAULT_CAL_MODE);
		device_id = iniparser_getint(conf, "svc:device_id", DEFAULT_DEVICE_ID);
		ratio_similar = iniparser_getint(conf, "svc:ratio_similar", DEFAULT_RATIO_SIMILAR);

		/* zbar */
		zbar_enable = iniparser_getint(conf, "svc:zbar_enable", DEFAULT_ZBAR_ENABLE);
		zbar_code_type = iniparser_getint(conf, "svc:zbar_code_type", DEFAULT_ZBAR_CODE_TYPE);
		zbar_det_x = iniparser_getint(conf, "svc:zbar_det_x", DEFAULT_ZBAR_DET_X);
		zbar_det_y = iniparser_getint(conf, "svc:zbar_det_y", DEFAULT_ZBAR_DET_Y);
		zbar_det_width = iniparser_getint(conf, "svc:zbar_det_width", DEFAULT_ZBAR_DET_WIDTH);
		zbar_det_height = iniparser_getint(conf, "svc:zbar_det_height", DEFAULT_ZBAR_DET_HEIGHT);
		zbar_timeout = iniparser_getint(conf, "svc:zbar_timeout", DEFAULT_ZBAR_TIMEOUT);
		zbar_debug = iniparser_getint(conf, "svc:zbar_debug", DEFAULT_ZBAR_DEBUG);

		/* temp measure */
		temp_measure_enable = iniparser_getint(conf, "svc:temp_measure_enable", DEFAULT_TEMP_MEASURE_ENABLE);
		temp_rect_left = iniparser_getint(conf, "svc:temp_rect_left", DEFAULT_TEMP_RECT_LEFT);
		temp_rect_top = iniparser_getint(conf, "svc:temp_rect_top", DEFAULT_TEMP_RECT_TOP);
		temp_rect_right = iniparser_getint(conf, "svc:temp_rect_right", DEFAULT_TEMP_RECT_RIGHT);
		temp_rect_bottom = iniparser_getint(conf, "svc:temp_rect_bottom", DEFAULT_TEMP_RECT_BOTTOM);
		temp_read_times = iniparser_getint(conf, "svc:temp_read_times", DEFAULT_TEMP_READ_TIMES);
		temp_read_interval = iniparser_getint(conf, "svc:temp_read_interval", DEFAULT_TEMP_READ_INTERVAL);
		temp_measure_debug = iniparser_getint(conf, "svc:temp_measure_debug", DEFAULT_TEMP_MEASURE_DEBUG);

		/* media conf */
		rgb_enable = iniparser_getint(conf, "media:rgb_enable", DEFAULT_RGB_ENABLE);
		rgb_device = (char *)iniparser_getstring(conf, "media:rgb_device", DEFAULT_RGB_DEVICE);
		strncpy(face_svc_conf->media.rgb_device, rgb_device, 16);
		rgb_width = iniparser_getint(conf, "media:rgb_width", DEFAULT_RGB_WIDTH);
		rgb_height = iniparser_getint(conf, "media:rgb_height", DEFAULT_RGB_HEIGHT);
		rgb_preview = iniparser_getint(conf, "media:rgb_preview", DEFAULT_RGB_PREVIEW);
		rgb_preview_x = iniparser_getint(conf,
			"media:rgb_preview_x", DEFAULT_RGB_PREVIEW_X);
		rgb_preview_y = iniparser_getint(conf,
			"media:rgb_preview_y", DEFAULT_RGB_PREVIEW_Y);
		rgb_preview_width = iniparser_getint(conf, "media:rgb_preview_width", DEFAULT_RGB_PREVIEW_WIDTH);
		rgb_preview_height = iniparser_getint(conf, "media:rgb_preview_height", DEFAULT_RGB_PREVIEW_HEIGHT);
		rgb_preview_rotation = iniparser_getint(conf, "media:rgb_preview_rotation", DEFAULT_RGB_PREVIEW_ROTATION);
		rgb_rotation = iniparser_getint(conf, "media:rgb_rotation", DEFAULT_RGB_ROTATION);
		rgb_mirror_enable = iniparser_getint(conf, "media:rgb_mirror_enable", DEFAULT_RGB_MIRROR_ENABLE);
		rgb_mirror_device = (char *)iniparser_getstring(conf, "media:rgb_mirror_device", DEFAULT_RGB_MIRROR_DEVICE);
		strncpy(face_svc_conf->media.rgb_mirror_device, rgb_mirror_device, 16);
		rgb_mirror_width = iniparser_getint(conf, "media:rgb_mirror_width", DEFAULT_RGB_MIRROR_WIDTH);
		rgb_mirror_height = iniparser_getint(conf, "media:rgb_mirror_height", DEFAULT_RGB_MIRROR_HEIGHT);
		rgb_mirror_preview = iniparser_getint(conf, "media:rgb_mirror_preview", DEFAULT_RGB_MIRROR_PREVIEW);
		rgb_mirror_preview_x = iniparser_getint(conf,
			"media:rgb_mirror_preview_x", DEFAULT_RGB_MIRROR_PREVIEW_X);
		rgb_mirror_preview_y = iniparser_getint(conf,
			"media:rgb_mirror_preview_y", DEFAULT_RGB_MIRROR_PREVIEW_Y);
		rgb_mirror_preview_width = iniparser_getint(conf, "media:rgb_mirror_preview_width", DEFAULT_RGB_MIRROR_PREVIEW_WIDTH);
		rgb_mirror_preview_height = iniparser_getint(conf, "media:rgb_mirror_preview_height", DEFAULT_RGB_MIRROR_PREVIEW_HEIGHT);
		rgb_mirror_preview_rotation = iniparser_getint(conf, "media:rgb_mirror_preview_rotation", DEFAULT_RGB_MIRROR_PREVIEW_ROTATION);
		rgb_mirror_rotation = iniparser_getint(conf, "media:rgb_mirror_rotation", DEFAULT_RGB_MIRROR_ROTATION);
		nir_enable = iniparser_getint(conf, "media:nir_enable", DEFAULT_NIR_ENABLE);
		nir_device = (char *)iniparser_getstring(conf, "media:nir_device", DEFAULT_NIR_DEVICE);
		strncpy(face_svc_conf->media.nir_device, nir_device, 16);
		nir_width = iniparser_getint(conf, "media:nir_width", DEFAULT_NIR_WIDTH);
		nir_height = iniparser_getint(conf, "media:nir_height", DEFAULT_NIR_HEIGHT);
		nir_preview = iniparser_getint(conf, "media:nir_preview", DEFAULT_NIR_PREVIEW);
		nir_preview_x = iniparser_getint(conf,
			"media:nir_preview_x", DEFAULT_NIR_PREVIEW_X);
		nir_preview_y = iniparser_getint(conf,
			"media:nir_preview_y", DEFAULT_NIR_PREVIEW_Y);
		nir_preview_width = iniparser_getint(conf, "media:nir_preview_width", DEFAULT_NIR_PREVIEW_WIDTH);
		nir_preview_height = iniparser_getint(conf, "media:nir_preview_height", DEFAULT_NIR_PREVIEW_HEIGHT);
		nir_preview_rotation = iniparser_getint(conf, "media:nir_preview_rotation", DEFAULT_NIR_PREVIEW_ROTATION);
		nir_rotation = iniparser_getint(conf, "media:nir_rotation", DEFAULT_NIR_ROTATION);

		/* professional svc conf */
		dump_yuv = iniparser_getint(conf, "svc:dump_yuv", DEFAULT_DUMP_YUV);
		dump_no_faces = iniparser_getint(conf, "svc:dump_no_faces", DEFAULT_DUMP_NO_FACES);
		dump_no_faces_step = iniparser_getint(conf, "svc:dump_no_faces_step", DEFAULT_DUMP_NO_FACES_STEP);
		dump_debug_unmatch_frame = iniparser_getint(conf, "svc:dump_debug_unmatch_frame", DEFAULT_DUMP_DEBUG_UNMATCH_FRAME);
		dump_debug_unmatch_face = iniparser_getint(conf, "svc:dump_debug_unmatch_face", DEFAULT_DUMP_DEBUG_UNMATCH_FACE);
		dump_debug_nir_living = iniparser_getint(conf, "svc:dump_debug_nir_living", DEFAULT_DUMP_DEBUG_NIR_LIVING);
		dump_debug_nir_no_face = iniparser_getint(conf, "svc:dump_debug_nir_no_face", DEFAULT_DUMP_DEBUG_NIR_NO_FACE);
		dump_debug_nir_det_face = iniparser_getint(conf, "svc:dump_debug_nir_det_face", DEFAULT_DUMP_DEBUG_NIR_DET_FACE);
		dump_debug_rgb_det_face = iniparser_getint(conf, "svc:dump_debug_rgb_det_face", DEFAULT_DUMP_DEBUG_RGB_DET_FACE);
		dump_debug_rgb_living = iniparser_getint(conf, "svc:dump_debug_rgb_living", DEFAULT_DUMP_DEBUG_RGB_LIVING);
		dump_debug_rgb_color_living = iniparser_getint(conf, "svc:dump_debug_rgb_color_living", DEFAULT_DUMP_DEBUG_RGB_COLOR_LIVING);
		dump_debug_rgb_yolo_living = iniparser_getint(conf, "svc:dump_debug_rgb_yolo_living", DEFAULT_DUMP_DEBUG_RGB_YOLO_LIVING);
		dump_debug_reg_img = iniparser_getint(conf, "svc:dump_debug_reg_img", DEFAULT_DUMP_DEBUG_REG_IMG);
		dump_living = iniparser_getint(conf, "svc:dump_living", DEFAULT_DUMP_LIVING);
		dump_fps = iniparser_getint(conf, "svc:dump_fps", DEFAULT_DUMP_FPS);
		dump_det_fps = iniparser_getint(conf, "svc:dump_det_fps", DEFAULT_DUMP_DET_FPS);
		dump_recg_fps = iniparser_getint(conf, "svc:dump_recg_fps", DEFAULT_DUMP_RECG_FPS);
		dump_nir_living_fps = iniparser_getint(conf, "svc:dump_nir_living_fps", DEFAULT_DUMP_NIR_LIVING_FPS);
		dump_rgb_living_fps = iniparser_getint(conf, "svc:dump_rgb_living_fps", DEFAULT_DUMP_RGB_LIVING_FPS);
		min_reg_face_pixel = iniparser_getint(conf, "svc:min_reg_face_pixel", DEFAULT_MIN_REG_FACE_PIXEL);
		max_reg_img_size = iniparser_getint(conf, "svc:max_reg_img_size", DEFAULT_MAX_REG_IMG_SIZE);
		pixel_distance_k = iniparser_getdouble(conf, "svc:pixel_distance_k", DEFAULT_PIXEL_DISTANCE_K);
		pixel_distance_b = iniparser_getdouble(conf, "svc:pixel_distance_b", DEFAULT_PIXEL_DISTANCE_B);

		det_step = iniparser_getint(conf, "svc:det_step", DEFAULT_DET_STEP);
		nn_det_queue_size = iniparser_getint(conf, "svc:nn_det_queue_size", DEFAULT_NN_DET_QUEUE_SIZE);
		frame_recg_max = iniparser_getint(conf, "svc:frame_recg_max", DEFAULT_FRAME_RECG_MAX);
		frame_living_max = iniparser_getint(conf, "svc:frame_living_max", DEFAULT_FRAME_LIVING_MAX);
		feat_cmp_size = iniparser_getint(conf, "svc:feat_cmp_size", DEFAULT_FEAT_CMP_SIZE);
		living_check_count = iniparser_getint(conf, "svc:living_check_count", DEFAULT_LIVING_CHECK_COUNT);
		living_yolo_enable = iniparser_getint(conf, "svc:living_yolo_enable", DEFAULT_LIVING_YOLO_ENABLE);
		living_color_enable = iniparser_getint(conf, "svc:living_color_enable", DEFAULT_LIVING_COLOR_ENABLE);
		living_confidence_thr = iniparser_getint(conf, "svc:living_confidence_thr", DEFAULT_LIVING_CONFIDENCE_THRESHOLD);
		fake_yolo_score = iniparser_getint(conf, "svc:fake_yolo_score", DEFAULT_FAKE_YOLO_SCORE);

		/* rgb2nir */
		map_ord_path = (char *)iniparser_getstring(conf, "svc:map_ord_path", DEFAULT_MAP_ORD_IMG_DIR);
		strncpy(face_svc_conf->svc.map_ord_path, map_ord_path, 32);
		map_ord_width = iniparser_getint(conf, "svc:map_ord_width", DEFAULT_MAP_ORD_WIDTH);
		map_ord_height = iniparser_getint(conf, "svc:map_ord_height", DEFAULT_MAP_ORD_HEIGHT);
		map_ord_grid_face = iniparser_getdouble(conf, "svc:map_ord_grid_face", DEFAULT_MAP_ORD_GRID_FACE);
		map_mod_path = (char *)iniparser_getstring(conf, "svc:map_mod_path", DEFAULT_MAP_MOD_IMG_DIR);
		strncpy(face_svc_conf->svc.map_mod_path, map_mod_path, 32);
		map_mod_width = iniparser_getint(conf, "svc:map_mod_width", DEFAULT_MAP_MOD_WIDTH);
		map_mod_height = iniparser_getint(conf, "svc:map_mod_height", DEFAULT_MAP_MOD_HEIGHT);
		map_mod_grid_face = iniparser_getdouble(conf, "svc:map_mod_grid_face", DEFAULT_MAP_MOD_GRID_FACE);
		map_lr = iniparser_getdouble(conf, "svc:map_lr", DEFAULT_MAP_LR);
		map_lr_cnt_thr = iniparser_getdouble(conf, "svc:map_lr_cnt_thr", DEFAULT_MAP_LR_CNT_THR);
		map_img_flip = iniparser_getint(conf, "svc:map_img_flip", DEFAULT_MAP_IMG_FLIP);

		/* isp */
		isp_cnt = iniparser_getint(conf, "isp:isp_cnt", DEFAULT_ISP_CNT);

		iniparser_freedict(conf);
	} else {
		ALOGE("%s load %s failed, used default", __func__,
			face_svc_get_conf_file_path());
	}

	/* debug conf */
	ALOGI("[face debug configuration]");
	ALOGI("  debug_log: %d", debug_log);
	ALOGI("  profile: %d", profile);

	/* svc conf */
	ALOGI("[face svc configuration]");
	ALOGI("  dump_save_img: %d", dump_save_img);
	ALOGI("  dump_match_frame: %d", dump_match_frame);
	ALOGI("  dump_match_face: %d", dump_match_face);
	ALOGI("  dump_unmatch_frame: %d", dump_unmatch_frame);
	ALOGI("  dump_unmatch_face: %d", dump_unmatch_face);
	ALOGI("  dump_unqualified_frame: %d", dump_unqualified_frame);
	ALOGI("  dump_unqualified_face: %d", dump_unqualified_face);
	ALOGI("  det_timeout: %d", det_timeout);
	ALOGI("  det_timeout_cond: %d", det_timeout_cond);
	ALOGI("  det_timeout_2nd_record: %d", det_timeout_2nd_record);
	ALOGI("  max_track_faces: %d", max_track_faces);
	ALOGI("  min_face_pixel: %d", min_face_pixel);
	ALOGI("  max_face_pixel: %d", max_face_pixel);
	ALOGI("  quality_debonce: %d", quality_debonce);
	ALOGI("  min_quality_debonce: %d", min_quality_debonce);
	ALOGI("  quality_threshold: %d", quality_threshold);
	ALOGI("  fquality_enable: %d", fquality_enable);
	ALOGI("  headpose_enable: %d", headpose_enable);
	ALOGI("  faceattr_enable: %d", faceattr_enable);
	ALOGI("  use_fast_track: %d", use_fast_track);
	ALOGI("  nn_det_mode: %d", nn_det_mode);
	ALOGI("  recg_enable: %d", recg_enable);
	ALOGI("  rgb_buffer_width: %d", rgb_buffer_width);
	ALOGI("  rgb_buffer_height: %d", rgb_buffer_height);
	ALOGI("  rgb_x_margin: %d", rgb_x_margin);
	ALOGI("  rgb_y_margin: %d", rgb_y_margin);
	ALOGI("  nir_buffer_width: %d", nir_buffer_width);
	ALOGI("  nir_buffer_height: %d", nir_buffer_height);
	ALOGI("  nir_x_expand: %d", nir_x_expand);
	ALOGI("  nir_y_expand: %d", nir_y_expand);
	ALOGI("  recg_rect_left: %d", recg_rect_left);
	ALOGI("  recg_rect_top: %d", recg_rect_top);
	ALOGI("  recg_rect_right: %d", recg_rect_right);
	ALOGI("  recg_rect_bottom: %d", recg_rect_bottom);
	ALOGI("  recg_male: %d", recg_male);
	ALOGI("  recg_wear_glasses: %d", recg_wear_glasses);
	ALOGI("  recg_wear_hat: %d", recg_wear_hat);
	ALOGI("  recg_left_eye_cover: %d", recg_left_eye_cover);
	ALOGI("  recg_right_eye_cover: %d", recg_right_eye_cover);
	ALOGI("  recg_mouse_cover: %d", recg_mouse_cover);
	ALOGI("  recg_mask: %d", recg_mask);
	ALOGI("  recg_repeat: %d", recg_repeat);
	ALOGI("  recg_repeat_interval: %d", recg_repeat_interval);
	ALOGI("  score_threshold: %f", score_threshold);
	ALOGI("  mask_det_enable: %d", mask_det_enable);
	ALOGI("  mask_recg_enable: %d", mask_recg_enable);
	ALOGI("  mask_score_threshold: %f", mask_score_threshold);
	ALOGI("  mask_living_enable: %d", mask_living_enable);
	ALOGI("  confidence_threshold: %d", confidence_threshold);
	ALOGI("  living_check: %d", living_check);
	ALOGI("  living_check_mode: %d", living_check_mode);
	ALOGI("  living_check_without_uv: %d", living_check_without_uv);
	ALOGI("  living_score_threshold: %d", living_score_threshold);
	ALOGI("  face_exposure: %d", face_exposure);
	ALOGI("  head_pose_yaw: %d", head_pose_yaw);
	ALOGI("  head_pose_pitch: %d", head_pose_pitch);
	ALOGI("  head_pose_roll: %d", head_pose_roll);
	ALOGI("  cal_mode: %d", cal_mode);
	ALOGI("  device_id: %d", device_id);
	ALOGI("  ratio_similar: %d", ratio_similar);

	/* zbar */
	ALOGI("  zbar_enable: %d", zbar_enable);
	ALOGI("  zbar_code_type: %d", zbar_code_type);
	ALOGI("  zbar_det_x: %d", zbar_det_x);
	ALOGI("  zbar_det_y: %d", zbar_det_y);
	ALOGI("  zbar_det_width: %d", zbar_det_width);
	ALOGI("  zbar_det_height: %d", zbar_det_height);
	ALOGI("  zbar_timeout: %d", zbar_timeout);
	ALOGI("  zbar_debug: %d", zbar_debug);

	/* temp measure */
	ALOGI("  temp_measure_enable: %d", temp_measure_enable);
	ALOGI("  temp_rect_left: %d", temp_rect_left);
	ALOGI("  temp_rect_top: %d", temp_rect_top);
	ALOGI("  temp_rect_right: %d", temp_rect_right);
	ALOGI("  temp_rect_bottom: %d", temp_rect_bottom);
	ALOGI("  temp_read_times: %d", temp_read_times);
	ALOGI("  temp_read_interval: %d", temp_read_interval);
	ALOGI("  temp_measure_debug: %d", temp_measure_debug);

	/* media conf */
	ALOGI("[face media configuration]");
	ALOGI("  rgb_enable: %d", rgb_enable);
	ALOGI("  rgb_device: %s", face_svc_conf->media.rgb_device);
	ALOGI("  rgb_width: %d", rgb_width);
	ALOGI("  rgb_height: %d", rgb_height);
	ALOGI("  rgb_preview: %d", rgb_preview);
	ALOGI("  rgb_preview_x: %d", rgb_preview_x);
	ALOGI("  rgb_preview_y: %d", rgb_preview_y);
	ALOGI("  rgb_preview_width: %d", rgb_preview_width);
	ALOGI("  rgb_preview_height: %d", rgb_preview_height);
	ALOGI("  rgb_preview_rotation: %d", rgb_preview_rotation);
	ALOGI("  rgb_rotation: %d", rgb_rotation);
	ALOGI("  rgb_mirror_enable: %d", rgb_mirror_enable);
	ALOGI("  rgb_mirror_device: %s", face_svc_conf->media.rgb_mirror_device);
	ALOGI("  rgb_mirror_width: %d", rgb_mirror_width);
	ALOGI("  rgb_mirror_height: %d", rgb_mirror_height);
	ALOGI("  rgb_mirror_preview: %d", rgb_mirror_preview);
	ALOGI("  rgb_mirror_preview_x: %d", rgb_mirror_preview_x);
	ALOGI("  rgb_mirror_preview_y: %d", rgb_mirror_preview_y);
	ALOGI("  rgb_mirror_preview_width: %d", rgb_mirror_preview_width);
	ALOGI("  rgb_mirror_preview_height: %d", rgb_mirror_preview_height);
	ALOGI("  rgb_mirror_preview_rotation: %d", rgb_mirror_preview_rotation);
	ALOGI("  rgb_mirror_rotation: %d", rgb_mirror_rotation);
	ALOGI("  nir_enable: %d", nir_enable);
	ALOGI("  nir_device: %s", face_svc_conf->media.nir_device);
	ALOGI("  nir_width: %d", nir_width);
	ALOGI("  nir_height: %d", nir_height);
	ALOGI("  nir_preview: %d", nir_preview);
	ALOGI("  nir_preview_x: %d", nir_preview_x);
	ALOGI("  nir_preview_y: %d", nir_preview_y);
	ALOGI("  nir_preview_width: %d", nir_preview_width);
	ALOGI("  nir_preview_height: %d", nir_preview_height);
	ALOGI("  nir_preview_rotation: %d", nir_preview_rotation);
	ALOGI("  nir_rotation: %d", nir_rotation);

	/* professional svc conf */
	ALOGI("[professional svc configuration]");
	ALOGI("  dump_yuv: %d", dump_yuv);
	ALOGI("  dump_no_faces: %d", dump_no_faces);
	ALOGI("  dump_no_faces_step: %d", dump_no_faces_step);
	ALOGI("  dump_debug_unmatch_frame: %d", dump_debug_unmatch_frame);
	ALOGI("  dump_debug_unmatch_face: %d", dump_debug_unmatch_face);
	ALOGI("  dump_debug_nir_living: %d", dump_debug_nir_living);
	ALOGI("  dump_debug_nir_no_face: %d", dump_debug_nir_no_face);
	ALOGI("  dump_debug_nir_det_face: %d", dump_debug_nir_det_face);
	ALOGI("  dump_debug_rgb_det_face: %d", dump_debug_rgb_det_face);
	ALOGI("  dump_debug_rgb_living: %d", dump_debug_rgb_living);
	ALOGI("  dump_debug_rgb_color_living: %d", dump_debug_rgb_color_living);
	ALOGI("  dump_debug_rgb_yolo_living: %d", dump_debug_rgb_yolo_living);
	ALOGI("  dump_debug_reg_img: %d", dump_debug_reg_img);
	ALOGI("  dump_living: %d", dump_living);
	ALOGI("  dump_fps: %d", dump_fps);
	ALOGI("  dump_det_fps: %d", dump_det_fps);
	ALOGI("  dump_recg_fps: %d", dump_recg_fps);
	ALOGI("  dump_nir_living_fps: %d", dump_nir_living_fps);
	ALOGI("  dump_rgb_living_fps: %d", dump_rgb_living_fps);
	ALOGI("  min_reg_face_pixel: %d", min_reg_face_pixel);
	ALOGI("  max_reg_img_size: %d", max_reg_img_size);
	ALOGI("  pixel_distance_k: %f", pixel_distance_k);
	ALOGI("  pixel_distance_b: %f", pixel_distance_b);

	ALOGI("  det_step: %d", det_step);
	ALOGI("  nn_det_queue_size: %d", nn_det_queue_size);
	ALOGI("  frame_recg_max: %d", frame_recg_max);
	ALOGI("  frame_living_max: %d", frame_living_max);
	ALOGI("  feat_cmp_size: %d", feat_cmp_size);
	ALOGI("  living_check_count: %d", living_check_count);
	ALOGI("  living_yolo_enable: %d", living_yolo_enable);
	ALOGI("  living_color_enable: %d", living_color_enable);
	ALOGI("  living_confidence_thr: %d", living_confidence_thr);
	ALOGI("  fake_yolo_score: %d", fake_yolo_score);

	/* rgb2nir */
	ALOGI("  map_ord_path: %s", face_svc_conf->svc.map_ord_path);
	ALOGI("  map_ord_width: %d", map_ord_width);
	ALOGI("  map_ord_height: %d", map_ord_height);
	ALOGI("  map_ord_grid_face: %lf", map_ord_grid_face);
	ALOGI("  map_mod_path: %s", face_svc_conf->svc.map_mod_path);
	ALOGI("  map_mod_width: %d", map_mod_width);
	ALOGI("  map_mod_height: %d", map_mod_height);
	ALOGI("  map_mod_grid_face: %lf", map_mod_grid_face);
	ALOGI("  map_lr: %lf", map_lr);
	ALOGI("  map_lr_cnt_thr: %d", map_lr_cnt_thr);
	ALOGI("  map_img_flip: %d", map_img_flip);

	/* isp conf */
	ALOGI("[face isp configuration]");
	ALOGI("  isp_cnt: %d", isp_cnt);

	/* debug conf */
	face_svc_conf->debug.debug_log = debug_log;
	face_svc_conf->debug.profile = profile;

	/* svc conf */
	face_svc_conf->svc.dump_save_img = dump_save_img;
	face_svc_conf->svc.dump_match_frame = dump_match_frame;
	face_svc_conf->svc.dump_match_face = dump_match_face;
	face_svc_conf->svc.dump_unmatch_frame = dump_unmatch_frame;
	face_svc_conf->svc.dump_unmatch_face = dump_unmatch_face;
	face_svc_conf->svc.dump_unqualified_frame = dump_unqualified_frame;
	face_svc_conf->svc.dump_unqualified_face = dump_unqualified_face;
	face_svc_conf->svc.det_timeout = det_timeout;
	face_svc_conf->svc.det_timeout_cond = det_timeout_cond;
	face_svc_conf->svc.det_timeout_2nd_record = det_timeout_2nd_record;
	face_svc_conf->svc.max_track_faces = max_track_faces;
	face_svc_conf->svc.min_face_pixel = min_face_pixel;
	face_svc_conf->svc.max_face_pixel = max_face_pixel;
	face_svc_conf->svc.quality_debonce = quality_debonce;
	face_svc_conf->svc.min_quality_debonce = min_quality_debonce;
	face_svc_conf->svc.quality_threshold = quality_threshold;
	face_svc_conf->svc.fquality_enable = fquality_enable;
	face_svc_conf->svc.headpose_enable = headpose_enable;
	face_svc_conf->svc.faceattr_enable = faceattr_enable;
	face_svc_conf->svc.use_fast_track = use_fast_track;
	face_svc_conf->svc.nn_det_mode = nn_det_mode;
	face_svc_conf->svc.recg_enable = recg_enable;
	face_svc_conf->svc.rgb_buffer_width = rgb_buffer_width;
	face_svc_conf->svc.rgb_buffer_height = rgb_buffer_height;
	face_svc_conf->svc.rgb_x_margin = rgb_x_margin;
	face_svc_conf->svc.rgb_y_margin = rgb_y_margin;
	face_svc_conf->svc.nir_buffer_width = nir_buffer_width;
	face_svc_conf->svc.nir_buffer_height = nir_buffer_height;
	face_svc_conf->svc.nir_x_expand = nir_x_expand;
	face_svc_conf->svc.nir_y_expand = nir_y_expand;
	face_svc_conf->svc.recg_rect_left = recg_rect_left;
	face_svc_conf->svc.recg_rect_top = recg_rect_top;
	face_svc_conf->svc.recg_rect_right = recg_rect_right;
	face_svc_conf->svc.recg_rect_bottom = recg_rect_bottom;
	face_svc_conf->svc.recg_male = recg_male;
	face_svc_conf->svc.recg_wear_glasses = recg_wear_glasses;
	face_svc_conf->svc.recg_wear_hat = recg_wear_hat;
	face_svc_conf->svc.recg_left_eye_cover = recg_left_eye_cover;
	face_svc_conf->svc.recg_right_eye_cover = recg_right_eye_cover;
	face_svc_conf->svc.recg_mouse_cover = recg_mouse_cover;
	face_svc_conf->svc.recg_repeat = recg_repeat;
	face_svc_conf->svc.recg_repeat_interval = recg_repeat_interval;
	face_svc_conf->svc.recg_mask = recg_mask;
	face_svc_conf->svc.score_threshold = score_threshold;
	face_svc_conf->svc.mask_det_enable = mask_det_enable;
	face_svc_conf->svc.mask_recg_enable = mask_recg_enable;
	face_svc_conf->svc.mask_score_threshold = mask_score_threshold;
	face_svc_conf->svc.mask_living_enable = mask_living_enable;
	face_svc_conf->svc.confidence_threshold = confidence_threshold;
	face_svc_conf->svc.living_check = living_check;
	face_svc_conf->svc.living_check_mode = living_check_mode;
	face_svc_conf->svc.living_check_without_uv = living_check_without_uv;
	face_svc_conf->svc.living_score_threshold = living_score_threshold;
	face_svc_conf->svc.face_exposure = face_exposure;
	face_svc_conf->svc.head_pose_yaw = head_pose_yaw;
	face_svc_conf->svc.head_pose_pitch = head_pose_pitch;
	face_svc_conf->svc.head_pose_roll = head_pose_roll;
	face_svc_conf->svc.cal_mode = cal_mode;
	face_svc_conf->svc.device_id = device_id;
	face_svc_conf->svc.ratio_similar = ratio_similar;

	/* zbar */
	face_svc_conf->svc.zbar_enable = zbar_enable;
	face_svc_conf->svc.zbar_code_type = zbar_code_type;
	face_svc_conf->svc.zbar_det_x = zbar_det_x;
	face_svc_conf->svc.zbar_det_y = zbar_det_y;
	face_svc_conf->svc.zbar_det_width = zbar_det_width;
	face_svc_conf->svc.zbar_det_height = zbar_det_height;
	face_svc_conf->svc.zbar_timeout = zbar_timeout;
	face_svc_conf->svc.zbar_debug = zbar_debug;

	/* temp measure */
	face_svc_conf->svc.temp_measure_enable = temp_measure_enable;
	face_svc_conf->svc.temp_rect_left = temp_rect_left;
	face_svc_conf->svc.temp_rect_top = temp_rect_top;
	face_svc_conf->svc.temp_rect_right = temp_rect_right;
	face_svc_conf->svc.temp_rect_bottom = temp_rect_bottom;
	face_svc_conf->svc.temp_read_times = temp_read_times;
	face_svc_conf->svc.temp_read_interval = temp_read_interval;
	face_svc_conf->svc.temp_measure_debug = temp_measure_debug;

	/* media conf */
	face_svc_conf->media.rgb_enable = rgb_enable;
	face_svc_conf->media.rgb_width = rgb_width;
	face_svc_conf->media.rgb_height = rgb_height;
	face_svc_conf->media.rgb_preview = rgb_preview;
	face_svc_conf->media.rgb_preview_x = rgb_preview_x;
	face_svc_conf->media.rgb_preview_y = rgb_preview_y;
	face_svc_conf->media.rgb_preview_width = rgb_preview_width;
	face_svc_conf->media.rgb_preview_height = rgb_preview_height;
	face_svc_conf->media.rgb_preview_rotation = rgb_preview_rotation;
	face_svc_conf->media.rgb_rotation = rgb_rotation;
	face_svc_conf->media.rgb_mirror_enable = rgb_mirror_enable;
	face_svc_conf->media.rgb_mirror_width = rgb_mirror_width;
	face_svc_conf->media.rgb_mirror_height = rgb_mirror_height;
	face_svc_conf->media.rgb_mirror_preview = rgb_mirror_preview;
	face_svc_conf->media.rgb_mirror_preview_x = rgb_mirror_preview_x;
	face_svc_conf->media.rgb_mirror_preview_y = rgb_mirror_preview_y;
	face_svc_conf->media.rgb_mirror_preview_width = rgb_mirror_preview_width;
	face_svc_conf->media.rgb_mirror_preview_height = rgb_mirror_preview_height;
	face_svc_conf->media.rgb_mirror_preview_rotation = rgb_mirror_preview_rotation;
	face_svc_conf->media.rgb_mirror_rotation = rgb_mirror_rotation;
	face_svc_conf->media.nir_enable = nir_enable;
	face_svc_conf->media.nir_width = nir_width;
	face_svc_conf->media.nir_height = nir_height;
	face_svc_conf->media.nir_preview = nir_preview;
	face_svc_conf->media.nir_preview_x = nir_preview_x;
	face_svc_conf->media.nir_preview_y = nir_preview_y;
	face_svc_conf->media.nir_preview_width = nir_preview_width;
	face_svc_conf->media.nir_preview_height = nir_preview_height;
	face_svc_conf->media.nir_preview_rotation = nir_preview_rotation;
	face_svc_conf->media.nir_rotation = nir_rotation;

	/* professional svc conf */
	face_svc_conf->svc.dump_yuv = dump_yuv;
	face_svc_conf->svc.dump_no_faces = dump_no_faces;
	face_svc_conf->svc.dump_no_faces_step = dump_no_faces_step;
	face_svc_conf->svc.dump_debug_unmatch_frame = dump_debug_unmatch_frame;
	face_svc_conf->svc.dump_debug_unmatch_face = dump_debug_unmatch_face;
	face_svc_conf->svc.dump_debug_nir_living = dump_debug_nir_living;
	face_svc_conf->svc.dump_debug_nir_no_face = dump_debug_nir_no_face;
	face_svc_conf->svc.dump_debug_nir_det_face = dump_debug_nir_det_face;
	face_svc_conf->svc.dump_debug_rgb_det_face = dump_debug_rgb_det_face;
	face_svc_conf->svc.dump_debug_rgb_living = dump_debug_rgb_living;
	face_svc_conf->svc.dump_debug_rgb_color_living = dump_debug_rgb_color_living;
	face_svc_conf->svc.dump_debug_rgb_yolo_living = dump_debug_rgb_yolo_living;
	face_svc_conf->svc.dump_debug_reg_img = dump_debug_reg_img;
	face_svc_conf->svc.dump_living = dump_living;
	face_svc_conf->svc.dump_fps = dump_fps;
	face_svc_conf->svc.dump_det_fps = dump_det_fps;
	face_svc_conf->svc.dump_recg_fps = dump_recg_fps;
	face_svc_conf->svc.dump_nir_living_fps = dump_nir_living_fps;
	face_svc_conf->svc.dump_rgb_living_fps = dump_rgb_living_fps;
	face_svc_conf->svc.min_reg_face_pixel = min_reg_face_pixel;
	face_svc_conf->svc.max_reg_img_size = max_reg_img_size;
	face_svc_conf->svc.pixel_distance_k = pixel_distance_k;
	face_svc_conf->svc.pixel_distance_b = pixel_distance_b;
	face_svc_conf->svc.det_step = det_step;
	face_svc_conf->svc.nn_det_queue_size = nn_det_queue_size;
	face_svc_conf->svc.frame_recg_max = frame_recg_max;
	face_svc_conf->svc.frame_living_max = frame_living_max;
	face_svc_conf->svc.feat_cmp_size = feat_cmp_size;
	face_svc_conf->svc.living_check_count = living_check_count;
	face_svc_conf->svc.living_yolo_enable = living_yolo_enable;
	face_svc_conf->svc.living_color_enable = living_color_enable;
	face_svc_conf->svc.living_confidence_thr = living_confidence_thr;
	face_svc_conf->svc.fake_yolo_score = fake_yolo_score;

	/* rgb2nir */
	face_svc_conf->svc.map_ord_width = map_ord_width;
	face_svc_conf->svc.map_ord_height = map_ord_height;
	face_svc_conf->svc.map_ord_grid_face = map_ord_grid_face;
	face_svc_conf->svc.map_mod_width = map_mod_width;
	face_svc_conf->svc.map_mod_height = map_mod_height;
	face_svc_conf->svc.map_mod_grid_face = map_mod_grid_face;
	face_svc_conf->svc.map_lr = map_lr;
	face_svc_conf->svc.map_lr_cnt_thr = map_lr_cnt_thr;
	face_svc_conf->svc.map_img_flip = map_img_flip;

	/* isp conf */
	face_svc_conf->isp.isp_cnt = isp_cnt;

	return 0;
}

void face_svc_free_conf(void)
{
	if (face_svc_conf) {
		face_svc_conf_mutex_term();
		free(face_svc_conf);
		face_svc_conf = NULL;
	}
}

#define BOARD_VERSION "/sys/class/lombo_camera/cccore/board_version"

void face_svc_load_cal_params(face_svc_t *svc)
{
	FILE *fp;
	char buf[4];
	char path[FILENAME_MAX];
	dictionary *conf;
	cal_params_t *params;

	if (!svc) {
		ALOGE("%s invalid paramter", __func__);
		return;
	}

	params = &svc->cal_params;

	snprintf(path, FILENAME_MAX, "%s", CAL_PARAMS_PATH);
	fp = fopen(BOARD_VERSION, "rb");
	if (fp) {
		if (fread(buf, 1, 1, fp) == 1 && buf[0] == '0')
			strncat(path, ".0", FILENAME_MAX);

		fclose(fp);
	}

	ALOGI("calibration parameters path: %s", path);

	conf = iniparser_load(path);
	if (conf) {
		params->xScaleMean = iniparser_getdouble(conf, "default:xScaleMean", DEFAULT_X_SCALE_MEAN);
		params->xOffsetMean = iniparser_getint(conf, "default:xOffsetMean", DEFAULT_X_OFFSET_MEAN);
		params->kX = iniparser_getdouble(conf, "default:kX", DEFAULT_K_X);
		params->bX = iniparser_getdouble(conf, "default:bX", DEFAULT_B_X);
		params->xDetaMin = iniparser_getint(conf, "default:xDetaMin", DEFAULT_X_DETA_MIN);
		params->xDetaMax = iniparser_getint(conf, "default:xDetaMax", DEFAULT_X_DETA_MAX);
		params->yScaleMean = iniparser_getdouble(conf, "default:yScaleMean", DEFAULT_Y_SCALE_MEAN);
		params->yOffsetMean = iniparser_getint(conf, "default:yOffsetMean", DEFAULT_Y_OFFSET_MEAN);
		params->kY = iniparser_getdouble(conf, "default:kY", DEFAULT_K_Y);
		params->bY = iniparser_getdouble(conf, "default:bY", DEFAULT_B_Y);
		params->yDetaMin = iniparser_getint(conf, "default:yDetaMin", DEFAULT_Y_DETA_MIN);
		params->yDetaMax = iniparser_getint(conf, "default:yDetaMax", DEFAULT_Y_DETA_MAX);

		iniparser_freedict(conf);
	} else {
		params->xScaleMean = DEFAULT_X_SCALE_MEAN;
		params->xOffsetMean = DEFAULT_X_OFFSET_MEAN;
		params->kX = DEFAULT_K_X;
		params->bX = DEFAULT_B_X;
		params->xDetaMin = DEFAULT_X_DETA_MIN;
		params->xDetaMax = DEFAULT_X_DETA_MAX;
		params->yScaleMean = DEFAULT_Y_SCALE_MEAN;
		params->yOffsetMean = DEFAULT_Y_OFFSET_MEAN;
		params->kY = DEFAULT_K_Y;
		params->bY = DEFAULT_B_Y;
		params->yDetaMin = DEFAULT_Y_DETA_MIN;
		params->yDetaMax = DEFAULT_Y_DETA_MAX;
	}

	ALOGI("face service cameras calibration parameters");
	ALOGI("  xScaleMean: %f", params->xScaleMean);
	ALOGI("  xOffsetMean: %d", params->xOffsetMean);
	ALOGI("  kX: %f", params->kX);
	ALOGI("  bX: %f", params->bX);
	ALOGI("  xDetaMin: %d", params->xDetaMin);
	ALOGI("  xDetaMax: %d", params->xDetaMax);
	ALOGI("  yScaleMean: %f", params->yScaleMean);
	ALOGI("  yOffsetMean: %d", params->yOffsetMean);
	ALOGI("  kY: %f", params->kY);
	ALOGI("  bY: %f", params->bY);
	ALOGI("  yDetaMin: %d", params->yDetaMin);
	ALOGI("  yDetaMax: %d", params->yDetaMax);
}

int face_svc_load_private_map_params(face_svc_t *svc)
{
	int i = 0;
	int ret = 0;
	face_map_params_t face_map_param;
	map_params_t *params;

	if (!svc) {
		ALOGE("%s invalid paramter", __func__);
		return -1;
	}

	params = &svc->map_params;
	memset(&face_map_param, 0, sizeof(face_map_params_t));
	ret = face_svc_get_private_data(RGBNIR_MAP_OFFSET, (char *)&face_map_param,
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
		params->x_map[i] = face_map_param.x_map[i];
		params->y_map[i] = face_map_param.y_map[i];
	}
	for (i = 0; i < 5; i++) {
		ALOGI("  x_map[%d]: %lf", i, params->x_map[i]);
	}
	for (i = 0; i < 5; i++) {
		ALOGI("  y_map[%d]: %lf", i, params->y_map[i]);
	}
	return 0;
}

void face_svc_load_map_params(face_svc_t *svc)
{
	int i;
	char path[FILENAME_MAX];
	dictionary *conf;
	map_params_t *params;

	if (!svc) {
		ALOGE("%s invalid paramter", __func__);
		return;
	}

	params = &svc->map_params;

	snprintf(path, FILENAME_MAX, "%s", MAP_PARAMS_PATH);
	ALOGI("map parameters path: %s", path);

	conf = iniparser_load(path);
	if (conf) {
		params->x_map[0] = iniparser_getdouble(conf, "map:x_map_0", DEFAULT_X_MAP_0);
		params->x_map[1] = iniparser_getdouble(conf, "map:x_map_1", DEFAULT_X_MAP_1);
		params->x_map[2] = iniparser_getdouble(conf, "map:x_map_2", DEFAULT_X_MAP_2);
		params->x_map[3] = iniparser_getdouble(conf, "map:x_map_3", DEFAULT_X_MAP_3);
		params->x_map[4] = iniparser_getdouble(conf, "map:x_map_4", DEFAULT_X_MAP_4);

		params->y_map[0] = iniparser_getdouble(conf, "map:y_map_0", DEFAULT_Y_MAP_0);
		params->y_map[1] = iniparser_getdouble(conf, "map:y_map_1", DEFAULT_Y_MAP_1);
		params->y_map[2] = iniparser_getdouble(conf, "map:y_map_2", DEFAULT_Y_MAP_2);
		params->y_map[3] = iniparser_getdouble(conf, "map:y_map_3", DEFAULT_Y_MAP_3);
		params->y_map[4] = iniparser_getdouble(conf, "map:y_map_4", DEFAULT_Y_MAP_4);

		iniparser_freedict(conf);
	} else {
		params->x_map[0] = DEFAULT_X_MAP_0;
		params->x_map[1] = DEFAULT_X_MAP_1;
		params->x_map[2] = DEFAULT_X_MAP_2;
		params->x_map[3] = DEFAULT_X_MAP_3;
		params->x_map[4] = DEFAULT_X_MAP_4;

		params->y_map[0] = DEFAULT_Y_MAP_0;
		params->y_map[1] = DEFAULT_Y_MAP_1;
		params->y_map[2] = DEFAULT_Y_MAP_2;
		params->y_map[3] = DEFAULT_Y_MAP_3;
		params->y_map[4] = DEFAULT_Y_MAP_4;
	}
	for (i = 0; i < 5; i++) {
		ALOGI("  x_map[%d]: %lf", i, params->x_map[i]);
	}
	for (i = 0; i < 5; i++) {
		ALOGI("  y_map[%d]: %lf", i, params->y_map[i]);
	}

	/* if private have map params will update params struct */
	face_svc_load_private_map_params(svc);
}

void face_svc_load_params(face_svc_t *svc)
{
	if(face_svc_conf->svc.cal_mode == 1) {
		face_svc_load_cal_params(svc);
	} else if (face_svc_conf->svc.cal_mode == 2) {
		face_svc_load_map_params(svc);
	} else {
		ALOGE("%s unsupport cal mode %d!", __func__, face_svc_conf->svc.cal_mode);
	}
}

