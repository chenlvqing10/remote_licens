/*
* ui_rotate_management.cpp - define interface function of UI rotate
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
#define LOG_TAG     "rotate"
#include <log/log.h>
#include "checkin_export_svc.h"
#include "hotplug.h"
#include "system_info.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QApplication>
#include <QModelIndex>
#include <QMessageBox>
#include <QTimer>
#include <QtCore/qnamespace.h>
#include <QTextEdit>
#include <QFile>

#include "ui_rotate_management.hpp"
#include "ui_bind_qrcode.hpp"
#include "ui_ble_qrcode.hpp"

#define TIMER_INTERVAL_COUNT 1000
#define SETTING_TIMER_COUNTS 60000

static const int enter_face_input_window = 1;
static const int exit_face_input_window = 2;
static const int enter_menu_window = 3;
static const int generate_qr_code = 4;

static ui_rotate_management *instance = nullptr;

static QPixmap PixmapToRound(QPixmap &src, int radius) {
  if (src.isNull()) {
    return QPixmap();
  }

  QSize size(2*radius, 2*radius);
  QBitmap mask(size);
  QPainter painter(&mask);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
  painter.setBrush(QColor(0, 0, 0));
  painter.drawRoundedRect(0, 0, size.width(),
    size.height(), size.width(), size.height());

  QPixmap image = src.scaled(size);
  image.setMask(mask);
  return image;
}

void ui_rotate_management::notes_init() {
  wifi_mgr->notes_init();
  recg_notes[RECG_STRANGER] = tr("陌生人");
  recg_notes[RECG_PROSTHESIS] = tr("假体");
  recg_notes[RECG_TEMP_NORMAL] = tr("%1℃ 体温正常");
  recg_notes[RECG_TEMP_ABNORMAL] = tr("%1℃ 体温异常");

  recg_notes[RECG_PASS] = tr("请通行");
  recg_notes[RECG_REGISTER] = tr("请注册通行");
  recg_notes[RECG_CONTACT_MANAGER] = tr("请联系管理员");
  recg_notes[RECG_WEAR_MASK] = tr("请佩戴口罩");

  recg_notes[RECG_WELCOME] = tr("欢迎光临");
  recg_notes[RECG_WELCOME_AGAIN] = tr("欢迎再次光临");
  recg_notes[RECG_MORNING] = tr("早上好");
  recg_notes[RECG_BEFORE_NOON] = tr("上午好");
  recg_notes[RECG_NOON] = tr("中午好");
  recg_notes[RECG_AFTER_NOON] = tr("下午好");
  recg_notes[RECG_EVENING] = tr("晚上好");

  recg_notes[RECG_TEMP_PREPARING] = tr("请对准测温框");
  recg_notes[RECG_TEMP_MEASURING] = tr("测温中...");
  recg_notes[RECG_POSE_ERROR] = tr("请正面对准摄像头");
}

ui_rotate_management::ui_rotate_management(QObject *parent) :
    QObject(parent), currnet_win(NULL), save_win(NULL),
    ota(new ui_ota), isNetConnected(false) {
  memset(&user_info, 0, sizeof(user_information_param_t));
  memset(&test_status, 0, sizeof(factory_test_result_t));
  mode_ui_param_t param;
  ui_control_get_current_mode(&param);
  usb_status = udisk_is_insert();
  ws_status = 0;
  is_upgrading = 0;

  is_recv_unbind_ = 0;
  attendance_num = QString("");
  attendance_start = -1;
  attendance_end = 0;

  is_bind = ui_face_webchat_is_active();
  eth_mgr = eth_management::getInstance();
  wifi_mgr = wifi_management::getInstance();
  notes_init();
  connect(eth_mgr, SIGNAL(signal_eth_state_change(int)),
          this, SLOT(slot_for_eth_state(int)));
  connect(wifi_mgr, SIGNAL(signal_wifi_state_change(int)),
          this, SLOT(slot_for_wifi_state(int)));

#if ATTENDANCE_MODE == 2
  /* do nothings */
#endif
  SignalTable *signal_table =
    reinterpret_cast<SignalTable *>(signal_get_handle());
  connect(signal_table, SIGNAL(signalForWebSocket(int)),
          this, SLOT(slot_for_show_webchat(int)));
  connect(signal_table, SIGNAL(signalForScreenSaver(int)),
          this, SLOT(slot_show_wait_clock(int)));
