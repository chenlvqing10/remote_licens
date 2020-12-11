/*
 * drm_ops.c - main interface of camera formation.
 *
 * Copyright (C) 2016 - 2019, LomboTech Co.Ltd.
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

#include "drm_ops.h"

static void open_drm(int *drm_fd)
{
	const char *card = "/dev/dri/card0";

	*drm_fd = drm_open_dev_dumb(card);
}

static int find_crtc(struct drm_data *drm_dat)
{
	int ret = -1;
	drmModeRes *res = drmModeGetResources(drm_dat->drm_fd);
	ASSERT(res);

	for (int i = 0; i < res->count_crtcs; ++i) {
		unsigned int crtc_id = res->crtcs[i];

		drmModeCrtc *crtc = drmModeGetCrtc(drm_dat->drm_fd, crtc_id);
		ASSERT(crtc);

		if (!crtc->mode_valid) {
			drmModeFreeCrtc(crtc);
			continue;
		}

		drm_dat->crtc_id = crtc->crtc_id;
		drm_dat->crtc_width = crtc->width;
		drm_dat->crtc_height = crtc->height;
		drm_dat->primary_buf_id = crtc->buffer_id;
		printf("crtc->crtc_id: %d\n"
			"crtc->width: %d\n"
			"crtc->height: %d\n"
			"crtc->buffer_id: %d\n",
			crtc->crtc_id,
			crtc->width,
			crtc->height,
			crtc->buffer_id);
		printf("%s %d find crtc success\n", __func__, __LINE__);

		drmModeFreeCrtc(crtc);
		drmModeFreeResources(res);
		ret = 0;
		goto END;
	}
END:
	return ret;
}
static void disable_planes_above_cur(int fd,
		unsigned int crtc_id, unsigned int current_plane_id)
{
	drmModePlaneRes *res = drmModeGetPlaneResources(fd);
	ASSERT(res);

	for (int i = 0; i < res->count_planes; i++) {
		uint32_t plane_id = res->planes[i];
		if (plane_id > current_plane_id) {
			drmModeSetPlane(fd, plane_id, crtc_id,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		}
	}
	drmModeFreePlaneResources(res);
}

void create_video_dma_bufs(struct drm_data *drm_dat,
			struct camera_data *cam_data,
			void *map, unsigned int size,
			int fd, int count)
{
	int ret = 0;

	unsigned int cou = count;
	unsigned int w = cam_data->cur_width;
	unsigned int h = cam_data->cur_height;
	unsigned int bo_handle = 0;
	unsigned int fb_handle = 0;
	unsigned int offsets[4] = { 0 };
	unsigned int pitches[4] = { 0 };
	unsigned int bo_handles[4] = { bo_handle };
	struct drm_gem_close arg;
	struct framebuffer *fbuf = (struct framebuffer *)drm_dat->video_bufs;

	drm_dat->video_dbuf_fds[cou] = fd;
	drm_dat->video_bufs[0].planes[0].map = map;

	ret = drmPrimeFDToHandle(drm_dat->drm_fd,
		drm_dat->video_dbuf_fds[cou], &bo_handle);
	if (ret < 0)
		printf("drmPrimeFDToHandle %d\n", ret);

	fbuf[cou].num_planes = 1;
	offsets[0] = 0;
	bo_handles[0] = bo_handle;
	pitches[0] = w;
	pitches[1] = pitches[0];
	offsets[1] = pitches[0] * h;
	bo_handles[1] = bo_handle;

	ret = drmModeAddFB2(drm_dat->drm_fd,
			w, h,
			DRM_FORMAT_NV12,
			bo_handles,
			pitches,
			offsets,
			&fb_handle, 0);
	if (ret < 0)
		printf("drmModeAddFB2 %d\n", ret);

	memset(&arg, 0, sizeof(arg));
	arg.handle = bo_handle;
	ret = drmIoctl(drm_dat->drm_fd, DRM_IOCTL_GEM_CLOSE, &arg);
	if (ret < 0)
		printf("DRM_IOCTL_GEM_CLOSE %d\n", ret);

	fbuf[cou].fd = drm_dat->drm_fd;
	fbuf[cou].format = DRM_FORMAT_NV12;
	fbuf[cou].width = w;
	fbuf[cou].height = h;
	fbuf[cou].fb_id = fb_handle;
	fbuf[cou].size = size;
	fbuf[cou].virtual_addr = map;
	fbuf[cou].planes[0].stride = w;

}
void create_video_map_bufs(struct drm_data *drm_dat,
			struct camera_data *cam_data,
			int count)
{
	int ret = 0;
	void *drm_mmap;
	struct drm_mode_create_dumb create;
	struct drm_mode_map_dumb dmap;
	unsigned int cou = count;
	unsigned int w = cam_data->cur_width;
	unsigned int h = cam_data->cur_height;
	unsigned int bo_handle = 0;
	unsigned int fb_handle = 0;
	unsigned int offsets[4] = { 0 };
	unsigned int pitches[4] = { 0 };
	unsigned int bo_handles[4] = { bo_handle };
	struct drm_gem_close arg;
	struct framebuffer *fbuf = (struct framebuffer *)drm_dat->video_bufs;

	memset(&create, 0, sizeof(create));
	memset(&dmap, 0, sizeof(dmap));

	create.width = w;
	create.height = h * 3 / 2;
	create.bpp = 8;
	printf("w:%d, h:%d\n", w, h);
	ret = drmIoctl(drm_dat->drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);
	if (ret)
		printf("DRM_IOCTL_MODE_CREATE_DUMB %d\n", ret);

	printf("create size: %llu\n", create.size);
	dmap.handle = create.handle;
	ret = drmIoctl(drm_dat->drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &dmap);
	if (ret)
		printf("DRM_IOCTL_MODE_MAP_DUMB %d\n", ret);

	drm_mmap = mmap(NULL, create.size, PROT_READ | PROT_WRITE, MAP_SHARED,
				drm_dat->drm_fd, dmap.offset);

	if (drm_mmap == MAP_FAILED)
		printf ("mmap failed %d\n", cou);
	memset(drm_mmap, 0xff,create.size);

	drm_dat->video_bufs[0].planes[0].map = drm_mmap;
	fbuf[cou].num_planes = 2;
	offsets[0] = 0;
	bo_handles[0] = create.handle;
	pitches[0] = w;
	pitches[1] = pitches[0];
	offsets[1] = pitches[0] * h;
	bo_handles[1] = create.handle;
	printf("create.handle %d\n", create.handle);
	printf("create.pitch %d\n", create.pitch);

	ret = drmModeAddFB2(drm_dat->drm_fd,
			w, h,
			DRM_FORMAT_NV12,
			bo_handles,
			pitches,
			offsets,
			&fb_handle, 0);
	if (ret)
		printf("drmModeAddFB2 %d\n", ret);

	memset(&arg, 0, sizeof(arg));
	arg.handle = create.handle;
	ret = drmIoctl(drm_dat->drm_fd, DRM_IOCTL_GEM_CLOSE, &arg);
	if (ret)
		printf("DRM_IOCTL_GEM_CLOSE %d\n", ret);

	fbuf[cou].fd = drm_dat->drm_fd;
	fbuf[cou].format = DRM_FORMAT_NV12;
	fbuf[cou].width = w;
	fbuf[cou].height = h;
	fbuf[cou].fb_id = fb_handle;
	fbuf[cou].size = create.size;
	fbuf[cou].virtual_addr = drm_mmap;
	fbuf[cou].planes[0].stride = w;
}
static void destroy_video_bufs(void *buffer, int buffer_size)
{
	struct framebuffer *buf = (struct framebuffer *) buffer;
	int n = 0;

	for (n = 0; n < buffer_size; ++n) {
		struct framebuffer *fb = &buf[n];

		/* delete video framebuffer */
		drmModeRmFB(fb->fd, fb->fb_id);

		memset(fb, 0, sizeof(*fb));
	}
}

