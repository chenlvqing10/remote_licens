/*
* ui_modify_password.cpp - define modify password function of UI
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

#include <QStandardItemModel>
#include <QDebug>

#include "ui_modify_password.hpp"
#include "ui_ui_modify_password.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define LINE_EDIT_WIDTH 300
#define FIELD_VALUE_SIZE 32
#define MAX_FIELD_VALUE_LEN 64
#define MAX_PASSWORD_LEN 16
#define MIN_PASSWORD_LEN 6
#define CURRENT_TMP_LEN 2
#define MD5_BUFFER_LEN 16

ui_modify_password::ui_modify_password(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_modify_password),
    old_passwd_edt(NULL),
    new_passwd_edt(NULL),
    new_passwd_confirm_edt(NULL),
    pModel(NULL),
    pItemDelegate(NULL) {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);
  this->setWindowFlags(Qt::FramelessWindowHint);

  QStringList net_style;
  net_style << tr("旧密码") << tr("新密码") << tr("确认密码");
  // QStandardItemModel *pModel = new QStandardItemModel();
  pModel = new QStandardItemModel();
  for (int i = 0; i < net_style.size(); ++i) {
    QStandardItem *pItem = new QStandardItem;
    MuItemData_password itemData;
    itemData.name = net_style.at(i);

    switch (i) {
      case 0:
        old_passwd_edt = new QLineEdit("");
        old_passwd_edt->setEchoMode(QLineEdit::Password);
        old_passwd_edt->setProperty("id", QString("%1").arg(i));
        old_passwd_edt->setAlignment(Qt::AlignHCenter);
        old_passwd_edt->setParent(ui->modify_password_listView);
        itemData.widget = old_passwd_edt;
        itemData.width = LINE_EDIT_WIDTH;
        break;

      case 1:
        new_passwd_edt = new QLineEdit("");
        new_passwd_edt->setEchoMode(QLineEdit::Password);
        new_passwd_edt->setProperty("id", QString("%1").arg(i));
        new_passwd_edt->setAlignment(Qt::AlignHCenter);
        new_passwd_edt->setParent(ui->modify_password_listView);
        itemData.widget = new_passwd_edt;
        itemData.width = LINE_EDIT_WIDTH;
        break;

      case 2:
        new_passwd_confirm_edt = new QLineEdit("");
        new_passwd_confirm_edt->setEchoMode(QLineEdit::Password);
        new_passwd_confirm_edt->setProperty("id", QString("%1").arg(i));
        new_passwd_confirm_edt->setAlignment(Qt::AlignHCenter);
        new_passwd_confirm_edt->setParent(ui->modify_password_listView);
        itemData.widget = new_passwd_confirm_edt;
        itemData.width = LINE_EDIT_WIDTH;
        break;
    }
    pItem->setData(QVariant::fromValue(itemData), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }

  // ui_modify_password_delegate *pItemDelegate =
  pItemDelegate = new ui_modify_password_delegate(this);
  ui->modify_password_listView->setItemDelegate(pItemDelegate);
  ui->modify_password_listView->setModel(pModel);

  connect(ui->modify_password_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  connect(ui->modify_password_save_btn, SIGNAL(clicked()),
          this, SLOT(set_for_modify_password_save()));

  ui->modify_password_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_modify_password::~ui_modify_password() {
  ALOGD("ui_modify_password delete");
  delete ui;

  if (old_passwd_edt) {
    delete old_passwd_edt;
    old_passwd_edt = NULL;
  }

  if (new_passwd_edt) {
    delete new_passwd_edt;
    new_passwd_edt = NULL;
  }

  if (new_passwd_confirm_edt) {
    delete new_passwd_confirm_edt;
    new_passwd_confirm_edt = NULL;
  }

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }
}

int ui_modify_password::check_str_is_validate(const char *str) {
  if (NULL == str) {
    return -1;
  }

  if ((strlen(str) > MAX_PASSWORD_LEN) || (strlen(str) < MIN_PASSWORD_LEN)) {
    return -1;
  }

  for (int i = 0; i < strlen(str); i++) {
    if ((*(str + i) < '0') || (*(str + i) > '9')) {
      return -1;
    }
  }

  return 0;
}

void ui_modify_password::set_for_modify_password_save() {
  int ret;
  char md5_pwd[MAX_FIELD_VALUE_LEN];
  char passwd[MAX_FIELD_VALUE_LEN];
  char old_pwd[FIELD_VALUE_SIZE];
  char new_pwd[FIELD_VALUE_SIZE];
  char confirm_pwd[FIELD_VALUE_SIZE];
  ui_tip_dialog *w;

  memset(passwd, 0, sizeof(passwd));
  memset(old_pwd, 0, sizeof(old_pwd));
  memset(new_pwd, 0, sizeof(new_pwd));
  memset(confirm_pwd, 0, sizeof(confirm_pwd));

  if (old_passwd_edt->text().isEmpty() ||
    new_passwd_edt->text().isEmpty() ||
    new_passwd_confirm_edt->text().isEmpty()) {
    w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(
      tr("密码不能为空！").toUtf8().data());
    w->show();
    return;
  }

  snprintf(old_pwd, sizeof(old_pwd),
    "%s", old_passwd_edt->text().toStdString().c_str());
  snprintf(new_pwd, sizeof(new_pwd),
    "%s", new_passwd_edt->text().toStdString().c_str());
  snprintf(confirm_pwd, sizeof(confirm_pwd),
    "%s", new_passwd_confirm_edt->text().toStdString().c_str());
  if (strcmp(old_pwd, new_pwd) == 0) {
    w = new ui_tip_dialog(TIP_WIN_TYPE_OLD_NEW_SAME_ERROR, this);
    w->show();
    return;
  }

  ui_control_get_password(passwd, sizeof(passwd));

  memset(md5_pwd, 0, sizeof(md5_pwd));
  get_md5_string(old_pwd, md5_pwd, sizeof(md5_pwd));
  ALOGD("%s[%d] old pwd %s md5_pwd %s ",
        __func__, __LINE__, old_pwd, md5_pwd);
  // if (strcmp(old_pwd, passwd)) {
  if (strcmp(md5_pwd, passwd)) {
    w = new ui_tip_dialog(TIP_WIN_TYPE_OLD_PWD_ERROR, this);
    w->show();
    // old_passwd_edt->setFocus();
    return;
  }

  if (check_str_is_validate(new_pwd)) {
    w = new ui_tip_dialog(TIP_WIN_TYPE_INPUT_PWD_ERROR, this);
    w->show();
    // new_passwd_edt->setFocus();
    return;
  }

  if (strcmp(new_pwd, confirm_pwd)) {
    w = new ui_tip_dialog(TIP_WIN_TYPE_INPUT_PWD_DISAGREE, this);
    w->show();
    // new_passwd_confirm_edt->setFocus();
    return;
  }

  memset(md5_pwd, 0, sizeof(md5_pwd));
  get_md5_string(new_pwd, md5_pwd, sizeof(md5_pwd));
  // ret = ui_control_set_password(new_pwd);
  ret = ui_control_set_password(md5_pwd);
  if (0 == ret) {
    w = new ui_tip_dialog(TIP_WIN_TYPE_PWD_SAVE_SUCCESS, this);
  } else {
    w = new ui_tip_dialog(TIP_WIN_TYPE_SAVE_FAIL, this);
  }
  w->show();
}

