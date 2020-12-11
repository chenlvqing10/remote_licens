#ifndef __MEDIA__H__
#define __MEDIA__H__

#ifdef __cplusplus
extern "C" {
#endif

#define CAM_BUF_QUEUE_SIZE		8
#define CAM_FMT_NUM_PLANES		1
#define CAM_IMAGE_FORMAT		"NV12"
/* set camera resolution */
#define CAM0_IMAGE_SIZE_WIDTH		1920
#define CAM0_IMAGE_SIZE_HEIGHT		1080

#define CAM1_IMAGE_SIZE_WIDTH		800
#define CAM1_IMAGE_SIZE_HEIGHT		600

struct flip_data {
	uint32_t plane_id;
	struct framebuffer plane_buf;
	int w, h;
};

struct cam_vid_pipe {
	uint32_t input_width, input_height;
	uint32_t output_x, output_y;
	uint32_t output_width, output_height;
	uint32_t format;

	int v4l2_fd;
	int enable_rot;
	int tr_mode;
	drm_lombo_rot_user_handle_t rot_handle;
	struct drm_lombo_rot_exec_arg exec_arg;

	uint32_t plane_id;
	int32_t plane_idx;

	int bufs_num;

	/* Used to save video data obtained from the camera */
	struct framebuffer video_bufs[CAM_BUF_QUEUE_SIZE];
	int video_dbuf_fds[CAM_BUF_QUEUE_SIZE];

	/* Used to save rotated data */
	struct framebuffer rot_bufs[CAM_BUF_QUEUE_SIZE];
	int rot_dbuf_fds[CAM_BUF_QUEUE_SIZE];

	/* Used to save 32-bit aligned video data */
	struct framebuffer cut_bufs[CAM_BUF_QUEUE_SIZE];
	int cut_dbuf_fds[CAM_BUF_QUEUE_SIZE];

	uint32_t b1, b2, b3;
	uint32_t *b;

	uint32_t cam_width;
	uint32_t cam_height;
};

void init_drm(int *drm_fd);

void uninit_drm(int drm_fd);

void queue_buffer(struct cam_vid_pipe *pipe, int index);

int dequeue_buffer(struct cam_vid_pipe *pipe);

void stream_on_camera(struct cam_vid_pipe *pipe);

void stream_off_camera(struct cam_vid_pipe *pipe);

void init_capture(struct cam_vid_pipe *pipe, const char *viddev);

void uninit_capture(struct cam_vid_pipe *pipe);

#ifdef __cplusplus
}
#endif

#endif
