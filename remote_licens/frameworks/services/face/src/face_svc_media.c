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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#define LOG_TAG "face_svc"
#include <log/log.h>

#include <iniparser.h>

#include "lombo_malloc.h"
#include "audio_stream.h"
#include "video_stream.h"
#include "video_recorder.h"
#include "face_svc.h"
#include "face_svc_media.h"

#include "face_svc_conf.h"

#define GPIO_EXPORT	"/sys/class/gpio/export"
#define GPIO_UNEXPORT	"/sys/class/gpio/unexport"
#define LED_GPIO	60
#define LED_DIRECTION	"/sys/class/gpio/gpio60/direction"
#define LED_VALUE	"/sys/class/gpio/gpio60/value"

static long long get_systime_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

static int led_ctrl(int on)
{
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", LED_GPIO);
	fclose(p);

	/* set gpio direction */
	p = fopen(LED_DIRECTION, "w");
	if (!p) {
		ALOGE("open %s failed", LED_DIRECTION);
		return -1;
	}
	fprintf(p, "%s", "out");
	fclose(p);

	/* set gpio value */
	p = fopen(LED_VALUE, "w");
	if (!p) {
		ALOGE("open %s failed", LED_VALUE);
		return -1;
	}
	if (on)
		fprintf(p, "%d", 1);
	else
		fprintf(p, "%d", 0);
	fclose(p);

	/* unexport gpio*/
	p = fopen(GPIO_UNEXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_UNEXPORT);
		return -1;
	}
	fprintf(p, "%d", LED_GPIO);
	fclose(p);

	return 0;
}

int led_on(void)
{
	return led_ctrl(1);
}

int led_off(void)
{
	return led_ctrl(0);
}

static void *face_stream_start(char *source, int width, int height, int rot)
{
	void *hdl;
	int ret;
	vdisp_rotate_mode_e rotation;

	switch (rot) {
	case 90:
		rotation = VDISP_ROTATE_90;
		break;
	case 270:
		rotation = VDISP_ROTATE_270;
		break;
	default:
		rotation = VDISP_ROTATE_NONE;
		break;
	}

	hdl = video_stream_create();
	if (!hdl) {
		ALOGE("stream create failed\n");
		return NULL;
	}

	ret = video_stream_set_camera_source(hdl, source);
	if (ret) {
		ALOGE("video_stream_set_camera_source failed\n");
		return NULL;
	}

	ret = video_stream_set_camera_size(hdl, width, height);
	if (ret) {
		ALOGE("video_stream_set_camera_size failed\n");
		return NULL;
	}

	ret = video_stream_set_rotate(hdl, rotation);
	if (ret) {
		ALOGE("video_stream_set_rotate failed\n");
		return NULL;
	}

	ret = video_stream_start_camera(hdl);
	if (ret) {
		ALOGE("video_stream_start_camera failed\n");
		return NULL;
	}

	return hdl;
}

static int face_media_frame_cb_handle(void *p, frame_t *in_frame)
{
	face_frame_t *f_frame = NULL;
	frame_t *frame = NULL;
	struct cb_priv *priv;
	int ret;

	priv = (struct cb_priv *)p;
	if (priv->cb) {
		f_frame = malloc(sizeof(face_frame_t));
		if (!f_frame)
			goto exit;
		frame = malloc(sizeof(frame_t));
		if (!frame)
			goto exit;
		memcpy(frame, in_frame, sizeof(frame_t));
		memset(f_frame, 0, sizeof(face_frame_t));
		f_frame->width = frame->info.video.width;
		f_frame->height = frame->info.video.height;
		f_frame->size[0] = frame->info.video.size[0];
		f_frame->size[1] = frame->info.video.size[1];
		f_frame->size[2] = frame->info.video.size[2];
		f_frame->viraddr[0] = frame->info.video.addr[0];
		f_frame->viraddr[1] = frame->info.video.addr[1];
		f_frame->viraddr[2] = frame->info.video.addr[2];
		f_frame->phyaddr[0] = lombo_vir_to_phy(f_frame->viraddr[0]);
		f_frame->phyaddr[1] = lombo_vir_to_phy(f_frame->viraddr[1]);
		f_frame->phyaddr[2] = 0; /* nv12 */
		/* Fix device sync time, frame timestamp is err */
		/* f_frame->time_stamp = frame->info.video.time_stamp; */
		f_frame->time_stamp = get_systime_us();
		f_frame->priv = frame;
		ret = priv->cb(priv->p, f_frame);
		if (ret == 0)
			return 0;
	}

exit:
	if (f_frame)
		free(f_frame);
	if (frame)
		free(frame);
	/* return frame immediately if cb == NULL or f_cb return failed */
	ret = video_stream_return_frame(frame);
	if (ret) {
		ALOGE("return frame failed\n");
		return -1;
	}

	return 0;
}

