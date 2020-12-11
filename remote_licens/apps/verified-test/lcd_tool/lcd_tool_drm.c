/*
 * lcd_tool_drm.c
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/time.h>

#include "kms.h"
#include "xf86drm.h"
#include "xf86drmMode.h"
#include "lombo_drm.h"

#define LCD_TOOL_DRM_DEBUG				0
#define	LCD_TOOL_WAIT_VBLANK_BLOCKING			0

#define USE_CMD_DRM_IOCTL_LOMBO_MODE_OBJ_SETPROPERTY	1
#define USE_CMD_DRM_IOCTL_LOMBO_MODE_SETGAMMA		1

#define memclear(s) memset(&s, 0, sizeof(s))

#define U642VOID(x) ((void *)(unsigned long)(x))
#define VOID2U64(x) ((uint64_t)(unsigned long)(x))

struct crtc {
	drmModeCrtc *crtc;
	drmModeObjectProperties *props;
	drmModePropertyRes **props_info;
	drmModeModeInfo *mode;
};

struct encoder {
	drmModeEncoder *encoder;
};

struct connector {
	drmModeConnector *connector;
	drmModeObjectProperties *props;
	drmModePropertyRes **props_info;
	char *name;
};

struct fb {
	drmModeFB *fb;
};

struct plane {
	drmModePlane *plane;
	drmModeObjectProperties *props;
	drmModePropertyRes **props_info;
};

struct resources {
	drmModeRes *res;
	drmModePlaneRes *plane_res;

	struct crtc *crtcs;
	struct encoder *encoders;
	struct connector *connectors;
	struct fb *fbs;
	struct plane *planes;
};

struct device {
	int fd;

	struct resources *resources;

	struct {
		unsigned int width;
		unsigned int height;

		unsigned int fb_id;
		struct bo *bo;
		struct bo *cursor_bo;
	} mode;

	int use_atomic;
	drmModeAtomicReq *req;

	struct drm_lombo_mode_set_plane mode_plane;
	struct drm_lombo_mode_fb mode_fb[4];
};

static void free_resources(struct resources *res)
{
	int i;

	if (!res)
		return;

#define free_resource(_res, __res, type, Type)					\
	do {									\
		if (!(_res)->type##s)						\
			break;							\
		for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) {	\
			if (!(_res)->type##s[i].type)				\
				break;						\
			drmModeFree##Type((_res)->type##s[i].type);		\
		}								\
		free((_res)->type##s);						\
	} while (0)

#define free_properties(_res, __res, type)					\
	do {									\
		for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) {	\
			drmModeFreeObjectProperties(res->type##s[i].props);	\
			free(res->type##s[i].props_info);			\
		}								\
	} while (0)

	if (res->res) {
		free_properties(res, res, crtc);

		free_resource(res, res, crtc, Crtc);
		free_resource(res, res, encoder, Encoder);

		for (i = 0; i < res->res->count_connectors; i++)
			free(res->connectors[i].name);

		free_resource(res, res, connector, Connector);
		free_resource(res, res, fb, FB);

		drmModeFreeResources(res->res);
	}

	if (res->plane_res) {
		free_properties(res, plane_res, plane);

		free_resource(res, plane_res, plane, Plane);

		drmModeFreePlaneResources(res->plane_res);
	}

	free(res);
}

static struct resources *get_resources(struct device *dev)
{
	struct resources *res;
	int i;

	res = calloc(1, sizeof(*res));
	if (res == 0)
		return NULL;

	drmSetClientCap(dev->fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);

	res->res = drmModeGetResources(dev->fd);
	if (!res->res) {
		fprintf(stderr, "drmModeGetResources failed: %s\n",
			strerror(errno));
		goto error;
	}

	res->crtcs = calloc(res->res->count_crtcs, sizeof(*res->crtcs));
	res->encoders = calloc(res->res->count_encoders, sizeof(*res->encoders));
	res->connectors = calloc(res->res->count_connectors, sizeof(*res->connectors));
	res->fbs = calloc(res->res->count_fbs, sizeof(*res->fbs));

	if (!res->crtcs || !res->encoders || !res->connectors || !res->fbs)
		goto error;

#define get_resource(_res, __res, type, Type)					\
	do {									\
		for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) {	\
			(_res)->type##s[i].type =				\
				drmModeGet##Type(dev->fd, (_res)->__res->type##s[i]); \
			if (!(_res)->type##s[i].type)				\
				fprintf(stderr, "could not get %s %i: %s\n",	\
					#type, (_res)->__res->type##s[i],	\
					strerror(errno));			\
		}								\
	} while (0)

	get_resource(res, res, crtc, Crtc);
	get_resource(res, res, encoder, Encoder);
	get_resource(res, res, connector, Connector);
	get_resource(res, res, fb, FB);

	/* Set the name of all connectors based on the type name and the per-type ID. */
	for (i = 0; i < res->res->count_connectors; i++) {
		struct connector *connector = &res->connectors[i];
		drmModeConnector *conn = connector->connector;
		int num;

		num = asprintf(&connector->name, "%s-%u",
			 util_lookup_connector_type_name(conn->connector_type),
			 conn->connector_type_id);
		if (num < 0)
			goto error;
	}

