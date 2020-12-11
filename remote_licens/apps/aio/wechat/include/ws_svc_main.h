/*
* ws_svc_main.h - define interface function of websocke server
*
* Copyright (C) 2019, LomboTech Co.Ltd.
* Author: lomboswer <lomboswer@lombotech.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef API_WEBSOCKET_INC_WS_SVC_MAIN_H_
#define API_WEBSOCKET_INC_WS_SVC_MAIN_H_

typedef enum {
	signal_enter_page = 0,
	signal_exit_page,
	signal_login,
	signal_qrcode,
	signal_active,
	signal_bind,
	signal_unbind,
	signal_link,
	signal_dislink,
	signal_health_status,
	signal_new_firmware,
	signal_firmware_success,
	signal_firmware_failed,
	signal_firmware_continue
} wechat_signal_t;


typedef void (*ws_svc_call_back)(wechat_signal_t signal);

void ws_svc_app_set_cb(ws_svc_call_back cb);

int ws_svc_app_wechat_is_link(void);

void ws_svc_app_net_reconnect(void);

int ws_svc_app_init(void);

int ws_svc_app_deinit(void);

int ws_svc_app_set_netlink(int link);

int ws_svc_app_qrcode(char *path);

int ws_app_checkin(void);

int ws_svc_app_qrcode_bind(char *path);

int ws_svc_app_ws_is_link(void);

/**
 * @brief  report temperature
 * @note   when recv ack callback  (signal_health_status)
 * @param  *user_id: empoyee id
 * @param  user_type: 0 empoyee(teacher) 2 student
 * @param  tmperature: 36.7
 * @param  status: 0 normal 1 high 2 low
 * @retval
 */
int ws_app_report_user_temperature(
	char *user_id,
	int user_type,
	float tmperature,
	int status);
/**
 * @brief get body status
 * @note  get body status when after report temperature
 * @param status int 0 normal 1 high 2 low
 * @retval
 */
void ws_app_get_body_status(int *status);
/**
 *
 *
 * @author
 *
 * @param void
 */
void ws_app_start_download_firmware(void);
/**
 *
 *
 * @author
 *
 * @param version
 */
void ws_app_get_firmware_version(char *version);

/**
 *
 *
 * @author
 *
 * @param void
 */
void ws_app_start_download_firmware(void);
/**
 *
 *
 * @author
 *
 * @param version
 */
void ws_app_get_firmware_version(char *version);

/**
 *
 *
 * @brief
 *
 * @param errmsg
 * @param errcode
 *
 * @return int
 */
int ws_app_error_report(char *errmsg, int errcode);
/**
 *
 *
 * @author
 *
 * @param file  "/data/log/system.log"
 *
 * @return int 0 success
 */
int ws_app_upload_log(char *file);

#endif /* API_WEBSOCKET_INC_WS_SVC_MAIN_H_ */
