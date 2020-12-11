#ifndef _DRM_DISPLAY_H_
#define _DRM_DISPLAY_H_

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>
#include <drm/drm.h>
#include <log/log.h>

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#define ERRSTR			strerror(errno)
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define ASSERT(x) if (unlikely(!(x))) \
	{ ALOGE("%s %d %s", __func__, __LINE__, ERRSTR); exit(1); }

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * @ROT_X: clockwise rotation.
 * COPY/FLIP_H/FLIP_V/ROT_90 are the base transform operation.
 * Others are the expand transform operation(flip first, then rot).
 */
typedef enum {
	LOMBO_DRM_TRANSFORM_COPY = 0, /* original image copy */
	LOMBO_DRM_TRANSFORM_FLIP_H = 1, /* (1 << 0) */
	LOMBO_DRM_TRANSFORM_FLIP_V = 2, /* (1 << 1) */
	LOMBO_DRM_TRANSFORM_ROT_90 = 4, /* (1 << 2) */
	LOMBO_DRM_TRANSFORM_ROT_180 = 3, /* (FLIP_H | FLIP_V) */
	LOMBO_DRM_TRANSFORM_ROT_270 = 7, /* ((FLIP_H | FLIP_V) + ROT_90) */
	LOMBO_DRM_TRANSFORM_FLIP_H_ROT_90 = 5, /* (FLIP_H + ROT_90) */
	LOMBO_DRM_TRANSFORM_FLIP_V_ROT_90 = 6, /* (FLIP_V + ROT_90) */
} drm_lombo_transform_t;


/* hw ip version */
struct drm_lombo_rot_ver {
	__u32 major;
	__u32 minor;
};

/* user handle of rot */
typedef __u32 drm_lombo_rot_user_handle_t;

/* rot timeout, default 1s in ditdrv */
struct drm_lombo_rot_timeout {
	drm_lombo_rot_user_handle_t handle;
	__u64 time_ns;
};

/*
 * @handle: client handle of rot.
 * @transform: rotate&flip mode. see drm_lombo_transform_t.
 * @in_fb: input fb id.
 * @out_fb: output fb id.
 */
struct drm_lombo_rot_exec_arg {
	drm_lombo_rot_user_handle_t handle;
	drm_lombo_transform_t transform;
	__u32 in_fb;
	__u32 out_fb;
};

/* user handle of scale */
typedef uint32_t drm_lombo_scale_user_handle_t;

/* scale timeout, default 3s in drv */
struct drm_lombo_scale_timeout {
	drm_lombo_scale_user_handle_t handle;
	uint64_t time_ns;
};

/*
 * @fb_id: id of drm framebuffer.
 * @crop_x/y/w/h: crop(x, y, width, height) of fb.
 */
struct drm_lombo_scale_fb {
	uint32_t fb_id;
	uint32_t crop_x;
	uint32_t crop_y;
	uint32_t crop_w;
	uint32_t crop_h;
};

/*
 * @handle: client handle of scale.
 * @in_fb: input fb id.
 * @out_fb: output fb id.
 */
struct drm_lombo_scale_exec_arg {
	drm_lombo_scale_user_handle_t handle;

	struct drm_lombo_scale_fb in_fb;
	struct drm_lombo_scale_fb out_fb;
};

struct framebuffer_plane {
	uint32_t handle;
	uint32_t size;
	uint32_t stride;
	void *map;
};

struct framebuffer {
	int fd;

	uint32_t width;
	uint32_t height;
	uint32_t format;
	uint32_t size;
	void *virtual_addr;

	int num_planes;
	struct framebuffer_plane planes[4];

	uint32_t fb_id;
};

struct plane_data {
	int32_t  plane_idx;
	/* uint32_t plane_id;*/  /* the id of plane to use */
	uint32_t crtc_id;  /* the id of CRTC to bind to */
	bool has_position;
	int32_t x, y;
	uint32_t w, h;
	double scale;
	uint32_t fb_id;
	char format_str[5]; /* need to leave room for terminating \0 */
	uint32_t fourcc;
};

struct modeset_out {
	struct modeset_out *next;

	int fd;

	unsigned int front_buf;
	int num_buffers;
	struct framebuffer *bufs;

	uint32_t output_id;

	drmModeModeInfo mode;
	uint32_t conn_id;
	uint32_t enc_id;
	uint32_t crtc_id;
	uint32_t crtc_idx;

	bool cleanup;

	void *data;
	void *data2;

	int pflip_pending;
	void (*flip_event)(void *);

	int dpms;

};

/* common.c */
void get_time_now(struct timespec *ts);
uint64_t get_time_elapsed_us(const struct timespec *ts_start,
	const struct timespec *ts_end);

int drm_open_dev_dumb(const char *node);
void drm_create_dumb_fb(int fd, uint32_t width, uint32_t height, struct framebuffer *buf);
void drm_create_dumb_fb2(int fd, uint32_t width, uint32_t height, uint32_t format,
	struct framebuffer *buf);
void drm_destroy_dumb_fb(struct framebuffer *buf);
void drm_set_dpms(int fd, uint32_t conn_id, int dpms);

#define for_each_output(pos, head) \
	for (struct modeset_out *(pos) = (head); (pos); (pos) = (pos)->next)

uint32_t drm_plane_index_to_id(int idx);
int drm_plane_bottom(int idx);
int drm_plane_top(int idx);
int drm_reserve_plane_index(int fd);
void drm_release_plane_index(int idx);

