/*
 * face_svc_main.c
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

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <execinfo.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <dirent.h>
#include <math.h>

#define LOG_TAG "face_svc"
#include <log/log.h>

#include "lombo_malloc.h"
#include "face_svc.h"
#include "face_svc_main.h"
#include "face_svc_nn.h"
#include "face_svc_media.h"
#include "face_svc_living.h"
#include "ax_nir_dt.h"
#include "face_svc_isp.h"
#include "jpeg/jpeg_enc.h"
#include "cut.h"
#include "flip.h"
#include "recg_img.h"
#include "time_conv.h"
#include "file_ops.h"
#include "zbar_svc.h"
#include "rgb2nir.h"
#include "rgb2nir_new.h"
#include "temp_measure.h"
#include "private.h"
#include "lombo_jpegdec_plugin.h"
#include "jpeg_rescale.h"

static face_svc_t *g_svc;

#define DEFAULT_RECORD_DIR "/data/face_recg"
#define REGISTER_PIC_SAVE_PATH "/data/face_register"

#define CLIPMINMAX(v, mn, mx) {		\
	if ((v) < (mn))			\
		(v) = (mn);		\
	else if ((v) > (mx))		\
		(v) = (mx);		\
}

#define CLIPMIN(v, mn) {		\
	if ((v) < (mn))			\
		(v) = (mn);		\
}

#define CLIPMAX(v, mx) {		\
	if ((v) > (mx))			\
		(v) = (mx);		\
}

#define ALIGN_UP(x, align) (((int)(x) + (align - 1)) & ~(align -1))
#define ALIGN_DOWN(x, align) (((int)(x) & ~(align -1)))
#define ALIGNED(x, align) (((int)(x) & (align - 1)) == 0)

long long get_systime_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

char *strtype(int type)
{
	char *str;

	if (type == FACE_FRAME_TYPE_RGB)
		str = "rgb";
	else if (type == FACE_FRAME_TYPE_NIR)
		str = "nir";
	else if (type == FACE_FRAME_TYPE_API_DET)
		str = "api-det";
	else if (type == FACE_FRAME_TYPE_API_DET_BLOCK)
		str = "api-det-block";
	else if (type == FACE_FRAME_TYPE_API_RECG)
		str = "api-recg";
	else if (type == FACE_FRAME_TYPE_API_RECG_BLOCK)
		str = "api-recg-block";
	else if (type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK)
		str = "api-capture-block";
	else
		str = "unknown";

	return str;
}

#define STACK_DEPTH_MAX 128
void dump_stack(void)
{
	void *stack_trace[STACK_DEPTH_MAX] = {0};
	char **stack_strings = NULL;
	int stack_depth = 0;
	int i = 0;

	stack_depth = backtrace(stack_trace, STACK_DEPTH_MAX);
	stack_strings = (char **)backtrace_symbols(stack_trace, stack_depth);
	if (NULL == stack_strings) {
		ALOGI(" Memory is not enough while dump stack trace\n");
		exit(EXIT_FAILURE);
	}

	ALOGI(" Stack trace:\n");
	for (i = 0; i < stack_depth; i++) {
		ALOGI(" [%d] %s\n", stack_depth - i, stack_strings[i]);
	}
	ALOGI("\n");
	free(stack_strings);
	stack_strings = NULL;
	/* exit(EXIT_FAILURE); */
}

#define _maxsize_ 32
#define _format1_ "%Y%m%d"
#define _format2_ "%Y%m%d%H%M%S"
static char *ts2str1(long long ts)
{
	static char str[_maxsize_];
	time_t t = ts / 1000000;
	strftime(str, _maxsize_, _format1_, localtime(&t));
	return str;
}

static char *ts2str2(long long ts)
{
	static char str[_maxsize_];
	time_t t = ts / 1000000;
	long usec = ts - (ts / 1000000) * 1000000;
	size_t len = strftime(str, _maxsize_, _format2_, localtime(&t));
	snprintf(&str[len], _maxsize_ - len, "%ld", usec);
	return str;
}

static int create_dir(const char *path)
{
	if (access(path, R_OK | W_OK | X_OK))
		return mkdir(path, 0755);
	return 0;
}

void dump_frame_jpg(char *path, face_frame_t *frame)
{
	struct input_image_info input = {0};
	struct output_image_info output  = {0};

	if (!path || !frame)
		return;

	input.addr[0] = frame->viraddr[0];
	input.addr[1] = frame->viraddr[1];
	input.size[0] = frame->size[0];
	input.size[1] = frame->size[1];
	input.width = frame->width;
	input.height = frame->height;
	input.format = IMG_COLOR_NV12;
	input.qualify = 100;
	input.file = path;
	if (jpeg_enc_file(&input, &output))
		ALOGE("%s encode frame to %s failed", __func__, path);
}

void dump_frame_yuv(char *path, face_frame_t *frame)
{
	FILE *fp;

	if (!path || !frame)
		return;

	fp = fopen(path, "wb+");
	if (!fp) {
		ALOGE("%s open %s failed, errno: %d (%s)", __func__, path,
			errno, strerror(errno));
		return;
	}

	fwrite(frame->viraddr[0], 1, frame->width * frame->height, fp);
	fwrite(frame->viraddr[1], 1, frame->width * frame->height / 2, fp);

	fclose(fp);
}

void dump_frame(char *path, face_frame_t *frame)
{
	if (face_svc_conf->svc.dump_yuv) {
		ALOGD_IF(face_svc_conf->debug.debug_log, "frame %p dump yuv", frame);
		dump_frame_yuv(path, frame);
	} else {
		ALOGD_IF(face_svc_conf->debug.debug_log, "frame %p dump jpg", frame);
		dump_frame_jpg(path, frame);
	}
}

void dump_ID_photo(char *path, face_frame_t *frame, face_rect_t *rect,
		int x_margin, int y_margin)
{
	struct cut_input cut_in  = {0};
	struct cut_output cut_out = {0};
	struct input_image_info input = {0};
	struct output_image_info output = {0};

	if (!path || !frame || !rect)
		return;

	cut_in.addr[0] = frame->viraddr[0];
	cut_in.addr[1] = frame->viraddr[1];
	cut_in.size[0] = frame->size[0];
	cut_in.size[1] = frame->size[1];
	cut_in.src_width = frame->width;
	cut_in.src_height = frame->height;
	cut_in.format = CUT_COLOR_NV12;
	cut_in.dest_x = rect->rect[0] + x_margin;
	cut_in.dest_y = rect->rect[1] + y_margin;

	/* width and height should be aligned with 16 when encode
	 * image by hardware
	 */
	cut_in.dest_width = (rect->rect[2] - rect->rect[0] + 15) & ~0x0f;
	if (cut_in.dest_x + cut_in.dest_width >= cut_in.src_width)
		cut_in.dest_width -= 16;
	cut_in.dest_height = ((rect->rect[3] - rect->rect[1]) * 3 / 2 + 15) & ~0x0f;
	if (cut_in.dest_y + cut_in.dest_height >= cut_in.src_height)
		cut_in.dest_height = (face_svc_conf->svc.rgb_buffer_height - rect->rect[1]) & ~0x0f;
	if (cut_in.dest_y + cut_in.dest_height >= cut_in.src_height)
		cut_in.dest_height -= 16;
	if (yuv_cut_start(&cut_in, &cut_out)) {
		ALOGE("%s crop face from frame failed", __func__);
		return;
	}

	input.addr[0] = cut_out.addr[0];
	input.addr[1] = cut_out.addr[1];
	input.size[0] = cut_out.size[0];
	input.size[1] = cut_out.size[1];
	input.width = cut_out.width;
	input.height = cut_out.height;
	input.format = IMG_COLOR_NV12;
	input.qualify = 100;
	input.file = path;
	if (jpeg_enc_file(&input, &output))
		ALOGE("%s encode frame to %s failed", __func__, path);
	yuv_cut_end(&cut_out);
}

void dump_face(char *path, face_frame_t *frame, face_rect_t *rect,
		int x_margin, int y_margin)
{
	struct cut_input cut_in = {0};
	struct cut_output cut_out = {0};
	struct input_image_info input = {0};
	struct output_image_info output = {0};

	if (!path || !frame || !rect)
		return;

	cut_in.addr[0] = frame->viraddr[0];
	cut_in.addr[1] = frame->viraddr[1];
	cut_in.size[0] = frame->size[0];
	cut_in.size[1] = frame->size[1];
	cut_in.src_width = frame->width;
	cut_in.src_height = frame->height;
	cut_in.format = CUT_COLOR_NV12;
	cut_in.dest_x = rect->rect[0] + x_margin;
	cut_in.dest_y = rect->rect[1] + y_margin;

	/* width and height should be aligned with 16 when encode
	 * image by hardware
	 */
	cut_in.dest_width = (rect->rect[2] - rect->rect[0] + 15) & ~0x0f;
	if (cut_in.dest_x + cut_in.dest_width >= cut_in.src_width)
		cut_in.dest_width -= 16;
	cut_in.dest_height = (rect->rect[3] - rect->rect[1] + 15) & ~0x0f;
	if (cut_in.dest_y + cut_in.dest_height >= cut_in.src_height)
		cut_in.dest_height -= 16;
	if (yuv_cut_start(&cut_in, &cut_out)) {
		ALOGE("%s crop face from frame failed", __func__);
		return;
	}

	input.addr[0] = cut_out.addr[0];
	input.addr[1] = cut_out.addr[1];
	input.size[0] = cut_out.size[0];
	input.size[1] = cut_out.size[1];
	input.width = cut_out.width;
	input.height = cut_out.height;
	input.format = IMG_COLOR_NV12;
	input.qualify = 100;
	input.file = path;
	if (jpeg_enc_file(&input, &output))
		ALOGE("%s encode frame to %s failed", __func__, path);
	yuv_cut_end(&cut_out);
}

void dump_face_yuv(face_frame_t *frame, face_rect_t *rect,
	int x_margin, int y_margin)
{
	char pic_path[64]={0};
	long long time_stamp;

	time_stamp = get_systime_us();
	create_dir(REGISTER_PIC_SAVE_PATH);

	struct cut_input cut_in;
	struct cut_output cut_out;
	FILE *fp;

	if (!frame || !rect)
		return;

	cut_in.addr[0] = frame->viraddr[0];
	cut_in.addr[1] = frame->viraddr[1];
	cut_in.size[0] = frame->size[0];
	cut_in.size[1] = frame->size[1];
	cut_in.src_width = frame->width;
	cut_in.src_height = frame->height;
	cut_in.format = CUT_COLOR_NV12;
	cut_in.dest_x = rect->rect[0] + x_margin;
	cut_in.dest_y = rect->rect[1] + y_margin;

	/* width and height should be aligned with 16 when encode
	 * image by hardware
	 */
	cut_in.dest_width = (rect->rect[2] - rect->rect[0] + 15) & ~0x0f;
	if (cut_in.dest_x + cut_in.dest_width >= cut_in.src_width)
		cut_in.dest_width -= 16;
	cut_in.dest_height = (rect->rect[3] - rect->rect[1] + 15) & ~0x0f;
	if (cut_in.dest_y + cut_in.dest_height >= cut_in.src_height)
		cut_in.dest_height -= 16;
	if (yuv_cut_start(&cut_in, &cut_out)) {
		ALOGE("%s crop face from frame failed", __func__);
		return;
	}

	if (yuv_cut_start(&cut_in, &cut_out)) {
		ALOGE("%s crop face from frame failed", __func__);
		return;
	}

	snprintf(pic_path, sizeof(pic_path), "%s/face-%lld_%dx%d.yuv",
		REGISTER_PIC_SAVE_PATH, time_stamp, cut_out.width, cut_out.height);

	fp = fopen(pic_path, "wb+");
	if (!fp) {
		ALOGE("%s open %s failed, errno: %d (%s)", __func__, pic_path,
			errno, strerror(errno));
		yuv_cut_end(&cut_out);
		return;
	}

	fwrite(cut_out.addr[0], 1, cut_out.width * cut_out.height, fp);
	fwrite(cut_out.addr[1], 1, cut_out.width * cut_out.height / 2, fp);

	fclose(fp);

	yuv_cut_end(&cut_out);
}

void dump_frame_yuv2(face_frame_t *frame, face_rect_t *rect, int dump_num, int score)
{
	FILE *fp;
	char pic_path[64]={0};
	create_dir(REGISTER_PIC_SAVE_PATH);

	if (!frame)
		return;


	snprintf(pic_path, sizeof(pic_path), "%s/face-%d-%d-%d-%d_%d_%d_%d-s%d.yuv",
			REGISTER_PIC_SAVE_PATH, dump_num, frame->width, frame->height, rect->rect[0], rect->rect[1], rect->rect[2], rect->rect[3], score);

	fp = fopen(pic_path, "wb+");
	if (!fp) {
		ALOGE("%s open %s failed, errno: %d (%s)", __func__, pic_path,
			errno, strerror(errno));
		return;
	}

	fwrite(frame->viraddr[0], 1, frame->width * frame->height, fp);
	fwrite(frame->viraddr[1], 1, frame->width * frame->height / 2, fp);

	fclose(fp);
}


/*static*/ void dump_result(rgb_recg_buf_t *recg_buf, const char *suffix)
{
	char path[FILENAME_MAX] = {'\0'};
	face_frame_t *frame;
	face_info_t *info;
	face_rect_t *rect;
	face_feat_t *feat;
	FILE *fp;

	if (!recg_buf || !suffix)
		return;

	frame = recg_buf->frame;
	info = &recg_buf->face_info;
	if (!frame || !info)
		return;

	snprintf(path, FILENAME_MAX, "/data/face_svc/%s-%s-%lld-%d.txt",
		strtype(frame->type), suffix, frame->time_stamp, info->rect.id);

	fp = fopen(path, "w");
	if (!fp) {
		ALOGE("%s open %s failed, errno: %d (%s) ", __func__, path,
			errno, strerror(errno));
		return;
	}

	rect = &info->rect;
	fprintf(fp, "position: [%d, %d, %d, %d]\n",
		rect->rect[0], rect->rect[1],
		rect->rect[2], rect->rect[3]);
	fprintf(fp, "detect score: %d\n", rect->score);
	fprintf(fp, "key points: [%f, %f], [%f, %f], "
		"[%f, %f], [%f, %f], [%f, %f]\n",
		rect->kpts[0], rect->kpts[1],
		rect->kpts[2], rect->kpts[3],
		rect->kpts[4], rect->kpts[5],
		rect->kpts[6], rect->kpts[7],
		rect->kpts[8], rect->kpts[9]);
	fprintf(fp, "quality socre: %d\n",
		rect->quality_score);

	feat = &info->feat;
	fprintf(fp, "compare score: %f\n", feat->score);
	fprintf(fp, "max id: %d\n", feat->max_id);
	fprintf(fp, "living body: %d\n", feat->living_body);
	fprintf(fp, "compare confidence: %d\n", feat->cmp_conf);

	fclose(fp);
}

int save_roi(face_svc_t *svc, rgb_recg_buf_t *recg_buf, const char *prefix,
	int x, int y, int width, int height)
{
	FILE *fp = NULL;
	char path[FILENAME_MAX] = {'\0'};
	snprintf(path, FILENAME_MAX, "%s/%s/%s-%s.txt",
		DEFAULT_RECORD_DIR,
		ts2str1(recg_buf->frame->time_stamp),
		prefix,
		ts2str2(recg_buf->frame->time_stamp));
	fp = fopen(path, "a");
	if (NULL == fp) {
		ALOGE("[%s] fopen %s fail !\n", __func__, path);
		return -1;
	}
	fprintf(fp, "%d %d %d %d\n", x, y, width, height);
	fclose(fp);
	return 0;
}

int save_rgb_living_frame(face_svc_t *svc, tracking_face_t *face, rgb_recg_buf_t *recg_buf,
	int living_score, int x, int y, int width, int height)
{
	if (living_score <= 0) {
		save_record(svc, face, recg_buf, debug_rgb_no_living);
		save_roi(svc, recg_buf, "debug_rgb_no_living",
			x, y, width, height);
	} else {
		save_record(svc, face, recg_buf, debug_rgb_living);
		save_roi(svc, recg_buf, "debug_rgb_living",
			x, y, width, height);
	}
	return 0;
}

void retain_frame(face_frame_t *frame)
{
	frame->ref++;
}

void free_frame(face_frame_t **pp)
{
	face_frame_t *frame = *pp;

	if (frame) {
		ALOGD_IF(0, "frame: %p type: %s, ref: %d",
			frame, strtype(frame->type), frame->ref);
		if (--frame->ref == 0) {
			ALOGD_IF(0, "~~~ frame out %p", frame);
			face_media_return_frame(frame);
			frame = NULL;
		} else if (frame->ref < 0) {
			dump_stack();
		}
	}
}

void free_frame_lock(face_frame_t **pp)
{
	face_svc_t *svc = (face_svc_t *)g_svc;
	pthread_mutex_lock(&svc->tracking_lock);
	free_frame(pp);
	pthread_mutex_unlock(&svc->tracking_lock);
}

static char *set_img_path_prefix(record_img_e type)
{
	if (type == debug_unmatch_frame) {
		return "debug_unmatch_frame";
	} else if (type == debug_unmatch_face) {
		return "debug_unmatch_face";
	} else if (type == debug_nir_living) {
		return "debug_nir_living";
	} else if (type == debug_nir_no_living) {
		return "debug_nir_no_living";
	} else if (type == debug_nir_no_face) {
		return "debug_nir_no_face";
	} else if (type == debug_nir_det_face) {
		return "debug_nir_det_face";
	} else if (type == debug_rgb_det_face) {
		return "debug_rgb_det_face";
	} else if (type == debug_rgb_living) {
		return "debug_rgb_living";
	} else if (type == debug_rgb_no_living) {
		return "debug_rgb_no_living";
	} else if (type == debug_rgb_color_living) {
		return "debug_rgb_color_living";
	} else if (type == debug_rgb_yolo_living) {
		return "debug_rgb_yolo_living";
	} else if (type == unmatch_frame) {
		return "unmatch_frame";
	} else if (type == unmatch_face) {
		return "unmatch_face";
	} else if (type == match_frame) {
		return "match_frame";
	} else if (type == match_face) {
		return "match_face";
	} else if (type == unqualified_frame) {
		return "unqualified_frame";
	} else if (type == unqualified_face) {
		return "unqualified_face";
	} else if (type == living) {
		return "living";
	} else if (type == no_face) {
		return "no-face";
	} else {
		return "unknown";
	}
}

static void set_img_path(face_frame_t *frame, record_img_t *img,
	record_img_e type)
{
	char dir[FILENAME_MAX] = {'\0'};
	snprintf(dir, FILENAME_MAX, "%s/%s",
		DEFAULT_RECORD_DIR, ts2str1(frame->time_stamp));
	if (create_dir(dir))
		return;

	snprintf(img->path[type], FILENAME_MAX,
		"%s/%s-%s.jpg",
		dir, set_img_path_prefix(type),
		ts2str2(frame->time_stamp));
	return;
}

static void set_img_face_path(face_frame_t *frame, int id, record_img_t *img,
	record_img_e type)
{
	char dir[FILENAME_MAX] = {'\0'};
	create_dir(DEFAULT_RECORD_DIR);
	snprintf(dir, FILENAME_MAX, "%s/%s",
		DEFAULT_RECORD_DIR, ts2str1(frame->time_stamp));
	if (create_dir(dir))
		return;

	snprintf(img->path[type], FILENAME_MAX,
		"%s/%s-%s-%d.jpg",
		dir, set_img_path_prefix(type),
		ts2str2(frame->time_stamp), id);
	return;
}

static void set_debug_img_path(face_frame_t *frame, record_img_t *img,
	char *filename)
{
	char dir[FILENAME_MAX] = {'\0'};
	snprintf(dir, FILENAME_MAX, "%s/%s",
		DEFAULT_RECORD_DIR, ts2str1(frame->time_stamp));
	if (create_dir(dir))
		return;

	snprintf(img->path[debug_frame], FILENAME_MAX,
		"%s/%s-%s.jpg",
		dir, filename,
		ts2str2(frame->time_stamp));
	return;
}

static face_frame_t *get_record_frame(record_img_e type, rgb_recg_buf_t *recg_buf)
{
	if (type == debug_unmatch_frame) {
		return recg_buf->frame;
	} else if (type == debug_unmatch_face) {
		return recg_buf->frame;
	} else if (type == debug_nir_living) {
		return recg_buf->nir_frame;
	} else if (type == debug_nir_no_living) {
		return recg_buf->nir_frame;
	} else if (type == debug_nir_no_face) {
		return recg_buf->nir_frame;
	} else if (type == debug_nir_det_face) {
		return recg_buf->nir_frame;
	} else if (type == debug_rgb_det_face) {
		return recg_buf->frame;
	} else if (type == debug_rgb_living) {
		return recg_buf->frame;
	} else if (type == debug_rgb_no_living) {
		return recg_buf->frame;
	} else if (type == debug_rgb_color_living) {
		return recg_buf->frame;
	} else if (type == debug_rgb_yolo_living) {
		return recg_buf->frame;
	} else if (type == unmatch_frame) {
		return recg_buf->frame;
	} else if (type == unmatch_face) {
		return recg_buf->frame;
	} else if (type == match_frame) {
		return recg_buf->frame;
	} else if (type == match_face) {
		return recg_buf->frame;
	} else if (type == unqualified_frame) {
		return recg_buf->frame;
	} else if (type == unqualified_face) {
		return recg_buf->frame;
	} else if (type == living) {
		if (recg_buf->nir_frame)
			return recg_buf->nir_frame;
		else
			return recg_buf->frame;
	} else {
		ALOGE("unknown record_img_e type");
		return recg_buf->frame;
	}
}

/*
 * should lock
 */
