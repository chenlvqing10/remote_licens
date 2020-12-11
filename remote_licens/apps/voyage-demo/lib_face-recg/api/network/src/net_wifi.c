/*
 * net_wifi.c - define interface function of operate wifi device
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "db_device_net_param.h"
#include "net_wifi.h"

#define LOG_TAG "network"
#include <log/log.h>
/*net inc*/
#include "ip_config.h"
#include "wifi_station.h"
#include "wifi_softap.h"
#include "state.h"

typedef struct tag_wifi_con_info {
	char ssid[64];
	char pwd[32];
	int security_type;
} wifi_con_info_t;

static wifi_event_para_t wifi_event_para;

static void net_wifi_event_deal_cb(int event, int type);

/**
 * wifi dhcp callback
 * @param status wifi dhcp event
 * @return NULL
 */
void net_wifi_dhcp_callback(int status)
{
	int wifi_event = 0;
	/*
	**DHCP_ABORT = -2,
	**DHCP_TIMEOUT = -1,
	**DHCP_SUCCESS = 0,
	*/
	ALOGD("%s: the current net dhcp result is %d\n", __func__, status);
	switch (status) {
	case DHCP_ABORT:
		wifi_event = wifi_dhcp_abort;
		break;

	case DHCP_TIMEOUT:
		wifi_event = wifi_dhcp_timeout;
		break;

	case DHCP_SUCCESS:
		wifi_event = wifi_dhcp_success;
		break;
	}
	wifi_event_para.status = wifi_event;
	net_wifi_event_deal_cb(wifi_event, NET_WIFI_EVENT);
}
int net_wifi_sort_by_rssi(SCAN_INFO *p_ret)
{
	HOTSPOT_INFO tmp_scan_info;
	int left = 0;
	int right = p_ret->num - 1;
	int i = 0, j = 0;

	if (p_ret == NULL) {
		ALOGE("%s: parameter pointer is null", __func__);
		return -1;
	}

	if (p_ret->num == 0) {
		ALOGE("%s: scan number is zero", __func__);
		return -1;
	}
	memset(&tmp_scan_info, 0x00, sizeof(tmp_scan_info));
	while (left < right) {
		for (i = left; i < right; i++) {
			if (p_ret->info[i].rssi < p_ret->info[i+1].rssi) {
				memcpy(&tmp_scan_info, &p_ret->info[i],
					sizeof(HOTSPOT_INFO));
				memcpy(&p_ret->info[i], &p_ret->info[i+1],
					sizeof(HOTSPOT_INFO));
				memcpy(&p_ret->info[i+1], &tmp_scan_info,
					sizeof(HOTSPOT_INFO));
				memset(&tmp_scan_info, 0x00, sizeof(tmp_scan_info));
			}
		}
		right = right - 1;
		for (j = right; j > left; j--) {
			if (p_ret->info[j+1].rssi > p_ret->info[j].rssi) {
				memcpy(&tmp_scan_info, &p_ret->info[j+1],
					sizeof(HOTSPOT_INFO));
				memcpy(&p_ret->info[j+1], &p_ret->info[j],
					sizeof(HOTSPOT_INFO));
				memcpy(&p_ret->info[j], &tmp_scan_info,
					sizeof(HOTSPOT_INFO));
				memset(&tmp_scan_info, 0x00, sizeof(tmp_scan_info));
			}
		}
	}
	return 0;
}
int net_wifi_sort_by_ssid(SCAN_INFO *p_ret)
{
	HOTSPOT_INFO tmp_scan_info;
	int number = 0;
	int i = 0, j = 0, k = 0, m = 0;

	if (p_ret == NULL) {
		ALOGE("%s: parameter pointer is null", __func__);
		return -1;
	}

	if (p_ret->num == 0) {
		ALOGE("%s: scan number is zero", __func__);
		return -1;
	}
	while (i < (p_ret->num-1)) {
		if (p_ret->info[i].rssi == p_ret->info[i+1].rssi) {
			if (number == 0) /*save init index*/
				j = i;
			/*save the same rssi number*/
			number++;
			i++;
		} else {
			if (!number) {
				i++;
				continue;
			}
			/*sort by ssid*/;
			for (k = j; k < (j+number); k++) {
				for (m = k; m < (j + number); m++) {
					if (strcmp(p_ret->info[k].ssid,
						p_ret->info[m].ssid) > 0) {
							memset(&tmp_scan_info, 0x00,
								sizeof(tmp_scan_info));

							memcpy(&tmp_scan_info,
								&p_ret->info[k],
								sizeof(HOTSPOT_INFO));

							memcpy(&p_ret->info[k],
								&p_ret->info[m],
								sizeof(HOTSPOT_INFO));

							memcpy(&p_ret->info[m],
								&tmp_scan_info,
								sizeof(HOTSPOT_INFO));
						}
				}
			}
			i++;
			number = 0;
		}
	}
	return 0;
}
int net_wifi_find_same_ssid_and_delete_small_rssi(SCAN_INFO *p_ret)
{
	int number = 0;
	int i = 0, j = 0, k = 0;

	if (p_ret == NULL) {
		ALOGE("%s: parameter pointer is null", __func__);
		return -1;
	}

	if (p_ret->num == 0) {
		ALOGE("%s: scan number is zero", __func__);
		return -1;
	}

	number = p_ret->num;
	for (i = 0; i < p_ret->num; i++) {
		for (j = i+1; j < p_ret->num; j++) {
			/*find the same ssid*/
			if (strcmp(p_ret->info[i].ssid, p_ret->info[j].ssid) == 0) {
				/*if finded, and delete the smaller rssi*/
				if (j == (p_ret->num - 1))/*if last ,direct delete*/
					memset(&p_ret->info[j], 0x00,
						sizeof(HOTSPOT_INFO));
				else {
				/* if (j != (p_ret->num - 1)), cover the front
				** with the back, after end all, free the last*/
					for (k = j; k < (p_ret->num - 1); k++) {
						memcpy(&p_ret->info[k],
							&p_ret->info[k+1],
							sizeof(HOTSPOT_INFO));
					}
					memset(&p_ret->info[k], 0x00,
						sizeof(HOTSPOT_INFO));
				}
				/*change the num*/
				number = number - 1;
				if (number <= i)
					goto exit_find;
				if (number <= j)
					break;
			}
		}
	}

exit_find:
	p_ret->num = number;
	return 0;

}
void net_wifi_sort_scan_info(SCAN_INFO *p_ret)
{
	int ret = 0;

	if (p_ret == NULL) {
		ALOGE("%s: parameter pointer is null", __func__);
		return;
	}

	if (p_ret->num == 0) {
		ALOGE("%s: scan number is zero", __func__);
		return;
	}

	/*first:sort by rssi*/
	ret = net_wifi_sort_by_rssi(p_ret);
	if (ret) {
		ALOGE("%s: sort by rssi failed", __func__);
		return;
	}

	/*second: find same rssi and sort by ssid*/
	ret = net_wifi_sort_by_ssid(p_ret);
	if (ret) {
		ALOGE("%s: sort by rssi failed", __func__);
		return;
	}

	/*find the same ssid, and delete the smaller rssi*/
	ret = net_wifi_find_same_ssid_and_delete_small_rssi(p_ret);
	if (ret) {
		ALOGE("%s: sort by rssi failed", __func__);
		return;
	}
}
/**
 * wifi dhcp callback
 * @param event wifi event
 * @return NULL
 */
