/*
* ui_wait_clock.cpp - define interface function of UI
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
#define LOG_TAG   "wait_clock"
#include <log/log.h>
#include "ui_control.h"
#ifdef __cplusplus
  }
#endif /* __cplusplus */

#include <QTimer>
#include <QIcon>
#include <QTime>
#include <QFont>
#include <QCoreApplication>
#include <QDebug>

#include "signal_table.hpp"
#include "signal_for_ui.hpp"
#include "ui_wait_clock.hpp"
#include "ui_ui_wait_clock.h"
#include "ui_rotate_management.hpp"

QMutex mutex;

const QPoint ui_wait_clock::hourHand[4] = {
  QPoint(3, 5),
  QPoint(0, 13),
  QPoint(-3, 5),
  QPoint(0, -40)
};

//  时针绘图区域
const QPoint ui_wait_clock::minuteHand[4] = {
  QPoint(3, 5),
  QPoint(0, 16),
  QPoint(-3, 5),
  QPoint(0, -68)
};

const QPoint ui_wait_clock::secondHand[4] = {
  QPoint(3, 5),
  QPoint(0, 18),
  QPoint(-3, 5),
  QPoint(0, -85)
};

ui_wait_clock::ui_wait_clock(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ui_wait_clock) {
  ALOGD("ui_wait_clock constructor");
  ui->setupUi(this);

  this->hide();
  QTimer::singleShot(1000, this, SLOT(showMaximized()));

  this->setWindowFlags(Qt::FramelessWindowHint |
                       Qt::Tool | Qt::WindowStaysOnTopHint);
  this->setWindowIcon(QIcon(":/ico/clock.ico"));
  this->setWindowTitle(tr("desktop clock"));
  this->resize(800, 600);
  //  this->setMouseTracking(true);
  //  ui->centralwidget->setMouseTracking(true);
  // this->setAttribute(Qt::WA_TranslucentBackground, true);

  device_lang_param_t info;
  db_device_lang_param_query(&info);
  lang = info.lang;

  mHourHandPen = QPen(palette().foreground(), 2.0);
  mMinuteHandPen = QPen(palette().foreground(), 1.0);
  pTimer = new QTimer(this);
  pTimer->start(1000);
  connect(pTimer, SIGNAL(timeout()), this, SLOT(update()));
  // resize(1280, 800);
  ui->clock_wait_show_lable->setAlignment(Qt::AlignCenter);
  resize(1280, 720);
  is_reset = false;
  ui_control_face_resume();

  // this->showMaximized();
}

ui_wait_clock::~ui_wait_clock() {
  ALOGD("ui_wait_clock delete >> %p", QThread::currentThread());
  mutex.lock();
  delete pTimer;
  pTimer = NULL;
  delete ui;
  ui = NULL;
  mutex.unlock();
}

void ui_wait_clock::deinit() {
  if (ui_rotate_management::getInstance()->
    is_face_det_start  == false) {
    ui_control_face_pause();
  }
  pTimer->stop();
  disconnect(pTimer, SIGNAL(timeout()), this, SLOT(update()));
  this->deleteLater();
}

void ui_wait_clock::paintEvent(QPaintEvent *e) {
  ALOGD("%s:%d >> %p", __func__, __LINE__, QThread::currentThread());
  mutex.lock();
  if (NULL == ui) {
    ALOGW("Mainwindow had been release!");
    mutex.unlock();
    return;
  }
  QPainter painter(this);
  QFont font("Microsoft Yahei", 10, 75);
  painter.setFont(font);
  painter.setRenderHint(QPainter::Antialiasing, true);
  // qDebug() << "width: " << this->width() << " height: " << this->height();
  int side = qMin(this->width() / 2, this->height() / 2);

  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(255, 255, 255, 125));
  painter.drawEllipse(QPoint(width() / 2, height() / 2), side / 2, side / 2);

  painter.setPen(QPen(QColor(233, 233, 216), 4));
  painter.drawEllipse(QPoint(width() / 2, height() / 2),
                      side / 2 - 3, side / 2 - 3);

  painter.translate(width() / 2, height() / 2);
  painter.scale(side / 200.0, side / 200.0);

  drawHourHand(&painter);
  drawMinuteHand(&painter);
  drawSecondHand(&painter);
  drawClockDial(&painter);

  painter.setBrush(Qt::black);
  painter.drawEllipse(QPoint(0, 0), 2, 2);

  QDateTime time = QDateTime::currentDateTime();

  QLocale locale;
  if (lang == 1)
    locale = QLocale::English;
  else
    locale = QLocale::Chinese;

  QString str_show = locale.toString(QDate::currentDate());
  ui->clock_wait_show_lable->setText(str_show);
  ALOGD("%s: str_show=%s", __func__, str_show.toUtf8().data());
  mutex.unlock();
}

void ui_wait_clock::drawHourHand(QPainter *painter) {
  QTime time = QTime::currentTime();
  painter->setBrush(Qt::black);
  painter->setPen(Qt::black);
  painter->save();
  painter->rotate(30.0 * (time.hour() + time.minute() / 60.0));
  painter->drawConvexPolygon(hourHand, 4);
  painter->restore();
}

void ui_wait_clock::drawMinuteHand(QPainter *painter) {
  QTime time = QTime::currentTime();
  painter->setBrush(Qt::black);
  painter->setPen(Qt::black);
  painter->save();
  painter->rotate(6.0 * (time.minute() + time.second() / 60.0));
  painter->drawConvexPolygon(minuteHand, 4);
  painter->restore();
}

void ui_wait_clock::drawSecondHand(QPainter *painter) {
  QTime time = QTime::currentTime();
  painter->setBrush(Qt::red);
  painter->setPen(Qt::red);
  painter->save();
  painter->rotate(6.0 * time.second());
  painter->drawConvexPolygon(secondHand, 4);
  painter->restore();
}

void ui_wait_clock::drawClockDial(QPainter* painter) {
  for (int i = 1; i <= 12; ++i) {
    painter->save();
    painter->rotate(30 * i);
    painter->translate(0, -78);
    painter->setBrush(Qt::black);
    painter->setPen(Qt::black);
    painter->rotate(-30 * i);
    painter->drawText(-20, -20, 40, 40,
                      Qt::AlignVCenter | Qt::AlignHCenter,
                      QString::number(i));
    painter->restore();
  }
  for (int i = 1; i <= 60; i++) {
    painter->save();
    painter->rotate(6 * i);

    if (i % 5 == 0) {
      painter->setPen(mHourHandPen);
      painter->drawLine(0, -95, 0, -88);
    } else {
      painter->setPen(mMinuteHandPen);
      painter->drawLine(0, 95, 0, 90);
    }
    painter->restore();
  }
}

void ui_wait_clock::mousePressEvent(QMouseEvent* event) {
  // mPos = (event->globalPos()) - (this->pos());
  if (is_reset) return;
  //  ui_control_wakeup_screen();
  is_reset = true;
}

void ui_wait_clock::mouseMoveEvent(QMouseEvent* event) {
  if (is_reset) return;
  //  ui_control_wakeup_screen();
  is_reset = true;
#if 0
  if (this->isFullScreen() == false) {
    //  this->move(event->globalPos() - mPos);
  }
#endif
}

void ui_wait_clock::createTray() {
  systemTray = new QSystemTrayIcon(this);
  systemTray->setToolTip(tr("desktop clock"));
  systemTray->setIcon(QIcon(":/ico/clock.ico"));
  systemTray->show();
}

