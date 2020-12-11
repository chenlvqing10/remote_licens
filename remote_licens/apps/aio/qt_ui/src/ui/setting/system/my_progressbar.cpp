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
#include "my_progressbar.hpp"

my_progressbar::my_progressbar(QWidget* parent) : QProgressBar(parent) {
}

void my_progressbar::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  draw_bg(&painter);
  draw_value(&painter);
}
void my_progressbar::draw_bg(QPainter* painter) {
  painter->save();
  painter->setPen(Qt::NoPen);
  painter->setBrush(QColor("#00ffff"));
  painter->drawRect(rect());
  painter->restore();
}

void my_progressbar::draw_value(QPainter* painter) {
  painter->save();
  painter->setPen(Qt::NoPen);

  //定义了画刷则取画刷,可以形成渐变效果
  painter->setBrush(QColor("#f9940e"));

  //计算当前值对应的百分比
  // double step = value / (get - minValue);
  // int progress = (width() - padding * 2) * step;
  int progress = value() * width() / maximum();
  QRect rect(0, 0, progress, height());
  painter->drawRect(rect);
  painter->restore();
}
