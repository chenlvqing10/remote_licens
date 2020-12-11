/*
 * sample_det_batch.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

#include "nna_api.h"
#include "common.h"
#include "tsemaphore.h"
#include "vx_ext_amd.h"

#define BUFFER_SIZE 102400

tsem_t thread_mutex;
int total_file_num;
int detect_face_num;
int noface_file_num;
bool no_face_flag;
char result_file[100];

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

int copyFile(const char *sourceFileNameWithPath,
		const char *targetFileNameWithPath)
{
	FILE *fpR, *fpW;
	char buffer[BUFFER_SIZE];
	int lenR, lenW;
	printf("enter copyFile\n");
	if ((fpR = fopen(sourceFileNameWithPath, "r")) == NULL) {
		printf("The file '%s' can not be opened! \n", sourceFileNameWithPath);
		return -1;
	}
	if ((fpW = fopen(targetFileNameWithPath, "wb")) == NULL) {
		printf("The file '%s' can not be opened! \n", targetFileNameWithPath);
		fclose(fpR);
		return -1;
	}

	memset(buffer, 0, BUFFER_SIZE);
	while ((lenR = fread(buffer, 1, BUFFER_SIZE, fpR)) > 0) {
		if ((lenW = fwrite(buffer, 1, lenR, fpW)) != lenR) {
			printf("Write to file '%s' failed!\n", targetFileNameWithPath);
			fclose(fpR);
			fclose(fpW);
			return -1;
		}
		memset(buffer, 0, BUFFER_SIZE);
	}

	fclose(fpR);
	fclose(fpW);
	return 0;
}

int face_det_callback(void *comp, void *data, unsigned int event,
		void *event_data)
{
	int i;
	nna_facecb_info_t *det_info;
	nna_face_t result[1000];
	total_file_num++;
	FILE *fy = fopen(result_file, "a");
	switch (event) {
	case CB_FACEDET_DONE:
		det_info = (nna_facecb_info_t *) event_data;

		for (i = 0; i < det_info->rects.num; i++) {
			memcpy(&result[i], &det_info->rects.face_rects[i],
					sizeof(nna_face_t));
			fprintf(fy,
					"face No: %d; rect: %ld,%ld,%ld,%ld, keyponits: %f,%f  %f,%f  %f,%f  %f,%f  %f,%f\n",
					i, result[i].rect[0], result[i].rect[1], result[i].rect[2],
					result[i].rect[3], result[i].kpts[0], result[i].kpts[1],
					result[i].kpts[2], result[i].kpts[3], result[i].kpts[4],
					result[i].kpts[5], result[i].kpts[6], result[i].kpts[7],
					result[i].kpts[8], result[i].kpts[9]);
			detect_face_num++;
		}
		if (!det_info->rects.num) {
			fprintf(fy, "******no face found******");
			no_face_flag = 1;
			noface_file_num++;
		} else {
			no_face_flag = 0;
		}
		break;

	case CB_FACEREC_DONE:
		break;
	default:
		printf("unknow event");
	}
	fprintf(fy, "\n");
	fclose(fy);

	tsem_signal(&thread_mutex);
	return 0;
}

void read_raw_data(char *fileName, vx_int8 *addr)
{
	FILE *fin;
	int flen;
	fin = fopen(fileName, "rb");
	if (fin == NULL) {
		printf("open y channel file fail!\n");
		exit(1);
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);
	fseek(fin, 0, SEEK_SET);
	fread(addr, 1, flen, fin);
	fclose(fin);
}

int readFileList_and_process(char *basePath, nna_feature_map_t *ftmap,
		nna_fn_t *nna_hdl)
{
	DIR *dir;
	struct dirent *ptr;
	char *probe_pt;
	char base[100];
	char dst_y[100];
	char dst_uv[100];
	char loss_file[200];

	printf("open dir %s\n", basePath);
	if ((dir = opendir(basePath)) == NULL) {
		perror("Open dir error...");
		exit(1);
	}

	while ((ptr = readdir(dir)) != NULL) {
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)///current dir OR parrent dir
			continue;
		else if (ptr->d_type == 8) {
			/* if it is a file, perform face detection */
			memset(dst_y, 0, 100);
			memset(dst_uv, 0, 100);
			sprintf(dst_y, "%s/%s", basePath, ptr->d_name);
			sprintf(dst_uv, "%s/%s", basePath, ptr->d_name);
			printf("d_name: %s\n", dst_y);

			probe_pt = strstr(dst_uv, "_y.bin");
			if (probe_pt) {
				memset(loss_file, 0, 100);
				sprintf(loss_file, "%s/_loss/result.txt", basePath);
				FILE *fy = fopen(loss_file, "a");
				fprintf(fy, "%s\n", dst_y);
				fclose(fy);
				printf("read y file %s\n", dst_y);
				read_raw_data(dst_y, (vx_int8 *) ftmap->buf_hdl.viraddr);
				strcpy(probe_pt, "_uv.bin");
				printf("read uv file %s\n", dst_uv);
				read_raw_data(dst_uv, (vx_int8 *) ftmap->buf_hdl.uv_viraddr);
				nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_DATA, (void*) ftmap);
				tsem_wait(&thread_mutex);
				if (no_face_flag) {
					printf("no face flag\n");
					memset(loss_file, 0, 100);
					sprintf(loss_file, "%s/_loss/%s", basePath,
							strrchr(dst_y, '/') + 1);
					int ret = copyFile(dst_y, loss_file);
					memset(loss_file, 0, 100);
					sprintf(loss_file, "%s/_loss/%s", basePath,
							strrchr(dst_uv, '/') + 1);
					ret = copyFile(dst_uv, loss_file);
					if (ret != 0)
						printf("copy loss file failed\n");
				}
			}
		} else if (ptr->d_type == 4) {
			/* dir */
			memset(base, '\0', sizeof(base));
			strcpy(base, basePath);
			strcat(base, "/");
			strcat(base, ptr->d_name);
			printf("can not process dir %s\n", base);
		}
	}
	closedir(dir);
	return 1;
}

