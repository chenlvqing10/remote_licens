/*
 * uvc-face-recg.c - USB Video Class Gadget Application
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
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

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/prctl.h>

#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define LOG_TAG		"uvc_comm"
#include <log/log.h>


#include "uvc_comm.h"
#include "config.h"
#include "db.h"
#include "person_register.h"
#include "db_person_info.h"
#include "system_private.h"


#define UVC_INFO_READ_JSON_FILE		"/data/uvc_info_read.json"
#define UVC_INFO_WRITE_JSON_FILE		"/data/uvc_info_write.json"
#define UVC_INFO_RESPONSE_STATUS    "/data/uvc_info_response_status.json"
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN  (EVENT_SIZE + 128)

#define RGBNIR_MAP_CHECK 0x5B
#define RGBNIR_MAP_OFFSET 0x1024

static recognition_para_t recg_para;

int uvc_save_mod_map_param(uvc_map_t *mod_map_params)
{
	int i = 0;
	uvc_map_params_t uvc_map_params;

	memset(&uvc_map_params, 0, sizeof(uvc_map_params_t));
	uvc_map_params.check = RGBNIR_MAP_CHECK;
	for (i = 0; i < CALIBRATION_NUM; i++) {
		uvc_map_params.x_map[i] = mod_map_params->x_map[i];
		ALOGV("x_map %f", uvc_map_params.x_map[i]);
	}
	for (i = 0; i < CALIBRATION_NUM; i++) {
		uvc_map_params.y_map[i] = mod_map_params->y_map[i];
		printf("y_map %f", uvc_map_params.y_map[i]);
	}

	return system_set_private_data(RGBNIR_MAP_OFFSET,
			 (char *)&uvc_map_params, sizeof(uvc_map_params_t));
}

int uvc_comm_file_open(uvc_comm_info_t **info)
{
	uvc_comm_info_t *tmp_info;

	tmp_info = calloc(1, sizeof(uvc_comm_info_t));
	if (!tmp_info) {
		ALOGE("failed to calloc for uvc comm info\n");
		return -1;
	}

	memset(tmp_info, 0x0, sizeof(uvc_comm_info_t));
	if (pthread_mutex_init(&tmp_info->response_lock, NULL)) {
		ALOGE("init response_lock failed");
		return -1;
	}

	tmp_info->write_face_info_fd = open(UVC_INFO_WRITE_JSON_FILE, O_CREAT | O_RDWR);
	if (tmp_info->write_face_info_fd < 0) {
		printf("open file[%s] err\n", UVC_INFO_WRITE_JSON_FILE);
		return -1;
	}

	tmp_info->write_respone_info_fd = open(UVC_INFO_RESPONSE_STATUS, O_CREAT | O_RDWR);
	if (tmp_info->write_respone_info_fd <= 0) {
		printf("open file[%s] err\n", UVC_INFO_RESPONSE_STATUS);
		close(tmp_info->write_face_info_fd);
		return -1;
	}

	tmp_info->read_info_fd = open(UVC_INFO_READ_JSON_FILE, O_CREAT | O_RDWR);
	if (tmp_info->read_info_fd < 0) {
		printf("open file[%s] err\n", UVC_INFO_READ_JSON_FILE);
		close(tmp_info->write_face_info_fd);
		close(tmp_info->write_respone_info_fd);
		return -1;
	}

	*info = tmp_info;

	return 0;
}

void uvc_comm_file_close(uvc_comm_info_t *info)
{
	if (info) {
		close(info->write_face_info_fd);
		close(info->write_respone_info_fd);
		close(info->read_info_fd);
		if(info->uvc_read_info)
			free(info->uvc_read_info);

		inotify_rm_watch(info->inotify_file_fd, info->inotify_watch_fd);
		close(info->inotify_file_fd);

		free(info);
	}

	ALOGD("uvc comm file have been closed\n");

}


void uvc_write_info_to_file(int fd, cJSON *root)
{
	char *buf = cJSON_Print(root);
	ftruncate(fd, 0);
	lseek(fd, 0, SEEK_SET);
	write(fd, buf, strlen(buf));
	fsync(fd);
	cJSON_Delete(root);
	free(buf);
}

static cJSON *uvc_read_info_from_file(int fd)
{
	int file_len, read_size;
	char *buf = NULL;
	cJSON *json =NULL;

	file_len = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	buf = malloc(file_len);

	if(buf == NULL) {
		printf("buf malloc err\n");
		return NULL;
	}

	read_size = read(fd, buf, file_len);
	if(read_size != file_len) {
		free(buf);
		buf = NULL;
		printf("read %s data err\n", UVC_INFO_READ_JSON_FILE);
		return NULL;
	}

	json = cJSON_Parse(buf);

	if(buf)
		free(buf);

	return json;
}

void uvc_cjson_face_info(cJSON *root, cJSON *header, cJSON *Array, faces_t *faces,
	face_camera_cfg_t *rgb_cfg)
{
	float threshold = face_recg_threshold();
	Array = cJSON_CreateArray();
	cJSON *face_info[faces->face_num];
	char buf[20];
	for (int i = 0; i < faces->face_num; i++) {
		face_t *face = (face_t *)&faces->faces[i];
		int is_living = 0;
		int is_recg = 0;

		if (face->score > threshold)
			is_recg = 1;
		if (face->living)
			is_living = 1;
		face_info[i] = cJSON_CreateObject();

		if((1920 == rgb_cfg->width) && (1080 == rgb_cfg->height)) {
			face->rect_smooth[0] = face->rect_smooth[0] * 1280 / 1920;
			face->rect_smooth[1] = face->rect_smooth[1] * 720 / 1080;
			face->rect_smooth[2] = face->rect_smooth[2] * 1280 / 1920;
			face->rect_smooth[3] = face->rect_smooth[3] * 720 / 1080;
		}
		cJSON_AddNumberToObject(face_info[i], "x0", face->rect_smooth[0]);
		cJSON_AddNumberToObject(face_info[i], "y0", face->rect_smooth[1]);
		cJSON_AddNumberToObject(face_info[i], "x1", face->rect_smooth[2]);
		cJSON_AddNumberToObject(face_info[i], "y1", face->rect_smooth[3]);
		cJSON_AddStringToObject(face_info[i], "name", face->name);
		cJSON_AddNumberToObject(face_info[i], "living", is_living);
		cJSON_AddNumberToObject(face_info[i], "recg", is_recg);
		cJSON_AddNumberToObject(face_info[i], "male", face->attr.male);
		sprintf(buf, "%f", face->head_pose[0]);
		cJSON_AddStringToObject(face_info[i], "head_pose[0]", buf);
		sprintf(buf, "%f", face->head_pose[1]);
		cJSON_AddStringToObject(face_info[i], "head_pose[1]", buf);
		sprintf(buf, "%f", face->head_pose[2]);
		cJSON_AddStringToObject(face_info[i], "head_pose[2]", buf);

		sprintf(buf, "%f", face->kpts[0]);
		cJSON_AddStringToObject(face_info[i], "kpts[0]", buf);
		sprintf(buf, "%f", face->kpts[1]);
		cJSON_AddStringToObject(face_info[i], "kpts[1]", buf);
		sprintf(buf, "%f", face->kpts[2]);
		cJSON_AddStringToObject(face_info[i], "kpts[2]", buf);
		sprintf(buf, "%f", face->kpts[3]);
		cJSON_AddStringToObject(face_info[i], "kpts[3]", buf);
		sprintf(buf, "%f", face->kpts[4]);
		cJSON_AddStringToObject(face_info[i], "kpts[4]", buf);
		sprintf(buf, "%f", face->kpts[5]);
		cJSON_AddStringToObject(face_info[i], "kpts[5]", buf);
		sprintf(buf, "%f", face->kpts[6]);
		cJSON_AddStringToObject(face_info[i], "kpts[6]", buf);
		sprintf(buf, "%f", face->kpts[7]);
		cJSON_AddStringToObject(face_info[i], "kpts[7]", buf);
		sprintf(buf, "%f", face->kpts[8]);
		cJSON_AddStringToObject(face_info[i], "kpts[8]", buf);
		sprintf(buf, "%f", face->kpts[9]);
		cJSON_AddStringToObject(face_info[i], "kpts[9]", buf);
		cJSON_AddItemToArray(Array,face_info[i]);

		ALOGI("face feature point :[%f %f %f %f %f %f %f %f %f %f]\n",
			face->kpts[0], face->kpts[1], face->kpts[2], face->kpts[3], face->kpts[4],
			face->kpts[5], face->kpts[6], face->kpts[7], face->kpts[8], face->kpts[9]);
	}

	cJSON_AddItemToObject(header, "type", cJSON_CreateNumber(FACE_INFO));
	cJSON_AddItemToObject(root, "header", header);
	cJSON_AddNumberToObject(root,"face_num", faces->face_num);
	cJSON_AddItemToObject(root, "face_info", Array);
}

static int uvc_cjson_register_info(cJSON *root, cJSON *header, cJSON *register_info,
	uvc_comm_info_t *comm_info)
{
	cJSON *tmp;

	register_info = cJSON_CreateObject();
	tmp = cJSON_GetObjectItem(header,"type");
	if(tmp != NULL) {
		cJSON_ReplaceItemInObject(header, "type", cJSON_CreateNumber(REG_PERSON));
	} else {
		cJSON_AddItemToObject(header, "type", cJSON_CreateNumber(REG_PERSON));
	}
	pthread_mutex_lock(&comm_info->response_lock);
	/* add protection to avoid null pointer operation */
	if(comm_info->uvc_read_info->cmd_id)
		cJSON_AddStringToObject(root,"cmd_id",comm_info->uvc_read_info->cmd_id);
	else
		return -1;

	if(comm_info->uvc_read_info->name)
		cJSON_AddItemToObject(register_info, "name", cJSON_CreateString(comm_info->uvc_read_info->name));
	else
		return -1;

	cJSON_AddItemToObject(register_info, "reg_status", cJSON_CreateNumber(comm_info->reg_result_status));
	pthread_mutex_unlock(&comm_info->response_lock);
	cJSON_AddItemToObject(root, "header", header);
	cJSON_AddItemToObject(root, "register_info", register_info);

	return 0;
}

