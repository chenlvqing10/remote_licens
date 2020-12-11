/*
* ui_attendance_data_dialog.cpp - define time dialog interface function of UI
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
#include "time_conv.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>

#include "ui_attendance_search.hpp"
#include "ui_ui_attendance_search.h"
#include "ui_rotate_management.hpp"

#define TRIGGER_INTERVAL 200

ui_attendance_search::ui_attendance_search(
  QWidget *parent) :QtMainWindow(parent),
  ui(new Ui::ui_attendance_search) {
  time_t seconds_time = 0;
  QString str_mon, str_day, str_mon_p , start, end;
  struct tm *ptm = NULL;

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  time(&seconds_time);
  ptm = localtime(&seconds_time);
  this->setWindowFlags(Qt::FramelessWindowHint);

  ui->setupUi(this);

  connect(ui->time_confirm_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_confirm()));
  connect(ui->attendance_search_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  p_time = new QTimer(this);
  p_time->setInterval(TRIGGER_INTERVAL);
  connect(p_time, SIGNAL(timeout()), this, SLOT(slot_for_timeout()));

  m_group = new QButtonGroup(this);
  m_group->addButton(ui->attendance_search_time_start, 0);
  m_group->addButton(ui->attendance_search_time_end, 1);
  connect(m_group, SIGNAL(buttonClicked(int)),
    this, SLOT(time_select_clicked(int)));

  counts[0].min = 1970;
  counts[0].max = 5000;
  counts[0].default_value = ptm->tm_year + 1900;
  counts[0].add = ui->count_add_bt0;
  counts[0].down = ui->count_down_bt0;
  counts[0].value = ui->attendance_search_unit_y;

  counts[1].min = 1;
  counts[1].max = 12;
  if (0 != ptm->tm_mon) {
    counts[1].default_value = ptm->tm_mon;
  } else {
    if (1970 != counts[0].default_value) {
      counts[1].default_value = 12;
      counts[0].default_value -= 1;
    } else {
      counts[1].default_value = ptm->tm_mon +1;
    }
  }
  counts[1].cur_value = counts[1].default_value;
  counts[1].add = ui->count_add_bt2;
  counts[1].down = ui->count_down_bt2;
  counts[1].value = ui->attendance_search_unit_m;

  counts[2].min = 1;
  counts[2].max = 31;
  counts[2].default_value = ptm->tm_mday;
  counts[2].cur_value = counts[2].default_value;
  counts[2].add = ui->count_add_bt3;
  counts[2].down = ui->count_down_bt3;
  counts[2].value = ui->attendance_search_unit_d;

  for (int i = 0; i < TIME_UNITS; i++) {
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

    str_mon = QString::number(ptm->tm_mon);
    str_mon_p = QString::number(ptm->tm_mon+1);
    str_day = QString::number(ptm->tm_mday);
  if (ptm->tm_year + 1900 != 1970) {
    start = (0 != ptm->tm_mon)?
      QString("%1/%2/%3")
      .arg(ptm->tm_year + 1900)
      .arg(str_mon)
      .arg(str_day):
      QString("%1/%2/%3")
      .arg(ptm->tm_year + 1899)
      .arg(12)
      .arg(str_day);
  } else {
      start = (0 != ptm->tm_mon)?
        QString("%1/%2/%3")
        .arg(ptm->tm_year + 1900)
        .arg(str_mon)
        .arg(str_day):
        QString("%1/%2/%3")
       .arg(ptm->tm_year + 1900)
       .arg(str_mon_p)
       .arg(str_day);
    }
  end = QString("%1/%2/%3")
    .arg(ptm->tm_year + 1900)
    .arg(str_mon_p)
    .arg(str_day);
  ui->attendance_search_time_start->setText(start);
  ui->attendance_search_time_end->setText(end);
  ui->attendance_search_time_start->setChecked(true);
  time_select_clicked(0);
}

ui_attendance_search::~ui_attendance_search() {
  delete m_group;
  if (p_time->isActive())
    p_time->stop();
  delete p_time;
  delete ui;
}

void ui_attendance_search::slot_for_confirm() {
  long start = 0, end = 0;
  int ret = 0, ret1 = 0;
  QString s_txt, e_txt;
  s_txt = QString("%1 00:00:00")
    .arg(ui->attendance_search_time_start->text());
  e_txt = QString("%1 23:59:59")
      .arg(ui->attendance_search_time_end->text());
  ret = time_str_to_second(s_txt.toUtf8().data(),
    "%Y/%m/%d %H:%M:%S", &start);
  if (start <= 0) {
    ALOGD("Change time[%d] to 1970/1/1 08:00:00", start);
    ret = 0;
    start = 0;
  }

  ret1 = time_str_to_second(e_txt.toUtf8().data(),
    "%Y/%m/%d %H:%M:%S", &end);
  if (ret ==-1 || ret1 == -1) {
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
      w->set_content(
        tr("日期非法，请重新输入！").toUtf8().data());
      w->show();
      return;
  } else if (ret ==0 && ret1 == 0) {
      if (start > end) {
        ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
        w->set_content(
          tr("抱歉，起始日期不能晚于结束日期！").toUtf8().data());
        w->show();
        return;
      }
  }
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  QString txt = ui->attendance_num_edit->text();
  if (txt.isNull())
    txt = QString("");
  rotate_signgleton->attendance_num = txt;
  rotate_signgleton->attendance_start = start;
  rotate_signgleton->attendance_end = end;
  rotate_signgleton->back_activity_win();
}

void ui_attendance_search::time_select_clicked(int id) {
  QList<QAbstractButton *> buttons = m_group->buttons();
  select_bt = (QRadioButton *)m_group->button(id);
  QStringList times = select_bt->text().split("/");
  for (int i = 0; i < times.size(); i++) {
    counts[i].cur_value = times[i].toInt();
    counts[i].value->setText(times[i]);
  }
}

void ui_attendance_search::map_pressed(QWidget* w) {
  if (!w)
    return;
  for (int i = 0; i < TIME_UNITS; i++) {
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
      break;
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
      break;
    }
  }
  select_bt->setText(QString("%1/%2/%3")
    .arg(counts[0].value->text())
    .arg(counts[1].value->text())
    .arg(counts[2].value->text()));
}

void ui_attendance_search::slot_for_released() {
  pressed_index = -1;
  if (p_time->isActive()) {
    p_time->stop();
  }
}

void ui_attendance_search::slot_for_timeout() {
  if (-1 == pressed_index)
    return;
  count_unit_t_s *cur_unit_s = &counts[pressed_index];
  if ((cur_unit_s->cur_value <= cur_unit_s->min && -1 == step) ||
  (cur_unit_s->cur_value >= cur_unit_s->max && 1 == step))
    return;
  cur_unit_s->cur_value += step;
  cur_unit_s->value->setText(QString::number(cur_unit_s->cur_value));
  select_bt->setText(QString("%1/%2/%3")
    .arg(counts[0].value->text())
    .arg(counts[1].value->text())
    .arg(counts[2].value->text()));
}

void ui_attendance_search::deinit() {
  QtMainWindow::deinit();
}

