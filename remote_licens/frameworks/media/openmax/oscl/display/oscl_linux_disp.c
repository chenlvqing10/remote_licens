/*
 * oscl_eos_disp.c - display api used by lombo media framework.
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

#define DBG_LEVEL		DBG_INFO
#define LOG_TAG			"oscl_linux_disp"

#include <sys/prctl.h>
#include <dlfcn.h>
#include "oscl_linux_disp.h"
#include "rot_sw.h"

/* #define _ROT_FPS_DISP	1 */
/* #define _SE_FPS_DISP		1 */

#define PROPERTY_MAX_STR_LEN    (16)
#define COLOR_ENCODING_NAME "COLOR_ENCODING"
#define COLOR_RANGE_NAME "COLOR_RANGE"

static struct modeset_out *modeset_list;   /* drm modeset parameters */
static disp_hdl_t g_vdisp;
static OMX_S32 open_disp_cnt;
static disp_hdl_t *g_hdl[5];
static pthread_mutex_t disp_mutex = PTHREAD_MUTEX_INITIALIZER;

static disp_rotmode_map_index_t disp_rot_mode_map_table[] = {
	{LOMBO_DRM_TRANSFORM_COPY, VDISP_ROTATE_NONE},
	{LOMBO_DRM_TRANSFORM_ROT_90, VDISP_ROTATE_90},
	{LOMBO_DRM_TRANSFORM_ROT_180, VDISP_ROTATE_180},
	{LOMBO_DRM_TRANSFORM_ROT_270, VDISP_ROTATE_270},
	{LOMBO_DRM_TRANSFORM_FLIP_H, VDISP_FLIP_H},
	{LOMBO_DRM_TRANSFORM_FLIP_H_ROT_90, VDISP_FLIP_H_ROT_90},
	{LOMBO_DRM_TRANSFORM_FLIP_V, VDISP_FLIP_V},
	{LOMBO_DRM_TRANSFORM_FLIP_V_ROT_90, VDISP_FLIP_V_ROT_90},

	{0, VDISP_SW_ROTATE_NONE},
	{1, VDISP_SW_ROTATE_90},
	{2, VDISP_SW_ROTATE_180},
	{3, VDISP_SW_ROTATE_270},
	{4, VDISP_SW_FLIP_H},
	{5, VDISP_SW_FLIP_H_ROT_90},
	{6, VDISP_SW_FLIP_V},
	{7, VDISP_SW_FLIP_V_ROT_90},
};

static disp_pixformat_map_index_t disp_pix_format_map_table[] = {
	{DRM_FORMAT_NV12, OMX_COLOR_FormatYUV420SemiPlanar},
	{DRM_FORMAT_RGB565, OMX_COLOR_Format16bitRGB565},
};

#define _SW_ROT_LIB_NAME "libsw_rot.so"

static int (*_sw_rot)(disp_rot_cfg_t *cfgs);
static void *_sw_lib_handle;
void _sw_rot_init(void)
{
	pthread_mutex_lock(&disp_mutex);
	if (_sw_lib_handle != NULL)
		goto EXIT;
	_sw_lib_handle = dlopen(_SW_ROT_LIB_NAME, RTLD_LAZY);
	if (_sw_lib_handle == NULL) {
		OSCL_LOGE("Failed to open lib:%s: %s", _SW_ROT_LIB_NAME, dlerror());
		goto EXIT;
	}
	_sw_rot = dlsym(_sw_lib_handle, "rot_sw_process");
	if (_sw_rot == NULL) {
		dlclose(_sw_lib_handle);
		OSCL_LOGE("%s dlsym fail:%s", _SW_ROT_LIB_NAME, dlerror());
		_sw_lib_handle = NULL;
		_sw_rot = NULL;
	}
EXIT:
	pthread_mutex_unlock(&disp_mutex);
}

static OMX_S32 omxrot_to_disprot_mode(vdisp_rotate_mode_e mode)
{
	OMX_S32 i = 0;
	disp_rotmode_map_index_t *rot_map = NULL;

	for (i = 0; i < ARRAY_SIZE(disp_rot_mode_map_table); i++) {
		rot_map = &disp_rot_mode_map_table[i];
		if (rot_map->omx_rot_mode == mode)
			return rot_map->disp_rot_mode;
	}

	return -1;
}

static OMX_S32 omxformat_to_dispformat(OMX_COLOR_FORMATTYPE format)
{
	OMX_S32 i = 0;
	disp_pixformat_map_index_t *pix_map = NULL;

	for (i = 0; i < ARRAY_SIZE(disp_pix_format_map_table); i++) {
		pix_map = &disp_pix_format_map_table[i];
		if (pix_map->eColorFormat == format)
			return pix_map->disp_format;
	}

	return -1;
}

static OMX_S32 set_disp_property(OMX_S32 fd, char (*para)[PROPERTY_MAX_STR_LEN])
{
	OMX_U32 obj_id, obj_type, prop_id;
	OMX_U64 value;

	obj_id = atoi(para[0]);

	if (!strcmp(para[1], "connector")) {
		obj_type = DRM_MODE_OBJECT_CONNECTOR;
	} else if (!strcmp(para[1], "crtc")) {
		obj_type = DRM_MODE_OBJECT_CRTC;
	} else if (!strcmp(para[1], "plane")) {
		obj_type = DRM_MODE_OBJECT_PLANE;
	} else {
		fprintf(stderr, "Invalid object type.\n");
		return 1;
	}

	prop_id = atoi(para[2]);
	value = atoll(para[3]);

	return drmModeObjectSetProperty(fd,
			obj_id, obj_type, prop_id, value);
}

static OMX_S32 find_fb_id(disp_hdl_t *vdisp, void *virt_addr)
{
	OMX_S32 i = 0;
	disp_fb_map_index_t *fb_map = NULL;

	for (i = 0; i < ARRAY_SIZE(vdisp->fb_map_table); i++) {
		fb_map = &vdisp->fb_map_table[i];
		if (fb_map->virtual_addr == virt_addr)
			return fb_map->fb_id;
	}

	return -1;
}

static OMX_S32 store_image_fb_id(disp_hdl_t *vdisp, void *virt_addr,
				OMX_U32 fb_id, OMX_U32 bo_handle)
{
	OMX_S32 i = 0;
	disp_fb_map_index_t *fb_map = NULL;

	oscl_param_check(vdisp, OMX_ErrorBadParameter, NULL);
	for (i = 0; i < ARRAY_SIZE(vdisp->fb_map_table); i++) {
		fb_map = &vdisp->fb_map_table[i];

		if (fb_map->virtual_addr == NULL) {
			fb_map->virtual_addr = virt_addr;
			fb_map->fb_id = fb_id;
			fb_map->bo_handle = bo_handle;

			return OMX_ErrorNone;
		}
	}

	return OMX_ErrorInsufficientResources;
}

