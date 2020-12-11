/*
* ui_about_setting.hpp define about setting function of UI
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

#ifndef UI_ABOAT_SETTING_H
#define UI_ABOAT_SETTING_H

#include <QStandardItemModel>
#include <QSignalMapper>
#include <QMap>

#include "QtMainWindow.hpp"
#include "ui_rotate_management.hpp"
#include "ui_system_maintenance_dialog.hpp"
#include "ui_setting_item_delegate.hpp"

namespace Ui {
class ui_about_setting;
}

class ui_about_setting : public QtMainWindow,
  ui_system_maintenance_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_about_setting(QWidget *parent = 0);
  ~ui_about_setting();
  void system_maintenance_callback(void *id, int choice, QString time);

 private slots:
  void slot_for_about_setting_item(const QModelIndex &index);

 private:
  void ui_init();

 private:
  Ui::ui_about_setting *ui;
  QList<item_t> menu_about;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
  int development_status;
  int click_count_;
  auto_reboot_param_t auto_reboot_param;
};

#endif // UI_ABOAT_SETTING_H

