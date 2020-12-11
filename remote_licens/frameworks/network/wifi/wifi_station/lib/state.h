#ifndef __STATE_H
#define __STATE_H

#if __cplusplus
extern "C" {
#endif

/**
 * SWITCH_STATUS - switch status of station.
 * @STA_DISABLED: station is disabled.
 * @STA_DISABLING: station is disabling.
 * @STA_ENABLED: station is enabled.
 * @STA_ENABLING: station is enabling.
 */
typedef enum {
	STA_DISABLED = 0,
	STA_DISABLING,
	STA_ENABLED,
	STA_ENABLING,
} SWITCH_STATUS;

/**
 * STA_STATE - connection state of station.
 * @STA_DISCONNECTED_STATE: station is disconnected.
 * @STA_CONNECTED_STATE: station is connected.
 */
typedef enum {
	STA_DISCONNECTED_STATE = 0,
	STA_CONNECTED_STATE,
} STA_STATE;

/**
 * CONNECT_EVENT - exception event that occurred while connecting.
 * @NETWORK_NOT_EXIST: the connection network does not exist.
 * @PASSWD_ERROR: the wifi password is not correct.
 * @CONNECT_REJECT: the wifi connection is rejected.
 * @UNKNOWN_EVENT: unknown event.
 */
typedef enum {
	UNKNOWN_EVENT = 0,
	NETWORK_NOT_EXIST,
	PASSWD_ERROR,
	CONNECT_REJECT,
	CONNECT_ABORT,
} CONNECT_EVENT;

/**
 * WORK_FLAG - work flag.
 * @ON: work is on.
 * @OFF: work is off.
 */
typedef enum {
	OFF = 0,
	ON,
} WORK_FLAG;

/**
 * STATION_INFO - station information.
 * @sta_state: connection state.
 * @connect_event: connection event.
 * @disconnect_flag: disconnection flag.
 * @scan_flag: scan flag.
 * @scan_result_flag: scan result flag.
 * @status_flag: get status flag.
 * @delete_netconf_flag: remove network flag.
 * @send_command_flag: send command flag.
 */
typedef struct {
	int sta_state;
	int connect_event;
	int connect_flag;
	int disconnect_flag;
	int scan_flag;
	int scan_result_flag;
	int status_flag;
	int delete_netconf_flag;
	int send_command_flag;
} STATION_INFO;

/**
 * set_switch_status - set switch status of station.
 *
 * @status: enabled status.
 *
 * return 0 if success; otherwise failed.
 */
int set_switch_status(int status);

/**
 * get_switch_status - get switch status of station.
 *
 * return switch status of station.
 */
int get_switch_status(void);

/**
 * is_wifi_enabled - get the enabled status of station.
 *
 * return 0 if station is enabled; -1 if station is disabled, or disabing,
 * or enabling.
 */
int is_wifi_enabled(void);

/**
 * set_sta_state - set connection state of station.
 *
 * @state: connection state.
 *
 * return 0 if success; otherwise failed.
 */
int set_sta_state(int state);

/**
 * get_sta_state - get connection state of station.
 *
 * return conection state of station.
 */
int get_sta_state(void);

/**
 * set_connect_event - set connection event of station.
 *
 * @event: connection event.
 *
 * return 0 if success; otherwise failed.
 */
int set_connect_event(int event);

/**
 * get_connect_event - get connection event of station.
 *
 * return connection event of station.
 */
int get_connect_event(void);

/**
 * set_connect_flag - set connection flag of station.
 *
 * @flag: connection flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_connect_flag(int flag);

/**
 * get_connect_flag - get connection flag of station.
 *
 * return connection flag of station.
 */
int get_connect_flag(void);

/**
 * set_scan_flag - set scan flag of station.
 *
 * @flag: scan flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_scan_flag(int flag);

/**
 * get_scan_flag - get scan flag of station.
 *
 * return scan flag of station.
 */
int get_scan_flag(void);

/**
 * set_disconnect_flag - set disconnection flag of station.
 *
 * @flag: disconnection flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_disconnect_flag(int flag);

/**
 * get_disconnect_flag - get disconnection flag of station.
 *
 * return disconnection flag of station.
 */
int get_disconnect_flag(void);

/**
 * set_status_flag - set getting status flag of station.
 *
 * @flag: getting status flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_status_flag(int flag);

/**
 * get_status_flag - get getting status flag of station.
 *
 * return getting status flag of station.
 */
int get_status_flag(void);

/**
 * set_scan_result_flag - set scan result flag of station.
 *
 * @flag: scan result flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_scan_result_flag(int flag);

/**
 * get_scan_result_flag - get scan result flag of station.
 *
 * return scan result flag of station.
 */
int get_scan_result_flag(void);

/**
 * set_delete_netconf_flag - set remove network flag of station.
 *
 * @flag: remove network flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_delete_netconf_flag(int flag);

/**
 * get_delete_netconf_flag - get remove network flag of station.
 *
 * return getting remove network flag of station.
 */
int get_delete_netconf_flag(void);

/**
 * set_send_command_flag - set send command flag of station.
 *
 * @flag: send command flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_send_command_flag(int flag);

/**
 * get_send_command_flag - get send command flag of station.
 *
 * return send command flag of station.
 */
int get_send_command_flag(void);

/**
 * init_sta_info - init station information.
 *
 * return 0 if success; -1 if failed; -2 if init operation has been done.
 */
int init_sta_info(void);

/**
 * uninit_sta_info - uninit station information.
 *
 * return 0 if success; -1 if failed; -2 if uninit operation has been done.
 */
int uninit_sta_info(void);

#if __cplusplus
};
#endif

#endif
