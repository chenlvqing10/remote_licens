/*
 * person_register.h - Define interface for personnel registration.
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

#ifndef _PERSON_REGISTER_H
#define _PERSON_REGISTER_H

int app_management_reg_start(const char *name);
void app_management_reg_stop();
int app_management_reg_by_jpg_and_name(const char *path, const char *name);
void app_management_reg_get_config(void *reg_param);
int app_management_get_reg_result();

#endif /* _PERSON_REGISTER_H */

