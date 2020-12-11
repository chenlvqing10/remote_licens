/*
 * face_svc.h - Face service hearder
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

#ifndef _FACE_SVC_H_
#define _FACE_SVC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include "video_recorder.h"

#define FACE_FEAT_SIZE		1024

/** Failed error code when importing personnel registration information */
typedef enum {
	FACE_ERROR = -1,
	FACE_SUCCESS = 0,		/* OK */

	FACE_SVC_INTERNAL_ERROR,

	/* get face feature error code */
	FACE_REG_PIC_NO_HAVE,	/* No picture */
	FACE_REG_PIC_DECODE_FAIL,
	FACE_REG_PIC_FLIP_FAIL,
	FACE_REG_PIC_NO_DETECT_FACE,
	FACE_REG_PIC_MULTIPLE_FACE,
	FACE_REG_PIC_SIZE_LARGER,
	FACE_REG_FACE_SIZE_LARGER,
	FACE_REG_FACE_SIZE_SMALLER,
	FACE_REG_PIC_QUALITY_TOO_LOW,
	FACE_REG_FACE_RECT_LIMIT,
} e_error_code;

typedef enum {
	debug_frame = 0,
	debug_unmatch_frame,
	debug_unmatch_face,
	debug_nir_living,
	debug_nir_no_living,
	debug_nir_no_face,
	debug_nir_det_face,
	debug_rgb_det_face,
	debug_rgb_living,
	debug_rgb_no_living,
	debug_rgb_color_living,
	debug_rgb_yolo_living,
	unmatch_frame,
	unmatch_face,
	match_frame,
	match_face,
	unqualified_frame,
	unqualified_face,
	living,
	no_face,
	record_img_max
} record_img_e;

typedef enum face_frame_type {
	FACE_FRAME_TYPE_RGB,
	FACE_FRAME_TYPE_NIR,
	FACE_FRAME_TYPE_API_DET,
	FACE_FRAME_TYPE_API_DET_BLOCK,
	FACE_FRAME_TYPE_API_RECG,
	FACE_FRAME_TYPE_API_RECG_BLOCK,
	FACE_FRAME_TYPE_API_CAPTURE_BLOCK,
	FACE_FRAME_TYPE_RGB_MIRROR,
	FACE_FRAME_TYPE_LIVING, /* for living check */
} face_frame_type_e;

typedef enum face_temp_status {
	TEMP_STATUS_NONE = 0,
	TEMP_STATUS_PREPARING,
	TEMP_STATUS_MEASURING,
	TEMP_STATUS_DONE,
} face_temp_status_e;

typedef struct _dump_img_config_s {
	int dump_save_img;
	int dump_unqualified_frame;
	int dump_unqualified_face;
	int dump_match_frame;
	int dump_match_face;
	int dump_unmatch_frame;
	int dump_unmatch_face;
	int det_timeout;
} dump_img_config_t;

typedef struct face_temp_measures {
	float value;
} face_temp_measure_t;

enum {
    /*poseture error and no recg timeout*/
	FACE_STATE_DETECT_TIMEOUT=0,
	FACE_STATE_DETECT_PASS,
	FACE_STATE_TEMP_STATUS_PREPARING,
	FACE_STATE_TEMP_STATUS_MEASURING,
	FACE_STATE_TEMP_STATUS_DONE,
	/*psy body*/
	FACE_STATE_LIVING_RECG_TIMEOUT,
	FACE_STATE_LIVING_BODY,
	FACE_STATE_RECG_REFUSE,
	FACE_STATE_RECG_PASS,
	FACE_STATE_RECG_MASK,
};

/* Face feature value */
typedef struct _feat {
	signed char values[FACE_FEAT_SIZE];
} feat_t;

/* Face key porint info */
typedef struct _kpts {
	float kpts[10];
} kpts_t;

/* Face feature info */
typedef struct _feats {
	feat_t *feat;	/* face feature array */
	float *norm;	/* face norm array
			ATTENTION: a face has two norm */
	kpts_t *kpts;	/* face key points array */
	int cnt;	/* face count in all */
	int version;
} feats_t;

/* Face service configuration */
typedef struct _face_config {
	float score_threshold;	/* score threshold of comparison for recognition */
	int living;		/* live detection switch, 0: off 1: on */
	int min_face_pixel;
	int max_face_pixel;
	int max_track_faces;
} face_config_t;

