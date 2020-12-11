/*
* signal_for_ui.cpp - define interface function of signal by cpp
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

#define LOG_TAG "signal_for_ui"
#include <log/log.h>

#include "signal_for_ui.hpp"
#include "signal_table.hpp"

static SignalTable* local_signal = NULL;

/**
 * @brief get signal handle
 * @note
 * @retval None
 */
void* signal_get_handle() {
  return local_signal;
}

/**
 * @brief signal init
 * @note
 * @retval None
 */
void signal_init() {
  local_signal = new SignalTable();
}

/**
 * @brief signal deinit
 * @note
 * @retval None
 */
void signal_deinit() {
  delete local_signal;
}

/**
 * @brief send signal of screen saver related
 * @note
 * @param  event:
 * @retval None
 */
void send_signal_for_screen_saver(int event) {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForScreenSaver(event);
}

/**
 * @brief send signal of draw face
 * @note
 * @retval None
 */
void send_signal_for_draw() {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForDraw();
}

/**
 * @brief send signal of qrcode notes
 * @note
 * @retval None
 */
void send_signal_for_qrcode(QString data)
{
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForQrcode(data);
}

/**
 * @brief send signal of recg notes
 * @note
 * @retval None
 */
void send_signal_for_recg_notes(int ret, QString name, QString status) {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForRecgNotes(ret, name, status);
}

/**
 * @brief send signal of login
 * @note
 * @retval None
 */
void send_signal_for_login() {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForLogin();
}

/**
 * @brief send signal of websocket related
 * @note
 * @param  event:
 * @retval None
 */
void send_signal_for_websocket(int event) {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForWebSocket(event);
}

/**
 * @brief send signal of batch percent
 * @note
 * @param  value:
 * @retval None
 */
void send_signal_for_batch_percent(int value) {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForBatchPercent(value);
}

/**
 * @brief send signal of factory default
 * @note
 * @retval None
 */
void send_signal_for_factory_default() {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForFactoryDefault();
}

/**
 * @brief send signal of update firmware
 * @note
 * @retval None
 */
void send_signal_for_update_firmware() {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForUpdateFirmware();
}

/**
 * @brief send signal of storage insert or pullout
 * @note
 * @param  event:
 * @param  type:
 * @retval None
 */
void send_signal_for_storage(int event, int type) {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForStorage(event, type);
}

/**
 * @brief send signal of user list output
 * @note
 * @param  event:
 * @param  type:
 * @retval None
 */
void send_signal_for_output(int event, int step) {
  if (NULL == local_signal) {
    ALOGD("local_signal is null\n");
    return;
  }
  local_signal->sendSignalForOutput(event, step);
}

