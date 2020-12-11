/*
* ui_count_dialog.cpp - define count dialog interface function of UI
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

#ifndef UI_COUNT_DIALOG_H
#define UI_COUNT_DIALOG_H

#include <QMainWindow>
#include <QtGui>

#include "ui_proc_thread.hpp"
#include "QtDialog.hpp"

namespace Ui {
class ui_count_dialog;
}

class ui_count_callback {
 public:
  virtual void count_callback(void *id, int count) = 0;
};

typedef struct count_args {
  void *id;
  QString title;
  int min, max;
  int default_value;
  ui_count_callback *cb;
} count_args_t;

class ui_count_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_count_dialog(count_args_t *args, QWidget* parent = 0,
    int step = 1, int decimals = 0);
  ~ui_count_dialog();
  void deinit();

 private slots:
  void slot_for_confirm();
  void slot_for_cancel();
  void slot_for_start_add();
  void slot_for_start_down();
  void slot_for_stop();
  void slot_for_timeout();

 private:
  Ui::ui_count_dialog* ui;
  count_args_t *count_info;
  QTimer *p_time;
  int cur_value;
  int step;
  int decimals;
};

#endif  //  UI_COUNT_DIALOG_H
