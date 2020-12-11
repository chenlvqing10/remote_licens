/*
* main.cpp - define interface function of main
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
#include <stddef.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>

#define LOG_TAG "main"
#include <log/log.h>
#include "time.h"
#include "face.h"
#include "led.h"
#include "time_conv.h"
#include "ws_svc_main.h"
#include "system_volume.h"
#include "system_chip_id.h"
#include "lombo_system_sound.h"
#include "file_ops.h"
#include "reset_svc.h"
#include "update_svc.h"
#include "screen_svc.h"
#include "light_svc.h"
#include "alarm_svc.h"
#include "open_door_svc.h"
#include "wiegand_svc.h"
#include "hotplug.h"
#include "gpio.h"
#include "person_import.h"
#include "clang.h"
#include "ws_bt_wifi.h"
#include "http_main.h"
#include "system_update.h"
#include "live_video.h"
#include "time_ntp.h"
#include "screen_size.h"
#include "watchdog.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QApplication>
#include <QTextCodec>
#include <QTime>
#include <QLocale>
#include <QTranslator>

#include "slot_table_test.hpp"
#include "signal_for_ui.hpp"
#include "signal_table.hpp"

#include "ui_rotate_management.hpp"
#include "ui_test_for_draw.hpp"
#include "ui_face_scan.hpp"
#include "ui_soft_keyboard_context.hpp"
#include "my_mouse.hpp"
#include "screen_size.hpp"
#include "wifi_management.hpp"

#define DB_ORIGIN "/etc/user.db"
#define DB_DEST "/data/user.db"

static ui_rotate_management *rotate;
static pthread_t wdog_tid;
static int wdog_dead;

#if __cplusplus
extern "C" {
#endif

void mouse_wakeup()
{
  if (rotate)
    rotate->set_setting_count_state(TIMER_COUNTS_RESTART);
}

#if __cplusplus
}
#endif

/**
 * @brief  lang init
 * @note   only used for batch import and recg of person
 * @retval 0
 */
int lang_init() {
  if (-1 == clang_load("/usr/share/aio/res/elang/elang.el")) {
    ALOGE("[%s:%d]Can't open language file\n", __func__, __LINE__);
    return -1;
  }
  clang_set_language(ELANG_SIMPLE_CHINESE);
  return 0;
}

/**
 * @brief  qss init
 * @note
 * @retval 0
 */
int qss_init() {
  QString style, common_style, special_style;
  QFile common_qss(":/common.css");
  QString special_qss_path;
  int s_width = get_screen_width();
  int s_height = get_screen_height();

  if (common_qss.open(QFile::ReadOnly)) {
    common_style = QLatin1String(common_qss.readAll());
    common_qss.close();
  } else {
    ALOGE("Open qss failed");
    return -1;
  }
  if ((s_width == 720) && (s_height == 1280))
    special_qss_path = QString(":/720p.css");
  else if ((s_width == 1280) && (s_height == 720))
    special_qss_path = QString(":/1280p.css");
  else
    special_qss_path = QString(":/720p.css");
  QFile qss(special_qss_path);
  if (qss.open(QFile::ReadOnly)) {
    special_style = QLatin1String(qss.readAll());
    qss.close();
  } else {
    ALOGE("Open qss failed");
    return -1;
  }

  style = common_style + special_style;
  qApp->setStyleSheet(style);

  return 0;
}

/**
 * @brief  screen saver open callback function
 * @note   NULL
 * @retval 0
 */
int app_main_saver_open_cb(void) {
  send_signal_for_screen_saver(1);

  return 0;
}

/**
 * @brief  screen saver close callback function
 * @note   NULL
 * @retval 0
 */
int app_main_saver_close_cb(void) {
  send_signal_for_screen_saver(0);

  return 0;
}

/**
 * @brief  qrcode callback
 * @note   send shoq signal to ui
 * @param  data:  data of code
 * @retval None
 */
void app_main_qrcode_cb(char *data) {
  send_signal_for_qrcode(QString(data));
}

