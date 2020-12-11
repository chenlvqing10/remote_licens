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

#ifndef _SYS_INFO_H_
#define _SYS_INFO_H_

int get_version_info(char *version);
const char *get_err(int restfault_status);

#define STATU_OK					0
#define STATU_FAIL					-1
#define ERR_INIT					-2
#define ERR_STR_VAL					-3
#define ERR_NOT_LOGIN					-4
#define ERR_USER_IS_EXIST				-5
#define PWD_ERROR					-6

// #define PERSON_REG_REFUSE				-7
#define PERSON_INFO_DATA_ERROR				-8
#define PERSON_INFO_NO_DETECT_FACE			-9
#define PERSON_INFO_FACE_FEATURE_FAIL			-10
#define PERSON_INFO_MULTIPLE_FACE			-11
#define PERSON_INFO_FACE_SIZE_ERROR			-12
#define PERSON_INFO_IMAGE_DECODE_FAIL			-13
#define PERSON_INFO_PIC_COPY_ERROR			-14
#define PERSON_INFO_PIC_NO_HAVE				-15
#define PERSON_INFO_INSERT_DB_ERROR			-16
#define PERSON_INFO_FACE_QUALITY_TOO_LOW		-17
#define PERSON_INFO_FACE_SIMILARITY_TOO_HIGH		-18
#define PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT		-19
#define PERSON_INFO_IMAGE_FORMAT_ERROR			-20
#define PERSON_INFO_INTERNAL_ERROR			-21
#define PERSON_REGISTRATION_ERROR			-22
#define PERSON_MIS_RECOGNIZED				-23
// #define PERSON_REG_COMPARE_RESULT_OK			-24

#define IS_LASTEST_VERSION				-25
#define IS_NOT_VALIDATE_FIRMWARE			-26
#define IS_NOT_ENOUGH_SPACE				-27

#define USER_NO_VALIDATE				-28
#define ERR_JPEG_DECODE					-29
#define ERR_FACE_DB_UPDATE				-30

#define AUTO_LIGHT_ALREAD_ENABLE			-31
#define WIFI_NOT_ENABLE					-32

#endif
