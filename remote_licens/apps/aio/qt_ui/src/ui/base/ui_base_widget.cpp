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
#include "ui_base_widget.hpp"

ui_base_widget::ui_base_widget(QWidget* parent) : QWidget(parent) {
  setObjectName("ui_base_widget_n");
  menu_widget = new QWidget(this);
  menu_widget->setObjectName("ui_base_widget_menu_widget");
  click_label_back = new clicked_Label();
  click_label_back->setObjectName("base_back_btn");
  click_label_back->setFixedSize(48, 48);
  label_title = new QLabel();
  label_title->setObjectName("label_title_base_widget");
  label_title->setAlignment(Qt::AlignCenter);
  layout_h = new QHBoxLayout(this);
  layout_h->addWidget(click_label_back, 1);
  layout_h->addStretch(2);
  layout_h->addWidget(label_title, 2);
  layout_h->addStretch(2);
  layout_h->setMargin(0);
  layout_h->setSpacing(6);

  menu_widget->setLayout(layout_h);

  center_widget = new QWidget(this);
  center_widget->setObjectName("ui_base_widget_center_widget");
  layout_v = new QVBoxLayout(this);
  layout_v->setSpacing(6);
  layout_v->setMargin(0);
  layout_v->addWidget(menu_widget, 1);
  layout_v->addWidget(center_widget, 9);
  setLayout(layout_v);

  setFixedSize(1280, 720);
  setWindowFlags(Qt::FramelessWindowHint);
  QObject::connect(click_label_back, SIGNAL(clicked()),
                   this, SLOT(slot_for_back_click()));
}
ui_base_widget::~ui_base_widget() {
  this->deleteLater();
}
void ui_base_widget::set_title_text(QString& text) {
  label_title->setText(text);
}
void ui_base_widget::slot_for_back_click() {
  emit signal_back_click();
}
