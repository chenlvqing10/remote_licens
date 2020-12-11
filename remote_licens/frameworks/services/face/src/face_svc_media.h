/*
 * face_svc_media.c
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

#ifndef _MEDIA_H_
#define _MEDIA_H_

#include "face_svc.h"

/*
* return as fast as possible.
* return 0 if deal with frame successfully, otherwise return -1;
*/
typedef int (*p_face_frame_cb)(void *p, face_frame_t *frame);

struct cb_priv {
	p_face_frame_cb cb;
	void *p;
};

typedef struct _vs_record_s {
	int width;
	int height;
	void *astream;
	void *vstream;
	void *recorder;
} vs_record_t;

struct face_media {
	void *rgb_hdl;
	void *rgb_prev_hdl;
	struct cb_priv *rgb_cb_priv;

	void *rgb_mirror_hdl;
	void *rgb_prev_mirror_hdl;
	struct cb_priv *rgb_mirror_cb_priv;

	void *nir_hdl;
	void *nir_prev_hdl;
	struct cb_priv *nir_cb_priv;

	vs_record_t *vs_rec[FACE_CAMERA_MAX];
};

int led_on(void);
int led_off(void);
void *face_media_create(void);
int face_media_start(void *hdl);
int face_media_start_rgb_preview(void *hdl, int x, int y,
	int width, int height, int rot, int mode);
int face_media_stop_rgb_preview(void *hdl);
int face_media_start_nir_preview(void *hdl, int x, int y,
	int width, int height, int rot, int mode);
int face_media_stop_nir_preview(void *hdl);
int face_media_stop(void *hdl);
int face_media_destroy(void *hdl);
int face_media_set_cb(void *hdl, enum face_camera_id camera_id,
	void *p, p_face_frame_cb f_cb);
int face_media_return_frame(face_frame_t *f_frame);
int face_media_take_picture(void *hdl, enum face_camera_id camera_id,
	p_face_img_cb f_cb);
int face_media_take_picture_file(void *hdl, enum face_camera_id camera_id,
	char *file);
int face_media_get_camera_config(void *hdl, enum face_camera_id camera_id,
	face_camera_cfg_t *cfg);
int face_media_rec_init(void *hdl);
int face_media_rec_destroy(void *hdl);
int face_media_rec_start(void *hdl, enum face_camera_id camera_id,
	enc_para_t *enc_para, const char *filename);
int face_media_rec_stop(void *hdl, enum face_camera_id camera_id);

#endif /* _MEDIA_H_ */
