/*
* ui_time_dialog.cpp - define time dialog interface function of UI
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

#ifndef UI_TIME_DIALOG_H
#define UI_TIME_DIALOG_H

#include <QMainWindow>
#include <QtGui>

#include "ui_proc_thread.hpp"
#include "QtDialog.hpp"

#define UNITS_SIZE 4

namespace Ui {
class ui_time_dialog;
}

class ui_time_callback {
 public:
  virtual void time_callback(void *id, QString time) = 0;
};

typedef struct time_args {
  void *id;
  QString title;
  QString time;
  ui_time_callback *cb;
} time_args_t;

typedef struct cout_unit {
  int min, max, default_value, cur_value;
  QPushButton *add, *down;
  QLabel *value;
} cout_unit_t;

class ui_time_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_time_dialog(time_args_t *args, QWidget* parent = 0);
  ~ui_time_dialog();
  void deinit();

 private slots:
  void slot_for_confirm();
  void slot_for_cancel();
  void map_pressed(QWidget* w);
  void slot_for_released();
  void slot_for_timeout();

 private:
  Ui::ui_time_dialog* ui;
  time_args_t *time_info;
  cout_unit_t counts[UNITS_SIZE];
  QSignalMapper pressed_mapper;
  QTimer *p_time;
  int pressed_index;
  int step;
};

#endif  //  UI_TIME_DIALOG_H
