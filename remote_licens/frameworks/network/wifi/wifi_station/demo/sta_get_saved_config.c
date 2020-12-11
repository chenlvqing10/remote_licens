#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define  LOG_TAG		"sta_get_saved_config"
#include <log/log.h>

#include "wifi_station.h"

SAVED_WIFI_CONFIG saved_wifi_config = {0, NULL};

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

	case STA_CONNECT_ABORT:
		ALOGW("connection is aborted!\n");
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

	ret = sta_get_saved_config(&saved_wifi_config);
	if (ret == -1) {
		ALOGE("get saved wifi config failed!\n");
		goto exit;
	} else {
		ALOGI("get saved wifi config ok!\n");
	}

	ALOGI("wifi_config num: %d\n", saved_wifi_config.num);
	for (i = 0; i < saved_wifi_config.num; i++) {
		ALOGI("network id: %d\n", saved_wifi_config.config[i].net_id);
		ALOGI("ssid: %s\n", saved_wifi_config.config[i].ssid);
		ALOGI("key_mgmt: %d\n", saved_wifi_config.config[i].security);
	}

exit:
	return ret;
}