void save_debug_frame(face_svc_t *svc, face_frame_t *frame, char *filename)
{
	face_record_buf_t *record_buf;

	if (!svc || !frame)
		return;

	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	record_buf->frame = frame;
	retain_frame(record_buf->frame);
	set_debug_img_path(frame, &record_buf->face_record.img, filename);
	record_buf->cb = 0;
	record_buf->save = 1;
	record_buf->face = 0;
	record_buf->face_record.time =frame->time_stamp;
	record_buf->face_record.type = debug_frame;

	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

/*
 * should lock
 */
void save_frame(face_svc_t *svc, face_frame_t *frame, record_img_e type)
{
	face_record_buf_t *record_buf;

	if (!svc || !frame)
		return;

	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	record_buf->frame = frame;
	retain_frame(record_buf->frame);
	set_img_path(frame, &record_buf->face_record.img, type);
	record_buf->cb = 0;
	record_buf->save = 1;
	record_buf->face = 0;
	record_buf->face_record.time =frame->time_stamp;
	record_buf->face_record.type = type;

	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

/*
 * should lock
 */
void save_record(face_svc_t *svc, tracking_face_t *face,
	rgb_recg_buf_t *recg_buf, record_img_e type)
{
	face_frame_t *frame;
	face_info_t *info;
	face_record_buf_t *record_buf;

	if (!svc || !recg_buf)
		return;

	frame = get_record_frame(type, recg_buf);
	info = &recg_buf->face_info;

	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	record_buf->frame = frame;
	retain_frame(record_buf->frame);
	set_img_path(frame, &face->img, type);
	record_buf->cb = 1;
	record_buf->save = 1;
	record_buf->face = 1;
	record_buf->face_record.time = face->time;
	memcpy(&record_buf->face_record.face_info, info, sizeof(face_info_t));
	memcpy(&record_buf->face_record.img, &face->img, sizeof(record_img_t));
	record_buf->face_record.type = type;
	record_buf->face_record.recg = face->recg;
	record_buf->face_record.living = face->living;
	record_buf->face_record.timeout = face->timeout;
	record_buf->face_record.add_timestamp = face->add_timestamp;
	record_buf->face_record.recg_timestamp = face->recg_timestamp;
	record_buf->face_record.living_timestamp = face->living_timestamp;

	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

/*
 * should lock
 * save when recg face is not rect attr pose is unqualified
 */
void save_record_unqualified(face_svc_t *svc, tracking_face_t *face,
	face_frame_t *frame, face_rect_t *p_rect, record_img_e type)
{
	face_record_buf_t *record_buf;

	if (!svc || !face || !p_rect) {
		return;
	}
	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	record_buf->frame = frame;
	retain_frame(record_buf->frame);
	set_img_path(frame, &face->img, type);
	if (face_svc_conf->svc.dump_unqualified_face && type == unqualified_frame) {
		set_img_face_path(frame, face->id, &face->img, unqualified_face);
	}

	record_buf->cb = 1;
	record_buf->save = 1;

	record_buf->face = face->id;
	memcpy(&record_buf->face_record.face_info.rect, p_rect, sizeof(face_rect_t));
	memcpy(&record_buf->face_record.img, &face->img, sizeof(record_img_t));
	record_buf->face_record.type = type;
	record_buf->face_record.face_pass = face->face_pass;
	record_buf->face_record.face_refuse = face->face_refuse;
	record_buf->face_record.time = face->time;
	record_buf->face_record.recg = face->recg;
	record_buf->face_record.living = face->living;
	record_buf->face_record.timeout = face->timeout;
	record_buf->face_record.add_timestamp = face->add_timestamp;
	record_buf->face_record.recg_timestamp = face->recg_timestamp;
	record_buf->face_record.living_timestamp = face->living_timestamp;

	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

/*
 * should lock
 */
void save_record_face(face_svc_t *svc, tracking_face_t *face,
	int left, int top, int right, int buttom, rgb_recg_buf_t *recg_buf,
	record_img_e type)
{
	face_frame_t *frame;
	face_info_t *info;
	face_record_buf_t *record_buf;

	if (!svc || !recg_buf)
		return;

	frame = get_record_frame(type, recg_buf);
	info = &recg_buf->face_info;

	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	record_buf->frame = frame;
	retain_frame(record_buf->frame);
	set_img_path(frame, &face->img, type);
	record_buf->cb = 1;
	record_buf->save = 1;
	record_buf->face = 1;
	record_buf->face_record.time = face->time;
	memcpy(&record_buf->face_record.face_info, info, sizeof(face_info_t));
	memcpy(&record_buf->face_record.img, &face->img, sizeof(record_img_t));
	record_buf->face_record.face_info.rect.rect[0] = left;
	record_buf->face_record.face_info.rect.rect[1] = top;
	record_buf->face_record.face_info.rect.rect[2] = right;
	record_buf->face_record.face_info.rect.rect[3] = buttom;
	record_buf->face_record.type = type;
	record_buf->face_record.add_timestamp = face->add_timestamp;
	record_buf->face_record.recg_timestamp = face->recg_timestamp;
	record_buf->face_record.living_timestamp = face->living_timestamp;


	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

/*
 * should lock
 */
void save_record_recg_det(face_svc_t *svc, tracking_face_t *face,
	rgb_recg_buf_t *recg_buf, record_img_e type)
{
	face_frame_t *frame;
	face_info_t *info;
	face_record_buf_t *record_buf;

	if (!svc || !face || !recg_buf)
		return;

	frame = get_record_frame(type, recg_buf);
	info = &recg_buf->face_info;

	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	record_buf->is_already_record = face->is_already_record;

	record_buf->frame = frame;
	retain_frame(record_buf->frame);
	set_img_path(frame, &face->img, type);
	if (face_svc_conf->svc.dump_match_face && type == match_frame) {
		set_img_face_path(frame, face->id, &face->img, match_face);
	}
	if (face_svc_conf->svc.dump_unmatch_face && type == unmatch_frame) {
		set_img_face_path(frame, face->id, &face->img, unmatch_face);
	}

	if (face->recg == 1 && face->living == 1) {
		record_buf->cb = 1;
		record_buf->save = 1;
		record_buf->delete = 0;
		face->is_already_record = 1;
	}

	if (face->recg == 1 && face->living == 0 && face_svc_conf->svc.living_check) {
		record_buf->cb = 0;
		record_buf->save = 1;
		record_buf->delete = 0;
	} else if (face->recg == 1) {
		record_buf->cb = 1;
		record_buf->save = 1;
		record_buf->delete = 0;
		face->is_already_record = 1;
	}

	if (face->recg == 0 && face->timeout == 1) {
		record_buf->cb = 1;
		record_buf->save = 1;
		record_buf->delete = 0;
		face->is_already_record = 1;
	}

	record_buf->face = face->id;
	memcpy(&record_buf->face_record.face_info, info, sizeof(face_info_t));
	memcpy(&record_buf->face_record.img, &face->img, sizeof(record_img_t));
	record_buf->face_record.type = type;
	record_buf->face_record.face_pass = face->face_pass;
	record_buf->face_record.face_refuse = face->face_refuse;
	record_buf->face_record.time = face->time;
	record_buf->face_record.recg = face->recg;
	record_buf->face_record.living = face->living;
	record_buf->face_record.timeout = face->timeout;
	record_buf->face_record.add_timestamp = face->add_timestamp;
	record_buf->face_record.recg_timestamp = face->recg_timestamp;
	record_buf->face_record.living_timestamp = face->living_timestamp;


	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

/*
 * should lock
 */
void save_record_living_det(face_svc_t *svc, tracking_face_t *face,
	rgb_recg_buf_t *recg_buf, record_img_e type)
{
	face_frame_t *frame;
	face_info_t *info;
	face_record_buf_t *record_buf;

	if (!svc || !face || !recg_buf)
		return;

	frame = get_record_frame(type, recg_buf);
	info = &recg_buf->face_info;

	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	record_buf->is_already_record = face->is_already_record;

	record_buf->frame = frame;
	retain_frame(record_buf->frame);
	if (face->recg == 1 && face->living == 1) {
		record_buf->cb = 1;
		record_buf->save = 0;
		record_buf->delete = 0;
		face->is_already_record = 1;
	}

	record_buf->face = face->id;
	memcpy(&record_buf->face_record.face_info, info, sizeof(face_info_t));
	memcpy(&record_buf->face_record.img, &face->img, sizeof(record_img_t));
	record_buf->face_record.type = type;
	record_buf->face_record.face_pass = face->face_pass;
	record_buf->face_record.face_refuse = 0;
	record_buf->face_record.time = face->time;
	record_buf->face_record.recg = face->recg;
	record_buf->face_record.living = face->living;
	record_buf->face_record.timeout = face->timeout;
	record_buf->face_record.add_timestamp = face->add_timestamp;
	record_buf->face_record.recg_timestamp = face->recg_timestamp;
	record_buf->face_record.living_timestamp = face->living_timestamp;


	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

/*
 * should lock
 */
void save_record_living_timeout(face_svc_t *svc, tracking_face_t *face,
	rgb_recg_buf_t *recg_buf, record_img_e type)
{
	face_frame_t *frame;
	face_record_buf_t *record_buf;
	if (!svc || !face || !recg_buf)
		return;

	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	record_buf->is_already_record = face->is_already_record;

	/*
	* face timeout
	* save recg face->img.patc[match_frame/face] img as refuse record
	*/
	if (face->timeout == 1 && face->recg == 1 && face->living == 0) {
		//face->face_pass = 0;
		//face->face_refuse = 1;
		record_buf->cb = 1;
		record_buf->save = 0;
		record_buf->delete = 0;
		face->is_already_record = 1;
	}

	if (face->timeout == 1 && face->recg == 0 && face->living == 0) {
		frame = get_record_frame(unmatch_frame, recg_buf);

		record_buf->frame = frame;
		retain_frame(record_buf->frame);

		set_img_path(frame, &face->img, unmatch_frame);
		if (face_svc_conf->svc.dump_unmatch_face && type == unmatch_frame) {
			set_img_face_path(frame, face->id, &face->img, unmatch_face);
		}

		//face->face_pass = 0;
		//face->face_refuse = 1;
		record_buf->cb = 1;
		record_buf->save = 1;
		record_buf->delete = 0;
		face->is_already_record = 1;
	}

	record_buf->face = face->id;
	memcpy(&record_buf->face_record.face_info.rect, &face->rect, sizeof(face_rect_t));
	memcpy(&record_buf->face_record.face_info.feat, &face->feat, sizeof(face_feat_t));
	memcpy(&record_buf->face_record.img, &face->img, sizeof(record_img_t));
	record_buf->face_record.type = type;

	record_buf->face_record.face_pass = face->face_pass;
	record_buf->face_record.face_refuse = face->face_refuse;
	record_buf->face_record.time = face->time;
	record_buf->face_record.recg = face->recg;
	record_buf->face_record.living = face->living;
	record_buf->face_record.timeout = face->timeout;
	record_buf->face_record.add_timestamp = face->add_timestamp;
	record_buf->face_record.recg_timestamp = face->recg_timestamp;
	record_buf->face_record.living_timestamp = face->living_timestamp;



	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

/*
 * should lock
 * this time frame is NULL
 */
void save_record_face_miss(face_svc_t *svc, tracking_face_t *face,
	record_img_e type)
{
	face_record_buf_t *record_buf;

	if (!svc || !face)
		return;

	record_buf = malloc(sizeof(face_record_buf_t));
	if (!record_buf) {
		ALOGE("%s malloc face_record_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return;
	}
	memset(record_buf, 0, sizeof(face_record_buf_t));

	/*
	* face recg but it is timeout need to
	* remove recg face->img.patc[match_frame/face] img
	*/
	if (face->timeout == 0 && face->recg == 1 && face->living == 0) {
//		face->face_pass = 0;
//		face->face_refuse = 0;
		record_buf->cb = 0;
		record_buf->save = 0;
		record_buf->delete = 1;
	}
	ALOGE("%s happen", __func__);

	record_buf->face = face->id;
	memcpy(&record_buf->face_record.face_info.rect, &face->rect, sizeof(face_rect_t));
	memcpy(&record_buf->face_record.face_info.feat, &face->feat, sizeof(face_feat_t));
	memcpy(&record_buf->face_record.img, &face->img, sizeof(record_img_t));
	record_buf->face_record.type = type;
	record_buf->face_record.face_pass = face->face_pass;
	record_buf->face_record.face_refuse = face->face_refuse;
	record_buf->face_record.time = face->time;
	record_buf->face_record.recg = face->recg;
	record_buf->face_record.living = face->living;
	record_buf->face_record.timeout = face->timeout;

	pthread_mutex_lock(&svc->record_lock);
	en_queue(&svc->record_queue, record_buf);
	pthread_cond_signal(&svc->record_cond);
	pthread_mutex_unlock(&svc->record_lock);
}

static void *face_record_thread(void *arg)
{
	face_svc_t *svc = (face_svc_t *)arg;
	face_record_buf_t *record_buf;
	face_frame_t *frame;
	face_record_t *record;
	face_info_t *info;
	int is_already_record = 0;
	prctl(PR_SET_NAME, "face-record");

	while (1) {
		pthread_mutex_lock(&svc->record_lock);
		if (empty_queue(&svc->record_queue))
			pthread_cond_wait(&svc->record_cond, &svc->record_lock);
		record_buf = de_queue(&svc->record_queue);
		pthread_mutex_unlock(&svc->record_lock);

		if (record_buf == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}

		/* record_buf->frame sometime is NULL */
		frame = record_buf->frame;
		record = &record_buf->face_record;



		is_already_record = record_buf->is_already_record;



		if ((0 == is_already_record) && record_buf->save && frame && face_svc_conf->svc.dump_save_img) {
			info = &record->face_info;
			ALOGD("%s dump_save_img record_buf=%p", __func__, record_buf);
			if (face_svc_conf->svc.dump_match_face && record->type == match_frame) {
				dump_ID_photo(record->img.path[match_face], frame, &info->rect,
					face_svc_conf->svc.rgb_x_margin, face_svc_conf->svc.rgb_y_margin);
			} else if (face_svc_conf->svc.dump_unmatch_face && record->type == unmatch_frame) {
				dump_ID_photo(record->img.path[unmatch_face], frame, &info->rect,
					face_svc_conf->svc.rgb_x_margin, face_svc_conf->svc.rgb_y_margin);
			} else if (face_svc_conf->svc.dump_debug_unmatch_face && record->type == debug_unmatch_face) {
				dump_face(record->img.path[debug_unmatch_face], frame, &info->rect,
					face_svc_conf->svc.rgb_x_margin, face_svc_conf->svc.rgb_y_margin);
			} else if (face_svc_conf->svc.dump_unqualified_face && record->type == unqualified_frame) {
				dump_ID_photo(record->img.path[unqualified_face], frame, &info->rect,
					face_svc_conf->svc.rgb_x_margin, face_svc_conf->svc.rgb_y_margin);
			} else if (face_svc_conf->svc.dump_debug_rgb_det_face && record->type == debug_rgb_det_face) {
				dump_face(record->img.path[debug_rgb_det_face], frame, &info->rect,
					face_svc_conf->svc.rgb_x_margin, face_svc_conf->svc.rgb_y_margin);
			} else if (face_svc_conf->svc.dump_debug_nir_det_face && record->type == debug_nir_det_face) {
				dump_face(record->img.path[debug_nir_det_face], frame, &info->rect,
					0, 0);
			}

			if (record->type == debug_frame) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_match_frame && record->type == match_frame) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_unmatch_frame && record->type == unmatch_frame) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_unqualified_frame && record->type == unqualified_frame) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_debug_unmatch_frame && record->type == debug_unmatch_frame) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_debug_nir_living && record->type == debug_nir_living) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_debug_nir_living && record->type == debug_nir_no_living) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_debug_nir_no_face && record->type == debug_nir_no_face) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_debug_rgb_living && record->type == debug_rgb_living ) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_debug_rgb_living && record->type == debug_rgb_no_living ) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_debug_rgb_color_living && record->type == debug_rgb_color_living ) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_debug_rgb_yolo_living && record->type == debug_rgb_yolo_living ) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_living && record->type == living) {
				dump_frame(record->img.path[record->type], frame);
			} else if (face_svc_conf->svc.dump_no_faces && record->type == no_face) {
				dump_frame(record->img.path[record->type], frame);
			}
		}

		if ((0 == is_already_record) && record_buf->cb && svc->record_cb) {
			ALOGD("[%s]: face:%d type:%d(%s) face_pass:%d face_refuse:%d time:%lld recg:%d living:%d timeout:%d save=%d delete=%d\n",
				__FUNCTION__,
				record_buf->face,
				record_buf->face_record.type,
				set_img_path_prefix(record_buf->face_record.type),
				record_buf->face_record.face_pass,
				record_buf->face_record.face_refuse,
				record_buf->face_record.time,
				record_buf->face_record.recg,
				record_buf->face_record.living,
				record_buf->face_record.timeout,
				record_buf->save,
				record_buf->delete);


			if ((face_svc_conf->svc.dump_match_frame ||
				face_svc_conf->svc.dump_match_face) && record->type == match_frame) {
				svc->record_cb(NULL, record);
			} else if ((face_svc_conf->svc.dump_unmatch_frame ||
				face_svc_conf->svc.dump_unmatch_face) && record->type == unmatch_frame) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_unqualified_frame && record->type == unqualified_frame) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_debug_unmatch_frame && record->type == debug_unmatch_frame) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_debug_unmatch_face && record->type == debug_unmatch_face) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_debug_nir_living && record->type == debug_nir_living) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_debug_nir_living && record->type == debug_nir_no_living) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_debug_rgb_living && record->type == debug_rgb_living ) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_debug_rgb_living && record->type == debug_rgb_no_living ) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_debug_rgb_color_living && record->type == debug_rgb_color_living ) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_debug_rgb_yolo_living && record->type == debug_rgb_yolo_living ) {
				svc->record_cb(NULL, record);
			} else if (face_svc_conf->svc.dump_living && record->type == living) {
				svc->record_cb(NULL, record);
			}
		}

		if (record_buf->delete) {
			ALOGD("%s delete  record_buf=%p", __func__, record_buf);
			if (face_svc_conf->svc.dump_match_face)
				file_remove(record->img.path[match_face]);
			if (face_svc_conf->svc.dump_match_frame)
				file_remove(record->img.path[match_frame]);
		}

		pthread_mutex_lock(&svc->tracking_lock);
		if(frame)
			free_frame(&frame);
		pthread_mutex_unlock(&svc->tracking_lock);

		free(record_buf);
	}

	return NULL;
}

void free_rgb_recg_buf_no_cb(face_svc_t *svc, rgb_recg_buf_t **pp)
{
	rgb_recg_buf_t *recg_buf = *pp;

	if (recg_buf) {
		if (--recg_buf->ref == 0) {
			free(recg_buf);
			recg_buf = NULL;
		} else if (recg_buf->ref < 0) {
			ALOGE("%s recg_buf memory has error", __func__);
			dump_stack();
		}
	}
}

void free_rgb_recg_buf(face_svc_t *svc, rgb_recg_buf_t **pp)
{
	rgb_recg_buf_t *recg_buf = *pp;
	face_frame_t *frame;
	face_info_t *face_info;
	tracking_face_t *face;

	if (recg_buf) {
		frame = recg_buf->frame;
		face_info = &recg_buf->face_info;

		ALOGD_IF(1, "frame: %p face[%d] score: %f, living_body: %d, feat_version: %d, recg_buf ref: %d", frame, face_info->rect.id, face_info->feat.score, face_info->feat.living_body, face_info->feat.feat_version, recg_buf->ref);

		if (--recg_buf->ref == 0) {
			face = tracking_get(svc, face_info->rect.id);
			if (face) {
				/*
				 * memcpy(&recg_buf->face_info.feat, &face->feat, sizeof(face_feat_t));
				 *
				 * fix multi det ploy issue , face recg and living had different recg_buf
				 * face->living = 1 faster than face->recg = 1, recg_buf can not correct callback face feat info
				 * face->recg = 1 faster than face->living = 1, recg_buf can not correct callback face feat info
				 * face->living = 1 faster than face->recg = 0, ALOGD face score and feat_version are zero
				 */
				memcpy(&recg_buf->face_info.feat, &face->feat, sizeof(face_feat_t));
				ALOGD_IF(1, "frame: %p face[%d] score: %f, living_body: %d, review: %d, feat_version: %d, recg_buf ref: %d",
					frame, face_info->rect.id, face_info->feat.score, face_info->feat.living_body, face_info->feat.review, face_info->feat.feat_version, recg_buf->ref);
				if (svc->recg_cb)
					svc->recg_cb(frame, face_info);
			}
			free(recg_buf);
			recg_buf = NULL;
		} else if (recg_buf->ref < 0) {
			dump_stack();
		}
	}
}

void set_det_timeout_2nd_record (tracking_face_t *face)
{
	if (face_svc_conf->svc.det_timeout > 0) {
		if (face_svc_conf->svc.det_timeout_2nd_record > 0 && face->timeout == 1) {
			long long cur_time = get_systime_us();
			if (cur_time - face->timeout_time >= face_svc_conf->svc.det_timeout_2nd_record * 1000000) {
				face->timeout = 0;
//				face->face_refuse = 0;
				face->timeout_time = cur_time + face_svc_conf->svc.det_timeout * 1000000;
			}
		}
	}
}

void tracking_update(face_svc_t *svc, nna_faces_t *rects)
{
	list_head_t *list;
	tracking_face_t *face, *tmp;
	int i;
	nna_face_t *face_rect;

	if (!svc)
		return;

	list = &svc->tracking_list;

	/* Cleanup */
	list_for_each_entry_safe(face, tmp, list, node) {
		for (i = 0; i < rects->num; i++) {
			face_rect = &rects->face_rects[i];
			if (face_rect->id == face->id)
				break;
		}

		if (i >= rects->num) {
			ALOGD_IF(face_svc_conf->debug.debug_log, "%s delete face[%d]", __func__,
				face->id);
			if (face_svc_conf->svc.living_check) {
				save_record_face_miss(svc, face, match_frame);
			}
			list_del(&face->node);
			/* free face related frame */
			if (face->frame)
				free_frame(&face->frame);
			if (face->nir_frame)
				free_frame(&face->nir_frame);
			free(face);
		}
	}

	/* Adding new face */
	for (i = 0; i < rects->num; i++) {
		face_rect = &rects->face_rects[i];

		tmp = NULL;
		list_for_each_entry(face, list, node) {
			if (face->id == face_rect->id) {
				face->occur++;
				set_det_timeout_2nd_record(face);
				tmp = face;
				break;
			}
		}

		if (!tmp) {
			face = malloc(sizeof(tracking_face_t));
			if (!face) {
				ALOGE("%s malloc tracking_face_t failed, errno: %d (%s)",
					__func__, errno, strerror(errno));
				return;
			}
			memset(face, 0, sizeof(tracking_face_t));

			ALOGD_IF(face_svc_conf->debug.debug_log, "%s add face[%d]", __func__, face_rect->id);

			face->id = face_rect->id;
			face->occur = 1;
			face->quality_debonce = 1;
			face->fake_yolo_score_acc = 0;
			face->living_score_acc = 0;
			face->add_timestamp = get_systime_us();
			if (face_svc_conf->svc.det_timeout > 0) {
				face->timeout_time = face->add_timestamp + face_svc_conf->svc.det_timeout * 1000;
			}
			if (face_svc_conf->svc.recg_repeat){
				//face->repeat_detect_timeout_time = face->add_timestamp + face_svc_conf->svc.recg_repeat_interval * 1000;
				int actual = 0;
				actual = (face_svc_conf->svc.recg_repeat_interval > (face_svc_conf->svc.det_timeout))?
						face_svc_conf->svc.recg_repeat_interval:face_svc_conf->svc.det_timeout;
				actual += FACE_REPEAT_RECG_TIMEOUT_OFFSET;
				face->repeat_detect_timeout_time = face->add_timestamp + actual*1000;

			}

			list_add(&face->node, list);
		}
	}
}

tracking_face_t *tracking_get(face_svc_t *svc, int id)
{
	list_head_t *list;
	tracking_face_t *face, *tmp = NULL;

	if (!svc)
		return NULL;

	list = &svc->tracking_list;

	list_for_each_entry(face, list, node) {
		if (face->id == id) {
			tmp = face;
			break;
		}
	}

	return tmp;
}

void tracking_clean(face_svc_t *svc)
{
	list_head_t *list;
	tracking_face_t *face, *tmp;

	if (!svc)
		return;

	list = &svc->tracking_list;

	/* Cleanup */
	list_for_each_entry_safe(face, tmp, list, node) {
		ALOGD_IF(face_svc_conf->debug.debug_log, "%s delete face[%d]", __func__,
			face->id);
		if (face_svc_conf->svc.living_check) {
			save_record_face_miss(svc, face, match_frame);
		}
		list_del(&face->node);
		/* free face related frame */
		if (face->frame)
			free_frame(&face->frame);
		if (face->nir_frame)
			free_frame(&face->nir_frame);
		free(face);
	}
}

/* set face timeout time */
void set_face_timeout_time(long long *time)
{
	*time = get_systime_us() + face_svc_conf->svc.det_timeout*1000;
}

/* when person not enter recg rect need reset face timeout_time */
void tracking_reset_recg_timeout(face_svc_t *svc, int id)
{
	list_head_t *list;
	tracking_face_t *face;

	if (!svc)
		return;

	list = &svc->tracking_list;

	list_for_each_entry(face, list, node) {
		if (face_svc_conf->svc.det_timeout > 0) {
			if (face->id == id) {
				set_face_timeout_time(&face->timeout_time);
				ALOGD("rest timeout_time to %lld", face->timeout_time);
				break;
			}
		}
	}
}

void tracking_check_living(face_svc_t *svc, face_info_t *face_info, tracking_face_t *face)
{
	float score, threshold;
	score = face_compare2(face->feat.values, face->feat.living_values);

	if (face_info->rect.attr.mask == 1 && face_svc_conf->svc.mask_recg_enable) {
		svc->face_feat->feature_part = 1;
		threshold = face_svc_conf->svc.mask_score_threshold;
	} else {
		svc->face_feat->feature_part = 0;
		threshold = face_svc_conf->svc.score_threshold;
	}

	if (score > threshold) {
		face->living = 1;
		face->feat.living_body = 1;
		face->feat.review = 1;
	} else {
		ALOGD_IF(face_svc_conf->debug.debug_log,
			"%s face[%d] score: %f", __func__, face->id, score);
		pthread_mutex_lock(&g_svc->pause_lock);
		g_svc->redet = 1;
		pthread_mutex_unlock(&g_svc->pause_lock);
		face->feat.living_body = 0;
		face->feat.score = 0;
	}
}

void tracking_set_recg(tracking_face_t *face, face_feat_t *feat)
{
	if (!face || !feat)
		return;

	face->recg = 1;
	memcpy(face->feat.values, feat->values,
		sizeof(signed char) * FACE_FEAT_SIZE);
	face->feat.score = feat->score;
	face->feat.max_id = feat->max_id;
	face->feat.cmp_conf = feat->cmp_conf;
	face->feat.feat_version = feat->feat_version;
	face->recg_timestamp = get_systime_us();
}

void tracking_set_living(tracking_face_t *face, face_feat_t *feat)
{
	if (!face || !feat)
		return;

	//face->living = 1;
	face->feat.living_body = feat->living_body;
	face->living_timestamp = get_systime_us();
}

int check_recg_permit(face_svc_t *svc, tracking_face_t *face, face_frame_t *frame)
{
	if (!svc || !face || !frame)
		return 0;

	if (face_svc_conf->svc.max_track_faces == 1)
		return 1;

	if (frame->recg_check_cnt >= face_svc_conf->svc.frame_recg_max)
		return 0;

	if (frame->recg_check_cnt == face_svc_conf->svc.frame_recg_max - 1) {
		if (face->recg_check_count < 2) {
			if (face->max_no_recg_face == 1) {
				ALOGD("%s max_no_recg_face [%d]", __func__,face->id);
				return 1;
			} else {
				ALOGD("%s is not max_no_recg_face [%d]", __func__,face->id);
			}
		}
	}

	if (face->quality_debonce >= face_svc_conf->svc.quality_debonce)
		return 1;

	return 0;
}

int check_living_permit(face_svc_t *svc, tracking_face_t *face, face_frame_t *frame)
{
	if (!svc || !face || !frame)
		return 0;

	if (face_svc_conf->svc.max_track_faces == 1)
		return 1;

	if (frame->living_check_cnt >= face_svc_conf->svc.frame_living_max)
		return 0;

	if (frame->living_check_cnt == face_svc_conf->svc.frame_living_max - 1) {
		if (face->living_check_count < 2) {
			if (face->max_no_living_face == 1) {
				ALOGD("%s max_no_living_face [%d]", __func__,face->id);
				return 1;
			} else {
				ALOGD("%s is not max_no_living_face [%d]", __func__,face->id);
			}
		}
	}

	if (frame->living_check_cnt < face_svc_conf->svc.frame_living_max - 1)
		return 1;

	return 0;
}

#if 0
int check_recg_permit(face_svc_t *svc, tracking_face_t *face)
{
	list_head_t *list = NULL;
	tracking_face_t *tmp = NULL;

	if (!svc || !face)
		return 0;

	int recg_min = face->recg_check_count;

	list = &svc->tracking_list;

	list_for_each_entry(tmp, list, node) {
		if (!tmp->recg && tmp->recg_check_count < recg_min) {
			recg_min = tmp->recg_check_count;
		}
	}

	return (face->recg_check_count == recg_min);
}

int check_living_permit(face_svc_t *svc, tracking_face_t *face)
{
	list_head_t *list = NULL;
	tracking_face_t *tmp = NULL;

	if (!svc || !face)
		return 0;

	int living_min = face->living_check_count;

	list = &svc->tracking_list;

	list_for_each_entry(tmp, list, node) {
		if (!tmp->living && tmp->living_check_count < living_min) {
			living_min = tmp->living_check_count;
		}
	}

	return (face->living_check_count == living_min);
}
#endif

static face_frame_t *find_near_nir_frame(face_svc_t *svc, face_frame_t *rgb_frame)
{
	face_frame_t *tmp_frame = NULL, *nir_frame = NULL;
	long long duration;

	pthread_mutex_lock(&svc->nir_frame_lock);
	while (!empty_queue(&svc->nir_frame_queue)) {
		/* many frames in queue */
		if (tmp_frame) {
			face_media_return_frame(tmp_frame);
			nir_frame = NULL;
		}

		tmp_frame = (face_frame_t *)de_queue(&svc->nir_frame_queue);
		duration = tmp_frame->time_stamp - rgb_frame->time_stamp;
		ALOGD_IF(0, "nir ts: %lld, rgb ts: %lld, duration: %lld",
			tmp_frame->time_stamp, rgb_frame->time_stamp, duration);
		if (llabs(duration) > DEFAULT_CAMERAS_TS_DIFF) {
			face_media_return_frame(tmp_frame);
			tmp_frame = NULL;
		} else {
			nir_frame = tmp_frame;
		}
	}
	pthread_mutex_unlock(&svc->nir_frame_lock);

	return nir_frame;
}

static void *face_det_thread(void *arg)
{
	face_svc_t *svc = (face_svc_t *)arg;
	face_frame_t *frame;
	face_frame_t *nir_frame;
	rgb_det_buf_t *det_buf;
	nna_feature_map_t *map;
	face_rects_t cb_face_rects;
	face_info_t cb_face_info;

	ALOGV("enter %s\n", __func__);

	prctl(PR_SET_NAME, "face-det");

	while (1) {
		pthread_mutex_lock(&svc->rgb_frame_lock);
		if (empty_queue(&svc->rgb_frame_queue))
			pthread_cond_wait(&svc->rgb_frame_cond, &svc->rgb_frame_lock);
		frame = (face_frame_t *)de_queue(&svc->rgb_frame_queue);
		pthread_mutex_unlock(&svc->rgb_frame_lock);

		if (frame == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}

		nir_frame = NULL;
		if (frame->type == FACE_FRAME_TYPE_RGB) {
			if (face_svc_conf->svc.living_check) {
				if (!face_svc_conf->svc.living_check_mode) {
					nir_frame = find_near_nir_frame(svc, frame);
					if (nir_frame){
						nir_frame->ref = 1;
						//retain_frame(nir_frame);
					}
					else
						ALOGI("%s near nir frame not found", __func__);
				}
				/* else rgb living_check*/
			}


			retain_frame(frame);
			pthread_mutex_lock(&svc->zbar_lock);
			if (svc->zbar_mode == 1) {
				zbar_det_frame(frame);
				free_frame(&frame);
				if (nir_frame) {
					free_frame(&nir_frame);
					nir_frame = NULL;
				}
				pthread_mutex_unlock(&svc->zbar_lock);
				continue;
			} else if (svc->zbar_mode == 2) {
				zbar_async_det_frame(frame);
			}
			pthread_mutex_unlock(&svc->zbar_lock);

			pthread_mutex_lock(&svc->face_capture_lock);
			if (svc->capture) {
				frame->type = FACE_FRAME_TYPE_API_CAPTURE_BLOCK;
				if (nir_frame) {
					free_frame(&nir_frame);
					nir_frame = NULL;
				}
			}
			pthread_mutex_unlock(&svc->face_capture_lock);

			pthread_mutex_lock(&svc->rgb_det_lock);
			if (queue_size(&svc->rgb_det_queue) >= face_svc_conf->svc.nn_det_queue_size) {
				ALOGI("%s nn detect queue is full", __func__);
				free_frame(&frame);
				if (nir_frame)
					free_frame(&nir_frame);
				pthread_mutex_unlock(&svc->rgb_det_lock);
				continue;
			}
			pthread_mutex_unlock(&svc->rgb_det_lock);


		}

		det_buf = malloc(sizeof(rgb_det_buf_t));
		if (!det_buf) {
			ALOGE("%s malloc rgb_det_buf_t failed, errno: %d (%s)",
				__func__, errno, strerror(errno));
			goto free_res;
		}

		det_buf->frame = frame;
		det_buf->nir_frame = nir_frame;

		ALOGD_IF(0, "~~~ frame in %p, type: %s", frame, strtype(frame->type));

		map = &det_buf->map;
		map->fmt = YUV420;
		if (frame->type == FACE_FRAME_TYPE_RGB) {
			pthread_mutex_lock(&svc->pause_lock);
			if (svc->pause)
				map->img_type = NNA_VIDEO_EMPTY;
			else
				map->img_type = NNA_VIDEO;

			if (svc->redet) {
				svc->redet = 0;
				ALOGD("NNA_VIDEO_EMPTY");
				map->img_type = NNA_VIDEO_EMPTY;
			}

			pthread_mutex_unlock(&svc->pause_lock);
		} else
			map->img_type = NNA_IMAGE;
		map->width = face_svc_conf->svc.rgb_buffer_width;
		map->height = face_svc_conf->svc.rgb_buffer_height;
		map->channal = 3;
		map->stride1 = map->width;
		map->stride2 = map->width;
		map->buf_hdl.size = frame->size[0] + frame->size[1];
		map->buf_hdl.phyaddr = (unsigned long)frame->phyaddr[0]
			+ frame->width * (frame->height - map->height) / 2;
		map->buf_hdl.viraddr = (unsigned long)frame->viraddr[0]
			+ frame->width * (frame->height - map->height) / 2;
		map->buf_hdl.uv_phyaddr = (unsigned long)frame->phyaddr[1]
			+ frame->width * (frame->height - map->height) / 4;
		map->buf_hdl.uv_viraddr = (unsigned long)frame->viraddr[1]
			+ frame->width * (frame->height - map->height) / 4;

		pthread_mutex_lock(&svc->rgb_det_lock);
		en_queue(&svc->rgb_det_queue, det_buf);
		pthread_mutex_unlock(&svc->rgb_det_lock);
		frame->det_begin = get_systime_us();
		if (nn_face_det(map)) {
			ALOGE("%s nn_face_det failed", __func__);
			goto free_res;
		} else
			continue;

free_res:
		if (frame->type == FACE_FRAME_TYPE_RGB && svc->det_cb) {
			memset(&cb_face_rects, 0, sizeof(face_rects_t));
			svc->det_cb(frame, &cb_face_rects);
			free_frame(&frame);
			if (nir_frame)
				free_frame(&nir_frame);
		} else if (frame->type == FACE_FRAME_TYPE_API_DET &&
				svc->det_cb) {
			memset(&cb_face_rects, 0, sizeof(face_rects_t));
			svc->det_cb(frame, &cb_face_rects);
		} else if (frame->type == FACE_FRAME_TYPE_API_RECG &&
				svc->recg_cb) {
			memset(&cb_face_info, 0, sizeof(face_info_t));
			cb_face_info.error = FACE_SVC_ERR;
			svc->recg_cb(frame, &cb_face_info);
		} else if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK) {
			/* wake up block thread even if no face is detected */
			pthread_mutex_lock(&svc->face_lock);
			svc->face_ret = FACE_SVC_ERR;
			pthread_cond_signal(&svc->face_cond);
			pthread_mutex_unlock(&svc->face_lock);
		} else if (frame->type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK) {
			pthread_mutex_lock(&svc->face_capture_lock);
			free_frame(&frame);
			pthread_mutex_unlock(&svc->face_capture_lock);
		}

		if (det_buf) {
			free(det_buf);
			det_buf  = NULL;
		}
	}

	return NULL;
}

