/*
 * sample_rec.cpp
 *
 * Copyright (C) 2016 - 2019, LomboTech Co.Ltd.
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

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>

#include "nna_api.h"
#include "common.h"
#include "vx_ext_amd.h"

void save(vx_uint8 *src, int length, char *filename)
{
	FILE *fin = fopen(filename, "wb");
	fwrite(src, 1, length, fin);
	fclose(fin);
}

int face_reg_callback(void *comp, void *data, unsigned int event,
		void *event_data)
{
	nna_face_feature_t *face_fet = (nna_face_feature_t*) event_data; //nna_face_feature_t

	printf("face_det_callback\n");
	if (CB_FACEREC_DONE == event) {
		printf("num %ld\n", face_fet->feature_size);

		printf("fet %x,%x,%x,%x\n", face_fet->feature_ref[0],
				face_fet->feature_ref[1], face_fet->feature_ref[2],
				face_fet->feature_ref[3]);
	}

	char result[100];

	strcpy(result, "/mnt/reg_result.bin");
	save((vx_uint8 *) face_fet->feature_ref, 512, result);
	usleep(1000);

	return 0;
}

int main(int argc, char** argv)
{
	int param = 0;
	nna_mem_handle_t nna_mem;
	int rect[4];
	float pts[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	FILE *fin;
	int flen;
	int i = 0;

	if (argc < 4) {
		printf("usage: sample_rec Y_DATA UV_DATA KEY_POINT_DATA\n");
		return 0;
	}

	fin = fopen(argv[3], "rb");

	/* get the key points information and coordinates of existing face */
	fscanf(fin, "%d,%d,%d,%d,  %f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", &rect[0],
			&rect[1], &rect[2], &rect[3], &pts[0], &pts[1], &pts[2], &pts[3],
			&pts[4], &pts[5], &pts[6], &pts[7], &pts[8], &pts[9]);

	printf("x1:%d, y1:%d, x2:%d, y2:%d\n", rect[0], rect[1], rect[2], rect[3]);

	printf("key points:");
	for (int j = 0; j < 10; j++) {
		printf("%f,", pts[j]);
		if (j % 2)
			printf("  ");
	}

	vx_context context = vxCreateContext();
	ERROR_CHECK_OBJECT(context);

	/* allocate memory */
	vx_size mem_size = 800 * 1024 + 800 * 1024 / 2;
	vx_size heap_handle = vxAllocateDeviceMemory(context, mem_size);

	if (heap_handle == 0) {
		printf("alloc memory error!\n");
	}

	nna_mem.handle_t = heap_handle;
	nna_mem.phyaddr = vxGetDeviceMemoryPhysicalAddress(heap_handle);
	nna_mem.viraddr = vxGetDeviceMemoryVirtualAddress(heap_handle);
	nna_mem.uv_phyaddr = nna_mem.phyaddr + 800 * 1024;
	nna_mem.uv_viraddr = nna_mem.viraddr + 800 * 1024;
	nna_mem.size = mem_size;

	fin = fopen(argv[1], "rb");
	if (fin == NULL) {
		printf("open image file fail!\n");
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);
	fseek(fin, 0, SEEK_SET);
	fread((vx_int8*) nna_mem.viraddr, 1, flen, fin);
	fclose(fin);

	fin = fopen(argv[2], "rb");
	if (fin == NULL) {
		printf("open image file fail!\n");
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);
	fseek(fin, 0, SEEK_SET);
	fread((vx_int8*) nna_mem.uv_viraddr, 1, flen, fin);
	fclose(fin);

	nna_face_rib_t face_rib;
	nna_feature_map_t ftmap;
	nna_face_t face_rects;

	ftmap.fmt = YVU420;
	ftmap.width = 800;
	ftmap.height = 1024;
	ftmap.channal = 3;
	ftmap.timestamp = 0;
	ftmap.buf_hdl = nna_mem;

	//126, 77, 513, 594
	face_rects.rect[0] = rect[0];
	face_rects.rect[1] = rect[1];
	face_rects.rect[2] = rect[2];
	face_rects.rect[3] = rect[3];

	for (i = 0; i < 10; i++)
		face_rects.kpts[i] = pts[i];

	face_rib.ftmap = ftmap;
	face_rib.rois = face_rects;

	nna_fn_t *nna_hdl = (nna_fn_t *) lomboax_open((void*) &param);
	usleep(10);
	int result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_INIT, (void*) &param);

	usleep(50);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_CALLBACK,
				(void *) face_reg_callback);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEREC_RUN, (void*) &param);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEREC_DATA,
				(void*) &face_rib);

	while (1) {
		usleep(10000000);

	}
	return 0;
}
