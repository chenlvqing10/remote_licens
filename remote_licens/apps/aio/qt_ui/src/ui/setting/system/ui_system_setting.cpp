/*
* ui_system_setting.cpp - define interface function of UI system setting
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
#define LOG_TAG    "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>
#include <QStandardItemModel>
#include "ui_system_setting.hpp"
#include "ui_ui_system_setting.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_volum_setting.hpp"
#include "ui_language_setting.hpp"
#include "ui_setting_item_delegate.hpp"
#include "ui_rtsp_setting.hpp"

#include "ui_setting_time.hpp"
#include "ui_zone_setting.hpp"
#include "ui_main_setting.hpp"
#include "ui_aboat_storage.hpp"
#include "ui_modify_password.hpp"
#include "ui_about_setting.hpp"

#if ATTENDANCE_MODE == 2
#define TIME        -2
#define ZONE        -1
#define VOLUME      0
#define BRIGHTNESS  1
#define LIGHT       2
#define LANGUAGE    3
#define PASSWORD    4
#define ABOAT       5
#else
#define TIME        0
#define ZONE        1
#define VOLUME      2
#define BRIGHTNESS  3
#define LIGHT       4
#define LANGUAGE    5
#define RTSP_VIDEO  6
#define MENU        7
#define STORAGE     8
#define PASSWORD    9
#define ABOAT       10

#endif

#define VOLUME_MAX 10
#define VOLUME_MIN 0
#define BRIGHTNESS_MAX  10
#define BRIGHTNESS_MIN  0

void ui_system_setting::ui_init() {
  language_choose[0] = "中文";
  language_choose[1] = "English";

  zone_text[Beijing] = tr("北京");
  zone_text[Tokyo] = tr("东京");
  zone_text[New_York] = tr("纽约");
  zone_text[London] = tr("伦敦");
  zone_text[Paris] = tr("巴黎");
  zone_text[Berlin] = tr("柏林");

  menu_system_setting = {
#if ATTENDANCE_MODE == 2
    {":icon/setting/volume_setting.png",
      tr("音量设置") , "5" , EN_ITEM_LABEL},
    {":icon/setting/light_setting.png",
      tr("亮度设置") , "80" , EN_ITEM_LABEL},
    {":icon/setting/brightness_setting.png",
      tr("补光设置") , "" , EN_ITEM_SWITCH},
    {":icon/setting/language_setting.png",
      tr("语言设置") , "" , EN_ITEM_LABEL},
    {":icon/setting/aboat/aboat_storage.png",
      tr("存储容量"), "", EN_ITEM_LABEL},
    {":icon/setting/password_setting.png",
      tr("登录密码修改") , "", EN_ITEM_LABEL},
    {":icon/setting/aboat_setting.png",
      tr("关于设备") , "" , EN_ITEM_LABEL}
#else
    {":icon/setting/time_setting.png",
      tr("时间设置") , "" , EN_ITEM_LABEL},      // TIME
    {":icon/setting/zone_setting.png",
      tr("时区设置") , "" , EN_ITEM_LABEL},      // ZONE
    {":icon/setting/volume_setting.png",
      tr("音量设置") , "5" , EN_ITEM_LABEL},      // VOLUME
    {":icon/setting/light_setting.png",
      tr("亮度设置") , "80" , EN_ITEM_LABEL},      // BRIGHTNESS
    {":icon/setting/brightness_setting.png",
      tr("补光设置") , "" , EN_ITEM_SWITCH},      // LIGHT
    {":icon/setting/language_setting.png",
      tr("语言设置") , "" , EN_ITEM_LABEL},      // LANGUAGE
    {":icon/setting/RTSP_video.png",
      tr("RTSP视频流") , "" , EN_ITEM_LABEL},     // RTSP_VIDEO
    {":icon/setting/MEMU.png",
      tr("主界面设置"), "", EN_ITEM_LABEL},       // MENU
    {":icon/setting/aboat/aboat_storage.png",
      tr("存储容量"), "", EN_ITEM_LABEL},      // STORAGE
    {":icon/setting/password_setting.png",
      tr("登录密码修改") , "", EN_ITEM_LABEL},        // PASSWORD
    {":icon/setting/aboat_setting.png",
      tr("关于设备") , "" , EN_ITEM_LABEL}      // ABOAT

#endif
  };
}

ui_system_setting::ui_system_setting(QWidget* parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_system_setting),
  pModel(NULL),
  pItemDelegate(NULL) {
  ui_rotate_management* rotate_signgleton =
    ui_rotate_management::getInstance();
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);
  refresh();
  connect(ui->system_setting_back_btn, SIGNAL(clicked()),
    rotate_signgleton, SLOT(back_activity_win()));
  connect(ui->system_setting_listView, SIGNAL(clicked(QModelIndex)),
    this, SLOT(slot_for_system_setting_listview(QModelIndex)));
  ui->system_setting_listView->setEditTriggers(
    QAbstractItemView::NoEditTriggers);
}

ui_system_setting::~ui_system_setting() {
  ALOGD("ui_system_setting delete");

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

void ui_system_setting::refresh() {
  QStandardItem *pItem;
  int count = 0;

  memset(&system_info, 0, sizeof(system_param_t));
  ui_control_get_system_info(&system_info);

  ui_init();

  ui_rotate_management* rotate_signgleton =
    ui_rotate_management::getInstance();
  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }
  pModel = new QStandardItemModel();

  count = menu_system_setting.count();
  for (int i = 0; i < count; ++i) {
      pItem = new QStandardItem;
      if (ZONE == i)
        menu_system_setting[i].value = zone_text[system_info.zone_area];
      else if (VOLUME == i)
        menu_system_setting[i].value = QString::number(system_info.volum / 10);
      else if (BRIGHTNESS == i)
        menu_system_setting[i].value =
          QString::number(system_info.brightness / 10);
      else if (LIGHT == i) {
        if (system_info.light == 0)
          menu_system_setting[i].value = "0";
        else if (system_info.light == 2)
          menu_system_setting[i].value = "1";
      } else if (LANGUAGE == i) {
        menu_system_setting[i].value =
          language_choose[system_info.language_choose];
      }
      pItem->setData(QVariant::fromValue(menu_system_setting[i]),
        Qt::UserRole + 1);
      pModel->appendRow(pItem);
    }

    if (!pItemDelegate)
      pItemDelegate = new ui_setting_item_delegate(this);
    ui->system_setting_listView->setItemDelegate(pItemDelegate);
    ui->system_setting_listView->setModel(pModel);
}

void ui_system_setting::slot_for_system_setting_listview(
  const QModelIndex& index) {
  int value = 0;
  ui_rotate_management* rotate_signgleton =
    ui_rotate_management::getInstance();
  radio_args_t *radio_info;
  count_args_t *count_info;
  ui_radio_dialog *w;
  ui_count_dialog *w_count;
  int ind = 0;
  QRegion region;
  switch (index.row()) {
    case TIME:
    /*time*/
      rotate_signgleton->start_activity_win(
        &ui_setting_time::staticMetaObject);
      break;
    case ZONE:
    /*zone*/
      rotate_signgleton->start_activity_win(
        &ui_zone_setting::staticMetaObject);
      break;
    case VOLUME:
    /*volume*/
      count_info = new count_args_t;
      count_info->id = reinterpret_cast<void *>(VOLUME);
      count_info->title = QString(menu_system_setting[VOLUME].title);
      count_info->min = VOLUME_MIN;
      count_info->max = VOLUME_MAX;
      count_info->default_value = system_info.volum / 10;
      count_info->cb = this;
      w_count = new ui_count_dialog(count_info, this);
      w_count->show();
      delete count_info;
      break;
    case BRIGHTNESS:
      count_info = new count_args_t;
      count_info->id = reinterpret_cast<void *>(BRIGHTNESS);
      count_info->title = QString(menu_system_setting[BRIGHTNESS].title);
      count_info->min = BRIGHTNESS_MIN;
      count_info->max = BRIGHTNESS_MAX;
      count_info->default_value = system_info.brightness / 10;
      count_info->cb = this;
      w_count = new ui_count_dialog(count_info, this);
      w_count->show();
      delete count_info;
      break;
    case LIGHT:
      if (2 == system_info.light) {
        menu_system_setting[LIGHT].value = "0";
        system_info.light = 0;
      } else {
        menu_system_setting[LIGHT].value = "1";
        system_info.light = 2;
      }
      ui_control_set_infrared_light(system_info.light);
      pModel->setData(pModel->index(LIGHT, 0),
        QVariant::fromValue(menu_system_setting[LIGHT]),
        Qt::UserRole + 1);
      ui->system_setting_listView->setModel(pModel);
      ui->system_setting_listView->update();
      break;
    case LANGUAGE:
    /*language*/
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(LANGUAGE);
      radio_info->title = QString(menu_system_setting[LANGUAGE].title);
      for (auto iter = language_choose.begin() ;
        iter != language_choose.end(); ++iter) {
        radio_info->radio_list << iter.value();
      }
      radio_info->default_index = system_info.language_choose;
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
      break;
    case RTSP_VIDEO:
      rotate_signgleton->start_activity_win(
        &ui_rtsp_setting::staticMetaObject);
      break;
    case MENU:
      rotate_signgleton->start_activity_win(
        &ui_main_setting::staticMetaObject);
      break;
    case STORAGE:
      rotate_signgleton->start_activity_win(
        &ui_aboat_storage::staticMetaObject);
      break;
    case PASSWORD:
    /*passwd*/
      rotate_signgleton->start_activity_win(
        &ui_modify_password::staticMetaObject);
      break;
    case ABOAT:
    /*aboat*/
      rotate_signgleton->start_activity_win(
        &ui_about_setting::staticMetaObject);
      break;
  }
}

