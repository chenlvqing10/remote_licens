/*
 * net_wifi.h - export some interface function of operate wifi device
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

#ifndef _NET_WIFI_H
#define _NET_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "wifi_station.h"

#define WIFI_DEVICE_NAME "wlan0"
#define NET_WIFI_EVENT 0xFF
#define NET_EVENT_CB_NO 5
typedef void (*wifi_event_cb)(int, int);

/*
 *typedef enum {
 *	STA_DISCONNECTED = 0,
 *	STA_SCAN_RESULTS,
 *	STA_CONNECTED,
 *	STA_NETWORK_NOT_EXIST,
 *	STA_PASSWD_ERROR,
 *	STA_CONNECT_REJECT,
 *} STA_EVENT;
*/
/*
	**DHCP_ABORT = -2,
	**DHCP_TIMEOUT = -1,
	**DHCP_SUCCESS = 0,
*/

typedef enum {
	wifi_closed = -1,
	wifi_disconnected = 0,
	wifi_scan_results,
	wifi_connect,
	wifi_network_not_exist,
	wifi_password_error,
	wifi_connect_reject,
	wifi_dhcp_abort,
	wifi_dhcp_timeout,
	wifi_dhcp_success,
	wifi_start_connect_failed,
	wifi_open_failed,
	wifi_open_hotspot_failed,
	wifi_open_scan_failed,
	wifi_close_failed,
	wifi_close_successful,
} wifi_event;
typedef struct tag_wifi_ip_para {
	char ip[32];
	char mask[32];
	char gateway[32];
	char dns[32];
} wifi_ip_para_t;
typedef struct tag_wifi_para {
	/* wifi enable */
	char enable_wifi;
	/* Hotspot Enable */
	char enable_hotspot;

	/* Hotspot name */
	char hotspot_name[32];

	/* Hotspot password */
	char hotspot_password[9];
} wifi_config_para_t;

typedef struct tag_wifi_scan {
	SCAN_INFO result;
	pthread_mutex_t mutex;
	int status;
	wifi_event_cb notify_cb[NET_EVENT_CB_NO];
} wifi_event_para_t;



/**
 * register wifi event deal callback function
 * @param p a pointer of callback function
 * @return NULL
 */
void net_wifi_register_event_cb(void *p);

/**
 * open wifi device
 * @param p_cb a poniter of wifi event callback
 * @return  0-success, other-failed
 */
int net_wifi_open(void *p_cb);

/**
 * close wifi device
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_close(void);

/**
 * get ssid of wifi connected
 * @param p_ssid a pointer of wifi ssid
 * @return  0-success, other-failed
 */
int net_wifi_get_connect_ssid(char *p_ssid);

/**
 * connect wifi net
 * @param p_ssid a pointer of wifi ssid
 * @param p_pwd a pointer of wifi password
 * @param security_type security type
 * SECURITY_OPEN = 0,
 * SECURITY_WPA_PSK = 1,
 * SECURITY_WPA2_PSK = 2,
 * @return  0-success, other-failed
 */
int net_wifi_start_connect(char *p_ssid, char *p_pwd, int security_type);

/**
 * disconnect wifi net
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_disconnect(void);

/**
 * start wifi scan
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_start_scan(void);

/**
 * get ip address
 * @param p_ip a poniter of save ip addr parameter
 * @return NULL
 */
int net_wifi_get_ip_addr(wifi_ip_para_t *p_ip);

/**
 * open wifi hotspot
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_open_hotspot(char *p_ssid, char *p_password);

/**
 * close wifi hotspot
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wifi_close_hotspot(void);

/**
 * get wifi device parameters
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wifi_get_para(wifi_config_para_t *p_wifi);

/**
 * get a pointer of wifi scan results
 * @param NULL
 * @return a pointer of wifi scan results
 */
wifi_event_para_t *net_wifi_get_events(void);

/**
 * update wifi device parameters
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wifi_set_config_para(wifi_config_para_t *p_wifi);

/**
 * update wifi device parameters when operate wifi device
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wifi_update_config_para(wifi_config_para_t *p_wifi);

/**
 * wifi init function
 * @param NULL
 * @return 0-successful,other-failed
 */
int net_wifi_init(wifi_event_cb p_cb);

/**
 * wifi deinit function
 * @param NULL
 * @return NULL
 */
void net_wifi_deinit(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

