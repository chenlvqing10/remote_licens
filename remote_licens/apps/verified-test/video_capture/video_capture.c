/*
 * video_capture.c - main interface of video capture testcase.
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

#define _GNU_SOURCE
#include "formation.h"

#ifdef DISPLAY
#include "drm_ops.h"
#endif

static struct {
	struct camera_data cam_data;
	int fd_yuv;
	int f2d;
	unsigned int restart_flag;
	unsigned int run;

#ifdef DISPLAY
	struct drm_data drm_dat;
#endif

	struct timeval now_time;
	frame_mode_t frame_enum;
	char *video_name;
	char *vfile_name;
} g_capture;

int queue_buffer_plans(struct camera_data *cam_data, unsigned int buf_count);

void memory_free(void)
{
	int ret = 0;
	int i, j;
	enum v4l2_memory memory = g_capture.cam_data.cur_memory;

	for(j = 0; j < g_capture.cam_data.buf_num; ++j) {
		for(i = 0; i < FMT_NUM_PLANES; i++) {
			if (memory == V4L2_MEMORY_DMABUF)
				lombo_free_cached(g_capture.cam_data.buffers[j].start[i]);
			else if (memory == V4L2_MEMORY_MMAP) {
				ret = munmap(g_capture.cam_data.buffers[j].start[i],
						g_capture.cam_data.buffers[j].length[i]);
				if (ret < 0)
					printf("%s %d:munmap error\n", __func__, __LINE__);
			}
			else
				printf("%s %d:free memory error !!\n", __func__, __LINE__);
		}
	}
}

int enum_frame(char *input_device)
{
	int ret = 0;
	int i = 0;
	int v4l2_fd = -1;
	frame_mode_t info;

	CLEAR(info);

	v4l2_fd = v4l2_open_dev(input_device, O_RDWR | O_NONBLOCK);
	if (v4l2_fd < 0) {
		printf(" open %s ERR\n", input_device);
		return -1;
	}

	printf(" open %s sucess!!\n", input_device);

	ret = v4l2_enum_frame(v4l2_fd, &info);
	if (ret < 0) {
		printf("enum frame error !!\n");
		return -1;
	}

	for (i = 0; i < (info.num); i++) {
		printf("framesize: %d X %d,\tframerate:%d\n",
			info.format[i].width, info.format[i].height,
			(info.format[i].frame_rate) / 1000);
	}
	close (v4l2_fd);

	return 0;
}

static int read_frame_example(void)
{
	int ret = 0;
	struct timeval last_time;
	double time_used;
	double frame_rate;
	int buf_index = 0;

	buf_index = dequeue_buffer(&g_capture.cam_data);
	if (buf_index < 0) {
		printf("%s %d: dequeue_buffer error\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	assert (buf_index < g_capture.cam_data.buf_num);

#ifdef DISPLAY
	drm_set_plane(&g_capture.drm_dat, &g_capture.cam_data, buf_index);
#endif

	if (g_capture.f2d % 100 == 0) {
		last_time = g_capture.now_time;
		gettimeofday(&g_capture.now_time, NULL);
		if (g_capture.f2d != 0) {
			gettimeofday(&g_capture.now_time, NULL);
			time_used = g_capture.now_time.tv_sec - last_time.tv_sec;
			time_used += (double)(g_capture.now_time.tv_usec - last_time.tv_usec) / 1000000;
			frame_rate = 100 / time_used;
			printf("100 frame time_used %lf, frame_rate %lf\n",
				time_used, frame_rate);
		}
	}

	if (g_capture.f2d > 100 && g_capture.f2d <= (100 + g_capture.cam_data.save_frames)) {
		ret = write(g_capture.fd_yuv, g_capture.cam_data.buffers[buf_index].start[0],
				g_capture.cam_data.buffers[buf_index].length[0]);
		if (ret < 0) {
			printf("%s %d: write error\n", __FUNCTION__, __LINE__);
		}
		ret = 0;
	}
	if (g_capture.f2d == (100 + g_capture.cam_data.save_frames))
		printf("%s %d: write %d framerate success:%s\n", __FUNCTION__, __LINE__,
				g_capture.cam_data.save_frames, g_capture.vfile_name);
	g_capture.f2d++;

	ret = queue_buffer_plans(&g_capture.cam_data, buf_index);
	if (ret < 0) {
		printf("%s %d: queue_buffer_plans error!\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	return ret;
}

void dma_planes(struct camera_data *cam_data, struct v4l2_buffer *bf, int count)
{
	struct buffer *buf = cam_data->buffers;
	struct v4l2_buffer *v4_bufs = bf;
	int cou = count;
	int i = 0;

	if(cam_data->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
		for (i = 0; i < FMT_NUM_PLANES; i++) {
			buf[cou].length[i] = v4_bufs->m.planes[i].length;
			buf[cou].start[i] = lombo_malloc_cached(v4_bufs->m.planes[i].length);

			if (buf[cou].start[i] == NULL) {
				printf ("lombo_malloc failed %d\n", i);
			}
			else {
				memset(buf[cou].start[i], 0xff, v4_bufs->m.planes[i].length);
				buf[cou].fd[i] = lombo_vir_to_fd(buf[cou].start[i]);
			}

#ifdef DISPLAY
			create_video_dma_bufs(&g_capture.drm_dat,
				cam_data, buf[cou].start[i],
				v4_bufs->m.planes[i].length,
				buf[cou].fd[i], cou);
#endif
#if DEBUG_PRINT
			printf("n_buffers:%d, dma length:0x%x, start addr:0x%x, fd:%u\n",
				cou, buf[cou].length[i],
				(unsigned int)buf[cou].start[i],
				buf[cou].fd[i]);
#endif
		}
	} else {
			printf("length: %d\n", v4_bufs->length);
			buf[cou].length[i] = v4_bufs->length;
			buf[cou].start[i] = lombo_malloc_cached(v4_bufs->length);

			if (buf[cou].start[i] == NULL) {
				printf ("lombo_malloc failed %d\n", i);
			}
			else {
				memset(buf[cou].start[i], 0xff, v4_bufs->length);
				buf[cou].fd[i] = lombo_vir_to_fd(buf[cou].start[i]);
			}

#ifdef DISPLAY
			create_video_dma_bufs(&g_capture.drm_dat,
				cam_data, buf[cou].start[i],
				v4_bufs->m.planes[i].length,
				buf[cou].fd[i], cou);
#endif

#if DEBUG_PRINT
			printf("n_buffers:%d, dma length:0x%x, start addr:0x%x, fd:%u\n",
				cou, buf[cou].length[i],
				(unsigned int)buf[cou].start[i],
				buf[cou].fd[i]);
#endif
	}
}

void mmap_planes(struct camera_data *cam_data, struct v4l2_buffer *bf, int count)
{
	int v4l2_fd = cam_data->v4l2_fd;
	struct buffer *buf = cam_data->buffers;
	struct v4l2_buffer *v4_bufs = bf;
	int cou = count;
	int i = 0;

	if(cam_data->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
		for (i = 0; i < FMT_NUM_PLANES; i++) {
			buf[cou].length[i] = v4_bufs->m.planes[i].length;
			buf[cou].start[i] = mmap(NULL,	/* start anywhere */
						v4_bufs->m.planes[i].length,
						PROT_READ | PROT_WRITE,	/* required */
						MAP_SHARED,		/* recommended */
						v4l2_fd, v4_bufs->m.planes[i].m.mem_offset);

			if (MAP_FAILED == buf[cou].start[i])
				printf ("mmap failed %d\n", i);
			else
				memset(buf[cou].start[i], 0xff,
					v4_bufs->m.planes[i].length);

