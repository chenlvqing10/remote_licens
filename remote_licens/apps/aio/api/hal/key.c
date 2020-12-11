/*
 * key.c - Interface to get reset key status
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "key.h"

#define LOG_TAG "hal-key"
#include <log/log.h>

#define MAX_LINE_BUF_LEN 256
#define MAX_CONTENT_LEN 4096

#define DEV_KEY_INPUT "/dev/input/event0"
#define INPUT_DEV_LIST_NAME "/proc/bus/input/devices"

/* key event thread structure */
typedef struct _key_event {
	int code; /* key of user pressed */
	key_event_cb cb; /* callback of key press */
	int thread_running; /* thread is running */
	pthread_t key_thread; /* thread id */
} key_event_t;

#if 0
/* key event thread */
static key_event_t s_key_thread;
#endif

/**
 * get_key_dev_name - get device name of key
 * @dev_name: A pointer of device name
 * @buf_len: length of buffer
 *
 * Returns NULL.
 */
static int get_key_dev_name(char *dev_name, int buf_len)
{
	char *pos, *pos1;
	char *find_name = "Sysfs=";
	char *find_str = "Phys=gpio-keys/input0";
	char buf[MAX_CONTENT_LEN];
	char name[MAX_LINE_BUF_LEN];
	char node_name[MAX_LINE_BUF_LEN];
	int index;
	FILE *fp;

	if (NULL == dev_name)
		return -1;

	fp = fopen(INPUT_DEV_LIST_NAME, "rb");
	if (NULL == fp)
		return -1;

	memset(buf, 0, sizeof(buf));
	fread(buf, 1, sizeof(buf), fp);
	fclose(fp);
	pos = strstr(buf, find_str);
	if (NULL == pos)
		return -1;

	pos += strlen(find_str);
	pos1 = strstr(pos, find_name);
	if (NULL == pos1)
		return -1;

	memset(name, 0, sizeof(name));
	pos = strstr(pos1, "\n");
	if (pos)
		memcpy(name, pos1, pos - pos1);
	else
		strcpy(name, pos1);

	memset(node_name, 0, sizeof(node_name));
	pos = strrchr(name, '/');
	if (pos) {
		pos++;
		strcpy(node_name, pos);
	} else
		strcpy(node_name, node_name);

	memset(name, 0, sizeof(name));
	sscanf(node_name, "%5s%d", name, &index);

	snprintf(dev_name, buf_len, "/dev/input/event%d", index);
	ALOGD("%s[%d] find dev_name %s", __func__, __LINE__, dev_name);

	return 0;
}

/**
 * key_detect_thread - thread of user press key detecting
 * @args: key event thread structure
 *
 * Returns NULL.
 */
static void *key_detect_thread(void *args)
{
	int fd, ret;
	fd_set rd;
	struct timeval tv;
	struct input_event ev;
	char dev_name[MAX_LINE_BUF_LEN];
	key_event_t *key_thread = (key_event_t *)args;

	prctl(PR_SET_NAME, "key_detect");
	if (NULL == key_thread)
		return NULL;

	memset(dev_name, 0, sizeof(dev_name));
	ret = get_key_dev_name(dev_name, sizeof(dev_name));
	if (ret)
		return NULL;

#if 1
	fd = open(dev_name, O_RDONLY);
#else
	fd = open(DEV_KEY_INPUT, O_RDONLY);
#endif
	if (fd < 0) {
		ALOGE("%s[%d] open failed, errno %d",
			__func__, __LINE__, errno);
		return NULL;
	}
	ALOGD("%s[%d] open(%s) code %d, ret %d",
			__func__, __LINE__, dev_name, key_thread->code, fd);
	while (key_thread->thread_running) {
		memset(&ev, 0, sizeof(struct input_event));
		FD_ZERO(&rd);
		FD_SET(fd, &rd);
		tv.tv_sec = 0;
		tv.tv_usec = 500 * 1000;
		ret = select(fd+1, &rd, NULL, NULL, &tv);
		if (ret == 0) /* time out */
			continue;
		else if (ret == -1)
			break;
		else {
			ret = read(fd, &ev, sizeof(struct input_event));
			ALOGD("%s[%d] read(%d) %d ev:code %d type %d value %d",
				__func__, __LINE__, fd, ret, ev.code, ev.type, ev.value);
			if (ret) {
				ALOGD("%s[%d] ev:code %d key_thread->code %d cb %p",
		__func__, __LINE__, ev.code, key_thread->code, key_thread->cb);
				if (ev.code == key_thread->code) {
					if (key_thread->cb)
					key_thread->cb(ev.code, ev.type, ev.value);
				}
			}
		}
	}

	close(fd);

	return NULL;
}

/**
 * key_input_init - Init key input
 * @code: key value
 * @cb： key event callback
 *
 * Returns key input devce object..
 */
void *key_input_init(int code, key_event_cb cb)
{
	int ret;
	key_event_t *key_event;

	key_event = (key_event_t *)malloc(sizeof(key_event_t));
	if (NULL == key_event)
		return NULL;

	memset(key_event, 0, sizeof(key_event_t));
	key_event->code = code;
	key_event->cb = cb;
	key_event->thread_running = 1;

	ret = pthread_create(&key_event->key_thread,
			NULL, key_detect_thread, key_event);
	if (ret) {
		ALOGE("%s[%d] pthread_create failed", __func__, __LINE__);
		free(key_event);
		return NULL;
	}

	return key_event;
}

/**
 * key_input_deinit - deinit key input
 * @handle: key input devce object
 *
 * Returns 0 if success, otherwise -1.
 */
int key_input_deinit(void *handle)
{
	key_event_t *key_event;

	if (NULL == handle)
		return -1;

	key_event = (key_event_t *)handle;
	key_event->thread_running = 0;
	pthread_join(key_event->key_thread, NULL);

	free(key_event);

	return 0;
}

