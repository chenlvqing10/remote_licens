#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "debug.h"
#include "wifi_common.h"
#include "wifi_monitor.h"
#include "wifi_station.h"
#include "net_conf_parser.h"
#include "state.h"

static EVENT_INFO evt_info = {
	NULL,
	0,
	0,
	{-1, -1}
};

/**
 * handle_event - handle the events reported by wpa_supplicant.
 *
 * @evt: buffer for storing the parsed events.
 *
 * return -1 if wpa_supplicant is terminated; 0 otherwise.
 */
static int handle_event(const char *evt)
{
	int ret = 0;

	if (!strcmp(evt, "CONNECTED")) {
		logi("receive connected event from wpa_supplicant!\n");
		set_sta_state(STA_CONNECTED_STATE);

		if (get_connect_flag() == ON) {
			write_event(STA_CONNECTED);
			set_connect_flag(OFF);
		} else if (get_status_flag() == ON) {
			set_status_flag(OFF);
		} else {
			logi("auto connect successed!\n");
			reset_abnormal_event_count();
			call_event_handler(STA_CONNECTED);
		}
	} else if (!strcmp(evt, "DISCONNECTED")) {
		logi("receive disconnected event from wpa_supplicant!\n");
		/* send disconnect command */
		if (get_disconnect_flag() == ON) {
			logi("Network disconnected!\n");
			set_sta_state(STA_DISCONNECTED_STATE);
			set_disconnect_flag(OFF);
			write_event(STA_DISCONNECTED);
		}

		/* connection interrupted */
		if ((get_sta_state() == STA_CONNECTED_STATE) &&
			(get_connect_flag() == OFF)) {
			logi("connection is interrupted!\n");
			set_sta_state(STA_DISCONNECTED_STATE);
			call_event_handler(STA_DISCONNECTED);
		}
	} else if (!strcmp(evt, "SCAN-RESULTS")) {
		/* send scan command */
		if (get_scan_flag() == ON) {
			set_scan_flag(OFF);
			write_event(STA_SCAN_RESULTS);
		}
	} else if (!strcmp(evt, "TERMINATING")) {
		/* wpa_supplicant is exiting */
		loge("wpa_supplicant is exiting!\n");
		set_switch_status(STA_DISABLED);
		ret = -1;
	} else if (!strcmp(evt, "ASSOC-REJECT")) {
		if (get_connect_flag() == ON) {
			evt_info.assoc_reject_count++;
			logi("ASSOC REJECT times: %d!\n", evt_info.assoc_reject_count);
		}

		if (evt_info.assoc_reject_count >= 2) {
			set_sta_state(STA_DISCONNECTED_STATE);
			set_connect_event(CONNECT_REJECT);
		}
	} else {
		/* logi("unconcerned evt!\n"); */
	}

	return ret;
}

/*
 * parse_event - parse the events reported by wpa_supplicant.
 *
 * @evt_buf: buffer for storing information reported by wpa_supplicant.
 *
 * return -1 if wpa_supplicant is terminated; 0 otherwise.
 */
static int parse_event(const char *evt_buf)
{
	char evt[16] = {0};
	char *ptr_start = NULL, *ptr_end = NULL;
	int ret = 0, evt_len = 0;

	if (!evt_buf) {
		logw("event is NULL!\n");
		return 0;
	}

	/* check if the wifi password is incorrect */
	if (strstr(evt_buf, "pre-shared key may be incorrect")) {
		if (get_connect_flag() == ON) {
			evt_info.auth_fail_count++;
			logi("AUTH FAILED times: %d!\n", evt_info.auth_fail_count);
		}

		if (evt_info.auth_fail_count >= 1) {
			set_sta_state(STA_DISCONNECTED_STATE);
			set_connect_event(PASSWD_ERROR);
		}
		goto exit;
	}

	/* extract event information */
	evt_buf = strstr(evt_buf, "CTRL-EVENT-");
	if ((evt_buf != NULL)) {
		ptr_start = (char *)((unsigned long)evt_buf + 11);
		ptr_end = strchr(ptr_start, ' ');
		if (ptr_end != NULL) {
			evt_len = ptr_end - ptr_start;
			if (evt_len > 15)
				evt_len = 15;
			strncpy(evt, ptr_start, evt_len);
			evt[evt_len] = '\0';
		} else {
			logw("event is empty!\n");
			goto exit;
		}
	} else {
		goto exit;
	}

	ret = handle_event(evt);

exit:
	return ret;
}