typedef struct _face_attr {
	int male;
	int wear_glasses;
	int wear_hat;
	int left_eye_cover;
	int right_eye_cover;
	int mouse_cover;
	int mask;
} face_attr_t;

typedef struct _face_rect {
	int rect[4];		/* face position [left, top, right, bottom],
				rect_smooth is better choice for drawing face rectangle
				*/
	int rect_smooth[4];	/* the face position with tracking and smoothing */
	int rect_render[4];	/* the face position for ui render */
	int quality_score;	/* quality of face */
	int id;			/* face is, same id when the same person */
	float kpts[10];		/* face key point coordinates */
	int score;		/* face detect socre, useless for now */
	float head_pose[3];	/* head gesture:
				head_pose[0]: yaw angle
				head_pose[1]: pitch angle
				head_pose[2]: roll angle
				*/
	int face_event;
	int face_state;
	face_attr_t attr;

	/* Following fields are for debug */
	int yolo_score;
	int yolo_sequence_score;
	unsigned int exposure_time;
	unsigned int shutter_speed;
	unsigned int aperture;
	unsigned int brightness;
	unsigned int exposure_bias;
	unsigned int iso_speed;
	unsigned int flash;
	unsigned int illu_id;
	unsigned int back_score;
	unsigned int res[16];

	unsigned int nir_exposure_time;
	unsigned int nir_iso_speed;
	unsigned int nir_avg_lum;

	/* temp measure */
	int temp;
	face_temp_status_e temp_status;
	face_temp_measure_t temp_measure;
} face_rect_t;

typedef struct _face_rects {
	face_rect_t *p_rect;	/* face rect array */
	int cnt;		/* face rect count */
	int error;		/* useless for now */
} face_rects_t;

typedef struct _face_feat {
	signed char values[FACE_FEAT_SIZE];	/* face feature value */
	float norm[2];				/* face norm */
	float score;		/* the max score in feats datebase */
	int max_id;		/* the index of max score face in feats */
	int living_body;	/* is the face living
				1: living
				0: not living
				*/
	int cmp_conf;		/* compare confidence */
	int feat_version; /* cmp db version */
	signed char living_values[FACE_FEAT_SIZE];	/* face living feature value */
	int review;
} face_feat_t;

typedef struct _face_info {
	face_rect_t rect;	/* face rect */
	face_rect_t nir_rect;	/* nir faces rect array for debug */
	face_feat_t feat;	/* face feature array */
	int error;

	/* Following field are for debug */
	long long recg_begin;
	long long recg_end;
	long long cmp_begin;
	long long cmp_end;
	long long living_begin;
	long long living_end;
} face_info_t;

typedef struct _record_img_t {
	char path[record_img_max][FILENAME_MAX];
} record_img_t;

typedef struct _face_record {
	face_info_t face_info;
	int face_refuse;
	int face_pass;
	long long time; /* face_refuse/face_pass frame timestamp as record time */
	int recg;
	int living;
	int timeout;
	record_img_e type;
	record_img_t img;
	long long add_timestamp; /* add face timestamp */
	long long recg_timestamp; /* recg finish timestamp */
	long long living_timestamp; /* living finish timestamp */
} face_record_t;

typedef struct _nir_det_rect {
	int x;
	int y;
	int width;
	int height;
	float score;
} nir_det_rect_t;

typedef struct _nir_det_rects {
	nir_det_rect_t *p_rect;	/* nir face rect array */
	int size;
	int capacity;
} nir_det_rects_t;

typedef struct _face_frame {
	face_frame_type_e type;
	unsigned char *viraddr[3];
	unsigned long phyaddr[3];
	int size[3];
	int width;
	int height;
	int color_fmt;
	long long time_stamp;	/* timestamp from v4l2 */

	/* Internal field */
	int ref;		/* frame refrence counter */
	long long det_begin;
	long long det_end;
	int recg_check_cnt;
	int living_check_cnt;

	void *priv;
} face_frame_t;

/* face detection callback */
typedef void *(*face_det_cb)(face_frame_t *, face_rects_t *);
/* face recognition callback */
typedef void *(*face_recg_cb)(face_frame_t *, face_info_t *);
/* face record callback */
typedef void *(*face_record_cb)(face_frame_t *, face_record_t *);
/* nir full image face detection callback */
typedef void *(*face_nir_det_cb)(face_frame_t *, nir_det_rects_t *);

