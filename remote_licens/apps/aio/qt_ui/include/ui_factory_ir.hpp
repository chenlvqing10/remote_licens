/*
* ui_factroy_ir.h - define factory test for ir function of UI
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

#ifndef UI_FACTORY_IR_H
#define UI_FACTORY_IR_H

#include "QtMainWindow.hpp"

#include "ui_proc_thread.hpp"

namespace Ui {
class ui_factory_ir;
}

class ui_factory_ir : public QtMainWindow, ui_basic_test {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_factory_ir(QWidget *parent = 0);
  ~ui_factory_ir();

 private slots:
  void slot_for_call_function();
  void slot_for_back_btn();
  void slot_for_ir_ok();
  void slot_for_ir_fail();

 public:
  virtual void proc_func();

 public:
  int is_test_end();

 private:
  Ui::ui_factory_ir *ui;
  QTimer *pTimer;
  proc_thread thread;
  int test_end_;
  int test_result_;
};

#endif  //  UI_FACTORY_IR_H
