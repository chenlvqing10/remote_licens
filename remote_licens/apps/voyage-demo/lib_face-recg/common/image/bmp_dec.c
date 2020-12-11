/*
* bmp_dec.c - Public interface for personnel management
*
* Copyright (C) 2019, LomboTech Co.Ltd.
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

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdint.h>


#define LOG_TAG "BMP_DEC"
#include "log/log.h"
#include "bmp_dec.h"
#include "lombo_malloc.h"

#define DEBUG (1)


static long long get_systime_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

void bmp_dump(char *path, unsigned char *virt_addr, int size)
{
	FILE *fp;

	if (!path || !virt_addr)
		return;

	fp = fopen(path, "wb");
	if (!fp) {
		ALOGE("%s open %s failed, errno: %d (%s)", __func__, path,
			errno, strerror(errno));
		return;
	}

	fwrite(virt_addr, 1, size, fp);
	fclose(fp);
}

static void show_bmp_head(BITMAPFILEHEADER *pBmpHead){
	ALOGI("bit file head :\n");
	ALOGI("file head type: %d\n", pBmpHead->bf_type);
	ALOGI("file size     : %d\n", pBmpHead->bf_size);
	ALOGI("reserved_1    : %d\n", pBmpHead->bf_reserved1);
	ALOGI("reserved_2    : %d\n", pBmpHead->bf_reserved2);
	ALOGI("bf_off_bits     : %d\n", pBmpHead->bf_off_bits);
	ALOGI("sizeof(BITMAPFILEHEADER):%d\n", sizeof(BITMAPFILEHEADER));
}

static void show_bmp_info_Head(BITMAPINFOHEADER *pBmpInfoHead){
	ALOGI("bit file info head:\n");
	ALOGI("BmpInforHead size : %d", pBmpInfoHead->bi_size);
	ALOGI("BmpInforHead bi_width: %d", pBmpInfoHead->bi_width);
	ALOGI("BmpInforHead.bi_height: %d", pBmpInfoHead->bi_height);
	ALOGI("bi_planes    : %d", pBmpInfoHead->bi_planes);
	ALOGI("bi_bit_count  : %d", pBmpInfoHead->bi_bit_count);
	ALOGI("Compression : %d", pBmpInfoHead->bi_compression);
	ALOGI("bi_size_image : %d", pBmpInfoHead->bi_size_image);
	ALOGI("X Pels      : %d", pBmpInfoHead->bi_x_pels_per_meter);
	ALOGI("Y Pels      : %d", pBmpInfoHead->bi_y_pels_per_meter);
	ALOGI("ClrUsed     : %d", pBmpInfoHead->bi_clr_used);
	ALOGI("ClrImportant: %d", pBmpInfoHead->bi_clr_important);
	ALOGI("sizeof(BITMAPINFODEADER):%d\n", sizeof(BITMAPINFOHEADER));
}

static int bmp2bgr(BITMAPFILEHEADER *file_header ,BITMAPINFOHEADER *info_header,
		FILE *file, unsigned char *bgr_buf)
{
	int ret = 0;
	unsigned long i = 0, j = 0;
	unsigned long width = 0, height = 0, count = 0;
	unsigned char *bmp_buf = NULL;
	unsigned char *line_buf = NULL;

	/* width: a line bit count */
	if (((info_header->bi_width * info_header->bi_bit_count / 8) % 4) == 0)
		width = info_header->bi_width * info_header->bi_bit_count / 8;
	else
		width = ((
			info_header->bi_width * info_header->bi_bit_count + 31) / 32) * 4;

	height = info_header->bi_height;

	bmp_buf = (u_int8_t *)malloc(width * height);
	if (bmp_buf == NULL) {
		ALOGE("malloc bmp buf fail\n");
		ret = -1;
		goto exit;
	}
	memset(bmp_buf, 0, width * height);

	fseek(file, file_header->bf_off_bits, 0);
	count = fread(bmp_buf, 1, width * height, file);
	if (count !=  width * height) {
		ALOGE("read bmp buf fail\n");
		ret = -1;
		goto exit;
	}

	ALOGI("width = %ld height = %ld\n", width, height);
	/* bmp_dump("/data/test-bmp2bgr.bmp", bmp_buf, width * height); */

	/* only support 24 32 bit count */
	if (info_header->bi_bit_count < 16) {
		ALOGI("bi_bit_count = %d\n", info_header->bi_bit_count);
		ret = -1;
		goto exit;
	} else if (info_header->bi_bit_count == 16) {
		ALOGI("bi_bit_count = %d\n", info_header->bi_bit_count);
		ret = -1;
		goto exit;
	} else if (info_header->bi_bit_count == 32) {
		for (j = 0; j < height; j++) {
			/* for flip */
			line_buf =
				bgr_buf + (info_header->bi_width * 3 *
					(info_header->bi_height - 1 - j));
			/*
			line_buf =
				bgr_buf + (info_header->bi_width * 3 *(j));
			*/
			int pix_cnt = 0;
			/* BGRA */
			for (i = 0; i < width; i += 4) {
				*line_buf = bmp_buf[i + width * j];/*B*/
				*(line_buf + 1) = bmp_buf[i + width * j + 1];/*G*/
				*(line_buf + 2) = bmp_buf[i + width * j + 2];/*R*/
				line_buf += 3;
				pix_cnt++;
				if (pix_cnt == info_header->bi_width) /* jump next line */
					break;
			}
		}
	} else if(info_header->bi_bit_count == 24) {
		for (j = 0; j < height; j++) {
			/* for flip */
			line_buf =
				bgr_buf + (info_header->bi_width * 3 *
					(info_header->bi_height - 1 - j));
			/*
			line_buf =
				bgr_buf + (info_header->bi_width * 3 *(j));
			*/
			int pix_cnt = 0;
			/* BGR */
			for (i = 0; i < width; i += 3) {
				*line_buf = bmp_buf[i + width * j];/*B*/
				*(line_buf + 1) = bmp_buf[i + width * j + 1];/*G*/
				*(line_buf + 2) = bmp_buf[i + width * j + 2];/*R*/
				line_buf += 3;
				pix_cnt++;
				if (pix_cnt == info_header->bi_width) /* jump next line */
					break;
			}
		}
	} else {
		ALOGI("unknow bi_bit_count = %d\n", info_header->bi_bit_count);
		ret =-1;
		goto exit;
	}
	ALOGI("bmp_buf size = %ld\n", width * height);

