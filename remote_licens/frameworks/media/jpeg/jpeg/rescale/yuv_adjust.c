#include "lombo_jpegdec_plugin.h"
#include "NV12_resize.h"
#include "oscl_mem.h"
#include "jpeg_dec.h"
#define LOG_TAG "JPEG_RESCALE"
#include "log/log.h"

static void yuvToArgb(unsigned char y, unsigned char u,
			unsigned char v, unsigned char *p) {
	int r, g, b;
	char a = 255;
	r = (int)((y & 0xff) + 1.4075 * ((v & 0xff) - 128));
	g = (int)((y & 0xff) - 0.3455 * ((u & 0xff) - 128) - 0.7169*((v & 0xff) - 128));
	b = (int)((y & 0xff) + 1.779 * ((u & 0xff) - 128));
	r = (r < 0 ? 0 : r > 255 ? 255 : r);
	g = (g < 0 ? 0 : g > 255 ? 255 : g);
	b = (b < 0 ? 0 : b > 255 ? 255 : b);
	*p++ = (unsigned char)a;
	*p++ = (unsigned char)r;
	*p++ = (unsigned char)g;
	*p++ = (unsigned char)b;
}

int NV12ToARGB888(struct jpeg_dec_output *output, unsigned char *rgb_buf)
{
	int i, j;
	unsigned char *position_y = output->addr[0];
	unsigned char *position_u = output->addr[1];
	unsigned char *position_v = output->addr[2], *position_u_start, *cour_u;
	int width = output->stride_width;
	int height = output->stride_height;
	if (output == NULL) {
		ALOGE("buf error.\n");
		return -1;
	}
	if (width <= 0 || height <= 0) {
		ALOGE("width or height error.\n");
		return -1;
	}
	for (i = 0; i < height; i++) {
		position_u_start = position_u + i / 2 * width;
		for (j = 0; j < width; j++) {
			cour_u = position_u_start + j / 2 * 2;
			position_v = cour_u + 1;
			yuvToArgb(*position_y++, *cour_u, *position_v, rgb_buf);
			rgb_buf += 4;
		}
	}
	return 0;
}

int YV12ToARGB888(struct jpeg_dec_output *output, unsigned char *rgb_buf,
				unsigned int width, unsigned int height)
{
	int num_pixel = output->size[0];
	int position_u = output->size[1];
	int position_v = output->size[2];
	int i, j, y, u, v, start_y, start_u, start_v, step;
	for (i = 0; i < height; i++) {
		start_y = i*width;
		step = (i / 2)*width / 2;
		start_v = position_v + step;
		start_u = position_u + step;
		for (j = 0; j < width; j++) {
			y = start_y + j;
			u = start_u + j/2;
			v = start_v + j/2;
			yuvToArgb(*(output->addr[0] + y), *(output->addr[1] + u),
						*(output->addr[2] + v), rgb_buf);
		}
	}
	return 0;
}