/**
 * @brief  face recg callback
 * @note   send draw signal to ui
 * @param  *f:  not used
 * @param  login:  not used
 * @retval None
 */
void app_main_face_note_cb(char ret, char *name,
  int name_sz, float temperature, int *note, int note_sz) {
  QString status("");
  QString name_str = QString(name);

  for (int i = 0; i < note_sz; i++) {
    if (RECG_TEMP_PREPARING == note[1] || RECG_TEMP_MEASURING == note[1] ||
      RECG_POSE_ERROR == note[1]) {
      name_str = "";
      status = rotate->recg_notes[(RECG_NOTE)note[1]];
      break;
    }
    if (RECG_TEMP_NORMAL == note[i] ||
      RECG_TEMP_ABNORMAL == note[i]) {
      status.append(QString(rotate->recg_notes[(RECG_NOTE)note[i]])
        .arg(temperature));
    } else {
      status.append(rotate->recg_notes[(RECG_NOTE)note[i]]);
    }

    if (i != note_sz - 1 && !(status.compare("") == 0))
      status.append("\n");
  }

  if (rotate->get_main_param()->name_format && name_str != "" &&
        name_str.compare(STRANGER_STR))
    name_str = name_str.left(1) + "**";
  if (rotate->get_main_param()->isName == 0 && name_str.compare(STRANGER_STR))
    name_str = "";
  if (name_str.compare(STRANGER_STR) == 0)
    name_str = rotate->recg_notes[RECG_STRANGER];
  send_signal_for_recg_notes((int)ret, name_str, status);
}

/**
 * @brief  appcation start stage 2
 * @param  *arg:
 * @retval None
 */
void* start_stage_2_proc(void* arg) {
  prctl(PR_SET_NAME, "start_stage_2");

  ALOGD("appcation start stage 2.");
  //todo: start state 2

  return NULL;
}

/**
 * @brief  face det callback
 * @note   send draw signal to ui
 * @param  *f:  not used
 * @param  login:  not used
 * @retval None
 */
void app_main_face_cb(faces_t* f, int login) {
  static bool working = false;

  if (!working) {
    pthread_t tid;
    pthread_create(&tid, NULL, start_stage_2_proc, NULL);
    pthread_detach(tid);
    working = true;
  }

  send_signal_for_draw();
  if (login == 2) {
    login = 0;
    send_signal_for_login();
  }
}

/**
 * @brief  reset key callback
 * @note   send factory signal to ui
 * @retval None
 */
void reset_cb() {
  send_signal_for_factory_default();
}

/**
 * @brief  alarm in callback
 * @note   send alarm in signal to ui
 * @retval None
 */
void alarmin_cb(int value) {
  // send_signal_for_websocket(WEB_SOCKET_EVENT_ALARMIN);
  ALOGD("alarmin is alarmed");

  int door_state = 0;
  if (value){
    door_state = DOOR_STATE_OPEN;
  }else{
    door_state = DOOR_STATE_CLOSE;
  }
  alarm_state_change(door_state);
  if (door_state == DOOR_STATE_OPEN)
    alarming();
  else
    alarming_stop();
}


/**
 * @brief  check upfate file in U disk
 * @note   called by usd insert event
 * @param  *arg:
 * @retval None
 */
void* check_update_file_thread(void* arg) {
  prctl(PR_SET_NAME, "updata_thread");

  if (find_pkg() == 0) {
    send_signal_for_update_firmware();
#if 0
  } else {
    send_signal_for_websocket(WEB_SOCKET_EVENT_RECORD_EXPORT);
#endif
  }

  return NULL;
}

/**
 * @brief  create a thread for check update file
 * @note   NULL
 * @retval None
 */
void start_check_update_file() {
  pthread_t tid;

/* Start check update file thread */
  pthread_create(&tid, NULL, check_update_file_thread, NULL);
  pthread_detach(tid);
}

/**
 * @brief  storage deivce monitor callback
 * @note   support sd card and udisk
 * @param  type: sd and udisk
 * @param  state: insert and pull out device
 * @retval None
 */
