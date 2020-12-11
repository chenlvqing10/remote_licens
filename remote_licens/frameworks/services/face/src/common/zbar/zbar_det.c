/*
 * zbar_svc_main.c
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
#include <sys/time.h>
#include <sys/prctl.h>
#include <zbar.h>

#define LOG_TAG "zbar_svc"
#include <log/log.h>
#include "cut.h"
#include "flip.h"
#include "face_svc.h"
#include "face_svc_main.h"
#include "face_svc_isp.h"
#include "face_svc_conf.h"
#include "zbar_det.h"

typedef struct _zbar_buf_s {
	face_frame_t *frame;
	zbar_frame_t *zbar_frame;
} zbar_buf_t;

zbar_svc_t *zbar_svc;

static void dump_zbar_yuv(char *path, zbar_frame_t *frame)
{
	FILE *fp = NULL;

	if (!path || !frame)
		return;

	fp = fopen(path, "wb");
	if (!fp) {
		ALOGE("%s open %s failed, errno: %d (%s)", __func__, path,
			errno, strerror(errno));
		return;
	}

	fwrite(frame->addr[0], 1, frame->width * frame->height, fp);
	fwrite(frame->addr[1], 1, frame->width * frame->height / 2, fp);
	fclose(fp);
}

static zbar_frame_t *zbar_handle_cut_frame(face_frame_t *f_frame)
{
	struct cut_input cut_in;
	struct cut_output cut_out;
	zbar_frame_t *zbar_frame = NULL;

	if (!zbar_svc) {
		ALOGE("[%s:%d]zbar zbar_svc be NULL\n", __func__, __LINE__);
		return NULL;
	}

	if (!zbar_svc->cb) {
		ALOGE("[%s:%d]cb cannot be NULL\n",  __func__, __LINE__);
		return NULL;
	}

	memset(&cut_in, 0, sizeof(struct cut_input));
	memset(&cut_out, 0, sizeof(struct cut_output));

	zbar_frame = malloc(sizeof(zbar_frame_t));
	if (zbar_frame == NULL) {
		ALOGE("malloc failed");
		goto free_res;
	}
	memset(zbar_frame, 0, sizeof(zbar_frame_t));
	zbar_frame->det_begin = get_systime_us();

	cut_in.addr[0] = f_frame->viraddr[0];
	cut_in.addr[1] = f_frame->viraddr[1];
	cut_in.addr[2] = f_frame->viraddr[2];
	cut_in.size[0] = f_frame->size[0];
	cut_in.size[1] = f_frame->size[1];
	cut_in.size[2] = f_frame->size[2];
	cut_in.src_width = f_frame->width;
	cut_in.src_height = f_frame->height;
	cut_in.format = CUT_COLOR_NV12;
	/* keep the same as the UI postion */
	cut_in.dest_x = face_svc_conf->svc.zbar_det_x + face_svc_conf->svc.rgb_x_margin;
	cut_in.dest_y = face_svc_conf->svc.zbar_det_y + face_svc_conf->svc.rgb_y_margin;
	cut_in.dest_width = face_svc_conf->svc.zbar_det_width;
	cut_in.dest_height = face_svc_conf->svc.zbar_det_height;

	if (yuv_cut_start(&cut_in, &cut_out)) {
		ALOGE("%s crop face from frame failed", __func__);
		goto free_res;
	}

	zbar_frame->addr[0] = malloc(cut_out.width * cut_out.height * 3 / 2);
	if (zbar_frame->addr[0] == NULL) {
		ALOGE("malloc failed");
		goto free_res;
	}

	zbar_frame->addr[1] = zbar_frame->addr[0] + cut_out.width * cut_out.height;
	memset(zbar_frame->addr[0], 0, cut_out.width * cut_out.height);
	memcpy(zbar_frame->addr[0], cut_out.addr[0], cut_out.width * cut_out.height);
	memset(zbar_frame->addr[1], 128, cut_out.width * cut_out.height / 2);

	zbar_frame->width = cut_out.width;
	zbar_frame->height = cut_out.height;
	zbar_frame->size[0] = cut_out.size[0];
	zbar_frame->size[1] = cut_out.size[1];
	zbar_frame->size[2] = cut_out.size[2];

	yuv_cut_end(&cut_out);
	return zbar_frame;

free_res:

	yuv_cut_end(&cut_out);
	if (zbar_frame->addr[0]) {
		free(zbar_frame->addr[0]);
		zbar_frame->addr[0] = NULL;
		zbar_frame->addr[1] = NULL;
	}
	if (zbar_frame) {
		free(zbar_frame);
		zbar_frame = NULL;
	}
	return NULL;
}

