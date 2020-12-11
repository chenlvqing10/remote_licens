/* net_dev_open_test.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>

#define  LOG_TAG		"net_utils"
#include <log/log.h>

#include "net_utils.h"

#define DEF_DEVICE		"eth0"

int main(int argc, char **argv)
{
	char device[IFNAMSIZ] = {0};
	int ret = 0;

	if (argc < 2)
		strncpy(device, DEF_DEVICE, IFNAMSIZ);
	else
		strncpy(device, argv[1], IFNAMSIZ);
	device[IFNAMSIZ - 1] = 0;

	ret = net_dev_open(device);
	if (ret) {
		ALOGE("failed to open device %s, ret: %d", device, ret);
		goto exit;
	}

	ret = net_dev_is_open(device);
	if (ret < 0) {
		ALOGE("failed to check if device %s is opened, ret: %d",
		      device, ret);
		goto exit;
	}

	ALOGI("device %s has been %s", device, ret ? "opened" : "closed");

	ret = net_dev_close(device);
	if (ret) {
		ALOGE("failed to close device %s, ret: %d", device, ret);
		goto exit;
	}

	ret = net_dev_is_open(device);
	if (ret < 0) {
		ALOGE("failed to check if device %s is opened, ret: %d",
		      device, ret);
		goto exit;
	}

	ALOGI("device %s has been %s", device, ret ? "opened" : "closed");

	ret = 0;

exit:
	return ret;
}