void hotplug_cb(hotplug_device_type_e type,
                hotplug_device_state_e state) {
  if (state == HOTPLUG_STATE_CONNECT) {
    if (HOTPLUG_TYPE_SDCARD == type) {
      ALOGD("%s: sdcard connect! mount= %d\n", __func__, sdcard_is_ok());
      send_signal_for_storage(STORAGE_EVENT_SDCARD, STORAGE_TYPE_CONNECT);
    } else {
      ALOGD("%s: udisk connect! mount= %d\n", __func__, udisk_is_ok());
      send_signal_for_storage(STORAGE_EVENT_UDISK, STORAGE_TYPE_CONNECT);
      start_check_update_file();
    }
  } else {
    if (HOTPLUG_TYPE_SDCARD == type) {
      ALOGD("%s: sdcard disconnect!\n", __func__);
      send_signal_for_storage(STORAGE_EVENT_SDCARD, STORAGE_TYPE_DISCONN);
    } else {
      ALOGD("%s: udisk disconnect!\n", __func__);
      send_signal_for_storage(STORAGE_EVENT_UDISK, STORAGE_TYPE_DISCONN);
    }
  }
}

#if ATTENDANCE_MODE == 2
/**
 * @brief  wifi callback event
 * @note   only used for wifi config through bluetooth in wechat mode
 * @param  event: bt_wifi_connected bt_wifi_disconnect
 * @retval None
 */
void bt_wifi_event_callback(bt_wifi_event event) {
  wifi_management *mgr = wifi_management::getInstance();

  if (event == bt_wifi_connected) {
    ws_svc_app_net_reconnect();
    mgr->update_current_connect();
    mgr->notify_wifi_state_change(WIFI_STATION_CONNECTED);
  } else {
    ws_svc_app_set_netlink(0);
    mgr->update_alive_dot(NULL);
    mgr->notify_wifi_state_change(WIFI_STATION_SCANNING);
  }
}

/**
 * @brief  wechat callback function
 * @note   NULL
 * @param  signal:  refer to ws_svc_main.h
 * @retval None
 */
void app_main_wechat_cb(wechat_signal_t signal) {
  int status = 0;
  char version[32] = {0};

  switch (signal) {
    case signal_enter_page:
      send_signal_for_websocket(WEB_SOCKET_EVENT_ENTER_PAGE);
      break;
    case signal_exit_page:
      send_signal_for_websocket(WEB_SOCKET_EVENT_EXIT_PAGE);
      break;
    case signal_login:
      send_signal_for_websocket(WEB_SOCKET_EVENT_LOGIN);
      break;
    case signal_qrcode:
      send_signal_for_websocket(WEB_SOCKET_EVENT_QRCODE);
      break;
    case signal_active:
      send_signal_for_websocket(WEB_SOCKET_EVENT_ACTIVE);
      break;
    case signal_bind:
      send_signal_for_websocket(WEB_SOCKET_EVENT_BIND);
      websocket_sound_play(sound_bind);
      break;
    case signal_unbind:
      send_signal_for_websocket(WEB_SOCKET_EVENT_UNBIND);
      websocket_sound_play(sound_unbind);
      break;
    case signal_link:
      ALOGD("websocket signal_link\n");
      send_signal_for_websocket(WEB_SOCKET_EVENT_LINKED);
      websocket_sound_play(sound_netlink);
      break;
    case signal_dislink:
      ALOGD("websocket signal_dislink\n");
      send_signal_for_websocket(WEB_SOCKET_EVENT_DIS_LINKED);
      websocket_sound_play(sound_netdislink);
      break;
  case signal_health_status:
      ws_app_get_body_status(&status);
      ALOGD("signal_health_status:%d\n", status);
      break;
  case signal_new_firmware:
      ws_app_get_firmware_version(version);
      ALOGD("%s:signal_new_firmware %s\n", __func__, version);
      ws_app_start_download_firmware();
      break;
  case signal_firmware_success:
      ALOGD("signal_firmware_success\n");
      ota_start();
      break;
  }
}
#endif