static int uvc_cjson_unregister_info(cJSON *root, cJSON *header, cJSON *unregister_info,
	uvc_comm_info_t *comm_info)
{
	cJSON *tmp;

	unregister_info = cJSON_CreateObject();
	tmp = cJSON_GetObjectItem(header,"type");
	if(tmp != NULL) {
		cJSON_ReplaceItemInObject(header, "type", cJSON_CreateNumber(UNREG_PERSON));
	} else {
		cJSON_AddItemToObject(header, "type", cJSON_CreateNumber(UNREG_PERSON));
	}
	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id)
		cJSON_AddStringToObject(root,"cmd_id",comm_info->uvc_read_info->cmd_id);
	else
		return -1;

	if(comm_info->uvc_read_info->name) {
		cJSON_AddItemToObject(unregister_info, "name", cJSON_CreateString(comm_info->uvc_read_info->name));
	}
	cJSON_AddItemToObject(unregister_info, "unreg_status", cJSON_CreateNumber(comm_info->unreg_result_status));
	pthread_mutex_unlock(&comm_info->response_lock);
	cJSON_AddItemToObject(root, "header", header);
	cJSON_AddItemToObject(root, "unregister_info", unregister_info);

	return 0;
}

static int uvc_cjson_get_init_info(cJSON *root, cJSON *header, cJSON *get_init_info,
	uvc_comm_info_t *comm_info)
{
	build_info_t p_build_info;
	const char *version;
	cJSON *tmp = NULL;
	char buf[20];

	memset(&p_build_info, 0x0, sizeof(build_info_t));
	recognition_get_param(&recg_para);
	system_info_get_build_info(&p_build_info);
	version = config_get_string("system", "version", "");
	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id)
		cJSON_AddStringToObject(root,"cmd_id",comm_info->uvc_read_info->cmd_id);
	else
		return -1;
	pthread_mutex_unlock(&comm_info->response_lock);
	get_init_info = cJSON_CreateObject();
	tmp = cJSON_GetObjectItem(header,"type");
	if(tmp != NULL) {
		cJSON_ReplaceItemInObject(header, "type", cJSON_CreateNumber(RECG_PARA_INFO));
	} else {
		cJSON_AddItemToObject(header, "type", cJSON_CreateNumber(RECG_PARA_INFO));
	}
	cJSON_AddItemToObject(get_init_info, "product_name", cJSON_CreateString(p_build_info.product_name));
	cJSON_AddItemToObject(get_init_info, "version", cJSON_CreateString(version));
	sprintf(buf, "%.2lf", recg_para.threshold_score);
	cJSON_AddItemToObject(get_init_info, "recg_threshold", cJSON_CreateString(buf));
	cJSON_AddItemToObject(get_init_info, "living_detect", cJSON_CreateNumber(recg_para.live_detect));
	cJSON_AddItemToObject(get_init_info, "min_face_pixel", cJSON_CreateNumber(recg_para.min_face_pixel));
	cJSON_AddItemToObject(get_init_info, "max_face_pixel", cJSON_CreateNumber(recg_para.max_face_pixel));
	cJSON_AddItemToObject(get_init_info, "max_track_faces", cJSON_CreateNumber(recg_para.max_track_faces));
	cJSON_AddItemToObject(get_init_info, "set_para_status", cJSON_CreateNumber(comm_info->set_para_status));
	cJSON_AddItemToObject(root, "header", header);
	cJSON_AddItemToObject(root, "get_init_info", get_init_info);

	return 0;
}

