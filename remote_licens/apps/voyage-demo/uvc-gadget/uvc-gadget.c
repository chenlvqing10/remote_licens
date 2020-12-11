/*
 * uvc-gadget.c - USB Video Class Gadget Application
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

#include <getopt.h>
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

#define  LOG_TAG			"uvc-gadget"
#include "log/log.h"

#include "base_component.h"
#include "omx_api.h"
#include "lb_recorder.h"
#include "recorder_private.h"

#include "uvc.h"
#include "list.h"

#define UVC_SUPPORT_H264		0

#define FRONT_INPUT_DEVICE_NAME		"/dev/video3"
#define UVC_OUTPUT_DEVICE_NAME		"/dev/video5"

#define CLEAR(x)			memset(&(x), 0, sizeof(x))
#define ARRAY_SIZE(a)			(sizeof(a) / sizeof((a)[0]))
#define pixfmtstr(x)			(x) & 0xff,			\
					((x) >> 8) & 0xff,		\
					((x) >> 16) & 0xff,		\
					((x) >> 24) & 0xff
#define clamp(val, min, max)						\
	({								\
		typeof(val) __val = (val);				\
		typeof(min) __min = (min);				\
		typeof(max) __max = (max);				\
		(void) (&__val == &__min);				\
		(void) (&__val == &__max);				\
		__val = __val < __min ? __min : __val;			\
		__val > __max ? __max : __val;				\
	})

struct video_buffer {
	struct list_entry		list;

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
	struct video_buffer		*buffers;
	struct list_entry		buffer_list;
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
	struct uvc_frame_info		*frames;
};

static struct uvc_frame_info uvc_frames_mjpeg[] = {
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
	{
		0,
		0,
		{0,},
	},
};

unsigned int mjpeg_fps_interval_tab[][8] = {

#define M_15_FPS_INDEX		0
#define M_30_FPS_INDEX		1

	{666666, 1000000, 5000000, 0},	/* 15 fps */
	{333333, 666666, 1000000, 0},	/* 30 fps */

	{0, 0, 0, 0},
};

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

