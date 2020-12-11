/*
* QtMainWindow.cpp define a main window inherit from QMainWindow
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
#define LOG_TAG "QtMainWindow"
#include <log/log.h>
#include "file_ops.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "QtMainWindow.hpp"

QtMainWindow::QtMainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  ALOGD("%s:%d", __func__, __LINE__);
  connect(this, SIGNAL(signalForMainUI(int, int)),
          this, SLOT(slot_for_mainui(int, int)));
}

QtMainWindow::~QtMainWindow()
{
  ALOGD("%s >> %s:%d", className, __func__, __LINE__);
  disconnect(SIGNAL(signalForMainUI(int, int)));
}

void QtMainWindow::deinit() {
  this->deleteLater();
  /* file_system_sync(); */
}

void QtMainWindow::slot_for_mainui(int event, int value) {
  ALOGD("[%s] %s:%d >> event=%d, value=%d",
    className, __func__, __LINE__, event, value);
}

/**
 * @brief sendSignalForStorage
 * @note
 * @param  event:
 * @param  value:
 * @retval None
 */
void QtMainWindow::sendSignalForMainUI(int event, int value) {
  ALOGD("[%s] %s:%d >> event=%d, value=%d",
    className, __func__, __LINE__, event, value);
  emit signalForMainUI(event, value);
}