void ui_system_setting::count_callback(void *id, int count) {
  int row = (int)id;
  if (row == VOLUME) {
    volume_callback(count);
  } else if (row == BRIGHTNESS) {
      brightness_callback(count);
  }
}

void ui_system_setting::radio_callback(void *id, int count,
  bool isCancel) {
  int ret;
  if ((int)id == LANGUAGE) {
    if (count == (int)system_info.language_choose) {
      ALOGD("[%s %d] language no change", __func__, __LINE__);
      return;
    }
    system_info.language_choose = count;
    switch_language(system_info.language_choose);
    menu_system_setting[LANGUAGE].value =
      language_choose[system_info.language_choose];
    refresh();
    ui->retranslateUi(this);
  }
}

void ui_system_setting:: volume_callback(int count) {
  int ret;
  ui_tip_dialog *w;
  count *= 10;
  if (count == system_info.volum) {
    ALOGD("[%s %d] volum no change", __func__, __LINE__);
    return;
  }

  ret = ui_control_set_volume(count);
  if (ret) {
    ALOGE("[%s %d] set volum fail", __func__, __LINE__);
    return;
  }
  menu_system_setting[VOLUME].value = QString::number(count / 10);
  system_info.volum = count;

  pModel->setData(pModel->index(VOLUME, 0),
    QVariant::fromValue(menu_system_setting[VOLUME]),
    Qt::UserRole + 1);
  ui->system_setting_listView->setModel(pModel);
  ui->system_setting_listView->update();
}

