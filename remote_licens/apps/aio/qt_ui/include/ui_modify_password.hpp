/*
* ui_modify_password.hpp - define modify password function of UI
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

#ifndef UI_MODIFY_PASSWORD_H
#define UI_MODIFY_PASSWORD_H

#include <QLineEdit>
#include <QStandardItemModel>

#include "QtMainWindow.hpp"
#include "ui_modify_password_delegate.hpp"

namespace Ui {
class ui_modify_password;
}

class ui_modify_password : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_modify_password(QWidget *parent = 0);
  ~ui_modify_password();
  int check_str_is_validate(const char *str);

 private slots:
  void set_for_modify_password_save();

 private:
  Ui::ui_modify_password *ui;
  QLineEdit *old_passwd_edt;
  QLineEdit *new_passwd_edt;
  QLineEdit *new_passwd_confirm_edt;
  QStandardItemModel *pModel;
  ui_modify_password_delegate *pItemDelegate;
};

#endif  //  UI_ALGORITHM_SETTING_H
