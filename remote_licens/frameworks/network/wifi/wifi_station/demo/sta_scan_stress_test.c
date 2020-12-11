#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define  LOG_TAG		"sta_scan_stress_test"
#include <log/log.h>

#include "wifi_station.h"

SCAN_INFO scan_info = {0, NULL};

static void sta_event_callback(STA_EVENT event)
{
	switch (event) {
	case STA_DISCONNECTED:
		ALOGI("wifi disconnected!\n");
		break;

	case STA_SCAN_RESULTS:
		sta_scan_results(&scan_info);
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
	printf("	sta_scan_stress_test [options]\n");
	printf("option = instruction:\n");
	printf("	-n = number of AP loop scans.\n");
	printf("	-h = show help\n");
	printf("example:\n");
	printf("	sta_scan_stress_test -n 1000\n");
}

int main(int argc, char *argv[])
{
	int ret, i, count, test_count = 0;
	int opt;
	int scan_successed_count = 0;
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

	test_count = atoi(number_ptr);
	ALOGI("total test time: %d!\n", test_count);
	for (count = 0; count < test_count; count++) {
		ALOGI("test times: %d!\n", count);

		ret = sta_scan();
		if (ret == -1) {
			ALOGE("scan failed!\n");
			goto exit;
		} else {
			scan_successed_count++;
			ALOGI("scan ok!\n");
		}

		ALOGI("hotspot num: %d\n", scan_info.num);
		for (i = 0; i < scan_info.num; i++)
			ALOGI("ssid: %s\n", scan_info.info[i].ssid);

	}
exit:
	ALOGI("scan_successed_count: %d", scan_successed_count);
	if (scan_successed_count == test_count)
		ALOGI("test is passed!\n");
	else
		ALOGI("test is not passed!\n");
	return ret;
}
