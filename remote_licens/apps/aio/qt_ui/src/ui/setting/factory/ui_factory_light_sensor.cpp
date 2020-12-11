/*
* ui_factory_light_sensor.cpp - define factory test light sensor interface function of UI
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
#include <QMutex>
#include <QWaitCondition>

#include "ui_factory_light_sensor.hpp"
#include "ui_ui_factory_light_sensor.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TIMER_INTERVAL_COUNT 1000

ui_factory_light_sensor::ui_factory_light_sensor(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_light_sensor),
    test_end_(0),
    test_result_(0),
    brightness_(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
          ui->factory_light_sensor_back_btn->setEnabled(false);
*/

  connect(ui->factory_light_sensor_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  ui->factory_light_sensor_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_light_sensor_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  connect(ui->factory_light_sensor_ok_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_light_sensor_ok()));
  connect(ui->factory_light_sensor_fail_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_light_sensor_fail()));

  pTimer = new QTimer;
    connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_funtion()));

  thread.set_test_ui(this);

  ui->factory_light_sensor_result_label->
      setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->factory_light_sensor_result_label->setWordWrap(true);

  ui->factory_light_sensor_result_label->
      setText(tr("\n\n\n  请手动遮挡或者加光,如果亮度值有变化,"
              "则手动选择正常,反之异常"
              "正在测试中,请稍候..."));
  pTimer->start(TIMER_INTERVAL_COUNT);

  is_stop_ = 0;
  thread.start();
}

ui_factory_light_sensor::~ui_factory_light_sensor() {
  delete ui;
  delete pTimer;
}

void ui_factory_light_sensor::slot_for_call_funtion() {
  QString str = QString(tr(
          "\n\n\n  请手动遮挡或者加光,如果亮度值有变化,"
          "则手动选择正常,反之异常"
          "当前亮度值:%1")).arg(brightness_);
  ui->factory_light_sensor_result_label->
      setText(str);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  if (test_end_) {
    pTimer->stop();
  }
}

void ui_factory_light_sensor::slot_for_back_btn() {
  is_stop_ = 1;
  while (test_end_ == 0) {
    QWaitCondition wait;
    QMutex mutex1;
    mutex1.lock();
    wait.wait(&mutex1, 1);
    mutex1.unlock();
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_factory_light_sensor::slot_for_light_sensor_ok() {
  is_stop_ = 1;
  while (test_end_ == 0) {
    QWaitCondition wait;
    QMutex mutex1;
    mutex1.lock();
    wait.wait(&mutex1, 1);
    mutex1.unlock();
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_light_sensor(0);
}

void ui_factory_light_sensor::slot_for_light_sensor_fail() {
  is_stop_ = 1;
  while (test_end_ == 0) {
    QWaitCondition wait;
    QMutex mutex1;
    mutex1.lock();
    wait.wait(&mutex1, 1);
    mutex1.unlock();
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_light_sensor(1);
}

void ui_factory_light_sensor::proc_func() {
  int i, ret, value = 0;

  while (is_stop_ == 0) {
    if ((i++ % 100) == 0) {
      ret = ui_control_test_get_sensor_illuminance_value(&value);
      ALOGD("get light value %d ret %d", value, ret);
      if (ret == 0) {
        brightness_ = value;
      }
    }

    QWaitCondition wait;
    QMutex mutex1;
    mutex1.lock();
    wait.wait(&mutex1, 1);
    mutex1.unlock();
  }

  test_end_ = 1;
}

int ui_factory_light_sensor::is_test_end() {
  return test_end_;
}

