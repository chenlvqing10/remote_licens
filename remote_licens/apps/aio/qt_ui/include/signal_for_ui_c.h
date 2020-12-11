/*
* signal_for_ui_c.h - define interface function of signal by c
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


#ifndef API_SIGNAL_INC_SIGNAL_FOR_UI_C_H_
#define API_SIGNAL_INC_SIGNAL_FOR_UI_C_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define NET_EVENT_WIRED 0x01
#define NET_EVENT_WIFI 0x02

#define NET_TYPE_DISCONNECT 0x01
#define NET_TYPE_CONNECTED 0x02
#define NET_TYPE_SCAN_COMP 0x03
#define NET_TYPE_CLOSE 0x04
#define NET_TYPE_SCAN_FAIL 0x05
#define NET_TYPE_OPEN_SUCCESSFUL 0x06

#define WEB_SOCKET_EVENT_ENTER_PAGE 0x01
#define WEB_SOCKET_EVENT_EXIT_PAGE 0x02
#define WEB_SOCKET_EVENT_LOGIN 0x03
#define WEB_SOCKET_EVENT_QRCODE 0x04
/**/
#define WEB_SOCKET_EVENT_ACTIVE 0x05
#define WEB_SOCKET_EVENT_UNBIND 0x06
#define WEB_SOCKET_EVENT_LINKED 0x07
#define WEB_SOCKET_EVENT_DIS_LINKED 0x08
#define WEB_SOCKET_EVENT_BIND 0x09

#define WEB_SOCKET_EVENT_ALARMIN 0x0A
#define WEB_SOCKET_EVENT_FACE_INIT_OK 0x0B
#define WEB_SOCKET_EVENT_RECORD_EXPORT 0x0C

void send_signal_for_websocket(int event);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  //  API_SIGNAL_INC_SIGNAL_FOR_UI_C_H_
