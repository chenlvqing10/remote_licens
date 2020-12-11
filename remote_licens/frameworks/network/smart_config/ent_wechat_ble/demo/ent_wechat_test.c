#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define  LOG_TAG		"ent_wechat_test"
#include <log/log.h>

#include "net_utils.h"
#include "ip_config.h"
#include "wifi_station.h"
#include "system_private.h"
#include "ent_wechat_ble_config.h"

#define SN_OFFSET				0
#define SECRETNO_OFFSET			0x200
#define DEVICEID_OFFSET			0x400
#define QRCODE_OFFSET			0x600
#define DEV_NAME 				"wlan0"

int timeout_flag = 0, success_flag = 0;
int stop_flag = 0;

static int get_ip_info(char *ip)
{
	int ret = 0;
	ip_static_config_t ip_config;

	memset(&ip_config, 0, sizeof(ip_config));
	ret = net_dev_get_static_ip(DEV_NAME, &ip_config);
	if (ret) {
		ALOGE("failed to get static ip for net device %s, error:%d",
				DEV_NAME, ret);
		goto exit;
	}

	sprintf(ip, "%d.%d.%d.%d", ip_config.ip_addr[0], ip_config.ip_addr[1],
								ip_config.ip_addr[2], ip_config.ip_addr[3]);

exit:
	return ret;
}

static long long stoi(const char *s)
{
	int i;
	long long n = 0;

	for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i) {
		n = 10 * n + (s[i] - '0');
	}

	return n;
}

static int get_device_info(DEVICE_INFO *device_info)
{
	int ret = 0, j = 0;
	unsigned long long tmp_buf;
	char buf[128] = {0};
	FILE *stream;

	/* obtain sn */
	ret = system_get_private_data(SN_OFFSET, device_info->sn, 24);
	if (ret) {
		ALOGE("get SN failed");
		ret = -1;
		goto exit;
	}
	ALOGD("SN:%s, sn_len:%d", device_info->sn, strlen(device_info->sn));

	/* obtain secret_no */
	ret = system_get_private_data(SECRETNO_OFFSET, device_info->secret_no, 48);
	if (ret) {
		ALOGE("get SECRETNO failed");
		ret = -1;
		goto exit;
	}
	ALOGD("SECRETNO:%s, secret_no_len:%d", device_info->secret_no,
			strlen(device_info->secret_no));

	/* obtain device_id */
	ret = system_get_private_data(DEVICEID_OFFSET, buf, 24);
	if (ret) {
		ALOGI("get DEVICEID failed\n");
		ret = -1;
		goto exit;
	}

	/* convert a decimal string to a hexadecimal string */
	tmp_buf = stoi(buf);
	ALOGI("DEVICE_ID:%lld\n", tmp_buf);
	for (j = 0; j < 8; j++) {
		device_info->device_id[j] = (char)((tmp_buf >> (56 - j * 8)) & 0xff);
		ALOGI("device_id[%d]: %x\n", j, device_info->device_id[j]);
	}

	/* obtain mac_address */
	stream = popen("hciconfig | grep \"BD Address\" |awk '{print $3}'", "r");
	fread(device_info->mac_addr, 1, sizeof(device_info->mac_addr)-1, stream);
	pclose(stream);
	ALOGD("mac_addr: %s, mac_addr_len: %d", device_info->mac_addr,
			strlen(device_info->mac_addr));

exit:
	return ret;
}

static void *stop_proto_process(void* args)
{
	/* detach from the main thread */
	pthread_detach(pthread_self());

	sleep(5);
	stop();
	stop_flag = 1;

    pthread_exit(NULL);
}

void activate_stop_proto_process(void)
{
	pthread_t thread_id;

	pthread_create(&thread_id, NULL, stop_proto_process, NULL);
}

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
		break;
	}
}

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
		net_dev_start_dhcp("wlan0", dhcp_handler);
		ALOGI("start dhcp!\n");
		break;

	case STA_NETWORK_NOT_EXIST:
		report_wifi_status(WIFI_NOT_EXIST, NULL);
		ALOGW("network is not exist!\n");
		break;

	case STA_PASSWD_ERROR:
		report_wifi_status(WIFI_PASSWORD_ERR, NULL);
		ALOGW("password is wrong!\n");
		break;

	case STA_CONNECT_REJECT:
		report_wifi_status(WIFI_NOT_EXIST, NULL);
		ALOGW("connection is rejected!\n");
		break;
	}
}

static int connect_wifi_callback(const char *ssid, const char *password,
		const char *protocol)
{
	int ret = 0;
	char ip_addr[20] = {0};
	SECURITY security;

	ret = sta_enable(sta_event_callback);
	if (ret == -1) {
		ALOGE("enable sta failed!\n");
		goto exit;
	} else {
		ALOGI("enable sta successed!\n");
	}

	if (!strcmp(protocol, "None")) {
		security = SECURITY_OPEN;
	} else if ((!strcmp(protocol, "WEP")) || (!strcmp(protocol, "WPA"))
			|| (!strcmp(protocol, "WPA2"))) {
		security = SECURITY_WPA2_PSK;
	} else {
		ALOGE("security is invalid!\n");
		ret = -1;
		goto exit;
	}

	ret = sta_connect(ssid, password, security);
	if (ret == -1) {
		ALOGE("connect ap fail!\n");
		goto exit;
	} else {
		ALOGI("connect ap successed!\n");
	}

	while (1) {
		if (timeout_flag == 1) {
			ALOGW("obtain IP timeout!\n");
			ret = -1;
			goto exit;
		}

		if (success_flag == 1) {
			ALOGI("network is available!\n");

			ret = get_ip_info(ip_addr);
			if (ret) {
				ALOGE("get ip address fail!\n");
				goto exit;
			}
			ALOGI("ip_addr: %s\n", ip_addr);
			report_wifi_status(WIFI_CONNECTED, ip_addr);

			/* stop BLE service */
			activate_stop_proto_process();
			break;
		}

		usleep(100000);
	}

exit:
	return ret;
}

static int disable_wifi_callback()
{
	int ret = 0;

	ret = sta_disable();
	if (ret == -1) {
		ALOGE("disable sta failed!\n");
		goto exit;
	} else {
		ALOGI("disable sta successed!\n");
	}

exit:
	return ret;
}

static int report_err_callback(int err)
{
	int ret = 0;

	return ret;
}

static int ble_connected_callback()
{
	int ret = 0;

	ALOGD("gatt-server is connected");

	return ret;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	DEVICE_INFO device_info;
	APP_HANDLER app_handler;

	memset(&device_info, 0, sizeof(DEVICE_INFO));
	app_handler.connect_wifi_handler = connect_wifi_callback;
	app_handler.disable_wifi_handler = disable_wifi_callback;
	app_handler.report_err_handler = report_err_callback;
	app_handler.ble_connected_handler = ble_connected_callback;

	ret = get_device_info(&device_info);
	if (ret) {
		ALOGE("get device info fail!\n");
		goto exit;
	}

	ret = start(&app_handler, &device_info);
	if (ret == -1) {
		ALOGE("start server failed!\n");
		goto exit;
	} else {
		ALOGI("start server successed!\n");
	}

	while(1) {
		if (stop_flag == 1)
			break;
		usleep(100000);
	}

exit:
	return ret;
}