static int face_media_img_cb_handle(void *p, frame_t *in_frame)
{
	face_frame_t *f_frame = NULL;
	frame_t *frame = NULL;
	p_face_img_cb cb;
	int ret;

	cb = (p_face_img_cb)p;
	if (cb) {
		f_frame = malloc(sizeof(face_frame_t));
		if (!f_frame)
			goto exit;
		frame = malloc(sizeof(frame_t));
		if (!frame)
			goto exit;
		memcpy(frame, in_frame, sizeof(frame_t));
		f_frame->width = frame->info.video.width;
		f_frame->height = frame->info.video.height;
		f_frame->size[0] = frame->info.video.size[0];
		f_frame->size[1] = frame->info.video.size[1];
		f_frame->size[2] = frame->info.video.size[2];
		f_frame->viraddr[0] = frame->info.video.addr[0];
		f_frame->viraddr[1] = frame->info.video.addr[1];
		f_frame->viraddr[2] = frame->info.video.addr[2];
		f_frame->phyaddr[0] = lombo_vir_to_phy(f_frame->viraddr[0]);
		f_frame->phyaddr[1] = lombo_vir_to_phy(f_frame->viraddr[1]);
		f_frame->phyaddr[2] = 0; /* nv12 */
		/* Fix device sync time, frame timestamp is err */
		/* f_frame->time_stamp = frame->info.video.time_stamp; */
		f_frame->time_stamp = get_systime_us();
		f_frame->priv = frame;
		ret = cb(f_frame);
		if (ret == 0)
			return 0;
	}

exit:
	/* return frame immediately if cb == NULL or f_cb return failed */
	ret = video_stream_return_frame(frame);
	if (ret) {
		ALOGE("return frame failed\n");
		return -1;
	}

	if (f_frame)
		free(f_frame);
	if (frame)
		free(frame);

	return 0;
}

int face_media_return_frame(face_frame_t *f_frame)
{
	int ret;

	if (!f_frame || !f_frame->priv) {
		ALOGE("%s frame is NULL", __func__);
		return -1;
	}

	ret = video_stream_return_frame((frame_t *)f_frame->priv);
	if (ret) {
		ALOGE("return frame failed\n");
		return -1;
	}

	free(f_frame->priv);
	free(f_frame);

	return ret;
}

static struct cb_priv *_face_media_set_cb(void *hdl, void *p,
							p_face_frame_cb f_cb)
{
	app_frame_cb_t cb;
	struct cb_priv *priv;
	int ret;

	if (!hdl || !f_cb)
		return NULL;

	priv = malloc(sizeof(struct cb_priv));
	if (!priv)
		return NULL;
	priv->cb = f_cb;
	priv->p = p;

	cb.type = VS_VIDEO_ROT_FRAME;
	cb.app_data = priv;
	cb.buf_handle = face_media_frame_cb_handle;
	ret = video_stream_set_frame_cb(hdl, &cb);
	if (ret) {
		ALOGE("set cb failed\n");
		return NULL;
	}

	return priv;
}

static int _face_media_take_picture(void *hdl, p_face_img_cb f_cb)
{
	app_frame_cb_t cb;
	int ret;

	if (!hdl || !f_cb)
		return -1;

	cb.type = VS_IMAGE_ROT_FRAME;
	cb.app_data = f_cb;
	cb.buf_handle = face_media_img_cb_handle;
	ret = video_stream_take_picture(hdl, &cb);
	if (ret) {
		ALOGE("take pic failed\n");
		return -1;
	}

	return 0;
}

int face_media_set_cb(void *hdl, enum face_camera_id camera_id,
						void *p, p_face_frame_cb f_cb)
{
	struct face_media *media = (struct face_media *)hdl;
	struct cb_priv *priv;
	void *handle;

	if (!media)
		return -1;

	if (camera_id == FACE_CAMERA_RGB && face_svc_conf->media.rgb_enable)
		handle = media->rgb_hdl;
	else if (camera_id == FACE_CAMERA_RGB_MIRROR && face_svc_conf->media.rgb_mirror_enable)
		handle = media->rgb_mirror_hdl;
	else if (camera_id == FACE_CAMERA_NIR && face_svc_conf->media.nir_enable)
		handle = media->nir_hdl;
	else {
		/* FIXME */
		ALOGI("camera id: %d", camera_id);
		return 0;
	}

	priv = _face_media_set_cb(handle, p, f_cb);
	if (priv == NULL)
		return -1;

	return 0;
}

