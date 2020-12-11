/*
* screen_size.cpp - define screen size
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
#include "screen_size.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */
#include "screen_size.hpp"

screen_size::screen_size(QObject *parent) : QObject(parent) {
  QDesktopWidget *p_desktop = QApplication::desktop();
  this->screen_width = p_desktop->screenGeometry().width();
  this->screen_height = p_desktop->screenGeometry().height();
}

screen_size::~screen_size() {
}

unsigned int screen_size::get_screen_width() {
  return this->screen_width;
}

unsigned int screen_size::get_screen_height() {
  return this->screen_height;
}

unsigned int get_screen_width() {
  screen_size sc;
  return sc.get_screen_width();
}

unsigned int get_screen_height() {
  screen_size sc;
  return sc.get_screen_height();
}

bool is_horizontal_screen() {
  screen_size sc;
  if (sc.get_screen_width() > sc.get_screen_height())
    return true;
  return false;
}

