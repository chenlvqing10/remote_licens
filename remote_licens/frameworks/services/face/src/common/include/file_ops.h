/*
 * face_svc_main.h
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

#ifndef __FILE_OPS_H__
#define __FILE_OPS_H__

#ifndef FACE_EXPORT
#define FACE_EXPORT
#if __GNUC__ >= 4 // it means the compiler is GCC version 4.0 or later
	#ifdef FACE_EXPORT
		#define PUBLIC __attribute__((visibility ("default")))
		#define LOCAL __attribute__((visibility("hidden")))
	#else
		#define PUBLIC
		#define LOCAL
	#endif
#else
	#error "##### requires gcc version >= 4.0 #####"
#endif
#endif

/*
* file_copy - copy source file to destination file
* @src_file_path path of source file
* @dest_file_path path of destination file
* return 0 if success, otherwise return -1
*/
LOCAL int file_copy(char *src_file_path, char *dest_file_path);

/*
* file_move - move src_file_path to dest_file_path
* @src_file_path path of source file
* @dest_file_path path of destination file
* return 0 if success, otherwise return -1
*/
LOCAL int file_move(char *src_file_path, char *dest_file_path);

/*
* file_wirte - wirte buffer to destination file
* @file_path path of destination file
* @buf data buffer
* @size buffer size
* return 0 if success, otherwise return -1
*/
LOCAL int file_write(char *file_path, char *buf, int size);

/*
* file_length - get length of file
* @file_path path of file
* return -1 if failed, otherwise return length
*/
LOCAL int file_length(char *file_path);

/*
* mk_mul_dirs - make directories
* @file_path path of directory
* return -1 if failed, otherwise return 0
*/
LOCAL int mk_mul_dirs(char *muldir);

/*
* is_file_exist -check whether file exists or not
* @file_path path of file
* return 0 if file exists, otherwise return -1
*/
LOCAL int is_file_exist(char *file_path);

/**
 * is_dir_exist - Check if the folder exists.
 * @dir_path: Folder path.
 *
 * Returns 0 if the folder exists, otherwise not 0.
 */
LOCAL int is_dir_exist(char *dir_path);

/*
* file_remove -remove file
* @file_path path of file
* return 0 if remove file successfully, otherwise return -1
*/
LOCAL int file_remove(char *file_path);

/**
 * get_filename_ext - Gettng file extension.
 * @file_name file name
 *
 * Returns file extension.
 */
LOCAL int get_filename_ext(const char *filename, char *ext, int ext_max_len);

/**
 * file_system_sync - Put the memory resident information about the file system into
 * the physical medium.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
LOCAL int file_system_sync(void);

#endif