int face_media_take_picture(void *hdl, enum face_camera_id camera_id,
						p_face_img_cb f_cb)
{
	struct face_media *media = (struct face_media *)hdl;
	void *handle;

	if (!media)
		return -1;

	if (camera_id == FACE_CAMERA_RGB)
		handle = media->rgb_hdl;
	else if (camera_id == FACE_CAMERA_RGB_MIRROR)
		handle = media->rgb_mirror_hdl;
	else if (camera_id == FACE_CAMERA_NIR)
		handle = media->nir_hdl;
	else {
		ALOGE("unknow camera id:%d", camera_id);
		return -1;
	}

	return _face_media_take_picture(handle, f_cb);
}

int face_media_take_picture_file(void *hdl, enum face_camera_id camera_id,
	char *file)
{
	struct face_media *media = (struct face_media *)hdl;
	vs_record_t *handle;
	enc_para_t enc_param;
	vdisp_rotate_mode_e rotation;
	int ret = 0;

	if (!media)
		return -1;

	if (camera_id == FACE_CAMERA_RGB) {
		if (media->vs_rec[FACE_CAMERA_RGB]) {
			handle = media->vs_rec[FACE_CAMERA_RGB];
		} else {
			ALOGE("camera id:%d rec is null", camera_id);
			return -1;
		}
	} else if (camera_id == FACE_CAMERA_RGB_MIRROR) {
		if (media->vs_rec[FACE_CAMERA_RGB_MIRROR]) {
			handle = media->vs_rec[FACE_CAMERA_RGB_MIRROR];
		} else {
			ALOGE("camera id:%d rec is null", camera_id);
			return -1;
		}
	} else if (camera_id == FACE_CAMERA_NIR) {
		if (media->vs_rec[FACE_CAMERA_NIR]) {
			handle = media->vs_rec[FACE_CAMERA_NIR];
		} else {
			ALOGE("camera id:%d rec is null", camera_id);
			return -1;
		}
	} else {
		ALOGE("unknow camera id:%d", camera_id);
		return -1;
	}

	memset(&enc_param, 0, sizeof(enc_para_t));
	ret = video_rec_get_para(handle->recorder, &enc_param);
	if (ret < 0) {
		ALOGE("LB_REC_GET_ENC_PARA error\n");
		return -1;
	}

	if (camera_id == FACE_CAMERA_RGB) {
		if (media->vs_rec[FACE_CAMERA_RGB]) {
			switch (face_svc_conf->media.rgb_rotation) {
				case 90:
					rotation = VDISP_ROTATE_90;
					break;
				case 270:
					rotation = VDISP_ROTATE_270;
					break;
				default:
					rotation = VDISP_ROTATE_NONE;
					break;
			}
			enc_param.rotate = rotation;
		} else {
			ALOGE("camera id:%d rec is null", camera_id);
			return -1;
		}
	} else if (camera_id == FACE_CAMERA_RGB_MIRROR) {
		if (media->vs_rec[FACE_CAMERA_RGB_MIRROR]) {
			switch (face_svc_conf->media.rgb_mirror_rotation) {
				case 90:
					rotation = VDISP_ROTATE_90;
					break;
				case 270:
					rotation = VDISP_ROTATE_270;
					break;
				default:
					rotation = VDISP_ROTATE_NONE;
					break;
			}
			enc_param.rotate = rotation;
		} else {
			ALOGE("camera id:%d rec is null", camera_id);
			return -1;
		}
	} else if (camera_id == FACE_CAMERA_NIR)
		if (media->vs_rec[FACE_CAMERA_NIR]){
			switch (face_svc_conf->media.nir_rotation) {
				case 90:
					rotation = VDISP_ROTATE_90;
					break;
				case 270:
					rotation = VDISP_ROTATE_270;
					break;
				default:
					rotation = VDISP_ROTATE_NONE;
					break;
			}
			enc_param.rotate = rotation;
		} else {
			ALOGE("camera id:%d rec is null", camera_id);
			return -1;
		}
	else {
		ALOGE("unknow camera id:%d", camera_id);
		return -1;
	}

	ret = video_rec_set_para(handle->recorder, &enc_param);
	if (ret < 0) {
		ALOGE("LB_REC_SET_ENC_PARA error\n");
		return -1;
	}

	return video_rec_take_picture(handle->recorder, file);
}