int jpeg_yuv_rotate(jpeg_dec_output_buf_t *output, jpeg_dec_output_buf_t *src,
	unsigned int format, unsigned int crop_width, unsigned int crop_heigh,
	unsigned int width_stride, unsigned int height, unsigned int rotate_degree)
{
	int i, j, m = 0, k = 0;
	unsigned char *des_luma, *src_luma;
	unsigned char *src_u, *src_v, *des_u, *des_v, *u_ptr, *v_ptr;
	unsigned int pic_width, pic_heigh, chroma_stride = 0;
	unsigned int pic_width_ch, uv_aligned;
	int unit;

	if (src->vir_addr == NULL || output->vir_addr == NULL ||
		src->offset[1] == 0 || output->offset[1] == 0) {
		ALOGE("parm error\n");
		return -1;
	}

	src_luma = src->vir_addr;
	des_luma = output->vir_addr;

	pic_width = crop_width&(~1);
	pic_heigh = crop_heigh&(~1);

	switch (format) {
	case JPEG_NV12:
		src_u = (unsigned char *)(src->vir_addr + src->offset[1]);
		src_v = (unsigned char *)(src_u + 1);
		des_u = (unsigned char *)(output->vir_addr + output->offset[1]);
		des_v = (unsigned char *)(des_u + 1);
		chroma_stride = width_stride;
		pic_width_ch = pic_width;
		unit = 2;
		break;
	case JPEG_NV21:
		src_v = (unsigned char *)(src->vir_addr + src->offset[1]);
		src_u = (unsigned char *)(src_v + 1);
		des_v = (unsigned char *)(output->vir_addr + output->offset[1]);
		des_u = (unsigned char *)(des_v + 1);
		chroma_stride = width_stride;
		pic_width_ch = pic_width;
		unit = 2;
		break;
	case JPEG_YV12:
		src_u = (unsigned char *)(src->vir_addr + src->offset[1]);
		src_v = (unsigned char *)(src->vir_addr + src->offset[2]);
		des_u = (unsigned char *)(output->vir_addr + output->offset[1]);
		des_v = (unsigned char *)(output->vir_addr + output->offset[2]);
		chroma_stride = width_stride/2;
		pic_width_ch = pic_width/2;
		unit = 1;
		break;
	case JPEG_YV21:
		src_v = (unsigned char *)(src->vir_addr + src->offset[1]);
		src_u = (unsigned char *)(src->vir_addr + src->offset[2]);
		des_v = (unsigned char *)(output->vir_addr + output->offset[1]);
		des_u = (unsigned char *)(output->vir_addr + output->offset[2]);
		chroma_stride = width_stride/2;
		pic_width_ch = pic_width/2;
		unit = 1;
		break;
	default:
		ALOGE("unsupported format\n");
		return -1;
	}



	switch (rotate_degree) {
	case 90:
		for (i = pic_width - 1; i >= 0; i--) {
			for (j = 0; j < pic_heigh; j++) {
				des_luma[k] = src_luma[width_stride*j + i];
				k++;
			}
			m += height;
			k = m;
		}

		m = k = 0;
		uv_aligned = height*unit/2;

		for (i = pic_width_ch - unit; i >= 0; i -= unit) {
			for (j = 0; j < pic_heigh/2; j++) {
				u_ptr = src_u + chroma_stride*j + i;
				v_ptr = src_v + chroma_stride*j + i;
				des_u[k] = *u_ptr;
				des_v[k] = *v_ptr;
				k += unit;
			}
			m += uv_aligned;
			k = m;
		}
		break;
	case 180:
		for (i = pic_heigh; i > 0; i--) {
			for (j = 1 + (width_stride - pic_width);
							j <= width_stride; j++) {
				des_luma[k] = src_luma[width_stride*i - j];
				k++;
			}
			m += width_stride;
			k = m;
		}

		m = k = 0;
		uv_aligned = width_stride*unit/2;

		for (i = pic_heigh/2; i > 0; i--) {
			for (j = unit + (chroma_stride - pic_width_ch);
							j <= chroma_stride; j += unit) {
				u_ptr = src_u + chroma_stride*i - j;
				v_ptr = src_v + chroma_stride*i - j;
				des_u[k] = *u_ptr;
				des_v[k] = *v_ptr;
				k += unit;
			}
			m += uv_aligned;
			k = m;
		}
		break;
	case 270:
		for (i = 0; i < pic_width; i++) {
			for (j = pic_heigh - 1; j >= 0; j--) {
				des_luma[k] = src_luma[width_stride*j + i];
				k++;
			}
			m += height;
			k = m;
		}

		m = k = 0;
		uv_aligned = height*unit/2;

		for (i = 0; i < pic_width_ch; i += unit) {
			for (j = pic_heigh/2 - 1; j >= 0; j--) {
				u_ptr = src_u + chroma_stride*j + i;
				v_ptr = src_v + chroma_stride*j + i;
				des_u[k] = *u_ptr;
				des_v[k] = *v_ptr;
				k += unit;
			}
			m += uv_aligned;
			k = m;
		}
		break;
	default:
		ALOGE("no this rotate option");
		return -1;
	}
	return 0;


}

int jpeg_resize_output(jpeg_dec_output_buf_t *output, jpeg_dec_output_buf_t *src,
	jpeg_dec_parm_t *dec_parm, unsigned int dest_width,
	unsigned int dest_height, unsigned int stride_width,
	unsigned int scale_mode)
{
	unsigned int scale_denom, FactorX, FactorY, ratio;
	unsigned int image_width, image_height, scale_width, scale_height;
	unsigned int actual_width, actual_height;
	structConvImage input_image;
	structConvImage output_image;
	int ret;
	image_width = dec_parm->width_stride;
	image_height = dec_parm->height_aligned;

	ratio = dec_parm->down_scale_ratio;
	actual_width = (dec_parm->crop_width >> ratio)&(~1);
	actual_height = (dec_parm->crop_height >> ratio)&(~1);

	if (actual_width > image_width ||
			actual_height > image_height) {
		ALOGE("pic size error!\n");
		return -1;
	}

	if (scale_mode) {
		scale_width = dest_width;
		scale_height = dest_height;
	} else {
		if (actual_width > dest_width ||
			actual_height > dest_height) {
			FactorX = (actual_width << 9) / dest_width;
			FactorY = (actual_height << 9) / dest_height;
			scale_denom = (FactorX > FactorY) ? FactorX : FactorY;
			if (scale_denom & 0x3ff)
				scale_denom = (scale_denom >> 9) + 1;
			else
				scale_denom = scale_denom >> 9;

			scale_width = (actual_width / scale_denom)&(~1);
			scale_height = (actual_height / scale_denom)&(~1);
		} else {
			scale_width = actual_width;
			scale_height = actual_height;
		}

	}

	input_image.eFormat = dec_parm->output_format;

	input_image.imgPtr = src->vir_addr;
	input_image.clrPtr = src->vir_addr + src->offset[1];
	input_image.clrPtr2 = src->vir_addr + src->offset[2];
	input_image.uOffset = 0;
	input_image.uWidth = actual_width;
	input_image.uStride = dec_parm->width_stride;
	input_image.uHeight = actual_height;

	output_image.imgPtr = output->vir_addr;
	output_image.clrPtr = output->vir_addr + output->offset[1];
	output_image.clrPtr2 = output->vir_addr + output->offset[2];
	output_image.uOffset = 0;
	output_image.eFormat = input_image.eFormat;
	output_image.uWidth = scale_width;
	output_image.uStride = stride_width;
	output_image.uHeight = scale_height;

	ret = VT_resizeFrame_Video_opt2_lp(&input_image, &output_image, NULL, 0);
	if (ret)
		return 0;
	return -1;
}

