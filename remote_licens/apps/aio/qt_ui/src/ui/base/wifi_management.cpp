/*
* wifi_management.cpp - define wifi manager of project
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

#define LOG_TAG     "wifi_mgr"
#include <log/log.h>
#include "ws_bt_wifi.h"
#include "ui_control.h"
#include "ip_config.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <sys/prctl.h>
#include "wifi_management.hpp"
#include "eth_management.hpp"

#define SCAN_INTERVSL 8000

#if NETWORK_DEBUG == 1
#define NATIVE_WIFI(func) do { \
  ALOGD("%s:%d >> native debug start!\n", __func__, __LINE__); \
  func \
  ALOGD("%s:%d >> native debug finish!\n", __func__, __LINE__); \
} while (0)

#else
#define NATIVE_WIFI(func) do { \
  func \
} while (0)
#endif

wifi_management *wifi_management::instance = nullptr;
wifi_dot softap_mode::concfig;
const char *WIFI_SEC[] = {"OPEN", "WPA", "WPA2", "UNKNOWN"};
SAVED_WIFI_CONFIG saved_wifi_config = {0, NULL};

#if __cplusplus
extern "C" {
#endif
void save_wifi_config()
{
  ALOGD("%s:%d", __func__, __LINE__);
  wifi_management *mgr = wifi_management::getInstance();
  mgr->save_config();
}

int sync_set_wifi_enabled(char on)
{
  int ret = 0;
  ALOGD("%s:%d >> on=%d", __func__, __LINE__, on);
  wifi_management *mgr = wifi_management::getInstance();
  ret = mgr->sync_set_wifi_enabled(0 != on);
  ALOGD("%s:%d >> return ret=%d", __func__, __LINE__, ret);
  return ret;
}

int sync_get_scan_list(SCAN_INFO *p_ret)
{
  ALOGD("%s:%d >> mum=%d", __func__, __LINE__, p_ret->num);
  QList<wifi_dot> wifi_list;

  wifi_management *mgr = wifi_management::getInstance();
  int ret = mgr->sync_get_scan_list(&wifi_list);
  if (0 == ret) {
    for (int i = 0; i < wifi_list.count(); i++) {
      if (p_ret->num <= i) break;
      wifi_dot tmp = wifi_list.at(i);

      p_ret->info[i].rssi = tmp.rssi;
      p_ret->info[i].security = tmp.security;
      memcpy(p_ret->info[i].ssid, &tmp.ssid, WIFI_NAME_LEN);
    }
  }
  ALOGD("%s:%d >> return ret=%d", __func__, __LINE__, ret);
  return ret;
}

int sync_connect_network(char *p_ssid, char *p_pwd,
  int security_type)
{
  int ret = 0;
  wifi_dot dot;

  ALOGD("%s:%d >> connect:%s[%s]", __func__, __LINE__,
    p_ssid, p_pwd);

  memset(&dot, 0, sizeof(dot));
  strncpy(dot.ssid, p_ssid, WIFI_NAME_LEN);
  strncpy(dot.pwd, p_pwd, WIFI_PWD_LEN);
  dot.security = (SECURITY)security_type;
  wifi_management *mgr = wifi_management::getInstance();
  ret = mgr->sync_connect_network(&dot);
  ALOGD("%s:%d >> return ret=%d", __func__, __LINE__, ret);
  return ret;
}

void get_wifi_para(wifi_config_para_t *param)
{
  ALOGD("%s:%d", __func__, __LINE__);
  wifi_management *mgr = wifi_management::getInstance();
  mgr->get_wifi_para(param);
}

void get_current_ssid(char *ssid)
{
  ALOGD("%s:%d", __func__, __LINE__);
  wifi_management *mgr = wifi_management::getInstance();
  QString cur_ssid = mgr->get_alive_ssid();
  memcpy(ssid, cur_ssid.toUtf8().data(), cur_ssid.length());
}

#if __cplusplus
}
#endif

int ssid_sort_by_rssi(SCAN_INFO *p_ret)
{
  HOTSPOT_INFO tmp_scan_info;
  int left = 0;
  int right = p_ret->num - 1;
  int i = 0, j = 0;

  if (p_ret == NULL) {
    ALOGE("%s: parameter pointer is null", __func__);
    return -1;
  }

  if (p_ret->num == 0) {
    ALOGE("%s: scan number is zero", __func__);
    return -1;
  }
  memset(&tmp_scan_info, 0x00, sizeof(tmp_scan_info));
  while (left < right) {
    for (i = left; i < right; i++) {
      if (p_ret->info[i].rssi < p_ret->info[i + 1].rssi) {
        memcpy(&tmp_scan_info, &p_ret->info[i],
          sizeof(HOTSPOT_INFO));
        memcpy(&p_ret->info[i], &p_ret->info[i + 1],
          sizeof(HOTSPOT_INFO));
        memcpy(&p_ret->info[i + 1], &tmp_scan_info,
          sizeof(HOTSPOT_INFO));
        memset(&tmp_scan_info, 0x00, sizeof(tmp_scan_info));
      }
    }
    right = right - 1;
    for (j = right; j > left; j--) {
      if (p_ret->info[j + 1].rssi > p_ret->info[j].rssi) {
        memcpy(&tmp_scan_info, &p_ret->info[j + 1],
          sizeof(HOTSPOT_INFO));
        memcpy(&p_ret->info[j + 1], &p_ret->info[j],
          sizeof(HOTSPOT_INFO));
        memcpy(&p_ret->info[j], &tmp_scan_info,
          sizeof(HOTSPOT_INFO));
        memset(&tmp_scan_info, 0x00, sizeof(tmp_scan_info));
      }
    }
  }

  return 0;
}

void *wifi_mgr_proc(void *arg) {
  prctl(PR_SET_NAME, "wifi_mgr_thr");

  wifi_management *mgr = (wifi_management *)arg;
  mgr->mgr_proc_impl();
  return NULL;
}

void net_dhcp_handler(dhcp_status_t status)
{
  ALOGD("%s:%d >> status=%d", __func__, __LINE__, status);
  wifi_management *mgr = wifi_management::getInstance();
  mgr->dhcp_handler(status);
}

void wifi_event_callback(int event) {
  ALOGD("%s:%d >> event=%d", __func__, __LINE__, event);
  wifi_management *mgr = wifi_management::getInstance();
  mgr->net_wifi_event_cb(event);
}

mode::mode(wifi_management *_mgr,    wifi_mode id, QObject *parent):
  mgr(_mgr), ID(id), cur_net(NULL) {
  ALOGD("%s:%d >> mdoe construtor", __func__, __LINE__);
  wifi_list.clear();
  is_enable = false;
  if (pthread_mutex_init(&list_lock, NULL)) {
    ALOGE("init list failed, errno: %d (%s)\n",
      errno, strerror(errno));
  }
}

mode::~mode() {
  ALOGD("%s:%d >> release", __func__, __LINE__);
  save_config();
  set_mode_enable(false);
  pthread_mutex_lock(&list_lock);
  if (!wifi_list.isEmpty())
    wifi_list.clear();
  pthread_mutex_unlock(&list_lock);
  if (cur_net) {
    delete cur_net;
    cur_net = NULL;
  }
  pthread_mutex_destroy(&list_lock);
}

int mode::get_native_connect(wifi_dot *dot) {
  ALOGD("%s:%d", __func__, __LINE__);
  if (cur_net) {
    memcpy(dot, cur_net, sizeof(wifi_dot));
    return 0;
  } else {
    return -1;
  }
}

int mode::get_scan_list(QList<wifi_dot> *list) {
  ALOGD("%s:%d", __func__, __LINE__);
  int ret = 0;
  list->clear();
  if (is_enable) {
    pthread_mutex_lock(&list_lock);
    for (; ret < wifi_list.count(); ++ret) {
      list->append(wifi_list.at(ret));
    }
    pthread_mutex_unlock(&list_lock);
  }
  return ret;
}

int mode::set_mode_enable(bool enable) {
  ALOGD("%s:%d >> enable=%d", __func__, __LINE__, enable);
  is_enable = enable;
  return 0;
}

void mode::save_config() {
  ALOGD("%s:%d >> save config to db", __func__, __LINE__);
}

station_mode::station_mode(wifi_management *mgr,
  QObject *parent): mode(mgr, WIFI_STATION, parent),
  scan_lock(false), connect_lock(false) {
  ALOGD("%s:%d >> constructor", __func__, __LINE__);
}

station_mode::~station_mode() {
  ALOGD("%s:%d >> release", __func__, __LINE__);
  save_config();
}

int station_mode::set_mode_enable(bool enable) {
  int ret = 0;
  ALOGD("%s:%d >> on?%d, is_enable=%d", __func__, __LINE__,
    enable, is_enable);

  if (enable) {
    if (is_enable)
      return ret;
    pthread_mutex_lock(&list_lock);
    wifi_list.clear();
    pthread_mutex_unlock(&list_lock);

    NATIVE_WIFI(ret = sta_enable((STA_EVENT_HANDLER)wifi_event_callback););
    if (ret != 0) {
      ALOGE("%s: enable wifi failed!\n", __func__);
      return ret;
    }
    is_enable = true;
  } else {
    if (!is_enable)
      return ret;
    pthread_mutex_lock(&list_lock);
    wifi_list.clear();
    pthread_mutex_unlock(&list_lock);

    NATIVE_WIFI(ret = sta_disable(););
    if (ret != 0) {
      ALOGE("%s: disenable wifi failed!\n", __func__);
      return ret;
    }
    is_enable = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    mgr->notify_wifi_state_change(WIFI_STATION_IDLE);
  }
  return ret;
}

int station_mode::connect_ssid(wifi_dot *dot) {
  int ret = -1;

  if (!is_enable)
    return ret;
  connect_lock = true;

  mgr->notify_wifi_state_change(WIFI_STATION_CONNECTING);
  ALOGD("%s:%d >> ssid=%s pwd=%s netid=%d", __func__, __LINE__,
    dot->ssid, dot->pwd, dot->netid);
  if (strlen(dot->pwd) == 0) {
    if (dot->netid > 0) {
      NATIVE_WIFI(ret = sta_connect_with_id(dot->netid););
    } else {
      NATIVE_WIFI(ret = sta_get_net_id(dot->ssid, dot->security, &dot->netid););
      if (ret == -1) {
        ALOGW("%s:obtain network configuration failed!\n", __func__);
      }
      NATIVE_WIFI(ret = sta_connect_with_id(dot->netid););
    }
    if (ret != 0) {
      ALOGE("%s: connect wifi failed!\n", __func__);
    }
  } else {
    NATIVE_WIFI(ret = sta_connect(dot->ssid, dot->pwd, dot->security););
    if (ret != 0) {
      ALOGE("%s: connect wifi failed!\n", __func__);
    }
  }
  return ret;
}

int station_mode::sync_connect(wifi_dot *dot) {
  int ret = -1;

  ret = connect_ssid(dot);
  if (ret)
    return -1;
  while (connect_lock && is_enable)
    usleep(200 * 1000);
#if 0
  if (is_enable && WIFI_STATION_CONNECTED == mgr->get_wifi_state()) {
    QString ssid = mgr->get_alive_ssid();
    if (0 == strcmp(dot->ssid, ssid.toUtf8().data())) {
      return 0;
    }
  }
#endif
  return mgr->get_wifi_state();
}

void station_mode::save_config() {
  int ret = 0;
  wifi_config_para_t net_info;
  memset(&net_info, 0, sizeof(wifi_config_para_t));
  ui_device_wifi_param_query(&net_info);
  net_info.enable_wifi = is_enable;
  ret = ui_device_wifi_param_update(&net_info);
  if (ret) {
    ALOGE("%s: update device net para failed(%d)\n", __func__, ret);
    return;
  }
}

int station_mode::scan_update() {
  ALOGD("%s:%d", __func__, __LINE__);
  int ret = 0;
  if (connect_lock) {
    ALOGW("%s: wifi connecting now, ignored scan update!\n", __func__);
    return -1;
  }
  scan_lock = true;
  NATIVE_WIFI(ret = sta_scan(););
  if (ret) {
    ALOGE("%s: scan failed!%d\n", __func__, ret);
  }
  return ret;
}

int station_mode::sync_scan_update(QList<wifi_dot> *list) {
  ALOGD("%s:%d", __func__, __LINE__);
  if (!scan_lock) {
    if (scan_update()) {
      ALOGE("%s:%d >> scan failed", __func__, __LINE__);
      return -1;
    }
  }
  while (scan_lock && is_enable)
    usleep(200 * 1000);
  if (is_enable) {
    get_scan_list(list);
    return 0;
  }
  return -1;
}

int station_mode::get_native_connect(wifi_dot *dot) {
  int ret = 0;
  CONNECT_INFO *connect_info = NULL;
  wifi_dot item;

  connect_info = (CONNECT_INFO *)calloc(sizeof(CONNECT_INFO), 1);
  if (connect_info == NULL) {
    ALOGW("%s: malloc connect_info failed!\n", __func__);
    return -1;
  }
  NATIVE_WIFI(ret = sta_get_connect_info(connect_info););
  if (ret != 0) {
    ALOGW("%s: get connect_info failed!\n", __func__);
    return -1;
  }

  strncpy(dot->ssid, connect_info->ssid, WIFI_NAME_LEN);
  memset(dot->pwd, 0, WIFI_PWD_LEN);
  dot->netid = connect_info->id;
  pthread_mutex_lock(&list_lock);
  foreach(item, wifi_list) {
    if (0 == strcmp(dot->ssid, item.ssid)) {
      dot->security = item.security;
      dot->rssi = item.rssi;
      dot->hidden = item.hidden;
      break;
    }
  }
  pthread_mutex_unlock(&list_lock);
  ALOGD("%s:%d >> ssid=%s pwd=%s netid=%d rssi=%d security=%d",
    __func__, __LINE__, dot->ssid, dot->pwd, dot->netid,
    dot->rssi, dot->security);
  delete connect_info;
  return 0;
}

void station_mode::dhcp_handler(dhcp_status_t status) {
  if (!is_enable) {
    ALOGD("wifi is closed!\n");
    connect_lock = false;
    return;
  }

  switch (status) {
  case DHCP_TIMEOUT:
    ALOGW("%s: dhcp timeout!\n", __func__);
    connect_lock = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    mgr->notify_wifi_state_change(WIFI_STATION_UNAVAILABLE);
    break;

  case DHCP_SUCCESS:
  {
    ALOGD("%s: dhcp success!\n", __func__);
    int ret = 0;
    NATIVE_WIFI(ret = net_dev_change_priority("wlan0"););
    if (ret < 0)
      ALOGE("change_priority failed");
    wifi_dot dot;
    memset(&dot, 0, sizeof(wifi_dot));
    get_native_connect(&dot);
    mgr->update_alive_dot(&dot);
    connect_lock = false;
    mgr->notify_wifi_state_change(WIFI_STATION_CONNECTED);
  }
    break;

  case DHCP_ABORT:
    ALOGW("%s: dhcp abort!\n", __func__);
    connect_lock = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    /* mgr->notify_wifi_state_change(WIFI_STATION_UNAVAILABLE); */
    break;
  }
}