static void system_init(void) {
  int ret = 0;
  int delay_time = 0, data_size = 0, alarm_switch = 0;
  net_config_para_t eth_para;
  wifi_config_para_t wifi_para;
  fill_light_param_t fl_param;
  screen_bright_t scr_bright;
  screen_off_status_t scr_status;
  access_param_t access_param;
  alarm_service_t alarm_service;

  memset(&eth_para, 0, sizeof(net_config_para_t));
  memset(&wifi_para, 0, sizeof(wifi_config_para_t));
  memset(&fl_param, 0, sizeof(fill_light_param_t));
  memset(&scr_bright, 0, sizeof(screen_bright_t));
  memset(&scr_status, 0, sizeof(screen_off_status_t));
  memset(&access_param, 0, sizeof(access_param_t));
  memset(&alarm_service, 0, sizeof(alarm_service_t));

#if 0
  ret = lang_init();
  if (ret)
    ALOGE("init lang_init  failed");
#endif
  ret = hotplug_mgr_init();
  if (ret)
    ALOGE("init hotplug_mgr failed");

  ret = ui_device_get_screen_param(&scr_bright, &scr_status);
  if (ret)
    ALOGE("ui_device_get_screen_param failed");

  ret = screen_svc_init(&scr_bright, &scr_status);
  if (ret)
    ALOGE("init screen svc failed");

  ret = ui_device_get_fill_light_param(&fl_param);
  if (ret)
    ALOGE("ui_device_get_fill_light_param failed");

  ret = filling_light_init(&fl_param);
  if (ret)
    ALOGE("init fill light failed");

  ret = system_volume_init();
  if (ret)
    ALOGE("init system volum init failed");

  ret = ui_device_eth_param_query(&eth_para);
  if (ret)
    ALOGE("net_wired_get_config_para failed");
  eth_management::getInstance()->init_management(&eth_para);

#if ATTENDANCE_MODE == 2
  ret = bt_init();
  if (!ret) {
    bt_set_cb(bt_wifi_event_callback);
  } else {
    ALOGE("init bt wifi device failed");
  }
  /* Initailize websocket */
  ret = ws_svc_app_init();
  if (ret) {
    ALOGE("ws_svc_app_init failed");
  } else {
    ws_svc_app_set_cb(app_main_wechat_cb);
  }
#else
  ret = ui_device_wifi_param_query(&wifi_para);
  if (ret)
    ALOGE("net_wifi_get_config_para failed");
  ret = wifi_management::getInstance()->init_management(&wifi_para);
  if (ret)
    ALOGE("init wifi manager failed");
#endif

  ret = reset_factory_svc_init(reset_cb);
  if (ret)
    ALOGE("init factory failed");

  ret = alarm_in_svc_init(alarmin_cb);
  if (ret)
    ALOGE("init alarm_in_svc failed");

#if ATTENDANCE_MODE == 2
  /* do nothings */
#else
  ui_device_get_access_param(&access_param);
  alarm_service.alarm_switch = access_param.alarm_switch;
  alarm_service.delay_time = access_param.door_sensor_delay;
  alarm_service.door_sensor_type = access_param.door_sensor;
  ret = alarm_start_service(&alarm_service);
  if (ret)
    ALOGE("alarm_start_service failed");

  ret = wiegand_write_set_data_size(access_param.wiegand_protocol);
  if (ret)
    ALOGE("wiegand_write_set_data_size failed");

  ret = open_door_start_service(access_param.relay_delay_time);
  if (ret)
    ALOGE("open_door_start_service failed");
#endif

  /* ntp thread */
  device_time_param_t info;
  memset(&info, 0, sizeof(device_time_param_t));
  ret = db_device_time_param_query(&info);
  ret = ntp_time_init(info.auto_set_time);
  if (ret)
    ALOGE("ntp time init failed");
  ret = ui_control_set_timezone(ui_control_get_timezone());
  if (ret)
    ALOGE("set time zone failed");

  /* auto reboot thread */
  auto_reboot_param_t auto_reboot_param;
  data_time_t reboot_time;
  memset(&auto_reboot_param, 0, sizeof(auto_reboot_param_t));
  memset(&reboot_time, 0, sizeof(data_time_t));
  ui_control_get_auto_reboot_param(&auto_reboot_param);
  ui_control_get_reboot_time(&auto_reboot_param, &reboot_time);
  reboot_start_service(&reboot_time);
}

