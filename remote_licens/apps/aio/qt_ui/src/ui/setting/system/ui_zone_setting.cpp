/*
* ui_zone_setting.hpp define zone setting function of UI
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
#include "ui_control.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>
#include <QModelIndex>
#include <QScrollBar>

#include "ui_zone_setting.hpp"
#include "ui_ui_zone_setting.h"
#include "ui_rotate_management.hpp"

ui_zone_setting::ui_zone_setting(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_zone_setting), model(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();

  /*read from db*/
  enum_timezone_t zone_area = ui_control_get_timezone();
  zone = 0;
  QStringList name_list;
  name_list << tr("北京") << tr("东京") << tr("纽约")
            << tr("伦敦") << tr("巴黎") << tr("柏林");
  list_view = ui->zone_setting_listView;
  model = new QStandardItemModel();
  for (int i = 0; i < name_list.size(); i++) {
    QStandardItem* item = new QStandardItem();
    data_zone_setting_delegate data;
    data.text = name_list.at(i);
    switch (i) {
      case 0:
        label_for_beijing = new clicked_Label(list_view);
        label_for_beijing->setObjectName("label_zone_beijing");

        data.widget = label_for_beijing;
        if (zone_area == Beijing)
          label_for_beijing->setStyleSheet(
              "#label_zone_beijing {border-image: "
              "url(:icon/setting/choice.png);}");
        break;

      case 1:
        label_for_tokyo = new clicked_Label(list_view);
        label_for_tokyo->setObjectName("label_zone_tokyo");

        data.widget = label_for_tokyo;
        if (zone_area == Tokyo)
          label_for_tokyo->setStyleSheet(
              "#label_zone_tokyo {border-image: "
              "url(:icon/setting/choice.png);}");
        break;

      case 2:
        label_for_newyork = new clicked_Label(list_view);
        label_for_newyork->setObjectName("label_zone_newyork");

        data.widget = label_for_newyork;
        if (zone_area == New_York)
          label_for_newyork->setStyleSheet(
              "#label_zone_newyork {border-image: "
              "url(:icon/setting/choice.png);}");
        break;

      case 3:
        label_for_london = new clicked_Label(list_view);
        label_for_london->setObjectName("label_zone_london");

        data.widget = label_for_london;
        if (zone_area == London)
          label_for_london->setStyleSheet(
              "#label_zone_london {border-image: "
              "url(:icon/setting/choice.png);}");
        break;

      case 4:
        label_for_paris = new clicked_Label(list_view);
        label_for_paris->setObjectName("label_zone_paris");

        data.widget = label_for_paris;
        if (zone_area == Paris)
          label_for_paris->setStyleSheet(
              "#label_zone_paris {border-image: "
              "url(:icon/setting/choice.png);}");
        break;

      case 5:
        label_for_berlin = new clicked_Label(list_view);
        label_for_berlin->setObjectName("label_zone_berlin");

        data.widget = label_for_berlin;
        if (zone_area == Berlin)
          label_for_berlin->setStyleSheet(
              "#label_zone_berlin {border-image: "
              "url(:icon/setting/choice.png);}");
        break;
    }
    item->setData(QVariant::fromValue(data), Qt::UserRole + 1);
    model->appendRow(item);
  }
  delegate = new ui_zone_setting_delegate();
  list_view->setModel(model);
  list_view->setItemDelegate(delegate);
  list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

  connect(list_view, SIGNAL(clicked(QModelIndex)),
    this, SLOT(slot_for_listview_clicked(QModelIndex)));

  connect(ui->zone_setting_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
}

void ui_zone_setting::slot_for_listview_clicked(const QModelIndex& index) {

  clear_label();

  switch (index.row()) {
      case 0:
        slot_for_beijing_clicked();
      break;
      case 1:
        slot_for_tokyo_clicked();
      break;
      case 2:
        slot_for_newyork_clicked();
      break;
      case 3:
        slot_for_london_clicked();
      break;
      case 4:
        slot_for_paris_clicked();
      break;
      case 5:
        slot_for_berlin_clicked();
      break;
  }
}

void ui_zone_setting::slot_for_scrollbar(int value) {
  if (value > 0) label_for_auto->hide();
  else
    label_for_auto->show();
}

