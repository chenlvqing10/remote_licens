/*
* ui_rotate_management.hpp - define slot functions manager of project
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

#ifndef UI_ROTATE_MANAGEMENT_H
#define UI_ROTATE_MANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "ui_control.h"
#include "face.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QModelIndex>
#include <QTimer>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QTranslator>

#include "ui_face_authentication.hpp"
#include "ui_face_scan.hpp"
#include "ui_proc_thread.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_radio_dialog.hpp"
#include "ui_count_dialog.hpp"
#include "ui_time_dialog.hpp"
#include "ui_input_dialog.hpp"
#include "ui_factory_main.hpp"
#include "ui_wait_clock.hpp"
#include "ui_ota.hpp"
#include "wifi_management.hpp"
#include "eth_management.hpp"

#define LOGO_SOURCE "/etc/logo.png"
#define LOGO_PATH "/data/logo.png"

#define TIMER_COUNTS_STOP 0
#define TIMER_COUNTS_START 1
#define TIMER_COUNTS_RESTART 2

typedef struct tag_main_param {
  QPixmap logo;
  QString company;
  QString name;
  int name_format;
  QString cnt;
  QString mac;
  QString ip;
  QString color;
  int isName, isCnt, isMac, isIp;
  un_debug_switch_t debug_switch;
  algorithm_system_info_t recg_params;
} main_param_t;

class ui_rotate_management : public QObject {
  Q_OBJECT

 public:
  static ui_rotate_management *getInstance();
  void update_management();
  user_information_param_t *get_user_info();
  void set_user_info(user_information_param_t *usr_info);
  void set_feat_info(ui_feat_local_t *feat);
  int get_usb_status();
  void set_usb_status(int status);
  int get_ws_status();
  int set_ws_status(int status);
  void start_ota_svc(char isShow = 0);
  int start_activity_win(const QMetaObject *aim_win);
  int is_wait_clock_win_opened();
  int is_menu_win_opened();
  int clean_upgrade_flag();
  void set_update_flag();
  void set_logo_pixmap();
  void set_company_name();
  void set_main_control();
  void set_recg_cnt();
  void set_border_color();
  void set_debug_switch();
  void set_algorithm_params();
  void update_net_state();
  void set_setting_count_state(int state);
  int is_update_firmware();
  void notes_init();

  bool is_net_connected() {
    return isNetConnected;
  }

  main_param_t *get_main_param() {
    return &main_param;
  }

  QtMainWindow *get_current_win() {
    return currnet_win;
  }

 public slots:
  void back_activity_win();
  void slot_for_eth_state(int state);
  void slot_for_wifi_state(int state);
  void slot_show_wait_clock(int);
  void slot_bind_choose();
  void get_factory_test_result(factory_test_result_t *result);
  void set_factory_test_result(factory_test_result_t *result);
  void slot_for_factory_lcd();
  void slot_for_factory_touch(int state);
  void slot_for_factory_camera();
  void slot_for_factory_wifi(int state);
  void slot_for_factory_ble(int state);
  void slot_for_factory_rtc(int state);
  void slot_for_factory_net(int state);
  void slot_for_factory_range_sensor();
  void slot_for_factory_light_sensor(int state);
  void slot_for_factory_light(int state);
  void slot_for_factory_ir(int state);
  void slot_for_factory_speaker(int state);
  void slot_for_factory_relay();
  void slot_for_factory_rs485(int state);
  void slot_for_factory_wiegand(int state);
  void slot_for_factory_alarm();
  void slot_for_factory_usb(int state);
  void slot_for_turn_main();
  void slot_for_factory_default_confirm();
  void slot_for_factory_default_concancel();
  void slot_for_show_webchat(int event);

  void slot_for_factory_auto_exit();
  void slot_for_factory_auto_resume();
  void slot_for_storage(int event, int type);
  void slot_for_dir_changed(QString path);
  void slot_for_update_firmware();

 private:
  // static ui_rotate_management* instance;
  ui_rotate_management &operator =(const ui_rotate_management&);
  ui_rotate_management(const ui_rotate_management&);
  explicit ui_rotate_management(QObject *parent = nullptr);

 public:
  QTranslator translator;
  auto_test_thread auto_test;
  int cursor_status;
  bool is_face_det_start;
  QString attendance_num;
  long attendance_start;
  long attendance_end;
  QMap<RECG_NOTE, QString> recg_notes;

 private:
  QtMainWindow *currnet_win;
  eth_management *eth_mgr;
  wifi_management *wifi_mgr;
  ui_ota *ota;
  bool isNetConnected;
  ui_wait_clock *save_win;
  QList<const QMetaObject*> wins_list;
  QFileSystemWatcher *file_watcher;
  main_param_t main_param;

  int is_recv_unbind_;

  int is_bind;
  user_information_param_t user_info;
  factory_test_result_t test_status;
  ui_face_authentication *websocket_input;
  int usb_status;
  int ws_status;
  int is_upgrading;
};

#endif  //  UI_ROTATE_MANAGEMENT_H