static zbar_frame_t *zbar_handle_full_frame(face_frame_t *f_frame)
{
	zbar_frame_t *zbar_frame = NULL;

	if (!zbar_svc) {
		ALOGE("[%s:%d]zbar zbar_svc be NULL\n", __func__, __LINE__);
		return NULL;
	}

	if (!zbar_svc->cb) {
		ALOGE("[%s:%d]cb cannot be NULL\n",  __func__, __LINE__);
		return NULL;
	}

	zbar_frame = malloc(sizeof(zbar_frame_t));
	if (zbar_frame == NULL) {
		ALOGE("malloc failed");
		goto free_res;
	}
	memset(zbar_frame, 0, sizeof(zbar_frame_t));
	zbar_frame->det_begin = get_systime_us();

	zbar_frame->addr[0] = malloc(f_frame->width * f_frame->height * 3 / 2);
	if (zbar_frame->addr[0] == NULL) {
		ALOGE("malloc failed");
		goto free_res;
	}

	/* copy frame */
	zbar_frame->addr[1] = zbar_frame->addr[0] + f_frame->width * f_frame->height;
	memset(zbar_frame->addr[0], 0, f_frame->width * f_frame->height);
	memcpy(zbar_frame->addr[0], f_frame->viraddr[0], f_frame->width * f_frame->height);
	memset(zbar_frame->addr[1], 128, f_frame->width * f_frame->height / 2);

	zbar_frame->width = f_frame->width;
	zbar_frame->height = f_frame->height;
	zbar_frame->size[0] = f_frame->size[0];
	zbar_frame->size[1] = f_frame->size[1];
	zbar_frame->size[2] = f_frame->size[2];

	return zbar_frame;

free_res:
	if (zbar_frame->addr[0]) {
		free(zbar_frame->addr[0]);
		zbar_frame->addr[0] = NULL;
		zbar_frame->addr[1] = NULL;
	}
	if (zbar_frame) {
		free(zbar_frame);
		zbar_frame = NULL;
	}
	return NULL;
}

static zbar_frame_t *zbar_handle_frame(face_frame_t *f_frame)
{
	zbar_frame_t *zbar_frame = NULL;

	if (!f_frame) {
		ALOGE("[%s:%d]zbar f_frame be NULL\n", __func__, __LINE__);
		return NULL;
	}

	if (!zbar_svc) {
		ALOGE("[%s:%d]zbar zbar_svc be NULL\n", __func__, __LINE__);
		return NULL;
	}

	if (!zbar_svc->cb) {
		ALOGE("[%s:%d]cb cannot be NULL\n",  __func__, __LINE__);
		return NULL;
	}

	if(zbar_svc->mode == 1) {
		zbar_frame = zbar_handle_cut_frame(f_frame);
	} else if (zbar_svc->mode == 2) {
		zbar_frame = zbar_handle_full_frame(f_frame);
	} else {
		ALOGE("[%s:%d] unknow mode %d\n",  __func__, __LINE__, zbar_svc->mode);
	}

	return zbar_frame;
}