static void destroy_image_fb_id(void *hdl)
{
	OMX_S32 i = 0;
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	disp_fb_map_index_t *fb_map = NULL;
	struct drm_gem_close arg;

	for (i = 0; i < ARRAY_SIZE(vdisp->fb_map_table); i++) {
		fb_map = &vdisp->fb_map_table[i];
		if (fb_map->fb_id != 0) {
			drmModeRmFB(vdisp->drm_fd, fb_map->fb_id);
			fb_map->fb_id = 0;
			memset(&arg, 0, sizeof(arg));
			arg.handle = fb_map->bo_handle;
			drmIoctl(vdisp->drm_fd, DRM_IOCTL_GEM_CLOSE, &arg);
		}
	}
}

static OMX_S32 store_vdisp(void *hdl)
{
	OMX_S32 i = 0;

	for (i = 0; i < ARRAY_SIZE(g_hdl); i++) {
		if (g_hdl[i] == NULL) {
			g_hdl[i] = hdl;
			return OMX_ErrorNone;
		}
	}

	return OMX_ErrorInsufficientResources;
}

static void destroy_vdisp(void *hdl)
{
	OMX_S32 i = 0;

	for (i = 0; i < ARRAY_SIZE(g_hdl); i++) {
		if (g_hdl[i] == hdl) {
			g_hdl[i] = NULL;
			break;
		}
	}
}

static OMX_S32 get_image_fb_id(disp_hdl_t *vdisp,
				void *image_virt_addr,
				OMX_S32 image_format,
				OMX_U32 size[3],
				OMX_S32 src_w,
				OMX_S32 aligned_width,
				OMX_S32 aligned_height)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_U32 dmabuf_fd = 0;
	OMX_S32 w = aligned_width;
	OMX_S32 h = aligned_height;
	OMX_S32 format = image_format;
	OMX_S32 bpp;
	OMX_U32 bo_handle = 0;
	uint32_t fb_handle = 0;
	/*
	 * OMX_S32 virtual_height = 0;
	 * size_t vir_size;
	 */

	size_t pitch;
	OMX_U32 offsets[4] = { 0, 0, 0, 0 };
	OMX_U32 pitches[4] = { 0, 0, 0, 0 };
	OMX_U32 bo_handles[4] = { bo_handle };

	oscl_param_check(vdisp, OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&disp_mutex);
	/* find fb_id in mapping table */
	fb_handle = find_fb_id(vdisp, image_virt_addr);
	if (fb_handle != -1) {
		pthread_mutex_unlock(&disp_mutex);
		return fb_handle;
	}

	OSCL_LOGD("format = %.4s", (char *)&format);
	OSCL_LOGD("aligned_width = %d", aligned_width);
	OSCL_LOGD("aligned_height = %d", aligned_height);

	switch (format) {
	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV21:
	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV61:
	case DRM_FORMAT_YUV420:
	case DRM_FORMAT_YVU420:
		bpp = 8;  /* 1byte per pixel */
		break;

	case DRM_FORMAT_ARGB4444:
	case DRM_FORMAT_XRGB4444:
	case DRM_FORMAT_ABGR4444:
	case DRM_FORMAT_XBGR4444:
	case DRM_FORMAT_RGBA4444:
	case DRM_FORMAT_RGBX4444:
	case DRM_FORMAT_BGRA4444:
	case DRM_FORMAT_BGRX4444:
	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_XRGB1555:
	case DRM_FORMAT_ABGR1555:
	case DRM_FORMAT_XBGR1555:
	case DRM_FORMAT_RGBA5551:
	case DRM_FORMAT_RGBX5551:
	case DRM_FORMAT_BGRA5551:
	case DRM_FORMAT_BGRX5551:
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_BGR565:
	case DRM_FORMAT_UYVY:
	case DRM_FORMAT_VYUY:
	case DRM_FORMAT_YUYV:
	case DRM_FORMAT_YVYU:
		bpp = 16;
		break;

	case DRM_FORMAT_BGR888:
	case DRM_FORMAT_RGB888:
		bpp = 24;
		break;

	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_RGBA8888:
	case DRM_FORMAT_RGBX8888:
	case DRM_FORMAT_BGRA8888:
	case DRM_FORMAT_BGRX8888:
	case DRM_FORMAT_ARGB2101010:
	case DRM_FORMAT_XRGB2101010:
	case DRM_FORMAT_ABGR2101010:
	case DRM_FORMAT_XBGR2101010:
	case DRM_FORMAT_RGBA1010102:
	case DRM_FORMAT_RGBX1010102:
	case DRM_FORMAT_BGRA1010102:
	case DRM_FORMAT_BGRX1010102:
		bpp = 32;
		break;

	default:
		OSCL_LOGE("unsupported format 0x%08x",  format);
		pthread_mutex_unlock(&disp_mutex);
		return OMX_ErrorInsufficientResources;
	}
#if 0
	switch (format) {
	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV21:
	case DRM_FORMAT_YUV420:
	case DRM_FORMAT_YVU420:
		virtual_height = h * 3 / 2;
		break;

	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV61:
		virtual_height = h * 2;
		break;

	default:
		virtual_height = h;
		break;
	}
