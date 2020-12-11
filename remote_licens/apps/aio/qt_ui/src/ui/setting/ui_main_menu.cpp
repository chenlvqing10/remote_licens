/*
* ui_main_menu.cpp - define interface function of main menu
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
#include "ui_control.h"
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>

#include "ui_main_menu.hpp"

#if ATTENDANCE_MODE == 2
#include "ui_ui_main_menu_wechat.h"
#else
#include "ui_ui_main_menu.h"
#endif

#include "ui_rotate_management.hpp"
#include "ui_user_management.hpp"
#include "ui_network_setting.hpp"
#include "ui_system_setting.hpp"
#include "ui_algorithm_setting.hpp"
#include "ui_access_setting.hpp"
#include "ui_attendance_setting.hpp"
#include "ui_checkin_setting.hpp"

ui_main_menu::ui_main_menu(QWidget *parent) :
  QtMainWindow(parent),
#if ATTENDANCE_MODE == 2
  ui(new Ui::ui_main_menu_wechat) {
#else
  ui(new Ui::ui_main_menu) {
#endif
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);
  ui->main_menu_back_btn->setScaledContents(true);
  connect(ui->main_menu_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_bind_choose()));

  bt_layout_t.append({ui->user_management_btn,
    &ui_user_management::staticMetaObject});
  bt_layout_t.append({ui->network_setting_btn,
    &ui_network_setting::staticMetaObject});
  bt_layout_t.append({ui->system_setting_btn,
    &ui_system_setting::staticMetaObject});
  bt_layout_t.append({ui->algorithm_setting_btn,
    &ui_algorithm_setting::staticMetaObject});

#if ATTENDANCE_MODE == 2
  /* do nothings in attendance wechat */
#else
#if ATTENDANCE_MODE == 0
  ui->attendance_setting_label->setText(tr("通行记录"));
  bt_layout_t.append({ui->attendance_setting_btn,
      &ui_checkin_setting::staticMetaObject});
#else
  bt_layout_t.append({ui->attendance_setting_btn,
    &ui_attendance_setting::staticMetaObject});
#endif
  bt_layout_t.append({ui->checkin_setting_btn,
    &ui_access_setting::staticMetaObject});
#endif
  for (int i = 0; i < bt_layout_t.size(); i++) {
    bt_layout_t.at(i).bt->setProperty("id", QString("%1").arg(i));
    connect(bt_layout_t.at(i).bt, SIGNAL(clicked()),
      this, SLOT(item_clicked()));
  }
}

void ui_main_menu::item_clicked() {
    ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();

    clicked_Label *chk_btn = reinterpret_cast <clicked_Label *>(sender());
    QString id = chk_btn->property("id").toString();
    foreach(it_win_t item, bt_layout_t) {
      if (id.compare(item.bt->property("id").toString()) == 0) {
        rotate_signgleton->start_activity_win(item.win);
        break;
      }
    }
}

ui_main_menu::~ui_main_menu() {
  delete ui;
}

