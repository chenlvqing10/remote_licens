/*
 * nn.h
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

#ifndef _FACE_SVC_NN_H_
#define _FACE_SVC_NN_H_

#include <nna_api.h>
#include "acc_svc.h"

#define NN_HEIGHT_LIMIT 1024

#define FACE_DETECT_ENABLED 1
#define FACE_TRACKING_ENABLED 1
#define FACE_QUALITY_ENABLED 1
#define FACE_REC_ENABLED 1
#define FACE_MAX_DET_OBJ 1

/* the confidence of face feature compare result */
#define FACE_FEAT_CMP_CONF 6

int nn_init(acc_svc_t *p);

int nn_face_det(nna_feature_map_t *map);

int nn_face_feat(nna_face_rib_t *face_rib);

int nn_face_feat_cmp(nna_face_feature_t *face_feat);

int nn_deinit(void);

#endif /* _FACE_SVC_NN_H_ */