void station_mode::net_wifi_event_cb(int event) {
  ALOGD("%s:%d >> wifi[%d] recv event=%d", __func__, __LINE__,
    is_enable, event);
  if (!is_enable)
    return;

  switch (event) {
  case STA_DISCONNECTED:
    ALOGD("wifi disconnected!\n");
    connect_lock = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    mgr->notify_wifi_state_change(WIFI_STATION_DISCONNECTED);
    break;

  case STA_SCAN_RESULTS:
  {
    SCAN_INFO result;
    wifi_state state;
    int ret = 0;
    memset(&result, 0, sizeof(result));
    NATIVE_WIFI(ret = sta_scan_results(&result););
    if (ret != 0) {
      ALOGE("%s:%d >> get wifi scan list failed!", __func__, __LINE__);
    }
    ALOGD("get scan_result num: %d!!\n", result.num);
    ssid_sort_by_rssi(&result);
    pthread_mutex_lock(&list_lock);
    if (!wifi_list.isEmpty())
      wifi_list.clear();
    if (!is_enable)
      break;

    QString ssid = mgr->get_alive_ssid();
    for (int i = 0; i < result.num; i++) {
      /* ALOGD("ssid[%d][%d]: %s \n", i, result.info[i].rssi, result.info[i].ssid); */
      wifi_dot dot;
      memset(&dot, 0, sizeof(wifi_dot));
      strncpy(dot.ssid, result.info[i].ssid, WIFI_NAME_LEN);
      dot.security = result.info[i].security;
      dot.rssi = result.info[i].rssi;
      wifi_list.append(dot);
      if(ssid.compare(dot.ssid) == 0)
        mgr->update_alive_dot(&dot);
    }
    pthread_mutex_unlock(&list_lock);
    scan_lock = false;
    mgr->notify_wifi_scan();
  }
    break;

  case STA_CONNECTED:
  {
    ALOGD("start dhcp!\n");
    mgr->notify_wifi_state_change(WIFI_STATION_DHCPCING);
    NATIVE_WIFI(net_dev_start_dhcp("wlan0", net_dhcp_handler););
  }
    break;

  case STA_NETWORK_NOT_EXIST:
    ALOGW("network is not exist!\n");
    connect_lock = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    mgr->notify_wifi_state_change(WIFI_STATION_UNAVAILABLE);
    break;

  case STA_PASSWD_ERROR:
    ALOGW("password is wrong!\n");
    connect_lock = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    mgr->notify_wifi_state_change(WIFI_STATION_AUTHENTICATING_ERROR);
    break;

  case STA_CONNECT_REJECT:
    ALOGW("connection is rejected!\n");
    connect_lock = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    mgr->notify_wifi_state_change(WIFI_STATION_FAILED);
    break;

  case STA_UNKNOWN_EVENT:
    ALOGE("wifi recv unknow event!\n");
    connect_lock = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    mgr->notify_wifi_state_change(WIFI_STATION_FAILED);
    break;

  case STA_CONNECT_ABORT:
    ALOGW("wifi connect abort!\n");
    connect_lock = false;
    NATIVE_WIFI(net_dev_del_ip_config("wlan0"););
    break;
  }
}

