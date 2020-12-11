/*
* signal_table.hpp - define signal type of project
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


#ifndef SIGNALTABLE_H
#define SIGNALTABLE_H

#include <QObject>


class SignalTable : public QObject {
  Q_OBJECT

 public:
  explicit SignalTable(QObject* parent = nullptr);
  void sendSignalForScreenSaver(int event);
  void sendSignalForWebSocket(int event);
  void sendSignalForBatchPercent(int value);
  void sendSignalForDraw();
  void sendSignalForLogin();
  void sendSignalForFactoryDefault();
  void sendSignalForUpdateFirmware();
  void sendSignalForStorage(int event, int type);
  void sendSignalForOutput(int event, int step);
  void sendSignalForRecgNotes(int ret, QString name, QString status);
  void sendSignalForQrcode(QString data);

 signals:
  void signalForScreenSaver(int event);
  /*1 enterPage 2 exitPage 3 login 4 generate login qrcode */
  void signalForWebSocket(int type);
  /* value :0-100;*/
  void signalForBatchPercent(int value);
  void signalForDraw();
  void signalForLogin();
  void signalForFactoryDefault();
  void signalForUpdateFirmware();
  /*event 1 u disk 2 sd  type 1 disconnect 2 connect*/
  void signalForStorage(int event, int type);
  /*event 0 idel 1 working 2 ok 3 faild  step*/
  void signalForOutput(int event, int step);
  void signalForRecgNotes(int ret, QString name, QString status);
  void signalForQrcode(QString date);

};

#endif  //  SIGNALTABLE_H