exit:

	if (bmp_buf) {
		free(bmp_buf);
		bmp_buf = NULL;
	}
	return ret;
}

/* Y' = 0.257*R' + 0.504*G' + 0.098*B' + 16 */
static __inline float rgb2y(float r0, float g0, float b0)
{
	float y0 = 0.257f*r0 + 0.504f*g0 + 0.098f*b0 + 16.0f;
	return y0;
}

/* U equals Cb' */
/* Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128 */
static __inline float rgb2u(float r0, float g0, float b0)
{
	float u0 = -0.148f*r0 - 0.291f*g0 + 0.439f*b0 + 128.0f;
	return u0;
}

/* V equals Cr' */
/* Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128 */
static __inline float rgb2v(float r0, float g0, float b0)
{
	float v0 = 0.439f*r0 - 0.368f*g0 - 0.071f*b0 + 128.0f;
	return v0;
}

static int bgr2nv12(struct bmp_dec_input *input,
		struct bmp_dec_output *output, u_int8_t *bgr_buf, int scale)
{
	ALOGI("%s enter", __func__);
	int ret = 0;
	int ix = 0, iy = 0, jx = 0, jy = 0;
	unsigned char *I = NULL, *J = NULL;

	I = bgr_buf;
	J = output->virt_addr[0];

	/* In NV12 format, UV plane starts below Y plane. */
	unsigned char *UV = output->virt_addr[1];

	/* I0 and I1 points two sequential source rows. */
	const unsigned char *I0;  /*I0 -> rgbrgbrgbrgbrgbrgb... */
	const unsigned char *I1;  /*I1 -> rgbrgbrgbrgbrgbrgb... */

	/* Y0 and Y1 points two sequential destination rows of Y plane. */
	unsigned char *Y0;	/* Y0 -> yyyyyy */
	unsigned char *Y1;	/* Y1 -> yyyyyy */

	/* UV0 points destination rows of interleaved UV plane. */
	unsigned char *UV0; /* UV0 -> uvuvuv */

