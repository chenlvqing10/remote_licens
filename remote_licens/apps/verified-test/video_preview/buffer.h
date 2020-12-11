#ifndef __BUFFER__H__
#define __BUFFER__H__

#ifdef __cplusplus
extern "C" {
#endif

void create_video_bufs(int drm_fd, void *buffer, int *dbuf_fds, int buffer_size,
		int width, int height, unsigned int format);

void destroy_video_bufs(void *buffer, int buffer_size);

void cut_buffer(void *src, void *dest);

#ifdef __cplusplus
}
#endif

#endif
