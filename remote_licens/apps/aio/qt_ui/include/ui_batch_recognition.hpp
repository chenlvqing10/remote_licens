/*
* ui_batch_recognition.hpp - define batch recognition of project
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

#ifndef UI_BATCH_RECOGNITION_H
#define UI_BATCH_RECOGNITION_H

#include "QtMainWindow.hpp"
#include "ui_rotate_management.hpp"

namespace Ui {
class ui_batch_recognition;
}

class ui_batch_recognition : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_batch_recognition(QWidget *parent = 0);
  ~ui_batch_recognition();

 private slots:
  void slot_for_batch_percent(int value);
  void slot_for_batch_recognition();
  void slot_for_update();
  void slot_for_back_btn();

 private:
  Ui::ui_batch_recognition *ui;
  ui_rotate_management *rotate;
  QTimer *pTimer;
  int do_end_;
  int processing_;
};

#endif  //  UI_BATCH_RECOGNITION_H
