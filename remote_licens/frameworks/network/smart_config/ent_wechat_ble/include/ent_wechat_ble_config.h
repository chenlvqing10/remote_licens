#ifndef _ENT_WECHAT_BLE_CONFIG_H
#define _ENT_WECHAT_BLE_CONFIG_H

#if __cplusplus
extern "C" {
#endif

#define SN_MAX_LENGTH			24
#define SECRET_NO_MAX_LENGTH	48
#define DEVICE_ID_MAX_LENGTH	8
#define MAC_ADDR_MAX_LENGTH		17

/**
 * PROTO_ERR_TYPE - protocol error type.
 * @ERR_REPORT_WIFI_STATUS: wifi connect failed.
 * @ERR_START_ADV_FAILED: start advertising failed
 * @ERR_START_SERVER_FAILED: start gatt server failed.
 */
typedef enum {
	ERR_REPORT_WIFI_STATUS = 0,
	ERR_START_ADV_FAILED,
	ERR_START_SERVER_FAILED,
} PROTO_ERR_TYPE;

/**
 * WIFI_ERR_CODE - wifi error code.
 * @WIFI_CONNECTED: wifi is connected.
 * @WIFI_NOT_EXIST: wifi hotspot is not exist.
 * @WIFI_PASSWORD_ERR: wifi password is wrong.
 */
typedef enum {
	WIFI_CONNECTED = 0,
	WIFI_NOT_EXIST,
	WIFI_PASSWORD_ERR,
} WIFI_ERR_CODE;

/**
 * DEVICE_INFO - device information.
 * @sn: device serial number.
 * @secret_no: device key.
 * @device_id: device id.
 * @mac_addr: bt mac address.
 */
typedef struct {
	char sn[SN_MAX_LENGTH+1];
	char secret_no[SECRET_NO_MAX_LENGTH+1];
	char device_id[DEVICE_ID_MAX_LENGTH+1];
	char mac_addr[MAC_ADDR_MAX_LENGTH+1];
} DEVICE_INFO;

/**
 * CONNECT_WIFI_HANDLER - connect wifi callback handler.
 * @ssid: hotspot name.
 * @password: hotspot password.
 * @protocol: hotspot encryption method.
 */
typedef int (*CONNECT_WIFI_HANDLER)(const char *ssid, const char *password,
		const char *protocol);

/**
 * DISABLE_WIFI_HANDLER - disable wifi callback handler.
 */
typedef int (*DISABLE_WIFI_HANDLER)(void);

/**
 * REPORT_ERR_HANDLER - report error of protocol callback handler.
 * @err: protocol err type.
 */
typedef int (*REPORT_ERR_HANDLER)(int err);

/**
 * BLE_CONNECTED_HANDLER - ble connected callback handler.
 */
typedef int (*BLE_CONNECTED_HANDLER)(void);

/**
 * APP_HANDLER - app callback handlers.
 * @connect_wifi_handler: connect wifi callback handler.
 * @disable_wifi_handler: disable wifi callback handler.
 * @report_err_handler: report error of protocol callback handler.
 * @ble_connected_handler: ble connected callback handler.
 */
typedef struct {
	CONNECT_WIFI_HANDLER connect_wifi_handler;
	DISABLE_WIFI_HANDLER disable_wifi_handler;
	REPORT_ERR_HANDLER report_err_handler;
	BLE_CONNECTED_HANDLER ble_connected_handler;
} APP_HANDLER;

/**
 * start - start ble service of setting wifi.
 * @app_handler: app callback handlers.
 * @device_info: device information.
 *
 * return 0 if success; otherwise failed.
 */
int start(APP_HANDLER *app_handler, DEVICE_INFO *device_info);

/**
 * report_wifi_status - report wifi connection status.
 * @wifi_err_code: wifi error code.
 * @ip: ip address of wlan.
 *
 * return 0 if success; otherwise failed.
 */
int report_wifi_status(int wifi_err_code, const char *ip);

/**
 * start - stop ble service of setting wifi.
 *
 * return 0 if success; otherwise failed.
 */
int stop(void);

#if __cplusplus
};
#endif

#endif
