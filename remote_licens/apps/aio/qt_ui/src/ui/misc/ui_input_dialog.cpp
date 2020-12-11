/*
* ui_input_dialog.cpp - define input dialog interface function of UI
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

#include <QObject>
#include <QDebug>

#include "ui_input_dialog.hpp"
#include "ui_ui_input_dialog.h"
#include "ui_rotate_management.hpp"

ui_input_dialog::ui_input_dialog(input_args_t *args, QWidget *parent) :
    QtDialog(parent),
    ui(new Ui::ui_input_dialog) {
  input_info = new input_args_t;
  input_info->id = args->id;
  input_info->title = args->title;
  input_info->default_input = args->default_input;
  input_info->echo_mode = args->echo_mode;
  input_info->max_len = args->max_len;
  input_info->cb = args->cb;

  this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();
  ui->setupUi(this);

  connect(ui->input_confirm_btn, SIGNAL(clicked()),
      this, SLOT(slot_for_confirm()));
  connect(ui->input_cancel_btn, SIGNAL(clicked()),
      this, SLOT(slot_for_cancel()));

  ui->input_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");
  ui->input_cancel_btn->setFocus();

  ui->tip_title_label->setAlignment(Qt::AlignCenter);
  ui->tip_title_label->setText(input_info->title);
  ui->input_lineedit->setText(input_info->default_input);

  showMaxLayout();
}

ui_input_dialog::~ui_input_dialog() {
  delete input_info;
  delete ui;
}

void ui_input_dialog::slot_for_confirm() {
  if (!input_info->cb)
    return;

  ui_tip_dialog *w_tip;
  QString tip = QString(tr("字符超出限制，最大支持%1个标准字符"))
      .arg(input_info->max_len);
  if (ui->input_lineedit->text().toUtf8().size() > input_info->max_len) {
    w_tip = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w_tip->set_content(tip.toUtf8().data());
    return;
  }

  input_info->cb->input_callback(input_info->id, ui->input_lineedit->text());

  deinit();
}

void ui_input_dialog::slot_for_cancel() {
  if (!input_info->cb)
    return;

  input_info->cb->input_callback(input_info->id, input_info->default_input);

  deinit();
}

void ui_input_dialog::deinit() {
  this->deleteLater();
}

