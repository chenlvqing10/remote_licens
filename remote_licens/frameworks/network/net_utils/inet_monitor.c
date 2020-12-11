/* inet_monitor.c */

#include <sys/time.h>
#include <sys/types.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

#define  LOG_TAG		"net_utils"
#include <log/log.h>

#include "list.h"
#include "inet_monitor.h"

#define INET_STA_CHANGE_CNT	1

#define NET_DBG(fmt, ...)	ALOGD("[%s %d] "fmt, __func__, \
				      __LINE__, ##__VA_ARGS__)
#define NET_INF(fmt, ...)	ALOGI("[%s %d] "fmt, __func__, \
				      __LINE__, ##__VA_ARGS__)
#define NET_WRN(fmt, ...)	ALOGW("[%s %d] "fmt, __func__, \
				      __LINE__, ##__VA_ARGS__)
#define NET_ERR(fmt, ...)	ALOGE("[%s %d] "fmt, __func__, \
				      __LINE__, ##__VA_ARGS__)

/**
 * struct inet_monitor_unit - internet connect monitor unit struct.
 * @list: list node.
 * @dev_name: net device's name.
 * @status: current internet connect status.
 * @callback: callback function which will be called when connect status change.
 * @thread_id: id of internet connect monitor handle thread.
 */
typedef struct inet_monitor_unit {
	struct list_entry	list;
	char			*dev_name;
	inet_connect_sta_t	status;
	inet_connect_handler	callback;
	pthread_t		thread_id;
} inet_monitor_unit_t;

static struct list_entry g_inet_monitor_list = {&g_inet_monitor_list,
						&g_inet_monitor_list};
static pthread_mutex_t g_inet_monitor_lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * inet_monitor_handle_thread - internet connect monitor handle thread.
 * @arg: pointer to internet connect monitor unit struct.
 *
 * return null.
 */
static void *inet_monitor_handle_thread(void *arg)
{
	inet_monitor_unit_t *unit = (inet_monitor_unit_t *)arg;
	struct timeval tv1, tv2;
	int cur_sta, last_sta = -1, count = 0;
	unsigned long long consume_us;

	NET_DBG("inet monitor handle thread for %s enter...",
		unit->dev_name ? unit->dev_name : "system");

	while (1) {
		gettimeofday(&tv1, NULL);

		cur_sta = inet_is_connect(unit->dev_name);
		if (cur_sta != last_sta) {
			count = 0;
			last_sta = cur_sta;
		}

		pthread_testcancel();

		if (count < INET_STA_CHANGE_CNT) {
			count++;
			if (INET_STA_CHANGE_CNT == count) {
				unit->status = cur_sta ? INET_STA_CONNECT :
					       INET_STA_DISCONNECT;
				if (unit->callback)
					unit->callback(unit->status);
			}
		}

		gettimeofday(&tv2, NULL);

		consume_us = tv2.tv_sec * 1000000 + tv2.tv_usec -
			     (tv1.tv_sec * 1000000 + tv1.tv_usec);

		pthread_testcancel();

		if (consume_us < 1200000)
			usleep(1200000 - consume_us);

		pthread_testcancel();
	}

	return NULL;
}

/**
 * __inet_monitor_get_unit - get internet connect monitor unit by device's name.
 * @dev_name: net device's name.
 *
 * return internet connect monitor unit or null if target unit not exist.
 */
static inet_monitor_unit_t *__inet_monitor_get_unit(const char *dev_name)
{
	inet_monitor_unit_t *unit;

	list_for_each_entry(unit, &g_inet_monitor_list, list) {
		if ((NULL == dev_name) || (NULL == unit->dev_name)) {
			if (dev_name == unit->dev_name)
				return unit;
		} else {
			if (!strcmp(unit->dev_name, dev_name))
				return unit;
		}
	}

	return NULL;
}

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
int net_is_connect(const char *dev_name, const char *dst)
{
	FILE *stream;
	unsigned int cnt = 1;
	char cmd_buf[256] = {0};
	char recv_buf[32] = {0};

	if (!dst) {
		NET_ERR("invalid destination: null");
		return 0;
	}

	memset(cmd_buf, 0, sizeof(cmd_buf));
	memset(recv_buf, 0, sizeof(recv_buf));

	if (dev_name)
		sprintf(cmd_buf,
			"ping %s -I%s -c%d -w%d -4 | grep time= | wc -l",
			dst, dev_name, cnt, cnt + 1);
	else
		sprintf(cmd_buf,
			"ping %s -c%d -w%d -4 | grep time= | wc -l",
			dst, cnt, cnt + 1);

	stream = popen(cmd_buf, "r");
	if (stream) {
		fread(recv_buf, sizeof(char), sizeof(recv_buf) - 1, stream);
		pclose(stream);

		if (atoi(recv_buf) > 0)
			return 1;
	}

	return 0;
}

