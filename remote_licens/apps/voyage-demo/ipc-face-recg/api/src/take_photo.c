/*
 * take_photo.c - define some function module
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

#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <semaphore.h>

#include "lombo_malloc.h"
#include "video_stream.h"
#include "video_recorder.h"
#include "jpeg_enc.h"
#include "face_svc.h"
#include "face.h"

#define LOG_TAG "take_photo"
#include <log/log.h>

#if 1
#if 0
#define JPG_INFO_SIZE		(8*1024)
#define JPG_OUT_SIZE		(1024*1024)

typedef struct yuv_frame {
	int size;
	unsigned char *p_frame;
} yuv_frame_t;

static void *svc_handle;
sem_t sem;

struct output_image_info rgb_output;
struct input_image_info rgb_input;
yuv_frame_t rgb_yuv_frame_info;

static void camera_free_mem(struct input_image_info *_input, yuv_frame_t *_yuv)
{

	ALOGD("enter %s\n", __func__);
	if (_yuv->p_frame) {
		ALOGD("***free frame_info.p_frame\n");
		free(_yuv->p_frame);
		_yuv->p_frame = NULL;
	}
	_yuv->size = 0;

	if (_input->jpeg_info_buf) {
		ALOGD("***image_input.jpeg_info_buf\n");
		free(_input->jpeg_info_buf);
	}
	if (_input->jpeg_out_buf) {
		ALOGD("***image_input.jpeg_out_buf\n");
		free(_input->jpeg_out_buf);
	}
	_input->jpeg_info_buf = NULL;
	_input->jpeg_out_buf = NULL;
}

static int on_frame_encode(face_frame_t *frame, struct input_image_info *_input,
	struct output_image_info *_output, yuv_frame_t *_yuv)
{
	int ret = 0;

	ALOGD("enter %s\n", __func__);
	_input->buf = frame->viraddr;
	_input->width = frame->width;
	_input->height = frame->height;
	_input->format = IMG_COLOR_NV12;
	_input->qualify = 99;

	if (_input->jpeg_info_buf == NULL)
		_input->jpeg_info_buf = (unsigned char *)malloc(JPG_INFO_SIZE);
	if (_input->jpeg_out_buf == NULL)
		_input->jpeg_out_buf = (unsigned char *)malloc(JPG_OUT_SIZE);

	ret = jpeg_enc_buffer(_input, _output);
	if (ret) {
		ALOGD("jpeg enc failed!\n");
		if (_input->jpeg_info_buf)
			free(_input->jpeg_info_buf);
		if (_input->jpeg_out_buf)
			free(_input->jpeg_out_buf);
		_input->jpeg_info_buf = NULL;
		_input->jpeg_out_buf = NULL;
	}

	_yuv->size = frame->width * frame->height;
	_yuv->size += _yuv->size / 2;
	if (NULL == _yuv->p_frame)
		_yuv->p_frame =
			(unsigned char *)malloc(_yuv->size);

	if (_yuv->p_frame) {
		memcpy(_yuv->p_frame, frame->viraddr, _yuv->size);
	} else {
		camera_free_mem(_input, _yuv);
		ALOGD("frame_info.p_frame is NULL\n");
	}
	ALOGD("return buff: %p\n", frame);

	return ret;
}

static int on_rgb_face_pic_cb(face_frame_t *frame)
{
	int ret = 0;

	ALOGD("enter %s\n", __func__);

	memset(&rgb_input, 0x00, sizeof(rgb_input));
	ret = on_frame_encode(frame, &rgb_input, &rgb_output, &rgb_yuv_frame_info);

	/*cam_photo_save(&rgb_output, &rgb_yuv_frame_info, "/tmp/capture_cam");*/
	ret = face_return_frame(svc_handle, frame);
	ALOGD("[camera_pic_return_frame]ret=%d\n", ret);
	if (ret)
		ALOGD("CAMERA_RGB: camera frame return failed!");

	ALOGD("send rgb callback message\n");
	sem_post(&sem);

	return ret;
}
#endif

void camera_free_mem_output(char *_out)
{
	if (_out) {
		ALOGD("***[camera_free_mem_output]free _out.\n");
		free(_out);
		_out = NULL;
	}
}

