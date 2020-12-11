#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <linux/videodev2.h>

#include "isp_api.h"

#define VIDEO_NODE "/dev/video4"

#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define FMT_NUM_PLANES  1

struct buffer
{
	void *start[FMT_NUM_PLANES];
	size_t length[FMT_NUM_PLANES];
};

int yuv_fd;
int frame_width = 1280;
int frame_height = 720;

int frame_cnt = 0;
int frame_sum = 250;

char save_path[32] = "/mnt/fb.yuv";

int video_fd = -1;
int isp_fd = -1;
int rgb_fd = -1;

struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;

struct timeval now_time, last_time;

enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

static int isp_api_test(int argc, char *argv[])
{
	struct viss_isp_exif exif;
	struct v4l2_rect rect = {100, 100, 100, 100};
	int ret = 0;

	isp_fd = get_isp_dev(video_fd);
	rgb_fd = get_sensor_dev(video_fd);

	ret = get_rgb_sensor_exif(isp_fd, &exif);

	if (argc >= 7)
		ret = set_exp_bias(isp_fd, atoi(argv[6]));

	if (argc >= 8)
		ret = set_wb_mode(isp_fd, atoi(argv[7]));

	if (argc >= 9)
		ret = set_wdr_mode(isp_fd, atoi(argv[8]));

	if (argc >= 10)
		ret = set_ae_para(isp_fd, &rect, atoi(argv[9]));

	return ret;
}

static int read_frame(void)
{
	struct v4l2_buffer buf;
	struct v4l2_plane planes[FMT_NUM_PLANES];
	double time_used;
	double frame_rate;
	int ret = 0;

	CLEAR(buf);
	memset(planes, 0 , FMT_NUM_PLANES * sizeof(struct v4l2_plane));

	buf.type = type;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.m.planes = planes;
	buf.length = FMT_NUM_PLANES;

	ioctl(video_fd, VIDIOC_DQBUF, &buf);
	assert(buf.index < n_buffers);

	if (frame_cnt % 100 == 0) {
		last_time = now_time;
		gettimeofday(&now_time, NULL);
		if (frame_cnt != 0) {
			gettimeofday(&now_time, NULL);
			time_used = now_time.tv_sec - last_time.tv_sec;
			time_used += (double)(now_time.tv_usec - last_time.tv_usec) / 1000000;

			frame_rate = 100 / time_used;
			printf("100 frame time_used %lf, frame_rate %lf\n", time_used, frame_rate);
		}
	}
	if (frame_cnt == (frame_sum / 2)) {
		ret = write(yuv_fd, buffers[buf.index].start[0], buffers[buf.index].length[0]);
		if (ret < 0)
			printf("save frame%d, write error\n", frame_cnt);
		ret = 0;
	}
	frame_cnt++;
	ioctl(video_fd, VIDIOC_QBUF, &buf);

	return ret;
}

