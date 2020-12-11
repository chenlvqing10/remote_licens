/*
 * person_lookup.c - Interface function for person lookup
 * information from Excel.
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
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "db_person_info.h"
#include "db_punch_record.h"
#include "record_info.h"
#include "person_lookup.h"
#include "file_ops.h"
#include "face.h"
#define LOG_TAG "PERSON_LOOKUP"
#include "log/log.h"

#define JPGEXT_LENGTH 16

/**
 * remove_register_pic_by_path_name - remove register pic file by path and name.
 * @path: input path.
 * @len1:  length of string path.
 * @name: input name.
 * @len2: length of string name.
 *
 * Returns 0 if success, otherwise not 0.
 */
static int remove_register_pic_by_path_name(char *path, int len1,
		char *name, int len2)
{
	char *file_path = NULL;
	int file_path_len = 0, ret;

	if (path == NULL || len1 <= 0 || name == NULL || len2 <= 0)
		return -1;

	file_path_len = len1 + len2 + JPGEXT_LENGTH + 1;
	ALOGD("[%s:%d] file_path_len=%d\n", __func__, __LINE__, file_path_len);
	file_path = calloc(1, file_path_len);
	if (NULL == file_path) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return -1;
	}

	memset(file_path, 0, file_path_len);
	snprintf(file_path, file_path_len, "%s/%s-large.jpg", path, name);
	ALOGD("[%s:%d] large file_path=%s\n", __func__, __LINE__, file_path);
	ret = file_remove(file_path);
	if (ret)
		ALOGE("%s: failed to remove file(%s)", __func__, file_path);

	memset(file_path, 0, file_path_len);
	snprintf(file_path, file_path_len, "%s/%s-small.jpg", path, name);
	ALOGD("[%s:%d] small file_path=%s\n", __func__, __LINE__, file_path);
	ret = file_remove(file_path);
	if (ret)
		ALOGE("%s: failed to remove file(%s)", __func__, file_path);

	if (file_path)
		free(file_path);

	return 0;
}

/**
 * db_person_all_pic_delete - Delete an user pic by name.
 * @user_name: A pointer to username string.
 * @len:  length of string user_name.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_delete_all_pic(char *user_name, int len)
{
	assert(user_name != NULL);

	char *pname = NULL;
	ee_queue_t head;
	int ret = 0;
	int query_count = 0;
	full_punch_rec_info_t *q = NULL;
	person_info_t *q_person = NULL;

	if (NULL == user_name || len <= 0)
		return -1;

	memset(&head, 0, sizeof(head));

	pname = calloc(1, len+1);
	if (NULL == pname) {
		ALOGE("[%s:%d] error\n", __func__, __LINE__);
		return -1;
	}

	ALOGD("[%s:%d] user_name=%s,len=%d\n", __func__, __LINE__, user_name, len);
	memcpy(pname, user_name, len);
	ALOGD("[%s:%d] pname=%s,len=%d\n", __func__, __LINE__, pname, len);

	query_count = db_person_info_query_begin_by_name(pname, &head);
	ALOGD("[%s:%d] query_count=%d\n", __func__, __LINE__, query_count);
	if (query_count > 0) {
		ee_queue_foreach(q_person, &head, person_info_t, queue) {
			ret = remove_register_pic_by_path_name(
				q_person->regist_photo_name,
				strlen(q_person->regist_photo_name), q_person->name,
				strlen(q_person->name));
			if (ret)
				ALOGE("%s: remove_register_pic_by_path_name filed",
						__func__);
		}
	}
	db_person_info_query_end(&head);

	query_count = db_punch_record_full_info_query_begin_by_name(pname, &head);
	ALOGD("[%s:%d]query_count=%d\n", __func__, __LINE__, query_count);
	if (query_count > 0) {
		ee_queue_foreach(q, &head, full_punch_rec_info_t, queue) {
			ret = remove(q->punch_live_large_pic_name);
			if (ret)
				ALOGE("%s: failed to remove file(%s)", __func__,
						q->punch_live_large_pic_name);
			ret = remove(q->punch_live_small_pic_name);
			if (ret)
				ALOGE("%s: failed to remove file(%s)", __func__,
						q->punch_live_large_pic_name);
		}
	}
	db_punch_record_full_info_query_end(&head);

	ALOGD("[%s:%d]free\n", __func__, __LINE__);

	/* Synchronize file system data into physical media */
	file_system_sync();

	if (pname)
		free(pname);

	ALOGD("[%s:%d]end\n", __func__, __LINE__);

	return ret;
}

/**
 * person_lookup_del_by_name - Delete person by name
 * @name: The name of the person who needs to be deleted.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_lookup_del_by_name(char *user_name)
{
	int ret = -1;

	assert(user_name != NULL);

	/**
	 * When deleting person, delete the corresponding registration and punch
	 * pictures first.
	 */
	person_delete_all_pic(user_name, strlen(user_name));

	/* Delete punch records and personnel information by user_name */
	ret = db_punch_record_delete(user_name);
	if (ret != 0) {
		ALOGE("db_punch_record_delete error: %s\n", strerror(errno));
		return -1;
	} else
		ALOGD("[%s:%d] Punch record delete ok\n", __func__, __LINE__);

	ret = db_person_info_delete(user_name);
	if (ret != 0) {
		ALOGE("db_person_info_delete error: %s\n", strerror(errno));
		return -1;
	} else
		ALOGD("[%s:%d] Person delete ok\n", __func__, __LINE__);

	/* Updating the face list after successful deletion */
	face_db_update();

	ALOGD("[%s:%d] Face db update ok\n", __func__, __LINE__);

	return 0;
}

/**
 * person_lookup_proc_begin - Query user information by name
 * @user_name: IN, A pointer to username string.
 * @person_info_queue_head: OUT, Point to the list header that stores person information.
 *
 * NOTE1: After the person information is used, it needs to be
 *	called person_lookup_proc_end
 * NOTE2: If user_name is an empty string, Query all user information.
 *
 * Returns the number of people who match the search condition.
 */
int person_lookup_proc_begin(char *user_name, ee_queue_t *person_info_queue_head)
{
	int max_count = 0;
	int person_count = 0;

	assert((user_name != NULL) && (person_info_queue_head != NULL));

	/* Step1: Get the number of person information */
	max_count = db_person_info_count();

	/* Step2: Find person from the database */
	person_count = db_person_info_query_begin(user_name, 0, max_count,
		person_info_queue_head);

	return person_count;
}

/**
 * person_lookup_proc_end - When the end of the use of the person information, you need
 * to release the allocated resources.
 * @person_info_queue_head: IN, Point to the list header that stores person information.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_lookup_proc_end(ee_queue_t *person_info_queue_head)
{
	int ret = -1;

	assert(person_info_queue_head != NULL);

	ret = db_person_info_query_end(person_info_queue_head);

	return ret;
}
