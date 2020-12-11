/*
 * uvc-face-recg.c - USB Video Class Gadget Application
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

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/usb/video.h>
#include <linux/videodev2.h>

#define  LOG_TAG			"uvc-face-recg"
#include "log/log.h"

#include "record_export.h"
#include "base_component.h"
#include "omx_api.h"
#include "lb_recorder.h"
#include "recorder_private.h"
#include "lombo_malloc.h"
#include "face.h"
#include "uvc.h"
#include "ee_list.h"
#include "config.h"
#include "db.h"
#include "person_register.h"
#include "db_person_info.h"
#include "face_svc.h"
#include "app_lang.h"
#include "person_import.h"
#include "watermark_api.h"
#include "elang.h"
#include "uvc_comm.h"

static int video_port_buf_handle(void *app_data, frame_t *pframe);

#define BLENDING_ENABLE           0
#define UVC_SUPPORT_1080p         1
#define UVC_SUPPORT_FRAME_RATE_30 1
#define UVC_USE_COM_INPUT_CMD     0
#define UVC_SUPPORT_H264          0


/* #define USE_DMA_BUFFER */

#define FRONT_INPUT_DEVICE_NAME		"/dev/video3"
#define UVC_OUTPUT_DEVICE_NAME		"/dev/video5"

#define CLEAR(x)			memset(&(x), 0, sizeof(x))
#define ARRAY_SIZE(a)			(sizeof(a) / sizeof((a)[0]))
#define pixfmtstr(x)							\
	(x) & 0xff, ((x) >> 8) & 0xff, ((x) >> 16) & 0xff, ((x) >> 24) & 0xff
#define clamp(val, min, max)						\
	({								\
		typeof(val) __val = (val);				\
		typeof(min) __min = (min);				\
		typeof(max) __max = (max);				\
		(void) (&__val == &__min);				\
		(void) (&__val == &__max);				\
		__val = __val < __min ? __min: __val;			\
		__val > __max ? __max: __val;				\
	})

struct video_buffer
{
	struct list_head		list;

	unsigned int			index;
	unsigned int			size;
	unsigned int			bytesused;
	void				*mem;
};

/* struct uvc_device - runtime info holder for uvc device. */
struct uvc_device {
	/* uvc device specific */
	int				uvc_fd;
	int				is_streaming;

	/* uvc control request specific */
	struct uvc_streaming_control	probe;
	struct uvc_streaming_control	commit;
	int				control;

	/* uvc buffer specific */
	unsigned int			nbufs;
	struct video_buffer 		*buffers;
	struct list_head		buffer_list;
	pthread_spinlock_t		buffer_lock;

	/* frame control specific */
	unsigned int			fcc;
	unsigned int			width;
	unsigned int			height;
	unsigned int			fps;
	unsigned int			fcnt;

	/* video recorder specific */
	char				*video_devname;
	void				*vstream;
	void				*video_rec;
};

struct uvc_frame_info {
	unsigned int			width;
	unsigned int			height;
	unsigned int			intervals[8];
};

struct uvc_format_info {
	unsigned int			fcc;
	const struct uvc_frame_info	*frames;
};

static const struct uvc_frame_info uvc_frames_mjpeg[] = {
#if UVC_SUPPORT_FRAME_RATE_30
	{
		640,
		360,
		{333333, 666666, 1000000, 0},
	},
	{
		1280,
		720,
		{333333, 666666, 1000000, 0},
	},
	{
		1920,
		1080,
		{333333, 666666, 1000000, 0},
	},
#else
	{
		640,
		360,
		{666666, 1000000, 5000000, 0},
	},
	{
		1280,
		720,
		{666666, 1000000, 5000000, 0},
	},
	{
		1920,
		1080,
		{666666, 1000000, 5000000, 0},
	},
#endif
	{
		0,
		0,
		{0,},
	},
};

/*h264 only supports frame rate of 30fps */
static struct uvc_frame_info uvc_frames_h264[] = {
	{
		640,
		360,
		{333333, 666666, 1000000, 0},
	},
	{
		1280,
		720,
		{333333, 666666, 1000000, 0},
	},
	{
		1920,
		1080,
		{333333, 666666, 1000000, 0},
	},
	{
		0,
		0,
		{0,},
	},
};

static struct uvc_format_info uvc_formats[1] = {
	{V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg},
};

static struct video_buffer *uvc_video_take_buffer(struct uvc_device *dev);
static int uvc_video_release_buffer(struct uvc_device *dev, unsigned int index);
int g_no_face_detect_flag;

struct uvc_comm_info *info;

static int video_stream_buf_handle(void *handler, avstream_t *stream)
{
	struct uvc_device *dev = (struct uvc_device *)handler;
	int ret;

	/* video recorder support only 30fps */
	if (dev->is_streaming && !((dev->fcnt++) % (30 / dev->fps))) {
		struct video_buffer *buf;
		struct v4l2_buffer vbuf;

		buf = uvc_video_take_buffer(dev);
		if (buf == NULL) {
			ALOGW("failed to take buffer\n");
			return 0;
		}

		buf->size = stream->data_size;
		memcpy(buf->mem, stream->data, stream->data_size);

		CLEAR(vbuf);
		vbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		vbuf.memory = V4L2_MEMORY_USERPTR;
		vbuf.index = buf->index;
		vbuf.length = stream->data_size;
		vbuf.bytesused = stream->data_size;
		vbuf.m.userptr = (unsigned long)buf->mem;

		ret = ioctl(dev->uvc_fd, VIDIOC_QBUF, &vbuf);
		if (ret < 0) {
			ALOGW("VIDIOC_QBUF failed %s (%d)\n",
				strerror(errno), errno);
			uvc_video_release_buffer(dev, buf->index);
			return 0;
		}
	}

	return 0;
}