#endif
	if (src_w)
		pitch = src_w * ((bpp + 7) / 8);
	else
		pitch = w * ((bpp + 7) / 8);
	/* vir_size = pitch * virtual_height; */

	dmabuf_fd = oscl_vir_to_fd(image_virt_addr);

	ret = drmPrimeFDToHandle(vdisp->drm_fd, dmabuf_fd, &bo_handle);
	if (ret != 0) {
		OSCL_LOGE("drmPrimeFDToHandle failed %d: %s\n", ret, ERRSTR);
		pthread_mutex_unlock(&disp_mutex);
		return OMX_ErrorInsufficientResources;
	}
	OSCL_LOGD("image_virt_addr = %p, dmabuf_fd:%d, bo_handle:%d",
				image_virt_addr, dmabuf_fd, bo_handle);
	/* just testing a limited # of formats to test single
	 * and multi-planar path.. would be nice to add more..
	 */
	if (src_w) {
		offsets[0] = size[0];
		offsets[1] = size[1];
		offsets[2] = 0;
	} else {
		offsets[0] = 0;
		offsets[1] = size[0];
		offsets[2] = size[0] + size[1];
	}
	switch (format) {
	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV21:
	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV61:
		bo_handles[0]	= bo_handle;
		pitches[0]	= pitch;
		pitches[1]	= pitches[0];
		/* be careful, cam src height(not aligned height) */
		bo_handles[1]	= bo_handle;
		offsets[2] = 0;
		break;

	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_XRGB1555:
	case DRM_FORMAT_ABGR1555:
	case DRM_FORMAT_XBGR1555:
	case DRM_FORMAT_RGBA5551:
	case DRM_FORMAT_RGBX5551:
	case DRM_FORMAT_BGRA5551:
	case DRM_FORMAT_BGRX5551:
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_BGR565:
	case DRM_FORMAT_BGR888:
		bo_handles[0]	= bo_handle;
		pitches[0]	= pitch;
		offsets[1] = 0;
		offsets[2] = 0;
		break;
	}
	ret = drmModeAddFB2(vdisp->drm_fd, w, h, format, bo_handles,
		pitches, offsets, &fb_handle, 0);
	if (ret != 0) {
		OSCL_LOGE("add fb fail. bo_handle:%#x(%p) dmabuf_fd:%d, %s",
			bo_handle, image_virt_addr, dmabuf_fd, ERRSTR);
		pthread_mutex_unlock(&disp_mutex);
		return OMX_ErrorInsufficientResources;
	}

	/* store fb_id in mapping table */
	ret = store_image_fb_id(vdisp, image_virt_addr, fb_handle, bo_handle);
	pthread_mutex_unlock(&disp_mutex);

	return (OMX_ErrorNone == ret ? fb_handle : OMX_ErrorInsufficientResources);
}

static void find_crtc(OMX_S32 fd, crtc_info_t *crtc_info)
{
	drmModeRes *res = drmModeGetResources(fd);
	ASSERT(res);

	for (int i = 0; i < res->count_crtcs; ++i) {
		OMX_U32 crtc_id = res->crtcs[i];

		drmModeCrtc *crtc = drmModeGetCrtc(fd, crtc_id);
		ASSERT(crtc);

		if (!crtc->mode_valid) {
			drmModeFreeCrtc(crtc);
			continue;
		}

		crtc_info->crtc_id = crtc->crtc_id;
		crtc_info->crtc_width = crtc->width;
		crtc_info->crtc_height = crtc->height;
		crtc_info->primary_buf_id = crtc->buffer_id;

		drmModeFreeCrtc(crtc);
		drmModeFreeResources(res);
		return;
	}
}

static void find_mode(OMX_S32 fd, struct modeset_out **out_list)
{
	/* Prepare all connectors and CRTCs */
	modeset_prepare(fd, out_list);
}

static OMX_S32 find_property_id(OMX_S32 fd,
				OMX_U32 obj_id,
				OMX_U32 obj_type,
				char *name)
{
	OMX_S32 i = 0;
	OMX_S32 id = -1;
	drmModeObjectPropertiesPtr properties = NULL;
	drmModePropertyPtr property = NULL;

	properties = drmModeObjectGetProperties(fd, obj_id, obj_type);
	oscl_param_check((NULL != properties), -1, NULL);
	for (i = 0; i < properties->count_props; i++) {
		property = drmModeGetProperty(fd, properties->props[i]);
		if (property) {
			if (strcmp(property->name, name) == 0) {
				id = property->prop_id;
				drmModeFreeProperty(property);
				property = NULL;
				break;
			}
			drmModeFreeProperty(property);
			property = NULL;
		}
	}
	drmModeFreeObjectProperties(properties);

	return id;
}

static OMX_ERRORTYPE wait_flush_frame(disp_hdl_t *vdisp, void *data)
{
	OMX_S32 ret = OMX_ErrorNone;
	int timeout = 1000; /* 1s */
	drmVBlank vbl;

	memset(&vbl, 0, sizeof(vbl));
	/* Get current count first */
	vbl.request.type = DRM_VBLANK_RELATIVE | DRM_VBLANK_EVENT;
	vbl.request.sequence = 1;
	vbl.request.signal = (unsigned long)data;
	ret = drmWaitVBlank(vdisp->drm_fd, &vbl);
	if (ret != 0) {
		OSCL_LOGE("drmWaitVBlank (relative) failed ret: %i\n", ret);
		return OMX_ErrorInsufficientResources;
	}
	ret = oscl_sem_timedwait_ms(vdisp->event_sem, timeout);
	if (ret) {
		OSCL_LOGE("Wait vblank timeout.");
		ret = OMX_ErrorInsufficientResources;
	}

	return ret;
}

static void vblank_handler(int fd, unsigned int frame, unsigned int sec,
			   unsigned int usec, void *data)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)data;

	if (vdisp) {
		OSCL_LOGD("post idx %d sem, seq:%d", vdisp->video_plane.plane_idx, frame);
		sem_post(vdisp->event_sem);
	}
}

void *drm_event_thread(void *param)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)param;
	drmEventContext evctx;
	fd_set fds;
	OMX_S32 ret;
	OMX_S32 fd = vdisp->drm_fd;

	prctl(PR_SET_NAME, "drm_event_thread");
	while (1) {
		struct timeval timeout = { .tv_sec = 0, .tv_usec = 80000 };

		if (vdisp->stop_flag)
			break;
		/* Set up our event handler */
		memset(&evctx, 0, sizeof(evctx));
		evctx.version = DRM_EVENT_CONTEXT_VERSION;
		evctx.vblank_handler = vblank_handler;
		evctx.page_flip_handler = NULL;

		/* Poll for events */
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		ret = select(fd + 1, &fds, NULL, NULL, &timeout);
		if (ret == -1) {
			OSCL_LOGE("select error(ret=%d, %s)", ret, strerror(errno));
			continue;
		} else if (ret == 0) {
			OSCL_LOGD("select timeout");
			continue;
		}

		ret = drmHandleEvent(fd, &evctx);
		if (ret != 0) {
			OSCL_LOGE("drmHandleEvent failed: %i\n", ret);
			continue;
		}

	}

	return NULL;
}

