/*
 * file_ops.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/vfs.h>
#include <errno.h>

#define LOG_TAG		"file"
#include <log/log.h>

#include "file_ops.h"

#define FILE_COPY_SIZE		(16 * 1024)

LOCAL int file_copy(char *src_file_path, char *dest_file_path)
{
	FILE *dest_fp = NULL, *src_fp = NULL;
	char *buff = NULL;
	int read_cnt, write_cnt, ret = -1, fd;

	buff = malloc(FILE_COPY_SIZE);
	if (!buff) {
		ALOGE("%s: alloc file buffer failed", __func__);
		goto free_res;
	}

	dest_fp = fopen(dest_file_path, "wb");
	if (dest_fp == NULL) {
		ALOGE("%s: open %s failed\n", __func__, dest_file_path);
		goto free_res;
	}

	src_fp = fopen(src_file_path, "r");
	if (src_fp == NULL) {
		ALOGE("%s: %s not found\n", __func__, src_file_path);
		goto free_res;
	}

	while (1) {
		read_cnt = fread(buff, 1, FILE_COPY_SIZE, src_fp);
		if (read_cnt > 0) {
			write_cnt = fwrite(buff, 1, read_cnt, dest_fp);
			if (read_cnt !=  write_cnt) {
				ALOGE("%s: write file failed\n", __func__);
				goto free_res;
			}
		} else
			break;
	}

	ret = fflush(dest_fp);
	if (ret) {
		ALOGE("%s: failed to flush file", __func__);
		goto free_res;
	}

	fd = fileno(dest_fp);
	if (fd == -1) {
		ALOGE("%s: fileno failed", __func__);
		goto free_res;
	}

	ret = fsync(fd);
	if (ret)
		ALOGE("%s: failed to sync file", __func__);

free_res:
	if (buff) {
		free(buff);
		buff = NULL;
	}
	if (dest_fp)
		fclose(dest_fp);
	if (src_fp)
		fclose(src_fp);

	return ret;
}

LOCAL int file_write(char *file_path, char *buf, int size)
{
	FILE *flip = NULL;
	int count = 0, fd, ret;

	flip = fopen(file_path, "wb");
	if (flip == NULL) {
		ALOGE("%s:open %s failed\n", __func__, file_path);
		return -1;
	}

	count = fwrite(buf, 1, size, flip);
	if (count != size) {
		ALOGE("write file(%s) failed, size:%d != write_count:%d\n",
					file_path, size, count);
		fclose(flip);
		return -1;
	}

	ret = fflush(flip);
	if (ret) {
		ALOGE("%s: failed to flush file", __func__);
		goto free_res;
	}

	fd = fileno(flip);
	if (fd == -1) {
		ALOGE("%s: fileno failed", __func__);
		goto free_res;
	}

	ret = fsync(fd);
	if (ret)
		ALOGE("%s: failed to sync file", __func__);

free_res:
	fclose(flip);

	return ret;
}

LOCAL int file_length(char *file_path)
{
	FILE *fp = NULL;
	int file_len;

	fp = fopen(file_path, "rb");
	if (fp == NULL) {
		ALOGE("open %s failed", file_path);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	if (file_len < 0) {
		fclose(fp);
		ALOGE("get file length error");
		return -1;
	}
	fseek(fp, 0, SEEK_SET);
	fclose(fp);

	return file_len;
}

LOCAL int file_move(char *src_file_path, char *dest_file_path)
{
	int ret;

	ret = file_copy(src_file_path, dest_file_path);
	if (ret) {
		ALOGE("failed to copy %s to %s", src_file_path, dest_file_path);
		return ret;
	}

	ret = remove(src_file_path);
	if (ret) {
		ALOGE("failed to remove %s", src_file_path);
		remove(dest_file_path);
		return ret;
	}

	return 0;
}

LOCAL int mk_mul_dirs(char *muldir)
{
	int i, len;
	char str[128];
	int ret = 0;

	strcpy(str, muldir);
	len = strlen(str);
	for (i = 0; i < len; i++) {
		if ((str[i] == '/') && i) {
			str[i] = '\0';
			if (access(str, 0) != 0) {
				ret = mkdir(str, 0777);
				if (ret != 0) {
					ALOGE("Create multiple folder: %s fail\n", str);
					return -1;
				}
			}
			str[i] = '/';
		}
	}
	if (len > 0 && access(str, 0) != 0) {
		ret = mkdir(str, 0777);
		if (ret != 0) {
			ALOGE("Create multiple folder: %s fail\n", str);
			return -1;
		}
	}

	return 0;
}

LOCAL int is_file_exist(char *file_path)
{
	if (!file_path)
		return -1;

	if (access(file_path, F_OK) != -1)
		return 0;

	return -1;
}

/**
 * is_dir_exist - Check if the folder exists.
 * @dir_path: Folder path.
 *
 * Returns 0 if the folder exists, otherwise not 0.
 */
LOCAL int is_dir_exist(char *dir_path)
{
	DIR *dir;
	int ret = -1;

	if (!dir_path)
		return -1;

	dir = opendir(dir_path);
	if (dir != NULL) {
		closedir(dir);

		ret = 0;
	}

	return ret;
}

LOCAL int file_remove(char *file_path)
{
	int ret;

	if (0 != is_file_exist(file_path)) {
		ALOGW("file(%s) does not exist", file_path);
		return -1;
	}

	ret = remove(file_path);
	if (ret) {
		ALOGE("failed to remove file(%s)", file_path);
		return -1;
	}

	return 0;
}

/**
 * get_filename_ext - Gettng file extension.
 * @file_name file name
 *
 * Returns file extension.
 */
LOCAL int get_filename_ext(const char *filename, char *ext, int ext_max_len)
{
	int ret = 0;
	const char *dot = NULL;

	assert((filename != NULL) && (ext != NULL));

	dot = strrchr(filename, '.');
	if (!dot || (dot == filename))
		ret = -1;
	else {
		ret = 0;
		snprintf(ext, ext_max_len, "%s", dot + 1);
	}

	return ret;
}

/**
 * file_system_sync - Put the memory resident information about the file system into
 * the physical medium.
 * None
 *
 * Returns 0 if success, otherwise not 0.
 */
LOCAL int file_system_sync(void)
{
	int ret = -1;

	ret = system("sync");
	if (ret != 0)
		ALOGE("sync error(%s)\n", strerror(errno));

	return ret;
}

