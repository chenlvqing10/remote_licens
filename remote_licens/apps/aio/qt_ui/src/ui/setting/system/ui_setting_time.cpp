/*
* ui_setting_time.cpp define system time setting function of UI
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
#include "time_conv.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>
#include <QModelIndex>
#include <time.h>

#include "ui_rotate_management.hpp"

#include "ui_setting_time.hpp"
#include "ui_ui_setting_time.h"
#include "ui_setting_item_delegate.hpp"
#include "ui_tip_dialog.hpp"

#define TRIGGER_INTERVAL 200

#define TIME_AUTO 0
#define TIME_SETTING 1

void ui_setting_time::ui_init() {
  menu_setting = {
    {" ", tr("自动设置"), "", EN_ITEM_SWITCH},
    {" ", tr("手动设置"), "1970/01/01 00:00:00", EN_ITEM_LABEL_ONLY}
  };
}

ui_setting_time::ui_setting_time(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_setting_time) {
  QStandardItem *pItem;
  time_t seconds_time = 0;
  struct tm *ptm = NULL;
  device_time_param_t param;

  time(&seconds_time);
  ptm = localtime(&seconds_time);
  ui_init();

  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  connect(ui->base_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
  connect(ui->time_confirm_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_confirm()));

  memset(&param, 0, sizeof(device_time_param_t));
  ui_control_get_time_param(&param);

  pModel = new QStandardItemModel();
  for (int i = 0; i < menu_setting.count(); ++i) {
    pItem = new QStandardItem;
    if (TIME_AUTO == i)
      menu_setting[i].value = QString::number(param.auto_set_time);
    else if (TIME_SETTING == i)
      menu_setting[i].value = QDateTime::currentDateTime().toString(
        "yyyy/MM/dd hh:mm:ss");
    pItem->setData(QVariant::fromValue(menu_setting[i]),
      Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_setting_item_delegate(this);
  ui->time_setting_listView->setItemDelegate(pItemDelegate);
  ui->time_setting_listView->setModel(pModel);
  connect(ui->time_setting_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_item_click(QModelIndex)));

  p_time = new QTimer(this);
  p_time->setInterval(TRIGGER_INTERVAL);
  connect(p_time, SIGNAL(timeout()),
    this, SLOT(slot_for_timeout()));

  counts[0].min = 1970;
  counts[0].max = 5000;
  counts[0].default_value = ptm->tm_year + 1900;
  counts[0].cur_value = counts[0].default_value;
  counts[0].add = ui->count_add_bt0_y;
  counts[0].down = ui->count_down_bt0_y;
  counts[0].value = ui->time_label_0_y;

  counts[1].min = 1;
  counts[1].max = 12;
  counts[1].default_value = ptm->tm_mon + 1;
  counts[1].cur_value = counts[1].default_value;
  counts[1].add = ui->count_add_bt0;
  counts[1].down = ui->count_down_bt0;
  counts[1].value = ui->time_label_0;

  counts[2].min = 1;
  counts[2].max = 31;
  counts[2].default_value = ptm->tm_mday;
  counts[2].cur_value = counts[2].default_value;
  counts[2].add = ui->count_add_bt1;
  counts[2].down = ui->count_down_bt1;
  counts[2].value = ui->time_label_1;

  counts[3].min = 0;
  counts[3].max = 23;
  counts[3].default_value = ptm->tm_hour;
  counts[3].cur_value = counts[3].default_value;
  counts[3].add = ui->count_add_bt2_y;
  counts[3].down = ui->count_down_bt2_y;
  counts[3].value = ui->time_label_2_y;

  counts[4].min = 0;
  counts[4].max = 59;
  counts[4].default_value = ptm->tm_min;
  counts[4].cur_value = counts[4].default_value;
  counts[4].add = ui->count_add_bt2;
  counts[4].down = ui->count_down_bt2;
  counts[4].value = ui->time_label_2;

  counts[5].min = 0;
  counts[5].max = 59;
  counts[5].default_value = ptm->tm_sec;
  counts[5].cur_value = counts[5].default_value;
  counts[5].add = ui->count_add_bt3;
  counts[5].down = ui->count_down_bt3;
  counts[5].value = ui->time_label_3;
  pressed_index = -1;

  for (int i = 0; i < UNITS_CNT; i++) {
    counts[i].value->setText(QString::number(counts[i].cur_value));
    pressed_mapper.setMapping(counts[i].add, counts[i].add);
    connect(counts[i].add, SIGNAL(pressed()), &pressed_mapper, SLOT(map()));
    connect(counts[i].add, SIGNAL(released()), this, SLOT(slot_for_released()));

    pressed_mapper.setMapping(counts[i].down, counts[i].down);
    connect(counts[i].down, SIGNAL(pressed()), &pressed_mapper, SLOT(map()));
    connect(counts[i].down, SIGNAL(released()),
      this, SLOT(slot_for_released()));
  }
  connect(&pressed_mapper, SIGNAL(mapped(QWidget*)),
    this, SLOT(map_pressed(QWidget*)));

  if (param.auto_set_time) {
    ui->time_setting_listView->setRowHidden(TIME_SETTING, true);
    ui->time_control->hide();
    ui->time_confirm_btn->hide();
  }
}

ui_setting_time::~ui_setting_time() {
  ALOGD("%s delete", __func__);

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }

  if (p_time->isActive())
    p_time->stop();
  delete p_time;

  delete ui;
}

void ui_setting_time::slot_for_item_click(const QModelIndex &index) {
  if (TIME_AUTO == index.row()) {
    if (0 == menu_setting[TIME_AUTO].value.compare("1")) {
      menu_setting[TIME_AUTO].value = "0";
      ui_control_set_auto_time(0);
      ui->time_setting_listView->setRowHidden(TIME_SETTING, false);
      ui->time_control->show();
      ui->time_confirm_btn->show();
    } else {
      menu_setting[TIME_AUTO].value = "1";
      ui_control_set_auto_time(1);
      ui->time_setting_listView->setRowHidden(TIME_SETTING, true);
      ui->time_control->hide();
      ui->time_confirm_btn->hide();
    }
    pModel->setData(pModel->index(TIME_AUTO, 0),
      QVariant::fromValue(menu_setting[TIME_AUTO]), Qt::UserRole + 1);
    ui->time_setting_listView->setModel(pModel);
    ui->time_setting_listView->update();
  }
}

void ui_setting_time::slot_for_confirm() {
  systemtime datetime;
  int ret;
  datetime.year = counts[0].value->text().toInt();
  datetime.month = counts[1].value->text().toInt();
  datetime.day = counts[2].value->text().toInt();
  datetime.hour = counts[3].value->text().toInt();
  datetime.min = counts[4].value->text().toInt();
  datetime.sec = counts[5].value->text().toInt();
  ret = ui_control_set_time(&datetime);
  if (ret == -1) {
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
      w->set_content(tr("日期非法，设置失败！").toUtf8().data());
      w->show();
  } else {
      menu_setting[TIME_SETTING].value =
        QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
      pModel->setData(pModel->index(TIME_SETTING, 0),
        QVariant::fromValue(menu_setting[TIME_SETTING]), Qt::UserRole + 1);
      ui->time_setting_listView->setModel(pModel);
      ui->time_setting_listView->update();
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
      w->set_content(tr("设置成功！").toUtf8().data());
      w->show();
  }
}

void ui_setting_time::map_pressed(QWidget* w) {
  if (!w)
    return;
  for (int i = 0; i < UNITS_CNT; i++) {
    if (w == counts[i].add) {
      if (counts[i].cur_value >= counts[i].max ||
        p_time->isActive())
        return;
      QString count = counts[i].value->text();
      int tmp = count.toInt();
      if (0 == tmp && 0 != QString::compare(count, "0")) {
        ALOGW("count_edit=%s err to int", count.toUtf8().data());
      } else {
        counts[i].cur_value = tmp;
        if (counts[i].cur_value >= counts[i].max)
          return;
      }
      step = 1;
      pressed_index = i;
      counts[i].cur_value += step;
      counts[i].value->setText(QString::number(counts[i].cur_value));
      p_time->start();
      return;
    } else if (w == counts[i].down) {
      if (counts[i].cur_value <= counts[i].min ||
        p_time->isActive())
        return;
      QString count = counts[i].value->text();
      int tmp = count.toInt();
      if (0 == tmp && 0 != QString::compare(count, "0")) {
        ALOGW("count_edit=%s err to int", count.toUtf8().data());
      } else {
        counts[i].cur_value = tmp;
        if (counts[i].cur_value <= counts[i].min)
          return;
      }
      step = -1;
      pressed_index = i;
      counts[i].cur_value += step;
      counts[i].value->setText(QString::number(counts[i].cur_value));
      p_time->start();
      return;
    }
  }
}

void ui_setting_time::slot_for_released() {
  pressed_index = -1;
  if (p_time->isActive()) {
    p_time->stop();
  }
}

void ui_setting_time::slot_for_timeout() {
  if (-1 == pressed_index)
    return;
  t_unit *cur_unit = &counts[pressed_index];
  if ((cur_unit->cur_value <= cur_unit->min && -1 == step) ||
  (cur_unit->cur_value >= cur_unit->max && 1 == step))
    return;
  cur_unit->cur_value += step;
  cur_unit->value->setText(QString::number(cur_unit->cur_value));
}

