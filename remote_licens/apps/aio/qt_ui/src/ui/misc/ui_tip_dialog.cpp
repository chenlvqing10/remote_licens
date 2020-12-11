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
#include <QObject>

#include "ui_tip_dialog.hpp"
#include "ui_ui_tip_dialog.h"
#include "ui_rotate_management.hpp"
#include "ui_user_view_list.hpp"
#include "ui_camera_calibration.hpp"
#include "ui_batch_import.hpp"
#include "ui_batch_recognition.hpp"

#define TIMER_INTERVAL_COUNT 2000
#define CONTENT_SOURCE 116
#define CONTENT_LINE 35

#define DECLARE_HEAD() \
  Ui::ui_tip_dialog *ui; \
  ui_tip_dialog *dialog = reinterpret_cast<ui_tip_dialog *>(context); \
  if (NULL == dialog) { \
    return; \
  } \
\
  ui = dialog->get_ui(); \
  if (NULL == ui) { \
    return; \
  }

ui_tip_dialog::func_list_t ui_tip_dialog::func_list_[] = {
  { TIP_WIN_TYPE_FACTORY_DEFAULT, ui_tip_dialog::set_factory_default },
  { TIP_WIN_TYPE_SYS_UPGRADE, ui_tip_dialog::set_sys_upgrade },
  { TIP_WIN_TYPE_FACTORY_TEST, ui_tip_dialog::set_factory_test },
  { TIP_WIN_TYPE_SAVE_SUCCESS, ui_tip_dialog::set_save_success },
  { TIP_WIN_TYPE_BATCH_IMPORT, ui_tip_dialog::set_batch_import },
  { TIP_WIN_TYPE_CAMERA_CALIBRATION, ui_tip_dialog::set_camera_calibration },

  { TIP_WIN_TYPE_SAVE_FAIL, ui_tip_dialog::set_save_fail },
  { TIP_WIN_TYPE_DELETE, ui_tip_dialog::set_delete },
  { TIP_WIN_TYPE_IP_ERROR, ui_tip_dialog::set_ip_error },
  { TIP_WIN_TYPE_CONN_SUCCESS, ui_tip_dialog::set_connect_success },
  { TIP_WIN_TYPE_DISCONNECTED, ui_tip_dialog::set_disconnected },
  { TIP_WIN_TYPE_REBOOTING, ui_tip_dialog::set_rebooting },
  { TIP_WIN_TYPE_ALARM_IN, ui_tip_dialog::set_alarm_in },
  { TIP_WIN_TYPE_INPUT_ERR, ui_tip_dialog::set_input_error },

  { TIP_WIN_TYPE_UPGRADE_FAIL, ui_tip_dialog::set_upgrade_fail },
  { TIP_WIN_TYPE_COPYING, ui_tip_dialog::set_copying },
  { TIP_WIN_TYPE_ID_ERROR, ui_tip_dialog::set_id_error },
  { TIP_WIN_TYPE_NAME_ERROR, ui_tip_dialog::set_name_error },
  { TIP_WIN_TYPE_LOGIN_ERROR, ui_tip_dialog::set_login_error },
  { TIP_WIN_TYPE_OLD_PWD_ERROR, ui_tip_dialog::set_old_passwd_error },
  { TIP_WIN_TYPE_OLD_NEW_SAME_ERROR, ui_tip_dialog::set_old_new_same_error },
  { TIP_WIN_TYPE_INPUT_PWD_ERROR, ui_tip_dialog::set_input_passwd_error },
  { TIP_WIN_TYPE_INPUT_PWD_DISAGREE,
      ui_tip_dialog::set_input_passwd_disagree },

  { TIP_WIN_TYPE_EXPORT_RECORD, ui_tip_dialog::set_export_record },
  { TIP_WIN_TYPE_DELETE_CONFRIM, ui_tip_dialog::set_delete_confirm },
  { TIP_WIN_TYPE_CONN_FAIL, ui_tip_dialog::set_connect_fail },
  { TIP_WIN_TYPE_EXIT_IMPORT_PROCESS, ui_tip_dialog::set_exit_import_process },
  { TIP_WIN_TYPE_EXIT_RECOGNITION_PROCESS,
      ui_tip_dialog::set_exit_recognition_process },
  { TIP_WIN_TYPE_FACE_NO_INPUT, ui_tip_dialog::set_face_no_input },
  { TIP_WIN_TYPE_FACE_SIMILARITY_TOO_HIGH,
      ui_tip_dialog::set_face_smilarity_too_high },
  { TIP_WIN_TYPE_CONNECTING, ui_tip_dialog::set_connecting },
  { TIP_WIN_TYPE_SCANNING, ui_tip_dialog::set_scanning },
  { TIP_WIN_TYPE_FACTORY_AUTO_TEST, ui_tip_dialog::set_factory_auto_test },
  { TIP_WIN_TYPE_TOAST, ui_tip_dialog::set_toast },
  { TIP_WIN_TYPE_NOTE, ui_tip_dialog::set_note },
  { TIP_WIN_TYPE_SNAPSHOT_OK, ui_tip_dialog::set_snapshot_ok },
  { TIP_WIN_TYPE_SNAPSHOT_FAIL, ui_tip_dialog::set_snapshot_fail },
  { TIP_WIN_TYPE_CALIBRATION_OK, ui_tip_dialog::set_calibration_ok },
  { TIP_WIN_TYPE_CALIBRATION_FAIL, ui_tip_dialog::set_calibration_fail },
  { TIP_WIN_TYPE_ACCESS_TIME_FAIL, ui_tip_dialog::set_access_time_fail },
  { TIP_WIN_TYPE_DOOR_SENSOR_DELAY_TIME_FAIL,
      ui_tip_dialog::set_door_sensor_time_fail },
  { TIP_WIN_TYPE_CONFRIM, ui_tip_dialog::set_confirm },
  { TIP_WIN_TYPE_PWD_SAVE_SUCCESS,
      ui_tip_dialog::set_input_passwd_success },
  { TIP_WIN_TYPE_SOURCE,
      ui_tip_dialog::set_source },
};

