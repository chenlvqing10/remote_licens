#include <sys/ioctl.h>
#include "drm_display.h"
#include "lombo_malloc.h"

void create_video_bufs(int drm_fd, void *buffer, int *dbuf_fds, int buffer_size,
		int width, int height, unsigned int format)
{
	int ret;
	void *virtual_addr;
	unsigned int w = width;
	unsigned int h = height;
	unsigned int virtual_height;
	unsigned int bpp;
	unsigned int bo_handle = 0;
	unsigned int fb_handle = 0;
	size_t size;
	size_t pitch;
	uint32_t offsets[4] = { 0 };
	uint32_t pitches[4] = { 0 };
	uint32_t bo_handles[4] = { bo_handle };
	struct drm_gem_close arg;

	struct framebuffer *buf = (struct framebuffer *) buffer;

	for (int n = 0; n < buffer_size; ++n) {
		switch (format) { /* drm format bpp prop equal to v4l2 format */
		case DRM_FORMAT_NV12:
		case DRM_FORMAT_NV21:
		case DRM_FORMAT_NV16:
		case DRM_FORMAT_NV61:
		case DRM_FORMAT_YUV420:
		case DRM_FORMAT_YVU420:
			bpp = 8; /* 1byte per pixel */
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
			fprintf(stderr, "unsupported format 0x%08x\n", format);
			return;
		}

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

		pitch = w * ((bpp + 7) / 8);
		printf("%s %d, w = %d, h = %d, pitch = %d\n", __func__, __LINE__, w, h,
				pitch);
		size = pitch * virtual_height;

		/* alloc ion uncached buffer */
		virtual_addr = buf[0].planes[0].map = lombo_malloc_cached(size);
		dbuf_fds[n] = lombo_vir_to_fd(virtual_addr);

		printf("%s %d, virtual_addr = %p\n", __func__, __LINE__, virtual_addr);
		printf("%s %d, pipe->dbuf_fds[%d] = %d\n", __func__, __LINE__, n,
				dbuf_fds[n]);

		ret = drmPrimeFDToHandle(drm_fd, dbuf_fds[n], &bo_handle);

		/* just testing a limited # of formats to test single
		 * and multi-planar path.. would be nice to add more..
		 */
		switch (format) {
		case DRM_FORMAT_NV12:
		case DRM_FORMAT_NV21:
		case DRM_FORMAT_NV16:
		case DRM_FORMAT_NV61:
			buf[n].num_planes = 2;
			offsets[0] = 0;
			bo_handles[0] = bo_handle;
			pitches[0] = pitch;
			pitches[1] = pitches[0];
			offsets[1] = pitches[0] * h;
			bo_handles[1] = bo_handle;
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
		case DRM_FORMAT_XRGB8888:
		case DRM_FORMAT_ARGB8888:
			buf[n].num_planes = 1;
			offsets[0] = 0;
			bo_handles[0] = bo_handle;
			pitches[0] = pitch;
			break;
		}

		ret = drmModeAddFB2(drm_fd, w, h, format, bo_handles, pitches, offsets,
				&fb_handle, 0);
		if (ret) {
			printf("drmModeAddFB2 failed: %s\n", ERRSTR);
			return;
		}

		memset(&arg, 0, sizeof(arg));
		arg.handle = bo_handle;
		drmIoctl(drm_fd, DRM_IOCTL_GEM_CLOSE, &arg);

		buf[n].fd = drm_fd;
		buf[n].format = format;
		buf[n].width = w;
		buf[n].height = h;
		buf[n].fb_id = fb_handle;
		buf[n].size = size;
		buf[n].virtual_addr = virtual_addr;
		buf[n].planes[0].stride = pitch;
	}
}

void destroy_video_bufs(void *buffer, int buffer_size)
{
	struct framebuffer *buf = (struct framebuffer *) buffer;
	int n = 0;

	for (n = 0; n < buffer_size; ++n) {
		struct framebuffer *fb = &buf[n];

		/* delete video framebuffer */
		drmModeRmFB(fb->fd, fb->fb_id);
		lombo_free_cached(fb->virtual_addr);

		memset(fb, 0, sizeof(*fb));
	}
}

void cut_buffer(void *src, void *dest)
{
	struct framebuffer *src_buf = (struct framebuffer *) src;
	struct framebuffer *dest_buf = (struct framebuffer *) dest;

	switch (src_buf->format) {
	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV21:
	case DRM_FORMAT_YUV420:
	case DRM_FORMAT_YVU420:
		memcpy(dest_buf->virtual_addr, src_buf->virtual_addr,
				dest_buf->width * dest_buf->height);

		memcpy(dest_buf->virtual_addr + dest_buf->width * dest_buf->height,
				src_buf->virtual_addr + src_buf->width * src_buf->height,
				dest_buf->width * dest_buf->height / 2);

		lombo_cache_flush_vir(dest_buf->virtual_addr);
		break;

	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV61:
		memcpy(dest_buf->virtual_addr, src_buf->virtual_addr,
				dest_buf->width * dest_buf->height);

		memcpy(dest_buf->virtual_addr + dest_buf->width * dest_buf->height,
				src_buf->virtual_addr + src_buf->width * src_buf->height,
				dest_buf->width * dest_buf->height);

		lombo_cache_flush_vir(dest_buf->virtual_addr);
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
		memcpy(dest_buf->virtual_addr, src_buf->virtual_addr,
				dest_buf->width * dest_buf->height * 2);

		lombo_cache_flush_vir(dest_buf->virtual_addr);
		break;

	case DRM_FORMAT_BGR888:
	case DRM_FORMAT_RGB888:
		memcpy(dest_buf->virtual_addr, src_buf->virtual_addr,
				dest_buf->width * dest_buf->height * 3);

		lombo_cache_flush_vir(dest_buf->virtual_addr);
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
		memcpy(dest_buf->virtual_addr, src_buf->virtual_addr,
				dest_buf->width * dest_buf->height * 4);

		lombo_cache_flush_vir(dest_buf->virtual_addr);
		break;
	default:
		memcpy(dest_buf->virtual_addr, src_buf->virtual_addr,
				dest_buf->width * dest_buf->height);

		lombo_cache_flush_vir(dest_buf->virtual_addr);
		break;
	}
}
