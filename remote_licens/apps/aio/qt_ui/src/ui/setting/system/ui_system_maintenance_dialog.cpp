/*
* ui_system_maintenance_dialog.cpp -
* define system maintenance dialog interface function of UI
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

#include <QObject>
#include <QDebug>

#include "ui_system_maintenance_dialog.hpp"
#include "ui_ui_system_maintenance_dialog.h"
#include "ui_rotate_management.hpp"

#define UNCHOICE_ICON ":icon/setting/con-unchoice.png"
#define TRIGGER_INTERVAL 200

ui_system_maintenance_dialog::ui_system_maintenance_dialog(
    system_maintenance_args_t *args, QWidget *parent) :
    QtDialog(parent),
    ui(new Ui::ui_system_maintenance_dialog) {
  system_maintenance_info = new system_maintenance_args_t;
  system_maintenance_info->id = args->id;
  system_maintenance_info->title = args->title;
  system_maintenance_info->default_choice = args->default_choice;
  system_maintenance_info->reboot_time = args->reboot_time;
  system_maintenance_info->cb = args->cb;

  this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();
  ui->setupUi(this);

  ui->reboot_time_lab->setText(system_maintenance_info->reboot_time);

  m_group = new QButtonGroup(this);
  m_group->setExclusive(true);
  m_group->addButton(ui->mon_radio_btn, 0);
  m_group->addButton(ui->week_radio_btn, 1);
  m_group->addButton(ui->day_radio_btn, 2);

  if (system_maintenance_info->default_choice == 0)
    ui->mon_radio_btn->setChecked(true);
  else if (system_maintenance_info->default_choice == 1)
    ui->week_radio_btn->setChecked(true);
  else
    ui->day_radio_btn->setChecked(true);


  QStringList time_strs = system_maintenance_info->reboot_time.split(":");
  if (time_strs.count() < 2) {
    ALOGD("[%s][%d] read reboot time error, used default time");
    time_strs.clear();
    time_strs << "03" << "00";
    system_maintenance_info->reboot_time = "03:00";
  }

  p_time = new QTimer(this);
	p_time->setInterval(TRIGGER_INTERVAL);
	connect(p_time, SIGNAL(timeout()), this, SLOT(slot_for_timeout()));

	counts[0].min = 0;
	counts[0].max = 23;
	counts[0].default_value = time_strs[0].toInt();
	counts[0].cur_value = counts[0].default_value;
	counts[0].add = ui->count_add_bt0;
	counts[0].down = ui->count_down_bt0;
	counts[0].value= ui->time_label_0;

	counts[1].min = 0;
	counts[1].max = 59;
	counts[1].default_value = time_strs[1].toInt();
	counts[1].cur_value = counts[1].default_value;
	counts[1].add = ui->count_add_bt1;
	counts[1].down = ui->count_down_bt1;
	counts[1].value= ui->time_label_1;

  pressed_index = -1;

	for (int i = 0; i < 2; i++) {
    if (counts[i].cur_value < 10)
		  counts[i].value->setText("0" + QString::number(counts[i].cur_value));
    else
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

  connect(ui->system_maintenance_confirm_btn, SIGNAL(clicked()),
      this, SLOT(slot_for_confirm()));
  connect(ui->system_maintenance_cancel_btn, SIGNAL(clicked()),
      this, SLOT(slot_for_cancel()));

  ui->system_maintenance_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");

  ui->tip_title_label->setAlignment(Qt::AlignCenter);
  ui->tip_title_label->setText(system_maintenance_info->title);

  showMaxLayout();
}

ui_system_maintenance_dialog::~ui_system_maintenance_dialog() {
  delete system_maintenance_info;
  delete ui;
}

void ui_system_maintenance_dialog::slot_for_confirm() {
  if (!system_maintenance_info->cb)
    return;

  system_maintenance_info->cb->system_maintenance_callback(
      system_maintenance_info->id, m_group->checkedId(),
      ui->reboot_time_lab->text());

  deinit();
}

void ui_system_maintenance_dialog::slot_for_cancel() {
  if (system_maintenance_info->cb)
    system_maintenance_info->cb->system_maintenance_callback(
        system_maintenance_info->id, system_maintenance_info->default_choice,
        system_maintenance_info->reboot_time);

  deinit();
}

void ui_system_maintenance_dialog::deinit() {
  this->deleteLater();
  /*
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->close_radio_dialog_win();
  */
}

void ui_system_maintenance_dialog::map_pressed(QWidget* w) {
  if (!w)
    return;

  for (int i = 0; i < 2; i++) {
    if (w == counts[i].add) {
      if (counts[i].cur_value >= counts[i].max || p_time->isActive())
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
      if (counts[i].cur_value < 10)
  		  counts[i].value->setText("0" + QString::number(counts[i].cur_value));
      else
        counts[i].value->setText(QString::number(counts[i].cur_value));
      p_time->start();
      break;
    } else if (w == counts[i].down) {
      if (counts[i].cur_value <= counts[i].min || p_time->isActive())
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
      if (counts[i].cur_value < 10)
  		  counts[i].value->setText("0" + QString::number(counts[i].cur_value));
      else
        counts[i].value->setText(QString::number(counts[i].cur_value));
      p_time->start();
      break;
    }
  }

  ui->reboot_time_lab->setText(QString("%1:%2")
      .arg(counts[0].value->text())
      .arg(counts[1].value->text()));
}

void ui_system_maintenance_dialog::slot_for_released() {
  pressed_index = -1;
  if(p_time->isActive()) {
    p_time->stop();
  }
}

void ui_system_maintenance_dialog::slot_for_timeout() {
  if (-1 == pressed_index)
    return;
  reboot_time_t *cur_unit = &counts[pressed_index];
  if((cur_unit->cur_value <= cur_unit->min && -1 == step) ||
  (cur_unit->cur_value >= cur_unit->max && 1 == step))
    return;
  cur_unit->cur_value += step;
  if (cur_unit->cur_value < 10)
	  cur_unit->value->setText("0" + QString::number(cur_unit->cur_value));
  else
    cur_unit->value->setText(QString::number(cur_unit->cur_value));

  ui->reboot_time_lab->setText(QString("%1:%2")
      .arg(counts[0].value->text())
      .arg(counts[1].value->text()));
}

