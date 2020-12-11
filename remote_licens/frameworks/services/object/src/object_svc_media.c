/*
 * object_svc_media.c
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

#define LOG_TAG "object_svc"
#include <log/log.h>

#include <iniparser.h>

#include "lombo_malloc.h"
#include "video_stream.h"

#include "object_svc.h"
#include "object_svc_media.h"

#include "object_svc_conf.h"

#define RGB_CAMERA_SOURCE	"/dev/video3"

struct cb_priv {
	p_object_frame_cb cb;
	void *p;
};

struct object_media {
	void *rgb_hdl;
	struct cb_priv *rgb_cb_priv;
	void *nir_hdl;
	struct cb_priv *nir_cb_priv;

	int rgb_enable;
	int rgb_width;
	int rgb_height;
	int rgb_preview;
	int rgb_preview_width;
	int rgb_preview_height;
	int rgb_rotation;
};

static void object_media_load_conf(struct object_media *media)
{
	dictionary *conf;

	int rgb_enable = DEFAULT_RGB_ENABLE;
	int rgb_width = DEFAULT_RGB_WIDTH;
	int rgb_height = DEFAULT_RGB_HEIGHT;
	int rgb_preview = DEFAULT_RGB_PREVIEW;
	int rgb_preview_width = DEFAULT_RGB_PREVIEW_WIDTH;
	int rgb_preview_height = DEFAULT_RGB_PREVIEW_HEIGHT;
	int rgb_rotation = DEFAULT_RGB_ROTATION;

	if (!media) {
		ALOGE("%s invlid parameter", __func__);
		return;
	}

	conf = iniparser_load(CONF_PATH);
	if (conf) {
		rgb_enable = iniparser_getint(conf, "media:rgb_enable", DEFAULT_RGB_ENABLE);
		rgb_width = iniparser_getint(conf, "media:rgb_width", DEFAULT_RGB_WIDTH);
		rgb_height = iniparser_getint(conf, "media:rgb_height", DEFAULT_RGB_HEIGHT);
		rgb_preview = iniparser_getint(conf, "media:rgb_preview", DEFAULT_RGB_PREVIEW);
		rgb_preview_width = iniparser_getint(conf, "media:rgb_preview_width", DEFAULT_RGB_PREVIEW_WIDTH);
		rgb_preview_height = iniparser_getint(conf, "media:rgb_preview_height", DEFAULT_RGB_PREVIEW_HEIGHT);
		rgb_rotation = iniparser_getint(conf, "media:rgb_rotation", DEFAULT_RGB_ROTATION);

		iniparser_freedict(conf);
	} else {
		ALOGE("%s load %s failed, used default", __func__, CONF_PATH);
	}

	ALOGI("object service media configuration");
	ALOGI("  rgb_enable: %d", rgb_enable);
	ALOGI("  rgb_width: %d", rgb_width);
	ALOGI("  rgb_height: %d", rgb_height);
	ALOGI("  rgb_preview: %d", rgb_preview);
	ALOGI("  rgb_preview_width: %d", rgb_preview_width);
	ALOGI("  rgb_preview_height: %d", rgb_preview_height);
	ALOGI("  rgb_rotation: %d", rgb_rotation);

	media->rgb_enable = rgb_enable;
	media->rgb_width = rgb_width;
	media->rgb_height = rgb_height;
	media->rgb_preview = rgb_preview;
	media->rgb_preview_width = rgb_preview_width;
	media->rgb_preview_height = rgb_preview_height;
	media->rgb_rotation = rgb_rotation;
}

static void *object_stream_start(char *source, int width, int height, int rot,
		int preview, int preview_width, int preview_height,
		vdisp_mode_e mode)
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
		printf("stream create failed\n");
		return NULL;
	}

	ret = video_stream_set_camera_source(hdl, source);
	if (ret) {
		printf("video_stream_set_camera_source failed\n");
		return NULL;
	}

	ret = video_stream_set_camera_size(hdl, width, height);
	if (ret) {
		printf("video_stream_set_camera_size failed\n");
		return NULL;
	}

	ret = video_stream_set_rotate(hdl, rotation);
	if (ret) {
		printf("video_stream_set_rotate failed\n");
		return NULL;
	}

	ret = video_stream_start_camera(hdl);
	if (ret) {
		printf("video_stream_start_camera failed\n");
		return NULL;
	}

	if (preview) {
		win_para_t disp_para;
		memset(&disp_para, 0, sizeof(disp_para));
		disp_para.rect.x = 0;
		disp_para.rect.y = 0;
		disp_para.rect.width = preview_width;
		disp_para.rect.height = preview_height;
		disp_para.mode = mode;
		ret = video_stream_set_disp_para(hdl, &disp_para, rotation);
		if (ret) {
			printf("video_stream_set_preview_window failed\n");
			return NULL;
		}

		ret = video_stream_prepare_preview(hdl);
		if (ret) {
			printf("video_stream_prepare_preview failed\n");
			return NULL;
		}

		ret = video_stream_start_preview(hdl);
		if (ret) {
			printf("video_stream_start_preview failed\n");
			return NULL;
		}
	}

	return hdl;
}

static int object_media_frame_cb_handle(void *p, frame_t *in_frame)
{
	object_frame_t *f_frame = NULL;
	frame_t *frame = NULL;
	struct cb_priv *priv;
	int ret;

	priv = (struct cb_priv *)p;
	if (priv->cb) {
		f_frame = malloc(sizeof(object_frame_t));
		if (!f_frame)
			goto exit;
		frame = malloc(sizeof(frame_t));
		if (!frame)
			goto exit;
		memcpy(frame, in_frame, sizeof(frame_t));
		memset(f_frame, 0, sizeof(object_frame_t));
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
		f_frame->time_stamp = frame->info.video.time_stamp;
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
		printf("return frame failed\n");
		return -1;
	}

	return 0;
}

static int object_media_img_cb_handle(void *p, frame_t *in_frame)
{
	object_frame_t *f_frame = NULL;
	frame_t *frame = NULL;
	p_object_img_cb cb;
	int ret;

	cb = (p_object_img_cb)p;
	if (cb) {
		f_frame = malloc(sizeof(object_frame_t));
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
		f_frame->time_stamp = frame->info.video.time_stamp;
		f_frame->priv = frame;
		ret = cb(f_frame);
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
		printf("return frame failed\n");
		return -1;
	}

	return 0;
}

int object_media_return_frame(object_frame_t *f_frame)
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

static struct cb_priv *_object_media_set_cb(void *hdl, void *p,
							p_object_frame_cb f_cb)
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

	cb.type = VS_VIDEO_RAW_FRAME;
	cb.app_data = priv;
	cb.buf_handle = object_media_frame_cb_handle;
	ret = video_stream_set_frame_cb(hdl, &cb);
	if (ret) {
		printf("set cb failed\n");
		return NULL;
	}

	return priv;
}

static int _object_media_take_picture(void *hdl, p_object_img_cb f_cb)
{
	app_frame_cb_t cb;
	int ret;

	if (!hdl || !f_cb)
		return -1;

	cb.type = VS_IMAGE_FRAME;
	cb.app_data = f_cb;
	cb.buf_handle = object_media_img_cb_handle;
	ret = video_stream_take_picture(hdl, &cb);
	if (ret) {
		printf("take pic failed\n");
		return -1;
	}

	return 0;
}

int object_media_set_cb(void *hdl, enum object_camera_id camera_id,
						void *p, p_object_frame_cb f_cb)
{
	struct object_media *media = (struct object_media *)hdl;
	struct cb_priv *priv;
	void *handle;

	if (!media)
		return -1;

	if (camera_id == OBJECT_CAMERA_RGB && media->rgb_enable)
		handle = media->rgb_hdl;
	else {
		/* FIXME */
		ALOGI("camera id: %d", camera_id);
		return 0;
	}

	priv = _object_media_set_cb(handle, p, f_cb);
	if (priv == NULL)
		return -1;

	return 0;
}

