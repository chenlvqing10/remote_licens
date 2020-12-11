/*
* ui_proc_thread.cpp - define process thread of project
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

#include <QPainter>
#include <QDebug>

#include "ui_proc_thread.hpp"

proc_thread::proc_thread(QObject *parent) :
    QThread(parent), test_ui_(NULL) {
}

void proc_thread::run() {
  if (test_ui_) {
    test_ui_->proc_func();
  }
}

void proc_thread::set_test_ui(ui_basic_test *test) {
  test_ui_ = test;
}

