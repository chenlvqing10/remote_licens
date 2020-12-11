/*
 * app_config.h
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

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

int config_init(void);
void config_deinit(void);
int config_get_int(const char *section, const char *key, int notfound);
long config_get_long(const char *section, const char *key, long notfound);
double config_get_double(const char *section, const char *key, double notfound);
const char *config_get_string(const char *section, const char *key,
						const char *notfound);
int config_get_boolean(const char *section, const char *key, int notfound);

#endif /* __APP_CONFIG_H__ */