static void *uvc_video_start(char *source, int width, int height,
		vdisp_rotate_mode_e rotation)
{
	void *hdl;
	int ret;
#if UVC_USE_COM_INPUT_CMD
	app_port_filter_t cb;
#endif

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

#if UVC_USE_COM_INPUT_CMD
	cb.buf_handle = video_port_buf_handle;
	cb.app_data = NULL;
	cb.type = CB_VCAM_OUT;
	ret = video_stream_set_port_cb(hdl, &cb);
	if (ret) {
		ALOGE("video_stream_set_port_cb failed\n");
		return NULL;
	}
#endif
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

static int video_record_start(struct uvc_device *dev)
{
	enc_para_t enc_para;
	app_stream_cb_t stream_cb;
	int ret = 0;

	dev->vstream = uvc_video_start(dev->video_devname, dev->width,
			dev->height, 0);
	if (!dev->vstream) {
		ALOGE("failed to create video stream\n");
		return -EIO;
	}

	dev->video_rec = video_rec_creat();
	if (!dev->video_rec) {
		ALOGE("failed to create video recorder\n");
		ret = -EIO;
		goto err;
	}

	video_rec_set_video_stream(dev->video_rec, dev->vstream);

	memset(&enc_para, 0, sizeof(enc_para));
	enc_para.width = dev->width;
	enc_para.height = dev->height;
	if (V4L2_PIX_FMT_H264 == dev->fcc) {
		enc_para.venc_format = VIDEO_ENCODER_H264;
		if (1920 == dev->width && 1080 == dev->height)
			enc_para.bitrate = 20000000;
		else if (1280 == dev->width && 720 == dev->height)
			enc_para.bitrate = 10000000;
		else if (640 == dev->width && 360 == dev->height)
			enc_para.bitrate = 5000000;
	} else if (V4L2_PIX_FMT_MJPEG == dev->fcc) {
		enc_para.venc_format = VIDEO_ENCODER_MJPEG;
		enc_para.quality = 60;
	}
	enc_para.fmt = REC_OUTPUT_FORMAT_RAW;
	ret = video_rec_set_para(dev->video_rec, &enc_para);
	if (ret) {
		ALOGE("failed to set video record parameters (%d)\n", ret);
		goto err;
	}

	stream_cb.app_data = dev;
	stream_cb.buf_handle = video_stream_buf_handle;
	video_rec_set_stream_cb(dev->video_rec, &stream_cb);

	ret = video_rec_start(dev->video_rec);
	if (ret) {
		ALOGE("failed to start video record (%d)\n", ret);
		goto err;
	}

	ALOGD("video record had been started\n");

	return 0;

err:
	if (dev->video_rec) {
		video_rec_release(dev->video_rec);
		dev->video_rec = NULL;
	}
	if (dev->vstream) {
		video_stream_release(dev->vstream);
		dev->video_rec = NULL;
	}

	return ret;
}

static void video_record_stop(struct uvc_device *dev)
{
	if (dev->video_rec) {
		video_rec_stop(dev->video_rec);
		video_rec_release(dev->video_rec);
		dev->video_rec = NULL;
	}

	if (dev->vstream) {
		video_stream_release(dev->vstream);
		dev->vstream = NULL;
	}

	ALOGD("video record had been stopped\n");
}

static int uvc_device_open(struct uvc_device **dev, char *dev_name)
{
	struct uvc_device *tmp_dev;
	struct v4l2_capability cap;
	int fd, ret = 0;

	fd = open(dev_name, O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		ALOGE("failed to open uvc device %s (%d)\n",
			strerror(errno), errno);
		return -EIO;
	}

	ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret < 0) {
		ALOGE("failed to query capabilities of uvc device %s (%d)\n",
			strerror(errno), errno);
		goto err;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) {
		ALOGE("%s is no video output device\n", dev_name);
		ret = -EINVAL;
		goto err;
	}

	tmp_dev = calloc(1, sizeof(struct uvc_device));
	if (!tmp_dev) {
		ALOGE("failed to calloc for uvc device\n");
		ret = -ENOMEM;
		goto err;
	}

	ALOGD("uvc device is %s on bus %s\n", cap.card, cap.bus_info);
	ALOGD("uvc device open succeeded, fd %d\n", fd);

	memset(tmp_dev, 0, sizeof(struct uvc_device));
	tmp_dev->uvc_fd = fd;
	*dev = tmp_dev;

	return 0;

err:
	close(fd);

	return ret;
}

static void uvc_device_close(struct uvc_device *dev)
{
	if (dev) {
		close(dev->uvc_fd);
		free(dev);
	}

	ALOGD("uvc device have been closed\n");
}

static int uvc_video_set_format(struct uvc_device *dev)
{
	struct v4l2_format fmt;
	int ret;

	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	fmt.fmt.pix.width = dev->width;
	fmt.fmt.pix.height = dev->height;
	fmt.fmt.pix.pixelformat = dev->fcc;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
#if 0
	if (dev->fcc == V4L2_PIX_FMT_MJPEG)
		fmt.fmt.pix.sizeimage = dev->width * dev->height * 1.5;
#endif

	ret = ioctl(dev->uvc_fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		ALOGE("failed to set format %s (%d)\n",
			strerror(errno), errno);
		return ret;
	}

	ALOGD("setting format to %c%c%c%c %ux%u\n",
		pixfmtstr(dev->fcc), dev->width, dev->height);

	return 0;
}