/**
 * reset_abnormal_event_count - reset abnormal event count.
 */
void reset_abnormal_event_count(void)
{
	evt_info.assoc_reject_count = 0;
	evt_info.auth_fail_count = 0;
}

/**
 * evt_transmit_init - event transmit initialize.
 *
 * return 0 if success; otherwise failed.
 */
static int evt_transmit_init(void)
{
	if (pipe(evt_info.event_fd) < 0) {
		loge("pipe error!\n");
		return -1;
	}
	return 0;
}

/**
 * evt_transmit_init - event transmit uninitialize.
 *
 * return 0 if success; otherwise failed.
 */
static int evt_transmit_uninit(void)
{
	if (evt_info.event_fd[0] >= 0) {
		close(evt_info.event_fd[0]);
		evt_info.event_fd[0] = -1;
	}

	if (evt_info.event_fd[1] >= 0) {
		close(evt_info.event_fd[1]);
		evt_info.event_fd[1] = -1;
	}
	return 0;
}

/**
 * event_monitor_thread - thread function that monitor the events of
 * wpa_supplicant.
 *
 * @args: NULL.
 *
 * return NULL.
 */
static void *event_monitor_thread(void *args)
{
	char evt_buf[1024] = {0};

	/* detach from the main thread */
	pthread_detach(pthread_self());

	while (1) {
		if (wifi_wait_for_event(evt_buf, sizeof(evt_buf)) > 0) {
			if (parse_event(evt_buf) == -1)
				break;
		}
	}

	evt_transmit_uninit();
	pthread_exit(NULL);
}

/*
 * start_monitor - start event monitor thread.
 */
void start_monitor(void)
{
	pthread_t thread_id;

	evt_transmit_init();
	pthread_create(&thread_id, NULL, event_monitor_thread, NULL);
}

/*
 * register_event_handler - register event callback function.
 *
 * @handler: registered callback function.
 *
 * return 0 if success; otherwise failed.
 */
int register_event_handler(STA_EVENT_HANDLER handler)
{
	evt_info.handler = handler;
	return 0;
}


/*
 * get_event_handler - get event callback function.
 *
 * @handler: get callback function.
 *
 */
STA_EVENT_HANDLER get_event_handler(void)
{
	return evt_info.handler;
}


/*
* unregister_event_handler - unregister event callback function.
*
* return 0 if success; otherwise failed.
*/
int unregister_event_handler(void)
{
	evt_info.handler = NULL;
	return 0;
}

/*
 * call_event_handler - call event callback function.
 *
 * @event: station event that need to be reported.
 *
 * return 0 if success; otherwise failed.
 */
int call_event_handler(STA_EVENT event)
{
	evt_info.handler(event);
	return 0;
}

/*
 * read_event - read station event.
 *
 * @event_ptr: buffer for storing station event.
 *
 * return 0 if success; otherwise failed.
 */
int read_event(int *event_ptr)
{
	int n;

	n = read(evt_info.event_fd[0], event_ptr, sizeof(int));

	if (n < 0)
		return -1;
	else
		return 0;
}

/*
 * write_event - transmit station event.
 *
 * @sta_event: station event that need to be reported.
 *
 * return 0 if success; otherwise failed.
 */
int write_event(int sta_event)
{
	int n;

	n = write(evt_info.event_fd[1], &sta_event, sizeof(int));

	if (n < 0)
		return -1;
	else
		return 0;
}
