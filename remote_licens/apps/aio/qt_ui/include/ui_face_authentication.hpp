/*
* ui_face_autherntication.hpp - define get face feature of project
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

#ifndef UI_FACE_AUTHETICATION_H
#define UI_FACE_AUTHETICATION_H

#include <QPushButton>
#include <QThread>

#include "QtMainWindow.hpp"

namespace Ui {
class ui_face_authentication;
}

class ui_face_authentication;
class detect_thread : public QThread {
  Q_OBJECT

 public:
  explicit detect_thread(QObject *parent = 0);
  void stop();
  void set_auth(ui_face_authentication *auth);

 protected:
  void run();

 signals:
  void detect_signal();

 public slots:
  void slot_for_detect_result();

 private:
  volatile bool stopped;
  int detect_ok;
  ui_face_authentication *auth_;
};

class ui_face_authentication : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_face_authentication(QWidget *parent = 0);
  ~ui_face_authentication();
  void set_type(int type);
  void back_btn_clicked();
  void set_check_status(int status);

 protected:
  void paintEvent(QPaintEvent*);

 public slots:
  void deinit();

 private slots:
  void slot_for_call_funtion();

 private:
  Ui::ui_face_authentication *ui;
  int src_type;
  detect_thread detect;
  QTimer *pTimer;
  int check_ok_;
};

#endif  //  UI_FACE_AUTHETICATION_H
