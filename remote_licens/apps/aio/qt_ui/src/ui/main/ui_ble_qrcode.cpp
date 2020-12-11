/*
* ui_ble_qrcode.cpp - define ble qrcode of ui
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

#include <QDebug>

#include "ui_ble_qrcode.hpp"
#include "ui_ui_ble_qrcode.h"
#include "ui_home_entering.hpp"
#include "ui_rotate_management.hpp"

#include "signal_for_ui.hpp"
#include "signal_table.hpp"

#define BUF_LEN 64
#define MAX_BUF_SIZE 256
#define MAX_SN_LEN 24

ui_ble_qrcode::ui_ble_qrcode(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_ble_qrcode) {
  ui->setupUi(this);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  this->setWindowFlags(Qt::FramelessWindowHint);

  ui->sn_label->setAlignment(Qt::AlignCenter);
  ui->tip_label->setAlignment(Qt::AlignCenter);
  ui->network_label->setAlignment(Qt::AlignCenter);
  ui->ble_qrcode_label->setAlignment(Qt::AlignCenter);

  char sn[BUF_LEN], str[MAX_BUF_SIZE];
  memset(sn, 0, sizeof(sn));
  ui_control_get_sn(sn, MAX_SN_LEN);
  snprintf(str, sizeof(str), "SN:%s", sn);
  ui->sn_label->setText(str);
  // ui->sn_label->adjustSize();
  ui->tip_label->setText(tr("暂不支持5G无线网络"));
  ui->network_label->setStyleSheet("color:#28B1B7");
  ui->network_label->setText(tr("配置网络"));
  ui->ble_qrcode_label->setText(tr("使用企业微信扫描二维码"));
  ui->ble_qrcode_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/ble.bmp)}");

  this->showMaximized();
}

ui_ble_qrcode::~ui_ble_qrcode() {
  delete ui;
}

void ui_ble_qrcode::slot_for_show_ble_qrcode(int type) {
  ui->ble_qrcode_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/ble.bmp)}");
}

