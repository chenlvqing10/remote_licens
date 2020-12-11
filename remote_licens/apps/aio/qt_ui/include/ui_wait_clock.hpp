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

#ifndef UI_WAIT_CLOCK_H
#define UI_WAIT_CLOCK_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QToolTip>
#include <QSystemTrayIcon>

namespace Ui {
class ui_wait_clock;
}

class ui_wait_clock : public QMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_wait_clock(QWidget *parent = 0);
  ~ui_wait_clock();
  void deinit();

 protected:
  void paintEvent(QPaintEvent *event);
  void drawHourHand(QPainter *painter);
  void drawMinuteHand(QPainter *painter);
  void drawSecondHand(QPainter *painter);

  void drawClockDial(QPainter *painter);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);

  void createTray();

 private:
  static const QPoint hourHand[4];
  static const QPoint minuteHand[4];
  static const QPoint secondHand[4];

  QPen mHourHandPen;
  QPen mMinuteHandPen;
  QPen mSecondHandPen;

  QPoint mPos;

  QSystemTrayIcon *systemTray;
  Ui::ui_wait_clock *ui;
  QTimer *pTimer;
  bool is_reset;
  int lang;
};

#endif  //  UI_WAIT_CLOCK_H
