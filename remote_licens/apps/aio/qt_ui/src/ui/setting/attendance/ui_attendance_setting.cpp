/*
* ui_attendance_setting.cpp - define interface function of attendance setting
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

#include "ui_attendance_setting.hpp"
#include "ui_ui_attendance_setting.h"
#include "ui_setting_item_delegate.hpp"
#include "ui_attendance_list.hpp"

#define SET_ACCESS_RECORDS 0
#define VIEW_ACCESS_RECORDS 1

#define SAVED_SWITCH_ROW   0
#define AVOID_STEP_ROW    1
#define REVIEW_RECORD_ROW   2

void ui_attendance_setting::ui_init() {
  avoid_steps = {
    {tr("30秒"), 30}, {tr("1分钟"), 60}, {tr("5分钟"), 300}
  };

  menu_attendance = {
    {":icon/save_pic.png", tr("保存照片"), "", EN_ITEM_SWITCH},
    {":icon/avoid_step.png", tr("重复确认时间"), "", EN_ITEM_LABEL},
    {":icon/review_record.png", tr("查看考勤记录"), "", EN_ITEM_LABEL}
  };
}

ui_attendance_setting::ui_attendance_setting(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_attendance_setting),
  pModel(NULL),
  pItemDelegate(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);
  ui_init();

  memset(&attendance_info, 0, sizeof(attendance_param_t));
  ui_device_get_attendance_param(&attendance_info);

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  pModel = new QStandardItemModel();
  int count = menu_attendance.count();
  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
  if (SAVED_SWITCH_ROW == i) {
    if (1 == attendance_info.saved_attendance_pic)
      menu_attendance[i].value = "1";
    else
      menu_attendance[i].value = "0";
  } else if (AVOID_STEP_ROW == i) {
    count = avoid_steps.count();
    for (int j = 0; j < count; ++j) {
      if (attendance_info.avoid_attendance_step ==
          avoid_steps[j].value) {
        menu_attendance[i].value = avoid_steps[j].key;
        break;
      }
    }
  }
    pItem->setData(QVariant::fromValue(menu_attendance[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }


  pItemDelegate = new ui_setting_item_delegate(this);
  ui->attendance_listView->setItemDelegate(pItemDelegate);
  ui->attendance_listView->setModel(pModel);

  connect(ui->attendance_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_attendance_setting_item(QModelIndex)));

  connect(ui->attendance_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
  ui->attendance_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_attendance_setting::~ui_attendance_setting() {
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

void ui_attendance_setting::slot_for_attendance_setting_item(
  const QModelIndex &index) {
  ALOGD("%s >> row = %d", __func__, index.row());
  int i = index.row();
  switch (i) {
    case SAVED_SWITCH_ROW:
      if (0 == menu_attendance[i].value.compare("1")) {
        menu_attendance[i].value = "0";
        attendance_info.saved_attendance_pic = 0;
      } else {
        menu_attendance[i].value = "1";
        attendance_info.saved_attendance_pic = 1;
      }
      ui_device_set_attendance_param(&attendance_info);
      pModel->setData(pModel->index(i, 0),
        QVariant::fromValue(menu_attendance[i]), Qt::UserRole + 1);
      ui->attendance_listView->setModel(pModel);
      ui->attendance_listView->update();
      set_dump_img_config();
      break;
    case AVOID_STEP_ROW:
      {
        radio_args_t *radio_info = new radio_args_t;
        radio_info->id = (void *)AVOID_STEP_ROW;
        radio_info->title = menu_attendance[i].title;
        int count = avoid_steps.count();
        for (int j = 0; j < count; ++j) {
          radio_info->radio_list << avoid_steps[j].key;
          if (attendance_info.avoid_attendance_step ==
            avoid_steps[j].value) {
            radio_info->default_index = j;
          }
        }
        radio_info->cb = this;
        ui_radio_dialog *w = new ui_radio_dialog(radio_info);
        w->show();
        delete radio_info;
      }
      break;
    case REVIEW_RECORD_ROW:
      {
        ui_rotate_management *rotate_signgleton =
          ui_rotate_management::getInstance();
        rotate_signgleton->start_activity_win(
          &ui_attendance_list::staticMetaObject);
        deinit();
      }
      break;

    default:
    ALOGD("%s >> row = %d", __func__, index.row());
    break;
  }
}

void ui_attendance_setting::radio_callback(void *id, int index,
  bool isCancel) {
  int row = (int)id, ret = 0;
  if (VIEW_ACCESS_RECORDS == row) {
  attendance_info.avoid_attendance_step = avoid_steps[index].value;
  ui_device_set_attendance_param(&attendance_info);
  menu_attendance[row].value = avoid_steps[index].key;

  pModel->setData(pModel->index(row, 0),
    QVariant::fromValue(menu_attendance[row]), Qt::UserRole + 1);
  ui->attendance_listView->setModel(pModel);
  ui->attendance_listView->update();
  set_dump_img_config();
  } else {
      if (index == attendance_info.access_records) {
        ALOGD("[%s %d] set_access_records no change", __func__, __LINE__);
        return;
      }
    attendance_info.access_records = index;
    ret = ui_device_set_attendance_param(&attendance_info);
    if (ret) {
      ALOGE("[%s %d] set_access_records fail", __func__, __LINE__);
      return;
    }
    pModel->setData(pModel->index(row, 0),
      QVariant::fromValue(menu_attendance[row]), Qt::UserRole + 1);
    ui->attendance_listView->setModel(pModel);
    ui->attendance_listView->update();
  }
}


