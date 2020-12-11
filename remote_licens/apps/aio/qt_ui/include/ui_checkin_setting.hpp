/*
* ui_checkin_setting.hpp define checkin setting function of UI
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

#ifndef UI_CHECKIN_SETTING_H
#define UI_CHECKIN_SETTING_H

#include <QStandardItemModel>
#include <map>
#include <QSignalMapper>

#include "QtMainWindow.hpp"
#include "db_all.h"
#include "ui_checkbox_dialog.hpp"
#include "ui_setting_item_delegate.hpp"

namespace Ui {
class ui_checkin_setting;
}

class ui_checkin_setting : public QtMainWindow,
    ui_checkbox_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_checkin_setting(QWidget *parent = 0);
  ~ui_checkin_setting();
  void checkbox_callback(void *id, QVector<int> indexs);
  void ui_init();

 private slots:
  void slot_for_checkin_setting_item(const QModelIndex &index);

 private:
  QList<item_t> menu_checkin;
  QMap<int, QString> set_access_records;
  Ui::ui_checkin_setting *ui;
  checkin_param_t checkin_info;
  int checkin_option[3];
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
};

#endif  //  UI_CHECKIN_SETTING_H