static void *zbar_scan_thread(void *arg)
{
	zbar_svc_t *zbar_svc = (zbar_svc_t *)arg;
	zbar_buf_t* zbar_buf = NULL;
	face_frame_t *frame = NULL;
	zbar_frame_t *zbar_frame = NULL;
	zbar_svc->is_running = 1;

	ALOGV("enter %s", __func__);

	prctl(PR_SET_NAME, "zbar-scan");

	while (zbar_svc->is_running) {

		pthread_mutex_lock(&zbar_svc->zbar_scan_lock);
		if (empty_queue(&zbar_svc->zbar_scan_queue))
			pthread_cond_wait(&zbar_svc->zbar_scan_cond, &zbar_svc->zbar_scan_lock);
		zbar_buf = (zbar_buf_t *)de_queue(&zbar_svc->zbar_scan_queue);
		pthread_mutex_unlock(&zbar_svc->zbar_scan_lock);

		if (zbar_buf == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}

		frame = zbar_buf->frame;
		if (frame) {
			zbar_buf->zbar_frame = zbar_handle_frame(frame);
			free_frame_lock(&frame);
		}
		zbar_frame = zbar_buf->zbar_frame;

		if (zbar_frame == NULL ||
			zbar_frame->addr[0] == NULL ||
			zbar_frame->addr[1] == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}
		/* create a reader */
		/* wrap image data */
		zbar_image_t *image = zbar_image_create();
		zbar_image_set_format(image, zbar_fourcc('Y','8','0','0'));
		zbar_image_set_size(image, zbar_frame->width, zbar_frame->height);
		zbar_image_set_data(image, zbar_frame->addr[0],
			zbar_frame->width * zbar_frame->height, zbar_image_free_data);

		/* scan the image for barcodes */
		int n = zbar_scan_image(zbar_svc->scanner, image);
		(void)n;

		/* extract results */
		const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
		for (; symbol; symbol = zbar_symbol_next(symbol)) {
			/* do something useful with results */
			zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
			const char *data = zbar_symbol_get_data(symbol);
			zbar_svc->cb(zbar_svc->arg, (char*)data);

			if (face_svc_conf->svc.zbar_debug) {
				ALOGD_IF(1, "decoded %s symbol \"%s\"\n",
						zbar_get_symbol_name(typ), data);
				zbar_frame->det_end = get_systime_us();
				ALOGD_IF(1,
					"%s qdcode_det begin = %lld end = %lld end - begin = %lld\n",
					__func__,zbar_frame->det_begin, zbar_frame->det_end,
					zbar_frame->det_end - zbar_frame->det_begin);
			}
		}

		if (face_svc_conf->svc.zbar_debug) {
			char path[128] = {0};
			snprintf(path, 128 - 1, "/data/zbar-%d-%d-%lld-%lld.yuv",
				zbar_frame->width, zbar_frame->height,
				zbar_frame->det_begin, zbar_frame->det_end);
			dump_zbar_yuv(path, zbar_frame);
		}

		/* clean up */
		zbar_image_destroy(image);
		if (zbar_frame->addr[0]) {
			/* set zbar_image_free_data
			 * zbar interal free zbar_frame->raw
			 * don't free zbar_frame->raw by myself
			 * */
			/* free(zbar_frame->raw); */
			/* zbar_frame->raw = NULL; */
		}

		if (zbar_frame) {
			free(zbar_frame);
			zbar_frame = NULL;
		}
		if(zbar_buf){
			free(zbar_buf);
			zbar_buf = NULL;
		}

		if (zbar_svc->time_out) {
			long long cur_time = get_systime_us();
			if (cur_time > zbar_svc->time_out) {
				zbar_svc->time_out = 0;
				zbar_svc->cb(zbar_svc->arg, NULL);
			}
		}
		if (zbar_svc->mode == 2)
			usleep(400*1000);
	}

	return NULL;
}

