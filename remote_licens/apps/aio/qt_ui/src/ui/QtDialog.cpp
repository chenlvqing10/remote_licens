/*
* QtDialog.cpp define a main window inherit from QMainWindow
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
#endif
#define LOG_TAG "QtDialog"
#include <log/log.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "QtDialog.hpp"
#include "screen_size.hpp"

QtDialog::QtDialog(QWidget *parent) :
  QMainWindow(parent)
{
  screen_size sc;
  resize(sc.get_screen_width(), sc.get_screen_height());
}

QtDialog::~QtDialog()
{
  ALOGD("%s >> %s:%d", className, __func__, __LINE__);
}

void QtDialog::showMaxLayout() {
  screen_size sc;
  this->resize(sc.get_screen_width(), sc.get_screen_height());
  this->showMaximized();
  setClassName();
}

