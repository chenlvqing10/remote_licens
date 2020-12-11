/*
 * oscl_linux_vrec.c - eos video receive api used by lombo media framework.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define DBG_LEVEL		DBG_WARNING
#define LOG_TAG			"oscl_vrec"

#include "oscl_linux_vrec.h"

#define USE_DMA_BUFFER

#define VIDIOC_VISS_ISP_EXIF_REQ \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 21, struct viss_isp_exif)

vrec_pixformat_map_index_t vrec_pix_format_map_table[] = {
	/* Y UV need to be in a buffer */
	/* {V4L2_PIX_FMT_NV12, OMX_COLOR_FormatYUV420SemiPlanar, 1}, */
	/* Y UV can be an independent buffer */
	{V4L2_PIX_FMT_NV12M, OMX_COLOR_FormatYUV420SemiPlanar, 2},
};

static OMX_COLOR_FORMATTYPE sensorformat_to_omxformat(OMX_S32 format)
{
	OMX_S32 i = 0;
	vrec_pixformat_map_index_t *pix_map = NULL;

	for (i = 0; i < ARRAY_SIZE(vrec_pix_format_map_table); i++) {
		pix_map = &vrec_pix_format_map_table[i];
		if (pix_map->sensor_format == format)
			return pix_map->omx_format;
	}

	return -1;
}

static OMX_S32 omxformat_to_sensorformat(OMX_COLOR_FORMATTYPE format)
{
	OMX_S32 i = 0;
	vrec_pixformat_map_index_t *pix_map = NULL;

	for (i = 0; i < ARRAY_SIZE(vrec_pix_format_map_table); i++) {
		pix_map = &vrec_pix_format_map_table[i];
		if (pix_map->omx_format == format)
			return pix_map->sensor_format;
	}

	return -1;
}

static OMX_S32 sensorfomat_plans(OMX_S32 format)
{
	OMX_S32 i = 0;
	vrec_pixformat_map_index_t *pix_map = NULL;

	for (i = 0; i < ARRAY_SIZE(vrec_pix_format_map_table); i++) {
		pix_map = &vrec_pix_format_map_table[i];
		if (pix_map->sensor_format == format)
			return pix_map->buf_num;
	}
	return -1;
}

static OMX_ERRORTYPE isp_get_exif(void *hdl, omx_isp_info_t *info)
{
	int ret = 0;
	viss_isp_exif_t exif;
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;

	oscl_param_check((NULL != vrec), OMX_ErrorInsufficientResources, NULL);
	if (vrec->isp_fd <= 0)
		return OMX_ErrorInsufficientResources;

	memset(&exif, 0, sizeof(exif));
	ret = ioctl(vrec->isp_fd, VIDIOC_VISS_ISP_EXIF_REQ, &exif);
	if (ret < 0) {
		OSCL_LOGE("get isp exif fail. ret:%d", ret);
		return OMX_ErrorInsufficientResources;
	}
	info->valid = 1;
	info->exposure_time = exif.exposure_time;
	info->shutter_speed = exif.shutter_speed;
	info->aperture = exif.aperture;
	info->brightness = exif.brightness;
	info->exposure_bias = exif.exposure_bias;
	info->iso_speed = exif.iso_speed;
	info->flash = exif.flash;
	info->illu_id = exif.illu_id;
	info->back_score = exif.back_score;
	if (sizeof(info->res) == sizeof(exif.res))
		memcpy(info->res, exif.res, sizeof(exif.res));

	return OMX_ErrorNone;
}

void *oscl_open_vrec(const char *name)
{
	vrec_hdl_t *vrec = NULL;

	oscl_param_check((NULL != name), NULL, NULL);
	vrec = oscl_zalloc(sizeof(vrec_hdl_t));
	oscl_param_check((NULL != vrec), NULL, NULL);

	vrec->v4l2_fd = v4l2_open_dev(name, O_RDWR);
	if (vrec->v4l2_fd < 0)
		goto EXIT;

	OSCL_LOGD("vrec->v4l2_fd:%d", vrec->v4l2_fd);
	strncpy(vrec->name, name, sizeof(vrec->name) - 1);

	return vrec;

EXIT:
	oscl_free(vrec);

	return NULL;
}

