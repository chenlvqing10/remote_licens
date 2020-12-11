/*
 * app_lang.c - Multi-language interface
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

#include "elang.h"
#include "lang.h"
#include "app_lang.h"
#include "system_multi_lang.h"
#include "db_device_status_info.h"

#define LOG_TAG "APP_LANG"
#include <log/log.h>

/**
* lang_init - Multi-language initialization
* None
*
* Returns 0 if success, otherwise not 0.
*/
int lang_init()
{
	int ret = 0;
	multi_lang_type_e lang_type;

	if (-1 == elang_load("/usr/share/ipc-face-recg/res/elang/elang_ipc-face-recg.el")) {
		ALOGE("[%s:%d]Can't open language file\n", __func__, __LINE__);
		return -1;
	}

	ret = system_multi_lang_get(&lang_type);
	if (ret != 0) {
		lang_type = MULTI_LANG_CHINESE;
		ALOGE("[%s:%d]Read info failed from database", __func__, __LINE__);
	}

	if (MULTI_LANG_CHINESE == lang_type)
		elang_set_language(ELANG_SIMPLIFIED);
	else if (MULTI_LANG_ENGLISH == lang_type)
		elang_set_language(ELANG_ENGLISH);

	return 0;
}

/**
* lang_deinit - Release multi-language resources
* None
*
* Returns 0 if success, otherwise not 0.
*/
int lang_deinit()
{
	elang_unload();

	return 0;
}

