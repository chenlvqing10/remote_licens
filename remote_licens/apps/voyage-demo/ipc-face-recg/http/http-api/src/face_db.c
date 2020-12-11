/*
 * api/face_db.c - all functions for face and person data
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>

#include "cJSON.h"
#include "config_db.h"

#define LOG_TAG "face_db.c"
#include <log/log.h>

#include "base64.h"
#include "config.h"
#include "person.h"
#include "face_db.h"
#include "sqlite3.h"
#include "web_biz_api.h"
#include "face.h"
#include "lb_os_http.h"
#include "db_person_info.h"
#include "time_conv.h"
#include "system_admin.h"
#include "sys_info.h"
/*
#include "system_face_db_update.h"
*/

#define ONE_PACK_MAX_RECORD_NUM 20

/**
 * remove_file: delete file
 * path: file name
 *
 * Returns 0: success.
 */
int remove_file(char *path)
{
	if (access(path, 0) == 0) {
		remove(path);
		sync();
	}

	return 0;
}

/**
 * person_reg_param_init: initialize person register parameter
 * param: person_reg_param_t
 * len: len of jpg data
 *
 * Returns None.
 */
void person_reg_param_init(person_reg_param_t *param)
{
	param->check_face_size = config_get_int("register", "check_face_size", 0);
	param->face_max = config_get_int("register", "face_max", 512);
	param->face_min = config_get_int("register", "face_min", 120);
	param->quality = config_get_int("register", "reg_quality", 0);
	param->threshold = config_get_double("register", "reg_threshold", 0.5f);
	ALOGD("%d------------->%f\n", __LINE__, param->threshold);
	param->refresh_db_each_time = config_get_int("register",
		"refresh_db_each_time", 0);
	param->jpeg_width_stride = config_get_int("register", "jpeg_w_stride", 800);
	param->jpeg_height_stride = config_get_int("register", "jpeg_h_stride", 1024);
	param->batch_reg = 0;
}

/**
 * append_pic_data_to_default_file: write jpg to file
 * pic: jpg data
 * len: len of jpg data
 *
 * Returns 0: success, -1: failure.
 */
int append_pic_data_to_default_file(char *pic, int len)
{
	FILE *flip = NULL;
	int count = 0;

	flip = fopen(REG_PIC_DEFAULT_PATH, "wb");
	if (flip == NULL) {
		ALOGE("[%s]open %s failed\n", __func__, REG_PIC_DEFAULT_PATH);
		return -1;
	}

	count = fwrite(pic, 1, len, flip);
	if (count != len) {
		ALOGE("write file(%s) failed, size:%d != write_count:%d\n",
					REG_PIC_DEFAULT_PATH, len, count);
		fclose(flip);
		return -1;
	}
	fflush(flip);
	fclose(flip);

	return 0;
}

/**
 * append_pic_data_to_default_file: write jpg to file
 * pic: jpg data
 * len: len of jpg data
 *
 * Returns 0: success, -1: failure.
 */
int append_pic_data_to_default_file_cloud(char *pic, int len)
{
	FILE *flip = NULL;
	int count = 0;

	flip = fopen(REG_PIC_DEFAULT_PATH2, "ab");
	if (flip == NULL) {
		ALOGE("[%s]open %s failed\n", __func__, REG_PIC_DEFAULT_PATH2);
		return -1;
	}

	count = fwrite(pic, 1, len, flip);
	if (count != len) {
		ALOGE("write file(%s) failed, size:%d != write_count:%d\n",
					REG_PIC_DEFAULT_PATH2, len, count);
		fclose(flip);
		return -1;
	}
	fflush(flip);
	fclose(flip);

	return 0;
}

