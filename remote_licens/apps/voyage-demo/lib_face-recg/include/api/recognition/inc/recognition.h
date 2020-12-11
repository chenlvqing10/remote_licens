/*
 * recognition.h - export some interface function of operate wifi device
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

#ifndef _RECOGNITION_H
#define _RECOGNITION_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct recognition_para {
	/* The closest distance allowed by face recognition(Unit: m) */
	float min_dis;

	/* The farthest distance allowed by face recognition(Unit: m)*/
	float max_dis;

	/* Identify threshold score
	 * Below this threshold, the face is considered to be a failure(Percentage)
	 */
	float threshold_score;

	/* Nir live detect
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char live_detect;
	int min_face_pixel;
	int max_face_pixel;
	int max_track_faces;
} recognition_para_t;

typedef struct isp_para {
	/* Hdr switch
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	int hdr_sw;
} isp_para_t;

typedef void (*recg_para_change_cb)(recognition_para_t *p);

typedef void (*isp_para_change_cb)(isp_para_t *p);

/**
 * recognition parameter change notify callback function
 * @param p a pointer of callback
 * @return NULL
 */
void recognition_register_change_cb(recg_para_change_cb cb);

/**
 * isp parameter change notify callback function
 * @param p a pointer of callback
 * @return NULL
 */
void isp_register_change_cb(isp_para_change_cb cb);

/**
 * get recognition parameter
 * @param p_recg a pointer of save recognition parameters
 * @return 0-successful, other-failed
 */
int recognition_get_param(recognition_para_t *p_recg);

/**
 * update recognition parameter
 * @param p_recg a pointer of save recognition parameters
 * @return 0-successful, other-failed
 */
int recognition_set_param(recognition_para_t *p_recg);

/**
 * get isp parameter
 * @param p_isp a pointer of save recognition parameters
 * @return 0-successful, other-failed
 */
int isp_get_param(isp_para_t *p_isp);

/**
 * update isp parameter
 * @param p_isp a pointer of save recognition parameters
 * @return 0-successful, other-failed
 */
int isp_set_param(isp_para_t *p_isp);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

