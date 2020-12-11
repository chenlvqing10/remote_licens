/*
* ui_network_setting.cpp - define interface function of network setting
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
#include <sys/prctl.h>

#include "file_ops.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>

#include <QDebug>
#include <QModelIndex>

#include "ui_network_setting.hpp"
#include "ui_ui_network_setting.h"
#include "ui_rotate_management.hpp"
#include "ui_wifi_list.hpp"
#include "ui_eth_setting.hpp"

void ui_network_setting::ui_init() {
  menu_network = {
    {":/icon/network_setting_wifi.png",
      tr("Wi-Fi设置"), "", EN_ITEM_LABEL},
    {":/icon/network_setting_eth.png",
      tr("网口设置"), "", EN_ITEM_LABEL}
  };
}

ui_network_setting::ui_network_setting(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_network_setting),
  pModel(NULL),
  pItemDelegate(NULL) {
  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();
  ui->setupUi(this);
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_init();
  pModel = new QStandardItemModel();
  int count = menu_network.count();
  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
    pItem->setData(QVariant::fromValue(menu_network[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_setting_item_delegate(this);
  ui->network_setting_listView->setItemDelegate(pItemDelegate);
  ui->network_setting_listView->setModel(pModel);
  connect(ui->network_setting_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_setting_network(QModelIndex)));
  connect(ui->network_setting_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
  if (is_file_exist("/usr/bin/wpa_supplicant"))
    ui->network_setting_listView->setRowHidden(0, true);
#if 0
  debug_test_network();
#endif
}

ui_network_setting::~ui_network_setting() {
  ALOGD("ui_network_setting delete");

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }

  delete ui;
}

void ui_network_setting::slot_for_setting_network(
  const QModelIndex &index) {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  if (index.row() == 0)
    rotate_signgleton->start_activity_win(
      &ui_wifi_list::staticMetaObject);
  else
    rotate_signgleton->start_activity_win(
      &ui_eth_setting::staticMetaObject);
}

void ui_network_setting::slot_for_mainui(int event, int value) {
  ALOGD("[ui_network_setting] %s:%d >> event=%d, value=%d",
    __func__, __LINE__, event, value);
  if (-1 == event) {
    if (0 == value)
      ui_control_wakeup_screen();
    else
      test_network();
  }
}

void ui_network_setting::test_network() {
  ui_rotate_management::getInstance()->start_activity_win(
    &ui_wifi_list::staticMetaObject);
}

static void *test_proc(void *arg) {
  prctl(PR_SET_NAME, "test_network_th");
  ALOGD("%s:%d >> test thread start.", __func__, __LINE__);
  int rand_t = 0;
  ui_network_setting *network = (ui_network_setting *)arg;

  network->sendSignalForMainUI(-1, 0);
  rand_t = rand()%3 + 1;
  ALOGD("%s:%d >> wait start sleep %ds.",
    __func__, __LINE__, rand_t);
  sleep(rand_t);

  network->sendSignalForMainUI(-1, 1);
  ALOGD("%s:%d >> test thread end.", __func__, __LINE__);
  return NULL;
}

void ui_network_setting::debug_test_network() {
  ALOGD("%s:%d", __func__, __LINE__);
  pthread_t test_th;
  if (pthread_create(&test_th, NULL, test_proc, (void *)this)) {
    ALOGE("create test network thread failed, errno: %d (%s)",
      errno, strerror(errno));
  }
}

