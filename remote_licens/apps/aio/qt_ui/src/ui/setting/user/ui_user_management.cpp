/*
* ui_user_management.cpp - define interface function of UI
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

#include "ui_user_management.hpp"
#include "ui_ui_user_management.h"
#include "ui_user_management_delegate.hpp"
#include "ui_rotate_management.hpp"
#include "ui_add_user.hpp"
#include "ui_batch_import.hpp"
#include "ui_user_view_list.hpp"

void ui_user_management::ui_init() {
  menu_user = {
    {":icon/user_add.png",
    	tr("新增用户"), "", EN_ITEM_LABEL},
    {":icon/batch_import.png",
    	tr("批量导入用户"), "", EN_ITEM_LABEL},
    {":icon/user_view.png",
    	tr("查看用户"), "", EN_ITEM_LABEL},
  };
}

ui_user_management::ui_user_management(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_user_management),
    pModel(NULL),
    pItemDelegate(NULL),
    mode(0) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

  ui_init();

  pModel = new QStandardItemModel();
  ui_get_ui_mode(&mode);
#if ATTENDANCE_MODE == 2
  QStandardItem *pItem = new QStandardItem;
  pItem->setData(QVariant::fromValue(menu_user[2]), Qt::UserRole + 1);
  pModel->appendRow(pItem);
#else
  int count = menu_user.count();
  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
    pItem->setData(QVariant::fromValue(menu_user[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
#endif

  pItemDelegate = new ui_setting_item_delegate(this);
  ui->user_management_listView->setItemDelegate(pItemDelegate);
  ui->user_management_listView->setModel(pModel);

  connect(ui->user_management_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_setting_item(QModelIndex)));

  connect(ui->user_management_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
}

ui_user_management::~ui_user_management() {
  ALOGD("%s:%d >> %s", __func__, __LINE__, metaObject()->className());
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
#if ATTENDANCE_MODE != 2
  ui_rotate_management *rotate =
    ui_rotate_management::getInstance();
  rotate->set_recg_cnt();
#endif
}

void ui_user_management::slot_for_setting_item
  (const QModelIndex &index) {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
#if ATTENDANCE_MODE == 2
  if (index.row() == 0) {
    rotate_signgleton->start_activity_win(
      &ui_user_view_list::staticMetaObject);
  }
#else
  switch(index.row()) {
    case 0:
      rotate_signgleton->start_activity_win(
        &ui_add_user::staticMetaObject);
      break;
    case 1:
      rotate_signgleton->start_activity_win(
        &ui_batch_import::staticMetaObject);
      break;
    case 2:
      rotate_signgleton->start_activity_win(
        &ui_user_view_list::staticMetaObject);
      break;
  }
#endif
}

