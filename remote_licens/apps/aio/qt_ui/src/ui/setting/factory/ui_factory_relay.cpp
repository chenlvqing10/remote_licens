/*
* ui_factory_relay.cpp - define interface function of UI
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

#include <QObject>

#include "ui_factory_relay.hpp"
#include "ui_ui_factory_relay.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TIMER_INTERVAL_COUNT 1000

ui_factory_relay::ui_factory_relay(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_relay),
    test_result_(0),
    relay_status(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
                ui->factory_relay_back_btn->setEnabled(false);
*/

  connect(ui->factory_relay_back_btn, SIGNAL(clicked()),
            rotate_signgleton, SLOT(back_activity_win()));

  ui->factory_relay_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_relay_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  ui->factory_relay_ok_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_OK));
  ui->factory_relay_fail_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_FAIL));

  connect(ui->factory_relay_ok_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_relay()));
  connect(ui->factory_relay_fail_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_relay()));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_function()));

  ui->relay_result_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->relay_result_label->setWordWrap(true);

  ui->relay_result_label->
      setText(tr("\n\n\n  通过自动打开关闭继电器测试是否正常,"
              "请倾听继电器声音并确认继电器是否正常"));
  pTimer->start(TIMER_INTERVAL_COUNT);
}

ui_factory_relay::~ui_factory_relay() {
  delete ui;
  delete pTimer;
}

void ui_factory_relay::deinit() {
  QtMainWindow::deinit();
  ui_control_relay_ctrl(RELAY_OFF);
}

void ui_factory_relay::slot_for_call_function() {
  relay_status = ~relay_status;
  ui_control_relay_ctrl(static_cast<RELAY_STATE>(relay_status));
}

