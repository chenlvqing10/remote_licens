/*
* ui_eth_setting.h - define eth setting function of UI
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

#ifndef UI_ETH_SETTING_H
#define UI_ETH_SETTING_H

#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

#include "QtMainWindow.hpp"
#include "ui_eth_setting_delegate.hpp"
#include "eth_management.hpp"

#define EDIT_ITEM_COUNT 4

namespace Ui {
class ui_eth_setting;
}

class ui_eth_setting : public QtMainWindow {
  Q_OBJECT

 public:
  void get_eth_param();
  int set_eth_param();
  Q_INVOKABLE explicit ui_eth_setting(QWidget *parent = 0);
  ~ui_eth_setting();
  void update_eth_show();

 private slots:
  void slot_for_dhcp_enable();
  void slot_for_save_btn();
  void slot_for_eth_state(int state);

 private:
  Ui::ui_eth_setting *ui;
  bool dhcp_state;
  QLineEdit *net_edt[EDIT_ITEM_COUNT];
  QPushButton *dhcp_btn;
  QStandardItemModel *pModel;
  ui_eth_setting_delegate *pItemDelegate;
  eth_management *mgr;
  net_config_para_t config;
};

#endif  //  UI_ETH_SETTING_H
