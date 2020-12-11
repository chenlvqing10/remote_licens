#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define  LOG_TAG		"sta_scan"
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
		ALOGI("go to scan_result!!\n");
		sta_scan_results(&scan_info);
		break;

	case STA_CONNECTED:
		ALOGI("start dhcp!\n");
		break;

	case STA_NETWORK_NOT_EXIST:
		ALOGI("network is not exist!\n");
		break;

	case STA_PASSWD_ERROR:
		ALOGI("password is wrong!\n");
		break;

	case STA_CONNECT_REJECT:
		ALOGI("connection is rejected!\n");
		break;

	case STA_UNKNOWN_EVENT:
		ALOGW("unknown wifi event!\n");
		break;
	}
}

int main(int argc, char *argv[])
{
	int ret, i;

	ret = sta_enable(sta_event_callback);
	if (ret == -1) {
		ALOGE("enable sta failed!\n");
		goto exit;
	} else {
		ALOGI("enable sta successed!\n");
	}

	ret = sta_scan();
	if (ret == -1) {
		ALOGE("scan failed!\n");
		goto exit;
	} else {
		ALOGI("scan ok!\n");
	}

	ALOGI("hotspot num: %d\n", scan_info.num);
	for (i = 0; i < scan_info.num; i++)
		ALOGI("ssid: %s\n", scan_info.info[i].ssid);

exit:
	return ret;
}
