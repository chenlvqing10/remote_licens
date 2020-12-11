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

#include "ui_access_setting.hpp"
#include "ui_ui_access_setting.h"
#include "ui_setting_item_delegate.hpp"
#include "ui_access_time_setting.hpp"

#define WG_FORMAT_ROW             0
#define DOOR_OPEN_DELAY_ROW       1
#define DOOR_SENSOR_TYPE_ROW      2
#define DOOR_SENSOR_DELAY_ROW     3
#define ALARM_SWITCH_ROW          4
#define ACCESS_TYPE               5
#define TIME_PERIOD_SETTING_ROW   6

#define DOOR_SENSOR_DELAY_MIN     1
#define DOOR_SENSOR_DELAY_MAX     255

void ui_access_setting::ui_init() {
  wiegand_format_steps[26] = "26bit";
  wiegand_format_steps[34] = "34bit";

  door_open_delay_steps[5] = "5s";
  door_open_delay_steps[10] = "10s";
  door_open_delay_steps[15] = "15s";
  door_open_delay_steps[30] = "30s";

  door_sensor_type_steps[0] = tr("常闭");
  door_sensor_type_steps[1] = tr("常开");
  door_sensor_type_steps[2] = tr("无");

  access_type[0] = tr("入闸");
  access_type[1] = tr("出闸");

  menu_access = {
    {":icon/setting/access/wiegand_format.png",
      tr("韦根输出格式"), "26bit", EN_ITEM_LABEL},
    {":icon/setting/access/door_open_relay.png",
      tr("开门延时(继电器)"), "30s", EN_ITEM_LABEL},
    {":icon/setting/access/door_sensor_type.png",
      tr("门磁类型(ALARM)"), "", EN_ITEM_LABEL},
    {":icon/setting/access/door_sensor_delay.png",
      tr("门磁延时(ALARM)"), "100s", EN_ITEM_LABEL},
    {":icon/setting/access/alarm.png",
      tr("门磁报警"), "", EN_ITEM_SWITCH},
    {":icon/setting/access/access_type.png",
      tr("出入类型"), "", EN_ITEM_LABEL},
    {":icon/setting/access/access_time.png",
      tr("时段设置"), "", EN_ITEM_LABEL},
  };
}

