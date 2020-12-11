/*
 * watchdog.c - Get the private data from private partition
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
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>

#define LOG_TAG	"watchdog"
#include <log/log.h>

#include "watchdog_def.h"
#include "watchdog.h"

#define WATCHDOG_DEV "/dev/watchdog"
#define WATCHDOG_TIMEOUT 10

typedef struct _watchdog_s {
	int fd;
	struct watchdog_info wi;
	pthread_t watchdog_thrd;
} watchdog_t;

watchdog_t *watchdog;

static int _watchdog_init(watchdog_t *svc)
{
	int disable_card = WDIOS_DISABLECARD;
	int time_out = WATCHDOG_TIMEOUT;
	int enable_card = WDIOS_ENABLECARD;
	svc->fd = open(WATCHDOG_DEV, O_RDWR | O_NONBLOCK);
	if (svc->fd < 0) {
		ALOGE("cannot open the watchdog device.\n");
		return -1;
	}

	ioctl(svc->fd, WDIOC_SETOPTIONS, &disable_card);

	ioctl(svc->fd, WDIOC_SETTIMEOUT, &time_out);

	ioctl(svc->fd, WDIOC_GETSUPPORT, &svc->wi);
	ALOGD("watchdog info:%d, %s\n", svc->wi.options, svc->wi.identity);
	/* default enable watchdog */
	ioctl(svc->fd, WDIOC_SETOPTIONS, &enable_card);

	return 0;
}

int watchdog_disable(void)
{
	watchdog_t *svc = watchdog;
	int tmp = 0;

	if (!svc) {
		ALOGE("watchdog not init\n");
		return -1;
	}

	ALOGD("Disable watchdog!\n");
	tmp = WDIOS_DISABLECARD;
	return ioctl(svc->fd, WDIOC_SETOPTIONS, &tmp);
}

int watchdog_enable(void)
{
	watchdog_t *svc = watchdog;
	int tmp = 0;

	if (!svc) {
		ALOGE("watchdog not init\n");
		return -1;
	}

	ALOGD("Enable watchdog!\n");
	tmp = WDIOS_ENABLECARD;
	return ioctl(svc->fd, WDIOC_SETOPTIONS, &tmp);
}

int watchdog_feed(void)
{
	watchdog_t *svc = watchdog;
	int feeddogvalue = 0;

	if (!svc) {
		ALOGE("watchdog not init\n");
		return -1;
	}

	if (svc->fd <= 0) {
		ALOGE("watchdog not init\n");
		return -1;
	}

	return ioctl(svc->fd, WDIOC_KEEPALIVE, &feeddogvalue);
}

int watchdog_init(void)
{
	watchdog_t *svc = NULL;

	svc = (watchdog_t *)malloc(sizeof(watchdog_t));
	if (!svc) {
		ALOGE("malloc watchdog_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}
	memset(svc, 0, sizeof(watchdog_t));

	watchdog = svc;

	_watchdog_init(svc);

	return 0;
}

int watchdog_deinit(void)
{
	watchdog_t *svc = watchdog;

	if (!svc) {
		ALOGE("watchdog not init\n");
		return -1;
	}
	watchdog_disable();
	close(svc->fd);
	free(svc);

	return 0;
}

