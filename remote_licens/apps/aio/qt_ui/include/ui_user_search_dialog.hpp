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

#ifndef UI_USER_SEARCH_DIALOG_H
#define UI_USER_SEARCH_DIALOG_H

#include <QMainWindow>
#include "QtDialog.hpp"
#include <QFont>
#include <QtGui>
#include "ui_proc_thread.hpp"
#include <QByteArray>

namespace Ui {
class ui_user_search_dialog;
}

class ui_user_search_callback {
 public:
  virtual void user_search_callback(QString id, QString name) = 0;
};

typedef struct user_search_args {
  ui_user_search_callback *cb;
} user_search_args_t;

class ui_user_search_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_user_search_dialog(user_search_args *args, QWidget* parent = 0);
  ~ui_user_search_dialog();

 private slots:
  void slot_for_confirm();

 private:
  Ui::ui_user_search_dialog* ui;
  user_search_args_t *user_search_info;
};

#endif  //  UI_USER_SEARCH_DIALOG_H
