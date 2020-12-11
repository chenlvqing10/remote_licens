/*
* ui_edit_dialog.cpp - define edit dialog interface function of UI
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

#ifndef UI_EDIT_DIALOG_H
#define UI_EDIT_DIALOG_H

#include <QMainWindow>
#include "QtDialog.hpp"

namespace Ui {
class ui_edit_dialog;
}

class ui_edit_callback;

typedef struct edit_args {
  void *id;
  QString title;
  QString key;
  QString value;
  ui_edit_callback *cb;
} edit_args_t;

class ui_edit_callback {
 public:
  virtual void edit_callback(edit_args_t *args) = 0;
};

class ui_edit_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_edit_dialog(const char *ssid, int sec_type, QWidget* parent = 0);
  explicit ui_edit_dialog(edit_args_t *args, QWidget* parent = 0);
  ~ui_edit_dialog();

 private slots:
  void slot_for_confirm();

 private:
  Ui::ui_edit_dialog* ui;
  edit_args_t *edit_info;
};

#endif  //  UI_EDIT_DIALOG_H