int get_e_code(int e_code)
{
	int ret = 0;
	if ((int)e_code == 2)
		ret = PERSON_INFO_DATA_ERROR;
	else if ((int)e_code == 3)
		ret = PERSON_INFO_NO_DETECT_FACE;
	else if ((int)e_code == 4)
		ret = PERSON_INFO_FACE_FEATURE_FAIL;
	else if ((int)e_code == 5)
		ret = PERSON_INFO_MULTIPLE_FACE;
	else if ((int)e_code == 6)
		ret = PERSON_INFO_FACE_SIZE_ERROR;
	else if ((int)e_code == 7)
		ret = PERSON_INFO_IMAGE_DECODE_FAIL;
	else if ((int)e_code == 8)
		ret = PERSON_INFO_PIC_COPY_ERROR;
	else if ((int)e_code == 9)
		ret = PERSON_INFO_PIC_NO_HAVE;
	else if ((int)e_code == 10)
		ret = PERSON_INFO_INSERT_DB_ERROR;
	else if ((int)e_code == 11)
		ret = PERSON_INFO_FACE_QUALITY_TOO_LOW;
	else if ((int)e_code == 12)
		ret = PERSON_INFO_FACE_SIMILARITY_TOO_HIGH;
	else if ((int)e_code == 13)
		ret = PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT;
	else if ((int)e_code == 14)
		ret = PERSON_INFO_IMAGE_FORMAT_ERROR;
	else if ((int)e_code == 15)
		ret = PERSON_INFO_INTERNAL_ERROR;
	else if ((int)e_code == 16)
		ret = PERSON_REGISTRATION_ERROR;
	else if ((int)e_code == 17)
		ret = PERSON_MIS_RECOGNIZED;
	else if ((int)e_code == 18)
		ret = PERSON_REG_COMPARE_RESULT_OK;

	return ret;
}

/**
 * add_member: add member to sqlite
 * p_info: the point of the struct face_number_info
 *
 * Returns 0: success, -1: failure.
 */
int add_member(face_number_info *p_info)
{
	int ret = -1, jpg_len = 0;
	img_info_t st_img;
	person_t pst_person;
	person_reg_param_t reg_param;
	e_person_reg_code e_code = PERSON_REG_OK;
	char detail_info[64] = {0};
	char *jpg_data;

	/** para check */
	if (p_info == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}
	if (!p_info->file_data) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_INIT;
		goto Exit;
	}

	/** para init */
	person_reg_param_init(&reg_param);
	memset(&pst_person, 0, sizeof(person_t));

	strcpy(st_img.pic_path, REG_PIC_DEFAULT_PATH);
	strcpy(pst_person.employee_number, p_info->employee_number);
	strcpy(pst_person.name, p_info->name);

	ret = base64_decode(p_info->file_data, strlen(p_info->file_data),
		&jpg_data, &jpg_len);
	if (!ret && jpg_data && (jpg_len > 0))
		append_pic_data_to_default_file((char *)jpg_data, jpg_len);
	else {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_JPEG_DECODE;
		goto Exit;
	}

	/** update data to sqlite */
	ret = person_reg_proc(&pst_person, &st_img, detail_info,
			sizeof(detail_info), &reg_param, &e_code, 0);
	if (ret) {
		ALOGE("[%s : %d : e_code %d]\n", __func__, __LINE__, e_code);
		ret = get_e_code((int)e_code);
		ALOGE("[%s : %d : ret %d]\n", __func__, __LINE__, ret);
		goto Exit;
	}
	/** update data to server */
	ret = face_db_update();
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = ERR_FACE_DB_UPDATE;
		goto Exit;
	}

Exit:
	remove_file(REG_PIC_DEFAULT_PATH);
	return ret;
}

/**
 * search_member: search member from database
 * p_info: the point to face_number_info
 * p_pack_info: the point to face_number_pack_info
 *
 * Returns 0: success, -1: failure.
 */
