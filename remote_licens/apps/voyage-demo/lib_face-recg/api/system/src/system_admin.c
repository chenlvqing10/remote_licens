/*
 * system_admin.c - Interface to modify the administrator password
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
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "system_admin.h"
#include "db_manager_info.h"

#undef LOG_TAG
#define LOG_TAG "SYSTEM_ADMIN"
#include "log/log.h"

/**
 * system_admin_get_info - Get administrator information.
 * @p_name OUT, Store the administrator name
 * @name_max_len Administrator name maximum length
 * @p_pwd OUT, Store the administrator password
 * @pwd_max_len Administrator password maximum length
 *
 * Returns 0 if get information successfully, otherwise not 0.
 */
int system_admin_get_info(char *p_name, unsigned int name_max_len,
			char *p_pwd, unsigned int pwd_max_len)
{
	int ret = -1;
	ee_queue_t queue_head;
	manager_info_t *q = NULL;

	assert((p_name != NULL) && (p_pwd != NULL));

	if ((p_name == NULL) || (p_pwd == NULL)) {
		ALOGE("%s: para pointer is NULL\n", __func__);
		return -1;
	}

	/*read one manager parameter for temporary*/
	ret = db_manager_info_query_begin("", &queue_head);
	if (!ret) {
		ALOGE("%s: read manager para failed(%d)\n", __func__, ret);
		return -1;
	}

	ee_queue_foreach(q, &queue_head, manager_info_t, queue) {
		snprintf(p_name, name_max_len, "%s", q->user_name);
		snprintf(p_pwd, pwd_max_len, "%s", q->user_password);
		ret = 0;
		break;
	}

	db_manager_info_query_end(&queue_head);

	return ret;
}

/**
 * system_admin_uptate_info Update administrator information
 * @p_name IN, Store the administrator name
 * @p_pwd IN, Store the administrator password
 *
 * Returns 0 if the update information is successful, otherwise not 0.
 */
int system_admin_uptate_info(char *p_name, char *p_pwd)
{
	manager_info_t user;

	assert((p_name != NULL) && (p_pwd != NULL));

	/* Update the admin user information */
	memset(&user, 0, sizeof(manager_info_t));
	snprintf(user.user_name, sizeof(user.user_name), "%s", p_name);
	snprintf(user.user_password, sizeof(user.user_password), "%s", p_pwd);
	db_manager_info_update(p_name, &user);

	return 0;
}