/**
 * @brief  db init
 * @note   include check and copy db from default dir
 * @retval 0 success
 */
static int db_init(void) {
  int ret;

  ALOGI("db check\n");
  db_all_init();
  /* whether there is a db in /data */
  if (!is_file_exist(DB_DEST)) return 0;

  /* copy db from /etc to /data */
  ALOGI("copy %s to %s", DB_ORIGIN, DB_DEST);
  ret = file_copy(DB_ORIGIN, DB_DEST);
  if (ret) {
    ALOGE("failed to copy %s to %s", DB_ORIGIN, DB_DEST);
    return -1;
  }

  return 0;
}

/**
 * signal_proc - Signal handler interface.
 * @signo signal number.
 * @info a pointer to a siginfo_t.
 * @context  a pointer to a ucontext_t (cast to void *).
 *
 * None.
 */
static void signal_proc(int signo, siginfo_t* info, void* context) {
  switch (signo) {
    case SIGTERM:
      ALOGI("Process SIGTERM signal");
      face_deinit();
#if ATTENDANCE_MODE > 0
      ws_svc_app_deinit();
#endif
#ifdef DEF_WEBSERVER
      http_stop_web_server();
#endif
      break;
    default:
      ALOGE("[%s:%d]Error: signo=%d\n", __func__, __LINE__, signo);
      break;
  }
}

/**
 * signal_handler_init - Signal processing initializatoin.
 * @None.
 *
 * Returns 0  if success, other if failure.
 */
static int signal_handler_init(void) {
  int i;
  sigset_t st_sigset;
  struct sigaction act;

  int signal_no[] = {
    SIGTERM,
    SIGPIPE,
  };

  sigemptyset(&st_sigset);
  sigprocmask(SIG_UNBLOCK, &st_sigset, NULL);
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = signal_proc;
  for (i = 0; i < (sizeof(signal_no) / sizeof(int)); i++) {
    if (sigaction(signal_no[i], &act, NULL) < 0) {
      ALOGE("[%s:%d]sigaction error\n", __func__, __LINE__);
      return -1;
    }
  }

  return 0;
}

/**
* @brief  web callback
* @note   NULL
* @event web callback event id.
* @value web event value.
*
* None.
*/
void app_main_web_cb(WEB_ID event, int value) {
  ALOGD("%s:%d >> web_id=%d, value=%d", __func__, __LINE__, event, value);
  if (WEB_PLATFORM_HEARBEAT == event) {
    face_set_platform_hearbeat();
  } else if (WEB_PLATFORM_CHANGE == event) {
    face_set_platform_param(value);
  }
}

/**
 * @brief  start face and system by async
 * @note   NULL
 * @param  *arg:
 * @retval None
 */
