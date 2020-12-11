/*
* ui_factory_camera.cpp - define factory test camera interface function of UI
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

#include "ui_factory_camera.hpp"
#include "ui_ui_factory_camera.h"
#include "ui_rotate_management.hpp"

ui_factory_camera::ui_factory_camera(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_camera) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
      ui->factory_camera_back_btn->setEnabled(false);
*/

  connect(ui->factory_camera_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  ui->factory_camera_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_camera_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  ui->factory_camera_ok_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_OK));
  ui->factory_camera_fail_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_FAIL));

  connect(ui->factory_camera_ok_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_camera()));
  connect(ui->factory_camera_fail_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_camera()));

  ui->tip_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->tip_label->setWordWrap(true);
  ui->tip_label->setText(tr("\n\n\n  通过图像预览来测试摄像头是否正常工作"));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_funtion()));

  pTimer->start();
}

ui_factory_camera::~ui_factory_camera() {
  delete ui;
  delete pTimer;
}

void ui_factory_camera::deinit() {
  QtMainWindow::deinit();
  ui_control_camera_nir_stop_preview();
}

void ui_factory_camera::paintEvent(QPaintEvent* e) {
  QPen pen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap);
  QPainter painter(this);
  painter.setCompositionMode(QPainter::CompositionMode_Clear);
  painter.setPen(pen);
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();
  painter.fillRect(0, 0, width, height, Qt::SolidPattern);
}

void ui_factory_camera::slot_for_call_funtion() {
  pTimer->stop();
  int ret = ui_control_camera_nir_stop_preview();
  ALOGD("ui_control_camera_nir_stop_preview() ret %d", ret);
  if (ret == 0) {
    // ret = ui_control_camera_nir_start_preview(320, 180, 640, 360, 90);
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    if (width > height) {
      ret = ui_control_camera_nir_start_preview(180, 320, 360, 640, 90);
    } else {
      ret = ui_control_camera_nir_start_preview(180, 320, 360, 640, 90);
    }
    ALOGD("ui_control_camera_nir_start_preview() ret %d", ret);
  }
}