ui_access_setting::ui_access_setting(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_access_setting),
  pModel(NULL),
  pItemDelegate(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  ui_init();

  memset(&access_info, 0, sizeof(access_param_t));
  ui_device_get_access_param(&access_info);

  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();

  pModel = new QStandardItemModel();
  int count = menu_access.count();

  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
    if (WG_FORMAT_ROW == i)
      menu_access[i].value = wiegand_format_steps[access_info.wiegand_protocol];
    else if (DOOR_OPEN_DELAY_ROW == i)
      menu_access[i].value = door_open_delay_steps[
        access_info.relay_delay_time];
    else if (DOOR_SENSOR_TYPE_ROW == i)
      menu_access[i].value = door_sensor_type_steps[access_info.door_sensor];
    else if (DOOR_SENSOR_DELAY_ROW == i)
      menu_access[i].value =
        QString::number(access_info.door_sensor_delay) + "s";
    else if (ALARM_SWITCH_ROW == i)
      menu_access[i].value = QString::number(access_info.alarm_switch);
    else if (ACCESS_TYPE == i)
      menu_access[i].value = access_type[access_info.access_type];
    pItem->setData(QVariant::fromValue(menu_access[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_setting_item_delegate(this);
  ui->access_listView->setItemDelegate(pItemDelegate);
  ui->access_listView->setModel(pModel);

  connect(ui->access_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_access_setting_item(QModelIndex)));

  connect(ui->access_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  ui->access_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_access_setting::~ui_access_setting() {
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

void ui_access_setting::slot_for_access_setting_item(const QModelIndex &index) {
  int i = index.row();
  int ind = 0;
  QString str;
  radio_args_t *radio_info;
  count_args_t *count_info;
  ui_radio_dialog *w;
  ui_count_dialog *w_count;
  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();

  switch (i) {
    case WG_FORMAT_ROW:
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(WG_FORMAT_ROW);
      radio_info->title = QString(menu_access[i].title);
      for (auto iter = wiegand_format_steps.begin();
          iter != wiegand_format_steps.end(); ++iter) {
        radio_info->radio_list << iter.value();
        if (menu_access[i].value.compare(iter.value()) == 0)
          radio_info->default_index = ind;
        ind++;
      }
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
      break;

    case DOOR_OPEN_DELAY_ROW:
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(DOOR_OPEN_DELAY_ROW);
      radio_info->title = QString(menu_access[i].title);
      for (auto iter = door_open_delay_steps.begin();
          iter != door_open_delay_steps.end(); ++iter) {
        radio_info->radio_list << iter.value();
        if (menu_access[i].value.compare(iter.value()) == 0)
          radio_info->default_index = ind;
        ind++;
      }
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
      break;

    case DOOR_SENSOR_TYPE_ROW:
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(DOOR_SENSOR_TYPE_ROW);
      radio_info->title = QString(menu_access[i].title);
      for (auto iter = door_sensor_type_steps.begin();
          iter != door_sensor_type_steps.end(); ++iter) {
        radio_info->radio_list << iter.value();
        if (menu_access[i].value.compare(iter.value()) == 0)
          radio_info->default_index = ind;
        ind++;
      }
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
      break;

    case DOOR_SENSOR_DELAY_ROW:
      count_info = new count_args_t;
      count_info->id = reinterpret_cast<void *>(DOOR_SENSOR_DELAY_ROW);
      count_info->title = QString(menu_access[i].title);
      count_info->min = DOOR_SENSOR_DELAY_MIN;
      count_info->max = DOOR_SENSOR_DELAY_MAX;
      str = menu_access[i].value;
      count_info->default_value = str.left(str.length() - 1).toInt();
      count_info->cb = this;
      w_count = new ui_count_dialog(count_info, this);
      w_count->show();
      delete count_info;
      break;

    case ALARM_SWITCH_ROW:
      if (menu_access[i].value.compare("1") == 0) {
        menu_access[i].value = "0";
        access_info.alarm_switch = 0;
      } else {
        menu_access[i].value = "1";
        access_info.alarm_switch = 1;
      }

      ui_control_set_alarm_switch(access_info.alarm_switch);
      pModel->setData(pModel->index(i, 0),
      QVariant::fromValue(menu_access[i]), Qt::UserRole + 1);
      ui->access_listView->setModel(pModel);
      ui->access_listView->update();
      break;

    case ACCESS_TYPE:
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(ACCESS_TYPE);
      radio_info->title = QString(menu_access[i].title);
      for (auto iter = access_type.begin();
          iter != access_type.end(); ++iter) {
        radio_info->radio_list << iter.value();
        if (menu_access[i].value.compare(iter.value()) == 0)
          radio_info->default_index = ind;
        ind++;
      }
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
      break;

    case TIME_PERIOD_SETTING_ROW:
      rotate_signgleton->start_activity_win(
        &ui_access_time_setting::staticMetaObject);
      break;

    default:
      break;
  }
}

void ui_access_setting::radio_callback(void *id, int index,
  bool isCancel) {
  int row = (int)id;

  switch (row) {
    case WG_FORMAT_ROW:
      wg_format_radio_callback(index);
      break;

    case DOOR_OPEN_DELAY_ROW:
      open_door_delay_radio_callback(index);
      break;

    case DOOR_SENSOR_TYPE_ROW:
      door_sensor_type_radio_callback(index);
      break;

    case ACCESS_TYPE:
      access_type_radio_callback(index);

    default:
      break;
  }
}

void ui_access_setting::count_callback(void *id, int count) {
  int ret;

  if (count == access_info.door_sensor_delay) {
    ALOGD("[%s %d] parameter no change", __func__, __LINE__);
    return;
  }

  if (count < DOOR_SENSOR_DELAY_MIN || count > DOOR_SENSOR_DELAY_MAX) {
    ui_tip_dialog *w = new ui_tip_dialog(
        TIP_WIN_TYPE_DOOR_SENSOR_DELAY_TIME_FAIL, this);
    w->show();
    return;
  }

  ret = ui_control_set_door_sensor_delay(count);
  if (ret) {
    ALOGE("[%s %d] set door sensor delay fail", __func__, __LINE__);
    return;
  }
  ALOGD("[%s:%d] set door sensor delay successful, door_sensor_delay %d",
    __func__, __LINE__, count);

  access_info.door_sensor_delay = count;
  menu_access[DOOR_SENSOR_DELAY_ROW].value = QString::number(count) + "s";

  pModel->setData(pModel->index(DOOR_SENSOR_DELAY_ROW, 0),
      QVariant::fromValue(menu_access[DOOR_SENSOR_DELAY_ROW]),
      Qt::UserRole + 1);
  ui->access_listView->setModel(pModel);
  ui->access_listView->update();
}

void ui_access_setting::wg_format_radio_callback(int index) {
  int m_wg_protocol, ret;

  if (index == 0)
    m_wg_protocol = 26;
  else
    m_wg_protocol = 34;

  if (m_wg_protocol == access_info.wiegand_protocol) {
    ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
    return;
  }

  ret = ui_control_set_wiegand_protocol(m_wg_protocol);
  if (ret) {
    ALOGE("[%s:%d] set wiegand format fail", __func__, __LINE__);
    return;
  }
  ALOGD("[%s:%d] set wiegand successful, wiegand %d",
    __func__, __LINE__, m_wg_protocol);

  access_info.wiegand_protocol = (m_wg_protocol);
  menu_access[WG_FORMAT_ROW].value = wiegand_format_steps[m_wg_protocol];

  pModel->setData(pModel->index(WG_FORMAT_ROW, 0),
      QVariant::fromValue(menu_access[WG_FORMAT_ROW]), Qt::UserRole + 1);
  ui->access_listView->setModel(pModel);
  ui->access_listView->update();
}

void ui_access_setting::open_door_delay_radio_callback(int index) {
  int i = 0;
  int ret, m_open_door_delay;

  QMapIterator<int, QString> m_map(door_open_delay_steps);
  while (m_map.hasNext()) {
    m_map.next();
    if (i == index) {
      m_open_door_delay = m_map.key();
      break;
    }
    i++;
  }

  if (m_open_door_delay == access_info.relay_delay_time) {
    ALOGD("[%s %d] parameter no change", __func__, __LINE__);
    return;
  }

  ret = ui_control_set_delay_time(m_open_door_delay);
  if (ret) {
    ALOGE("[%s %d] set open door delay fail", __func__, __LINE__);
    return;
  }
  ALOGD("[%s:%d] set open door delay successful, open_door_delay %d",
    __func__, __LINE__, m_open_door_delay);

  access_info.relay_delay_time = m_open_door_delay;
  menu_access[DOOR_OPEN_DELAY_ROW].value = door_open_delay_steps
    [m_open_door_delay];

  pModel->setData(pModel->index(DOOR_OPEN_DELAY_ROW, 0),
      QVariant::fromValue(menu_access[DOOR_OPEN_DELAY_ROW]), Qt::UserRole + 1);
  ui->access_listView->setModel(pModel);
  ui->access_listView->update();
}

void ui_access_setting::door_sensor_type_radio_callback(int index) {
  int ret;

  if (index == (int)access_info.door_sensor) {
    ALOGD("[%s %d] parameter no change", __func__, __LINE__);
    return;
  }

  ret = ui_control_set_door_sensor_type(index);
  if (ret) {
    ALOGE("[%s %d] set door sensor type fail", __func__, __LINE__);
    return;
  }
  ALOGD("[%s:%d] set door sensor type successful, door_sensor %d",
    __func__, __LINE__, index);

  access_info.door_sensor = index;
  menu_access[DOOR_SENSOR_TYPE_ROW].value = door_sensor_type_steps[index];

  pModel->setData(pModel->index(DOOR_SENSOR_TYPE_ROW, 0),
      QVariant::fromValue(menu_access[DOOR_SENSOR_TYPE_ROW]), Qt::UserRole + 1);
  ui->access_listView->setModel(pModel);
  ui->access_listView->update();
}

void ui_access_setting::access_type_radio_callback(int index) {
  /*TODO:8.12 access_type_radio_callback set access type*/
  int ret;

  if (index == (int)access_info.access_type) {
    ALOGD("[%s %d] parameter no change", __func__, __LINE__);
    return;
  }

  ret = ui_control_set_access_type(index);
  if (ret) {
    ALOGE("[%s %d] set access type fail", __func__, __LINE__);
    return;
  }
  ALOGD("[%s:%d] set access type successful, access type %d",
    __func__, __LINE__, index);

  access_info.access_type = index;
  menu_access[ACCESS_TYPE].value = access_type[index];

  pModel->setData(pModel->index(ACCESS_TYPE, 0),
      QVariant::fromValue(menu_access[ACCESS_TYPE]), Qt::UserRole + 1);
  ui->access_listView->setModel(pModel);
  ui->access_listView->update();
}

