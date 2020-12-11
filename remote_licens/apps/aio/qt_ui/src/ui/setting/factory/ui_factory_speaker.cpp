/*
* ui_factory_speaker.cpp - define interface function of UI
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

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>

#include "ui_factory_speaker.hpp"
#include "ui_ui_factory_speaker.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TIMER_INTERVAL_COUNT 1000

ui_factory_speaker::ui_factory_speaker(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_speaker),
    test_end_(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
      ui->factory_speaker_back_btn->setEnabled(false);
*/

  connect(ui->factory_speaker_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  ui->factory_speaker_ok_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
  ui->factory_speaker_fail_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");

  connect(ui->factory_speaker_ok_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_speaker_ok()));
  connect(ui->factory_speaker_fail_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_speaker_fail()));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_funtion()));

  ui->factory_speaker_result_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->factory_speaker_result_label->setWordWrap(true);

  thread.set_test_ui(this);

  ui->factory_speaker_result_label->
      setText(tr("\n\n\n  通过播放声音测试扬声器是否正常工作"
              "请聆听并确认播放是否正常"));

  thread.start();
}

ui_factory_speaker::~ui_factory_speaker() {
  delete ui;
  delete pTimer;
}

void ui_factory_speaker::slot_for_call_funtion() {
  pTimer->stop();
  int ret = ui_control_test_speaker();
  if (ret == 0) {
    ui->factory_speaker_result_label->
        setText(tr("\n\n\n  通过播放声音测试扬声器是否正常工作"
                "请聆听并确认播放是否正常"));
  } else {
    ui->factory_speaker_result_label->
        setText(tr("\n\n\n  通过播放声音测试扬声器是否正常工作"
                "请聆听并确认播放是否正常"));
  }
}

void ui_factory_speaker::slot_for_back_btn() {
  if (test_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_TEST, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_factory_speaker::slot_for_speaker_ok() {
  if (test_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_TEST, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_speaker(0);
}

void ui_factory_speaker::slot_for_speaker_fail() {
  if (test_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_TEST, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_speaker(1);
}

void ui_factory_speaker::proc_func() {
  int ret = ui_control_test_speaker();
  ui->factory_speaker_result_label->
        setText(tr("\n\n\n  通过播放声音测试扬声器是否正常工作"
                "播放结束"));
  test_end_ = 1;
}

int ui_factory_speaker::is_test_end() {
  return test_end_;
}