static int uvc_cjson_export_register_img(cJSON *root, cJSON *header, cJSON *export_register_img,
	uvc_comm_info_t *comm_info)
{
	cJSON *tmp = NULL;
	int max_id, img_lenth;
	person_info_t p_info;

	memset(&p_info, 0x0, sizeof(person_info_t));
	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id)
		cJSON_AddStringToObject(root, "cmd_id", comm_info->uvc_read_info->cmd_id);
	else
		return -1;
	pthread_mutex_unlock(&comm_info->response_lock);
	export_register_img = cJSON_CreateObject();
	max_id = db_person_info_query_max_id();
	if(max_id) {
		db_person_info_query_by_id(max_id, &p_info);
		cJSON_AddItemToObject(export_register_img, "latest_reg_img_path", cJSON_CreateString(p_info.regist_photo_name));
		strcat(p_info.name, "-small.jpg");
		sprintf(p_info.regist_photo_name, "%s/%s",p_info.regist_photo_name,p_info.name);
		comm_info->reg_img_file = fopen(p_info.regist_photo_name, "rb");
		if(!comm_info->reg_img_file) {
			printf("%s,open %s fail!\n",__FUNCTION__,p_info.regist_photo_name);
			return -1;
		}
		fseek(comm_info->reg_img_file, 0, SEEK_END);
		img_lenth = ftell(comm_info->reg_img_file);
		fseek(comm_info->reg_img_file, 0, SEEK_SET);
		cJSON_AddItemToObject(export_register_img, "latest_reg_img_size", cJSON_CreateNumber(img_lenth));
		cJSON_AddItemToObject(export_register_img, "latest_reg_img_name", cJSON_CreateString(p_info.name));
		if(comm_info->reg_img_file) {
			fclose(comm_info->reg_img_file);
			comm_info->reg_img_file = NULL;
		}
	} else {
		cJSON_AddItemToObject(export_register_img, "have_not_reg_person", cJSON_CreateNumber(1));
	}

	tmp = cJSON_GetObjectItem(header,"type");
	if(tmp != NULL) {
		cJSON_ReplaceItemInObject(header, "type", cJSON_CreateNumber(EXPORT_LATEST_REG_PERSON_IMG));
	} else {
		cJSON_AddItemToObject(header, "type", cJSON_CreateNumber(EXPORT_LATEST_REG_PERSON_IMG));
	}
	cJSON_AddItemToObject(root, "header", header);
	cJSON_AddItemToObject(root, "export_register_img", export_register_img);

	return 0;
}