int search_member(face_number_info *p_info, face_number_pack_info *p_pack_info)
{
	ee_queue_t head;
	person_info_t *q = NULL;
	char buf[128] = {0};
	int record_count, record_offset, record_total, ret, i = 0;

	/** para check */
	if (p_info == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	/** para init */
	ee_queue_init(&head);
	record_count = record_offset = record_total = 0;
	record_offset = p_info->start_id;

	if (record_count < 0)
		record_count = 1;
	if (record_count > ONE_PACK_MAX_RECORD_NUM)
		record_count = ONE_PACK_MAX_RECORD_NUM;

	record_total = db_person_info_count();
	if (record_total < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	ret = db_person_info_query_begin(p_info->name,
		p_info->start_id,
		p_info->record_count,
		&head);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}
	ALOGD("[%s --> %d]", __func__, ret);

	p_info->record_total = record_total;
	if (p_info->record_count > ret)
		p_info->record_count = ret;

	ret = 0;
	ee_queue_foreach(q, &head, person_info_t, queue) {
		p_pack_info[i].id = q->id;
		p_pack_info[i].access_right = q->access_right;
		strcpy(p_pack_info[i].employee_number, q->employee_number);
		strcpy(p_pack_info[i].name, q->name);
		strcpy(p_pack_info[i].gender, q->gender);
		strcpy(p_pack_info[i].nationa, q->nationa);
		strcpy(p_pack_info[i].id_card_number, q->id_card_number);
		strcpy(p_pack_info[i].mobile, q->mobile);
		strcpy(p_pack_info[i].department_name, q->department_name);

		memset(buf, 0, sizeof(buf));
		time_second_to_str(q->temporary_access_start_time, buf, sizeof(buf));
		strcpy(p_pack_info[i].temporary_access_start_time, buf);

		memset(buf, 0, sizeof(buf));
		time_second_to_str(q->temporary_access_end_time, buf, sizeof(buf));
		strcpy(p_pack_info[i].temporary_access_end_time, buf);
		i++;
	}

Exit:
	db_person_info_query_end(&head);
	return ret;
}

/**
 * update_member: search member from database
 * p_info: the point to face_number_info
 *
 * Returns 0: success, -1: failure.
 */
int update_member(face_number_info *p_info)
{
	person_info_t person;
	char *bin_data;
	int bin_len = 0, ret = 0;

	/** para check */
	if (p_info == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}
	if (strlen(p_info->employee_number) == 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	/** para init */
	memset(&person, 0, sizeof(person_t));
	person.access_right = -1;
	person.temporary_access_start_time = -1;
	person.temporary_access_end_time = -1;

	strcpy(person.employee_number, p_info->employee_number);
	strcpy(person.name, p_info->name);
	strcpy(person.gender, p_info->gender);
	strcpy(person.nationa, p_info->nationa);
	strcpy(person.id_card_number, p_info->id_card_number);
	strcpy(person.mobile, p_info->mobile);
	strcpy(person.department_name, p_info->department_name);
	person.access_right = p_info->access_right;
	person.face_norm[0] = p_info->face_norm[0];
	person.face_norm[1] = p_info->face_norm[1];
	person.temporary_access_start_time = p_info->temporary_access_start_time;
	person.temporary_access_end_time = p_info->temporary_access_end_time;

	if (strlen(p_info->face_features) > 0) {
		ret = base64_decode(p_info->face_features, strlen(p_info->face_features),
			&bin_data, &bin_len);
		if (!ret && bin_data && (bin_len == 1024))
				memcpy(person.face_features, bin_data, bin_len);
	}
	if (strlen(p_info->face_key_points) > 0) {
		ret = base64_decode(p_info->face_key_points,
			strlen(p_info->face_key_points),
			&bin_data, &bin_len);
		if (!ret && bin_data && (bin_len > 0)) {
			memcpy(person.face_key_points, bin_data, bin_len);
		}
	}

	ret = db_person_info_update(p_info->name, &person);
	if (ret != SQLITE_OK) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}
	ret = face_db_update();
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -3;
		goto Exit;
	}

Exit:
	return ret;
}

/**
 * total_number: total member from database
 * total_count: the point to total count
 *
 * Returns 0: success, -1: failure.
 */
