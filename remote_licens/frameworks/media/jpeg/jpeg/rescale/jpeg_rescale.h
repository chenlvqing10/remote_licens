#ifndef __JPEG_RESCALE_H__
#define __JPEG_RESCALE_H__
#include "lombo_jpegdec_plugin.h"

int NV12ToARGB888(struct jpeg_dec_output *output, unsigned char *rgb_buf);
int jpeg_output_adjust(jpeg_dec_output_buf_t *output, jpeg_dec_parm_t *dec_parm,
	unsigned int dest_width, unsigned int dest_height,
	unsigned int stride_width, unsigned int stride_height,
	unsigned int unlimited_flag);
#endif