ui_tip_dialog::ui_tip_dialog(int type, QWidget *parent) :
    QtDialog(parent),
    ui(new Ui::ui_tip_dialog),
    pTimer(NULL),
    user_win_(NULL),
    tip_info(NULL),
    do_end_(0),
    do_result_(0) {
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  ui->setupUi(this);
  this->type = type;

#if 0
  setAutoFillBackground(false);
  setAttribute(Qt::WA_TranslucentBackground, true);
#endif

  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          this, SLOT(deleteLater()));
  connect(ui->tip_cancel_btn, SIGNAL(clicked()),
              this, SLOT(deleteLater()));
  ui->tip_title_label->setAlignment(Qt::AlignCenter);
  ui->tip_content_label->setAlignment(Qt::AlignCenter);
  ui->tip_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");
  for (int i = 0; i < sizeof(func_list_) / sizeof(func_list_[0]); i++) {
    if (func_list_[i].index_ == type) {
      if (func_list_[i].func) {
        func_list_[i].func(this);
        break;
      }
    }
  }

  showMaxLayout();
}

ui_tip_dialog::ui_tip_dialog(tip_args_t *args, QWidget* parent) :
  QtDialog(parent), ui(new Ui::ui_tip_dialog), pTimer(NULL), user_win_(NULL),
  do_end_(0), do_result_(0) {
  tip_info = new tip_args_t;
  tip_info->id = args->id;
  tip_info->title = QString(args->title);
  tip_info->content = QString(args->content);
  tip_info->value = QString(args->value);
  tip_info->cb = args->cb;

  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  ui->setupUi(this);

  ui->tip_title_label->setAlignment(Qt::AlignCenter);
  ui->tip_content_label->setAlignment(Qt::AlignCenter);
  if (!tip_info->title.isEmpty())
    ui->tip_title_label->setText(tip_info->title);
  else
    ui->tip_title_label->hide();
  ui->tip_content_label->setText(tip_info->content);

  ui->tip_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
	this, SLOT(slot_for_args_confirm()));
  connect(ui->tip_cancel_btn, SIGNAL(clicked()),
  	this, SLOT(slot_for_args_cancel()));

  showMaxLayout();
}

void ui_tip_dialog::slot_for_args_confirm() {
  int ret = 0;
  if (tip_info && tip_info->cb) {
    ret = tip_info->cb->tip_callback(tip_info);
  }
  if (!ret)
    deleteLater();
}

void ui_tip_dialog::slot_for_args_cancel() {
  int ret = 0;
  if (tip_info && tip_info->cb) {
    ret = tip_info->cb->tip_callback(tip_info, true);
  }
  if (!ret)
    deleteLater();
}

