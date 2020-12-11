/*
* ui_access_tiem_setting.hpp define access time setting function of UI
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

#ifndef UI_ACCESS_TIME_SETTING_HPP
#define UI_ACCESS_TIME_SETTING_HPP

#ifdef __cplusplus
extern "C" {
#endif
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "QtMainWindow.hpp"

#include "ui_time_dialog.hpp"
#include "ui_rotate_management.hpp"
#include "ui_setting_item_delegate.hpp"

namespace Ui {
class ui_access_time_setting;
}

class ui_access_time_setting : public QtMainWindow, ui_time_callback
{
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_access_time_setting(QWidget *parent = 0);
  ~ui_access_time_setting();
  void time_callback(void *id, QString time);

 private slots:
  void slot_for_access_time_item(const QModelIndex &index);

 private:
  Ui::ui_access_time_setting *ui;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
  static item_t menu_access[];
  QStringList access_time_list;
};

#endif  //  UI_ACCESS_TIME_SETTING_HPP