static int uvc_video_stream_on(struct uvc_device *dev)
{
	int ret, type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	ret = ioctl(dev->uvc_fd, VIDIOC_STREAMON, &type);
	if (ret < 0) {
		ALOGE("failed to start streaming %s (%d)\n",
			strerror(errno), errno);
		return ret;
	}

	ALOGD("starting video stream\n");

	return 0;
}

static int uvc_video_stream_off(struct uvc_device *dev)
{
	int ret, type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	ret = ioctl(dev->uvc_fd, VIDIOC_STREAMOFF, &type);
	if (ret < 0) {
		ALOGE("failed to stop streaming %s (%d)\n",
			strerror(errno), errno);
		return ret;
	}

	printf("stopping video stream\n");

	return 0;
}

static int uvc_video_process(struct uvc_device *dev)
{
	struct video_buffer *buffer;
	struct v4l2_buffer buf;
	int ret;

	if (!dev->is_streaming)
		return 0;

	/* Prepare a v4l2 buffer to be dequeued from UVC domain. */
	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_USERPTR;

	/* Dequeue the spent buffer from UVC domain. */
	ret = ioctl(dev->uvc_fd, VIDIOC_DQBUF, &buf);
	if (ret < 0) {
		ALOGE("failed to dequeue buffer %s (%d)\n",
			strerror(errno), errno);
		return ret;
	}

	if (buf.index >= dev->nbufs) {
		ALOGE("unexpected buffer index %u\n", buf.index);
		return -EINVAL;
	}

	buffer = &dev->buffers[buf.index];
	if (((void *)buf.m.userptr) != buffer->mem) {
		ALOGE("unexpected buffer %p index %u size %u\n",
			(void *)buf.m.userptr, buf.index, buf.length);
		return -EINVAL;
	}

	return uvc_video_release_buffer(dev, buf.index);
}

static struct video_buffer *uvc_video_take_buffer(struct uvc_device *dev)
{
	struct video_buffer *buffer;

	pthread_spin_lock(&dev->buffer_lock);

	if (list_empty(&dev->buffer_list)) {
		pthread_spin_unlock(&dev->buffer_lock);
		return NULL;
	}

	buffer = list_first_entry(&dev->buffer_list, typeof(*buffer), list);
	list_del(&buffer->list);

	pthread_spin_unlock(&dev->buffer_lock);

	return buffer;
}

static int uvc_video_release_buffer(struct uvc_device *dev, unsigned int index)
{
	struct video_buffer *buffer;

	if (index >= dev->nbufs)
		return -EINVAL;

	pthread_spin_lock(&dev->buffer_lock);

	buffer = &dev->buffers[index];
	list_add_tail(&buffer->list, &dev->buffer_list);

	pthread_spin_unlock(&dev->buffer_lock);

	return 0;
}

static int uvc_video_free_buffers(struct uvc_device *dev)
{
	struct video_buffer *buffer;
	unsigned int i;

	if (0 == dev->nbufs)
		return 0;

	for (i = 0; i < dev->nbufs; i++) {
		buffer = &dev->buffers[i];
		free(buffer->mem);
		memset(buffer, 0, sizeof(*buffer));
	}

	free(dev->buffers);
	dev->buffers = NULL;
	dev->nbufs = 0;

	return 0;
}

static int uvc_video_alloc_buffers(struct uvc_device *dev, unsigned int nbufs)
{
	unsigned int i, size = dev->width * dev->height * 1.5;
	int ret;

	if (dev->nbufs != 0) {
		ALOGE("there's some buffers hasn't been free\n");
		return -EBUSY;
	}

	dev->nbufs = nbufs;

	dev->buffers = calloc(dev->nbufs, sizeof *dev->buffers);
	if (!dev->buffers) {
		ALOGE("failed to alloc for uvc device buffers\n");
		return -ENOMEM;
	}

	for (i = 0; i < dev->nbufs; ++i) {
		dev->buffers[i].mem = malloc(size);
		if (!dev->buffers[i].mem) {
			ALOGE("failed to alloc for buffer %u\n", i);
			ret = -ENOMEM;
			goto done;
		}
		dev->buffers[i].index = i;
		dev->buffers[i].size = size;
	}

	ret = 0;
	ALOGD("%u buffers alloc\n", dev->nbufs);

done:
	if (ret < 0)
		uvc_video_free_buffers(dev);

	return ret;
}

static int uvc_video_reqbufs(struct uvc_device *dev, int nbufs)
{
	struct v4l2_requestbuffers rb;
	int ret = 0;

	CLEAR(rb);
	rb.count = nbufs;
	rb.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	rb.memory = V4L2_MEMORY_USERPTR;

	ret = ioctl(dev->uvc_fd, VIDIOC_REQBUFS, &rb);
	if (ret < 0) {
		if (-EINVAL == ret)
			ALOGE("does not support user pointer i/o\n");
		else
			ALOGE("failed to request buffers %s (%d)\n",
				strerror(errno), errno);
		goto err;
	}

	ALOGD("%u buffers allocated\n", rb.count);

	return rb.count;

err:
	return ret;
}

