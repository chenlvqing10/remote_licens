#ifndef __WIFI_MONITOR_H
#define __WIFI_MONITOR_H

#include "wifi_station.h"

#if __cplusplus
extern "C" {
#endif

/**
 * EVENT_INFO - system information related to the station event.
 * @handler: callback function.
 * @assoc_reject_count: count of associate reject.
 * @auth_fail_count: count of authenticate failed.
 * @event_fd: pipe handle.
 */
typedef struct {
	STA_EVENT_HANDLER handler;
	int assoc_reject_count;
	int auth_fail_count;
	int event_fd[2];
} EVENT_INFO;

/**
 * reset_abnormal_event_count - reset abnormal event count.
 */
void reset_abnormal_event_count(void);

/**
 * start_monitor - start event monitor thread.
 */
void start_monitor(void);

/**
 * register_event_handler - register event callback function.
 *
 * @handler: registered callback function.
 *
 * return 0 if success; otherwise failed.
 */
int register_event_handler(STA_EVENT_HANDLER handler);

/**
 * get_event_handler - get event callback function.
 *
 * @handler: get callback function.
 *
 */
STA_EVENT_HANDLER get_event_handler(void);

/**
 * unregister_event_handler - unregister event callback function.
 *
 * return 0 if success; otherwise failed.
 */
int unregister_event_handler(void);

/**
 * call_event_handler - call event callback function.
 *
 * @event: station event that need to be reported.
 *
 * return 0 if success; otherwise failed.
 */
int call_event_handler(STA_EVENT event);

/**
 * read_event - read station event.
 *
 * @event_ptr: buffer for storing station event.
 *
 * return 0 if success; otherwise failed.
 */
int read_event(int *event_ptr);

/**
 * write_event - transmit station event.
 *
 * @sta_event: station event that need to be reported.
 *
 * return 0 if success; otherwise failed.
 */
int write_event(int sta_event);

#if __cplusplus
};
#endif

#endif /*__WIFI_MONITOR_H*/
