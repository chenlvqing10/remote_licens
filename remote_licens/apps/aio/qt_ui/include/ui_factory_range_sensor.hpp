/*
* ui_factory_range_sensor.h - define factory test range sensor interface function of UI
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

#ifndef UI_FACTORY_RANGE_SENSOR_H
#define UI_FACTORY_RANGE_SENSOR_H

#include "QtMainWindow.hpp"

namespace Ui {
class ui_factory_range_sensor;
}

class ui_factory_range_sensor : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_factory_range_sensor(QWidget *parent = 0);
  ~ui_factory_range_sensor();

 private slots:
  void slot_for_factory_range_sensor_test();
  void slot_for_call_funtion();

 private:
  Ui::ui_factory_range_sensor *ui;
  QTimer *pTimer;
};

#endif  //  UI_FACTORY_RANGE_SENSOR_H
