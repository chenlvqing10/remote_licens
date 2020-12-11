 /*
 * signal_for_ui.h - define interface function of signal operation
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


#ifndef API_SIGNAL_INC_SIGNAL_FOR_UI_H_
#define API_SIGNAL_INC_SIGNAL_FOR_UI_H_

#include "signal_for_ui_c.h"
#include <QObject>

#define STORAGE_EVENT_UDISK 0x01
#define STORAGE_EVENT_SDCARD 0x02
#define STORAGE_TYPE_DISCONN 0x01
#define STORAGE_TYPE_CONNECT 0x02

void *signal_get_handle();

void signal_init();

void signal_deinit();

void send_signal_for_net_status(int event, int type);

void send_signal_for_screen_saver(int event);

void send_signal_for_batch_percent(int value);

void send_signal_for_draw();

void send_signal_for_qrcode(QString data);

void send_signal_for_recg_notes(int ret, QString name, QString status);

void send_signal_for_login();

void send_signal_for_factory_default();

void send_signal_for_update_firmware();

/*event 1 u disk 2 sd  type 1 disconnect 2 connect*/
void send_signal_for_storage(int event, int type);
/*event 0 idel 1 working 2 ok 3 faild  step*/
void send_signal_for_output(int event, int step);

#endif  //  API_SIGNAL_INC_SIGNAL_FOR_UI_H_
