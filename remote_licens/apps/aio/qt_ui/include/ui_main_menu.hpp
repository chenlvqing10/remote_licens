/*
* ui_main_menu.h - define main menu function of UI
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

#ifndef UI_MAIN_MENU_H
#define UI_MAIN_MENU_H

#include "QtMainWindow.hpp"

namespace Ui {
#if ATTENDANCE_MODE == 2
  class ui_main_menu_wechat;
#else
  class ui_main_menu;
#endif
}
#include "clicked_label.hpp"
class clicked_Label;

typedef struct it_win_ {
  clicked_Label *bt;
  const QMetaObject *win;
} it_win_t;

class ui_main_menu : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_main_menu(QWidget *parent = 0);
  ~ui_main_menu();

 public slots:
  void item_clicked();

 private:
#if ATTENDANCE_MODE == 2
  Ui::ui_main_menu_wechat *ui;
#else
  Ui::ui_main_menu *ui;
#endif
  QList<it_win_t> bt_layout_t;
};

#endif  //  UI_MAIN_MENU_H
