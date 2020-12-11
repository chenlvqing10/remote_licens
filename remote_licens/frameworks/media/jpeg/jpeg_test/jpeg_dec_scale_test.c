
#define LOG_TAG "JPEG_DEC"
#include "log/log.h"

#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include "jpeg_dec.h"
#include "oscl_mem.h"

#define MAX_OUT_IMAGE_WIDTH	(2048)
#define MAX_OUT_IMAGE_HEIGHT	(2048)

void usage(char *func)
{
	printf("Usage: %s outformat\n", func);
}

/* max out size: 2048X1536 */
int main(int argc, char **argv)
{
	char in_file[128] = {0};
	char out_file[128] = {0};
	char resolution[12] = {0};
	int out_w = 1024, out_h = 600;
	char *tmp = NULL;
	int ret = 0;
	unsigned int size = 0;
	int out_format = 0; /* 1: yuv, 0: argb8888 */
	struct pic_dec_param decparam;
	char *data = NULL;
	int data_size = 0;
	FILE *fp = NULL;

	if (argc < 3) {
		usage(argv[0]);
		return -1;
	}
	out_format = atoi(argv[2]);
	if (argv[3])
		ret = atoi(argv[3]);
	if (ret > 0)
		out_w = ret;
	if (argv[4])
		ret = atoi(argv[4]);
	if (ret > 0)
		out_h = ret;
	ALOGD("decoder out size: %d x %d\n", out_w, out_h);
	strncpy(in_file, argv[1], sizeof(in_file));
	tmp = strchr(argv[1], '.');
	if (!tmp) {
		ALOGE("input file error.");
		return -1;
	}
	memcpy(out_file, argv[1], tmp - argv[1]);
	memset(&decparam, 0, sizeof(decparam));

	fp = fopen(in_file, "rb");
	if (fp == NULL) {
		ALOGE("open %s failed", in_file);
		return -1;
	}

	/* get file length */
	fseek(fp, 0, SEEK_END);
	data_size = ftell(fp);
	if (data_size <= 0) {
		ALOGE("get file length error");
		fclose(fp);
		return -1;
	}
	fseek(fp, 0, SEEK_SET);

	data = (char *)malloc(data_size);
	if (data == NULL) {
		ALOGE("alloc mem for file failed");
		fclose(fp);
		return -1;
	}

	ret = fread(data, 1, data_size, fp);
	if (ret != data_size) {
		ALOGE("read file failed, count:%d, file_len:%d", ret, data_size);
		free(data);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	decparam.out_width = out_w > MAX_OUT_IMAGE_WIDTH ? MAX_OUT_IMAGE_WIDTH : out_w;
	decparam.out_height = out_h > MAX_OUT_IMAGE_HEIGHT ? MAX_OUT_IMAGE_HEIGHT : out_h;
	if ((decparam.out_width == 0) || (decparam.out_height == 0)) {
		decparam.out_width = MAX_OUT_IMAGE_WIDTH;
		decparam.out_height = MAX_OUT_IMAGE_HEIGHT;
	}
	snprintf(resolution, sizeof(resolution), "%dx%d",
		decparam.out_width, decparam.out_height);
	strcat(out_file, "_");
	strcat(out_file, resolution);
	strcat(out_file, ".");
	if (out_format == 1) {
		strcat(out_file, "yuv");
		decparam.format = IMG_FMT_NV12;
		size = ((decparam.out_width * decparam.out_height * 3) >> 1) + 1024;
	} else if (out_format == 0) {
		strcat(out_file, "rgb");
		decparam.format = IMG_FMT_ARGB8888;
		size = decparam.out_width * decparam.out_height * 4 + 1024;
	} else {
		ALOGE("out file format unsupport.");
		goto EXIT;
	}
	decparam.bufsize = size;
	decparam.buf = oscl_malloc_cached(decparam.bufsize);
	if (!decparam.buf) {
		ALOGE("oscl_malloc_cached fail. size:%d", decparam.bufsize);
		goto EXIT;
	}
	ret = jpeg_decode(data, data_size, &decparam);
	if (ret == 0) {
		fp = fopen((const char *)out_file, "wb");
		if (fp == NULL) {
			ALOGE("open %s failed", out_file);
			ret = -1;
			goto EXIT;
		}
		fwrite(decparam.buf, decparam.size[0] + decparam.size[1], 1, fp);
		fclose(fp);
		ALOGD("%s -> %s decode success.", in_file, out_file);
	}
EXIT:
	if (decparam.buf)
		oscl_free_cached(decparam.buf);
	if (data)
		free(data);

	return ret;
}

