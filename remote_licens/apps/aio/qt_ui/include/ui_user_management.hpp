/*
* ui_user_management.h - define uesr management function of UI
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

#ifndef UI_USER_MANAGEMENT_H
#define UI_USER_MANAGEMENT_H

#include <QStandardItemModel>

#include "QtMainWindow.hpp"
#include "ui_setting_item_delegate.hpp"

namespace Ui {
class ui_user_management;
}

class ui_user_management : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_user_management(QWidget *parent = 0);
  ~ui_user_management();
  void ui_init();

 private slots:
  void slot_for_setting_item(const QModelIndex &index);

 private:
  Ui::ui_user_management *ui;
  QList<item_t> menu_user;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
  int mode;
};

#endif  //  UI_USER_MANAGEMENT_H