void *oscl_open_disp_engine(void)
{
	disp_hdl_t *vdisp = NULL;
	const char *card = "/dev/dri/card0";

	pthread_mutex_lock(&disp_mutex);

	vdisp = oscl_zalloc(sizeof(disp_hdl_t));
	oscl_param_check((NULL != vdisp), NULL, NULL);

	OSCL_LOGI("open display engine, hdl:%p cnt:%d", vdisp, open_disp_cnt);

	vdisp->event_sem = (sem_t *)oscl_zalloc(sizeof(sem_t));
	if (vdisp->event_sem == NULL) {
		oscl_free(vdisp);
		pthread_mutex_unlock(&disp_mutex);
		return NULL;
	}
	sem_init(vdisp->event_sem, 0, 0);
	if (open_disp_cnt > 0) {
		OSCL_LOGD("vdisp:%p, event_sem:%p", vdisp, vdisp->event_sem);
		vdisp->drm_fd = g_vdisp.drm_fd;
		open_disp_cnt++;
		pthread_mutex_unlock(&disp_mutex);
		return vdisp;
	}

	vdisp->drm_fd = drm_open_dev_dumb(card);
	if (vdisp->drm_fd >= 0) {
		find_crtc(vdisp->drm_fd, &vdisp->crtc_info);
		find_mode(vdisp->drm_fd, &modeset_list);
		memset(g_hdl, 0, sizeof(g_hdl));
		memset(&g_vdisp, 0, sizeof(g_vdisp));
		g_vdisp.drm_fd = vdisp->drm_fd;
		g_vdisp.event_thread_id = -1;
		open_disp_cnt++;
	} else {
		oscl_free(vdisp);
		vdisp = NULL;
		memset(&g_vdisp, 0, sizeof(disp_hdl_t));
		open_disp_cnt = 0;
		g_vdisp.event_thread_id = -1;
	}
	pthread_mutex_unlock(&disp_mutex);

	return vdisp;
}

void oscl_close_disp_engine(void *hdl)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;

	if (vdisp == NULL)
		return;

	pthread_mutex_lock(&disp_mutex);
	OSCL_LOGI("close display engine, hdl:%p cnt:%d", hdl, open_disp_cnt);
	open_disp_cnt--;

	if (vdisp->event_sem) {
		sem_destroy(vdisp->event_sem);
		oscl_free(vdisp->event_sem);
		vdisp->event_sem = NULL;
	}
	destroy_image_fb_id(vdisp);
	memset(vdisp->fb_map_table, 0, sizeof(vdisp->fb_map_table));
	if (open_disp_cnt > 0) {
		/* rm fb_id mapping table */
		oscl_free(vdisp);
	} else {
		if (g_vdisp.event_thread_id == 0) {
			g_vdisp.stop_flag = 1;
			pthread_join(g_vdisp.event_thread, NULL);
			g_vdisp.event_thread_id = -1;
			g_vdisp.event_thread = 0;
		}
		close(g_vdisp.drm_fd);
		g_vdisp.drm_fd = -1;
		memset(&g_vdisp, 0, sizeof(disp_hdl_t));
		oscl_free(vdisp);
		g_vdisp.event_thread_id = -1;
		open_disp_cnt = 0;
		OSCL_LOGI("display engine close complete.");
	}
	pthread_mutex_unlock(&disp_mutex);
}

OMX_S32 oscl_open_rot_submodule(void *hdl)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	struct drm_lombo_rot_timeout timeout;
	int ret = 0;

	oscl_param_check((NULL != vdisp), OMX_ErrorInsufficientResources, NULL);
	pthread_mutex_lock(&disp_mutex);
	OSCL_LOGI("rot open:%d", vdisp->drm_fd);
	if (lombo_rot_open(vdisp->drm_fd, &vdisp->rot_handle)) {
		OSCL_LOGE("failed to open rot device");
		pthread_mutex_unlock(&disp_mutex);
		return OMX_ErrorInsufficientResources;
	}
	OSCL_LOGI("rot open complete:%p rot_handle:%d", vdisp, vdisp->rot_handle);

	timeout.handle = vdisp->rot_handle;
	timeout.time_ns = 50000000ULL; /* 50ms */
	ret = lombo_rot_set_timeout(vdisp->drm_fd, &timeout);
	if (0 != ret) {
		OSCL_LOGE("rotate set_timeout failed. rot_handle:%d", vdisp->rot_handle);
		pthread_mutex_unlock(&disp_mutex);
		return OMX_ErrorInsufficientResources;
	}
	pthread_mutex_unlock(&disp_mutex);

	return OMX_ErrorNone;
}

void oscl_close_rot_submodule(void *hdl)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;

	pthread_mutex_lock(&disp_mutex);
	OSCL_LOGI("rot close:%p rot_handle:%d", vdisp, vdisp ? vdisp->rot_handle : -1);
	if (vdisp && vdisp->rot_handle != 0) {
		lombo_rot_close(vdisp->drm_fd, &vdisp->rot_handle);
		vdisp->rot_handle = 0;
	}
	OSCL_LOGI("rot close complete.");
	pthread_mutex_unlock(&disp_mutex);
}

OMX_S32 oscl_open_scale_submodule(void *hdl)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	struct drm_lombo_scale_timeout timeout;
	int ret = 0;

	oscl_param_check((NULL != vdisp), OMX_ErrorInsufficientResources, NULL);
	pthread_mutex_lock(&disp_mutex);
	OSCL_LOGI("scale open:%d", vdisp->drm_fd);
	if (lombo_scale_open(vdisp->drm_fd, &vdisp->scale_handle)) {
		OSCL_LOGE("failed to open rot device");
		pthread_mutex_unlock(&disp_mutex);
		return OMX_ErrorInsufficientResources;
	}
	OSCL_LOGI("scale open complete:%p scale_handle:%d", vdisp, vdisp->scale_handle);

	timeout.handle = vdisp->scale_handle;
	timeout.time_ns = 50000000ULL; /* 50ms */
	ret = lombo_scale_set_timeout(vdisp->drm_fd, &timeout);
	if (0 != ret) {
		OSCL_LOGE("scale set_timeout failed. rot_handle:%d", vdisp->scale_handle);
		pthread_mutex_unlock(&disp_mutex);
		return OMX_ErrorInsufficientResources;
	}
	pthread_mutex_unlock(&disp_mutex);

	return OMX_ErrorNone;
}

void oscl_close_scale_submodule(void *hdl)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;

	pthread_mutex_lock(&disp_mutex);
	OSCL_LOGI("scale close:%p scale_handle:%d",
			vdisp, vdisp ? vdisp->scale_handle : -1);
	if (vdisp && vdisp->scale_handle != 0) {
		lombo_scale_close(vdisp->drm_fd, &vdisp->scale_handle);
		vdisp->scale_handle = 0;
	}
	OSCL_LOGI("scale close complete.");
	pthread_mutex_unlock(&disp_mutex);
}

