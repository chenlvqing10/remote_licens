/* mac_addr_test.c */

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
	unsigned char mac_addr[6];
	int ret = 0;

	if (argc < 2)
		strncpy(device, DEF_DEVICE, IFNAMSIZ);
	else
		strncpy(device, argv[1], IFNAMSIZ);
	device[IFNAMSIZ - 1] = 0;

	memset(mac_addr, 0, sizeof(mac_addr));
	ret = net_dev_get_mac_addr(device, mac_addr);
	if (ret) {
		ALOGE("failed to get mac addr of device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	ALOGI("mac addr of %s is: %x:%x:%x:%x:%x:%x", device,
	      mac_addr[0], mac_addr[1], mac_addr[2],
	      mac_addr[3], mac_addr[4], mac_addr[5]);

	/* mac address can not be set when net device is running */
	ret = net_dev_close(device);
	if (ret) {
		ALOGE("failed to close device %s, ret: %d", device, ret);
		goto exit;
	}

	mac_addr[0] = 0x9A;
	mac_addr[1] = 0x5E;
	mac_addr[2] = 0x68;
	mac_addr[3] = 0xE5;
	mac_addr[4] = 0xF6;
	mac_addr[5] = 0x6A;
	/* set mac address to: 9A:5E:68:E5:F6:6A */
	ret = net_dev_set_mac_addr(device, mac_addr);
	if (ret) {
		ALOGE("failed to set mac addr for device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	memset(mac_addr, 0, sizeof(mac_addr));
	ret = net_dev_get_mac_addr(device, mac_addr);
	if (ret) {
		ALOGE("failed to get mac addr of device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	ALOGI("new mac addr of %s is: %x:%x:%x:%x:%x:%x",
	      device,
	      mac_addr[0], mac_addr[1], mac_addr[2],
	      mac_addr[3], mac_addr[4], mac_addr[5]);

	ret = 0;

exit:
	return ret;
}
