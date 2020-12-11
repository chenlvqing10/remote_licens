#ifndef __WPA_CONF_PARSER_H
#define __WPA_CONF_PARSER_H

#if __cplusplus
extern "C" {
#endif

#include "wifi_station.h"

/**
 * NET_CONF_OPERATION - operation of netconf.
 * @ENABLE: enable all netconf.
 * @DISABLE: disable all netconf.
 * @REMOVE: remove all netconf.
 * @OBTAIN_MAX_PRIO: obtain the highest priority.
 * @OBTAIN_SAVED_CONFIG_NUM: obtain the number of saved wifi configuration.
 * @OBTAIN_SAVED_CONFIG: obtain all saved wifi configuratio.
 */
typedef enum {
	ENABLE = 0,
	DISABLE,
	REMOVE,
	OBTAIN_MAX_PRIO,
	OBTAIN_SAVED_CONFIG_NUM,
	OBTAIN_SAVED_CONFIG,
} NET_CONF_OPERATION;

/**
 * enable_all_netconf - enable all netconf.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int enable_all_netconf(void);

/**
 * disable_all_netconf - disable all netconf.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int disable_all_netconf(void);

/**
 * remove_all_netconf - remove all netconf.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int remove_all_netconf(void);

/**
* obtain_max_priority - obtain the highest priority.
*
* @max_priority: buffer for storing the highest priority.
*
* return 0 if success; -1 if it is failed to obtain netconf;
* -2 if no netconf exists.
*/
int obtain_max_priority(int *max_priority);

/**
 * obtain_saved_config_num - obtain the number of saved wifi configuration.
 *
 * @saved_config_num: number of saved hotspot.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int obtain_saved_config_num(int *saved_config_num);

/**
 * obtain_saved_config - obtain all saved wifi configuration.
 *
 * @save_config: configuration of saved hotspot.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int obtain_saved_config(WIFI_CONFIG *saved_config);

/**
 * is_netconf_empty - check if netconf is empty.
 *
 * return 0 if netconf is empty; 1 if it is failed to obtain netconf.
 */
int is_netconf_empty(void);

/**
 * is_sta_connected - check if station is connected.
 *
 * return 0 if station is not connected; 1 if station is connected.
 */
int is_sta_connected(void);

/**
 * is_netconf_exist - check if the specified netconf exists.
 *
 * @ssid: ssid for the specified netconf.
 * @security: security for the specified netconf.
 * @netid: buffer for storing netid if the specified netconf exists.
 *
 * return 0 if the specified netconf does not exists;
 * 1 if the specified netconf exists;
 * -1 if it is failed to obtain netconf.
 */
int is_netconf_exist(const char *ssid, SECURITY security, char *netid);

/*
 * str_to_utf8 - convert string data to utf-8 format data.
 *
 * @buf: buffer for storing string data.
 * @dst: buffer for storing utf-8 format data.
 *
 * return 0 if success; otherwise failed.
 */
int str_to_utf8(const char *buf, char *dst);

#if __cplusplus
};
#endif

#endif /* __WPA_CONF_PARSER_H */
