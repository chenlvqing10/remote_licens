/*
 * system_admin.h - Interface to modify the administrator password
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

#ifndef _SYSTEM_ADMIN_H
#define _SYSTEM_ADMIN_H

/**
 * system_admin_get_info - Get administrator information.
 * @p_name OUT, Store the administrator name
 * @name_max_len Administrator name maximum length
 * @p_pwd OUT, Store the administrator password
 * @pwd_max_len Administrator password maximum length
 *
 * Returns 0 if get information successfully, otherwise not 0.
 */
int system_admin_get_info(char *p_name, unsigned int name_max_len,
			char *p_pwd, unsigned int pwd_max_len);

/**
 * system_admin_uptate_info Update administrator information
 * @p_name IN, Store the administrator name
 * @p_pwd IN, Store the administrator password
 *
 * Returns 0 if the update information is successful, otherwise not 0.
 */
int system_admin_uptate_info(char *p_name, char *p_pwd);

#endif /* _SYSTEM_ADMIN_H */
