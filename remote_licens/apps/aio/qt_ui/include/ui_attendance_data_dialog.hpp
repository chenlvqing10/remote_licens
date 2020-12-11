/*
* ui_attendance_data_dialog.cpp - define time dialog interface function of UI
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

#ifndef UI_ATTENDANCE_DATA_DIALOG_H
#define UI_ATTENDANCE_DATA_DIALOG_H

#include <QMainWindow>
#include <QtGui>

#include "ui_proc_thread.hpp"
#include "QtDialog.hpp"

#define TIME_UNITS 3

namespace Ui {
class ui_attendance_data_dialog;
}

class ui_attendance_callback {
 public:
  virtual void attendance_callback(void *id, QString time) = 0;
};

typedef struct attendance_args {
  void *id;
  ui_attendance_callback *cb;
} attendance_args_t;

typedef struct count_unit {
  int min, max, default_value, cur_value;
  QPushButton *add, *down;
  QLabel *value;
} count_unit_t;

class ui_attendance_data_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_attendance_data_dialog(attendance_args_t *args, QWidget* parent = 0);
  ~ui_attendance_data_dialog();
  void deinit();

 private slots:
  void slot_for_confirm();
  void slot_for_cancel();
  void map_pressed(QWidget* w);
  void slot_for_released();
  void slot_for_timeout();
  void time_select_clicked(int id);

 private:
  Ui::ui_attendance_data_dialog* ui;
  attendance_args_t *attendance_info;
  count_unit_t counts[TIME_UNITS];
  QSignalMapper pressed_mapper;
  QTimer *p_time;
  QButtonGroup *m_group;
  QRadioButton *select_bt;
  int pressed_index;
  int step;
};

#endif  //  UI_ATTENDANCE_DATA_DIALOG_H