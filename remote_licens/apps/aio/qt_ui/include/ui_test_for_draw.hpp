/*
* ui_test_for_draw.h - define test draw of UI
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

#ifndef UI_TEST_FOR_DRAW_H
#define UI_TEST_FOR_DRAW_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class TestForDraw;
}

class TestForDraw : public QWidget {
  Q_OBJECT

 public:
  explicit TestForDraw(QWidget *parent = nullptr);
  ~TestForDraw();

 protected:
  void paintEvent(QPaintEvent *);

 private slots:
  void on_timeout();

 private:
  int x;
  int y;
  Ui::TestForDraw *ui;
  QTimer *timer;
};

#endif  //  UI_TEST_FOR_DRAW_H