int main(int argc, char **argv)
{
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	char video_name[16] = {0};
	int i;

	if (argc == 1) {
		snprintf(video_name, 16, "%s", VIDEO_NODE);
	} else if (argc == 2) {
		snprintf(video_name, 16, "%s", argv[1]);
	} else if (argc == 4) {
		snprintf(video_name, 16, "%s", argv[1]);
		frame_width = atoi(argv[2]);
		frame_height = atoi(argv[3]);
	} else if (argc == 5) {
		snprintf(video_name, 16, "%s", argv[1]);
		frame_width = atoi(argv[2]);
		frame_height = atoi(argv[3]);
		frame_sum = atoi(argv[4]);
	} else if (argc == 6) {
		snprintf(video_name, 16, "%s", argv[1]);
		frame_width = atoi(argv[2]);
		frame_height = atoi(argv[3]);
		frame_sum = atoi(argv[4]);
	} else {
		snprintf(video_name, 16, "%s", argv[1]);
		frame_width = atoi(argv[2]);
		frame_height = atoi(argv[3]);
		frame_sum = atoi(argv[4]);
	}

	video_fd = open(video_name, O_RDWR | O_NONBLOCK);
	if (video_fd < 0) {
		printf("open %s error!!\n", video_name);
		return -1;
	} else {
		printf("open %s sucess!!\n", video_name);
	}

	isp_api_test(argc, argv);

	CLEAR (fmt);
	fmt.type = type;
	fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12;
	fmt.fmt.pix_mp.width = frame_width;
	fmt.fmt.pix_mp.height = frame_height;

	if ((ioctl(video_fd, VIDIOC_S_FMT, &fmt)) < 0) {
		printf("VIDIOC_S_FMT failed!\n");
		return -1;
	} else {
		printf("VIDIOC_S_FMT ok!\n");
	}
	printf("resoluton: %dx%d\n", fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height);

	req.count = 5;
	req.type = type;
	req.memory = V4L2_MEMORY_MMAP;
	if ((ioctl(video_fd, VIDIOC_REQBUFS, &req)) < 0) {
		printf("VIDIOC_REQBUFS failed!\n");
		return -1;
	} else {
		printf("VIDIOC_REQBUFS ok!\n");
	}

	buffers = calloc(req.count, sizeof(*buffers));
	for(n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;
		struct v4l2_plane planes_mmap[FMT_NUM_PLANES];

		CLEAR (buf);
		memset(planes_mmap, 0 , FMT_NUM_PLANES * sizeof(struct v4l2_plane));

		buf.type = type;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;
		buf.length = FMT_NUM_PLANES;
		buf.m.planes = planes_mmap;

		if (0 != ioctl(video_fd, VIDIOC_QUERYBUF, &buf))
			printf ("VIDIOC_QUERYBUF error\n");

		for (i = 0; i < FMT_NUM_PLANES; i++) {
			buffers[n_buffers].length[i] = buf.m.planes[i].length;

			buffers[n_buffers].start[i] = mmap(NULL,/* start anywhere */
					buf.m.planes[i].length,
					PROT_READ | PROT_WRITE,	/* required */
					MAP_SHARED,		/* recommended */
					video_fd, buf.m.planes[i].m.mem_offset);

			if (MAP_FAILED == buffers[n_buffers].start[i])
				printf ("mmap failed %d\n", i);
		}
	}

	for (i = 0; i < n_buffers; ++i)
	{
		struct v4l2_buffer buf;
		struct v4l2_plane planes_qbuf[FMT_NUM_PLANES];

		CLEAR (buf);
		memset(planes_qbuf, 0 , FMT_NUM_PLANES * sizeof(struct v4l2_plane));

		buf.m.planes = planes_qbuf;
		buf.type = type;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.length = FMT_NUM_PLANES;
		if (0 != ioctl (video_fd, VIDIOC_QBUF, &buf))
			printf ("VIDIOC_QBUF failed!\n");
	}

	if (argc == 6)
		snprintf(save_path, 32, "%s/fb_%dx%d.yuv", argv[5], fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height);

	yuv_fd = open(save_path, O_RDWR | O_CREAT, 0666);
	if (yuv_fd < 0)	{
		printf("open %s failed!\n", save_path);
		return -1;
	} else {
		printf("open %s ok!\n", save_path);
	}

	if((ioctl(video_fd, VIDIOC_STREAMON, &type)) < 0) {
		printf("VIDIOC_STREAMON failed!\n");
		return 0;
	} else {
		printf("VIDIOC_STREAMON ok!\n");
	}

	while (frame_cnt < frame_sum) {
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(video_fd, &fds);

		/* Timeout. */
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		r = select(video_fd + 1, &fds, NULL, NULL, &tv);
		if (-1 == r) {
			printf ("%s select err\n", video_name);
			break;
		}

		if (0 == r) {
			printf("select timeout!!!\n");
			continue;
		}

		if (read_frame()) {
			break;
		}
	}
	if((ioctl(video_fd, VIDIOC_STREAMOFF, &type)) < 0) {
		printf("VIDIOC_STREAMOFF failed!\n");
		return 0;
	} else {
		printf("VIDIOC_STREAMOFF ok!\n");
	}

	for(n_buffers = 0; n_buffers < req.count; ++n_buffers)
		for(i = 0;i < FMT_NUM_PLANES; i++)
			if (-1 == munmap(buffers[n_buffers].start[i], buffers[n_buffers].length[i]))
				printf ("munmap error");
	fsync(yuv_fd);
	close(yuv_fd);
	close (video_fd);
	close (isp_fd);
	close (rgb_fd);
	return 0;
}