void ui_tip_dialog::set_content(char *content) {
  if (TIP_WIN_TYPE_TOAST == type || TIP_WIN_TYPE_NOTE == type) {
    int len = strlen(content);
    if (len > CONTENT_LINE) {
      int height = CONTENT_SOURCE + (ceil(len * 1.0f / CONTENT_LINE) -
        1) * 50;
      ui->tip_dialog_frame->setStyleSheet(
        QString("max-height:%1px;").arg(height));
    }
  }
  ui->tip_content_label->setText(QString(content));
}

void ui_tip_dialog::start_count_exit(void) {
  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()),
    this, SLOT(slot_for_time_out_exit()));
  pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_factory_default(void *context) {
  DECLARE_HEAD();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->tip_title_label->setText(tr("恢复出厂设置"));
  ui->tip_content_label->setText(
          tr("恢复出厂设置后所有数据将被清除,\n是否继续进行该操作？"));
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_default_confirm()));
  connect(ui->tip_cancel_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_default_concancel()));
}

void ui_tip_dialog::set_sys_upgrade(void *context) {
  DECLARE_HEAD();

  ui->tip_title_label->setText(tr("系统升级"));
  ui->tip_content_label->setText(
          tr("系统升级时,不能进行任何操作,\n是否继续进行该操作？"));
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          dialog, SLOT(sloft_for_update_firmware_confirm()));
}

void ui_tip_dialog::sloft_for_update_firmware_confirm() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  rotate_signgleton->set_update_flag();
  ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_COPYING);
  if (w)
    w->show();
}

void ui_tip_dialog::set_factory_test(void *context) {
  DECLARE_HEAD();

  /*mode test tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(tr("正在测试中,请稍候..."));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_factory_auto_test(void *context) {
  DECLARE_HEAD();

  ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();

  /*mode test tip dialog*/
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(tr("正在自动化测试中,是否退出？"));
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_auto_exit()));
  connect(ui->tip_cancel_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(slot_for_factory_auto_resume()));
}

void ui_tip_dialog::set_save_success(void *context) {
  DECLARE_HEAD();

  /*mode save tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("保存成功"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_batch_import(void *context) {
  DECLARE_HEAD();

  /*mode batch import tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("正在导入中,请稍候..."));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_camera_calibration(void *context) {
  DECLARE_HEAD();

  /*mode calc camera calibration parameter tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("正在处理中,请稍候..."));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_save_fail(void *context) {
  DECLARE_HEAD();

  /*mode save tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("保存失败"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_delete(void *context) {
  DECLARE_HEAD();

  /*delete all user from employee table tip dialog*/
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("将从用户表中删除所有用户,\n是否继续进行该操作？"));
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          dialog, SLOT(slot_for_clean_all_users()));
}
void ui_tip_dialog::set_ip_error(void *context) {
  DECLARE_HEAD();

  /*ip error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("您没有输入地址信息,或地址输入有误,\n请输入正确格式的地址信息!"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_connect_success(void *context) {
  DECLARE_HEAD();

  /*mode connect success tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("\n连接成功"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_disconnected(void *context) {
  DECLARE_HEAD();

  /*disconnect tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("连接断开"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_rebooting(void *context) {
  DECLARE_HEAD();

  /*mode reboot tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("设备重启中,请稍候..."));
}

void ui_tip_dialog::set_alarm_in(void *context) {
  DECLARE_HEAD();

  /*mode disconnect tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setStyleSheet("color:red;");
  ui->tip_content_label->setStyleSheet("color:red;");
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("!!!请注意,设备报警中!!!"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_input_error(void *context) {
  DECLARE_HEAD();

  /*mode input error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(
      tr("输入无效,请输入0-100以内的整数"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_upgrade_fail(void *context) {
  DECLARE_HEAD();

  /*mode input error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("复制升级文件出错,升级失败！！！"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_copying(void *context) {
  DECLARE_HEAD();

  /*mode copying tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(
      tr("升级文件正在复制中,请稍候..."));
  ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();
  rotate_signgleton->clean_upgrade_flag();
  rotate_signgleton->set_setting_count_state(TIMER_COUNTS_STOP);
  dialog->thread.set_test_ui(dialog);
  dialog->thread.start();
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_check_copy_result()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_id_error(void *context) {
  DECLARE_HEAD();

  /*id error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("您输入的工号错误(只能是0~5000),\n请重新输入工号!"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_toast(void *context) {
  DECLARE_HEAD();

  ui->widget->hide();
  ui->tip_title_label->hide();
  ui->tip_frame->hide();
  ui->tip_dialog_frame->setStyleSheet(
    QString("max-height:%1px;").arg(CONTENT_SOURCE));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_note(void *context) {
  DECLARE_HEAD();

  ui->widget->hide();
  ui->tip_title_label->hide();
  ui->tip_frame->hide();
  ui->tip_dialog_frame->setStyleSheet(
    QString("max-height:%1px;").arg(CONTENT_SOURCE));
}

void ui_tip_dialog::set_name_error(void *context) {
  DECLARE_HEAD();

  /*name error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("您输入的姓名长度超过限制(中文不能超过10个,\n"
      "字母不超过30个),请重新输入姓名!"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_login_error(void *context) {
  DECLARE_HEAD();

  /*login error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("您输入的密码错误"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_old_passwd_error(void *context) {
  DECLARE_HEAD();

  /*input old password error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("原始密码错误"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}
void ui_tip_dialog::set_old_new_same_error(void *context) {
  DECLARE_HEAD();

  /*input old password error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("新旧密码不能相同"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}
void ui_tip_dialog::set_input_passwd_error(void *context) {
  DECLARE_HEAD();

  /*input password error tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("请输入6-16位数字"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_input_passwd_disagree(void *context) {
  DECLARE_HEAD();

  /*input pwd disagree tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("新密码输入不一致"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_input_passwd_success(void *context) {
  DECLARE_HEAD();

  /*input pwd disagree tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("密码修改成功"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_source (void *context) {
  return;
}

void ui_tip_dialog::set_export_record(void *context) {
  DECLARE_HEAD();

  /*export record tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("记录导出"));
  ui->tip_content_label->setText(tr("正在导出0条/共0条"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_check_export_result()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_delete_confirm(void *context) {
  DECLARE_HEAD();

  /*confirm delete record tip dialog*/
  ui->tip_title_label->hide();
  ui->tip_frame->hide();
  ui->tip_content_label->setText(tr("是否删除用户？"));
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          dialog, SLOT(slot_for_confirm_delete()));
}