int station_mode::get_wifi_dot(wifi_dot *dot, QString ssid) {
  int ret = -1;
  wifi_dot item;

  strncpy(dot->ssid, ssid.toUtf8().data(),WIFI_NAME_LEN);
  dot->netid = 0;
  pthread_mutex_lock(&list_lock);
  foreach(item, wifi_list) {
    if (0 == strcmp(dot->ssid, item.ssid)) {
      strncpy(dot->pwd, item.pwd, WIFI_PWD_LEN);
      dot->security = item.security;
      dot->rssi = item.rssi;
      dot->hidden = 0;
      ret = 0;
      break;
    }
  }
  pthread_mutex_unlock(&list_lock);

  return ret;
}

softap_mode::~softap_mode() {
  ALOGD("%s:%d >> release", __func__, __LINE__);
  save_config();
}

int softap_mode::set_mode_enable(bool enable) {
  int ret = 0;
  ALOGD("%s:%d >> on?%d, is_enable=%d", __func__, __LINE__,
    enable, is_enable);

  if (enable) {
    if (is_enable)
      return ret;
    NATIVE_WIFI(ret = softap_enable(););
    if (ret != 0) {
      ALOGE("%s: enable softap failed!\n", __func__);
      return ret;
    }
    is_enable = true;
  } else {
    if (!is_enable)
      return ret;
    NATIVE_WIFI(ret = softap_disable(););
    if (ret != 0) {
      ALOGE("%s: disenable softap failed!\n", __func__);
      return ret;
    }
    is_enable = false;
  }
  return ret;
}

