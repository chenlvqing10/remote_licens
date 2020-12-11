/*
 * sample_face_det.cpp
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
#include <cstring>
#include <signal.h>

#include "nna_api.h"
#include "vx_ext_amd.h"
#include "common.h"

#define NNA_SET_DEBUG_LEVEL 0xa200
#define ONCE 1
#define CIRCLE 3
int DET_MODE = CIRCLE;
int frame_index = 0;
nna_feature_map_t ftmap[3];

vx_context context;
nna_fn_t *nna_hdl;

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
	int score0;
	int score1;
	int dxy[4];
	int area;
	int exist;
	int pts[10];
} box_t;

void save_result(vx_uint8 *src, int length, const char *filename)
{
	FILE *fin = fopen(filename, "wb");
	fwrite(src, 1, length, fin);
	fclose(fin);
}

int face_det_callback(void *comp, void *data, unsigned int event,
		void *event_data)
{

	printf("face_det_callback\n");

	int i;
	nna_facecb_info_t *det_info;
	nna_fn_t *nna_hdl;
	nna_face_t result[1000];
	box_t box[1000];

	FILE *fin = fopen("/mnt/usb/det_result.txt", "a");

	printf("enter %s\n", __func__);
	printf("nna event: %d\n", event);
	switch (event) {
	case CB_FACEDET_DONE:
	case CB_FACETRK_DONE:
		frame_index++;
		det_info = (nna_facecb_info_t *) event_data;
		if (DET_MODE == ONCE) {
			for (i = 0; i < det_info->rects.num; i++) {
				memcpy(&result[i], &det_info->rects.face_rects[i],
						sizeof(nna_face_t));
				fprintf(fin, "%ld,%ld,%ld,%ld,  %f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
						result[i].rect[0], result[i].rect[1], result[i].rect[2],
						result[i].rect[3], result[i].kpts[0], result[i].kpts[1],
						result[i].kpts[2], result[i].kpts[3], result[i].kpts[4],
						result[i].kpts[5], result[i].kpts[6], result[i].kpts[7],
						result[i].kpts[8], result[i].kpts[9]);
				box[i].x1 = result[i].rect[0];
				box[i].y1 = result[i].rect[1];
				box[i].x2 = result[i].rect[2];
				box[i].y2 = result[i].rect[3];
				for (int j = 0; j < 10; j++)
					box[i].pts[j] = (int) result[i].kpts[j];
			}

			save_result((vx_uint8 *) box, det_info->rects.num * sizeof(box_t),
					"/mnt/usb/result_v.bin");
		}

		printf("detect face %ld\n", det_info->rects.num);
		if (DET_MODE == CIRCLE) {
			nna_hdl = (nna_fn_t *) comp;
			nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_DATA,
					(void*) &ftmap[frame_index % 3]);
			if (frame_index == 9999)
				frame_index = 0;
		}
		break;

	case CB_FACEREC_DONE:
		break;

	default:
		printf("unknow event");
	}
	fclose(fin);

	return 0;
}

int read_yuv(vx_context context, nna_feature_map_t *ftmap, const char *y_name,
		const char *uv_name, int w, int h)
{
	vx_size mem_size = w * h * 3 / 2;
	FILE *fin;
	int flen;
	printf("det mem_size: %d\n", mem_size);
	vx_size heap_handle = vxAllocateDeviceMemory(context, mem_size);

	if (heap_handle == 0) {
		printf("alloc memory error!\n");
	}

	ftmap->buf_hdl.handle_t = heap_handle;
	ftmap->buf_hdl.phyaddr = vxGetDeviceMemoryPhysicalAddress(heap_handle);
	ftmap->buf_hdl.viraddr = vxGetDeviceMemoryVirtualAddress(heap_handle);

	ftmap->buf_hdl.uv_phyaddr = ftmap->buf_hdl.phyaddr + w * h;
	ftmap->buf_hdl.uv_viraddr = ftmap->buf_hdl.viraddr + w * h;

	ftmap->buf_hdl.size = mem_size;

	ftmap->width = w;
	ftmap->height = h;
	ftmap->stride1 = w;
	ftmap->stride2 = w;
	ftmap->fmt = YUV420;

	fin = fopen(y_name, "rb");
	if (fin == NULL) {
		printf("open y channel file fail! %s \n", y_name);
		exit(1);
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);
	fseek(fin, 0, SEEK_SET);
	fread((vx_int8*) ftmap->buf_hdl.viraddr, 1, flen, fin);
	fclose(fin);

	fin = fopen(uv_name, "rb");
	if (fin == NULL) {
		printf("open uv channel file fail!\n");
	} else {
		fseek(fin, 0, SEEK_END);
		flen = ftell(fin);
		fseek(fin, 0, SEEK_SET);
		fread((vx_int8*) ftmap->buf_hdl.uv_viraddr, 1, flen, fin);
		fclose(fin);
	}

	return 0;
}

static void stop(int signo)
{
	printf("%s %d stop!!!! signo %d\n", __func__, __LINE__, signo);

	lomboax_close(nna_hdl);
	vxReleaseContext(&context);
}

int main(int argc, char** argv)
{
	int param = 0;
	int debug = 0;
	int trk_enable = 0;
	int w = 800;
	int h = 1024;
	int c;

	const char *y_name[3] = {
			"/mnt/usb/1_y.bin",
			"/mnt/usb/2_y.bin",
			"/mnt/usb/3_y.bin"
	};
	const char *uv_name[3] = {
			"/mnt/usb/1_uv.bin",
			"/mnt/usb/2_uv.bin",
			"/mnt/usb/3_uv.bin"
	};

	while ((c = getopt(argc, argv, "hH")) != -1) {
		switch (c) {
		case 'h':
			printf("usage: sample_face_det [y_data] [uv_data] [debug] [track]\n");
			return 0;
		}
	}

	if (argc > 1) {
		/* if the path of yuv image is entered, detect the image entered by the user*/
		if (strlen(argv[1]) > 3)
			DET_MODE = ONCE;
	}

	if (argc > 3)
		debug = atoi(argv[3]);

	if (argc > 4)
		trk_enable = atoi(argv[4]);

	context = vxCreateContext();
	ERROR_CHECK_OBJECT(context);

	if (DET_MODE == ONCE) {
		read_yuv(context, &ftmap[0], argv[1], argv[2], w, h);
	} else {
		for (int i = 0; i < 3; i++) {
			read_yuv(context, &ftmap[i], y_name[i], uv_name[i], w, h);
		}
	}

	nna_hdl = (nna_fn_t *) lomboax_open((void*) &param);
	usleep(10);

	nna_facedet_cfg_t nna_cfg;
	nna_cfg.fdet_en = 1;
	nna_cfg.ftracking_en = 1;
	nna_cfg.fquality_en = 1;
	nna_cfg.frec_en = 1;
	nna_cfg.max_det_faces = 5;
	nna_cfg.max_track_frames = 20;
	nna_cfg.visual_track_thresh = 50;
	nna_cfg.max_track_faces = 3;
	nna_cfg.fdet_max_size = 512;
	nna_cfg.fdet_min_size = 80;
	nna_cfg.frame_width = w;
	nna_cfg.frame_height = h;
	int run_mode = BATCH_REGISTER_MODE;

	int result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, (void *) &nna_cfg);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_INIT, (void*) &param);

	usleep(50);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_CALLBACK,
				(void *) face_det_callback);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_TRK_ENABLE,
				(void*) &trk_enable);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_RUN_MODE,
				(void *) &run_mode);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_DEBUG_LEVEL, (void*) &debug);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_RUN, (void*) &param);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_DATA,
				(void*) &ftmap[frame_index]);

	usleep(5000);

	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACETRK_RUN, (void*) &param);

	signal(SIGINT, stop);
	signal(SIGTERM, stop);

	while (1) {
		usleep(50000);
	}

	stop(-1);

	return 0;
}
