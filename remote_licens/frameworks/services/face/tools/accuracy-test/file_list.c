
/*
 * file_list.c
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
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "ee_list.h"
#include "file_list.h"

void trave_dir(char *path, struct list_head *list, int *cnt)
{
	DIR *d = NULL;
	struct dirent *dp = NULL;
	struct stat st;
	char p[MAX_PATH_LEN] = {0};
	file_t *file;

	if (stat(path, &st) < 0 || !S_ISDIR(st.st_mode)) {
		printf("invalid path: %s\n", path);
		return;
	}

	d = opendir(path);
	if (!d) {
		printf("opendir[%s] error: %m\n", path);
		return;
	}

	while ((dp = readdir(d)) != NULL) {
		if ((!strncmp(dp->d_name, ".", 1)) || (!strncmp(dp->d_name, "..", 2)))
			continue;
		snprintf(p, sizeof(p) - 1, "%s/%s", path, dp->d_name);
		stat(p, &st);
		if (!S_ISDIR(st.st_mode)) {
			char *tmp = strrchr(dp->d_name, '.');
			if ((tmp != NULL) && (strcmp(tmp, ".jpg") == 0)) {
				file = malloc(sizeof(file_t));
				strcpy(file->p, p);
				list_add(&file->node, list);
				*cnt += 1;
			}
		} else {
			trave_dir(p, list, cnt);
		}
	}
	closedir(d);

	return;
}

