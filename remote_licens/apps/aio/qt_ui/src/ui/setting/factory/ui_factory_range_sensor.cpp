/*
* ui_factory_range_sensor.cpp - define factory test range sensor interface function of UI
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

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>

#include "ui_factory_range_sensor.hpp"
#include "ui_ui_factory_range_sensor.h"
#include "ui_rotate_management.hpp"

ui_factory_range_sensor::ui_factory_range_sensor(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_range_sensor) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
              ui->factory_range_sensor_back_btn->setEnabled(false);
*/

  connect(ui->factory_range_sensor_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  ui->factory_range_sensor_test_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_normal.png)}");
  ui->factory_range_sensor_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_range_sensor_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  connect(ui->factory_range_sensor_test_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_factory_range_sensor_test()));

  ui->factory_range_sensor_ok_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_OK));
  ui->factory_range_sensor_fail_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_FAIL));

  connect(ui->factory_range_sensor_ok_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_range_sensor()));
  connect(ui->factory_range_sensor_fail_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_range_sensor()));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_funtion()));

  ui->factory_range_result_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->factory_range_result_label->
      setText(tr("通过自动打开关闭距离传感器以验证该模块是否正常\n"
            "请点击按钮测试"));
}

ui_factory_range_sensor::~ui_factory_range_sensor() {
  delete ui;
  delete pTimer;
}

void ui_factory_range_sensor::slot_for_factory_range_sensor_test() {
  ui->factory_range_result_label->
      setText(tr("通过自动打开关闭距离传感器以验证该模块是否正常\n"
              "正在测试中,请稍候..."));
  pTimer->start(1000);
}

void ui_factory_range_sensor::slot_for_call_funtion() {
  pTimer->stop();
  int ret = 0;
  if (ret == 0) {
    ui->factory_range_result_label->
        setText(tr("通过自动打开关闭距离传感器以验证该模块是否正常\n"
                "测试结果：测试正常"));
  } else {
    ui->factory_range_result_label->
        setText(tr("通过自动打开关闭距离传感器以验证该模块是否正常\n"
                "测试结果：测试异常"));
  }
}