void modeset_prepare(int fd, struct modeset_out **out_list);
void modeset_alloc_fbs(struct modeset_out *list, int num_buffers);
void modeset_set_modes(struct modeset_out *list);
void modeset_start_flip(struct modeset_out *out);
void modeset_main_loop(struct modeset_out *modeset_list, void (*flip_event)(void *));
void modeset_cleanup(struct modeset_out *out_list);

static inline struct modeset_out *find_output(struct modeset_out *list, int output_id)
{
	for_each_output(out, list)
	if (out->output_id == output_id)
		return out;
	return NULL;
}

static inline uint32_t MAKE_RGB(uint8_t r, uint8_t g, uint8_t b)
{
	return (r << 16) | (g << 8) | (b << 0);
}

static inline uint32_t MAKE_ARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

static inline uint16_t MAKE_RGB565(uint8_t r, uint8_t g, uint8_t b)
{
	return (r << 11) | (g << 6) | (b << 0);
}

static inline uint32_t MAKE_YUV_601_Y(uint8_t r, uint8_t g, uint8_t b)
{
	return ((66 * r + 129 * g +  25 * b + 128) >> 8) + 16;
}

static inline uint32_t MAKE_YUV_601_U(uint8_t r, uint8_t g, uint8_t b)
{
	return ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
}

static inline uint32_t MAKE_YUV_601_V(uint8_t r, uint8_t g, uint8_t b)
{
	return ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;
}

void drm_draw_color_bar(struct framebuffer *buf, int old_xpos, int xpos, int width);
void draw_pixel(struct framebuffer *buf, int x, int y, uint32_t color);
void drm_draw_test_pattern(struct framebuffer *fb, int pattern);
void drm_clear_fb(struct framebuffer *fb);

#define DRM_LOMBO_IGNORE_IOCTL_PERMIT 0x07

/* ROT */
#define DRM_LOMBO_ROT_GET_VER 0x18
#define DRM_LOMBO_ROT_OPEN 0x19
#define DRM_LOMBO_ROT_CLOSE 0x1A
#define DRM_LOMBO_ROT_SET_TIMEOUT 0x1B
#define DRM_LOMBO_ROT_EXEC 0x1C

#define DRM_LOMBO_SCALE_OPEN 0x20
#define DRM_LOMBO_SCALE_CLOSE 0x21
#define DRM_LOMBO_SCALE_SET_TIMEOUT 0x22
#define DRM_LOMBO_SCALE_EXEC 0x23

#define DRM_IOCTL_LOMBO_IGNORE_IOCTL_PERMIT DRM_IOWR(DRM_COMMAND_BASE +	\
		DRM_LOMBO_IGNORE_IOCTL_PERMIT, unsigned int)

#define DRM_IOCTL_LOMBO_ROT_GET_VER DRM_IOR(DRM_COMMAND_BASE + \
		DRM_LOMBO_ROT_GET_VER, struct drm_lombo_rot_ver)
#define DRM_IOCTL_LOMBO_ROT_OPEN DRM_IOR(DRM_COMMAND_BASE + \
		DRM_LOMBO_ROT_OPEN, drm_lombo_rot_user_handle_t)
#define DRM_IOCTL_LOMBO_ROT_CLOSE DRM_IOWR(DRM_COMMAND_BASE + \
		DRM_LOMBO_ROT_CLOSE, drm_lombo_rot_user_handle_t)
#define DRM_IOCTL_LOMBO_ROT_SET_TIMEOUT DRM_IOW(DRM_COMMAND_BASE + \
		DRM_LOMBO_ROT_SET_TIMEOUT, struct drm_lombo_rot_timeout)
#define DRM_IOCTL_LOMBO_ROT_EXEC DRM_IOW(DRM_COMMAND_BASE + \
		DRM_LOMBO_ROT_EXEC, struct drm_lombo_rot_exec_arg)

#define DRM_IOCTL_LOMBO_SCALE_OPEN DRM_IOR(DRM_COMMAND_BASE +	\
			DRM_LOMBO_SCALE_OPEN, drm_lombo_scale_user_handle_t)
#define DRM_IOCTL_LOMBO_SCALE_CLOSE DRM_IOWR(DRM_COMMAND_BASE +	\
			DRM_LOMBO_SCALE_CLOSE, drm_lombo_scale_user_handle_t)
#define DRM_IOCTL_LOMBO_SCALE_SET_TIMEOUT DRM_IOW(DRM_COMMAND_BASE +	\
			DRM_LOMBO_SCALE_SET_TIMEOUT, struct drm_lombo_scale_timeout)
#define DRM_IOCTL_LOMBO_SCALE_EXEC DRM_IOW(DRM_COMMAND_BASE +	\
			DRM_LOMBO_SCALE_EXEC, struct drm_lombo_scale_exec_arg)

int lombo_rot_get_version(int drm_fd, struct drm_lombo_rot_ver *ver);
int lombo_rot_open(int drm_fd, drm_lombo_rot_user_handle_t *handle);
int lombo_rot_exec(int drm_fd, struct drm_lombo_rot_exec_arg *exec_arg);
int lombo_rot_close(int drm_fd, drm_lombo_rot_user_handle_t *handle);
int lombo_rot_set_timeout(int drm_fd, struct drm_lombo_rot_timeout *timeout);

int lombo_scale_open(int drm_fd, drm_lombo_scale_user_handle_t *handle);
int lombo_scale_exec(int drm_fd, struct drm_lombo_scale_exec_arg *exec_arg);
int lombo_scale_close(int drm_fd, drm_lombo_scale_user_handle_t *handle);
int lombo_scale_set_timeout(int drm_fd, struct drm_lombo_scale_timeout *timeout);

#endif