#ifdef DISPLAY
			create_video_map_bufs(&g_capture.drm_dat, cam_data, cou);
#endif

#if DEBUG_PRINT
			printf("n_buffers:%d, mmap length:0x%x, start addr:0x%x\n",
				i, buf[cou].length[i],
				(unsigned int)buf[cou].start[i]);
#endif
		}
	} else {
			buf[cou].length[i] = v4_bufs->length;
			buf[cou].start[i] = mmap(NULL,	/* start anywhere */
						v4_bufs->length,
						PROT_READ | PROT_WRITE,	/* required */
						MAP_SHARED,		/* recommended */
						v4l2_fd, v4_bufs->m.offset);

			if (MAP_FAILED == buf[cou].start[i])
				printf ("mmap failed %d\n", i);
			else
				memset(buf[cou].start[i], 0xff,
					v4_bufs->length);

#ifdef DISPLAY
			create_video_map_bufs(&g_capture.drm_dat, cam_data, cou);
#endif

#if DEBUG_PRINT
			printf("n_buffers:%d, mmap length:0x%x, start addr:0x%x\n",
				i, buf[cou].length[i],
				(unsigned int)buf[cou].start[i]);
#endif
	}
}

int queue_buffer_plans(struct camera_data *cam_data, unsigned int buf_count)
{
	int ret = 0;
	int i = 0;
	int v4l2_fd = cam_data->v4l2_fd;
	struct buffer *bufs = cam_data->buffers;
	enum v4l2_memory memory = cam_data->cur_memory;
	struct v4l2_buffer buf;
	struct v4l2_plane planes_qbuf[FMT_NUM_PLANES];
	int count = buf_count;

	CLEAR(buf);
	memset(planes_qbuf, 0, FMT_NUM_PLANES * sizeof(struct v4l2_plane));

	buf.type = cam_data->type;
	buf.memory = memory;
	buf.index = count;

	if(cam_data->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
		buf.m.planes = planes_qbuf;
		buf.length = FMT_NUM_PLANES;

		if(memory == V4L2_MEMORY_DMABUF)
			for (i = 0; i < FMT_NUM_PLANES; i++)
				planes_qbuf[i].m.fd = bufs[count].fd[i];
	} else {
		if(memory == V4L2_MEMORY_DMABUF)
			buf.m.fd = bufs[count].fd[i];
	}

	ret = v4l2_queue_buffer(v4l2_fd, &buf);
	if (ret < 0)
		return ret;

	return 0;
}

