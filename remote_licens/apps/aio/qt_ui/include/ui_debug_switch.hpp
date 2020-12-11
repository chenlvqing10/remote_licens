/*
* ui_debug_switch.hpp - define debug switch function of main
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

#ifndef UI_DEBUG_SWITCH_H
#define UI_DEBUG_SWITCH_H

#include <QModelIndex>
#include <QPushButton>
#include <QStandardItemModel>

#include "QtMainWindow.hpp"
#include "ui_debug_switch_delegate.hpp"

namespace Ui {
class ui_debug_switch;
}

class ui_debug_switch : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_debug_switch(QWidget *parent = 0);
  ~ui_debug_switch();
  void update_debug_icon_status(int index, QPushButton *btn);

 public slots:
  void deinit();

 private slots:
  void slot_for_debug_listview(const QModelIndex &index);
  void slot_for_check_btn();

 private:
  Ui::ui_debug_switch *ui;
  QPushButton *isp_btn;
  QPushButton *rec_btn;
  QPushButton *time_btn;
  QStandardItemModel *pModel;
  ui_debug_switch_delegate *pItemDelegate;
};

#endif  //  UI_DEBUG_SWITCH_H
