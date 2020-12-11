/*
* ui_time_dialog.cpp - define time dialog interface function of UI
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

#include "ui_time_dialog.hpp"
#include "ui_ui_time_dialog.h"
#include "ui_rotate_management.hpp"

#define TRIGGER_INTERVAL 200

ui_time_dialog::ui_time_dialog(time_args_t *args, QWidget *parent) :
    QtDialog(parent),
    ui(new Ui::ui_time_dialog) {
  time_info = new time_args_t;
  time_info->id = args->id;
  time_info->title = QString(args->title);
  time_info->time = QString(args->time);
  time_info->cb = args->cb;

  this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

#if 0
  setAutoFillBackground(false);
  setAttribute(Qt::WA_TranslucentBackground, true);
#endif

  connect(ui->time_confirm_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_confirm()));
  connect(ui->time_cancel_btn, SIGNAL(clicked()),
              this, SLOT(slot_for_cancel()));

  ui->tip_title_label->setAlignment(Qt::AlignCenter);
  ui->tip_title_label->setText(time_info->title);

  p_time = new QTimer(this);
  p_time->setInterval(TRIGGER_INTERVAL);
  connect(p_time, SIGNAL(timeout()), this, SLOT(slot_for_timeout()));
  QStringList times;
  QStringList time_list = time_info->time.split("--");
  for (int i = 0; i < time_list.size(); i++) {
    times += time_list[i].split(":");
  }

  //  QStringList times = time_info->time.split(QRegExp("[-:]"));
#if 0
  for (int i = 0; i < times.size(); ++i) {
    ALOGD("%s >> [%d]=%s", __func__, i, times.at(i).toUtf8().data());
  }
#endif
  counts[0].min = 0;
  counts[0].max = 23;
  counts[0].default_value = times.at(0).toInt();
  counts[0].cur_value = counts[0].default_value;
  counts[0].add = ui->count_add_bt0;
  counts[0].down = ui->count_down_bt0;
  counts[0].value = ui->time_label_0;

  counts[1].min = 0;
  counts[1].max = 59;
  counts[1].default_value = times.at(1).toInt();
  counts[1].cur_value = counts[1].default_value;
  counts[1].add = ui->count_add_bt1;
  counts[1].down = ui->count_down_bt1;
  counts[1].value = ui->time_label_1;

  counts[2].min = 0;
  counts[2].max = 23;
  counts[2].default_value = times.at(2).toInt();
  counts[2].cur_value = counts[2].default_value;
  counts[2].add = ui->count_add_bt2;
  counts[2].down = ui->count_down_bt2;
  counts[2].value = ui->time_label_2;

  counts[3].min = 0;
  counts[3].max = 59;
  counts[3].default_value = times.at(3).toInt();
  counts[3].cur_value = counts[3].default_value;
  counts[3].add = ui->count_add_bt3;
  counts[3].down = ui->count_down_bt3;
  counts[3].value = ui->time_label_3;
  pressed_index = -1;

  for (int i = 0; i < UNITS_SIZE; i++) {
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
    ui->time_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");

  showMaxLayout();
}

ui_time_dialog::~ui_time_dialog() {
  delete ui;
  delete time_info;
  if (p_time->isActive())
    p_time->stop();
  delete p_time;
}

void ui_time_dialog::slot_for_confirm() {
  QStringList time_list;
  QString tmp;
  if (time_info->cb) {
    for (int i = 0; i < UNITS_SIZE; i++) {
      if (counts[i].value->text().size() == 1)
        time_list << QString("%1%2").arg("0").arg(counts[i].value->text());
      else
        time_list << counts[i].value->text();
    }
    tmp = QString("%1:%2--%3:%4")
      .arg(time_list[0].toUtf8().data())
      .arg(time_list[1].toUtf8().data())
      .arg(time_list[2].toUtf8().data())
      .arg(time_list[3].toUtf8().data());
    time_info->cb->time_callback(
      time_info->id, tmp);
  }
  deinit();
}

void ui_time_dialog::slot_for_cancel() {
#if 0
  if (time_info->cb)
    time_info->cb->time_callback(
      time_info->id, time_info->time);
#endif
  deinit();
}

void ui_time_dialog::map_pressed(QWidget* w) {
  if (!w)
    return;
  for (int i = 0; i < UNITS_SIZE; i++) {
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

void ui_time_dialog::slot_for_released() {
  pressed_index = -1;
  if (p_time->isActive()) {
    p_time->stop();
  }
}

void ui_time_dialog::slot_for_timeout() {
  if (-1 == pressed_index)
    return;
  cout_unit_t *cur_unit = &counts[pressed_index];
  if ((cur_unit->cur_value <= cur_unit->min && -1 == step) ||
  (cur_unit->cur_value >= cur_unit->max && 1 == step))
    return;
  cur_unit->cur_value += step;
  cur_unit->value->setText(QString::number(cur_unit->cur_value));
}

void ui_time_dialog::deinit() {
  this->deleteLater();
}

