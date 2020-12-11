/*
* ui_access_setting.cpp - define interface function of access setting
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

#include "ui_rotate_management.hpp"

#include "ui_access_time_setting.hpp"
#include "ui_ui_access_time_setting.h"

#define MAX_ACCESS_COUNT  3

item_t ui_access_time_setting::menu_access[] = {
  {"01", "", "", EN_ITEM_BT},
  {"02", "", "", EN_ITEM_BT},
  {"03", "", "", EN_ITEM_BT}
};

ui_access_time_setting::ui_access_time_setting(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_access_time_setting),
  pModel(NULL),
  pItemDelegate(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  char access_time[128];
  memset(access_time, 0, 128);
  ui_control_get_access_time(access_time);
  access_time_list = QString(QLatin1String(access_time)).split("/");
  if (access_time_list.size() == 1)
    access_time_list << 0 << 0 << 0;

  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();

  pModel = new QStandardItemModel();
  int count = sizeof(menu_access)/sizeof(item_t);

  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
    if (access_time_list[i].compare("0"))
      menu_access[i].title = access_time_list[i];
    else
      menu_access[i].title = tr("无定义");

    pItem->setData(QVariant::fromValue(menu_access[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }

  pItemDelegate = new ui_setting_item_delegate(this);
  ui->access_time_listView->setItemDelegate(pItemDelegate);
  ui->access_time_listView->setModel(pModel);

  connect(ui->access_time_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_access_time_item(QModelIndex)));

  connect(ui->access_time_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  ui->access_time_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_access_time_setting::~ui_access_time_setting() {
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

  access_time_list.clear();

  delete ui;
}

void ui_access_time_setting::time_callback(void *id, QString time) {
  int ret;
  int i = (int)id;
  QStringList time_list;

  if (!time.compare(access_time_list[i])) {
    ALOGD("[%s %d] parameter no change", __func__, __LINE__);
    return;
  }

  time_list = time.split("--");
  if (time_list.count() < 2) {
    ALOGE("[%s %d] time_callback error", __func__, __LINE__);
    return;
  }
  if (time_list[0].compare(time_list[1]) >= 0) {
    ALOGE("[%s %d] time_callback error", __func__, __LINE__);
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_ACCESS_TIME_FAIL, this);
    w->show();
    return;
  }

  QString tmp = access_time_list[i];
  access_time_list[i] = time;
  QString access_time_all = access_time_list.join("/");

  ret = ui_control_set_access_time(access_time_all.toLatin1().data());
  if (ret) {
    ALOGE("[%s %d] set access time fail", __func__, __LINE__);
    access_time_list[i] = tmp;
    return;
  }
  ALOGD("[%s:%d] set access time successful, access_time %s",
    __func__, __LINE__, access_time_all.toLatin1().data());

  menu_access[i].title = access_time_list[i];

  pModel->setData(pModel->index(i, 0),
      QVariant::fromValue(menu_access[i]), Qt::UserRole + 1);
  ui->access_time_listView->setModel(pModel);
  ui->access_time_listView->update();
}

void ui_access_time_setting::slot_for_access_time_item(
    const QModelIndex &index) {
  time_args_t *time_info;
  ui_time_dialog *w;

  if (index.row() < MAX_ACCESS_COUNT) {
    time_info = new time_args_t;
    time_info->id = (void *)index.row();
    time_info->title = QString(tr("时段设置"));
    if (access_time_list[index.row()].compare("0"))
      time_info->time = access_time_list[index.row()];
    else
      time_info->time = "08:30--12:30";
    time_info->cb = (ui_time_callback *)this;
    w = new ui_time_dialog(time_info, this);
    w->show();
    delete time_info;
  }
}

