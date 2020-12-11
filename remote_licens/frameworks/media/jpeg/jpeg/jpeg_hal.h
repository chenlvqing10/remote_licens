/*
 * jpeg.h
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

#ifndef __JPEG_HAL_H__
#define __JPEG_HAL_H__
#include "lombo_jpegenc_plugin.h"
#include "lombo_jpegdec_plugin.h"

void *hal_jpeg_enc_init(void);
int hal_jpeg_enc_frame(void *hdl,
			jpeg_enc_capbuf_t *jpeg_enc_capbuf,
			jpeg_enc_packet_t *jpeg_enc_packet,
			jpeg_enc_parm_t *jpeg_enc_parm);
int hal_jpeg_enc_deinit(void *hdl);

void *hal_jpeg_dec_init(void);
int hal_jpeg_dec_frame(void *hdl,
			jpeg_dec_packet_t *jpeg_dec_packet,
			jpeg_dec_output_buf_t *jpeg_dec_output_buf,
			jpeg_dec_parm_t *jpeg_dec_parm);
int hal_jpeg_dec_read_header(void *hdl, jpeg_dec_packet_t *jpeg_dec_packet,
			jpeg_dec_header_info_t *jpeg_dec_info);
int hal_jpeg_dec_deinit(void *hdl);

#endif

