/*
 * udisk.c - Interface for operating U Disk
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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <string.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "hotplug.h"

#define LOG_TAG "UDISK"
#include "log/log.h"

#define FILE_MOUNT_CHECK		"/proc/mounts"
#define FILE_DISC_PARTS_CHECK		"/proc/partitions"
#define UDISK_DRIVER_PREFIX_NAME	"sd"
#define SDCARD_DRIVER_PREFIX_NAME	"mmcblk1"

#define RECV_BUFFER_SIZE	1024
#define UEVENT_BUFFER_SIZE	4096

struct hotplug_manager {
	int hotplug_sock;
	int monitor_stop;
	pthread_t monitor_thr;
	int udisk_insert;
	int sdcard_insert;
	hotplug_change_cb change_cb;
};

struct hotplug_manager *hotplug_mgr;

/**
 * udisk_get_partitions - Get the partition status
 * @p_str_name: OUT, partition name
 * @name_max_length: Maximum length of the name
 *
 * Returns 0 if success, otherwise not 0.
 */
static int udisk_get_partitions(char *p_str_name, int name_max_length)
{
	int ret = -1;
	int len = 0;
	int fd = 0;
	char buf[1024];
	char *delim = "\n";
	char *line = NULL;
	char *strtok_tmp_ptr = NULL;
	char *seek_sd = NULL;      /* USED FOR DEVICE MOUNTED ON SD* */

	if (NULL == p_str_name)
		return -1;

	fd = open(FILE_DISC_PARTS_CHECK, O_RDONLY);
	if (fd != -1) {
		memset(buf, 0, sizeof(buf));
		len = read(fd, buf, sizeof(buf));
		close(fd);

		if (len > 0) {
			line = strtok_r(buf, delim, &strtok_tmp_ptr);
			while (line) {
				seek_sd = strstr(line, UDISK_DRIVER_PREFIX_NAME);
				if ((seek_sd != NULL) && (3 == strlen(seek_sd))) {
					snprintf(p_str_name, name_max_length, "%s",
						seek_sd);
					ALOGD("find sd device [%s]\n", p_str_name);
					ret = 0;
					break;
				}
				line = strtok_r(NULL, delim, &strtok_tmp_ptr);
			}
		}

	} else {
		ret = -1;
		ALOGE("[%s:%d] Open %s fail\n", __func__, __LINE__,
			FILE_DISC_PARTS_CHECK);
	}

	return ret;
}

/**
 * udisk_check_mount - Check the mount status of the partition
 * @partition_name: partition name
 *
 * Returns 0 if success, otherwise not 0.
 */
static int check_mount(char *partition_name)
{
	int fd;
	int len;
	char *seek;
	int ret = -1;
	char buf[1024];

	if (NULL == partition_name) {
		ALOGE("The device is NULL\n");
		return -1;
	}

	fd = open(FILE_MOUNT_CHECK, O_RDONLY);
	if (fd != -1) {
		memset(buf, 0, sizeof(buf));
		while ((len = read(fd, buf, sizeof(buf)-1)) > 0) {
			buf[len] = '\0';
			seek = strstr(buf, partition_name);
			if (seek != NULL) {
				ret = 0;
				break;
			}
			memset(buf, 0, sizeof(buf));
		}
		close(fd);
	} else
		ALOGE("Open file %s error\n", FILE_MOUNT_CHECK);

	return ret;
}


/**
 * udisk_is_insert - Check if the U Disk is insert
 * None
 *
 * Returns 1 if insert, otherwise not mount.
 */
int udisk_is_insert(void)
{
	if (!hotplug_mgr)
		return -1;

	return hotplug_mgr->udisk_insert;
}

/**
 * udisk_is_ok - Check if the U Disk is mounted
 * None
 *
 * Returns 0 if mount, otherwise not mount.
 */
int udisk_is_ok(void)
{
	int ret = -1;
	char p_str_name[64];

	ret = udisk_get_partitions(p_str_name, sizeof(p_str_name));
	if (0 == ret) {
		ret = check_mount(p_str_name);
		if (0 == ret)
			ret = 0;
	}

	return ret;
}

