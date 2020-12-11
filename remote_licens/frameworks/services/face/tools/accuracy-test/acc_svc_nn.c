/*
 * nn.c
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
#include <pthread.h>

#define LOG_TAG "acc_svc_nn"
#include <log/log.h>

#include "acc_svc.h"
#include "acc_svc_nn.h"

static acc_svc_t *svc;
static nna_fn_t *nna_hdl;
static nna_facedet_cfg_t nna_cfg;

static void det_cb_event(nna_feature_map_t *map, nna_facecb_info_t *det_info)
{
	int cnt = 0;
	signed long quality = 0;
	ALOGV("enter %s\n", __func__);
	cnt = det_info->rects.num;
	svc->face_cnt = cnt;
	if (cnt == 1) {
		ALOGV("%s face quality %ld\n", __func__, 
			det_info->rects.face_rects[0].quality_score);
		quality = det_info->rects.face_rects[0].quality_score;
	}
	if ((cnt == 1) && (quality > svc->quality_threshold)) {
		nna_face_t *face_rect = &det_info->rects.face_rects[0];
		memcpy(&svc->face_rib.ftmap, map, sizeof(nna_feature_map_t));
		memcpy(&svc->face_rib.rois, face_rect, sizeof(nna_face_t));
		memcpy(svc->face_info->kpts, &det_info->rects.face_rects[0].kpts, sizeof(float)*10);
		svc->face_info->quality_score = det_info->rects.face_rects[0].quality_score;
		nn_face_feat(&svc->face_rib);
	} else {
		svc->face_cnt = 0;
		ALOGV("%s no face or more than 1 face detected\n", __func__);
		pthread_mutex_lock(&svc->face_lock);
		pthread_cond_signal(&svc->face_cond);
		pthread_mutex_unlock(&svc->face_lock);
	}
	ALOGV("exit %s\n", __func__);
}

static void recg_cb_event(nna_face_rib_t *rib, nna_face_feature_t *feat_info)
{
	ALOGV("enter %s\n", __func__);

	/* seem always have feature */
	if (feat_info)
		memcpy(svc->face_info->values, feat_info->feature_ref, sizeof(signed char)*FACE_FEAT_SIZE);
	else
		ALOGI("%s unknown error", __func__);

	pthread_mutex_lock(&svc->face_lock);
	pthread_cond_signal(&svc->face_cond);
	pthread_mutex_unlock(&svc->face_lock);
	ALOGV("exit %s\n", __func__);
}

static int nna_event(void *comp, void *data, unsigned int event, void *event_data)
{
	switch (event) {
	case CB_FACEDET_DONE:
	case CB_FACETRK_DONE:
	{
		det_cb_event(data, event_data);
		break;
	}
	case CB_FACEREC_DONE:
	{
		recg_cb_event(data, event_data);
		break;
	}
	default:
		ALOGE("unknown event");
	}

	return 0;
}

int nn_init(acc_svc_t *p)
{
	int ret;
	int time;

	ALOGV("enter %s\n", __func__);

	if (!p) {
		ALOGE("%s invalid parameter", __func__);
		return -1;
	}
	svc = p;

	nna_hdl = (nna_fn_t *)lomboax_open(NULL);
	if (!nna_hdl) {
		ALOGE("%s ax open failed\n", __func__);
		return -1;
	}
	usleep(10);

	/* nna configuration */
	ALOGV("starting NNA_SET_FACE_CFG\n");
	nna_cfg.fdet_en = FACE_DETECT_ENABLED;
	nna_cfg.ftracking_en = FACE_TRACKING_ENABLED;
	nna_cfg.fquality_en = FACE_QUALITY_ENABLED;
	nna_cfg.frec_en = 1;
	nna_cfg.max_det_faces = 1;
	nna_cfg.max_track_frames = 20;
	nna_cfg.visual_track_thresh = 50;
	nna_cfg.max_track_faces = 1;
	nna_cfg.fdet_max_size = 512;
	nna_cfg.fdet_min_size = 64;
	nna_cfg.frame_width = 800;
	nna_cfg.frame_height = 1024;
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACE_CFG, &nna_cfg);
	if (ret) {
		ALOGE("%s starting NNA_SET_FACE_CFG failed, %d", __func__, ret);
		return -1;
	}

	ALOGV("starting NNA_SET_INIT\n");
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_INIT, 0);
	if (ret) {
		ALOGE("%s starting NNA_SET_INIT failed, %d", __func__, ret);
		return -1;
	}
	usleep(50);

	ALOGV("starting NNA_SET_CALLBACK\n");
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_CALLBACK, nna_event);
	if (ret) {
		ALOGE("%s starting NNA_SET_CALLBACK failed, %d", __func__, ret);
		return -1;
	}

	ALOGV("starting NNA_SET_FACEDET_RUN\n");
	time = 0;
	do {
		ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_RUN, 0);
		ALOGV("  return %d", ret);
		usleep(10000);
	} while (ret && ++time < 3);
	if (ret) {
		ALOGE("%s starting NNA_SET_FACEDET_RUN failed, %d", __func__, ret);
		return -1;
	}
	usleep(10000);

	ALOGV("starting NNA_SET_FACETRK_RUN\n");
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACETRK_RUN, 0);
	if (ret) {
		ALOGE("%s starting NNA_SET_FACETRK_RUN failed, %d", __func__, ret);
		return -1;
	}

	ALOGV("starting NNA_SET_FACEREC_RUN\n");
	ret = nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEREC_RUN, 0);
	if (ret) {
		ALOGE("%s starting NNA_SET_FACEREC_RUN failed, %d", __func__, ret);
		return -1;
	}

	return ret;
}

int nn_face_det(nna_feature_map_t *map)
{
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_DATA, map);
}

int nn_face_feat(nna_face_rib_t *face_rib)
{
	return nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEREC_DATA, face_rib);
}

int nn_face_feat_cmp(nna_face_feature_t *face_feat)
{
	return nna_hdl->nna_facematch(nna_hdl, face_feat);
}

int nn_deinit(void)
{
	ALOGV("enter nn_deinit\n");
	nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEDET_STOP, 0);
	nna_hdl->nna_cmd(nna_hdl, NNA_SET_FACEREC_STOP, 0);
	if (nna_hdl) {
		lomboax_close(nna_hdl);
		nna_hdl = NULL;
	}

	return 0;
}
