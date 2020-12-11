/* inet_monitor.h */

#ifndef __INET_MONITOR_H__
#define __INET_MONITOR_H__

/**
 * enum inet_connect_sta - internet connect status.
 * @INET_STA_UNKNOWN: unknown.
 * @INET_STA_DISCONNECT: internet is disconnected.
 * @INET_STA_CONNECT: internet is connected.
 */
typedef enum inet_connect_sta {
	INET_STA_UNKNOWN = 0,
	INET_STA_DISCONNECT = 1,
	INET_STA_CONNECT = 2,
} inet_connect_sta_t;

/**
 * inet_connect_handler - internet connect status callback handler.
 * @status: internet connect status.
 */
typedef void (*inet_connect_handler)(inet_connect_sta_t status);

/**
 * net_is_connect - check if destination is reachable.
 * @dev_name: net device's name, can be null.
 * @dst: destination.
 *
 * The parameter dev_name can be set to NULL.
 * In case that value of dev_name equals NULL,
 * return value 0 means destination is unreachable by all net device,
 * return value 1 means destination is reachable by at least one net device.
 *
 * Destination can take the form of IP Address such as: 192.168.1.1,
 * or take the form of URL such as: www.baidu.com .
 *
 * return 0 if destination is unreachable, 1 if reachable.
 */
int net_is_connect(const char *dev_name, const char *dst);

/**
 * inet_is_connect - check if internet is reachable.
 * @dev_name: net device's name, can be null.
 *
 * The parameter dev_name can be set to NULL.
 * In case that value of dev_name equals NULL,
 * return value 0 means internet is unreachable by all net device,
 * return value 1 means internet is reachable by at least one net device.
 *
 * return 0 if internet is unreachable, 1 if is reachable.
 */
int inet_is_connect(const char *dev_name);

/**
 * inet_monitor_start - start internet connect monitor.
 * @dev_name: net device's name.
 * @handler: callback function which will be called when connect status change.
 *
 * The parameter dev_name can be set to NULL.
 * In case that value of dev_name equals NULL, an internet connect monitor
 * for all net device will be started.
 * It means that the monitor will monitor the internet connect status of
 * the whole system.
 *
 * return 0 if success; otherwise failed.
 */
int inet_monitor_start(const char *dev_name, inet_connect_handler handler);

/**
 * inet_monitor_stop - stop internet connect monitor.
 * @dev_name: net device's name.
 *
 * return 0 if success; otherwise failed.
 */
int inet_monitor_stop(const char *dev_name);

/**
 * inet_monitor_get_status - get current status from internet connect monitor.
 * @dev_name: net device's name.
 *
 * This function should be called only after internet connect monitor
 * for target device start.
 *
 * return current internet connect status or error number.
 */
int inet_monitor_get_status(const char *dev_name);

#endif /* __INET_MONITOR_H__ */
