/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _WIFI_COMMON_H
#define _WIFI_COMMON_H

#if __cplusplus
extern "C" {
#endif

/**
 * Start supplicant.
 *
 * @return 0 on success, < 0 on failure.
 */
int wifi_start_supplicant();

/**
 * Stop supplicant.
 *
 * @return 0 on success, < 0 on failure.
 */
int wifi_stop_supplicant();

/**
 * Open a connection to supplicant
 *
 * @return 0 on success, < 0 on failure.
 */
int wifi_connect_to_supplicant();

/**
 * Close connection to supplicant
 *
 * @return 0 on success, < 0 on failure.
 */
void wifi_close_supplicant_connection();

/**
 * wifi_wait_for_event() performs a blocking call to
 * get a Wi-Fi event and returns a string representing
 * a Wi-Fi event when it occurs.
 *
 * @param buf is the buffer that receives the event
 * @param len is the maximum length of the buffer
 *
 * @returns number of bytes in buffer, 0 if no
 * event (for instance, no connection), and less than 0
 * if there is an error.
 */
int wifi_wait_for_event(char *buf, size_t len);

/**
 * wifi_command() issues a command to the Wi-Fi driver.
 *
 * Android extends the standard commands listed at
 * /link http://hostap.epitest.fi/wpa_supplicant/devel/ctrl_iface_page.html
 * to include support for sending commands to the driver:
 *
 * See wifi/java/android/net/wifi/WifiNative.java for the details of
 * driver commands that are supported
 *
 * @param command is the string command (preallocated with 32 bytes)
 * @param commandlen is command buffer length
 * @param reply is a buffer to receive a reply string
 * @param reply_len on entry, this is the maximum length of
 *        the reply buffer. On exit, the number of
 *        bytes in the reply buffer.
 *
 * @return 0 if successful, < 0 if an error.
 */
int wifi_command(const char *command, char *reply, size_t reply_len);

int is_proc_run(const char *process);

/**
 * PATH_MAX
 */
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Evaluate EXPRESSION, and repeat as long as it returns -1 with `errno'
    set to EINTR.  */
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
	(__extension__ \
		({ long int __result; do __result = (long int) (expression); \
			while (__result == -1L && errno == EINTR) \
				; __result; }))
#endif

#if __cplusplus
};
#endif

#endif
