/*
 * system_info.c - define some function of system infomation
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
#include <string.h>
#include <iniparser.h>
#include <sys/utsname.h>

#include "system_info.h"

#define LOG_TAG "SYSTEM_INFO"
#include "log/log.h"

#define BUILD_INFO_CONFIG_PATH "/build.prop"

static dictionary *build_conf_dic;

static const char *system_info_config_get_string(const char *section, const char *key,
						const char *notfound)
{
	char buf[32];

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

	/* Get compile time */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.date", "");
	if (p_str_temp)
		snprintf(p_build_info->build_date, sizeof(p_build_info->build_date),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'build_date' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get compiled author */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.user", "");
	if (p_str_temp) {
		snprintf(p_build_info->build_user, sizeof(p_build_info->build_user),
			"%s", p_str_temp);
	} else {
		ALOGE("Read 'build_user' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get compiled host */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.build.host", "");
	if (p_str_temp)
		snprintf(p_build_info->build_host, sizeof(p_build_info->build_host),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'build_host' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	/* Get the hardware model */
	p_str_temp = system_info_config_get_string("buildinfo",
		"ro.product.name", "");
	if (p_str_temp)
		snprintf(p_build_info->product_name, sizeof(p_build_info->product_name),
			"%s", p_str_temp);
	else {
		ALOGE("Read 'product_name' fail\n");
		goto READ_BUILD_INFO_ERR;
	}

	ret = 0;

READ_BUILD_INFO_ERR:

	system_info_config_deinit();

	return ret;
}

