/*
 * system_info.c - Define some function of system infomation
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

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <iniparser.h>
#include <sys/utsname.h>

#include "system_info.h"

#define LOG_TAG "SYSTEM_INFO"
#include "log/log.h"

#define BUILD_INFO_CONFIG_PATH "/build.prop"

#define MAX_BUFFER_SIZE 256

static dictionary *build_conf_dic;

static const char *system_info_config_get_string(
			const char *section, const char *key,
			const char *notfound)
{
	char buf[MAX_BUFFER_SIZE];

	if (!build_conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);

	return iniparser_getstring(build_conf_dic, buf, notfound);
}

static int system_info_config_init(void)
{
	build_conf_dic = iniparser_load(BUILD_INFO_CONFIG_PATH);
	if (!build_conf_dic) {
		ALOGE("failed to load app config file:%s", BUILD_INFO_CONFIG_PATH);
		return -1;
	}

	return 0;
}

static void system_info_config_deinit(void)
{
	if (build_conf_dic)
		iniparser_freedict(build_conf_dic);
}

/**
 * system_info_get_kernel_version - Get the kernel version
 * @kernel_version: OUT, Store kernel version string
 * @max_len: IN, Maximum length of string that can be stored
 *
 * Returns 0 if Get the kernel version successfully, otherwise -1.
 */
int system_info_get_kernel_version(char *kernel_version, int max_len)
{
	int ret = -1;
	struct utsname kernel_info;

	ret = uname(&kernel_info);
	if (0 == ret) {
		snprintf(kernel_version, max_len, "%s %s %s %s %s",
			kernel_info.sysname, kernel_info.nodename, kernel_info.release,
			kernel_info.version, kernel_info.machine);
		ret = 0;
	} else {
		ALOGE("Get kernel version fail:%s\n", strerror(errno));
		ret = -1;
	}

	return ret;
}

/**
 * system_info_get_build_info - Get compilation information
 * @p_build_info: OUT, Store kernel version string
 *
 * Returns 0 if Get the compilation information, otherwise -1.
 */
int system_info_get_build_info(build_info_t *p_build_info)
{
	int ret = -1;
	const char *p_str_temp;

	memset(p_build_info, 0, sizeof(build_info_t));

	system_info_config_init();

	/* Get build id */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.id", "");
	if (p_str_temp)
		snprintf(p_build_info->build_id, sizeof(p_build_info->build_id),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'build_id' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get build display id */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.display.id", "");
	if (p_str_temp)
		snprintf(p_build_info->build_display_id,
			sizeof(p_build_info->build_display_id),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'build_display_id' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get build version incremental */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.version.incremental", "");
	if (p_str_temp)
		snprintf(p_build_info->build_ver_inc,
			sizeof(p_build_info->build_ver_inc),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'build_ver_inc' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get compiled time */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.date", "");
	if (p_str_temp)
		snprintf(p_build_info->build_date,
			sizeof(p_build_info->build_date),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'build_date' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get compiled time utc */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.date.utc", "");
	if (p_str_temp)
		snprintf(p_build_info->build_date_utc,
			sizeof(p_build_info->build_date_utc),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'build_date_utc' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get compiled author */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.user", "");
	if (p_str_temp) {
		snprintf(p_build_info->build_user,
			sizeof(p_build_info->build_user),
			"%s", p_str_temp);
	} else {
		ALOGE("Read 'build_user' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get compiled host */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.host", "");
	if (p_str_temp)
		snprintf(p_build_info->build_host,
			sizeof(p_build_info->build_host),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'build_host' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get the product model */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.product.model", "");
	if (p_str_temp)
		snprintf(p_build_info->product_model,
			sizeof(p_build_info->product_model),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'product_model' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get the product brand */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.product.brand", "");
	if (p_str_temp)
		snprintf(p_build_info->product_brand,
			sizeof(p_build_info->product_brand),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'product_brand' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get the product name */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.product.name", "");
	if (p_str_temp)
		snprintf(p_build_info->product_name,
			sizeof(p_build_info->product_name),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'product_name' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get the product device */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.product.device", "");
	if (p_str_temp)
		snprintf(p_build_info->product_device,
			sizeof(p_build_info->product_device),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'product_device' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get the product chip */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.product.chip", "");
	if (p_str_temp)
		snprintf(p_build_info->product_chip,
			sizeof(p_build_info->product_chip),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'product_chip' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get the product manufacturer */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.product.manufacturer", "");
	if (p_str_temp)
		snprintf(p_build_info->product_manufacturer,
			sizeof(p_build_info->product_manufacturer),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'product_manufacturer' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get the system version */
#if 0
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.firmware.version", "");
	if (p_str_temp)
		snprintf(p_build_info->system_version,
			sizeof(p_build_info->system_version),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'system_version' fail\n");
		goto READ_BUILD_INFO_ERR;
	}
#else
	snprintf(p_build_info->system_version,
			sizeof(p_build_info->system_version),
			"%s", p_build_info->build_display_id);
			/* p_build_info->product_model); */
	snprintf(p_build_info->device_name,
			sizeof(p_build_info->device_name),
			"%s", p_build_info->product_name);

#endif
	ret = 0;

READ_BUILD_INFO_ERR:

	system_info_config_deinit();

	return ret;
}

/**
 * system_info_get_model_name - Get compilation information
 * @model: OUT, Store model name string
 *
 * Returns 0 if Get the compilation information, otherwise -1.
 */
int system_info_get_model_name(char *model, int str_max_len)
{
	int ret = -1;
	const char *p_str_temp;

	assert(model != NULL);

	system_info_config_init();

	/* Get the model name */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.product.model", "");
	if (p_str_temp) {
		memset(model, 0, str_max_len);
		strncpy(model, p_str_temp, str_max_len);
	} else {
		ALOGE("Read 'model' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	ret = 0;

READ_BUILD_INFO_ERR:

	system_info_config_deinit();

	return ret;
}

