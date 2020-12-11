/*
* ui_proc_thread.hpp - define process thread of project
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

#ifndef UI_PROC_THREAD_H
#define UI_PROC_THREAD_H

#include <QMainWindow>
#include <QThread>

class ui_basic_test {
 public:
  virtual void proc_func() = 0;
};

class proc_thread : public QThread {
  Q_OBJECT

 public:
  explicit proc_thread(QObject *parent = 0);
  void set_test_ui(ui_basic_test *test);

 protected:
  void run();

 private:
  ui_basic_test *test_ui_;
};

#endif  //  UI_PROC_THREAD_H
