/*
 * uvc.h -USB Video Class Gadget Application
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

#ifndef __UVC_COMM_H__
#define __UVC_COMM_H__

#include "face_svc.h"
#include "recognition.h"
#include "system_info.h"
#include "person_lookup.h"
#include "file_ops.h"
#include "db_punch_record.h"
#include "face.h"

#include "cJSON.h"
#include "person.h"
#include "threadpool.h"

#define CALIBRATION_NUM 5
#define DIR_REGISTER "/data/face_register"
#define DIR_RECG "/data/face_recg"

typedef struct uvc_cjson_info {
	char *cmd_id;
	int type;
	char *name;

	int x;
	int y;
	int w;
	int h;
} uvc_cjson_info_t;

typedef struct uvc_comm_info {
	int				write_face_info_fd;
	int				write_respone_info_fd;
	int				read_info_fd;

	int             inotify_file_fd;
	int             inotify_watch_fd;

	uvc_cjson_info_t    *uvc_read_info;
	recognition_para_t  recg_para;

	int set_para_status;
	int take_pic_status;
	int start_cal_map;

	FILE *reg_img_file;

	e_person_reg_code reg_result_status;
	e_person_unreg_code unreg_result_status;
	threadpool_t *pool;
	int exit_flag;
	pthread_mutex_t response_lock;
}uvc_comm_info_t;

typedef enum {
	REG_PERSON = 0,
	UNREG_PERSON,
	FACE_INFO,
	RECG_PARA_INFO,
	EXPORT_LATEST_REG_PERSON_IMG,
	RECV_OTA_FILE,
	TAKE_MOD_PIC,
} info_type_t;

/* camera calibration param */
typedef struct _uvc_map {
	double x_map[CALIBRATION_NUM];
	double y_map[CALIBRATION_NUM];
} uvc_map_t;

typedef struct uvc_map_params {
	int check;
	double x_map[CALIBRATION_NUM];
	double y_map[CALIBRATION_NUM];
} uvc_map_params_t;

int uvc_comm_file_open(uvc_comm_info_t **info);
void uvc_comm_file_close(uvc_comm_info_t *info);
void uvc_read_info(uvc_comm_info_t *comm_info);
void uvc_write_info(uvc_comm_info_t *comm_info);
void uvc_write_info_to_file(int fd, cJSON *root);
void uvc_cjson_face_info(cJSON *root, cJSON *header, cJSON *Array, faces_t *faces,
	face_camera_cfg_t *rgb_cfg);

#endif /* __FACE_H__ */
