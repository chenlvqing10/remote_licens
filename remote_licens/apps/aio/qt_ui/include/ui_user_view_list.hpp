/*
* ui_user_view_list.hpp define user view list function of UI
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

#ifndef UI_USER_VIEW_LIST_H
#define UI_USER_VIEW_LIST_H

#include <QStandardItemModel>
#include <map>
#include <QSignalMapper>
#include <QList>

#include "QtMainWindow.hpp"
#include "db_all.h"
#include "ui_setting_item_delegate.hpp"
#include "person_output.h"
#include "ui_user_search_dialog.hpp"

namespace Ui {
class ui_user_view_list;
}

typedef enum {
    BT_GO_FIRST,  /*0*/
    BT_GO_PREV, /*1*/
    BT_GO_NEXT, /*2*/
    BT_GO_LAST, /*3*/
    BT_GO_MAX
} BT_GO_TYPE;

class ui_user_view_list : public QtMainWindow , ui_user_search_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_user_view_list(QWidget *parent = 0);
  ~ui_user_view_list();
  void get_user_list();
  void update_page(BT_GO_TYPE type);
  void delete_selected_user();
  void refresh_page();
  void user_search_callback(QString        id, QString name);

 private slots:
  void slot_for_mainui(int event, int value);
  void slot_for_first();
  void slot_for_prev();
  void slot_for_next();
  void slot_for_last();
  void slot_for_output();
  void slot_for_search();
  void slot_for_del_user_item(const QModelIndex &index);
  void output_user_cb(int state, int step);

 private:
  Ui::ui_user_view_list *ui;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
  QList<item_t> user_list;
  int page, pages, count, select_index;
  int count_height;
  char user_id[64], user_name[64];
};

#endif  //  UI_USER_VIEW_LIST_H
