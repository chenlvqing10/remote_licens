/*
* take_photo.h
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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef TAKE_PHOTO_H
#define TAKE_PHOTO_H

#include <stdlib.h>

/**
 * take_photo_proc - Interface function for photo capture.
 * @file_name: IN, File name.
 *
 * NOTE: Wait for the 'record_init' call before you can use this function.
 *
 * Returns 0 if success, otherwise not 0.
 */
int take_photo_proc(char *file_name);

/**
 * take_photo - Interface function for photo capture.
 * @pbin: OUT, data of photo capture.
 * @len: OUT, length of data.
 *
 * NOTE: Wait for the 'record_init' call before you can use this function.
 *
 * Returns 0 if success, otherwise not 0.
 */
int take_photo(unsigned char **pbin, int *len);

/**
 * camera_free_mem_output - Interface function for free data of photo capture.
 * @out: data buffer of photo capture.
 *
 * NOTE: Wait for the 'record_init' call before you can use this function.
 *
 * Returns None.
 */
void camera_free_mem_output(char *out);


#endif /* TAKE_PHOTO_H */