/**
 * face_svc_init -Initialize face service
 * @None
 *
 * Return the handle of face service
 */
void *face_svc_init(void);

/**
 * face_svc_handle -Get face service handle
 * @None
 *
 * Return the handle of face service if face service has been initialized,
 * otherwise return NULL.
 */
void *face_svc_handle(void);

/**
 * face_svc_deinit -Deinit face service
 * @None
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_svc_deinit(void *handle);

/**
 * face_svc_start - Start face service
 * @handle the handle of face service
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_svc_start(void *handle);

/**
 * face_svc_stop - Stop face service
 * @handle the handle of face service
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_svc_stop(void *handle);

/**
 * face_register_face_feats - Register face features to face service
 * @handle the handle of face service
 * @feats A porinter to face features database
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_register_face_feats(void *handle, const feats_t *feats);

/**
 * face_svc_set_config - Configure face service
 * @handle the handle of face service
 * @config A porinter to face service configuation
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_svc_set_config(void *handle, face_config_t *config);

/**
 * face_svc_set_config2 - Dynamic configure face recognition parameters to NN
 * @handle the handle of face service
 * @config A porinter to face service configuation
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_svc_set_config2(void *handle, face_config_t *config);

/**
 * face_svc_get_config - Get configure of face service
 * @handle the handle of face service
 * @config A porinter to face service configuation which will be set by service
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_svc_get_config(void *handle, face_config_t *config);
int face_get_max_track_faces(void *handle);
int face_set_max_track_faces(void *handle, int count);
int face_svc_reset_recg(void *handle);
int face_svc_set_mask_det_enable(void *handle, int enable);
int face_svc_get_mask_det_enable(void *handle);
int face_svc_set_mask_recg_enable(void *handle, int enable);
int face_svc_get_mask_recg_enable(void *handle);
int face_svc_set_mask_score_threshold(void *handle, float threshold);
float face_svc_get_mask_score_threshold(void *handle);
int face_svc_set_recg_repeat(void *handle, int enable);
int face_svc_get_recg_repeat(void *handle);
int face_svc_set_recg_repeat_interval(void *handle, int interval);
int face_svc_get_recg_repeat_interval(void *handle);
int face_get_dump_img_config(dump_img_config_t *config);
int face_set_dump_img_config(dump_img_config_t *config);

int face_get_fquality_enable(void *handle);
int face_set_fquality_enable(void *handle, int enable);
int face_get_headpose_enable(void *handle);
int face_set_headpose_enable(void *handle, int enable);
int face_get_faceattr_enable(void *handle);
int face_set_faceattr_enable(void *handle, int enable);
int face_get_use_fast_trak(void *handle);
int face_set_use_fast_trak(void *handle, int use);
int face_get_det_mode(void *handle);
int face_set_det_mode(void *handle, int mode);

int face_svc_set_exp_bias(int id, int bias);
int face_svc_set_temp_measure(int enable);
int face_svc_set_min_face_pixel(int min_face_pixel);
int face_svc_set_living_score_threshold(int threshold);
int face_svc_set_det_distance(int cm);

/**
 * face_register_det_cb - Regiter detection callback to face service
 * @handle the handle of face service
 * @cb face detection callback
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_register_det_cb(void *handle, face_det_cb cb);

/**
 * face_register_recg_cb - Regiter recognition callback to face service
 * @handle the handle of face service
 * @cb face recognition callback
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_register_recg_cb(void *handle, face_recg_cb cb);

/**
 * face_register_record_cb - Regiter record callback to face service
 * @handle the handle of face service
 * @cb face record callback
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_register_record_cb(void *handle, face_record_cb cb);

/**
 * face_register_nir_det_cb - Regiter nir_det callback to face service
 * @cb face nir_det callback
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_register_nir_det_cb(face_nir_det_cb cb);

/**
 * start face service
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_nir_det_start(void);

/**
 * stop face nir det service
 *
 * Return 0 if success, otherwise return non-zero.
 */
int face_nir_det_stop(void);

typedef struct _face_feat_opt_t {
	int face_num;		/* face feature array count */
	face_info_t info;	/* face feature array */
	float simulity;
	int quality;
	int rect[4];	/* face position [left, top, right, bottom] */
} face_feat_opt_t;

