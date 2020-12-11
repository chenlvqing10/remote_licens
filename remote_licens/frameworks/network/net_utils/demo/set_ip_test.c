/* set_ip_test.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>

#define  LOG_TAG		"net_utils"
#include <log/log.h>

#include "net_utils.h"
#include "ip_config.h"

#define DEF_DEVICE		"eth0"

int main(int argc, char **argv)
{
	char device[IFNAMSIZ] = {0};
	ip_static_config_t ip_config;
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

	/**
	 * To set:
	 * IP Address: 192.168.1.3
	 * Netmask:    255.255.255.0
	 * Gateway:    192.168.1.1
	 * DNS0:       8.8.8.8
	 * No to set:
	 * DNS1
	 */
	memset(&ip_config, 0, sizeof(ip_config));
	ip_config.ip_addr[0] = 192;
	ip_config.ip_addr[1] = 168;
	ip_config.ip_addr[2] = 1;
	ip_config.ip_addr[3] = 3;
	ip_config.netmask[0] = 255;
	ip_config.netmask[1] = 255;
	ip_config.netmask[2] = 255;
	ip_config.netmask[3] = 0;
	ip_config.gateway[0] = 192;
	ip_config.gateway[1] = 168;
	ip_config.gateway[2] = 1;
	ip_config.gateway[3] = 1;
	ip_config.dns0[0] = 8;
	ip_config.dns0[1] = 8;
	ip_config.dns0[2] = 8;
	ip_config.dns0[3] = 8;

	ret = net_dev_set_static_ip(device, &ip_config);
	if (ret) {
		ALOGE("failed to set ip config for device %s, ret: %d",
		      device, ret);
		goto exit;
	}

	/* get and check ip config */
	memset(&ip_config, 0, sizeof(ip_config));
	ret = net_dev_get_static_ip(device, &ip_config);
	if (ret) {
		ALOGE("failed to get ip config for device %s, ret: %d",
			device, ret);
		goto exit;
	}

	ALOGI("ipconfig for device %s we get is:\n"
	      "IP Address: %u.%u.%u.%u\n"
	      "NetMask:    %u.%u.%u.%u\n"
	      "Gateway:    %u.%u.%u.%u\n"
	      "DNS0:       %u.%u.%u.%u\n"
	      "DNS1:       %u.%u.%u.%u\n",
	      device,
	      ip_config.ip_addr[0], ip_config.ip_addr[1],
	      ip_config.ip_addr[2], ip_config.ip_addr[3],
	      ip_config.netmask[0], ip_config.netmask[1],
	      ip_config.netmask[2], ip_config.netmask[3],
	      ip_config.gateway[0], ip_config.gateway[1],
	      ip_config.gateway[2], ip_config.gateway[3],
	      ip_config.dns0[0],    ip_config.dns0[1],
	      ip_config.dns0[2],    ip_config.dns0[3],
	      ip_config.dns1[0],    ip_config.dns1[1],
	      ip_config.dns1[2],    ip_config.dns1[3]);

	ret = net_dev_close(device);
	if (ret) {
		ALOGE("failed to close device %s, ret: %d", device, ret);
		goto exit;
	}

exit:
	return ret;
}