int memory_config(void)
{
	int i = 0;
	int idx = 0;
	unsigned int memory = g_capture.cam_data.cur_memory;
	int ret = 0;
	struct v4l2_buffer buf;
	struct v4l2_plane planes[FMT_NUM_PLANES];

	ret = request_buffers(&g_capture.cam_data);
	if (ret < 0)
		return ret;

	g_capture.cam_data.buffers = calloc(g_capture.cam_data.buf_num,
					sizeof(*g_capture.cam_data.buffers));
	if (g_capture.cam_data.buffers == NULL ) {
		printf("%s %d: calloc fail !!", __func__, __LINE__);
		return -1;
	}

	for(idx = 0; idx < g_capture.cam_data.buf_num; ++idx) {
		CLEAR (buf);
		memset(planes, 0 , FMT_NUM_PLANES * sizeof(struct v4l2_plane));

		buf.type = g_capture.cam_data.type;
		buf.index = idx;
		buf.memory = memory;

		if(g_capture.cam_data.type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
			buf.length = FMT_NUM_PLANES;
			buf.m.planes = planes;
		}

		ret = query_buffer(&g_capture.cam_data, &buf);
		if (ret < 0)
			return ret;

		if (memory == V4L2_MEMORY_DMABUF)
			dma_planes(&g_capture.cam_data, &buf, idx);
		else if (memory == V4L2_MEMORY_MMAP)
			mmap_planes(&g_capture.cam_data, &buf, idx);
		else
			printf("%s %d: %d memory error !!\n", __func__, __LINE__, idx);
	}

	for (i = 0; i < idx; ++i) {
		ret = queue_buffer_plans(&g_capture.cam_data, i);
		if (ret < 0)
			printf("%s %d:queur buffer error %d \n", __func__, __LINE__, i);
	}

	return 0;
}

