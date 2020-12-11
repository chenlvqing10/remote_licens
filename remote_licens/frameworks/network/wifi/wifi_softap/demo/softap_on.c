#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  LOG_TAG		"softap_on"
#include <log/log.h>

#include "wifi_softap.h"

int main(int argc, char *argv[])
{
	AP_CONFIG *config;
	AP_INFO *info;
	int ret, tmp;

	config = (AP_CONFIG *)malloc(sizeof(AP_CONFIG));
	memset(config, 0, sizeof(AP_CONFIG));
	info = (AP_INFO *)malloc(sizeof(AP_INFO));
	memset(info, 0, sizeof(AP_INFO));

	config->ssid = argv[1];
	config->passwd = argv[2];

	tmp = atoi(argv[3]);
	switch (tmp) {
	case 0:
		config->security = WIFI_OPEN;
		break;

	case 1:
		config->security = WIFI_WPA_PSK;
		break;

	case 2:
		config->security = WIFI_WPA2_PSK;
		break;
	}

	ret = softap_set_config(config);
	if (ret == -1) {
		ALOGE("set softap failed!\n");
		goto exit;
	} else {
		ALOGI("set softap successed!\n");
	}

	ret = softap_enable();
	if (ret == -1) {
		ALOGE("enable softap failed!\n");
		goto exit;
	} else {
		ALOGI("enable softap successed!\n");
	}

	if (softap_get_info(info) == 0)
		ALOGI("softap 's ssid: %s\n", info->ssid);

exit:
	free(config);
	free(info);
	return ret;
}