	for (iy=0, jy=0; iy < output->height - 1; iy+=(2*scale), jy+=2) {

		/* Input row width is image_width*3 bytes (each pixel is R,G,B). */
		I0 = &I[iy*output->width*3];
		I1 = &I[(iy+1)*output->width*3];

		/* Output Y row width is image_width bytes (one Y element per pixel). */
		Y0 = &J[jy*output->stride_width];
		Y1 = &J[(jy+1)*output->stride_width];

		UV0 = &UV[(jy/2)*output->stride_width];

		for (ix=0, jx=0; ix < output->width - 1; ix+=(2*scale), jx+=2) {

		/* Load R,G,B elements from first row (and convert to float). */
		float b00 = (float)I0[ix*3 + 0];
		float g00 = (float)I0[ix*3 + 1];
		float r00 = (float)I0[ix*3 + 2];

		/* Load next R,G,B elements from first row (and convert to float). */
		float b01 = (float)I0[ix*3 + 3];
		float g01 = (float)I0[ix*3 + 4];
		float r01 = (float)I0[ix*3 + 5];

		/* Load R,G,B elements from second row (and convert to float). */
		float b10 = (float)I1[ix*3 + 0];
		float g10 = (float)I1[ix*3 + 1];
		float r10 = (float)I1[ix*3 + 2];

		/* Load next R,G,B elements from second row (and convert to float). */
		float b11 = (float)I1[ix*3 + 3];
		float g11 = (float)I1[ix*3 + 4];
		float r11 = (float)I1[ix*3 + 5];

		/* Calculate 4 Y elements. */
		float y00 = rgb2y(r00, g00, b00);
		float y01 = rgb2y(r01, g01, b01);
		float y10 = rgb2y(r10, g10, b10);
		float y11 = rgb2y(r11, g11, b11);

		/* Calculate 4 U elements. */
		float u00 = rgb2u(r00, g00, b00);
		float u01 = rgb2u(r01, g01, b01);
		float u10 = rgb2u(r10, g10, b10);
		float u11 = rgb2u(r11, g11, b11);

		/* Calculate 4 V elements. */
		float v00 = rgb2v(r00, g00, b00);
		float v01 = rgb2v(r01, g01, b01);
		float v10 = rgb2v(r10, g10, b10);
		float v11 = rgb2v(r11, g11, b11);

		/* Calculate destination U element: average of 2x2 "original" U elements. */
		float u0 = (u00 + u01 + u10 + u11)*0.25f;

		/* Calculate destination V element: average of 2x2 "original" V elements. */
		float v0 = (v00 + v01 + v10 + v11)*0.25f;

		/* Store 4 Y elements (two in first row and two in second row). */
		Y0[jx + 0] = (unsigned char)(y00 + 0.5f);
		Y0[jx + 1] = (unsigned char)(y01 + 0.5f);
		Y1[jx + 0] = (unsigned char)(y10 + 0.5f);
		Y1[jx + 1] = (unsigned char)(y11 + 0.5f);

		/* Store destination U element. */
		UV0[jx + 0] = (unsigned char)(u0 + 0.5f);

		/* Store destination V element (next to stored U element). */
		UV0[jx + 1] = (unsigned char)(v0 + 0.5f);

		}
	}

	/*bmp_dump("/data/test-nv12.yuv",
		yuv_buf, output->width * output->height * 3 / 2);*/

	return ret;
}

static int init_yuv_buf(struct bmp_dec_input *input,
		struct bmp_dec_output *output)
{
	int ret = 0;
	void *viraddr = NULL;
	int size = 0;
	size = output->stride_width * output->stride_height * 3 / 2;
	viraddr =(void *)lombo_malloc_cached(size);
	if (viraddr == NULL) {
		ret =-1;
		goto exit;
	}
	memset(viraddr, 0, size);

	switch (input->format) {
	case BMP_COLOR_NV12: {
		output->size[0] = output->stride_width * output->stride_height;
		output->size[1] = output->size[0] / 2;
		output->size[2] = 0;
		output->virt_addr[0] = viraddr;
		output->virt_addr[1] = viraddr + output->size[0];
		output->virt_addr[2] = NULL;
		output->phy_addr[0] = lombo_vir_to_phy(output->virt_addr[0]);
		output->phy_addr[1] = lombo_vir_to_phy(output->virt_addr[1]);
		output->phy_addr[2] = 0;
		break;
		}
	case BMP_COLOR_MAX:
		ALOGE("format error\n");
		break;
	default:
		ALOGE("format error\n");
		break;
	}

exit:

