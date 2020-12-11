/*
* ui_factory_alarm.cpp - define interface function of UI
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
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_factory_alarm.hpp"
#include "ui_ui_factory_alarm.h"

ui_factory_alarm::ui_factory_alarm(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_factory_alarm) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
      ui->factory_alarm_back_btn->setEnabled(false);
*/

  connect(ui->factory_alarm_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  ui->factory_alarm_ok_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_alarm_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  ui->factory_alarm_ok_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_OK));
  ui->factory_alarm_fail_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_FAIL));

  connect(ui->factory_alarm_ok_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_alarm()));
  connect(ui->factory_alarm_fail_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_alarm()));

  ui->factory_alarm_ok_btn->hide();
  ui->factory_alarm_fail_btn->hide();

  ui->alarm_tip_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->alarm_tip_label->setWordWrap(true);
  ui->alarm_tip_label->setText(
      tr("\n\n\n  3秒后自动报警，报警2秒结束"
      "请聆听并确认是否有报警语音"));

  memset(&alarm_param, 0, sizeof(alarm_service_t));
  alarming_test(&alarm_param);
  ptimer = new QTimer;
  connect(ptimer, SIGNAL(timeout()), this, SLOT(alarming_stopped()));
  ptimer->start(4000);
}

ui_factory_alarm::~ui_factory_alarm() {
  delete ptimer;
  delete ui;
}

void ui_factory_alarm::slot_for_back_btn() {
  alarming_test_stop(&alarm_param);

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_factory_alarm::alarming_stopped() {
  alarming_test_stop(&alarm_param);

  ui->factory_alarm_ok_btn->show();
  ui->factory_alarm_fail_btn->show();

  ptimer->stop();
}

