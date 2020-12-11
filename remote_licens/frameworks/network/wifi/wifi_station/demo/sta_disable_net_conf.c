#include <stdio.h>
#include <unistd.h>

#define  LOG_TAG		"sta_disable_net_conf"
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
	printf("	sta_disable_net_conf [options]\n");
	printf("option = instruction:\n");
	printf("	-n = the  network ID to be disabled.\n");
	printf("	-h = show help\n");
	printf("example:\n");
	printf("	sta_disable_net_conf -n 2\n");
}

int main(int argc, char *argv[])
{
	int ret, opt;
	int net_id;
	char *number_ptr = NULL;

	if (argc == 1) {
		use_help();
		ret = -1;
		goto exit;
	}

	for (;;) {
		opt = getopt(argc, argv, "n:h");
		if (opt < 0)
			break;
		switch (opt) {
		case 'n':
			number_ptr = optarg;
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

	net_id = atoi(number_ptr);
	ret = sta_disable_net_conf(net_id);
	if (ret == -1) {
		ALOGE("disable network ID failed!\n");
		goto exit;
	} else {
		ALOGI("disable network ID successed!\n");
		printf("the disabled network ID : %d\n", net_id);
	}

exit:
	return ret;
}


