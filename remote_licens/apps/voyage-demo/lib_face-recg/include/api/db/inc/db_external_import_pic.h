/*
 * external_import_pic.h - Interface for import external images
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

#ifndef __EXTERNAL_IMPORT_PIC_H
#define __EXTERNAL_IMPORT_PIC_H

#include "ee_queue.h"

typedef struct tag_external_import_pic {
	int id;

	/* Image name */
	char pic_name[128];

	/* Bidirectional queue */
	ee_queue_t queue;
} external_import_pic_t;

/**
 * external_import_pic_print - Display information in a queue for debugging
 * purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void external_import_pic_print(ee_queue_t *head);

/**
 * db_external_import_pic_update - Update pic by name.
 * @user_name: A pointer to username string.
 * @p_info: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_external_import_pic_update(char *pic_name,
				external_import_pic_t *p_info);

/**
 * db_external_import_pic_insert - Combine statements and execute it.
 * @stmt: A pointer to the [sqlite3_stmt] object.
 * @index: not use.
 * @arg: A pointer to manager_info_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_external_import_pic_insert(external_import_pic_t *p_info);

/**
 * db_external_import_pic_delete - Delete an pic.
 * @user_name: A pointer to username string.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_external_import_pic_delete(char *pic_name);

/**
 * db_external_import_pic_query_begin - Query pic.
 * @user_name: A pointer to username string.
 * @head: A pointer to the head of the queue.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_external_import_pic_query_begin(ee_queue_t *head);

/**
 * db_external_import_pic_query_end - Free the queue.
 * @h:  A pointer to the head of the queue.
 *
 * Returns 0 if success, otherwise not 0.
 */
int db_external_import_pic_query_end(ee_queue_t *head);

#endif /* __EXTERNAL_IMPORT_PIC_H */

