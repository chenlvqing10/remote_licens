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
#include "db_all.h"

#include "ui_user_search_dialog.hpp"
#include "ui_ui_user_search_dialog.h"
#include "ui_rotate_management.hpp"

ui_user_search_dialog::ui_user_search_dialog(
  user_search_args_t *args, QWidget *parent) :QtDialog(parent),
  ui(new Ui::ui_user_search_dialog) {
  user_search_info = new user_search_args_t;
  user_search_info->cb = args->cb;
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  connect(ui->edt_confirm_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_confirm()));
  connect(ui->edt_cancel_btn, SIGNAL(clicked()),
              this, SLOT(deleteLater()));
  ui->edt_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");
  showMaxLayout();
}

ui_user_search_dialog::~ui_user_search_dialog() {
  delete user_search_info;
  delete ui;
}

void ui_user_search_dialog::slot_for_confirm() {
  QString str_id = ui->edt_item_1->text();
  QString str_name = ui->edt_item_2->text();
  if (str_id.isEmpty() && str_name.isEmpty()) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(tr("请输入工号或姓名！").toUtf8().data());
    w->show();
    return;
  } else {
    if (user_search_info->cb)
      user_search_info->cb->user_search_callback(str_id, str_name);
  }
  deleteLater();
}
