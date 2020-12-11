/*
* ui_factory_usb.cpp - define factory test usb interface function of UI
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
#include <hotplug.h>
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>

#include "ui_factory_usb.hpp"
#include "ui_ui_factory_usb.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"
#include "signal_table.hpp"

#define TIMER_INTERVAL_COUNT 1000

ui_factory_usb::ui_factory_usb(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_usb),
    test_end_(0),
    test_result_(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

  connect(ui->factory_usb_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_function()));

  ui->factory_usb_result_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->factory_usb_result_label->setWordWrap(true);

  thread.set_test_ui(this);

  QString str = tr("\n\n\n  自动往U盘写入一个文件，然后删除判断USB是否正常");
  if (udisk_is_ok())
    str = str + tr("\n  请插入U盘！");
  else
    thread.start();
  ui->factory_usb_result_label->setText(str);
  pTimer->start(TIMER_INTERVAL_COUNT);

  SignalTable *signal_table =
    reinterpret_cast<SignalTable *>(signal_get_handle());
  connect(signal_table, SIGNAL(signalForStorage(int, int)),
          this, SLOT(slot_for_storage(int, int)));
}

ui_factory_usb::~ui_factory_usb() {
  delete ui;
  delete pTimer;
}

void ui_factory_usb::slot_for_storage(int event, int type) {
  if (event == STORAGE_EVENT_UDISK) {
    if (type == STORAGE_TYPE_CONNECT) {
      thread.start();
    }
    if (type == STORAGE_TYPE_DISCONN) {
      thread.quit();
      thread.wait();
      if (test_end_ == 0)
        ui->factory_usb_result_label->setText(tr("\n\n\n  测试异常中断"));
    }
  }
}

void ui_factory_usb::slot_for_call_function() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  if (test_end_) {
    pTimer->stop();
    if (test_result_ == 1) {
      ui->factory_usb_result_label->
          setText(tr("\n\n\n  自动往U盘写入一个文件，然后删除判断USB是否正常"
                  "\n  测试结果:正常"));
    } else {
      ui->factory_usb_result_label->
          setText(tr("\n\n\n  自动往U盘写入一个文件，然后删除判断USB是否正常"
                  "\n  测试结果:异常"));
    }

    QTimer *delay_timer = new QTimer;
    connect(delay_timer, SIGNAL(timeout()), this, SLOT(slot_for_exit()));
    delay_timer->start(1000);
  }
}

void ui_factory_usb::slot_for_back_btn() {
  if (thread.isRunning()) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_TEST, this);
    w->show();
    return;
  }
  SignalTable *signal_table =
    reinterpret_cast<SignalTable *>(signal_get_handle());
  disconnect(signal_table, SIGNAL(signalForStorage(int, int)),
          this, SLOT(slot_for_storage(int, int)));
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_usb(test_result_);
}

void ui_factory_usb::proc_func() {
  int ret = -1;
  ret = ui_control_test_usb();
  if (ret == 0)
    test_result_ = 1;
  else {
    ALOGE("%s >> usb test failed!", __func__);
    test_result_ = 2;
  }
  test_end_ = 1;
}

int ui_factory_usb::is_test_end() {
  return test_end_;
}

void ui_factory_usb::slot_for_exit() {
  // usleep(1000 * 1000);
  slot_for_back_btn();
}

