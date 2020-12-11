/*
* ui_base_widget.cpp - define ui_base_widget of project
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

extern "C" {
#define LOG_TAG     "ui"
#include <log/log.h>
#include "ui_control.h"
}

#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_aboat_device_main.hpp"
#include "ui_aboat_storage.hpp"
#include "ui_factory_main.hpp"

const QStringList local_icons = {
  ":icon/setting/aboat/aboat_device.png",
  ":icon/setting/aboat/aboat_system_version.png",
  ":icon/setting/aboat/aboat_alg_version.png",
  ":icon/setting/aboat/aboat_storage.png",
  ":icon/setting/aboat/aboat_update_firmware.png",
  ":icon/setting/aboat/aboat_default_factory.png",
  ":/icon/factory_test.png"
};
ui_aboat_device_main::ui_aboat_device_main() {
  QString title = tr("关于设备");
  set_title_text(title);

  center_widget->resize(1280, 648);
  click_count_ = 0;
  QStringList name_list;
  name_list << tr("设备名称") << tr("系统版本")
      << tr("算法版本") << tr("存储容量")
      << tr("系统升级") << tr("恢复出厂设置") << tr("工厂测试");
  device_system_info_t dev_info;
  memset(&dev_info, 0, sizeof(device_system_info_t));
  development_status = ui_control_get_development_status();
  get_sys_info(&dev_info);
  pModel = new QStandardItemModel();
  for (int i = 0; i < name_list.size(); i++) {
    QStandardItem* pItem = new QStandardItem;
    data_aboat_setting_delegate data;
    data.name = name_list.at(i);
    data.icon_path = local_icons[i];
    switch (i) {
      case 0:
        data.text = dev_info.device_name;
        data.is_display = true;
        break;
      case 1:
        data.text = dev_info.firmware_version;
        data.is_display = true;
        break;
      case 2:
        data.text = "V1.0";
        data.is_display = true;
        break;
      case 3:
      case 4:
      case 5:
        data.is_display = true;
        break;
      case 6:
        data.is_display = false;
        break;
    }
    pItem->setData(QVariant::fromValue(data), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  list_view = new QListView(center_widget);
  list_view->setGeometry(QRect(0, 0, 1280, 648));
  pItemDelegate = new ui_aboat_device_main_delegate();
  list_view->setItemDelegate(pItemDelegate);
  list_view->setModel(pModel);
  connect(this, SIGNAL(signal_back_click()), this, SLOT(slot_for_back_click()));
  connect(list_view, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_list_view_clicked(QModelIndex)));
  list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
  clicked = false;
}
ui_aboat_device_main::~ui_aboat_device_main() {
  ALOGD("ui_system_setting delete");

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }
}
void ui_aboat_device_main::slot_for_back_click() {
  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_aboat_device_main::slot_for_list_view_clicked(
    const QModelIndex& index) {
  ui_tip_dialog *w;
  if (clicked) return;
  clicked = true;
  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();
  switch (index.row()) {
    case 0:
    case 1:
      break;
    case 2:
      /**/
      click_count_++;
      if (click_count_ >= CLICK_ALG_VERSION_COUNT) {
        QVariant var = pModel->index(6, 0).data(Qt::UserRole + 1);
        data_aboat_setting_delegate itemData =
            var.value<data_aboat_setting_delegate>();
        if (itemData.is_display) {
          itemData.is_display = false;
        } else {
          itemData.is_display = true;
        }
        pModel->setData(
            pModel->index(6, 0),
            QVariant::fromValue(itemData),
            Qt::UserRole + 1);
        click_count_ = 0;
        QRegion region;
        region = region.united(list_view->frameGeometry());
        this->repaint(region);
      }
      break;
    case 3:
      /*storage*/
      rotate_signgleton->start_activity_win(
        &ui_aboat_storage::staticMetaObject);
      break;
    case 4:
      if (rotate_signgleton->is_update_firmware())
        return;
      w = new ui_tip_dialog(TIP_WIN_TYPE_SYS_UPGRADE, this);
      if (w) {
        w->show();
      }
      break;
    case 5:
      w = new ui_tip_dialog(
        TIP_WIN_TYPE_SYS_UPGRADE, this);
      if (w)
        w->show();
      break;
    case 6:
      QVariant var = index.data(Qt::UserRole + 1);
      data_aboat_setting_delegate itemData =
          var.value<data_aboat_setting_delegate>();
      if (itemData.is_display) {
        rotate_signgleton->start_activity_win(
          &ui_factory_main::staticMetaObject);
      }
      break;
  }
  clicked = false;
}



