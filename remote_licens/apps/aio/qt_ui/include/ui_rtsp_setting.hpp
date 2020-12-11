/*
* ui_network_setting.hpp define network setting function of UI
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

#ifndef UI_RTSP_SETTING_H
#define UI_RTSP_SETTING_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "ui_control.h"
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>

#include "ui_setting_item_delegate.hpp"
#include "ui_radio_dialog.hpp"
#include "clicked_label.hpp"
#include "QtMainWindow.hpp"

namespace Ui {
class ui_rtsp_setting;
}

class ui_rtsp_setting : public QtMainWindow,
ui_radio_callback {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_rtsp_setting(QWidget *parent = 0);
  ~ui_rtsp_setting();
  void ui_init();
  void radio_callback(void *id, int index, bool isCancel = false);
  void rtsp_video_pasue();
  void rtsp_video_set();

 private slots:
  void slot_for_setting_rtsp(const QModelIndex & index);

 private:
  Ui::ui_rtsp_setting *ui;
  QStandardItemModel *pModel;
  QList<item_t> menu_rtsp;
  QMap<int, QString> video_resolution;
  ui_setting_item_delegate *pItemDelegate;
  system_param_t system_info;
};

#endif  //  UI_RTSP_SETTING_H
