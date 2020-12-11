/*
* ui_about_setting.hpp define about setting function of UI
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
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>
#include <QModelIndex>

#include "ui_about_setting.hpp"
#include "ui_ui_about_setting.h"
#include "ui_setting_item_delegate.hpp"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_factory_main.hpp"

#define SYSTEM_VERSION    0
#define ALGORITHM_VERSION 1
#define SYSTEM_UPDATE     2
#define SYSTEM_MAINTAIN   3
#define RESET_FACTORY     4
#define FACTORY_TEST      5

#define CLICK_ALG_VERSION_COUNT 6

void ui_about_setting::ui_init() {
  menu_about = {
    {":icon/setting/aboat/aboat_system_version.png",
      tr("系统版本"), "", EN_ITEM_LABEL_ONLY},
    {":icon/setting/aboat/aboat_alg_version.png",
      tr("算法版本"), "", EN_ITEM_LABEL_ONLY},
    {":icon/setting/aboat/aboat_update_firmware.png",
      tr("系统升级"), "", EN_ITEM_LABEL},
    {":icon/setting/aboat/aboat_update_firmware.png",
      tr("系统维护"), "", EN_ITEM_LABEL},
    {":icon/setting/aboat/aboat_default_factory.png",
      tr("恢复出厂设置"), "", EN_ITEM_LABEL},
    {":/icon/factory_test.png",
      tr("工厂测试"), "", EN_ITEM_LABEL}
  };
}

ui_about_setting::ui_about_setting(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_about_setting), pModel(NULL), pItemDelegate(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  click_count_ = 0;
  ui_init();
  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();

  device_system_info_t dev_info;
  memset(&dev_info, 0, sizeof(device_system_info_t));
  development_status = ui_control_get_development_status();
  get_sys_info(&dev_info);
  memset(&auto_reboot_param, 0, sizeof(auto_reboot_param_t));
  ui_control_get_auto_reboot_param(&auto_reboot_param);

  pModel = new QStandardItemModel();
  int count = menu_about.count();
  for (int i = 0; i < count - 1; ++i) {
    QStandardItem *pItem = new QStandardItem;
    if (SYSTEM_VERSION == i)
      menu_about[i].value = QString(dev_info.firmware_version);
    else if (ALGORITHM_VERSION == i)
      menu_about[i].value = QString(dev_info.face_recg_alg_version);

    pItem->setData(QVariant::fromValue(menu_about[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_setting_item_delegate(this);
  ui->about_setting_listView->setItemDelegate(pItemDelegate);
  ui->about_setting_listView->setModel(pModel);

  connect(ui->about_setting_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_about_setting_item(QModelIndex)));

  connect(ui->about_setting_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  ui->about_setting_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_about_setting::~ui_about_setting() {
  ALOGD("[%s %d] delete", __func__, __LINE__);

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

void ui_about_setting::slot_for_about_setting_item(const QModelIndex &index) {
  system_maintenance_args_t *system_maintenance_info;
  ui_system_maintenance_dialog *sw;
  ui_tip_dialog *tw;

  int i = index.row();
  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();

  switch (i) {
    case ALGORITHM_VERSION:
      click_count_++;
      if (click_count_ == CLICK_ALG_VERSION_COUNT) {
        QStandardItem *pItem = new QStandardItem;
        pItem->setData(QVariant::fromValue(menu_about[FACTORY_TEST]),
            Qt::UserRole + 1);
        pModel->appendRow(pItem);
        ui->about_setting_listView->setModel(pModel);
        ui->about_setting_listView->update();
      }
      break;

    case SYSTEM_UPDATE:
      rotate_signgleton->slot_for_update_firmware();
      break;

    case SYSTEM_MAINTAIN:
      system_maintenance_info = new system_maintenance_args_t;
      system_maintenance_info->id = reinterpret_cast<void *>(i);
      system_maintenance_info->title = menu_about[i].title;
      system_maintenance_info->default_choice = auto_reboot_param.reboot_interval;
      system_maintenance_info->reboot_time = QString(auto_reboot_param.reboot_time);
      system_maintenance_info->cb = this;
      sw = new ui_system_maintenance_dialog(system_maintenance_info, this);
      sw->show();
      delete system_maintenance_info;
      break;

    case RESET_FACTORY:
      tw = new ui_tip_dialog(
        TIP_WIN_TYPE_FACTORY_DEFAULT, this);
      if (tw)
        tw->show();
      break;

    case FACTORY_TEST:
      rotate_signgleton->start_activity_win(
        &ui_factory_main::staticMetaObject);
      break;

    default:
      break;
  }
}

void ui_about_setting::system_maintenance_callback(
    void *id, int choice, QString time) {
  int row = (int)id;
  int ret;

  if (auto_reboot_param.reboot_interval == choice &&
      !QString(auto_reboot_param.reboot_time).compare(time)) {
    ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
    return;
  }

  auto_reboot_param.reboot_interval = choice;
  memcpy(auto_reboot_param.reboot_time, time.toLatin1().data(), 16);

  ret = ui_control_set_auto_reboot_param(&auto_reboot_param);
  if (ret)
    ALOGD("[%s:%d] set parameter fail", __func__, __LINE__);
}

