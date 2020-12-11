/*
* ui_tip_dialog.cpp - define tip dialog interface function of UI
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

#ifndef UI_TIP_DIALOG_H
#define UI_TIP_DIALOG_H

#include <QMainWindow>

#include "ui_proc_thread.hpp"
#include "QtDialog.hpp"

// tip window type
#define TIP_WIN_TYPE_FACTORY_DEFAULT 0
#define TIP_WIN_TYPE_SYS_UPGRADE 1
#define TIP_WIN_TYPE_FACTORY_TEST 4
#define TIP_WIN_TYPE_SAVE_SUCCESS 5
#define TIP_WIN_TYPE_BATCH_IMPORT 6
#define TIP_WIN_TYPE_CAMERA_CALIBRATION 7
#define TIP_WIN_TYPE_SAVE_FAIL 8
#define TIP_WIN_TYPE_DELETE 9
#define TIP_WIN_TYPE_IP_ERROR 10
#define TIP_WIN_TYPE_CONN_SUCCESS 11
#define TIP_WIN_TYPE_DISCONNECTED 12
#define TIP_WIN_TYPE_REBOOTING 13
#define TIP_WIN_TYPE_ALARM_IN 14
#define TIP_WIN_TYPE_INPUT_ERR 15
#define TIP_WIN_TYPE_UPGRADE_FAIL 16
#define TIP_WIN_TYPE_COPYING 17
#define TIP_WIN_TYPE_ID_ERROR 18
#define TIP_WIN_TYPE_NAME_ERROR 19
#define TIP_WIN_TYPE_LOGIN_ERROR 20
#define TIP_WIN_TYPE_OLD_PWD_ERROR 21
#define TIP_WIN_TYPE_INPUT_PWD_ERROR 22
#define TIP_WIN_TYPE_INPUT_PWD_DISAGREE 23
#define TIP_WIN_TYPE_EXPORT_RECORD 24
#define TIP_WIN_TYPE_DELETE_CONFRIM 25
#define TIP_WIN_TYPE_CONN_FAIL 26
#define TIP_WIN_TYPE_EXIT_IMPORT_PROCESS 27
#define TIP_WIN_TYPE_EXIT_RECOGNITION_PROCESS 28
#define TIP_WIN_TYPE_FACE_NO_INPUT 29
#define TIP_WIN_TYPE_FACE_SIMILARITY_TOO_HIGH 30
#define TIP_WIN_TYPE_CONNECTING 31
#define TIP_WIN_TYPE_SCANNING 32
#define TIP_WIN_TYPE_FACTORY_AUTO_TEST 33
#define TIP_WIN_TYPE_TOAST 34
#define TIP_WIN_TYPE_SNAPSHOT_OK 35
#define TIP_WIN_TYPE_SNAPSHOT_FAIL 36
#define TIP_WIN_TYPE_CALIBRATION_OK 37
#define TIP_WIN_TYPE_CALIBRATION_FAIL 38
#define TIP_WIN_TYPE_ACCESS_TIME_FAIL 39
#define TIP_WIN_TYPE_DOOR_SENSOR_DELAY_TIME_FAIL 40
#define TIP_WIN_TYPE_OLD_NEW_SAME_ERROR 41
#define TIP_WIN_TYPE_NOTE 42
#define TIP_WIN_TYPE_CONFRIM 43
#define TIP_WIN_TYPE_PWD_SAVE_SUCCESS 44
#define TIP_WIN_TYPE_SOURCE 45

namespace Ui {
class ui_tip_dialog;
}

class ui_tip_callback;

typedef struct tip_args {
  void *id;
  QString title;
  QString content;
  QString value;
  ui_tip_callback *cb;
} tip_args_t;

class ui_tip_callback {
 public:
  virtual int tip_callback(tip_args_t *args, bool isCancel = false) = 0;
};

class ui_tip_dialog : public QtDialog, ui_basic_test {
  Q_OBJECT

 public:
  explicit ui_tip_dialog(int type, QWidget* parent = 0);
  explicit ui_tip_dialog(tip_args_t *args, QWidget* parent = 0);
  ~ui_tip_dialog();
  void set_user_win(void *user_win);

 public:
  virtual void proc_func();
  void set_content(char *content);
  void start_count_exit(void);

 public:
  Ui::ui_tip_dialog *get_ui() { return ui; }

 public:
  static void set_factory_default(void *context);
  static void set_sys_upgrade(void *context);
  static void set_factory_test(void *context);
  static void set_factory_auto_test(void *context);
  static void set_save_success(void *context);
  static void set_batch_import(void *context);
  static void set_camera_calibration(void *context);

  static void set_save_fail(void *context);
  static void set_delete(void *context);
  static void set_ip_error(void *context);
  static void set_connect_success(void *context);
  static void set_disconnected(void *context);
  static void set_rebooting(void *context);
  static void set_alarm_in(void *context);
  static void set_input_error(void *context);
  static void set_upgrade_fail(void *context);
  static void set_copying(void *context);
  static void set_id_error(void *context);
  static void set_name_error(void *context);
  static void set_login_error(void *context);
  static void set_old_passwd_error(void *context);
  static void set_old_new_same_error(void *context);
  static void set_input_passwd_error(void *context);
  static void set_input_passwd_disagree(void *context);
  static void set_input_passwd_success(void *context);
  static void set_source(void *context);
  static void set_export_record(void *context);
  static void set_delete_confirm(void *context);
  static void set_connect_fail(void *context);
  static void set_exit_import_process(void *context);
  static void set_exit_recognition_process(void *context);
  static void set_face_no_input(void *context);
  static void set_face_smilarity_too_high(void *context);
  static void set_connecting(void *context);
  static void set_scanning(void *context);
  static void set_toast(void *context);
  static void set_note(void *context);
  static void set_snapshot_ok(void *context);
  static void set_snapshot_fail(void *context);
  static void set_calibration_ok(void *context);
  static void set_calibration_fail(void *context);
  static void set_access_time_fail(void *context);
  static void set_door_sensor_time_fail(void *context);
  static void set_confirm(void *context);

 private slots:
  void sloft_for_update_firmware_confirm();
  void slot_for_time_out_exit();
  void slot_for_check_export_result();
  void slot_for_clean_all_users();
  void slot_for_check_copy_result();
  void slot_for_confirm_delete();
  void slot_for_exit_import_process();
  void slot_for_exit_recognition_process();
  void slot_for_press_ok();
  void slot_for_args_confirm();
  void slot_for_args_cancel();

 private:
  typedef struct tag_func_list {
    int index_;
    void (*func)(void *context);
  } func_list_t;

  static func_list_t func_list_[];

 private:
  Ui::ui_tip_dialog* ui;
  tip_args_t *tip_info;

 public:
  int type;
  QTimer *pTimer;
  void *user_win_;
  proc_thread thread;
  int do_end_;
  int do_result_;
};

#endif  //  UI_TIP_DIALOG_H