int object_media_take_picture(void *hdl, enum object_camera_id camera_id,
						p_object_img_cb f_cb)
{
	struct object_media *media = (struct object_media *)hdl;
	void *handle;

	if (!media)
		return -1;

	if (camera_id == OBJECT_CAMERA_RGB)
		handle = media->rgb_hdl;
	else if (camera_id == OBJECT_CAMERA_NIR)
		handle = media->nir_hdl;
	else {
		ALOGE("unknow camera id:%d", camera_id);
		return -1;
	}

	return _object_media_take_picture(handle, f_cb);
}

int object_media_get_camera_config(void *hdl, enum object_camera_id camera_id,
						camera_cfg_t *cfg)
{
	struct object_media *media = (struct object_media *)hdl;

	if (!media || cfg == NULL)
		return -1;

	if (camera_id == OBJECT_CAMERA_RGB) {
		cfg->width = media->rgb_width;
		cfg->height = media->rgb_height;
	} else {
		ALOGE("unknow camera id:%d", camera_id);
		return -1;
	}

	return 0;
}

void *object_media_create(void)
{
	struct object_media *media;
	int ret;

	/* initialize media framework */
	ret = media_init();
	if (ret) {
		printf("media init failed\n");
		return NULL;
	}

	media = malloc(sizeof(struct object_media));
	if (!media) {
		printf("object media create failed\n");
		return NULL;
	}
	memset(media, 0, sizeof(struct object_media));

	object_media_load_conf(media);

	return media;
}

int object_media_start(void *hdl)
{
	struct object_media *media = (struct object_media *)hdl;

	if (media == NULL)
		return -1;

	if (media->rgb_enable) {
		media->rgb_hdl = object_stream_start(RGB_CAMERA_SOURCE,
			media->rgb_width, media->rgb_height,
			media->rgb_rotation, media->rgb_preview,
			media->rgb_preview_width, media->rgb_preview_height,
			VDISP_WINDOW_FULL_SCREEN_SCREEN_RATIO);
		if (!media->rgb_hdl) {
			printf("object rgb stream start failed\n");
			return -1;
		}
	}

	return 0;
}

int object_media_stop(void *hdl)
{
	struct object_media *media = (struct object_media *)hdl;
	app_frame_cb_t cb;
	int ret;

	if (media == NULL)
		return -1;

	if (media->rgb_enable) {
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

	return 0;
}

int object_media_destroy(void *hdl)
{
	struct object_media *media = (struct object_media *)hdl;

	if (media == NULL)
		return -1;

	return 0;
}

