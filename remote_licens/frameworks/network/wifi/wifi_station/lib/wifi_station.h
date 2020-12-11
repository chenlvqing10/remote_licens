#ifndef __WIFI_STATION_H
#define __WIFI_STATION_H

#if __cplusplus
extern "C" {
#endif

#define BSSID_MAX_LENGTH 17
#define SSID_MAX_LENGTH 32

/**
 * STA_EVENT - station event.
 * @STA_DISCONNECTED: station has been disconnected.
 * @STA_SCAN_RESULTS: scan has been completed and scan result is available.
 * @STA_CONNECTED: station has been connected.
 * @STA_NETWORK_NOT_EXIST: the connecting network does not exist.
 * @STA_PASSWD_ERROR: hotspot password is incorrect.
 * @STA_CONNECT_REJECT: wifi connection is rejected.
 * @STA_CONNECT_ABORT: wifi connection is aborted.
 * @STA_UNKNOWN_EVENT: unknown event.
 */
typedef enum {
	STA_DISCONNECTED = 0,
	STA_SCAN_RESULTS,
	STA_CONNECTED,
	STA_NETWORK_NOT_EXIST,
	STA_PASSWD_ERROR,
	STA_CONNECT_REJECT,
	STA_CONNECT_ABORT,
	STA_UNKNOWN_EVENT,
} STA_EVENT;

/**
 * SECURITY - encryption method.
 * @SECURITY_OPEN: open.
 * @SECURITY_WPA_PSK: wpa_psk.
 * @SECURITY_WPA2_PSK: wpa2_psk.
 */
typedef enum {
	SECURITY_OPEN = 0,
	SECURITY_WPA_PSK,
	SECURITY_WPA2_PSK,
} SECURITY;

/**
 * HOTSPOT_INFO - information of hosspot.
 * @channel: channel.
 * @rssi: signal strength.
 * @security: encryption method.
 * @bssid: bssid.
 * @ssid: ssid.
 */
typedef struct {
	int channel;
	int rssi;
	SECURITY security;
	char bssid[BSSID_MAX_LENGTH+1];
	char ssid[SSID_MAX_LENGTH+1];
} HOTSPOT_INFO;

/**
 * SCAN_INFO - scan information.
 * @num: number of hotspot.
 * @info: information of hotspot.
 */
typedef struct {
	int num;
	HOTSPOT_INFO *info;
} SCAN_INFO;

/**
 * WIFI_CONFIG - configuration of saved hotspot.
 * @net_id: network id.
 * @security: encryption method.
 * @ssid: ssid.
 */
typedef struct {
	int net_id;
	SECURITY security;
	char ssid[SSID_MAX_LENGTH+1];
} WIFI_CONFIG;

/**
 * SAVED_WIFI_CONFIG - saved wifi configuration information.
 * @num: number of saved hotspot.
 * @config: configuration of saved hotspot.
 */
typedef struct {
	int num;
	WIFI_CONFIG *config;
} SAVED_WIFI_CONFIG;

/**
 * CONNECT_INFO - current connection status information.
 * @id: network id.
 * @bssid: bssid.
 * @ssid: ssid.
 */
typedef struct {
	int id;
	char bssid[BSSID_MAX_LENGTH+1];
	char ssid[SSID_MAX_LENGTH+1];
} CONNECT_INFO;

/* callback function type */
typedef void (*STA_EVENT_HANDLER)(STA_EVENT event);

/**
 * sta_enable - enable station.
 *
 * @handler: registered callback function.
 *
 * return 0 if success; otherwise failed.
 */
int sta_enable(STA_EVENT_HANDLER handler);

/**
 * sta_disable - disable station.
 *
 * return 0 if success; otherwise failed.
 */
int sta_disable(void);

/**
 * sta_connect - connect the visible hotspot.
 *
 * @ssid: hotspot name.
 * @passwd: hotspot password.
 * @security: hotspot encryption method.
 *
 * return 0 if success; otherwise failed.
 */
int sta_connect(const char *ssid, const char *passwd, SECURITY security);

/**
 * sta_connect_hidden - connect the invisible hotspot.
 *
 * @ssid: hotspot name.
 * @passwd: hotspot password.
 * @security: hotspot encryption method.
 *
 * return 0 if success; otherwise failed.
 */
int sta_connect_hidden(const char *ssid, const char *passwd, SECURITY security);

/**
 * sta_connect_with_bssid - connect to a hotspot for a specific bssid.
 *
 * @ssid: hotspot name.
 * @passwd: hotspot password.
 * @bssid: hotapot bssid.
 * @security: hotspot encryption mode.
 *
 * return 0 if success; otherwise failed.
 */
int sta_connect_with_bssid(const char *ssid, const char *passwd,
			   SECURITY security, const char *bssid);

/**
 * sta_connect_with_id - connect AP by using network id.
 *
 * @netid: network id.
 *
 * return 0 if success; otherwise failed.
 */
int sta_connect_with_id(int netid);

/**
 * sta_disconnect - disconnect from AP.
 *
 * return 0 if success; otherwise failed.
 */
int sta_disconnect(void);

/**
 * sta_scan - perform scan operation once.
 *
 * return 0 if success; otherwise failed.
 */
int sta_scan(void);

/**
 * sta_scan_results - obtain scan results.
 *
 * @scan_info: buffer for storing scan information.
 *
 * return 0 if success; otherwise failed.
 */
int sta_scan_results(SCAN_INFO *scan_info);

/**
 * sta_get_connect_info - obtain current connection status information.
 *
 * @connect_info: buffer for storing connection status information.
 *
 * return 0 if success; otherwise failed.
 */
int sta_get_connect_info(CONNECT_INFO *connect_info);

/**
 * sta_disable_net_conf - disable network configuration.
 *
 * @net_id: network id.
 *
 * return 0 if success; otherwise failed.
 */
int sta_disable_net_conf(int net_id);

/**
 * sta_delete_net_conf - delete network configuration.
 *
 * @net_id: network id.
 *
 * return 0 if success; otherwise failed.
 */
int sta_delete_net_conf(int net_id);

/**
 * sta_delete_all_net_conf - delete all network configuration.
 *
 * return 0 if success; otherwise failed.
 */
int sta_delete_all_net_conf();

/**
 * sta_disable_all_net_conf - disable all network configuration.
 *
 * return 0 if success; otherwise failed.
 */
int sta_disable_all_net_conf();

/**
 * sta_get_net_id - obtain network id.
 *
 * @ssid: ssid for the specified netconf.
 * @security: security for the specified netconf.
 * @netid: buffer for storing netid if the specified netconf exists.
 *
 * return 0 if success; otherwise failed.
 */
int sta_get_net_id(const char *ssid, SECURITY security, int *netid);

/**
 * sta_get_save_config - obtain all saved wifi configuration information.
 *
 * @saved_wifi_config: buffer for storing saved wifi configuration.
 *
 * return 0 if success; otherwise failed.
 */
int sta_get_saved_config(SAVED_WIFI_CONFIG *saved_wifi_config);

#if __cplusplus
};
#endif

#endif