void net_wifi_event_callback(int event)
{
	unsigned int para_tmp = NET_WIFI_EVENT;
	int wifi_event = 0;

	switch (event) {
	case STA_DISCONNECTED:
		ALOGD("wifi disconnected!\n");
		wifi_event = wifi_disconnected;
		break;

	case STA_SCAN_RESULTS:
		ALOGD("go to scan_result:%p!!\n", wifi_event_para.result.info);
		pthread_mutex_lock(&wifi_event_para.mutex);
		sta_scan_results(&wifi_event_para.result);
		para_tmp = (unsigned int)&wifi_event_para;
		ALOGD("get scan_result num: %d!!\n", wifi_event_para.result.num);
		net_wifi_sort_scan_info(&wifi_event_para.result);
		ALOGD("after sort the num: %d!!\n", wifi_event_para.result.num);
		wifi_event = wifi_scan_results;
		pthread_mutex_unlock(&wifi_event_para.mutex);
		break;

	case STA_CONNECTED:
		ALOGD("start dhcp!\n");
		wifi_event = wifi_connect;
		net_dev_start_dhcp(WIFI_DEVICE_NAME,
			(dhcp_status_handler)net_wifi_dhcp_callback);
		break;

	case STA_NETWORK_NOT_EXIST:
		wifi_event = wifi_network_not_exist;
		ALOGD("network is not exist!\n");
		break;

	case STA_PASSWD_ERROR:
		wifi_event = wifi_password_error;
		ALOGD("password is wrong!\n");
		break;

	case STA_CONNECT_REJECT:
		wifi_event = wifi_connect_reject;
		ALOGD("connection is rejected!\n");
		break;
	}

	ALOGD("send msg to UI interface!!\n");
	wifi_event_para.status = wifi_event;

	net_wifi_event_deal_cb(wifi_event, para_tmp);
}
static void net_wifi_event_deal_cb(int event, int type)
{
	int i;

	for (i = 0; i < NET_EVENT_CB_NO; i++) {
		if (wifi_event_para.notify_cb[i])
			wifi_event_para.notify_cb[i](event, type);
		else
			break;
	}

}
/**
 * register wifi event deal callback function
 * @param p a pointer of callback function
 * @return NULL
 */
