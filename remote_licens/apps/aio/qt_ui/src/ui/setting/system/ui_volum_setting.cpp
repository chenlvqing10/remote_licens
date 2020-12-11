/*
* ui_base_widget.cpp - define ui_base_widget of project
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

#include <QPainter>
#include <QFrame>
#include <QCloseEvent>

extern "C" {
#define LOG_TAG     "ui"
#include <log/log.h>
#include "ui_control.h"
}
#include "ui_volum_setting.hpp"

ui_volum_setting::ui_volum_setting(QWidget* parent, int type) :
QWidget(parent), _type(type) {
  setFixedSize(1280, 720);
  setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_DeleteOnClose);

  small_dlg = new QWidget(this);
  small_dlg->setGeometry(QRect(220, 168, 840, 380));
  small_dlg->setAttribute(Qt::WA_StyledBackground, true);
  small_dlg->setObjectName("volume_small_dlg");

  label_for_title = new QLabel(small_dlg);
  label_for_title->setGeometry(QRect(330, 32, 180, 40));
  label_for_title->setObjectName("volume_label_for_title");
  /**/
  QFrame* ll = new QFrame(small_dlg);
  ll->setObjectName("volume_ll");
  ll->setStyleSheet("#volume_ll{border:2px solid #102050;}");
  ll->setGeometry(QRect(0, 94, 840, 4));

  label_for_sub = new clicked_Label(small_dlg);
  label_for_sub->setGeometry(QRect(168, 152, 72, 72));
  label_for_sub->setObjectName("volume_label_for_sub");

  label_for_add = new clicked_Label(small_dlg);
  label_for_add->setGeometry(QRect(604, 152, 72, 72));
  label_for_add->setObjectName("volume_label_for_add");

  edit = new QLineEdit(small_dlg);
  edit->setGeometry(QRect(267, 152, 310, 72));
  edit->setObjectName("volume_edit");
  edit->setEnabled(false);
  edit->setAlignment(Qt::AlignCenter);
  btn = new QPushButton(small_dlg);
  btn->setGeometry(QRect(304, 272, 224, 72));
  btn->setObjectName("volume_btn");
  btn->setText(tr("确定"));

  connect(btn, SIGNAL(clicked()), this, SLOT(slot_for_btn_clicked()));
  connect(label_for_sub, SIGNAL(clicked()), this, SLOT(slot_for_sub_clicked()));
  connect(label_for_add, SIGNAL(clicked()), this, SLOT(slot_for_add_clicked()));
  /**/
  screen_bright_t bright;
  memset(&bright, 0, sizeof(screen_bright_t));
  screen_off_status_t bright_status;
  memset(&bright_status, 0, sizeof(screen_off_status_t));

  switch (type) {
    case 0:
      /*volum*/
      label_for_title->setText(tr("音量设置"));
      /*read form db*/
      ui_control_get_volume(&value);
      value += 5;
      value /= 10;
      break;
    case 1:
      /*brightness*/
      label_for_title->setText(tr("亮度设置"));
      /*read form db*/
      ui_device_get_screen_param(&bright, &bright_status);
      value = bright.display_brightness;
      break;
  }
  edit->setText(QString("%1").arg(value));
}
void ui_volum_setting::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.fillRect(this->rect(), QColor(0, 0, 0, 200));
  QWidget::paintEvent(event);
}
void ui_volum_setting::slot_for_add_clicked() {
  value++;
  if (_type == 0) {
    /*volume*/
    if (value > 10) {
      value = 10;
    }
  } else {
    /*brightness*/
    if (value > 100) {
      value = 100;
    }
  }
  edit->setText(QString("%1").arg(value));
}
void ui_volum_setting::slot_for_sub_clicked() {
  value--;
  if (value < 0)
    value = 0;
  edit->setText(QString("%1").arg(value));
}
void ui_volum_setting::slot_for_btn_clicked() {
  if (_type == 0) {
    ui_control_set_volume(value*10);
  } else {
    ui_control_set_display_brightness(value);
  }
  close();
}
void ui_volum_setting::closeEvent(QCloseEvent* event) {
  if (m_Loop != NULL) {
    m_Loop->exit();
  }
  event->accept();
}
int ui_volum_setting::exec() {
  this->show();
  m_Loop = new QEventLoop(this);
  m_Loop->exec();
  return value;
}
