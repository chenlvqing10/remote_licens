/*
* ui_count_dialog.cpp - define count dialog interface function of UI
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

#include <math.h>
#include <QStandardItemModel>
#include <QObject>
#include <QValidator>
#include <QDebug>

#include "ui_tip_dialog.hpp"
#include "ui_count_dialog.hpp"
#include "ui_ui_count_dialog.h"
#include "ui_rotate_management.hpp"

#define TRIGGER_INTERVAL 200

ui_count_dialog::ui_count_dialog(count_args_t *args,
    QWidget *parent, int _step, int _decimals) :
    QtDialog(parent),
    ui(new Ui::ui_count_dialog),
    decimals(_decimals) {
  count_info = new count_args_t;
  count_info->id = args->id;
  count_info->title = QString(args->title);
  count_info->min = args->min;
  count_info->max = args->max;
  count_info->default_value = args->default_value;
  count_info->cb = args->cb;
  cur_value = count_info->default_value;

  this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

#if 0
  setAutoFillBackground(false);
  setAttribute(Qt::WA_TranslucentBackground, true);
#endif

  connect(ui->count_confirm_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_confirm()));
  connect(ui->count_cancel_btn, SIGNAL(clicked()),
              this, SLOT(slot_for_cancel()));

  ui->count_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");

  ui->tip_title_label->setAlignment(Qt::AlignCenter);
  ui->tip_title_label->setText(count_info->title);
  ui->count_edit->setText(QString("%1").arg(cur_value / pow(10, decimals)));
  // ui->count_edit->setEnabled(false);
  QValidator *vail;
  if (decimals == 0) {
    /*
    vail = new QIntValidator(count_info->min,
      count_info->max, ui->count_edit);
    */
    vail = new QIntValidator(ui->count_edit);
  } else {
    /*
    vail = new QDoubleValidator(count_info->min / pow(10, decimals),
      count_info->max / pow(10, decimals), decimals, ui->count_edit);
    */
    vail = new QDoubleValidator(ui->count_edit);
    qobject_cast<QDoubleValidator *>(vail)->setDecimals(decimals);
  }
  ui->count_edit->setValidator(vail);
  ui->count_cancel_btn->setFocus();

  p_time = new QTimer(this);
  p_time->setInterval(TRIGGER_INTERVAL);

  connect(ui->count_down_btn, SIGNAL(pressed()),
          this, SLOT(slot_for_start_down()));
  connect(ui->count_down_btn, SIGNAL(released()),
              this, SLOT(slot_for_stop()));
  connect(ui->count_add_btn, SIGNAL(pressed()),
          this, SLOT(slot_for_start_add()));
  connect(ui->count_add_btn, SIGNAL(released()),
              this, SLOT(slot_for_stop()));
  connect(p_time, SIGNAL(timeout()),
              this, SLOT(slot_for_timeout()));
  showMaxLayout();
}

ui_count_dialog::~ui_count_dialog() {
  delete ui;
  delete count_info;
  if (p_time->isActive())
    p_time->stop();
  delete p_time;
}

void ui_count_dialog::slot_for_start_add() {
  if (cur_value >= count_info->max ||
    p_time->isActive())
    return;
  QString count = ui->count_edit->text();
  float tmp = count.toFloat();
  if (0 == tmp && 0 != QString::compare(count, "0")) {
    ALOGW("count_edit=%s err to int", count.toUtf8().data());
  } else {
    cur_value = tmp * static_cast<int>(pow(10, decimals));
    if (cur_value >= count_info->max)
      return;
  }
  step = 1;
  cur_value += step;
  ui->count_edit->setText(QString("%1").arg(cur_value / pow(10, decimals)));
  p_time->start();
}

void ui_count_dialog::slot_for_start_down() {
  if (cur_value <= count_info->min ||
    p_time->isActive())
    return;
  QString count = ui->count_edit->text();
  float tmp = count.toFloat();
  if (0 == tmp && 0 != QString::compare(count, "0")) {
    ALOGW("count_edit=%s err to int", count.toUtf8().data());
  } else {
    cur_value = tmp * static_cast<int>(pow(10, decimals));
    if (cur_value <= count_info->min)
      return;
  }
  step = -1;
  cur_value += step;
  ui->count_edit->setText(QString("%1").arg(cur_value / pow(10, decimals)));
  p_time->start();
}

void ui_count_dialog::slot_for_stop() {
  if (p_time->isActive()) {
    p_time->stop();
  }
}

void ui_count_dialog::slot_for_timeout() {
  if ((cur_value <= count_info->min && -1 == step) ||
    (cur_value >= count_info->max && 1 == step))
    return;
  cur_value += step;
  ui->count_edit->setText(QString("%1").arg(cur_value / pow(10, decimals)));
}

void ui_count_dialog::slot_for_confirm() {
  if (count_info->cb) {
    QString count = ui->count_edit->text();
    float tmp = count.toFloat();

    if ((0 == tmp && 0 != QString::compare(count, "0")) ||
        tmp < count_info->min / pow(10, decimals) ||
        tmp > count_info->max / pow(10, decimals)) {
      ALOGW("count_edit=%s err to int", count.toUtf8().data());
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
      QString content = QString(tr("请输入%1-%2的数值"))
          .arg(count_info->min / pow(10, decimals))
          .arg(count_info->max / pow(10, decimals));
      w->set_content(content.toUtf8().data());
      w->show();

      return;
    } else {
      count_info->cb->count_callback(
          count_info->id, tmp * static_cast<int>(pow(10, decimals)));
    }
  }
  deinit();
}

void ui_count_dialog::slot_for_cancel() {
  if (count_info->cb)
    count_info->cb->count_callback(
      count_info->id, count_info->default_value);
  deinit();
}

void ui_count_dialog::deinit() {
  this->deleteLater();
}