static void show_help(void)
{
	const char *arg_format = "  %-20s %s\n";

	printf("usage: video_capture \n");
	printf("arguments:\n");
	printf(arg_format, "-d <device number>", "select input device");
	printf(arg_format, "-r <horizontal_resolution vertical_resolution>", "video resolution");
	printf(arg_format, "-n <bufs number>", "select video input buffer number, must be >= 3");
	printf(arg_format, "-m <memory number>", "select memory mode number, DMA:0   MMAP:1");
	printf(arg_format, "-f <framerate>", "select framerate, eg:60");
	printf(arg_format, "-e","enumerate device resolutions and frame rates,eg：video_capture -d 0 -e");
	printf(arg_format, "-s","save frames (Save 20 frames by default)");
	printf(arg_format, "-t","0:test mode, 1:repeatly restart test, Do not test by default");
	printf(arg_format, "-h,-H", "Show help");
	printf("\nexample: video_capture -d \"0\" -r \"640 480\" -n \"3\" -m \"0\"\n");
}

static void stop(int signo)
{
	printf("%s %d stop!!!! signal %d\n", __func__, __LINE__, signo);

	stream_off_camera(&g_capture.cam_data);

	fsync(g_capture.fd_yuv);

	close(g_capture.cam_data.v4l2_fd);

#ifdef DISPLAY
	drm_exit(&g_capture.drm_dat, &g_capture.cam_data);
#endif

	memory_free();

	exit(EXIT_SUCCESS);

	fprintf(stderr, "exiting\n");
}

static int get_parameters(int argc, char **argv, char *input_device)
{
	int c;
	int i;

	if (argc < 2) {
		printf("invalid arguments\n");
		goto help_exit;
	}

	while ((c = getopt(argc, argv, "ehHd:r:n:f:m:s:t:p:")) != -1) {
		switch (c) {
		case 'd': {
			g_capture.cam_data.vid_idx = (unsigned int)strtol(argv[optind - 1], NULL, 10);
			snprintf(input_device, 16, "%s%d", V4L2_DEV_NODE, g_capture.cam_data.vid_idx);
			break;
		}
		case 't': {
			g_capture.restart_flag = (unsigned int)strtol(argv[optind - 1], NULL, 10);
			printf("restart mode\n");
			break;
		}
		case 'r': {
			g_capture.cam_data.cur_width = (unsigned int)strtol(argv[optind - 1], NULL, 10);
			g_capture.cam_data.cur_height = (unsigned int)strtol(argv[optind], NULL, 10);
			break;
		}
		case 'n': {
			g_capture.cam_data.buf_num = (int) strtol(argv[optind - 1], NULL, 10);
			break;
		}
		case 'f': {
			g_capture.cam_data.framerate = (int) strtol(argv[optind - 1], NULL, 10);
			break;
		}
		case 's': {
			g_capture.cam_data.save_frames = (int) strtol(argv[optind - 1], NULL, 10);
			break;
		}
		case 'p': {
			char *fourcc = argv[optind - 1];
			i = set_fourcc(fourcc, &g_capture.cam_data.foc);
			if (i)
				goto help_exit;
			break;
		}
		case 'm': {
			i = (int) strtol(argv[optind - 1], NULL, 10);

			if (i == 0)
				g_capture.cam_data.cur_memory = V4L2_MEMORY_DMABUF;
			else if (i == 1)
				g_capture.cam_data.cur_memory = V4L2_MEMORY_MMAP;
			else
				goto help_exit;

			break;
		}
		case 'e': {
			enum_frame(input_device);
			goto help_exit;
		}
		default:
			printf("Unexcepted case, please let us know, option '%c'\n",
					optopt);
			goto help_exit;
		}
	}

	return 0;

help_exit: show_help();
	return -1;
}


