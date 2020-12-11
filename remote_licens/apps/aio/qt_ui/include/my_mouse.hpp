/*
* db_all.h - define interface function of db
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
#ifndef __MY_MOUSE_H
#define __MY_MOUSE_H
#include <QObject>
#include <QDebug>
#include "ui_control.h"
/*global listen mouse event*/
class my_mouse : public QObject {
 public:
  struct timeval tv;
  struct timeval tv_l;
  my_mouse() {
    gettimeofday(&tv_l, NULL);
  }
  bool eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseMove ||
      event->type() == QEvent::MouseButtonPress) {
      gettimeofday(&tv, NULL);
      unsigned int ret;
      ret = tv.tv_sec * 1000 + tv.tv_usec / 1000 -
            (tv_l.tv_sec * 1000 + tv_l.tv_usec / 1000);
      // qDebug() << "MouseMove() ret " << ret;
      if (ret >= 200) {
        /*wakeup*/
        ui_control_wakeup_screen();
        tv_l.tv_sec = tv.tv_sec;
        tv_l.tv_usec = tv.tv_usec;
        // qDebug() << "MouseMove() ui_control_wakeup_screen()";
      }
    }
    return QObject::eventFilter(obj, event);
  }
};
#endif  //  __MY_MOUSE_H
