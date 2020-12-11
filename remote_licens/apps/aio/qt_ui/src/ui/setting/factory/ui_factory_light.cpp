/*
* ui_factory_light.cpp - define interface function of UI
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
#include "gpio.h"
#include "ui_control.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>

#include "ui_factory_light.hpp"
#include "ui_ui_factory_light.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TIMER_INTERVAL_COUNT 1000

ui_factory_light::ui_factory_light(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_light),
    test_end_(0),
    test_result_(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
        ui->factory_light_back_btn->setEnabled(false);
*/

  connect(ui->factory_light_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  ui->factory_light_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_light_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  connect(ui->factory_light_ok_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_light_ok()));
  connect(ui->factory_light_fail_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_light_fail()));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_funtion()));

  ui->light_result_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->light_result_label->setWordWrap(true);

  thread.set_test_ui(this);

  ui->light_result_label->
      setText(tr("\n\n\n  通过自动打开关闭白补光灯测试是否正常,"
              "请观察并确认补光灯是否正常"));
  pTimer->start(TIMER_INTERVAL_COUNT);

  thread.start();
}

ui_factory_light::~ui_factory_light() {
  delete ui;
  delete pTimer;
}

void ui_factory_light::slot_for_call_funtion() {
    ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();
    if (test_end_) {
      pTimer->stop();
      ui->light_result_label->
        setText(tr("\n\n\n  通过自动打开关闭白光灯测试是否正常,"
                "请观察并确认补光灯是否正常,"
                "测试完毕"));
    }
}

void ui_factory_light::slot_for_back_btn() {
  if (test_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_TEST, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_factory_light::slot_for_light_ok() {
  if (test_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_TEST, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_light(0);
}

void ui_factory_light::slot_for_light_fail() {
  if (test_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_TEST, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_light(1);
}

void ui_factory_light::proc_func() {
  test_result_ = ui_control_test_light();
  test_end_ = 1;
}

int ui_factory_light::is_test_end() {
  return test_end_;
}