void softap_mode::save_config() {
  int ret = 0;
  wifi_config_para_t net_info;
  memset(&net_info, 0, sizeof(wifi_config_para_t));
  ui_device_wifi_param_query(&net_info);
  net_info.enable_hotspot = is_enable;
  memcpy(net_info.hotspot_name, concfig.ssid, WIFI_NAME_LEN);
  memcpy(net_info.hotspot_password, concfig.pwd, WIFI_PWD_LEN);
  ret = ui_device_wifi_param_update(&net_info);
  if (ret) {
    ALOGE("%s: update device net para failed(%d)\n", __func__, ret);
    return;
  }
}

wifi_management::wifi_management(QObject *parent) :
  QObject(parent), cur_state(WIFI_STATION_IDLE),
  pTimer(new QTimer) {
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_scan_update()));
  //set_wifi_ap_config(QString("root"), QString("12345668"), SEC_WPA2);
  cur_mode = new station_mode(this);
  memset(&alive_dot, 0, sizeof(wifi_dot));

  if (pthread_mutex_init(&alive_lock, NULL)) {
    ALOGE("init alive failed, errno: %d (%s)\n",
      errno, strerror(errno));
  }

  if (pthread_mutex_init(&loop_lock, NULL)) {
    ALOGE("init loop failed, errno: %d (%s)\n",
      errno, strerror(errno));
  }

  if (pthread_cond_init(&loop_cond, NULL)) {
    ALOGE("init loop_cond failed, errno: %d (%s)\n",
      errno, strerror(errno));
  }

  if (pthread_create(&mgr_th, NULL, wifi_mgr_proc, (void *)this)) {
    ALOGE("create loop thread failed, errno: %d (%s)",
      errno, strerror(errno));
  }

}

wifi_management::wifi_management(const wifi_management&) {}

wifi_management& wifi_management::operator = (const wifi_management&) {}

wifi_management* wifi_management::getInstance() {
  if (!instance) {
    /* must be constuct in qt gui thread, or the timer works error */
    instance = new wifi_management();
  }
  return instance;
}

int wifi_management::init_management(wifi_config_para_t *wifi_info) {
  ALOGD("%s:%d >> init management", __func__, __LINE__);
  int ret = -1;
  if (!wifi_info)
    return ret;
  pthread_mutex_lock(&alive_lock);
  memset(&alive_dot, 0, sizeof(wifi_dot));
  pthread_mutex_unlock(&alive_lock);
  if (wifi_info->enable_hotspot) {
    set_wifi_ap_config(wifi_info->hotspot_name,
      wifi_info->hotspot_password, SECURITY_WPA2_PSK);
    set_wifi_ap_enabled(true);
  } else {
    set_wifi_enabled(wifi_info->enable_wifi);
  }
  return 0;
}