#define get_properties(_res, __res, type, Type)					\
	do {									\
		for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) {	\
			struct type *obj = &res->type##s[i];			\
			unsigned int j;						\
			obj->props =						\
				drmModeObjectGetProperties(dev->fd, obj->type->type##_id, \
							   DRM_MODE_OBJECT_##Type); \
			if (!obj->props) {					\
				fprintf(stderr,					\
					"could not get %s %i properties: %s\n", \
					#type, obj->type->type##_id,		\
					strerror(errno));			\
				continue;					\
			}							\
			obj->props_info = calloc(obj->props->count_props,	\
						 sizeof(*obj->props_info));	\
			if (!obj->props_info)					\
				continue;					\
			for (j = 0; j < obj->props->count_props; ++j)		\
				obj->props_info[j] =				\
					drmModeGetProperty(dev->fd, obj->props->props[j]); \
		}								\
	} while (0)

	get_properties(res, res, crtc, CRTC);
	get_properties(res, res, connector, CONNECTOR);

	for (i = 0; i < res->res->count_crtcs; ++i)
		res->crtcs[i].mode = &res->crtcs[i].crtc->mode;

	res->plane_res = drmModeGetPlaneResources(dev->fd);
	if (!res->plane_res) {
		fprintf(stderr, "drmModeGetPlaneResources failed: %s\n",
			strerror(errno));
		return res;
	}

	res->planes = calloc(res->plane_res->count_planes, sizeof(*res->planes));
	if (!res->planes)
		goto error;

	get_resource(res, plane_res, plane, Plane);
	get_properties(res, plane_res, plane, PLANE);

	return res;

error:
	free_resources(res);
	return NULL;
}

/* -----------------------------------------------------------------------------
 * Properties
 */

struct property_arg {
	uint32_t obj_id;
	uint32_t obj_type;
	char name[DRM_PROP_NAME_LEN+1];
	uint32_t prop_id;
	uint64_t value;
};

#if USE_CMD_DRM_IOCTL_LOMBO_MODE_OBJ_SETPROPERTY
static int drmLomboModeObjectSetProperty(int fd, uint32_t object_id,
	uint32_t object_type, uint32_t property_id, uint64_t value)
{
	struct drm_mode_obj_set_property prop;

	memset(&prop, 0, sizeof(prop));
	prop.value = value;
	prop.prop_id = property_id;
	prop.obj_id = object_id;
	prop.obj_type = object_type;

	return drmIoctl(fd, DRM_IOCTL_LOMBO_MODE_OBJ_SETPROPERTY, &prop);
}
#endif

