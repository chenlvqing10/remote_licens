/*
 * face_svc_conf.h
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

#ifndef _FACE_SVC_CONF_H_
#define _FACE_SVC_CONF_H_

#define CONF_PATH "/etc/face_svc.conf"

#define DEFAULT_DEBUG_LOG 1
#define DEFAULT_PROFILE 0
#define DEFAULT_DUMP_SAVE_IMG 1
#define DEFAULT_DUMP_MATCH_FRAME 0
#define DEFAULT_DUMP_MATCH_FACE 0
#define DEFAULT_DUMP_UNMATCH_FRAME 0
#define DEFAULT_DUMP_UNMATCH_FACE 0
#define DEFAULT_DUMP_UNQUALIFIED_FRAME 0
#define DEFAULT_DUMP_UNQUALIFIED_FACE 0
#define DEFAULT_DET_TIMEOUT 0
#define DEFAULT_DET_TIMEOUT_COND 1
#define DEFAULT_DET_TIMEOUT_2ND_RECORD 0
#define DEFAULT_MAX_TRACK_FACES 3
#define DEFAULT_MIN_FACE_PIXEL 80
#define DEFAULT_MAX_FACE_PIXEL 512
#define DEFAULT_QUALITY_DEBONCE 3
#define DEFAULT_MIN_QUALITY_DEBONCE 2 /* when set quality_threshold effective */
#define DEFAULT_QUALITY_THRESHOLD 0
#define DEFAULT_FQUALITY_ENABLE 1
#define DEFAULT_HEADPOSE_ENABLE 1
#define DEFAULT_FACEATTR_ENABLE 1
#define DEFAULT_USE_FAST_TRACK 0
#define DEFAULT_NN_DET_MODE 0
#define DEFAULT_RECG_ENABLE 1
#define DEFAULT_RGB_BUFFER_WIDTH 800
#define DEFAULT_RGB_BUFFER_HEIGHT 1024
#define DEFAULT_RGB_X_MARGIN 0
#define DEFAULT_RGB_Y_MARGIN 128
#define DEFAULT_NIR_BUFFER_WIDTH 576
#define DEFAULT_NIR_BUFFER_HEIGHT 800
#define DEFAULT_NIR_X_EXPAND 72
#define DEFAULT_NIR_Y_EXPAND 48
#define DEFAULT_RECG_RECT_LEFT 0
#define DEFAULT_RECG_RECT_TOP 0
#define DEFAULT_RECG_RECT_RIGHT 800
#define DEFAULT_RECG_RECT_BOTTOM 1024
#define DEFAULT_RECG_MALE -1
#define DEFAULT_RECG_WEAR_GLASSES -1
#define DEFAULT_RECG_WEAR_HAT -1
#define DEFAULT_RECG_LEFT_EYE_COVER -1
#define DEFAULT_RECG_RIGHT_EYE_COVER -1
#define DEFAULT_RECG_MOUSE_COVER -1
#define DEFAULT_RECG_MASK -1
#define DEFAULT_FRAME_RECG_MAX 1
#define DEFAULT_RECG_REPEAT 1
#define DEFAULT_RECG_REPEAT_INTERVAL 1000
#define DEFAULT_SCORE_THRESHOLD 0.54
#define DEFAULT_MASK_DET_ENABLE 1
#define DEFAULT_MASK_RECG_ENABLE 1
#define DEFAULT_MASK_SCORE_THRESHOLD 0.53
#define DEFAULT_MASK_LIVING_ENABLE 1
#define DEFAULT_CONFIDENCE_THRESHOLD 90
#define DEFAULT_LIVING_CHECK 1
#define DEFAULT_LIVING_CHECK_MODE 0
#define DEFAULT_LIVING_CHECK_WITHOUT_UV 0
#define DEFAULT_LIVING_SCORE_THRESHOLD 10
#define DEFAULT_FRAME_LIVING_MAX 5
#define DEFAULT_FACE_EXPOSURE 1
#define DEFAULT_HEAD_POSE_YAW 30
#define DEFAULT_HEAD_POSE_PITCH 30
#define DEFAULT_HEAD_POSE_ROLL 45
#define DEFAULT_ZBAR_ENABLE 0
#define DEFAULT_ZBAR_CODE_TYPE 64 /* ZBAR_QRCODE 64 */
#define DEFAULT_ZBAR_DET_X 200
#define DEFAULT_ZBAR_DET_Y 200
#define DEFAULT_ZBAR_DET_WIDTH 400
#define DEFAULT_ZBAR_DET_HEIGHT 400
#define DEFAULT_ZBAR_TIMEOUT 0
#define DEFAULT_ZBAR_DEBUG 0
#define DEFAULT_TEMP_MEASURE_ENABLE 0
#define DEFAULT_TEMP_RECT_LEFT 0
#define DEFAULT_TEMP_RECT_TOP 0
#define DEFAULT_TEMP_RECT_RIGHT 800
#define DEFAULT_TEMP_RECT_BOTTOM 1024
#define DEFAULT_TEMP_READ_TIMES 5
#define DEFAULT_TEMP_READ_INTERVAL 100 /* ms */
#define DEFAULT_TEMP_MEASURE_DEBUG 0

