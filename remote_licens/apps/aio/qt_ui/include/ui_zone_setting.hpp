/*
* ui_zone_setting.hpp define zone setting function of UI
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

#ifndef UI_ZONE_SETTING_H
#define UI_ZONE_SETTING_H

#include <QObject>
#include <QListView>
#include <QStandardItemModel>

#include "QtMainWindow.hpp"
#include "ui_zone_setting_delegate.hpp"
#include "clicked_label.hpp"

namespace Ui {
class ui_zone_setting;
}

class ui_zone_setting : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_zone_setting(QWidget *parent = 0);
  ~ui_zone_setting();

 private slots:
  void slot_for_auto_clicked();
  void slot_for_beijing_clicked();
  void slot_for_tokyo_clicked();
  void slot_for_newyork_clicked();
  void slot_for_london_clicked();
  void slot_for_paris_clicked();
  void slot_for_berlin_clicked();
  void slot_for_back_click();
  void slot_for_scrollbar(int value);
  void slot_for_listview_clicked(const QModelIndex& index);

 private:
  void clear_label();

 private:
  clicked_Label *label_for_auto;
  clicked_Label *label_for_beijing;
  clicked_Label *label_for_tokyo;
  clicked_Label *label_for_newyork;
  clicked_Label *label_for_london;
  clicked_Label *label_for_paris;
  clicked_Label *label_for_berlin;

  QListView *list_view;
  QStandardItemModel *model;
  ui_zone_setting_delegate *delegate;
  int zone;
  Ui::ui_zone_setting *ui;
};

#endif // UI_ZONE_SETTING_H