int take_photo(unsigned char **pbin, int *len)
{
	int ret = 0;
#if 0
	struct timeval now;
	struct timespec out_time;

	ret = sem_init(&sem, 0, 0);
	ALOGD("[sem_init]ret=%d\n", ret);

	svc_handle = get_face_handle();

	if (NULL == svc_handle) {
		ALOGE("%s: svc_handle is NULL\n", __func__);
		return ret;
	}

	ret = face_take_picture(svc_handle, FACE_CAMERA_RGB, on_rgb_face_pic_cb);
	ALOGD("[face_take_picture]ret=%d\n", ret);

	gettimeofday(&now, NULL);
	out_time.tv_sec = now.tv_sec + 5;
	out_time.tv_nsec = now.tv_usec * 1000;
	ALOGD("[sem_timedwait]wait photo...\n");
	ret = sem_timedwait(&sem, &out_time);
	ALOGD("[sem_timedwait]ret=%d\n", ret);

	*pbin = rgb_output.buf;
	*len = rgb_output.size;

	sem_destroy(&sem);
#endif
	return ret;
}

/**
 * take_photo_proc - Interface function for photo capture.
 * @file_name: IN, File name.
 *
 * NOTE: Wait for the 'record_init' call before you can use this function.
 *
 * Returns 0 if success, otherwise not 0.
 */
int take_photo_proc(char *file_name)
{
#if 0
	FILE *fp = NULL;
	unsigned char *bin = NULL;
	int len = 0, ret = 0;

	assert(file_name != NULL);

	ret = take_photo(&bin, &len);
	if (ret)
		return ret;

	fp = fopen(file_name, "wb");
	if (fp) {
		fwrite(bin, 1, len, fp);
		fclose(fp);
	}

	return ret;
#else
	return 0;
#endif

}

#else
/**
 * take_photo_proc - Interface function for photo capture.
 * @file_name: IN, File name.
 *
 * NOTE: Wait for the 'record_init' call before you can use this function.
 *
 * Returns 0 if success, otherwise not 0.
 */
int take_photo_proc(char *file_name)
{
	int ret = -1;
	void *video_rec_hdl;

	assert(file_name != NULL);

	video_rec_hdl = get_recorder_handle();
	if (NULL == video_rec_hdl) {
		ALOGE("[%s:%d] get_recorder_handle fail (record_init can't call)\n",
			__func__, __LINE__);
		return -1;
	}

	ret = video_rec_take_picture(video_rec_hdl, file_name);

	return ret;
}

/**
 * take_photo - Interface function for photo capture.
 * @pbin: OUT, data of photo capture.
 * @len: OUT, length of data.
 *
 * NOTE: Wait for the 'record_init' call before you can use this function.
 *
 * Returns 0 if success, otherwise not 0.
 */
int take_photo(unsigned char **pbin, int *len)
{
	char *buffer;
	char file_name[256];
	int ret = 0, length;
	FILE *fp;

	snprintf(file_name, sizeof(file_name), "%s", "/data/mytest.jpg");
	ret = take_photo_proc(file_name);
	if (0 != ret) {
		*pbin = NULL;
		*len = 0;
		return ret;
	}

	fp = fopen(file_name, "rb");
	if (NULL == fp) {
		*pbin = NULL;
		*len = 0;
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	length = ftell(fp);

	buffer = (char *)malloc(length + 1);
	if (NULL == buffer) {
		fclose(fp);
		*pbin = NULL;
		*len = 0;
		return -2;
	}

	memset(buffer, 0, length + 1);
	fseek(fp, 0, SEEK_SET);
	fread(buffer, 1, length, fp);
	fclose(fp);

	remove(file_name);

	*pbin = (unsigned char *)buffer;
	*len = length;

	return 0;
}

/**
 * camera_free_mem_output - Interface function for free data of photo capture.
 * @out: data buffer of photo capture.
 *
 * NOTE: Wait for the 'record_init' call before you can use this function.
 *
 * Returns None.
 */
void camera_free_mem_output(char *out)
{
	if (out)
		free(out);
}
#endif

