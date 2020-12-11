/*
 * ws_bt_wifi.c - export  interface function of wifi config by ble
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <arpa/inet.h>
#include <errno.h>
#define  LOG_TAG		"bt"
#include <log/log.h>

#include "net_utils.h"
#include "ip_config.h"
#include "wifi_station.h"
#include "ent_wechat_ble_config.h"
#include "ws_bt_wifi.h"
#include "sound_svc.h"
#include "ws_control.h"

#define DEV_NAME				"wlan0"

#define WIFI_SUCCESS_SOUND_PATH "/usr/share/aio/res/sound/wifi_success.wav"
#define WIFI_FAILED_SOUND_PATH "/usr/share/aio/res/sound/wifi_failed.wav"
#define BLE_CONNECT_SOUND_PATH "/usr/share/aio/res/sound/ble_connect.wav"

typedef enum {
	bt_wifi_idle = 0,
	bt_wifi_link,
	bt_dhcp_link,
	bt_ping_link,
	bt_sta_error,
	bt_dhcp_error,
	bt_ping_error,
	bt_wifi_success,
	bt_wifi_failed,
	bt_not_link
} bt_status_e;

typedef struct {
	DEVICE_INFO device_info;
	APP_HANDLER app_handler;
	pthread_t thread_id;
	int is_run;
	int is_ble;
	int err_code;
	bt_status_e status;
	pthread_mutex_t lock_for_sta;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	bt_wifi_event_cb cb;
	int link_status;
} bt_status_t;

bt_status_t *bt_status;

/**
 * @brief  check wifi link status
 * @note   NULL
 * @retval 0 not link  1 linked
 */
int bt_wifi_is_link()
{
	if (bt_status)
		return bt_status->link_status;
	return 0;
}

/**
 * @brief  enable bluetooth prograss
 * @note   not used
 * @retval None
 */
static void bt_enable(void)
{
	#if 0
	system("/etc/init.d/bluetooth restart > /dev/null");
	#endif
}

/**
 * @brief  switch char num to long long
 * @note   used for switch device id from char to longlong
 * @param  *s:  device id
 * @retval device id by long long
 */
static long long bt_stoi(const char *s)
{
	int i;
	long long n = 0;

	for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
		n = 10 * n + (s[i] - '0');

	return n;
}

/**
 * @brief  get wifi ip info
 * @note   NULL
 * @param  *ip_config:
 * @retval 0 success
 */
int bt_get_ip_info(ip_static_config_t *ip_config)
{
	int ret = 0;

	ret = net_dev_get_static_ip(DEV_NAME, ip_config);
	if (ret) {
		ALOGE("failed to get static ip for net device %s, error:%d",
			  DEV_NAME, ret);
		goto exit;
	}

exit:
	return ret;
}

/**
 * @brief  get device info from private partition
 * @param  *device_info:
 * @retval 0 success
 */
static int bt_get_device_info(DEVICE_INFO *device_info)
{
	int ret = 0, j = 0;
	unsigned long long tmp_buf;
	char buf[128] = {0};
	FILE *stream;

	/* obtain sn */
	ret = ws_control_get_sn(device_info->sn, 24);
	if (ret) {
		ALOGE("get SN failed");
		ret = -1;
		goto exit;
	}
	ALOGD("SN:%s, sn_len:%d", device_info->sn, strlen(device_info->sn));

	/* obtain secret_no */
	ret = ws_control_get_secret_no(device_info->secret_no, 48);
	if (ret) {
		ALOGE("get SECRETNO failed");
		ret = -1;
		goto exit;
	}
	ALOGD("SECRETNO:%s, secret_no_len:%d", device_info->secret_no,
		  strlen(device_info->secret_no));

	/* obtain device_id */
	ret = ws_control_get_device_id(buf, 24);
	if (ret) {
		ALOGE("get DEVICEID failed\n");
		ret = -1;
		goto exit;
	}
	ALOGI("device id string:%s\n", buf);

	/* convert a decimal string to a hexadecimal string */
	tmp_buf = bt_stoi(buf);
	ALOGI("DEVICE_ID:%lld\n", tmp_buf);
	for (j = 0; j < 8; j++) {
		device_info->device_id[j] = (char)((tmp_buf >> (56 - j * 8)) & 0xff);
		ALOGI("device_id[%d]: %x\n", j, device_info->device_id[j]);
	}

	/* obtain mac_address */
	stream = popen("hciconfig | grep \"BD Address\" |awk '{print $3}'", "r");
	fread(device_info->mac_addr, 1, sizeof(device_info->mac_addr) - 1, stream);
	pclose(stream);
	ALOGD("mac_addr: %s, mac_addr_len: %d", device_info->mac_addr,
		  strlen(device_info->mac_addr));

exit:
	return ret;
}

