/*
* ui_input_dialog.cpp - define input dialog interface function of UI
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

#ifndef UI_INPUT_DIALOG_H
#define UI_INPUT_DIALOG_H

#include <QMainWindow>
#include <QLineEdit>

#include "QtDialog.hpp"

namespace Ui {
class ui_input_dialog;
}

class ui_input_callback {
 public:
  virtual void input_callback(void *id, QString text) = 0;
};

typedef struct input_args {
  void *id;
  QString title;
  QString default_input;
  QLineEdit::EchoMode echo_mode;
  int max_len;
  ui_input_callback *cb;
} input_args_t;

class ui_input_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_input_dialog(input_args_t *args, QWidget* parent = 0);
  ~ui_input_dialog();
  void deinit();

 private slots:
  void slot_for_confirm();
  void slot_for_cancel();

 private:
  Ui::ui_input_dialog* ui;
  input_args_t *input_info;
};

#endif  //  UI_INPUT_DIALOG_H

