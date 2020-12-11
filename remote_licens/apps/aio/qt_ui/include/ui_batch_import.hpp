/*
* ui_batch_import.hpp - define batch import of project
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

#ifndef UI_BATCH_IMPORT_H
#define UI_BATCH_IMPORT_H

#include "QtMainWindow.hpp"
#include "ui_rotate_management.hpp"

namespace Ui {
class ui_batch_import;
}

class ui_batch_import : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_batch_import(QWidget *parent = 0);
  ~ui_batch_import();

 public slots:
  void deinit();

 private slots:
  void slot_for_batch_percent(int value);
  void slot_for_batch_import();
  void slot_for_update();
  void slot_for_back_btn();

 private:
  Ui::ui_batch_import *ui;
  ui_rotate_management *rotate;
  QTimer *pTimer;
  int do_end_;
  int processing_;
};

#endif  //  UI_BATCH_IMPORT_H
