/*
* signal_table.cpp - define signal type of project
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

#include <QDebug>
#include "signal_table.hpp"
/**
 * @brief construct function
 * @note
 * @param  *parent:
 * @retval
 */
SignalTable::SignalTable(QObject *parent) : QObject(parent) {
}

/**
 * @brief sendSignalForScreenSaver
 * @note
 * @param  event:
 * @retval None
 */
void SignalTable::sendSignalForScreenSaver(int event) {
  emit signalForScreenSaver(event);
}

/**
 * @brief sendSignalForDraw
 * @note
 * @retval None
 */
void SignalTable::sendSignalForDraw() {
  emit signalForDraw();
}

/**
 * @brief sendSignalForWebSocket
 * @note
 * @param  event:
 * @retval None
 */
void SignalTable::sendSignalForWebSocket(int event) {
  emit signalForWebSocket(event);
}

/**
 * @brief sendSignalForBatchPercent
 * @note
 * @param  value:
 * @retval None
 */
void SignalTable::sendSignalForBatchPercent(int value) {
  emit signalForBatchPercent(value);
}

/**
 * @brief sendSignalForQrcode
 * @note
 * @retval None
 */
void SignalTable::sendSignalForQrcode(QString data) {
  emit signalForQrcode(data);
}

/**
 * @brief sendSignalForRecgNotes
 * @note
 * @retval None
 */
void SignalTable::sendSignalForRecgNotes(int ret, QString name, QString status) {
  emit signalForRecgNotes(ret, name, status);
}

/**
 * @brief sendSignalForLogin
 * @note
 * @retval None
 */
void SignalTable::sendSignalForLogin() {
  emit signalForLogin();
}

/**
 * @brief sendSignalForFactoryDefault
 * @note
 * @retval None
 */
void SignalTable::sendSignalForFactoryDefault() {
  emit signalForFactoryDefault();
}

/**
 * @brief sendSignalForUpdateFirmware
 * @note
 * @retval None
 */
void SignalTable::sendSignalForUpdateFirmware() {
  emit signalForUpdateFirmware();
}

/**
 * @brief sendSignalForStorage
 * @note
 * @param  event:
 * @param  type:
 * @retval None
 */
void SignalTable::sendSignalForStorage(int event, int type) {
  emit signalForStorage(event, type);
}

/**
 * @brief sendSignalForOutput
 * @note
 * @param  event:
 * @param  step:
 * @retval None
 */
void SignalTable::sendSignalForOutput(int event, int step) {
  emit signalForOutput(event, step);
}