void oscl_close_vrec(void *hdl)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;

	if (vrec == NULL)
		return;
	v4l2_exit_dev(vrec->v4l2_fd);
	oscl_free(vrec);
}

OMX_ERRORTYPE oscl_open_isp(void *hdl, const char *name)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;

	oscl_param_check((NULL != vrec), OMX_ErrorInsufficientResources, NULL);
	oscl_param_check((NULL != name), OMX_ErrorInsufficientResources, NULL);

	vrec->isp_fd = open(name, 0, 0);
	oscl_param_check((vrec->isp_fd > 0), OMX_ErrorInsufficientResources, NULL);

	return OMX_ErrorNone;
}

void oscl_close_isp(void *hdl)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;

	if (NULL == vrec)
		return;
	if (vrec->isp_fd > 0)
		close(vrec->isp_fd);
	vrec->isp_fd = -1;
}

OMX_S32 oscl_vrec_wait_frame(void *hdl, OMX_S32 timeout)
{
#if 0
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_S32 r;
	OMX_S32 poll_state;
	struct pollfd poll_events;

	/* v4l2 poll one capture frame if complete */
	poll_events.fd = vrec->v4l2_fd;
	poll_events.events = POLLIN | POLLERR;
	poll_events.revents = 0;

	while (1) {
		poll_state = poll((struct pollfd *)&poll_events, 1, timeout);
		if (poll_state <= 0) {
			OSCL_LOGD("poll return error");
			break;
		}

		if (poll_events.revents & POLLERR) {
			OSCL_LOGD("poll return POLLERR event");
			break;
		}

		if (poll_events.revents & POLLIN)
			break;

		OSCL_LOGD("wait frame, vrec->v4l2_fd:%d", vrec->v4l2_fd);
	}
#endif
	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_get_info(void *hdl, vrec_info_t *info)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	frame_mode_t frm_cfg;
	OMX_S32 ret = 0;
	OMX_S32 i = 0;

	/* vic max channle_num is 4, current is 1 defaultly. */
	vrec->info.channel_num = 1;
	info->channel_num = 1;
	memset(&frm_cfg, 0, sizeof(frm_cfg));
	ret = v4l2_enum_frame(vrec->v4l2_fd, &frm_cfg);
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	vrec->info.fmt_num = frm_cfg.num;
	vrec->info.format = oscl_zalloc(sizeof(vrec_format_t) * frm_cfg.num);
	oscl_param_check(vrec->info.format, OMX_ErrorInsufficientResources, NULL);
	for (i = 0; i < frm_cfg.num; i++) {
		vrec->info.format[i].index = i;
		vrec->info.format[i].format = OMX_COLOR_FormatUnused;
		vrec->info.format[i].frame_size.nTop = 0;
		vrec->info.format[i].frame_size.nLeft = 0;
		vrec->info.format[i].frame_size.nWidth = frm_cfg.format[i].width;
		vrec->info.format[i].frame_size.nHeight = frm_cfg.format[i].height;
		vrec->info.format[i].frame_rate = frm_cfg.format[i].frame_rate;
		OSCL_LOGI("Sensor info %d>>w:%d h:%d rate:%d",
			i, frm_cfg.format[i].width, frm_cfg.format[i].height,
			frm_cfg.format[i].frame_rate);
	}
	memcpy(info, &vrec->info, sizeof(vrec_info_t));

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_set_format(void *hdl, vrec_format_t *format)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_S32 ret = 0;
	OMX_S32 frame_rate = 0;

	oscl_param_check(vrec, OMX_ErrorBadParameter, NULL);
	oscl_param_check(format, OMX_ErrorBadParameter, NULL);

	struct v4l2_format fmt = {
		.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
	};

	fmt.fmt.pix_mp.pixelformat = omxformat_to_sensorformat(format->format);
	fmt.fmt.pix_mp.width = format->frame_size.nWidth;
	fmt.fmt.pix_mp.height = format->frame_size.nHeight;
	vrec->plans = sensorfomat_plans(fmt.fmt.pix_mp.pixelformat);
	OSCL_LOGI("vrec->plans:%d, %#x", vrec->plans, fmt.fmt.pix_mp.pixelformat);
	oscl_param_check((vrec->plans > 0), OMX_ErrorInsufficientResources, NULL);

	ret = v4l2_set_format(vrec->v4l2_fd, &fmt);
	oscl_param_check_exit((ret == 0), OMX_ErrorInsufficientResources, NULL);

	frame_rate = v4l2_get_cur_framerate(vrec->v4l2_fd);
	if ((format->frame_rate > 0) && (format->frame_rate != frame_rate)) {
		ret = v4l2_set_framerate(vrec->v4l2_fd, format->frame_rate);
		oscl_param_check((vrec->plans > 0), OMX_ErrorInsufficientResources, NULL);
	}

	return OMX_ErrorNone;
EXIT:

	return ret;
}