void *zbar_svc_init(void)
{
	ALOGV("enter %s\n", __func__);

	zbar_svc = (zbar_svc_t *)malloc(sizeof(zbar_svc_t));
	if (!zbar_svc) {
		ALOGE("malloc face_svc_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(zbar_svc, 0, sizeof(zbar_svc_t));

	zbar_svc->scanner = zbar_image_scanner_create();

	/* configure the reader */
	zbar_image_scanner_set_config(zbar_svc->scanner,
		face_svc_conf->svc.zbar_code_type, ZBAR_CFG_ENABLE, 1);

	TAILQ_INIT(&zbar_svc->zbar_scan_queue);
	if (pthread_mutex_init(&zbar_svc->zbar_scan_lock, NULL)) {
		ALOGE("init rgb_frame_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	if (pthread_cond_init(&zbar_svc->zbar_scan_cond, NULL)) {
		ALOGE("init rgb_frame_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}

	/* create face service detect thread */
	ALOGV("create face service detect thread");
	if (pthread_create(&zbar_svc->zbar_scan_thrd, NULL,
			zbar_scan_thread, (void *)zbar_svc)) {
		ALOGE("create face service detect thread failed, "
			"errno: %d (%s)", errno, strerror(errno));
		goto exit;
	}

	return zbar_svc;

exit:

	zbar_svc_deinit();
	return NULL;
}

int zbar_register_det_cb(int mode, f_zbar_scan_cb cb, void *arg)
{
	if (!zbar_svc) {
		ALOGE("[%s:%d]zbar zbar_svc be NULL\n", __func__, __LINE__);
		return -1;
	}

	if (!cb) {
		ALOGE("[%s:%d]cb cannot be NULL\n",  __func__, __LINE__);
		return -1;
	}

	if (!mode) {
		ALOGE("[%s:%d]mode cannot be 0\n",  __func__, __LINE__);
		return -1;
	}

	zbar_svc->cb = cb;
	zbar_svc->arg = arg;
	zbar_svc->mode = mode;
	if (face_svc_conf->svc.zbar_timeout > 0) {
		zbar_svc->time_out = get_systime_us() + face_svc_conf->svc.zbar_timeout * 1000;
	}
	return 0;
}

static void zbar_set_exposure(void)
{
	viss_isp_rects_t rgb_rects;

	memset(&rgb_rects, 0, sizeof(viss_isp_rects_t));

	rgb_rects.cnt = 1;
	rgb_rects.rects = malloc(sizeof(viss_isp_rect_t) * 1);
	if (!rgb_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		rgb_rects.cnt = 0;
		goto exit;
	}

	/* rgb rect */
	rgb_rects.rects[0].left = face_svc_conf->svc.zbar_det_x;
	rgb_rects.rects[0].top = face_svc_conf->svc.zbar_det_y + face_svc_conf->svc.rgb_y_margin;
	rgb_rects.rects[0].right = (face_svc_conf->svc.zbar_det_x + face_svc_conf->svc.zbar_det_width);
	rgb_rects.rects[0].bottom = (face_svc_conf->svc.zbar_det_y + face_svc_conf->svc.zbar_det_height);

exit:
	face_set_ae_para(FACE_CAMERA_RGB, &rgb_rects);

	if (rgb_rects.rects) {
		free(rgb_rects.rects);
		rgb_rects.rects = NULL;
	}
}

void zbar_async_det_frame(face_frame_t *f_frame)
{
	zbar_buf_t *zbar_buf = NULL;
	face_frame_t *frame = f_frame;

	if (!zbar_svc) {
		ALOGE("[%s:%d]zbar zbar_svc be NULL\n", __func__, __LINE__);
		return;
	}

	if (!zbar_svc->cb) {
		ALOGE("[%s:%d]cb cannot be NULL\n",  __func__, __LINE__);
		return;
	}

	pthread_mutex_lock(&zbar_svc->zbar_scan_lock);
	if (queue_size(&zbar_svc->zbar_scan_queue) < 1) {
		zbar_buf = (zbar_buf_t *)malloc(sizeof(zbar_buf_t));
		if (!zbar_buf) {
			ALOGE("malloc zbar_buf_t failed, errno: %d (%s)\n",
				errno, strerror(errno));
			goto error;
		}
		retain_frame(frame);
		zbar_buf->frame = frame;
		zbar_buf->zbar_frame = NULL;
		en_queue(&zbar_svc->zbar_scan_queue, zbar_buf);
		pthread_cond_signal(&zbar_svc->zbar_scan_cond);
	}
error:
	pthread_mutex_unlock(&zbar_svc->zbar_scan_lock);
	return;
}

void zbar_det_frame(face_frame_t *f_frame)
{
	zbar_buf_t *zbar_buf = NULL;

	if (!zbar_svc) {
		ALOGE("[%s:%d]zbar zbar_svc be NULL\n", __func__, __LINE__);
		return;
	}

	if (!zbar_svc->cb) {
		ALOGE("[%s:%d]cb cannot be NULL\n",  __func__, __LINE__);
		return;
	}

	/*  */
	if (zbar_svc->mode == 1) {
		zbar_set_exposure();
	}
	pthread_mutex_lock(&zbar_svc->zbar_scan_lock);
	if (queue_size(&zbar_svc->zbar_scan_queue) < 1) {
		zbar_buf = (zbar_buf_t *)malloc(sizeof(zbar_buf_t));
		if (!zbar_buf) {
			ALOGE("malloc face_svc_t failed, errno: %d (%s)\n",
				errno, strerror(errno));
			goto error;
		}

		zbar_buf->frame = NULL;
		zbar_buf->zbar_frame = zbar_handle_frame(f_frame);
		en_queue(&zbar_svc->zbar_scan_queue, zbar_buf);
		pthread_cond_signal(&zbar_svc->zbar_scan_cond);
	}
error:
	pthread_mutex_unlock(&zbar_svc->zbar_scan_lock);
	return;
}

void zbar_svc_deinit(void)
{
	if (zbar_svc) {
		if (zbar_svc->is_running) {
			zbar_svc->is_running = 0;
			pthread_mutex_lock(&zbar_svc->zbar_scan_lock);
			pthread_cond_signal(&zbar_svc->zbar_scan_cond);
			pthread_mutex_unlock(&zbar_svc->zbar_scan_lock);
		}

		if (&zbar_svc->zbar_scan_queue) {
			pthread_mutex_lock(&zbar_svc->zbar_scan_lock);
			free_queue(&zbar_svc->zbar_scan_queue);
			pthread_mutex_unlock(&zbar_svc->zbar_scan_lock);
		}

		if (&zbar_svc->zbar_scan_cond)
			pthread_cond_destroy(&zbar_svc->zbar_scan_cond);
		if (&zbar_svc->zbar_scan_lock)
			pthread_mutex_destroy(&zbar_svc->zbar_scan_lock);
		if (zbar_svc->zbar_scan_thrd)
			pthread_join(zbar_svc->zbar_scan_thrd, NULL);

		/* stop zbar */
		if (zbar_svc->scanner)
			zbar_image_scanner_destroy(zbar_svc->scanner);
		free(zbar_svc);
		zbar_svc = NULL;
	}
	return;
}

