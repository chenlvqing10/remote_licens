/*
 * face_db.h - export some interface function of face and person data
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

#ifndef _FACE_DB_H
#define _FACE_DB_H

#include "person.h"
#include "db_manager_info.h"

#define REG_PIC_DEFAULT_PATH		"/tmp/reg_default.jpg"
#define REG_PIC_DEFAULT_PATH2		"/tmp/reg_default2.jpg"
#define REG_PIC_DEFAULT_PATH3		"/tmp/reg_default3.jpg"

typedef struct face_db {
	/* Employee mumber(E.g SW200) */
	char employee_number[32];

	int id;

	/* Name */
	char name[32];

	/* Gender(E.g "Man/Women/..." */
	char gender[8];

	/* Nationa */
	char nationa[32];

	/* ID card number */
	char id_card_number[32];

	/* Telephone number */
	char mobile[20];

	/* Department name(E.g "Software Lab001") */
	char department_name[128];

	/* Access right
	 * Options:
	 *	0: Free access
	 *	1: Temporary access mode 1
	 *	2: Temporary access mode 2
	 */
	int access_right;

	/* Access card number.
	 * Effective when the punch mode is "Card reader"
	 */
	char access_card_number[32];

	/* When the permission is temporary access permission mode 1,
	 * Allowed start time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	double temporary_access_start_time;

	/* When the permission is temporary access permission mode 1,
	 * Allowed end time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	double temporary_access_end_time;

	/* Role type:
	 * Options:
	 *	0: Whitelist
	 *	1: Blacklist
	 *	2: Commonlist
	 */
	int role_type;

	/** file data point */
	char *file_data;

	/** face fratures */
	char *face_features;

	/** face key points */
	char *face_key_points;

	/* norm: Calculated by face_features
	 * The sum of the squares of the values of face_features, the the root number
	 */
	double face_norm[2];

	int start_id;
	int record_count;
	int record_total;
} face_number_info;

typedef struct face_pack_db {
	int id;
	/* Employee mumber(E.g SW200) */
	char employee_number[32];

	/* Name */
	char name[32];

	/* Gender(E.g "Man/Women/..." */
	char gender[8];

	/* Nationa */
	char nationa[32];

	/* ID card number */
	char id_card_number[32];

	/* Telephone number */
	char mobile[20];

	/* Department name(E.g "Software Lab001") */
	char department_name[128];

	/* Access right
	 * Options:
	 *	0: Free access
	 *	1: Temporary access mode 1
	 *	2: Temporary access mode 2
	 */
	int access_right;

	/* Role type:
	 * Options:
	 *	0: Whitelist
	 *	1: Blacklist
	 *	2: Commonlist
	 */
	int role_type;

	/* When the permission is temporary access permission mode 1,
	 * Allowed start time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	char temporary_access_start_time[64];

	/* When the permission is temporary access permission mode 1,
	 * Allowed end time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	char temporary_access_end_time[64];
} face_number_pack_info;

typedef struct manger_info {
	int id;
	char username[32];
	char username2[32];
	char password[32];
} manger_face_info;

/**
 * remove_file: delete file
 * path: file name
 *
 * Returns 0: success.
 */
int remove_file(char *path);

/**
 * person_reg_param_init: initialize person register parameter
 * param: person_reg_param_t
 * len: len of jpg data
 *
 * Returns None.
 */
void person_reg_param_init(person_reg_param_t *param);

/**
 * append_pic_data_to_default_file: write jpg to file
 * pic: jpg data
 * len: len of jpg data
 *
 * Returns 0: success, -1: failure.
 */
int append_pic_data_to_default_file(char *pic, int len);

int append_pic_data_to_default_file_cloud(char *pic, int len);

/**
 * add_member: add member to sqlite
 * p_info: the point of the struct face_number_info
 *
 * Returns 0: success, -1: failure.
 */
int add_member(face_number_info *p_info);

/**
 * search_member: search member from database
 * p_info: the point to face_number_info
 * p_pack_info: the point to face_number_pack_info
 *
 * Returns 0: success, -1: failure.
 */
int search_member(face_number_info *p_info, face_number_pack_info *p_pack_info);

/**
 * update_member: search member from database
 * p_info: the point to face_number_info
 *
 * Returns 0: success, -1: failure.
 */
int update_member(face_number_info *p_info);

/**
 * total_number: total member from database
 * total_count: the point to total count
 *
 * Returns 0: success, -1: failure.
 */
int total_number(int *total_count);

/**
 * delete_one_member_by_json: delete one member by json object
 * @obj: json object of member information
 *
 * Returns 0 means success, other means failure.
 */
int delete_one_member_by_json(cJSON *obj);

/**
 * delete_members_by_json: delete members by json object
 * @json: json object of members information
 * @ret_arr: json array object of result
 *
 * Returns 0 means success, other means failure.
 */
int delete_members_by_json(cJSON *json, cJSON *ret_arr);

/**
 * delete_all_memmbers: delete all member
 *
 * Returns 0 means success, other means failure.
 */
int delete_all_memmbers();

/**
 * delete_member: delete member from database
 * del_json_data: the point to delete json data
 * arr: the point to arr
 *
 * Returns 0: success, -1: failure.
 */
int delete_member(char *del_json_data, cJSON *arr);

/**
 * update_admin_pwd: update administator password from database
 * old_pwd: the point to old password
 * new_pwd: the point to new password
 *
 * Returns 0: success, -1: failure.
 */
int update_admin_pwd(char *old_pwd, char *new_pwd);

/**
 * add_admin: add administator to database
 * use_name: the point to add adminstator name
 * use_name: the point to add adminstator password
 *
 * Returns 0: success, -1: failure.
 */
int add_admin(char *use_name, char *password);

/**
 * search_manger: search manger to database
 * p_info: the point to struct manger_face_info
 *
 * Returns 0: success, -1: failure.
 */
int search_manger(manger_face_info *p_info);

/**
 * update_number: update manger to database
 * user_name: the point to updated number
 * user_password: the point to new password
 *
 * Returns 0: success, -1: failure.
 */
int update_number(char *user_name, char *user_password);

/**
 * delete_number: delete manger to database
 * user_name: the point to delete number
 *
 * Returns 0: success, -1: failure.
 */
int delete_number(char *user_name);

/**
 * add_one_member_by_cjson: batch import members
 * @obj: json object for one employee information
 *
 * Returns none.
 */
int add_one_member_by_cjson(cJSON *obj, char *employee_number);

/**
 * batch_import_members: batch import members
 * data: the point to menbers data
 * employee_number: the point to failed employee number
 *
 * Returns 0: success, -1: failure.
 */
int batch_import_members(char *data, cJSON *obj);
#endif