void net_wifi_register_event_cb(void *p)
{
	int i;

	if (p == NULL) {
		ALOGE("%s: cb function is NULL!\n", __func__);
		return;
	}
	/*lookup cb is exist*/
	for (i = 0; i < NET_EVENT_CB_NO; i++) {
		if ((p == wifi_event_para.notify_cb[i]) &&
			(wifi_event_para.notify_cb[i]))
			return;
	}
	for (i = 0; i < NET_EVENT_CB_NO; i++) {
		if (wifi_event_para.notify_cb[i] == NULL) {
			wifi_event_para.notify_cb[i] = (wifi_event_cb)p;
			break;
		}
	}
}
void net_wifi_init_scan_result(void)
{
	ALOGD("%s: scan pointer: %p", __func__, wifi_event_para.result.info);
	pthread_mutex_lock(&wifi_event_para.mutex);
	if (wifi_event_para.result.info)
		free(wifi_event_para.result.info);
	wifi_event_para.result.info = NULL;
	wifi_event_para.status = wifi_closed;
	pthread_mutex_unlock(&wifi_event_para.mutex);
}
void *net_wifi_open_task(void *p)
{
	int ret;

	/*init scan var*/
	net_wifi_init_scan_result();
	/*open wifi*/
	ret = sta_enable((STA_EVENT_HANDLER)p);
	if (ret == -1) {
		ALOGE("%s: enable wifi failed!\n", __func__);
		net_wifi_event_deal_cb(wifi_open_failed, NET_WIFI_EVENT);
		return NULL;
	}
	ret = sta_scan();
	if (ret == -1) {
		ALOGE("%s: scan failed!\n", __func__);
		net_wifi_event_deal_cb(wifi_open_scan_failed, NET_WIFI_EVENT);
	}
	return NULL;
}
/**
 * open wifi device
 * @param p_cb a poniter of wifi event callback
 * @return  0-success, other-failed
 */
int net_wifi_open(void *p_cb)
{
	pthread_t thread_wifi_open;

	pthread_create(&thread_wifi_open, NULL, net_wifi_open_task, (void *)p_cb);
	pthread_detach(thread_wifi_open);

	return 0;
}

