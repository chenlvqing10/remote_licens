/* link_handler_test.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>

#define  LOG_TAG		"link_handler"
#include <log/log.h>

#include "net_utils.h"
#include "link_handler.h"

#define DEF_DEVICE		"eth0"

static void eth_link_handler_1(eth_dev_link_status_t status)
{
	ALOGI("link handler(001) get status: %s",
	      status ? "link up" : "link down");
}

static void eth_link_handler_2(eth_dev_link_status_t status)
{
	ALOGI("link handler(002) get status: %s",
	      status ? "link up" : "link down");
}

int main(int argc, char **argv)
{
	char device[IFNAMSIZ] = {0};
	int ret = 0;

	if (argc < 2)
		strncpy(device, DEF_DEVICE, IFNAMSIZ);
	else
		strncpy(device, argv[1], IFNAMSIZ);
	device[IFNAMSIZ - 1] = 0;

	ret = eth_dev_start_link_detect(device);
	if (ret) {
		ALOGE("failed to start link detect for device %s, ret: %d",
		      device, ret);
		return ret;
	}

	ret = net_dev_open(device);
	if (ret) {
		ALOGE("failed to open device %s, ret: %d", device, ret);
		goto exit;
	}

	ret = eth_dev_add_link_handler(device, eth_link_handler_1);
	if (ret) {
		ALOGE("failed to add link handler for device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	ret = eth_dev_add_link_handler(device, eth_link_handler_2);
	if (ret) {
		ALOGE("failed to add link handler for device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	/* wait 20s for link status to be changed, we do this for test */
	printf("[001 & 002]: waitting for %s's link status to be changed\n",
	       device);
	sleep(20);

	ret = eth_dev_del_link_handler(device, eth_link_handler_1);
	if (ret) {
		ALOGE("failed to delete link handler for device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	/* wait 20s for link status to be changed, we do this for test */
	printf("[002]: waitting for %s's link status to be changed\n", device);
	sleep(20);

	ret = eth_dev_add_link_handler(device, eth_link_handler_1);
	if (ret) {
		ALOGE("failed to add link handler for device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	ret = eth_dev_del_link_handler(device, eth_link_handler_2);
	if (ret) {
		ALOGE("failed to delete link handler for device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	/* wait 20s for link status to be changed, we do this for test */
	printf("[001]: waitting for %s's link status to be changed\n", device);
	sleep(20);

	ret = net_dev_close(device);
	if (ret) {
		ALOGE("failed to close device %s, ret: %d", device, ret);
		goto exit;
	}

	ret = 0;

exit:
	eth_dev_stop_link_detect(device);

	return ret;
}