void ui_zone_setting::clear_label() {
  label_for_beijing->setStyleSheet(
      "#label_zone_beijing  {border-image: url(:/icons/choice1.png);}");
  label_for_tokyo->setStyleSheet(
      "#label_zone_tokyo  {border-image: url(:/icons/choice1.png);}");
  label_for_newyork->setStyleSheet(
      "#label_zone_newyork  {border-image: url(:/icons/choice1.png);}");
  label_for_london->setStyleSheet(
      "#label_zone_london  {border-image: url(:/icons/choice1.png);}");
  label_for_paris->setStyleSheet(
      "#label_zone_paris  {border-image: url(:/icons/choice1.png);}");
  label_for_berlin->setStyleSheet(
      "#label_zone_berlin  {border-image: url(:/icons/choice1.png);}");
}
void ui_zone_setting::slot_for_auto_clicked() {
}
void ui_zone_setting::slot_for_beijing_clicked() {
  clear_label();
  label_for_beijing->setStyleSheet(
      "#label_zone_beijing  {border-image: url(:icon/setting/choice.png);}");
  ui_control_set_timezone(Beijing);
  device_time_param_t p_info;
	memset(&p_info, 0, sizeof(device_time_param_t));
	db_device_time_param_query(&p_info);
  strcpy(p_info.timezone, "Asia/Shanghai");
	db_device_time_param_update(&p_info);
}
void ui_zone_setting::slot_for_tokyo_clicked() {
  clear_label();
  label_for_tokyo->setStyleSheet(
      "#label_zone_tokyo  {border-image: url(:icon/setting/choice.png);}");
  ui_control_set_timezone(Tokyo);
  device_time_param_t p_info;
	memset(&p_info, 0, sizeof(device_time_param_t));
	db_device_time_param_query(&p_info);
  strcpy(p_info.timezone, "Asia/Tokyo");
	db_device_time_param_update(&p_info);
}
void ui_zone_setting::slot_for_newyork_clicked() {
  clear_label();
  label_for_newyork->setStyleSheet(
      "#label_zone_newyork  {border-image: url(:icon/setting/choice.png);}");
  ui_control_set_timezone(New_York);
  device_time_param_t p_info;
	memset(&p_info, 0, sizeof(device_time_param_t));
	db_device_time_param_query(&p_info);
	strcpy(p_info.timezone, "America/New_York");
	db_device_time_param_update(&p_info);
}
void ui_zone_setting::slot_for_london_clicked() {
  clear_label();
  label_for_london->setStyleSheet(
      "#label_zone_london  {border-image: url(:icon/setting/choice.png);}");
  ui_control_set_timezone(London);
  device_time_param_t p_info;
	memset(&p_info, 0, sizeof(device_time_param_t));
	db_device_time_param_query(&p_info);
  strcpy(p_info.timezone, "Europe/London");
  db_device_time_param_update(&p_info);
}
void ui_zone_setting::slot_for_paris_clicked() {
  clear_label();
  label_for_paris->setStyleSheet(
      "#label_zone_paris  {border-image: url(:icon/setting/choice.png);}");
  ui_control_set_timezone(Paris);
  device_time_param_t p_info;
	memset(&p_info, 0, sizeof(device_time_param_t));
	db_device_time_param_query(&p_info);
  strcpy(p_info.timezone, "Europe/Paris");
	db_device_time_param_update(&p_info);
}
void ui_zone_setting::slot_for_berlin_clicked() {
  clear_label();
  label_for_berlin->setStyleSheet(
      "#label_zone_berlin  {border-image: url(:icon/setting/choice.png);}");
  ui_control_set_timezone(Berlin);
  device_time_param_t p_info;
	memset(&p_info, 0, sizeof(device_time_param_t));
	db_device_time_param_query(&p_info);
  strcpy(p_info.timezone, "Europe/Berlin");
	db_device_time_param_update(&p_info);
}
void ui_zone_setting::slot_for_back_click() {
  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

ui_zone_setting::~ui_zone_setting() {
  ALOGD("[%s %d] delete", __func__, __LINE__);

  if (model) {
    model->clear();
    delete model;
    model = NULL;
  }

  delete ui;
}