static int uvc_cjson_recv_ota_file(cJSON *root, cJSON *header, uvc_comm_info_t *comm_info)
{
	cJSON *tmp = NULL;
	char buf[] = "/cache/";
	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id)
		cJSON_AddStringToObject(root, "cmd_id", comm_info->uvc_read_info->cmd_id);
	else
		return -1;
	pthread_mutex_unlock(&comm_info->response_lock);
	tmp = cJSON_GetObjectItem(header,"type");
	if(tmp != NULL) {
		cJSON_ReplaceItemInObject(header, "type", cJSON_CreateNumber(RECV_OTA_FILE));
	} else {
		cJSON_AddItemToObject(header, "type", cJSON_CreateNumber(RECV_OTA_FILE));
	}
	cJSON_AddItemToObject(root, "header", header);
	cJSON_AddItemToObject(root, "recv_ota_file_path", cJSON_CreateString(buf));

	return 0;
}

static int uvc_cjson_take_mod_pic(cJSON *root, cJSON *header, uvc_comm_info_t *comm_info)
{
	cJSON *tmp = NULL;
	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id)
		cJSON_AddStringToObject(root, "cmd_id", comm_info->uvc_read_info->cmd_id);
	else
		return -1;
	pthread_mutex_unlock(&comm_info->response_lock);
	tmp = cJSON_GetObjectItem(header,"type");
	if(tmp != NULL) {
		cJSON_ReplaceItemInObject(header, "type", cJSON_CreateNumber(TAKE_MOD_PIC));
	} else {
		cJSON_AddItemToObject(header, "type", cJSON_CreateNumber(TAKE_MOD_PIC));
	}
	cJSON_AddItemToObject(root, "header", header);
	cJSON_AddItemToObject(root, "take_mod_pic_success", cJSON_CreateNumber(comm_info->take_pic_status));

	return 0;
}

static void uvc_cjson_parse_header(cJSON *header, cJSON *json_string, uvc_comm_info_t *comm_info)
{
	cJSON *tmp;
	header = cJSON_GetObjectItem(json_string, "header");
	if (!header) {
		printf("[%s]fail to get item_header\n", __FUNCTION__);
		return;
	}

	tmp = cJSON_GetObjectItem(header, "type");
	if(tmp != NULL)
		comm_info->uvc_read_info->type = tmp->valueint;
}