void* async_starter(void* arg) {
  int ret;

  /* Attention for 256M ddr
  * Serailize face-init and system-init because nn init will alloc
  * large memory and remain little free memory which cause hardware init failed.
  * e.g. fork udhcp failed.
  */
  prctl(PR_SET_NAME, "async_starter");

  ALOGI("async_starter");

  /* Initialize, config, start face service */
  ret = face_init();
  face_set_cb(app_main_face_cb);
  if (ret)
    ALOGE("face init failed");
  else
    send_signal_for_websocket(WEB_SOCKET_EVENT_FACE_INIT_OK);

  /* Enable callback */
  saver_open_set_cb(app_main_saver_open_cb);
  saver_close_set_cb(app_main_saver_close_cb);

  lang_init();
  system_init();
  hotplug_register_change_cb(hotplug_cb);

  ret = face_set_qrcode_cb(app_main_qrcode_cb);
  if (ret)
    ALOGE("set qrcode cb failed");

  ret = face_set_note_cb(app_main_face_note_cb);
  if (ret)
    ALOGE("set face note cb failed");

#if ATTENDANCE_MODE == 2
  /* Initailize websocket */
  ret = ws_svc_app_init();
  if (ret) {
    ALOGE("ws_svc_app_init failed");
  } else {
    ws_svc_app_set_cb(app_main_wechat_cb);
  }
#endif
#ifdef DEF_WEBSERVER
  http_set_web_handle_cb(app_main_web_cb);
  ret = http_start_web_server();
  if (ret != 0)
    ALOGE("http_start_web_server failed!");
#endif

#if ATTENDANCE_MODE == 0
  ret = live_video_init(face_rgb_hdl(get_face_handle()));
  if (ret != 0)
    ALOGE("live_video_init failed!");
#endif

  /* Turn on infra led temporarily */
  infrared_led_ctrl(LED_ON);
  white_led_ctrl(LED_OFF);

  /* check udisk */
  ret = udisk_is_ok();
  if (0 == ret) {
    hotplug_cb(HOTPLUG_TYPE_UDISK, HOTPLUG_STATE_CONNECT);
  } else {
    hotplug_cb(HOTPLUG_TYPE_UDISK, HOTPLUG_STATE_DISCONNECT);
  }

  return NULL;
}

/**
 * @brief  start face and system by async
 * @note   NULL
 * @param  *arg:
 * @retval None
 */
void* wdog_feed(void* arg) {
  watchdog_init();
  wdog_dead = 0;
  while(!wdog_dead) {
    ALOGD("feed the watch dog.");
    watchdog_feed();
    sleep(1);
  }
}

/**
 * @brief  main function
 * @note   NULL
 * @param  argc:
 * @param  *argv[]:
 * @retval 0
 */
QTranslator translator;
int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  pthread_t tid;
  int ret = 0;
  int hasDog = is_file_exist("/data/wdog");

  if (hasDog)
    pthread_create(&wdog_tid, NULL, wdog_feed, NULL);

  /* Set signal processing function */
  ALOGI("Set signal processing function");
  ret = signal_handler_init();
  if (ret)
    ALOGE("Signal init failed");

  /* Check and initialize database */
  ret = db_init();
  if (ret) {
    ALOGE("db check failed");
    return -1;
  }

  if (is_file_exist(LOGO_PATH))
    file_copy(LOGO_SOURCE, LOGO_PATH);
  eth_management::getInstance();
  wifi_management::getInstance();

  /* Start face svc asynchronously */
  pthread_create(&tid, NULL, async_starter, NULL);
  pthread_detach(tid);

  /* Set global text codec to UTF-8 */
  QTextCodec* codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForLocale(codec);
  QTextCodec::setCodecForCStrings(codec);
  QTextCodec::setCodecForTr(codec);
  QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));

  /* Set global font */
  QFont font;
  font.setFamily("SourceHanSansCN-Normal");
  font.setBold(false);
  font.setPixelSize(16);
  a.setFont(font);

  /* Open qss and set stype sheet */
  qss_init();
  /* Initialize global signal slot */
  signal_init();

  /* Set input method */
  ui_soft_keyboard_context input;
  a.setInputContext(&input);

  /* Show the first UI */
  rotate = ui_rotate_management::getInstance();
  rotate->slot_bind_choose();

  /* Mouse cursor init */
  qApp->setOverrideCursor(Qt::BlankCursor);
  rotate->cursor_status = 0;
  rotate->slot_for_dir_changed("/dev/input");

  /*add mouse listener*/
  my_mouse mouse;
  a.installEventFilter(&mouse);
  a.exec();
  ALOGE(">>>> Aio process would been dead!!! >>>>");
  if (hasDog) {
    wdog_dead = 1;
    pthread_join(wdog_tid, NULL);
  }

  return 0;
}