/**
 * udisk_get_partitions - Get the partition status
 * @p_str_name: OUT, partition name
 * @name_max_length: Maximum length of the name
 *
 * Returns 0 if success, otherwise not 0.
 */
static int sdcard_get_partitions(char *p_str_name, int name_max_length)
{
	int ret = -1;
	int len = 0;
	int fd = 0;
	char buf[1024];
	char *delim = "\n";
	char *line = NULL;
	char *strtok_tmp_ptr = NULL;
	char *seek_sd = NULL;      /* USED FOR DEVICE MOUNTED ON SD* */

	if (NULL == p_str_name)
		return -1;

	fd = open(FILE_DISC_PARTS_CHECK, O_RDONLY);
	if (fd != -1) {
		memset(buf, 0, sizeof(buf));
		len = read(fd, buf, sizeof(buf));
		close(fd);

		if (len > 0) {
			line = strtok_r(buf, delim, &strtok_tmp_ptr);
			while (line) {
				seek_sd = strstr(line, SDCARD_DRIVER_PREFIX_NAME);
				if ((seek_sd != NULL) && (7 == strlen(seek_sd))) {
					snprintf(p_str_name, name_max_length, "%s",
						seek_sd);
					ALOGD("find mmc device [%s]\n", p_str_name);
					ret = 0;
					break;
				}
				line = strtok_r(NULL, delim, &strtok_tmp_ptr);
			}
		}

	} else {
		ret = -1;
		ALOGE("[%s:%d] Open %s fail\n", __func__, __LINE__,
			FILE_DISC_PARTS_CHECK);
	}

	return ret;
}

/**
 * udisk_is_ok - Check if the U Disk is mounted
 * None
 *
 * Returns 0 if mount, otherwise not mount.
 */
int sdcard_is_ok(void)
{
	int ret = -1;
	char p_str_name[64];

	ret = sdcard_get_partitions(p_str_name, sizeof(p_str_name));
	if (0 == ret)
		ret = check_mount(p_str_name);

	return ret;
}

int hotplug_register_change_cb(hotplug_change_cb cb)
{
	if (!hotplug_mgr)
		return -1;

	hotplug_mgr->change_cb = cb;

	return 0;
}

static char *type2device(hotplug_device_type_e type)
{
	switch (type) {
	case HOTPLUG_TYPE_SDCARD:
		return "sdcard";
	case HOTPLUG_TYPE_UDISK:
		return "udisk";
	default:
		return "unknown";
	}
}

/* check whether the device's actual state is equal to plug state */
static void check_device_state(hotplug_device_type_e type,
					hotplug_device_state_e state)
{
	int ret, i;

	for (i = 0; i < 25; i++) {
		if (type == HOTPLUG_TYPE_UDISK)
			ret = udisk_is_ok();
		else if (type == HOTPLUG_TYPE_SDCARD)
			ret = sdcard_is_ok();
		else
			break;

		if (!ret) { /* device mounted */
			if (state == HOTPLUG_STATE_CONNECT) {
				if (type == HOTPLUG_TYPE_UDISK &&
					hotplug_mgr->udisk_insert != 1) {
					ALOGD("udisk insert");
					hotplug_mgr->udisk_insert = 1;
					if (hotplug_mgr->change_cb)
						hotplug_mgr->change_cb(type, state);
				} else if (type == HOTPLUG_TYPE_SDCARD &&
					hotplug_mgr->sdcard_insert != 1) {
					ALOGD("sdcard insert");
					hotplug_mgr->sdcard_insert = 1;
					if (hotplug_mgr->change_cb)
						hotplug_mgr->change_cb(type, state);
				}
				return;
			}
		} else { /* device unmounted */
			if (state == HOTPLUG_STATE_DISCONNECT) {
				if (type == HOTPLUG_TYPE_UDISK &&
					hotplug_mgr->udisk_insert != 0) {
					ALOGD("udisk remove");
					hotplug_mgr->udisk_insert = 0;
					if (hotplug_mgr->change_cb)
						hotplug_mgr->change_cb(type, state);
				} else if (type == HOTPLUG_TYPE_SDCARD &&
					hotplug_mgr->sdcard_insert != 0) {
					ALOGD("sdcard remove");
					hotplug_mgr->sdcard_insert = 0;
					if (hotplug_mgr->change_cb)
						hotplug_mgr->change_cb(type, state);
				}
				return;
			}
		}

		/* wait for mounting or unmounting device */
		usleep(100 * 1000);
	}

	ALOGD("%s 's state is not %s", type2device(type),
			state == HOTPLUG_STATE_CONNECT ? "connected" : "disconnected");
}