static void app_main_print_recg_info(face_t *face)
{
	char info[128];
	float threshold = face_recg_threshold();

	snprintf(info, sizeof(info), "db num:%5d", face_db_get_count());
	ALOGD("%s ; ", info);

	snprintf(info, sizeof(info), "face id:%5d", face->id);
	ALOGD("%s ; ", info);

	snprintf(info, sizeof(info), "size:  %dx%d", face->rect[2] - face->rect[0],
			face->rect[3] - face->rect[1]);
	ALOGD("%s ; ", info);

	snprintf(info, sizeof(info), "rect:  left %d top %d right %d bottom %d ",
			face->rect[0], face->rect[1], face->rect[2], face->rect[3]);
	ALOGD("%s ; ", info);

	snprintf(info, sizeof(info), "quality:%5d", face->quality);
	ALOGD("%s ; ", info);

	snprintf(info, sizeof(info), "score:%10.3f", face->score);
	ALOGD("%s ; ", info);

	snprintf(info, sizeof(info), "living:%d", face->living);
	ALOGD("%s ; ", info);

	if (face->score > threshold) {
		snprintf(info, sizeof(info), "name:%s", face->name);
		ALOGD("\n%s ;\n", info);
	}

	ALOGD("\n\n");
}

static void app_main_face_cb(faces_t *faces)
{
	int i = 0;
	cJSON *root;
	cJSON *header;
	cJSON *Array;
	face_camera_cfg_t rgb_cfg;

	memset(&rgb_cfg, 0x0, sizeof(face_camera_cfg_t));
	face_get_camera_config(get_face_handle(), FACE_CAMERA_RGB, &rgb_cfg);
	root = cJSON_CreateObject();
	header = cJSON_CreateObject();
	uvc_cjson_face_info(root, header, Array, faces, &rgb_cfg);
	uvc_write_info_to_file(info->write_face_info_fd, root);
	if (faces != NULL && faces->face_num > 0) {
		for (i = 0; i < faces->face_num; i++)
			app_main_print_recg_info(&faces->faces[i]);

		g_no_face_detect_flag = 0;
	} else {
		if (g_no_face_detect_flag == 0) {
			printf("%s\n", "NO DETECT FACE");
			g_no_face_detect_flag = 1;
		}
	}
}

#if BLENDING_ENABLE
#include "yuv_blending.h"
#define MAX_DET_PERSON 6
static yuv_blending_t g_blending_det[MAX_DET_PERSON];

static void live_video_show_name_img(int index, frame_t *pframe, face_t *face)
{
	int x_start = pframe->info.video.width
			* face->rect_smooth[0] / 1280;
	int y_start = pframe->info.video.height
			* face->rect_smooth[1] / 720;
	int x_end = pframe->info.video.width
			* face->rect_smooth[2] / 1280;

	g_blending_det[index].source.buffer_y = pframe->info.video.addr[0];
	g_blending_det[index].source.buffer_uv = pframe->info.video.addr[1];
	g_blending_det[index].blending_x_pos = x_start
			+ (x_end - x_start - g_blending_det[index].blending_width) / 2;
	g_blending_det[index].blending_y_pos = y_start;
	g_blending_det[index].source.width_stride = pframe->info.video.width;
	g_blending_det[index].source.height_aligned = pframe->info.video.height;

	blending_operation(&g_blending_det[index]);
}

static int blending_handle(face_t *face, frame_t *pframe)
{
	float threshold = face_recg_threshold();
	if (face->score > threshold) {
		if (strcmp(face->name, "A") == 0)
			live_video_show_name_img(0, pframe, face);
		else if (strcmp(face->name, "B") == 0)
			live_video_show_name_img(1, pframe, face);
		else if (strcmp(face->name, "C") == 0)
			live_video_show_name_img(2, pframe, face);
		else if (strcmp(face->name, "D") == 0)
			live_video_show_name_img(3, pframe, face);
		else if (strcmp(face->name, "E") == 0)
			live_video_show_name_img(4, pframe, face);
		else if (strcmp(face->name, "F") == 0)
			live_video_show_name_img(5, pframe, face);
	}

	return 0;
}