void *oscl_disp_request_layer(void *hdl)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	OMX_U32 plane_id;
	OMX_S32 plane_idx;
	OMX_S32 color_id;
	void *video_plane_addr;
	char para_colorspace[4][PROPERTY_MAX_STR_LEN];

	oscl_param_check((NULL != vdisp), NULL, NULL);

	pthread_mutex_lock(&disp_mutex);
	OSCL_LOGE("vdisp->drm_fd:%d", vdisp->drm_fd);
	plane_idx = drm_reserve_plane_index(vdisp->drm_fd);
	oscl_param_check(plane_idx >= 0, NULL, NULL);
	plane_id = drm_plane_index_to_id(plane_idx);
	ASSERT(plane_id > 0);

	memset(para_colorspace, 0, sizeof(para_colorspace));
	vdisp->video_plane.plane_idx = plane_idx;
	video_plane_addr = &vdisp->video_plane;
	vdisp->valid_plane = 1;
	OSCL_LOGI("request layer id:%d, idx:%d", plane_id, plane_idx);
	store_vdisp(vdisp);
	for_each_output(out, modeset_list)
		vdisp->video_plane.crtc_id = out->crtc_id;

	/* Bt601 full range defautly */
	color_id = find_property_id(vdisp->drm_fd, plane_id,
			DRM_MODE_OBJECT_PLANE, COLOR_RANGE_NAME);
	OSCL_LOGD("color_id:%d", color_id);
	sprintf(para_colorspace[0], "%d", plane_id);
	sprintf(para_colorspace[1], "%s", "plane");
	if (color_id >= 0) {
		sprintf(para_colorspace[2], "%d", color_id);
		/* DRM_COLOR_YCBCR_FULL_RANGE */
		sprintf(para_colorspace[3], "%d", 1);
		set_disp_property(vdisp->drm_fd, para_colorspace);
	}
	color_id = find_property_id(vdisp->drm_fd, plane_id,
			DRM_MODE_OBJECT_PLANE, COLOR_ENCODING_NAME);
	OSCL_LOGD("color_id:%d", color_id);
	if (color_id >= 0) {
		sprintf(para_colorspace[2], "%d", color_id);
		/* DRM_COLOR_YCBCR_BT601 */
		sprintf(para_colorspace[3], "%d", 0);
		set_disp_property(vdisp->drm_fd, para_colorspace);
	}
	pthread_mutex_unlock(&disp_mutex);

	return video_plane_addr;
}