void *net_wifi_close_task(void *p)
{
	int ret = 0;
	/*stop dhcp*/
	net_dev_stop_dhcp(WIFI_DEVICE_NAME);
	ALOGD("%s: sta_disable start", __func__);
	/*close wifi*/
	ret = sta_disable();
	if (ret == -1) {
		ALOGE("disable wifi failed!\n");
		net_wifi_event_deal_cb(wifi_close_failed, NET_WIFI_EVENT);
		return NULL;
	}
	net_wifi_init_scan_result();
	net_wifi_event_deal_cb(wifi_close_successful, NET_WIFI_EVENT);

	return NULL;
}

/**
 * close wifi device
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_close(void)
{
	pthread_t thread_wifi_close;

	pthread_create(&thread_wifi_close, NULL, net_wifi_close_task, NULL);
	pthread_detach(thread_wifi_close);

	return 0;
}

/**
 * get ssid of wifi connected
 * @param p_ssid a pointer of wifi ssid
 * @return  0-success, other-failed
 */
int net_wifi_get_connect_ssid(char *p_ssid)
{
	int ret = 0;
	CONNECT_INFO connect_info;

	if (p_ssid == NULL) {
		ALOGE("%s: parameter pointer is NULL", __func__);
		return -1;
	}

	ret = sta_get_connect_info(&connect_info);
	if (ret) {
		ALOGE("%s: get wifi connect ssid failed\n", __func__);
		return -1;
	}
	ALOGD("%s: %s", __func__, connect_info.bssid);
	ALOGD("%s: %s", __func__, connect_info.ssid);
	ALOGD("%s: %d", __func__, connect_info.id);
	memcpy(p_ssid, connect_info.ssid, SSID_MAX_LENGTH);

	return 0;
}

/**
 * connect wifi net
 * @param p_ssid a pointer of wifi ssid
 * @param p_pwd a pointer of wifi password
 * @param security_type security type
 * @return  0-success, other-failed
 */
void *net_wifi_connect_task(void *p)
{
	int ret = 0;
	wifi_con_info_t connect_info;

	/*parameter check*/
	if (!p) {
		ALOGE("%s: parameter pointer is NULL", __func__);
		net_wifi_event_deal_cb(wifi_start_connect_failed, NET_WIFI_EVENT);
		return NULL;
	}

	memcpy(&connect_info, (wifi_con_info_t *)p, sizeof(connect_info));
	if (p)
		free(p);
	ret = sta_connect(connect_info.ssid, connect_info.pwd,
		connect_info.security_type);
	if (ret == -1) {
		ALOGE("connect wifi failed!\n");
		net_wifi_event_deal_cb(wifi_start_connect_failed, NET_WIFI_EVENT);
		return NULL;
	}
	return NULL;
}
/**
 * start wifi connect
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_start_connect(char *p_ssid, char *p_pwd, int security_type)
{
	pthread_t thread_wifi_connect;
	wifi_con_info_t *connect_info = NULL;

	/*parameter check*/
	if (!p_ssid || !p_pwd) {
		ALOGE("%s: parameter pointer is NULL", __func__);
		return -1;
	}

	connect_info = (wifi_con_info_t *)malloc(sizeof(wifi_con_info_t));
	if (connect_info == NULL) {
		ALOGE("%s: malloc memery failed\n", __func__);
		return -1;

	}
	/*init parameters*/
	memset(connect_info, 0x00, sizeof(wifi_con_info_t));
	snprintf(connect_info->ssid, sizeof(connect_info->ssid), "%s", p_ssid);
	snprintf(connect_info->pwd, sizeof(connect_info->pwd), "%s", p_pwd);
	connect_info->security_type = security_type;

	pthread_create(&thread_wifi_connect, NULL,
		net_wifi_connect_task, (void *)connect_info);
	pthread_detach(thread_wifi_connect);

	return 0;
}

