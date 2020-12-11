/*
 * db.h - Interface for database
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

#ifndef __DB_H__
#define __DB_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* #define __DB_F09 */
/*
 * db_init - initialize database.
 *
 * Return 0 if success, otherwise return -1.
 */
int db_init(void);
/*
 * get_err_no - get last db execution error no.
 *
 * Return error no.
 */
int get_err_no();

/*
 * get_err_msg - get last db execution error message.
 *
 * Return error message.
 */
const char *get_err_msg();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DB_H__ */
