#ifndef _V4L2_CAMERA_H_
#define _V4L2_CAMERA_H_

#include <linux/videodev2.h>

#define MAX_WINS_COUNT	8
#define MAX_FRS_COUNT	4

#define VIDIOC_VISS_GET_ISP_DEV \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 26, int)
#define VIDIOC_VISS_GET_SENSOR_DEV \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 27, int)

typedef struct frame_fmt {
	unsigned int format;
	unsigned int width;
	unsigned int height;
	unsigned int frame_rate;
} frame_fmt_t;

typedef struct frame_mode {
	int num;
	frame_fmt_t format[MAX_WINS_COUNT * MAX_FRS_COUNT];
} frame_mode_t;

int v4l2_open_dev(const char *node, int flag);
int v4l2_query_cap(int fd, struct v4l2_capability *caps);
int v4l2_rqbufs_bufs(int fd, struct v4l2_requestbuffers *rqbufs);
int v4l2_get_format(int fd, struct v4l2_format *fmt);
int v4l2_set_format(int fd, struct v4l2_format *fmt);
int v4l2_stream_on(int fd, enum v4l2_buf_type *type);
int v4l2_stream_off(int fd, enum v4l2_buf_type *type);
int v4l2_queue_buffer(int fd, struct v4l2_buffer *buf);
int v4l2_dequeue_buffer(int fd, struct v4l2_buffer *buf, int *buf_index);
int v4l2_exit_dev(int fd);
int v4l2_get_control(int fd, unsigned int id, int *value);
int v4l2_set_control(int fd, unsigned int id, int *value);
int v4l2_enum_frame(int fd, frame_mode_t *info);
int v4l2_get_cur_framerate(int fd);
int v4l2_set_framerate(int fd, int frame_rate);
int v4l2_get_isp_dev(int fd);
int v4l2_get_sensor_dev(int fd);

#endif
