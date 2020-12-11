/*
 * jpeg_enc.c - queue api used by lombo jpeg encode
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "JPEG_ENC"
#include "log/log.h"

#include "oscl_mem.h"
#include "lombo_jpegenc_plugin.h"
#include "jpeg_enc.h"
#include "jpeg.h"
#include "jpeg_hal.h"

#define STREAM_BUF_SIZE		(4 * 1024 * 1024)
#define JPEG_INFO_SIZE		(8 * 1024)

void *jpeg_fast_enc_init(void)
{
	return hal_jpeg_enc_init();
}
int jpeg_fast_enc_deinit(void *handle)
{
	return hal_jpeg_enc_deinit(handle);
}

static int _recive_watermark_parm(struct input_image_info *input,
				jpeg_enc_parm_t *enc_parm)
{
	int i , nmark = 0;
	watermark_t *out = NULL;
	watermark_picture_t *watermark = NULL;
	struct  watermark_info *input_parm = NULL;
	unsigned int total_nmark;
	out = &enc_parm->wm;
	memset(out, 0, sizeof(watermark_t));

	total_nmark = input->watermark_picture_num;

	if (total_nmark != 0 && total_nmark <= MAX_ENC_BLENDING_NUM)
		for (i = 0; i < MAX_ENC_BLENDING_NUM; i++) {
			input_parm = &input->watermark_pictures[i];
			watermark = &out->watermark_pictures[i];

			if (input_parm->blending_enable == 1) {

				if (input_parm->data == NULL && input->size == 0) {
					ALOGE("blending picture %d buffer error\n", i);
					continue;
				}
				if (input_parm->blending_width == 0 ||
					input_parm->blending_height == 0 ||
					input_parm->blending_stride == 0) {
					ALOGE("blending picture %d param error\n", i);
					continue;
			}

			switch (input_parm->colorspace) {
			case IMG_COLOR_ARGB:
				watermark->colorspace = ENC_ARGB8888;
				break;
			case IMG_COLOR_RGBA:
				watermark->colorspace = ENC_RGBA8888;
				break;
			case IMG_COLOR_ABGR:
				watermark->colorspace = ENC_ABGR8888;
				break;
			case IMG_COLOR_BGRA:
				watermark->colorspace = ENC_BGRA8888;
				break;
			default:
				ALOGW("watermark %d colorspace may be incorrect\n", i);
				continue;
			}


				watermark->vir_addr = oscl_malloc_unca(input_parm->size);
				if (watermark->vir_addr == NULL) {
					ALOGE("malloc blendings buffer error!\n");
					return -1;
				}

				watermark->phy_addr =
					oscl_virt_to_phys(watermark->vir_addr);
				memcpy(watermark->vir_addr,
					input_parm->data, input_parm->size);

				watermark->blending_x_pos = input_parm->blending_x_pos;
				watermark->blending_y_pos = input_parm->blending_y_pos;
				watermark->blending_stride = input_parm->blending_stride;
				watermark->blending_width = input_parm->blending_width;
				watermark->blending_height = input_parm->blending_height;
				watermark->blending_enable = 1;

				nmark++;
			}
		}

	if (nmark == 0) {
		if (total_nmark != 0)
			ALOGW("watermark_picture_num: %d, param may be incorrect.\n"
					, total_nmark);
	} else{
		if (nmark != total_nmark) {
			ALOGE("watermark_picture_num: %d, found %d, parm mismatch!\n"
					, total_nmark, nmark);
			return -1;
		}
	}

	out->watermark_picture_num = nmark;
	return 0;

}

static int _destroy_watermark_parm(jpeg_enc_parm_t *enc_parm)
{
	watermark_t *out = NULL;
	watermark_picture_t *watermark = NULL;
	int i, nmark = 0;

	out = &enc_parm->wm;

	for (i = 0; i < MAX_ENC_BLENDING_NUM; i++) {
		watermark = &out->watermark_pictures[i];

		if (watermark->blending_enable == 1) {
			if (watermark->vir_addr != NULL) {
				oscl_free_unca(watermark->vir_addr);
				nmark++;
			}
		}
	}
	if (out->watermark_picture_num != nmark) {
		ALOGE("watermark destroy check fail!\n");
		return -1;
	}
	return 0;
}
static int _jpeg_enc_fast(struct input_image_info *input,
							struct output_image_info *output,
							void *handle)
{
	jpeg_enc_capbuf_t capbuf;
	jpeg_enc_packet_t packet;
	jpeg_enc_parm_t enc_param;
	unsigned char *input_buffer = NULL;
	int buf_size, ret = 0, y_len;
	unsigned int uv_offset[2];
	/* Physical address is required for jpeg driver. In order to get physical address,
	* get memory by oscl_malloc instead malloc.
	*/
	buf_size = input->width * input->height * 3 / 2;
	y_len = input->height * input->width;
	uv_offset[0] = y_len;
	input_buffer = oscl_malloc_unca(buf_size);

	if (!input_buffer) {
		ALOGE("malloc for input buffer failed\n");
		ret = -1;
		goto exit;
	}
	if (input->size[0] < y_len) {
		ALOGE("y buffer size error %d %d\n", input->size[0], y_len);
		return -1;
	}
	memcpy(input_buffer, input->addr[0], y_len);

	if (input->format < IMG_COLOR_NV12) {
		if (input->size[1] < y_len/4 || input->size[2] < y_len/4) {
			ALOGE("uv buffer size error %d , %d, %d\n",
				input->size[1], input->size[2], y_len/4);
			return -1;
		}
		if (input->addr[1] == NULL || input->addr[2] == NULL) {
			ALOGE("uv buffer addr error\n");
			return -1;
		}

		memcpy(input_buffer + uv_offset[0], input->addr[1], y_len/4);
		uv_offset[1] = uv_offset[0] + y_len/4;
		memcpy(input_buffer + uv_offset[1], input->addr[2], y_len/4);

	} else{
		if (input->size[1] < y_len/2) {
			ALOGE("uv buffer size error %d %d\n",
				input->size[1], y_len/2);
			return -1;
		}
		if (input->addr[1] == NULL) {
			ALOGE("uv buffer addr error\n");
			return -1;
		}
		memcpy(input_buffer + uv_offset[0], input->addr[1], y_len/2);
		uv_offset[1] = 0;
	}

	capbuf.vir_addr[0] = (unsigned char *)input_buffer;
	capbuf.phy_addr[0] = oscl_virt_to_phys(capbuf.vir_addr[0]);
	capbuf.vir_addr[1] = (unsigned char *)(capbuf.vir_addr[0] + uv_offset[0]);
	capbuf.phy_addr[1] = capbuf.phy_addr[0] + uv_offset[0];
	if (input->format < IMG_COLOR_NV12) {
		capbuf.vir_addr[2] = (unsigned char *)(capbuf.vir_addr[0] + uv_offset[1]);
		capbuf.phy_addr[2] = capbuf.phy_addr[0] + uv_offset[1];
	} else{
		capbuf.vir_addr[2] = NULL;
		capbuf.phy_addr[2] = 0;
	}

	input->out_buf_size = STREAM_BUF_SIZE;
	packet.buf_size = input->out_buf_size;
	packet.vir_addr = input->jpeg_out_buf;

	enc_param.in_width = input->width;
	enc_param.in_height = input->height;
	enc_param.in_stride = (input->width + 15) & ~0x0f;
	enc_param.out_width = (input->width + 15) & ~0x0f;
	enc_param.out_height = (input->height + 15) & ~0x0f;
	enc_param.input_mode = input->format;
	enc_param.quality = input->qualify;

	ret = _recive_watermark_parm(input , &enc_param);
	if (ret) {
		ALOGE("get watermark param failed\n");
		goto exit;
	}

	if (!handle) {
		ALOGE("jpeg enc open failed\n");
		ret = -1;
		goto exit;
	}

	ret = hal_jpeg_enc_frame(handle, &capbuf, &packet, &enc_param);
	if (ret) {
		ALOGE("jpeg enc frame failed\n");
		goto exit;
	}

	output->buf = packet.vir_addr;
	output->size = packet.stream_length;

