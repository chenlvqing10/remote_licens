/*
* ui_camera_calibration.cpp - define camera calibration interface function of UI
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
#include "ui_control.h"
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>
#include <QDir>

#include "ui_camera_calibration.hpp"
#include "ui_ui_camera_calibration.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define SNAPSHOT_COUNT 3

#define MAX_BUFFER_SIZE 256
#define TIMER_INTERVAL_COUNT 1000

#define CALIBRATION_LINE_WIDTH 20

#define CALIBRATION_WIDTH_50CM 440
#define CALIBRATION_HEIGHT_50CM 640

#define CALIBRATION_WIDTH_1M 250
#define CALIBRATION_HEIGHT_1M 375

ui_camera_calibration::ui_camera_calibration(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_camera_calibration),
    index_(0),
    do_end_(1),
    do_result_(0),
    clicked_count_(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

  connect(ui->camera_calibration_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  connect(ui->camera_calibration_ok_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_snapshot()));

  ui->camera_calibration_tip_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->camera_calibration_tip_label->setWordWrap(true);
  ui->camera_calibration_tip_label->
      setText(tr("  请离设备0.8m处, 移动标定源图片,"
              "使其在屏幕的红色矩形框内,按抓拍按钮开始标定"));

  thread.set_test_ui(this);

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_funtion()));
}

ui_camera_calibration::~ui_camera_calibration() {
  delete ui;
  delete pTimer;
}

void ui_camera_calibration::paintEvent(QPaintEvent* e) {
  QPen pen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap);
  QPainter painter(this);
  painter.setCompositionMode(QPainter::CompositionMode_Clear);
  painter.setPen(pen);
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();
  painter.fillRect(0, 0, width, height, Qt::SolidPattern);

  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.setPen(QPen(Qt::red, CALIBRATION_LINE_WIDTH));
  if (0 == clicked_count_) {
    painter.drawRect(
      (width - CALIBRATION_WIDTH_50CM) / 2,
      (height - CALIBRATION_HEIGHT_50CM) / 2,
      CALIBRATION_WIDTH_50CM, CALIBRATION_HEIGHT_50CM);
  } else {
    painter.drawRect(
      (width - CALIBRATION_WIDTH_1M) / 2,
      (height - CALIBRATION_HEIGHT_1M) / 2,
      CALIBRATION_WIDTH_1M, CALIBRATION_HEIGHT_1M);
  }
}

void ui_camera_calibration::slot_for_back_btn() {
  if (do_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_CAMERA_CALIBRATION, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_camera_calibration::slot_for_call_funtion() {
  int status;
  ui_tip_dialog *w;

  if (do_end_ == 1) {
    pTimer->stop();
    ui->camera_calibration_ok_btn->setEnabled(true);
    ui->camera_calibration_ok_btn->setStyleSheet("background:#00d5cf");

#if 0
    if (do_result_ == 0) {
      status = TIP_WIN_TYPE_SNAPSHOT_OK;
    } else {
          status = TIP_WIN_TYPE_SNAPSHOT_FAIL;
    }

    w = new ui_tip_dialog(status);
    if ((clicked_count_ == 1) && (status = TIP_WIN_TYPE_SNAPSHOT_OK)) {
      w->set_user_win(this);
    }
    w->show();
#else
    if (do_result_ == -1) {
      status = TIP_WIN_TYPE_SNAPSHOT_FAIL;
      w = new ui_tip_dialog(status, this);
      w->show();
    }

    if ((0 == do_result_) && (clicked_count_ == 1)) {
      press_ok();
    }
#endif
  if ((0 == do_result_) && (clicked_count_ == 0)) {
    clicked_count_++;
    ui->camera_calibration_tip_label->
      setText(tr("  请离设备1.3m处, 移动标定源图片,"
      "使其在屏幕的红色矩形框内,按抓拍按钮开始标定"));
     this->repaint();
      }
  }
}

void ui_camera_calibration::slot_for_snapshot() {
#if 0
  if (0 == do_result_) {
    clicked_count_++;
  }
#endif
  pTimer->start(TIMER_INTERVAL_COUNT);
  ui->camera_calibration_ok_btn->setEnabled(false);
  ui->camera_calibration_ok_btn->setStyleSheet("background:#454A60");
  thread.start();
}

void ui_camera_calibration::proc_func() {
  int ret = 0;
  int x, y, w, h;
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();

  do_end_ = 0;

  if (0 == clicked_count_) {
    x = (width - CALIBRATION_WIDTH_50CM) / 2;
    y = (height - CALIBRATION_HEIGHT_50CM) / 2;
    w = CALIBRATION_WIDTH_50CM;
    h = CALIBRATION_HEIGHT_50CM;
  } else {
    x = (width - CALIBRATION_WIDTH_1M) / 2;
    y = (height - CALIBRATION_HEIGHT_1M) / 2;
    w = CALIBRATION_WIDTH_1M;
    h = CALIBRATION_HEIGHT_1M;
  }
  ret = ui_control_take_mod_pic(x - 40, y - 40, w + 80, h + 80);
  if (0 == ret) {
    do_result_ = 0;
  } else {
    do_result_ = -1;
  }

  do_end_ = 1;
}

void ui_camera_calibration::press_ok() {
  int status;
  ui_tip_dialog *w;
  map_params_t map_param;
  ui_map_t maps;
  int i, ret;
  ret = ui_control_cal_mod_map(&map_param);
  if (ret == 0) {
    memset(&maps, 0, sizeof(ui_map_t));
    for (i = 0; i < CALIBRATION_NUM; i++) {
      maps.x_map[i] = map_param.x_map[i];
      maps.y_map[i] = map_param.y_map[i];
    }
    ret = ui_control_save_mod_map_param(&maps);
    if (ret == 0) {
      status = TIP_WIN_TYPE_CALIBRATION_OK;
    } else {
      status = TIP_WIN_TYPE_CALIBRATION_FAIL;
    }

    w = new ui_tip_dialog(status, this);
    w->show();

    clicked_count_ = 0;
    ui->camera_calibration_tip_label->
      setText(tr("  请离设备0.8m处, 移动标定源图片,"
              "使其在屏幕的红色矩形框内,按抓拍按钮开始标定"));
    this->repaint();
  } else {
    status = TIP_WIN_TYPE_CALIBRATION_FAIL;
    w = new ui_tip_dialog(status, this);
    w->show();
  }
}