#define DEFAULT_RGB_ENABLE 1
#define DEFAULT_RGB_DEVICE "/dev/video3"
#define DEFAULT_RGB_WIDTH 1408
#define DEFAULT_RGB_HEIGHT 800
#define DEFAULT_RGB_PREVIEW 1
#define DEFAULT_RGB_PREVIEW_X 0
#define DEFAULT_RGB_PREVIEW_Y 0
#define DEFAULT_RGB_PREVIEW_WIDTH 800
#define DEFAULT_RGB_PREVIEW_HEIGHT 1024
#define DEFAULT_RGB_PREVIEW_ROTATION 90
#define DEFAULT_RGB_ROTATION 90
#define DEFAULT_RGB_MIRROR_ENABLE 0
#define DEFAULT_RGB_MIRROR_DEVICE "/dev/video4"
#define DEFAULT_RGB_MIRROR_WIDTH 1152
#define DEFAULT_RGB_MIRROR_HEIGHT 720
#define DEFAULT_RGB_MIRROR_PREVIEW 0
#define DEFAULT_RGB_MIRROR_PREVIEW_X 440
#define DEFAULT_RGB_MIRROR_PREVIEW_Y 0
#define DEFAULT_RGB_MIRROR_PREVIEW_WIDTH 360
#define DEFAULT_RGB_MIRROR_PREVIEW_HEIGHT 576
#define DEFAULT_RGB_MIRROR_PREVIEW_ROTATION 90
#define DEFAULT_RGB_MIRROR_ROTATION 90
#define DEFAULT_NIR_ENABLE 1
#define DEFAULT_NIR_DEVICE "/dev/video1"
#define DEFAULT_NIR_WIDTH 800
#define DEFAULT_NIR_HEIGHT 600
#define DEFAULT_NIR_PREVIEW 1
#define DEFAULT_NIR_PREVIEW_X 0
#define DEFAULT_NIR_PREVIEW_Y 0
#define DEFAULT_NIR_PREVIEW_WIDTH 300
#define DEFAULT_NIR_PREVIEW_HEIGHT 400
#define DEFAULT_NIR_PREVIEW_ROTATION 90
#define DEFAULT_NIR_ROTATION 90

#define CAL_PARAMS_PATH "/etc/cal_params"

#define DEFAULT_X_SCALE_MEAN 0.725680
#define DEFAULT_X_OFFSET_MEAN 21
#define DEFAULT_K_X -0.147436
#define DEFAULT_B_X 19.615385
#define DEFAULT_X_DETA_MIN -14
#define DEFAULT_X_DETA_MAX 9
#define DEFAULT_Y_SCALE_MEAN 0.729617
#define DEFAULT_Y_OFFSET_MEAN -108
#define DEFAULT_K_Y 0.027778
#define DEFAULT_B_Y -19.972223
#define DEFAULT_Y_DETA_MIN -10
#define DEFAULT_Y_DETA_MAX 9

#define MAP_PARAMS_PATH "/etc/map_params"
#define DEFAULT_X_MAP_0 0.687458
#define DEFAULT_X_MAP_1 -0.015446
#define DEFAULT_X_MAP_2 19.762796
#define DEFAULT_X_MAP_3 -0.092930
#define DEFAULT_X_MAP_4 -0.097486
#define DEFAULT_Y_MAP_0 -0.018391
#define DEFAULT_Y_MAP_1 0.711256
#define DEFAULT_Y_MAP_2 -88.623155
#define DEFAULT_Y_MAP_3 -0.015141
#define DEFAULT_Y_MAP_4 -0.020780