/**
 * inet_is_connect - check if internet is connected.
 * @dev_name: net device's name, can be null.
 *
 * The parameter dev_name can be set to NULL.
 * In case that value of dev_name equals NULL,
 * return value 0 means internet is unreachable by all net device,
 * return value 1 means internet is reachable by at least one net device.
 *
 * return 0 if internet is disconnected, 1 if is connected.
 */
int inet_is_connect(const char *dev_name)
{
	return net_is_connect(dev_name, "114.114.114.114");
}

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
int inet_monitor_start(const char *dev_name, inet_connect_handler handler)
{
	inet_monitor_unit_t *unit;

	pthread_mutex_lock(&g_inet_monitor_lock);
	if (__inet_monitor_get_unit(dev_name)) {
		NET_WRN("inet monitor for %s has been started",
			dev_name ? dev_name : "system");
		return -EBUSY;
	}
	pthread_mutex_unlock(&g_inet_monitor_lock);

	NET_DBG("inet monitor for %s start", dev_name ? dev_name : "system");

	unit = malloc(sizeof(*unit));
	if (!unit) {
		NET_ERR("failed to alloc inet monitor unit for %s",
			dev_name ? dev_name : "system");
		return -ENOMEM;
	}

	memset(unit, 0, sizeof(*unit));

	if (dev_name) {
		unit->dev_name = malloc(IFNAMSIZ);
		if (!unit->dev_name) {
			NET_ERR("failed to alloc inet monitor unit's name for %s",
				dev_name ? dev_name : "system");
			return -ENOMEM;
		}
		memset(unit->dev_name, 0, IFNAMSIZ);
		strncpy(unit->dev_name, dev_name, IFNAMSIZ);
		unit->dev_name[IFNAMSIZ - 1] = 0;
	}

	unit->status = INET_STA_UNKNOWN;
	unit->callback = handler;
	pthread_create(&unit->thread_id, NULL,
		       &inet_monitor_handle_thread, unit);

	pthread_mutex_lock(&g_inet_monitor_lock);
	list_append(&unit->list, &g_inet_monitor_list);
	pthread_mutex_unlock(&g_inet_monitor_lock);

	return 0;
}

/**
 * inet_monitor_stop - stop internet connect monitor.
 * @dev_name: net device's name.
 *
 * return 0 if success; otherwise failed.
 */
int inet_monitor_stop(const char *dev_name)
{
	inet_monitor_unit_t *unit = NULL;

	NET_DBG("inet monitor for %s stop", dev_name ? dev_name : "system");

	pthread_mutex_lock(&g_inet_monitor_lock);

	unit = __inet_monitor_get_unit(dev_name);
	if (!unit) {
		NET_WRN("inet monitor for %s hasn't been started",
			dev_name ? dev_name : "system");

		pthread_mutex_unlock(&g_inet_monitor_lock);
		return -ENODEV;
	}

	list_remove(&unit->list);

	pthread_mutex_unlock(&g_inet_monitor_lock);

	pthread_cancel(unit->thread_id);
	pthread_join(unit->thread_id, NULL);

	if (unit->dev_name)
		free(unit->dev_name);
	free(unit);

	return 0;
}

/**
 * inet_monitor_get_status - get current status from internet connect monitor.
 * @dev_name: net device's name.
 *
 * This function should be called only after internet connect monitor
 * for target device start.
 *
 * return current internet connect status or error number.
 */
int inet_monitor_get_status(const char *dev_name)
{
	inet_monitor_unit_t *unit = NULL;
	inet_connect_sta_t status;

	pthread_mutex_lock(&g_inet_monitor_lock);

	unit = __inet_monitor_get_unit(dev_name);
	if (!unit) {
		NET_ERR("inet monitor for %s hasn't been started",
			dev_name ? dev_name : "system");

		pthread_mutex_unlock(&g_inet_monitor_lock);
		return -ENODEV;
	}

	status = unit->status;

	pthread_mutex_unlock(&g_inet_monitor_lock);

	return status;
}
