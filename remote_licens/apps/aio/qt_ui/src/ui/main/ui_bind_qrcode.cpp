/*
* ui_bind_qrcode.cpp - define wechat bind qrcode ui of webchat
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
#include <QMouseEvent>

#include "ui_bind_qrcode.hpp"
#include "ui_ui_bind_qrcode.h"
#include "ui_rotate_management.hpp"

#include "signal_for_ui.hpp"
#include "signal_table.hpp"

#define BUF_LEN 64
#define MAX_BUF_SIZE 256
#define MAX_SN_LEN 24

/* update qrcode time (s) */
#define QRCODE_UPDATE_TIME 300

#define QRCODE_ICON_WIDTH 324
#define QRCODE_ICON_HEIGHT 324

static const int enter_face_input_window = 1;
static const int exit_face_input_window = 2;
static const int enter_menu_window = 3;
static const int generate_qr_code = 4;

ui_bind_qrcode::ui_bind_qrcode(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_bind_qrcode) {
  ui->setupUi(this);
  // ui->login_btn->close();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  this->setWindowFlags(Qt::FramelessWindowHint);

  QIcon icon;
  char path[MAX_BUF_SIZE] = "/data/wechat.bmp";

  if (ui_get_qrcode_path(path) == 0) {
    QString wechat_url(path);
    icon.addFile(wechat_url);
    ui->webchat_btn->setIcon(icon);
    ui->webchat_btn->setIconSize(
        QSize(QRCODE_ICON_WIDTH, QRCODE_ICON_HEIGHT));
  }

  signal_t = reinterpret_cast<SignalTable *>(signal_get_handle());
  connect(signal_t, SIGNAL(signalForWebSocket(int)),
          this, SLOT(slot_for_show_webchat(int)));

  ui->wechat_label->setAlignment(Qt::AlignCenter);
  ui->login_btn->hide();
  // ui->login_btn->setDisable();

  ui->sn_label->setAlignment(Qt::AlignCenter);
  ui->weixin_label->setAlignment(Qt::AlignCenter);

  char sn[BUF_LEN], str[MAX_BUF_SIZE];
  memset(sn, 0, sizeof(sn));
  ui_control_get_sn(sn, MAX_SN_LEN);
  snprintf(str, sizeof(str), "SN:%s", sn);
  ui->sn_label->setText(str);
  // ui->sn_label->adjustSize();
  ui->weixin_label->setStyleSheet("color:#28B1B7");
  ui->weixin_label->setText(tr("绑定企业"));
  ui->wechat_label->setText(tr("使用企业微信扫描二维码"));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_funtion()));
  pTimer->start(1000 * QRCODE_UPDATE_TIME);
  this->showMaximized();
}

ui_bind_qrcode::~ui_bind_qrcode() {
  delete ui;
  delete pTimer;
}

void ui_bind_qrcode::deinit() {
  QtMainWindow::deinit();

  disconnect(signal_t, SIGNAL(signalForWebSocket(int)),
             this, SLOT(slot_for_show_webchat(int)));

  pTimer->stop();
}

void ui_bind_qrcode::slot_for_show_webchat(int type) {
  QIcon icon;
  char path[MAX_BUF_SIZE] = "/data/wechat.bmp";
  if (type == WEB_SOCKET_EVENT_QRCODE) {
    if (ui_get_qrcode_path(path) == 0) {
      QString wechat_url(path);
      icon.addFile(wechat_url);
      ui->webchat_btn->setIcon(icon);
      ui->webchat_btn->setIconSize(
          QSize(QRCODE_ICON_WIDTH, QRCODE_ICON_HEIGHT));
      ALOGD("ui_bind_qrcode::slot_for_show_webchat() %s", path);
    }
  }
}

void ui_bind_qrcode::slot_for_call_funtion() {
  QIcon icon;
  char path[MAX_BUF_SIZE] = "/data/wechat.bmp";
  if (ui_get_qrcode_path(path) == 0) {
    QString wechat_url(path);
    icon.addFile(wechat_url);
    ui->webchat_btn->setIcon(icon);
    ui->webchat_btn->setIconSize(
        QSize(QRCODE_ICON_WIDTH, QRCODE_ICON_HEIGHT));
  }

  ALOGD("slot_for_call_funtion() %s", path);
}

