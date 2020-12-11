/*
 * jpeg_enc.h - Standard functionality for lombo jpeg encode
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

#ifndef __JPEG_ENC_H__
#define __JPEG_ENC_H__

#define MAX_BLENDING_NUM 4

enum image_format {
	IMG_COLOR_YUV,
	IMG_COLOR_YVU,
	IMG_COLOR_NV12,
	IMG_COLOR_NV21,
	IMG_COLOR_ARGB,
	IMG_COLOR_RGBA,
	IMG_COLOR_ABGR,
	IMG_COLOR_BGRA,
};

typedef struct en_rect {
	int   x;
	int   y;
	int   width;
	int   height;
} en_rect_t;

struct watermark_info {
	unsigned int blending_enable;	/* watermark enable flag */
	unsigned char *data;			/* watermark picture data */
	unsigned int size;				/* watermark picture data size */
	enum image_format colorspace;	/* watermark colorspace */
	unsigned int blending_x_pos;	/* watermark start x pos */
	unsigned int blending_y_pos;	/* watermark start y pos */
	unsigned int blending_width;	/* watermark width in pixel */
	unsigned int blending_height;	/* watermark height in pixel */
	unsigned int blending_stride;	/* watermark stride in bytes */
};

struct input_image_info {
	int width;			/* should be aligned with 16 */
	int height;			/* should be aligned with 16 */
	enum image_format format;
	int qualify;			/* range: 1 - 99, 99 is the best quality */
	unsigned char *addr[3];		/* yuv buffer */
	int	size[3];				/* y, u(or uv), v buffer size*/
	unsigned char *jpeg_info_buf;	/* jpeg header info buffer */
	unsigned char *jpeg_out_buf;	/* no use for now */
	int out_buf_size;		/* jpeg output buffer size */
	char *file;			/* file path to save */
	struct watermark_info
		watermark_pictures[MAX_BLENDING_NUM];/* watermark input param */
	unsigned int watermark_picture_num;/* the numbers of watermark pictures input */
	en_rect_t enc_rect;		/* encode area */
};

struct output_image_info {
	unsigned char *buf;
	int size;
};

/*
* jpeg_out_buf: buffer used to save jpeg data, alloced by caller.
* out_buf_size: size of jpeg_out_buf.
*/
int jpeg_enc_buffer(struct input_image_info *input,
					struct output_image_info *output);
/*
* jpeg_out_buf: not required.
* file: file path to save jpeg.
*/
int jpeg_enc_file(struct input_image_info *input,
				struct output_image_info *output);


/*
* return a handle if  successfully, return NULL if failed.
*/
void *jpeg_fast_enc_init(void);

/*
* encoder: jpeg_fast_enc_init returned handle
*/
int jpeg_fast_enc_deinit(void *encoder);

/*
* jpeg_out_buf: buffer used to save jpeg data, alloced by caller.
* out_buf_size: size of jpeg_out_buf.
* encoder: jpeg_fast_enc_init returned handle
*/
int jpeg_fast_enc_buffer(struct input_image_info *input,
						struct output_image_info *output,
						void *encoder);

/*
* jpeg_out_buf: not required.
* file: file path to save jpeg.
* encoder: jpeg_fast_enc_init returned handle
*/
int jpeg_fast_enc_file(struct input_image_info *input,
				struct output_image_info *output, void *encoder);



#endif