void wifi_management::notes_init() {
  int adcd = 0;
  if (!state_notes.isEmpty())
    state_notes.clear();
  state_notes[WIFI_STATION_IDLE] = tr("");
  state_notes[WIFI_STATION_STARTING] = tr("正在启动wifi ......");
  state_notes[WIFI_STATION_SCANNING] = tr("正在扫描wifi ......");
  state_notes[WIFI_STATION_CONNECTING] = tr("正在连接");
  state_notes[WIFI_STATION_DHCPCING] = tr("获取IP中");
  state_notes[WIFI_STATION_CONNECTED] = tr("已连接");

  state_notes[WIFI_STATION_DISCONNECTING] = tr("正在断开");
  state_notes[WIFI_STATION_DISCONNECTED] = tr("已断开");
  state_notes[WIFI_STATION_AUTHENTICATING_ERROR] = tr("密码错误");

  state_notes[WIFI_STATION_UNAVAILABLE] = tr("网络不可达");
  state_notes[WIFI_STATION_FAILED] = tr("网络连接失败");
  state_notes[WIFI_STATION_CLOSING] = tr("正在关闭wifi ......");
}

QString wifi_management::get_state_notes(wifi_state state) {
  return state_notes[state];
}

void wifi_management::mgr_proc_impl() {
  wifi_msg msg;
  int ret = 0;

  while(1) {
    pthread_mutex_lock(&loop_lock);
    if (loops.isEmpty())
      pthread_cond_wait(&loop_cond, &loop_lock);
    msg = loops.first();
    loops.pop_front();
    pthread_mutex_unlock(&loop_lock);
    ALOGD("%s:%d >> act=%d", __func__, __LINE__, msg.act);

    if (WIFI_STATION == cur_mode->ID) { //STATIOM MODE
      switch (msg.act) {
        case ACT_WIFI_ENABLE:
        {
          bool on = *(bool *)msg.args;
          ALOGD("%s:%d >> args=%d", __func__, __LINE__, *(bool *)msg.args);
          if (WIFI_STATION != cur_mode->ID) {
            mode *tmp = cur_mode;
            cur_mode = new station_mode(this);
            delete tmp;
          }
          pthread_mutex_lock(&alive_lock);
          memset(&alive_dot, 0, sizeof(wifi_dot));
          pthread_mutex_unlock(&alive_lock);
          ret = cur_mode->set_mode_enable(on);
          if (on && 0 == ret) {
            update_netid_list();
            notify_wifi_state_change(WIFI_STATION_SCANNING);
            slot_scan_update();
          }
          delete msg.args;
        }
          break;
        case ACT_WIFI_SCAN:
        {
          bool timing = *(bool *)msg.args;
          if (cur_mode->is_mode_enable()) {
            if (WIFI_STATION_CONNECTING != cur_state)
              ((station_mode *)cur_mode)->scan_update();
            if (timing)
              pTimer->start(SCAN_INTERVSL);
          } else {
            pTimer->stop();
          }
          delete msg.args;
        }
          break;
        case ACT_WIFI_CONNECT:
        {
          if (WIFI_STATION_CONNECTING == cur_state ||
            WIFI_STATION_DHCPCING == cur_state) {
            ALOGI("%s:%d >> wait 200ms for wifi connect reset!",
              __func__, __LINE__);
            usleep(200 * 1000);
          }
          if (!cur_mode->is_mode_enable()) {
            ALOGI("%s:%d >> wifi had been shut down!",
              __func__, __LINE__);
            delete msg.args;
            break;
          }
          pthread_mutex_lock(&alive_lock);
          memcpy(&alive_dot, (wifi_dot *)msg.args, sizeof(wifi_dot));
          pthread_mutex_unlock(&alive_lock);
          ALOGD("%s:%d >> ssid=%s pwd=%s netid=%d", __func__, __LINE__,
            alive_dot.ssid, alive_dot.pwd, alive_dot.netid);
          ((station_mode *)cur_mode)->connect_ssid(&alive_dot);
          delete msg.args;
        }
          break;
        case ACT_WIFI_DISABLE:
        {
          disable_network_imp((wifi_dot *)msg.args);
          delete msg.args;
        }
          break;
        case ACT_WIFI_DELDOT:
        {
          remove_network_imp((wifi_dot *)msg.args);
          delete msg.args;
        }
          break;
        default:
        {
          ALOGW("%s recv action[%d] not found.", __func__, msg.act);
          if (msg.args) {
            delete msg.args;
            msg.args = NULL;
          }
        }
          break;
      }
    } else { //AP MODE
      switch (msg.act) {
        case ACT_AP_ENABLE:
        {
          bool on = *(bool *)msg.args;
          if (WIFI_SOFTAP != cur_mode->ID) {
            mode *tmp = cur_mode;
            cur_mode = new softap_mode(this);
            delete tmp;
          }
          pthread_mutex_lock(&alive_lock);
          memset(&alive_dot, 0, sizeof(wifi_dot));
          pthread_mutex_unlock(&alive_lock);
          ret = cur_mode->set_mode_enable(on);
          delete msg.args;
        }
          break;
        case ACT_AP_CONFIG:
        {
          AP_CONFIG config;
          memset(&config, 0, sizeof(AP_CONFIG));
          config.ssid = softap_mode::concfig.ssid;
          config.passwd = softap_mode::concfig.pwd;
          config.security = (AP_SECURITY)softap_mode::concfig.security;
          NATIVE_WIFI(ret = softap_set_config(&config););
          if (ret != 0) {
            ALOGE("%s: set ap config failed!\n", __func__);
          }
        }
          break;
        default:
        {
          ALOGW("%s recv action[%d] not found.", __func__, msg.act);
          if (msg.args) {
            delete msg.args;
            msg.args = NULL;
          }
        }
          break;
      }
    }
  }
}

