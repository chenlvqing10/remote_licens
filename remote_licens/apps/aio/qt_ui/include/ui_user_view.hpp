/*
* ui_user view.h - define user view of UI
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

#ifndef UI_USER_VIEW_H
#define UI_USER_VIEW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "ui_control.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QModelIndex>
#include <QStandardItemModel>

#include "QtMainWindow.hpp"

#define USER_ID_LEN 64

namespace Ui {
class ui_user_view;
}

class ui_user_view : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_user_view(QWidget *parent = 0);
  ~ui_user_view();
  void clean_all_items();
  void load_user_from_db();
  void display_users();
  void delete_selected_user();

 private slots:
  void onEditTableBtnClicked();
  void onDelTableBtnClicked();
  void slot_for_call_function();
  void slot_for_first();
  void slot_for_prev();
  void slot_for_next();
  void slot_for_last();

 private:
  Ui::ui_user_view *ui;
  QTimer *pTimer;
  int rec_count_;
  int index_;
  char user_id[USER_ID_LEN];
  QStandardItemModel *tableModel;
};

#endif  //  UI_USER_VIEW_H
