/*
 * lb_recorder.h - Standard functionality for lombo recorder.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
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

#ifndef __RECORDER_PANO_H_
#define __RECORDER_PANO_H_

typedef struct img {
	char path[64];
	char format[8]; /* img format, support nv12,nv21 */
	int width;
	int height;
} img_t;

/* panoramic calibration para */
typedef struct cali_param {
	int  box_rows;	/* the number of rows in the back and white grid */
	int  box_cols;	/* the number of columns in the back and white grid */
	int  box_width;	/* back and white grid width (unit: cm) */
	int  box_height;	/* back and white grid height (unit: cm) */
	int  dist_2_rear;	/* the bottom row of the grid to the rear of the car */
	int  car_width;	/* car width */
	int  car_length;	/* car length */

	int  front_dist;    /* car front distance (unit: cm) */
	int  rear_dist;	/* car rear distance (unit: cm)  */
	int  align;	    /* rear direction. -1 or 1*/

	int use_ext_cali_img;  /* use extern calibration image, only test */
	img_t ext_cali_img; /* extern calibration image */
} cali_param_t;

typedef struct cali_contex {
	int cutline_dnthr;
	int cutline_upthr;
	int cutline;
	win_rect_t car_rect;
} cali_contex_t;

/* panoramic calibration out data */
typedef struct cali_out_data {
	int data_size;
	void *data; /* openmax malloc, app need to free memory */
} cali_out_data_t;

/* panoramic init para */
typedef struct pano_param {
	int in_gps;	  /* use gps speed */
	int in_obd;	  /* use obd para */

	/* car para */
	int car_para_en;	/* whether to enabel use paras */
	int car_width;      /* car width (unit: cm) */

	img_t carboard_img;
	char *data_format;

	int use_ext_cutline;
	int culine;
} pano_param_t;

#ifdef ENABLE_PANO
int video_pano_cali_process(video_recorder_t *video_rec, cali_contex_t *ctx);
int video_pano_start(video_recorder_t *video_rec);
int video_pano_stop(video_recorder_t *video_rec);
int video_pano_get_cali_data(video_recorder_t *video_rec, cali_out_data_t *out);
int video_pano_reset(video_recorder_t *video_rec);
int video_pano_set_cali_data(video_recorder_t *video_rec, cali_out_data_t *out);
int video_pano_set_cali_para(video_recorder_t *video_rec, cali_param_t *para);
int video_pano_set_disp_mode(video_recorder_t *video_rec,	win_para_t *win);
int video_pano_set_init_para(video_recorder_t *video_rec,	pano_param_t *para);
int video_pano_set_preview_size(video_recorder_t *video_rec, vsize_t *size);
int video_pano_set_layer_level(video_recorder_t *video_rec,
			video_layer_level_t level);
int video_pano_creat(video_recorder_t *video_rec);
void video_pano_release(video_recorder_t *video_rec);
#else
#define video_pano_cali_process(video_rec, ctx) -1
#define video_pano_start(video_rec) -1
#define video_pano_stop(video_rec) -1
#define video_pano_get_cali_data(video_rec, out) -1
#define video_pano_reset(video_rec) -1
#define video_pano_set_cali_data(video_rec, out) -1
#define video_pano_set_cali_para(video_rec, para) -1
#define video_pano_set_disp_mode(video_rec, win) -1
#define video_pano_set_init_para(video_rec, para) -1
#define video_pano_set_preview_size(video_rec, size) -1
#define video_pano_set_layer_level(video_rec, level) -1
#define video_pano_creat(video_rec) -1
#define video_pano_release(video_rec)
#endif

#endif

