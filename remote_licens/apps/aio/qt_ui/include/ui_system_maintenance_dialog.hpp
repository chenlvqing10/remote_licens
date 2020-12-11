/*
* ui_system_maintenance_dialog.cpp -
* define system maintenance dialog interface function of UI
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

#ifndef UI_SYSTEM_MAINTENANCE_DIALOG_H
#define UI_SYSTEM_MAINTENANCE_DIALOG_H

#include <QMainWindow>
#include <map>
#include <QSignalMapper>
#include <QPushButton>
#include <QLabel>
#include "QtDialog.hpp"

namespace Ui {
class ui_system_maintenance_dialog;
}

typedef struct {
  int min, max, default_value, cur_value;
  QPushButton *add, *down;
  QLabel *value;
} reboot_time_t;

class ui_system_maintenance_callback {
 public:
  virtual void system_maintenance_callback(void *id,
      int choice, QString time) = 0;
};

typedef struct system_maintenance_args {
  void *id;
  QString title;
  int default_choice;
  QString reboot_time;
  ui_system_maintenance_callback *cb;
} system_maintenance_args_t;

class ui_system_maintenance_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_system_maintenance_dialog(
      system_maintenance_args_t *args, QWidget* parent = 0);
  ~ui_system_maintenance_dialog();
  void deinit();

 private slots:
  void slot_for_confirm();
  void slot_for_cancel();
  void map_pressed(QWidget* w);
  void slot_for_released();
  void slot_for_timeout();

 private:
  Ui::ui_system_maintenance_dialog* ui;
  system_maintenance_args_t *system_maintenance_info;
  reboot_time_t counts[2];
  QSignalMapper pressed_mapper;
  QTimer *p_time;
  int pressed_index;
  int step;
  QButtonGroup *m_group;
};

#endif  //  UI_SYSTEM_MAINTENANCE_DIALOG_H

