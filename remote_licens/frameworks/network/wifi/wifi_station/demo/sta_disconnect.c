#include <stdio.h>
#include <unistd.h>

#define  LOG_TAG		"sta_disconnect"
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
	int ret;

	ret = sta_enable(sta_event_callback);
	if (ret == -1) {
		ALOGE("enable sta failed!\n");
		goto exit;
	} else {
		ALOGI("enable sta successed!\n");
	}

	ret = sta_disconnect();
	if (ret == -1) {
		ALOGE("disconnect fail!\n");
		goto exit;
	} else {
		ALOGI("disconnect ok!\n");
	}

exit:
	return ret;
}
