/*
* ui_home_entering.h - define home entering function of UI
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

#ifndef UI_HOME_ENTERING_H
#define UI_HOME_ENTERING_H

#include "QtMainWindow.hpp"

namespace Ui {
class ui_home_entering;
}

class ui_home_entering : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_home_entering(QWidget *parent = 0);
  ~ui_home_entering();

 private slots:
  void slot_for_net_status(int event,int type);
  void slot_for_show_webchat(int type);
  void slot_set_face_input_mode();

 private:
  Ui::ui_home_entering *ui;
};

#endif  //  UI_HOME_ENTERING_H
