/*
* ui_attendance_setting.hpp define attendance setting function of UI
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

#ifndef UI_ATTENDANCE_SETTING_H
#define UI_ATTENDANCE_SETTING_H

#include <QStandardItemModel>
#include <map>
#include <QSignalMapper>

#include "QtMainWindow.hpp"
#include "db_all.h"
#include "ui_radio_dialog.hpp"
#include "ui_setting_item_delegate.hpp"

namespace Ui {
class ui_attendance_setting;
}

typedef struct {
  QString key;
  int value;
}step_t;

class ui_attendance_setting : public QtMainWindow,
    ui_radio_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_attendance_setting(QWidget *parent = 0);
  ~ui_attendance_setting();
  void radio_callback(void *id, int index, bool isCancel = false);
  void ui_init();

 private slots:
  void slot_for_attendance_setting_item(const QModelIndex &index);

 private:
  QList<step_t> avoid_steps;
  QList<item_t> menu_attendance;
  QMap<int, QString> set_access_records;
  Ui::ui_attendance_setting *ui;
  attendance_param_t attendance_info;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
};

#endif  //  UI_ATTENDANCE_SETTING_H