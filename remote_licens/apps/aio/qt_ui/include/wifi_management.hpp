/*
* wifi_management.hpp - define wifi manager of project
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

#ifndef WIFI_MANAGEMENT_H
#define WIFI_MANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "wifi_station.h"
#include "wifi_softap.h"
#include "ip_config.h"
#include "db_all.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QObject>
#include <pthread.h>
#include <QTimer>
#include <QMap>

typedef enum {
  WIFI_STATION = 0,
  WIFI_SOFTAP
} wifi_mode;

typedef enum {
  ACT_WIFI_ENABLE,
  ACT_WIFI_SCAN,
  ACT_WIFI_CONNECT,
  ACT_WIFI_DISABLE,
  ACT_WIFI_DELDOT,

  ACT_AP_CONFIG,
  ACT_AP_ENABLE,
  ACT_AP_SCAN,
} wifi_action;

typedef struct {
  wifi_action act;
  void *args;
} wifi_msg;

typedef enum {
  WIFI_STATION_IDLE = 0,
  WIFI_STATION_STARTING, //1
  WIFI_STATION_SCANNING, //2
  WIFI_STATION_CONNECTING, //3
  WIFI_STATION_DHCPCING, //4
  WIFI_STATION_CONNECTED, //5
  WIFI_STATION_DISCONNECTING, //6
  WIFI_STATION_DISCONNECTED, //7
  WIFI_STATION_AUTHENTICATING_ERROR, //8
  WIFI_STATION_UNAVAILABLE, //9
  WIFI_STATION_FAILED, //10
  WIFI_STATION_CLOSING, //11

  WIFI_SOFTAP_DISABLING,
  WIFI_SOFTAP_DISABLED,
  WIFI_SOFTAP_ENABLING,
  WIFI_SOFTAP_ENABLED,
  WIFI_SOFTAP_FAILED,
} wifi_state;

typedef struct {
  char ssid[WIFI_NAME_LEN];
  char pwd[WIFI_PWD_LEN];
  SECURITY security; /* encryption method */
  int rssi; /* signal strength */
  int netid;
  char hidden;
} wifi_dot;

class wifi_management;

  class mode : public QObject {
  Q_OBJECT

 public:
  explicit mode(wifi_management *mgr,
    wifi_mode id, QObject *parent = nullptr);
  ~mode();

  virtual int set_mode_enable(bool enable);
  virtual void save_config();
  virtual int get_native_connect(wifi_dot *dot);
  int get_scan_list(QList<wifi_dot> *list);

  bool is_mode_enable() {
    return is_enable;
  }

 protected:
  wifi_management *mgr;
  wifi_dot *cur_net;
  QList<wifi_dot> wifi_list;
  pthread_mutex_t list_lock;
  bool is_enable;

 public:
  wifi_mode ID;
};

class station_mode : public mode
{
  Q_OBJECT

 public:
  explicit station_mode(wifi_management *mgr,
    QObject *parent = nullptr);
  ~station_mode();

  int set_mode_enable(bool enable);
  int connect_ssid(wifi_dot *dot);
  int sync_connect(wifi_dot *dot);
  int get_native_connect(wifi_dot *dot);
  void save_config();
  int scan_update();
  int sync_scan_update(QList<wifi_dot> *list);
  void dhcp_handler(dhcp_status_t status);
  void net_wifi_event_cb(int event);
  int get_wifi_dot(wifi_dot *dot, QString ssid);

 private:
  bool scan_lock;
  bool connect_lock;
};

class softap_mode : public mode
{
  Q_OBJECT

 public:
  explicit softap_mode(wifi_management *mgr,
    QObject *parent = nullptr):
    mode(mgr, WIFI_SOFTAP, parent) {};
  ~softap_mode();

  int set_mode_enable(bool enable);
  void save_config();

 public:
  static wifi_dot concfig;
};

class wifi_management : public QObject {
  Q_OBJECT

 public:
  static wifi_management *getInstance();
  void notes_init();
  int init_management(wifi_config_para_t *wifi_info);
  QString get_state_notes(wifi_state state);
  void mgr_proc_impl();
  wifi_mode get_wifi_mode();
  wifi_state get_wifi_state();
  int get_alive_connect(wifi_dot *dot);
  void update_alive_dot(wifi_dot *dot);
  void update_netid_list();
  QString get_alive_ssid();
  int get_scan_list(QList<wifi_dot> *list);
  void save_config();
  void notify_wifi_state_change(wifi_state state);
  void notify_wifi_scan();
  void request_scan_now();
  void get_wifi_para(wifi_config_para_t *param);

  /* wifi station api */
  void set_wifi_enabled(bool on);
  bool is_wifi_enabled();
  bool is_wifi_connected();
  int get_wifi_dot(wifi_dot *dot, QString ssid);
  int connect_network(wifi_dot *dot);
  int disable_network(wifi_dot *dot);
  int remove_network(wifi_dot *dot);
  void dhcp_handler(dhcp_status_t status);
  void net_wifi_event_cb(int event);
  int sync_set_wifi_enabled(bool on);
  int sync_get_scan_list(QList<wifi_dot> *list);
  int sync_connect_network(wifi_dot *dot);

  /* wifi softap api */
  void set_wifi_ap_config(const char *ssid,
    const char *pwd, SECURITY security);
  void set_wifi_ap_enabled(bool on);
  bool is_wifi_ap_enabled();

 signals:
  void signal_wifi_state_change(int state);
  void signal_wifi_scan();

 private:
  wifi_management &operator =(const wifi_management&);
  wifi_management(const wifi_management&);
  explicit wifi_management(QObject *parent = nullptr);
  int remove_network_imp(wifi_dot *dot);
  int disable_network_imp(wifi_dot *dot);

 private slots:
  void slot_scan_update();

 private:
  static wifi_management *instance;
  QMap<wifi_state, QString> state_notes;
  pthread_t mgr_th;
  pthread_mutex_t loop_lock;
  pthread_cond_t loop_cond;
  pthread_mutex_t alive_lock;
  QList<wifi_msg> loops;
  wifi_dot alive_dot;

  mode *cur_mode;
  wifi_state cur_state;
  QTimer *pTimer;
};

#endif  //  WIFI_MANAGEMENT_H
