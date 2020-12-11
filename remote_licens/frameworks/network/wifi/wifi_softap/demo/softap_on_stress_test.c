#include <stdio.h>
#include <stdlib.h>

#define  LOG_TAG		"softap_on_stress_test"
#include <log/log.h>

#include "wifi_softap.h"

int main(int argv, char *argc[])
{
	AP_CONFIG *config;
	int ret, tmp, count, test_count = 0;
	int enable_successed_count = 0, disable_successed_count = 0;

	config = (AP_CONFIG *)malloc(sizeof(AP_CONFIG));
	config->ssid = argc[1];
	config->passwd = argc[2];

	tmp = atoi(argc[3]);
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

	test_count = atoi(argc[4]);
	ALOGI("total test time: %d!\n", test_count);
	for (count = 0; count < test_count; count++) {
		ALOGI("test times: %d!\n", count);

		ret = softap_enable();
		if (ret == -1) {
			ALOGE("enable softap failed!\n");
			goto exit;
		} else {
			enable_successed_count++;
			ALOGI("enable softap successed!\n");
		}

		ret = softap_disable();
		if (ret == -1) {
			ALOGE("disable softap failed!\n");
			goto exit;
		} else {
			disable_successed_count++;
			ALOGI("disable softap successed!\n");
		}
	}
exit:
	free(config);
	ALOGI("enable_successed_count: %d, disable_successed_count: %d",
			enable_successed_count, disable_successed_count);
	if ((enable_successed_count == test_count) &&
		(disable_successed_count == test_count))
		ALOGI("test is passed!\n");
	else
		ALOGI("test is not passed!\n");

	return ret;
}
