/*
 * acc_svc.c
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <execinfo.h>
#include <sys/stat.h>

#define LOG_TAG "acc_svc"
#include <log/log.h>

#include "jpeg_enc.h"
#include "jpeg_dec.h"
#include "lombo_malloc.h"
#include "acc_svc.h"
#include "acc_svc_nn.h"



void *acc_svc_init(void)
{
	acc_svc_t *svc;

	ALOGV("enter %s\n", __func__);
	svc = (acc_svc_t *)malloc(sizeof(acc_svc_t));
	if (!svc) {
		ALOGE("malloc acc_svc_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	memset(svc, 0, sizeof(acc_svc_t));

	if (pthread_mutex_init(&svc->face_lock, NULL)) {
		ALOGE("init face_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}
	if (pthread_cond_init(&svc->face_cond, NULL)) {
		ALOGE("init face_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	if (pthread_mutex_init(&svc->face_feat_lock, NULL)) {
		ALOGE("init face_feat_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return NULL;
	}

	svc->face_feat = malloc(sizeof(nna_face_feature_t));
	if (!svc->face_feat) {
		ALOGE("malloc nna_face_feature_t failed, errno: %d (%s)",
			errno, strerror(errno));
		return NULL;
	}
	memset(svc->face_feat, 0, sizeof(nna_face_feature_t));

	ALOGV("face nn init");
	if (nn_init(svc)) {
		ALOGE("face nn init failed");
		return NULL;
	}

	svc->quality_threshold = 50;
	return svc;
}



int acc_svc_deinit(void *handle)
{
	acc_svc_t *svc = (acc_svc_t *)handle;

	ALOGV("enter %s\n", __func__);

	nn_deinit();

	if (svc) {
		free(svc->face_feat);
		free(svc);
		svc = NULL;
	}

	return 0;
}


int acc_svc_set_cfg(void *handle, signed long quality_threshold)
{
	acc_svc_t *svc = (acc_svc_t *)handle;
	ALOGV("enter %s\n", __func__);
	if (!svc)
		return -1;

	svc->quality_threshold = quality_threshold;
	return 0;
}

int acc_svc_get_feats(void *handle, char *file_path, face_info_t *face_info)
{
	struct jpeg_dec_input input;
	struct jpeg_dec_output jpg_decode_result;
	int ret;
	int j;
	float norm;
	nna_feature_map_t *map;

	ALOGV("enter %s\n", __func__);

	acc_svc_t *svc = (acc_svc_t *)handle;
	svc->face_info = face_info;
	if (!svc || !file_path || !face_info)
		return -1;


	memset(&jpg_decode_result, 0x00, sizeof(jpg_decode_result));
	ALOGD("photo path:%s", file_path);
	input.file = file_path;
	input.format = JPEG_COLOR_NV12;
	input.scale = JPEG_SCALEDOWN_0;
	input.width_stride = 800;
	input.height_stride = 1024;
	ret = jpeg_dec_begin(&input, &jpg_decode_result);
	if (ret) {
		ALOGE("jpeg dec failed");
		return -1;
	}


	map = malloc(sizeof(nna_feature_map_t));
	map->fmt = YUV420;
	map->img_type = NNA_IMAGE;
	map->width = 800;
	map->height = 1024;
	map->channal = 3;
	map->stride1 = map->width;
	map->stride2 = map->width;
	map->buf_hdl.size = jpg_decode_result.size[0]+jpg_decode_result.size[1];
	map->buf_hdl.phyaddr = (unsigned long)jpg_decode_result.phy_addr[0];
	map->buf_hdl.viraddr = (unsigned long)jpg_decode_result.addr[0];
	map->buf_hdl.uv_phyaddr = (unsigned long)jpg_decode_result.phy_addr[1];
	map->buf_hdl.uv_viraddr = (unsigned long)jpg_decode_result.addr[1];
	if (nn_face_det(map))
		ALOGE("%s nn_face_det failed", __func__);
	else
		ALOGE("%s nn_face_det succee", __func__);

	pthread_mutex_lock(&svc->face_lock);
	pthread_cond_wait(&svc->face_cond, &svc->face_lock);
	pthread_mutex_unlock(&svc->face_lock);
	ALOGE("%s nn_face_det finish:det %d faces", __func__, svc->face_cnt);


	svc->face_info->error = 1;
	if (svc->face_cnt == 1) {
		/* norm0 */
		norm = 0.0f;
		for (j = 0; j < FACE_FEAT_SIZE / 2; j++)
			norm += svc->face_info->values[j] * svc->face_info->values[j];
		svc->face_info->norm[0] = sqrtf(norm);
		/* norm1 */
		norm = 0.0f;
		for (; j < FACE_FEAT_SIZE; j++)
			norm += svc->face_info->values[j] * svc->face_info->values[j];
		svc->face_info->norm[1] = sqrtf(norm);
		svc->face_info->error = 0;
	}

	usleep(1000);
	free(map);
	jpeg_dec_end(&jpg_decode_result);
	ALOGV("exit %s\n", __func__);
	return 0;
}

int acc_svc_register_base(void *handle, feats_t *feats)
{
	acc_svc_t *svc = (acc_svc_t *)handle;
	int ret = 0;

	ALOGV("enter %s\n", __func__);
	if (!svc || !feats) {
		ALOGE("%s invalid parameters", __func__);
		return -1;
	}

	if (feats->cnt < 0)
		return -1;

	ALOGI("registering %d faces", feats->cnt);

	pthread_mutex_lock(&svc->face_feat_lock);

	svc->face_feat->features = feats->values;
	svc->face_feat->features_norm = feats->norm;
	svc->face_feat->face_kpts = feats->kpts;
	svc->face_feat->features_num = feats->cnt;

	pthread_mutex_unlock(&svc->face_feat_lock);

	ALOGV("exit %s\n", __func__);
	return ret;
}


int acc_svc_compare_to_base(void *handle, face_info_t *face_info)
{
	acc_svc_t *svc = (acc_svc_t *)handle;
	int conf;
	float *scores;

	ALOGV("enter %s\n", __func__);
	if (!svc || !face_info) {
		ALOGE("%s invalid parameters", __func__);
		return -1;
	}
	scores = malloc(sizeof(float) * svc->face_feat->features_num);

	svc->face_feat->feature_ref = face_info->values;
	svc->face_feat->feature_size = FACE_FEAT_SIZE;
	svc->face_feat->feature_cmp_size = FACE_FEAT_SIZE;
	svc->face_feat->score = scores;
	conf = nn_face_feat_cmp(svc->face_feat);
	face_info->max_id = svc->face_feat->max_id;
	face_info->score = scores[face_info->max_id];
	ALOGV("exit %s\n", __func__);
	free(scores);
	return conf;
}