int total_number(int *total_count)
{
	int ret = 0;

	/** para check */
	if (total_count == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	*total_count = db_person_info_count();
	if (*total_count < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

Exit:
	return ret;
}

/**
 * delete_one_member_by_json: delete one member by json object
 * @obj: json object of member information
 *
 * Returns 0 means success, other means failure.
 */
int delete_one_member_by_json(cJSON *obj)
{
	int ret = 0;
	person_info_t ps_info;
	cJSON *item;

	if (NULL == obj)
		return -1;

	memset(&ps_info, 0, sizeof(person_info_t));

	item = cJSON_GetObjectItem(obj, "employee_number");
	if (item)
		strcpy(ps_info.employee_number, item->valuestring);

	item = cJSON_GetObjectItem(obj, "name");
	if (item)
		strcpy(ps_info.name, item->valuestring);

	item = cJSON_GetObjectItem(obj, "access_right");
	if (item)
		ps_info.access_right = item->valueint;

	ret = db_person_info_delete(ps_info.name);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

	ret = face_db_update();
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -3;
		goto Exit;
	}

Exit:
	return ret;
}

/**
 * delete_members_by_json: delete members by json object
 * @json: json object of members information
 * @ret_arr: json array object of result
 *
 * Returns 0 means success, other means failure.
 */
int delete_members_by_json(cJSON *json, cJSON *ret_arr)
{
	int i, ret = -1;
	int arr_size;
	cJSON *obj, *value, *item;

	if (!json || !ret_arr)
		return -1;

	/* get size of add */
	arr_size = cJSON_GetArraySize(json);
	for (i = 0; i < arr_size; i++) {
		obj = cJSON_GetArrayItem(json, i);
		ret = delete_one_member_by_json(obj);
		/** if fail  pack failed data */
		if (ret) {
			value = cJSON_CreateObject();
			if (obj && value) {
				item = cJSON_GetObjectItem(obj, "employee_number");
				if (item) {
					cJSON_AddStringToObject(value, "employee_number",
						item->valuestring);
					cJSON_AddNumberToObject(value, "err_reason",
						ret);
				}
			}
			cJSON_AddItemToArray(ret_arr, value);
		}
	}

	return 0;
}

/**
 * delete_all_memmbers: delete all member
 *
 * Returns 0 means success, other means failure.
 */
int delete_all_memmbers()
{
	int query_count = 0;
	int ret = 0;
	ee_queue_t head;
	person_t person;
	person_info_t *q = NULL;

	memset(&person, 0, sizeof(person));
	person.access_right = -1;
	person.temporary_access_start_time = -1;
	person.temporary_access_end_time = -1;

	ee_queue_init(&head);
	query_count = db_person_info_query_begin("", 0, db_person_info_count(), &head);
	if (query_count < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	ee_queue_foreach(q, &head, person_info_t, queue) {
		ret = db_person_info_delete(q->name);
		if (ret) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			ret = -1;
			goto Exit;
		}
	}

	ret = face_db_update();
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

Exit:
	db_person_info_query_end(&head);
	return ret;
}

/**
 * delete_member: delete member from database
 * del_json_data: the point to delete json data
 * arr: the point to arr
 *
 * Returns 0: success, -1: failure.
 */
int delete_member(char *del_json_data, cJSON *arr)
{
	int ret = 0;
	cJSON *json = NULL;

	json = cJSON_Parse(del_json_data);
	if (!json) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	ret = delete_members_by_json(json, arr);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

	face_db_update();

Exit:
	return ret;
}

/**
 * update_admin_pwd: update administator password from database
 * old_pwd: the point to old password
 * new_pwd: the point to new password
 *
 * Returns 0: success, -1: failure.
 */
int update_admin_pwd(char *old_pwd, char *new_pwd)
{
	int ret = 0;
	char bs_username[32] = {0};
	char bs_password[32] = {0};

	/** para init */
	if (old_pwd == NULL || new_pwd == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	ret = system_admin_get_info(bs_username, sizeof(bs_username),
		bs_password, sizeof(bs_password));
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

	if (strcmp(old_pwd, bs_password) != 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -3;
		goto Exit;
	}

	ret = system_admin_uptate_info(bs_username, new_pwd);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -4;
		goto Exit;
	}

Exit:
	return ret;
}

/**
 * add_admin: add administator to database
 * use_name: the point to add adminstator name
 * use_name: the point to add adminstator password
 *
 * Returns 0: success, -1: failure.
 */
int add_admin(char *use_name, char *password)
{
	int ret;
	manager_info_t mgr_info;

	/** para init */
	if (use_name == NULL || password == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	memset(&mgr_info, 0, sizeof(manager_info_t));
	strcpy(mgr_info.user_name, use_name);
	strcpy(mgr_info.user_password, password);

	ret = db_manager_info_insert(&mgr_info);
	if (ret) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}
Exit:
	return ret;
}

/**
 * search_manger: search manger to database
 * p_info: the point to struct manger_face_info
 *
 * Returns 0: success, -1: failure.
 */
int search_manger(manger_face_info *p_info)
{
	int ret = 0;
	ee_queue_t head;
	manager_info_t *q = NULL;

	/** para init */
	if (p_info == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	memset(&head, 0, sizeof(head));
	ret = db_manager_info_query_begin(p_info->username, &head);
	if (ret < 0) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}
	ret = 0;

	ee_queue_foreach(q, &head, manager_info_t, queue) {
#if 0
		if (strcmp(q->user_name, "root")) {
			p_info->id = q->id;
			strcpy(p_info->username2, q->user_name);
			strcpy(p_info->password, q->user_password);
			break;
		}
#endif
		p_info->id = q->id;
		strcpy(p_info->username2, q->user_name);
		strcpy(p_info->password, q->user_password);
		break;
	}

Exit:
	db_manager_info_query_end(&head);
	return ret;
}

/**
 * update_number: update manger to database
 * user_name: the point to updated number
 * user_password: the point to new password
 *
 * Returns 0: success, -1: failure.
 */
int update_number(char *user_name, char *user_password)
{
	int ret = 0;
	manager_info_t mgr_info;

	/** para check */
	if (user_name == NULL ||
		user_password == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	memset(&mgr_info, 0, sizeof(manager_info_t));
	strcpy(mgr_info.user_name, user_name);
	strcpy(mgr_info.user_password, user_password);
	ret = db_manager_info_update(user_name, &mgr_info);
	if (ret != SQLITE_OK) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

Exit:
	return ret;
}

/**
 * delete_number: delete manger to database
 * user_name: the point to delete number
 *
 * Returns 0: success, -1: failure.
 */
int delete_number(char *user_name)
{
	int ret = 0;

	/** para check */
	if (user_name == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	if (!strcmp(user_name, "root")) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}

	ret = db_manager_info_delete(user_name);
	if (ret != SQLITE_OK) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -3;
		goto Exit;
	}

Exit:
	return ret;
}

/**
 * add_one_member_by_cjson: batch import members
 * @obj: json object for one employee information
 *
 * Returns none.
 */
int add_one_member_by_cjson(cJSON *obj, char *employee_number)
{
	int ret = 0;
	img_info_t st_img;
	e_person_reg_code e_code = PERSON_REG_OK;
	char detail_info[64];
	char *jpg_data;
	int jpg_len = 0;
	cJSON *item;
	person_t pst_person;
	person_reg_param_t reg_param;

	/** para check */
	if (!obj)
		return -1;

	/** para init */
	memset(detail_info, 0, sizeof(detail_info));
	memset(&st_img, 0, sizeof(img_info_t));
	memset(&pst_person, 0, sizeof(person_t));
	memset(&reg_param, 0, sizeof(person_reg_param_t));
	person_reg_param_init(&reg_param);

	item = cJSON_GetObjectItem(obj, "employee_number");
	if (item)
		strcpy(pst_person.employee_number, item->valuestring);

	item = cJSON_GetObjectItem(obj, "name");
	if (item)
		strcpy(pst_person.name, item->valuestring);

	item = cJSON_GetObjectItem(obj, "base64_pic");
	if (item) {
		/*
		append_pic_data_to_default_file(
			(char *)item->valuestring, strlen(item->valuestring));
		*/
		ret = base64_decode(item->valuestring,
			strlen(item->valuestring),
			&jpg_data, &jpg_len);
		if (ret) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			ret = ERR_JPEG_DECODE;
			goto Exit;
		}

		append_pic_data_to_default_file(
			(char *)jpg_data, jpg_len);
		strcpy(st_img.pic_path, REG_PIC_DEFAULT_PATH);

		ret = person_reg_proc(&pst_person, &st_img, detail_info,
			sizeof(detail_info), &reg_param, &e_code, 0);

		if (ret) {
			ALOGE("[%s : %d : e_code = %d]\n",
				__func__, __LINE__, e_code);
			ret = (int)e_code;
			goto Exit;
		}
#if 0
		system_face_db_update_recv_net_message();
#else
		ret = face_db_update();
		if (ret) {
			ALOGE("[%s : %d]\n", __func__, __LINE__);
			ret = ERR_FACE_DB_UPDATE;
			goto Exit;
		}
#endif
	}

Exit:
	/*
	remove_file(REG_PIC_DEFAULT_PATH);
	*/
	if (jpg_data) {
		lb_mem_free(jpg_data);
		jpg_data = NULL;
	}
	strcpy(employee_number, pst_person.employee_number);
	return ret;
}

