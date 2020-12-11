#include <poll.h>
#include <sys/ioctl.h>
#include "drm_display.h"
#include "media.h"
#include <getopt.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include "format.h"
#include "buffer.h"
#include "time.h"

#define ISP_TEST_ENABLE 0
#if ISP_TEST_ENABLE
#include "isp_api.h"

int isp_fd;
#endif

#if 0
#define DEBUG_CAM0		1
#endif

#define FORMAT_DEBUG	0

#define NIR_PREVIEW 1

#define ALIGN(value, alignment) ((value) % (alignment) ? (((value) - 1) & ~(alignment - 1)) : (value))

static struct modeset_out *modeset_list;

static struct {
	int cam0_yuv_fd;
	int cam0_cnt;
	int drm_fd;
	int default_primary_buf_id;
	int running_flag;
	int camera_restart_times;

	bool dual_camera;
	uint32_t fourcc;
	uint32_t image_mode; /*0:stretch 1:trim*/
	uint32_t show_reduced_multiple;

	uint32_t crtc_id;
	uint32_t primary_buf_id;
	uint32_t crtc_width;
	uint32_t crtc_height;

	struct framebuffer primary_bufs;
	int primary_dmafds; /* just for primay plane */

	struct cam_vid_pipe pipes[2];

	pthread_t ui1_treadid;
	pthread_t ui2_treadid;


	char *input_device[2];
} global;

static int find_crtc(int fd)
{
	int ret = -1;
	drmModeRes *res = drmModeGetResources(fd);

	ASSERT(res);

	for (int i = 0; i < res->count_crtcs; ++i) {
		uint32_t crtc_id = res->crtcs[i];

		drmModeCrtc *crtc = drmModeGetCrtc(fd, crtc_id);
		ASSERT(crtc);

		if (!crtc->mode_valid) {
			drmModeFreeCrtc(crtc);
			continue;
		}

		global.crtc_id = crtc->crtc_id;
		global.crtc_width = crtc->width;
		global.crtc_height = crtc->height;
		global.primary_buf_id = crtc->buffer_id;

		drmModeFreeCrtc(crtc);
		drmModeFreeResources(res);
		ret = 0;
		goto END;
	}
END:
	return ret;
}

static void init_mode_width_height()
{
	for_each_output(out, modeset_list) {
		fprintf(stderr, "Output %u: Connector %u, Encoder %u, CRTC %u,",
				out->output_id,
				out->conn_id, out->enc_id, out->crtc_id);
		fprintf(stderr, " Mode %ux%u\n",
				out->mode.hdisplay, out->mode.vdisplay);
					global.crtc_id = out->crtc_id;

		global.crtc_width = out->mode.hdisplay;
		global.crtc_height = out->mode.vdisplay;
	}
}

static void find_mode(int fd)
{
	/* Prepare all connectors and CRTCs */
	modeset_prepare(fd, &modeset_list);
}

static void set_crtc_mode(uint32_t fb_id)
{
	int r;
	for_each_output(out, modeset_list) {
		fprintf(stderr, "Output %u: Connector %u, Encoder %u, CRTC %u,",
				out->output_id,
				out->conn_id, out->enc_id, out->crtc_id);
		fprintf(stderr, " FB %u/%u, Mode %ux%u\n",
				fb_id, fb_id,
				out->mode.hdisplay, out->mode.vdisplay);

		r = drmModeSetCrtc(out->fd, out->crtc_id, fb_id, 0, 0,
				     &out->conn_id, 1, &out->mode);
		ASSERT(r == 0);

		global.primary_buf_id = fb_id;
	}
}

static void create_primary_bufs2()
{
	/* Allocate private data */
	for_each_output(out, modeset_list)
		out->data = calloc(1, sizeof(struct flip_data));

	for_each_output(out, modeset_list) {
		struct flip_data *pdata = out->data;
		int dmabuf_fd = 0;

		create_video_bufs(out->fd, &pdata->plane_buf, &dmabuf_fd, 1,
				out->mode.hdisplay, out->mode.vdisplay, DRM_FORMAT_XRGB8888);

		drm_draw_test_pattern(&pdata->plane_buf, 0);

		pdata->w = out->mode.hdisplay;
		pdata->h = out->mode.vdisplay;

		/* sync info for global var */
		global.primary_bufs.fd = global.drm_fd;
		global.primary_bufs.format = DRM_FORMAT_XRGB8888;
		global.primary_bufs.width = pdata->w;
		global.primary_bufs.height = pdata->h;
		global.primary_bufs.fb_id = pdata->plane_buf.fb_id;
		global.primary_bufs.virtual_addr = pdata->plane_buf.virtual_addr;
	}
}