int main(void)
{
	int param = 0;
	total_file_num = 0;
	detect_face_num = 0;
	noface_file_num = 0;
	no_face_flag = 0;
	char basePath[100];
	tsem_init(&thread_mutex, 0);
	nna_feature_map_t ftmap;

	vx_context context = vxCreateContext();
	ERROR_CHECK_OBJECT(context);

	vx_size mem_size = 819200 + 409600;
	printf("det mem_size: %d\n", mem_size);
	vx_size heap_handle = vxAllocateDeviceMemory(context, mem_size);
	if (heap_handle == 0) {
		printf("alloc memory error!\n");
	}
	ftmap.buf_hdl.handle_t = heap_handle;
	ftmap.buf_hdl.phyaddr = vxGetDeviceMemoryPhysicalAddress(heap_handle);
	ftmap.buf_hdl.viraddr = vxGetDeviceMemoryVirtualAddress(heap_handle);
	ftmap.buf_hdl.uv_phyaddr = ftmap.buf_hdl.phyaddr + 819200;
	ftmap.buf_hdl.uv_viraddr = ftmap.buf_hdl.viraddr + 819200;
	ftmap.buf_hdl.size = mem_size;
	ftmap.width = 800;
	ftmap.height = 1024;
	ftmap.stride1 = 800;
	ftmap.stride2 = 800;
	ftmap.fmt = YUV420;

	nna_fn_t *nna_hdl = (nna_fn_t *) lomboax_open((void*) &param);
	usleep(10);

	nna_facedet_cfg_t nna_cfg;
	nna_cfg.fdet_en = 1;
	nna_cfg.ftracking_en = 0;
	nna_cfg.fquality_en = 0;
	nna_cfg.frec_en = 1;
	nna_cfg.max_det_faces = 5;
	nna_cfg.max_track_frames = 20;
	nna_cfg.visual_track_thresh = 50;
	nna_cfg.max_track_faces = 3;
	nna_cfg.fdet_max_size = 512;
	nna_cfg.fdet_min_size = 80;
	nna_cfg.frame_width = 800;
	nna_cfg.frame_height = 1024;
	int run_mode = BATCH_REGISTER_MODE;

	int result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, (void *) &nna_cfg);

	result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_INIT, (void*) &param);
	usleep(500);
	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_CALLBACK,
				(void *) face_det_callback);
	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_RUN_MODE,
				(void *) &run_mode);
	if (NO_ERR == result)
		result = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_RUN, (void*) &param);

	///get the current absoulte path
	memset(basePath, '\0', sizeof(basePath));
	getcwd(basePath, 999);
	printf("the current dir is : %s\n", basePath);
	memset(result_file, '\0', sizeof(result_file));
	sprintf(result_file, "%s/_loss", basePath);

	if (0 == access(result_file, 0)) {
		printf("%s exist\n", result_file);
	} else {
		if (-1 == mkdir(result_file, 0755)) {
			printf("%s create failed\n", result_file);
			exit(1);
		}
	}

	memset(result_file, '\0', sizeof(result_file));
	sprintf(result_file, "%s/_loss/result.txt", basePath);

	readFileList_and_process(basePath, &ftmap, nna_hdl);
	printf("loss ratio %f\n", (float) noface_file_num / (float) total_file_num);

	FILE *fy = fopen(result_file, "a");
	fprintf(fy, "total img %d, detect face %d,  loss img %d\n", total_file_num,
			detect_face_num, noface_file_num);
	fprintf(fy, "loss ratio %f\n",
			(float) noface_file_num / (float) total_file_num);
	fclose(fy);
	return 0;
}
