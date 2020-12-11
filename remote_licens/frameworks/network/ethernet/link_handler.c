/* link_handler.c */

#include <features.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#define  LOG_TAG		"link_handler"
#include <log/log.h>

#include "link_handler.h"
#include "if_req.h"

#define DEF_ETH_DEV_NAME	"eth0"

typedef CIRCLEQ_HEAD(eth_link_handler_head, eth_link_handler_node)
	eth_link_handler_head_t;

typedef struct eth_link_handler_node {
	CIRCLEQ_ENTRY(eth_link_handler_node) handler_list;
	eth_dev_link_handler handler;
} eth_link_handler_node_t;

static eth_link_handler_head_t g_link_handler_list =
			       CIRCLEQ_HEAD_INITIALIZER(g_link_handler_list);
static pthread_mutex_t g_link_handler_list_lock = PTHREAD_MUTEX_INITIALIZER;
static eth_dev_link_status_t g_last_link_status = ETH_DEV_LINK_UNKNOW;

static pthread_t g_link_handle_thread_id;
static int g_link_handle_thread_exit_flag = 1;
static pthread_mutex_t g_link_handle_thread_exit_lock
		       = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_link_handle_thread_exit_cond
		       = PTHREAD_COND_INITIALIZER;

/**
 * eth_link_status_handle - ethernet device link status handle.
 */
static void eth_link_status_handle(void)
{
	eth_dev_link_status_t link_status;
	eth_link_handler_node_t *handler;

	/* only support net device "eth0" link status handle */
	link_status = eth_dev_link_is_up(DEF_ETH_DEV_NAME);
	if (link_status < 0)
		return;

	if (link_status != g_last_link_status) {
		pthread_mutex_lock(&g_link_handler_list_lock);
		CIRCLEQ_FOREACH(handler, &g_link_handler_list, handler_list)
				handler->handler(link_status);
		pthread_mutex_unlock(&g_link_handler_list_lock);

		g_last_link_status = link_status;
	}
}

/**
 * eth_link_handle_thread - ethernet device link status handle thread function.
 * @args: NULL.
 *
 * return NULL.
 */
static void *eth_link_handle_thread(void *args)
{
	int exit_flag = 0;
	struct timespec timeout;

	while (1) {
		eth_link_status_handle();

		/* use 50ms as the period */
		clock_gettime(CLOCK_MONOTONIC, &timeout);
		timeout.tv_nsec += (500 * 1000 * 1000UL);
		timeout.tv_sec += (timeout.tv_nsec / (1000 * 1000 * 1000UL));
		timeout.tv_nsec %= (1000 * 1000 * 1000UL);

		pthread_mutex_lock(&g_link_handle_thread_exit_lock);
		if (0 == g_link_handle_thread_exit_flag)
			pthread_cond_timedwait(&g_link_handle_thread_exit_cond,
					       &g_link_handle_thread_exit_lock,
					       &timeout);
		exit_flag = g_link_handle_thread_exit_flag;
		pthread_mutex_unlock(&g_link_handle_thread_exit_lock);

		if (exit_flag)
			goto thread_exit;
	}

thread_exit:
	pthread_exit(NULL);
}

/**
 * eth_dev_link_is_up - check if ethernet device is link up.
 * @dev_name: device name.
 *
 * return 0 if device is link down; 1 if device is link up;
 * otherwise some error happened.
 */
int eth_dev_link_is_up(const char *dev_name)
{
	int ret;
	unsigned int flags;

	ret = ifreq_get_flags(dev_name, &flags);
	if (ret) {
		ALOGE("failed to check if ethernet device is link up");
		return -1;
	}

	return (flags & IFF_RUNNING) ? 1 : 0;
}