static void set_property(struct device *dev, struct property_arg *p)
{
	drmModeObjectProperties *props = NULL;
	drmModePropertyRes **props_info = NULL;
	const char *obj_type;
	int ret;
	int i;

	p->obj_type = 0;
	p->prop_id = 0;

#define find_object(_res, __res, type, Type)					\
	do {									\
		for (i = 0; i < (int)(_res)->__res->count_##type##s; ++i) {	\
			struct type *obj = &(_res)->type##s[i];			\
			if (obj->type->type##_id != p->obj_id)			\
				continue;					\
			p->obj_type = DRM_MODE_OBJECT_##Type;			\
			obj_type = #Type;					\
			props = obj->props;					\
			props_info = obj->props_info;				\
		}								\
	} while(0)								\

	find_object(dev->resources, res, crtc, CRTC);
	if (p->obj_type == 0)
		find_object(dev->resources, res, connector, CONNECTOR);
	if (p->obj_type == 0)
		find_object(dev->resources, plane_res, plane, PLANE);
	if (p->obj_type == 0) {
		fprintf(stderr, "Object %i not found, can't set property\n",
			p->obj_id);
			return;
	}

	if (!props) {
		fprintf(stderr, "%s %i has no properties\n",
			obj_type, p->obj_id);
		return;
	}

	for (i = 0; i < (int)props->count_props; ++i) {
		if (!props_info[i])
			continue;
		if (strcmp(props_info[i]->name, p->name) == 0)
			break;
	}

	if (i == (int)props->count_props) {
		fprintf(stderr, "%s %i has no %s property\n",
			obj_type, p->obj_id, p->name);
		return;
	}

	p->prop_id = props->props[i];

	if (!dev->use_atomic) {
#if USE_CMD_DRM_IOCTL_LOMBO_MODE_OBJ_SETPROPERTY
		/* this function do not need master jurisdiction */
		ret = drmLomboModeObjectSetProperty(dev->fd,
						p->obj_id, p->obj_type,
						p->prop_id, p->value);
#else
		/* this function need master jurisdiction */
		ret = drmModeObjectSetProperty(dev->fd,
						p->obj_id, p->obj_type,
						p->prop_id, p->value);
#endif
	} else
		ret = drmModeAtomicAddProperty(dev->req, p->obj_id,
						p->prop_id, p->value);

	if (ret < 0)
		fprintf(stderr, "failed to set %s %i property %s to %" PRIu64 ": %s\n",
			obj_type, p->obj_id, p->name, p->value, strerror(errno));
}

typedef enum {
	PROP_GAMMA_ENABLE,
	PROP_COLOR_ENHANCE_ENABLE,
	PROP_COLOR_ENHANCE_BRIGHTNESS,
	PROP_COLOR_ENHANCE_SATURATION,
	PROP_COLOR_ENHANCE_CONTRAST,
	PROP_VIDEO_LTI,
	PROP_VIDEO_PEAK,
	PROP_MAX_NUM,
}lcd_prop_type;

static const char *PROP_TYPE_STRING[] = {
	"gamma_enable",
	"color_enhance_enable",
	"color_enhance_brightness",
	"color_enhance_saturation",
	"color_enhance_contrast",
	"video_lti",
	"video_peak",
};

static struct device dev;

static int lcd_tool_set_property(lcd_prop_type type, int val)
{
	drmModeRes *res;
	drmModePlaneRes *plane_res;

	unsigned int prop_count = 0;
	struct property_arg *prop_args = NULL;
	struct property_arg *prop_arg = NULL;

	unsigned int i;
	bool prop_crtc_flag;

	if (type >= PROP_MAX_NUM) {
		printf("illegal prop type[%d], max less than %d\n",
			type, PROP_MAX_NUM);
		return __LINE__;
	} else if (type <= PROP_COLOR_ENHANCE_CONTRAST)
		prop_crtc_flag = true; /* for crtcs */
	else
		prop_crtc_flag = false;  /* for planes */

	if (dev.fd < 0) {
		printf("[%s %d] use lcd_tool_init to open dev first\n",
			__FUNCTION__, __LINE__);
		return __LINE__;
	}

	if (!dev.resources) {
		printf("[%s %d] resources is NULL\n", __FUNCTION__, __LINE__);
		return __LINE__;
	}

	res = dev.resources->res;
	plane_res = dev.resources->plane_res;
	#if 0
	printf("drmModeGetResources count_crtcs %d\n",
		res->count_crtcs);
	printf("drmModeGetResources count_planes %d\n",
		plane_res->count_planes);
	#endif

	if (prop_crtc_flag)
		prop_count = res->count_crtcs;
	else
		prop_count = plane_res->count_planes;

	prop_args = calloc(1, prop_count * sizeof *prop_args);
	if (prop_args == NULL) {
		fprintf(stderr, "memory allocation failed\n");
		return __LINE__;
	}

	for (i = 0; i < prop_count; i++){
		prop_arg = &prop_args[i];
		if (prop_crtc_flag)
			prop_arg->obj_id = res->crtcs[i];
		else
			prop_arg->obj_id = plane_res->planes[i];

		prop_arg->obj_type = 0;

		memcpy(prop_arg->name, PROP_TYPE_STRING[type],
			strlen(PROP_TYPE_STRING[type]));
		prop_arg->name[DRM_PROP_NAME_LEN] = '\0';

		prop_arg->value = val;
	}

	for (i = 0; i < prop_count; ++i)
		set_property(&dev, &prop_args[i]);

	free(prop_args);

	return 0;
}

static long long time_get_us(const char *fun, int line)
{
        struct timeval tv;

        gettimeofday(&tv, NULL);

	#if LCD_TOOL_DRM_DEBUG
	printf("[%s %d]: sec:%ld, usec:%ld\n", fun, line,
		tv.tv_sec, tv.tv_usec);
	#endif

        return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

#if !LCD_TOOL_WAIT_VBLANK_BLOCKING
static int lcd_tool_drmHandleEvent(int fd, drmEventContextPtr evctx)
{
	char buffer[1024];
	int len, i;
	struct drm_event *e;

	/* The DRM read semantics guarantees that we always get only
	 * complete events. */
	len = read(fd, buffer, sizeof buffer);
	if (len == 0)
		return 0;
	if (len < (int)sizeof *e)
		return -1;

	i = 0;
	while (i < len) {
		e = (struct drm_event *)(buffer + i);
		/* printf("%s: type:%08x\n", __FUNCTION__, e->type); */
		if (e->type == DRM_EVENT_VBLANK) {
			/* just wait thevblank  event */
			/* printf("%s: DRM_EVENT_VBLANK\n", __FUNCTION__); */
			return e->type;
		}
		i += e->length;
	}

	return 0;
}
#endif

static int lcd_tool_wait_vblank(struct device *dev)
{
#if !LCD_TOOL_WAIT_VBLANK_BLOCKING
	drmEventContext evctx;
#endif
	int ret;

	drmVBlank vbl;
#if LCD_TOOL_WAIT_VBLANK_BLOCKING
	/*  no DRM_VBLANK_EVENT mean drmWaitVBlank is blocking to wait vblank */
	vbl.request.type = DRM_VBLANK_RELATIVE;
#else
	/* DRM_VBLANK_EVENT mean drmWaitVBlank is not blocking*/
	vbl.request.type = DRM_VBLANK_RELATIVE | DRM_VBLANK_EVENT;
#endif
	vbl.request.sequence = 1; /* wait event count */
	/* time_get_us(__FUNCTION__, __LINE__); */
	ret = drmWaitVBlank(dev->fd, &vbl);
	if (ret != 0) {
		printf("drmWaitVBlank (relative) failed ret: %i\n", ret);
		return -1;
	}

	#if 0
	printf("[%s %d]: type:%08x sequence:%d sec:%ld, usec:%ld\n",
		__FUNCTION__, __LINE__,
		vbl.reply.type, vbl.reply.sequence,
		vbl.reply.tval_sec, vbl.reply.tval_usec);
	#endif
	/* time_get_us(__FUNCTION__, __LINE__); */

/* do not need to wait events when drmWaitVBlank is blocking */
#if !LCD_TOOL_WAIT_VBLANK_BLOCKING
	memset(&evctx, 0, sizeof evctx);
	evctx.version = DRM_EVENT_CONTEXT_VERSION;
	evctx.vblank_handler = NULL;
	evctx.page_flip_handler = NULL;

	while (1) {
#if 0
		struct pollfd pfd[2];

		pfd[0].fd = 0;
		pfd[0].events = POLLIN;
		pfd[1].fd = fd;
		pfd[1].events = POLLIN;

		if (poll(pfd, 2, -1) < 0) {
			fprintf(stderr, "poll error\n");
			break;
		}

		if (pfd[0].revents)
			break;
#else
		struct timeval timeout = { .tv_sec = 3, .tv_usec = 0 };
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(0, &fds);
		FD_SET(dev->fd, &fds);
		ret = select(dev->fd + 1, &fds, NULL, NULL, &timeout);

		if (ret <= 0) {
			fprintf(stderr, "select timed out or error (ret %d)\n",
				ret);
			continue;
		} else if (FD_ISSET(0, &fds)) {
			break;
		}
#endif
		if (lcd_tool_drmHandleEvent(dev->fd, &evctx) == DRM_EVENT_VBLANK)
			break;
	}
#endif

	return 0;
}

int lcd_tool_color_enhance_enable(void)
{
	return lcd_tool_set_property(PROP_COLOR_ENHANCE_ENABLE, 1);
}

int lcd_tool_color_enhance_disable(void)
{
	return lcd_tool_set_property(PROP_COLOR_ENHANCE_ENABLE, 0);
}

/* val range: 0 -100 */
int lcd_tool_set_color_enhance_brightness(int val)
{
	return lcd_tool_set_property(PROP_COLOR_ENHANCE_BRIGHTNESS, val);
}

/* val range: 0 -100 */
int lcd_tool_set_color_enhance_saturation(int val)
{
	return lcd_tool_set_property(PROP_COLOR_ENHANCE_SATURATION, val);
}

/* val range: 0 -100 */
int lcd_tool_set_color_enhance_contrast(int val)
{
	return lcd_tool_set_property(PROP_COLOR_ENHANCE_CONTRAST, val);
}

/* val range: 0 -31, 0 mean close this function */
int lcd_tool_set_video_lti(int val)
{
	return lcd_tool_set_property(PROP_VIDEO_LTI, val);
}

/* val range: 0 -15, 0 mean close this function */
int lcd_tool_set_video_peak(int val)
{
	return lcd_tool_set_property(PROP_VIDEO_PEAK, val);
}

int lcd_tool_gamma_enable(void)
{
	time_get_us(__FUNCTION__, __LINE__);
	lcd_tool_wait_vblank(&dev);
	time_get_us(__FUNCTION__, __LINE__);
	return lcd_tool_set_property(PROP_GAMMA_ENABLE, 1);
}

int lcd_tool_gamma_disable(void)
{
	time_get_us(__FUNCTION__, __LINE__);
	lcd_tool_wait_vblank(&dev);
	time_get_us(__FUNCTION__, __LINE__);
	return lcd_tool_set_property(PROP_GAMMA_ENABLE, 0);
}

#if USE_CMD_DRM_IOCTL_LOMBO_MODE_SETGAMMA
static int drmLomboModeCrtcSetGamma(int fd,
	uint32_t crtc_id, uint32_t size,
	uint16_t *red, uint16_t *green, uint16_t *blue)
{
	struct drm_mode_crtc_lut l;

	memclear(l);
	l.crtc_id = crtc_id;
	l.gamma_size = size;
	l.red = VOID2U64(red);
	l.green = VOID2U64(green);
	l.blue = VOID2U64(blue);

	return drmIoctl(fd, DRM_IOCTL_LOMBO_MODE_SETGAMMA, &l);
}
#endif

int lcd_tool_gamma_update(char *gamma_tbl, int tbl_size, bool enabled)
{
	drmModeRes *res;
	uint32_t crtc_id;

	uint32_t *tbl = (uint32_t *)gamma_tbl;
	uint16_t *red;
	uint16_t *green;
	uint16_t *blue;

	int i, size;

	printf("%s++\n", __FUNCTION__);
	if (dev.fd < 0) {
		printf("[%s %d] use lcd_tool_init to open dev first\n",
			__FUNCTION__, __LINE__);
		return __LINE__;
	}

	if (!dev.resources) {
		printf("[%s %d] resources is NULL\n", __FUNCTION__, __LINE__);
		return __LINE__;
	}

	/* disable it first */
	lcd_tool_gamma_disable();

	res = dev.resources->res;
	crtc_id = res->crtcs[0];

	size = tbl_size * sizeof(uint16_t);
	red = calloc(1, size);
	green = calloc(1, size);
	blue = calloc(1, size);

	for (i = 0; i < tbl_size; i++) {
		red[i] = tbl[i] & 0x00ff;
		tbl[i] = tbl[i] >> 8;
		green[i] = tbl[i] & 0x00ff;
		tbl[i] = tbl[i] >> 8;
		blue[i] = tbl[i] & 0x00ff;
	}

	#if USE_CMD_DRM_IOCTL_LOMBO_MODE_SETGAMMA
	/* this do not need master */
	drmLomboModeCrtcSetGamma(dev.fd, crtc_id, tbl_size, red, green, blue);
	#else
	drmModeCrtcSetGamma(dev.fd, crtc_id, tbl_size, red, green, blue);
	#endif

	free(red);
	free(green);
	free(blue);

	/* enable it when gamma is enabled */
	if (enabled)
		lcd_tool_gamma_enable();

	printf("%s--\n", __FUNCTION__);

	return 0;
}

int lcd_tool_init(void)
{
	printf("%s++\n", __FUNCTION__);

	memset(&dev, 0x00, sizeof(struct device));
	dev.fd = util_open(NULL, NULL);
	if (dev.fd <= 0) {
		printf("[%s %d] fail to open drm\n", __FUNCTION__, __LINE__);
		return __LINE__;
	}

	dev.resources = get_resources(&dev);
	if (!dev.resources) {
		printf("[%s %d] get_resources err\n", __FUNCTION__, __LINE__);
		return __LINE__;
	}

	printf("%s--\n", __FUNCTION__);

	return 0;
}

int lcd_tool_exit(void)
{
	printf("%s++\n", __FUNCTION__);

	if (dev.fd <= 0) {
		printf("[%s %d] use lcd_tool_init to open dev first\n",
			__FUNCTION__, __LINE__);
		return -1;
	}

	free_resources(dev.resources);
	drmClose(dev.fd);

	printf("%s--\n", __FUNCTION__);

	return 0;
}