int face_media_get_camera_config(void *hdl, enum face_camera_id camera_id,
						face_camera_cfg_t *cfg)
{
	struct face_media *media = (struct face_media *)hdl;

	if (!media || cfg == NULL)
		return -1;

	if (camera_id == FACE_CAMERA_RGB) {
		cfg->width = face_svc_conf->media.rgb_width;
		cfg->height = face_svc_conf->media.rgb_height;
	} else if (camera_id == FACE_CAMERA_NIR) {
		cfg->width = face_svc_conf->media.nir_width;
		cfg->height = face_svc_conf->media.nir_height;
	} else {
		ALOGE("unknow camera id:%d", camera_id);
		return -1;
	}

	return 0;
}

void *face_media_create(void)
{
	struct face_media *media;
	int ret;

	/* initialize media framework */
	ret = media_init();
	if (ret) {
		ALOGE("media init failed\n");
		return NULL;
	}

	media = malloc(sizeof(struct face_media));
	if (!media) {
		ALOGE("face media create failed\n");
		return NULL;
	}
	memset(media, 0, sizeof(struct face_media));

	return media;
}

static int face_stream_start_preview(void *stream_hdl, int x, int y,
					int width, int height, int rot, int mode)
{
	win_para_t disp_para;
	vdisp_rotate_mode_e rotation;
	int ret;

	if (!stream_hdl) {
		ALOGE("stream hdl == NULL");
		return -EINVAL;
	}

	switch (rot) {
	case 90:
		rotation = VDISP_ROTATE_90;
		break;
	case 270:
		rotation = VDISP_ROTATE_270;
		break;
	default:
		rotation = VDISP_ROTATE_NONE;
		break;
	}

	memset(&disp_para, 0, sizeof(disp_para));
	disp_para.rect.x = x;
	disp_para.rect.y = y;
	disp_para.rect.width = width;
	disp_para.rect.height = height;
	disp_para.mode = mode;
	ret = video_stream_set_disp_para(stream_hdl, &disp_para, rotation);
	if (ret) {
		ALOGE("video_stream_set_preview_window failed\n");
		return ret;
	}

	ret = video_stream_prepare_preview(stream_hdl);
	if (ret) {
		ALOGE("video_stream_prepare_preview failed\n");
		return ret;
	}

	ret = video_stream_start_preview(stream_hdl);
	if (ret) {
		ALOGE("video_stream_start_preview failed\n");
		return ret;
	}

	return 0;
}

int face_media_start_rgb_preview(void *hdl, int x, int y,
			int width, int height, int rot, int mode)
{
	struct face_media *media = (struct face_media *)hdl;
	int ret;

	if (!media || !media->rgb_prev_hdl) {
		ALOGE("hdl == NULL");
		return -EINVAL;
	}

	ret = face_stream_start_preview(media->rgb_prev_hdl, x, y, width, height, rot, mode);
	if (ret) {
		ALOGE("face start preview failed");
		return ret;
	}

	return 0;
}
int face_media_stop_rgb_preview(void *hdl)
{
	struct face_media *media = (struct face_media *)hdl;
	int ret;

	if (!media || !media->rgb_prev_hdl) {
		ALOGE("hdl == NULL");
		return -EINVAL;
	}

	ret = video_stream_stop_preview(media->rgb_prev_hdl);
	if (ret) {
		ALOGE("video_stream_start_preview failed\n");
		return ret;
	}

	return 0;
}

int face_media_start_nir_preview(void *hdl, int x, int y,
	int width, int height, int rot, int mode)
{
	struct face_media *media = (struct face_media *)hdl;
	int ret;

	if (!media || !media->nir_prev_hdl) {
		ALOGE("hdl == NULL");
		return -EINVAL;
	}

	ret = face_stream_start_preview(media->nir_prev_hdl, x, y, width, height, rot, mode);
	if (ret) {
		ALOGE("face start preview failed");
		return ret;
	}

	return 0;
}

int face_media_stop_nir_preview(void *hdl)
{
	struct face_media *media = (struct face_media *)hdl;
	int ret;

	if (!media || !media->nir_hdl) {
		ALOGE("hdl == NULL");
		return -EINVAL;
	}

	ret = video_stream_stop_preview(media->nir_prev_hdl);
	if (ret) {
		ALOGE("video_stream_start_preview failed\n");
		return ret;
	}

	return 0;
}



