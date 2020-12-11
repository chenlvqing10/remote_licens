/*
 * jpeg_dec.h
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
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

#ifndef __JPEG_DEC_H__
#define __JPEG_DEC_H__

enum jpeg_format {
	JPEG_COLOR_YV12,
	JPEG_COLOR_YV21,
	JPEG_COLOR_NV12,
	JPEG_COLOR_NV21,
	JPEG_COLOR_MAX
};

enum jpeg_scale {
	JPEG_SCALEDOWN_0,
	JPEG_SCALEDOWN_1,
	JPEG_SCALEDOWN_2,
	JPEG_SCALE_AUTO,
	JPEG_SCALE_UNLIM,
	JPEG_SCALE_MAX
};

struct jpeg_dec_input {
	char *file;		/*absolute path of jpeg file */
	enum jpeg_format format;	/* output yuv format */
	enum jpeg_scale scale;		/* zoom/narrow scale */
	int width_stride;		/* 0 means low-level determine stride */
	int height_stride;
};

/*
*	      actual  encode  stride
* ----------------------------
*|		|	|	|
*|		|	|	|
*|		|	|	|
*|		|	|	|
* --------------	|	|
*|			|	|
*|			|	|
* ----------------------------
*
* actual size: actual size of yuv
* encode size : making alignment required by hardware while encoding
* stride size: stride width for hardware while decoding (stride width only)
*/
struct jpeg_dec_output {
	int actual_width;		/* actual width of yuv */
	int actual_height;		/* actual height of yuv */
	int encode_width;
	int encode_height;
	int stride_width;
	int stride_height;
	unsigned char *addr[3];	/* virtual address of yuv buffer */
	unsigned long phy_addr[3];	/* physical address of yuv buffer */
	int size[3];			/* y, u(or uv), v buffer size*/
};

enum img_format {
	IMG_FMT_ARGB8888,
	IMG_FMT_NV12,
	IMG_FMT_MAX,
};

struct pic_dec_param {
	enum img_format format;
	unsigned char *buf;  /* need use lombo_malloc_cached or lombo_malloc_unca */
	unsigned int bufsize;
	int out_width;  /* user set, if ==0: use decode out param */
	int out_height; /* user set, if ==0: use decode out param */
	int size[3];	/* decoder output each component size,
			 * if format is argb8888, use size[0] */
};

struct pic_scale_param {
	enum img_format format;
	unsigned char *buf;  /* need use lombo_malloc_cached or lombo_malloc_unca */
	int bufsize;
	int width;
	int height;
	int stride_width;
	int stride_height;
	int size[3];
};

/*
* jpeg_dec_begin: decode jpeg according to specified parameters.
*
* return zero if decode successfully, return none zero if failed.
*/
int jpeg_dec_begin(struct jpeg_dec_input *input, struct jpeg_dec_output *output);

/*
* jpeg_dec_data: decode jpeg data according to specified parameters.
*
* return zero if decode successfully, return none zero if failed.
*/
int jpeg_dec_data(char *jpeg_data, int data_len, int width_stride,
			int height_stride, enum jpeg_scale scale_mode,
			struct jpeg_dec_output *output);
/*
* free resource allocated internally, jpeg_dec_begin and jpeg_dec_end come in pairs.
*/
void jpeg_dec_end(struct jpeg_dec_output *output);


/*
* return a handle if  successfully, return NULL if failed.
*/
void *jpeg_fast_dec_init(void);

/*
* decoder: jpeg_fast_dec_init returned handle
*
* return zero if  successfully, return none zero if failed.
*/
int jpeg_fast_dec_deinit(void *decoder);
/*
* jpeg_dec_begin: decode jpeg according to specified parameters.
*
* return zero if decode successfully, return none zero if failed.
*/
int jpeg_fast_dec_begin(struct jpeg_dec_input *input,
						struct jpeg_dec_output *output,
						void *decoder);
/*
* jpeg_dec_data: decode jpeg data according to specified parameters.
*
* return zero if decode successfully, return none zero if failed.
*/
int jpeg_fast_dec_data(char *jpeg_data, int data_len, int width_stride,
			int height_stride, enum jpeg_scale scale_mode,
			struct jpeg_dec_output *output, void *decoder);

/*
* free resource allocated internally, jpeg_dec_begin and jpeg_dec_end come in pairs.
*/
void jpeg_fast_dec_end(struct jpeg_dec_output *output);

int image_scale(struct pic_scale_param *src, struct pic_scale_param *dst);
int jpeg_decode(char *data, int size,	struct pic_dec_param *outparam);

/*
* jpeg_fast_read_header: read jpeg data dimension parameters.
*
* return zero if decode successfully, return none zero if failed.
*/
int jpeg_fast_read_header(void *handle, char *jpeg_data, int data_len, int *width_stride,
			int *height_aligned);
/*
* jpeg_fast_read_file_header: read jpeg file dimension parameters.
*
* return zero if decode successfully, return none zero if failed.
*/
int jpeg_fast_read_file_header(void *handle, char *file_name, int *width_stride,
			int *height_aligned);
/*
* jpeg_read_data_header: read jpeg data dimension parameters.
*
* return zero if decode successfully, return none zero if failed.
*/
int jpeg_read_data_header(char *jpeg_data, int data_len, int *width_stride,
			int *height_aligned);
/*
* jpeg_read_file_header: read jpeg file dimension parameters.
*
* return zero if decode successfully, return none zero if failed.
*/
int jpeg_read_file_header(char *file_name, int *width_stride,
			int *height_aligned);


#endif