/**
 * disconnect wifi net
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_disconnect(void)
{
	int ret = 0;
	ret = sta_disconnect();
	if (ret == -1) {
		ALOGE("disconnect wifi failed!\n");
		return -1;
	}
	return ret;
}

/**
 * wifi hotspot scan task
 * @param NULL
 * @return  0-success, other-failed
 */
void *net_wifi_scan_task(void *p)
{
	int ret = 0;
	ret = sta_scan();
	if (ret == -1) {
		ALOGE("%s: scan failed!\n", __func__);
		net_wifi_event_deal_cb(wifi_open_scan_failed, NET_WIFI_EVENT);
	}

	return NULL;
}

/**
 * start wifi scan
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_start_scan(void)
{
	pthread_t thread_wifi_scan;
	pthread_create(&thread_wifi_scan, NULL, net_wifi_scan_task, NULL);
	pthread_detach(thread_wifi_scan);

	return 0;
}

/**
 * start net dhcp
 * @param p_cb a pointer of callback funtion
 * @return NULL
 */
void net_wifi_start_dhcp(void *p_cb)
{
	net_dev_start_dhcp(WIFI_DEVICE_NAME, (dhcp_status_handler)p_cb);
}
/**
 * get ip address
 * @param p_ip a poniter of save ip addr parameter
 * @return NULL
 */
int net_wifi_get_ip_addr(wifi_ip_para_t *p_ip)
{
	int ret = 0;
	ip_static_config_t net_para;

	ret = net_dev_get_static_ip(WIFI_DEVICE_NAME, &net_para);
	if (ret) {
		ALOGE("%s: get net para failed\n", __func__);
		return -1;
	}
	snprintf(p_ip->ip, sizeof(p_ip->ip), "%d.%d.%d.%d", net_para.ip_addr[0],
		net_para.ip_addr[1], net_para.ip_addr[2], net_para.ip_addr[3]);
	snprintf(p_ip->mask, sizeof(p_ip->mask), "%d.%d.%d.%d", net_para.netmask[0],
		net_para.netmask[1], net_para.netmask[2], net_para.netmask[3]);
	snprintf(p_ip->gateway, sizeof(p_ip->gateway), "%d.%d.%d.%d", net_para.gateway[0],
		net_para.gateway[1], net_para.gateway[2], net_para.gateway[3]);
	snprintf(p_ip->dns, sizeof(p_ip->dns), "%d.%d.%d.%d", net_para.dns0[0],
		net_para.dns0[1], net_para.dns0[2], net_para.dns0[3]);
	return 0;
}

/**
 * close wifi hotspot
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_config_hotspot(char *p_ssid, char *p_password)
{
	int ret = 0;
	AP_CONFIG config;

	/*parameter check*/
	if (!p_password || !p_ssid) {
		ALOGE("%s: parameter pointer is NULL %p-%p\n", __func__,
			p_password, p_ssid);
		return -1;
	}
	/*copy data*/
	memset(&config, 0x00, sizeof(config));
	config.ssid = p_ssid;
	config.passwd = p_password;
	config.security = WIFI_WPA2_PSK;
	/*config hotspot parameters*/
	ret = softap_set_config(&config);
	if (ret == -1) {
		ALOGE("scan failed!\n");
		return -1;
	}

	return ret;
}
void *net_wifi_open_hotspot_task(void *p)
{
	int ret = 0;
	wifi_con_info_t connect_info;

	/*parameter check*/
	if (!p) {
		ALOGE("%s: parameter pointer is NULL", __func__);
		net_wifi_event_deal_cb(wifi_open_hotspot_failed, NET_WIFI_EVENT);
		return NULL;
	}

	memcpy(&connect_info, (wifi_con_info_t *)p, sizeof(connect_info));
	if (p)
		free(p);

	ret = net_wifi_config_hotspot(connect_info.ssid, connect_info.pwd);
	if (ret) {
		ALOGE("%s: config hotspot parameters!\n", __func__);
		net_wifi_event_deal_cb(wifi_open_hotspot_failed, NET_WIFI_EVENT);
		return NULL;
	}
	ret = softap_enable();
	if (ret == -1) {
		ALOGE("%s: open hotspot failed!\n", __func__);
		net_wifi_event_deal_cb(wifi_open_hotspot_failed, NET_WIFI_EVENT);
		return NULL;
	}
	return NULL;

}
/**
 * open wifi hotspot
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_open_hotspot(char *p_ssid, char *p_password)
{
	pthread_t thread_wifi_hotspot;
	wifi_con_info_t *connect_info = NULL;

	/*parameter check*/
	if (!p_ssid || !p_password) {
		ALOGE("%s: parameter pointer is NULL", __func__);
		return -1;
	}
	connect_info = (wifi_con_info_t *)malloc(sizeof(wifi_con_info_t));
	if (connect_info == NULL) {
		ALOGE("%s: malloc memery failed\n", __func__);
		return -1;

	}
	/*init parameters*/
	memset(connect_info, 0x00, sizeof(wifi_con_info_t));
	snprintf(connect_info->ssid, sizeof(connect_info->ssid), "%s", p_ssid);
	snprintf(connect_info->pwd, sizeof(connect_info->pwd), "%s", p_password);

	pthread_create(&thread_wifi_hotspot, NULL,
		net_wifi_open_hotspot_task, (void *)connect_info);
	pthread_detach(thread_wifi_hotspot);

	return 0;

}