static void *face_rgb_mirror_dist_thread(void *arg)
{
	face_svc_t *svc = (face_svc_t *)arg;
	face_frame_t *frame;

	ALOGV("enter %s\n", __func__);

	prctl(PR_SET_NAME, "face-det");

	while (1) {
		pthread_mutex_lock(&svc->rgb_mirror_frame_lock);
		if (empty_queue(&svc->rgb_mirror_frame_queue))
			pthread_cond_wait(&svc->rgb_mirror_frame_cond, &svc->rgb_mirror_frame_lock);
		frame = (face_frame_t *)de_queue(&svc->rgb_mirror_frame_queue);
		pthread_mutex_unlock(&svc->rgb_mirror_frame_lock);

		if (frame == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}

		if (frame->type == FACE_FRAME_TYPE_RGB_MIRROR) {
			frame->ref++;
			goto free_res;
		}

free_res:
		if (frame->type == FACE_FRAME_TYPE_RGB_MIRROR) {
			free_frame(&frame);
		}
	}
	return NULL;
}

#define TOP_BLACK_ADJUST_LOW 0.55
#define TOP_BLACK_ADJUST_HIGH 2.5
#define HALF_FACE_ADJUST_LOW 0.35
#define HALF_FACE_ADJUST_HIGH 0.70
#define SCORE_ADJUST_LOW 0.01
#define SCORE_ADJUST_HIGH 0.03

float caculate_score_correct(float top_black, float half_face)
{
	float top_score_correct = 0.0;
	float total_score_correct = 0.0;
	float top_black_delta = TOP_BLACK_ADJUST_HIGH - TOP_BLACK_ADJUST_LOW;
	float half_face_delta = HALF_FACE_ADJUST_HIGH - HALF_FACE_ADJUST_LOW;
	float score_delat = SCORE_ADJUST_HIGH - SCORE_ADJUST_LOW;

	if (top_black < TOP_BLACK_ADJUST_LOW)
		top_score_correct = 0.0;
	else
		top_score_correct = SCORE_ADJUST_LOW + (top_black - TOP_BLACK_ADJUST_LOW) * score_delat / top_black_delta;

	if (half_face < HALF_FACE_ADJUST_LOW)
		half_face_delta = 0.0;
	else
		half_face_delta = SCORE_ADJUST_LOW + (half_face - HALF_FACE_ADJUST_LOW) * score_delat / half_face_delta;

	total_score_correct = half_face_delta + top_score_correct;

	return total_score_correct < SCORE_ADJUST_HIGH ?
		total_score_correct : SCORE_ADJUST_HIGH;
}