#define DEFAULT_MAP_ORD_IMG_DIR "/data/ord_img"
#define DEFAULT_MAP_ORD_WIDTH 6
#define DEFAULT_MAP_ORD_HEIGHT 9
#define DEFAULT_MAP_ORD_GRID_FACE (5.8)
#define DEFAULT_MAP_MOD_IMG_DIR "/data/mod_img"
#define DEFAULT_MAP_MOD_WIDTH 6
#define DEFAULT_MAP_MOD_HEIGHT 9
#define DEFAULT_MAP_MOD_GRID_FACE (5.8)
#define DEFAULT_MAP_LR 0.005
#define DEFAULT_MAP_LR_CNT_THR 100
#define DEFAULT_MAP_IMG_FLIP 0

#define DEFAULT_ISP_CNT 1

#define DEFAULT_DUMP_YUV 0
#define DEFAULT_DUMP_NO_FACES 0
#define DEFAULT_NO_FACES 0
#define DEFAULT_DUMP_NO_FACES_STEP 15
#define DEFAULT_DUMP_DEBUG_UNMATCH_FRAME 0
#define DEFAULT_DUMP_DEBUG_UNMATCH_FACE 0
#define DEFAULT_DUMP_DEBUG_NIR_LIVING 0
#define DEFAULT_DUMP_DEBUG_NIR_NO_FACE 0
#define DEFAULT_DUMP_DEBUG_NIR_DET_FACE 0
#define DEFAULT_DUMP_DEBUG_RGB_DET_FACE 0
#define DEFAULT_DUMP_DEBUG_RGB_LIVING 0
#define DEFAULT_DUMP_DEBUG_RGB_COLOR_LIVING 0
#define DEFAULT_DUMP_DEBUG_RGB_YOLO_LIVING 0
#define DEFAULT_DUMP_DEBUG_REG_IMG 0
#define DEFAULT_DUMP_LIVING 0
#define DEFAULT_DUMP_FPS 0
#define DEFAULT_DUMP_DET_FPS 0
#define DEFAULT_DUMP_RECG_FPS 0
#define DEFAULT_DUMP_NIR_LIVING_FPS 0
#define DEFAULT_DUMP_RGB_LIVING_FPS 0
#define DEFAULT_MIN_REG_FACE_PIXEL 100
#define DEFAULT_MAX_REG_IMG_SIZE 0
#define DEFAULT_PIXEL_DISTANCE_K -1
#define DEFAULT_PIXEL_DISTANCE_B 256

#define DEFAULT_DET_STEP 2
#define DEFAULT_NN_DET_QUEUE_SIZE 2
#define DEFAULT_FEAT_CMP_SIZE 1024

#define DEFAULT_LIVING_CHECK_COUNT 1
#define DEFAULT_LIVING_YOLO_ENABLE 1
#define DEFAULT_LIVING_COLOR_ENABLE 1
#define DEFAULT_LIVING_CONFIDENCE_THRESHOLD -128
#define DEFAULT_FAKE_YOLO_SCORE 200

#define DEFAULT_CAL_MODE 1
#define DEFAULT_DEVICE_ID 0
#define DEFAULT_RATIO_SIMILAR 1

typedef struct _conf_debug {
	int debug_log;
	int profile;
} conf_debug_t;

