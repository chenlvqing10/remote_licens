/*
* ui_factory_rs485.cpp - define interface function of UI
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

#include "ui_factory_rs485.hpp"
#include "ui_ui_factory_rs485.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TIMER_INTERVAL_COUNT 1000
#define BUTTON_WIDTH 224
#define BUTTON_HEIGHT 72

#define LABEL_WIDTH 400
#define LABEL_HEIGHT 60

#define MAX_SEND_BUFFER_SIZE 256

ui_factory_rs485::ui_factory_rs485(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_rs485),
    test_result_(0),
    rs_handle(NULL),
    tip_label(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
                  ui->factory_rs485_back_btn->setEnabled(false);
*/

  connect(ui->factory_rs485_back_btn, SIGNAL(clicked()),
            this, SLOT(slot_for_back_btn()));

  ui->factory_rs485_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_rs485_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  connect(ui->factory_rs485_ok_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_rs485_ok()));
  connect(ui->factory_rs485_fail_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_rs485_fail()));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_function()));

  ui->rs485_result_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->rs485_result_label->setWordWrap(true);

  ui->rs485_result_label->
      setText(tr("  请观察接收方收到的数据是否正确,以确认RS485是否正常"));
  pTimer->start(TIMER_INTERVAL_COUNT);

  this->showMaximized();

  int x, y;
  int width = ui->rs485_result_label->frameGeometry().width();
  int height = ui->rs485_result_label->frameGeometry().height();
  y = (height - BUTTON_HEIGHT) / 2;

  tip_label = new QLabel(tr("发送数据：hello world"));
  tip_label->setStyleSheet("QLable{font-size:38px;color:#FFF;}");
  tip_label->setParent(ui->rs485_result_label);
  x = (width - LABEL_WIDTH) / 2;
  tip_label->setGeometry(x, y-LABEL_HEIGHT, LABEL_WIDTH, LABEL_HEIGHT);
  tip_label->show();

#if 1
  rs_handle = ui_control_rs_open("/dev/ttySLB2", RS_OPEN_WRITE_ONLY,
                                  9600, 8, 'N', 1, NULL, NULL);
#else
  rs_handle = ui_control_rs_open("/dev/ttySLB2", RS_OPEN_RDWR,
                                  9600, 8, 'N', 1, NULL, NULL);
#endif
}

ui_factory_rs485::~ui_factory_rs485() {
  delete ui;
  delete pTimer;

  if (tip_label) {
    delete tip_label;
    tip_label = NULL;
  }

  ui_control_rs_close(rs_handle);
}

void ui_factory_rs485::slot_for_call_function() {
  char buffer[MAX_SEND_BUFFER_SIZE];

  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s", "hello world");
  ui_control_rs_send_data(rs_handle, buffer, strlen(buffer));
}

void ui_factory_rs485::slot_for_back_btn() {
  pTimer->stop();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_factory_rs485::slot_for_rs485_ok() {
  pTimer->stop();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_rs485(0);
}

void ui_factory_rs485::slot_for_rs485_fail() {
  pTimer->stop();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_rs485(1);
}

