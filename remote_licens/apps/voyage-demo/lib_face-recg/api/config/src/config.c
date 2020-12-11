/*
 * app_config.c
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

#include <iniparser.h>

#define LOG_TAG "config"
#include <log/log.h>

#define APP_CONFIG_PATH		"/etc/face-recg.conf"

static dictionary *conf_dic;

int config_get_int(const char *section, const char *key, int notfound)
{
	char buf[128];

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);

	return iniparser_getint(conf_dic, buf, notfound);
}

long config_get_long(const char *section, const char *key, long notfound)
{
	char buf[128];

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);

	return iniparser_getlongint(conf_dic, buf, notfound);
}

double config_get_double(const char *section, const char *key, double notfound)
{
	char buf[128];

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);

	return iniparser_getdouble(conf_dic, buf, notfound);
}

const char *config_get_string(const char *section, const char *key,
						const char *notfound)
{
	char buf[128];

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);

	return iniparser_getstring(conf_dic, buf, notfound);
}

int config_get_boolean(const char *section, const char *key, int notfound)
{
	char buf[128];

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);

	return iniparser_getboolean(conf_dic, buf, notfound);
}

int config_init(void)
{
	conf_dic = iniparser_load(APP_CONFIG_PATH);
	if (!conf_dic) {
		ALOGE("failed to load app config file:%s", APP_CONFIG_PATH);
		return -1;
	}

	return 0;
}

void config_deinit(void)
{
	if (conf_dic)
		iniparser_freedict(conf_dic);
}