static int video_record_start(struct uvc_device *dev)
{
	vsrc_param_t src_para;
	enc_para_t enc_para;
	app_stream_cb_t cb;
	int ret = 0;

	dev->vstream = video_stream_create();
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

	ret = video_stream_set_camera_source(dev->vstream, dev->video_devname);
	if (ret) {
		ALOGE("failed to set video stream source device as %s (%d)\n",
		      dev->video_devname, ret);
		goto err;
	}

	memset(&src_para, 0, sizeof(src_para));
	src_para.width = dev->width;
	src_para.height = dev->height;
	src_para.frame_rate = 30;
	ret = video_stream_set_src_para(dev->vstream, &src_para);
	if (ret) {
		ALOGE("failed to set video stream source parameters (%d)\n",
		      ret);
		goto err;
	}

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

	cb.app_data = dev;
	cb.buf_handle = video_stream_buf_handle;
	video_rec_set_stream_cb(dev->video_rec, &cb);

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
	list_remove(&buffer->list);

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
	list_append(&buffer->list, &dev->buffer_list);

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

	dev->buffers = calloc(dev->nbufs, sizeof(*dev->buffers));
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

/**
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
	list_init(&dev->buffer_list);
	for (i = 0; i < dev->nbufs; i++)
		list_append(&dev->buffers[i].list, &dev->buffer_list);
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
		video_record_stop(dev);
		uvc_video_stream_off(dev);
		uvc_video_reqbufs(dev, 0);
		dev->is_streaming = 0;
		uvc_video_free_buffers(dev);
	}
}

static void uvc_fill_streaming_control(struct uvc_device *dev,
				       struct uvc_streaming_control *ctrl,
				       int iframe,
				       int iformat)
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

	memset(ctrl, 0, sizeof(*ctrl));
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
					struct usb_ctrlrequest *ctrl,
					struct uvc_request_data *resp)
{
	 ALOGD("standard request\n");
}

static void uvc_events_process_control(struct uvc_device *dev,
				       uint8_t req,
				       uint8_t cs,
				       uint8_t entity_id,
				       uint8_t len,
				       struct uvc_request_data *resp)
{
	ALOGD("control request (req %02x cs %02x)\n", req, cs);
}

static void uvc_events_process_streaming(struct uvc_device *dev,
					 uint8_t req,
					 uint8_t cs,
					 struct uvc_request_data *resp)
{
	struct uvc_streaming_control *ctrl;

	ALOGD("streaming request (req %02x cs %02x)\n", req, cs);

	if (cs != UVC_VS_PROBE_CONTROL && cs != UVC_VS_COMMIT_CONTROL)
		return;

	ctrl = (struct uvc_streaming_control *)&resp->data;
	resp->length = sizeof(*ctrl);

	switch (req) {
	case UVC_SET_CUR:
		dev->control = cs;
		resp->length = 34;
		break;

	case UVC_GET_CUR:
		if (cs == UVC_VS_PROBE_CONTROL)
			memcpy(ctrl, &dev->probe, sizeof(*ctrl));
		else
			memcpy(ctrl, &dev->commit, sizeof(*ctrl));
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
				     struct usb_ctrlrequest *ctrl,
				     struct uvc_request_data *resp)
{
	if ((ctrl->bRequestType & USB_RECIP_MASK) != USB_RECIP_INTERFACE)
		return;

	switch (ctrl->wIndex & 0xff) {
	case UVC_INTF_CONTROL:
		uvc_events_process_control(dev, ctrl->bRequest,
					   ctrl->wValue >> 8,
					   ctrl->wIndex >> 8,
					   ctrl->wLength,
					   resp);
		break;

	case UVC_INTF_STREAMING:
		uvc_events_process_streaming(dev, ctrl->bRequest,
					     ctrl->wValue >> 8,
					     resp);
		break;

	default:
		break;
	}
}

static void uvc_events_process_setup(struct uvc_device *dev,
				     struct usb_ctrlrequest *ctrl,
				     struct uvc_request_data *resp)
{
	dev->control = 0;

	ALOGD("\nbRequestType %02x bRequest %02x wValue %04x \
	wIndex %04x wLength %04x\n",
	      ctrl->bRequestType, ctrl->bRequest, ctrl->wValue,
	      ctrl->wIndex, ctrl->wLength);

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

	memset(&resp, 0, sizeof(resp));
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

	memset(&sub, 0, sizeof(sub));
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

static void usage(const char *argv0)
{
	fprintf(stderr, "Usage: %s [options]\n", argv0);
	fprintf(stderr, "Available options are\n");

	fprintf(stderr, " -h, --help					");
	fprintf(stderr, "Print this help screen and exit\n");

	fprintf(stderr, " -U, --uvc	<device>(default:/dev/video5)	");
	fprintf(stderr, "UVC Video Output device\n");

	fprintf(stderr, " -V, --video	<device>(default:/dev/video3)	");
	fprintf(stderr, "Video Input device\n");

	fprintf(stderr, "     --format	<format>(default:mjpeg)		");
	fprintf(stderr, "Format of video streaming. support mjpeg or h264\n");

	fprintf(stderr, "     --fps	<fps>	(default:15)		");
	fprintf(stderr, "Frames per second. support 15 or 30\n");

	return;
}

int set_uvc_mjpeg_fps(char *fps)
{
	unsigned char i = 0;
	unsigned char fps_index;

	ALOGD("%s: %s\n", __func__, fps);

	if (!strcmp(fps, "15")) {
		fps_index = M_15_FPS_INDEX;
	} else if (!strcmp(fps, "30")) {
		fps_index = M_30_FPS_INDEX;
	} else {
		ALOGE("mjpeg not support this fps!");
		return -EINVAL;
	}

	for (i = 0; i < ARRAY_SIZE(uvc_frames_mjpeg) - 1; i++) {
		memcpy(uvc_frames_mjpeg[i].intervals,
		       mjpeg_fps_interval_tab[fps_index],
		       sizeof(mjpeg_fps_interval_tab[0]));
	}

	return 0;
}

int set_uvc_h264_fps(char *fps)
{
	ALOGD("%s: %s\n", __func__, fps);

	if (!strcmp(fps, "15")) {
		ALOGW("h264 not support 15 fps.");
		return -EINVAL;
	} else if (!strcmp(fps, "30")) {
		ALOGD("h264 set 30 fps.");
	} else {
		ALOGE("h264 not support this fps!");
		return -EINVAL;
	}

	return 0;
}


int set_uvc_fps(struct uvc_format_info *uvc_info, char *fps)
{
	int retval = -EINVAL;

	ALOGD("%s: %s\n", __func__, fps);

	if (V4L2_PIX_FMT_MJPEG == uvc_info->fcc)
		retval = set_uvc_mjpeg_fps(fps);
	else if (V4L2_PIX_FMT_H264 == uvc_info->fcc)
		retval = set_uvc_h264_fps(fps);
	else {
		ALOGE("uvc format is inval!\n");
		return -EINVAL;
	}

	return retval;
}

int set_uvc_format(struct uvc_format_info *uvc_info, char *format)
{
	ALOGD("%s: %s\n", __func__, format);

	if (!strcmp(format, "mjpeg")) {
		uvc_info->fcc = V4L2_PIX_FMT_MJPEG;
		uvc_info->frames = uvc_frames_mjpeg;
	} else if (!strcmp(format, "h264")) {
		uvc_info->fcc = V4L2_PIX_FMT_H264;
		uvc_info->frames = uvc_frames_h264;
	} else {
		ALOGE("Not support video format %s\n", optarg);
		return -EINVAL;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct uvc_device *dev;
	struct uvc_format_info *uvc_info = &(uvc_formats[0]);

	char *video_devname = FRONT_INPUT_DEVICE_NAME;
	char *uvc_devname = UVC_OUTPUT_DEVICE_NAME;
	unsigned int video_format = V4L2_PIX_FMT_MJPEG;

	fd_set fdsu;
	int ret, opt;

	char * const short_options = "h:U:V:";
	int long_opt_index = 0;
	unsigned int param_set_map = 0;

	struct option long_options[] = {
		{"help", 0, NULL, 'h'},
		{"uvc", 1, NULL, 'u'},
		{"video", 1, NULL, 'v'},

		{"format", 1, &ret, E_RET_FORMAT},
		{"fps", 1, &ret, E_RET_FPS},

		{0, 0, 0, 0},
	};

	while ((opt =
	getopt_long(argc, argv, short_options, long_options, &long_opt_index))
	!= -1) {

		switch (opt) {
		case 'h':
			usage(argv[0]);
			return 0;

		case 'u':
		case 'U':
			if (optarg)
				ALOGD("uvc dev is %s\n", optarg);
			uvc_devname = optarg;
			break;

		case 'v':
		case 'V':
			if (optarg)
				ALOGD("video dev is %s\n", optarg);
			video_devname = optarg;
			break;

		case 0:
			if (E_RET_FORMAT == ret) {
				ret = set_uvc_format(uvc_info, optarg);
				if (ret) {
					printf("set_uvc_format error!\n");
					return -EINVAL;
				}
				param_set_map |= BIT(E_RET_FORMAT);
			} else if (E_RET_FPS == ret) {
				if (0 == (param_set_map & BIT(E_RET_FORMAT))) {
					printf("Please set the video ");
					printf("streaming format before!\n");
					return -EINVAL;
				}

				ret = set_uvc_fps(uvc_info, optarg);
				if (ret) {
					printf("set uvc fps error!\n");
					return -EINVAL;
				}
			}

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

	dev->video_devname = video_devname;
	dev->fcc = video_format;
	pthread_spin_init(&dev->buffer_lock, 0);

	uvc_video_set_format(dev);

	/* Init UVC events. */
	uvc_events_init(dev);

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
	}

	uvc_handle_streamoff_event(dev);

	pthread_spin_destroy(&dev->buffer_lock);

	uvc_device_close(dev);

	return 0;
}