/* for living check use */
static int face_det_living_rgb_feats(void *handle, tracking_face_t *face, face_frame_t *frame)
{
	face_svc_t *svc = (face_svc_t *)handle;
	rgb_recg_buf_t *recg_buf;

	recg_buf = malloc(sizeof(rgb_recg_buf_t));
	if (!recg_buf) {
		ALOGE("%s malloc rgb_recg_buf_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		return -1;
	}
	memset(recg_buf, 0, sizeof(rgb_recg_buf_t));
	recg_buf->ref++;

	/* process api frame */
	recg_buf->frame = frame;
	retain_frame(recg_buf->frame);
	recg_buf->type = FACE_FRAME_TYPE_LIVING;
	memcpy(&recg_buf->face_rib.ftmap, &face->map, sizeof(nna_feature_map_t));
	memcpy(&recg_buf->face_rib.rois, &face->nna_rect, sizeof(nna_face_t));
	memcpy(&recg_buf->face_info.rect, &face->rect, sizeof(face_rect_t));
	recg_buf->face_info.feat.max_id = face->feat.max_id;

	recg_buf->face_info.recg_begin = get_systime_us();
	pthread_mutex_lock(&svc->rgb_recg_lock);
	en_queue(&svc->rgb_recg_queue, recg_buf);
	pthread_mutex_unlock(&svc->rgb_recg_lock);

	nn_face_feat(&recg_buf->face_rib);

	return 0;
}

static void tracking_set_living_feats(tracking_face_t *face, face_feat_t *feat)
{
	if (!face || !feat)
		return;
	memcpy(face->feat.living_values, feat->values,
			sizeof(signed char) * FACE_FEAT_SIZE);
	return;
}

static void face_set_living_feats(face_svc_t *svc, rgb_recg_buf_t *rgb_recg_buf)
{
	int live = 0;
	face_frame_t *frame;
	face_info_t *face_info;
	rgb_recg_buf_t *recg_buf;
	tracking_face_t *face;

	recg_buf = rgb_recg_buf;
	frame = recg_buf->frame;
	face_info = &recg_buf->face_info;

	pthread_mutex_lock(&svc->tracking_lock);
	face = tracking_get(svc, face_info->rect.id);
	if (face == NULL) {
		goto free_res;
	}

	tracking_set_living_feats(face, &face_info->feat);
	tracking_check_living(svc, face_info, face);
	live = face->living;

free_res:
	if(live){
	    save_record_living_det(svc, face, recg_buf, match_frame);
	}
	else{
	    save_record_living_timeout(svc, face, recg_buf, unmatch_frame);
	}

	free_rgb_recg_buf(svc, &recg_buf);
	free_frame(&frame);
	pthread_mutex_unlock(&svc->tracking_lock);
	return;
}

static void *face_cmp_thread(void *arg)
{
	face_svc_t *svc = (face_svc_t *)arg;
	rgb_recg_buf_t *recg_buf;
	face_frame_t *frame;
	face_info_t *face_info;
	int conf;
	float score, threshold;
	int max_id;
	int feat_version;
	tracking_face_t *face;

	ALOGV("enter %s", __func__);

	prctl(PR_SET_NAME, "face-cmp");

	while (1) {
		pthread_mutex_lock(&svc->rgb_cmp_lock);
		if (empty_queue(&svc->rgb_cmp_queue))
			pthread_cond_wait(&svc->rgb_cmp_cond, &svc->rgb_cmp_lock);
		recg_buf = (rgb_recg_buf_t *)de_queue(&svc->rgb_cmp_queue);
		pthread_mutex_unlock(&svc->rgb_cmp_lock);

		if (recg_buf == NULL || recg_buf->frame == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}

		frame = recg_buf->frame;
		face_info = &recg_buf->face_info;

		if (recg_buf->type == FACE_FRAME_TYPE_LIVING) {
			face_set_living_feats(svc, recg_buf);
			continue;
		}

		pthread_mutex_lock(&svc->face_feat_lock);

		if (!svc->face_feat->features || svc->face_feat->features_num == 0) {
			ALOGD_IF(face_svc_conf->debug.debug_log, "face features db hasn't been registered");
			face_info->error |= FACE_SVC_ERR;
			pthread_mutex_unlock(&svc->face_feat_lock);

			if (frame->type == FACE_FRAME_TYPE_RGB) {
				pthread_mutex_lock(&svc->tracking_lock);
				face = tracking_get(svc, face_info->rect.id);
				if(face == NULL) {
					pthread_mutex_unlock(&svc->tracking_lock);
					goto free_res;
				}
				face->time = frame->time_stamp;
				long long cur_time = get_systime_us();

				memcpy(face->feat.values, face_info->feat.values,
					sizeof(signed char) * FACE_FEAT_SIZE);

				if (face_svc_conf->svc.det_timeout && cur_time > face->timeout_time
					&& (FACE_GET_BIT(face->face_state, FACE_STATE_RECG_REFUSE) == 0) /*face->face_refuse == 0*/) {
					ALOGD_IF(face_svc_conf->debug.debug_log,
						"%s svc_timeout %dsec timeout_time %lld cur_time = %lld", __func__,
						face_svc_conf->svc.det_timeout, face->timeout_time, cur_time);
					face->timeout = 1;
					face->face_refuse = 1;
					face->face_pass = 0;
					save_record_recg_det(svc, face, recg_buf, unmatch_frame);
				}
				pthread_mutex_unlock(&svc->tracking_lock);
			}

			goto free_res;
		}

		ALOGD_IF(face_svc_conf->debug.debug_log, "%d faces in db", svc->face_feat->features_num);

		/* for reg check test */
		//if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK && face_info->rect.attr.mask == 1 && face_svc_conf->svc.mask_recg_enable)
		//	memcpy(face_info->feat.values, face_info->feat.values + 512, 512);

		svc->face_feat->max_id = -1;
		svc->face_feat->feature_ref = face_info->feat.values;
		svc->face_feat->feature_size = FACE_FEAT_SIZE;
		svc->face_feat->feature_cmp_size = face_svc_conf->svc.feat_cmp_size;

		if (face_info->rect.attr.mask == 1 && face_svc_conf->svc.mask_recg_enable) {
			svc->face_feat->feature_part = 1;
			threshold = face_svc_conf->svc.mask_score_threshold;
		} else {
			svc->face_feat->feature_part = 0;
			threshold = face_svc_conf->svc.score_threshold;
		}

		face_info->cmp_begin = get_systime_us();
		conf = nn_face_feat_cmp(svc->face_feat);
		face_info->cmp_end = get_systime_us();
		ALOGI_IF(face_svc_conf->debug.profile, "face[%d] comparing toke %lld (us)",
			face_info->rect.id,
			face_info->cmp_end - face_info->cmp_begin);

		max_id = svc->face_feat->max_id;
		score = svc->face_feat->score[max_id];
		feat_version = svc->feat_version;
		ALOGD_IF(face_svc_conf->debug.debug_log, "frame %p face[%d] feature compare score: %f threshold: %f, max id: %d,"
			" confidence: %d feat_version: %d feature_part: %d",
			frame, face_info->rect.id, score, threshold, max_id, conf, feat_version, svc->face_feat->feature_part);

		pthread_mutex_unlock(&svc->face_feat_lock);

		/* correct score for half face */
		if (frame->type == FACE_FRAME_TYPE_RGB) {
			pthread_mutex_lock(&svc->tracking_lock);
			face = tracking_get(svc, face_info->rect.id);
			if (!face) {
				ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
				pthread_mutex_unlock(&svc->tracking_lock);
				goto free_res;
			}

			if (conf < face_svc_conf->svc.confidence_threshold) {
				score = 0.0;
				ALOGD_IF(face_svc_conf->debug.debug_log, "score restore to %f due to low confidence", score);
			}

			face_info->feat.score = score;
			face_info->feat.max_id = max_id;
			face_info->feat.cmp_conf = conf;
			face_info->feat.feat_version = feat_version;

			if ((!face->timeout || face_svc_conf->svc.det_timeout_cond) && face && score >= threshold) {
				tracking_set_recg(face, &face_info->feat);
				face->time = frame->time_stamp;
				face->face_pass = 1;
				face->face_refuse = 0;
				if (svc->recg_cb)
					svc->recg_cb(frame, face_info);
				save_record_recg_det(svc, face, recg_buf, match_frame);
			} else if (/*!face->timeout &&*/ face && ((score < threshold) || (isnan(score)) )  ) {
				face->time = frame->time_stamp;
				face->unmatch_count++;
				long long cur_time = get_systime_us();

				if (face_svc_conf->svc.det_timeout && cur_time > face->timeout_time
					&& (FACE_GET_BIT(face->face_state, FACE_STATE_RECG_REFUSE) == 0) /*face->face_refuse == 0*/) {
					ALOGD_IF(face_svc_conf->debug.debug_log,
						"svc_timeout %dsec timeout_time %lld cur_time = %lld",
						face_svc_conf->svc.det_timeout, face->timeout_time, cur_time);
					face->timeout = 1;
					face->face_refuse = 1;
					face->face_pass = 0;
					save_record_recg_det(svc, face, recg_buf, unmatch_frame);
				}
				if (face_svc_conf->svc.dump_debug_unmatch_face > 0 &&
					(!(face->unmatch_count % face_svc_conf->svc.dump_debug_unmatch_face)))
						save_record(svc, face, recg_buf, debug_unmatch_face);
				if (face_svc_conf->svc.dump_debug_unmatch_frame > 0 &&
					(!(face->unmatch_count % face_svc_conf->svc.dump_debug_unmatch_frame)))
						save_record(svc, face, recg_buf, debug_unmatch_frame);
			}
			pthread_mutex_unlock(&svc->tracking_lock);
		} else {
			face_info->feat.score = score;
			face_info->feat.max_id = max_id;
			face_info->feat.cmp_conf = conf;
			face_info->feat.feat_version = feat_version;
		}

free_res:
		if (frame->type == FACE_FRAME_TYPE_RGB) {
			pthread_mutex_lock(&svc->tracking_lock);
			face = tracking_get(svc, face_info->rect.id);
			if (recg_buf->type == FACE_FRAME_TYPE_LIVING || (face && face->feat.living_body == 1))
				free_rgb_recg_buf_no_cb(svc, &recg_buf);
			else{
				 free_rgb_recg_buf(svc, &recg_buf);
			}

			//free_rgb_recg_buf(svc, &recg_buf);
			free_frame(&frame);
			pthread_mutex_unlock(&svc->tracking_lock);
		} else if (frame->type == FACE_FRAME_TYPE_API_RECG && svc->recg_cb) {
			svc->recg_cb(frame, face_info);

			pthread_mutex_lock(&svc->tracking_lock);
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
			pthread_mutex_unlock(&svc->tracking_lock);
		} else if (frame->type == FACE_FRAME_TYPE_API_RECG_BLOCK) {
			/* process recg block api frame */
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
		} else if (frame->type == FACE_FRAME_TYPE_API_CAPTURE_BLOCK) {
			pthread_mutex_lock(&svc->face_capture_lock);
			if (svc->capture) {
				svc->capture = 0;
				retain_frame(frame);
				svc->capture_param->frame = frame;
				svc->capture_param->face_cnt = 1;
				face_info_t *info = svc->capture_info;
				memcpy(info, face_info, sizeof(face_info_t));
			}
			free_frame(&frame);
			pthread_cond_signal(&svc->face_capture_cond);
			pthread_mutex_unlock(&svc->face_capture_lock);

			pthread_mutex_lock(&svc->tracking_lock);
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
			pthread_mutex_unlock(&svc->tracking_lock);
		}
	}

	return NULL;
}

void rect_rgb_to_nir1(face_svc_t *svc, IRECTC *in, IRECTC *out)
{
	int xDeta, yDeta;
	cal_params_t *params = &svc->cal_params;

	out->x = (int)(params->xScaleMean * in->x + (params->xOffsetMean));
	xDeta = (int)(params->kX * in->width + (params->bX));
	CLIPMINMAX(xDeta, params->xDetaMin, params->xDetaMax);
	out->x += xDeta;

	out->y = (int)(params->yScaleMean * in->y + (params->yOffsetMean));
	yDeta = (int)(params->kY * (in->y + (in->height >> 1)) + (params->bY));
	CLIPMINMAX(yDeta, params->yDetaMin, params->yDetaMax);
	out->y += yDeta;

	out->width = (int)(params->xScaleMean * in->width);
	out->height = (int)(params->yScaleMean * in->height);

	CLIPMIN(out->y, 0);
	CLIPMAX(out->height, face_svc_conf->svc.nir_buffer_height - out->y);
	CLIPMIN(out->x, 0);
	CLIPMAX(out->width, face_svc_conf->svc.nir_buffer_width - out->x);
}

int face_svc_set_map_params(void *handle, map_params_t *map_params)
{
	int i = 0;
	face_svc_t *svc = (face_svc_t *)handle;
	if (!svc)
		return -1;

	for (i = 0; i < 5; i++) {
		svc->map_params.x_map[i] = map_params->x_map[i];
		ALOGV("x_map %f", svc->map_params.x_map[i]);
	}
	for (i = 0; i < 5; i++) {
		svc->map_params.y_map[i] = map_params->y_map[i];
		ALOGV("y_map %f", svc->map_params.y_map[i]);
	}

	return 0;
}

int face_svc_save_map_params(void *handle, map_params_t *map_params)
{
	int i = 0;
	face_map_params_t face_map_params;

	memset(&face_map_params, 0, sizeof(face_map_params_t));
	face_map_params.check = RGBNIR_MAP_CHECK;
	for (i = 0; i < 5; i++) {
		face_map_params.x_map[i] = map_params->x_map[i];
		ALOGV("x_map %f", face_map_params.x_map[i]);
	}
	for (i = 0; i < 5; i++) {
		face_map_params.y_map[i] = map_params->y_map[i];
		ALOGV("y_map %f", face_map_params.y_map[i]);
	}

	return face_svc_set_private_data(RGBNIR_MAP_OFFSET,
		 (char *)&face_map_params, sizeof(face_map_params_t));
}

int face_svc_get_ord_map_params(map_params_t *ord_map_params)
{
	int i = 0;
	map_t map;
	get_ord_map_t get_ord_map;
	memset(&map, 0, sizeof(map_t));
	memset(&get_ord_map, 0, sizeof(get_ord_map_t));

	get_ord_map.ord_path = (char*)face_svc_conf->svc.map_ord_path;
	get_ord_map.ord_grid_face_scale = face_svc_conf->svc.map_ord_grid_face;
	get_ord_map.ord_width = face_svc_conf->svc.map_ord_width;
	get_ord_map.ord_hight = face_svc_conf->svc.map_ord_height;
	get_ord_map.flip = face_svc_conf->svc.map_img_flip;
	get_ord_map.ord_map = &map;

	get_ord_map_params(&get_ord_map);

	for (i = 0; i < 5; i++) {
		ALOGV("x_map %f", map.x_map[i]);
		ord_map_params->x_map[i] = map.x_map[i];
	}

	for (i = 0; i < 5; i++) {
		ALOGV("y_map %f", map.y_map[i]);
		ord_map_params->y_map[i] = map.y_map[i];
	}

	return 0;
}

int face_svc_get_mod_map_params(map_params_t *ord_map_params,
	map_params_t *mod_map_params)
{
	int i = 0;
	map_t ord_map;
	map_t mod_map;
	get_mod_ord_map_t get_mod_ord_map;
	memset(&ord_map, 0, sizeof(map_t));
	memset(&mod_map, 0, sizeof(map_t));
	memset(&get_mod_ord_map, 0, sizeof(get_mod_ord_map_t));

	if (ord_map_params == NULL) {
		face_svc_load_params(g_svc);
		map_params_t *params = &g_svc->map_params;
		for (i = 0; i < 5; i++) {
			ord_map.x_map[i] = params->x_map[i];
			ord_map.y_map[i] = params->y_map[i];
		}
	} else {
		for (i = 0; i < 5; i++) {
			ord_map.x_map[i] = ord_map_params->x_map[i];
			ord_map.y_map[i] = ord_map_params->y_map[i];
		}
	}

	get_mod_ord_map.ord_map = &ord_map;
	get_mod_ord_map.mod_path = (char*)face_svc_conf->svc.map_mod_path;
	get_mod_ord_map.mod_width = face_svc_conf->svc.map_mod_width;
	get_mod_ord_map.mod_height = face_svc_conf->svc.map_mod_height;
	get_mod_ord_map.lr = face_svc_conf->svc.map_lr;
	get_mod_ord_map.lr_cnt_thr = face_svc_conf->svc.map_lr_cnt_thr;
	get_mod_ord_map.mod_grid_face_scale = face_svc_conf->svc.map_mod_grid_face;
	get_mod_ord_map.flip = face_svc_conf->svc.map_img_flip;
	get_mod_ord_map.mod_map = &mod_map;

	get_modify_ord_map_params(&get_mod_ord_map);
	for (i = 0; i < 5; i++) {
		mod_map_params->x_map[i] = mod_map.x_map[i];
		mod_map_params->y_map[i] = mod_map.y_map[i];
	}

	face_svc_set_map_params(g_svc, mod_map_params);
	return 0;
}

int face_svc_get_map_params(map_params_t *map_params)
{
	int i = 0;
	map_t map;
	get_map_t get_map;
	memset(&map, 0, sizeof(map_t));
	memset(&get_map, 0, sizeof(get_map_t));

	get_map.ord_path = (char*)face_svc_conf->svc.map_ord_path;
	get_map.ord_width = face_svc_conf->svc.map_ord_width;
	get_map.ord_hight = face_svc_conf->svc.map_ord_height;
	get_map.ord_grid_face_scale = face_svc_conf->svc.map_ord_grid_face;
	get_map.mod_path = (char*)face_svc_conf->svc.map_mod_path;
	get_map.mod_width = face_svc_conf->svc.map_mod_width;
	get_map.mod_height = face_svc_conf->svc.map_mod_height;
	get_map.mod_grid_face_scale = face_svc_conf->svc.map_mod_grid_face;
	get_map.lr = face_svc_conf->svc.map_lr;
	get_map.lr_cnt_thr = face_svc_conf->svc.map_lr_cnt_thr;
	get_map.flip = face_svc_conf->svc.map_img_flip;
	get_map.mod_map = &map;

	get_map_params(&get_map);
	for (i = 0; i < 5; i++) {
		map_params->x_map[i] = map.x_map[i];
		map_params->y_map[i] = map.y_map[i];
	}
	return 0;
}

void rect_rgb_to_nir2(face_svc_t *svc, IRECTC *in, IRECTC *out)
{
	int i = 0;
	double x_map[5] = {0};
	double y_map[5] = {0};
	int r_x0, r_y0, r_x1, r_y1, r_w, r_h, n_x0, n_y0, n_x1, n_y1;

	for (i = 0; i < 5; i++) {
		x_map[i] = svc->map_params.x_map[i];
		y_map[i] = svc->map_params.y_map[i];
	}

	r_x0 = in->x;
	r_y0 = in->y;
	r_x1 = in->x + in->width;
	r_y1 = in->y + in->height;
	r_w = in->width;
	r_h = in->height;

	n_x0 = x_map[0] * r_x0 + x_map[1] * r_y0 + x_map[2] + x_map[3] * r_w +
		x_map[4] * r_h;
	n_y0 = y_map[0] * r_x0 + y_map[1] * r_y0 + y_map[2] + y_map[3] * r_w +
		y_map[4] * r_h;

	n_x1 = x_map[0] * r_x1 + x_map[1] * r_y1 + x_map[2] + x_map[3] * r_w +
		x_map[4] * r_h;
	n_y1 = y_map[0] * r_x1 + y_map[1] * r_y1 + y_map[2] + y_map[3] * r_w +
		y_map[4] * r_h;

	out->x = n_x0;
	out->y = n_y0;

	out->width = n_x1 - n_x0;
	out->height = n_y1 - n_y0;

	CLIPMIN(out->y, 0);
	CLIPMAX(out->height, face_svc_conf->svc.nir_buffer_height - out->y);
	CLIPMIN(out->x, 0);
	CLIPMAX(out->width, face_svc_conf->svc.nir_buffer_width - out->x);
}

void rect_rgb_to_nir(face_svc_t *svc, IRECTC *in, IRECTC *out)
{
	if(face_svc_conf->svc.cal_mode == 1) {
		rect_rgb_to_nir1(svc, in, out);
	} else if (face_svc_conf->svc.cal_mode == 2) {
		rect_rgb_to_nir2(svc, in, out);
	} else {
		ALOGE("%s unsupport cal mode %d!", __func__, face_svc_conf->svc.cal_mode);
	}
}

int face_svc_take_mod_pic(void *handle, int x, int y, int w, int h)
{
	face_svc_t *svc = (face_svc_t *)handle;
	if(!svc) {
		return -1;
	}

	return cal_iface->take_mod_pic(x, y, w, h);
}

int face_svc_cal_mod_map(void *handle, map_params_t *map)
{
	int ret = -1;
	face_svc_t *svc = (face_svc_t *)handle;

	if(!svc) {
		return -1;
	}
	memcpy(map, &svc->map_params, sizeof(map_params_t));
	ret = cal_iface->cal_mod_map((struct map_t *)map);
	if(ret == 0)
		face_svc_set_map_params(svc, map);

	return face_svc_save_map_params(svc, map);
}

void overlap_rect(IRECTC *b1, IRECTC *b2, IRECTC *olRect, float *olSmall, float *olBig)
{
	ALOGD_IF(0, "b1 x: %d, y: %d, w: %d, h: %d", b1->x, b1->y, b1->width, b1->height);
	ALOGD_IF(0, "b2 x: %d, y: %d, w: %d, h: %d", b2->x, b2->y, b2->width, b2->height);

	int up1 = b1->y, dn1 = b1->y + b1->height - 1;
	int lt1 = b1->x, rt1 = b1->x + b1->width - 1;
	int up2 = b2->y, dn2 = b2->y + b2->height - 1;
	int lt2 = b2->x, rt2 = b2->x + b2->width - 1;
	unsigned char goon = 1;

	if (olSmall)
		*olSmall = 0;
	if (olBig)
		*olBig = 0;

	if (up1 >= dn2 || up2 >= dn1)
		goon = 0;
	else if (lt1 >= rt2 || lt2 >= rt1)
		goon = 0;

	if (goon) {
		int tmp = 0;
		int i = 0, j = 0;
		int lr[4], ud[4];
		lr[0] = lt1;
		lr[1] = rt1;
		lr[2] = lt2;
		lr[3] = rt2;
		ud[0] = up1;
		ud[1] = dn1;
		ud[2] = up2;
		ud[3] = dn2;
		for (i = 0; i <= 2; i++) {
			for (j = i+1; j <= 3; j++) {
				if (lr[i] > lr[j]) {
					tmp = lr[i];
					lr[i] = lr[j];
					lr[j] = tmp;
				}
				if (ud[i] > ud[j]) {
					tmp = ud[i];
					ud[i] = ud[j];
					ud[j] = tmp;
				}
			}
		}
		int area = (ud[2] - ud[1]) * (lr[2] - lr[1]);
		int area1 = b1->width * b1->height;
		int area2 = b2->width * b2->height;
		if (area1 > area2) {
			tmp = area1;
			area1 = area2;
			area2 = tmp;
		}
		if (olSmall)
			*olSmall = (float)area / area1;
		if (olBig)
			*olBig = (float)area / area2;
		if (olRect) {
			olRect->x = lr[1];
			olRect->y = ud[1];
			olRect->width = lr[2] - lr[1];
			olRect->height = ud[2] - ud[1];
		}
		ALOGD_IF(0, "area: %d, area1: %d, area2: %d", area, area1, area2);
	}
}

#if DUMP_NIR_WINDOW
static void face_set_nir_exposure(face_frame_t *nir_frame, ax_rect_t *p_rect)
#else
static void face_set_nir_exposure(ax_rect_t *p_rect)
#endif
{
	viss_isp_rects_t nir_rects;
	viss_isp_rect_t *nir_rect;
	int width, height;

	memset(&nir_rects, 0, sizeof(viss_isp_rects_t));

	if (!p_rect) {
		goto exit;
	}

	nir_rects.rects = malloc(sizeof(viss_isp_rect_t));
	if (!nir_rects.rects) {
		ALOGE("%s malloc viss_isp_rect_t failed, errno: %d (%s)",
			__func__, errno, strerror(errno));
		goto exit;
	}
	nir_rects.cnt = 1;
	nir_rect = &nir_rects.rects[0];

	nir_rect->left = p_rect->x;
	nir_rect->top = p_rect->y;
	nir_rect->right = nir_rect->left + p_rect->width;
	nir_rect->bottom = nir_rect->top + p_rect->height;

	width = nir_rect->right - nir_rect->left;
	height = nir_rect->bottom - nir_rect->top;

	nir_rect->left += width / 4;
	nir_rect->top += height / 4;
	nir_rect->right -= width / 4;
	nir_rect->bottom -= height / 4;

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

#if DUMP_NIR_WINDOW
	if (nir_frame) {
		ALOGD_IF(NIR_FACE_DEBUG, "%s nir_debug [%d, %d, %d, %d]", __func__,
			nir_rect->left, nir_rect->top, nir_rect->right, nir_rect->bottom);
		static int i = 0;
		char path[FILENAME_MAX];
		face_rect_t rect;
		rect.rect[0] = nir_rect->left;
		rect.rect[1] = nir_rect->top;
		rect.rect[2] = nir_rect->right;
		rect.rect[3] = nir_rect->bottom;
		snprintf(path, FILENAME_MAX, "/data/nir-%d.jpg", i);
		dump_face(path, nir_frame, &rect, 0, 0);
		i++;
	}
#endif

exit:
	face_set_ae_para(FACE_CAMERA_NIR, &nir_rects);

	if (nir_rects.rects)
		free(nir_rects.rects);
}

static int update_face_living_score(face_svc_t *svc, tracking_face_t* face_trk, rgb_recg_buf_t *recg_buf, face_rect_t* face_rect ,nirdt_in_t rgb_face, nirdt_in_t nir_face, int device_id)
{
	int living_score=0, nir_living_confidence=0;
	living_result_t living_result;
	int rgb_height;
	nirdt_in_t dt_in;
	int living_total_score=0;
	int color_score[3]={0,0,0};
	int rgb_score=0, nir_score=0;
	int mask_en=0;

	rgb_height = face_rect->rect[3] - face_rect->rect[1];
	mask_en = face_rect->attr.mask;

	//nir roi decrease
	dt_in = nir_face;
	float decrease = 0.05;
	int d_w = (int)(dt_in.dt_roi.width*decrease);
	int d_h = (int)(dt_in.dt_roi.height*decrease);
	dt_in.dt_roi.x += d_w;
	dt_in.dt_roi.y += d_h;
	dt_in.dt_roi.width -= 2*d_w;
	dt_in.dt_roi.height -= 2*d_h;

	if (!face_trk) {
		ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
		return living_total_score;
	}

	if (face_svc_conf->svc.dump_debug_nir_det_face > 0) {
		int left = dt_in.dt_roi.x;
		int top = dt_in.dt_roi.y;
		int right = dt_in.dt_roi.x + dt_in.dt_roi.width;
		int buttom = dt_in.dt_roi.y + dt_in.dt_roi.height;
		save_record_face(svc, face_trk, left, top, right, buttom, recg_buf, debug_nir_det_face);
	}

	//isp judge
	float rgb_exposure_time_rate = (float)face_trk->rect.exposure_time / 450.0;
	float rgb_exposure_dark_rate = rgb_exposure_time_rate * (float)face_trk->rect.iso_speed / 2000.0;
	float nir_exposure_time_rate = (float)face_trk->rect.nir_exposure_time / 60;
	float nir_exposure_dark_rate = (float)face_trk->rect.nir_iso_speed / 220 * nir_exposure_time_rate;
	int nir_avg_lum = face_trk->rect.nir_avg_lum / nir_exposure_dark_rate;
	int indoor = ((nir_exposure_time_rate > 1) && (nir_exposure_dark_rate > 1)) || (nir_avg_lum<150);
	ALOGI_IF(1, "liveness nir isp debug, nir_exposure_time: %d, nir_iso_speed: %d, face_trk->rect.nir_avg_lum: %d, nir_exposure_time_rate: %f, nir_exposure_dark_rate: %f, nir_avg_lum: %d\n", face_trk->rect.nir_exposure_time, face_trk->rect.nir_iso_speed, face_trk->rect.nir_avg_lum, nir_exposure_time_rate, nir_exposure_dark_rate, nir_avg_lum);

	//nir score
	if (mask_en == 1 && face_svc_conf->svc.mask_living_enable)
		living_result = svc->nir_living_hdl_mask->nir_recv_buffer(svc->nir_living_hdl_mask, &dt_in, device_id);
	else if (indoor == 1)
		living_result = svc->nir_living_hdl->nir_recv_buffer(svc->nir_living_hdl, &dt_in, device_id);
	else
		living_result = svc->nir_living_hdl_outdoors->nir_recv_buffer(svc->nir_living_hdl_outdoors, &dt_in, device_id);
	nir_score = living_result.living_score;
	nir_living_confidence = living_result.living_confidence;

	if (face_svc_conf->svc.dump_debug_nir_living > 0) {
			face_trk->living_count++;
			if (!(face_trk->living_count % face_svc_conf->svc.dump_debug_nir_living)) {
					if (nir_score <= 0) {
							save_record(svc, face_trk, recg_buf, debug_nir_no_living);
					} else {
							save_record(svc, face_trk, recg_buf, debug_nir_living);
					}
			}
	}

	int ahp = 128;	/* 0~256 */
	int face_mean_color_thr[2]={0, 5};
	int living_penalty_sw=0;
	int nir_neg_thr=-40;

	/*update nir acc*/
	face_trk->living_score_acc += living_score;
	CLIPMIN(face_trk->living_score_acc, -256);
	CLIPMAX(face_trk->living_score_acc, 256);

	//cal color thr
	if (face_trk->living_score_acc > 60)
		face_mean_color_thr[0] -= 5;

	if (rgb_height<120) {
		face_mean_color_thr[0] -= 10;
		face_mean_color_thr[1] += 15;
	}
	else if (rgb_height<200) {
		face_mean_color_thr[0] -= 5;
		face_mean_color_thr[1] += 10;
	}
	else if (rgb_height<300) {
		face_mean_color_thr[1] += 5;
	}

	int living_confidence_thr = face_svc_conf->svc.living_confidence_thr;
	living_penalty_sw = rgb_height;
	CLIPMAX(living_penalty_sw, 380);
	living_penalty_sw /= 100;
	switch(living_penalty_sw)//living_confidence_thr  can not large than 0
	{
		case 3:
			living_confidence_thr -= 0;
			break;
		case 2:
			living_confidence_thr -= 5;
			break;
		case 1:
			living_confidence_thr -= 15;
			break;
		case 0:
			living_confidence_thr -= 30;
			break;

		default:
			living_confidence_thr -= 30;
	}

	living_score = ((nir_living_confidence >= living_confidence_thr) || (nir_score<nir_neg_thr))? nir_score:0;

	//update nir score when nir conf higher than nir conf thr
	face_trk->living_score = ((256 - ahp) * living_score >> 8) + face_trk->living_score;
	CLIPMIN(face_trk->living_score, -256);
	CLIPMAX(face_trk->living_score, 64);
	face_trk->living_last_score = living_score;

	if (living_score>0) {
		/* rgb color living detect */
		/* rgb color living*/
		int x1,x2,y1,y2;
		ahp=0;

		/* rgb 4x3 living*/
		dt_in = rgb_face;
		x1 = rgb_face.dt_roi.x-rgb_face.dt_roi.width;
		y1 = rgb_face.dt_roi.y-rgb_face.dt_roi.height;
		x2 = x1 + (rgb_face.dt_roi.width)*3;
		y2 = y1 + (rgb_face.dt_roi.height)*4;
		CLIPMIN(x1, 0);
		CLIPMAX(x2, dt_in.yuv.size.width-1);
		CLIPMIN(y1, 0);
		CLIPMAX(y2, dt_in.target_sz.height-1);
		dt_in.dt_roi.x = x1;
		dt_in.dt_roi.y = y1;
		dt_in.dt_roi.width = x2-x1;
		dt_in.dt_roi.height = y2-y1;

		if (face_svc_conf->svc.dump_debug_rgb_det_face > 0) {
			int left = dt_in.dt_roi.x;
			int top = dt_in.dt_roi.y;
			int right = dt_in.dt_roi.x + dt_in.dt_roi.width;
			int buttom = dt_in.dt_roi.y + dt_in.dt_roi.height;
			save_record_face(svc, face_trk, left, top, right, buttom, recg_buf, debug_rgb_det_face);
		}

		living_result = svc->rgb_living_hdl->nir_recv_buffer(svc->rgb_living_hdl,
				&dt_in, device_id);

		//rgb score
		rgb_score = living_result.living_score;
		face_trk->rect_mean_rgb = (face_trk->rect_mean_rgb * (128) + rgb_score * (128)) >> 8;

		//rgb color
		//face_svc_conf->svc.living_color_enable: 0->dont run color model; 1->run color model, get color and cover score; 2->run color model, get color score only
		if (face_svc_conf->svc.living_color_enable>0) {
			dt_in = rgb_face;
			living_result = svc->rgb_color_living_hdl->nir_recv_buffer(svc->rgb_color_living_hdl, &dt_in, device_id);
			memcpy(color_score, living_result.living_color_score, sizeof(color_score));
			ALOGD_IF(face_svc_conf->debug.debug_log, "liveness debug color: %d, %d, %d\n", color_score[0], color_score[1], color_score[2]);
			if(face_svc_conf->svc.living_color_enable==1)
				face_trk->face_mean_color[1] = (face_trk->face_mean_color[1] * (128) + color_score[1] * (128)) >> 8;
			if (rgb_exposure_time_rate < 1 && rgb_exposure_dark_rate < 1) {
				face_trk->face_mean_color[0] = (face_trk->face_mean_color[0] * (128) + color_score[0] * (128)) >> 8;
			}
		}

		int ahp_up,ahp_down;
		//nir rgb ahp
		y1 = dt_in.dt_roi.y;
		y2 = y1+dt_in.dt_roi.height;
		ahp_down = (int)((dt_in.yuv.size.height-y2-rgb_height)*128/rgb_height);
		CLIPMIN(ahp_down, 0);
		CLIPMAX(ahp_down, 128)
		ahp_up = (int)((y1-rgb_height/2)*128/rgb_height);
		CLIPMIN(ahp_up, 0);
		CLIPMAX(ahp_up, 64)
		ahp = ahp_down+ahp_up;
		CLIPMAX(ahp, 128)
		ALOGI_IF(0, "liveness debug ahp:%d, %d, %d\n", ahp_up, ahp_down, ahp);
		ALOGI_IF(1, "liveness new debug ahp: %d, rgb_height: %d, rect_mean_rgb: %d\n", ahp, rgb_height, rgb_score);
	}

	if ((living_score>0) && (face_trk->face_mean_color[0]>=face_mean_color_thr[0] && face_trk->face_mean_color[1]<=face_mean_color_thr[1]))
		//living_total_score = rgb_score<0? 0:((face_trk->living_score*(256-ahp) + rgb_score*ahp)>>8);
		living_total_score = ((face_trk->living_score*(256-ahp) + (face_trk->rect_mean_rgb+ (int)(face_trk->rect_mean_rgb<0?face_trk->rect_mean_rgb:0))* ahp )>>8);

	ALOGD_IF(face_svc_conf->debug.debug_log, "liveness new record, mean color score[0]: %d, mean color thr[0]: %d, mean color score[1]: %d, mean color thr[1]: %d, face_trk->living_score: %d, nir living_score: %d, nir living_confidence: %d, nir living_confidence thr: %d, rgb_rect_score: %d, living_total_score: %d, %s\n",
										color_score[0], face_mean_color_thr[0], color_score[1], face_mean_color_thr[1], face_trk->living_score, nir_score, nir_living_confidence, living_confidence_thr, rgb_score, living_total_score, mask_en? "mask":(indoor == 1? "indoor" : "outdoor"));
	return living_total_score;
}

static void *face_living_thread(void *arg)
{

	face_svc_t *svc = (face_svc_t *)arg;
	rgb_recg_buf_t *recg_buf;
	face_frame_t *rgb_frame, *nir_frame;
	face_info_t *face_info;
	face_rect_t *face_rect;
	LivingFrameIn living_in;
	/* LivingFrameOut living_out; */
	nirdt_in_t dt_in;
	nirdt_out_t *dt_ret;
	int i, index;
	float max_big;
	tracking_face_t *face;
	int face_id;
	int motion_blur=0;
	int penalty, rgb_height;

	ALOGV("enter %s", __func__);

	prctl(PR_SET_NAME, "face-living");

	while (1) {
		if (face_svc_conf->svc.dump_nir_living_fps) {
			static long long start = 0;
			long long end = get_systime_us();
			static int fps = 0;

			fps++;
			if (end - start > 1000000) {
				ALOGI("face nir living frame %d fps", fps);
				start = get_systime_us();
				fps = 0;
			}
		}

		pthread_mutex_lock(&svc->rgb_living_lock);
		if (empty_queue(&svc->rgb_living_queue))
			pthread_cond_wait(&svc->rgb_living_cond, &svc->rgb_living_lock);
		recg_buf = (rgb_recg_buf_t *)de_queue(&svc->rgb_living_queue);
		pthread_mutex_unlock(&svc->rgb_living_lock);

		if (recg_buf == NULL || recg_buf->frame == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}

		rgb_frame = recg_buf->frame;
		nir_frame = recg_buf->nir_frame;
		face_info = &recg_buf->face_info;
		face_rect = &face_info->rect;
		face_id = face_rect->id;

		pthread_mutex_lock(&svc->tracking_lock);
		face = tracking_get(svc, face_id);
		if (!face) {
			ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}
		if (face->living) {
			ALOGI("frame %p face[%d] already living checked",
				rgb_frame, face_id);
			face_info->feat.living_body = 1;
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}

		pthread_mutex_unlock(&svc->tracking_lock);

		/* send nir face rects to app for draw box */
		/* fill rgb and nir frame info */
		living_in.imgYuv.yP = rgb_frame->viraddr[0];
		living_in.imgYuv.uvP = rgb_frame->viraddr[1];
		living_in.imgYuv.size.width = rgb_frame->width;
		living_in.imgYuv.size.height = rgb_frame->height;
		living_in.imgNir.yP = nir_frame->viraddr[0];
		living_in.imgNir.uvP = nir_frame->viraddr[1];
		living_in.imgNir.size.width = nir_frame->width;
		living_in.imgNir.size.height = nir_frame->height;
		living_in.yuvFaceLocExist = 1;
		living_in.nirFaceLocExist = 1;

		living_in.yuvFaceLoc.x = face_rect->rect[0] + face_svc_conf->svc.rgb_x_margin;
		living_in.yuvFaceLoc.y = face_rect->rect[1] + face_svc_conf->svc.rgb_y_margin;
		living_in.yuvFaceLoc.width = face_rect->rect[2] - face_rect->rect[0];
		living_in.yuvFaceLoc.height = face_rect->rect[3] - face_rect->rect[1];
		rect_rgb_to_nir(svc, &living_in.yuvFaceLoc, &living_in.nirFaceLoc);
		ALOGD_IF(0, "face[%d] in x:%d, y:%d, w:%d, h:%d",
			face_id,
			living_in.yuvFaceLoc.x,
			living_in.yuvFaceLoc.y,
			living_in.yuvFaceLoc.width,
			living_in.yuvFaceLoc.height);
		ALOGD_IF(0, "face[%d] out x:%d, y:%d, w:%d, h:%d",
			face_id,
			living_in.nirFaceLoc.x,
			living_in.nirFaceLoc.y,
			living_in.nirFaceLoc.width,
			living_in.nirFaceLoc.height);

		/*motion blur detect*/
		motion_blur=0;
		rgb_height = face_rect->rect[3] - face_rect->rect[1];
		int mx = (face_rect->rect[0]+face_rect->rect[2])/2;
		int my = (face_rect->rect[1]+face_rect->rect[3])/2;

		pthread_mutex_lock(&svc->tracking_lock);
		face = tracking_get(svc, face_id);
		if (!face) {
			ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}
		if (face->rgb_mid_recg[0]!=0 || face->rgb_mid_recg[1]!=0)
		{
			int mdx = abs(face->rgb_mid_recg[0]-mx);
			int mdy = abs(face->rgb_mid_recg[1]-my);

			if ((mdx+mdy)>50)
				motion_blur=1;

			ALOGI_IF(1, "liveness motion_blur:%d, mdx:%d, mdy:%d\n", motion_blur, mdx, mdy);
		}
		face->rgb_mid_recg[0]=mx;
		face->rgb_mid_recg[1]=my;
		pthread_mutex_unlock(&svc->tracking_lock);

		/* fill nir detect info */
		dt_in.yuv.y = (void *)living_in.imgNir.yP;
		dt_in.yuv.uv = (void *)living_in.imgNir.uvP;
		dt_in.yuv.y_phy = (void *)nir_frame->phyaddr[0];
		dt_in.yuv.uv_phy = (void *)nir_frame->phyaddr[1];
		dt_in.yuv.size.width = living_in.imgNir.size.width;
		dt_in.yuv.size.height = living_in.imgNir.size.height;

#if 0
		/* expand 0.6 times */
		int dx = (int)(living_in.nirFaceLoc.width * 0.3f);
		int dy = (int)(living_in.nirFaceLoc.height * 0.3f);
		dt_in.dt_roi.x = living_in.nirFaceLoc.x - dx;
		dt_in.dt_roi.y = living_in.nirFaceLoc.y - dy;
		dt_in.dt_roi.width = living_in.nirFaceLoc.width + 2 * dx;
		dt_in.dt_roi.height = living_in.nirFaceLoc.height + 2 * dy;

		face_info->nir_rect.rect[0] = dt_in.dt_roi.x;
		face_info->nir_rect.rect[1] = dt_in.dt_roi.y;
		face_info->nir_rect.rect[2] = dt_in.dt_roi.x + dt_in.dt_roi.width;
		face_info->nir_rect.rect[3] = dt_in.dt_roi.y + dt_in.dt_roi.height;

		ALOGD_IF(0, "face[%d] out x:%d, y:%d, w:%d, h:%d",
			face_id,
			face_info->nir_rect.rect[0],
			face_info->nir_rect.rect[1],
			face_info->nir_rect.rect[2],
			face_info->nir_rect.rect[3]);
#endif

		/* expand detect pixel */
		int dx = face_svc_conf->svc.nir_x_expand;
		int dy = face_svc_conf->svc.nir_y_expand;
		dt_in.dt_roi.x = living_in.nirFaceLoc.x - dx;
		dt_in.dt_roi.y = living_in.nirFaceLoc.y - dy;
		dt_in.dt_roi.width = living_in.nirFaceLoc.width + 2 * dx;
		dt_in.dt_roi.height = living_in.nirFaceLoc.height + 2 * dy;

		face_info->nir_rect.rect[0] = dt_in.dt_roi.x;
		face_info->nir_rect.rect[1] = dt_in.dt_roi.y;
		face_info->nir_rect.rect[2] = dt_in.dt_roi.x + dt_in.dt_roi.width;
		face_info->nir_rect.rect[3] = dt_in.dt_roi.y + dt_in.dt_roi.height;

#if 0
		/* expand 1 times */
		dt_in.dt_roi.x = living_in.nirFaceLoc.x -
			living_in.nirFaceLoc.width / 2;
		dt_in.dt_roi.y = living_in.nirFaceLoc.y -
			living_in.nirFaceLoc.height / 2;
		dt_in.dt_roi.width = living_in.nirFaceLoc.width * 2;
		dt_in.dt_roi.height = living_in.nirFaceLoc.height * 2;
#endif
		CLIPMIN(dt_in.dt_roi.y, 0);
		CLIPMAX(dt_in.dt_roi.height, living_in.imgNir.size.height -
			dt_in.dt_roi.y);
		CLIPMIN(dt_in.dt_roi.x, 0);
		CLIPMAX(dt_in.dt_roi.width, living_in.imgNir.size.width -
			dt_in.dt_roi.x);
		ALOGD_IF(0, "face[%d] dt in x:%d, y:%d, w:%d, h:%d",
			face_id,
			dt_in.dt_roi.x,
			dt_in.dt_roi.y,
			dt_in.dt_roi.width,
			dt_in.dt_roi.height);

		/* face size in estimation */
		dt_in.target_sz.width = living_in.nirFaceLoc.width;
		dt_in.target_sz.height = living_in.nirFaceLoc.height;
		dt_in.enlarge_x = 0.05;
		dt_in.enlarge_y = 0.04;
		dt_in.detect_whole_image = 0;

		nir_frame->det_begin = get_systime_us();
		nirdt_detect(svc->nirdt_hdl, &dt_in, &dt_ret);
		nir_frame->det_end = get_systime_us();
		ALOGI_IF(face_svc_conf->debug.profile, "face[%d] nir frame %p detecting toke %lld",
			face_id, nir_frame,
			nir_frame->det_end - nir_frame->det_begin);

		if (dt_ret->size == 0) {
			ALOGW("face[%d] nir no face detected", face_id);
			pthread_mutex_lock(&svc->tracking_lock);
			face = tracking_get(svc, face_id);
			if (!face) {
				ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
				pthread_mutex_unlock(&svc->tracking_lock);
				goto free_res;
			}
			face->nir_no_face = 1;
			if (face_svc_conf->svc.dump_debug_nir_no_face) {
				save_record(svc, face, recg_buf, debug_nir_no_face);
			}

			if (motion_blur==1)
				penalty = 0;
			else {
				penalty = -7;
			}
			face->living_score += penalty;
			face->living_last_score = penalty;
			if (face->living_score < -256)
				face->living_score = -256;
			ALOGD_IF(face_svc_conf->debug.debug_log, "frame: %p face[%d] living final score: %d, lable: %d, line:%d",
				rgb_frame, face_id, face->living_score, face_info->feat.living_body, __LINE__);
			pthread_mutex_unlock(&svc->tracking_lock);
			goto no_face;
		}

		pthread_mutex_lock(&svc->tracking_lock);
		face = tracking_get(svc, face_id);
		if (!face) {
			ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}
		face->nir_no_face = 0;
		pthread_mutex_unlock(&svc->tracking_lock);
		index = -1;
		max_big = 0.0f;
		float ol_big_thr = 0.2f;
		if (rgb_height<200)
			ol_big_thr = 0.0f;
		else if (rgb_height<250)
			ol_big_thr = 0.1f;

		for (i = 0; i < dt_ret->size; i++) {
			IRECTC rgb2nir, det_nir, overlap_nir;
			float ol_big = 0.0f, ol_small = 0.0f;
			ALOGD_IF(0, "dt out [%d] x: %d, y: %d, w: %d, h: %d, score: %f",
				i, dt_ret->p[i].roi.x, dt_ret->p[i].roi.y,
				dt_ret->p[i].roi.width, dt_ret->p[i].roi.height,
				dt_ret->p[i].score);

			rgb2nir.x = living_in.nirFaceLoc.x;
			rgb2nir.y = living_in.nirFaceLoc.y;
			rgb2nir.width = living_in.nirFaceLoc.width;
			rgb2nir.height = living_in.nirFaceLoc.height;
			det_nir.x = dt_ret->p[i].roi.x;
			det_nir.y = dt_ret->p[i].roi.y;
			det_nir.width = dt_ret->p[i].roi.width;
			det_nir.height = dt_ret->p[i].roi.height;
			overlap_rect(&rgb2nir, &det_nir, &overlap_nir,
				&ol_small, &ol_big);
			ALOGD_IF(1, "ol_big: %f, ol_small: %f, ol_big:%f, max_big:%f",
				ol_big, ol_small, ol_big, max_big);
			if (ol_big > ol_big_thr && max_big < ol_big) {
				index = i;
				max_big = ol_big;
			}
		}
		if (index==-1) {
			ALOGE("liveness new no_face\n");
			goto no_face;
		}

		living_in.nirFaceLoc.x = dt_ret->p[index].roi.x;
		living_in.nirFaceLoc.y = dt_ret->p[index].roi.y;
		living_in.nirFaceLoc.width = dt_ret->p[index].roi.width;
		if (living_in.nirFaceLoc.y < 0) {
			living_in.nirFaceLoc.height = dt_ret->p[index].roi.height;
			living_in.nirFaceLoc.height += living_in.nirFaceLoc.y;
			CLIPMIN(living_in.nirFaceLoc.y, 0);
		} else
			living_in.nirFaceLoc.height = dt_ret->p[index].roi.height;

		/* set nir image exposure window */
#if DUMP_NIR_WINDOW
		pthread_mutex_lock(&svc->tracking_lock);
		face_set_nir_exposure(nir_frame , &dt_ret->p[index].roi);
		pthread_mutex_unlock(&svc->tracking_lock);
#else
		face_set_nir_exposure(&dt_ret->p[index].roi);
#endif

		nirdt_in_t rgb_face, nir_face;

		//nir face
		nir_face.target_sz.width = nir_frame->width;
		nir_face.target_sz.height = nir_frame->height;
		nir_face.yuv.size.width = living_in.imgNir.size.width;
		nir_face.yuv.size.height = living_in.imgNir.size.height;
		nir_face.yuv.y = (void *)living_in.imgNir.yP;
		nir_face.yuv.uv = (void *)living_in.imgNir.uvP;
		nir_face.yuv.y_phy = (void *)nir_frame->phyaddr[0];
		nir_face.yuv.uv_phy = (void *)nir_frame->phyaddr[1];
		nir_face.dt_roi.x = living_in.nirFaceLoc.x;
		nir_face.dt_roi.y = living_in.nirFaceLoc.y;
		nir_face.dt_roi.width = living_in.nirFaceLoc.width;
		nir_face.dt_roi.height = living_in.nirFaceLoc.height;
		nir_face.set_gray = face_svc_conf->svc.living_check_without_uv; //default 0 , if (set_gray==1) uv=128

		//rgb face
		rgb_face.yuv.size.width = face_svc_conf->svc.rgb_buffer_width;
		rgb_face.yuv.size.height = face_svc_conf->svc.rgb_buffer_height;
		rgb_face.target_sz.width = face_svc_conf->svc.rgb_buffer_width;
		rgb_face.target_sz.height = face_svc_conf->svc.rgb_buffer_height;
		rgb_face.yuv.y = (void *)rgb_frame->viraddr[0] + rgb_frame->width * face_svc_conf->svc.rgb_y_margin + face_svc_conf->svc.rgb_x_margin;
		rgb_face.yuv.uv = (void *)rgb_frame->viraddr[1] + rgb_frame->width * face_svc_conf->svc.rgb_y_margin / 2 + face_svc_conf->svc.rgb_x_margin;
		rgb_face.yuv.y_phy = (void *)rgb_frame->phyaddr[0] + rgb_frame->width * face_svc_conf->svc.rgb_y_margin + face_svc_conf->svc.rgb_x_margin;
		rgb_face.yuv.uv_phy = (void *)rgb_frame->phyaddr[1] + rgb_frame->width * (face_svc_conf->svc.rgb_y_margin) / 2 + face_svc_conf->svc.rgb_x_margin;
		rgb_face.dt_roi.x = face_rect->rect[0];
		rgb_face.dt_roi.y = face_rect->rect[1];
		rgb_face.dt_roi.width = face_rect->rect[2]-face_rect->rect[0];
		rgb_face.dt_roi.height = face_rect->rect[3]-face_rect->rect[1];

		pthread_mutex_lock(&svc->tracking_lock);
		face = tracking_get(svc, face_id);
		if (!face) {
			ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}
		int device_id = face_svc_conf->svc.device_id;
		int living_total_score = update_face_living_score(svc, face, recg_buf, face_rect, rgb_face, nir_face, device_id);

		if ((!face->timeout || face_svc_conf->svc.det_timeout_cond) &&
			living_total_score >= face_svc_conf->svc.living_score_threshold && (face->living_check_count >= face_svc_conf->svc.living_check_count)) {
			ALOGD("face_state living_body=1 living_low_score_cnt=%d", face->living_low_score_cnt);
			face_info->feat.living_body = 1;
			face->living_low_score_cnt = 0;

			tracking_set_living(face, &face_info->feat);
			FACE_CLEAR_BIT(face->face_state, FACE_STATE_LIVING_RECG_TIMEOUT);
			if (face_svc_conf->svc.dump_living == 1)
				save_record(svc, face, recg_buf, living);
//			if (face->recg) {
//				save_record_living_det(svc, face, recg_buf, match_frame);
//			}

			if (face->recg && face->feat.score > 0) {
				face_det_living_rgb_feats(svc, face, rgb_frame);
			}

		} else {
			face_info->feat.living_body = 0;
			tracking_set_living(face, &face_info->feat);

			face->living_low_score_cnt++;
			long long cur_time = get_systime_us();
			/*continuous low score. 4fps*/
			if ((face->living_low_score_cnt >= 3) &&
				face_svc_conf->svc.det_timeout &&
				cur_time > face->timeout_time  &&
				FACE_GET_BIT(face->face_state, FACE_STATE_LIVING_RECG_TIMEOUT) == 0) {
				ALOGD_IF(face_svc_conf->debug.debug_log, " living timeout_time");
				face->timeout = 1;
				//face->face_refuse = 1;
				FACE_SET_BIT(face->face_state, FACE_STATE_LIVING_RECG_TIMEOUT);
				if (face->recg && face->feat.score > 0)
					save_record_living_timeout(svc, face, recg_buf, match_frame);
				else
					save_record_living_timeout(svc, face, recg_buf, unmatch_frame);
			}
		}

		ALOGD_IF(face_svc_conf->debug.debug_log, "liveness frame: %p face[%d] living final score: %d, lable: %d",
			rgb_frame, face_id, face->living_score, face_info->feat.living_body);

		pthread_mutex_unlock(&svc->tracking_lock);
		goto free_res;

no_face:
		pthread_mutex_lock(&svc->tracking_lock);
		face = tracking_get(svc, face_id);
		if (!face) {
			ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}
		face_info->feat.living_body = 0;

		face->living_low_score_cnt++;
		long long cur_time = get_systime_us();
		/*continuous low score. 4fps*/
		if ((face->living_low_score_cnt >= 3) &&
			(FACE_GET_BIT(face->face_state, FACE_STATE_LIVING_RECG_TIMEOUT) == 0) &&
			((face_svc_conf->svc.det_timeout && cur_time > face->timeout_time) || face->timeout) /*&& face->face_refuse == 0*/) {
			ALOGD_IF(face_svc_conf->debug.debug_log, "living timeout");
			face->timeout = 1;
			//face->face_refuse = 1;
			FACE_SET_BIT(face->face_state, FACE_STATE_LIVING_RECG_TIMEOUT);
			if (face->recg && face->feat.score > 0)
				save_record_living_timeout(svc, face, recg_buf, match_frame);
			else
				save_record_living_timeout(svc, face, recg_buf, unmatch_frame);
		}
		pthread_mutex_unlock(&svc->tracking_lock);


free_res:
		pthread_mutex_lock(&svc->tracking_lock);
//		free_rgb_recg_buf(svc, &recg_buf);
		if (face && face->feat.living_body == 1 && face->recg) {
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
		}else{
			free_rgb_recg_buf(svc, &recg_buf);
		}
		free_frame(&rgb_frame);
		free_frame(&nir_frame);
		pthread_mutex_unlock(&svc->tracking_lock);
	}

	return NULL;
}

static int update_face_living_score_rgb(face_svc_t *svc, tracking_face_t* face_trk, rgb_recg_buf_t *recg_buf, face_rect_t* face_rect ,nirdt_in_t rgb_face, int device_id)
{
	int living_score=0;
	living_result_t living_result;
	int rgb_height;
	nirdt_in_t dt_in;
	int living_total_score=0;
	int color_score[3]={0,0,0};
	int rgb_score=0;

	rgb_height = face_rect->rect[3] - face_rect->rect[1];
	//cal color thr
	int face_mean_color_thr[2]={0, 0};
	if (rgb_height<120) {
		face_mean_color_thr[0] -= 10;
		face_mean_color_thr[1] += 15;
	}
	else if (rgb_height<200) {
		face_mean_color_thr[0] -= 5;
		face_mean_color_thr[1] += 10;
	}
	else if (rgb_height<300) {
		face_mean_color_thr[1] += 5;
	}

	//isp judge
	float rgb_exposure_time_rate = (float)face_trk->rect.exposure_time / 450.0;
	float rgb_exposure_dark_rate = rgb_exposure_time_rate * (float)face_trk->rect.iso_speed / 2000.0;

	int x1,x2,y1,y2;

	/* rgb 4x3 living*/
	dt_in = rgb_face;
	x1 = rgb_face.dt_roi.x-rgb_face.dt_roi.width;
	y1 = rgb_face.dt_roi.y-rgb_face.dt_roi.height;
	x2 = x1 + (rgb_face.dt_roi.width)*3;
	y2 = y1 + (rgb_face.dt_roi.height)*4;
	CLIPMIN(x1, 0);
	CLIPMAX(x2, dt_in.yuv.size.width-1);
	CLIPMIN(y1, 0);
	CLIPMAX(y2, dt_in.target_sz.height-1);
	dt_in.dt_roi.x = x1;
	dt_in.dt_roi.y = y1;
	dt_in.dt_roi.width = x2-x1;
	dt_in.dt_roi.height = y2-y1;

	if (face_svc_conf->svc.dump_debug_rgb_det_face == 1) {
		int left = dt_in.dt_roi.x;
		int top = dt_in.dt_roi.y;
		int right = dt_in.dt_roi.x + dt_in.dt_roi.width;
		int buttom = dt_in.dt_roi.y + dt_in.dt_roi.height;
		save_record_face(svc, face_trk, left, top, right, buttom, recg_buf, debug_rgb_det_face);
	}

	living_result = svc->rgb_living_hdl->nir_recv_buffer(svc->rgb_living_hdl,
			&dt_in, device_id);

	//rgb score
	rgb_score = living_result.living_score;
	face_trk->living_score = (128 * rgb_score >> 8) + face_trk->living_score;
	CLIPMIN(face_trk->living_score, -256);
	CLIPMAX(face_trk->living_score, 64);

	if (rgb_score>0) {
		/* rgb color living detect */
		/* rgb color living*/

		//rgb color
		//face_svc_conf->svc.living_color_enable: 0->dont run color model; 1->run color model, get color and cover score; 2->run color model, get color score only
		if (face_svc_conf->svc.living_color_enable>0) {
			dt_in = rgb_face;
			if (face_svc_conf->svc.dump_debug_rgb_det_face == 2) {
				int left = dt_in.dt_roi.x;
				int top = dt_in.dt_roi.y;
				int right = dt_in.dt_roi.x + dt_in.dt_roi.width;
				int buttom = dt_in.dt_roi.y + dt_in.dt_roi.height;
				save_record_face(svc, face_trk, left, top, right, buttom, recg_buf, debug_rgb_det_face);
			}
			living_result = svc->rgb_color_living_hdl->nir_recv_buffer(svc->rgb_color_living_hdl, &dt_in, device_id);
			memcpy(color_score, living_result.living_color_score, sizeof(color_score));
			ALOGD_IF(face_svc_conf->debug.debug_log, "liveness debug color: %d, %d, %d\n", color_score[0], color_score[1], color_score[2]);
			if(face_svc_conf->svc.living_color_enable==1)
				face_trk->face_mean_color[1] = (face_trk->face_mean_color[1] * (128) + color_score[1] * (128)) >> 8;
			if (rgb_exposure_time_rate < 1 && rgb_exposure_dark_rate < 1) {
				face_trk->face_mean_color[0] = (face_trk->face_mean_color[0] * (128) + color_score[0] * (128)) >> 8;
			}
		}
	}

	if ((rgb_score>0) && (face_trk->face_mean_color[0]>=face_mean_color_thr[0] && face_trk->face_mean_color[1]<=face_mean_color_thr[1]))
		//living_total_score = rgb_score<0? 0:((face_trk->living_score*(256-ahp) + rgb_score*ahp)>>8);
		living_total_score = face_trk->living_score;

	ALOGD_IF(face_svc_conf->debug.debug_log, "liveness new record, mean color score[0]: %d, mean color thr[0]: %d, mean color score[1]: %d, mean color thr[1]: %d, rgb_score: %d, living_score: %d\n",
										color_score[0], face_mean_color_thr[0], color_score[1], face_mean_color_thr[1], rgb_score, living_score);
	return living_total_score;
}

static void *face_rgb_living_thread(void *arg)
{
	face_svc_t *svc = (face_svc_t *)arg;
	rgb_recg_buf_t *recg_buf;
	face_frame_t *rgb_frame;
	face_info_t *face_info;
	face_rect_t *face_rect;
	LivingFrameIn living_in;
	/*
	LivingFrameOut living_out;
	*/
	nirdt_in_t dt_in;
	/*
	nirdt_out_t *dt_ret;
	int i, index;
	float max_big;
	*/
	tracking_face_t *face;
	int face_id;

	ALOGV("enter %s", __func__);

	prctl(PR_SET_NAME, "face-rgb-living");

	while (1) {
		if (face_svc_conf->svc.dump_rgb_living_fps) {
			static long long start = 0;
			long long end = get_systime_us();
			static int fps = 0;

			fps++;
			if (end - start > 1000000) {
				ALOGI("face rgb living frame %d fps", fps);
				start = get_systime_us();
				fps = 0;
			}
		}

		pthread_mutex_lock(&svc->rgb_living_lock);
		if (empty_queue(&svc->rgb_living_queue))
			pthread_cond_wait(&svc->rgb_living_cond, &svc->rgb_living_lock);
		recg_buf = (rgb_recg_buf_t *)de_queue(&svc->rgb_living_queue);
		pthread_mutex_unlock(&svc->rgb_living_lock);
		if (recg_buf == NULL || recg_buf->frame == NULL) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}
		rgb_frame = recg_buf->frame;
		face_info = &recg_buf->face_info;
		face_rect = &face_info->rect;
		face_id = face_rect->id;
		pthread_mutex_lock(&svc->tracking_lock);
		face = tracking_get(svc, face_id);
		if (!face) {
			ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}
		if (face->living) {
			ALOGI("frame %p face[%d] already living checked",
				rgb_frame, face_id);
			face_info->feat.living_body = 1;
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}
		pthread_mutex_unlock(&svc->tracking_lock);
		living_in.imgYuv.size.width = face_svc_conf->svc.rgb_buffer_width;
		living_in.imgYuv.size.height = face_svc_conf->svc.rgb_buffer_height;
		living_in.yuvFaceLoc.x = face_rect->rect[0];
		living_in.yuvFaceLoc.y = face_rect->rect[1];
		living_in.yuvFaceLoc.width = face_rect->rect[2] - face_rect->rect[0];
		living_in.yuvFaceLoc.height = face_rect->rect[3] - face_rect->rect[1];
		ALOGD_IF(0, "face[%d] in x:%d, y:%d, w:%d, h:%d",
				face_id,
				living_in.yuvFaceLoc.x,
				living_in.yuvFaceLoc.y,
				living_in.yuvFaceLoc.width,
				living_in.yuvFaceLoc.height);
		dt_in.target_sz.width = face_svc_conf->svc.rgb_buffer_width;
		dt_in.target_sz.height = face_svc_conf->svc.rgb_buffer_height;
		dt_in.yuv.y = (void *)rgb_frame->viraddr[0]
			+ rgb_frame->width * (rgb_frame->height
			- face_svc_conf->svc.rgb_buffer_height) / 2;
		dt_in.yuv.uv = (void *)rgb_frame->viraddr[1]
			+ rgb_frame->width * (rgb_frame->height
			- face_svc_conf->svc.rgb_buffer_height) / 4;
		dt_in.yuv.y_phy = (void *)rgb_frame->phyaddr[0]
			+ rgb_frame->width * (rgb_frame->height
			- face_svc_conf->svc.rgb_buffer_height) / 2;
		dt_in.yuv.uv_phy = (void *)rgb_frame->phyaddr[1]
			+ rgb_frame->width * (rgb_frame->height
			- face_svc_conf->svc.rgb_buffer_height) / 4;
		dt_in.yuv.size.width = living_in.imgYuv.size.width;
		dt_in.yuv.size.height = living_in.imgYuv.size.height;
		dt_in.dt_roi.x = living_in.yuvFaceLoc.x;
		CLIPMIN(dt_in.dt_roi.x, 0);
		dt_in.dt_roi.width = living_in.yuvFaceLoc.width;
		CLIPMAX(dt_in.dt_roi.width, living_in.imgYuv.size.width - dt_in.dt_roi.x);
		dt_in.dt_roi.y = living_in.yuvFaceLoc.y;
		CLIPMIN(dt_in.dt_roi.y, 0);
		dt_in.dt_roi.height = living_in.yuvFaceLoc.height;
		CLIPMAX(dt_in.dt_roi.height,
				living_in.imgYuv.size.height - dt_in.dt_roi.y);
		ALOGD_IF(0, "face[%d] dt in x:%d, y:%d, w:%d, h:%d",
				face_id,
				dt_in.dt_roi.x,
				dt_in.dt_roi.y,
				dt_in.dt_roi.width,
				dt_in.dt_roi.height);
		dt_in.enlarge_x = 0.05;
		dt_in.enlarge_y = 0.04;
		face_info->living_begin = get_systime_us();

		pthread_mutex_lock(&svc->tracking_lock);
		face = tracking_get(svc, face_id);
		if (!face) {
			ALOGE("%s tracking_get failed. usualy, should not happen", __func__);
			pthread_mutex_unlock(&svc->tracking_lock);
			goto free_res;
		}

		int device_id = face_svc_conf->svc.device_id;
		int living_total_score = update_face_living_score_rgb(svc, face, recg_buf, face_rect, dt_in, device_id);
		pthread_mutex_unlock(&svc->tracking_lock);

		face_info->living_end = get_systime_us();
		ALOGI_IF(face_svc_conf->debug.profile,
				"face[%d] living body checking anti-spoofing toke %lld (us)",
				face_id, face_info->living_end - face_info->living_begin);

		ALOGD_IF(face_svc_conf->debug.debug_log, "frame: %p face[%d] living_total_score: %d",
			rgb_frame, face_id, living_total_score);
		pthread_mutex_lock(&svc->tracking_lock);
		face = tracking_get(svc, face_id);
		if (face) {
			/* dump rgb */
			face->time = rgb_frame->time_stamp;
			if (face_svc_conf->svc.dump_debug_rgb_living > 0) {
				face->living_count++;
				if (!(face->living_count % face_svc_conf->svc.dump_debug_rgb_living)) {
					save_rgb_living_frame(svc, face, recg_buf, living_total_score,
						dt_in.dt_roi.x,
						dt_in.dt_roi.y,
						dt_in.dt_roi.width,
						dt_in.dt_roi.height);
				}
			}

			if (living_total_score >= face_svc_conf->svc.living_score_threshold) {
				face_info->feat.living_body = 1;
				tracking_set_living(face, &face_info->feat);
				if (face_svc_conf->svc.dump_living == 1)
					save_record(svc, face, recg_buf, living);
				if (face->recg) {
					face_det_living_rgb_feats(svc, face, rgb_frame);
				}
			} else {
				face_info->feat.living_body = 0;
				tracking_set_living(face, &face_info->feat);
				long long cur_time = get_systime_us();
				if (face_svc_conf->svc.det_timeout && cur_time > face->timeout_time) {
					ALOGD_IF(face_svc_conf->debug.debug_log, "rgb living timeout_time");
					face->timeout = 1;
				}
			}
			ALOGD_IF(face_svc_conf->debug.debug_log, "frame: %p face[%d] living final score: %d, lable: %d",
				rgb_frame, face_id, face->living_score, face_info->feat.living_body);
		}
		pthread_mutex_unlock(&svc->tracking_lock);

free_res:
		pthread_mutex_lock(&svc->tracking_lock);
		//free_rgb_recg_buf(svc, &recg_buf);
		if (face && face->feat.living_body == 1 && face->recg) {
			free_rgb_recg_buf_no_cb(svc, &recg_buf);
		}else{
			free_rgb_recg_buf(svc, &recg_buf);
		}
		free_frame(&rgb_frame);
		pthread_mutex_unlock(&svc->tracking_lock);
	}
	return NULL;
}

