/*
* ui_algorithm_setting.hpp - define algorithm_setting function of main
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

#ifndef UI_ALGORITHM_SETTING_H
#define UI_ALGORITHM_SETTING_H

#include <QModelIndex>
#include <QPushButton>
#include <QLineEdit>
#include <QStandardItemModel>

#include "QtMainWindow.hpp"
#include "ui_radio_dialog.hpp"
#include "ui_count_dialog.hpp"
#include "ui_checkbox_dialog.hpp"
#include "ui_setting_item_delegate.hpp"
#include "ui_rotate_management.hpp"

namespace Ui {
class ui_algorithm_setting;
}

class ui_algorithm_setting : public QtMainWindow,
    ui_radio_callback, ui_count_callback, ui_checkbox_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_algorithm_setting(QWidget *parent = 0);
  ~ui_algorithm_setting();
  void update_debug_item_info();
  int check_str_is_validate(const char *str);
  void radio_callback(void *id, int index, bool isCancel = false);
  void count_callback(void *id, int count);
  void checkbox_callback(void *id, QVector<int> counts);

 public slots:
  void deinit();

 private slots:
  void slot_for_algorithm_setting_item(const QModelIndex &index);

 private:
  void ui_init();
  void switch_clicked(int index);
  void update_listview(int index);

 private:
  Ui::ui_algorithm_setting *ui;
  QMap<int, QString> voice_mode_steps;
  QMap<int, QString> grettings_steps;
  QMap<int, QString> stranger_voice_steps;
  QList<item_t> menu_algorithm;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
  un_debug_switch_t debug_sw;
  algorithm_system_info_t alg_info;

 protected:
  void mouseReleaseEvent(QMouseEvent * event);
  time_t begin_time;
  int clicked_count;
};

#endif  //  UI_ALGORITHM_SETTING_H