exit:
	_destroy_watermark_parm(&enc_param);
	if (input_buffer)
		oscl_free_unca(input_buffer);

	return ret;
}

int jpeg_fast_enc_buffer(struct input_image_info *input,
							struct output_image_info *output,
							void *handle)
{
	int ret;

	if (input == NULL || output == NULL || input->addr[0] == NULL ||
				input->width <= 0 || input->height <= 0) {
		ALOGE("param error\n");
		return -1;
	}

	if (input->jpeg_out_buf == NULL) {
		ALOGE("out buf == NULL\n");
		return -1;
	}

	if (input->height & 0x07 || input->width & 0x0f) {
		ALOGE("width(%d) and height(%d) should be aligned with 16 or 8\n",
			input->width, input->height);
		return -1;
	}

	if (input->format < IMG_COLOR_YUV || input->format > IMG_COLOR_NV21) {
		ALOGE("input format error\n");
		return -1;
	}

	ret = _jpeg_enc_fast(input, output, handle);
	if (ret)
		ALOGE("jpeg enc failed");


	return ret;
}

int jpeg_fast_enc_file(struct input_image_info *input,
						struct output_image_info *output,
						void *handle)
{
	int ret, count;
	void *output_buffer = NULL;
	FILE *filp = NULL;

	/* alloc mem for out buffer according to experience value */
	output_buffer = malloc(STREAM_BUF_SIZE);
	if (!output_buffer) {
		ALOGE("malloc for out buffer failed");
		return -1;
	}

	input->jpeg_out_buf = output_buffer;
	ret = jpeg_fast_enc_buffer(input, output, handle);
	if (ret)
		goto exit;

	filp = fopen(input->file, "wb");
	if (!filp) {
		ALOGE("open file error:%s\n", input->file);
		ret = -1;
		goto exit;
	}

	count = fwrite(output->buf, 1, output->size, filp);
	if (count != output->size) {
		ALOGE("write file error, count:%d, expected:%d\n",
				count, output->size);
		ret = -1;
	}

exit:
	if (output_buffer)
		free(output_buffer);
	if (filp)
		fclose(filp);

	return ret;
}