/**
 * close wifi hotspot
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_close_hotspot(void)
{
	int ret = 0;
	ret = softap_disable();
	if (ret == -1) {
		ALOGE("close hotspot failed!\n");
		return -1;
	}
	return ret;
}

/**
 * get wifi device parameters
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wifi_get_para(wifi_config_para_t *p_wifi)
{
	int ret = 0;
	device_net_param_t net_info;

	/*verify parameters*/
	if (!p_wifi) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}

	/*init var*/
	memset(&net_info, 0x00, sizeof(net_info));

	/*read device net parameter*/
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return -1;
	}

	/*data copy*/
	p_wifi->enable_wifi = net_info.enable_wifi;
	p_wifi->enable_hotspot = net_info.enable_hotspot;
	strcpy(p_wifi->hotspot_password, net_info.hotspot_password);
	strcpy(p_wifi->hotspot_name, net_info.hotspot_name);

	return 0;
}

/**
 * get a pointer of wifi scan results
 * @param NULL
 * @return a pointer of wifi scan results
 */
wifi_event_para_t *net_wifi_get_events(void)
{
	/*verify parameters*/
	return &wifi_event_para;
}

/**
 * set wifi device parameters
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wifi_set_config_para(wifi_config_para_t *p_wifi)
{
	int ret = 0;
	device_net_param_t net_info;

	ALOGD("%s:enter", __func__);
	/* verify parameters */
	if (!p_wifi) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}

	/*read device net parameter*/
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return -1;
	}
	/*close model*/
	if (net_info.enable_wifi && !p_wifi->enable_wifi) {
		/*close wifi*/
		ret = net_wifi_close();
		if (ret) {
			ALOGE("%s: close wifi device failed\n", __func__);
			return -1;
		}
	} else if (net_info.enable_wifi && p_wifi->enable_wifi &&
		!p_wifi->enable_hotspot) {
		net_wifi_start_scan();
	}

	/* init wifi module */
	if (!net_info.enable_wifi && p_wifi->enable_wifi && !p_wifi->enable_hotspot) {
		ret = net_wifi_open(net_wifi_event_callback);
		if (ret) {
			ALOGE("%s: open wifi device failed\n", __func__);
			return -1;
		}
	} else if (!p_wifi->enable_wifi) {

		/*close hotspot*/
		if ((net_info.enable_hotspot) && (!p_wifi->enable_hotspot))
			net_wifi_close_hotspot();
		else if (p_wifi->enable_hotspot) {

			/*first close wifi hotspot*/
			net_wifi_close_hotspot();

			/* config parameters, init wifi hotspot */
			ret = net_wifi_open_hotspot(p_wifi->hotspot_name,
				p_wifi->hotspot_password);
			if (ret) {
				ALOGE("%s: init hotspot function failed\n", __func__);
				return -1;
			}
			strcpy(net_info.hotspot_password, p_wifi->hotspot_password);
			strcpy(net_info.hotspot_name, p_wifi->hotspot_name);
		}
	}

	/*save parameter to database*/
	ALOGD("%s: update parameter to database", __func__);
	net_info.enable_wifi = p_wifi->enable_wifi;
	net_info.enable_hotspot = p_wifi->enable_hotspot;
	ret = db_device_net_param_update(&net_info);
	if (ret) {
		ALOGE("%s: save para failed(%d)\n", __func__, ret);
		return ret;
	}

	return ret;
}
/**
 * update wifi device parameters when operate wifi device
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wifi_update_config_para(wifi_config_para_t *p_wifi)
{
	int ret = 0;
	device_net_param_t net_info;

	ALOGD("%s:enter", __func__);
	/* verify parameters */
	if (!p_wifi) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}

	/*read device net parameter*/
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return -1;
	}
	/*save parameter to database*/
	ALOGD("%s: update parameter to database", __func__);
	if (p_wifi->enable_hotspot) {
		strcpy(net_info.hotspot_password, p_wifi->hotspot_password);
		strcpy(net_info.hotspot_name, p_wifi->hotspot_name);
	}
	net_info.enable_wifi = p_wifi->enable_wifi;
	net_info.enable_hotspot = p_wifi->enable_hotspot;
	ret = db_device_net_param_update(&net_info);
	if (ret) {
		ALOGE("%s: save para failed(%d)\n", __func__, ret);
		return ret;
	}

	return ret;
}

