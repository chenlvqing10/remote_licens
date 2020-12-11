/*
* slot_table_test.hpp - define slot table test
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

#ifndef SLOT_TABLE_TEST_H
#define SLOT_TABLE_TEST_H

#include <QObject>

class SlotTableTest : public QObject {
  Q_OBJECT

 public:
  explicit SlotTableTest(QObject *parent = nullptr);

 signals:

 public slots:
  void slotForScreenSaver(int event);
  // 1 enterPage 2 exitPage 3 login 4 generate login qrcode
  void slotForWebSocket(int type);
  void slotForBatchPercent(int value); // 0-100;
  void slotForDraw(); // Draw
  void slotForLogin(); // face login
};

#endif  //  SLOT_TABLE_TEST_H