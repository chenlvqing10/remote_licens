/*
* ui_home_entering.cpp - define home entering function of main
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
#define LOG_TAG   "ui"
#include <log/log.h>
#include "ui_control.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>
#include <QTime>
#include <QDir>

#include "ui_home_entering.hpp"
#include "ui_ui_home_entering.h"
#include "ui_rotate_management.hpp"
#include "signal_for_ui.hpp"
#include "signal_table.hpp"

static const int enter_face_input_window = 1;
static const int exit_face_input_window = 2;
static const int enter_menu_window = 3;
static const int generate_qr_code = 4;

const char net_event_wired = 1;
const char net_event_wifi = 2;

const char net_type_disconnect = 1;
const char net_type_connected = 2;
//  const char net_type_scan_comp = 3;

ui_home_entering::ui_home_entering(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_home_entering) {
  ui->setupUi(this);
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();

  SignalTable *signal_table = NULL;
  signal_table = reinterpret_cast<SignalTable *> (signal_get_handle());
  QDateTime time = QDateTime::currentDateTime();

  QString str_show = time.toString("yyyy年MM月dd日 hh:mm");
  ui->home_clock_label->setText(str_show);

  QString curPath = QDir::currentPath();
  qDebug() << curPath;
  QIcon icon;
  char path[128] = "/data/wechat.bmp";
  QString wechat_url(path);
  icon.addFile(wechat_url);
  ui->wechat_btn->setIcon(icon);
  ui->wechat_btn->setIconSize(QSize(250, 250));

  ui->eth_icon_label->hide();
  ui->wifi_icon_label->hide();
  connect(signal_table, SIGNAL(signalForWebSocket(int)),
          this, SLOT(slot_for_show_webchat(int)));
  connect(ui->password_login_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_bind_choose()));
}

ui_home_entering::~ui_home_entering() {
  delete ui;
}

void ui_home_entering::slot_for_show_webchat(int type) {
  qDebug() << "slot_for_show_webchat";

  QIcon icon;
  char path[128] = "/data/wechat.bmp";
  QString curPath = QDir::currentPath();
  qDebug() << curPath;
  if (type == generate_qr_code) {
    if (ui_get_qrcode_path(path) == 0) {
      QString wechat_url(path);
      icon.addFile(wechat_url);
      ui->wechat_btn->setIcon(icon);
      ui->wechat_btn->setIconSize(QSize(250, 250));
    }
  }
}

void ui_home_entering::slot_for_net_status(int event, int type) {
  if ((net_event_wired == event) && (net_type_disconnect == type)) {
    ui->eth_icon_label->hide();
  } else if ((net_event_wired == event) && (net_type_connected == type)) {
    ui->eth_icon_label->show();
  } else if ((net_event_wifi == event) && (net_type_disconnect == type)) {
    ui->wifi_icon_label->hide();
  } else if ((net_event_wifi == event) && (net_type_connected == type)) {
    ui->wifi_icon_label->show();
  } else {
    ui->eth_icon_label->hide();
    ui->wifi_icon_label->hide();
  }
}

void ui_home_entering::slot_set_face_input_mode() {
  ui_face_set_login_mode();
}