void ui_tip_dialog::set_confirm(void *context) {
  DECLARE_HEAD();

  /*confirm delete record tip dialog*/
  ui->tip_title_label->hide();
  ui->tip_frame->hide();
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          dialog, SLOT(slot_for_confirm_delete()));
}

void ui_tip_dialog::set_connect_fail(void *context) {
  DECLARE_HEAD();

  /*connect failure tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("连接失败"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_exit_import_process(void *context) {
  DECLARE_HEAD();

  /*confirm exit import tip dialog*/
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("将终止当前处理,\n是否继续进行该操作？"));
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          dialog, SLOT(slot_for_exit_import_process()));
}

void ui_tip_dialog::set_exit_recognition_process(void *context) {
  DECLARE_HEAD();

  /*confirm exit recognition tip dialog*/
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(
      tr("将终止当前处理,\n是否继续进行该操作？"));
  connect(ui->tip_confirm_btn, SIGNAL(clicked()),
          dialog, SLOT(slot_for_exit_recognition_process()));
}

void ui_tip_dialog::set_face_no_input(void *context) {
  DECLARE_HEAD();

  /*face no input tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(tr("人脸未录入"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_face_smilarity_too_high(void *context) {
  DECLARE_HEAD();

  /*face similarity too high tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(tr("人脸相似度过高,保存失败"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_connecting(void *context) {
  DECLARE_HEAD();

  /*connecting tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(tr("正在连接中..."));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_scanning(void *context) {
  DECLARE_HEAD();

  /*connecting tip dialog*/
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(tr("Wi-Fi正在扫描中..."));
}

void ui_tip_dialog::set_snapshot_ok(void *context) {
  DECLARE_HEAD();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(
          tr("抓拍成功,\n点确认继续下一步操作!"));

  for (int i = 0; i < 2; i++) {
    QLayoutItem *child = ui->widget->layout()->takeAt(1);
    if (child) {
      delete child;
    }
  }
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->setText(tr("确认"));
  connect(ui->tip_cancel_btn, SIGNAL(clicked()),
          dialog, SLOT(slot_for_press_ok()));
}

void ui_tip_dialog::set_snapshot_fail(void *context) {
  DECLARE_HEAD();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(
          tr("抓拍失败,\n点确认继续当前操作!"));

  for (int i = 0; i < 2; i++) {
    QLayoutItem *child = ui->widget->layout()->takeAt(1);
    if (child) {
      delete child;
    }
  }
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->setText(tr("确认"));
}

void ui_tip_dialog::set_calibration_ok(void *context) {
  DECLARE_HEAD();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(
          tr("保存成功!"));

  for (int i = 0; i < 2; i++) {
    QLayoutItem *child = ui->widget->layout()->takeAt(1);
    if (child) {
      delete child;
    }
  }
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->setText(tr("确认"));
}