int face_media_start(void *hdl)
{
	struct face_media *media = (struct face_media *)hdl;
	int ret;

	if (media == NULL)
		return -1;

	if (face_svc_conf->media.rgb_enable) {
		media->rgb_hdl = face_stream_start(face_svc_conf->media.rgb_device,
			face_svc_conf->media.rgb_width, face_svc_conf->media.rgb_height,
			face_svc_conf->media.rgb_rotation);
		if (!media->rgb_hdl) {
			ALOGE("face rgb stream start failed\n");
			return -1;
		}

		if (face_svc_conf->media.rgb_preview) {
			media->rgb_prev_hdl = face_stream_start(face_svc_conf->media.rgb_device,
				face_svc_conf->media.rgb_preview_width, face_svc_conf->media.rgb_preview_height,
				face_svc_conf->media.rgb_preview_rotation);
			if (!media->rgb_prev_hdl) {
				ALOGE("face rgb preview stream start failed\n");
					return -1;
			}
			ret = face_stream_start_preview(media->rgb_prev_hdl,
				face_svc_conf->media.rgb_preview_x, face_svc_conf->media.rgb_preview_y,
				face_svc_conf->media.rgb_preview_width, face_svc_conf->media.rgb_preview_height,
				face_svc_conf->media.rgb_preview_rotation,
				VDISP_WINDOW_FULL_SCREEN_SCREEN_RATIO);
			if (ret) {
				ALOGE("face start rgb preview failed");
				return -1;
			}
		}
	}

	if (face_svc_conf->media.rgb_mirror_enable) {
		media->rgb_mirror_hdl = face_stream_start(face_svc_conf->media.rgb_mirror_device,
			face_svc_conf->media.rgb_mirror_width, face_svc_conf->media.rgb_mirror_height,
			face_svc_conf->media.rgb_mirror_rotation);
		if (!media->rgb_mirror_hdl) {
			ALOGE("face rgb stream start failed\n");
			return -1;
		}
		if (face_svc_conf->media.rgb_mirror_preview) {
			media->rgb_prev_mirror_hdl = face_stream_start(face_svc_conf->media.rgb_mirror_device,
				face_svc_conf->media.rgb_mirror_preview_width, face_svc_conf->media.rgb_mirror_preview_height,
				face_svc_conf->media.rgb_mirror_preview_rotation);
			if (!media->rgb_prev_mirror_hdl) {
				ALOGE("face rgb preview stream start failed\n");
					return -1;
			}
			ret = face_stream_start_preview(media->rgb_prev_mirror_hdl,
				face_svc_conf->media.rgb_mirror_preview_x, face_svc_conf->media.rgb_mirror_preview_y,
				face_svc_conf->media.rgb_mirror_preview_width, face_svc_conf->media.rgb_mirror_preview_height,
				face_svc_conf->media.rgb_mirror_preview_rotation,
				VDISP_WINDOW_FULL_SCREEN_SCREEN_RATIO);
			if (ret) {
				ALOGE("face start rgb preview failed");
				return -1;
			}
		}
	}

	if (face_svc_conf->media.nir_enable) {
		media->nir_hdl = face_stream_start(face_svc_conf->media.nir_device,
			face_svc_conf->media.nir_width, face_svc_conf->media.nir_height,
			face_svc_conf->media.nir_rotation);
		if (!media->nir_hdl) {
			ALOGE("face nir stream start failed\n");
			return -1;
		}

		if (face_svc_conf->media.nir_preview) {
			media->nir_prev_hdl = face_stream_start(face_svc_conf->media.nir_device,
				face_svc_conf->media.nir_preview_width, face_svc_conf->media.nir_preview_height,
				face_svc_conf->media.nir_preview_rotation);
			if (!media->nir_prev_hdl) {
				ALOGE("face rgb preview stream start failed\n");
					return -1;
			}
			ret = face_stream_start_preview(media->nir_prev_hdl,
				face_svc_conf->media.nir_preview_x, face_svc_conf->media.nir_preview_y,
				face_svc_conf->media.nir_preview_width, face_svc_conf->media.nir_preview_height,
				face_svc_conf->media.nir_preview_rotation,
				VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO);
			if (ret) {
				ALOGE("face start nir preview failed");
				return -1;
			}
		}
	}

	return 0;
}

