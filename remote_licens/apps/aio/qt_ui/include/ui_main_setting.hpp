/*
* ui_main_setting.hpp - define face preview setting of project
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

#ifndef UI_MAIN_SETTING_H
#define UI_MAIN_SETTING_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "ui_control.h"
#define LOG_TAG     "ui_main_setting"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QModelIndex>
#include <QStandardItemModel>
#include <QMap>
#include <QLineEdit>

#include "QtMainWindow.hpp"
#include "ui_setting_item_delegate.hpp"
#include "ui_radio_dialog.hpp"
#include "ui_input_dialog.hpp"
#include "ui_tip_dialog.hpp"

namespace Ui {
class ui_main_setting;
}

class ui_main_setting : public QtMainWindow,
ui_radio_callback, ui_input_callback, ui_tip_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_main_setting(QWidget *parent = 0);
  ~ui_main_setting();
  void radio_callback(void *id, int index, bool isCancel = false);
  void input_callback(void *id, QString text);
  int tip_callback(tip_args_t *args, bool isCancle);

 public slots:
  void deinit();

 private slots:
  void slot_for_item_clicked(const QModelIndex &index);

 private:
  void ui_init();
  void switch_clicked(int index);
  void update_listview(int row);

 private:
  Ui::ui_main_setting* ui;
  QMap<int, QString> face_box_color_steps;
  QMap<int, QString> color_steps;
  QMap<int, QString> name_format_steps;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
  main_setting_param_t main_setting_param;
  QList<item_t> menu_main;
};

#endif  //  UI_MAIN_SETTING_H