static int _jpeg_enc(struct input_image_info *input, struct output_image_info *output)
{
	jpeg_enc_capbuf_t capbuf;
	jpeg_enc_packet_t packet;
	jpeg_enc_parm_t enc_param;
	void *handle = NULL;
	unsigned char *input_buffer = NULL;
	int buf_size, ret = 0, y_len;
	unsigned int en_offset[3] = {0};
	unsigned int uv_offset[3] = {0};

	int src_height;
	int src_width;
	/* Physical address is required for jpeg driver. In order to get physical address,
	* get memory by oscl_malloc instead malloc.
	*/

	src_height = input->height;
	src_width = input->width;
	if (input->enc_rect.width > src_width || input->enc_rect.width <= 0)
		input->enc_rect.width = src_width;
	if (input->enc_rect.height > src_height || input->enc_rect.height <= 0)
		input->enc_rect.height = src_height;
	if (input->enc_rect.width != src_width && input->enc_rect.height != src_height) {
		en_offset[0] = src_width * input->enc_rect.y;
		en_offset[0] += input->enc_rect.x;
		if (input->format >= IMG_COLOR_NV12) {
			en_offset[1] = src_width * input->enc_rect.y / 2;
			en_offset[1] += input->enc_rect.x;
		} else {
			en_offset[1] = src_width * input->enc_rect.y / 4;
			en_offset[1] += input->enc_rect.x / 2;
			en_offset[2] = en_offset[1];
		}
	}
	buf_size = src_width * src_height * 3 / 2;
	y_len = src_width * src_height;
	uv_offset[0] = y_len;
	input_buffer = oscl_malloc_unca(buf_size);
	ALOGE("encode area: x: %d, y: %d, w: %d, h: %d", input->enc_rect.x,
		input->enc_rect.y, input->enc_rect.width, input->enc_rect.height);

	if (!input_buffer) {
		ALOGE("malloc for input buffer failed\n");
		ret = -1;
		goto exit;
	}
	if (input->size[0] < y_len) {
		ALOGE("y buffer size error %d %d\n", input->size[0], y_len);
		return -1;
	}

	memcpy(input_buffer, input->addr[0], y_len);
	if (input->format < IMG_COLOR_NV12) {
		if (input->size[1] < y_len/4 || input->size[2] < y_len/4) {
			ALOGE("uv buffer size error %d , %d, %d\n",
				input->size[1], input->size[2], y_len/4);
			return -1;
		}
		if (input->addr[1] == NULL || input->addr[2] == NULL) {
			ALOGE("uv buffer addr error\n");
			return -1;
		}

		memcpy(input_buffer + uv_offset[0], input->addr[1], y_len/4);
		uv_offset[1] = uv_offset[0] + y_len/4;
		memcpy(input_buffer + uv_offset[1], input->addr[2], y_len/4);

	} else{
		if (input->size[1] < y_len/2) {
			ALOGE("uv buffer size error %d %d\n",
				input->size[1], y_len/2);
			return -1;
		}
		if (input->addr[1] == NULL) {
			ALOGE("uv buffer addr error\n");
			return -1;
		}
		memcpy(input_buffer + uv_offset[0], input->addr[1], y_len/2);
		uv_offset[1] = 0;
	}

	capbuf.vir_addr[0] = (unsigned char *)(input_buffer + en_offset[0]);
	capbuf.phy_addr[0] = oscl_virt_to_phys(input_buffer);
	capbuf.phy_addr[0] += en_offset[0];

	capbuf.vir_addr[1] = (unsigned char *)(input_buffer + uv_offset[0] +
		en_offset[1]);
	capbuf.phy_addr[1] = oscl_virt_to_phys(input_buffer + uv_offset[0]);
	capbuf.phy_addr[1] += en_offset[1];
	if (input->format < IMG_COLOR_NV12) {
		capbuf.vir_addr[2] = (unsigned char *)(input_buffer + uv_offset[1] +
			en_offset[2]);
		capbuf.phy_addr[2] = oscl_virt_to_phys(input_buffer + uv_offset[1]);
		capbuf.phy_addr[2] += en_offset[2];
	} else{
		capbuf.vir_addr[2] = NULL;
		capbuf.phy_addr[2] = 0;
	}

	input->out_buf_size = STREAM_BUF_SIZE;
	packet.buf_size = input->out_buf_size;
	packet.vir_addr = input->jpeg_out_buf;

	enc_param.in_width = input->enc_rect.width;
	enc_param.in_height = input->enc_rect.height;
	enc_param.in_stride = (src_width + 15) & ~0x0f;
	enc_param.out_width = (input->enc_rect.width + 15) & ~0x0f;
	enc_param.out_height = (input->enc_rect.height + 15) & ~0x0f;
	enc_param.input_mode = input->format;
	enc_param.quality = input->qualify;

	ret = _recive_watermark_parm(input , &enc_param);
	if (ret) {
		ALOGE("get watermark param failed\n");
		goto exit;
	}

	handle = hal_jpeg_enc_init();
	if (!handle) {
		ALOGE("jpeg enc open failed\n");
		ret = -1;
		goto exit;
	}

	ret = hal_jpeg_enc_frame(handle, &capbuf, &packet, &enc_param);
	if (ret) {
		ALOGE("jpeg enc frame failed\n");
		goto exit;
	}

	output->buf = packet.vir_addr;
	output->size = packet.stream_length;

exit:
	_destroy_watermark_parm(&enc_param);
	if (input_buffer)
		oscl_free_unca(input_buffer);
	if (handle)
		hal_jpeg_enc_deinit(handle);

	return ret;
}

