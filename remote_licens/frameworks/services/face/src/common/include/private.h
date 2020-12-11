/*
 * system_private.h - Get the private data from private partition
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

#ifndef __FACE_SVC_PRIVATE_H__
#define __FACE_SVC_PRIVATE_H__

/**
 * system_get_private_data - Get the private data from private partition.
 * @offset: Address offset of private data in private partition
 * @data: Pointer to data
 * @len: Required length of data
 *
 * Return 0 if success, otherwise return non-zere.
 */
int face_svc_get_private_data(int offset, char *data, int len);

/**
 * system_set_private_data - Set the private data from private partition.
 * @offset: Address offset of private data in private partition
 * @data: Pointer to data
 * @len: Required length of data
 *
 * Return 0 if success, otherwise return non-zere.
 */
int face_svc_set_private_data(int offset, char *data, int len);

#endif /* __FACE_SVC_PRIVATE_H__ */

