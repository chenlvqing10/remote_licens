/*
 * recognition.c - define interface function of operate wifi device
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
#include "sqlite3.h"
#include "db_face_recognition_param.h"
#include "recognition.h"

#define LOG_TAG "api_recognition"
#include <log/log.h>


static recg_para_change_cb recognition_para_change_notify_cb;
static isp_para_change_cb isp_para_change_notify_cb;

/**
 * recognition parameter change notify callback function
 * @param p a pointer of callback
 * @return NULL
 */
void recognition_register_change_cb(recg_para_change_cb cb)
{
	recognition_para_change_notify_cb = cb;
}

/**
 * isp parameter change notify callback function
 * @param p a pointer of callback
 * @return NULL
 */
void isp_register_change_cb(isp_para_change_cb cb)
{
	isp_para_change_notify_cb = cb;
}

/**
 * get recognition parameter
 * @param p_recg a pointer of save recognition parameters
 * @return 0-successful, other-failed
 */
int recognition_get_param(recognition_para_t *p_recg)
{
	face_recognition_param_t info;
	int ret = 0;

	if (NULL == p_recg) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}

	ret = db_face_recognition_param_query(&info);
	if (ret <= 0) {
		ALOGE("%s: read recg data failed from db", __func__);
		return -1;
	}
	memcpy(p_recg, &info, sizeof(info));

	return 0;
}

/**
 * update recognition parameter
 * @param p_recg a pointer of save recognition parameters
 * @return 0-successful, other-failed
 */
int recognition_set_param(recognition_para_t *p_recg)
{
	face_recognition_param_t info;
	int ret = 0;

	if (NULL == p_recg) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}
	/*read data from database*/
	ret = db_face_recognition_param_query(&info);
	if (ret <= 0) {
		ALOGE("%s: read recg data failed from db", __func__);
		return -1;
	}

	if ((info.identify_threshold_score == p_recg->threshold_score) &&
		(info.nir_live_detect == p_recg->live_detect) &&
		(info.min_face_pixel == p_recg->min_face_pixel) &&
		(info.max_face_pixel == p_recg->max_face_pixel) &&
		(info.max_track_faces == p_recg->max_track_faces)) {
		ALOGD("%s: parameter is not change\n", __func__);
		return 0;
	}

	info.identify_threshold_score = p_recg->threshold_score;
	info.nir_live_detect = p_recg->live_detect;
	info.min_face_pixel = p_recg->min_face_pixel;
	info.max_face_pixel = p_recg->max_face_pixel;
	info.max_track_faces = p_recg->max_track_faces;
	/*save data to database*/
	memcpy(&info, p_recg, sizeof(recognition_para_t));
	ret = db_face_recognition_param_update(&info);
	if (ret != SQLITE_OK) {
		ALOGE("%s: read recg data failed from db", __func__);
		return -1;
	}
	/* set parameter to face service */

	if (recognition_para_change_notify_cb)
		recognition_para_change_notify_cb(p_recg);
	return 0;
}

/**
 * get isp parameter
 * @param p_isp a pointer of save recognition parameters
 * @return 0-successful, other-failed
 */
int isp_get_param(isp_para_t *p_isp)
{
	isp_para_t info;
	int ret = 0;

	if (NULL == p_isp) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}

	ret = db_face_isp_param_query(&info);
	if (ret <= 0) {
		ALOGE("%s: read recg data failed from db", __func__);
		return -1;
	}
	memcpy(p_isp, &info, sizeof(info));

	return 0;
}

/**
 * update isp parameter
 * @param p_isp a pointer of save recognition parameters
 * @return 0-successful, other-failed
 */
int isp_set_param(isp_para_t *p_isp)
{
	isp_para_t info;
	int ret = 0;

	if (NULL == p_isp) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}
	/*read data from database*/
	ret = db_face_isp_param_query(&info);
	if (ret <= 0) {
		ALOGE("%s: read isp data failed from db", __func__);
		return -1;
	}
	if (info.hdr_sw == p_isp->hdr_sw) {
		ALOGD("%s: parameter is not change\n", __func__);
		return 0;
	}

	info.hdr_sw = p_isp->hdr_sw;
	/*save data to database*/
	ret = db_face_isp_param_update(&info);
	if (ret != SQLITE_OK) {
		ALOGE("%s: update isp data failed", __func__);
		return -1;
	}
	if (isp_para_change_notify_cb)
		isp_para_change_notify_cb(p_isp);
	return 0;
}

