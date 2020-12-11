/*
 * acc_svc.h
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

#ifndef _FACE_SVC_H_
#define _FACE_SVC_H_

#define FACE_SVC_OK   0
#define FACE_SVC_ERR -1

#include <nna_api.h>

#define FACE_FEAT_SIZE 1024

typedef struct _face_info {
	signed char values[FACE_FEAT_SIZE];
	float norm[2];
	float kpts[10];
	signed long quality_score;
	int error;
	/* compare result,only max score */
	float score;
	signed long max_id;
} face_info_t;

typedef struct _feats {
	/* faces feature array */
	signed char *values;
	/* faces norm */
	float *norm;
	/* faces key points on register */
	float *kpts;
	/* face count */
	int cnt;
} feats_t;

typedef struct _acc_svc {
	signed long quality_threshold;
	int face_cnt;
	face_info_t *face_info;
	nna_face_rib_t face_rib;
	pthread_mutex_t face_lock;
	pthread_cond_t face_cond;
	pthread_mutex_t face_feat_lock;
	nna_face_feature_t *face_feat;
} acc_svc_t;

/*
 * init face service
 *
 * return the handle of face service
 */
void *acc_svc_init(void);

/*
 * deinit face service
 *
 * handle: the handle of face service
 */
int acc_svc_deinit(void *handle);


/*
 * set cfg
 *
 */
int acc_svc_set_cfg(void *handle, signed long quality_threshold);

/*
 * get feature from pic
 *
 */
int acc_svc_get_feats(void *handle, char *file_path, face_info_t *face_info);


/*
 * register base
 *
 */
int acc_svc_register_base(void *handle, feats_t *feats);

/*
 * get score compare to base
 *
 */
int acc_svc_compare_to_base(void *handle, face_info_t *face_info);
#endif /* _FACE_SVC_H_ */

