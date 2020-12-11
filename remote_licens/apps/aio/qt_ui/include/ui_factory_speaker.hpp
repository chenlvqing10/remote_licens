/*
* ui_factroy_speaker.h - define factory test for speaker function of UI
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

#ifndef UI_FACTORY_SPEAKER_H
#define UI_FACTORY_SPEAKER_H

#include "QtMainWindow.hpp"
#include "ui_proc_thread.hpp"

namespace Ui {
class ui_factory_speaker;
}

class ui_factory_speaker : public QtMainWindow, ui_basic_test {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_factory_speaker(QWidget *parent = 0);
  ~ui_factory_speaker();

 private slots:
  void slot_for_call_funtion();
  void slot_for_back_btn();
  void slot_for_speaker_ok();
  void slot_for_speaker_fail();

 public:
  virtual void proc_func();

 public:
  int is_test_end();

 private:
  Ui::ui_factory_speaker *ui;
  QTimer *pTimer;
  proc_thread thread;
  int test_end_;
};

#endif  //  UI_FACTORY_SPEAKER_H
