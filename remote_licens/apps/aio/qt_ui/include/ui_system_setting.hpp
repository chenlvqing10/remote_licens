/*
* ui_system_setting.hpp - define face preview type of project
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

#ifndef UI_SYSTEM_SETTING_H
#define UI_SYSTEM_SETTING_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "ui_control.h"
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QModelIndex>
#include <QStandardItemModel>
#include <QDateTime>
#include <QTranslator>
#include <QMap>

#include "QtMainWindow.hpp"
#include "signal_for_ui.hpp"
#include "signal_table.hpp"
#include "ui_system_setting_delegate.hpp"
#include "clicked_label.hpp"
#include "ui_setting_item_delegate.hpp"
#include "ui_count_dialog.hpp"
#include "ui_radio_dialog.hpp"

#define SYS_ITEM_COUNT 8

namespace Ui {
class ui_system_setting;
}

class ui_system_setting : public QtMainWindow,
ui_radio_callback, ui_count_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_system_setting(QWidget *parent = 0);
  ~ui_system_setting();
  void radio_callback(void *id, int index, bool isCancel = false);
  void count_callback(void *id, int count);
  void switch_language(int type);

 private slots:
  void slot_for_system_setting_listview(const QModelIndex &index);

 private:
  void volume_callback(int count);
  void brightness_callback(int count);
  void refresh();
  void ui_init();

 private:
  Ui::ui_system_setting* ui;
  QStandardItemModel *pModel;
  ui_setting_item_delegate *pItemDelegate;
  int rtsp_video_enable;
  QTranslator translator;
  QList<item_t> menu_system_setting;
  QMap<int, QString> language_choose;
  QMap<int, QString> zone_text;
  system_param_t system_info;
};

#endif  //  UI_SYSTEM_SETTING_H