void ui_tip_dialog::set_calibration_fail(void *context) {
  DECLARE_HEAD();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->tip_title_label->setText(tr("提示"));
  ui->tip_content_label->setText(
          tr("保存失败!"));

  for (int i = 0; i < 2; i++) {
    QLayoutItem *child = ui->widget->layout()->takeAt(1);
    if (child) {
      delete child;
    }
  }
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->setText(tr("确认"));
}

void ui_tip_dialog::set_access_time_fail(void *context) {
  DECLARE_HEAD();

  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(tr("请输入正确结束时间"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

void ui_tip_dialog::set_door_sensor_time_fail(void *context) {
  DECLARE_HEAD();

  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  ui->tip_title_label->setText(tr("警告"));
  ui->tip_content_label->setText(tr("请输入1-255范围内的数字"));
  dialog->pTimer = new QTimer;
  connect(dialog->pTimer, SIGNAL(timeout()),
          dialog, SLOT(slot_for_time_out_exit()));
  dialog->pTimer->start(TIMER_INTERVAL_COUNT);
}

ui_tip_dialog::~ui_tip_dialog() {
  if (pTimer) {
    delete pTimer;
  }
  delete ui;
}

void ui_tip_dialog::set_user_win(void *user_win) {
  user_win_ = user_win;
}

void ui_tip_dialog::slot_for_time_out_exit() {
  pTimer->stop();
  deleteLater();
}

void ui_tip_dialog::slot_for_press_ok() {
  ui_camera_calibration * calibration =
    reinterpret_cast<ui_camera_calibration *>(user_win_);
  if (calibration) {
    calibration->press_ok();
  }
}

void ui_tip_dialog::proc_func() {
  ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();
  do_result_ = ui_update_device_start();
  do_end_ = 1;
}

void ui_tip_dialog::slot_for_check_copy_result() {
  if (do_end_) {
    pTimer->stop();
    if (do_result_) {
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_UPGRADE_FAIL);
      if (w) {
        w->show();
      }
      ui_rotate_management *rotate_signgleton =
          ui_rotate_management::getInstance();
      rotate_signgleton->clean_upgrade_flag();
      rotate_signgleton->set_setting_count_state(TIMER_COUNTS_START);
      deleteLater();
    }
  }
}

void ui_tip_dialog::slot_for_check_export_result() {
  char result[256];
  record_export_ctx_t ctx;
  ui_control_get_export_result(&ctx);

  snprintf(result, sizeof(result),
      tr("正在导出%d条/共%d条").toUtf8().data(),
      ctx.status.current_count, ctx.status.sum_count);
  ui->tip_content_label->setText(result);

  ALOGD("%s[%d] result %s", __func__, __LINE__, result);

  if (ctx.done == EE_THREAD_DONE) {
    if (0 == do_end_) {
      do_end_ = 1;
      return;
    }

    if (1 == do_end_) {
      if (RECORD_EXPORT_OK == ctx.result_code &&
        ctx.status.current_count == ctx.status.sum_count) {
        snprintf(result, sizeof(result), "%s",
          tr("导出成功").toUtf8().data());
      } else {
        if (RECORD_EXPORT_NO_UDISK_ERR == ctx.result_code) {
          snprintf(result, sizeof(result), "%s",
            tr("未检测到U盘，导出失败").toUtf8().data());
        } else if (RECORD_EXPORT_UDISK_FULL_ERR == ctx.result_code) {
          snprintf(result, sizeof(result), "%s",
            tr("U盘空间不足，导出失败").toUtf8().data());
        } else {
          snprintf(result, sizeof(result), "%s",
            tr("导出失败").toUtf8().data());
        }
      }
      ui->tip_content_label->setText(result);
      do_end_ = 2;
      return;
    }

    pTimer->stop();
    deleteLater();
  }
}

void ui_tip_dialog::slot_for_clean_all_users() {
  ui_control_person_info_del(TYPE_PERSON);
}

void ui_tip_dialog::slot_for_confirm_delete() {
  ui_user_view_list *user_view =
    reinterpret_cast<ui_user_view_list *>(user_win_);
  if (user_view) {
    user_view->delete_selected_user();
  }
}

void ui_tip_dialog::slot_for_exit_import_process() {
  ui_control_exit_person_import();
}

void ui_tip_dialog::slot_for_exit_recognition_process() {
  ui_control_exit_person_recg();
}

