#define LOG_TAG		"libdisplay"

#include "drm_display.h"

int lombo_rot_get_version(int drm_fd,
	struct drm_lombo_rot_ver *ver)
{
	ASSERT(ver != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_ROT_GET_VER, ver);
}

int lombo_rot_open(int drm_fd,
	drm_lombo_rot_user_handle_t *handle)
{
	ASSERT(handle != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_ROT_OPEN, handle);
}

int lombo_rot_exec(int drm_fd,
	struct drm_lombo_rot_exec_arg *exec_arg)
{
	ASSERT(exec_arg != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_ROT_EXEC, exec_arg);
}

int lombo_rot_close(int drm_fd,
	drm_lombo_rot_user_handle_t *handle)
{
	ASSERT(handle != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_ROT_CLOSE, handle);
}

int lombo_rot_set_timeout(int drm_fd,
	struct drm_lombo_rot_timeout *timeout)
{
	ASSERT(timeout != NULL);
	return drmIoctl(drm_fd, DRM_IOCTL_LOMBO_ROT_SET_TIMEOUT, timeout);
}
