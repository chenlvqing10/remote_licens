/*
 * system_develop_sel.c - Interface to Set system developer selection
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


#include "system_develop_sel.h"
#include "db_device_status_info.h"
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"
#undef LOG_TAG
#define LOG_TAG "system_develop"
#include "log/log.h"

develop_sel_para_change_cb 	develop_sel_para_change_notify_cb;
/**
* developer selection parameter change notify callback function
* @param p a pointer of callback
* @return NULL
*/
void develop_sel_register_change_cb(develop_sel_para_change_cb cb)
{
	develop_sel_para_change_notify_cb = cb;
}

/**
 * get debug switch
 * @param debug_sw a pointer of parameter
 * @return 0-successful, ohter-failed
 */
int system_develop_sel_get_para(develop_sel_para_t *p_develop_sel)
{
	device_status_param_t info;
	int ret = 0;

	if (p_develop_sel == NULL) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}

	memset(&info, 0x00, sizeof(info));

	ret = db_device_status_param_query(&info);
	if (ret <= 0) {
		ALOGE("%s: read info failed from database", __func__);
		return -1;
	}
	p_develop_sel->debug_sw = info.debug_switch;

	return 0;
}

/**
 * get debug switch
 * @param debug_sw var of debug switch
 * @return 0-successful, ohter-failed
 */
int system_develop_sel_set_para(develop_sel_para_t *p_develop_sel)
{
	device_status_param_t info;
	int ret = 0;

	memset(&info, 0x00, sizeof(info));

	ret = db_device_status_param_query(&info);
	if (ret <= 0) {
		ALOGE("%s: read info failed from database", __func__);
		return -1;
	}
	if (info.debug_switch == p_develop_sel->debug_sw) {
		ALOGD("%s: parameter is not changed\n", __func__);
		return 0;
	}
	info.debug_switch = p_develop_sel->debug_sw;

	ret = db_device_status_param_update(&info);
	if (ret != SQLITE_OK ) {
		ALOGE("%s: write info failed from database", __func__);
		return -1;
	}

	if (develop_sel_para_change_notify_cb)
		develop_sel_para_change_notify_cb(p_develop_sel);
	return 0;
}