int face_media_rec_init(void *hdl)
{
	int ret;
	vsrc_param_t vsrc_param;

	struct face_media *media = (struct face_media *)hdl;
	if (media == NULL)
		return -1;

	if (face_svc_conf->media.rgb_enable) {
		media->vs_rec[FACE_CAMERA_RGB] = calloc(1, sizeof(vs_record_t));
		if (!media->vs_rec[FACE_CAMERA_RGB]) {
			ALOGE("calloc return NULL\n");
			return -1;
		}
		media->vs_rec[FACE_CAMERA_RGB]->vstream = media->rgb_hdl;
		media->vs_rec[FACE_CAMERA_RGB]->astream = audio_stream_creat();
		media->vs_rec[FACE_CAMERA_RGB]->recorder = video_rec_creat();
		video_rec_set_video_stream(media->vs_rec[FACE_CAMERA_RGB]->recorder
			, media->vs_rec[FACE_CAMERA_RGB]->vstream);
		video_rec_set_audio_stream(media->vs_rec[FACE_CAMERA_RGB]->recorder
			, media->vs_rec[FACE_CAMERA_RGB]->astream);
		ret = video_stream_get_src_para(media->vs_rec[FACE_CAMERA_RGB]->vstream,
			&vsrc_param);
		if (ret) {
			ALOGE("vs get src para failed");
			return ret;
		}
		media->vs_rec[FACE_CAMERA_RGB]->width = vsrc_param.width;
		media->vs_rec[FACE_CAMERA_RGB]->height = vsrc_param.height;
	}

	if (face_svc_conf->media.rgb_mirror_enable) {
		media->vs_rec[FACE_CAMERA_RGB_MIRROR] = calloc(1, sizeof(vs_record_t));
		if (!media->vs_rec[FACE_CAMERA_RGB_MIRROR]) {
			ALOGE("calloc return NULL\n");
			return -1;
		}
		media->vs_rec[FACE_CAMERA_RGB_MIRROR]->vstream = media->rgb_hdl;
		media->vs_rec[FACE_CAMERA_RGB_MIRROR]->astream = audio_stream_creat();
		media->vs_rec[FACE_CAMERA_RGB_MIRROR]->recorder = video_rec_creat();
		video_rec_set_video_stream(media->vs_rec[FACE_CAMERA_RGB_MIRROR]->recorder
			, media->vs_rec[FACE_CAMERA_RGB_MIRROR]->vstream);
		video_rec_set_audio_stream(media->vs_rec[FACE_CAMERA_RGB_MIRROR]->recorder
			, media->vs_rec[FACE_CAMERA_RGB_MIRROR]->astream);
		ret = video_stream_get_src_para(media->vs_rec[FACE_CAMERA_RGB_MIRROR]->vstream,
			&vsrc_param);
		if (ret) {
			ALOGE("vs get src para failed");
			return ret;
		}
		media->vs_rec[FACE_CAMERA_RGB_MIRROR]->width = vsrc_param.width;
		media->vs_rec[FACE_CAMERA_RGB_MIRROR]->height = vsrc_param.height;
	}

	if (face_svc_conf->media.nir_enable) {
		media->vs_rec[FACE_CAMERA_NIR] = calloc(1, sizeof(vs_record_t));
		if (!media->vs_rec[FACE_CAMERA_NIR]) {
			ALOGE("calloc return NULL\n");
			return -1;
		}
		media->vs_rec[FACE_CAMERA_NIR]->vstream = media->nir_hdl;
		media->vs_rec[FACE_CAMERA_NIR]->astream = audio_stream_creat();
		media->vs_rec[FACE_CAMERA_NIR]->recorder = video_rec_creat();
		video_rec_set_video_stream(media->vs_rec[FACE_CAMERA_NIR]->recorder
			, media->vs_rec[FACE_CAMERA_NIR]->vstream);
		video_rec_set_audio_stream(media->vs_rec[FACE_CAMERA_NIR]->recorder
			, media->vs_rec[FACE_CAMERA_NIR]->astream);
		ret = video_stream_get_src_para(media->vs_rec[FACE_CAMERA_NIR]->vstream,
			&vsrc_param);
		if (ret) {
			ALOGE("vs get src para failed");
			return ret;
		}
		media->vs_rec[FACE_CAMERA_NIR]->width = vsrc_param.width;
		media->vs_rec[FACE_CAMERA_NIR]->height = vsrc_param.height;
	}
	return 0;
}

