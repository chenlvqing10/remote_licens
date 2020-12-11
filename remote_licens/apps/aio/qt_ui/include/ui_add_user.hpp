/*
* ui_add_user.hpp - define add user of project
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

#ifndef UI_ADD_USER_H
#define UI_ADD_USER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "ui_control.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QLineEdit>
#include <QModelIndex>
#include <QPushButton>
#include <QStandardItemModel>

#include "ui_add_user_delegate.hpp"
#include "QtMainWindow.hpp"

#define UESR_ITEM_COUNT 2
#define FIELD_VALUE_LEN 64

namespace Ui {
class ui_add_user;
}

class ui_add_user : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_add_user(QWidget *parent = 0);
  ~ui_add_user();
  void update_user_info(user_information_param_t *usr_info);

 public slots:
  void deinit();

 private slots:
  void slot_for_user_add_listview(const QModelIndex &index);
  void slot_for_face_input();
  void slot_for_cancel_save();
  void slot_for_confirm_save();

 private:
  Ui::ui_add_user *ui;
  user_information_param_t user_info;
  QLineEdit *user_edt[UESR_ITEM_COUNT];
  QPushButton *face_input_btn;
  char finput_value[FIELD_VALUE_LEN];
  QStandardItemModel *pModel;
  ui_add_user_delegate *pItemDelegate;
  int user_information;
  int userid_step;
};

#endif  //  UI_ADD_USER_H