typedef struct _face_image_file_t {
	char image_path[FILENAME_MAX];
	int save_face;
	char save_face_path[FILENAME_MAX];
	int hflip;
} face_image_file_t;

typedef struct _face_image_data_t {
	char *image_ptr;
	int len;
	int save_face;
	char save_face_path[FILENAME_MAX];
	int save_frame;
	char save_frame_path[FILENAME_MAX];
	int hflip;
} face_image_data_t;

/**
 * face_get_feats_from_image - Get face feats from image
 * @handle the handle of face service
 * @cb
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_get_feats_from_image_file(void *handle,
	face_image_file_t *file, face_feat_opt_t *opt);

/**
 * face_get_feats_from_image - Get face feats from image
 * @handle the handle of face service
 * @cb
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_get_feats_from_image_data(void *handle,
	face_image_data_t *data, face_feat_opt_t *opt);

typedef struct _face_capture_limit_t {
	int head_pose_yaw;
	int head_pose_pitch;
	int head_pose_roll;
	int quality;
	int x0;
	int y0;
	int x1;
	int y1;
	int min;
	int max;
} face_capture_limit;

typedef struct _face_capture_param_t {
	int timeout_sec;
	face_capture_limit face;
	int face_cnt;
	face_frame_t *frame;
	int save_face;
	char save_face_path[FILENAME_MAX];
	int save_frame;
	char save_frame_path[FILENAME_MAX];
} face_capture_param_t;

/**
 * face_get_feats_from_image - Get face feats from image
 * @handle the handle of face service
 * @cb
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_get_feats_from_capture(void *handle,
	face_capture_param_t *capture_param, face_feat_opt_t *opt);

int face_stop_feats_from_capture(void *handle);

typedef int(*f_qr_scan_cb)(void *arg, char *data);

/**
 * face_svc_start_qrmode - start qr mode service, get qr from frame
 * @handle the handle of face service
 * @cb det qr callback
 * @arg this is callback arg
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_start_qrmode_set(void *handle, int mode, f_qr_scan_cb cb, void *arg);
int face_svc_start_qrmode(void *handle, f_qr_scan_cb cb, void *arg);
int face_svc_set_qrmode_timeout(void *handle, int ms);

/**
 * face_svc_stop_qrmode stop qr mode service
 * @handle the handle of face service
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_stop_qrmode(void *handle);

/**
 * face_svc_recg_pause pause recg service
 * @handle the handle of face service
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_recg_pause(void *handle);

/**
 * face_svc_recg_resume resume recg service
 * @handle the handle of face service
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_recg_resume(void *handle);

/**
 * face_svc_get_recg_face - Get recg face limit info
 * @handle the handle of face service
 * @ attr limit data
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_get_recg_face(void *handle, face_attr_t *attr);

/**
 * face_svc_set_recg_face - Get recg face limit info
 * @handle the handle of face service
 * @ attr limit data
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_set_recg_face(void *handle, face_attr_t *attr);

/**
 * face_svc_get_recg_face - Get recg face limit info
 * @handle the handle of face service
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_get_recg_rect(void *handle, int *left, int *top,
	int *right, int *bottom);

/**
 * face_svc_set_recg_face - Get recg face limit info
 * @handle the handle of face service
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_set_recg_rect(void *handle, int left, int top,
	int right, int bottom);

int face_svc_get_temp_measure_rect(void *handle, int *left, int *top,
	int *right, int *bottom);

int face_svc_set_temp_measure_rect(void *handle, int left, int top,
	int right, int bottom);

typedef struct _map_params {
	double x_map[5];
	double y_map[5];
} map_params_t;

/**
 * face_svc_set_map_params - Set rgb 2 nir map params
 * @map_params
 *
 * Return SUCCESS if success, otherwise return e_error_code.
 */
int face_svc_set_map_params(void *handle, map_params_t *map_params);

int face_svc_take_mod_pic(void *handle, int x, int y, int w, int h);

int face_svc_cal_mod_map(void *handle, map_params_t *map);

enum face_camera_id {
	FACE_CAMERA_RGB,
	FACE_CAMERA_NIR,
	FACE_CAMERA_RGB_MIRROR,
	FACE_CAMERA_MAX
};

