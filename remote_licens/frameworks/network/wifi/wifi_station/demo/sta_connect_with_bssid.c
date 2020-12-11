#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define  LOG_TAG		"sta_connect_with_bssid"
#include <log/log.h>

#include "ip_config.h"
#include "wifi_station.h"

int timeout_flag = 0, success_flag = 0;
char *ssid_ptr;
SECURITY security = SECURITY_OPEN;

static void dhcp_handler(dhcp_status_t status)
{
	int netid, ret = 0;

	switch (status) {
	case DHCP_TIMEOUT:
		ret = sta_get_net_id(ssid_ptr, security, &netid);
		if (ret != 0)
			ALOGE("get net_id failed!\n");
		else
			ALOGW("get net_id successed!\n");

		ret = sta_disable_net_conf(netid);
		if (ret != 0)
			ALOGE("disable net_conf failed!\n");
		else
			ALOGW("disable net_id successed!\n");

		ALOGW("dhcp timeout!\n");
		timeout_flag = 1;
		break;

	case DHCP_SUCCESS:
		ALOGI("dhcp success!\n");
		success_flag = 1;
		break;

	case DHCP_ABORT:
		ret = sta_get_net_id(ssid_ptr, security, &netid);
		if (ret != 0)
			ALOGE("get net_id failed!\n");
		else
			ALOGW("get net_id successed!\n");

		ret = sta_disable_net_conf(netid);
		if (ret != 0)
			ALOGE("disable net_conf failed!\n");
		else
			ALOGW("disable net_id successed!\n");

		ALOGI("dhcp abort!\n");
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
	printf("	sta_connect_with_bssid [options]\n");
	printf("option = instruction:\n");
	printf("	-s = ssid of ap.\n");
	printf("	-p = password of ap.\n");
	printf("	-b = bssid of ap.\n");
	printf("	-e = security of ap, it can set 0, 1 or 2.\n");
	printf("		0 means no password.\n");
	printf("		1 means the security is wpa-psk.\n");
	printf("		2 means the security is wpa2-psk.\n");
	printf("	-v = visibility of ap, it can set 0 or 1.\n");
	printf("		0 means the hotspot is hidden.\n");
	printf("		1 means the hotspot is not hidden.\n");
	printf("	-h = show help\n");
	printf("example:\n");
	printf("	sta_connect_with_bssid -s test -p 12345678");
	printf(" -b 04:d4:c4:3c:22:58 -e 2 -v 1\n");
}

int main(int argc, char *argv[])
{
	int ret, tmp, opt, visibility;
	char *passwd_ptr = NULL, *bssid_ptr = NULL;
	char *security_ptr = NULL, *visibility_ptr = NULL;
	CONNECT_INFO *connect_info;

	if (argc == 1) {
		use_help();
		ret = -1;
		goto exit;
	}

	for (;;) {
		opt = getopt(argc, argv, "s:p:b:e:v:h");
		if (opt < 0)
			break;
		switch (opt) {
		case 's':
			ssid_ptr = optarg;
			break;
		case 'p':
			passwd_ptr = optarg;
			break;
		case 'b':
			bssid_ptr = optarg;
			break;
		case 'e':
			security_ptr = optarg;
			break;
		case 'v':
			visibility_ptr = optarg;
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

	if (visibility_ptr == NULL) {
		ALOGE("no visibility parameter is inputed!\n");
		ret = -1;
		goto exit;
	}

	visibility = atoi(visibility_ptr);
	if ((visibility != 0) && (visibility != 1)) {
			ALOGE("visibility is not supported!\n");
			ret = -1;
			goto exit;
	}

	if (visibility == 0) {
		ret = sta_connect_with_bssid(ssid_ptr, passwd_ptr, security, bssid_ptr);
		if (ret == -1) {
			ALOGE("connect invisible ap fail!\n");
			goto exit;
		} else {
			ALOGI("connect invisible ap successed!\n");
		}
	} else if (visibility == 1) {
		ret = sta_connect_with_bssid(ssid_ptr, passwd_ptr, security, bssid_ptr);
		if (ret == -1) {
			ALOGE("connect visible ap fail!\n");
			goto exit;
		} else {
			ALOGI("connect visible ap successed!\n");
		}
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

	if (sta_get_connect_info(connect_info) == 0) {
		ALOGI("connected hotspot ssid: %s!\n", connect_info->ssid);
		ALOGI("connected hotspot bssid: %s!\n", connect_info->bssid);
	}

exit:
	return ret;
}