int face_register_nir_det_cb(face_nir_det_cb cb)
{
	if (!g_svc) {
		ALOGE("%s g_svc is null", __func__);
		return -1;
	}
	if (!cb) {
		ALOGE("%s cb cannot be NULL\n",  __func__);
		return -1;
	}
	g_svc->nir_det_cb = cb;
	return 0;
}

int face_nir_det_start(void)
{
	if (!g_svc) {
		ALOGE("%s g_svc is null", __func__);
		return -1;
	}
	pthread_mutex_lock(&g_svc->nir_det_lock);
	g_svc->nir_det = 1;
	pthread_mutex_unlock(&g_svc->nir_det_lock);
	return 0;
}

int face_nir_det_stop(void)
{
	if (!g_svc) {
		ALOGE("%s g_svc is null", __func__);
		return -1;
	}
	pthread_mutex_lock(&g_svc->nir_det_lock);
	g_svc->nir_det = 0;
	pthread_mutex_unlock(&g_svc->nir_det_lock);
	return 0;
}

static face_frame_t *get_nir_det_frame(face_svc_t *svc)
{
	face_frame_t *nir_frame = NULL;

	pthread_mutex_lock(&svc->nir_frame_lock);
	while (!empty_queue(&svc->nir_frame_queue)) {
		nir_frame = (face_frame_t *)de_queue(&svc->nir_frame_queue);
		break;
	}
	pthread_mutex_unlock(&svc->nir_frame_lock);

	return nir_frame;
}

static void *face_nir_det_thread(void *arg)
{
	int i = 0;
	face_svc_t *svc = (face_svc_t *)arg;
	face_frame_t *nir_frame = NULL;
	nirdt_in_t dt_in = {0};
	nirdt_out_t *dt_out = NULL;
	nir_det_rects_t cb_dt_rects = {0};

	prctl(PR_SET_NAME, "face-nir-det");

	while (1) {
		pthread_mutex_lock(&svc->nir_det_lock);
		pthread_cond_wait(&svc->nir_det_cond, &svc->nir_det_lock);
		pthread_mutex_unlock(&svc->nir_det_lock);
		nir_frame = get_nir_det_frame(svc);
		if (nir_frame) {
			nir_frame->ref = 1;
			/* nir det face */
			dt_in.yuv.y = (void *)nir_frame->viraddr[0];
			dt_in.yuv.uv = (void *)nir_frame->viraddr[1];
			dt_in.yuv.y_phy = (void *)nir_frame->phyaddr[0];
			dt_in.yuv.uv_phy = (void *)nir_frame->phyaddr[1];
			dt_in.yuv.size.width = nir_frame->width;
			dt_in.yuv.size.height = nir_frame->height;
			dt_in.dt_roi.x = 0;
			dt_in.dt_roi.y = 0;
			dt_in.dt_roi.width = face_svc_conf->svc.nir_buffer_width;
			dt_in.dt_roi.height = face_svc_conf->svc.nir_buffer_height;
			dt_in.target_sz.width = face_svc_conf->svc.nir_buffer_width;
			dt_in.target_sz.height = face_svc_conf->svc.nir_buffer_height;
			dt_in.enlarge_x = 0.05;
			dt_in.enlarge_y = 0.04;
			dt_in.detect_whole_image = 1;

			nir_frame->det_begin = get_systime_us();
			nirdt_detect(svc->nirdt_hdl, &dt_in, &dt_out);
			nir_frame->det_end = get_systime_us();
			ALOGI_IF(face_svc_conf->debug.profile,
				"nir det frame %p detecting toke %lld",
				nir_frame, nir_frame->det_end - nir_frame->det_begin);

			if (dt_out->size == 0)
				ALOGI_IF(face_svc_conf->debug.profile, "nir no face detected");
			else {
				ALOGI_IF(face_svc_conf->debug.profile, "nir face detected: %d",
					dt_out->size);
				if (svc->nir_det_cb) {
					cb_dt_rects.p_rect = malloc(sizeof(nir_det_rect_t) *
						dt_out->size);
					if (!cb_dt_rects.p_rect) {
						ALOGE("malloc nir_det_rect_t failed, errno: %d (%s)\n",
							errno, strerror(errno));
						goto free_res;
					}
					for (i = 0; i < dt_out->size; i++) {
						cb_dt_rects.p_rect[i].x = dt_out->p[i].roi.x;
						cb_dt_rects.p_rect[i].y = dt_out->p[i].roi.y;
						cb_dt_rects.p_rect[i].width = dt_out->p[i].roi.width;
						cb_dt_rects.p_rect[i].height = dt_out->p[i].roi.height;
						cb_dt_rects.p_rect[i].score = dt_out->p[i].score;
					}
					cb_dt_rects.size = dt_out->size;
					svc->nir_det_cb(nir_frame, &cb_dt_rects);
					free(cb_dt_rects.p_rect);
					cb_dt_rects.p_rect = NULL;
				}
			}
free_res:
			free_frame(&nir_frame);
		}
	}
	return NULL;
}

#if 0
static void face_living_init(void)
{
	LivingIniOut livingIniOut;
	LivingIniIn livingIniIn;

	livingIniIn.yuvSize.width = RGB_SIZE_WIDTH;
	livingIniIn.yuvSize.height = RGB_SIZE_HEIGHT;
	livingIniIn.nirSize.width = NIR_SIZE_WIDTH;
	livingIniIn.nirSize.height = NIR_SIZE_HEIGHT;
	CreatLiving(&livingIniOut, &livingIniIn);
}
#endif