static void uvc_parse_cjson_register(cJSON *register_info, cJSON *json_string, uvc_comm_info_t *comm_info)
{
	cJSON *tmp = NULL;
	register_info = cJSON_GetObjectItem(json_string, "register_info");
	if (!register_info) {
		printf("[%s]fail to get register\n", __FUNCTION__);
		return;
	}

	tmp = cJSON_GetObjectItem(register_info, "name");
	pthread_mutex_lock(&comm_info->response_lock);
	if(tmp != NULL) {
		if(!comm_info->uvc_read_info->name) {
			comm_info->uvc_read_info->name = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
			memset(comm_info->uvc_read_info->name, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
			memcpy(comm_info->uvc_read_info->name, tmp->valuestring, strlen(tmp->valuestring));
		}
	}
	tmp = cJSON_GetObjectItem(json_string, "cmd_id");
	if(tmp != NULL) {
		if(!comm_info->uvc_read_info->cmd_id) {
			comm_info->uvc_read_info->cmd_id = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
			memset(comm_info->uvc_read_info->cmd_id, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
			memcpy(comm_info->uvc_read_info->cmd_id, tmp->valuestring, strlen(tmp->valuestring));
		}
	}
	pthread_mutex_unlock(&comm_info->response_lock);

}

static int uvc_parse_cjson_unregister(cJSON *unregister_info, cJSON *json_string, uvc_comm_info_t *comm_info)
{
	int unregister_flag = 0;
	cJSON *tmp = NULL;
	unregister_info = cJSON_GetObjectItem(json_string, "unregister_info");
	if (!unregister_info) {
		printf("[%s]fail to get unregister\n", __FUNCTION__);
		return -1;
	}

	tmp = cJSON_GetObjectItem(unregister_info, "delete_all");
	if(tmp != NULL)
		unregister_flag = 1;

	tmp = cJSON_GetObjectItem(unregister_info, "name");
	pthread_mutex_lock(&comm_info->response_lock);
	if(tmp != NULL) {
		if(!comm_info->uvc_read_info->name) {
			comm_info->uvc_read_info->name = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
			memset(comm_info->uvc_read_info->name, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
			memcpy(comm_info->uvc_read_info->name, tmp->valuestring, strlen(tmp->valuestring));
		}
	}
	tmp = cJSON_GetObjectItem(json_string, "cmd_id");
	if(tmp != NULL) {
		if(!comm_info->uvc_read_info->cmd_id) {
			comm_info->uvc_read_info->cmd_id = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
			memset(comm_info->uvc_read_info->cmd_id, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
			memcpy(comm_info->uvc_read_info->cmd_id, tmp->valuestring, strlen(tmp->valuestring));
		}
	}
	pthread_mutex_unlock(&comm_info->response_lock);

	return unregister_flag;
}

static int uvc_parse_cjson_recg_para(cJSON *recg_para, cJSON *json_string, uvc_comm_info_t *comm_info)
{
	cJSON *tmp = NULL;
	int set_para_flag = 0;
	tmp = cJSON_GetObjectItem(json_string, "get_init_info");

	if (tmp != NULL) {
		tmp = cJSON_GetObjectItem(json_string, "cmd_id");
		pthread_mutex_lock(&comm_info->response_lock);
		if(tmp != NULL) {
			if(!comm_info->uvc_read_info->cmd_id) {
				comm_info->uvc_read_info->cmd_id = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
				memset(comm_info->uvc_read_info->cmd_id, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
				memcpy(comm_info->uvc_read_info->cmd_id, tmp->valuestring, strlen(tmp->valuestring));
			}
		}
		pthread_mutex_unlock(&comm_info->response_lock);
	} else {
		recg_para = cJSON_GetObjectItem(json_string, "recg_para_info");
		if(!recg_para) {
			printf("[%s]fail to get recg_para\n", __FUNCTION__);
			return -1;
		}
		set_para_flag = 1;
		tmp = cJSON_GetObjectItem(recg_para, "recg_threshold");
		if(tmp != NULL)
			comm_info->recg_para.threshold_score = atof(tmp->valuestring);

		tmp = cJSON_GetObjectItem(recg_para, "living_detect");
		if(tmp != NULL)
			comm_info->recg_para.live_detect = tmp->valueint;

		tmp = cJSON_GetObjectItem(recg_para, "min_face_pixel");
		if(tmp != NULL)
			comm_info->recg_para.min_face_pixel = tmp->valueint;

		tmp = cJSON_GetObjectItem(recg_para, "max_face_pixel");
		if(tmp != NULL)
			comm_info->recg_para.max_face_pixel = tmp->valueint;

		tmp = cJSON_GetObjectItem(recg_para, "max_track_faces");
		if(tmp != NULL)
			comm_info->recg_para.max_track_faces = tmp->valueint;

		tmp = cJSON_GetObjectItem(json_string, "cmd_id");
		pthread_mutex_lock(&comm_info->response_lock);
		if(tmp != NULL) {
			if(!comm_info->uvc_read_info->cmd_id) {
				comm_info->uvc_read_info->cmd_id = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
				memset(comm_info->uvc_read_info->cmd_id, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
				memcpy(comm_info->uvc_read_info->cmd_id, tmp->valuestring, strlen(tmp->valuestring));
			}
		}
		pthread_mutex_unlock(&comm_info->response_lock);
	}

	return set_para_flag;
}

static void uvc_parse_cjson_export_img(cJSON *json_string, uvc_comm_info_t *comm_info)
{
	cJSON *tmp = NULL;

	tmp = cJSON_GetObjectItem(json_string, "cmd_id");
	pthread_mutex_lock(&comm_info->response_lock);
	if(tmp != NULL) {
		if(!comm_info->uvc_read_info->cmd_id) {
			comm_info->uvc_read_info->cmd_id = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
			memset(comm_info->uvc_read_info->cmd_id, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
			memcpy(comm_info->uvc_read_info->cmd_id, tmp->valuestring, strlen(tmp->valuestring));
		}
	}
	pthread_mutex_unlock(&comm_info->response_lock);
}

static void uvc_parse_cjson_recv_ota_file(cJSON *json_string, uvc_comm_info_t *comm_info)
{
	cJSON *tmp = NULL;

	tmp = cJSON_GetObjectItem(json_string, "recv_ota_file_success");

	if(tmp != NULL) {
		printf("enter OTA upgrade status!\n");
		system("fw_upgrade");
	}

	tmp = cJSON_GetObjectItem(json_string, "cmd_id");
	pthread_mutex_lock(&comm_info->response_lock);
	if(tmp != NULL) {
		if(!comm_info->uvc_read_info->cmd_id) {
			comm_info->uvc_read_info->cmd_id = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
			memset(comm_info->uvc_read_info->cmd_id, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
			memcpy(comm_info->uvc_read_info->cmd_id, tmp->valuestring, strlen(tmp->valuestring));
		}
	}
	pthread_mutex_unlock(&comm_info->response_lock);
}

static int uvc_parse_cjson_take_mod_pic(cJSON *take_pic_rect, cJSON *json_string, uvc_comm_info_t *comm_info)
{
	cJSON *tmp = NULL;
	map_params_t map_param;
	uvc_map_t maps;
	int ret = -1;
	int i;

	take_pic_rect = cJSON_GetObjectItem(json_string, "take_pic_rect");
	if(!take_pic_rect) {
		printf("[%s]fail to get take_pic_rect\n", __FUNCTION__);
		return ret;
	}

	tmp = cJSON_GetObjectItem(take_pic_rect, "start_cal_map");
	if(tmp != NULL) {
		if(tmp->valueint) {
			ret = face_cal_mod_map(&map_param);
			if (ret == 0) {
			    memset(&maps, 0, sizeof(uvc_map_t));
			    for (i = 0; i < CALIBRATION_NUM; i++) {
				maps.x_map[i] = map_param.x_map[i];
				maps.y_map[i] = map_param.y_map[i];
			    }
			    ret = uvc_save_mod_map_param(&maps);
			}
		}
		comm_info->start_cal_map = tmp->valueint;
	}

	tmp = cJSON_GetObjectItem(take_pic_rect, "x");
	if(tmp != NULL)
		comm_info->uvc_read_info->x = tmp->valueint;

	tmp = cJSON_GetObjectItem(take_pic_rect, "y");
	if(tmp != NULL)
		comm_info->uvc_read_info->y = tmp->valueint;

	tmp = cJSON_GetObjectItem(take_pic_rect, "w");
	if(tmp != NULL)
		comm_info->uvc_read_info->w = tmp->valueint;

	tmp = cJSON_GetObjectItem(take_pic_rect, "h");
	if(tmp != NULL)
		comm_info->uvc_read_info->h = tmp->valueint;

	tmp = cJSON_GetObjectItem(json_string, "cmd_id");
	pthread_mutex_lock(&comm_info->response_lock);
	if(tmp != NULL) {
		if(!comm_info->uvc_read_info->cmd_id) {
			comm_info->uvc_read_info->cmd_id = (char *)malloc(sizeof(char) * (strlen(tmp->valuestring) + 1));
			memset(comm_info->uvc_read_info->cmd_id, 0x0, sizeof(char) * (strlen(tmp->valuestring) + 1));
			memcpy(comm_info->uvc_read_info->cmd_id, tmp->valuestring, strlen(tmp->valuestring));
		}
	}
	pthread_mutex_unlock(&comm_info->response_lock);

	return ret;
}

void response_person_reg(void *arg)
{
	uvc_comm_info_t *comm_info = (uvc_comm_info_t *)arg;
	int ret = 0;
	cJSON *root = cJSON_CreateObject();
	cJSON *header = cJSON_CreateObject();
	cJSON *register_info = cJSON_CreateObject();
	prctl(PR_SET_NAME, "response_person_reg");

	ret = uvc_cjson_register_info(root, header, register_info, comm_info);
	if(!ret)
		uvc_write_info_to_file(comm_info->write_respone_info_fd, root);

	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id) {
		free(comm_info->uvc_read_info->cmd_id);
		comm_info->uvc_read_info->cmd_id = NULL;
	}

	if(comm_info->uvc_read_info->name) {
		free(comm_info->uvc_read_info->name);
		comm_info->uvc_read_info->name = NULL;
	}
	pthread_mutex_unlock(&comm_info->response_lock);
}

void response_person_unreg(void *arg)
{
	uvc_comm_info_t *comm_info = (uvc_comm_info_t *)arg;
	int ret = 0;
	cJSON *root = cJSON_CreateObject();
	cJSON *header = cJSON_CreateObject();
	cJSON *unregister_info = cJSON_CreateObject();
	prctl(PR_SET_NAME, "response_person_unreg");

	ret = uvc_cjson_unregister_info(root, header, unregister_info, comm_info);
	if(!ret)
		uvc_write_info_to_file(comm_info->write_respone_info_fd, root);

	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id) {
		free(comm_info->uvc_read_info->cmd_id);
		comm_info->uvc_read_info->cmd_id = NULL;
	}

	if(comm_info->uvc_read_info->name) {
		free(comm_info->uvc_read_info->name);
		comm_info->uvc_read_info->name = NULL;
	}
	pthread_mutex_unlock(&comm_info->response_lock);
}

void response_get_init_info(void *arg)
{
	uvc_comm_info_t *comm_info = (uvc_comm_info_t *)arg;
	int ret = 0;
	cJSON *root = cJSON_CreateObject();
	cJSON *header = cJSON_CreateObject();
	cJSON *get_init_info = cJSON_CreateObject();
	prctl(PR_SET_NAME, "response_get_init_info");

	ret = uvc_cjson_get_init_info(root, header, get_init_info, comm_info);
	if(!ret)
		uvc_write_info_to_file(comm_info->write_respone_info_fd, root);

	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id) {
		free(comm_info->uvc_read_info->cmd_id);
		comm_info->uvc_read_info->cmd_id = NULL;
	}

	if(comm_info->uvc_read_info->name) {
		free(comm_info->uvc_read_info->name);
		comm_info->uvc_read_info->name = NULL;
	}
	pthread_mutex_unlock(&comm_info->response_lock);
}

void response_export_img(void *arg)
{
	uvc_comm_info_t *comm_info = (uvc_comm_info_t *)arg;
	int ret = 0;
	cJSON *root = cJSON_CreateObject();
	cJSON *header = cJSON_CreateObject();
	cJSON *export_register_img = cJSON_CreateObject();
	prctl(PR_SET_NAME, "response_export_img");

	ret = uvc_cjson_export_register_img(root, header, export_register_img,comm_info);
	if(!ret)
		uvc_write_info_to_file(comm_info->write_respone_info_fd, root);

	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id) {
		free(comm_info->uvc_read_info->cmd_id);
		comm_info->uvc_read_info->cmd_id = NULL;
	}

	if(comm_info->uvc_read_info->name) {
		free(comm_info->uvc_read_info->name);
		comm_info->uvc_read_info->name = NULL;
	}
	pthread_mutex_unlock(&comm_info->response_lock);
}

void response_recv_ota_file(void *arg)
{
	uvc_comm_info_t *comm_info = (uvc_comm_info_t *)arg;
	int ret = 0;
	cJSON *root = cJSON_CreateObject();
	cJSON *header = cJSON_CreateObject();
	prctl(PR_SET_NAME, "response_recv_ota_file");

	ret = uvc_cjson_recv_ota_file(root, header, comm_info);
	if(!ret)
		uvc_write_info_to_file(comm_info->write_respone_info_fd, root);

	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id) {
		free(comm_info->uvc_read_info->cmd_id);
		comm_info->uvc_read_info->cmd_id = NULL;
	}

	if(comm_info->uvc_read_info->name) {
		free(comm_info->uvc_read_info->name);
		comm_info->uvc_read_info->name = NULL;
	}
	pthread_mutex_unlock(&comm_info->response_lock);
}

void response_take_mod_pic(void *arg)
{
	uvc_comm_info_t *comm_info = (uvc_comm_info_t *)arg;
	int ret = 0;
	cJSON *root = cJSON_CreateObject();
	cJSON *header = cJSON_CreateObject();
	prctl(PR_SET_NAME, "response_take_mod_pic");

	ret = uvc_cjson_take_mod_pic(root, header, comm_info);
	if(!ret)
		uvc_write_info_to_file(comm_info->write_respone_info_fd, root);

	pthread_mutex_lock(&comm_info->response_lock);
	if(comm_info->uvc_read_info->cmd_id) {
		free(comm_info->uvc_read_info->cmd_id);
		comm_info->uvc_read_info->cmd_id = NULL;
	}

	if(comm_info->uvc_read_info->name) {
		free(comm_info->uvc_read_info->name);
		comm_info->uvc_read_info->name = NULL;
	}
	pthread_mutex_unlock(&comm_info->response_lock);
}

void uvc_read_info(uvc_comm_info_t *comm_info)
{
	int ret;
	char buf[EVENT_BUF_LEN];
	struct inotify_event *event;
	char *ptr;
	fd_set fdsu;

	ftruncate(comm_info->read_info_fd, 0);
	lseek(comm_info->read_info_fd, 0, SEEK_SET);

	comm_info->uvc_read_info = malloc(sizeof(uvc_cjson_info_t));
	if(!comm_info->uvc_read_info) {
		printf("uvc_cjson_info_t malloc err\n");
		return;
	}
	memset(comm_info->uvc_read_info, 0x0, sizeof(uvc_cjson_info_t));
	memset(&recg_para, 0x0, sizeof(recognition_para_t));

	comm_info->inotify_file_fd = inotify_init();
	if(comm_info->inotify_file_fd < 0) {
		printf("inotify init fail!\n");
		return;
	}

	comm_info->inotify_watch_fd = inotify_add_watch(comm_info->inotify_file_fd, UVC_INFO_READ_JSON_FILE, IN_MODIFY);
	if(comm_info->inotify_watch_fd == -1) {
		printf("inotify_add_watch fail!\n");
		return;
	}

	while(1) {
		FD_ZERO(&fdsu);
		FD_SET(comm_info->inotify_file_fd, &fdsu);
		ret = select(comm_info->inotify_file_fd + 1, &fdsu, NULL, NULL, NULL);

		if (ret < 0) {
			ALOGE("select error %s (%d)\n", strerror(errno), errno);
			if (EINTR == errno)
				continue;
			break;
		} else if (!ret) {
			ALOGE("select timeout\n");
			break;
		}

		if (FD_ISSET(comm_info->inotify_file_fd, &fdsu)) {

			int length = read(comm_info->inotify_file_fd, buf, sizeof(buf));
			if(length < 0)
				printf("read inotify_event fail!\n");

			for(ptr = buf; ptr < buf + length;
				ptr += sizeof(struct inotify_event) + event->len) {
				event = (struct inotify_event *)ptr;

				if(event->mask & IN_MODIFY) {
					printf("%s[%d],file is modified!\n",__FUNCTION__,__LINE__);
					cJSON *json = NULL;
					cJSON *item_header, *item_register, *item_unregister, *item_recg_para, *item_take_mod_pic;
					json = uvc_read_info_from_file(comm_info->read_info_fd);

					if (json) {
						uvc_cjson_parse_header(item_header, json, comm_info);
						if (comm_info->uvc_read_info->type == REG_PERSON) {
							uvc_parse_cjson_register(item_register, json, comm_info);
							app_management_reg_start(comm_info->uvc_read_info->name);
							ret = app_management_get_reg_result();
							pthread_mutex_lock(&comm_info->response_lock);
							comm_info->reg_result_status = ret;
							pthread_mutex_unlock(&comm_info->response_lock);
							ret = threadpool_add(comm_info->pool, response_person_reg, (void *)comm_info, 0);
							if (ret) {
								ALOGE("%s: failed to add task to thread pool\n", __func__);
							}
						} else if(comm_info->uvc_read_info->type == UNREG_PERSON) {
							ret = uvc_parse_cjson_unregister(item_unregister, json, comm_info);
							if(ret) {
								if (db_person_info_delete_all() == 0 && db_punch_record_delete_all() == 0) {
									face_clear_face_list();
									face_db_update();
									pthread_mutex_lock(&comm_info->response_lock);
									comm_info->unreg_result_status = PERSON_UNREG_OK;
									if (is_dir_exist(DIR_REGISTER) == 0) {
										if (dir_remove(DIR_REGISTER) == 0) {
											printf("Successfully deleted all registrants' info!\n");
										} else {
											printf("Deleting registered personnel info failed!\n");
											comm_info->unreg_result_status = PERSON_UNREG_FAIL;
										}
									}
									if (is_dir_exist(DIR_RECG) == 0) {
										if (dir_remove(DIR_RECG) == 0) {
											printf("Successfully deleted all registrants' punch record!\n");
										} else {
											printf("Deleting registered personnel punch record failed!\n");
											comm_info->unreg_result_status = PERSON_UNREG_FAIL;
										}
									}
								} else {
									printf("Deleting registered personnel failed!\n");
									pthread_mutex_lock(&comm_info->response_lock);
									comm_info->unreg_result_status = PERSON_UNREG_FAIL;
								}
								pthread_mutex_unlock(&comm_info->response_lock);

							} else {
								ee_queue_t head;
								int count = 0;

								count = db_person_info_query_begin_by_name(comm_info->uvc_read_info->name, &head);
								db_person_info_query_end(&head);

								if(count <= 0) {
									pthread_mutex_lock(&comm_info->response_lock);
									comm_info->unreg_result_status = PERSON_HAVE_NOT;
									pthread_mutex_unlock(&comm_info->response_lock);
									ret = threadpool_add(comm_info->pool, response_person_unreg, (void *)comm_info, 0);
									if (ret) {
										ALOGE("%s: failed to add task to thread pool\n", __func__);
									}
									continue;
								}
								pthread_mutex_lock(&comm_info->response_lock);
								if(person_lookup_del_by_name(comm_info->uvc_read_info->name) == 0) {
									face_clear_face_list_by_name(comm_info->uvc_read_info->name);
									face_db_update();
									face_reset_recg();
									printf("Successfully deleted %s!\n", comm_info->uvc_read_info->name);
									comm_info->unreg_result_status = PERSON_UNREG_OK;
								} else {
									printf("Deleting %s failed!\n", comm_info->uvc_read_info->name);
									comm_info->unreg_result_status = PERSON_UNREG_FAIL;
								}
								pthread_mutex_unlock(&comm_info->response_lock);
							}
							ret = threadpool_add(comm_info->pool, response_person_unreg, (void *)comm_info, 0);
							if (ret) {
								ALOGE("%s: failed to add task to thread pool\n", __func__);
							}

						} else if(comm_info->uvc_read_info->type == RECG_PARA_INFO) {
							ret = uvc_parse_cjson_recg_para(item_recg_para, json, comm_info);

							if(ret) {
								pthread_mutex_lock(&comm_info->response_lock);
								if ((recg_para.threshold_score == comm_info->recg_para.threshold_score) &&
									(recg_para.live_detect == comm_info->recg_para.live_detect) &&
									(recg_para.min_face_pixel == comm_info->recg_para.min_face_pixel) &&
									(recg_para.max_face_pixel == comm_info->recg_para.max_face_pixel) &&
									(recg_para.max_track_faces == comm_info->recg_para.max_track_faces)) {
									printf("%s: parameter is not change\n", __func__);
									comm_info->set_para_status = ret;
								} else {
									ret = recognition_set_param(&comm_info->recg_para);
									comm_info->set_para_status = ret;
								}
								pthread_mutex_unlock(&comm_info->response_lock);
							}
							ret = threadpool_add(comm_info->pool, response_get_init_info, (void *)comm_info, 0);
							if (ret) {
								ALOGE("%s: failed to add task to thread pool\n", __func__);
							}
						} else if(comm_info->uvc_read_info->type == EXPORT_LATEST_REG_PERSON_IMG) {
							uvc_parse_cjson_export_img(json, comm_info);
							ret = threadpool_add(comm_info->pool, response_export_img, (void *)comm_info, 0);
							if (ret) {
								ALOGE("%s: failed to add task to thread pool\n", __func__);
							}

						} else if(comm_info->uvc_read_info->type == RECV_OTA_FILE) {
							uvc_parse_cjson_recv_ota_file(json, comm_info);
							ret = threadpool_add(comm_info->pool, response_recv_ota_file, (void *)comm_info, 0);
							if (ret) {
								ALOGE("%s: failed to add task to thread pool\n", __func__);
							}

						} else if(comm_info->uvc_read_info->type == TAKE_MOD_PIC) {
							ret = uvc_parse_cjson_take_mod_pic(item_take_mod_pic, json, comm_info);
							if(!comm_info->start_cal_map) {
								ret = face_take_mod_pic(comm_info->uvc_read_info->x - 40, comm_info->uvc_read_info->y - 40,
									comm_info->uvc_read_info->w + 80, comm_info->uvc_read_info->h + 80);
							}

							comm_info->take_pic_status = ret;
							ret = threadpool_add(comm_info->pool, response_take_mod_pic, (void *)comm_info, 0);
							if (ret) {
								ALOGE("%s: failed to add task to thread pool\n", __func__);
							}

						}
						cJSON_Delete(json);
					} else {
						printf("cJSON_Parse err, json is null\n");
					}

				}
			}

		}

	}
}