typedef struct _conf_svc {
	int dump_save_img;
	int dump_yuv;
	int dump_no_faces;
	int no_faces;
	int dump_no_faces_step;
	int dump_debug_unmatch_frame;
	int dump_debug_unmatch_face;
	int dump_debug_nir_living;
	int dump_debug_nir_no_face;
	int dump_debug_nir_det_face;
	int dump_debug_rgb_det_face;
	int dump_debug_rgb_living;
	int dump_debug_rgb_color_living;
	int dump_debug_rgb_yolo_living;
	int dump_debug_reg_img;
	int dump_unqualified_frame;
	int dump_unqualified_face;
	int dump_match_frame;
	int dump_match_face;
	int dump_unmatch_frame;
	int dump_unmatch_face;
	int det_timeout;
	int det_timeout_cond;
	int det_timeout_2nd_record;
	int dump_living;
	int dump_fps;
	int dump_det_fps;
	int dump_recg_fps;
	int dump_nir_living_fps;
	int dump_rgb_living_fps;
	int max_track_faces;
	int min_face_pixel;
	int max_face_pixel;
	int min_reg_face_pixel;
	int max_reg_img_size;
	float pixel_distance_k;
	float pixel_distance_b;
	int rgb_buffer_width;
	int rgb_buffer_height;
	int rgb_x_margin;
	int rgb_y_margin;
	int nir_buffer_width;
	int nir_buffer_height;
	int nir_x_expand;
	int nir_y_expand;
	int det_step;
	int nn_det_queue_size;
	int quality_threshold;
	int quality_debonce;
	int min_quality_debonce; /* when set quality_threshold effective */
	int fquality_enable;
	int headpose_enable;
	int faceattr_enable;
	int use_fast_track;
	int nn_det_mode;
	int recg_enable;
	int recg_rect_left;
	int recg_rect_top;
	int recg_rect_right;
	int recg_rect_bottom;
	int recg_male;
	int recg_wear_glasses;
	int recg_wear_hat;
	int recg_left_eye_cover;
	int recg_right_eye_cover;
	int recg_mouse_cover;
	int recg_mask;
	int recg_repeat;
	int recg_repeat_interval;
	int frame_recg_max;
	float score_threshold;
	int mask_det_enable;
	int mask_recg_enable;
	float mask_score_threshold;
	int mask_living_enable;
	int confidence_threshold;
	int living_check;
	int living_check_mode;
	int living_check_without_uv;
	int living_score_threshold;
	int living_check_count;
	int living_yolo_enable;
	int living_color_enable;
	int fake_yolo_score;
	int living_confidence_thr;
	int frame_living_max;
	int face_exposure;
	int feat_cmp_size ;
	int head_pose_yaw;
	int head_pose_pitch;
	int head_pose_roll;
	int cal_mode;
	int device_id; /* 0:zhaji, 1:kaoqin */
	/*
	* rgb_buffer_width rgb_preview_width ratio1
	* rgb_buffer_height rgb_preview_height ratio2
	* ratio1 ~= ratio2  is 1  ratio1 != ratio2 is 0
	* default 1, 1: is similar , 0 :is not similar
	*/
	int ratio_similar;
	/* zbar */
	int zbar_enable;
	int zbar_code_type;
	int zbar_det_x;
	int zbar_det_y;
	int zbar_det_width;
	int zbar_det_height;
	int zbar_timeout;
	int zbar_debug;
	/* rgb2nir */
	char map_ord_path[32];
	int map_ord_width;
	int map_ord_height;
	double map_ord_grid_face;
	char map_mod_path[32];
	int map_mod_width;
	int map_mod_height;
	double map_mod_grid_face;
	double map_lr;
	int map_lr_cnt_thr;
	int map_img_flip;

	/* face temp measure */
	int temp_measure_enable;
	int temp_rect_left;
	int temp_rect_top;
	int temp_rect_right;
	int temp_rect_bottom;
	int temp_read_times;
	int temp_read_interval;
	int temp_measure_debug;
} conf_svc_t;

typedef struct _conf_media {
	int rgb_enable;
	char rgb_device[16];
	int rgb_width;
	int rgb_height;
	int rgb_preview;
	int rgb_preview_x;
	int rgb_preview_y;
	int rgb_preview_width;
	int rgb_preview_height;
	int rgb_preview_rotation;
	int rgb_rotation;
	int rgb_mirror_enable;
	char rgb_mirror_device[16];
	int rgb_mirror_width;
	int rgb_mirror_height;
	int rgb_mirror_preview;
	int rgb_mirror_preview_x;
	int rgb_mirror_preview_y;
	int rgb_mirror_preview_width;
	int rgb_mirror_preview_height;
	int rgb_mirror_preview_rotation;
	int rgb_mirror_rotation;
	int nir_enable;
	char nir_device[16];
	int nir_width;
	int nir_height;
	int nir_preview;
	int nir_preview_x;
	int nir_preview_y;
	int nir_preview_width;
	int nir_preview_height;
	int nir_preview_rotation;
	int nir_rotation;
} conf_media_t;

typedef struct _conf_isp {
	int isp_cnt;
} conf_isp_t;

typedef struct _face_svc_conf {
	conf_debug_t debug;
	conf_svc_t svc;
	conf_media_t media;
	conf_isp_t isp;
	pthread_mutex_t conf_mutex;
} face_svc_conf_t;

extern face_svc_conf_t *face_svc_conf;

/*set face svc config data*/
int face_svc_load_conf(void);

/*free face svc config data*/
void face_svc_free_conf(void);

/* update conf lock */
void face_svc_conf_mutex_lock(void);
void face_svc_conf_mutex_unlock(void);

#endif /* _FACE_SVC_CONF_H_ */