int face_media_rec_destroy(void *hdl)
{
	struct face_media *media = (struct face_media *)hdl;
	if (media == NULL)
		return -1;

	if (face_svc_conf->media.rgb_enable) {
		if (media->vs_rec[FACE_CAMERA_RGB]) {
			if (media->vs_rec[FACE_CAMERA_RGB]->recorder)
				video_rec_release(media->vs_rec[FACE_CAMERA_RGB]->recorder);
			if (media->vs_rec[FACE_CAMERA_RGB]->astream != NULL)
				audio_stream_release(media->vs_rec[FACE_CAMERA_RGB]->astream);
			if (media->vs_rec[FACE_CAMERA_RGB]) {
				free(media->vs_rec[FACE_CAMERA_RGB]);
				media->vs_rec[FACE_CAMERA_RGB] = NULL;
			}
		}
	}

	if (face_svc_conf->media.rgb_mirror_enable) {
		if (media->vs_rec[FACE_CAMERA_RGB_MIRROR]) {
			if (media->vs_rec[FACE_CAMERA_RGB_MIRROR]->recorder)
				video_rec_release(media->vs_rec[FACE_CAMERA_RGB_MIRROR]->recorder);
			if (media->vs_rec[FACE_CAMERA_RGB_MIRROR]->astream != NULL)
				audio_stream_release(media->vs_rec[FACE_CAMERA_RGB_MIRROR]->astream);
			if (media->vs_rec[FACE_CAMERA_RGB_MIRROR]) {
				free(media->vs_rec[FACE_CAMERA_RGB_MIRROR]);
				media->vs_rec[FACE_CAMERA_RGB_MIRROR] = NULL;
			}
		}
	}

	if (face_svc_conf->media.nir_enable) {
		if (media->vs_rec[FACE_CAMERA_NIR]) {
			if (media->vs_rec[FACE_CAMERA_NIR]->recorder)
				video_rec_release(media->vs_rec[FACE_CAMERA_NIR]->recorder);
			if (media->vs_rec[FACE_CAMERA_NIR]->astream != NULL)
				audio_stream_release(media->vs_rec[FACE_CAMERA_NIR]->astream);
			if (media->vs_rec[FACE_CAMERA_NIR]) {
				free(media->vs_rec[FACE_CAMERA_NIR]);
				media->vs_rec[FACE_CAMERA_NIR] = NULL;
			}
		}
	}
	return 0;
}

int face_media_rec_start(void *hdl, enum face_camera_id camera_id,
	enc_para_t *enc_para, const char *filename)
{
	int ret = 0;
	vs_record_t *vs_rec;
	enc_para_t enc_param;
	vdisp_rotate_mode_e rotation;
	int rec_width;
	int rec_height;

	struct face_media *media = (struct face_media *)hdl;
	if (media == NULL)
		return -1;
	vs_rec = media->vs_rec[camera_id];
	if (vs_rec == NULL)
		return -1;

	ret = video_rec_set_output_file(vs_rec->recorder, filename);
	if (ret < 0) {
		ALOGE("LB_REC_SET_OUTPUT_FILE error\n");
		return -1;
	}

	memset(&enc_param, 0, sizeof(enc_para_t));
	ret = video_rec_get_para(vs_rec->recorder, &enc_param);
	if (ret < 0) {
		ALOGE("LB_REC_GET_ENC_PARA error\n");
		return -1;
	}

	if (camera_id == FACE_CAMERA_RGB) {
		switch (face_svc_conf->media.rgb_rotation) {
			case 90:
				rotation = VDISP_ROTATE_90;
				rec_width = face_svc_conf->media.rgb_height;
				rec_height = face_svc_conf->media.rgb_width;
				break;
			case 270:
				rotation = VDISP_ROTATE_270;
				rec_width = face_svc_conf->media.rgb_height;
				rec_height = face_svc_conf->media.rgb_width;
				break;
			default:
				rotation = VDISP_ROTATE_NONE;
				rec_width = face_svc_conf->media.rgb_width;
				rec_height = face_svc_conf->media.rgb_height;
				break;
		}
	} else if (camera_id == FACE_CAMERA_RGB_MIRROR) {
		switch (face_svc_conf->media.rgb_mirror_rotation) {
			case 90:
				rotation = VDISP_ROTATE_90;
				rec_width = face_svc_conf->media.rgb_mirror_height;
				rec_height = face_svc_conf->media.rgb_mirror_width;
				break;
			case 270:
				rotation = VDISP_ROTATE_270;
				rec_width = face_svc_conf->media.rgb_mirror_height;
				rec_height = face_svc_conf->media.rgb_mirror_width;
				break;
			default:
				rotation = VDISP_ROTATE_NONE;
				rec_width = face_svc_conf->media.rgb_mirror_width;
				rec_height = face_svc_conf->media.rgb_mirror_height;
				break;
		}
	} else if (camera_id == FACE_CAMERA_NIR) {
		switch (face_svc_conf->media.nir_rotation) {
			case 90:
				rotation = VDISP_ROTATE_90;
				rec_width = face_svc_conf->media.nir_height;
				rec_height = face_svc_conf->media.nir_width;
				break;
			case 270:
				rotation = VDISP_ROTATE_270;
				rec_width = face_svc_conf->media.nir_height;
				rec_height = face_svc_conf->media.nir_width;
				break;
			default:
				rotation = VDISP_ROTATE_NONE;
				rec_width = face_svc_conf->media.nir_width;
				rec_height = face_svc_conf->media.nir_height;
				break;
		}
	} else {
		ALOGE("unknow camera id:%d", camera_id);
		return -1;
	}

	if (enc_para) {
		enc_param.width = enc_para->width;
		enc_param.height = enc_para->height;
		enc_param.bitrate = enc_para->bitrate;
		enc_param.rotate = enc_para->rotate;
		enc_param.fmt = enc_para->fmt;
	} else {
		enc_param.width = rec_width;
		enc_param.height = rec_height;
		enc_param.bitrate = 4 * 1000 * 1000;
		enc_param.rotate = rotation;
		enc_param.fmt = REC_OUTPUT_FORMAT_MP4;
	}

	ret = video_rec_set_para(vs_rec->recorder, &enc_param);
	if (ret < 0) {
		ALOGE("LB_REC_SET_ENC_PARA error\n");
		return -1;
	}

	ret = video_rec_start(vs_rec->recorder);
	if (ret < 0) {
		ALOGE("LB_REC_START error\n");
		return -1;
	}

	return 0;
}

