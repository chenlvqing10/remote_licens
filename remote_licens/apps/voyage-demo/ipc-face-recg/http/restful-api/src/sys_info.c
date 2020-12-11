/*
 * api/sys_info.c - all functions to get system information such as
 * version, issue data and MAC address and etc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include "sys_info.h"

/**
 * get_version_info: get system version information (for test)
 * version: a pointer of store version information
 *
 * Returns 0 means success, other means failure.
 */
int get_version_info(char *version)
{
	return 0;
}

/**
 * get_restful_status_str: query status string
 * @restfault_status: status of restful response
 *
 * Returns desc of status.
 */
const char *get_err(int restfault_status)
{
	switch (restfault_status) {
	case STATU_OK:
		return "OK";
	case STATU_FAIL:
		return "FAIL";
	case ERR_INIT:
		return "ERR_INIT";
	case ERR_STR_VAL:
		return "ERR_STR_VAL";
	case ERR_NOT_LOGIN:
		return "ERR_NOT_LOGIN";
	case ERR_USER_IS_EXIST:
		return "USER_IS_EXIST";
	case PWD_ERROR:
		return "PWD_ERROR";

	case PERSON_INFO_DATA_ERROR:
		return "PERSON_INFO_DATA_ERROR";
	case PERSON_INFO_NO_DETECT_FACE:
		return "PERSON_INFO_NO_DETECT_FACE";
	case PERSON_INFO_FACE_FEATURE_FAIL:
		return "PERSON_INFO_FACE_FEATURE_FAIL";
	case PERSON_INFO_MULTIPLE_FACE:
		return "STATUS_PERSON_INFO_MULTIPLE_FACE";
	case PERSON_INFO_FACE_SIZE_ERROR:
		return "PERSON_INFO_FACE_SIZE_ERROR";
	case PERSON_INFO_IMAGE_DECODE_FAIL:
		return "PERSON_INFO_IMAGE_DECODE_FAIL";
	case PERSON_INFO_PIC_COPY_ERROR:
		return "PERSON_INFO_PIC_COPY_ERROR";
	case PERSON_INFO_PIC_NO_HAVE:
		return "PERSON_INFO_PIC_NO_HAVE";
	case PERSON_INFO_INSERT_DB_ERROR:
		return "PERSON_INFO_INSERT_DB_ERROR";
	case PERSON_INFO_FACE_QUALITY_TOO_LOW:
		return "PERSON_INFO_FACE_QUALITY_TOO_LOW";
	case PERSON_INFO_FACE_SIMILARITY_TOO_HIGH:
		return "PERSON_INFO_FACE_SIMILARITY_TOO_HIGH";
	case PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT:
		return "PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT";
	case PERSON_INFO_IMAGE_FORMAT_ERROR:
		return "PERSON_INFO_IMAGE_FORMAT_ERROR";
	case PERSON_INFO_INTERNAL_ERROR:
		return "PERSON_INFO_INTERNAL_ERROR";
	case PERSON_REGISTRATION_ERROR:
		return "PERSON_REGISTRATION_ERROR";
	case PERSON_MIS_RECOGNIZED:
		return "PERSON_MIS_RECOGNIZED";

	case IS_LASTEST_VERSION:
		return "IS_LASTEST_VERSION";
	case IS_NOT_VALIDATE_FIRMWARE:
		return "IS_NOT_VALIDATE_FIRMWARE";
	case IS_NOT_ENOUGH_SPACE:
		return "IS_NOT_ENOUGH_SPACE";

	case USER_NO_VALIDATE:
		return "USER_NO_VALIDATE";
	case ERR_JPEG_DECODE:
		return "ERR_JPEG_DECODE";
	case ERR_FACE_DB_UPDATE:
		return "ERR_FACE_DB_UPDATE";
	case AUTO_LIGHT_ALREAD_ENABLE:
		return "AUTO_LIGHT_ALREAD_ENABLE";
	case WIFI_NOT_ENABLE:
		return "WIFI_NOT_ENABLE";

	}

	return NULL;
}


