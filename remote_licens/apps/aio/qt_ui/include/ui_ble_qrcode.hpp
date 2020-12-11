/*
* ui_ble_qrcode.hpp - define ble qrcode of ui
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

#ifndef UI_BLE_QRCODE_H
#define UI_BLE_QRCODE_H

#include "QtMainWindow.hpp"

namespace Ui {
class ui_ble_qrcode;
}

class ui_ble_qrcode : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_ble_qrcode(QWidget *parent = 0);
  ~ui_ble_qrcode();

 private slots:
  void slot_for_show_ble_qrcode(int type);

 private:
  Ui::ui_ble_qrcode *ui;
};

#endif  //  UI_BLE_QRCODE_H
