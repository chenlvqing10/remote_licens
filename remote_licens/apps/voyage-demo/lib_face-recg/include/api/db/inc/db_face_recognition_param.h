/*
 * external_import_pic.h - Interface for face recognition param
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

#ifndef __FACE_RECOGNITION_PARAM_H
#define __FACE_RECOGNITION_PARAM_H

#include "ee_queue.h"
#include "recognition.h"

typedef struct tag_face_recognition_param {
	/* The closest distance allowed by face recognition(Unit: m) */
	float identify_closest_distance;

	/* The farthest distance allowed by face recognition(Unit: m)*/
	float identify_furthest_distance;

	/* Identify threshold score
	 * Below this threshold, the face is considered to be a failure(Percentage)
	 */
	float identify_threshold_score;

	/* Nir live detect
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char nir_live_detect;
	int min_face_pixel;
	int max_face_pixel;
	int max_track_faces;
} face_recognition_param_t;

/**
 * face_recognition_param_print - Display information in a queue for debugging
 * purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void face_recognition_param_print(face_recognition_param_t *p_info);

/**
 * db_face_recognition_param_update - Update face recognition param information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_recognition_param_update(face_recognition_param_t *p_info);

/**
 * db_face_recognition_param_query - Query face recognition param.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_recognition_param_query(face_recognition_param_t *p_info);

/**
 * db_face_isp_param_query - Query face recognition param.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_isp_param_query(isp_para_t *p_info);

/**
 * db_face_isp_param_update - Update face recognition param information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_isp_param_update(isp_para_t *p_info);

#endif /* __FACE_RECOGNITION_PARAM_H */