OMX_ERRORTYPE oscl_vrec_get_cur_format(void *hdl, vrec_format_t *format)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_S32 ret = 0;

	struct v4l2_format fmt = {
		.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
	};

	oscl_param_check(vrec, OMX_ErrorBadParameter, NULL);
	oscl_param_check(format, OMX_ErrorBadParameter, NULL);

	ret = v4l2_get_format(vrec->v4l2_fd, &fmt);
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	format->format = sensorformat_to_omxformat(fmt.fmt.pix_mp.pixelformat);
	format->frame_size.nWidth = fmt.fmt.pix_mp.width;
	format->frame_size.nHeight = fmt.fmt.pix_mp.height;
	format->frame_rate = v4l2_get_cur_framerate(vrec->v4l2_fd);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_set_buffer(void *hdl, vrec_frame_queue_t *frame_q)
{
#ifndef USE_DMA_BUFFER
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_S32 v4l2_fd = vrec->v4l2_fd;
	OMX_S32 j = 0, i = 0;
	OMX_S32 ret = OMX_ErrorNone;
	vrec_frame_addr_t *frame_addr = NULL;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_qbuf[VIDEO_MAX_PLANES];

	oscl_param_check(frame_q, OMX_ErrorBadParameter, NULL);
	oscl_param_check((vrec->plans > 0), OMX_ErrorBadParameter, NULL);
	oscl_param_check((vrec->plans <= VIDEO_MAX_PLANES), OMX_ErrorBadParameter, NULL);

	struct v4l2_requestbuffers reqbuf = {
		.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
		.memory = V4L2_MEMORY_USERPTR,
		.count = frame_q->frame_num,
	};

	ret = v4l2_rqbufs_bufs(vrec->v4l2_fd, &reqbuf);
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	OSCL_LOGI("reqbuf.count:%d, frame_num:%d", reqbuf.count, frame_q->frame_num);

	for (i = 0; i < frame_q->frame_num; i++) {
		memset(&buf, 0, sizeof(buf));
		memset(planes_qbuf, 0 , sizeof(planes_qbuf));

		frame_addr = &frame_q->frame_addr[i];
		buf.m.planes = planes_qbuf;
		buf.index = frame_addr->frame_index;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		buf.memory = V4L2_MEMORY_USERPTR;
		buf.length = vrec->plans;

		/* multiplane dmabuf's special handle */
		for (j = 0; j < vrec->plans; j++) {
			buf.m.planes[j].m.userptr = (long)frame_addr->addr[j];
			buf.m.planes[j].length = frame_addr->size[j];
			buf.m.planes[j].bytesused = frame_addr->size[j];
			OSCL_LOGE("%d>> addr:%p frame_index:%ld, size:%d\n",
						j,
						frame_addr->addr[j],
						frame_addr->frame_index,
						frame_addr->size[j]);
		}

		/**
		 * According to different buffer format, has different handle,
		 * for example NV12.
		 */
		ret = v4l2_queue_buffer(v4l2_fd, &buf);
		oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);
	}