/**
 * batch_import_members: batch import members
 * data: the point to menbers data
 * employee_number: the point to failed employee number
 *
 * Returns 0: success, -1: failure.
 */
int batch_import_members(char *data, cJSON *arr)
{
	int ret = 0, arr_size, i, ret_for_add = 0;
	cJSON *json = NULL, *number, *obj;
	char employee_number[64] = {0};

	/** para check */
	if (data == NULL) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -1;
		goto Exit;
	}

	json = cJSON_Parse(data);
	if (!json) {
		ALOGE("[%s : %d]\n", __func__, __LINE__);
		ret = -2;
		goto Exit;
	}
	arr_size = cJSON_GetArraySize(json); /* get size of add */
	ALOGD("[arr_size---> %d]", arr_size);
	printf("[arr_size---> %d]", arr_size);
	for (i = 0; i < arr_size; i++) {
		number = cJSON_GetArrayItem(json, i);
		ret = add_one_member_by_cjson(number, employee_number);
		printf("add_one_member_by_cjson(%s) ret %d\n", employee_number, ret);
		if (ret) {
			obj = cJSON_CreateObject();
			cJSON_AddStringToObject(obj, "employee_number", employee_number);
			cJSON_AddNumberToObject(obj, "err_reason", get_e_code(ret));
			cJSON_AddItemToArray(arr, obj);
			ret_for_add = 1;
		}
	}

Exit:
	if (json)
		cJSON_Delete(json);
	return ret_for_add;
}