static void blending_init()
{
	for (int i = 0; i < MAX_DET_PERSON; i++) {
		FILE *fp_blending;

		char filename[128];

		sprintf(filename, "/usr/share/uvc-face-recg/res/img/%d.rgb", i + 1);

		fp_blending = fopen(filename, "rb");
		if (fp_blending == NULL) {
			printf("open file error\n");
			continue;
		}

		g_blending_det[i].colorspace = RGBA8888;
		g_blending_det[i].text_color = unknow_text;
		g_blending_det[i].blending_width = 64;
		g_blending_det[i].blending_height = 64;
		g_blending_det[i].blending_stride = g_blending_det[i].blending_width
				* 4;
		unsigned int blending_size = 4 * g_blending_det[i].blending_width
				* g_blending_det[i].blending_height;
		g_blending_det[i].blending_data = (unsigned char *) malloc(
				blending_size);
		fread(g_blending_det[i].blending_data, 1, blending_size, fp_blending);

		fclose(fp_blending);
	}

}
static void blending_deinit()
{
	for (int i = 0; i < MAX_DET_PERSON; i++) {
		if (g_blending_det[i].blending_data != NULL) {
			free(g_blending_det[i].blending_data);
			g_blending_det[i].blending_data = NULL;
		}
	}
}
#endif
#if UVC_USE_COM_INPUT_CMD
static int video_port_buf_handle(void *app_data, frame_t *pframe)
{
	int i;

	if (pframe == NULL || pframe->info.video.addr[0] == NULL)
		return -1;

	float threshold = face_recg_threshold();
	faces_t *face_info = faces_get();
	ALOGD("%s face_info.face_num: %d\n", __func__, face_info->face_num);

	if (face_info->face_num > 0) {
		for (i = 0; i < face_info->face_num; i++) {
			if (face_info->faces[i].score > threshold && face_info->faces[i].living)
				watermark_face_retangle_draw_by_color(
					&face_info->faces[i], pframe, 1280, 720,
					ENC_YUV420SP, 0x00FF00);
			else
				watermark_face_retangle_draw(&face_info->faces[i], pframe,
					1280, 720, ENC_YUV420SP);

#if BLENDING_ENABLE
			blending_handle(&face_info->faces[i], pframe);
#endif
		}
	}

	if (face_info)
		faces_put(face_info);

	return 0;
}
#endif
static int uvc_face_recg_init()
{
	int ret;

	/* Initialize config */
	ret = config_init();
	if (ret) {
		ALOGE("config init failed");
		return -1;
	}

	/* Check and initialize database */
	ret = db_init("uvc");
	if (ret) {
		ALOGE("db init failed");
		return -1;
	}

	ret = lang_init();
	if (ret) {
		ALOGE("lang init failed\n");
		return -1;
	}

	face_svc_set_conf_file_path("/etc/uvc_face_svc.conf");

	/* Initialize, config, start face service */
	ret = face_init();
	if (ret) {
		ALOGE("face init failed");
		return -1;
	}

	ret = face_set_cb(app_main_face_cb);
	if (ret) {
		ALOGE("set face cb failed");
		return -1;
	}

	return 0;
}

static void uvc_face_recg_deinit()
{
	face_set_cb(NULL);

	lang_deinit();

	face_deinit();

	config_deinit();
}

/*
 * This function is called in response to either:
 * - A SET_ALT(interface 1, alt setting 1) command from USB host,
 *   if the UVC gadget supports an ISOCHRONOUS video streaming endpoint
 *   or,
 * - A UVC_VS_COMMIT_CONTROL command from USB host, if the UVC gadget
 *   supports a BULK type video streaming endpoint.
 */
static int uvc_handle_streamon_event(struct uvc_device *dev)
{
	int ret, i;

	ret = uvc_video_reqbufs(dev, 4);
	if (ret < 0)
		goto err;

	ret = uvc_video_alloc_buffers(dev, ret);
	if (ret < 0)
		goto err;

	pthread_spin_lock(&dev->buffer_lock);
	INIT_LIST_HEAD(&dev->buffer_list);
	for (i = 0; i < dev->nbufs; i++)
		list_add_tail(&dev->buffers[i].list, &dev->buffer_list);
	pthread_spin_unlock(&dev->buffer_lock);

	video_record_start(dev);
	uvc_video_stream_on(dev);
	dev->is_streaming = 1;

	return 0;

err:
	return ret;
}

static void uvc_handle_streamoff_event(struct uvc_device *dev)
{
	if (dev->is_streaming) {
		dev->is_streaming = 0;
		video_record_stop(dev);
		uvc_video_stream_off(dev);
		uvc_video_reqbufs(dev, 0);
		/*Wait for the middleware to complete before releasing buffers.*/
		usleep(20000);
		uvc_video_free_buffers(dev);
	}
}

static void uvc_fill_streaming_control(struct uvc_device *dev,
	struct uvc_streaming_control *ctrl, int iframe, int iformat)
{
	const struct uvc_format_info *format;
	const struct uvc_frame_info *frame;
	unsigned int nframes;

	if (iformat < 0)
		iformat = ARRAY_SIZE(uvc_formats) + iformat;
	if (iformat < 0 || iformat >= (int)ARRAY_SIZE(uvc_formats))
		return;
	format = &uvc_formats[iformat];

	nframes = 0;
	while (format->frames[nframes].width != 0)
		++nframes;

	if (iframe < 0)
		iframe = nframes + iframe;
	if (iframe < 0 || iframe >= (int)nframes)
		return;
	frame = &format->frames[iframe];

	memset(ctrl, 0, sizeof *ctrl);
	ctrl->bmHint = 1;
	ctrl->bFormatIndex = iformat + 1;
	ctrl->bFrameIndex = iframe + 1;
	ctrl->dwFrameInterval = frame->intervals[0];
	switch (format->fcc) {
	case V4L2_PIX_FMT_YUYV:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case V4L2_PIX_FMT_MJPEG:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 1.5;
		break;
	case V4L2_PIX_FMT_H264:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 1.5;
		break;
	default:
		ALOGW("unsupported format %c%c%c%c\n", pixfmtstr(dev->fcc));
		break;
	}

	ctrl->dwMaxPayloadTransferSize = 512;
	ctrl->bmFramingInfo = 3;
	ctrl->bPreferedVersion = 1;
	ctrl->bMaxVersion = 1;
}

static void uvc_events_process_standard(struct uvc_device *dev,
	struct usb_ctrlrequest *ctrl, struct uvc_request_data *resp)
{
	 ALOGD("standard request\n");
}

static void uvc_events_process_control(struct uvc_device *dev, uint8_t req,
	uint8_t cs, uint8_t entity_id, uint8_t len,
	struct uvc_request_data *resp)
{
	ALOGD("control request (req %02x cs %02x)\n", req, cs);
}

