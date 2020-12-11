/*
 * lcd_tool.c - main interface of lcd tool testcase.
 *
 * Copyright (C) 2016 - 2019, LomboTech Co.Ltd.
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

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>

#include <assert.h>

#include "lcd_tool_drm.h"
#include "disp_cjson.h"

#if 0
struct stat {
	mode_t    st_mode;      /* file type & mode (permissions) */
	ino_t     st_ino;       /* i-node number (serial number) */
	dev_t     st_dev;       /* device number (file system) */
	dev_t     st_rdev;      /* device number for special files */
	nlink_t   st_nlink;     /* number of links */
	uid_t     st_uid;       /* user ID of owner */
	gid_t     st_gid;       /* group ID of owner */
	off_t     st_size;      /* size in bytes, for regular files */
	time_t    st_atime;     /* time of last access */
	time_t    st_mtime;     /* time of last modification */
	time_t    st_ctime;     /* time of last file status change */
	blksize_t st_blksize;   /* best I/O block size */
	blkcnt_t  st_blocks;    /* number of disk blocks allocated */
};
#endif

static void lcd_tool_csjon_file_change_do(void)
{
	disp_cjson_cfg_t *dcc;

	disp_cjson_update();
	dcc = disp_cjson_get();

	assert(dcc != NULL);

	printf("ce_enable:%d\n", dcc->ce_enable);
	printf("brightness:%d\n", dcc->brightness);
	printf("saturation:%d\n", dcc->saturation);
	printf("contrast:%d\n", dcc->contrast);
	printf("lti_enable:%d\n", dcc->lti_enable);
	printf("lti_level:%d\n", dcc->lti_level);
	printf("peak_enable:%d\n", dcc->peak_enable);
	printf("peak_level:%d\n", dcc->peak_level);
	printf("gamma_enable:%d\n", dcc->gamma_enable);

	if (dcc->ce_enable) {
		lcd_tool_color_enhance_enable();
		/* val range: 0 -100 */
		lcd_tool_set_color_enhance_brightness(dcc->brightness);
		/* val range: 0 -100 */
		lcd_tool_set_color_enhance_saturation(dcc->saturation);
		/* val range: 0 -100 */
		lcd_tool_set_color_enhance_contrast(dcc->contrast);
	} else {
		lcd_tool_color_enhance_disable();
	}

	/* val range: 0 -31, 0 mean close this function */
	if (dcc->lti_enable)
		lcd_tool_set_video_lti(dcc->lti_level);
	else
		lcd_tool_set_video_lti(0);

	/* val range: 0 -15, 0 mean close this function */
	if (dcc->peak_enable)
		lcd_tool_set_video_peak(dcc->peak_level);
	else
		lcd_tool_set_video_peak(0);

	#if 0 /* do this in lcd_tool_gamma_update instead of here */
	if (dcc->gamma_enable)
		lcd_tool_gamma_enable();
	else
		lcd_tool_gamma_disable();
	#endif
}

static int lcd_tool_check_cjson_file(void)
{
	int ret;
	static time_t last_time = 0;
	struct stat fild_stat;

	ret = access(DISP_JSON_SRC_FILE, 0);
	if (ret) {
		printf("[%s %d] file %s is not exit\n",
			__FUNCTION__, __LINE__,
			DISP_JSON_SRC_FILE);
		return ret;
	}

	ret = stat(DISP_JSON_SRC_FILE, &fild_stat);
	if (ret) {
		printf("[%s %d] get %s stat err ret[%d]\n",
			__FUNCTION__, __LINE__,
			DISP_JSON_SRC_FILE, ret);
		return ret;
	}

	if (last_time != fild_stat.st_mtime) {
		printf("\nwarning: json file change\n\n");
		lcd_tool_csjon_file_change_do();
	}

	last_time = fild_stat.st_mtime;

	#if 0
	printf("st_ctime:%lu st_mtime:%lu st_atime:%lu\n",
		fild_stat.st_ctime, fild_stat.st_mtime, fild_stat.st_atime);
	#endif

	return ret;
}