int drm_plane_to_id(struct drm_data *drm_dat)
{
	int ret = 0;
	int index;

	index = drm_reserve_plane_index(drm_dat->drm_fd);
	drm_dat->plane_idx = index;
	drm_dat->plane_id = drm_plane_index_to_id(index);
	disable_planes_above_cur(drm_dat->drm_fd,
		drm_dat->crtc_id, drm_dat->plane_id);
	printf("%s %d g_capture.plane_id:%d\n",
		__func__, __LINE__, drm_dat->plane_id);

	return ret;
}
void drm_set_plane(struct drm_data *drm_dat,
			struct camera_data *cam_data, int buf_index)
{
	int ret = 0;
	if (cam_data->cur_memory == V4L2_MEMORY_MMAP) {
		memcpy(drm_dat->video_bufs[buf_index].virtual_addr,
			cam_data->buffers[buf_index].start[0],
			drm_dat->video_bufs[buf_index].size);
	}

	ret = drmModeSetPlane(drm_dat->drm_fd,
				drm_dat->plane_id,
				drm_dat->crtc_id,
				drm_dat->video_bufs[buf_index].fb_id,
				0,
				/* output */
				0,
				0,
				drm_dat->crtc_width,
				drm_dat->crtc_height,
				/* input */
				0 << 16,
				0 << 16,
				cam_data->cur_width << 16,
				cam_data->cur_height << 16);
	ASSERT(ret == 0);
}
int drm_init(struct drm_data *drm_dat)
{
	int ret = 0;
	open_drm(&drm_dat->drm_fd);
	drm_dat->default_primary_buf_id = -1;

	if (find_crtc(drm_dat) == 0)
		drm_dat->default_primary_buf_id = drm_dat->primary_buf_id;

	printf("find crtc %d !\n", drm_dat->default_primary_buf_id);

	modeset_prepare(drm_dat->drm_fd, &drm_dat->modeset_list);

	printf("modeset_prepare\n");

	return ret;
}
int drm_exit(struct drm_data *drm_dat, struct camera_data *cam_data)
{
	int ret = 0;
	int i, j;

	for(j = 0; j < cam_data->buf_num; ++j) {
		for(i = 0; i < FMT_NUM_PLANES; i++) {
			if (cam_data->cur_memory == V4L2_MEMORY_MMAP) {
				ret = munmap(drm_dat->video_bufs[j].virtual_addr,
						drm_dat->video_bufs[j].size);
				if (ret < 0)
					printf("%s %d:munmap error\n", __func__, __LINE__);
			}
		}
	}

	drm_release_plane_index(drm_dat->plane_idx);
	destroy_video_bufs(drm_dat->video_bufs, cam_data->buf_num);
	close(drm_dat->drm_fd);

	return ret;
}