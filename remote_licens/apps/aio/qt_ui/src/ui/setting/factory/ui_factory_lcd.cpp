/*
* ui_factory_lcd.cpp - define interface function of UI
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

#include "ui_factory_lcd.hpp"
#include "ui_ui_factory_lcd.h"
#include "ui_rotate_management.hpp"
#include <QStandardItemModel>
#include <QObject>
#include <QDebug>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */


ui_factory_lcd::ui_factory_lcd(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_factory_lcd), index(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
      ui->factory_lcd_back_btn->setEnabled(false);
*/

  connect(ui->factory_lcd_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  ui->factory_lcd_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_lcd_fail_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");
  ui->factory_lcd_ok_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_OK));
  ui->factory_lcd_fail_btn->setProperty("id",
    QString("%1").arg(TEST_STATUS_FAIL));

  connect(ui->factory_lcd_ok_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_lcd()));
  connect(ui->factory_lcd_fail_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_lcd()));

  ui->factory_lcd_ok_btn->hide();
  ui->factory_lcd_fail_btn->hide();

  ui->factory_lcd_tip_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->factory_lcd_tip_label->setWordWrap(true);

  // timer of updating time
  ptimer = new QTimer;
  connect(ptimer, SIGNAL(timeout()), this, SLOT(slot_for_updatet_color()));
  ptimer->start(1000);

  ui->factory_lcd_tip_label->setText(
        tr("\n  大色块水平进入，垂直进入，请细心检查屏幕是否有色彩或坏点\n"));
}

ui_factory_lcd::~ui_factory_lcd() {
  delete ui;
  delete ptimer;
}

void ui_factory_lcd::slot_for_updatet_color() {
  switch (index) {
    case 0:
      this->setStyleSheet("background-color:white");
      break;

    case 1:
      this->setStyleSheet("background-color:red;");
      break;

    case 2:
      this->setStyleSheet("background-color:green;");
      break;

    case 3:
      this->setStyleSheet("background-color:black;");
      break;

    case 4:
      this->setStyleSheet("background-color:blue;");
      ui->factory_lcd_ok_btn->show();
      ui->factory_lcd_fail_btn->show();
      ui->factory_lcd_tip_label->setText(tr(
        "  大色块水平进入，垂直进入，请细心检查屏幕是否有色彩或坏点\n\n\n\n\n"
        "  屏幕上是否存在3个及3个以上坏点或者显示颜色严重偏差？"));
      ptimer->stop();
      break;
  }
  this->show();
  index++;
}