#else
	vrec_frame_addr_t *frame_addr = NULL;
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_U32 dmafd = 0;
	OMX_S32 v4l2_fd = 0;
	OMX_S32 j = 0, i = 0;
	OMX_S32 ret = 0;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_qbuf[VIDEO_MAX_PLANES];

	OSCL_LOGE("vrec:%p", vrec);
	oscl_param_check(vrec, OMX_ErrorBadParameter, NULL);
	v4l2_fd = vrec->v4l2_fd;

	struct v4l2_requestbuffers reqbuf = {
		.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
		.memory = V4L2_MEMORY_DMABUF,
		.count = frame_q->frame_num,
	};

	ret = v4l2_rqbufs_bufs(vrec->v4l2_fd, &reqbuf);
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	for (i = 0; i < frame_q->frame_num; i++) {

		frame_addr = &frame_q->frame_addr[i];

		memset(&buf, 0, sizeof(buf));
		memset(planes_qbuf, 0 , sizeof(planes_qbuf));

		buf.m.planes = planes_qbuf;
		buf.index = frame_addr->frame_index;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		buf.memory = V4L2_MEMORY_DMABUF;
		buf.length = vrec->plans;

		dmafd = oscl_vir_to_fd(frame_addr->addr[0]);
		/* multiplane dmabuf's special handle */
		for (j = 0; j < vrec->plans; j++) {
			/*
			 * Because FMT_NUM_PLANES is 1 (NV12) currently,
			 * In case of mplane TODO
			 * also neet to handle multi dmabuf fd
			 */
			buf.m.planes[j].m.fd = dmafd;
			buf.m.planes[j].data_offset =
				frame_addr->addr[j] - frame_addr->addr[0];
			OSCL_LOGI("v4l2_fd:%d, dmafd:%d frame_index:%ld, offset:%d\n",
				v4l2_fd, dmafd,
				frame_addr->frame_index,
				buf.m.planes[j].data_offset);
		}

		/**
		 * According to different buffer format, has different handle,
		 * for example NV12.
		 */
		ret = v4l2_queue_buffer(v4l2_fd, &buf);
		if (ret != 0)
			return OMX_ErrorInsufficientResources;
	}
