/*
* ui_attendance_data.cpp - define interface function of attendance data
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

#include "ui_attendance_data.hpp"
#include "ui_ui_attendance_data.h"
#include "ui_setting_item_delegate.hpp"
#include "ui_tip_dialog.hpp"

#define TRIGGER_INTERVAL 200

ui_attendance_data::ui_attendance_data(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_attendance_data)
{
	time_t seconds_time = 0;
	struct tm *ptm = NULL;
	time(&seconds_time);
	ptm = localtime(&seconds_time);

  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  ui_rotate_management *rotate_signgleton =
  	ui_rotate_management::getInstance();

  connect(ui->attendance_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
  connect(ui->attendance_confirm_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_confirm()));

	p_time = new QTimer(this);
	p_time->setInterval(TRIGGER_INTERVAL);
	connect(p_time, SIGNAL(timeout()),
			this, SLOT(slot_for_timeout()));

	counts[0].min = 1970;
	counts[0].max = 5000;
	if (0 != ptm->tm_mon)
		counts[0].default_value = ptm->tm_year + 1900;
	else
		counts[0].default_value = ptm->tm_year + 1899;
	counts[0].cur_value = counts[0].default_value;
	counts[0].add = ui->count_add_bt0_y;
	counts[0].down = ui->count_down_bt0_y;
	counts[0].value= ui->time_label_0_y;

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
	counts[1].add = ui->count_add_bt0;
	counts[1].down = ui->count_down_bt0;
	counts[1].value= ui->time_label_0;

	counts[2].min = 1;
	counts[2].max = 31;
	counts[2].default_value = ptm->tm_mday;
	counts[2].cur_value = counts[2].default_value;
	counts[2].add = ui->count_add_bt1;
	counts[2].down = ui->count_down_bt1;
	counts[2].value= ui->time_label_1;

	counts[3].min = 1900;
	counts[3].max = 5000;
	counts[3].default_value = ptm->tm_year + 1900;
	counts[3].cur_value = counts[3].default_value;
	counts[3].add = ui->count_add_bt2_y;
	counts[3].down = ui->count_down_bt2_y;
	counts[3].value= ui->time_label_2_y;

	counts[4].min = 1;
	counts[4].max = 12;
	counts[4].default_value = ptm->tm_mon + 1;
	counts[4].cur_value = counts[4].default_value;
	counts[4].add = ui->count_add_bt2;
	counts[4].down = ui->count_down_bt2;
	counts[4].value= ui->time_label_2;

	counts[5].min = 1;
	counts[5].max = 31;
	counts[5].default_value = ptm->tm_mday;
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
		connect(counts[i].down, SIGNAL(released()), this, SLOT(slot_for_released()));
	}
	connect(&pressed_mapper, SIGNAL(mapped(QWidget*)),
			this, SLOT(map_pressed(QWidget*)));
}

ui_attendance_data::~ui_attendance_data()
{
  ALOGD("%s delete", __func__);
  delete ui;
  if(p_time->isActive())
    p_time->stop();
  delete p_time;
}

void ui_attendance_data::slot_for_confirm() {
	long start = 0, end = 0;
	int ret = 0, ret1 = 0;
	QString s_txt = QString("%1-%2-%3 00:00:00")
		.arg(counts[0].value->text().toUtf8().data())
		.arg(counts[1].value->text().toUtf8().data())
		.arg(counts[2].value->text().toUtf8().data());
	QString e_txt = QString("%1-%2-%3 23:59:59")
		.arg(counts[3].value->text().toUtf8().data())
		.arg(counts[4].value->text().toUtf8().data())
		.arg(counts[5].value->text().toUtf8().data());
	ret = time_str_to_second(s_txt.toUtf8().data(), "%Y-%m-%d %H:%M:%S", &start);
	ret1 = time_str_to_second(e_txt.toUtf8().data(), "%Y-%m-%d %H:%M:%S", &end);
	if (ret ==-1 || ret1 == -1) {
		ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST);
		w->set_content(tr("日期非法，请重新输入！").toUtf8().data());
		w->show();
		return;
	} else if (ret ==0 && ret1 == 0) {
		if (start > end) {
			ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
			w->set_content(tr("抱歉，起始日期不能晚于结束日期！").toUtf8().data());
			w->show();
			return;
		}
	}
	ui_rotate_management *rotate_signgleton =
	  ui_rotate_management::getInstance();
	rotate_signgleton->attendance_num =
		ui->attendance_num_edit->text();
	rotate_signgleton->attendance_start = start;
	rotate_signgleton->attendance_end = end;
	rotate_signgleton->back_activity_win();
}

void ui_attendance_data::map_pressed(QWidget* w) {
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

void ui_attendance_data::slot_for_released() {
  pressed_index = -1;
  if (p_time->isActive()) {
    p_time->stop();
  }
}

void ui_attendance_data::slot_for_timeout() {
  if (-1 == pressed_index)
    return;
  time_unit_t *cur_unit = &counts[pressed_index];
  if ((cur_unit->cur_value <= cur_unit->min && -1 == step) ||
  (cur_unit->cur_value >= cur_unit->max && 1 == step))
    return;
  cur_unit->cur_value += step;
  cur_unit->value->setText(QString::number(cur_unit->cur_value));
}

