#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "state.h"

static STATION_INFO *sta_info;
static SWITCH_STATUS switch_status = STA_DISABLED;

/**
 * set_switch_status - set switch status of station.
 *
 * @status: enabled status.
 *
 * return 0 if success; otherwise failed.
 */
int set_switch_status(int status)
{
	switch_status = status;
	return 0;
}

/**
 * get_switch_status - get switch status of station.
 *
 * return switch status of station.
 */
int get_switch_status(void)
{
	return switch_status;
}

/**
 * is_wifi_enabled - get the enabled status of station.
 *
 * return 0 if station is enabled; -1 if station is disabled, or disabing,
 * or enabling.
 */
int is_wifi_enabled(void)
{
	if (switch_status == STA_ENABLED)
		return 0;
	else
		return -1;
}

/**
 * set_sta_state - set connection state of station.
 *
 * @state: connection state.
 *
 * return 0 if success; otherwise failed.
 */
int set_sta_state(int state)
{
	sta_info->sta_state = state;
	return 0;
}

/**
 * get_sta_state - get connection state of station.
 *
 * return conection state of station.
 */
int get_sta_state(void)
{
	return sta_info->sta_state;
}

/**
 * set_connect_event - set connection event of station.
 *
 * @event: connection event.
 *
 * return 0 if success; otherwise failed.
 */
int set_connect_event(int event)
{
	sta_info->connect_event = event;
	return 0;
}

/**
 * get_connect_event - get connection event of station.
 *
 * return connection event of station.
 */
int get_connect_event(void)
{
	return sta_info->connect_event;
}

/**
 * set_connect_flag - set connection flag of station.
 *
 * @flag: connection flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_connect_flag(int flag)
{
	sta_info->connect_flag = flag;
	return 0;
}

/**
 * get_connect_flag - get connection flag of station.
 *
 * return connection flag of station.
 */
int get_connect_flag(void)
{
	return sta_info->connect_flag;
}

/**
 * set_scan_flag - set scan flag of station.
 *
 * @flag: scan flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_scan_flag(int flag)
{
	sta_info->scan_flag = flag;
	return 0;
}

/**
 * get_scan_flag - get scan flag of station.
 *
 * return scan flag of station.
 */
int get_scan_flag(void)
{
	return sta_info->scan_flag;
}

/**
 * set_disconnect_flag - set disconnect flag of station.
 *
 * @flag: disconnect flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_disconnect_flag(int flag)
{
	sta_info->disconnect_flag = flag;
	return 0;
}

/**
 * get_disconnect_flag - get disconnect flag of station.
 *
 * return disconnect flag of station.
 */
int get_disconnect_flag(void)
{
	return sta_info->disconnect_flag;
}

/**
 * set_status_flag - set getting status flag of station.
 *
 * @flag: getting status flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_status_flag(int flag)
{
	sta_info->status_flag = flag;
	return 0;
}

/**
 * get_status_flag - get getting status flag of station.
 *
 * return getting status flag of station.
 */
int get_status_flag(void)
{
	return sta_info->status_flag;
}

/**
 * set_delete_netconf_flag - set remove network flag of station.
 *
 * @flag: remove network flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_delete_netconf_flag(int flag)
{
	sta_info->delete_netconf_flag = flag;
	return 0;
}

/**
 * get_delete_netconf_flag - get remove network flag of station.
 *
 * return remove network flag of station.
 */
int get_delete_netconf_flag(void)
{
	return sta_info->delete_netconf_flag;
}

/**
 * set_scan_result_flag - set scan result flag of station.
 *
 * @flag: scan result flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_scan_result_flag(int flag)
{
	sta_info->scan_result_flag = flag;
	return 0;
}

/**
 * get_scan_result_flag - get scan result flag of station.
 *
 * return scan result flag of station.
 */
int get_scan_result_flag(void)
{
	return sta_info->scan_result_flag;
}

/**
 * set_send_command_flag - set send command flag of station.
 *
 * @flag: send command flag.
 *
 * return 0 if success; otherwise failed.
 */
int set_send_command_flag(int flag)
{
	sta_info->send_command_flag = flag;
	return 0;
}

/**
 * get_send_command_flag - get send command flag of station.
 *
 * return send command flag of station.
 */
int get_send_command_flag(void)
{
	return sta_info->send_command_flag;
}

/**
 * init_sta_info - init station information.
 *
 * return 0 if success; -1 if failed; -2 if init operation has been done.
 */
int init_sta_info(void)
{
	if (sta_info != NULL)
		return -2;

	sta_info = (STATION_INFO *)malloc(sizeof(STATION_INFO));
	if (sta_info == NULL) {
		loge("malloc sta_info failed!\n");
		return -1;
	} else {
		memset(sta_info, 0, sizeof(STATION_INFO));
		return 0;
	}
}

/**
 * uninit_sta_info - uninit station information.
 *
 * return 0 if success; -1 if failed; -2 if uninit operation has been done.
 */
int uninit_sta_info(void)
{
	if (sta_info == NULL)
		return -1;

	free(sta_info);
	sta_info = NULL;
	return 0;
}