/**
 * wifi init function
 * @param NULL
 * @return 0-successful,other-failed
 */
int net_wifi_init(wifi_event_cb p_cb)
{
	int ret = 0;
	device_net_param_t net_info;

	/*init static global var*/
	memset(&wifi_event_para, 0x00, sizeof(wifi_event_para));
	memset(&net_info, 0x00, sizeof(net_info));
	net_wifi_init_scan_result();
	net_wifi_register_event_cb(p_cb);

	pthread_mutex_init(&wifi_event_para.mutex, NULL);

	/*read device net parameter*/
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return -1;
	}
	ret = 0;
	/*init wifi device*/
	if (net_info.enable_wifi) {
		ALOGD("%s: open wifi module device\n", __func__);
		ret = net_wifi_open(net_wifi_event_callback);
		if (ret) {
			ALOGE("%s: open wifi device failed\n", __func__);
			return -1;
		}
	} else if (net_info.enable_hotspot) {
		/*config parameters*/
		ret = net_wifi_open_hotspot(net_info.hotspot_name,
			net_info.hotspot_password);
		if (ret) {
			ALOGE("%s: init hotspot function failed\n", __func__);
			return -1;
		}
	}

	return ret;
}

/**
 * wifi deinit function
 * @param NULL
 * @return NULL
 */
void net_wifi_deinit(void)
{
	int i;
	int ret = 0;
	device_net_param_t net_info;

	memset(&net_info, 0x00, sizeof(net_info));
	pthread_mutex_destroy(&wifi_event_para.mutex);

	/*read device net parameter*/
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return;
	}

	/*init wifi device*/
	if (!net_info.enable_wifi && !net_info.enable_hotspot)
		return;

	/*free source*/
	if (wifi_event_para.result.info)
		free(wifi_event_para.result.info);
	wifi_event_para.result.info = NULL;

	for (i = 0; i < NET_EVENT_CB_NO; i++)
		wifi_event_para.notify_cb[i] = NULL;

	/*close wifi device*/
	if (net_info.enable_wifi) {
		/*stop dhcp*/
		net_dev_stop_dhcp(WIFI_DEVICE_NAME);
		ALOGD("%s: sta_disable start", __func__);
		/*close wifi*/
		sta_disable();
	}

	/*close wifi hotspot*/
	net_wifi_close_hotspot();
}