static void lcd_tool_gamma_file_change_do(void)
{
	disp_cjson_cfg_t *dcc;

	disp_cjson_gamma_update();
	dcc = disp_cjson_get();

	assert(dcc != NULL);

	printf("%s:gamma_enable:%d gamma_tbl_size:%d\n",
		__func__ , dcc->gamma_enable, dcc->gamma_tbl_size);

	/* argb is 4bytes */
	lcd_tool_gamma_update(dcc->gamma_tbl, dcc->gamma_tbl_size / 4,
			dcc->gamma_enable);
}

static int lcd_tool_check_gamma_file(void)
{
	int ret;
	static time_t last_time = 0;
	struct stat fild_stat;

	ret = access(DISP_GAMMA_SRC_FILE, 0);
	if (ret) {
		printf("[%s %d] file %s is not exit\n",
			__FUNCTION__, __LINE__, DISP_GAMMA_SRC_FILE);
		return ret;
	}

	ret = stat(DISP_GAMMA_SRC_FILE, &fild_stat);
	if (ret) {
		printf("[%s %d] get %s stat err ret[%d]\n",
			__FUNCTION__, __LINE__, DISP_GAMMA_SRC_FILE, ret);
		return ret;
	}

	if (last_time != fild_stat.st_mtime) {
		printf("\nwarning: gamma file change\n\n");
		lcd_tool_gamma_file_change_do();
	}

	last_time = fild_stat.st_mtime;

	#if 0
	printf("st_ctime:%lu st_mtime:%lu st_atime:%lu\n",
		fild_stat.st_ctime, fild_stat.st_mtime, fild_stat.st_atime);
	#endif

	return ret;
}

void *lcd_tool_check(void *arg)
{
	bool *exit_flag = (bool *)arg;

	printf("lcd_tool_check++\n");
	while (!(*exit_flag)) {
		/* printf("lcd_tool_check\n"); */
		lcd_tool_check_cjson_file();
		lcd_tool_check_gamma_file();
		usleep(1000 * 1000);
		printf("press any key to exit\n");
	}
	printf("lcd_tool_check--\n");

	return NULL;
}

int main(int argc, char **argv)
{
	int ret, type, val;
	pthread_t pid;
	bool exit_flag = false;

	ret = lcd_tool_init();
	if (ret != LCD_TOOL_OK)
		printf("[%s %d] lcd_tool_init err\n", __FUNCTION__, __LINE__);

	#if 1 /* for debug */
	if (argc >= 3) {
		type = atoi(argv[1]);
		val = atoi(argv[2]);
		switch (type) {
		case 0:
			if (val == 0)
				lcd_tool_color_enhance_disable();
			else
				lcd_tool_color_enhance_enable();
			break;
		case 1:
			/* val range: 0 -100 */
			lcd_tool_set_color_enhance_brightness(val);
			break;
		case 2:
			/* val range: 0 -100 */
			lcd_tool_set_color_enhance_saturation(val);
			break;
		case 3:
			/* val range: 0 -100 */
			lcd_tool_set_color_enhance_contrast(val);
			break;
		case 4:
			/* val range: 0 -31, 0 mean close this function */
			lcd_tool_set_video_lti(val);
			break;
		case 5:
			/* val range: 0 -15, 0 mean close this function */
			lcd_tool_set_video_peak(val);
			break;
		case 6:
			if (val == 0)
				lcd_tool_gamma_disable();
			else
				lcd_tool_gamma_enable();
			break;
		default:
			printf("illegal type\n");
			break;
		}
	} else
		printf("too few parameters, only do update\n");
	#endif

	disp_cjson_init();

	pthread_create(&pid, NULL, lcd_tool_check, &exit_flag);
	getchar(); /* wait any input to exit */
	exit_flag = true;
	pthread_join(pid, NULL);

	disp_cjson_exit();

	ret = lcd_tool_exit();
	if (ret != LCD_TOOL_OK)
		printf("[%s %d] lcd_tool_exit err\n", __FUNCTION__, __LINE__);

	return 0;
}