	return ret;
}


int bmp_auto_scale(struct bmp_dec_output *output)
{
	int scale_w = 0, scale_h = 0;
	int scale = 1; /*default  no scale*/

	if ((output->width > output->stride_width) ||
			(output->height > output->stride_height)) {

		scale_w = output->width / output->stride_width;
		if ((output->width % output->stride_width) != 0)
			scale_w+=1;

		scale_h = output->height / output->stride_height;
		if ((output->height % output->stride_height) != 0)
			scale_h+=1;

		scale = (scale_w > scale_h) ? scale_w : scale_h;
	}

	ALOGI("[%s] scale = %d\n", __func__, scale);
	return scale;
}

__attribute__((visibility("hidden")))
void bmp_dec_end(struct bmp_dec_output *output)
{
	if (output == NULL)
		return;

	if (output->virt_addr[0]) {
		lombo_free_cached(output->virt_addr[0]);
		output->virt_addr[0] = NULL;
		output->virt_addr[1] = NULL;
		output->virt_addr[2] = NULL;
	}
	ALOGI("[%s] end\n", __func__);
}

__attribute__((visibility("hidden")))
int bmp_dec_begin(struct bmp_dec_input *input,
		struct bmp_dec_output *output)
{
	int ret = 0;
	int scale = 0;
	u_int8_t *bgr_buf = NULL;
	FILE *fp = NULL;

	BITMAPFILEHEADER head;
	BITMAPINFOHEADER info;
	output->begin = get_systime_us();

	memset(&head, 0, sizeof(head));
	memset(&info, 0, sizeof(info));

	fp = fopen(input->file, "rb");
	if (fp == NULL) {
		ALOGE("fopen file fail\n");
		ret = -1;
		goto exit;
	}

	fread(&head, 1, sizeof(BITMAPFILEHEADER), fp);
	fread(&info, 1, sizeof(BITMAPINFOHEADER), fp);
	#ifdef DEBUG
	show_bmp_head(&head);
	show_bmp_info_Head(&info);
	#endif
	output->width = info.bi_width; /* img width px */
	output->height = info.bi_height; /* img height px */
	bgr_buf = (u_int8_t *)malloc(output->width * output->height * 3);
	if (bgr_buf == NULL) {
		ALOGE("malloc bgr_buf fail\n");
		ret = -1;
		goto exit;
	}
	memset(bgr_buf, 0, output->width * output->height * 3);

	ret = bmp2bgr(&head, &info, fp, bgr_buf);
	if (ret) {
		goto exit;
	}

	switch (input->scale) {
	case BMP_SCALE_OFF:
		output->stride_width = output->width;
		output->stride_height = output->height;
		break;
	case BMP_SCALE_AUTO:
		output->stride_width = input->stride_width;
		output->stride_height = input->stride_height;
		break;
	default:
		output->stride_width = input->stride_width;
		output->stride_height = input->stride_height;
		break;
	}

	/* according to stride_width stride_height malloc res */
	ret = init_yuv_buf(input, output);
	if (ret) {
		goto exit;
	}

	/* scale :
		1 default not scale
		2 ...
	*/
	scale = bmp_auto_scale(output);

	/* only support NV12 */
	switch (input->format) {
	case BMP_COLOR_NV12:

		bgr2nv12(input, output, bgr_buf, scale);
		break;
	case BMP_COLOR_MAX:
		break;
	default:
		break;
	}

	ret = lombo_cache_flush((int)output->phy_addr[0]);
	if (ret)
		ALOGE("lombo cache flush failed");

	output->end = get_systime_us();
	ALOGI(" time = %lld \n", output->end - output->begin);

exit:

	/*lombo_free_cached*/

	if (bgr_buf) {
		free(bgr_buf);
		bgr_buf = NULL;
	}
	if (fp) {
		fclose(fp);
		fp = NULL;
	}

	return ret;
}

