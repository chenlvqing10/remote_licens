/*
* ui_test_for_draw.cpp - define interface function of UI test
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

#include <sys/time.h>

#include <QPainter>
#include <QPen>
#include <QTime>
#include <QDebug>

#include "ui_test_for_draw.hpp"
#include "ui_ui_test_for_draw.h"

TestForDraw::TestForDraw(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::TestForDraw) {
  ui->setupUi(this);
  x = 0;
  y = 0;
  timer = new QTimer();
  connect(timer, SIGNAL(timeout()), this, SLOT(on_timeout()));
  timer->start(300);
}

TestForDraw::~TestForDraw() {
  delete timer;
  delete ui;
}

void TestForDraw::on_timeout() {
  int time = 0;
  struct timeval tpstart, tpstop;
  gettimeofday(&tpstart, NULL);
  this->repaint();
  gettimeofday(&tpstop, NULL);
  time = tpstop.tv_sec * 1000000 + tpstop.tv_usec -
    (tpstart.tv_sec * 1000000 + tpstart.tv_usec);

  qDebug() << "rrtime= " << time << " us";
}

void TestForDraw::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  painter.setPen(QPen(Qt::green, 6));

  painter.drawRect(QRect(x, y, 100, 100));

  x += 10;

  if (x >= 800) {
    x = 0;
    y += 10;
    if (y >= 600) {
      y = 0;
    }
  }
}

