/*
 * jpeg_enc_test.c
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "JPEG_ENC"
#include "log/log.h"

#include "lombo_malloc.h"
#include "lombo_jpegenc_plugin.h"
#include "jpeg_hal.h"

static int out_width = 1920;
static int out_height = 1088;
static int in_width = 1920;
static int in_height = 1088;

static FILE *filp_out_raw;
static FILE *filp_in;

#define STREAM_BUF_SIZE (2*1024*1024)//(800*1024)
#define JPEG_INFO_SIZE  (8*1024)

int main(int argc, char *argv[])
{
	FILE *flip = NULL;
	char file_name[1024];
	unsigned int input_buf_len_y, input_buf_len_c;
	jpeg_enc_capbuf_t capbuf;
	jpeg_enc_packet_t packet;
	jpeg_enc_parm_t encode_parm;
	void *handle;

	input_buf_len_y = in_height * ((in_width + 15) / 16 * 16);
	input_buf_len_c = (in_height / 2) * ((in_width / 2 + 7) / 8 * 8);

	ALOGD(" main\n");

	flip = fopen("/mnt/usb/list.txt", "rb");
	if (flip == NULL) {
		ALOGE("no list.txt\n");
		return -1;
	}

	handle = hal_jpeg_enc_init();

	while (fgets((char *)file_name, 1024, flip) != NULL) {
		char fileout[1024] = "/mnt/usb/";
		char *p1 = NULL;
		char *p2 = NULL;
		int len = strlen(file_name);
		int ret = 0;

		if ((file_name[len - 2] == '\r') &&
			((file_name[len - 1] == '\n')))
			file_name[len - 2] = '\0';

		p1 = strstr((char *)file_name, "/mnt/usb/");
		p1 = p1 + strlen("/mnt/usb/");
		p2 = strstr((char *)file_name, ".yuv");
		memcpy((char *)fileout + strlen((char *)fileout), p1, p2 - p1);
		strcat((char *)fileout, "_out");
		strcat((char *)fileout, ".jpg");

		ALOGD(" -- filename : %s -- out file : %s\n",
			file_name, fileout);

		filp_in = fopen(file_name, "rb");
		if (filp_in == NULL) {
			ALOGE(" open filp_in error\n ");
			continue;
		}

		filp_out_raw = fopen(fileout, "wb");
		if (filp_out_raw == NULL) {
			ALOGE(" open filp_out_raw error\n");
			continue;
		}

		packet.vir_addr = (unsigned char *)lombo_malloc(
			STREAM_BUF_SIZE, MEM_VIRT, NULL, __FILE__, __LINE__);
		if (packet.vir_addr == NULL)
			ALOGE("packet.vir_addr malloc faild");
		packet.buf_size = STREAM_BUF_SIZE;

		capbuf.vir_addr[0] = (unsigned char *)lombo_malloc(
			input_buf_len_y, MEM_UC, &(capbuf.phy_addr[0]),
			__FILE__, __LINE__);
		if (capbuf.vir_addr[0] == NULL)
			ALOGE("capbuf.vir_addr[0] malloc faild");
		capbuf.vir_addr[1] = (unsigned char *)lombo_malloc(
			input_buf_len_c, MEM_UC, &(capbuf.phy_addr[1]),
			__FILE__, __LINE__);
		if (capbuf.vir_addr[1] == NULL)
			ALOGE("capbuf.vir_addr[1] malloc faild");
		capbuf.vir_addr[2] = (unsigned char *)lombo_malloc(
			input_buf_len_c, MEM_UC, &(capbuf.phy_addr[2]),
			__FILE__, __LINE__);
		if (capbuf.vir_addr[2] == NULL)
			ALOGE("capbuf.vir_addr[2] malloc faild");
		fread(capbuf.vir_addr[0], 1, input_buf_len_y, filp_in);
		fread(capbuf.vir_addr[1], 1, input_buf_len_c, filp_in);
		fread(capbuf.vir_addr[2], 1, input_buf_len_c, filp_in);

		encode_parm.in_width = in_width;
		encode_parm.in_height = in_height;
		encode_parm.in_stride = (in_width + 15) / 16 * 16;
		encode_parm.out_width = out_width;
		encode_parm.out_height = out_height;
		encode_parm.input_mode = 0;

		ret = hal_jpeg_enc_frame(handle, &capbuf, &packet, &encode_parm);
		if (ret)
			ALOGE("jpeg_enc_frame error %d\n",ret);


		ALOGD("-------- packet.data_size = 0x%x\n", packet.stream_length);
		fwrite(packet.vir_addr, 1, packet.stream_length, filp_out_raw);

		if (packet.vir_addr != NULL)
			lombo_free(packet.vir_addr, MEM_VIRT);
		if (capbuf.vir_addr[0] != NULL)
			lombo_free(capbuf.vir_addr[0], MEM_UC);
		if (capbuf.vir_addr[1] != NULL)
			lombo_free(capbuf.vir_addr[1], MEM_UC);
		if (capbuf.vir_addr[2] != NULL)
			lombo_free(capbuf.vir_addr[2], MEM_UC);

		fclose(filp_in);
		fclose(filp_out_raw);
	}

	hal_jpeg_enc_deinit(handle);

	fclose(flip);
	ALOGD(" encode end end\n");
	return 0;
}
