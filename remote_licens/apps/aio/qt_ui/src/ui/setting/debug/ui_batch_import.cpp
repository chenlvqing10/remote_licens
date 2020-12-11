/*
* ui_batch_import.cpp - define batch import function of main
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

#include <QDebug>

#include "ui_batch_import.hpp"
#include "ui_ui_batch_import.h"
#include "ui_tip_dialog.hpp"
#include "screen_size.hpp"

#define MAX_BUFFER_SIZE 256
#define TIMER_INTERVAL_COUNT 1000

ui_batch_import::ui_batch_import(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_batch_import),
    do_end_(1),
    processing_(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  rotate = ui_rotate_management::getInstance();
  ui->setupUi(this);
#if 0
  screen_size sc;
  int l_width = sc.get_screen_width();
  int l_height = sc.get_screen_height();
  qDebug() << "width " << l_width << " height " << l_height;
  if (l_width > l_height) {
    ui->label11->hide();
    ui->label12->hide();
  }
#endif
  ui->import_percent_label->setAlignment(Qt::AlignCenter);
  ui->import_tip_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->import_tip_label->setWordWrap(true);
  ui->import_tip_label->setText(tr(
    "\n"
    "第一步：按说明书上准备好批量导入的excel文件\n\n"
    "第二步：拷贝excel文件到U盘根目录\n\n"
    "第三步：点击下面导入按钮\n\n"));

  connect(ui->batch_import_back_btn, SIGNAL(clicked()),
           this, SLOT(slot_for_back_btn()));

  connect(ui->batch_import_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_batch_import()));
  ui->import_percent_label->hide();
  pTimer = new QTimer(this);
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_update()));
}

ui_batch_import::~ui_batch_import() {
  delete pTimer;
  delete ui;
}

void ui_batch_import::deinit() {
  QtMainWindow::deinit();
  rotate->set_recg_cnt();
}

void ui_batch_import::slot_for_batch_percent(int value) {
  QString value_str = QString("%1").arg(value);
  value_str.append("%");
  ui->import_percent_label->setText(value_str);
}

void ui_batch_import::slot_for_batch_import() {
  do_end_ = 0;
#if 1
  if (0 == processing_) {
    rotate->set_setting_count_state(TIMER_COUNTS_STOP);
    ui->batch_import_btn->setText(tr("终止"));
    ui->import_percent_label->show();
    pTimer->start(TIMER_INTERVAL_COUNT);
    ui_control_person_import();
    ui->import_percent_label->
        setText(tr(
             "总记录数:             0\n"
             "导入成功记录数： 0\n"
             "失败记录数：        0"));
    processing_ = 1;
  } else {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_EXIT_IMPORT_PROCESS, this);
    w->show();
  }
#else
  ui->batch_import_btn->setEnabled(false);
  ui->batch_import_btn->setStyleSheet("background:#454A60");
  ui->import_percent_label->show();

  pTimer->start(TIMER_INTERVAL_COUNT);
  ui_control_person_import();
  ui->import_percent_label->
    setText(tr(
             "总记录数:             0\n"
             "导入成功记录数： 0\n"
             "失败记录数：        0"));
#endif
}

void ui_batch_import::slot_for_update() {
  char content[MAX_BUFFER_SIZE];
  person_import_ctx_t info;
  ui_tip_dialog *w;

  memset(&info, 0, sizeof(person_import_ctx_t));
  int ret = ui_control_get_peron_import_status(&info);
  if (!ret) {
    snprintf(content, sizeof(content),
             tr("总记录数:             %5d\n"
             "导入成功记录数： %5d\n"
             "失败记录数：        %5d").toUtf8().data(),
             info.status.sum_count,
             info.status.current_count - info.status.fail_count,
             info.status.fail_count);
    ui->import_percent_label->setText(content);
    if (udisk_is_insert() != 1 || info.done == EE_THREAD_DONE) {
      do_end_ = 1;
#if 1
      processing_ = 0;
      ui->batch_import_btn->setText(tr("导入"));
#else
      ui->batch_import_btn->setStyleSheet("background:#00d5cf");
      ui->batch_import_btn->setEnabled(true);
#endif
      pTimer->stop();

      w = new ui_tip_dialog(TIP_WIN_TYPE_CONFRIM, this);
      QString result = QString(tr("导入完成，成功：%1，失败：%2"))
        .arg(info.status.current_count - info.status.fail_count)
        .arg(info.status.fail_count);
      w->set_content(result.toUtf8().data());
      w->show();
      rotate->set_setting_count_state(TIMER_COUNTS_START);
    }
  }
}

void ui_batch_import::slot_for_back_btn() {
  if (do_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_BATCH_IMPORT, this);
    w->show();
    return;
  }
  rotate->back_activity_win();
}

