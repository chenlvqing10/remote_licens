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

#ifndef UI_ATTENDANCE_SEARCH_H
#define UI_ATTENDANCE_SEARCH_H

#include "QtMainWindow.hpp"
#include <QtGui>

#include "ui_proc_thread.hpp"

#define TIME_UNITS 3

namespace Ui {
class ui_attendance_search;
}

typedef struct count_unit_s {
  int min, max, default_value, cur_value;
  QPushButton *add, *down;
  QLabel *value;
} count_unit_t_s;

class ui_attendance_search : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_attendance_search (QWidget* parent = 0);
  ~ui_attendance_search();
  void deinit();

 private slots:
  void slot_for_confirm();
  void map_pressed(QWidget* w);
  void slot_for_released();
  void slot_for_timeout();
  void time_select_clicked(int id);

 private:
  Ui::ui_attendance_search* ui;
  count_unit_t_s counts[TIME_UNITS];
  QSignalMapper pressed_mapper;
  QTimer *p_time;
  QButtonGroup *m_group;
  QRadioButton *select_bt;
  int pressed_index;
  int step;
};

#endif  //  UI_ATTENDANCE_SEARCH_H