static void uvc_events_process_streaming(struct uvc_device *dev, uint8_t req,
	uint8_t cs, struct uvc_request_data *resp)
{
	struct uvc_streaming_control *ctrl;

	ALOGD("streaming request (req %02x cs %02x)\n", req, cs);

	if (cs != UVC_VS_PROBE_CONTROL && cs != UVC_VS_COMMIT_CONTROL)
		return;

	ctrl = (struct uvc_streaming_control *)&resp->data;
	resp->length = sizeof *ctrl;

	switch (req) {
	case UVC_SET_CUR:
		dev->control = cs;
		resp->length = 34;
		break;

	case UVC_GET_CUR:
		if (cs == UVC_VS_PROBE_CONTROL)
			memcpy(ctrl, &dev->probe, sizeof *ctrl);
		else
			memcpy(ctrl, &dev->commit, sizeof *ctrl);
		break;

	case UVC_GET_MIN:
	case UVC_GET_MAX:
	case UVC_GET_DEF:
		uvc_fill_streaming_control(dev, ctrl,
			UVC_GET_MAX == req ? -1 : 0,
			UVC_GET_MAX == req ? -1 : 0);
		break;

	case UVC_GET_RES:
		CLEAR(ctrl);
		break;

	case UVC_GET_LEN:
		resp->data[0] = 0x00;
		resp->data[1] = 0x22;
		resp->length = 2;
		break;

	case UVC_GET_INFO:
		resp->data[0] = 0x03;
		resp->length = 1;
		break;

	default:
		break;
	}
}

static void uvc_events_process_class(struct uvc_device *dev,
	struct usb_ctrlrequest *ctrl, struct uvc_request_data *resp)
{
	if ((ctrl->bRequestType & USB_RECIP_MASK) != USB_RECIP_INTERFACE)
		return;

	switch (ctrl->wIndex & 0xff) {
	case UVC_INTF_CONTROL:
		uvc_events_process_control(dev, ctrl->bRequest,
			ctrl->wValue >> 8, ctrl->wIndex >> 8,
			ctrl->wLength, resp);
		break;

	case UVC_INTF_STREAMING:
		uvc_events_process_streaming(dev, ctrl->bRequest,
			ctrl->wValue >> 8, resp);
		break;

	default:
		break;
	}
}

static void uvc_events_process_setup(struct uvc_device *dev,
	struct usb_ctrlrequest *ctrl, struct uvc_request_data *resp)
{
	dev->control = 0;

#if 1
	ALOGD("\nbRequestType %02x bRequest %02x wValue %04x wIndex %04x "
		"wLength %04x\n", ctrl->bRequestType, ctrl->bRequest,
		ctrl->wValue, ctrl->wIndex, ctrl->wLength);
#endif

	switch (ctrl->bRequestType & USB_TYPE_MASK) {
	case USB_TYPE_STANDARD:
		uvc_events_process_standard(dev, ctrl, resp);
		break;

	case USB_TYPE_CLASS:
		uvc_events_process_class(dev, ctrl, resp);
		break;

	default:
		break;
	}
}

static int uvc_events_process_data(struct uvc_device *dev,
	struct uvc_request_data *data)
{
	struct uvc_streaming_control *target;
	struct uvc_streaming_control *ctrl;
	const struct uvc_format_info *format;
	const struct uvc_frame_info *frame;
	const unsigned int *interval;
	unsigned int iformat, iframe;
	unsigned int nframes;

	switch (dev->control) {
	case UVC_VS_PROBE_CONTROL:
		ALOGD("setting probe control, length %d\n", data->length);
		target = &dev->probe;
		break;

	case UVC_VS_COMMIT_CONTROL:
		ALOGD("setting commit control, length %d\n", data->length);
		target = &dev->commit;
		break;

	default:
		ALOGW("setting unknown control, length %d\n", data->length);
		return -EINVAL;
	}

	ctrl = (struct uvc_streaming_control *)&data->data;
	iformat = clamp((unsigned int)ctrl->bFormatIndex, 1U,
		(unsigned int)ARRAY_SIZE(uvc_formats));
	format = &uvc_formats[iformat - 1];

	nframes = 0;
	while (format->frames[nframes].width != 0)
		++nframes;

	iframe = clamp((unsigned int)ctrl->bFrameIndex, 1U, nframes);
	frame = &format->frames[iframe - 1];
	interval = frame->intervals;

	while (interval[0] < ctrl->dwFrameInterval && interval[1])
		++interval;

	target->bFormatIndex = iformat;
	target->bFrameIndex = iframe;
	switch (format->fcc) {
	case V4L2_PIX_FMT_YUYV:
		target->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case V4L2_PIX_FMT_MJPEG:
		target->dwMaxVideoFrameSize =
			frame->width * frame->height * 1.5;
		break;
	case V4L2_PIX_FMT_H264:
		target->dwMaxVideoFrameSize =
			frame->width * frame->height * 1.5;
		break;
	default:
		ALOGW("unsupported format %c%c%c%c\n", pixfmtstr(format->fcc));
		break;
	}

	target->dwFrameInterval = *interval;

	if (dev->control == UVC_VS_COMMIT_CONTROL) {
		dev->fcc = format->fcc;
		dev->width = frame->width;
		dev->height = frame->height;
		dev->fps = 1.0 / (target->dwFrameInterval / 10000000.0);

		uvc_video_set_format(dev);
	}