/*
 * disable the planes above the current plane*/
void disable_planes_above_cur(int fd, uint32_t current_plane_id)
{
	drmModePlaneRes *res = drmModeGetPlaneResources(fd);
	ASSERT(res);

	for (int i = 0; i < res->count_planes; i++) {
		uint32_t plane_id = res->planes[i];
		if (plane_id > current_plane_id) {
			drmModeSetPlane(fd, plane_id, global.crtc_id,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		}
	}
	drmModeFreePlaneResources(res);
}

static void init_camera_video_pipe(struct cam_vid_pipe *pipe,
				   const char *viddev,
				   uint32_t iw,
				   uint32_t ih,
				   uint32_t ox,
				   uint32_t oy,
				   uint32_t ow,
				   uint32_t oh)
{
	int32_t index = 0;

	pipe->input_width = iw;
	pipe->input_height = ih;
	printf("%s 32bytes align, pipe->input_width:%d, pipe->input_height:%d\n",
				__func__, pipe->input_width, pipe->input_height);

	pipe->output_x = ox;
	pipe->output_y = oy;
	pipe->output_width = ow;
	pipe->output_height = oh;

	init_capture(pipe, viddev);
	printf("%s pipe->v4l2_fd:%d, enable_rot:%d\n", __func__,
						pipe->v4l2_fd, pipe->enable_rot);

#if ISP_TEST_ENABLE
	isp_fd = get_isp_dev(pipe->v4l2_fd);
	struct v4l2_rect rect = {100, 100, 100, 100};
	struct viss_isp_exif exif;
	int ret = get_rgb_sensor_exif(isp_fd, &exif);

	if (ret == 0)
		printf("%s %d get_rgb_sensor_exif success\n", __func__, __LINE__);
	else
		printf("%s %d get_rgb_sensor_exif fail\n", __func__, __LINE__);

	ret = set_ae_para(isp_fd, &rect, 1);
	if (ret == 0)
		printf("%s %d set ae success\n", __func__, __LINE__);
	else
		printf("%s %d set ae fail\n", __func__, __LINE__);
#endif

	/* ion buf for dmabuf sharing */
	create_video_bufs(global.drm_fd, pipe->video_bufs, pipe->video_dbuf_fds,
			pipe->bufs_num, pipe->input_width, pipe->input_height,
			global.fourcc);

	if (pipe->enable_rot == 1) {
		if (lombo_rot_open(global.drm_fd, &pipe->rot_handle)) {
			printf("failed to open rot device\n");
			return;
		}

		int algin_w = ALIGN(pipe->input_width, 32);
		int algin_h = ALIGN(pipe->input_height, 32);

		if ((pipe->tr_mode == LOMBO_DRM_TRANSFORM_ROT_90)
				|| (pipe->tr_mode == LOMBO_DRM_TRANSFORM_ROT_270)
				|| (pipe->tr_mode == LOMBO_DRM_TRANSFORM_FLIP_H_ROT_90)
				|| (pipe->tr_mode == LOMBO_DRM_TRANSFORM_FLIP_V_ROT_90))
			create_video_bufs(global.drm_fd, pipe->rot_bufs, pipe->rot_dbuf_fds,
					pipe->bufs_num, algin_h, algin_w,
					global.fourcc);
		else
			create_video_bufs(global.drm_fd, pipe->rot_bufs, pipe->rot_dbuf_fds,
					pipe->bufs_num, algin_w, algin_h,
					global.fourcc);

		/* if need 32-bit alignment, add clipping buffers to crop */
		create_video_bufs(global.drm_fd, pipe->cut_bufs, pipe->cut_dbuf_fds,
				pipe->bufs_num, algin_w, algin_h, global.fourcc);
	}

	/* get a video plane */
	index = drm_reserve_plane_index(global.drm_fd);
	pipe->plane_idx = index;
	pipe->plane_id = drm_plane_index_to_id(index);
	disable_planes_above_cur(global.drm_fd, pipe->plane_id);

	printf("%s %d pipe->plane_id:%d\n", __func__, __LINE__, pipe->plane_id);

	for (int i = 0; i < pipe->bufs_num; ++i)
		queue_buffer(pipe, i);

	stream_on_camera(pipe);
}

static void free_camera_video_pipe(struct cam_vid_pipe *pipe)
{
	if (pipe->enable_rot == 1) {
		if (lombo_rot_close(global.drm_fd, &pipe->rot_handle)) {
			printf("failed to close rot device\n");
			return;
		}
	}

	stream_off_camera(pipe);
	drm_release_plane_index(pipe->plane_idx);

	if (pipe->enable_rot == 1) {
		destroy_video_bufs(pipe->rot_bufs, pipe->bufs_num);
		destroy_video_bufs(pipe->cut_bufs, pipe->bufs_num);
	}

	destroy_video_bufs(pipe->video_bufs, pipe->bufs_num);

#if ISP_TEST_ENABLE
	close(isp_fd);
#endif

	uninit_capture(pipe);
}

static void init_display_engine()
{
	/**
	  * enable fbdev defaultly,
	  * otherwise will get not_found_crtc_mode problem.
	  * or set_crtc_mode will be ok.
	  */
	global.default_primary_buf_id = -1;

	if (find_crtc(global.drm_fd) == 0)
		global.default_primary_buf_id = global.primary_buf_id;

	printf("%s %d global.default_primary_buf_id is %d\n",
			__func__, __LINE__, global.default_primary_buf_id);

	find_mode(global.drm_fd);

	if (global.default_primary_buf_id <= 0) {

		init_mode_width_height();
		/* primary plane bufs used in crtc initial */
		create_primary_bufs2();

		/* set up crtc, connector, mode, fb's association */
		set_crtc_mode(global.primary_bufs.fb_id);
	}
}

static void uninit_display_engine()
{
	if (global.default_primary_buf_id <= 0) {
		close(global.primary_dmafds);

		destroy_video_bufs(&global.primary_bufs, 1);

		memset(&global.primary_bufs, 0, sizeof(global.primary_bufs));
	}
}

static void process_pipe_init(struct cam_vid_pipe *pipe)
{
	pipe->b1 = pipe->b2 = pipe->b3 = -1;
}

static void image_trim(const uint32_t out_w,
		const uint32_t out_h,
		uint32_t *src_x,
		uint32_t *src_y,
		uint32_t *src_w,
		uint32_t *src_h)
{
	if (*src_w > out_w) {
		*src_x += (*src_w - out_w) / 2;
		*src_w = out_w;
	}

	if (*src_h > out_h) {
		*src_y += (*src_h - out_h) / 2;
		*src_h = out_h;
	}
}

static void process_pipe(struct cam_vid_pipe *pipe)
{
	uint32_t src_x, src_y, src_w, src_h;

	if (*pipe->b != -1) {
		queue_buffer(pipe, *pipe->b);
		*pipe->b = -1;
	}

	int b = dequeue_buffer(pipe);
	if (b == -1)
		return;

	pipe->b3 = pipe->b2;
	pipe->b2 = pipe->b1;
	pipe->b1 = b;

	struct framebuffer *video_buf = &pipe->video_bufs[pipe->b1];
	int r;

#ifdef DEBUG_CAM0
	if (global.cam0_cnt < 60) {
		/**
		  * filesystem sync method
		  * mount /dev/mmcblk0p1 /mnt
		  */
		if (global.cam0_cnt == 51) {
			write(global.cam0_yuv_fd, video_buf->virtual_addr, video_buf->size);
			close(global.cam0_yuv_fd);
		}

		global.cam0_cnt++;
	}

#endif

	if (pipe->enable_rot == 1) {
		int algin_w = ALIGN(video_buf->width, 32);
		int algin_h = ALIGN(video_buf->height, 32);
		struct framebuffer *temp_buf;
		struct framebuffer *rot_buf = &pipe->rot_bufs[pipe->b1];

		if (algin_w != video_buf->width || algin_h != video_buf->height) {
			temp_buf = &pipe->cut_bufs[pipe->b1];

			cut_buffer(video_buf, temp_buf);
		} else {
			temp_buf = video_buf;
		}
		pipe->exec_arg.handle = pipe->rot_handle;
		pipe->exec_arg.in_fb = temp_buf->fb_id;
		pipe->exec_arg.out_fb = rot_buf->fb_id;
		pipe->exec_arg.transform = pipe->tr_mode;
		r = lombo_rot_exec(global.drm_fd, &pipe->exec_arg);
		if (r) {
			printf("%s,%d: lombo_rot_exec fail. r = %d\n", __func__, __LINE__, r);
			return;
		}

		src_x = 0;
		src_y = 0;
		src_w = rot_buf->width;
		src_h = rot_buf->height;

		if (global.image_mode == 1) {
			image_trim(pipe->output_width, pipe->output_height, &src_x, &src_y,
					&src_w, &src_h);
		}

		r = drmModeSetPlane(global.drm_fd,
				pipe->plane_id,
				global.crtc_id,
				rot_buf->fb_id,
				0,
				/* output */
				pipe->output_x,
				pipe->output_y,
				pipe->output_width,
				pipe->output_height,
				/* input */
				src_x << 16,
				src_y << 16,
				src_w << 16,
				src_h << 16);
		ASSERT(r == 0);
	} else {
		src_x = 0;
		src_y = 0;
		src_w = video_buf->width;
		src_h = video_buf->height;

		if (global.image_mode == 1) {
			image_trim(pipe->output_width, pipe->output_height,
					&src_x, &src_y, &src_w, &src_h);
		}

		r = drmModeSetPlane(global.drm_fd,
			    pipe->plane_id,
			    global.crtc_id,
			    video_buf->fb_id,
			    0,
			    /* output */
			    pipe->output_x,
			    pipe->output_y,
			    pipe->output_width,
			    pipe->output_height,
			    /* input */
			    src_x << 16,
			    src_y << 16,
				src_w << 16,
				src_h << 16);
		ASSERT(r == 0);
	}
}

static void show_help(void)
{
	const char *arg_format = "  %-20s %s\n";

	printf("usage: video_preview <-d device> [argument]\n");
	printf("arguments:\n");
	printf(arg_format, "-d <device> [-d <device>]", "select input device");
	printf(arg_format, "-n <bufs number>", "select video input buffer number, must be >= 3");
	printf(arg_format, "-r <horizontal_resolution vertical_resolution>", "video resolution");
	printf(arg_format, "-t <transform mode>", "0:none; 1:hflip; 2:vflip; 4:90rot; 3:180rot 7:270rot; 5:90hflip; 6:90vflip");
	printf(arg_format, "-f <format>", "select format , eg:NV12 or RG16 etc.");
	printf(arg_format, "-i <image mode>", "0:stretch 1:trim");
	printf(arg_format, "-s <multiples>", "Show reduced multiples");
	printf(arg_format, "-c <multiples>", "The number of times to restart the camera test");
	printf(arg_format, "-h,-H", "Show help");
	printf("\nexample: video_preview -d \"/dev/video0\" -d \"/dev/video1\"\n");
}

static void stop(int signo)
{
	printf("%s %d stop!!!! signo %d\n", __func__, __LINE__, signo);

	free_camera_video_pipe(&global.pipes[0]);
	if (global.dual_camera)
		free_camera_video_pipe(&global.pipes[1]);

	uninit_display_engine();
	uninit_drm(global.drm_fd);

	fprintf(stderr, "exiting\n");
}

static void init_global(void)
{
	memset(&global, 0, sizeof(global));

	global.fourcc = DRM_FORMAT_NV12;
	global.dual_camera = false;
	global.pipes[0].bufs_num = CAM_BUF_QUEUE_SIZE;
	global.pipes[1].bufs_num = CAM_BUF_QUEUE_SIZE;
	global.pipes[0].cam_width = CAM0_IMAGE_SIZE_WIDTH;
	global.pipes[0].cam_height = CAM0_IMAGE_SIZE_HEIGHT;
	global.pipes[1].cam_width = CAM1_IMAGE_SIZE_WIDTH;
	global.pipes[1].cam_height = CAM1_IMAGE_SIZE_HEIGHT;
	global.show_reduced_multiple = 1;
}

static int get_parameters(int argc, char **argv, char **input_device)
{
	int c;

	if (argc < 2) {
		printf("invalid arguments\n");
		goto help_exit;
	}

	while ((c = getopt(argc, argv, "hHd:n:t:r::f:i:s:c:")) != -1) {
		switch (c) {
		case 'd': {
			char *argvtemp = argv[optind - 1];
			if (!argvtemp) {
				printf("-d %s is invalid\n", argv[optind - 1]);
				goto help_exit;
			}

			if (!input_device[0]) {
				input_device[0] = argvtemp;
				printf("-d input_device 0 %s\n", input_device[0]);
			} else if (!input_device[1]) {
				input_device[1] = argvtemp;
				global.dual_camera = true;
				printf("-d input_device 1 %s\n", input_device[1]);
			}
			break;
		}
		case 'r': {
			global.pipes[0].cam_width = (int) strtol(argv[optind], NULL, 10);
			global.pipes[0].cam_height = (int) strtol(argv[optind + 1], NULL,
					10);
			break;
		}
		case 'n': {
			global.pipes[0].bufs_num = (int) strtol(argv[optind - 1], NULL, 10);
			global.pipes[1].bufs_num = global.pipes[0].bufs_num;
			break;
		}
		case 't': {
			global.pipes[0].tr_mode = (int) strtol(argv[optind - 1], NULL, 10);
			global.pipes[1].tr_mode = global.pipes[0].tr_mode;
			break;
		}
		case 'f': {
			char *format = argv[optind - 1];

			if (!format) {
				printf("-f %s is invalid\n", argv[optind - 1]);
				goto help_exit;
			}

			global.fourcc = util_format_fourcc(format);
			if (global.fourcc == 0) {
				printf("%s %d unknown format %s\n", __func__, __LINE__, format);
				global.fourcc = DRM_FORMAT_NV12;
			}
#if FORMAT_DEBUG
			printf("-f format  %s %d\n", format, global.fourcc);
			const struct util_format_info *info =
			util_format_info_find(global.fourcc);

			printf("%s %d format %s\n", __func__, __LINE__, info->name);
#endif
			break;
		}
		case 'i':
			global.image_mode = (int) strtol(argv[optind - 1], NULL, 10);
			printf("image_mode option '%d'.\n", global.image_mode);
			break;
		case 's':
			global.show_reduced_multiple = (int) strtol(argv[optind - 1], NULL,
					10);
			printf("show_reduced_multiple '%d'.\n",
					global.show_reduced_multiple);
			break;
		case 'c':
			global.camera_restart_times = (int) strtol(argv[optind - 1], NULL,
					10);
			break;
		case '?':
			printf("unknown option '%s'.\n", argv[optind - 1]);
			goto help_exit;
		default:
			printf("Unexcepted case, please let us know, option '%c'\n",
					optopt);
			goto help_exit;
		}
	}

	if (!input_device[0])
		goto help_exit;

	return 0;

help_exit: show_help();
	return -1;
}

static int start_video_preview()
{
	init_drm(&global.drm_fd);
	init_display_engine();

	/* camera pipe rot prop */
	if (global.pipes[0].bufs_num > CAM_BUF_QUEUE_SIZE) {
		global.pipes[0].bufs_num = CAM_BUF_QUEUE_SIZE;
		global.pipes[1].bufs_num = global.pipes[0].bufs_num;
	}

	if (global.pipes[0].bufs_num < 3) {
		global.pipes[0].bufs_num = 3;
		global.pipes[1].bufs_num = global.pipes[0].bufs_num;
	}
	global.pipes[1].bufs_num = global.pipes[0].bufs_num;
	if (global.pipes[0].tr_mode != 0) {
		global.pipes[0].enable_rot = 1;  /* 1080P */
		global.pipes[1].enable_rot = 1;

		if (global.pipes[0].cam_height >= CAM0_IMAGE_SIZE_HEIGHT)
			global.pipes[0].cam_height = CAM0_IMAGE_SIZE_HEIGHT;
	} else {
		global.pipes[0].enable_rot = 0;  /* 1080P */
		global.pipes[1].enable_rot = 0;  /* 800x600 */
	}

	if (global.pipes[0].bufs_num >= 5) {
		global.pipes[0].b = &global.pipes[0].b3;
		global.pipes[1].b = &global.pipes[1].b3;
	} else if (global.pipes[0].bufs_num == 4) {
		global.pipes[0].b = &global.pipes[0].b2;
		global.pipes[1].b = &global.pipes[1].b2;
	} else if (global.pipes[0].bufs_num <= 3) {
		global.pipes[0].b = &global.pipes[0].b1;
		global.pipes[1].b = &global.pipes[1].b1;
	}

#ifdef DEBUG_CAM0
	global.cam0_yuv_fd = open("/mnt/test_camera0.yuv", O_RDWR | O_CREAT, 0666);
	if (global.cam0_yuv_fd < 0) {
		printf("open /mnt/test_camera0.yuv failed!\n");
		return -1;
	}
#endif

	init_camera_video_pipe(&global.pipes[0],
				global.input_device[0],
				global.pipes[0].cam_width,
				global.pipes[0].cam_height,
				global.crtc_width - global.crtc_width / global.show_reduced_multiple,
				global.crtc_height - global.crtc_height / global.show_reduced_multiple,
				global.crtc_width / global.show_reduced_multiple,
				global.crtc_height / global.show_reduced_multiple);

	if (global.dual_camera) {
		init_camera_video_pipe(&global.pipes[1],
					global.input_device[1],
					global.pipes[1].cam_width,
					global.pipes[1].cam_height,
					25,
					25,
					global.crtc_width / 3,
					global.crtc_height / 3);
	}

/* just disable planescale test */
#if 0
	printf("%s %d  create ui1 thread.\n", __func__, __LINE__);
	pthread_create(&global.ui1_treadid, NULL, init_planescale, NULL);
	usleep(1000);
#endif

	/* just general plane2 test */
	/* init_plane2_test(); */

	process_pipe_init(&global.pipes[0]);
	if (global.dual_camera)
		process_pipe_init(&global.pipes[1]);

	struct pollfd fds[] = {
		{ .fd = global.pipes[0].v4l2_fd, .events =  POLLIN },
		{ .fd = global.dual_camera ? global.pipes[1].v4l2_fd : 0,
		  .events = POLLIN },
	};

	signal(SIGINT, stop);
	signal(SIGTERM, stop);

	while (global.running_flag) {
		int r = poll(fds, ARRAY_SIZE(fds), -1);
		ASSERT(r > 0);

		if (fds[0].revents & POLLIN)
			process_pipe(&global.pipes[0]);
#if NIR_PREVIEW
		if (fds[1].revents & POLLIN) {
			if (global.dual_camera)
				process_pipe(&global.pipes[1]);
		}
#endif
	}
	stop(-1);
	return 0;
}

void *preview_thread(void *arg)
{
	start_video_preview();
	return NULL;
}


int main(int argc, char **argv)
{
	printf("capture_test built: %s %s\n", __TIME__, __DATE__);

	init_global();

	if (get_parameters(argc, argv, global.input_device))
		return -1;

	global.running_flag = 1;

	pthread_t preview_task_t;
	pthread_create(&preview_task_t, NULL, preview_thread, NULL);

	if (global.camera_restart_times == 0) {
		while (true) {
			sleep(10);
		}
	} else {
		for (int i = 0; i < global.camera_restart_times; i++) {
			sleep(3);
			global.running_flag = 0;
			pthread_join(preview_task_t, NULL);
			sleep(3);
			global.running_flag = 1;
			pthread_create(&preview_task_t, NULL, preview_thread, NULL);
		}
	}

	return 0;
}
