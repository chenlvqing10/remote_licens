#define LOG_TAG		"libdisplay"

#include "drm_display.h"

int lombo_scale_open(int drm_fd, drm_lombo_scale_user_handle_t *handle)
{
	ASSERT(handle != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_SCALE_OPEN, handle);
}

int lombo_scale_exec(int drm_fd, struct drm_lombo_scale_exec_arg *exec_arg)
{
	ASSERT(exec_arg != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_SCALE_EXEC, exec_arg);
}

int lombo_scale_close(int drm_fd, drm_lombo_scale_user_handle_t *handle)
{
	ASSERT(handle != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_SCALE_CLOSE, handle);
}

int lombo_scale_set_timeout(int drm_fd,
	struct drm_lombo_scale_timeout *timeout)
{
	ASSERT(timeout != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_SCALE_SET_TIMEOUT, timeout);
}