#if 0
  connect(signal_table, SIGNAL(signalForFactoryDefault()),
          this, SLOT(slot_for_factory_default()));
#else
  connect(signal_table, SIGNAL(signalForFactoryDefault()),
          this, SLOT(slot_for_factory_default_confirm()));
#endif
  connect(signal_table, SIGNAL(signalForUpdateFirmware()),
          this, SLOT(slot_for_update_firmware()));
  connect(signal_table, SIGNAL(signalForStorage(int, int)),
          this, SLOT(slot_for_storage(int, int)));

  file_watcher = new QFileSystemWatcher(this);
  file_watcher->addPath("/dev/input/");
  connect(file_watcher, SIGNAL(directoryChanged(QString)),
          this, SLOT(slot_for_dir_changed(QString)));

  // curretWin = NULL;
}

void ui_rotate_management::slot_for_eth_state(int state) {
  ALOGD("%s:%d >> isNetConnected=%d, state=%d", __func__, __LINE__,
    isNetConnected, state);
  if (ETH_DHCPC_SUCCESS == state || ETH_STATIC_SUCCESS == state) {
    if (!isNetConnected) {
      isNetConnected = true;
#if ATTENDANCE_MODE == 2
      ws_svc_app_net_reconnect();
#else
      start_ota_svc(false);
#endif
    }
  } else if (isNetConnected) {
    if (WIFI_STATION_CONNECTED != wifi_mgr->get_wifi_state()) {
      isNetConnected = false;
#if ATTENDANCE_MODE == 2
      ws_svc_app_set_netlink(0);
#endif
    }
  }
}

void ui_rotate_management::slot_for_wifi_state(int state) {
  static bool isConnected = false;
  ALOGD("%s:%d >> isConnected=%d, state=%d", __func__, __LINE__,
    isConnected, state);
  if (WIFI_STATION_CONNECTED == state) {
    if (!isNetConnected) {
      isNetConnected = true;
#if ATTENDANCE_MODE == 2
      ws_svc_app_net_reconnect();
#else
      start_ota_svc(false);
#endif
    }
  } else if (isNetConnected) {
    int eth_state = eth_mgr->get_eth_state();
    if (ETH_DHCPC_SUCCESS != eth_state &&
      ETH_STATIC_SUCCESS != eth_state) {
      isNetConnected = false;
#if ATTENDANCE_MODE == 2
      ws_svc_app_set_netlink(0);
#endif
    }
  }
}

void ui_rotate_management::get_factory_test_result(
  factory_test_result_t *result) {
  if (result)
    memcpy(result, &test_status, sizeof(factory_test_result_t));
}

void ui_rotate_management::set_factory_test_result(
  factory_test_result_t *result) {
  if (result)
    memcpy(&test_status, result, sizeof(factory_test_result_t));
}

int ui_rotate_management::get_usb_status() {
  return usb_status;
}

void ui_rotate_management::set_usb_status(int status) {
  usb_status = status;
}

int ui_rotate_management::get_ws_status() {
  return ws_status;
}

int ui_rotate_management::set_ws_status(int status) {
  ws_status = status;
}

ui_rotate_management::ui_rotate_management(const ui_rotate_management&) {
}

ui_rotate_management& ui_rotate_management::operator =
(const ui_rotate_management&) {
}

ui_rotate_management* ui_rotate_management::getInstance() {
  if (instance == nullptr) {
    instance = new ui_rotate_management();
    device_lang_param_t info;
    db_device_lang_param_query(&info);
    if (info.lang == 1) {
      qDebug() << __func__ <<__LINE__<< "english";
      instance->translator.load(":icon/setting/qt_gui_en_US.qm");
      qApp->installTranslator(&instance->translator);
    }
    instance->notes_init();
    if(udisk_is_ok() == 0)
      instance->set_usb_status(1);
    else
      instance->set_usb_status(0);
  }
    return instance;
}

void ui_rotate_management::update_management() {
#if ATTENDANCE_MODE != 2
  set_logo_pixmap();
  set_company_name();
  set_main_control();
  set_recg_cnt();
  set_border_color();
  update_net_state();
  set_debug_switch();
  set_algorithm_params();
#endif
}