void wifi_management::dhcp_handler(dhcp_status_t status) {
  if (WIFI_STATION != cur_mode->ID) {
    ALOGD("%s:%d >> mode not match throw status[%d]",
      __func__, __LINE__, status);
    return;
  }
  ((station_mode *)cur_mode)->dhcp_handler(status);
}

void wifi_management::net_wifi_event_cb(int event) {
  if (WIFI_STATION != cur_mode->ID) {
    ALOGD("%s:%d >> mode not match throw event[%d]",
      __func__, __LINE__, event);
    return;
  }
  ((station_mode *)cur_mode)->net_wifi_event_cb(event);
}

void wifi_management::slot_scan_update() {
  ALOGD("%s:%d", __func__, __LINE__);
  wifi_msg msg;
  memset(&msg, 0, sizeof(wifi_msg));
  msg.act = ACT_WIFI_SCAN;
  msg.args = new bool(true);

  pthread_mutex_lock(&loop_lock);
  loops.append(msg);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
}

void wifi_management::request_scan_now() {
  wifi_msg msg;
  memset(&msg, 0, sizeof(wifi_msg));
  msg.act = ACT_WIFI_SCAN;
  msg.args = new bool(false);

  pthread_mutex_lock(&loop_lock);
  loops.append(msg);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
}

wifi_mode wifi_management::get_wifi_mode() {
  return cur_mode->ID;
}

wifi_state wifi_management::get_wifi_state() {
  return cur_state;
}

int wifi_management::get_alive_connect(wifi_dot *dot) {
  int ret = -1;
  if (!dot)
    return ret;
  pthread_mutex_lock(&alive_lock);
  memcpy(dot, &alive_dot, sizeof(wifi_dot));
  pthread_mutex_unlock(&alive_lock);
  return strlen(dot->ssid)?0:-1;
}

void wifi_management::update_alive_dot(wifi_dot *dot) {
  pthread_mutex_lock(&alive_lock);
  if (dot) {
    if (0 != strcmp(dot->ssid, alive_dot.ssid)) {
      strncpy(alive_dot.ssid, dot->ssid, WIFI_NAME_LEN);
      memset(dot->pwd, 0, WIFI_PWD_LEN);
      alive_dot.netid = dot->netid;
    }
    alive_dot.security = dot->security;
    alive_dot.rssi = dot->rssi;
    alive_dot.hidden = dot->hidden;
  } else {
    if (WIFI_STATION_CONNECTED != cur_state)
      memset(&alive_dot, 0, sizeof(wifi_dot));
  }
  pthread_mutex_unlock(&alive_lock);
}

void wifi_management::update_netid_list() {
#if 1
  return;
#else
  int ret = 0;
  ALOGD("%s:%d >> try to get netid list.", __func__, __LINE__);
  NATIVE_WIFI(ret = sta_get_saved_config(&saved_wifi_config););
  if (ret == -1) {
    ALOGE("%s: get saved wifi config failed!\n", __func__);
  } else {
    ALOGD("%s: my wifi_config num: %d\n", __func__, saved_wifi_config.num);
#if 1
    for (int i = 0; i < saved_wifi_config.num; i++) {
      ALOGD("%s: my network[%d] id: %d, ssid: %s, key_mgmt: %d\n",
        __func__, i, saved_wifi_config.config[i].net_id,
        saved_wifi_config.config[i].ssid,
        saved_wifi_config.config[i].security);
    }
#endif
  }
#endif
}

int wifi_management::get_scan_list(QList<wifi_dot> *list) {
  if (WIFI_STATION_CLOSING == cur_state) {
    list->clear();
    return 0;
  }
  return cur_mode->get_scan_list(list);
}

int wifi_management::sync_get_scan_list(QList<wifi_dot> *list) {
  if (!is_wifi_enabled())
    return -1;
  return ((station_mode *)cur_mode)->sync_scan_update(list);
}

void wifi_management::save_config() {
  cur_mode->save_config();
}

QString wifi_management::get_alive_ssid() {
  return QString(alive_dot.ssid);
}

void wifi_management::get_wifi_para(wifi_config_para_t *param) {
  if (!param)
    return;
  param->enable_wifi = is_wifi_enabled();
  param->enable_hotspot = is_wifi_ap_enabled();
  strncpy(param->hotspot_name, softap_mode::concfig.ssid, WIFI_NAME_LEN);
  strncpy(param->hotspot_password, softap_mode::concfig.pwd, WIFI_PWD_LEN);
}

void wifi_management::notify_wifi_state_change(wifi_state state) {
  ALOGD("%s:%d >> notify wifi state=%d", __func__, __LINE__, state);
  if (state == cur_state || (WIFI_STATION_CLOSING == cur_state &&
    WIFI_STATION_IDLE != state)) return;
  cur_state = state;
  emit signal_wifi_state_change((int)state);
}

void wifi_management::notify_wifi_scan() {
  emit signal_wifi_scan();
}

