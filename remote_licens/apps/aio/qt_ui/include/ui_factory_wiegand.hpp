/*
* ui_factroy_wiegand.h - define factory test for wiegand function of UI
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

#ifndef UI_FACTORY_WIEGAND_H
#define UI_FACTORY_WIEGAND_H

#include <QLabel>
#include <QPushButton>
#include "QtMainWindow.hpp"

namespace Ui {
class ui_factory_wiegand;
}

class ui_factory_wiegand : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_factory_wiegand(QWidget *parent = 0);
  ~ui_factory_wiegand();

 private slots:
  void slot_for_call_function();
  void slot_for_back_btn();
  void slot_for_wiegand_ok();
  void slot_for_wiegand_fail();

 private:
  Ui::ui_factory_wiegand *ui;
  QTimer *pTimer;
  int test_result_;
  QLabel *tip_label;
};

#endif  //  UI_FACTORY_WIEGAND_H