void ui_rotate_management::slot_bind_choose() {
  mode_ui_param_t param;
  memset(&param, 0, sizeof(mode_ui_param_t));
  // db_mode_ui_param_query(&param);
  ui_control_get_current_mode(&param);
  wins_list.clear();
#if ATTENDANCE_MODE == 2
  // WECHAT MODE
  is_bind = ui_face_webchat_is_active();
  ALOGD("slot_bind_choose() is_bind %d", is_bind);
  if (is_bind == 0) {
    // unbind
    int status = ui_control_websocket_is_link();
    if (status == 0) {
      status = get_ws_status();
    }
    ALOGD("ui_control_websocket_is_link() status %d", status);
    if (status == 1) {
      // network is normal, display wechat bind ui
      start_activity_win(&ui_bind_qrcode::staticMetaObject);
    } else {
      // network is abnormal, display ble config wifi ui
      start_activity_win(&ui_ble_qrcode::staticMetaObject);
    }
  } else {
    // bind, display face_input ui
    start_activity_win(&ui_face_scan::staticMetaObject);
  }
#else
  // LOCAL MODE
  start_activity_win(&ui_face_scan::staticMetaObject);
#endif
}

void ui_rotate_management::start_ota_svc(char isShow) {
  ALOGD("%s try to visit ote sever", __func__);
#if 1
  ota->try_ota_visit(isShow);
#else
  static bool running = false;
  if (!running) {
    running = true;
    ota->debug_test_ota();
  }
#endif
}

int ui_rotate_management::start_activity_win(const QMetaObject *aim_win) {
  const char *className;

  ALOGD("%s >> currnet_win:%s --> aim_win:%s ", __func__,
    currnet_win?currnet_win->metaObject()->className():"null",
    aim_win->className());
  if (currnet_win && strcmp(currnet_win->metaObject()->className(),
    aim_win->className()) == 0)
    return 1;

  if (currnet_win) {
    currnet_win->deinit();
    wins_list.append(currnet_win->metaObject());
    className = currnet_win->metaObject()->className();
    if(strcmp(className, "ui_face_scan") == 0)
      set_setting_count_state(TIMER_COUNTS_START);
  }
  className = aim_win->className();
  if (strcmp(className, "ui_face_scan") == 0)
    set_setting_count_state(TIMER_COUNTS_STOP);

  currnet_win = static_cast<QtMainWindow *>(aim_win->newInstance());
  currnet_win->setClassName();
  if (currnet_win) {
    currnet_win->showMaximized();
    currnet_win->show();
  }
  return 0;
}

void ui_rotate_management::back_activity_win() {
  const QMetaObject *last_win;

  if (!wins_list.isEmpty()) {
    last_win = wins_list.last();
    wins_list.pop_back();
  }

  if (last_win) {
    ALOGD("%s >> currnet_win:%s --> last_win:%s ", __func__,
      currnet_win?currnet_win->metaObject()->className():"null",
      last_win->className());
    if (currnet_win)
      currnet_win->deinit();
    currnet_win = static_cast<QtMainWindow *>(last_win->newInstance());
    currnet_win->setClassName();
    if (currnet_win) {
      currnet_win->showMaximized();
      currnet_win->show();
    }
  } else {
    ALOGW("%s >> currnet_win:%s --> last_win:null ", __func__,
      currnet_win?currnet_win->metaObject()->className():"null");
  }
}

void ui_rotate_management::slot_show_wait_clock(int event) {
  ALOGD("slot_show_wait_clock() event %d", event);
  if (event == 1) {
    if (save_win) return;
    save_win = new ui_wait_clock();
#if 0
    if (save_win) {
      save_win->showMaximized();
      save_win->show();
    }
#endif
  } else {
    if (!save_win) return;
    save_win->deinit();
    save_win = NULL;
  }
}

void ui_rotate_management::slot_for_factory_default_confirm() {
  ui_factory_default();
}

void ui_rotate_management::slot_for_factory_default_concancel() {
  ui_factory_set_flag();
}

int ui_rotate_management::is_update_firmware() {
  return is_upgrading;
}

int ui_rotate_management::clean_upgrade_flag() {
  is_upgrading = 0;
}

void ui_rotate_management::set_update_flag() {
  is_upgrading = 1;
}

