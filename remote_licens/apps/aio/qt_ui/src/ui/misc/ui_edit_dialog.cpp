/*
* ui_edit_dialog.cpp - define edit dialog interface function of UI
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
#endif
#define LOG_TAG     "ui"
#include <log/log.h>
#include "ui_control.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>

#include "ui_edit_dialog.hpp"
#include "ui_ui_edit_dialog.h"
#include "ui_rotate_management.hpp"

ui_edit_dialog::ui_edit_dialog(
  edit_args_t *args, QWidget *parent) :
  QtDialog(parent),
  ui(new Ui::ui_edit_dialog) {

  edit_info = new edit_args_t;
  edit_info->id = args->id;
  edit_info->title = QString(args->title);
  edit_info->key = QString(args->key);
  edit_info->cb = args->cb;

  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  ui->edt_title_label->setAlignment(Qt::AlignCenter);
  ui->edt_item_label_1->setText(tr("密 码"));
  ui->edt_title_label->setText(edit_info->title);
  ui->edt_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");
  connect(ui->edt_confirm_btn, SIGNAL(clicked()),
        this, SLOT(slot_for_confirm()));
  connect(ui->edt_cancel_btn, SIGNAL(clicked()),
        this, SLOT(deleteLater()));
  showMaxLayout();
}

ui_edit_dialog::~ui_edit_dialog() {
  if (edit_info) {
    delete edit_info;
    edit_info = NULL;
  }
  delete ui;
}

void ui_edit_dialog::slot_for_confirm() {
  // QString str_ssid = ui->edt_item_1->text();
  // QString str_pwd = ui->edt_item_2->text();
  //QString str_ssid = ui->edt_title_label->text();
  QString str_pwd = ui->edt_item_1->text();

  if (str_pwd.length() < 8) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST);
    w->set_content(tr("密码长度最小8个字符！").toUtf8().data());
    w->show();
    return;
  }

  if (edit_info) {
    edit_info->value = QString(str_pwd);
    edit_info->cb->edit_callback(edit_info);
  }
  deleteLater();
}

