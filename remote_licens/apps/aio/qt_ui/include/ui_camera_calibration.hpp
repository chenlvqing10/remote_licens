/*
* ui_camera_calibration.hpp - define camera calibration interface function of UI
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

#ifndef UI_CAMERA_CALIBRATION_H
#define UI_CAMERA_CALIBRATION_H

#include "QtMainWindow.hpp"
#include "ui_proc_thread.hpp"

namespace Ui {
class ui_camera_calibration;
}

class ui_camera_calibration : public QtMainWindow, ui_basic_test {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_camera_calibration(QWidget *parent = 0);
  ~ui_camera_calibration();
  void press_ok();

 protected:
  void paintEvent(QPaintEvent*);

 private slots:
  void slot_for_back_btn();
  void slot_for_call_funtion();
  void slot_for_snapshot();

 public:
  virtual void proc_func();

 private:
  Ui::ui_camera_calibration *ui;
  QTimer *pTimer;
  proc_thread thread;
  int index_;
  int do_end_;
  int do_result_;
  int clicked_count_;
};

#endif  //  UI_CAMERA_CALIBRATION_H