void wifi_management::set_wifi_enabled(bool on) {
  wifi_msg msg;
  ALOGD("%s:%d >> on=%d", __func__, __LINE__, on, cur_state);
  if (on) {
    if (WIFI_STATION_STARTING == cur_state)
      return;
    notify_wifi_state_change(WIFI_STATION_STARTING);
  } else {
    if (WIFI_STATION_CLOSING == cur_state ||
      WIFI_STATION_IDLE == cur_state) {
      return;
    } else if (WIFI_STATION_CONNECTING == cur_state) {
      ALOGD("%s:%d >> abort wifi connecting", __func__, __LINE__);
      NATIVE_WIFI(sta_disable_all_net_conf(););
    } else if (WIFI_STATION_DHCPCING == cur_state) {
      ALOGD("%s:%d >> abort wifi dhcpcing", __func__, __LINE__);
      NATIVE_WIFI(net_dev_stop_dhcp("wlan0"););
    }
    notify_wifi_state_change(WIFI_STATION_CLOSING);
  }

  memset(&msg, 0, sizeof(wifi_msg));
  msg.act = ACT_WIFI_ENABLE;
  msg.args = new bool(on);

  pthread_mutex_lock(&loop_lock);
  loops.append(msg);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
}

int wifi_management::sync_set_wifi_enabled(bool on) {
  int ret = 0;
  if (WIFI_STATION != cur_mode->ID)
    return -1;
  if (on) {
    if (WIFI_STATION_STARTING == cur_state)
      return ret;
  } else {
    if (WIFI_STATION_CLOSING == cur_state ||
      WIFI_STATION_IDLE == cur_state) {
      return ret;
    } else if (WIFI_STATION_CONNECTING == cur_state) {
      ALOGD("%s:%d >> abort wifi connecting", __func__, __LINE__);
      NATIVE_WIFI(sta_disable_all_net_conf(););
    } else if (WIFI_STATION_DHCPCING == cur_state) {
      ALOGD("%s:%d >> abort wifi dhcpcing", __func__, __LINE__);
      NATIVE_WIFI(net_dev_stop_dhcp("wlan0"););
    }
    notify_wifi_state_change(WIFI_STATION_CLOSING);
  }

  pthread_mutex_lock(&alive_lock);
  memset(&alive_dot, 0, sizeof(wifi_dot));
  pthread_mutex_unlock(&alive_lock);
  ret = cur_mode->set_mode_enable(on);
  if (on && 0 == ret) {
    update_netid_list();
    notify_wifi_state_change(WIFI_STATION_SCANNING);
    slot_scan_update();
  }
  cur_mode->save_config();

  return ret;
}

bool wifi_management::is_wifi_enabled() {
  return WIFI_STATION == cur_mode->ID && cur_mode->is_mode_enable();
}

bool wifi_management::is_wifi_connected() {
  return WIFI_STATION_CONNECTED == cur_state;
}

bool wifi_management::is_wifi_ap_enabled() {
  return WIFI_SOFTAP == cur_mode->ID && cur_mode->is_mode_enable();
}

int wifi_management::get_wifi_dot(wifi_dot *dot, QString ssid) {
  if (NULL == dot || !is_wifi_enabled())
    return -1;
  return ((station_mode *)cur_mode)->get_wifi_dot(dot, ssid);
}

int wifi_management::connect_network(wifi_dot *dot) {
  int ret = -1;
  int netid;
  wifi_msg msg;

  if (NULL == dot || !is_wifi_enabled())
    return ret;
  if (0 == strcmp(dot->ssid, alive_dot.ssid) &&
    (WIFI_STATION_STARTING == cur_state ||
    WIFI_STATION_CONNECTING == cur_state ||
    WIFI_STATION_DHCPCING == cur_state)) {
    return 0;
  }
  if (strlen(dot->pwd) <= 0) {
    NATIVE_WIFI(ret = sta_get_net_id(dot->ssid, dot->security, &netid););
    if (ret == -1) {
      ALOGW("%s:obtain network configuration failed!\n", __func__);
      return -2;
    }
    dot->netid = netid;
  }
  if (WIFI_STATION_CONNECTING == cur_state) {
    ALOGD("%s:%d >> abort wifi connecting", __func__, __LINE__);
    NATIVE_WIFI(sta_disable_all_net_conf(););
  } else if (WIFI_STATION_DHCPCING == cur_state) {
    ALOGD("%s:%d >> abort wifi dhcpcing", __func__, __LINE__);
    NATIVE_WIFI(net_dev_stop_dhcp("wlan0"););
  }

  memset(&msg, 0, sizeof(wifi_msg));
  msg.act = ACT_WIFI_CONNECT;
  msg.args = (void *) new wifi_dot;
  memcpy(msg.args, dot, sizeof(wifi_dot));

  pthread_mutex_lock(&loop_lock);
  loops.append(msg);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
  return ret;
}

int wifi_management::sync_connect_network(wifi_dot *dot) {
  if (NULL == dot || !is_wifi_enabled())
    return -1;
  if (WIFI_STATION_CONNECTING == cur_state) {
    ALOGD("%s:%d >> abort wifi connecting", __func__, __LINE__);
    NATIVE_WIFI(sta_disable_all_net_conf(););
    ALOGI("%s:%d >> wait 200ms for wifi connect reset!",
      __func__, __LINE__);
    usleep(200 * 1000);
  } else if (WIFI_STATION_DHCPCING == cur_state) {
    ALOGD("%s:%d >> abort wifi dhcpcing", __func__, __LINE__);
    NATIVE_WIFI(net_dev_stop_dhcp("wlan0"););
    ALOGI("%s:%d >> wait 200ms for wifi connect reset!",
      __func__, __LINE__);
    usleep(200 * 1000);
  }
  if (!cur_mode->is_mode_enable()) {
    ALOGI("%s:%d >> wifi had been shut down!",
      __func__, __LINE__);
    return -1;
  }
  ALOGD("%s:%d >> connect:%s[%s]", __func__, __LINE__,
    dot->ssid, dot->pwd);
  pthread_mutex_lock(&alive_lock);
  memcpy(&alive_dot, dot, sizeof(wifi_dot));
  pthread_mutex_unlock(&alive_lock);

  return ((station_mode *)cur_mode)->sync_connect(&alive_dot);
}

