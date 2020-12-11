/*
* ui_factory_touch.cpp - define factory test touch interface function of UI
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

#include "ui_factory_touch.hpp"
#include "ui_ui_factory_touch.h"
#include "ui_rotate_management.hpp"

#define MAX_TEST_COUNT 5
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 100
#define OFFSET_SIZE 10

#define TIMER_INTERVAL_COUNT 100
#define LABEL_HEIGHT 120

ui_factory_touch::ui_factory_touch(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_touch),
    touch_btn(NULL),
    index(0),
    test_result(-1) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
                      ui->factory_touch_back_btn->setEnabled(false);
*/

  connect(ui->factory_touch_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  this->showMaximized();

  int width = ui->touch_result_label->frameGeometry().width();
  int height = ui->touch_result_label->frameGeometry().height();

  tip_label = new QLabel("");
  tip_label->setParent(ui->touch_result_label);
  tip_label->setAlignment(Qt::AlignCenter);
  tip_label->setText(tr("请点击屏幕左上角的×按钮开始测试"));
  tip_label->setGeometry(0, 0, width, LABEL_HEIGHT);
  tip_label->show();

  touch_btn = new QPushButton("");
  touch_btn->setStyleSheet(
        "QPushButton{border-image:url(:/icon/cross.png)}");
  touch_btn->setParent(ui->touch_result_label);
  touch_btn->setGeometry(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);
  touch_btn->show();

  connect(touch_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_touch_btn()));
}

ui_factory_touch::~ui_factory_touch() {
  delete ui;
  if (touch_btn) {
    delete touch_btn;
    touch_btn = NULL;
  }

  if (tip_label) {
    delete tip_label;
    tip_label = NULL;
  }
}

void ui_factory_touch::update_touch_btn_position() {
  int x = 0, y = 0, is_hide = 0;
  int width = ui->touch_result_label->frameGeometry().width();
  int height = ui->touch_result_label->frameGeometry().height();
  switch (index) {
    case 0:
      x = OFFSET_SIZE; y = OFFSET_SIZE;
      tip_label->setText(tr("请点击屏幕左上角的×按钮开始测试"));
      break;

    case 1:
      x = width - BUTTON_WIDTH - OFFSET_SIZE; y = OFFSET_SIZE;
      tip_label->setText(tr("请点击屏幕右上角的×按钮继续测试"));
      break;

    case 2:
      x = OFFSET_SIZE; y = height - BUTTON_HEIGHT - OFFSET_SIZE;
      tip_label->setText(tr("请点击屏幕左下角的×按钮继续测试"));
      break;

    case 3:
      x = width - BUTTON_WIDTH - OFFSET_SIZE;
      y = height - BUTTON_HEIGHT - OFFSET_SIZE;
      tip_label->setText(tr("请点击屏幕右下角的×按钮继续测试"));
      break;

    case 4:
      x = (width - BUTTON_WIDTH) / 2; y = (height - BUTTON_HEIGHT) / 2;
      tip_label->setText(tr("请点击屏幕中间的×按钮继续测试"));
      break;

    case 5:
      is_hide = 1;
      break;
  }

  touch_btn->setGeometry(x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
  if (is_hide) {
    touch_btn->hide();
  } else {
    touch_btn->show();
  }
}

void ui_factory_touch::slot_for_touch_btn() {
  index++;
  update_touch_btn_position();
  if (index >= MAX_TEST_COUNT) {
    test_result = 0;
    tip_label->setText(tr("测试结束,测试结果：正常"));
    QTimer *delay_timer = new QTimer;
    connect(delay_timer, SIGNAL(timeout()), this, SLOT(slot_for_exit()));
    delay_timer->start(1000);
  }
}

void ui_factory_touch::slot_for_back_btn() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_touch(test_result);
}

void ui_factory_touch::slot_for_exit() {
  // usleep(1000 * 1000);
  slot_for_back_btn();
}