void ui_system_setting::brightness_callback(int count) {
  ui_tip_dialog *w;
  int ret = 0;
  count *= 10;
  if (count == system_info.brightness) {
      ALOGD("[%s %d] parameter no change", __func__, __LINE__);
      return;
  }

  ret = ui_control_set_display_brightness(count);
  if (ret) {
    ALOGE("[%s %d] set brightness fail", __func__, __LINE__);
    return;
  }

  system_info.brightness = count;
  menu_system_setting[BRIGHTNESS].value = QString::number(count / 10);
  pModel->setData(pModel->index(BRIGHTNESS, 0),
    QVariant::fromValue(menu_system_setting[BRIGHTNESS]),
    Qt::UserRole + 1);
  ui->system_setting_listView->setModel(pModel);
  ui->system_setting_listView->update();
}

void ui_system_setting::switch_language(int type) {
  int ret = 0;
  ret = ui_control_set_lang(type);
  if (ret) {
    ALOGE("[%s %d] set language fail", __func__, __LINE__);
    return;
  }

  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();
  if (type == 0) {
      qApp->removeTranslator(&rotate_signgleton->translator);
  } else {
      rotate_signgleton->translator.load(":icon/setting/qt_gui_en_US.qm");
      qApp->installTranslator(&rotate_signgleton->translator);
  }
  rotate_signgleton->notes_init();
}