static void *hotplug_monitor(void *args)
{
	char *result, *buf, *seek;
	struct hotplug_manager *mgr = (struct hotplug_manager *)args;
	int device_type;
	prctl(PR_SET_NAME, "hotplug_monitor");

	pthread_detach(pthread_self());

	buf = malloc(UEVENT_BUFFER_SIZE);
	if (!buf) {
		ALOGE("%s: malloc uevent buffer error", __func__);
		return NULL;
	}

	while (!mgr->monitor_stop) {
		memset(buf, 0, UEVENT_BUFFER_SIZE);
		recv(mgr->hotplug_sock, buf, UEVENT_BUFFER_SIZE, 0);
		seek = strstr(buf, "mmcblk");
		if (seek) /* mmc event */
			device_type = HOTPLUG_TYPE_SDCARD;
		else {
			if (strstr(buf, "sd") && strstr(buf, "usb")) /* udisk event */
				device_type = HOTPLUG_TYPE_UDISK;
			else
				continue; /* other else */
		}

		/* get string before @ */
		result = strtok(buf, "@");
		if (result != NULL) {
			if (strcmp(result, "add") == 0)
				check_device_state(device_type, HOTPLUG_STATE_CONNECT);
			else if (strcmp(result, "remove") == 0)
				check_device_state(device_type, HOTPLUG_STATE_DISCONNECT);
		}
	}

	if (buf)
		free(buf);

	return NULL;
}

int hotplug_mgr_init(void)
{
	int buffersize = RECV_BUFFER_SIZE;
	struct sockaddr_nl snl;
	int ret;

	hotplug_mgr = calloc(sizeof(struct hotplug_manager), 1);
	if (!hotplug_mgr) {
		ALOGE("malloc udisk mgr failed");
		return -1;
	}
	if (udisk_is_ok() == 0)
		hotplug_mgr->udisk_insert = 1;
	else
		hotplug_mgr->udisk_insert = -1;
	if (sdcard_is_ok() == 0)
		hotplug_mgr->sdcard_insert = 1;
	else
		hotplug_mgr->sdcard_insert = -1;

	hotplug_mgr->hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM,
							NETLINK_KOBJECT_UEVENT);
	if (hotplug_mgr->hotplug_sock == -1) {
		ALOGE("%s:socket error", __func__);
		goto free_res;
	}

	ret = setsockopt(hotplug_mgr->hotplug_sock, SOL_SOCKET, SO_RCVBUF, &buffersize,
						sizeof(buffersize));
	if (ret) {
		ALOGE("%s:socket error", __func__);
		goto free_res;
	}

	memset(&snl, 0, sizeof(struct sockaddr_nl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;
	ret = bind(hotplug_mgr->hotplug_sock, (struct sockaddr *)&snl,
						sizeof(struct sockaddr_nl));
	if (ret) {
		ALOGE("%s:bind error", __func__);
		goto free_res;
	}

	hotplug_mgr->monitor_stop = 0;
	ret = pthread_create(&hotplug_mgr->monitor_thr, NULL,
						hotplug_monitor, hotplug_mgr);
	if (ret) {
		ALOGE("pthread_create error = %d\n", ret);
		goto free_res;
	}

	return 0;

free_res:
	if (hotplug_mgr) {
		if (hotplug_mgr->hotplug_sock != -1)
			close(hotplug_mgr->hotplug_sock);
		free(hotplug_mgr);
	}

	return -1;
}

void hotplug_mgr_deinit(void)
{
	if (hotplug_mgr) {
		/* fix me, monitor thread may be in block state */
		hotplug_mgr->monitor_stop = 1;
		if (hotplug_mgr->hotplug_sock != -1)
			close(hotplug_mgr->hotplug_sock);
		free(hotplug_mgr);
	}
}
