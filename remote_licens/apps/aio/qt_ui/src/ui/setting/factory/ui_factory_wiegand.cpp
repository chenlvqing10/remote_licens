/*
* ui_factory_wiegand.cpp - define interface function of UI
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
#include <QDebug>

#include "ui_factory_wiegand.hpp"
#include "ui_ui_factory_wiegand.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TIMER_INTERVAL_COUNT 1000
#define BUTTON_WIDTH 224
#define BUTTON_HEIGHT 72

#define LABEL_WIDTH 400
#define LABEL_HEIGHT 60

#define WIEGAND_SEND_DATA (0xaa55ff00)
#define WIEGAND_SEND_DATA_LEN 34

ui_factory_wiegand::ui_factory_wiegand(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_wiegand),
    test_result_(0),
    tip_label(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
                        ui->factory_wiegand_back_btn->setEnabled(false);
*/

  connect(ui->factory_wiegand_back_btn, SIGNAL(clicked()),
            this, SLOT(slot_for_back_btn()));

  ui->factory_wiegand_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_wiegand_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  connect(ui->factory_wiegand_ok_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_wiegand_ok()));
  connect(ui->factory_wiegand_fail_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_wiegand_fail()));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_function()));

  ui->wiegand_result_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->wiegand_result_label->setWordWrap(true);

  ui->wiegand_result_label->
      setText(tr("  请观察接收方收到的数据是否正确,以确认韦根设备是否正常"));
  pTimer->start(TIMER_INTERVAL_COUNT);

  this->showMaximized();

  int x, y;
  int width = ui->wiegand_result_label->frameGeometry().width();
  int height = ui->wiegand_result_label->frameGeometry().height();
  y = (height - BUTTON_HEIGHT) / 2;

  tip_label = new QLabel(tr("发送数据：0xaa55ff00"));
  tip_label->setStyleSheet("QLable{font-size:38px;color:#FFF;}");
  tip_label->setParent(ui->wiegand_result_label);
  x = (width - LABEL_WIDTH) / 2;
  tip_label->setGeometry(x, y-LABEL_HEIGHT, LABEL_WIDTH, LABEL_HEIGHT);
  tip_label->show();
}

ui_factory_wiegand::~ui_factory_wiegand() {
  delete ui;
  delete pTimer;

  if (tip_label) {
    delete tip_label;
    tip_label = NULL;
  }
}

void ui_factory_wiegand::slot_for_call_function() {
  ui_control_wg_write(WIEGAND_SEND_DATA_LEN, WIEGAND_SEND_DATA);
}

void ui_factory_wiegand::slot_for_back_btn() {
  pTimer->stop();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_factory_wiegand::slot_for_wiegand_ok() {
  pTimer->stop();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_wiegand(0);
}

void ui_factory_wiegand::slot_for_wiegand_fail() {
  pTimer->stop();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_wiegand(1);
}