OMX_ERRORTYPE oscl_disp_enable_layer(void *hdl, void **layer)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;

	oscl_param_check((NULL != vdisp), OMX_ErrorBadParameter, NULL);
	vdisp->disable_layer = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_disp_disable_layer(void *hdl, void **layer)
{
	OMX_S32 ret = 0;
	OMX_S32 fb_id = 0;
	OMX_U32 plane_id;
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	struct plane_data *pdata = *(struct plane_data **)layer;

	oscl_param_check((NULL != vdisp), OMX_ErrorBadParameter, NULL);
	oscl_param_check((NULL != pdata), OMX_ErrorBadParameter, NULL);

	plane_id = drm_plane_index_to_id(pdata->plane_idx);
	ret = drmModeSetPlane(vdisp->drm_fd,
				      plane_id,
				      pdata->crtc_id,
				      fb_id,
				      0,
				      /* output */
				      vdisp->win_rect.left,
				      vdisp->win_rect.top,
				      vdisp->win_rect.width,
				      vdisp->win_rect.height,
				      /* input */
				      vdisp->crop_rect.left << 16,
				      vdisp->crop_rect.top << 16,
				      vdisp->crop_rect.width << 16,
				      vdisp->crop_rect.height << 16);
	oscl_param_check((ret == 0), OMX_ErrorBadParameter, NULL);
	vdisp->disable_layer = OMX_TRUE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_disp_release_layer(void *hdl, void **layer)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	struct plane_data *pdata = *(struct plane_data **)layer;

	oscl_param_check((NULL != vdisp), OMX_ErrorBadParameter, NULL);
	oscl_param_check((NULL != pdata), OMX_ErrorBadParameter, NULL);
	OSCL_LOGD("release layer idx:%d", pdata->plane_idx);

	pthread_mutex_lock(&disp_mutex);
	oscl_disp_disable_layer(hdl, layer);
	vdisp->valid_plane = 0;
	drm_release_plane_index(pdata->plane_idx);
	destroy_image_fb_id(vdisp);
	destroy_vdisp(hdl);
	memset(vdisp->fb_map_table, 0, sizeof(vdisp->fb_map_table));
	pthread_mutex_unlock(&disp_mutex);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_disp_get_screen_size(void *hdl, disp_size_t *size)
{
	oscl_param_check((NULL != size), OMX_ErrorBadParameter, NULL);

	/* Only support 1 connnector */
	pthread_mutex_lock(&disp_mutex);
	for_each_output(out, modeset_list) {
		size->width = out->mode.hdisplay;
		size->height = out->mode.vdisplay;

		OSCL_LOGD("w:%d, h:%d", size->width, size->height);
	}
	pthread_mutex_unlock(&disp_mutex);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_disp_set_layer_top(void *hdl, void *layer)
{
	struct plane_data *pdata = (struct plane_data *)layer;

	oscl_param_check((NULL != pdata), OMX_ErrorBadParameter, NULL);
	drm_plane_top(pdata->plane_idx);
	OSCL_LOGD("set layer top. index:%d id:%d",
		pdata->plane_idx, drm_plane_index_to_id(pdata->plane_idx));
	return OMX_ErrorNone;
}

OMX_ERRORTYPE oscl_disp_set_layer_bottom(void *hdl, void *layer)
{
	struct plane_data *pdata = (struct plane_data *)layer;

	oscl_param_check((NULL != pdata), OMX_ErrorBadParameter, NULL);
	drm_plane_bottom(pdata->plane_idx);
	OSCL_LOGD("set layer bottom. index:%d id:%d",
		pdata->plane_idx, drm_plane_index_to_id(pdata->plane_idx));
	return OMX_ErrorNone;
}

#if 0
static void save_video_frame(char *prefix, char *y, int y_size, char *uv, int uv_size)
{
	OMX_S32 fd = 0;
	char path[64] = {0};
	static OMX_S32 frm_count;

	if (y && y_size) {
		snprintf(path, sizeof(path), "/mnt/card/%s_%d.y", prefix, frm_count);
		fd = open(path, O_WRONLY | O_CREAT);
		if (fd < 0) {
			OSCL_LOGE("fail to open file: %s", path);
			return;
		}
		OSCL_LOGD("open %s success.", path);
		write(fd, (void *)y, y_size);
		close(fd);
	}
	if (uv && uv_size) {
		snprintf(path, sizeof(path), "/mnt/card/%s_%d.uv", prefix, frm_count);
		fd = open(path, O_WRONLY | O_CREAT);
		if (fd < 0) {
			OSCL_LOGE("fail to open file: %s", path);
			return;
		}
		OSCL_LOGD("open %s success.", path);
		write(fd, (void *)uv, uv_size);
		close(fd);
	}
	frm_count++;
}
#endif

OMX_ERRORTYPE oscl_disp_update_layer_data(void *hdl,
						   void *layer,
						   layer_data_t *data)
{
	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	struct plane_data *pdata = (struct plane_data *)layer;
	OMX_S32 format = omxformat_to_dispformat(data->eColorFormat);
	OMX_U32 fb_id;
	struct timeval start, start1;
	struct timeval end;
	OMX_S64 t, t1;
	OMX_U32 plane_id;

	oscl_param_check((NULL != vdisp), OMX_ErrorBadParameter, NULL);
	oscl_param_check((NULL != layer), OMX_ErrorBadParameter, NULL);
	oscl_param_check((NULL != data), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&disp_mutex);
	if (g_vdisp.event_thread_id == -1) {
		g_vdisp.event_thread_id = pthread_create(
						&g_vdisp.event_thread, NULL,
						drm_event_thread, &g_vdisp);
		if (g_vdisp.event_thread_id != 0) {
			pthread_mutex_unlock(&disp_mutex);
			return OMX_ErrorInsufficientResources;
		}
	}
	pthread_mutex_unlock(&disp_mutex);
	fb_id = get_image_fb_id(vdisp,
			(void *)data->addr[0],
			format,
			data->size, 0,
			data->src_size.width,
			data->src_size.height);
	if (0 == vdisp->dgb_flag) {
		vdisp->dgb_flag = 1;

		OSCL_LOGI("data->crop_rect.left = %d", data->crop_rect.left);
		OSCL_LOGI("data->crop_rect.top = %d", data->crop_rect.top);
		OSCL_LOGI("data->crop_rect.width = %d", data->crop_rect.width);
		OSCL_LOGI("data->crop_rect.height = %d", data->crop_rect.height);

		OSCL_LOGI("data->src_size.width = %d", data->src_size.width);
		OSCL_LOGI("data->src_size.height = %d", data->src_size.height);

		OSCL_LOGI("data->win_rect.x = %d", data->win_rect.left);
		OSCL_LOGI("data->win_rect.y = %d", data->win_rect.top);
		OSCL_LOGI("data->win_rect.width = %d", data->win_rect.width);
		OSCL_LOGI("data->win_rect.height = %d", data->win_rect.height);
	}
	memcpy(&vdisp->win_rect, &data->win_rect, sizeof(data->win_rect));
	memcpy(&vdisp->crop_rect, &data->crop_rect, sizeof(data->crop_rect));
	gettimeofday(&start, NULL);
	plane_id = drm_plane_index_to_id(pdata->plane_idx);
	ret = drmModeSetPlane(vdisp->drm_fd,
			      plane_id,
			      pdata->crtc_id,
			      fb_id,
			      0,
			      /* output */
			      data->win_rect.left,
			      data->win_rect.top,
			      data->win_rect.width,
			      data->win_rect.height,
			      /* input */
			      data->crop_rect.left << 16,
			      data->crop_rect.top << 16,
			      data->crop_rect.width << 16,
			      data->crop_rect.height << 16);
	ASSERT(ret == 0);
	gettimeofday(&start1, NULL);
	wait_flush_frame(vdisp, (void *)vdisp);
	gettimeofday(&end, NULL);

	t = start1.tv_sec * 1e6 + start1.tv_usec - start.tv_sec * 1e6 - start.tv_usec;
	t1 = end.tv_sec * 1e6 + end.tv_usec - start1.tv_sec * 1e6 - start1.tv_usec;
	if (t1 > (21 * 1000))
		OSCL_LOGE("layer %d>>update wait:%lld %lld", plane_id, t, t1);

	return OMX_ErrorNone;
}

/* Crop rotate not support */
OMX_ERRORTYPE oscl_disp_hw_rotate(void *hdl,
					 vdisp_rotate_mode_e mode,
					 rotate_buffer_t *src,
					 rotate_buffer_t *dst)
{
	oscl_param_check((NULL != hdl), OMX_ErrorBadParameter, NULL);

	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_U32 img_w, img_h;
	OMX_U32 input_fb_id;
	OMX_U32 output_fb_id;
	OMX_U32 size[3] = {0};
	disp_rect_t src_size;
	struct drm_lombo_rot_exec_arg exec_arg;
#ifdef _ROT_FPS_DISP
	struct timeval rot_start;
	struct timeval rot_end;
#endif
	OSCL_LOGD("rot src>> mode:%d src[%d %d %d %d] disp[%d %d %d %d]",
		mode,
		src->src_rect.left, src->src_rect.top,
		src->src_rect.width, src->src_rect.height,
		src->crop_rect.top, src->crop_rect.left,
		src->crop_rect.width, src->crop_rect.height);

	src_size.left = 0;
	src_size.top = 0;
	src_size.width = src->src_rect.width;
	src_size.height = src->src_rect.height;

	/* input para */
	if (DRM_FORMAT_NV12 == omxformat_to_dispformat(src->eColorFormat)) {
		img_w = src->crop_rect.width;
		img_h = src->crop_rect.height;
		size[0] = src->src_rect.width * src->crop_rect.top + src->crop_rect.left;
		size[1] = (src->src_rect.width * src->crop_rect.top >> 1) +
				src->crop_rect.left + src->size[0];

		input_fb_id = get_image_fb_id(vdisp,
					(void *)src->addr[0],
					DRM_FORMAT_NV12,
					size,
					src_size.width,
					img_w,
					img_h);

		OSCL_LOGD("input_fb_id:%d, img_w:%d, img_h:%d",
				input_fb_id, img_w, img_h);
	} else {
		OSCL_LOGE("Input format not support.");
		return OMX_ErrorBadParameter;
	}
	/* output para */
	if (DRM_FORMAT_NV12 == omxformat_to_dispformat(dst->eColorFormat)) {
		img_w = dst->crop_rect.width;
		img_h = dst->crop_rect.height;
		size[0] = dst->src_rect.width * dst->crop_rect.top + dst->crop_rect.left;
		size[1] = (dst->src_rect.width * dst->crop_rect.top >> 1) +
				dst->crop_rect.left + dst->size[0];

		output_fb_id = get_image_fb_id(vdisp,
					(void *)dst->addr[0],
					DRM_FORMAT_NV12,
					size,
					dst->src_rect.width,
					img_w,
					img_h);

		OSCL_LOGD("output_fb_id:%d, img_w:%d, img_h:%d",
				output_fb_id, img_w, img_h);
	} else {
		OSCL_LOGE("output format not support.");
		return OMX_ErrorBadParameter;
	}

	/* rotate process */
	exec_arg.handle = vdisp->rot_handle;
	exec_arg.in_fb = input_fb_id;
	exec_arg.out_fb = output_fb_id;
	exec_arg.transform =  omxrot_to_disprot_mode(mode);
	OSCL_LOGD("rot mode:%d", exec_arg.transform);

#ifdef _ROT_FPS_DISP
	gettimeofday(&rot_start, NULL);
#endif
	ret = lombo_rot_exec(vdisp->drm_fd, &exec_arg);
	if (0 != ret) {
		OSCL_LOGE("rotate fail. rot_handle:%d ret:%d %s",
			vdisp->rot_handle, ret, ERRSTR);
		OSCL_LOGE("rotate fail. exec_arg>>hd:%d in_fb:%d out_fb:%d transform:%#x",
			exec_arg.handle, exec_arg.in_fb,
			exec_arg.out_fb, exec_arg.transform);
		return OMX_ErrorInsufficientResources;
	}

#ifdef _ROT_FPS_DISP
	gettimeofday(&rot_end, NULL);
	t = rot_end.tv_sec + rot_end.tv_usec * 1e-6 -
		(rot_start.tv_sec + rot_start.tv_usec * 1e-6);
	fprintf(stderr, "time: %fms, resolution:%dx%d\n", t * 1000,
							  src->crop_rect.width,
							  src->crop_rect.height);
#endif
	/* flush output cached buffer */
	/* lombo_cache_flush_vir(dst->virtual_addr); */

	OSCL_LOGD("rot dst>>src[%d %d %d %d] disp[%d %d %d %d]",
		dst->src_rect.left, dst->src_rect.top,
		dst->src_rect.width, dst->src_rect.height,
		dst->crop_rect.top, dst->crop_rect.left,
		dst->crop_rect.width, dst->crop_rect.height);

	return ret;
}

#ifdef CONFIG_ARCH_LOMBO_N7V1
OMX_ERRORTYPE oscl_disp_sw_rotate(void *hdl, vdisp_rotate_mode_e mode,
		rotate_buffer_t *src, rotate_buffer_t *dst)
{
	 disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	 OMX_ERRORTYPE ret = OMX_ErrorNone;
	 OMX_U32 img_w, img_h;
	 disp_rot_cfg_t cfgs;
	 disp_rect_t src_size;
	 OMX_U32 pre_time, cur_time;

	if (!_sw_rot)
		_sw_rot_init();
	oscl_param_check(_sw_rot, OMX_ErrorInsufficientResources, NULL);
	OSCL_LOGD("b>> mode:%d src[%d %d %d %d] disp[%d %d %d %d]\n",
		mode,
		src->src_rect.left, src->src_rect.top,
		src->src_rect.width, src->src_rect.height,
		src->crop_rect.top, src->crop_rect.left,
		src->crop_rect.width, src->crop_rect.height);

	src_size.left = 0;
	src_size.top = 0;
	src_size.width = src->src_rect.width;
	src_size.height = src->src_rect.height;
	memset(&cfgs, 0, sizeof(cfgs));
	cfgs.rot_way = SW_ROT;
	cfgs.mode = omxrot_to_disprot_mode(mode);

	oscl_cache_flush_vir(src->addr[0]);
	oscl_cache_flush_vir(dst->addr[0]);
	/* input para */
	 if (DRM_FORMAT_NV12 == omxformat_to_dispformat(src->eColorFormat)) {
		img_w = src_size.width;
		img_h = src_size.height;
		cfgs.infb.format = DISP_FORMAT_NV12;

		cfgs.infb.addr[0] = (unsigned int)src->addr[0];
		cfgs.infb.width[0] = img_w;
		cfgs.infb.height[0] = img_h;
		cfgs.infb.linestride_bit[0] = src->src_rect.width;
		cfgs.infb.addr[1] = (unsigned int)src->addr[1];
		cfgs.infb.width[1] = img_w >> 1;
		cfgs.infb.height[1] = img_h >> 1;
		cfgs.infb.linestride_bit[1] = src->src_rect.width;
	} else {
		OSCL_LOGE("Input format not support.");
		return OMX_ErrorBadParameter;
	}

	/* output para */
	if (DRM_FORMAT_NV12 == omxformat_to_dispformat(dst->eColorFormat)) {
		img_w = dst->crop_rect.width;
		img_h = dst->crop_rect.height;
		cfgs.outfb.format = DISP_FORMAT_NV12;
		cfgs.outfb.width[0] = img_w;
		cfgs.outfb.height[0] = img_h;
		cfgs.outfb.addr[0] = (unsigned int)dst->addr[0];
		cfgs.outfb.linestride_bit[0] = src->src_rect.width;
		cfgs.outfb.width[1] = img_w >> 1;
		cfgs.outfb.height[1] = img_h >> 1;
		cfgs.outfb.addr[1] = (unsigned int)dst->addr[1];
		cfgs.outfb.linestride_bit[1] = src->src_rect.width;
	} else {
		OSCL_LOGE("Output format not support.");
		return OMX_ErrorBadParameter;
	}
	pre_time = oscl_get_msec();
	/* rotate process */
	ret = _sw_rot(&cfgs);
	if (0 != ret) {
		OSCL_LOGE("Rotate fail.");
		ret = OMX_ErrorInsufficientResources;
	}
	oscl_cache_flush_vir(src->addr[0]);
	oscl_cache_flush_vir(dst->addr[0]);
	cur_time = oscl_get_msec();
	if ((cur_time - pre_time) > 5) {
		OSCL_LOGE("SW rotate wast:%d, src[w:%d h:%d]\n",
			(cur_time - pre_time),
			src->src_rect.width, src->src_rect.height);
	}
	OSCL_LOGD("a>>src[%d %d %d %d] disp[%d %d %d %d]",
		dst->src_rect.left, dst->src_rect.top,
		dst->src_rect.width, dst->src_rect.height,
		dst->crop_rect.top, dst->crop_rect.left,
		dst->crop_rect.width, dst->crop_rect.height);

	 return ret;
}
#else
OMX_ERRORTYPE oscl_disp_sw_rotate(void *hdl, vdisp_rotate_mode_e mode,
		rotate_buffer_t *src, rotate_buffer_t *dst)
{
	return -1;
}
#endif

OMX_ERRORTYPE oscl_disp_hw_scale(void *hdl,
					  scale_buffer_t *src,
					  scale_buffer_t *dst)
{
	oscl_param_check((NULL != hdl), OMX_ErrorBadParameter, NULL);

	disp_hdl_t *vdisp = (disp_hdl_t *)hdl;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_U32 img_w, img_h;
	OMX_U32 input_fb_id;
	OMX_U32 output_fb_id;
	struct drm_lombo_scale_exec_arg exec_arg;

#ifdef _SE_FPS_DISP
	struct timeval se_start;
	struct timeval se_end;
	long t = 0L;
#endif

	OSCL_LOGD("hw se>>infb[%d %d] crop[%d %d %d %d]-> outfb[%d %d] crop[%d %d %d %d]",
			src->fb_size.width, src->fb_size.height,
			src->crop_rect.top, src->crop_rect.left,
			src->crop_rect.width, src->crop_rect.height,
			dst->fb_size.width, dst->fb_size.height,
			dst->crop_rect.top, dst->crop_rect.left,
			dst->crop_rect.width, dst->crop_rect.height);
	if ((src->crop_rect.top > 0) || (src->crop_rect.left > 0) ||
		(src->fb_size.width != src->crop_rect.width) ||
		(src->fb_size.height != src->crop_rect.height)) {
		if (0 == vdisp->dgb_flag) {
			OSCL_LOGW("Se driver not support crop. [%d %d]->[%d %d %d %d]",
					src->fb_size.width, src->fb_size.height,
					src->crop_rect.top, src->crop_rect.left,
					src->crop_rect.width, src->crop_rect.height);
		}
#if 1 /* debug: force disable crop */
		{
			dst->crop_rect.left = 0;
			src->crop_rect.top = 0;
			src->crop_rect.width = src->fb_size.width;
			src->crop_rect.height = src->fb_size.height;
		}
#else
		return OMX_ErrorBadParameter;
#endif
	}
	/* input para */
	if (DRM_FORMAT_NV12 == omxformat_to_dispformat(src->eColorFormat)) {
		img_w = src->fb_size.width;
		img_h = src->fb_size.height;

		input_fb_id = get_image_fb_id(vdisp,
				(void *)src->addr[0],
				DRM_FORMAT_NV12,
				src->size, 0,
				img_w,
				img_h);

		OSCL_LOGD("input_fb_id:%d(%p), img_w:%d, img_h:%d",
			 input_fb_id, src->addr[0], img_w, img_h);
	} else {
		OSCL_LOGE("Input format not support.");
		return OMX_ErrorBadParameter;
	}

	/* output para */
	if (DRM_FORMAT_RGB565 == omxformat_to_dispformat(dst->eColorFormat)) {
		img_w = dst->fb_size.width;
		img_h = dst->fb_size.height;

		output_fb_id = get_image_fb_id(vdisp,
				 (void *)dst->addr[0],
				 DRM_FORMAT_RGB565,
				 dst->size, 0,
				 img_w,
				 img_h);

		OSCL_LOGD("output_fb_id:%d(%p), img_w:%d, img_h:%d",
			 output_fb_id, dst->addr[0], img_w, img_h);
	} else {
		OSCL_LOGE("output format not support. %#x", dst->eColorFormat);
		return OMX_ErrorBadParameter;
	}
	exec_arg.handle = vdisp->scale_handle;

	exec_arg.in_fb.fb_id = input_fb_id;
	exec_arg.in_fb.crop_x = src->crop_rect.left;
	exec_arg.in_fb.crop_y = src->crop_rect.top;
	exec_arg.in_fb.crop_w = src->crop_rect.width;
	exec_arg.in_fb.crop_h = src->crop_rect.height;

	exec_arg.out_fb.fb_id = output_fb_id;
	exec_arg.out_fb.crop_x = dst->crop_rect.left;
	exec_arg.out_fb.crop_y = dst->crop_rect.top;
	exec_arg.out_fb.crop_w = dst->crop_rect.width;
	exec_arg.out_fb.crop_h = dst->crop_rect.height;
#ifdef _SE_FPS_DISP
	gettimeofday(&se_start, NULL);
#endif

	OSCL_LOGD("SCALE in: fb:%d x:%d y:%d w:%d h:%d",
					exec_arg.in_fb.fb_id,
					exec_arg.in_fb.crop_x,
					exec_arg.in_fb.crop_y,
					exec_arg.in_fb.crop_w,
					exec_arg.in_fb.crop_h);
	OSCL_LOGD("SCALE out: fb:%d x:%d y:%d w:%d h:%d",
					exec_arg.out_fb.fb_id,
					exec_arg.out_fb.crop_x,
					exec_arg.out_fb.crop_y,
					exec_arg.out_fb.crop_w,
					exec_arg.out_fb.crop_h);
	ret = lombo_scale_exec(vdisp->drm_fd, &exec_arg);
	if (0 != ret) {
		OSCL_LOGE("scale fail. scale_handle:%d, ret:%d %s",
			vdisp->scale_handle, ret, ERRSTR);
		return OMX_ErrorInsufficientResources;
	}
#ifdef _SE_FPS_DISP
	gettimeofday(&se_end, NULL);
	t = se_end.tv_sec * 1e+3 + se_end.tv_usec * 1e-3 -
		(se_start.tv_sec * 1e+3 + se_start.tv_usec * 1e-3);
	fprintf(stderr, "scale >> time: %dms, resolution:(%dx%d)->(%dx%d)\n", t,
					src->fb_size.width,
					src->fb_size.height,
					dst->fb_size.width,
					dst->fb_size.height);
#endif

	return ret;
}