static int swap_uint(unsigned *num1, unsigned int *num2)
{
	unsigned int swap_temp;

	swap_temp = *num1;
	*num1 = *num2;
	*num2 = swap_temp;
	return 0;
}

int jpeg_output_adjust(jpeg_dec_output_buf_t *output, jpeg_dec_parm_t *dec_parm,
	unsigned int dest_width, unsigned int dest_height,
	unsigned int stride_width, unsigned int stride_height,
	unsigned int unlimit_flag)
{
	jpeg_dec_output_buf_t src, adjust_output;
	unsigned int buf_len, size_luma, yuv_size, aligned_bytes, yuv_format;
	unsigned int image_width, image_height, status = 0, rotate_degree = 0;
	unsigned int out_stride, out_height, crop_width, crop_height, scale_mode = 0;
	unsigned int ratio;
	int ret = 0;

	memcpy(&src, output, sizeof(jpeg_dec_output_buf_t));

	image_width = dec_parm->width_stride;
	image_height = dec_parm->height_aligned;
	ratio = dec_parm->down_scale_ratio;
	crop_width = (dec_parm->crop_width >> ratio)&(~1);
	crop_height = (dec_parm->crop_height >> ratio)&(~1);

	out_stride = dest_width;
	out_height = dest_height;

	if (stride_width > dest_width)
		out_stride = stride_width;

	if (stride_height > dest_height)
		out_height = stride_height;

	size_luma = out_stride*out_height;

	aligned_bytes = ((size_luma + 0x3ff)&(~0x3ff)) - size_luma;
	yuv_size = size_luma * 3 / 2 + aligned_bytes;

	buf_len = src.offset[2] * 2 - src.offset[1];
	if (!unlimit_flag) {
		if (buf_len < yuv_size) {
			ALOGE("buffer size error");
			return -1;
		}
	}

	/* rotate */
	if (dec_parm->rotation_degree != 0) {
		rotate_degree = 360 - dec_parm->rotation_degree;
		memcpy(&adjust_output, &src, sizeof(jpeg_dec_output_buf_t));
		buf_len = src.offset[2] * 2 - src.offset[1];
		adjust_output.vir_addr = oscl_malloc_cached(buf_len);
		if (adjust_output.vir_addr == NULL) {
			ALOGE("malloc buffer error");
			return -1;
		}
		yuv_format = dec_parm->output_format;

		ret = jpeg_yuv_rotate(&adjust_output, &src, yuv_format,
						crop_width, crop_height, image_width,
						image_height, rotate_degree);
		if (ret != 0) {
			ALOGE("rotate error");
		} else {
			oscl_free_cached(src.vir_addr);
			if (rotate_degree == 90 || rotate_degree == 270) {
				swap_uint(&dec_parm->crop_width,
							&dec_parm->crop_height);
				swap_uint(&dec_parm->width_stride,
							&dec_parm->height_aligned);
				image_width = dec_parm->width_stride;
				image_height = dec_parm->height_aligned;
			}

			memcpy(&src, &adjust_output, sizeof(jpeg_dec_output_buf_t));
			status++;
		}
	}

	/* scale */
	if (dest_width != 0 && dest_height != 0) {
		if ((image_width > dest_width || image_height > dest_height)
			|| unlimit_flag) {
				adjust_output.vir_addr = oscl_malloc_cached(yuv_size);
				if (adjust_output.vir_addr == NULL) {
					ALOGE("malloc buffer error");
					return -1;
				}
				adjust_output.offset[0] = 0;
				adjust_output.offset[1] = size_luma + aligned_bytes;
				adjust_output.offset[2] = adjust_output.offset[1]
								+ size_luma/4;
				if (unlimit_flag)
					scale_mode = 1;

				ret = jpeg_resize_output(&adjust_output, &src, dec_parm,
							dest_width, dest_height,
							out_stride, scale_mode);
				if (ret != 0) {
					ALOGE("resize error\n");
				} else {
					oscl_free_cached(src.vir_addr);
					status++;
					dec_parm->width_stride = out_stride;
					dec_parm->height_aligned = out_height;
				}
			}
	}

	if (status > 0) {
		adjust_output.phy_addr = oscl_virt_to_phys(adjust_output.vir_addr);
		oscl_cache_flush(adjust_output.phy_addr);
		memcpy(output, &adjust_output, sizeof(jpeg_dec_output_buf_t));
		ALOGD("scale to %dx%d, rotate %d°",
		dest_width, dest_height, rotate_degree);
	}
		return ret;
}
