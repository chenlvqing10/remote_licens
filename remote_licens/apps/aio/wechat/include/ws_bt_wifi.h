/*
 * net_wired.h - export  interface function of wifi config by ble
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
#ifndef _WS_BT_WIFI_H
#define _WS_BT_WIFI_H
#include "ip_config.h"

typedef enum {
	bt_wifi_connected = 0,
	bt_wifi_disconnect
} bt_wifi_event;

typedef void (*bt_wifi_event_cb)(bt_wifi_event event);

/**
 * @brief  set callback interface
 * @note   NULL
 * @param  func:  callback function
 * @retval None
 */
void bt_set_cb(bt_wifi_event_cb func);

/**
 * @brief  get ip info
 * @note   NULL
 * @param  *ip_config:
 * @retval 0 success other failed
 */
int bt_get_ip_info(ip_static_config_t *ip_config);

/**
 * @brief  init config wifi by bluetooth funciton
 * @note   support wechat
 * @retval 0 success other failed
 */
int bt_init();

/**
 * @brief  deinit config wifi by bluetooth fuction
 * @note   NULL
 * @retval None
 */
void bt_deinit();

/**
 * @brief  close wifi
 * @note   NULL
 * @retval None
 */
void bt_wifi_close();

/**
 * @brief  open wifi
 * @note   NULL
 * @retval None
 */
void bt_wifi_open();

/**
 * @brief  check wifi link status
 * @note   NULL
 * @retval 0 not link  1 linked
 */
int bt_wifi_is_link();
#endif
