/*
* ui_password_login.cpp - define interface function of UI
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
#include "dd_md5_i.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>

#include "ui_password_login.hpp"
#include "ui_ui_password_login.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_main_menu.hpp"

#define FIELD_VALUE_SIZE 32
#define MAX_FIELD_VALUE_LEN 64
#define CURRENT_TMP_LEN 2
#define MD5_BUFFER_LEN 16

ui_password_login::ui_password_login(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_password_login) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  connect(ui->password_login_confirm_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_confirm()));

  connect(ui->password_login_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_bind_choose()));

#if 0
  ui->password_lineEdit->setText("123456");
#else
  ui->password_lineEdit->setText("");
#endif
  ui->password_lineEdit->setEchoMode(QLineEdit::Password);
  ui->password_login_confirm_btn->setFocus();
}

ui_password_login::~ui_password_login() {
  delete ui;
}

void ui_password_login::slot_for_confirm() {
  int ret;
  char pwd[FIELD_VALUE_SIZE];
  char md5_pwd[MAX_FIELD_VALUE_LEN];
  char password[MAX_FIELD_VALUE_LEN];
  QString str_pwd = ui->password_lineEdit->text();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  memset(pwd, 0, sizeof(pwd));
  memset(password, 0, sizeof(password));
  snprintf(pwd, sizeof(pwd), "%s", str_pwd.toUtf8().data());

  ret = ui_control_get_password(password, sizeof(password));
  if (0 == ret) {
    /* sysinfo table is exist password field */
    memset(md5_pwd, 0, sizeof(md5_pwd));
    get_md5_string(pwd, md5_pwd, sizeof(md5_pwd));
    ALOGD("%s[%d] md5_pwd %s", __func__, __LINE__, md5_pwd);
    // if (0 == strcmp(pwd, password)) {
    if (0 == strcmp(md5_pwd, password)) {
      rotate_signgleton->start_activity_win(
        &ui_main_menu::staticMetaObject);
      return;
    }
  } else {
    /* sysinfo table is not exist password field */
    if (0 == strcmp(pwd, "123456")) {
      rotate_signgleton->start_activity_win(
        &ui_main_menu::staticMetaObject);
      return;
    }
  }

  ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_LOGIN_ERROR, this);
  w->show();
}

