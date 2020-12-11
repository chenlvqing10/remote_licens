/*
 * system_multi_lang.c - Multi-language setting business layer interface
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

#include <string.h>
#include <sqlite3.h>
#include <assert.h>

#include "system_multi_lang.h"
#include "db_device_status_info.h"

#define LOG_TAG "system_multi_lang"
#include "log/log.h"

/**
 * system_multi_lang_set Set system language type
 * @lang_type System language type
 *
 * @return 0-successful, ohter-failed
 */
int system_multi_lang_set(multi_lang_type_e lang_type)
{
	int ret = 0;
	device_status_param_t info;

	memset(&info, 0x00, sizeof(info));

	ret = db_device_status_param_query(&info);
	if (ret <= 0) {
		ALOGE("[%s:%d]read info failed from database", __func__, __LINE__);
		return -1;
	}

	if (info.language == lang_type) {
		ALOGD("[%s:%d]parameter is not changed\n", __func__, __LINE__);
		return 0;
	}

	info.language = lang_type;

	ret = db_device_status_param_update(&info);
	if (ret != SQLITE_OK) {
		ALOGE("[%s:%d]write info failed from database", __func__, __LINE__);
		return -1;
	}

	return 0;
}

/**
 * system_multi_lang_get Get system language type
 * @type Pointer to multi_lang_type_e
 *
 * @return 0-successful, ohter-failed
 */
int system_multi_lang_get(multi_lang_type_e *type)
{
	int ret = 0;
	int db_ret = 0;
	device_status_param_t info;

	assert(type != NULL);

	memset(&info, 0x00, sizeof(info));
	db_ret = db_device_status_param_query(&info);
	if (db_ret <= 0) {
		ALOGE("[%s:%d]read info failed from database", __func__, __LINE__);
		ret = -1;
	} else {
		ret = 0;
		switch (info.language) {
		case 0:
			*type = MULTI_LANG_ENGLISH;
			break;
		case 1:
			*type = MULTI_LANG_CHINESE;
			break;
		default:
			ret = -1;
			ALOGE("[%s:%d]Not support the language type: %d\n", __func__,
				__LINE__, info.language);
			break;
		}
	}

	return ret;
}

