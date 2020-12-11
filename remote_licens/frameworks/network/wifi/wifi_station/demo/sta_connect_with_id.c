#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define  LOG_TAG		"sta_connect_with_id"
#include <log/log.h>

#include "ip_config.h"
#include "wifi_station.h"

int timeout_flag = 0, success_flag = 0;

static void dhcp_handler(dhcp_status_t status)
{
	switch (status) {
	case DHCP_TIMEOUT:
		ALOGW("dhcp timeout!\n");
		timeout_flag = 1;
		break;

	case DHCP_SUCCESS:
		ALOGI("dhcp success!\n");
		success_flag = 1;
		break;

	case DHCP_ABORT:
		ALOGI("dhcp abort!\n");
		success_flag = 1;
		break;
	}
}

static void sta_event_callback(STA_EVENT event)
{
	switch (event) {
	case STA_DISCONNECTED:
		ALOGI("wifi disconnected!\n");
		net_dev_del_ip_config("wlan0");
		break;

	case STA_SCAN_RESULTS:
		ALOGI("go to scan_result!!\n");
		break;

	case STA_CONNECTED:
		net_dev_start_dhcp("wlan0", dhcp_handler);
		ALOGI("start dhcp!\n");
		break;

	case STA_NETWORK_NOT_EXIST:
		ALOGW("network is not exist!\n");
		break;

	case STA_PASSWD_ERROR:
		ALOGW("password is wrong!\n");
		break;

	case STA_CONNECT_REJECT:
		ALOGW("connection is rejected!\n");
		break;

	case STA_UNKNOWN_EVENT:
		ALOGW("unknown wifi event!\n");
		break;
	}
}

static void use_help(void)
{
	printf("usage:\n");
	printf("  sta_connect_with_id [options]\n");
	printf("option:\n");
	printf("  -i = network id.\n");
	printf("  -h = show help\n");
	printf("example:\n");
	printf("  sta_connect_with_id -i 1\n");
}

int main(int argc, char *argv[])
{
	int ret, net_id, opt;
	CONNECT_INFO *connect_info;
	char *id_ptr;

	if (argc == 1) {
		use_help();
		ret = -1;
		goto exit;
	}

	for (;;) {
		opt = getopt(argc, argv, "i:h");
		if (opt < 0)
			break;
		switch (opt) {
		case 'i':
			id_ptr = optarg;
			break;
		case 'h':
			use_help();
			ret = 0;
			goto exit;
		default:
			use_help();
			ret = -1;
			goto exit;
		}
	}

	ret = sta_enable(sta_event_callback);
	if (ret == -1) {
		ALOGE("enable sta failed!\n");
		goto exit;
	} else {
		ALOGI("enable sta successed!\n");
	}

	net_id = atoi(id_ptr);

	ret = sta_connect_with_id(net_id);
	if (ret == -1) {
		ALOGE("connect ap fail!\n");
		goto exit;
	} else {
		ALOGI("connect ap successed!\n");
	}

	while (1) {
		if (timeout_flag == 1) {
			ALOGW("obtain IP timeout!\n");
			ret = -1;
			goto exit;
		}

		if (success_flag == 1) {
			ALOGI("network is available!\n");
			break;
		}

		usleep(100000);
	}

	connect_info = (CONNECT_INFO *)malloc(sizeof(CONNECT_INFO));
	if (connect_info == NULL) {
		ALOGW("malloc failed!\n");
		ret = -1;
		goto exit;
	}
	memset(connect_info, 0, sizeof(CONNECT_INFO));

	if (sta_get_connect_info(connect_info) == 0)
		ALOGI("connected hotspot ssid: %s!\n", connect_info->ssid);

exit:
	return ret;
}