void ui_rotate_management::set_logo_pixmap() {
  QPixmap logo_sr(LOGO_PATH);
  main_param.logo = logo_sr.scaled(146, 146,
    Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  main_param.logo = PixmapToRound(main_param.logo, 73);
}

void ui_rotate_management::set_company_name() {
  char name[128] = {0};
  db_ui_get_value("company_name", name, sizeof(name));
  main_param.company = QString(name);
}

void ui_rotate_management::set_main_control() {
  main_setting_param_t main_setting_param;
  memset(&main_setting_param, 0, sizeof(main_setting_param_t));
  ui_control_get_main_setting_param(&main_setting_param);
  main_param.company = QString(main_setting_param.company_name);
  main_param.name = QString(main_setting_param.device_name);
  if (main_param.name.isEmpty()) {
    build_info_t build_info;
    memset(&build_info, 0, sizeof(build_info_t));
    system_info_get_build_info(&build_info);
    main_param.name = QString(build_info.device_name);
  }
  main_param.color = QString(main_setting_param.face_border_color);
  main_param.isName = main_setting_param.name_display;
  main_param.isCnt = main_setting_param.register_count_display;
  main_param.isMac = main_setting_param.mac_display;
  main_param.isIp = main_setting_param.ip_display;
  main_param.name_format = main_setting_param.name_format;
}

void ui_rotate_management::set_recg_cnt() {
  int count = ui_control_person_info_get_count();
  main_param.cnt = QString::number(count, 10);
}

void ui_rotate_management::set_border_color() {
  char color[32] = {0};
  db_ui_get_value("border_color", color, sizeof(color));
  main_param.color = QString(color);
}

void ui_rotate_management::set_debug_switch() {
  memset(&(main_param.debug_switch), 0, sizeof(un_debug_switch_t));
  if (ui_control_get_debug_switch(&(main_param.debug_switch)) < 0) {
    ALOGE("%s failed\n", __func__);
  }
}

void ui_rotate_management::set_algorithm_params() {
  memset(&(main_param.recg_params), 0, sizeof(algorithm_system_info_t));
  if (ui_get_algorithm_system_info(&(main_param.recg_params)) < 0) {
    ALOGE("%s failed\n", __func__);
  }
}

void ui_rotate_management::update_net_state() {
  char ip[32] = {0};
  char mac[32] = {0};
  if(net_get_info(ip, sizeof(ip), mac, sizeof(mac)) == 0) {
    ALOGD("%s >> mac:%s, ip:%s", __func__, mac, ip);
    main_param.mac = QString(mac);
    main_param.ip = QString(ip);
  }
}

/* get checkin count */
int get_checkin_count_cb() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  return ui_control_export_by_time_get_count(
    rotate_signgleton->attendance_num.toUtf8().data(),
    rotate_signgleton->attendance_start,
    rotate_signgleton->attendance_end);
}

/* get checkin record information */
int get_checkin_rec_info_cb(int index, int count, ee_queue_t *head) {
  int ret;
  checkin_rec_info_t *u;
  web_socket_export_checkin_info_t *q = NULL;
  ee_queue_t checkin_head;
  ee_queue_init(&checkin_head);

  ee_queue_t* h = head;
  ee_queue_init(h);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ret = ui_control_export_rec_query_begin_by_id_time(
    index, count, &checkin_head,
    rotate_signgleton->attendance_num.toUtf8().data(),
    rotate_signgleton->attendance_start,
    rotate_signgleton->attendance_end);
  if (ret > 0) {
    ee_queue_foreach(q, &checkin_head,
          web_socket_export_checkin_info_t, queue) {
      u = new checkin_rec_info_t();
      snprintf(u->user_id, sizeof(u->user_id), "%s", q->userid);
      snprintf(u->name, sizeof(u->name), "%s", q->name);
      u->checkin_time = q->check_in_time;
      ee_queue_insert_tail(h, &(u->queue));
    }
  }

  ui_control_export_rec_query_end(&checkin_head);
  return ret;
}