/**
 * eth_dev_start_link_detect - start link status detect process.
 * @dev_name: device name.
 *
 * This function should be called before you first call any function to
 * add, delete or clear link status handler.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_start_link_detect(const char *dev_name)
{
	pthread_condattr_t g_link_handle_thread_exit_condattr;

	g_last_link_status = ETH_DEV_LINK_UNKNOW;
	pthread_mutex_init(&g_link_handler_list_lock, NULL);
	CIRCLEQ_INIT(&g_link_handler_list);

	g_link_handle_thread_exit_flag = 0;
	pthread_mutex_init(&g_link_handle_thread_exit_lock, NULL);
	pthread_condattr_init(&g_link_handle_thread_exit_condattr);
	pthread_condattr_setclock(&g_link_handle_thread_exit_condattr,
				  CLOCK_MONOTONIC);
	pthread_cond_init(&g_link_handle_thread_exit_cond,
			  &g_link_handle_thread_exit_condattr);
	pthread_create(&g_link_handle_thread_id, NULL,
		       &eth_link_handle_thread, NULL);

	return 0;
}

/**
 * eth_dev_stop_link_detect - stop link status detect process.
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_stop_link_detect(const char *dev_name)
{
	pthread_mutex_lock(&g_link_handle_thread_exit_lock);
	g_link_handle_thread_exit_flag = 1;
	pthread_cond_signal(&g_link_handle_thread_exit_cond);
	pthread_mutex_unlock(&g_link_handle_thread_exit_lock);

	pthread_join(g_link_handle_thread_id, NULL);
	g_link_handle_thread_id = 0;

	eth_dev_clr_link_handler(DEF_ETH_DEV_NAME);

	return 0;
}

/**
 * eth_dev_add_link_handler - add link status callback handler.
 * @dev_name: dev_name.
 * @handler: callback handler.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_add_link_handler(const char *dev_name,
	eth_dev_link_handler handler)
{
	eth_link_handler_node_t *node;

	if (strcmp(dev_name, DEF_ETH_DEV_NAME)) {
		ALOGE("unsupported add link handler for device %s", dev_name);
		return -1;
	}

	if (!handler) {
		ALOGE("invalid link handler");
		return -1;
	}

	pthread_mutex_lock(&g_link_handle_thread_exit_lock);
	if (g_link_handle_thread_exit_flag) {
		ALOGE("link handler can not be added while handle thread exit");
		pthread_mutex_unlock(&g_link_handle_thread_exit_lock);
		return -1;
	}
	pthread_mutex_unlock(&g_link_handle_thread_exit_lock);

	pthread_mutex_lock(&g_link_handler_list_lock);
	CIRCLEQ_FOREACH(node, &g_link_handler_list, handler_list) {
		if (node->handler == handler) {
			ALOGW("link handler has been added");
			pthread_mutex_unlock(&g_link_handler_list_lock);
			return 0;
		}
	}

	node = malloc(sizeof(eth_link_handler_node_t));
	node->handler = handler;

	CIRCLEQ_INSERT_TAIL(&g_link_handler_list, node, handler_list);
	pthread_mutex_unlock(&g_link_handler_list_lock);

	return 0;
}

/**
 * eth_dev_del_link_handler - delete link status callback handler.
 * @dev_name: dev_name.
 * @handler: callback handler.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_del_link_handler(const char *dev_name,
	eth_dev_link_handler handler)
{
	eth_link_handler_node_t *node;

	if (strcmp(dev_name, DEF_ETH_DEV_NAME)) {
		ALOGE("unsupported delete link handler for device %s",
		      dev_name);
		return -1;
	}

	if (!handler) {
		ALOGE("invalid link handler");
		return -1;
	}

	pthread_mutex_lock(&g_link_handler_list_lock);
	CIRCLEQ_FOREACH(node, &g_link_handler_list, handler_list) {
		if (node->handler == handler) {
			CIRCLEQ_REMOVE(&g_link_handler_list, node,
				       handler_list);
			free(node);
			break;
		}
	}
	pthread_mutex_unlock(&g_link_handler_list_lock);

	return 0;
}

/**
 * eth_dev_clr_link_handler - delete all link status callback handler.
 * @dev_name: dev_name.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_clr_link_handler(const char *dev_name)
{
	eth_link_handler_node_t *node;

	if (strcmp(dev_name, DEF_ETH_DEV_NAME)) {
		ALOGE("unsupported clear link handler for device %s",
		      dev_name);
		return -1;
	}

	pthread_mutex_lock(&g_link_handler_list_lock);
	while (!CIRCLEQ_EMPTY(&g_link_handler_list)) {
		node = CIRCLEQ_FIRST(&g_link_handler_list);
		CIRCLEQ_REMOVE(&g_link_handler_list, node, handler_list);
		free(node);
	}
	pthread_mutex_unlock(&g_link_handler_list_lock);

	return 0;
}
