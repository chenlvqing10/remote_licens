/*
* ui_setting_time.hpp define system time setting function of UI
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

#ifndef UI_SETTING_TIME_H
#define UI_SETTING_TIME_H

#include <QStandardItemModel>
#include <map>
#include <QSignalMapper>
#include <QtGui>

#include "QtMainWindow.hpp"
#include "ui_setting_item_delegate.hpp"

#define UNITS_CNT 6

namespace Ui {
class ui_setting_time;
}

typedef struct {
  int min, max, default_value, cur_value;
  QPushButton *add, *down;
  QLabel *value;
} t_unit;

class ui_setting_time : public QtMainWindow
{
  Q_OBJECT
public:
  Q_INVOKABLE explicit ui_setting_time(QWidget *parent = 0);
  ~ui_setting_time();
  void ui_init(void);

private slots:
  void slot_for_item_click(const QModelIndex &index);
  void slot_for_confirm();
  void map_pressed(QWidget* w);
  void slot_for_released();
  void slot_for_timeout();

private:
  Ui::ui_setting_time *ui;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
  t_unit counts[UNITS_CNT];
  QList<item_t> menu_setting;
  QSignalMapper pressed_mapper;
  QTimer *p_time;
  int pressed_index;
  int step;
};

#endif // UI_SETTING_TIME_H