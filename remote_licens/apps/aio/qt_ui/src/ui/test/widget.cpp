/*
* widget.cpp - define interface function of UI base class
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

#include "widget.hpp"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Widget) {
  ui->setupUi(this);

  setAttribute(Qt::WA_TranslucentBackground, true);
}
Widget::~Widget() {
  delete ui;
}
