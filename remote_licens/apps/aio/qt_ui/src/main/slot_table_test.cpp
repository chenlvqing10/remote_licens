/*
* slot_table_test.h - test signal table
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define LOG_TAG   "slot_test"
#include <log/log.h>
#include "ui_control.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>

#include "slot_table_test.hpp"
#include "signal_for_ui.hpp"

SlotTableTest::SlotTableTest(QObject *parent) : QObject(parent) {
}

/*1 net_wired 2 net_wifi  type 1 net_disconnectd 2 net_connected*/
void SlotTableTest::slotForScreenSaver(int event) {
  ALOGE("slotForScreenSaver %d\n", event);
}

void SlotTableTest::slotForWebSocket(int type) {
  ALOGE("slotForWebSocket type = %d\n", type);
}

/*1 enterPage 2 exitPage 3 login 4 generate login qrcode*/
void SlotTableTest::slotForBatchPercent(int value) {
}

/*0-100*/
void SlotTableTest::slotForDraw() {
}

/*  Draw */
void SlotTableTest::slotForLogin() {
}

