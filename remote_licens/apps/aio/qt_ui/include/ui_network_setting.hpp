/*
* ui_network_setting.hpp define network setting function of UI
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

#ifndef UI_NETWORK_SETTING_H
#define UI_NETWORK_SETTING_H

#include <QStandardItemModel>

#include "QtMainWindow.hpp"
#include "ui_network_setting_item_delegate.hpp"
#include "ui_setting_item_delegate.hpp"

namespace Ui {
class ui_network_setting;
}

class ui_network_setting : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_network_setting(QWidget *parent = 0);
  ~ui_network_setting();
  void ui_init();
  void test_network();
  void debug_test_network();

 protected slots:
  void slot_for_mainui(int event, int value);

 private slots:
  void slot_for_setting_network(const QModelIndex & index);

 private:
  Ui::ui_network_setting *ui;
  QStandardItemModel *pModel;
  QList<item_t> menu_network;
  ui_setting_item_delegate *pItemDelegate;
};

#endif  //  UI_NETWORK_SETTING_H
