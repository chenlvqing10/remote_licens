/*
* ui_batch_register.cpp - define batch register function of main
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
#include "hotplug.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "ui_batch_register.hpp"
#include "ui_ui_batch_register.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TIMER_INTERVAL_COUNT 1000

ui_batch_register::ui_batch_register(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_batch_register),
    do_end_(1) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

  connect(ui->batch_register_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  connect(ui->batch_register_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_batch_register()));
  ui->register_percent_label->hide();

  pTimer = new QTimer(this);
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_update()));

  ui->register_percent_label->setText(tr(
             "总记录数:            0\n"
             "导入成功记录数：0\n"
             "失败记录数：       0"));
}

ui_batch_register::~ui_batch_register() {
  delete ui;
  delete pTimer;
}

void ui_batch_register::slot_for_batch_percent(int value) {
  QString value_str = QString("%1").arg(value);
  value_str.append("%");
  ui->register_percent_label->setText(value_str);
  if (value_str.compare(QString::fromLocal8Bit("100")) == 0) {
    ui->batch_register_btn->setEnabled(true);
    ui->batch_register_btn->setStyleSheet("background:#00d5cf");
  }
}

void ui_batch_register::slot_for_batch_register() {
  do_end_ = 0;
  ui->batch_register_btn->setEnabled(false);
  ui->batch_register_btn->setStyleSheet("background:#454A60");
  ui->register_percent_label->show();
  pTimer->start(TIMER_INTERVAL_COUNT);
  ui_control_person_import();
}

void ui_batch_register::slot_for_update() {
  char content[256];
  person_import_ctx_t info;
  int ret = ui_control_get_peron_import_status(&info);
  if (!ret) {
    snprintf(content, sizeof(content),
             tr("总记录数:            %5d\n"
             "导入成功记录数：%5d\n"
             "失败记录数：       %5d").toUtf8().data(),
             info.status.sum_count,
             info.status.current_count - info.status.fail_count,
             info.status.fail_count);
    ui->register_percent_label->setText(content);

    if (udisk_is_insert() != 1 || info.done == EE_THREAD_DONE) {
      do_end_ = 1;
      ui->batch_register_btn->setStyleSheet("background:#00d5cf");
      ui->batch_register_btn->setEnabled(true);
      pTimer->stop();
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
      QString result = QString(tr("导入完成，成功：%1，失败：%2"))
        .arg(info.status.current_count - info.status.fail_count)
        .arg(info.status.fail_count);
      w->set_content(result.toUtf8().data());
      w->show();
    }
  }
}

void ui_batch_register::slot_for_back_btn() {
  if (do_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_BATCH_IMPORT, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

