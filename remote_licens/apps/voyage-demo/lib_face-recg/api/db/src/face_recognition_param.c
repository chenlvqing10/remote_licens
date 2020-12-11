/*
 * face_recognition_param.c - Interface for face recognition param
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "db_sqlite_middle.h"
#include "db_face_recognition_param.h"
#include "recognition.h"
#include "db_config.h"

#define LOG_TAG "API_DB"
#include <log/log.h>

/**
 * face_recognition_param_print - Display information in a queue for debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void face_recognition_param_print(face_recognition_param_t *p_info)
{
	assert(p_info != NULL);

	ALOGD("[%s:%d] face_recognition_param:\n"
		"identify_closest_distance=%f\n"
		"identify_furthest_distance=%f\n"
		"identify_threshold_score=%f\n"
		"nir_live_detect=%d\n",
		__func__,
		__LINE__,
		p_info->identify_closest_distance,
		p_info->identify_furthest_distance,
		p_info->identify_threshold_score,
		p_info->nir_live_detect);
}

/**
 * db_face_recognition_param_update - Update face recognition param information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_recognition_param_update(face_recognition_param_t *p_info)
{
	int ret = 0;

	assert(p_info != NULL);

	ret += db_config_set_double(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_IDENTIFYCLOSESTDISTANCE,
		p_info->identify_closest_distance);
	ret += db_config_set_double(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_IDENTIFYFURTHESTDISTANCE,
		p_info->identify_furthest_distance);
	ret += db_config_set_double(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_IDENTIFYTHRESHOLDSCORE,
		p_info->identify_threshold_score);
	ret += db_config_set_int(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_NIRLIVEDETECT,
		p_info->nir_live_detect);
	ret += db_config_set_int(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_MINFACEPIXEL,
		p_info->min_face_pixel);
	ret += db_config_set_int(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_MAXFACEPIXEL,
		p_info->max_face_pixel);
	ret += db_config_set_int(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_MAXTRACKFACES,
		p_info->max_track_faces);

	return ret;
}

/**
 * db_face_recognition_param_query - Query face recognition param.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns Returns number of data queried.
 */
int db_face_recognition_param_query(face_recognition_param_t *p_info)
{
	double def_val = 0;

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(face_recognition_param_t));

	def_val = 0;
	sscanf(DBCONF_FACERECOGNITIONPARAM_IDENTIFYCLOSESTDISTANCE_VALUE,
		"%lf", &def_val);
	p_info->identify_closest_distance = db_config_get_double(
		DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_IDENTIFYCLOSESTDISTANCE,
		def_val);
	def_val = 0;
	sscanf(DBCONF_FACERECOGNITIONPARAM_IDENTIFYFURTHESTDISTANCE_VALUE,
		"%lf", &def_val);
	p_info->identify_furthest_distance = db_config_get_double(
		DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_IDENTIFYFURTHESTDISTANCE,
		def_val);
	def_val = 0;
	sscanf(DBCONF_FACERECOGNITIONPARAM_IDENTIFYTHRESHOLDSCORE_VALUE,
		"%lf", &def_val);
	p_info->identify_threshold_score = db_config_get_double(
		DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_IDENTIFYTHRESHOLDSCORE,
		def_val);
	p_info->nir_live_detect = db_config_get_int(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_NIRLIVEDETECT,
		atoi(DBCONF_FACERECOGNITIONPARAM_NIRLIVEDETECT_VALUE));
	p_info->min_face_pixel = db_config_get_int(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_MINFACEPIXEL,
		atoi(DBCONF_FACERECOGNITIONPARAM_MINFACEPIXEL_VALUE));
	p_info->max_face_pixel = db_config_get_int(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_MAXFACEPIXEL,
		atoi(DBCONF_FACERECOGNITIONPARAM_MAXFACEPIXEL_VALUE));
	p_info->max_track_faces = db_config_get_int(DBCONF_FACERECOGNITIONPARAM,
		DBCONF_FACERECOGNITIONPARAM_MAXTRACKFACES,
		atoi(DBCONF_FACERECOGNITIONPARAM_MAXTRACKFACES_VALUE));

	return 1;
}

/**
 * db_face_isp_param_query - Query face recognition param.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns Returns number of data queried.
 */
int db_face_isp_param_query(isp_para_t *p_info)
{
	int def_val = 0;

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(isp_para_t));

	p_info->hdr_sw = db_config_get_int(
		DBCONF_ISP,
		DBCONF_ISP_HDR,
		def_val);

	return 1;
}

/**
 * db_face_isp_param_update - Update face recognition param information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_isp_param_update(isp_para_t *p_info)
{
	int ret = 0;

	assert(p_info != NULL);

	ret += db_config_set_int(DBCONF_ISP,
		DBCONF_ISP_HDR,
		p_info->hdr_sw);

	return ret;
}

