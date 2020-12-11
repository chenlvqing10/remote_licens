#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define  LOG_TAG		"sta_get_net_id"
#include <log/log.h>

#include "wifi_station.h"

static void sta_event_callback(STA_EVENT event)
{
	switch (event) {
	case STA_DISCONNECTED:
		ALOGI("wifi disconnected!\n");
		break;

	case STA_SCAN_RESULTS:
		ALOGI("go to scan_result!!\n");
		break;

	case STA_CONNECTED:
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
	printf("	sta_git_net_id [options]\n");
	printf("option = instruction:\n");
	printf("	-s = ssid of ap.\n");
	printf("	-e = security of ap, it can set 0, 1 or 2.\n");
	printf("		0 means no password.\n");
	printf("		1 means the security is wpa-psk.\n");
	printf("		2 means the security is wpa2-psk.\n");
	printf("	-h = show help\n");
	printf("example:\n");
	printf("	sta_get_net_id -s test -e 2\n");
}

int main(int argc, char *argv[])
{
	int ret, tmp, opt;
	SECURITY security;
	int netid;
	char *ssid_ptr = NULL, *security_ptr = NULL;

	if (argc == 1) {
		use_help();
		ret = -1;
		goto exit;
	}

	for (;;) {
		opt = getopt(argc, argv, "s:e:h");
		if (opt < 0)
			break;
		switch (opt) {
		case 's':
			ssid_ptr = optarg;
			break;
		case 'e':
			security_ptr = optarg;
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

	tmp = atoi(security_ptr);
	switch (tmp) {
	case 0:
		security = SECURITY_OPEN;
		break;

	case 1:
		security = SECURITY_WPA_PSK;
		break;

	case 2:
		security = SECURITY_WPA2_PSK;
		break;
	default:
		ALOGE("security is not supported!\n");
		ret = -1;
		goto exit;
	}

	ret = sta_get_net_id(ssid_ptr, security, &netid);
	if (ret == -1) {
		ALOGE("obtain network configuration failed!\n");
		goto exit;
	} else {
		ALOGI("obtain network configuration successed!\n");
		printf("ssid\t security\t network ID\n");
		printf("%s\t %d\t\t %d\n", ssid_ptr, security, netid);
	}

exit:
	return ret;
}

