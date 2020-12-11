#include "stdio.h"
#define LOG_TAG "YUV_BLENDING"
#include <log/log.h>
#include "yuv_blending.h"

#define BT601_C0 0.257
#define BT601_C1 0.504
#define BT601_C2 0.098
#define BT601_C3 0.148
#define BT601_C4 0.291
#define BT601_C5 0.439
#define BT601_C6 0.439
#define BT601_C7 0.368
#define BT601_C8 0.071
#define RGBTOYUV_FIXBIT 11
#define CONVERT_COLOR_BLACK_GROUND_TH 64
#define CONVERT_COLOR_WHITE_GROUND_TH 192

#define ICLIP3(min, max, var) (var < min ? min : (var > max ? max : var))

typedef struct rgb_format_def {
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
} rgb_format;

void rgbtoyuv_blending(rgb_format *psrc, unsigned char *y, unsigned char *u, unsigned char *v)
{
	int offset_y, offset_uv;
	int coef[9];
	int temp;

	/* BT601 */
	{
		offset_y = 16 << 4;
		offset_uv = 128 << 4;
		coef[0] = (int)(BT601_C0 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
		coef[1] = (int)(BT601_C1 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
		coef[2] = (int)(BT601_C2 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
		coef[3] = (int)(BT601_C3 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
		coef[4] = (int)(BT601_C4 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
		coef[5] = (int)(BT601_C5 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
		coef[6] = (int)(BT601_C6 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
		coef[7] = (int)(BT601_C7 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
		coef[8] = (int)(BT601_C8 * (1 << RGBTOYUV_FIXBIT) + 1) >> 1;
	}

	temp = ((psrc->r * coef[0] >> 6) + (psrc->g * coef[1] >> 6) + (psrc->b * coef[2] >> 6) + offset_y) >> 4;
	*y = ICLIP3(0, 255, temp);

	temp = (-(psrc->r * coef[3] >> 6) - (psrc->g * coef[4] >> 6) + (psrc->b * coef[5] >> 6) + offset_uv) >> 4;
	*u= ICLIP3(0, 255, temp);

	temp = ((psrc->r * coef[6] >> 6) - (psrc->g * coef[7] >> 6) - (psrc->b * coef[8] >> 6) + offset_uv) >> 4;
	*v = ICLIP3(0, 255, temp);
}

void blending_operation(yuv_blending_t *yuv_blending)
{
	int x, y;
	int j, i;
	int m, n;
	int start_x, end_x, start_y, end_y;
	unsigned char *p_dst, *p_dst_u, *p_dst_v;
	unsigned char pixel_y, pixel_u, pixel_v;
	rgb_format *psrc;
	rgb_format pdst_rgb;
	/* int luma_size; */
	int temp;
	int sum_y,average_y;
	int block_number;
	int convert_color = 0;
	/* int total_size; */
	unsigned char *blending_addr = NULL;
	unsigned char *buffer_y_addr = NULL;
	unsigned char *buffer_uv_addr = NULL;
	unsigned int width_stride;
	/* unsigned int height_aligned; */
	unsigned int blending_stride;

	blending_addr = yuv_blending->blending_data;
	buffer_y_addr = (unsigned char *)yuv_blending->source.buffer_y;
	buffer_uv_addr = (unsigned char *)yuv_blending->source.buffer_uv;

	width_stride = yuv_blending->source.width_stride;
	/* height_aligned = yuv_blending->source.height_aligned; */

	blending_stride = yuv_blending->blending_stride;

	/* luma_size = width_stride * height_aligned; */
	start_x = yuv_blending->blending_x_pos / 16;
	start_y = yuv_blending->blending_y_pos / 16;
	end_x = (yuv_blending->blending_x_pos + yuv_blending->blending_width) / 16;
	end_y = (yuv_blending->blending_y_pos + yuv_blending->blending_height) / 16;

	/* total_size = blending_stride * (end_y - start_y) * 16; */

	for (y = start_y; y < end_y; y++) {
		for (x = start_x; x < end_x ; x++) {
			for (block_number = 0; block_number < 16 ; block_number++) {
				for (j = 0; j < 4; j++) {
					for (i = 0; i < 4 ; i++) {
						p_dst = buffer_y_addr +
						       y * 16 * width_stride +
						       x * 16 +
						       ((block_number / 4) * 4 + j) * width_stride +
						       (block_number & 0x03) * 4 + i;

						if (!(i % 2 || j % 2)) {
							p_dst_u = buffer_uv_addr +
								 (y * 16 * width_stride / 2 + x * 16 + ((block_number / 4) * 4 + j) * width_stride / 2 + (block_number & 0x03) * 4 + (i / 2) * 2);
							p_dst_v = p_dst_u + 1;
						}

						psrc = (rgb_format *)(blending_addr + (y - start_y) * 16 * blending_stride + (x - start_x) * 16 * 4 + ((block_number / 4) * 4 + j) * blending_stride + ((block_number & 0x03) * 4 + i) * 4);

						if ((i == 0) && (j == 0)) {
							if (yuv_blending->text_color != unknow_text) {
								sum_y = 0;
								for (m = 0; m < 4 ; m++)
									for (n = 0; n < 4 ; n++)
										sum_y += p_dst[m * width_stride + n];

									average_y = sum_y >> 4;
									if (yuv_blending->text_color == black_text) {
										/* convert to black background and white text */
										if (average_y < CONVERT_COLOR_BLACK_GROUND_TH)
											convert_color = 1;
										else
											convert_color = 0;
									} else if (yuv_blending->text_color == white_text) {
										/* convert to white background and black text */
										if (average_y > CONVERT_COLOR_WHITE_GROUND_TH)
											convert_color = 1;
										else
											convert_color = 0;
									}
							} else
								convert_color = 0;
						}

						/* RGB888 */
						switch (yuv_blending->colorspace) {
						case ARGB8888: {
							/* ARGB8888 */
							pdst_rgb.a = psrc->a;
							pdst_rgb.r = psrc->r;
							pdst_rgb.g = psrc->g;
							pdst_rgb.b = psrc->b;
							break;
						}
						case RGBA8888: {
							/* RGBA8888 */
							pdst_rgb.a = psrc->b;
							pdst_rgb.r = psrc->a;
							pdst_rgb.g = psrc->r;
							pdst_rgb.b = psrc->g;
							break;
						}
						case ABGR8888: {
							/* ABGR8888 */
							pdst_rgb.a = psrc->a;
							pdst_rgb.r = psrc->b;
							pdst_rgb.g = psrc->g;
							pdst_rgb.b = psrc->r;
							break;
						}
						case BGRA8888: {
							/* BGRA8888 */
							pdst_rgb.a = psrc->b;
							pdst_rgb.r = psrc->g;
							pdst_rgb.g = psrc->r;
							pdst_rgb.b = psrc->a;
							break;
						}
						}

						rgbtoyuv_blending(&pdst_rgb, &pixel_y, &pixel_u, &pixel_v);
						if (convert_color)
							pixel_y = 255 - pixel_y;

						temp = (pdst_rgb.a * pixel_y + (255 - pdst_rgb.a) * (*p_dst) + 128) >> 8;
						*p_dst = ICLIP3(0, 255, temp);

						/* yuv420 */
						if (!(i % 2 || j % 2)) {
						    temp = (pdst_rgb.a * pixel_u + (255 - pdst_rgb.a) * (*p_dst_u) + 128) >> 8;
						    *p_dst_u = ICLIP3(0, 255, temp);
						    temp = (pdst_rgb.a * pixel_v + (255 - pdst_rgb.a) * (*p_dst_v) + 128) >> 8;
						    *p_dst_v = ICLIP3(0, 255, temp);
						}
					}
				}
			}
		}
	}
}