void *face_svc_init(void)
{
	face_svc_t *svc;
	/*
	int rgb_rotation = 0;
	int nir_rotation = 0;
	*/
	int ret;

	ALOGV("enter %s\n", __func__);
	svc = (face_svc_t *)malloc(sizeof(face_svc_t));
	if (!svc) {
		ALOGE("malloc face_svc_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	memset(svc, 0, sizeof(face_svc_t));

	g_svc = svc;

	if (create_dir(DEFAULT_RECORD_DIR))
		return NULL;

	if (face_svc_load_conf()) {
		ALOGE("face svc load conf failed");
		return NULL;
	}

	face_svc_load_params(svc);

	face_ae_init();

	TAILQ_INIT(&svc->rgb_frame_queue);
	if (pthread_mutex_init(&svc->rgb_frame_lock, NULL)) {
		ALOGE("init rgb_frame_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->rgb_frame_cond, NULL)) {
		ALOGE("init rgb_frame_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	TAILQ_INIT(&svc->rgb_mirror_frame_queue);
	if (pthread_mutex_init(&svc->rgb_mirror_frame_lock, NULL)) {
		ALOGE("init rgb_mirror_frame_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->rgb_mirror_frame_cond, NULL)) {
		ALOGE("init rgb_mirror_frame_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	TAILQ_INIT(&svc->rgb_det_queue);
	if (pthread_mutex_init(&svc->rgb_det_lock, NULL)) {
		ALOGE("init rgb_det_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	if (pthread_mutex_init(&svc->pause_lock, NULL)) {
		ALOGE("init pause_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	TAILQ_INIT(&svc->rgb_recg_queue);
	if (pthread_mutex_init(&svc->rgb_recg_lock, NULL)) {
		ALOGE("init rgb_recg_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->rgb_recg_cond, NULL)) {
		ALOGE("init rgb_recg_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	TAILQ_INIT(&svc->rgb_cmp_queue);
	if (pthread_mutex_init(&svc->rgb_cmp_lock, NULL)) {
		ALOGE("init rgb_cmp_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->rgb_cmp_cond, NULL)) {
		ALOGE("init rgb_cmp_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	INIT_LIST_HEAD(&svc->tracking_list);
	if (pthread_mutex_init(&svc->tracking_lock, NULL)) {
		ALOGE("init tracking_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	TAILQ_INIT(&svc->nir_frame_queue);
	if (pthread_mutex_init(&svc->nir_frame_lock, NULL)) {
		ALOGE("init nir_frame_lock failed, errno: %d (%s)\n",
					errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->nir_frame_cond, NULL)) {
		ALOGE("init nir_frame_cond failed, errno: %d (%s)\n",
					errno, strerror(errno));
		return NULL;
	}

	TAILQ_INIT(&svc->rgb_living_queue);
	if (pthread_mutex_init(&svc->rgb_living_lock, NULL)) {
		ALOGE("init rgb_living_lock failed, errno: %d (%s)\n",
					errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->rgb_living_cond, NULL)) {
		ALOGE("init rgb_living_cond failed, errno: %d (%s)\n",
					errno, strerror(errno));
		return NULL;
	}

	TAILQ_INIT(&svc->record_queue);
	if (pthread_mutex_init(&svc->record_lock, NULL)) {
		ALOGE("init record_lock failed, errno: %d (%s)\n",
					errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->record_cond, NULL)) {
		ALOGE("init record_cond failed, errno: %d (%s)\n",
					errno, strerror(errno));
		return NULL;
	}

	if (pthread_mutex_init(&svc->block_api_lock, NULL)) {
		ALOGE("init block_api_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_mutex_init(&svc->face_lock, NULL)) {
		ALOGE("init face_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->face_cond, NULL)) {
		ALOGE("init face_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	if (pthread_mutex_init(&svc->face_capture_lock, NULL)) {
		ALOGE("init face_capture_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->face_capture_cond, NULL)) {
		ALOGE("init face_capture_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	if (pthread_mutex_init(&svc->zbar_lock, NULL)) {
		ALOGE("init face_capture_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	if (pthread_mutex_init(&svc->nir_det_lock, NULL)) {
		ALOGE("init nir_det_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->nir_det_cond, NULL)) {
		ALOGE("init nir_det_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	ALOGV("face media init");
	svc->media_hdl = face_media_create();
	if (!svc->media_hdl) {
		ALOGE("media create failed");
		return NULL;
	}

	ret = face_media_start(svc->media_hdl);
	if (ret) {
		ALOGE("face media start failed");
		return NULL;
	}

	/* need after face_media_start */
	ret = face_media_rec_init(svc->media_hdl);
	if (ret) {
		ALOGE("face media rec start failed");
		return NULL;
	}

	ALOGV("face nn init");
	if (nn_init(svc)) {
		ALOGE("face nn init failed");
		return NULL;
	}

	svc->face_feat = malloc(sizeof(nna_face_feature_t));
	if (!svc->face_feat) {
		ALOGE("malloc nna_face_feature_t failed, errno: %d (%s)",
			errno, strerror(errno));
		return NULL;
	}
	memset(svc->face_feat, 0, sizeof(nna_face_feature_t));

	if (pthread_mutex_init(&svc->face_feat_lock, NULL)) {
		ALOGE("init face_feat_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	/* create face service detect thread */
	ALOGV("create face service detect thread");
	if (pthread_create(&svc->face_det_thrd, NULL,
				face_det_thread, (void *)svc)) {
		ALOGE("create face service detect thread failed, "
			"errno: %d (%s)", errno, strerror(errno));
		return NULL;
	}

	ALOGV("create face service rgb mirror distribute thread");
	if (pthread_create(&svc->face_rgb_mirror_dist_thrd, NULL,
				face_rgb_mirror_dist_thread, (void *)svc)) {
		ALOGE("create face service detect thread failed, "
			"errno: %d (%s)", errno, strerror(errno));
		return NULL;
	}

	/* create face service compare thread */
	ALOGV("create face service compare thread");
	if (pthread_create(&svc->face_cmp_thrd, NULL,
				face_cmp_thread, (void *)svc)) {
		ALOGE("create face service compare thread failed, "
			"errno: %d (%s)", errno, strerror(errno));
		return NULL;
	}

	/* create face service living thread */
	ax_size_t nir_img_size;
	nir_img_size.width = face_svc_conf->svc.nir_buffer_width;
	nir_img_size.height = face_svc_conf->svc.nir_buffer_height;
	svc->nirdt_hdl = nirdt_create(nir_img_size);
	if (!svc->nirdt_hdl) {
		ALOGE("nir dt create failed");
		return NULL;
	}

#if 0
	face_living_init();
#else
	if (face_svc_conf->svc.living_check) {
		if (face_svc_conf->svc.living_check_mode == 0) {
			svc->nir_living_hdl = nir_open(
					face_svc_conf->svc.nir_buffer_width,
					face_svc_conf->svc.nir_buffer_height);
			if (!svc->nir_living_hdl) {
				ALOGE("nir living create failed");
				return NULL;
			}

			svc->nir_living_hdl_outdoors = nir_open_outdoors(
					face_svc_conf->svc.nir_buffer_width,
					face_svc_conf->svc.nir_buffer_height);
			if (!svc->nir_living_hdl_outdoors) {
				ALOGE("nir living outdoors create failed");
				return NULL;
			}

			if (face_svc_conf->svc.mask_living_enable) {
				svc->nir_living_hdl_mask = nir_open_mask(
						face_svc_conf->svc.nir_buffer_width,
						face_svc_conf->svc.nir_buffer_height);
				if (!svc->nir_living_hdl_mask) {
					ALOGE("nir living mask create failed");
					return NULL;
				}
			}

			svc->rgb_color_living_hdl = rgb_color_living_open(
					face_svc_conf->svc.rgb_buffer_width,
					face_svc_conf->svc.rgb_buffer_height);
			if (!svc->rgb_color_living_hdl) {
				ALOGE("RGB color living create failed");
				return NULL;
			}

			svc->rgb_living_hdl = rgb_living_open_new(
					face_svc_conf->svc.rgb_buffer_width,
					face_svc_conf->svc.rgb_buffer_height);
			if (!svc->rgb_living_hdl) {
				ALOGE("RGB living new create failed");
				return NULL;
			}
		} else if (face_svc_conf->svc.living_check_mode == 1) {

			svc->rgb_color_living_hdl = rgb_color_living_open(
					face_svc_conf->svc.rgb_buffer_width,
					face_svc_conf->svc.rgb_buffer_height);
			if (!svc->rgb_color_living_hdl) {
				ALOGE("RGB color living create failed");
				return NULL;
			}

			svc->rgb_living_hdl = rgb_living_open(
					face_svc_conf->svc.rgb_buffer_width,
					face_svc_conf->svc.rgb_buffer_height);
			if (!svc->rgb_living_hdl) {
				ALOGE("RGB living create failed");
				return NULL;
			}

		} else {
			ALOGE("unknow living_check_mode:%d ", face_svc_conf->svc.living_check_mode);
		}
	}
#endif
	if (face_svc_conf->svc.living_check) {
		if (face_svc_conf->svc.living_check_mode == 0) {
			if (pthread_create(&svc->face_living_thrd, NULL,
					face_living_thread, (void*) svc)) {
				ALOGE("create face service living thread failed, errno: %d (%s)\n",
					errno, strerror(errno));
				return NULL;
			}
		} else if (face_svc_conf->svc.living_check_mode == 1) {
			if (pthread_create(&svc->face_living_thrd, NULL,
					face_rgb_living_thread, (void*) svc)) {
				ALOGE("create face service living thread failed, errno: %d (%s)\n",
					errno, strerror(errno));
				return NULL;
			}
		} else {
			ALOGE("unknow living_check_mode:%d ", face_svc_conf->svc.living_check_mode);
		}
	}

	if (face_svc_conf->svc.zbar_enable) {
		if(!zbar_svc_init()) {
			ALOGE("zbar_svc_init failed");
			return NULL;
		}
	}

	if (face_svc_conf->svc.temp_measure_enable) {
		if(temp_measure_init()){
			ALOGE("temp_measure_init failed");
			return NULL;
		}
	}

	/* create face service record thread */
	ALOGV("create face service record thread");
	if (pthread_create(&svc->face_record_thrd, NULL,
				face_record_thread, (void *)svc)) {
		ALOGE("create face service record thread failed, errno: %d (%s)",
					errno, strerror(errno));
		return NULL;
	}

	/* create face nir det thread */
	ALOGV("create face nir det thread");
	if (pthread_create(&svc->nir_det_thrd, NULL,
				face_nir_det_thread, (void *)svc)) {
		ALOGE("create face nir det thread failed, errno: %d (%s)",
					errno, strerror(errno));
		return NULL;
	}

	return svc;
}

int face_svc_get_config(void *handle, face_config_t *config)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!config || !svc)
		return -1;

	config->living = face_svc_conf->svc.living_check;
	config->score_threshold = face_svc_conf->svc.score_threshold;

	return 0;
}

int face_svc_set_config(void *handle, face_config_t *config)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!config || !svc)
		return -1;

	if (config->score_threshold <= 0 || config->score_threshold >= 1) {
		ALOGE("service config failed, score_threshold(%f) error",
					config->score_threshold);
		return -1;
	}

	face_svc_conf->svc.living_check = config->living;
	face_svc_conf->svc.score_threshold = config->score_threshold;
	return 0;
}

int face_svc_set_config2(void *handle, face_config_t *config)
{
        int ret = 0;
        face_svc_t *svc = (face_svc_t *)handle;

        if (!config || !svc)
                return -1;

        if (config->score_threshold <= 0 || config->score_threshold >= 1) {
                ALOGE("service config failed, score_threshold(%f) error",
                                        config->score_threshold);
                return -1;
        }

        face_svc_conf->svc.living_check = config->living;
        face_svc_conf->svc.score_threshold = config->score_threshold;
        face_svc_conf->svc.min_face_pixel = config->min_face_pixel;
        face_svc_conf->svc.max_face_pixel = config->max_face_pixel;
        face_svc_conf->svc.max_track_faces = config->max_track_faces;

        ret = nn_face_para_cfg(face_svc_conf);
        if(ret) {
                ALOGE("%s starting NNA_SET_FACE_CFG failed, %d", __func__, ret);
                return -1;
        }
        return 0;
}

int face_set_dump_img_config(dump_img_config_t *config)
{
	face_svc_conf->svc.dump_save_img = config->dump_save_img;
	face_svc_conf->svc.dump_match_frame = config->dump_match_frame;
	face_svc_conf->svc.dump_match_face = config->dump_match_face;
	face_svc_conf->svc.dump_unmatch_frame = config->dump_unmatch_frame;
	face_svc_conf->svc.dump_unmatch_face = config->dump_unmatch_face;
	face_svc_conf->svc.dump_unqualified_frame = config->dump_unqualified_frame;
	face_svc_conf->svc.dump_unqualified_face = config->dump_unqualified_face;
	face_svc_conf->svc.det_timeout = config->det_timeout;
	ALOGD("dump_save_img: %d\n", face_svc_conf->svc.dump_save_img);
	ALOGD("dump_match_frame: %d\n", face_svc_conf->svc.dump_match_frame);
	ALOGD("dump_match_face: %d\n", face_svc_conf->svc.dump_match_face);
	ALOGD("dump_unmatch_frame: %d\n", face_svc_conf->svc.dump_unmatch_frame);
	ALOGD("dump_unmatch_face: %d\n", face_svc_conf->svc.dump_unmatch_face);
	ALOGD("dump_unqualified_frame: %d\n", face_svc_conf->svc.dump_unqualified_frame);
	ALOGD("dump_unqualified_face: %d\n", face_svc_conf->svc.dump_unqualified_face);
	ALOGD("det_timeout: %d\n", face_svc_conf->svc.det_timeout);
	return 0;
}

int face_get_dump_img_config(dump_img_config_t *config)
{
	config->dump_save_img = face_svc_conf->svc.dump_save_img;
	config->dump_match_frame = face_svc_conf->svc.dump_match_frame;
	config->dump_match_face = face_svc_conf->svc.dump_match_face;
	config->dump_unmatch_frame = face_svc_conf->svc.dump_unmatch_frame;
	config->dump_unmatch_face = face_svc_conf->svc.dump_unmatch_face;
	config->dump_unqualified_frame = face_svc_conf->svc.dump_unqualified_frame;
	config->dump_unqualified_face = face_svc_conf->svc.dump_unqualified_face;
	config->det_timeout = face_svc_conf->svc.det_timeout;
	return 0;
}

int face_get_max_track_faces(void *handle)
{
	ALOGD("[%s]max_track_faces = %d", __func__, face_svc_conf->svc.max_track_faces);
	return face_svc_conf->svc.max_track_faces;
}

int face_set_max_track_faces(void *handle, int count)
{
	ALOGD("[%s]max_track_faces = %d", __func__, count);
	face_svc_conf->svc.max_track_faces = count;
	return nn_set_max_track_faces(face_svc_conf->svc.max_track_faces);
}

int face_get_fquality_enable(void *handle)
{
	ALOGD("[%s]fquality_enable = %d", __func__, face_svc_conf->svc.fquality_enable);
	return face_svc_conf->svc.fquality_enable;
}

int face_set_fquality_enable(void *handle, int enable)
{
	ALOGD("[%s]fquality_enable = %d", __func__, enable);
	face_svc_conf->svc.fquality_enable = enable;
	return nn_set_fquality_enable(face_svc_conf->svc.fquality_enable);
}

int face_get_headpose_enable(void *handle)
{
	ALOGD("[%s]headpose_enable = %d", __func__, face_svc_conf->svc.headpose_enable);
	return face_svc_conf->svc.headpose_enable;
}

int face_set_headpose_enable(void *handle, int enable)
{
	ALOGD("[%s]headpose_enable = %d", __func__, enable);
	face_svc_conf->svc.headpose_enable = enable;
	return nn_set_headpose_enable(face_svc_conf->svc.headpose_enable);
}

int face_get_faceattr_enable(void *handle)
{
	ALOGD("[%s]faceattr_enable = %d", __func__, face_svc_conf->svc.faceattr_enable);
	return face_svc_conf->svc.faceattr_enable;
}

int face_set_faceattr_enable(void *handle, int enable)
{
	ALOGD("[%s]faceattr_enable = %d", __func__, enable);
	face_svc_conf->svc.faceattr_enable = enable;
	return nn_set_faceattr_enable(face_svc_conf->svc.faceattr_enable);
}

int face_get_use_fast_trak(void *handle)
{
	ALOGD("[%s]use_fast_track = %d", __func__, face_svc_conf->svc.use_fast_track);
	return face_svc_conf->svc.use_fast_track;
}

int face_set_use_fast_trak(void *handle, int use)
{
	ALOGD("[%s]use_fast_track = %d", __func__, use);
	face_svc_conf->svc.use_fast_track = use;
	return nn_set_use_fast_trak(face_svc_conf->svc.use_fast_track);
}

int face_get_det_mode(void *handle)
{
	ALOGD("[%s]nn_det_mode = %d", __func__, face_svc_conf->svc.nn_det_mode);
	return face_svc_conf->svc.nn_det_mode;
}

int face_set_det_mode(void *handle, int mode)
{
	ALOGD("[%s]nn_det_mode = %d", __func__, mode);
	face_svc_conf->svc.nn_det_mode = mode;
	return nn_set_det_mode(face_svc_conf->svc.nn_det_mode);
}

int face_svc_reset_recg(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	pthread_mutex_lock(&svc->tracking_lock);
	tracking_clean(svc);
	pthread_mutex_unlock(&svc->tracking_lock);
	return 0;
}

int face_svc_set_mask_det_enable(void *handle, int enable)
{
	ALOGE("[%s]mask_det_enable = %d", __func__, enable);
	face_svc_conf->svc.mask_det_enable = enable;
	return nn_set_mask_det_enable(face_svc_conf->svc.mask_det_enable);
}

int face_svc_get_mask_det_enable(void *handle)
{
	ALOGE("[%s]mask_det_enable = %d", __func__, face_svc_conf->svc.mask_det_enable);
	return face_svc_conf->svc.mask_det_enable;
}

int face_svc_set_mask_recg_enable(void *handle, int enable)
{
	ALOGE("[%s]mask_recg_enable = %d", __func__, enable);
	face_svc_conf->svc.mask_recg_enable = enable;
	return nn_set_mask_recg_enable(face_svc_conf->svc.mask_recg_enable);
}

int face_svc_get_mask_recg_enable(void *handle)
{
	ALOGE("[%s]mask_recg_enable = %d", __func__, face_svc_conf->svc.mask_recg_enable);
	return face_svc_conf->svc.mask_recg_enable;
}

int face_svc_set_recg_repeat(void *handle, int enable)
{
	int res = -1;
	ALOGE("[%s]recg_repeat = %d", __func__, enable);
	if ((enable == 0) || (enable == 1)){
		face_svc_conf->svc.recg_repeat = enable;
		res = 0;
	}else{
		res = -1;
	}
	return res;
}

int face_svc_get_recg_repeat(void *handle)
{
	ALOGE("[%s]recg_repeat = %d", __func__, face_svc_conf->svc.recg_repeat);
	return face_svc_conf->svc.recg_repeat;

}

int face_svc_set_recg_repeat_interval(void *handle, int interval)
{
	ALOGE("[%s]recg_repeat_interval = %d", __func__, interval);
	if (interval < 0){
		return -1;
	}
	face_svc_conf->svc.recg_repeat_interval = interval;
	return 0;
}

int face_svc_get_recg_repeat_interval(void *handle)
{
	ALOGE("[%s]recg_repeat_interval = %d", __func__, face_svc_conf->svc.recg_repeat_interval);
	return face_svc_conf->svc.recg_repeat_interval;
}


int face_svc_set_mask_score_threshold(void *handle, float threshold)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc)
		return -1;

	if (threshold <= 0 || threshold >= 1) {
		ALOGE("service config failed, mask_score_threshold(%f) error",
				threshold);
		return -1;
	}

	ALOGD("[%s]threshold = %f", __func__, threshold);
	face_svc_conf->svc.mask_score_threshold = threshold;
	return 0;
}

float face_svc_get_mask_score_threshold(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc)
		return -1;

	return face_svc_conf->svc.mask_score_threshold;
}

int face_svc_set_recg_rect(void *handle, int left, int top,
	int right, int bottom)
{
	face_svc_t *svc = (face_svc_t *)handle;
	//int ratio = 0;

	if (!svc)
		return -1;

	if (left < 0 || top < 0 || left > right || top > bottom ||
		right > face_svc_conf->svc.rgb_buffer_width ||
		bottom > face_svc_conf->svc.rgb_buffer_height) {
		ALOGE("[%s][%d] set failed error", __func__, __LINE__);
		return -1;
	}

	//ratio = face_svc_conf->svc.rgb_buffer_width * 1000 / face_svc_conf->media.rgb_preview_width;
	//if (ratio == face_svc_conf->svc.rgb_buffer_height * 1000 / face_svc_conf->media.rgb_preview_height) {
	if (face_svc_conf->svc.ratio_similar) {
		face_svc_conf->svc.recg_rect_left = face_svc_conf->svc.rgb_buffer_width * left / face_svc_conf->media.rgb_preview_width;
		face_svc_conf->svc.recg_rect_top = face_svc_conf->svc.rgb_buffer_height * top / face_svc_conf->media.rgb_preview_height;
		face_svc_conf->svc.recg_rect_right = face_svc_conf->svc.rgb_buffer_width * right / face_svc_conf->media.rgb_preview_width;
		face_svc_conf->svc.recg_rect_bottom = face_svc_conf->svc.rgb_buffer_height * bottom / face_svc_conf->media.rgb_preview_height;
	} else {
		face_svc_conf->svc.recg_rect_left = face_svc_conf->svc.rgb_buffer_width * left / face_svc_conf->media.rgb_preview_height;
		face_svc_conf->svc.recg_rect_top = face_svc_conf->svc.rgb_buffer_height * top / face_svc_conf->media.rgb_preview_width;
		face_svc_conf->svc.recg_rect_right = face_svc_conf->svc.rgb_buffer_width * right / face_svc_conf->media.rgb_preview_height;
		face_svc_conf->svc.recg_rect_bottom = face_svc_conf->svc.rgb_buffer_height * bottom / face_svc_conf->media.rgb_preview_width;
	}
	ALOGD("[%s]recg_rect_left = %d", __func__, face_svc_conf->svc.recg_rect_left);
	ALOGD("[%s]recg_rect_top = %d", __func__, face_svc_conf->svc.recg_rect_top);
	ALOGD("[%s]recg_rect_right = %d", __func__, face_svc_conf->svc.recg_rect_right);
	ALOGD("[%s]recg_rect_bottom = %d", __func__, face_svc_conf->svc.recg_rect_bottom);
	return 0;
}

int face_svc_set_temp_measure_rect(void *handle, int left, int top,
	int right, int bottom)
{
	face_svc_t *svc = (face_svc_t *)handle;
	//int ratio = 0;

	if (!svc)
		return -1;

	if (left < 0 || top < 0 || left > right || top > bottom ||
		right > face_svc_conf->svc.rgb_buffer_width ||
		bottom > face_svc_conf->svc.rgb_buffer_height) {
		ALOGE("[%s][%d] set failed error", __func__, __LINE__);
		return -1;
	}

	//ratio = face_svc_conf->svc.rgb_buffer_width * 1000 / face_svc_conf->media.rgb_preview_width;
	//if (ratio == face_svc_conf->svc.rgb_buffer_height * 1000 / face_svc_conf->media.rgb_preview_height) {
	if (face_svc_conf->svc.ratio_similar) {
		face_svc_conf->svc.temp_rect_left = face_svc_conf->svc.rgb_buffer_width * left / face_svc_conf->media.rgb_preview_width;
		face_svc_conf->svc.temp_rect_top = face_svc_conf->svc.rgb_buffer_height * top / face_svc_conf->media.rgb_preview_height;
		face_svc_conf->svc.temp_rect_right = face_svc_conf->svc.rgb_buffer_width * right / face_svc_conf->media.rgb_preview_width;
		face_svc_conf->svc.temp_rect_bottom = face_svc_conf->svc.rgb_buffer_height * bottom / face_svc_conf->media.rgb_preview_height;
	} else {
		face_svc_conf->svc.temp_rect_left = face_svc_conf->svc.rgb_buffer_width * left / face_svc_conf->media.rgb_preview_height;
		face_svc_conf->svc.temp_rect_top = face_svc_conf->svc.rgb_buffer_height * top / face_svc_conf->media.rgb_preview_width;
		face_svc_conf->svc.temp_rect_right = face_svc_conf->svc.rgb_buffer_width * right / face_svc_conf->media.rgb_preview_height;
		face_svc_conf->svc.temp_rect_bottom = face_svc_conf->svc.rgb_buffer_height * bottom / face_svc_conf->media.rgb_preview_width;
	}
	ALOGD("[%s]temp_rect_left = %d", __func__, face_svc_conf->svc.temp_rect_left);
	ALOGD("[%s]temp_rect_top = %d", __func__, face_svc_conf->svc.temp_rect_top);
	ALOGD("[%s]temp_rect_right = %d", __func__, face_svc_conf->svc.temp_rect_right);
	ALOGD("[%s]temp_rect_bottom = %d", __func__, face_svc_conf->svc.temp_rect_bottom);
	return 0;
}

int face_svc_set_det_distance(int cm)
{
	int min_face_pixel = 0;

	if (cm < 0)
		return -1;

	min_face_pixel = ((face_svc_conf->svc.pixel_distance_k) * cm + face_svc_conf->svc.pixel_distance_b);
	min_face_pixel = ALIGN_DOWN(min_face_pixel, 2);
	ALOGD("[%s]min_face_pixel = %d", __func__, min_face_pixel);
	if (min_face_pixel < 40 || (min_face_pixel > face_svc_conf->svc.max_face_pixel)){
		return -1;
	}

	face_svc_conf->svc.min_face_pixel = min_face_pixel;
	return nn_set_min_face_pixel(min_face_pixel);
}

int face_svc_set_min_face_pixel(int min_face_pixel)
{
	ALOGD("[%s]min_face_pixel = %d", __func__, min_face_pixel);
	if (min_face_pixel < 40 || (min_face_pixel > face_svc_conf->svc.max_face_pixel)){
		return -1;
	}

	face_svc_conf->svc.min_face_pixel = min_face_pixel;
	return nn_set_min_face_pixel(min_face_pixel);
}

int face_svc_set_min_reg_face_pixel(int reg_face_pixel)
{
	ALOGD("[%s]reg_face_pixel = %d", __func__, reg_face_pixel);
	if (reg_face_pixel < 40 || (reg_face_pixel > face_svc_conf->svc.max_face_pixel)){
		return -1;
	}

	face_svc_conf->svc.min_reg_face_pixel = reg_face_pixel;
	return 0;
}

int face_svc_set_living_score_threshold(int threshold)
{
	ALOGD("[%s]threshold = %d", __func__, threshold);
	face_svc_conf->svc.living_score_threshold = threshold;
	return 0;
}

int face_svc_set_exp_bias(int id, int exp_bias)
{
	return set_exp_bias(id, exp_bias);
}

int face_svc_set_temp_measure(int enable)
{
	int res = -1;
	if (enable == 0 || (enable == 1)){
		if (enable){
			temp_measure_init();
		}
		face_svc_conf->svc.temp_measure_enable = enable;
		res = 0;
	}
	return res;
}

int face_svc_get_recg_rect(void *handle, int *left, int *top,
	int *right, int *bottom)
{
	face_svc_t *svc = (face_svc_t *)handle;
	if (!svc)
		return -1;

	*left = face_svc_conf->svc.recg_rect_left;
	*top = face_svc_conf->svc.recg_rect_top;
	*right = face_svc_conf->svc.recg_rect_right;
	*bottom = face_svc_conf->svc.recg_rect_bottom;
	return 0;
}

int face_svc_get_temp_measure_rect(void *handle, int *left, int *top,
	int *right, int *bottom)
{
	face_svc_t *svc = (face_svc_t *)handle;
	if (!svc)
		return -1;

	*left = face_svc_conf->svc.temp_rect_left;
	*top = face_svc_conf->svc.temp_rect_top;
	*right = face_svc_conf->svc.temp_rect_right;
	*bottom = face_svc_conf->svc.temp_rect_bottom;
	return 0;
}

int face_svc_set_recg_face(void *handle, face_attr_t *attr)
{
	face_svc_t *svc = (face_svc_t *)handle;
	if (!svc)
		return -1;

	face_svc_conf->svc.recg_male = attr->male;
	face_svc_conf->svc.recg_wear_glasses = attr->wear_glasses;
	face_svc_conf->svc.recg_wear_hat = attr->wear_hat;
	face_svc_conf->svc.recg_left_eye_cover = attr->left_eye_cover;
	face_svc_conf->svc.recg_right_eye_cover = attr->right_eye_cover;
	face_svc_conf->svc.recg_mouse_cover = attr->mouse_cover;
	face_svc_conf->svc.recg_mask = attr->mask;
	return 0;
}

int face_svc_get_recg_face(void *handle, face_attr_t *attr)
{
	face_svc_t *svc = (face_svc_t *)handle;
	if (!svc)
		return -1;

	attr->male = face_svc_conf->svc.recg_male;
	attr->wear_glasses = face_svc_conf->svc.recg_wear_glasses;
	attr->wear_hat = face_svc_conf->svc.recg_wear_hat;
	attr->left_eye_cover = face_svc_conf->svc.recg_left_eye_cover;
	attr->right_eye_cover = face_svc_conf->svc.recg_right_eye_cover;
	attr->mouse_cover = face_svc_conf->svc.recg_mouse_cover;
	attr->mask = face_svc_conf->svc.recg_mask;
	return 0;
}

static int face_svc_rgb_frame_cb(void *p, face_frame_t *frame)
{
	face_svc_t *svc = (face_svc_t *)p;
	int ret;
	static unsigned int idx = 1;

	if (!frame || !p)
		return -1;

	if (face_svc_conf->svc.dump_fps) {
		static long long start = 0;
		long long end = get_systime_us();
		static int fps = 0;

		fps++;
		if (end - start > 1000000) {
			ALOGI("rgb frame %d fps", fps);
			start = get_systime_us();
			fps = 0;
		}
	}

	/* ALOGV("rgb cb, vir: 0x%x, width: %d, height: %d, ts: %lld\n", frame->viraddr,
			frame->width, frame->height, frame->time_stamp); */

	if (idx++ % face_svc_conf->svc.det_step) {
		//ALOGV("%s dropped frame: %d", __func__, idx);
		face_media_return_frame(frame);
		return 0;
	}

	/* for calculating time consuming of each algorithm module */
	frame->type = FACE_FRAME_TYPE_RGB;
	frame->det_begin = -1;
	frame->det_end = -1;
	frame->recg_check_cnt = 0;
	frame->living_check_cnt = 0;

	pthread_mutex_lock(&svc->rgb_frame_lock);
	ret = en_queue(&svc->rgb_frame_queue, frame);
	pthread_cond_signal(&svc->rgb_frame_cond);
	pthread_mutex_unlock(&svc->rgb_frame_lock);

	return ret;
}

static int face_svc_rgb_mirror_frame_cb(void *p, face_frame_t *frame)
{
	face_svc_t *svc = (face_svc_t *)p;
	int ret;
	static unsigned int idx = 1;

	if (!frame || !p)
		return -1;

	if (face_svc_conf->svc.dump_fps) {
		static long long start = 0;
		long long end = get_systime_us();
		static int fps = 0;

		fps++;
		if (end - start > 1000000) {
			ALOGI("rgb frame %d fps", fps);
			start = get_systime_us();
			fps = 0;
		}
	}

	/* ALOGV("rgb cb, vir: 0x%x, width: %d, height: %d, ts: %lld\n", frame->viraddr,
			frame->width, frame->height, frame->time_stamp); */

	if (idx++ % face_svc_conf->svc.det_step) {
		//ALOGV("%s dropped frame: %d", __func__, idx);
		face_media_return_frame(frame);
		return 0;
	}

	/* for calculating time consuming of each algorithm module */
	frame->type = FACE_FRAME_TYPE_RGB_MIRROR;
	frame->det_begin = -1;
	frame->det_end = -1;
	frame->recg_check_cnt = 0;
	frame->living_check_cnt = 0;

	pthread_mutex_lock(&svc->rgb_mirror_frame_lock);
	ret = en_queue(&svc->rgb_mirror_frame_queue, frame);
	pthread_cond_signal(&svc->rgb_mirror_frame_cond);
	pthread_mutex_unlock(&svc->rgb_mirror_frame_lock);

	return ret;
}

static int face_svc_nir_frame_cb(void *p, face_frame_t *frame)
{
	face_svc_t *svc = (face_svc_t *)p;
	int ret;

	if (!frame || !svc)
		return -1;

	if (face_svc_conf->svc.dump_fps) {
		static long long start = 0;
		long long end = get_systime_us();
		static int fps = 0;

		fps++;
		if (end - start > CLOCKS_PER_SEC) {
			ALOGI("nir frame %d fps", fps);
			start = get_systime_us();
			fps = 0;
		}
	}

	if ((!face_svc_conf->svc.living_check && !svc->nir_det)
				|| face_svc_conf->svc.living_check_mode) {
		face_media_return_frame(frame);
		return 0;
	}

	/* ALOGV("nir cb, vir: 0x%x, width: %d, height: %d, ts: %lld\n", frame->viraddr,
			frame->width, frame->height, frame->time_stamp); */

	/* for calculating time consuming of each algorithm module */
	frame->type = FACE_FRAME_TYPE_NIR;
	frame->det_begin = -1;
	frame->det_end = -1;

	pthread_mutex_lock(&svc->nir_frame_lock);
	ret = en_queue(&svc->nir_frame_queue, frame);
	pthread_mutex_unlock(&svc->nir_frame_lock);

	pthread_mutex_lock(&svc->nir_det_lock);
	if (svc->nir_det)
		pthread_cond_signal(&svc->nir_det_cond);
	pthread_mutex_unlock(&svc->nir_det_lock);

	return ret;
}

int face_take_picture(void *handle, enum face_camera_id camera_id,
						p_face_img_cb f_cb)
{
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGV("enter %s\n", __func__);
	if (!handle)
		return -1;

	return face_media_take_picture(svc->media_hdl, camera_id, f_cb);
}

int face_take_picture_cb(enum face_camera_id camera_id,
	p_face_img_cb f_cb)
{
	face_svc_t *svc = (face_svc_t *)g_svc;

	if (!svc)
		return -1;

	ALOGV("enter %s\n", __func__);
	return face_media_take_picture(svc->media_hdl, camera_id, f_cb);
}

int face_take_picture_file(enum face_camera_id camera_id, char *file)
{
	face_svc_t *svc = (face_svc_t *)g_svc;

	if (!svc)
		return -1;

	ALOGV("enter %s\n", __func__);
	return face_media_take_picture_file(svc->media_hdl, camera_id, file);
}

int face_svc_start_rec(enum face_camera_id camera_id, enc_para_t *enc_para,
	const char *filename)
{
	face_svc_t *svc = (face_svc_t *)g_svc;
	ALOGV("enter %s\n", __func__);
	if (!svc)
		return -1;

	return face_media_rec_start(svc->media_hdl, camera_id, enc_para, filename);
}

int face_svc_stop_rec(enum face_camera_id camera_id)
{
	face_svc_t *svc = (face_svc_t *)g_svc;
	ALOGV("enter %s\n", __func__);
	if (!svc)
		return -1;

	return face_media_rec_stop(svc->media_hdl, camera_id);
}

int face_return_frame(void *handle, face_frame_t *f_frame)
{
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGV("enter %s\n", __func__);
	if (!svc)
		return -1;

	return face_media_return_frame(f_frame);
}

int face_get_camera_config(void *handle, enum face_camera_id camera_id,
						face_camera_cfg_t *cfg)
{
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGV("enter %s\n", __func__);
	if (!handle)
		return -1;

	return face_media_get_camera_config(svc->media_hdl, camera_id, cfg);
}

int face_svc_start(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	int ret;

	ALOGV("enter %s\n", __func__);
	ret = face_media_set_cb(svc->media_hdl, FACE_CAMERA_RGB, svc,
			face_svc_rgb_frame_cb);
	if (ret) {
		ALOGE("face_media_rgb_set_cb failed");
		return -1;
	}

	ret = face_media_set_cb(svc->media_hdl, FACE_CAMERA_RGB_MIRROR, svc,
			face_svc_rgb_mirror_frame_cb);
	if (ret) {
		ALOGE("face_media_rgb_mirror_set_cb failed");
		return -1;
	}

	ret = face_media_set_cb(svc->media_hdl, FACE_CAMERA_NIR, svc,
			face_svc_nir_frame_cb);
	if (ret) {
		ALOGE("face_media_nir_set_cb failed");
		return -1;
	}

	return 0;
}

int face_svc_pause(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc)
		return -1;
	svc->pause = 1;

	return 0;
}

int face_svc_resume(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc)
		return -1;
	svc->pause = 0;

	return 0;
}

int face_svc_pause_status(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc)
		return -1;

	return svc->pause;
}

int face_svc_stop(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	int ret;

	ALOGV("enter %s\n", __func__);
	ret = face_media_rec_destroy(svc->media_hdl);
	if (ret) {
		ALOGE("face media rec stop failed");
		return -1;
	}

	ret = face_media_stop(svc->media_hdl);
	if (ret) {
		ALOGE("face media stop failed");
		return -1;
	}

	return 0;
}

int face_svc_deinit(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	int ret;

	ALOGV("enter %s\n", __func__);

	face_ae_exit();

	nn_deinit();

	if (svc->nirdt_hdl) {
		nirdt_release(svc->nirdt_hdl);
		svc->nirdt_hdl = NULL;
	}
	if (svc->nir_living_hdl) {
		nir_close(svc->nir_living_hdl);
		svc->nir_living_hdl = NULL;
	}
	if (svc->nir_living_hdl_outdoors) {
		nir_close(svc->nir_living_hdl_outdoors);
		svc->nir_living_hdl_outdoors = NULL;
	}
	if (svc->nir_living_hdl_mask) {
		nir_close(svc->nir_living_hdl_mask);
		svc->nir_living_hdl_mask = NULL;
	}
	if (svc->rgb_color_living_hdl) {
		rgb_living_close(svc->rgb_color_living_hdl);
		svc->rgb_color_living_hdl = NULL;
	}

	if (svc->rgb_living_hdl) {
		rgb_living_close(svc->rgb_living_hdl);
		svc->rgb_living_hdl = NULL;
	}

	if (face_svc_conf->svc.zbar_enable) {
		zbar_svc_deinit();
	}

	if (face_svc_conf->svc.temp_measure_enable) {
		temp_measure_deinit();
	}

	ret = face_media_destroy(svc->media_hdl);
	if (ret)
		ALOGE("face media destroy failed");

	if (svc->face_feat->score) {
		free(svc->face_feat->score);
		svc->face_feat->score = NULL;
	}
	if (svc->face_feat) {
		free(svc->face_feat);
		svc->face_feat = NULL;
	}
	if (svc) {
		free(svc);
		svc = NULL;
	}

	/* free face conf data */
	face_svc_free_conf();

	return 0;
}

void *face_rgb_hdl(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	struct face_media *media = (struct face_media *)(svc->media_hdl);

	ALOGD("svc->media_hdl:%p", svc->media_hdl);

	return (media->rgb_hdl);
}

void *face_nir_hdl(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	struct face_media *media = (struct face_media *)(svc->media_hdl);

	return (media->nir_hdl);
}



int face_svc_pause_nir_preview(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	return face_media_stop_nir_preview(svc->media_hdl);
}

int face_svc_resume_nir_preview(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	int ret = 0;
	ret = face_media_start_nir_preview(svc->media_hdl,
		face_svc_conf->media.nir_preview_x, face_svc_conf->media.nir_preview_y,
		face_svc_conf->media.nir_preview_width, face_svc_conf->media.nir_preview_height,
		face_svc_conf->media.nir_preview_rotation,
		VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO);
	return ret;
}

void *face_media_hdl(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	struct face_media *media = (struct face_media *)(svc->media_hdl);

	return media;
}

int face_svc_start_rgb_preview(void *handle, int x, int y,
			int width, int height, int rot, int mode)
{
	face_svc_t *svc = (face_svc_t *)handle;
	struct face_media *media;

	if (!svc)
		return -EINVAL;

	media = (struct face_media *)(svc->media_hdl);
	if (!media)
		return -EINVAL;

	return face_media_start_rgb_preview(media, x, y, width, height, rot, mode);
}

int face_svc_stop_rgb_preview(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	struct face_media *media;

	if (!svc)
		return -EINVAL;

	media = (struct face_media *)(svc->media_hdl);
	if (!media)
		return -EINVAL;

	return face_media_stop_rgb_preview(media);
}

int face_svc_start_nir_preview(void *handle, int x, int y,
	int width, int height, int rot, int mode)
{
	face_svc_t *svc = (face_svc_t *)handle;
	struct face_media *media;

	if (!svc)
		return -EINVAL;

	media = (struct face_media *)(svc->media_hdl);
	if (!media)
		return -EINVAL;

	return face_media_start_nir_preview(media, x, y, width, height, rot, mode);
}

int face_svc_stop_nir_preview(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	struct face_media *media;

	if (!svc)
		return -EINVAL;

	media = (struct face_media *)(svc->media_hdl);
	if (!media)
		return -EINVAL;

	return face_media_stop_nir_preview(media);
}

int face_svc_start_preview(enum face_camera_id id, int x, int y,
	int width, int height, int rot, int mode)
{
	face_svc_t *svc = (face_svc_t *)g_svc;
	struct face_media *media;

	if (!svc)
		return -EINVAL;

	media = (struct face_media *)(svc->media_hdl);
	if (!media)
		return -EINVAL;
	switch (id) {
		case FACE_CAMERA_RGB:
			return face_media_start_rgb_preview(media, x, y, width, height,
				rot, mode);
		case FACE_CAMERA_NIR:
			return face_media_start_nir_preview(media, x, y, width, height,
				rot, mode);
		case FACE_CAMERA_MAX:
		default:
			ALOGE("invalid face_camera_id %d", id);
			return -EINVAL;
	}
}

int face_svc_stop_preview(enum face_camera_id id)
{
	face_svc_t *svc = (face_svc_t *)g_svc;
	struct face_media *media;

	if (!svc)
		return -EINVAL;

	media = (struct face_media *)(svc->media_hdl);
	if (!media)
		return -EINVAL;

	switch (id) {
		case FACE_CAMERA_RGB:
			return face_media_stop_rgb_preview(media);
		case FACE_CAMERA_NIR:
			return face_media_stop_nir_preview(media);
		case FACE_CAMERA_MAX:
		default:
			ALOGE("invalid face_camera_id %d", id);
			return -EINVAL;
	}
}

int face_register_det_cb(void *handle, face_det_cb cb)
{
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGV("enter %s\n", __func__);

	if (!svc) {
		ALOGE("face service not init\n");
		return -1;
	}

	if (!cb) {
		ALOGE("cb cannot be NULL\n");
		return -1;
	}

	svc->det_cb = cb;

	return 0;
}

int face_register_recg_cb(void *handle, face_recg_cb cb)
{
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGV("enter %s\n", __func__);
	if (!svc) {
		ALOGE("face service not init\n");
		return -1;
	}

	if (!cb) {
		ALOGE("cb cannot be NULL\n");
		return -1;
	}

	svc->recg_cb = cb;

	return 0;
}

int face_register_record_cb(void *handle, face_record_cb cb)
{
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGV("enter %s\n", __func__);
	if (!svc) {
		ALOGE("face service not init\n");
		return -1;
	}

	if (!cb) {
		ALOGE("cb cannot be NULL\n");
		return -1;
	}

	svc->record_cb = cb;

	return 0;
}

int face_register_face_feats(void *handle, const feats_t *feats)
{
	face_svc_t *svc = (face_svc_t *)handle;
	int ret = 0;

	ALOGV("enter %s\n", __func__);
	if (!svc || !feats) {
		ALOGE("%s invalid parameters", __func__);
		return -1;
	}

	if (feats->cnt < 0) {
		ALOGI("feats->feat: %p, feats->cnt: %d", feats->feat, feats->cnt);
		return -1;
	}

	ALOGI("registering %d faces version: %d", feats->cnt, feats->version);

	pthread_mutex_lock(&svc->face_feat_lock);

	svc->face_feat->features = feats->feat->values;
	svc->face_feat->features_norm = feats->norm;
	svc->face_feat->face_kpts = (float *)feats->kpts;
	svc->face_feat->features_num = feats->cnt;

	if (svc->face_feat->score) {
		free(svc->face_feat->score);
		svc->face_feat->score = NULL;
	}

	svc->feat_update = 1;
	svc->feat_version = feats->version;

	if (feats->cnt == 0)
		goto out;

	svc->face_feat->score = malloc(sizeof(float) * feats->cnt);
	if (!svc->face_feat->score) {
		ALOGE("%s realloc score failed", __func__);
		ret = -1;
		goto out;
	}

out:
	pthread_mutex_unlock(&svc->face_feat_lock);

	return ret;
}

static int _face_det(void *handle, face_frame_t *frame, enum face_frame_type type)
{
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGV("enter %s\n", __func__);

	frame->type = type;
	pthread_mutex_lock(&svc->rgb_frame_lock);
	en_queue(&svc->rgb_frame_queue, frame);
	pthread_cond_signal(&svc->rgb_frame_cond);
	pthread_mutex_unlock(&svc->rgb_frame_lock);

	return 0;
}

int face_det(void *handle, face_frame_t *frame)
{
	if (!handle || !frame) {
		ALOGE("%s parameters cannot be NULL, handle: %p, frame: %p",
			__func__, handle, frame);
		return -1;
	}

	return _face_det(handle, frame, FACE_FRAME_TYPE_API_DET);
}

int face_recg(void *handle, face_frame_t *frame)
{
	if (!handle || !frame) {
		ALOGE("%s parameters cannot be NULL, handle: %p, frame: %p",
			__func__, handle, frame);
		return -1;
	}

	return _face_det(handle, frame, FACE_FRAME_TYPE_API_RECG);
}

int face_recg_block(void *handle, face_frame_t *frame, int cnt,
		face_info_t *info)
{
	return face_recg_block2(handle, frame, cnt, info, 1);
}

int face_recg_block2(void *handle, face_frame_t *frame, int cnt,
		face_info_t *info, int compare)
{
	face_svc_t *svc = (face_svc_t *)handle;
	int ret;
	int i, j;
	float norm;
	face_feat_t *feat;

	if (!svc || !frame || cnt <= 0 || !info) {
		ALOGE("%s invalid parameters", __func__);
		return -1;
	}

	ALOGD_IF(1, "enter %s", __func__);

	pthread_mutex_lock(&svc->block_api_lock);
	pthread_mutex_lock(&svc->face_lock);

	svc->face_cnt = cnt;
	svc->face_info = info;
	svc->face_compare = compare;

	frame->type = FACE_FRAME_TYPE_API_RECG_BLOCK;
	frame->time_stamp = get_systime_us();

	pthread_mutex_lock(&svc->rgb_frame_lock);
	en_queue(&svc->rgb_frame_queue, frame);
	pthread_cond_signal(&svc->rgb_frame_cond);
	pthread_mutex_unlock(&svc->rgb_frame_lock);

	/* wait for done */
	pthread_cond_wait(&svc->face_cond, &svc->face_lock);

	for (i = 0; i < svc->face_cnt; i++) {
		feat = &svc->face_info[i].feat;
		/* norm0 */
		norm = 0.0f;
		for (j = 0; j < face_svc_conf->svc.feat_cmp_size / 2; j++) {
			norm += feat->values[j] * feat->values[j];
		}
		feat->norm[0] = sqrtf(norm);
		/* norm1 */
		norm = 0.0f;
		for (; j < face_svc_conf->svc.feat_cmp_size; j++) {
			norm += feat->values[j] * feat->values[j];
		}
		feat->norm[1] = sqrtf(norm);
	}

	ret = svc->face_ret;
	pthread_mutex_unlock(&svc->face_lock);
	pthread_mutex_unlock(&svc->block_api_lock);

	ALOGD_IF(1, "leave %s, ret: %d", __func__, ret);

	return ret;
}

float face_compare(void *handle, face_info_t *info1, face_info_t *info2)
{
	face_svc_t *svc = (face_svc_t *)handle;
	float score;
	nna_face_feature_t face_feat;
	int conf;
	(void)conf;

	if (!svc || !info1 || !info2) {
		ALOGE("%s invalid parameters", __func__);
		return 0.0;
	}

	memset(&face_feat, 0, sizeof(nna_face_feature_t));

	face_feat.features = info1->feat.values;
	face_feat.features_num = 1;
	face_feat.features_norm = info1->feat.norm;

	face_feat.feature_ref = info2->feat.values;
	face_feat.feature_size = FACE_FEAT_SIZE;
	face_feat.feature_cmp_size = face_svc_conf->svc.feat_cmp_size;
	face_feat.score = &score;

	conf = nn_face_feat_cmp(&face_feat);

	return score;
}

float face_compare2(signed char *info1, signed char *info2)
{
	float score = 0.0f;
	nna_face_feature_t face_feat;
	int j = 0;
	float norm;
	float norm_m[2] = {0};

	if (!info1 || !info2) {
			ALOGE("%s invalid parameters", __func__);
			return 0.0f;
	}

	memset(&face_feat, 0, sizeof(nna_face_feature_t));

	/* norm0 */
	norm = 0.0f;
	for (j = 0; j < face_svc_conf->svc.feat_cmp_size / 2; j++) {
		norm += info1[j] * info1[j];
	}
	norm_m[0] = sqrtf(norm);
	/* norm1 */
	norm = 0.0f;
	for (; j < face_svc_conf->svc.feat_cmp_size; j++) {
		norm += info1[j] * info1[j];
	}
	norm_m[1] = sqrtf(norm);

	face_feat.features = info1;
	face_feat.features_num = 1;
	face_feat.features_norm = norm_m;

	face_feat.feature_ref = info2;
	face_feat.feature_size = FACE_FEAT_SIZE;
	face_feat.feature_cmp_size = face_svc_conf->svc.feat_cmp_size;
	face_feat.score = &score;

	nn_face_feat_cmp(&face_feat);

	return score;
}

float face_compare2_with_mask(signed char *info1, signed char *info2)
{
	float score = 0.0f;
	nna_face_feature_t face_feat;
	int j = 0;
	float norm;
	float norm_m[2] = {0};

	if (!info1 || !info2) {
			ALOGE("%s invalid parameters", __func__);
			return 0.0f;
	}

	memset(&face_feat, 0, sizeof(nna_face_feature_t));

	/* norm0 */
	norm = 0.0f;
	for (j = 0; j < face_svc_conf->svc.feat_cmp_size / 2; j++) {
		norm += info1[j] * info1[j];
	}
	norm_m[0] = sqrtf(norm);
	/* norm1 */
	norm = 0.0f;
	for (; j < face_svc_conf->svc.feat_cmp_size; j++) {
		norm += info1[j] * info1[j];
	}
	norm_m[1] = sqrtf(norm);

	/* with mask */
	face_feat.feature_part = 1;
	face_feat.features = info1;
	face_feat.features_num = 1;
	face_feat.features_norm = norm_m;

	face_feat.feature_ref = info2;
	face_feat.feature_size = FACE_FEAT_SIZE;
	face_feat.feature_cmp_size = face_svc_conf->svc.feat_cmp_size;
	face_feat.score = &score;

	nn_face_feat_cmp(&face_feat);

	return score;
}

int face_recg_capture_block(void *handle,
	face_capture_param_t *capture_param, face_info_t *info)
{
	face_svc_t *svc = (face_svc_t *)handle;
	int ret;
	int j;
	float norm;
	face_feat_t *feat;

	if (!svc || !capture_param || !info) {
		ALOGE("%s invalid parameters", __func__);
		return -1;
	}

	ALOGD_IF(1, "enter %s", __func__);
	if (svc->capture == 1) {
		ALOGE("%s already start capture", __func__);
		return -1;
	}

	pthread_mutex_lock(&svc->block_api_lock);
	pthread_mutex_lock(&svc->face_capture_lock);

	svc->capture = 1;
	svc->capture_param = capture_param;
	svc->capture_info = info;

	/* wait for done */
	struct timespec timeout;
	timeout.tv_nsec = 0;
	timeout.tv_sec = time(NULL) + capture_param->timeout_sec;
	pthread_cond_timedwait(&svc->face_capture_cond, &svc->face_capture_lock,
		&timeout);
	svc->capture = 0;

	if (svc->capture_param->face_cnt > 0) {
		feat = &svc->capture_info->feat;
		/* norm0 */
		norm = 0.0f;
		for (j = 0; j < face_svc_conf->svc.feat_cmp_size / 2; j++) {
			norm += feat->values[j] * feat->values[j];
		}
		feat->norm[0] = sqrtf(norm);
		/* norm1 */
		norm = 0.0f;
		for (; j < face_svc_conf->svc.feat_cmp_size; j++) {
			norm += feat->values[j] * feat->values[j];
		}
		feat->norm[1] = sqrtf(norm);
	}
	ret = svc->capture_param->face_cnt;
	pthread_mutex_unlock(&svc->face_capture_lock);
	pthread_mutex_unlock(&svc->block_api_lock);

	ALOGD_IF(1, "leave %s, ret: %d", __func__, ret);

	return ret;
}

int face_get_feats_from_image_file(void *handle,
	face_image_file_t *file, face_feat_opt_t *opt)
{
	e_error_code err_code = FACE_SUCCESS;
	struct recg_img_dec_info dec_info = {0};
	struct recg_img_intput input = {0};
	struct recg_img_output output = {0};
	struct input_image_info jpeg_input = {0};
	struct output_image_info jpeg_output = {0};
	struct flip_input flip = {0};
	face_frame_t frame = {0};
	struct cut_input cut_in = {0};
	struct cut_output cut_out = {0};
	face_info_t *info = NULL;
	int ret = -1, face_max = 0, face_min = 0, face_width = 0, face_height = 0;
	char pic_path[64] = {0};
	long long time_stamp = 0;

	int larger = 0;
	int larger_fsize = 0;
	jpeg_dec_output_buf_t dec_output = {0};
	jpeg_dec_parm_t dec_parm = {0};
	int dest_width = 0,dest_height = 0, stride_width = 0, stride_height = 0;

	assert((handle != NULL) && (file != NULL) && (opt != NULL));
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGD_IF(face_svc_conf->debug.debug_log, "register photo path:%s",
		file->image_path);

	/* check file */
	ret = is_file_exist(file->image_path);
	if (ret) {
		ALOGE("%s not found", file->image_path);
		err_code = FACE_REG_PIC_NO_HAVE;
		return err_code;
	}

	if (face_svc_conf->svc.dump_debug_reg_img) {
		time_stamp = get_systime_us();
		create_dir(REGISTER_PIC_SAVE_PATH);
		snprintf(pic_path, sizeof(pic_path), "%s/from_file_%lld.jpg",
			REGISTER_PIC_SAVE_PATH, time_stamp);
		file_copy(file->image_path, pic_path);
	}

	input.file = file->image_path;
	input.stride_width = face_svc_conf->svc.rgb_buffer_width;
	input.stride_height = face_svc_conf->svc.rgb_buffer_height;

	if (face_svc_conf->svc.max_reg_img_size) {
		recg_img_dec_info(&input, &dec_info);
		if (dec_info.width > face_svc_conf->svc.max_reg_img_size ||
			dec_info.height > face_svc_conf->svc.max_reg_img_size) {
			ALOGE("image size limit width: %d height: %d\n",
				dec_info.width, dec_info.height);
			err_code = FACE_REG_PIC_SIZE_LARGER;
			return err_code;
		}
	}

	ret = recg_img_dec_begin(&input, &output);
	if (ret) {
		ALOGE("image dec failed");
		err_code = FACE_REG_PIC_DECODE_FAIL;
		return err_code;
	}

	if (file->hflip) {
		flip.addr[0] = output.virt_addr[0];
		flip.addr[1] = output.virt_addr[1];
		flip.addr[2] = output.virt_addr[2];
		flip.size[0] = output.size[0];
		flip.size[1] = output.size[1];
		flip.size[2] = output.size[2];
		flip.width = output.stride_width;
		flip.height = output.stride_height;
		flip.format = FLIP_COLOR_NV12;
		ret = yuv_flip(&flip);
		if (ret) {
			ALOGE("flip failed");
			err_code = FACE_REG_PIC_FLIP_FAIL;
			goto free_res;
		}
		lombo_cache_flush_vir(output.virt_addr[0]);
	}

handle_larger:
	if (larger) {
		ALOGD("handle_larger");
		dec_output.vir_addr = output.virt_addr[0];
		dec_output.phy_addr = output.phy_addr[0];
		dec_output.offset[0] = 0;
		dec_output.offset[1] = output.virt_addr[1] - output.virt_addr[0];
		dec_output.offset[2] = dec_output.offset[1] + output.size[1] / 2;
		dec_parm.width_stride = output.stride_width;
		dec_parm.height_aligned = output.stride_height;
		dec_parm.crop_width = output.stride_width;
		dec_parm.crop_height = output.stride_height;
		dec_parm.rotation_degree = 0;
		dec_parm.output_format = JPEG_NV12;

		dest_width = output.stride_width * face_svc_conf->svc.max_face_pixel / larger_fsize;
		dest_height = output.stride_height * face_svc_conf->svc.max_face_pixel / larger_fsize;
		stride_width = output.stride_width;
		stride_height = output.stride_height;
		ALOGD("larger_fsize: %d, dest_width:%d dest_height:%d stride_width:%d stride_height: %d\n",
			larger_fsize, dest_width, dest_height, stride_width, stride_height);
		ret = jpeg_output_adjust(&dec_output, &dec_parm, dest_width, dest_height,
			stride_width, stride_height, 0);
		if (ret) {
			ALOGE("jpeg_output_adjust failed");
			err_code = FACE_REG_FACE_SIZE_LARGER;
			goto free_res;
		}

		output.stride_width = stride_width;
		output.stride_height = stride_height;
		output.virt_addr[0] = dec_output.vir_addr;
		output.virt_addr[1] = dec_output.vir_addr + dec_output.offset[1];
		output.virt_addr[2] = NULL;
		output.phy_addr[0] = dec_output.phy_addr;
		output.phy_addr[1] = dec_output.phy_addr + dec_output.offset[1];
		output.phy_addr[2] = 0;

		output.size[0] = stride_width*stride_height;
		output.size[1] = stride_width*stride_height / 2;
		output.size[2] = 0;

		/* to update addr for free */
		recg_img_output_adjust_end(&output);
	}

	/* get face info */
	frame.width = output.stride_width;
	frame.height = output.stride_height;
	frame.viraddr[0] = output.virt_addr[0];
	frame.viraddr[1] = output.virt_addr[1];
	frame.viraddr[2] = output.virt_addr[2];
	frame.phyaddr[0] = output.phy_addr[0];
	frame.phyaddr[1] = output.phy_addr[1];
	frame.phyaddr[2] = output.phy_addr[2];
	frame.size[0] = output.size[0];
	frame.size[1] = output.size[1];
	frame.size[2] = output.size[2];
	info = &opt->info;
	opt->face_num = face_recg_block(svc, &frame, 1, info);
	if (opt->face_num < 0) {
		ALOGE("%s: face recg block failed\n", __func__);
		err_code = FACE_SVC_INTERNAL_ERROR;
		goto free_res;
	}
	/* check face num */
	if (opt->face_num == 0) {
		ALOGE("%s: no face is detected\n", __func__);
		err_code = FACE_REG_PIC_NO_DETECT_FACE;
		goto free_res;
	} else if (opt->face_num > 1) {
		ALOGE("%s: more than one face(%d) are detected\n", __func__, opt->face_num);
		err_code = FACE_REG_PIC_MULTIPLE_FACE;
		goto free_res;
	}

	/* check face size */
	face_max = face_svc_conf->svc.max_face_pixel;
	face_min = face_svc_conf->svc.min_reg_face_pixel;
	face_width = info->rect.rect[2] - info->rect.rect[0];
	face_height = info->rect.rect[3] - info->rect.rect[1];
	if (face_width > face_max || face_height > face_max) {
		ALOGE("face size(%dx%d) > face max(%d)",
			face_width, face_height, face_max);
		larger = 1;
		larger_fsize = (face_width > face_height) ? face_width:face_height;
		goto handle_larger;
	}
	if (face_width < face_min || face_height < face_min) {
		ALOGE("face size(%dx%d) < face min(%d)",
			face_width, face_height, face_min);
		err_code = FACE_REG_FACE_SIZE_SMALLER;
		goto free_res;
	}

	/* check face quality */
	if (info->rect.quality_score < opt->quality) {
		ALOGE("%s: face quality(%d) is too low\n", __func__,
						info->rect.quality_score);
		err_code = FACE_REG_PIC_QUALITY_TOO_LOW;
		goto free_res;
	}

	/* check face rect */
	if (info->rect.rect[0] < opt->rect[0] ||
		info->rect.rect[1] < opt->rect[1] ||
		info->rect.rect[2] > face_svc_conf->svc.rgb_buffer_width - opt->rect[2] ||
		info->rect.rect[3] > face_svc_conf->svc.rgb_buffer_height - opt->rect[3]) {
		ALOGE("%s: face rect(%d %d %d %d) limit(%d %d %d %d)\n", __func__,
						info->rect.rect[0], info->rect.rect[1],
						info->rect.rect[2], info->rect.rect[3],
						opt->rect[0], opt->rect[1],
						opt->rect[2], opt->rect[3]);
		err_code = FACE_REG_FACE_RECT_LIMIT;
		goto free_res;
	}

	if (file->save_face) {
		/* cut the face */
		cut_in.addr[0] = frame.viraddr[0];
		cut_in.addr[1] = frame.viraddr[1];
		cut_in.addr[2] = frame.viraddr[2];
		cut_in.size[0] = frame.size[0];
		cut_in.size[1] = frame.size[1];
		cut_in.size[2] = frame.size[2];
		cut_in.src_width = frame.width;
		cut_in.src_height = frame.height;
		cut_in.format = IMG_COLOR_NV12;
		cut_in.dest_x = info->rect.rect[0];
		cut_in.dest_y = info->rect.rect[1];
		/*16 byte alignment for jpeg enc hardware */
		cut_in.dest_width = info->rect.rect[2] - info->rect.rect[0];
		cut_in.dest_width = (cut_in.dest_width + 15) & (~0x0f);
		cut_in.dest_height = info->rect.rect[3] - info->rect.rect[1];
		cut_in.dest_height = (cut_in.dest_height + 15) & (~0x0f);
		/* avoid crossing the boundary */
		if (cut_in.dest_x + cut_in.dest_width >= cut_in.src_width)
			cut_in.dest_width -= 16;
		if (cut_in.dest_y + cut_in.dest_height >= cut_in.src_height)
			cut_in.dest_height -= 16;
		/*call function to cutout*/
		ret = yuv_cut_start(&cut_in, &cut_out);
		if (ret) {
			ALOGE("%s: cut failed\n", __func__);
			err_code = FACE_SVC_INTERNAL_ERROR;
			goto free_res;
		}

		ALOGD_IF(face_svc_conf->debug.debug_log, "face pic save path:%s",
			file->save_face_path);
		jpeg_input.addr[0] = cut_out.addr[0];
		jpeg_input.addr[1] = cut_out.addr[1];
		jpeg_input.addr[2] = cut_out.addr[2];
		jpeg_input.size[0] = cut_out.size[0];
		jpeg_input.size[1] = cut_out.size[1];
		jpeg_input.size[2] = cut_out.size[2];
		jpeg_input.width = cut_out.width;
		jpeg_input.height = cut_out.height;
		jpeg_input.format = IMG_COLOR_NV12;
		jpeg_input.qualify = 99;
		jpeg_input.file = file->save_face_path;
		ret = jpeg_enc_file(&jpeg_input, &jpeg_output);
		yuv_cut_end(&cut_out);
		if (ret) {
			ALOGE("jpeg enc small image failed\n");
			err_code = FACE_SVC_INTERNAL_ERROR;
			goto free_res;
		}
	}

	/* recognition threshold as simulity*/
	opt->simulity = info->feat.score;
	opt->quality = info->rect.quality_score;

free_res:

	recg_img_dec_end(&output);
	return err_code;
}

int face_get_feats_from_image_data(void *handle,
	face_image_data_t *data, face_feat_opt_t *opt)
{
	e_error_code err_code = FACE_SUCCESS;
	struct recg_img_dec_info dec_info = {0};
	struct recg_img_intput input = {0};
	struct recg_img_output output = {0};
	struct input_image_info jpeg_input = {0};
	struct output_image_info jpeg_output = {0};
	struct flip_input flip = {0};
	face_frame_t frame = {0};
	struct cut_input cut_in = {0};
	struct cut_output cut_out = {0};
	face_info_t *info = NULL;
	int ret = -1, face_max = 0, face_min = 0, face_width = 0, face_height = 0;
	char pic_path[64] = {0};
	long long time_stamp = 0;
	FILE *fp = NULL;

	assert((handle != NULL) && (data != NULL) && (opt != NULL));
	face_svc_t *svc = (face_svc_t *)handle;

	if (face_svc_conf->svc.dump_debug_reg_img) {
		time_stamp = get_systime_us();
		create_dir(REGISTER_PIC_SAVE_PATH);
		snprintf(pic_path, sizeof(pic_path), "%s/from_data_%lld.jpg",
			REGISTER_PIC_SAVE_PATH, time_stamp);
		fp = fopen(pic_path, "wb+");
		if (fp) {
			/* fopen succes */
			fwrite(data->image_ptr, 1, data->len, fp);
			fclose(fp);
		}
	}

	input.data = data->image_ptr;
	input.len = data->len;
	input.stride_width = face_svc_conf->svc.rgb_buffer_width;
	input.stride_height = face_svc_conf->svc.rgb_buffer_height;

	if (face_svc_conf->svc.max_reg_img_size) {
		recg_img_data_dec_info(&input, &dec_info);
		if (dec_info.width > face_svc_conf->svc.max_reg_img_size ||
			dec_info.height > face_svc_conf->svc.max_reg_img_size) {
			ALOGE("image size limit width: %d height: %d\n",
				dec_info.width, dec_info.height);
			err_code = FACE_REG_PIC_SIZE_LARGER;
			return err_code;
		}
	}

	ret = recg_img_dec_data(&input, &output);
	if (ret) {
		ALOGE("image dec failed");
		err_code = FACE_REG_PIC_DECODE_FAIL;
		return err_code;
	}

	if (data->hflip) {
		flip.addr[0] = output.virt_addr[0];
		flip.addr[1] = output.virt_addr[1];
		flip.addr[2] = output.virt_addr[2];
		flip.size[0] = output.size[0];
		flip.size[1] = output.size[1];
		flip.size[2] = output.size[2];
		flip.width = output.stride_width;
		flip.height = output.stride_height;
		flip.format = FLIP_COLOR_NV12;
		ret = yuv_flip(&flip);
		if (ret) {
			ALOGE("flip failed");
			err_code = FACE_REG_PIC_FLIP_FAIL;
			goto free_res;
		}
		lombo_cache_flush_vir(output.virt_addr[0]);
	}

	/* get face info */
	frame.width = output.stride_width;
	frame.height = output.stride_height;
	frame.viraddr[0] = output.virt_addr[0];
	frame.viraddr[1] = output.virt_addr[1];
	frame.viraddr[2] = output.virt_addr[2];
	frame.phyaddr[0] = output.phy_addr[0];
	frame.phyaddr[1] = output.phy_addr[1];
	frame.phyaddr[2] = output.phy_addr[2];
	frame.size[0] = output.size[0];
	frame.size[1] = output.size[1];
	frame.size[2] = output.size[2];
	info = &opt->info;
	opt->face_num = face_recg_block(svc, &frame, 1, info);
	if (opt->face_num < 0) {
		ALOGE("%s: face recg block failed\n", __func__);
		err_code = FACE_SVC_INTERNAL_ERROR;
		goto free_res;
	}
	/* check face num */
	if (opt->face_num == 0) {
		ALOGE("%s: no face is detected\n", __func__);
		err_code = FACE_REG_PIC_NO_DETECT_FACE;
		goto free_res;
	} else if (opt->face_num > 1) {
		ALOGE("%s: more than one face(%d) are detected\n", __func__, opt->face_num);
		err_code = FACE_REG_PIC_MULTIPLE_FACE;
		goto free_res;
	}

	/* check face size */
	face_max = face_svc_conf->svc.max_face_pixel;
	face_min = face_svc_conf->svc.min_reg_face_pixel;
	face_width = info->rect.rect[2] - info->rect.rect[0];
	face_height = info->rect.rect[3] - info->rect.rect[1];
	if (face_width > face_max || face_height > face_max) {
		ALOGE("face size(%dx%d) > face max(%d)",
			face_width, face_height, face_max);
		err_code = FACE_REG_FACE_SIZE_LARGER;
		goto free_res;
	}
	if (face_width < face_min || face_height < face_min) {
		ALOGE("face size(%dx%d) < face min(%d)",
			face_width, face_height, face_min);
		err_code = FACE_REG_FACE_SIZE_SMALLER;
		goto free_res;
	}

	/* recognition threshold as simulity*/
	opt->simulity = info->feat.score;
	opt->quality = info->rect.quality_score;

	/* check face quality */
	if (info->rect.quality_score < opt->quality) {
		ALOGE("%s: face quality(%d) is too low\n", __func__,
						info->rect.quality_score);
		err_code = FACE_REG_PIC_QUALITY_TOO_LOW;
		goto free_res;
	}

	/* check face rect */
	if (info->rect.rect[0] < opt->rect[0] ||
		info->rect.rect[1] < opt->rect[1] ||
		info->rect.rect[2] > face_svc_conf->svc.rgb_buffer_width - opt->rect[2] ||
		info->rect.rect[3] > face_svc_conf->svc.rgb_buffer_height - opt->rect[3]) {
		ALOGE("%s: face rect(%d %d %d %d) limit(%d %d %d %d)\n", __func__,
						info->rect.rect[0], info->rect.rect[1],
						info->rect.rect[2], info->rect.rect[3],
						opt->rect[0], opt->rect[1],
						opt->rect[2], opt->rect[3]);
		err_code = FACE_REG_FACE_RECT_LIMIT;
		goto free_res;
	}

	if (data->save_face) {
		/* cut the face */
		cut_in.addr[0] = frame.viraddr[0];
		cut_in.addr[1] = frame.viraddr[1];
		cut_in.addr[2] = frame.viraddr[2];
		cut_in.size[0] = frame.size[0];
		cut_in.size[1] = frame.size[1];
		cut_in.size[2] = frame.size[2];
		cut_in.src_width = frame.width;
		cut_in.src_height = frame.height;
		cut_in.format = IMG_COLOR_NV12;
		cut_in.dest_x = info->rect.rect[0];
		cut_in.dest_y = info->rect.rect[1];
		/*16 byte alignment for jpeg enc hardware */
		cut_in.dest_width = info->rect.rect[2] - info->rect.rect[0];
		cut_in.dest_width = (cut_in.dest_width + 15) & (~0x0f);
		cut_in.dest_height = info->rect.rect[3] - info->rect.rect[1];
		cut_in.dest_height = (cut_in.dest_height + 15) & (~0x0f);
		/* avoid crossing the boundary */
		if (cut_in.dest_x + cut_in.dest_width >= cut_in.src_width)
			cut_in.dest_width -= 16;
		if (cut_in.dest_y + cut_in.dest_height >= cut_in.src_height)
			cut_in.dest_height -= 16;
		/*call function to cutout*/
		ret = yuv_cut_start(&cut_in, &cut_out);
		if (ret) {
			ALOGE("%s: cut failed\n", __func__);
			err_code = FACE_SVC_INTERNAL_ERROR;
			goto free_res;
		}

		ALOGD_IF(face_svc_conf->debug.debug_log, "face pic save path:%s",
			data->save_face_path);
		jpeg_input.addr[0] = cut_out.addr[0];
		jpeg_input.addr[1] = cut_out.addr[1];
		jpeg_input.addr[2] = cut_out.addr[2];
		jpeg_input.size[0] = cut_out.size[0];
		jpeg_input.size[1] = cut_out.size[1];
		jpeg_input.size[2] = cut_out.size[2];
		jpeg_input.width = cut_out.width;
		jpeg_input.height = cut_out.height;
		jpeg_input.format = IMG_COLOR_NV12;
		jpeg_input.qualify = 99;
		jpeg_input.file = data->save_face_path;
		ret = jpeg_enc_file(&jpeg_input, &jpeg_output);
		yuv_cut_end(&cut_out);
		if (ret) {
			ALOGE("jpeg enc small image failed\n");
			err_code = FACE_SVC_INTERNAL_ERROR;
			goto free_res;
		}
	}

	if (data->save_frame) {
		jpeg_input.addr[0] = frame.viraddr[0];
		jpeg_input.addr[1] = frame.viraddr[1];
		jpeg_input.size[0] = frame.size[0];
		jpeg_input.size[1] = frame.size[1];
		jpeg_input.width = frame.width;
		jpeg_input.height = frame.height;
		jpeg_input.format = IMG_COLOR_NV12;
		jpeg_input.qualify = 100;
		jpeg_input.file = data->save_frame_path;
		if (jpeg_enc_file(&jpeg_input, &jpeg_output))
			ALOGE("%s encode frame to %s failed", __func__,
				data->save_frame_path);
	}

free_res:

	recg_img_dec_end(&output);
	return err_code;
}

int face_get_feats_from_capture(void *handle,
	face_capture_param_t *capture_param, face_feat_opt_t *opt)
{
	int ret = -1;
	e_error_code err_code = FACE_SUCCESS;
	face_info_t *info = NULL;
	face_frame_t *frame = NULL;
	struct input_image_info jpeg_input = {0};
	struct output_image_info jpeg_output = {0};
	struct cut_input cut_in = {0};
	struct cut_output cut_out = {0};
	char pic_path[64] = {0};
	long long time_stamp = 0;

	assert((handle != NULL) && (opt != NULL));
	face_svc_t *svc = (face_svc_t *)handle;

	info = &opt->info;
	opt->face_num = face_recg_capture_block(svc, capture_param, info);
	if (opt->face_num < 0) {
		ALOGE("%s: FACE_SVC_INTERNAL_ERROR\n", __func__);
		err_code = FACE_SVC_INTERNAL_ERROR;
		goto free_res;
	}
	if (opt->face_num == 0) {
		ALOGE("%s: FACE_REG_PIC_NO_DETECT_FACE\n", __func__);
		err_code = FACE_REG_PIC_NO_DETECT_FACE;
		goto free_res;
	}

	/* recognition threshold as simulity*/
	opt->simulity = info->feat.score;
	opt->quality = info->rect.quality_score;
	frame = capture_param->frame;

	if (face_svc_conf->svc.dump_debug_reg_img) {
		time_stamp = get_systime_us();
		create_dir(REGISTER_PIC_SAVE_PATH);
		snprintf(pic_path, sizeof(pic_path), "%s/from_capture_%lld.jpg",
			REGISTER_PIC_SAVE_PATH, time_stamp);
		dump_frame_jpg(pic_path, frame);
	}

	/* check face quality */
	if (info->rect.quality_score < opt->quality) {
		ALOGE("%s: face quality(%d) is too low\n", __func__,
						info->rect.quality_score);
		err_code = FACE_REG_PIC_QUALITY_TOO_LOW;
		goto free_res;
	}

	if (capture_param->save_face) {
		/* cut the face */
		cut_in.addr[0] = frame->viraddr[0];
		cut_in.addr[1] = frame->viraddr[1];
		cut_in.addr[2] = frame->viraddr[2];
		cut_in.size[0] = frame->size[0];
		cut_in.size[1] = frame->size[1];
		cut_in.size[2] = frame->size[2];
		cut_in.src_width = frame->width;
		cut_in.src_height = frame->height;
		cut_in.format = IMG_COLOR_NV12;
		cut_in.dest_x = info->rect.rect[0] + face_svc_conf->svc.rgb_x_margin;
		cut_in.dest_y = info->rect.rect[1] + face_svc_conf->svc.rgb_y_margin;
		/*16 byte alignment for jpeg enc hardware */
		cut_in.dest_width = info->rect.rect[2] - info->rect.rect[0];
		cut_in.dest_width = (cut_in.dest_width + 15) & (~0x0f);
		cut_in.dest_height = info->rect.rect[3] - info->rect.rect[1];
		cut_in.dest_height = (cut_in.dest_height + 15) & (~0x0f);
		/* avoid crossing the boundary */
		if (cut_in.dest_x + cut_in.dest_width >= cut_in.src_width)
			cut_in.dest_width -= 16;
		if (cut_in.dest_y + cut_in.dest_height >= cut_in.src_height)
			cut_in.dest_height -= 16;
		/*call function to cutout*/
		ret = yuv_cut_start(&cut_in, &cut_out);
		if (ret) {
			ALOGE("%s: cut failed\n", __func__);
			err_code = FACE_SVC_INTERNAL_ERROR;
			goto free_res;
		}

		ALOGD_IF(face_svc_conf->debug.debug_log, "face pic save path:%s",
			capture_param->save_face_path);
		jpeg_input.addr[0] = cut_out.addr[0];
		jpeg_input.addr[1] = cut_out.addr[1];
		jpeg_input.addr[2] = cut_out.addr[2];
		jpeg_input.size[0] = cut_out.size[0];
		jpeg_input.size[1] = cut_out.size[1];
		jpeg_input.size[2] = cut_out.size[2];
		jpeg_input.width = cut_out.width;
		jpeg_input.height = cut_out.height;
		jpeg_input.format = IMG_COLOR_NV12;
		jpeg_input.qualify = 99;
		jpeg_input.file = capture_param->save_face_path;
		ret = jpeg_enc_file(&jpeg_input, &jpeg_output);
		yuv_cut_end(&cut_out);
		if (ret) {
			ALOGE("%s encode face to %s failed", __func__,
				capture_param->save_face_path);
			err_code = FACE_SVC_INTERNAL_ERROR;
			goto free_res;
		}
	}

	if (capture_param->save_frame) {
		jpeg_input.addr[0] = frame->viraddr[0];
		jpeg_input.addr[1] = frame->viraddr[1];
		jpeg_input.size[0] = frame->size[0];
		jpeg_input.size[1] = frame->size[1];
		jpeg_input.width = frame->width;
		jpeg_input.height = frame->height;
		jpeg_input.format = IMG_COLOR_NV12;
		jpeg_input.qualify = 100;
		jpeg_input.file = capture_param->save_frame_path;
		if (jpeg_enc_file(&jpeg_input, &jpeg_output))
			ALOGE("%s encode frame to %s failed", __func__,
				capture_param->save_frame_path);
	}

	free_frame(&frame);
free_res:

	return err_code;
}

int face_stop_feats_from_capture(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;
	if (!svc) {
		ALOGE("%s invalid parameters\n", __func__);
		return -1;
	}
	if (svc->capture != 1) {
		ALOGE("%s capture not start\n", __func__);
		return -1;
	}
	pthread_mutex_lock(&svc->face_capture_lock);
	svc->capture = 0;
	svc->capture_param->face_cnt = -1;
	pthread_cond_signal(&svc->face_capture_cond);
	pthread_mutex_unlock(&svc->face_capture_lock);
	return 0;
}

int face_svc_start_qrmode_set(void *handle, int mode, f_qr_scan_cb cb, void *arg)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc || !face_svc_conf->svc.zbar_enable)
		return -1;
	pthread_mutex_lock(&svc->zbar_lock);
	svc->zbar_mode = mode;
	zbar_register_det_cb(mode, cb, arg);
	pthread_mutex_unlock(&svc->zbar_lock);
	return 0;
}

int face_svc_start_qrmode(void *handle, f_qr_scan_cb cb, void *arg)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc || !face_svc_conf->svc.zbar_enable)
		return -1;
	face_svc_start_qrmode_set(handle, 1, cb, arg);
	return 0;
}

int face_svc_set_qrmode_timeout(void *handle, int ms)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc || !face_svc_conf->svc.zbar_enable)
		return -1;

	face_svc_conf->svc.zbar_timeout = ms;
	return 0;
}

int face_svc_stop_qrmode(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc || !face_svc_conf->svc.zbar_enable)
		return -1;
	pthread_mutex_lock(&svc->zbar_lock);
	svc->zbar_mode = 0;
	pthread_mutex_unlock(&svc->zbar_lock);
	return 0;
}

int face_svc_recg_pause(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc)
		return -1;
	face_svc_conf_mutex_lock();
	face_svc_conf->svc.recg_enable = 0;
	face_svc_conf_mutex_unlock();

	return 0;
}

int face_svc_recg_resume(void *handle)
{
	face_svc_t *svc = (face_svc_t *)handle;

	if (!svc)
		return -1;
	face_svc_conf_mutex_lock();
	face_svc_conf->svc.recg_enable = 1;
	face_svc_conf_mutex_unlock();

	return 0;
}

int face_svc_rgb_color_living_check(void *handle, char *image_path)
{
	e_error_code err_code = FACE_SUCCESS;
	struct recg_img_dec_info dec_info = {0};
	struct recg_img_intput input = {0};
	struct recg_img_output output = {0};
	face_frame_t frame = {0};
	face_info_t face_info = {0};
	face_info_t *info = NULL;
	int ret = -1, face_max = 0, face_width = 0, face_height = 0;

	int larger = 0;
	int larger_fsize = 0;
	jpeg_dec_output_buf_t dec_output = {0};
	jpeg_dec_parm_t dec_parm = {0};
	int dest_width = 0,dest_height = 0, stride_width = 0, stride_height = 0;

	assert((handle != NULL) && (image_path != NULL));
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGD_IF(face_svc_conf->debug.debug_log, "check photo path:%s", image_path);

	/* check file */
	ret = is_file_exist(image_path);
	if (ret) {
		ALOGE("%s not found", image_path);
		err_code = FACE_REG_PIC_NO_HAVE;
		return err_code;
	}

	input.file = image_path;
	input.stride_width = face_svc_conf->svc.rgb_buffer_width;
	input.stride_height = face_svc_conf->svc.rgb_buffer_height;

	if (face_svc_conf->svc.max_reg_img_size) {
		recg_img_dec_info(&input, &dec_info);
		if (dec_info.width > face_svc_conf->svc.max_reg_img_size ||
			dec_info.height > face_svc_conf->svc.max_reg_img_size) {
			ALOGE("image size limit width: %d height: %d\n",
				dec_info.width, dec_info.height);
			err_code = FACE_REG_PIC_SIZE_LARGER;
			return err_code;
		}
	}

	ret = recg_img_dec_begin(&input, &output);
	if (ret) {
		ALOGE("image dec failed");
		err_code = FACE_REG_PIC_DECODE_FAIL;
		return err_code;
	}

handle_larger:
	if (larger) {
		ALOGD("handle_larger");
		dec_output.vir_addr = output.virt_addr[0];
		dec_output.phy_addr = output.phy_addr[0];
		dec_output.offset[0] = 0;
		dec_output.offset[1] = output.virt_addr[1] - output.virt_addr[0];
		dec_output.offset[2] = dec_output.offset[1] + output.size[1] / 2;
		dec_parm.width_stride = output.stride_width;
		dec_parm.height_aligned = output.stride_height;
		dec_parm.crop_width = output.stride_width;
		dec_parm.crop_height = output.stride_height;
		dec_parm.rotation_degree = 0;
		dec_parm.output_format = JPEG_NV12;

		dest_width = output.stride_width * face_svc_conf->svc.max_face_pixel / larger_fsize;
		dest_height = output.stride_height * face_svc_conf->svc.max_face_pixel / larger_fsize;
		stride_width = output.stride_width;
		stride_height = output.stride_height;
		ALOGD("larger_fsize: %d, dest_width:%d dest_height:%d stride_width:%d stride_height: %d\n",
			larger_fsize, dest_width, dest_height, stride_width, stride_height);
		ret = jpeg_output_adjust(&dec_output, &dec_parm, dest_width, dest_height,
			stride_width, stride_height, 0);
		if (ret) {
			ALOGE("jpeg_output_adjust failed");
			err_code = FACE_REG_FACE_SIZE_LARGER;
			goto free_res;
		}

		output.stride_width = stride_width;
		output.stride_height = stride_height;
		output.virt_addr[0] = dec_output.vir_addr;
		output.virt_addr[1] = dec_output.vir_addr + dec_output.offset[1];
		output.virt_addr[2] = NULL;
		output.phy_addr[0] = dec_output.phy_addr;
		output.phy_addr[1] = dec_output.phy_addr + dec_output.offset[1];
		output.phy_addr[2] = 0;

		output.size[0] = stride_width*stride_height;
		output.size[1] = stride_width*stride_height / 2;
		output.size[2] = 0;

		/* to update addr for free */
		recg_img_output_adjust_end(&output);
	}

	/* get face info */
	frame.width = output.stride_width;
	frame.height = output.stride_height;
	frame.viraddr[0] = output.virt_addr[0];
	frame.viraddr[1] = output.virt_addr[1];
	frame.viraddr[2] = output.virt_addr[2];
	frame.phyaddr[0] = output.phy_addr[0];
	frame.phyaddr[1] = output.phy_addr[1];
	frame.phyaddr[2] = output.phy_addr[2];
	frame.size[0] = output.size[0];
	frame.size[1] = output.size[1];
	frame.size[2] = output.size[2];
	info = &face_info;
	int face_num = face_recg_block(svc, &frame, 1, info);
	if (face_num < 0) {
		ALOGE("%s: face recg block failed\n", __func__);
		err_code = FACE_SVC_INTERNAL_ERROR;
		goto free_res;
	}
	/* check face num */
	if (face_num == 0) {
		ALOGE("%s: no face is detected\n", __func__);
		err_code = FACE_REG_PIC_NO_DETECT_FACE;
		goto free_res;
	} else if (face_num > 1) {
		ALOGE("%s: more than one face(%d) are detected\n", __func__, face_num);
		err_code = FACE_REG_PIC_MULTIPLE_FACE;
		goto free_res;
	}

	/* check face size */
	face_max = face_svc_conf->svc.max_face_pixel;
	face_width = info->rect.rect[2] - info->rect.rect[0];
	face_height = info->rect.rect[3] - info->rect.rect[1];
	if (face_width > face_max || face_height > face_max) {
		ALOGE("face size(%dx%d) > face max(%d)",
			face_width, face_height, face_max);
		larger = 1;
		larger_fsize = (face_width > face_height) ? face_width:face_height;
		goto handle_larger;
	}

	living_result_t living_result = {0};
	int color_score[3]={0,0,0};
	nirdt_in_t rgb_face;
	face_frame_t *rgb_frame = &frame;

	//rgb face
	rgb_face.yuv.size.width = face_svc_conf->svc.rgb_buffer_width;
	rgb_face.yuv.size.height = face_svc_conf->svc.rgb_buffer_height;
	rgb_face.target_sz.width = face_svc_conf->svc.rgb_buffer_width;
	rgb_face.target_sz.height = face_svc_conf->svc.rgb_buffer_height;
	rgb_face.yuv.y = (void *)rgb_frame->viraddr[0] + rgb_frame->width * face_svc_conf->svc.rgb_y_margin + face_svc_conf->svc.rgb_x_margin;
	rgb_face.yuv.uv = (void *)rgb_frame->viraddr[1] + rgb_frame->width * face_svc_conf->svc.rgb_y_margin / 2 + face_svc_conf->svc.rgb_x_margin;
	rgb_face.yuv.y_phy = (void *)rgb_frame->phyaddr[0] + rgb_frame->width * face_svc_conf->svc.rgb_y_margin + face_svc_conf->svc.rgb_x_margin;
	rgb_face.yuv.uv_phy = (void *)rgb_frame->phyaddr[1] + rgb_frame->width * (face_svc_conf->svc.rgb_y_margin) / 2 + face_svc_conf->svc.rgb_x_margin;
	rgb_face.dt_roi.x = info->rect.rect[0];
	rgb_face.dt_roi.y = info->rect.rect[1];
	rgb_face.dt_roi.width = info->rect.rect[2]-info->rect.rect[0];
	rgb_face.dt_roi.height = info->rect.rect[3]-info->rect.rect[1];

	ALOGE("face rect x: %d y: %d w: %d h: %d", rgb_face.dt_roi.x, rgb_face.dt_roi.y, rgb_face.dt_roi.width, rgb_face.dt_roi.height);

	int device_id = face_svc_conf->svc.device_id;
	/* rgb 4x3 living*/
	int x1,x2,y1,y2;
	nirdt_in_t dt_in = {0};
	dt_in = rgb_face;
	x1 = rgb_face.dt_roi.x-rgb_face.dt_roi.width;
	y1 = rgb_face.dt_roi.y-rgb_face.dt_roi.height;
	x2 = x1 + (rgb_face.dt_roi.width)*3;
	y2 = y1 + (rgb_face.dt_roi.height)*4;
	CLIPMIN(x1, 0);
	CLIPMAX(x2, dt_in.yuv.size.width-1);
	CLIPMIN(y1, 0);
	CLIPMAX(y2, dt_in.target_sz.height-1);
	dt_in.dt_roi.x = x1;
	dt_in.dt_roi.y = y1;
	dt_in.dt_roi.width = x2-x1;
	dt_in.dt_roi.height = y2-y1;

	living_result = svc->rgb_living_hdl->nir_recv_buffer(svc->rgb_living_hdl,
			&dt_in, device_id);
	//rgb 4x3 living score
	ALOGD_IF(face_svc_conf->debug.debug_log, "living_score: %d\n", living_result.living_score);

	//rgb color
	living_result = svc->rgb_color_living_hdl->nir_recv_buffer(svc->rgb_color_living_hdl, &rgb_face, 0);
	memcpy(color_score, living_result.living_color_score, sizeof(color_score));
	ALOGD_IF(face_svc_conf->debug.debug_log, "liveness debug color: %d, %d, %d\n", color_score[0], color_score[1], color_score[2]);

free_res:

	recg_img_dec_end(&output);
	return err_code;
}

int face_svc_nir_living_check(void *handle, int type, char *image_path)
{
	e_error_code err_code = FACE_SUCCESS;
	struct recg_img_intput input = {0};
	struct recg_img_output output = {0};
	face_frame_t frame = {0};
	nirdt_in_t dt_in = {0};
	nirdt_out_t *dt_out = NULL;
	living_result_t living_result = {0};
	int ret = -1;

	assert((handle != NULL) && (image_path != NULL));
	face_svc_t *svc = (face_svc_t *)handle;

	ALOGD_IF(face_svc_conf->debug.debug_log, "check photo path:%s", image_path);

	/* check file */
	ret = is_file_exist(image_path);
	if (ret) {
		ALOGE("%s not found", image_path);
		err_code = FACE_REG_PIC_NO_HAVE;
		return err_code;
	}

	input.file = image_path;
	input.stride_width = face_svc_conf->svc.nir_buffer_width;
	input.stride_height = face_svc_conf->svc.nir_buffer_height;

	ret = recg_img_dec_begin(&input, &output);
	if (ret) {
		ALOGE("image dec failed");
		err_code = FACE_REG_PIC_DECODE_FAIL;
		return err_code;
	}

	/* get face info */
	frame.width = output.stride_width;
	frame.height = output.stride_height;
	frame.viraddr[0] = output.virt_addr[0];
	frame.viraddr[1] = output.virt_addr[1];
	frame.viraddr[2] = output.virt_addr[2];
	frame.phyaddr[0] = output.phy_addr[0];
	frame.phyaddr[1] = output.phy_addr[1];
	frame.phyaddr[2] = output.phy_addr[2];
	frame.size[0] = output.size[0];
	frame.size[1] = output.size[1];
	frame.size[2] = output.size[2];

	/* nir det face */
	dt_in.yuv.y = (void *)frame.viraddr[0];
	dt_in.yuv.uv = (void *)frame.viraddr[1];
	dt_in.yuv.y_phy = (void *)frame.phyaddr[0];
	dt_in.yuv.uv_phy = (void *)frame.phyaddr[1];
	dt_in.yuv.size.width = frame.width;
	dt_in.yuv.size.height = frame.height;
	dt_in.dt_roi.x = 0;
	dt_in.dt_roi.y = 0;
	dt_in.dt_roi.width = face_svc_conf->svc.nir_buffer_width;
	dt_in.dt_roi.height = face_svc_conf->svc.nir_buffer_height;
	dt_in.target_sz.width = face_svc_conf->svc.nir_buffer_width;
	dt_in.target_sz.height = face_svc_conf->svc.nir_buffer_height;
	dt_in.enlarge_x = 0.05;
	dt_in.enlarge_y = 0.04;
	dt_in.detect_whole_image = 1;

	frame.det_begin = get_systime_us();
	nirdt_detect(svc->nirdt_hdl, &dt_in, &dt_out);
	frame.det_end = get_systime_us();
	ALOGI_IF(face_svc_conf->debug.profile,
		"nir det frame %p detecting toke %lld",
		&frame, frame.det_end - frame.det_begin);
	if (dt_out->size < 0) {
		ALOGE("%s: face recg block failed\n", __func__);
		err_code = FACE_SVC_INTERNAL_ERROR;
		goto free_res;
	} else if (dt_out->size == 0) {
		ALOGE("%s: no face is detected\n", __func__);
		err_code = FACE_REG_PIC_NO_DETECT_FACE;
		goto free_res;
	} else if (dt_out->size > 1) {
		ALOGE("%s: more than one face(%d) are detected\n", __func__, dt_out->size);
		err_code = FACE_REG_PIC_MULTIPLE_FACE;
		goto free_res;
	}

	//nir face
	dt_in.target_sz.width = frame.width;
	dt_in.target_sz.height = frame.height;
	dt_in.yuv.size.width = frame.width;
	dt_in.yuv.size.height = frame.height;
	dt_in.yuv.y = (void *)frame.viraddr[0];
	dt_in.yuv.uv = (void *)frame.viraddr[1];
	dt_in.yuv.y_phy = (void *)frame.phyaddr[0];
	dt_in.yuv.uv_phy = (void *)frame.phyaddr[1];
	dt_in.dt_roi.x = dt_out->p[0].roi.x;
	dt_in.dt_roi.y = dt_out->p[0].roi.y;
	dt_in.dt_roi.width = dt_out->p[0].roi.width;
	dt_in.dt_roi.height = dt_out->p[0].roi.height;
	dt_in.set_gray = face_svc_conf->svc.living_check_without_uv; //default 0 , if (set_gray==1) uv=128

	float decrease = 0.05;
	int d_w = (int)(dt_in.dt_roi.width*decrease);
	int d_h = (int)(dt_in.dt_roi.height*decrease);
	dt_in.dt_roi.x += d_w;
	dt_in.dt_roi.y += d_h;
	dt_in.dt_roi.width -= 2*d_w;
	dt_in.dt_roi.height -= 2*d_h;

	ALOGE("face rect x: %d y: %d w: %d h: %d", dt_in.dt_roi.x, dt_in.dt_roi.y, dt_in.dt_roi.width, dt_in.dt_roi.height);

	//nir score
	int device_id = face_svc_conf->svc.device_id;
	if (type == 1)
		living_result = svc->nir_living_hdl_mask->nir_recv_buffer(svc->nir_living_hdl_mask, &dt_in, device_id);
	else if (type == 2)
		living_result = svc->nir_living_hdl->nir_recv_buffer(svc->nir_living_hdl, &dt_in, device_id);
	else
		living_result = svc->nir_living_hdl_outdoors->nir_recv_buffer(svc->nir_living_hdl_outdoors, &dt_in, device_id);

	ALOGE("%s: nir score :%d confidence :%d", __func__, living_result.living_score, living_result.living_confidence);

free_res:

	recg_img_dec_end(&output);
	return err_code;
}

