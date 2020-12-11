/*
* ui_face_authentication.cpp - define face authentication functions interface
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "screen_size.h"
#include "ui_control.h"
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QPainter>
#include <QDebug>

#include "ui_face_authentication.hpp"
#include "ui_ui_face_authentication.h"
#include "ui_rotate_management.hpp"
#include "screen_size.hpp"

#ifndef PORTRAIT_SCREEN
#define MAIN_AREA_X 457
#define MAIN_AREA_Y 80
#define MAIN_AREA_W 360
#define MAIN_AREA_H 520
#else
#define MAIN_AREA_X 150
#define MAIN_AREA_Y 180
#define MAIN_AREA_W 500
#define MAIN_AREA_H 664
#endif

#define MAIN_CIRCLE_R 197

detect_thread::detect_thread(QObject *parent) :
    QThread(parent), stopped(false), detect_ok(0), auth_(NULL) {
  connect(this, SIGNAL(detect_signal()), this, SLOT(slot_for_detect_result()));
}

void detect_thread::run() {
  ui_feat_local_t feat;
  while (!stopped) {
    int ret = ui_control_calc_feat_from_photo(&feat, 1);
    ALOGD("ui_control_calc_feat_from_photo() ret %d norm: %f %f", ret,
          feat.face_norm[0], feat.face_norm[1]);
    if (ret == 0) {
      ui_rotate_management *rotate_signgleton =
        ui_rotate_management::getInstance();
      rotate_signgleton->set_feat_info(&feat);
      if (auth_) {
        auth_->set_check_status(1);
      }
      ui_control_add_person_sound();
      QThread::usleep(1000 * 1000);
      emit detect_signal();
      detect_ok = 1;
      ALOGD("detect_signal");
      break;
    }
  }
  stopped = false;
}

void detect_thread::stop() {
  stopped = true;
  while (stopped && !detect_ok) {
    QThread::usleep(100);
  }
}

void detect_thread::slot_for_detect_result() {
  ALOGD("detect_thread::slot_for_detect_result(): tid= %d",
        currentThreadId());
}

void detect_thread::set_auth(ui_face_authentication *auth) {
  auth_ = auth;
}

ui_face_authentication::ui_face_authentication(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_face_authentication),
    src_type(1), check_ok_(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  face_set_register_mode(1);

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_funtion()));
  pTimer->start(100);
}

ui_face_authentication::~ui_face_authentication() {
  face_set_register_mode(0);
  delete ui;
  delete pTimer;
}

void ui_face_authentication::set_type(int type) {
  int ret = 0;

  if (src_type == type)
    return;
  src_type = type;
  if (0 == src_type) {
#if 0
    ret = face_photo_set_area(get_screen_width() / 2 - MAIN_CIRCLE_R,
      get_screen_height() / 2 - MAIN_CIRCLE_R,
      get_screen_width() / 2 + MAIN_CIRCLE_R,
      get_screen_height() / 2 + MAIN_CIRCLE_R);
#endif
    ret = face_photo_set_area(MAIN_AREA_X, MAIN_AREA_Y,
      MAIN_AREA_X + MAIN_AREA_W, MAIN_AREA_Y + MAIN_AREA_H);

    if (ret)
      ALOGE("set face input limit area fail!");

    ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();
    connect(ui->face_authentication_back_btn, SIGNAL(clicked()),
            rotate_signgleton, SLOT(back_activity_win()));
    QObject::connect(&detect, SIGNAL(detect_signal()),
            rotate_signgleton, SLOT(back_activity_win()));
    detect.set_auth(this);
    detect.start();
  }
}


void ui_face_authentication::deinit() {
  QtMainWindow::deinit();

  if (pTimer->isActive()) {
    pTimer->stop();
  }
  if (src_type == 0) {
    detect.stop();
  }
}

void ui_face_authentication::set_check_status(int status) {
  check_ok_ = status;
}

void ui_face_authentication::paintEvent(QPaintEvent* e) {
  QPen pen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap);
  QLinearGradient myGradient;
  QPainter painter(this);
  painter.setCompositionMode(QPainter::CompositionMode_Clear);
  painter.setPen(pen);
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();
  painter.fillRect(0, 0, width, height, Qt::SolidPattern);
}

void ui_face_authentication::slot_for_call_funtion() {
  if (check_ok_ == 1) {
    pTimer->stop();
    ui->authentication_label->setText(tr("人脸录入成功"));
  }
}

