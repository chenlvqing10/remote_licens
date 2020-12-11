#ifndef _LOMBO_SCALE_H_
#define _LOMBO_SCALE_H_

struct drm_device;
struct drm_file;

int lombo_scale_open_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

int lombo_scale_close_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

int lombo_scale_set_timeout_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

int lombo_scale_exec_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

int lombo_scale_module_init(void);

#endif /* _LOMBO_SCALE_H_ */

