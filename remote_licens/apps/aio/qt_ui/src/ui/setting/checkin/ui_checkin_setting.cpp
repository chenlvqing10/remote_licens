/*
* ui_checkin_setting.cpp - define interface function of checkin setting
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
#include "face.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>
#include <QModelIndex>

#include "ui_rotate_management.hpp"

#include "ui_checkin_setting.hpp"
#include "ui_ui_checkin_setting.h"
#include "ui_setting_item_delegate.hpp"
#include "ui_attendance_list.hpp"

#define SET_ACCESS_RECORDS 0
#define VIEW_ACCESS_RECORDS 1

void ui_checkin_setting::ui_init() {
  set_access_records[0] = tr("保存全景图片");
  set_access_records[1] = tr("保存人脸图片");
  set_access_records[2] = tr("保存陌生人记录");
  menu_checkin = {
    {":icon/set_access_records.png", tr("设置通行记录"), "", EN_ITEM_LABEL},
    {":icon/view_access_records.png", tr("查看通行记录"), "", EN_ITEM_LABEL}
  };
}
ui_checkin_setting::ui_checkin_setting(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_checkin_setting),
  pModel(NULL),
  pItemDelegate(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  ui_init();

  memset(&checkin_info, 0, sizeof(checkin_param_t));
  memset(checkin_option, 0, sizeof(checkin_option));
  ui_control_get_checkin_info(&checkin_info);
  checkin_option[0] = checkin_info.save_large_pic;
  checkin_option[1] = checkin_info.save_small_pic;
  checkin_option[2] = checkin_info.save_stranger_record;

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  pModel = new QStandardItemModel();

  int count = menu_checkin.count();
  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
    pItem->setData(QVariant::fromValue(menu_checkin[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }

  pItemDelegate = new ui_setting_item_delegate(this);
  ui->checkin_listView->setItemDelegate(pItemDelegate);
  ui->checkin_listView->setModel(pModel);

  connect(ui->checkin_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_checkin_setting_item(QModelIndex)));

  connect(ui->checkin_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
  ui->checkin_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_checkin_setting::~ui_checkin_setting() {
  ALOGD("%s delete", __func__);

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }

  delete ui;
}

void ui_checkin_setting::slot_for_checkin_setting_item(
  const QModelIndex &index) {
  ALOGD("%s >> row = %d", __func__, index.row());
  int i = index.row();

  checkbox_args_t *checkbox_info;
  ui_checkbox_dialog *w;

  switch (i) {
    case SET_ACCESS_RECORDS:
      checkbox_info = new checkbox_args_t;
      checkbox_info->id = reinterpret_cast<void *>(i);
      checkbox_info->title = menu_checkin[i].title;
      for (auto iter = set_access_records.begin();
          iter != set_access_records.end(); ++iter) {
        checkbox_info->checkbox_list << iter.value();
        if (checkin_option[iter.key()])
          checkbox_info->default_indexs << iter.key();
      }
      checkbox_info->cb = this;
      w = new ui_checkbox_dialog(checkbox_info, this);
      w->show();
      delete checkbox_info;
      break;
    case VIEW_ACCESS_RECORDS:
      {
        ui_rotate_management *rotate_signgleton =
        ui_rotate_management::getInstance();

        if (-1 != rotate_signgleton->attendance_start) {
          rotate_signgleton->attendance_num = QString("");
          rotate_signgleton->attendance_start = -1;
          rotate_signgleton->attendance_end = 0;
        }

        rotate_signgleton->start_activity_win(
          &ui_attendance_list::staticMetaObject);
      }
      break;
    }
}

void ui_checkin_setting::checkbox_callback(void *id, QVector<int> indexs) {
  int ret;
  int row = (int)id;

  memset(checkin_option, 0, sizeof(checkin_option));
  for (auto item : indexs) {
    checkin_option[item] = 1;
  }

  checkin_info.save_large_pic = checkin_option[0];
  checkin_info.save_small_pic = checkin_option[1];
  checkin_info.save_stranger_record = checkin_option[2];
  ret = ui_control_set_checkin_info(&checkin_info);
  if (ret) {
    ALOGD("%s[%d] set checkin parameter fail!", __func__, __LINE__);
    return;
  }
}

