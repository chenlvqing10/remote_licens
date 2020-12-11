/*
* screen_size.hpp - define screen size
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

#ifndef SCREEN_SIZE_H
#define SCREEN_SIZE_H

#include <QObject>
#include <QApplication>
#include <QDesktopWidget>
#include "screen_size.h"

class screen_size : public QObject {
  Q_OBJECT

 public:
  explicit screen_size(QObject *parent = 0);
  ~screen_size();

  unsigned int get_screen_width();
  unsigned int get_screen_height();

 private:
  unsigned int screen_width;
  unsigned int screen_height;
};

#endif  // SCREEN_SIZE_H