/**
 * @brief  ping work
 * @note   NULL
 * @retval 0 success
 */
static int bt_ping_work()
{
#if 0
	char *hostnames[] = {
		"www.qq.com",
		"www.163.com",
		"www.sohu.com",
	};
	for (int i = 0; i < 3; i++) {
		if (net_is_connect("wlan0", hostnames[i]) == 1)
			return 0;
	}
	return -1;
#endif
	return 0;
}

/**
 * @brief  wifi dhcp callback
 * @note   called by dhcp server
 * @param  status:
 * @retval None
 */
static void dhcp_handler(dhcp_status_t status)
{
	switch (status) {
	case DHCP_TIMEOUT:
		ALOGW("dhcp timeout!\n");
		bt_status->status = bt_dhcp_error;
		bt_status->err_code = DHCP_TIMEOUT;
		if (bt_status->is_ble == 0) {
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_signal(&bt_status->cond);
			pthread_mutex_unlock(&bt_status->lock);
		}
		break;

	case DHCP_SUCCESS:
		ALOGI("dhcp success!\n");
		/*set priority*/
		if (net_dev_change_priority(DEV_NAME) < 0)
			ALOGE("change_priority failed");
		bt_status->status = bt_dhcp_link;
		bt_status->err_code = DHCP_SUCCESS;
		if (bt_status->is_ble == 0) {
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_signal(&bt_status->cond);
			pthread_mutex_unlock(&bt_status->lock);
		}
		break;
	case DHCP_ABORT:
		ALOGI("dhcp abort!\n");
		bt_status->status = bt_dhcp_error;
		bt_status->err_code = DHCP_ABORT;
		if (bt_status->is_ble == 0) {
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_signal(&bt_status->cond);
			pthread_mutex_unlock(&bt_status->lock);
		}
		break;
	default:
		break;
	}
}

/**
 * @brief  wifi connect callback
 * @note   called by wifi
 * @param  event:
 * @retval None
 */
static void sta_event_callback(STA_EVENT event)
{
	switch (event) {
	case STA_DISCONNECTED:
		ALOGI("wifi disconnected!\n");
		bt_status->status = bt_sta_error;
		bt_status->err_code = STA_DISCONNECTED;
		if (bt_status->is_ble == 0) {
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_signal(&bt_status->cond);
			pthread_mutex_unlock(&bt_status->lock);
		}
		break;

	case STA_SCAN_RESULTS:
		ALOGI("go to scan_result!!\n");
		break;

	case STA_CONNECTED:
		bt_status->status = bt_wifi_link;
		ALOGI("start dhcp!\n");
		net_dev_start_dhcp("wlan0", dhcp_handler);
		break;

	case STA_NETWORK_NOT_EXIST:
		ALOGW("network is not exist!\n");
		bt_status->status = bt_sta_error;
		bt_status->err_code = STA_NETWORK_NOT_EXIST;
		if (bt_status->is_ble == 0) {
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_signal(&bt_status->cond);
			pthread_mutex_unlock(&bt_status->lock);
		}
		break;

	case STA_PASSWD_ERROR:
		ALOGW("password is wrong!\n");
		bt_status->status = bt_sta_error;
		bt_status->err_code = STA_PASSWD_ERROR;
		if (bt_status->is_ble == 0) {
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_signal(&bt_status->cond);
			pthread_mutex_unlock(&bt_status->lock);
		}

		break;

	case STA_CONNECT_REJECT:
		ALOGW("connection is rejected!\n");
		bt_status->status = bt_sta_error;
		bt_status->err_code = STA_CONNECT_REJECT;
		if (bt_status->is_ble == 0) {
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_signal(&bt_status->cond);
			pthread_mutex_unlock(&bt_status->lock);
		}

		break;

	case STA_UNKNOWN_EVENT:
		ALOGW("recv unknow event!\n");
		bt_status->status = bt_sta_error;
		bt_status->err_code = STA_UNKNOWN_EVENT;
		if (bt_status->is_ble == 0) {
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_signal(&bt_status->cond);
			pthread_mutex_unlock(&bt_status->lock);
		}

		break;

	case STA_CONNECT_ABORT:
		//todo: wifi connection abort
		break;
	}
}

/**
 * @brief  bt connect callback
 * @note   called  by bt conect in another prograss
 * @param  *ssid:
 * @param  *password:
 * @param  *protocol:
 * @retval 0 success
 */
