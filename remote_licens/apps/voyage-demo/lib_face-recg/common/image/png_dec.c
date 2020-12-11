/*
* png_dec.c - Public interface for personnel management
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
#include <png.h>

#define LOG_TAG "PNG_DEC"
#include "log/log.h"
#include "png_dec.h"
#include "lombo_malloc.h"

static long long get_systime_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

void png_dump(char *path, unsigned char *virt_addr, int size)
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

static int bgr2nv12(struct png_dec_input *input,
		struct png_dec_output *output, u_int8_t *bgr_buf, int scale)
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

static int init_yuv_buf(struct png_dec_input *input,
		struct png_dec_output *output)
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
	case PNG_COLOR_NV12: {
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
	case PNG_COLOR_MAX:
		ALOGE("format error\n");
		break;
	default:
		ALOGE("format error\n");
		break;
	}

exit:

	return ret;
}

int png_auto_scale(struct png_dec_output *output)
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
void png_dec_end(struct png_dec_output *output)
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
int png_dec_begin(struct png_dec_input *input,
		struct png_dec_output *output)
{
	int ret = 0;
	int width = 0, height = 0;
	int scale = 0;
	u_int8_t *bgr_buf = NULL;
	FILE *fp = NULL;
	png_structp png = NULL;
	png_infop info = NULL;
	png_bytep *row = NULL;
	int color = 0;
	int bits = 0;
	int i=0, j=0, pos=0;

	output->begin = get_systime_us();

	fp = fopen(input->file, "rb");
	if (fp == NULL) {
		ALOGE("fopen file fail\n");
		ret = -1;
		goto exit;
	}

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png == NULL) {
		ret = -1;
		goto exit;
	}

	if (setjmp(png_jmpbuf(png))) {
		ret = -1;
		goto exit;
	}

	info = png_create_info_struct(png);
	if (info == NULL) {
		ret = -1;
		goto exit;
	}

	png_init_io(png, fp);
	png_read_png(png, info,
		PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, NULL);

	color= info->color_type;
	bits = info->pixel_depth;
	width =info->width;
	height =info->height;
	output->width = width;
	output->height = height;

	if (color == PNG_COLOR_TYPE_PALETTE) {
		ALOGI("PNG_COLOR_TYPE_PALETTE\n");
		ret = -1;
		goto exit;
	} else if (color == PNG_COLOR_TYPE_RGB) {
		ALOGI("PNG_COLOR_TYPE_RGB\n");

	} else if (color == PNG_COLOR_TYPE_RGB_ALPHA) {
		ALOGI("PNG_COLOR_TYPE_RGB_ALPHA\n");
		ret = -1;
		goto exit;
	} else if (color == PNG_COLOR_TYPE_RGBA) {
		ALOGI("PNG_COLOR_TYPE_RGBA\n");

	} else {
		ALOGI("unknow color = %d\n", color);
		ret = -1;
		goto exit;
	}

	bgr_buf = (u_int8_t *)malloc(width * height * 3);
	if (bgr_buf == NULL) {
		ALOGE("malloc bgr_buf fail\n");
		ret = -1;
		goto exit;
	}
	memset(bgr_buf, 0, width * height * 3);

	row = png_get_rows(png, info);
	switch (color) {
		case PNG_COLOR_TYPE_RGB_ALPHA:
			for (i = 0; i < height; i++) {
				for (j = 0; j < width*4; j+=4) {
					bgr_buf[pos++] = row[i][j+2]; /*b*/
					bgr_buf[pos++] = row[i][j+1]; /*g*/
					bgr_buf[pos++] = row[i][j+0]; /*r*/
				}
			}
			break;
		case PNG_COLOR_TYPE_RGB:
			for (i = 0; i < height; i++) {
				for (j = 0; j < width*3; j+=3) {
					bgr_buf[pos++] = row[i][j+2]; /*b*/
					bgr_buf[pos++] = row[i][j+1]; /*g*/
					bgr_buf[pos++] = row[i][j+0]; /*r*/
				}
			}
			break;
		default:
			break;
	}

	ALOGI("bits = %d color = %d width = %d height = %d \n",
		bits, color, width, height);

	switch (input->scale) {
	case PNG_SCALE_0FF:
		output->stride_width = output->width;
		output->stride_height = output->height;
		break;
	case PNG_SCALE_AUTO:
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
	scale = png_auto_scale(output);

	/* only support NV12 */
	switch (input->format) {
	case PNG_COLOR_NV12:
		ret = bgr2nv12(input, output, bgr_buf, scale);
		break;
	default:
		break;
	}

	ret = lombo_cache_flush(output->phy_addr[0]);
	if (ret)
		ALOGE("lombo cache flush failed");

	output->end = get_systime_us();
	ALOGI(" time = %lld \n", output->end - output->begin);

exit:

	/*lombo_free_cached*/
	if (png) {
		png_destroy_read_struct(&png, &info, NULL);
	}
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