int run_capture(void)
{
	int ret = 0;

#ifdef DISPLAY
	ret = drm_init(&g_capture.drm_dat);
	if (ret < 0) {
		printf(" init drm failed\n");
		return ret;
	}
#endif

	g_capture.cam_data.v4l2_fd = v4l2_open_dev(g_capture.video_name, O_RDWR | O_NONBLOCK);
	if (g_capture.cam_data.v4l2_fd < 0) {
		printf(" open %s ERR\n", g_capture.video_name);
		return -1;
	}

	printf(" open %s sucess!!\n", g_capture.video_name);

	sprintf(g_capture.vfile_name, "/data/test_%d.%s",
		g_capture.cam_data.vid_idx, g_capture.cam_data.foc.name);

	g_capture.fd_yuv = open(g_capture.vfile_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (g_capture.fd_yuv < 0) {
		printf("open %s failed!\n", g_capture.vfile_name);
		return -1;
	}

	printf(" open %s sucess!!\n", g_capture.vfile_name);

	ret = query_capability(&g_capture.cam_data);
	if (ret < 0) {
		printf(" query_capability failed\n");
		return -1;
	}

	ret = set_format(&g_capture.cam_data);
	if (ret < 0) {
		printf(" set_format failed\n");
		return -1;
	}

	set_framerate(&g_capture.cam_data);

	ret = memory_config();
	if (ret < 0) {
		printf(" memory_config failed\n");
		return -1;
	}

	printf(" memory config ok\n");

#ifdef DISPLAY
	ret = drm_plane_to_id(&g_capture.drm_dat);
	if (ret < 0) {
		printf(" drm_plane_to_id failed\n");
		return ret;
	}
#endif

#if DEBUG_PRINT
	printf(" VIDIOC_STREAMON begin \n");
#endif

	ret = stream_on_camera(&g_capture.cam_data);
	if(ret < 0)
		return ret;

	printf(" stream on camera ok\n");

	while (g_capture.run) {
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(g_capture.cam_data.v4l2_fd, &fds);

		/* Timeout. */
		tv.tv_sec = 10;
		tv.tv_usec = 0;

#if DEBUG_SELECT
		r = select(g_capture.cam_data.v4l2_fd + 1, &fds, NULL, NULL, &tv);
		if (-1 == r) {
			if (EINTR == errno) {
				printf("%s %d\n", __FUNCTION__, __LINE__);
				continue;
			}
			printf ("%s %d select err\n", __FUNCTION__, __LINE__);
			//break;
		}

		if (0 == r) {
			printf("%s %d  select timeout\n", __FUNCTION__, __LINE__);
			fprintf (stderr, "select timeout\n");
			//exit(EXIT_FAILURE);
			//break;
			continue;
		}
#endif
		if (read_frame_example() != 0)
			break;

	}

	fsync(g_capture.fd_yuv);
	printf("%s %d  close file\n", __FUNCTION__, __LINE__);
	stream_off_camera(&g_capture.cam_data);

#ifdef DISPLAY
	drm_exit(&g_capture.drm_dat, &g_capture.cam_data);
#endif

	memory_free();
	close (g_capture.cam_data.v4l2_fd);
	close(g_capture.fd_yuv);

	return 0;
}
void sigalrm_fn(int sig)
{
	printf("sigalrm_fn\n");
	g_capture.run = 0;
	alarm(5);
	return;
}
int main(int argc, char **argv)
{
	int ret;

	char video_name[16] = {0};
	char vfile_name[32] = {0};

	signal(SIGINT, stop);
	signal(SIGTERM, stop);
	signal(SIGALRM, sigalrm_fn);

	init_cap(&g_capture.cam_data);

	ret = get_parameters(argc, argv, video_name);
	if (ret < 0) {
		printf("get parameters invalid !\n");
		return -1;
	}
	g_capture.video_name = video_name;
	g_capture.vfile_name = vfile_name;
	g_capture.run = 1;

	if (g_capture.restart_flag == 0) {
		ret = run_capture();
		if (ret < 0)
			return ret;
	} else {
		alarm(5);
		while (1) {
			g_capture.run = 1;
			ret = run_capture();
			if (ret < 0)
				return ret;
		}
	}
	return 0;
}