#endif
	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_clean_buffer(void *hdl)
{
	OMX_S32 ret = OMX_ErrorNone;
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;

	struct v4l2_requestbuffers reqbuf = {
		.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,
#ifndef USE_DMA_BUFFER
		.memory = V4L2_MEMORY_USERPTR,
#else
		.memory = V4L2_MEMORY_DMABUF,
#endif
		.count = 0,
	};

	oscl_param_check(vrec, OMX_ErrorBadParameter, NULL);
	ret = v4l2_rqbufs_bufs(vrec->v4l2_fd, &reqbuf);
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_set_gpts_cb(void *hdl, vrec_pts_cb_t *callback)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;

	oscl_param_check(vrec, OMX_ErrorBadParameter, NULL);
	memcpy(&vrec->pts_cb, callback, sizeof(vrec_pts_cb_t));

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_request_frame(void *hdl, vrec_reqframe_t *vrec_frame)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_S32 ret = OMX_ErrorNone;
	OMX_S32 v4l2_fd = vrec->v4l2_fd;
	OMX_S32 buf_index = 0;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_qbuf[VIDEO_MAX_PLANES];

	oscl_param_check((vrec->plans > 0), OMX_ErrorBadParameter, NULL);
	oscl_param_check(vrec_frame, OMX_ErrorBadParameter, NULL);
	oscl_param_check((vrec->plans <= VIDEO_MAX_PLANES), OMX_ErrorBadParameter, NULL);

	memset(&buf, 0, sizeof(buf));
	memset(planes_qbuf, 0, sizeof(planes_qbuf));

	/* dequeue buffer */
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
#ifndef USE_DMA_BUFFER
	buf.memory = V4L2_MEMORY_USERPTR;
#else
	buf.memory = V4L2_MEMORY_DMABUF;
#endif
	buf.m.planes = planes_qbuf;
	buf.length = vrec->plans;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	ret = v4l2_dequeue_buffer(v4l2_fd, &buf, &buf_index);
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	memset(&vrec_frame->isp_exif, 0, sizeof(vrec_frame->isp_exif));
	ret = isp_get_exif(hdl, &vrec_frame->isp_exif);
	if (ret != OMX_ErrorNone) {
		/* get isp information from video device. */
		/* TO DO */
	}

	vrec_frame->paddr.frame_index = buf_index;
	vrec_frame->pts = (OMX_S64)((buf.timestamp.tv_sec)*1000LL +
					buf.timestamp.tv_usec/1000LL);
	/*
	if (vrec->pts_cb.vrec_get_pts)
		vrec->pts_cb.vrec_get_pts(&vrec_frame->pts, vrec->pts_cb.para);
	*/

	OSCL_LOGD("deq idx:%d", buf_index);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_release_frame(void *hdl, vrec_reqframe_t *vrec_frame)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_S32 ret = OMX_ErrorNone;
	OMX_S32 v4l2_fd = vrec->v4l2_fd;
	OMX_S32 j = 0;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_qbuf[VIDEO_MAX_PLANES];

	oscl_param_check((vrec->plans > 0), OMX_ErrorBadParameter, NULL);
	oscl_param_check((vrec->plans <= VIDEO_MAX_PLANES), OMX_ErrorBadParameter, NULL);
	oscl_param_check(vrec_frame, OMX_ErrorBadParameter, NULL);
retry:
	memset(&buf, 0, sizeof(buf));
	memset(planes_qbuf, 0, sizeof(planes_qbuf));
#ifndef USE_DMA_BUFFER
	buf.m.planes = planes_qbuf;
	buf.index = vrec_frame->paddr.frame_index;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_USERPTR;
	buf.length = vrec->plans;

	/* multiplane dmabuf's special handle */
	for (j = 0; j < vrec->plans; j++) {
		buf.m.planes[j].m.userptr = (long)vrec_frame->paddr.addr[j];
		buf.m.planes[j].length = vrec_frame->paddr.size[j];
		buf.m.planes[j].bytesused = vrec_frame->paddr.size[j];
	}
	OSCL_LOGD("enq idx:%d buffer:0x%lx",
			vrec_frame->paddr.frame_index,
			buf.m.planes[0].m.userptr);
#else
	buf.m.planes = planes_qbuf;
	buf.index = vrec_frame->paddr.frame_index;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = V4L2_MEMORY_DMABUF;
	buf.length = vrec->plans;

	/* multiplane dmabuf's special handle */
	for (j = 0; j < vrec->plans; j++) {
		/*
		 * Because FMT_NUM_PLANES is 1 (NV12) currently,
		 * In case of mplane TODO
		 * also neet to handle multi dmabuf fd
		 */
		buf.m.planes[j].m.fd = oscl_vir_to_fd(vrec_frame->paddr.addr[0]);
		buf.m.planes[j].data_offset =
				vrec_frame->paddr.addr[j] - vrec_frame->paddr.addr[0];
	}
#endif

	/**
	 * According to different buffer format, has different handle,
	 * for example NV12.
	 */
	ret = v4l2_queue_buffer(v4l2_fd, &buf);

	if (ret != 0) {
		for (j = 0; j < vrec->plans; j++) {
			OSCL_LOGE("%d planes[%p %d] >> %lx %ud %d",
				buf.index, buf.m.planes, j,
				buf.m.planes[j].m.userptr,
				buf.m.planes[j].length,
				buf.m.planes[j].bytesused);
			OSCL_LOGE("%d vrec_frame planes[%p %d] >> %p %d",
				buf.index, buf.m.planes, j,
				vrec_frame->paddr.addr[j],
				vrec_frame->paddr.size[j]);
		}
		OSCL_LOGE("release frame fail.");
		goto retry;
	}
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_capture_on(void *hdl, OMX_S32 ch)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_S32 ret = 0;
	OMX_S32 v4l2_fd = vrec->v4l2_fd;

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

	ret = v4l2_stream_on(v4l2_fd, &type);
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_vrec_capture_off(void *hdl, OMX_S32 ch)
{
	vrec_hdl_t *vrec = (vrec_hdl_t *)hdl;
	OMX_S32 ret = 0;
	OMX_S32 v4l2_fd = vrec->v4l2_fd;

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

	ret = v4l2_stream_off(v4l2_fd, &type);
	oscl_param_check((ret == 0), OMX_ErrorInsufficientResources, NULL);

	return OMX_ErrorNone;
}

