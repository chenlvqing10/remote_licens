/*
* ui_access_setting.hpp define access setting function of UI
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

#ifndef UI_ACCESS_SETTING_H
#define UI_ACCESS_SETTING_H

#include <QStandardItemModel>
#include <QSignalMapper>
#include <QMap>

#include "QtMainWindow.hpp"
#include "ui_rotate_management.hpp"
#include "ui_radio_dialog.hpp"
#include "ui_count_dialog.hpp"
#include "ui_setting_item_delegate.hpp"

namespace Ui {
class ui_access_setting;
}

class ui_access_setting : public QtMainWindow,
    ui_radio_callback, ui_count_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_access_setting(QWidget *parent = 0);
  ~ui_access_setting();

  void radio_callback(void *id, int index, bool isCancel = false);
  void count_callback(void *id, int count);

 private slots:
  void slot_for_access_setting_item(const QModelIndex &index);

 private:
  void wg_format_radio_callback(int index);
  void open_door_delay_radio_callback(int index);
  void door_sensor_type_radio_callback(int index);
  void access_type_radio_callback(int index);
  void ui_init();

 private:
  QList<item_t> menu_access;
  QMap<int, QString> wiegand_format_steps;
  QMap<int, QString> door_open_delay_steps;
  QMap<int, QString> door_sensor_type_steps;
  QMap<int, QString> access_type;

  Ui::ui_access_setting *ui;
  access_param_t access_info;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
};

#endif  //  UI_ACCESS_SETTING_H
