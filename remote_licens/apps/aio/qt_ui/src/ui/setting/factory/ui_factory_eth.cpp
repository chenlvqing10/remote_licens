/*
* ui_factory_eth.cpp - define factory test eth interface function of UI
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

#include "ui_factory_eth.hpp"
#include "ui_ui_factory_eth.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TIMER_INTERVAL_COUNT 1000

ui_factory_eth::ui_factory_eth(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_factory_eth),
    test_end_(1) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

/*
  if (rotate_signgleton->auto_test.auto_test_run)
            ui->factory_net_back_btn->setEnabled(false);
*/

  connect(ui->factory_net_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_back_btn()));

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_function()));

  ui->factory_net_result_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  ui->factory_net_result_label->setWordWrap(true);

  thread.set_test_ui(this);

  ui->factory_net_result_label->
    setText(tr("\n\n\n  通过自动联网验证以太网测试是否正常工作"
            "正在尝试数据传输,请稍等..."));
  pTimer->start(TIMER_INTERVAL_COUNT);

  thread.start();
}

ui_factory_eth::~ui_factory_eth() {
  delete ui;
  if (pTimer)
    delete pTimer;
}

void ui_factory_eth::slot_for_factory_net_test() {
  ui->factory_net_result_label->
    setText(tr("\n\n\n  通过自动联网验证以太网测试是否正常工作"
            "正在尝试数据传输,请稍等..."));
  pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_factory_eth::slot_for_call_function() {
  ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();
  if (test_end_) {
    pTimer->stop();
    if (test_result_ == 0) {
      ui->factory_net_result_label->
          setText(tr("\n\n\n  通过自动联网验证以太网测试是否正常工作"
                  "测试结果:测试正常"));
    } else {
      ui->factory_net_result_label->
          setText(tr("\n\n\n  通过自动联网验证以太网测试是否正常工作"
                  "测试结果:测试异常"));
    }

    QTimer *delay_timer = new QTimer;
    connect(delay_timer, SIGNAL(timeout()), this, SLOT(slot_for_exit()));
    delay_timer->start(1000);
  }
}

void ui_factory_eth::slot_for_back_btn() {
  if (test_end_ == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_TEST, this);
    w->show();
    return;
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->slot_for_factory_net(test_result_);
}

void ui_factory_eth::proc_func() {
  test_result_ = 1;
  test_end_ = 0;
  int ret = ui_control_test_eth();
  if (ret == 0) {
    ret = ui_control_test_gmac();
    if (ret == 0) {
      test_result_ = 0;
    }
  }
  test_end_ = 1;
}

int ui_factory_eth::is_test_end() {
  return test_end_;
}

void ui_factory_eth::slot_for_exit() {
  // usleep(1000 * 1000);
  slot_for_back_btn();
}