int face_media_rec_stop(void *hdl, enum face_camera_id camera_id)
{
	vs_record_t *vs_rec;
	struct face_media *media = (struct face_media *)hdl;
	if (media == NULL)
		return -1;
	vs_rec = media->vs_rec[camera_id];
	if (vs_rec == NULL)
		return -1;
	if (video_rec_stop(vs_rec->recorder) < 0) {
		ALOGE("LB_REC_STOP error\n");
		return -1;
	}
	return 0;
}

int face_media_stop(void *hdl)
{
	struct face_media *media = (struct face_media *)hdl;
	app_frame_cb_t cb;
	int ret;

	if (media == NULL)
		return -1;

	if (face_svc_conf->media.nir_enable) {
		led_off();
		cb.type = VS_VIDEO_RAW_FRAME;
		cb.app_data = NULL;
		cb.buf_handle = NULL;
		ret = video_stream_set_frame_cb(media->nir_hdl, &cb);
		if (ret)
			ALOGE("set cb failed\n");
	}

	if (face_svc_conf->media.rgb_enable) {
		cb.type = VS_VIDEO_RAW_FRAME;
		cb.app_data = NULL;
		cb.buf_handle = NULL;
		ret = video_stream_set_frame_cb(media->rgb_hdl, &cb);
		if (ret)
			ALOGE("set cb failed\n");

		ret = video_stream_stop_preview(media->rgb_hdl);
		if (ret)
			ALOGE("video_stream_stop_preview failed");

		ret = video_stream_release_preview(media->rgb_hdl);
		if (ret)
			ALOGE("video_stream_release_preview failed");

		ret = video_stream_stop_camera(media->rgb_hdl);
		if (ret)
			ALOGE("video_stream_stop_camera failed");
		video_stream_release(media->rgb_hdl);
	}

	if (face_svc_conf->media.rgb_mirror_enable) {
		cb.type = VS_VIDEO_RAW_FRAME;
		cb.app_data = NULL;
		cb.buf_handle = NULL;
		ret = video_stream_set_frame_cb(media->rgb_mirror_hdl, &cb);
		if (ret)
			ALOGE("set cb failed\n");

		ret = video_stream_stop_preview(media->rgb_mirror_hdl);
		if (ret)
			ALOGE("video_stream_stop_preview failed");

		ret = video_stream_release_preview(media->rgb_mirror_hdl);
		if (ret)
			ALOGE("video_stream_release_preview failed");

		ret = video_stream_stop_camera(media->rgb_mirror_hdl);
		if (ret)
			ALOGE("video_stream_stop_camera failed");
		video_stream_release(media->rgb_mirror_hdl);
	}

	if (face_svc_conf->media.nir_enable) {
		ret = video_stream_stop_preview(media->nir_hdl);
		if (ret)
			ALOGE("video_stream_stop_preview failed");

		ret = video_stream_release_preview(media->nir_hdl);
		if (ret)
			ALOGE("video_stream_release_preview failed");

		ret = video_stream_stop_camera(media->nir_hdl);
		if (ret)
			ALOGE("video_stream_stop_camera failed");
		video_stream_release(media->nir_hdl);
	}

	return 0;
}

int face_media_destroy(void *hdl)
{
	struct face_media *media = (struct face_media *)hdl;

	if (media == NULL)
		return -1;

	media_deinit();

	if (media) {
		free(media);
		media = NULL;
	}

	return 0;
}