void ui_rotate_management::slot_for_show_webchat(int event) {
  ui_face_scan *t = NULL;
  ui_bind_qrcode *wt = NULL;
  if (currnet_win)
    t = qobject_cast<ui_face_scan *>(currnet_win);
    if (!t)
      wt = qobject_cast<ui_bind_qrcode *>(currnet_win);

  switch (event) {
    case WEB_SOCKET_EVENT_ENTER_PAGE:
      ALOGD("slot_for_show_webchat() face_authentication_win_opened_");
      start_activity_win(&ui_face_authentication::staticMetaObject);
      ((ui_face_authentication *)currnet_win)->set_type(1);
      break;

    case WEB_SOCKET_EVENT_EXIT_PAGE:
      if (websocket_input) {
        delete websocket_input;
        websocket_input = NULL;
      }
      break;

    case WEB_SOCKET_EVENT_UNBIND:
      is_recv_unbind_ = 1;
      ALOGD("%s WEB_SOCKET_EVENT_UNBIND", __func__);
      // unbind success

      // check if current ui is settings ui
      if (is_menu_win_opened()) {
          break;
      }
      slot_bind_choose();
      break;

    case WEB_SOCKET_EVENT_LINKED:
      ALOGD("%s WEB_SOCKET_EVENT_LINKED", __func__);
      // network is connected
      set_ws_status(1);
      if (t)
        t->update_icon_status();

      // check if current ui is settings ui
      if (is_menu_win_opened()) {
          break;
      }

      slot_bind_choose();
      break;

    case WEB_SOCKET_EVENT_DIS_LINKED:
      ALOGD("%s WEB_SOCKET_EVENT_DIS_LINKED", __func__);
      set_ws_status(0);
      if (t)
        t->update_icon_status();

      if (wt) {
        // network is disconnected
        is_bind = ui_face_webchat_is_active();
        if (is_bind == 0) {
          start_activity_win(&ui_ble_qrcode::staticMetaObject);
        }
      }
      break;

    case WEB_SOCKET_EVENT_BIND:
      is_recv_unbind_ = 0;
      ALOGD("WEB_SOCKET_EVENT_BIND");
      // bind success

      // check if current ui is settings ui
      if (is_menu_win_opened()) {
          break;
      }

      slot_bind_choose();
      break;
#if 0
    case WEB_SOCKET_EVENT_ALARMIN:
      w = new ui_tip_dialog(TIP_WIN_TYPE_ALARM_IN, this);
      w->show();
      break;
#endif
    case WEB_SOCKET_EVENT_FACE_INIT_OK:
      if (NULL == t &&
          (NULL == save_win)) {
        ui_control_face_pause();
      }
      break;

    case WEB_SOCKET_EVENT_RECORD_EXPORT: {
#if ATTENDANCE_MODE != 2
      record_export_start_service(get_checkin_count_cb,
            get_checkin_rec_info_cb);
#endif
      }
      break;

    default:
      break;
  }
}

user_information_param_t * ui_rotate_management::get_user_info() {
  return &user_info;
}

void ui_rotate_management::set_user_info(user_information_param_t *usr_info) {
  memset(&user_info, 0, sizeof(user_information_param_t));
  if (usr_info) {
    memcpy(&user_info, usr_info, sizeof(user_information_param_t));
  }
}

void ui_rotate_management::set_feat_info(ui_feat_local_t *feat) {
  if (feat) {
    memcpy(user_info.face_feat, feat->face_feature, FACE_FEAT_SIZE);
    memcpy(user_info.key_points, feat->key_points,
            sizeof(user_info.key_points));
    memcpy(user_info.face_norm, feat->face_norm,
            sizeof(user_info.face_norm));
    memcpy(user_info.save_frame_path, feat->save_frame_path,
            sizeof(user_info.save_frame_path));
    user_info.score = feat->score;
  }
}