typedef int (*p_face_img_cb)(face_frame_t *frame);

int face_svc_start_nir_preview(void *handle, int x, int y,
	int width, int height, int rot, int mode);

int face_svc_stop_nir_preview(void *handle);

int face_svc_start_preview(enum face_camera_id id, int x, int y,
	int width, int height, int rot, int mode);

int face_svc_stop_preview(enum face_camera_id id);

int face_take_picture_cb(enum face_camera_id camera_id,
	p_face_img_cb f_cb);

int face_take_picture_file(enum face_camera_id camera_id, char *file);

int face_svc_start_rec(enum face_camera_id camera_id, enc_para_t *enc_para,
	const char *filename);
int face_svc_stop_rec(enum face_camera_id camera_id);

int face_svc_get_ord_map_params(map_params_t *ord_map_params);

int face_svc_get_mod_map_params(map_params_t *ord_map_params, map_params_t *mod_map_params);

int face_svc_get_map_params(map_params_t *map_params);


/*---------------------   Faceid 1.x api -------------------------*/

#define FACE_SVC_OK   0
#define FACE_SVC_ERR -1

/*
 * pause face service
 *
 * handle: the handle of face service
 */
int face_svc_pause(void *handle);

/*
 * resume face service
 *
 * handle: the handle of face service
 */
int face_svc_resume(void *handle);

/*
 * get face service
 *
 * handle: the handle of face service
 */
int face_svc_pause_status(void *handle);


/*
 * get  face rgb handle for app using
 *
 * handle: the handle of face service
 */
void *face_rgb_hdl(void *handle);

/*
 * get  face nir handle for app using
 *
 * handle: the handle of face service
 */
void *face_nir_hdl(void *handle);

/*
 * get  face media handle for app using
 *
 * handle: the handle of face service
 */
void *face_media_hdl(void *handle);

int face_svc_start_rgb_preview(void *handle, int x, int y,
	int width, int height, int rot, int mode);

int face_svc_stop_rgb_preview(void *handle);

int face_svc_pause_nir_preview(void *handle);
int face_svc_resume_nir_preview(void *handle);

typedef struct _face_camera_cfg {
	int width;
	int height;
} face_camera_cfg_t;

int face_take_picture(void *handle, enum face_camera_id camera_id,
						p_face_img_cb f_cb);
int face_return_frame(void *handle, face_frame_t *f_frame);
int face_get_camera_config(void *handle, enum face_camera_id camera_id,
						face_camera_cfg_t *cfg);

/*
 * face detect interface, non-blocking
 *
 * frame: frame to be recognise
 */
int face_det(void *handle, face_frame_t *image);

/*
 * face recognise interface, non-blocking
 *
 * frame: frame to be recognise
 */
int face_recg(void *handle, face_frame_t *image);

/*
 * face recognise interface, blocking
 *
 * frame: frame to be recognise
 * cnt: the number of faces to be recognise
 * info: result
 *
 * On success, return the number of faces have been recognise.
 * If an error occurs, the return value is -1.
 */
int face_recg_block(void *handle, face_frame_t *frame, int cnt,
		face_info_t *info);

/*
 * face recognise interface, blocking
 *
 * frame: frame to be recognise
 * cnt: the number of faces to be recognise
 * info: result
 * compare:
 *   0: will not compare with all registered faces
 *   1: will compare with all registered faces
 */
int face_recg_block2(void *handle, face_frame_t *frame, int cnt,
		face_info_t *info, int compare);

/*
 * face 1:1 compare interface
 */
float face_compare(void *handle, face_info_t *info1, face_info_t *info2);
float face_compare2(signed char *info1, signed char *info2);
float face_compare2_with_mask(signed char *info1, signed char *info2);

/*get face svc config path interface, default is /etc/face_svc.conf*/
char *face_svc_get_conf_file_path();

/*set face svc config path to differ app*/
void face_svc_set_conf_file_path(const char *path);

/*---------------------   Faceid 1.x api -------------------------*/

/*---------------------   debug api   -----------------------------*/
int face_svc_rgb_color_living_check(void *handle, char *image_path);
int face_svc_nir_living_check(void *handle, int type, char *image_path);

#ifdef __cplusplus
}
#endif

#endif /* _FACE_SVC_H_ */