static int bt_connect_wifi_callback(
	const char *ssid,
	const char *password,
	const char *protocol)
{
	int ret = 0;
	SECURITY security;

	if (bt_status->is_ble) {
		ALOGE("ble is already run\n");
		return 0;
	}
	ret = sta_enable(sta_event_callback);
	if (ret == -1) {
		ALOGE("enable sta failed!\n");
		goto exit;
	} else {
		ALOGI("enable sta successed by cc!\n");
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
	bt_status->is_ble = 1;
	ret = sta_connect(ssid, password, security);
	if (ret == -1) {
		ALOGE("connect ap fail!\n");
		bt_status->status = bt_sta_error;
	} else {
		ALOGI("connect ap successed!\n");
	}
	pthread_mutex_lock(&bt_status->lock);
	pthread_cond_signal(&bt_status->cond);
	pthread_mutex_unlock(&bt_status->lock);

exit:
	return ret;
}

/**
 * @brief  disbale wifi callback
 * @note   call by bt server
 * @retval 0 success
 */
static int bt_disable_wifi_callback()
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

/**
 * @brief  bt err callback
 * @note   call by by server
 * @param  err:
 * @retval 0
 */
static int bt_report_err_callback(int err)
{
	int ret = 0;

	return ret;
}

/**
 * @brief  bt connect callback
 * @note   play success sound
 * @retval 0 success
 */
static int bt_connected_callback()
{
	int ret = 0;

	ALOGD("gatt-server is connected");
	sound_play_file_faster(BLE_CONNECT_SOUND_PATH);

	return ret;
}

/**
 * @brief  bt thread task
 * @note   maintain a state machine for wifi connect throuth bt
 * @retval None
 */
static void *bt_thread_task()
{
	int ret = 0;
	char ip_addr[20];
	ip_static_config_t ip_info;
	prctl(PR_SET_NAME, "bt_thread");

	pthread_detach(pthread_self());
	/* turn on bt */
	bt_enable();
	/*get device info*/
	ret = bt_get_device_info(&bt_status->device_info);
	if (ret) {
		ALOGE("get device info fail!\n");
		goto exit;
	}
	ret = start(&bt_status->app_handler, &bt_status->device_info);
	if (ret == -1) {
		ALOGE("start server failed!\n");
		goto exit;
	} else {
		ALOGI("start server successed!\n");
	}
	/* turn on wifi*/
	pthread_mutex_lock(&bt_status->lock_for_sta);
	ret = sta_enable(sta_event_callback);
	pthread_mutex_unlock(&bt_status->lock_for_sta);
	if (ret == 0) {
		ALOGI("enable sta successed!\n");
	} else {
		ALOGE("enable sta failed!\n");
		goto exit;
	}
	ALOGD("bt_thread_task init comp %d\n", bt_status->is_run);
	while (bt_status->is_run) {
		/*wait*/
		ALOGD("status = %d\n", bt_status->status);
		switch (bt_status->status) {
		case bt_dhcp_link:
			/*ping*/
			usleep(1000 * 1000);
			ALOGD("start ping work\n");
			if (bt_ping_work() == 0) {
				/* success */
				ALOGD("ping success\n");
				bt_status->status = bt_wifi_success;
			} else {
				/*failed*/
				ALOGD("ping failed\n");
				bt_status->status = bt_ping_error;
			}
			break;
		case bt_wifi_success:
			if (bt_status->is_ble) {
				memset(&ip_info, 0, sizeof(ip_static_config_t));
				bt_get_ip_info(&ip_info);
				memset(ip_addr, 0, sizeof(ip_addr));
				sprintf(ip_addr, "%d.%d.%d.%d",
						ip_info.ip_addr[0],
						ip_info.ip_addr[1],
						ip_info.ip_addr[2],
						ip_info.ip_addr[3]);
				report_wifi_status(WIFI_CONNECTED, (const char *)ip_addr);
				bt_status->is_ble = 0;
			}
			/*cb */
			if (bt_status->cb)
				bt_status->cb(bt_wifi_connected);
			/*play sound success*/
			/*bt_play_sound(WIFI_SUCCESS_SOUND_PATH);*/
			bt_status->status = bt_wifi_idle;
			bt_status->link_status = 1;
			break;
		case bt_sta_error:
		case bt_dhcp_error:
		case bt_ping_error:
			ALOGD("wifi error\n");
			if (bt_status->is_ble) {
				report_wifi_status(WIFI_NOT_EXIST, NULL);
				bt_status->is_ble = 0;
			}
			net_dev_del_ip_config(DEV_NAME);
			/*cb*/
			if (bt_status->cb)
				bt_status->cb(bt_wifi_disconnect);
			/*play sound failed*/
			/*bt_play_sound(WIFI_FAILED_SOUND_PATH);*/
			bt_status->status = bt_wifi_idle;
			bt_status->link_status = 0;
			break;
		case bt_wifi_idle:
#if 1
			pthread_mutex_lock(&bt_status->lock);
			pthread_cond_wait(&bt_status->cond, &bt_status->lock);
			pthread_mutex_unlock(&bt_status->lock);
#endif
			if (bt_status->status == bt_sta_error ||
				bt_status->status == bt_dhcp_error ||
				bt_status->status == bt_ping_error)
				break;
			if (bt_status->is_ble)
				bt_status->status = bt_not_link;
			break;
		case bt_not_link:
		case bt_wifi_link:
		default:
			usleep(1000 * 1000);
			break;
		}
	}

exit:
	ALOGD("bt_thread_task init comp\n");
	return NULL;
}
/**
 * @brief  set callback function
 * @note   callback exec as wifi connect success or failed
 * @param  func:
 * @retval None
 */
void bt_set_cb(bt_wifi_event_cb func)
{
	if (bt_status && NULL == bt_status->cb)
		bt_status->cb = func;
}

/**
 * @brief  bt init
 * @note   allocate resoures for bt
 * @retval 0 success
 */
int bt_init()
{
	int ret = 0;

	if (bt_status) {
		ALOGE("bt_status is init ok\n");
		return 1;
	}
	bt_status = calloc(sizeof(bt_status_t), 1);
	if (NULL == bt_status) {
		ALOGE("bt_status is NULL\n");
		return -1;
	}
	/*init mutex*/
	if (pthread_mutex_init(&bt_status->lock_for_sta, NULL)) {
		ALOGE("init sta lock failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto exit;
	}
	if (pthread_mutex_init(&bt_status->lock, NULL)) {
		ALOGE("init ble lock failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto exit;
	}
	if (pthread_cond_init(&bt_status->cond, NULL)) {
		ALOGE("init ble cond failed, errno: %d (%s)\n",
			  errno, strerror(errno));
		ret = -1;
		goto exit;
	}
	bt_status->app_handler.connect_wifi_handler = bt_connect_wifi_callback;
	bt_status->app_handler.disable_wifi_handler = bt_disable_wifi_callback;
	bt_status->app_handler.report_err_handler = bt_report_err_callback;
	bt_status->app_handler.ble_connected_handler = bt_connected_callback;
	/*main task*/
	bt_status->is_run = 1;
	pthread_create(&bt_status->thread_id, NULL, bt_thread_task, NULL);
	return 0;

exit:
	if (bt_status)
		free(bt_status);
	return ret;
}

/**
 * @brief  close wifi
 * @note   NULL
 * @retval None
 */
void bt_wifi_close()
{
#if 0
	int ret = 0;
#endif

	if (NULL == bt_status)
		return;
	/* turn off wifi*/
#if 0
	pthread_mutex_lock(&bt_status->lock_for_sta);
	ret = sta_disable();
	pthread_mutex_unlock(&bt_status->lock_for_sta);


	if (ret == 0) {
		ALOGI("disable sta successed!\n");
		if (bt_status->cb)
			bt_status->cb(bt_wifi_disconnect);
	} else
		ALOGE("disable sta failed!\n");
#endif

}

/**
 * @brief  open wifi
 * @note  NULL
 * @retval None
 */
void bt_wifi_open()
{
#if 0
	int ret = 0;
#endif

	if (NULL == bt_status)
		return;
	/* turn off wifi*/
#if 0
	pthread_mutex_lock(&bt_status->lock_for_sta);
	ret = sta_enable(sta_event_callback);

	if (ret == 0) {
		ALOGI("enable sta successed!\n");
	} else {
		ALOGE("enable sta failed!\n");
	}
	pthread_mutex_unlock(&bt_status->lock_for_sta);
#endif
}

/**
 * @brief  bt deinit
 * @note   free resources
 * @retval None
 */
void bt_deinit()
{
	if (NULL == bt_status)
		return;

	bt_status->is_run = 0;
	pthread_mutex_lock(&bt_status->lock);
	pthread_cond_signal(&bt_status->cond);
	pthread_mutex_unlock(&bt_status->lock);
	pthread_join(bt_status->thread_id, NULL);
	stop();
	free(bt_status);
	bt_status = NULL;

}
