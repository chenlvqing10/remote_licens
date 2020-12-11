#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define  LOG_TAG		"sta_connect_stress_test"
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
	printf("	sta_connect_stress_test [options]\n");
	printf("option = instruction:\n");
	printf("	-s = ssid of ap.\n");
	printf("	-p = password of ap.\n");
	printf("	-e = security of ap, it can set 0, 1 or 2.\n");
	printf("		0 means no password.\n");
	printf("		1 means the security is wpa-psk.\n");
	printf("		2 means the security is wpa2-psk.\n");
	printf("	-n = number of ap loop connections.\n");
	printf("	-h = show help\n");
	printf("example:\n");
	printf("  sta_connect_stress_test -s test -p 12345678 -e 2 -n 1000\n");
}

int main(int argc, char *argv[])
{
	int ret, tmp, i, test_count = 0;
	int opt;
	SECURITY security = SECURITY_OPEN;
	int connect_successed_count = 0, disconnect_successed_count = 0;
	char *ssid_ptr = NULL, *passwd_ptr = NULL;
	char *security_ptr = NULL, *number_ptr = NULL;

	if (argc == 1) {
		use_help();
		ret = -1;
		goto exit;
	}

	for (;;) {
		opt = getopt(argc, argv, "s:p:e:n:h");
		if (opt < 0)
			break;
		switch (opt) {
		case 's':
			ssid_ptr = optarg;
			break;
		case 'p':
			passwd_ptr = optarg;
			break;
		case 'e':
			security_ptr = optarg;
			break;
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
	}

	test_count = atoi(number_ptr);
	ALOGI("total test time: %d!\n", test_count);
	for (i = 0; i < test_count; i++) {
		ALOGI("test times: %d!\n", i);

		ret = sta_connect(ssid_ptr, passwd_ptr, security);
		if (ret == -1) {
			ALOGE("connect ap fail!\n");
			goto exit;
		} else {
			ALOGI("connect ap successed!\n");
		}

		while (1) {
			if (timeout_flag == 1) {
				ALOGW("obtain IP timeout!\n");
				break;
			}

			if (success_flag == 1) {
				ALOGI("network is available!\n");
				connect_successed_count++;
				break;
			}

			usleep(100000);
		}

		sleep(1);
		ret = sta_disconnect();
		if (ret == -1) {
			ALOGE("disconnect fail!\n");
			goto exit;
		} else {
			ALOGI("disconnect ok!\n");
			disconnect_successed_count++;
		}
	}
exit:
	ALOGI("connect_successed_count: %d, disconnect_successed_count: %d!\n",
			connect_successed_count, disconnect_successed_count);
	if ((connect_successed_count == test_count) &&
			(disconnect_successed_count == test_count))
		ALOGI("test is passed!\n");
	else
		ALOGI("test is not passed!\n");
	return ret;
}
