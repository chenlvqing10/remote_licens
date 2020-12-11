/*
 * efuse.h - efuse head file
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __LOMBO_READ_EFUSE_H___
#define __LOMBO_READ_EFUSE_H___

/* lombo_get_efuse_module_len -- get the length of module in efuse
 * @module_id: module id
 * @module_len_byte: the length of module, unit: byte
 *
 * @return: 0, success; else, failed
 * If you want to read efuse, first use this funtion to get
 * the length of module.
 */
int lombo_get_efuse_module_len(u32 module_id, u32 *module_len_byte);

/* lombo_efuse_read -- read the module in efuse
 * @module_id: module id
 * @buf: buf to save read efuse
 * @module_len_byte: the length of module, unit: byte
 *
 * @return: 0, success; else, failed
 */
int lombo_efuse_read(u32 module_id, void *buf, u32 module_len_byte);

#endif