	return 0;
}

static void uvc_events_process(struct uvc_device *dev)
{
	struct v4l2_event v4l2_event;
	struct uvc_event *uvc_event = (void *)&v4l2_event.u.data;
	struct uvc_request_data resp;
	int ret;

	ret = ioctl(dev->uvc_fd, VIDIOC_DQEVENT, &v4l2_event);
	if (ret < 0) {
		ALOGE("VIDIOC_DQEVENT failed: %s (%d)\n",
			strerror(errno), errno);
		return;
	}

	memset(&resp, 0, sizeof resp);
	resp.length = -EL2HLT;

	switch (v4l2_event.type) {
	case UVC_EVENT_CONNECT:
		return;

	case UVC_EVENT_DISCONNECT:
		uvc_handle_streamoff_event(dev);
		return;

	case UVC_EVENT_SETUP:
		uvc_events_process_setup(dev, &uvc_event->req, &resp);
		break;

	case UVC_EVENT_DATA:
		ret = uvc_events_process_data(dev, &uvc_event->data);
		if (ret < 0)
			break;
		return;

	case UVC_EVENT_STREAMON:
		uvc_handle_streamon_event(dev);
		return;

	case UVC_EVENT_STREAMOFF:
		uvc_handle_streamoff_event(dev);
		return;

	default:
		break;
	}

	ret = ioctl(dev->uvc_fd, UVCIOC_SEND_RESPONSE, &resp);
	if (ret < 0) {
		ALOGE("UVCIOC_SEND_RESPONSE failed: %s (%d)\n",
			strerror(errno), errno);
		return;
	}
}

static int uvc_events_init(struct uvc_device *dev)
{
	struct v4l2_event_subscription sub;

	uvc_fill_streaming_control(dev, &dev->probe, 0, 0);
	uvc_fill_streaming_control(dev, &dev->commit, 0, 0);

	memset(&sub, 0, sizeof sub);
	sub.type = UVC_EVENT_CONNECT;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_DISCONNECT;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_SETUP;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_DATA;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_STREAMON;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	sub.type = UVC_EVENT_STREAMOFF;
	ioctl(dev->uvc_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);

	return 0;
}

static void uvc_process_thread(void *arg)
{
	struct uvc_device *dev = (struct uvc_device *) arg;
	fd_set fdsu;
	int ret;
	prctl(PR_SET_NAME, "uvc_process_thread");

	while (1) {
		FD_ZERO(&fdsu);
		/* We want both setup and data events on UVC interface. */
		FD_SET(dev->uvc_fd, &fdsu);

		fd_set efds = fdsu;
		fd_set dfds = fdsu;
		if (1 == dev->is_streaming)
			ret = select(dev->uvc_fd + 1, NULL, &dfds, &efds, NULL);
		else
			ret = select(dev->uvc_fd + 1, NULL, NULL, &efds, NULL);

		if (ret < 0) {
			ALOGE("select error %s (%d)\n", strerror(errno), errno);
			if (EINTR == errno)
				continue;
			break;
		} else if (!ret) {
			ALOGE("select timeout\n");
			break;
		}

		if (FD_ISSET(dev->uvc_fd, &efds))
			uvc_events_process(dev);
		if (FD_ISSET(dev->uvc_fd, &dfds))
			uvc_video_process(dev);
		pthread_testcancel();
	}
}

static void uvc_read_process_thread(void *arg)
{
	uvc_comm_info_t *comm_info = (uvc_comm_info_t *) arg;
	prctl(PR_SET_NAME, "uvc_read_process_thread");
	uvc_read_info(comm_info);
}

void *import_result_thread_func(void *args)
{
	person_import_ctx_t ctx;
	int i = 0;

	for (i = 0; i < 100; i++) {
		memset(&ctx, 0, sizeof(person_import_ctx_t));
		if (person_import_get_ctx(&ctx) == 0) {
			if (ctx.done != EE_THREAD_RUNNING) {
				if (ctx.result_code == PERSON_IMPORT_OK)
					printf("%s\n",
							elang_str("TEXT_PERSONAL_IMPORT_SUCCESSED"));
				else
					printf("%s\n",
							elang_str("TEXT_PERSONAL_IMPORT_FAILED"));

				break;
			}
		}

		sleep(1);
	}

	return NULL;
}

static void usage(const char *argv0)
{
	fprintf(stderr, "Usage: %s [options]\n", argv0);
	fprintf(stderr, "Available options are\n");
	fprintf(stderr, " -h	Print this help screen and exit\n");
	fprintf(stderr, " -u <device>	UVC Video Output device\n");
	fprintf(stderr, " -v <device>	Video Input device\n");
}

static void handler(int signum)
{
	printf("%s[%d],info->exit_flag = %d\n",__FUNCTION__,__LINE__,info->exit_flag);
	info->exit_flag = 1;
}
int main(int argc, char *argv[])
{
	struct uvc_device *dev;
	char *video_devname = FRONT_INPUT_DEVICE_NAME;
	char *uvc_devname = UVC_OUTPUT_DEVICE_NAME;
	int ret, opt;

	while ((opt = getopt(argc, argv, "bf:hm:n:r:s:t:u:v:")) != -1) {
		switch (opt) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'u':
			uvc_devname = optarg;
			break;
		case 'v':
			video_devname = optarg;
			break;
		default:
			printf("Invalid option '-%c'\n", opt);
			usage(argv[0]);
			return -EINVAL;
		}
	}

	ret = uvc_device_open(&dev, uvc_devname);
	if (ret || !dev) {
		ALOGE("failed to open uvc device %s\n", uvc_devname);
		return ret;
	}

	ret = uvc_comm_file_open(&info);
	if (ret) {
		ALOGE("failed to open uvc comm file\n");
		return ret;
	}

	dev->video_devname = video_devname;
