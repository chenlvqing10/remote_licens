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
#include <time.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#define RTSP_VIDEO  0
#define VIDEO       1

#define RTSP_DB_1280X800  0
#define RTSP_DB_768X480 1
#define RTSP_DB_ALL 2

#include <QStandardItemModel>
#include <QObject>

#include <QDebug>
#include <QModelIndex>
#include <QThread>

#include "ui_rtsp_setting.hpp"
#include "ui_ui_rtsp_setting.h"
#include "ui_rotate_management.hpp"

void ui_rtsp_setting::ui_init() {
  video_resolution[0] = "720p@30fps";
  video_resolution[1] = "480p@30fps";
  video_resolution[2] = tr("全选");

  menu_rtsp = {
    {":icon/setting/RTSP_video.png",
      tr("RTSP视频流") , "" , EN_ITEM_SWITCH},
    {":icon/setting/video.png",
      tr("视频流分辨率"), "", EN_ITEM_LABEL}
  };
}

ui_rtsp_setting::ui_rtsp_setting(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_rtsp_setting),
  pModel(NULL),
  pItemDelegate(NULL) {
  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);
  ui_init();
  memset(&system_info, 0, sizeof(system_param_t));
  ui_control_get_system_info(&system_info);
  pModel = new QStandardItemModel();
  int count = menu_rtsp.count();
  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
    if (RTSP_VIDEO == i) {
        if (system_info.rtsp_video == 0)
          menu_rtsp[i].value = "0";
        else
          menu_rtsp[i].value = "1";
      } else if (VIDEO == i) {
      menu_rtsp[i].value =
        video_resolution[system_info.video_type];
      }
      pItem->setData(QVariant::fromValue(menu_rtsp[i]),
        Qt::UserRole + 1);
      pModel->appendRow(pItem);
    }
  rtsp_video_set();
  if (system_info.rtsp_video != 1) {
    rtsp_video_pasue();
  }
  pItemDelegate = new ui_setting_item_delegate(this);
  ui->rtsp_setting_listView->setItemDelegate(pItemDelegate);
  ui->rtsp_setting_listView->setModel(pModel);
  ui->rtsp_setting_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  connect(ui->rtsp_setting_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_setting_rtsp(QModelIndex)));
  connect(ui->rtsp_setting_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
}

ui_rtsp_setting::~ui_rtsp_setting() {
  ALOGD("ui_rtsp_setting delete");
  delete ui;

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }
}

void ui_rtsp_setting::slot_for_setting_rtsp(
  const QModelIndex &index) {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  radio_args_t *radio_info;
  ui_radio_dialog *w;
  if (index.row() == RTSP_VIDEO) {
    if (1 == system_info.rtsp_video) {
      menu_rtsp[RTSP_VIDEO].value = "0";
      rtsp_video_pasue();
      system_info.rtsp_video = 0;
    } else {
      menu_rtsp[RTSP_VIDEO].value = "1";
      rtsp_video_set();
      system_info.rtsp_video = 1;
    }
    ui_control_set_rtsp_video(system_info.rtsp_video);
    pModel->setData(pModel->index(RTSP_VIDEO, 0),
      QVariant::fromValue(menu_rtsp[RTSP_VIDEO]),
      Qt::UserRole + 1);
    ui->rtsp_setting_listView->setModel(pModel);
    ui->rtsp_setting_listView->update();
  } else {
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(VIDEO);
      radio_info->title = QString(menu_rtsp[VIDEO].title);
      for (auto iter = video_resolution.begin() ;
        iter != video_resolution.end(); ++iter) {
        radio_info->radio_list << iter.value();
      }
      radio_info->default_index = system_info.video_type;
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
    }
  }

void ui_rtsp_setting::radio_callback(void *id, int index,
  bool isCancel) {
  int ret;
  if (index == (int)system_info.video_type) {
    return;
  }
  ret = ui_control_set_video_type(index);
  if (ret) {
    ALOGE("[%s %d] set video type fail", __func__, __LINE__);
    return;
  }
  system_info.video_type = index;
  if (system_info.rtsp_video == 1) {
      rtsp_video_set();
  }
  menu_rtsp[VIDEO].value =
    video_resolution[system_info.video_type];
  ALOGD("[%s:%d] set video type successful, access type %s",
    __func__, __LINE__, video_resolution[index]);
  pModel->setData(pModel->index(VIDEO, 0),
    QVariant::fromValue(menu_rtsp[VIDEO]), Qt::UserRole + 1);
  ui->rtsp_setting_listView->setModel(pModel);
  ui->rtsp_setting_listView->update();
}

void ui_rtsp_setting::rtsp_video_set() {
  live_video_para_t rtsp_resolution;
  memset(&rtsp_resolution, 0, sizeof(live_video_para_t));

  if (system_info.video_type == RTSP_DB_1280X800) {
    live_video_pause(1);
    live_video_resume(0);
    usleep(50 * 1000);
    live_video_get_para((int)system_info.video_type, &rtsp_resolution);
    rtsp_resolution.height = 1280;
    rtsp_resolution.width = 800;
    live_video_set_para((int)system_info.video_type, &rtsp_resolution);
  } else if (system_info.video_type == RTSP_DB_768X480) {
    live_video_pause(0);
    live_video_resume(1);
    usleep(50 * 1000);
    live_video_get_para((int)system_info.video_type, &rtsp_resolution);
    rtsp_resolution.height = 768;
    rtsp_resolution.width = 480;
    live_video_set_para((int)system_info.video_type, &rtsp_resolution);
  } else if (system_info.video_type == RTSP_DB_ALL) {
    live_video_resume(0);
    live_video_resume(1);
    usleep(50 * 1000);
    live_video_get_para(0, &rtsp_resolution);
    rtsp_resolution.height = 1280;
    rtsp_resolution.width = 800;
    live_video_set_para(0, &rtsp_resolution);
    live_video_get_para(1, &rtsp_resolution);
    rtsp_resolution.height = 768;
    rtsp_resolution.width = 480;
    live_video_set_para(1, &rtsp_resolution);
  }
}

void ui_rtsp_setting::rtsp_video_pasue() {
    live_video_pause(0);
    live_video_pause(1);
}

