/*
 * jpeg_hal.c
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
#include <dlfcn.h>

#define LOG_TAG "JPEG"
#include "log/log.h"
#include "jpeg_hal.h"

typedef struct {
	void *so_hdl;
	void *jpg_hdl;
	void *encode;
	void *decode;
	void *read_header;
	void *deinit;
} __jpg_hal_hdl_t;

static int __jpg_deinit(__jpg_hal_hdl_t *hdl)
{
	int (*_deinit)(void *) = NULL;
	__jpg_hal_hdl_t *jpg_hal_hdl = hdl;
	JPEGENC_STATUS ret = -1;

	if (jpg_hal_hdl == NULL)
		goto exit;
	_deinit = jpg_hal_hdl->deinit;
	if (_deinit && jpg_hal_hdl->jpg_hdl)
		ret = _deinit(jpg_hal_hdl->jpg_hdl);
	if (jpg_hal_hdl->so_hdl)
		dlclose(jpg_hal_hdl->so_hdl);
	if (jpg_hal_hdl)
		free(jpg_hal_hdl);
exit:
	return ret;
}
static __jpg_hal_hdl_t *__jpg_init(char *so_name, char *init_name,
	char *deinit_name, char *enc_name, char *dec_name, char *read_name)
{
	__jpg_hal_hdl_t *jpg_hal_hdl = NULL;
	void *(*_init)(void);
	jpg_hal_hdl = malloc(sizeof(__jpg_hal_hdl_t));
	if (jpg_hal_hdl == NULL || init_name == NULL || deinit_name == NULL)
		goto exit;
	memset(jpg_hal_hdl, 0, sizeof(__jpg_hal_hdl_t));

	jpg_hal_hdl->so_hdl = dlopen(so_name, RTLD_LAZY);
	if (jpg_hal_hdl->so_hdl == NULL) {
		ALOGE("Failed to open lib:%s, %s", so_name, dlerror());
		goto exit;
	}
	_init = dlsym(jpg_hal_hdl->so_hdl, init_name);
	jpg_hal_hdl->deinit = dlsym(jpg_hal_hdl->so_hdl, deinit_name);
	if (enc_name)
		jpg_hal_hdl->encode = dlsym(jpg_hal_hdl->so_hdl, enc_name);
	if (dec_name)
		jpg_hal_hdl->decode = dlsym(jpg_hal_hdl->so_hdl, dec_name);
	if (read_name)
		jpg_hal_hdl->read_header = dlsym(jpg_hal_hdl->so_hdl, read_name);
	if (_init)
		jpg_hal_hdl->jpg_hdl = _init();
exit:
	if (jpg_hal_hdl && jpg_hal_hdl->jpg_hdl == NULL) {
		ALOGE("jpg_hal_hdl:%p, jpghdl:%p", jpg_hal_hdl, jpg_hal_hdl->jpg_hdl);
		__jpg_deinit(jpg_hal_hdl);
		jpg_hal_hdl = NULL;
	}
	return jpg_hal_hdl;

}
void *hal_jpeg_enc_init(void)
{
	__jpg_hal_hdl_t *jpg_hal_hdl;

	jpg_hal_hdl = __jpg_init("libie_jpg.so", "jpeg_enc_init", "jpeg_enc_deinit",
				"jpeg_enc_frame", NULL, NULL);
	if (jpg_hal_hdl != NULL && jpg_hal_hdl->encode == NULL) {
		__jpg_deinit(jpg_hal_hdl);
		jpg_hal_hdl = NULL;
	}
	return jpg_hal_hdl;
}

int hal_jpeg_enc_frame(void *hdl,
			jpeg_enc_capbuf_t *jpeg_enc_capbuf,
			jpeg_enc_packet_t *jpeg_enc_packet,
			jpeg_enc_parm_t *jpeg_enc_parm)
{
	__jpg_hal_hdl_t *jpg_hal_hdl = hdl;
	JPEGENC_STATUS ret = -1;
	JPEGENC_STATUS (*__encode_frame)(void *, jpeg_enc_capbuf_t *,
				jpeg_enc_packet_t *, jpeg_enc_parm_t *) = NULL;
	if (jpg_hal_hdl == NULL)
		return -1;
	__encode_frame = jpg_hal_hdl->encode;
	if (__encode_frame) {
		ret = __encode_frame(jpg_hal_hdl->jpg_hdl, jpeg_enc_capbuf,
					jpeg_enc_packet, jpeg_enc_parm);
	}
	return ret;
}
int hal_jpeg_enc_deinit(void *hdl)
{
	__jpg_hal_hdl_t *jpg_hal_hdl = hdl;
	return __jpg_deinit(jpg_hal_hdl);
}

void *hal_jpeg_dec_init(void)
{
	__jpg_hal_hdl_t *jpg_hal_hdl;

	jpg_hal_hdl = __jpg_init("libid_jpg.so", "jpeg_dec_init", "jpeg_dec_deinit",
				NULL, "jpeg_dec_frame", "jpeg_dec_read_header");
	if (jpg_hal_hdl != NULL) {
		if (jpg_hal_hdl->decode == NULL || jpg_hal_hdl->read_header == NULL) {
			__jpg_deinit(jpg_hal_hdl);
			jpg_hal_hdl = NULL;
		}
	}
	return jpg_hal_hdl;
}

int hal_jpeg_dec_frame(void *hdl,
				jpeg_dec_packet_t *jpeg_dec_packet,
				jpeg_dec_output_buf_t *jpeg_dec_output_buf,
				jpeg_dec_parm_t *jpeg_dec_parm)
{
	__jpg_hal_hdl_t *jpg_hal_hdl = hdl;
	int ret = -1;
	int (*_dec_frame)(void *, jpeg_dec_packet_t *,
			jpeg_dec_output_buf_t *, jpeg_dec_parm_t *);
	if (jpg_hal_hdl == NULL)
		return -1;
	_dec_frame = jpg_hal_hdl->decode;
	if (_dec_frame) {
		ret = _dec_frame(jpg_hal_hdl->jpg_hdl, jpeg_dec_packet,
				jpeg_dec_output_buf, jpeg_dec_parm);
	}
	return ret;
}
int hal_jpeg_dec_read_header(void *hdl, jpeg_dec_packet_t *jpeg_dec_packet,
	jpeg_dec_header_info_t *jpeg_dec_info)
{
	__jpg_hal_hdl_t *jpg_hal_hdl = hdl;
	int ret = -1;
	int (*_read_header)(void *, jpeg_dec_packet_t *, jpeg_dec_header_info_t *);

	if (jpg_hal_hdl == NULL)
		return -1;
	_read_header = jpg_hal_hdl->read_header;
	if (_read_header)
		ret = _read_header(jpg_hal_hdl->jpg_hdl, jpeg_dec_packet, jpeg_dec_info);
	return ret;
}
int hal_jpeg_dec_deinit(void *hdl)
{
	__jpg_hal_hdl_t *jpg_hal_hdl = hdl;
	return __jpg_deinit(jpg_hal_hdl);
}