int jpeg_enc_buffer(struct input_image_info *input, struct output_image_info *output)
{
	int ret;

	if (input == NULL || output == NULL || input->addr[0] == NULL ||
				input->width <= 0 || input->height <= 0) {
		ALOGE("param error\n");
		return -1;
	}

	if (input->jpeg_out_buf == NULL) {
		ALOGE("out buf == NULL\n");
		return -1;
	}

	if (input->height & 0x07 || input->width & 0x0f) {
		ALOGE("width(%d) and height(%d) should be aligned with 16 or 8\n",
			input->width, input->height);
		return -1;
	}

	if (input->format < IMG_COLOR_YUV || input->format > IMG_COLOR_NV21) {
		ALOGE("input format error\n");
		return -1;
	}

	jpeg_lock();

	ret = _jpeg_enc(input, output);
	if (ret)
		ALOGE("jpeg enc failed");

	jpeg_unlock();

	return ret;
}

int jpeg_enc_file(struct input_image_info *input,
				struct output_image_info *output)
{
	int ret, count;
	void *output_buffer = NULL;
	FILE *filp = NULL;

	/* alloc mem for out buffer according to experience value */
	output_buffer = malloc(STREAM_BUF_SIZE);
	if (!output_buffer) {
		ALOGE("malloc for out buffer failed");
		return -1;
	}

	input->jpeg_out_buf = output_buffer;
	ret = jpeg_enc_buffer(input, output);
	if (ret)
		goto exit;

	filp = fopen(input->file, "wb");
	if (!filp) {
		ALOGE("open file error:%s\n", input->file);
		ret = -1;
		goto exit;
	}

	count = fwrite(output->buf, 1, output->size, filp);
	if (count != output->size) {
		ALOGE("write file error, count:%d, expected:%d\n",
				count, output->size);
		ret = -1;
	}

exit:
	if (output_buffer)
		free(output_buffer);
	if (filp)
		fclose(filp);

	return ret;
}

