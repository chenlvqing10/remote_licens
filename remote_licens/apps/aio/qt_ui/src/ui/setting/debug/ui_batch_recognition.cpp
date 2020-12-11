/*
* ui_batch_recognition.cpp - define batch recognition function of main
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

#include "ui_batch_recognition.hpp"
#include "ui_ui_batch_recognition.h"
#include "ui_tip_dialog.hpp"

#define MAX_BUFFER_SIZE 256
#define TIMER_INTERVAL_COUNT 1000

ui_batch_recognition::ui_batch_recognition(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_batch_recognition),
    do_end_(1),
    processing_(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

  ui->recognition_percent_label->setAlignment(Qt::AlignCenter);
  ui->recognition_tip_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->recognition_tip_label->setWordWrap(true);
  ui->recognition_tip_label->setText(tr(
    "\n\n\n  第一步：按说明书上准备好批量识别的excel文件\n"
    "  第二步：拷贝excel文件到U盘根目录\n"
    "  第三步：点击下面识别按钮\n"));

  connect(ui->batch_recognition_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  connect(ui->batch_recognition_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_batch_recognition()));
  ui->recognition_percent_label->hide();

  pTimer = new QTimer(this);
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_update()));
  pTimer->start(1000);

}

ui_batch_recognition::~ui_batch_recognition() {
  delete pTimer;
  delete ui;
}

void ui_batch_recognition::slot_for_batch_percent(int value) {
  QString value_str = QString("%1").arg(value);
  value_str.append("%");
  ui->recognition_percent_label->setText(value_str);
  if (value_str.compare(QString::fromLocal8Bit("100")) == 0) {
    ui->batch_recognition_btn->setEnabled(true);
    ui->batch_recognition_btn->setStyleSheet("background:#00d5cf");
  }
}

void ui_batch_recognition::slot_for_batch_recognition() {
  do_end_ = 0;
#if 1
  if (0 == processing_) {
    rotate->set_setting_count_state(TIMER_COUNTS_STOP);
    ui->batch_recognition_btn->setText(tr("终止"));
    ui->recognition_percent_label->show();
    pTimer->start(TIMER_INTERVAL_COUNT);
    ui->recognition_percent_label->
        setText(tr(
             "总记录数:            0\n"
             "识别成功记录数：0\n"
             "失败记录数：       0"));
    ui_control_person_recg();
    processing_ = 1;
  } else {
    ui_tip_dialog *w =
      new ui_tip_dialog(TIP_WIN_TYPE_EXIT_RECOGNITION_PROCESS, this);
    w->show();
  }
#else
  ui->batch_recognition_btn->setEnabled(false);
  ui->batch_recognition_btn->setStyleSheet("background:#454A60");
  ui->recognition_percent_label->show();
  pTimer->start(TIMER_INTERVAL_COUNT);
  ui->recognition_percent_label->
    setText(tr(
             "总记录数:            0\n"
             "识别成功记录数：0\n"
             "失败记录数：       0"));
  ui_control_person_recg();
#endif
}

void ui_batch_recognition::slot_for_update() {
  char content[MAX_BUFFER_SIZE];
  person_reg_check_ctx_t info;
  memset(&info, 0, sizeof(person_reg_check_ctx_t));
  int ret = ui_control_get_person_recg_status(&info);
  if (!ret) {
    snprintf(content, sizeof(content),
             tr("总记录数:            %5d\n"
             "识别成功记录数：%5d\n"
             "失败记录数：       %5d").toUtf8().data(),
             info.status.sum_count,
             info.status.current_count - info.status.fail_count,
             info.status.fail_count);
    ui->recognition_percent_label->setText(content);
    if (info.done == EE_THREAD_DONE) {
      do_end_ = 1;
#if 1
      processing_ = 0;
      ui->batch_recognition_btn->setText(tr("识别"));
#else
      ui->batch_recognition_btn->setStyleSheet("background:#00d5cf");
      ui->batch_recognition_btn->setEnabled(true);
#endif
      pTimer->stop();
      rotate->set_setting_count_state(TIMER_COUNTS_START);
    }
  }
}

void ui_batch_recognition::slot_for_back_btn() {
  if (do_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_BATCH_IMPORT, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

