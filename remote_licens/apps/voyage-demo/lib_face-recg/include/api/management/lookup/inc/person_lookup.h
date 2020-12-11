/*
 * person_lookup.h - Interface function for person lookup
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

#ifndef _PERSON_LOOKUP_H
#define _PERSON_LOOKUP_H

#include "ee_queue.h"

/**
 * person_lookup_del_by_name - Delete person by name
 * @name: A pointer to username string.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_lookup_del_by_name(char *user_name);

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
int person_lookup_proc_begin(char *user_name, ee_queue_t *person_info_queue_head);

/**
 * person_lookup_proc_end - When the end of the use of the person information, you need
 * to release the allocated resources.
 * @person_info_queue_head: IN, Point to the list header that stores person information.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_lookup_proc_end(ee_queue_t *person_info_queue_head);

#endif /* _PERSON_LOOKUP_H */

