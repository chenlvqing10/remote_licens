/* dhcp_test.c */

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

static void dhcp_handler(dhcp_status_t status)
{
	const char *str = "unknown";

	if (DHCP_SUCCESS == status)
		str = "success";
	else if (DHCP_TIMEOUT == status)
		str = "timeout";
	else if (DHCP_ABORT == status)
		str = "abort";
	else if (DHCP_ERROR == status)
		str = "error";

	ALOGI("dhcp status: %s", str);
}

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

	/* start dhcp and abort it, we do this just for test */
	net_dev_start_dhcp(device, dhcp_handler);

	usleep(10000);

	net_dev_stop_dhcp(device);

	sleep(1);

	/* restart dhcp and waitting for it to be finished or timeout */
	net_dev_start_dhcp(device, dhcp_handler);

	sleep(25);

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

exit:
	return ret;
}