#if  UVC_SUPPORT_H264
	dev->fcc = V4L2_PIX_FMT_H264;
	uvc_formats[1].fcc = V4L2_PIX_FMT_H264;
	uvc_formats[1].frames = uvc_frames_h264;
#else
	dev->fcc = V4L2_PIX_FMT_MJPEG;
#endif
#if UVC_SUPPORT_1080p
	dev->width = 1920;
	dev->height = 1080;
#else
	dev->width = 1280;
	dev->height = 720;
#endif
	pthread_spin_init(&dev->buffer_lock, 0);

	uvc_video_set_format(dev);

	/* Init UVC events. */
	uvc_events_init(dev);

	uvc_face_recg_init();

	info->pool = threadpool_create(6, 6, 0);
	if (!info->pool) {
		ALOGE("init thread pool faild\n");
		return -1;
	}

	ret = threadpool_add(info->pool, &uvc_process_thread, (void *)dev, 0);
	if (ret) {
		ALOGE("%s: failed to add task to thread pool\n", __func__);
		return ret;
	}

	ret = threadpool_add(info->pool, &uvc_read_process_thread, (void *)info, 0);
	if (ret) {
		ALOGE("%s: failed to add task to thread pool\n", __func__);
		return ret;
	}

#if BLENDING_ENABLE
	blending_init();
#endif
#if UVC_USE_COM_INPUT_CMD
	while (1) {
		char cmd[256];

		printf("\n>");
		fgets(cmd, 256, stdin);

		printf("Command is %s\n", cmd);
		if (strncmp(cmd, "delete", 6) == 0) {
			char name[64];
			printf("\nname>");
			fgets(name, sizeof(name), stdin);

			printf("name is %s\n", name);
			int len = strlen(name);
			if (len > 0 && name[len - 1] == '\n') {
				name[len - 1] = 0;
				len--;
			}

			if (len <= 0) {
				printf("Input cannot be empty,please input correctly(eg: name or all)!\n");
				continue;
			}
			else if (strncmp(name, "all", 3) == 0) {
				if (db_person_info_delete_all() == 0 && db_punch_record_delete_all() == 0) {
					face_clear_face_list();
					face_db_update();
					if (is_dir_exist(DIR_REGISTER) == 0) {
						if (dir_remove(DIR_REGISTER) == 0) {
							printf("Successfully deleted all registrants' info!\n");
						} else {
							printf("Deleting registered personnel info failed!\n");
						}
					} else {
						printf("No registrant info exist!\n");
					}
					if (is_dir_exist(DIR_RECG) == 0) {
						if (dir_remove(DIR_RECG) == 0) {
							printf("Successfully deleted all registrants' punch record!\n");
						} else {
							printf("Deleting registered personnel punch record failed!\n");
						}
					} else {
						printf("No registrant punch record exist!\n");
					}
				} else {
					printf("Deleting registered personnel failed!\n");
				}
			} else {
				ee_queue_t head;
				int count = 0;

				count = db_person_info_query_begin_by_name(name, &head);
				db_person_info_query_end(&head);
				if (count <= 0) {
					printf("%s\n", elang_str("TEXT_PERSON_REC_HAVE_NOT"));
					continue;
				}

				if (person_lookup_del_by_name(name) == 0) {
					face_clear_face_list_by_name(name);
					face_db_update();
					printf("Successfully deleted %s!\n", name);
				} else {
					printf("Deleting %s failed!\n", name);
				}
			}
		} else if (strncmp(cmd, "exit", 4) == 0) {
			break;
		} else if (strncmp(cmd, "add", 3) == 0) {
			char name[64];
			printf("\nname>");
			fgets(name, sizeof(name), stdin);

			printf("name is %s\n", name);
			int len = strlen(name);
			if (name[len - 1] == '\n')
				name[len - 1] = 0;
			app_management_reg_start(name);
		} else if (strncmp(cmd, "jpg_reg", 7) == 0) {
			char name[64];
			printf("\nname>");
			fgets(name, sizeof(name), stdin);

			printf("name is %s\n", name);
			int len = strlen(name);
			if (name[len - 1] == '\n')
				name[len - 1] = 0;

			char path[128];
			printf("\njpg path>");
			fgets(path, sizeof(path), stdin);

			printf("path is %s\n", path);
			len = strlen(path);
			if (path[len - 1] == '\n')
				path[len - 1] = 0;

			ret = app_management_reg_by_jpg_and_name(path, name);

			printf("jpg_reg is %s!\n", ret == 0 ? "successful" : "fail");
		} else {
			system(cmd);
		}
	}
#else
	signal(SIGINT, handler);
	while(!info->exit_flag) {
		usleep(1000 *1000);
	}
#endif

#if BLENDING_ENABLE
	blending_deinit();
#endif
	video_record_stop(dev);
	uvc_face_recg_deinit();
	uvc_handle_streamoff_event(dev);
	pthread_spin_destroy(&dev->buffer_lock);
	uvc_device_close(dev);
	threadpool_destroy(info->pool, 0);
	uvc_comm_file_close(info);

	return 0;
}