void ui_rotate_management::slot_for_factory_auto_exit() {
  ALOGD("%s", __func__);
  this->auto_test.stopped = true;
  this->auto_test.auto_test_resume();
  this->auto_test.auto_test_run = 0;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_auto_resume() {
  ALOGD("%s", __func__);
  this->auto_test.auto_test_resume();
}

void ui_rotate_management::slot_for_factory_lcd() {
  ALOGD("%s", __func__);
  QPushButton *chk_btn = reinterpret_cast <QPushButton *>(sender());
  test_status.lcd_status = chk_btn->property("id").toString().toInt();
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_touch(int state) {
  ALOGD("%s", __func__);
  test_status.touch_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_camera() {
  ALOGD("%s", __func__);
  QPushButton *chk_btn = reinterpret_cast <QPushButton *>(sender());
  test_status.camera_status = chk_btn->property("id").toString().toInt();
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_wifi(int state) {
  ALOGD("%s", __func__);
  test_status.wifi_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_ble(int state) {
  ALOGD("%s", __func__);
  test_status.ble_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_rtc(int state) {
  ALOGD("%s", __func__);
  test_status.rtc_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_net(int state) {
  ALOGD("%s", __func__);
  test_status.eth_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_range_sensor() {
  ALOGD("%s", __func__);
  QPushButton *chk_btn = reinterpret_cast <QPushButton *>(sender());
  test_status.range_sensor_status = chk_btn->property("id").toString().toInt();
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_light_sensor(int state) {
  ALOGD("%s", __func__);
  test_status.light_sensor_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_light(int state) {
  ALOGD("%s", __func__);
  test_status.light_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_ir(int state) {
  ALOGD("%s", __func__);
  test_status.ir_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_speaker(int state) {
  ALOGD("%s", __func__);
  test_status.speaker_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_relay() {
  ALOGD("%s", __func__);
  QPushButton *chk_btn = reinterpret_cast <QPushButton *>(sender());
  test_status.relay_status = chk_btn->property("id").toString().toInt();
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_rs485(int state) {
  ALOGD("%s", __func__);
  test_status.rs485_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_wiegand(int state) {
  ALOGD("%s", __func__);
  test_status.wiegand_status = state?TEST_STATUS_FAIL:TEST_STATUS_OK;
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_alarm() {
  ALOGD("%s", __func__);
  QPushButton *chk_btn = reinterpret_cast <QPushButton *>(sender());
  test_status.alarm_status = chk_btn->property("id").toString().toInt();
  back_activity_win();
}

void ui_rotate_management::slot_for_factory_usb(int state) {
  ALOGD("%s", __func__);
  test_status.usb_status = state;
  back_activity_win();
}

void ui_rotate_management::slot_for_storage(int event, int type) {
  ui_face_scan *t = NULL;
  if (currnet_win)
    t = qobject_cast<ui_face_scan *>(currnet_win);

  if (event == STORAGE_EVENT_UDISK) {
    if (type == STORAGE_TYPE_CONNECT) {
      set_usb_status(1);
#if 0
      if (w_face_scan) {
        record_export_start_service(get_checkin_count_cb,
              get_checkin_rec_info_cb);
        ui_tip_dialog *tip = new ui_tip_dialog(TIP_WIN_TYPE_EXPORT_RECORD);
        tip->show();
      }
#endif
    }

    if (type == STORAGE_TYPE_DISCONN) {
      set_usb_status(0);
    }
  }

  if (t) {
    t->update_icon_status();
  }
}

void ui_rotate_management::slot_for_dir_changed(QString path) {
  QFile *tp_file = new QFile("/proc/bus/input/devices");

  tp_file->open(QIODevice::ReadOnly);
  QString data = QString(tp_file->readAll());
  if (data.contains("mouse") && this->cursor_status == 0) {
    this->cursor_status = 1;
    QApplication::restoreOverrideCursor();
  }
  if (!data.contains("mouse") && this->cursor_status != 0) {
    this->cursor_status = 0;
    QApplication::setOverrideCursor(Qt::BlankCursor);
  }
  tp_file->close();
}

int ui_rotate_management::is_wait_clock_win_opened() {
  return save_win?1:0;
}

int ui_rotate_management::is_menu_win_opened() {
  int ret = 1;
  const char *className = currnet_win?currnet_win->metaObject()->className():"";

  if (strcmp(className, "ui_face_scan") == 0 ||
    strcmp(className, "ui_bind_qrcode") == 0 ||
    strcmp(className, "ui_ble_qrcode") == 0)
    ret = 0;
  return ret;
}

void ui_rotate_management::slot_for_turn_main() {
  set_setting_count_state(TIMER_COUNTS_STOP);
  ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST);
  w->set_content(tr("未检测到用户输入！").toUtf8().data());
  w->show();
  QTimer::singleShot(1000, this, SLOT(slot_bind_choose()));
}

void ui_rotate_management::set_setting_count_state(int state) {
  static QTimer *time;

  if (TIMER_COUNTS_RESTART == state) {
    if (time) {
      time->stop();
      time->start(SETTING_TIMER_COUNTS);
    }
  } else if (TIMER_COUNTS_STOP == state) {
    if (time)
      time->stop();
    time = NULL;
  } else {
    if (!time) {
      time = new QTimer();
      connect(time, SIGNAL(timeout()),
        ui_rotate_management::getInstance(), SLOT(slot_for_turn_main()));
      time->start(SETTING_TIMER_COUNTS);
    } else {
      time->stop();
      time->start(SETTING_TIMER_COUNTS);
    }
  }
}

void ui_rotate_management::slot_for_update_firmware() {
  if (is_update_firmware()) {
    return;
  }
  ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_SYS_UPGRADE);
  if (w) {
    w->show();
  }
}