int wifi_management::disable_network(wifi_dot *dot) {
  if (WIFI_STATION != cur_mode->ID) {
    ALOGD("%s:%d >> mode don't match", __func__, __LINE__);
    return -1;
  }

  ALOGD("%s:%d >> ssid=%s", __func__, __LINE__, dot->ssid);
  disable_network_imp(dot);
#if 0
  wifi_msg msg;
  memset(&msg, 0, sizeof(wifi_msg));
  msg.act = ACT_WIFI_DISABLE;
  msg.args = (void *) new wifi_dot;
  memcpy(msg.args, dot, sizeof(wifi_dot));

  pthread_mutex_lock(&loop_lock);
  loops.append(msg);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
#endif
  return 0;
}

int wifi_management::disable_network_imp(wifi_dot *dot) {
  int ret = 0;
  ALOGD("%s:%d >> disable ssid=%s", __func__, __LINE__, dot->ssid);
  NATIVE_WIFI(ret = sta_get_net_id(dot->ssid, dot->security, &dot->netid););
  if (ret == -1) {
    ALOGW("%s:obtain network configuration failed!\n", __func__);
    return ret;
  }
  ALOGD("%s:%d >> disable netid=%d", __func__, __LINE__, dot->netid);
  NATIVE_WIFI(ret = sta_disable_net_conf(dot->netid););
  if (ret != 0) {
    ALOGE("%s: disable ssid failed!\n", __func__);
  } else if (0 == strcmp(dot->ssid, alive_dot.ssid)) {
    pthread_mutex_lock(&alive_lock);
    memset(&alive_dot, 0, sizeof(wifi_dot));
    pthread_mutex_unlock(&alive_lock);
  }
  ALOGD("%s:%d >> disable ssid=%s, netid=%d, ret=%d", __func__, __LINE__,
    dot->ssid, dot->netid, ret);
  return ret;
}

int wifi_management::remove_network(wifi_dot *dot) {
  if (WIFI_STATION != cur_mode->ID) {
    ALOGD("%s:%d >> mode don't match", __func__, __LINE__);
    return -1;
  }

  ALOGD("%s:%d >> ssid=%s", __func__, __LINE__, dot->ssid);
  remove_network_imp(dot);
#if 0
  wifi_msg msg;
  memset(&msg, 0, sizeof(wifi_msg));
  msg.act = ACT_WIFI_DELDOT;
  msg.args = (void *) new wifi_dot;
  memcpy(msg.args, dot, sizeof(wifi_dot));

  pthread_mutex_lock(&loop_lock);
  loops.append(msg);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
#endif
  return 0;
}

int wifi_management::remove_network_imp(wifi_dot *dot) {
  int ret = 0;
  ALOGD("%s:%d >> delete ssid=%s", __func__, __LINE__, dot->ssid);
  NATIVE_WIFI(ret = sta_get_net_id(dot->ssid, dot->security, &dot->netid););
  if (ret == -1) {
    ALOGW("%s:obtain network configuration failed!\n", __func__);
    return ret;
  }
  ALOGD("%s:%d >> delete netid=%d", __func__, __LINE__, dot->netid);
  NATIVE_WIFI(ret = sta_delete_net_conf(dot->netid););
  if (ret != 0) {
    ALOGE("%s: delete ssid failed!\n", __func__);
  } else if (0 == strcmp(dot->ssid, alive_dot.ssid)) {
    pthread_mutex_lock(&alive_lock);
    memset(&alive_dot, 0, sizeof(wifi_dot));
    pthread_mutex_unlock(&alive_lock);
  }
  ALOGD("%s:%d >> delete ssid=%s, netid=%d, ret=%d", __func__, __LINE__,
    dot->ssid, dot->netid, ret);
  return ret;
}

void wifi_management::set_wifi_ap_config(const char *ssid,
  const char *pwd, SECURITY security) {
  int len = 0;
  wifi_msg msg;
  ALOGD("%s:%d >> ssid=%s, pwd=%s, security=%d", __func__, __LINE__,
    ssid, pwd, (int)security);
  if (strlen(ssid) > 0) {
    len = strlen(ssid) > WIFI_NAME_LEN?
       WIFI_NAME_LEN:strlen(ssid);
    strncpy(softap_mode::concfig.ssid, ssid, len);
  }
  if (strlen(pwd) > 0) {
    len = strlen(pwd) > WIFI_PWD_LEN?
       WIFI_PWD_LEN:strlen(pwd);
    strncpy(softap_mode::concfig.pwd, pwd, len);
  }
  softap_mode::concfig.security = security;
  softap_mode::concfig.rssi = 0;
  softap_mode::concfig.netid = 0;
  softap_mode::concfig.hidden = 0;

  memset(&msg, 0, sizeof(wifi_msg));
  msg.act = ACT_AP_CONFIG;

  pthread_mutex_lock(&loop_lock);
  loops.append(msg);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
}

void wifi_management::set_wifi_ap_enabled(bool on) {
  ALOGD("%s:%d", __func__, __LINE__);
  wifi_msg msg;
  memset(&msg, 0, sizeof(wifi_msg));
  msg.act = ACT_AP_ENABLE;
  msg.args = new bool(on);

  pthread_mutex_lock(&loop_lock);
  loops.append(msg);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
}

