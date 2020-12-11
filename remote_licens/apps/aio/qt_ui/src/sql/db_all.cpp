
/*
* db_all.cpp - define interface function of db by cpp
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
#define LOG_TAG "db_all"
#include <log/log.h>

#include "file_ops.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QMap>

#include "db_all.h"
#include "sqlapi.hpp"
#include "sql_head.hpp"
#include "person.hpp"
#include "checkin.hpp"
#include "exportcheckin.hpp"
#include "biitem.hpp"
#include "rule.hpp"

struct struct_sysinfo_table {
  // hardware parameters:
  const char* volume = "volume";
  const char* auto_display_brightness = "auto_display_brightness";
  const char* display_brightness = "display_brightness";
  const char* auto_screen_off = "auto_screen_off";
  const char* auto_screen_off_timeout = "auto_screen_off_timeout";
  const char* auto_screen_saver_off = "auto_screen_saver_off";
  const char* auto_screen_saver_timeout = "auto_screen_saver_timeout";
  const char* infrared_light_set = "infrared_light_set";
  const char* light_sensitive = "light_sensitive";
  const char* light_high_brightness = "light_high_brightness";
  const char* light_low_brightness = "light_low_brightness";
  const char* light_det_interval = "light_det_interval";
  const char* light_delay_time = "light_delay_time";
  const char* gpio = "gpio";
  const char* human_induction = "human_induction";

  // access parameter
  const char *wiegand_protocol = "wiegand_protocol";
  const char *door_sensor = "door_sensor";
  const char *door_sensor_delay = "door_sensor_delay";
  const char *alarm_switch = "alarm_switch";
  const char *access_time = "access_time";
  const char *access_type = "access_type";

  //  net parameters:
  const char* enable_eth = "enable_eth";
  const char* enable_eth_priority = "enable_eth_priority";
  const char* enable_eth_dhcp = "enable_eth_dhcp";
  const char* eth_ip = "eth_ip";
  const char* eth_mask = "eth_mask";
  const char* eth_gateway = "eth_gateway";
  const char* eth_dns = "eth_dns";
  const char* enable_wifi = "enable_wifi";
  const char* enable_hotspot = "enable_hotspot";
  const char* hotspot_name = "hotspot_name";
  const char* hotspot_password = "hotspot_password";

  // face recg parameters:
  const char* identify_closest_distance = "identify_closest_distance";
  const char* identify_furthest_distance = "identify_furthest_distance";
  const char* identify_threshold_score = "identify_threshold_score";
  const char* nir_live_detect = "nir_live_detect";
  const char* mask_threshold_value = "mask_threshold_value";
  const char* mask_enable = "mask_recg_enable";
  const char* recg_enable = "recg_enable";
  const char* living_detect_threshold_score = "living_detect_threshold_score";
  const char* mask_detect_enable = "mask_detect_enable";
  const char* infrared_image_enable = "infrared_image_enable";
  const char* recg_timeout = "recg_timeout";
  const char* repeat_recg_enable = "repeat_recg_enable";
  const char* recg_interval = "recg_interval";
  const char* recg_distance = "recg_distance";
  const char* voice_mode = "voice_mode";
  const char* grettings = "grettings";
  const char* stranger_voice = "stranger_voice";
  const char* qr_code_recg = "qr_code_recg";
  const char* body_temperature_detect = "body_temperature_detect";
  const char* temperature_compensation = "temperature_compensation";
  const char* alarm_temperature = "alarm_temperature";
  const char* exposure_values = "exposure_values";

  // system info parameters:
  const char* face_recg_alg_version = "face_recg_alg_version";
  const char* firmware_version = "firmware_version";
  const char* device_name = "device_name";

  // mode ui paramters:
  const char* rotate = "rotate";
  const char* mode = "mode";

  // web socket client parameters;
  const char* sn = "sn";
  const char* seed_secret_no = "seed_secret_no";
  const char* websocket_url = "websocket_url";
  const char* device_id = "device_id";
  const char* static_qrcode_url = "static_qrcode_url";
  const char* secret_no = "secret_no";
  const char* active_code = "active_code";
  const char* firmware_verson = "firmware_verson";
  const char* perm_version = "perm_version";
  const char* secret = "secret";
  const char* factory_test = "factory_test";
  const char* debug_switch = "debug_switch";

  // relay delay time parameter
  const char* delay_time = "delay_time";

  // development status parameter
  const char* development = "development";

  // password of enter main menu
  const char *password = "password";

  const char *saved_attendance_pic = "saved_attendance_pic";
  const char *avoid_attendance_step = "avoid_attendance_step";
  const char *access_records = "access_records";

  const char *auto_set_time = "auto_set_time";
  const char *auto_set_timezone = "auto_set_timezone";
  const char *lang = "lang";
  const char *timezone = "timezone";
  const char *rtsp_video_set = "rtsp_video";
  const char *video_type_set = "video_type";

  // platform_enable of enter main menu
  const char *platform_enable = "platform_enable";
  // platform_ip of enter main menu
  const char *platform_ip = "platform_ip";

  // main ui setting param
  const char *company_name = "company_name";
  const char *border_color = "border_color";
  const char *name_display = "name_display";
  const char *name_format = "name_format";
  const char *mac_display = "mac_display";
  const char *ip_display = "ip_display";
  const char *registered_count_display = "registered_count_display";

  // auto reboot param
  const char *reboot_interval = "reboot_interval";
  const char *reboot_time = "reboot_time";

  // check in param
  const char *save_large_pic = "save_large_pic";
  const char *save_small_pic = "save_small_pic";
  const char *save_stranger_record = "save_stranger_record";
};

struct struct_sysinfo_table sysinfo_table;
static SqlApi* sql;

/*****************************************
 * sys-info related
 *
 ****************************************/
static QMap<QString, int> sysinfo_map;

/**
 * db_all_init - database system parameter initialize.
 *
 * Returns none.
 */
void db_all_init() {
  sql = SqlApi::Instance();

  sysinfo_map["volume"] = 0;
  sysinfo_map["auto_display_brightness"] = 1;
  sysinfo_map["display_brightness"] = 2;
  sysinfo_map["auto_screen_off"] = 3;
  sysinfo_map["auto_screen_off_timeout"] = 4;
  sysinfo_map["auto_screen_saver_timeout"] = 5;
  sysinfo_map["infrared_light_set"] = 6;
  sysinfo_map["light_sensitive"] = 7;
  sysinfo_map["light_high_brightness"] = 8;
  sysinfo_map["light_low_brightness"] = 9;
  sysinfo_map["light_det_interval"] = 10;
  sysinfo_map["light_delay_time"] = 11;
  sysinfo_map["gpio"] = 12;
  sysinfo_map["human_induction"] = 13;
  sysinfo_map["auto_screen_saver_off"] = 14;

  sysinfo_map["wiegand_protocol"] = 15;
  sysinfo_map["door_sensor"] = 16;
  sysinfo_map["door_sensor_delay"] = 17;
  sysinfo_map["alarm_switch"] = 18;
  sysinfo_map["access_time"] = 19;

  sysinfo_map["platform_enable"] = 20;
  sysinfo_map["platform_ip"] = 21;

  sysinfo_map["access_type"] = 22;

  sysinfo_map["enable_eth"] = 100;
  sysinfo_map["enable_eth_priority"] = 101;
  sysinfo_map["enable_eth_dhcp"] = 102;
  sysinfo_map["eth_ip"] = 103;
  sysinfo_map["eth_mask"] = 104;
  sysinfo_map["eth_gateway"] = 105;
  sysinfo_map["eth_dns"] = 106;
  sysinfo_map["enable_wifi"] = 107;
  sysinfo_map["enable_hotspot"] = 108;
  sysinfo_map["hotspot_name"] = 109;
  sysinfo_map["hotspot_password"] = 110;

  sysinfo_map["identify_closest_distance"] = 200;
  sysinfo_map["identify_furthest_distance"] = 201;
  sysinfo_map["identify_threshold_score"] = 202;
  sysinfo_map["nir_live_detect"] = 203;
  sysinfo_map["mask_threshold_value"] = 204;
  sysinfo_map["mask_recg_enable"] = 205;
  sysinfo_map["recg_enable"] = 206;
  sysinfo_map["living_detect_threshold_score"] = 207;
  sysinfo_map["mask_detect_enable"] = 208;
  sysinfo_map["infrared_image_enable"] = 209;
  sysinfo_map["recg_timeout"] = 210;
  sysinfo_map["repeat_recg_enable"] = 211;
  sysinfo_map["recg_interval"] = 212;
  sysinfo_map["recg_distance"] = 213;
  sysinfo_map["voice_mode"] = 214;
  sysinfo_map["grettings"] = 215;
  sysinfo_map["stranger_voice"] = 216;
  sysinfo_map["qr_code_recg"] = 217;
  sysinfo_map["body_temperature_detect"] = 218;
  sysinfo_map["temperature_compensation"] = 219;
  sysinfo_map["alarm_temperature"] = 220;
  sysinfo_map["exposure_values"] = 221;

  sysinfo_map["face_recg_alg_version"] = 300;
  sysinfo_map["firmware_version"] = 301;
  sysinfo_map["device_name"] = 302;

  sysinfo_map["sn"] = 400;
  sysinfo_map["seed_secret_no"] = 401;
  sysinfo_map["websocket_url"] = 402;
  sysinfo_map["device_id"] = 403;
  sysinfo_map["static_qrcode_url"] = 404;
  sysinfo_map["secret_no"] = 405;
  sysinfo_map["active_code"] = 406;
  sysinfo_map["firmware_verson"] = 407;
  sysinfo_map["perm_version"] = 408;
  sysinfo_map["secret"] = 409;

  sysinfo_map["mode"] = 500;
  sysinfo_map["rotate"] = 501;

  sysinfo_map["factory_test"] = 600;
  sysinfo_map["debug_switch"] = 601;

  sysinfo_map["delay_time"] = 700;
  sysinfo_map["development"] = 701;
  sysinfo_map["password"] = 702;

  sysinfo_map["saved_attendance_pic"] = 703;
  sysinfo_map["avoid_attendance_step"] = 704;
  sysinfo_map["access_records"] = 705;

  sysinfo_map["auto_set_time"] = 800;
  sysinfo_map["auto_set_timezone"] = 801;
  sysinfo_map["timezone"] = 802;

  sysinfo_map["lang"] = 900;
  sysinfo_map["rtsp_video"] = 950;
  sysinfo_map["video_type"] = 951;

  sysinfo_map["company_name"] = 1000;
  sysinfo_map["border_color"] = 1001;
  sysinfo_map["name_display"] = 1002;
  sysinfo_map["name_format"] = 1003;
  sysinfo_map["mac_display"] = 1004;
  sysinfo_map["ip_display"] = 1005;
  sysinfo_map["registered_count_display"] = 1006;

  sysinfo_map["reboot_interval"] = 1100;
  sysinfo_map["reboot_time"] = 1101;

  sysinfo_map["save_large_pic"] = 1200;
  sysinfo_map["save_small_pic"] = 1201;
  sysinfo_map["save_stranger_record"] = 1202;

  sysinfo_map["default"] = INT_MAX;
}

void db_deinit() {
  if (sql) {
    delete sql;
    sql = NULL;
  }
}

/**
 * db_device_hw_param_update - Update device hardware information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_hw_param_update(device_hw_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.volume, p_info->volume},
    bi_item_t{sysinfo_table.auto_display_brightness,
      p_info->auto_display_brightness},
    bi_item_t{sysinfo_table.display_brightness, p_info->display_brightness},
    bi_item_t{sysinfo_table.auto_screen_off, p_info->auto_screen_off},
    bi_item_t{sysinfo_table.auto_screen_off_timeout,
      p_info->auto_screen_off_timeout},
    bi_item_t{sysinfo_table.auto_screen_saver_off,
      p_info->auto_screen_saver_off},
    bi_item_t{sysinfo_table.auto_screen_saver_timeout,
      p_info->auto_screen_saver_timeout},
    bi_item_t{sysinfo_table.infrared_light_set, p_info->infrared_light_set},
    bi_item_t{sysinfo_table.light_sensitive, p_info->light_sensitive},
    bi_item_t{sysinfo_table.light_high_brightness,
      p_info->light_high_brightness},
    bi_item_t{sysinfo_table.light_low_brightness, p_info->light_low_brightness},
    bi_item_t{sysinfo_table.light_det_interval, p_info->light_det_interval},
    bi_item_t{sysinfo_table.light_delay_time, p_info->light_delay_time},
    bi_item_t{sysinfo_table.gpio, p_info->gpio},
    bi_item_t{sysinfo_table.human_induction, p_info->human_induction},
    bi_item_t{sysinfo_table.wiegand_protocol, p_info->wiegand_protocol},
    bi_item_t{sysinfo_table.door_sensor, p_info->door_sensor},
    bi_item_t{sysinfo_table.door_sensor_delay, p_info->door_sensor_delay},
    bi_item_t{sysinfo_table.alarm_switch, p_info->alarm_switch},
    bi_item_t{sysinfo_table.access_type, p_info->access_type}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_device_hw_param_query - Query device hardware information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_hw_param_query(device_hw_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.volume)
      .query(sysinfo_table.auto_display_brightness)
      .query(sysinfo_table.display_brightness)
      .query(sysinfo_table.auto_screen_off)
      .query(sysinfo_table.auto_screen_off_timeout)
      .query(sysinfo_table.auto_screen_saver_off)
      .query(sysinfo_table.auto_screen_saver_timeout)
      .query(sysinfo_table.infrared_light_set)
      .query(sysinfo_table.light_sensitive)
      .query(sysinfo_table.light_high_brightness)
      .query(sysinfo_table.light_low_brightness)
      .query(sysinfo_table.light_det_interval)
      .query(sysinfo_table.light_delay_time)
      .query(sysinfo_table.gpio)
      .query(sysinfo_table.human_induction)
      .query(sysinfo_table.device_name)
      .query(sysinfo_table.wiegand_protocol)
      .query(sysinfo_table.door_sensor)
      .query(sysinfo_table.door_sensor_delay)
      .query(sysinfo_table.alarm_switch)
      .query(sysinfo_table.access_time)
      .query(sysinfo_table.access_type);
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      //  qDebug() << item._key;
      switch (sysinfo_map[QString(item._key)]) {
        case 0:
          p_info->volume = static_cast<char>(atoi(item._value));
          break;
        case 1:
            p_info->auto_display_brightness =
                static_cast<char>(atoi(item._value));
          break;
        case 2:
          p_info->display_brightness = static_cast<char>(atoi(item._value));
          break;
        case 3:
          p_info->auto_screen_off = static_cast<char>(atoi(item._value));
          break;
          /*int*/
        case 4:
          p_info->auto_screen_off_timeout = QString(item._value).toInt();
          break;
          /*int*/
        case 5:
          p_info->auto_screen_saver_timeout = QString(item._value).toInt();
          break;
        case 6:
          p_info->infrared_light_set = static_cast<char>(atoi(item._value));
          break;
        case 7:
          p_info->light_sensitive = static_cast<char>(atoi(item._value));
          break;
          /*int*/
        case 8:
          p_info->light_high_brightness = QString(item._value).toInt();
          break;
          /*int*/
        case 9:
          p_info->light_low_brightness = QString(item._value).toInt();
          break;
        case 10:
          p_info->light_det_interval = static_cast<char>(atoi(item._value));
          break;
        case 11:
          p_info->light_delay_time = static_cast<char>(atoi(item._value));
          break;
          /*int*/
        case 12:
          p_info->gpio = QString(item._value).toInt();
          break;
        case 13:
          p_info->human_induction = static_cast<char>(atoi(item._value));
          break;
        case 14:
          p_info->auto_screen_saver_off = static_cast<char>(atoi(item._value));
          break;
        case 15:
          p_info->wiegand_protocol = static_cast<char>(atoi(item._value));
          break;
        case 16:
          p_info->door_sensor = static_cast<char>(atoi(item._value));
          break;
        case 17:
          p_info->door_sensor_delay = static_cast<char>(atoi(item._value));
          break;
        case 18:
          p_info->alarm_switch = static_cast<char>(atoi(item._value));
        case 19:
          memcpy(p_info->access_time, item._value, 128);
          break;
        case 22:
          p_info->access_type = static_cast<char>(atoi(item._value));
          break;
        case 302:
          memcpy(p_info->device_name, item._value, 32);
          break;

        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_auto_reboot_param_update - Update auto reboot information.
 * @p_info: A pointer to auto_reboot_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_auto_reboot_param_update(auto_reboot_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.reboot_interval, p_info->reboot_interval},
    bi_item_t{sysinfo_table.reboot_time, p_info->reboot_time},
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert auto reboot param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_auto_reboot_param_query - Query auto reboot information.
 * @p_info: A pointer to auto_reboot_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_auto_reboot_param_query(auto_reboot_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.reboot_interval)
      .query(sysinfo_table.reboot_time);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        case 1100:
          p_info->reboot_interval = static_cast<char>(atoi(item._value));
          break;
        case 1101:
          memcpy(p_info->reboot_time, item._value, 16);
          break;

        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_main_setting_param_update - Update main ui information.
 * @p_info: A pointer to main_setting_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_main_setting_param_update(main_setting_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.company_name, p_info->company_name},
    bi_item_t{sysinfo_table.device_name, p_info->device_name},
    bi_item_t{sysinfo_table.border_color, p_info->face_border_color},
    bi_item_t{sysinfo_table.name_display, p_info->name_display},
    bi_item_t{sysinfo_table.name_format, p_info->name_format},
    bi_item_t{sysinfo_table.mac_display, p_info->mac_display},
    bi_item_t{sysinfo_table.ip_display, p_info->ip_display},
    bi_item_t{sysinfo_table.registered_count_display,
        p_info->register_count_display}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_main_setting_param_query - Query main ui information.
 * @p_info: A pointer to main_setting_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_main_setting_param_query(main_setting_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.company_name)
      .query(sysinfo_table.device_name)
      .query(sysinfo_table.border_color)
      .query(sysinfo_table.name_display)
      .query(sysinfo_table.name_format)
      .query(sysinfo_table.mac_display)
      .query(sysinfo_table.ip_display)
      .query(sysinfo_table.registered_count_display);
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        case 1000:
          memcpy(p_info->company_name, item._value, 127);
          break;
        case 302:
          memcpy(p_info->device_name, item._value, 127);
          break;
        case 1001:
          memcpy(p_info->face_border_color, item._value, 16);
          break;
        case 1002:
          p_info->name_display = static_cast<char>(atoi(item._value));
          break;
        case 1003:
          p_info->name_format = static_cast<char>(atoi(item._value));
          break;
        case 1004:
          p_info->mac_display = static_cast<char>(atoi(item._value));
          break;
        case 1005:
          p_info->ip_display = static_cast<char>(atoi(item._value));
          break;
        case 1006:
          p_info->register_count_display = static_cast<char>(atoi(item._value));
          break;

        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_device_time_param_update - Update device time information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_time_param_update(device_time_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.auto_set_time, p_info->auto_set_time},
    bi_item_t{sysinfo_table.auto_set_timezone,
      p_info->auto_set_timezone},
    bi_item_t{sysinfo_table.timezone, p_info->timezone}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_device_time_param_query - Query device time information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_time_param_query(device_time_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.auto_set_time)
      .query(sysinfo_table.auto_set_timezone)
      .query(sysinfo_table.timezone);
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      //  qDebug() << item._key;
      switch (sysinfo_map[QString(item._key)]) {
        case 800:
          p_info->auto_set_time = static_cast<char>(atoi(item._value));
          break;
        case 801:
            p_info->auto_set_timezone =
                static_cast<char>(atoi(item._value));
        case 802:
                memcpy(p_info->timezone, item._value, 32);
          break;
        default:
          break;
      }
    }
  }

  return 0;
}


/**
 * db_device_lang_param_update - Update device language information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_lang_param_update(device_lang_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.lang, p_info->lang},
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert lang param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_device_lang_param_query - Query device language information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_lang_param_query(device_lang_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.lang);
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      //  qDebug() << item._key;
      switch (sysinfo_map[QString(item._key)]) {
        case 900:
          p_info->lang = static_cast<char>(atoi(item._value));
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_device_lang_param_update - Update device language information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_video_param_update(device_video_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.rtsp_video_set, p_info->rtsp_video_set},
    bi_item_t{sysinfo_table.video_type_set, p_info->video_type_set}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert lang param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_device_lang_param_query - Query device language information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_video_param_query(device_video_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.rtsp_video_set)
        .query(sysinfo_table.video_type_set);
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      //  qDebug() << item._key;
      switch (sysinfo_map[QString(item._key)]) {
        case 950:
          p_info->rtsp_video_set = static_cast<char>(atoi(item._value));
          break;
        case 951:
          p_info->video_type_set = static_cast<char>(atoi(item._value));
          break;
        default:
          break;
      }
    }
  }

  return 0;
}


/**
 * device_net_param_print - Display information in a queue for
 * debugging purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void device_net_param_print(device_net_param_t* p_info) {
}

/**
 * db_device_net_param_update - Update cloud platform information.
 * @p_info: A pointer to device_net_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_net_param_update(device_net_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.enable_eth, p_info->enable_eth},
    bi_item_t{sysinfo_table.enable_eth_priority, p_info->enable_eth_priority},
    bi_item_t{sysinfo_table.enable_eth_dhcp, p_info->enable_eth_dhcp},
    bi_item_t{sysinfo_table.eth_ip, p_info->eth_ip},
    bi_item_t{sysinfo_table.eth_mask, p_info->eth_mask},
    bi_item_t{sysinfo_table.eth_gateway, p_info->eth_gateway},
    bi_item_t{sysinfo_table.eth_dns, p_info->eth_dns},
    bi_item_t{sysinfo_table.enable_wifi, p_info->enable_wifi},
    bi_item_t{sysinfo_table.enable_hotspot, p_info->enable_hotspot},
    bi_item_t{sysinfo_table.hotspot_name, p_info->hotspot_name},
    bi_item_t{sysinfo_table.hotspot_password, p_info->hotspot_password}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_device_net_param_query - Query cloud platform information.
 * @p_info: A pointer to device_net_param_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_device_net_param_query(device_net_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.enable_eth)
      .query(sysinfo_table.enable_eth_priority)
      .query(sysinfo_table.enable_eth_dhcp)
      .query(sysinfo_table.eth_ip)
      .query(sysinfo_table.eth_mask)
      .query(sysinfo_table.eth_gateway)
      .query(sysinfo_table.eth_dns)
      .query(sysinfo_table.enable_wifi)
      .query(sysinfo_table.enable_hotspot)
      .query(sysinfo_table.hotspot_name)
      .query(sysinfo_table.hotspot_password);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        case 100:
          p_info->enable_eth =
              static_cast<char>(atoi(item._value));
          break;
        case 101:
          p_info->enable_eth_priority =
              static_cast<char>(atoi(item._value));
          break;
        case 102:
          p_info->enable_eth_dhcp =
              static_cast<char>(atoi(item._value));
          break;
          /*char256*/case 103:
          memcpy(p_info->eth_ip, item._value, MAX_VAL_LEN);
          break;
          /*char256*/case 104:
          memcpy(p_info->eth_mask, item._value, MAX_VAL_LEN);
          break;
          /*char256*/case 105:
          memcpy(p_info->eth_gateway, item._value, MAX_VAL_LEN);
          break;
          /*char256*/case 106:
          memcpy(p_info->eth_dns, item._value, MAX_VAL_LEN);
          break;
        case 107:
          p_info->enable_wifi = static_cast<char>(atoi(item._value));
          break;
        case 108:
          p_info->enable_hotspot = static_cast<char>(atoi(item._value));
          break;
          /*char32*/ case 109:
          memcpy(p_info->hotspot_name, item._value, WIFI_NAME_LEN);
          break;
          /*char9*/ case 110:
          memcpy(p_info->hotspot_password, item._value, WIFI_PWD_LEN);
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

// face recg related
/**
 * face_recognition_param_print - Display information in a queue for debugging
 * purposes.
 * @h:  A pointer to the head of the queue.
 *
 * Returns None.
 */
void face_recognition_param_print(face_recognition_param_t* p_info) {
}

/**
 * db_face_recognition_param_update - Update face recognition param information.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_recognition_param_update(face_recognition_param_t* p_info) {
  QList<SqlBase*> item_list;
  ALOGD("update threshold_score=%f\n", p_info->identify_threshold_score);
  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.identify_closest_distance,
      p_info->identify_closest_distance},
    bi_item_t{sysinfo_table.identify_furthest_distance,
      p_info->identify_furthest_distance},
    bi_item_t{sysinfo_table.identify_threshold_score,
      p_info->identify_threshold_score},
    bi_item_t{sysinfo_table.nir_live_detect, p_info->nir_live_detect},
    bi_item_t{sysinfo_table.mask_threshold_value,
      p_info->mask_threshold_value},
    bi_item_t{sysinfo_table.mask_enable, p_info->mask_enable},
    bi_item_t{sysinfo_table.recg_enable, p_info->recg_enable},
    bi_item_t{sysinfo_table.living_detect_threshold_score,
      p_info->living_detect_threshold_score},
    bi_item_t{sysinfo_table.mask_detect_enable,
      p_info->mask_detect_enable},
    bi_item_t{sysinfo_table.infrared_image_enable,
      p_info->infrared_image_enable},
    bi_item_t{sysinfo_table.recg_timeout, p_info->recg_timeout},
    bi_item_t{sysinfo_table.repeat_recg_enable, p_info->repeat_recg_enable},
    bi_item_t{sysinfo_table.recg_interval, p_info->recg_interval},
    bi_item_t{sysinfo_table.recg_distance, p_info->recg_distance},
    bi_item_t{sysinfo_table.voice_mode, p_info->voice_mode},
    bi_item_t{sysinfo_table.grettings, p_info->grettings},
    bi_item_t{sysinfo_table.stranger_voice, p_info->stranger_voice},
    bi_item_t{sysinfo_table.qr_code_recg, p_info->qr_code_recg},
    bi_item_t{sysinfo_table.body_temperature_detect,
      p_info->body_temperature_detect},
    bi_item_t{sysinfo_table.temperature_compensation,
      p_info->temperature_compensation},
    bi_item_t{sysinfo_table.alarm_temperature, p_info->alarm_temperature},
    bi_item_t{sysinfo_table.exposure_values, p_info->exposure_values}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_face_recognition_param_query - Query face recognition param.
 * @p_info: A pointer to cloud_platform_t.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_face_recognition_param_query(face_recognition_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.identify_closest_distance)
      .query(sysinfo_table.identify_furthest_distance)
      .query(sysinfo_table.identify_threshold_score)
      .query(sysinfo_table.nir_live_detect)
      .query(sysinfo_table.mask_threshold_value)
      .query(sysinfo_table.mask_enable)
      .query(sysinfo_table.recg_enable)
      .query(sysinfo_table.living_detect_threshold_score)
      .query(sysinfo_table.mask_detect_enable)
      .query(sysinfo_table.infrared_image_enable)
      .query(sysinfo_table.recg_timeout)
      .query(sysinfo_table.repeat_recg_enable)
      .query(sysinfo_table.recg_interval)
      .query(sysinfo_table.recg_distance)
      .query(sysinfo_table.voice_mode)
      .query(sysinfo_table.grettings)
      .query(sysinfo_table.stranger_voice)
      .query(sysinfo_table.qr_code_recg)
      .query(sysinfo_table.body_temperature_detect)
      .query(sysinfo_table.temperature_compensation)
      .query(sysinfo_table.alarm_temperature)
      .query(sysinfo_table.exposure_values);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
          /*float*/
        case 200:
          p_info->identify_closest_distance =
              QString(item._value).toFloat();
          break;
          /*float*/
        case 201:
          p_info->identify_furthest_distance =
              QString(item._value).toFloat();
          break;
          /*float*/
        case 202:
          p_info->identify_threshold_score =
              QString(item._value).toFloat();
          ALOGD("threshold_score =%f\n", p_info->identify_threshold_score);
          break;
          /*char*/
        case 203:
          p_info->nir_live_detect = static_cast<char>(atoi(item._value));
          ALOGD("nir_live_detect =%d\n", p_info->nir_live_detect);
          break;
          /*float*/
        case 204:
          p_info->mask_threshold_value =
              QString(item._value).toFloat();
          ALOGD("mask_threshold_score =%f\n", p_info->mask_threshold_value);
          break;
          /*char*/
        case 205:
          p_info->mask_enable = static_cast<char>(atoi(item._value));
          ALOGD("mask_enable =%d\n", p_info->mask_enable);
          break;
          /*char*/
        case 206:
          p_info->recg_enable = static_cast<char>(atoi(item._value));
          break;
          /*float*/
        case 207:
          p_info->living_detect_threshold_score =
              QString(item._value).toFloat();
          break;
          /*char*/
        case 208:
          p_info->mask_detect_enable = static_cast<char>(atoi(item._value));
          break;
          /*char*/
        case 209:
          p_info->infrared_image_enable = static_cast<char>(atoi(item._value));
          break;
          /*int*/
        case 210:
          p_info->recg_timeout = QString(item._value).toInt();
          break;
          /*char*/
        case 211:
          p_info->repeat_recg_enable = static_cast<char>(atoi(item._value));
          break;

          /*int*/
        case 212:
          p_info->recg_interval = QString(item._value).toInt();
          break;
          /*int*/
        case 213:
          p_info->recg_distance = QString(item._value).toInt();
          break;
          /*int*/
        case 214:
          p_info->voice_mode = QString(item._value).toInt();
          break;
          /*char*/
        case 215:
          p_info->grettings = static_cast<char>(atoi(item._value));
          break;
          /*int*/
        case 216:
          p_info->stranger_voice = QString(item._value).toInt();
          break;
          /*char*/
        case 217:
          p_info->qr_code_recg = static_cast<char>(atoi(item._value));
          break;
          /*char*/
        case 218:
          p_info->body_temperature_detect =
            static_cast<char>(atoi(item._value));
          break;
          /*float*/
        case 219:
          p_info->temperature_compensation = QString(item._value).toFloat();
          break;
          /*float*/
        case 220:
          p_info->alarm_temperature = QString(item._value).toFloat();
          break;
          /*int*/
        case 221:
          p_info->exposure_values = QString(item._value).toInt();
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_ui_get_value - get value of key information
 *
 * Returns 0 if success, otherwise failure.
 */
int db_ui_get_value(const char *key, char *value, int size) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(key);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      memcpy(value, item._value, size);
    }
  }

  return 0;
}

/**
 * db_ui_set_value - set value of key information
 * @p_info: A pointer to device_system_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_ui_set_value(const char *key, char *value) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{key, value}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_device_system_info_update - Update system info parameter.
 * @p_info: A pointer to device_system_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_device_system_info_update(device_system_info_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.face_recg_alg_version,
      p_info->face_recg_alg_version},
    bi_item_t{sysinfo_table.firmware_version, p_info->firmware_version},
    bi_item_t{sysinfo_table.device_name, p_info->device_name}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_device_system_info_query - Query system info parameter.
 * @p_info: A pointer to device_system_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_device_system_info_query(device_system_info_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.face_recg_alg_version)
      .query(sysinfo_table.firmware_version)
      .query(sysinfo_table.device_name);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        /*char32*/ case 300:
          memcpy(p_info->face_recg_alg_version, item._value, 32);
          break;
          /*char32*/ case 301:
          memcpy(p_info->firmware_version, item._value, 32);
          break;
          /*char32*/ case 302:
          memcpy(p_info->device_name, item._value, 32);
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_device_platform_info_update - Update platform info parameter.
 * @p_info: A pointer to device_platform_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_device_platform_info_update(device_platform_info_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.platform_enable,
      p_info->platform_enable},
    bi_item_t{sysinfo_table.platform_ip, p_info->platform_ip}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_device_platform_info_query - Query platform info parameter.
 * @p_info: A pointer to device_platform_info_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_device_platform_info_query(device_platform_info_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.platform_enable)
      .query(sysinfo_table.platform_ip);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        /*char32*/ case 20:
          p_info->platform_enable = QString(item._value).toInt();
          break;
          /*char64*/ case 21:
          memcpy(p_info->platform_ip, item._value, 64);
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_mode_ui_param_update - Update ui mode parameter.
 * @p_info: A pointer to mode_ui_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_mode_ui_param_update(mode_ui_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.mode, p_info->mode},
    bi_item_t{sysinfo_table.rotate, p_info->rotate}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_mode_ui_param_query - Query ui mode parameter.
 * @p_info: A pointer to mode_ui_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_mode_ui_param_query(mode_ui_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.rotate)
      .query(sysinfo_table.mode);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        /*int*/ case 500:
          p_info->mode = QString(item._value).toInt();
          break;
          /*int*/ case 501:
          p_info->rotate = QString(item._value).toInt();
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * ui_device_set_attendance_param - Query attendance parameter.
 * @p_info: A pointer to attendance_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int ui_device_set_attendance_param(attendance_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.saved_attendance_pic,
    p_info->saved_attendance_pic},
    bi_item_t{sysinfo_table.avoid_attendance_step,
    p_info->avoid_attendance_step},
    bi_item_t{sysinfo_table.access_records,
    p_info->access_records}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * ui_device_get_attendance_param - Query attendance parameter.
 * @p_info: A pointer to attendance_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int ui_device_get_attendance_param(attendance_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.saved_attendance_pic)
      .query(sysinfo_table.avoid_attendance_step)
      .query(sysinfo_table.access_records);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        /*int*/ case 703:
          p_info->saved_attendance_pic =
      QString(item._value).toInt();
          break;
          /*int*/ case 704:
          p_info->avoid_attendance_step =
      QString(item._value).toInt();
          break;
          /*int*/ case 705:
          p_info->access_records = QString(item._value).toInt();
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * ui_device_set_checkin_param - Query checkin parameter.
 * @p_info: A pointer to checkin_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int ui_device_set_checkin_param(checkin_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.save_large_pic,
    p_info->save_large_pic},
    bi_item_t{sysinfo_table.save_small_pic,
    p_info->save_small_pic},
    bi_item_t{sysinfo_table.save_stranger_record,
    p_info->save_stranger_record}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert checkin param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * ui_device_get_checkin_param - Query checkin parameter.
 * @p_info: A pointer to checkin_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int ui_device_get_checkin_param(checkin_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.save_large_pic)
      .query(sysinfo_table.save_small_pic)
      .query(sysinfo_table.save_stranger_record);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        /*int*/
        case 1200:
          p_info->save_large_pic = QString(item._value).toInt();
          break;
        /*int*/
        case 1201:
          p_info->save_small_pic = QString(item._value).toInt();
          break;
        /*int*/
        case 1202:
          p_info->save_stranger_record = QString(item._value).toInt();
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_websocket_client_param_update - Update websocket client parameter.
 * @p_info: A pointer to websocket_client_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_websocket_client_param_update(websocket_client_param_t* p_info) {
  QList<SqlBase*> item_list;

  QVector<bi_item_t> s{
    bi_item_t{sysinfo_table.sn, p_info->sn},
    bi_item_t{sysinfo_table.seed_secret_no, p_info->seed_secret_no},
    bi_item_t{sysinfo_table.websocket_url, p_info->websocket_url},
    bi_item_t{sysinfo_table.device_id, p_info->device_id},
    bi_item_t{sysinfo_table.static_qrcode_url, p_info->static_qrcode_url},
    bi_item_t{sysinfo_table.secret_no, p_info->secret_no},
    bi_item_t{sysinfo_table.secret, p_info->secret},
    bi_item_t{sysinfo_table.active_code, p_info->active_code},
    bi_item_t{sysinfo_table.firmware_verson, p_info->firmware_verson},
    bi_item_t{sysinfo_table.perm_version, p_info->perm_version}
  };

  for (auto elem : s) {
    Sysinfo* tmp = new Sysinfo(elem);
    item_list.push_back(tmp);
  }
  int cnt = sql->updateBind(item_list);

  if (cnt != 0)
    ALOGE("%s: insert hw param error\n", __func__);
  for (auto elem : item_list) {
    delete elem;
  }

  return 0;
}

/**
 * db_websocket_client_param_query - Query websocket client parameter.
 * @p_info: A pointer to websocket_client_param_t.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_websocket_client_param_query(websocket_client_param_t* p_info) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.sn)
      .query(sysinfo_table.seed_secret_no)
      .query(sysinfo_table.websocket_url)
      .query(sysinfo_table.device_id)
      .query(sysinfo_table.static_qrcode_url)
      .query(sysinfo_table.secret_no)
      .query(sysinfo_table.secret)
      .query(sysinfo_table.active_code)
      .query(sysinfo_table.firmware_verson)
      .query(sysinfo_table.perm_version);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (sysinfo_map[QString(item._key)]) {
        /*char64*/ case 400:
          memcpy(p_info->sn, item._value, 64);
          break;
          /*char64*/ case 401:
          memcpy(p_info->seed_secret_no, item._value, 64);
          break;
          /*char64*/ case 402:
          memcpy(p_info->websocket_url, item._value, 64);
          break;
          /*char64*/ case 403:
          memcpy(p_info->device_id, item._value, 64);
          break;
          /*char64*/ case 404:
          memcpy(p_info->static_qrcode_url, item._value, 128);
          break;
          /*char64*/ case 405:
          memcpy(p_info->secret_no, item._value, 64);
          break;
          /*char64*/ case 406:
          memcpy(p_info->active_code, item._value, 64);
          break;
          /*char32*/ case 407:
          memcpy(p_info->firmware_verson, item._value, 32);
          break;
          /*int*/ case 408:
          p_info->perm_version = QString(item._value).toInt();
          break;
          /*char64*/ case 409:
          memcpy(&p_info->secret, item._value, 64);
          break;
        default:
          break;
      }
    }
  }

  return 0;
}

/**
 * db_perm_version_update - Update perm version parameter.
 * @version: perm version.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_perm_version_update(int version) {
  bi_item_t item(sysinfo_table.perm_version, version);

  Sysinfo s_info(item);

  return sql->updateBind(s_info);
}

/**
 * db_perm_version_query - Query perm version parameter.
 * @version: perm version.
 *
 * Returns perm version.
 */
int db_perm_version_query(void) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.perm_version);

  int perm_version = 0;
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      perm_version = QString(item._value).toInt();
    }
  }

  return perm_version;
}

/**
 * db_perm_factory_test_update - Update factory test flag value.
 * @bit_value: A 32bit value for restore factory test flag.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_factory_test_update(int bit_value) {
  bi_item_t item(sysinfo_table.factory_test, bit_value);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_perm_factory_test_query - query factory test flag value.
 *
 * Returns factory test flag value. if error, this value will
 * be -1;
 */
int db_factory_test_query(void) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.factory_test);

  int factory_test_value = -1;
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      factory_test_value = QString(item._value).toInt();
    } else {
      factory_test_value = -1;
    }
  }

  return factory_test_value;
}

/**
 * db_debug_switch_update - Update debug mode switch value.
 * @mode: The debug mode value.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_debug_switch_update(int mode) {
  bi_item_t item(sysinfo_table.debug_switch, mode);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_debug_switch_query - query debug mode value.
 *
 * Returns debug switch value. if error, this value will
 * be -1;
 */
int db_debug_switch_query(void) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.debug_switch);

  int mode = -1;
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      mode = QString(item._value).toInt();
    } else {
      mode = -1;
    }
  }

  return mode;
}

/**
 * db_delay_time_update - Update delay time value.
 * @mode: The relay time value.
 *
 * Returns 0 if success, otherwise -1.
 */ //delay_time
int db_delay_time_update(int relay_time) {
  bi_item_t item(sysinfo_table.delay_time, relay_time);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_delay_time_query - query delay time value.
 *
 * Returns relay time value. if error, this value will
 * be -1;
 */
int db_delay_time_query(void) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.delay_time);

  int delay_time = -1;
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      delay_time = QString(item._value).toInt();
    } else {
      delay_time = -1;
    }
  }

  return delay_time;
}

/**
 * db_wiegand_protocol_update - Update wiegand protocol value.
 * @wiegand_protocol: The wiegand protocol value.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_wiegand_protocol_update(int wiegand_protocol) {
  bi_item_t item(sysinfo_table.wiegand_protocol, wiegand_protocol);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_wiegand_protocol_query - query wiegand protocol.
 *
 * Returns wiegand data size value. if error, this value will
 * be -1;
 */
int db_wiegand_protocol_query(void) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.wiegand_protocol);

  int data_size = -1;
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      data_size = QString(item._value).toInt();
    } else {
      data_size = -1;
    }
  }

  return data_size;
}

/**
 * db_door_sensor_type_update - Update wiegand protocol value.
 * @door_sensor_type: The door sensor type.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_door_sensor_type_update(int door_sensor_type) {
  bi_item_t item(sysinfo_table.door_sensor, door_sensor_type);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_door_sensor_type_query - query door sensor type.
 *
 * Returns door sensor type. if error, this value will
 * be -1;
 */
int db_door_sensor_type_query(void) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.door_sensor);

  int door_sensor = -1;
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      door_sensor = QString(item._value).toInt();
    } else {
      door_sensor = -1;
    }
  }

  return door_sensor;
}

/**
 * db_door_sensor_delay_update - Update door sensor delay time.
 * @delay_time: The door sensor delay time.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_door_sensor_delay_update(int delay_time) {
  bi_item_t item(sysinfo_table.door_sensor_delay, delay_time);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_door_sensor_delay_query - query door sensor delay.
 *
 * Returns door sensor delay. if error, this value will
 * be -1;
 */
int db_door_sensor_delay_query(void) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.door_sensor_delay);

  int door_sensor_delay = -1;
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      door_sensor_delay = QString(item._value).toInt();
    } else {
      door_sensor_delay = -1;
    }
  }

  return door_sensor_delay;
}

/**
 * db_alarm_switch_update - Update alarm switch.
 * @alarm_switch: The alarm switch.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_alarm_switch_update(int alarm_switch) {
  bi_item_t item(sysinfo_table.alarm_switch, alarm_switch);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_access_type_query - query access type.
 *
 * Returns access type.
 */
int db_access_type_query() {
  int access_type;
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.access_type);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      access_type = QString(item._value).toInt();
    }
  }

  return access_type;
}

/**
 * db_access_time_query - query access time.
 *
 * Returns 0 if success, otherwise failure.
 */
int db_access_time_query(char *access_time) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.access_time);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      memcpy(access_time, item._value, 128);
    }
  }

  return 0;
}

/**
 * db_access_time_update - Update access time.
 * @access_time: The access time.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_access_time_update(char *access_time) {
  bi_item_t item(sysinfo_table.access_time, access_time);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_query_sysinfo_by_key - Query sysinfo table by key.
 * @in key: query key.
 * @in type: query out type: int,float,string
 * @out value: query result pointer, need allocate by user
 *
 * Returns 0 if success, otherwise -1.
 */
int db_query_sysinfo_by_key(char* key, query_type_t type, void* value_ptr) {
  if (key == NULL) {
    ALOGD("%s: empty key!\n", __func__);
    return -1;
  }

  QString query_key(key);
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(query_key);

  QList<QVariant> res = sql->find(s_info);
  if (res.empty()) {
    ALOGD("%s: Query sysinfo by unknown key!\n", __func__);
    return -1;
  }

  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      switch (type) {
      case TYPE_INT: {
        int *tmp = static_cast<int*>(value_ptr);
        *tmp = QString(item._value).toInt();
      }
      break;

      case TYPE_FLT: {
        float *tmp = static_cast<float*>(value_ptr);
        *tmp = QString(item._value).toDouble();
      }
      break;

      case TYPE_STR: {
        memset(value_ptr, 0x00, 128);
        strncpy(reinterpret_cast<char*>(value_ptr), item._value, 128);
      }
      break;

      default:
        ALOGD("%s: unknown convert type\n", __func__);
      }
    }
  }

  return 0;
}

/**
 * db_websocket_admin_info_query_begin - query admin information.
 * @head: A pointer of admin info queue.
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_websocket_admin_info_query_begin(ee_queue_t* head) {
  if (head == nullptr) {
    return -1;
  }

  Admin _ai;
  QList<QVariant> res = sql->find(_ai);
  if (res.size() == 0) {
    return 0;
  }

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  websocket_admin_info_t* u;
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();

      u = new websocket_admin_info_t();
      memcpy(u->user_id, item._key, 64);
      memcpy(u->name, item._value, 32);
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    } else {
      ALOGE("cannot convert to person_info_t");
    }
  }

  return cnt;
}

/**
 * db_websocket_admin_info_query_end - free admin information.
 * @head: A pointer of admin info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_admin_info_query_end(ee_queue_t* head) {
  if (head == nullptr) {
    return -1;
  }

  ee_queue_t* h = head, *pos, *n;

  websocket_admin_info_t* p = nullptr;
  ee_queue_for_each_safe(pos, n, h) {
    p = ee_queue_data(pos, websocket_admin_info_t, queue);
    free(p);
  }
  ee_queue_init(head);

  return 0;
}

/**
 * db_websocket_admin_info_del - delete all admin information.
 * @head: A pointer of admin info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_admin_info_del() {
  Admin _ai;
  return sql->del(_ai);
}

/**
 * db_websocket_admin_info_insert - insert admin information.
 * @head: A pointer of admin info queue.
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_websocket_admin_info_insert(ee_queue_t* head) {
  if (head == nullptr) return -1;
  db_websocket_admin_info_del();
  QList<SqlBase*> item_list;
  Admin* tmp;
  ee_queue_t* h = head, *pos;
  websocket_admin_info_t* p = nullptr;
  ee_queue_for_each(pos, h) {
    p = ee_queue_data(pos, websocket_admin_info_t, queue);
    bi_item_t _bi;
    strncpy(_bi._key, p->user_id, 64);
    strncpy(_bi._value, p->name, 32);
    tmp = new Admin(_bi);
    item_list.append(tmp);
  }
  sql->insertBind(item_list);
  for (auto elem : item_list)
    free(elem);

  return item_list.size();
}

/**
 * db_websocket_person_info_query_begin - query person information.
 * @head: A pointer of admin info queue.
 * @type: table type: 0/1: employee/visitor;
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
#if 1
int db_websocket_person_info_query_begin(ee_queue_t* head, int type) {
  if (head == nullptr) {
    return -1;
  }

  Person _pi;
  if (type == TYPE_PERSON) {
    _pi.setTableName(EMPLOYEE_TABLE);
  } else if (type == TYPE_VISITOR) {
    _pi.setTableName(GUEST_TABLE);
  } else {
    return -1;
  }

  QList<QVariant> res = sql->find(_pi);

  if (res.size() == 0) {
    return 0;
  }

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  tag_websocket_person_info* u;
  for (auto elem : res) {
    if (elem.canConvert<person_info_t>()) {
      person_info_t item = elem.value<person_info_t>();

      u = new tag_websocket_person_info();
      memcpy(u->user_id, item.base.userid, 64);
      memcpy(u->name, item.base.name, 32);

      u->user_type = item.base.user_type;
      u->openvid = item.base.openvid;
      u->add_time = item.base.add_time;
      u->add_source = item.base.add_type;

      memcpy(u->face_feature, item.feat.face_feature, FEATURE_LEN);
      memcpy(u->key_points, item.feat.face_key_point, 10 * sizeof(float));
      memcpy(u->pic_large, item.feat.pic_large, 32);
      memcpy(u->pic_small, item.feat.pic_small, 32);
      u->face_id = item.feat.face_id;
      memcpy(u->face_norm, item.feat.face_norm, 2 * sizeof(float));
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }

  return cnt;
}

/**
 * db_person_info_mmc - query person information.
 * @type: table type: 0/1: employee/visitor;
 * @face_db: A pointer of admin info queue.
 * @index: the index of start.
 * @count: query the most count of person;
 *
 * Returns >= 0 admin info record count
 */
int db_person_info_mmc(int type, void *face_db,
  int feat_cnt, int *index, int count) {
  int ret = 0;

  if (*index >= feat_cnt) {
    ALOGW("%s:%d >> index[%d] is out of feat_cnt[%d]",
      __func__, __LINE__, *index, feat_cnt);
    return ret;
  }

  Person _pi;
  if (type == TYPE_PERSON) {
    _pi.setTableName(EMPLOYEE_TABLE);
  } else if (type == TYPE_VISITOR) {
    _pi.setTableName(GUEST_TABLE);
  } else {
    return -1;
  }
  _pi.setPageMode(*index, count);
  ret = sql->load_person(&_pi, face_db, feat_cnt, *index);
  if (ret > 0)
    *index += ret;
  return ret;
}

/**
 * db_person_info_query_begin - query person information.
 * @head: A pointer of admin info queue.
 * @type: table type: 0/1: employee/visitor;
 * @index: the index of start.
 * @count: query the most count of person;
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_person_info_query_begin(ee_queue_t* head,
  int type, int index, int count) {
  if (head == nullptr) {
    return -1;
  }

  Person _pi;
  if (type == TYPE_PERSON) {
    _pi.setTableName(EMPLOYEE_TABLE);
  } else if (type == TYPE_VISITOR) {
    _pi.setTableName(GUEST_TABLE);
  } else {
    return -1;
  }
  _pi.setPageMode(index, count);

  QList<QVariant> res = sql->find(_pi);

  if (res.size() == 0) {
    return 0;
  }

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  tag_websocket_person_info* u;
  for (auto elem : res) {
    if (elem.canConvert<person_info_t>()) {
      person_info_t item = elem.value<person_info_t>();

      u = new tag_websocket_person_info();
      memcpy(u->user_id, item.base.userid, 64);
      memcpy(u->name, item.base.name, 32);

      u->user_type = item.base.user_type;
      u->openvid = item.base.openvid;
      u->add_time = item.base.add_time;
      u->add_source = item.base.add_type;

      memcpy(u->face_feature, item.feat.face_feature, FEATURE_LEN);
      memcpy(u->key_points, item.feat.face_key_point, 10 * sizeof(float));
      memcpy(u->pic_large, item.feat.pic_large, 32);
      memcpy(u->pic_small, item.feat.pic_small, 32);
      u->face_id = item.feat.face_id;
      memcpy(u->face_norm, item.feat.face_norm, 2 * sizeof(float));
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }

  return cnt;
}

#else
int db_websocket_person_info_query_begin(ee_queue_t* head, int type) {
  if (head == nullptr) {
    return -1;
  }

  int ret;
  Person _pi;
  if (type == TYPE_PERSON) {
    _pi.setTableName(EMPLOYEE_TABLE);
  } else if (type == TYPE_VISITOR) {
    _pi.setTableName(GUEST_TABLE);
  } else {
    return -1;
  }

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  cnt = sql->findPersonInfo(h, _pi);

  return cnt;
}
#endif

/**
 * db_websocket_person_info_query_end - free person information.
 * @head: A pointer of person info queue.
 * @type: table type: 0/1: employee/visitor;
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_person_info_query_end(ee_queue_t* head, int type) {
  if (head == nullptr) {
    return -1;
  }

  ee_queue_t* h = head, *pos, *n;

  tag_websocket_person_info* p = nullptr;
  ee_queue_for_each_safe(pos, n, h) {
    p = ee_queue_data(pos, tag_websocket_person_info, queue);
    free(p);
  }
  ee_queue_init(head);

  return 0;
}

/**
 * db_person_info_get_begin - query person information with user_id.
 * @head: A pointer of admin info queue.
 * @type: table type: 0/1: employee/visitor;
 * @user_id: the user id regiseter.
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_person_info_get_begin(ee_queue_t* head,
  int type, char *user_id) {
  if (head == nullptr) {
    return -1;
  }

  Person _pi;
  if (type == TYPE_PERSON) {
    _pi.setTableName(EMPLOYEE_TABLE);
  } else if (type == TYPE_VISITOR) {
    _pi.setTableName(GUEST_TABLE);
  } else {
    return -1;
  }
  _pi.clearQuery();
  _pi.query("userid", user_id);


  QList<QVariant> res = sql->find(_pi);
  if (res.size() == 0) {
    return 0;
  }

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  tag_websocket_person_info* u;
  for (auto elem : res) {
    if (elem.canConvert<person_info_t>()) {
      person_info_t item = elem.value<person_info_t>();

      u = new tag_websocket_person_info();
      memcpy(u->user_id, item.base.userid, 64);
      memcpy(u->name, item.base.name, 32);

      u->user_type = item.base.user_type;
      u->openvid = item.base.openvid;
      u->add_time = item.base.add_time;
      u->add_source = item.base.add_type;

      memcpy(u->face_feature, item.feat.face_feature, FEATURE_LEN);
      memcpy(u->key_points, item.feat.face_key_point, 10 * sizeof(float));
      memcpy(u->pic_large, item.feat.pic_large, 32);
      memcpy(u->pic_small, item.feat.pic_small, 32);
      u->face_id = item.feat.face_id;
      memcpy(u->face_norm, item.feat.face_norm, 2 * sizeof(float));
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }
  return cnt;
}


/**
 * db_ui_person_info_exist - is the user id regiseter
 *
 * Returns  0 not exist, other exist..
 */
int db_ui_person_info_exist(char *user_id) {
  Person _pi;
  _pi.setTableName(EMPLOYEE_TABLE);
  _pi.clearQuery();
  _pi.query("userid", user_id);

  int cnt = 0;
  cnt = sql->getCount(_pi);
  return cnt;
}

int db_access_card_number_exit(char* cardno) {
  if ((cardno == NULL) || strlen(cardno) == 0)
    return -1;
  Person_LZ tmp;
  tmp.setTableName(EMPLOYEE_TABLE);
  tmp.clearQuery();
  tmp.query("access_card_number", cardno);
  //ALOGD("%s:%d >> %s", __func__, __LINE__, tmp.getCount().toUtf8().data());
  return sql->getCount(tmp);
}

/**
 * db_ui_person_info_get_count - get count of person information
 *
 * Returns count of person information.
 */
int db_ui_person_info_get_count(int type) {
  Person _pi;
  if (type == TYPE_PERSON) {
    _pi.setTableName(EMPLOYEE_TABLE);
  } else {
    _pi.setTableName(GUEST_TABLE);
  }

  int cnt = 0;
  cnt = sql->getCount(_pi);

  return cnt;
}

/**
 * db_ui_person_info_query_begin - query person information
 * by index, count parameter.
 * @head: A pointer of person info queue.
 * @index: index of record in table;
 * @count: count of record;
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_ui_person_info_query_begin(ee_queue_t *head, int index, int count) {
  if (head == nullptr) {
    return -1;
  }

  int ret;
  Person _pi;
  _pi.setTableName(EMPLOYEE_TABLE);

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  cnt = sql->findByLimit(h, _pi, index, count);

  return cnt;
}

int db_ui_local_person_query_imp(ee_queue_t* h, Person_LZ &_pi) {
  local_person_info_t* u;
  int cnt = 0;

  ee_queue_init(h);
  //ALOGD("%s:%d >> %s", __func__, __LINE__, _pi.find().toUtf8().data());
  QList<QVariant> res = sql->find(_pi);
  if (res.size() == 0) {
    return cnt;
  }

  for (auto elem : res) {
    if (elem.canConvert<person_lz_t>()) {
      person_lz_t item = elem.value<person_lz_t>();
      u = new local_person_info_t();
      memcpy(u->user_id, item.userid, 64);
      memcpy(u->name, item.name, 32);
      memcpy(u->pic_large, item.pic_large, MAX_PIC_PATH);
      memcpy(u->pic_small, item.pic_small, MAX_PIC_PATH);
      memcpy(u->access_card_number, item.access_card_number, 32);
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }
  return cnt++;
}
/**
 * db_ui_local_person_query_begin - query person information
 * by index, count parameter.
 * @head: A pointer of person info queue.
 * @index: index of record in table;
 * @count: count of record;
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_ui_local_person_query_begin(ee_queue_t *head, int index, int count) {
  Person_LZ _pi;
  _pi.setLimit(index, count);
  return db_ui_local_person_query_imp(head, _pi);
}

int db_ui_local_person_query_search_begin(ee_queue_t *head,
  char *userid, char *name, int index, int count) {
  Person_LZ _pi;
  QString query_st;

  if (NULL != userid && strlen(userid) > 0)
    query_st = QString("userid='%1'").arg(userid);
  if (NULL != name && strlen(name) > 0)
    if (query_st.isEmpty())
      query_st = QString("name='%1'").arg(name);
    else
      query_st += QString(" OR name='%1'").arg(name);
  _pi.query(query_st);
  _pi.setLimit(index, count);
  return db_ui_local_person_query_imp(head, _pi);
}

int db_ui_local_person_query_end(ee_queue_t *head) {
  if (head == nullptr) {
    return -1;
  }

  ee_queue_t* h = head, *pos, *n;

  local_person_info_t* p = nullptr;
  ee_queue_for_each_safe(pos, n, h) {
    p = ee_queue_data(pos, local_person_info_t, queue);
    if (p) {
      free(p);
      p = NULL;
    }
  }
  ee_queue_init(head);

  return 0;
}

int db_ui_local_person_get_count(char *userid, char *name) {
  Person_LZ _pi;
  QString query_st;

  if (NULL != userid && strlen(userid) > 0)
    query_st = QString("userid='%1'").arg(userid);
  if (NULL != name && strlen(name) > 0)
    if (query_st.isEmpty())
      query_st = QString("name='%1'").arg(name);
    else
      query_st += QString(" OR name='%1'").arg(name);
  _pi.query(query_st);

  return sql->getCount(_pi);
}

/**
 * db_ui_person_info_query_end - free person information.
 * @head: A pointer of person info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_ui_person_info_query_end(ee_queue_t *head) {
  if (head == nullptr) {
    return -1;
  }

  ee_queue_t* h = head, *pos, *n;

  tag_websocket_person_info* p = nullptr;
  ee_queue_for_each_safe(pos, n, h) {
    p = ee_queue_data(pos, tag_websocket_person_info, queue);
    if (p) {
      free(p);
      p = NULL;
    }
  }
  ee_queue_init(head);

  return 0;
}

/**
 * db_websocket_person_info_del_by_user_id - delete person information.
 * @user_id: user id.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_person_info_del_by_user_id(char* user_id) {
  Person _pi;
  _pi.setTableName(EMPLOYEE_TABLE);
  _pi.clearQuery();
  _pi.query("userid", user_id);

  QList<QVariant> res = sql->find(_pi);
  if (res.size() != 0) {
    for (auto elem : res) {
      if (elem.canConvert<person_info_t>()) {
        person_info_t item = elem.value<person_info_t>();
        remove_pic_by_path(item.feat.pic_large);
        remove_pic_by_path(item.feat.pic_small);
      }
    }
  }

  return sql->del(_pi);
}

/**
 * db_websocket_person_info_del - delete all person information from tale.
 * @type: table type(0/1:employee/vistor table).
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_person_info_del(int type) {
  Person _pi;
  if (type == EMPLOYEE) {
    _pi.setTableName(EMPLOYEE_TABLE);
  } else {
    _pi.setTableName(GUEST_TABLE);
  }
  _pi.clearQuery();

  QList<QVariant> res = sql->find(_pi);
  if (res.size() != 0) {
    for (auto elem : res) {
      if (elem.canConvert<person_info_t>()) {
        person_info_t item = elem.value<person_info_t>();
        remove_pic_by_path(item.feat.pic_large);
        remove_pic_by_path(item.feat.pic_small);
      }
    }
  }

  return sql->del(_pi);
}

/**
 * db_websocket_person_info_insert - insert person information.
 * @head: A pointer of person info queue.
 *
 * Returns 0 success, -1: failure.
 */

int db_websocket_person_info_insert(ee_queue_t* head) {
  if (head == nullptr) {
    return -1;
  }

  QList<SqlBase*> item_list;
  Person* tmp;
  ee_queue_t* h = head, *pos;
  tag_websocket_person_info* p = nullptr;
  ee_queue_for_each(pos, h) {
    p = ee_queue_data(pos, tag_websocket_person_info, queue);

    person_info_t item;
    memcpy(item.base.userid, p->user_id, 64);
    memcpy(item.base.name, p->name, 32);
    memcpy(&item.base.user_type, &p->user_type, sizeof(int));
    memcpy(&item.base.openvid, &p->openvid, sizeof(uint64_t));
    memcpy(&item.base.add_time, &p->add_time, sizeof(int));
    memcpy(&item.base.add_type, &p->add_source, sizeof(int));

    memcpy(item.feat.face_feature, p->face_feature, FEATURE_LEN);
    memcpy(item.feat.face_key_point, p->key_points, 10 * sizeof(float));
    memcpy(item.feat.pic_large, p->pic_large, MAX_PIC_PATH);
    memcpy(item.feat.pic_small, p->pic_small, MAX_PIC_PATH);
    ALOGD("%s:%d >> pic_large=%s, pic_small=%s\n", __func__, __LINE__,
      item.feat.pic_large, item.feat.pic_small);
    item.feat.face_id = p->face_id;
    memcpy(item.feat.face_norm, p->face_norm, 2 * sizeof(float));
    memcpy(item.access_card_number, p->access_card_number, 32);
    tmp = new Person(item);
    item_list.append(tmp);
  }
  int insNum = sql->insertBind(item_list);
  for (auto elem : item_list)
    free(elem);

  return (item_list.size() == insNum ? 0 : -1);
}

/**
 * db_websocket_person_info_update_feat - update person information(feature).
 * @p_info: A pointer of websocket_person_info_t.
 *
 * Returns 0 success, -1: failure.
 */
int db_websocket_person_info_update_feat(websocket_person_info_t* p_info) {
  Person tmp;
  tmp.setTableName(EMPLOYEE_TABLE);
  tmp.clearQuery();
  tmp.query("userid", p_info->user_id).query("face_id", p_info->face_id);
  QList<QVariant> res = sql->find(tmp);
  if (res.size() != 1) return -1;
  int recordEffect = 0;
  for (auto elem : res) {
    if (elem.canConvert<person_info_t>()) {
      person_info_t _f = elem.value<person_info_t>();
      memcpy(_f.feat.face_feature, p_info->face_feature, FEATURE_LEN);
      memcpy(_f.feat.face_key_point, p_info->key_points, 10 * sizeof(float));
      memcpy(_f.feat.face_norm, p_info->face_norm, 2 * sizeof(float));
      Person _update(_f);
      recordEffect = sql->updateBind(_update);
    }
  }

  return (recordEffect == 1 ? 0 : -1);
}

/**
 * db_websocket_person_info - get person information(feature).
 * @p_info: A pointer of websocket_person_info_t.
 *
 * Returns 0 success, -1: failure.
 */
int db_websocket_person_info(websocket_person_info_t* p_info) {
  Person tmp;
  int found = -1;
  tmp.setTableName(EMPLOYEE_TABLE);
  tmp.clearQuery();
  tmp.query("userid", p_info->user_id);
  QList<QVariant> res = sql->find(tmp);
  if (res.size() != 1) return found;
  for (auto elem : res) {
    if (elem.canConvert<person_info_t>()) {
      person_info_t _f = elem.value<person_info_t>();
      memcpy(p_info->name, _f.base.name, sizeof(p_info->name));
      memcpy(p_info->pic_large, _f.feat.pic_large,
        sizeof(p_info->pic_large));
      memcpy(p_info->pic_small, _f.feat.pic_small,
        sizeof(p_info->pic_small));
      memcpy(p_info->access_card_number, _f.access_card_number,
        sizeof(p_info->access_card_number));
      found = 0;
      break;
    }
  }

  return found;
}

/**
 * db_websocket_person_info_query_begin_by_name
 * Query employee infos by name
 * @user_name A string of user name
 * @head The pointer to result queue
 *
 * Returns -1 if errors, otherwise the nums of related records.
 */
int db_websocket_person_info_query_begin_by_name(
    char* user_name, ee_queue_t* head) {
  if ((user_name == NULL) || (head == NULL)) {
    return -1;
  }

  ee_queue_t* h = head;
  ee_queue_init(h);

  char query_name[MAX_VAL_LEN] = { 0 };
  snprintf(query_name, MAX_VAL_LEN, "%s", user_name);

  Person tmp;
  tmp.setTableName(EMPLOYEE_TABLE);
  tmp.clearQuery();
  tmp.query("name", query_name);
  QList<QVariant> res = sql->find(tmp);
  if (res.size() == 0) {
    return 0;
  }

  tag_websocket_person_info* u;
  int cnt = 0;
  for (auto elem : res) {
    if (elem.canConvert<person_info_t>()) {
      person_info_t item = elem.value<person_info_t>();

      u = new tag_websocket_person_info();
      memcpy(u->user_id, item.base.userid, 64);
      memcpy(u->name, item.base.name, 32);

      u->user_type = item.base.user_type;
      u->openvid = item.base.openvid;
      u->add_time = item.base.add_time;
      u->add_source = item.base.add_type;

      memcpy(u->face_feature, item.feat.face_feature, FEATURE_LEN);
      memcpy(u->key_points, item.feat.face_key_point, 10 * sizeof(float));
      memcpy(u->pic_large, item.feat.pic_large, 32);
      memcpy(u->pic_small, item.feat.pic_small, 32);
      u->face_id = item.feat.face_id;
      memcpy(u->face_norm, item.feat.face_norm, 2 * sizeof(float));
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }

  return cnt++;
}

/**
 * db_websocket_rule_info_query_begin - query rule information
 * @head: A pointer of rule info queue.
 *
 * Returns >= 0 admin info record count, -1: failure.
 */
int db_websocket_rule_info_query_begin(ee_queue_t* head) {
  if (head == nullptr) {
    return -1;
  }

  Rule _ri;
  QList<QVariant> res = sql->find(_ri);

  if (res.size() == 0) {
    return 0;
  }

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  tag_websocket_rule_info* u;
  for (auto elem : res) {
    if (elem.canConvert<person_info_t>()) {
      rule_info_t item = elem.value<rule_info_t>();

      u = new tag_websocket_rule_info();
      memcpy(u->user_id, item.userid, 64);
      u->rule_id = item.rule_id;
      memcpy(u->pass_rule, item.rule, 64);
      u->effect_time = item.effect_time;
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }

  return cnt;
}

/**
 * db_websocket_rule_info_query_end - free rule information.
 * @head: A pointer of rule info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_rule_info_query_end(ee_queue_t* head) {
  if (head == nullptr) {
    return -1;
  }

  ee_queue_t* h = head, *pos, *n;

  web_socket_rule_info_t* p = nullptr;
  ee_queue_for_each_safe(pos, n, h) {
    p = ee_queue_data(pos, web_socket_rule_info_t, queue);
    free(p);
  }
  ee_queue_init(head);

  return 0;
}

/**
 * db_websocket_rule_info_del_by_user_id - delete rule information.
 * @user_id: user id.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_rule_info_del_by_user_id(char* user_id) {
  Rule _ri;
  _ri.clearQuery();
  _ri.query("userid", user_id);

  return sql->del(_ri);
}

/**
 * db_websocket_rule_info_by_userid - delete rule information.
 * @userid: user id.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_rule_info_by_userid(char* userid) {
  Rule _ri;
  _ri.clearQuery();
  _ri.query("userid", userid);

  return sql->del(_ri);
}

int db_websocket_rule_info_of_guest() {
  Person _pi;
  _pi.clearQuery();
  _pi.setTableName(GUEST_TABLE);
  QList<QVariant> res = sql->find(_pi);
  int cnt = 0;
  for (auto elem : res) {
    if (elem.canConvert<person_info_t>()) {
      person_info_t item = elem.value<person_info_t>();
      cnt += db_websocket_rule_info_by_userid(item.base.userid);
    }
  }

  return cnt;
}

/**
 * db_websocket_rule_info_insert - add rule information.
 * @head: A pointer of rule info queue.
 * @type: type: 0/1: person/guest
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_rule_info_insert(ee_queue_t* head, int type) {
  if (head == nullptr) {
    return -1;
  }

  if (type == TYPE_PERSON) {
    db_websocket_rule_info_of_guest();
  }

  QList<SqlBase*> item_list;
  Rule* tmp;
  char userid_tmp[64];
  memset(userid_tmp, 0x00, 64);
  ee_queue_t* h = head, *pos;
  tag_websocket_rule_info* p = nullptr;
  ee_queue_for_each(pos, h) {
    p = ee_queue_data(pos, tag_websocket_rule_info, queue);
    rule_info_t item;
    memcpy(item.userid, p->user_id, 64);
    item.rule_id = p->rule_id;
    memcpy(item.rule, p->pass_rule, 64);
    item.effect_time = p->effect_time;
    tmp = new Rule(item);
    item_list.append(tmp);

    if (userid_tmp[0] == 0 || strcmp(userid_tmp, item.userid)) {
      strncpy(userid_tmp, item.userid, 64);
      db_websocket_rule_info_by_userid(item.userid);
    }
  }
  sql->insertBind(item_list);
  for (auto elem : item_list)
    free(elem);

  return item_list.size();
}

/**
 * db_websocket_checkin_info_query_begin - query checkin information.
 * @head: A pointer of checkin info queue.
 *
 * Returns >= 0 record count, -1: failure.
 */
int db_websocket_checkin_info_query_begin(
  ee_queue_t* head, int count) {
  Checkin _ci;
  _ci.set_first_count(count);

  QList<QVariant> res = sql->find(_ci);
  if (res.size() == 0) {
    return 0;
  }

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  web_socket_checkin_info_t* u;
  for (auto elem : res) {
    if (elem.canConvert<record_info_t>()) {
      record_info_t item = elem.value<record_info_t>();
      u = new web_socket_checkin_info_t();
      memcpy(u, &item, sizeof(record_info_t));
      ALOGD("%s %d\n", u->user_id, u->timestamp);
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }

  return cnt;
}

int db_websocket_checkin_info_count_by_id_time (
  char * user_id, long start, long end) {
  Checkin _ci;
  _ci.clearQuery();
  if (0 != strcmp(user_id, ""))
    _ci.query("userid", user_id);
  _ci.query(QString("%1 <= check_in_time").arg(start));
  if (0 < end)
    _ci.query(QString("check_in_time <= %1").arg(end));
  return sql->getCount(_ci);
}

/**
 * db_websocket_checkin_info_query_by_id_time_begin - query checkin information.
 * @head: A pointer of checkin info queue.
 * @user_id: The user_id of checkin info.
 * @start: The start time of checkin info.
 * @end: The end time of checkin info.
 *
 * Returns >= 0 record count, -1: failure.
 */
int db_websocket_checkin_info_query_by_id_time_begin (
  ee_queue_t* head, char *user_id, long start, long end) {
  Checkin _ci;
  _ci.clearQuery();
  if (0 != strcmp(user_id, "")) {
    _ci.query("userid", user_id);
  }
  _ci.query(QString("%1 <= check_in_time").arg(start));
  if (0 < end)
      _ci.query(QString("check_in_time <= %1").arg(end));
  QList<QVariant> res = sql->find(_ci);

  if (res.size() == 0)
    return 0;

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  web_socket_checkin_info_t* u;
  for (auto elem : res) {
    if (elem.canConvert<record_info_t>()) {
      record_info_t item = elem.value<record_info_t>();
      u = new web_socket_checkin_info_t();
      memcpy(u, &item, sizeof(record_info_t));
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }
  return cnt;
}

int db_websocket_checkin_query_limit_begin(
  ee_queue_t *head, char *user_id, long start, long end,
  int index, int count) {
  Checkin _ci;
  _ci.clearQuery();
  if (0 != strcmp(user_id, "")) {
    _ci.query("userid", user_id);
  }
  _ci.query(QString("%1 <= check_in_time").arg(start));
  if (0 < end)
    _ci.query(QString("check_in_time <= %1").arg(end));
  _ci.set_limit(index, count);
  QList<QVariant> res = sql->find(_ci);
  //ALOGD("%s:%d >> %s", __func__, __LINE__, _ci.find().toUtf8().data());
  if (res.size() == 0)
    return 0;

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  web_socket_checkin_info_t* u;
  for (auto elem : res) {
    if (elem.canConvert<record_info_t>()) {
      record_info_t item = elem.value<record_info_t>();
      u = new web_socket_checkin_info_t();
      memcpy(u, &item, sizeof(record_info_t));
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }
  return cnt;
}

/**
 * db_websocket_checkin_info_query_by_time_begin - query checkin information.
 * @head: A pointer of checkin info queue.
 * @start: The start time of checkin info.
 * @end: The end time of checkin info.
 *
 * Returns >= 0 record count, -1: failure.
 */
int db_websocket_checkin_info_query_by_time_begin(
  ee_queue_t* head, long start, long end) {
  Checkin _ci;
  _ci.clearQuery();
  _ci.query(QString("%1 <= check_in_time").arg(start));
  if (0 < end)
      _ci.query(QString("check_in_time <= %1").arg(end));
  QList<QVariant> res = sql->find(_ci);

  if (res.size() == 0)
    return 0;

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  web_socket_checkin_info_t* u;
  for (auto elem : res) {
    if (elem.canConvert<record_info_t>()) {
      record_info_t item = elem.value<record_info_t>();
      u = new web_socket_checkin_info_t();
      memcpy(u, &item, sizeof(record_info_t));
      ALOGD("%s %d\n", u->user_id, u->timestamp);
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }
  return cnt;
}


/**
 * db_websocket_checkin_info_query_end - free checkin information.
 * @head: A pointer of checkin info queue.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_checkin_info_query_end(ee_queue_t* head) {
  if (head == nullptr) {
    return -1;
  }

  ee_queue_t* h = head, *pos, *n;
  web_socket_checkin_info_t* p = nullptr;
  ee_queue_for_each_safe(pos, n, h) {
    p = ee_queue_data(pos, web_socket_checkin_info_t, queue);
    free(p);
  }
  ee_queue_init(head);

  return 0;
}

int db_websocket_checkin_info_del_by_day(int days) {
  return 0;
}

int db_websocket_checkin_info_del_by_time(int timestamp) {
  return 0;
}

int db_websocket_checkin_info_del_table() {
  Checkin _ci;
  _ci.clearQuery();

  return sql->del(_ci);
}

int db_websocket_checkin_info_del_by_last(int count) {
  Checkin _ci;
  _ci.clearQuery();
  _ci.set_last_count(count);

  QList<QVariant> res = sql->find(_ci);

  if (res.size() != 0) {
    for (auto elem : res) {
      if (elem.canConvert<record_info>()) {
        record_info item = elem.value<record_info>();
        file_remove(item.face_path);
        file_remove(item.frame_path);
      }
    }
  }
  return sql->del(_ci);
}

/**
 * db_websocket_checkin_info_insert - insert checkin information.
 * @p_info: A pointer of web_socket_checkin_info_t.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_checkin_info_insert(web_socket_checkin_info_t *p_info) {
  if (p_info == nullptr) {
    return -1;
  }

  record_info item;
  memcpy(item.userid, p_info->user_id, 64);
  item.user_type = p_info->user_type;
  item.check_in_time = p_info->timestamp;
  item.is_uploaded = p_info->is_upload;
  item.upload_time = p_info->upload_time;
  memcpy(item.face_path, p_info->face_path, 128);
  memcpy(item.frame_path, p_info->frame_path, 128);
  memcpy(item.name, p_info->name, 64);
  item.temperature = p_info->temperature;
  Checkin tmp(item);

  return sql->insertBind(tmp);
}

/**
 * db_websocket_checkin_info_get_count - get count of checkin information
 *
 * Returns count of checkin information.
 */
int db_websocket_checkin_info_get_count(void) {
  Checkin _ci;
  _ci.clearQuery();

  int cnt = 0;
  cnt = sql->getCount(_ci);

  return cnt;
}

/**
 * db_websocket_checkin_info_del - delete checkin information.
 * @p_info: A pointer of web_socket_checkin_info_t.
 *
 * Returns  0 success, -1: failure.
 */
int db_websocket_checkin_info_del(web_socket_checkin_info_t* p_info) {
  Checkin _ci;
  _ci.clearQuery();

  _ci.query("userid", p_info->user_id)
    .query("check_in_time", p_info->timestamp);
  file_remove(p_info->face_path);
  file_remove(p_info->frame_path);
  int res = sql->del(_ci);

  return (res == 1 ? 0 : -1);
}

/**
 * db_ui_get_development_status - get development status.
 *
 * Returns development status.
 */
int db_ui_get_development_status(void) {
  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.development);

  int development = 0;
  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      development = QString(item._value).toInt();
    } else {
      development = 0;
    }
  }

  return development;
}

/**
 * db_ui_get_password - get password of enter main menu.
 * @password: A pointer of password.
 * @pwd_len: length of password buffer
 *
 * Returns  0 success, -1: failure.
 */
int db_ui_get_password(char *password, int pwd_len) {
  int ret = -1;
  if ((NULL == password) || (0 == pwd_len)) {
    return -1;
  }

  Sysinfo s_info;
  s_info.clearyQuery();
  s_info.query(sysinfo_table.password);

  QList<QVariant> res = sql->find(s_info);
  for (auto elem : res) {
    if (elem.canConvert<bi_item_t>()) {
      bi_item_t item = elem.value<bi_item_t>();
      snprintf(password, pwd_len, "%s", QString(item._value).toUtf8().data());
      ret = 0;
    }
  }

  return ret;
}

/**
 * db_ui_set_password - set password of enter main menu.
 * @password: password of enter main menu.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_ui_set_password(const char *password) {
  if (NULL == password) {
    return -1;
  }

  bi_item_t item(sysinfo_table.password, password);

  Sysinfo s_info(item);

  int ret = sql->updateBind(s_info);

  return (ret == 1) ? 0 : -1;
}

/**
 * db_ui_export_rec_get_count - get total count of export record.
 * @password: password of enter main menu.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_ui_export_rec_get_count(void) {
  ExportCheckin _pi;

  int cnt = 0;
  cnt = sql->getCount(_pi);

  return cnt;
}

int db_ui_export_by_time_get_count(char *user_id,
  long start, long end) {
  ExportCheckin _pi;
  _pi.clearQuery();
  if (0 != strcmp(user_id, ""))
    _pi.query(QString("userid='%1'").arg(user_id));
  _pi.query(QString("%1 <= check_in_time").arg(start));
  if (0 < end)
    _pi.query(QString("check_in_time <= %1").arg(end));

  return sql->getCount(_pi);
}

/**
 * db_ui_export_rec_query_begin - query export record.
 * @index: index of total record.
 * @count: count of current record.
 * @head: a pointer of query result queue
 *
 * Returns 0 if success, otherwise -1.
 */
int db_ui_export_rec_query_begin(int index, int count,
  ee_queue_t *head) {
  if (head == nullptr) {
    return -1;
  }

  int ret;
  ExportCheckin _pi;
  _pi.query(index, count);
  QList<QVariant> res = sql->find(_pi);
  if (res.size() == 0) {
    return 0;
  }

  int cnt = 0;
  ee_queue_t* h = head;
  ee_queue_init(h);
  web_socket_export_checkin_info_t *u;
  for (auto elem : res) {
    if (elem.canConvert<export_record_info_t>()) {
      export_record_info_t item = elem.value<export_record_info_t>();
      u = new web_socket_export_checkin_info_t();
      snprintf(u->userid, sizeof(u->userid), "%s", item.userid);
      snprintf(u->name, sizeof(u->name), "%s", item.name);
      u->check_in_time = item.check_in_time;
      // ALOGD("%s %s %d\n", u->userid, u->name, u->check_in_time);
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }
  return cnt;
}

int db_ui_export_rec_query_by_id_time_begin(
  int index, int count, ee_queue_t* head, char *user_id,
  long start, long end) {
  int cnt = 0;
  ExportCheckin _pi;
  _pi.clearQuery();
  if (0 != strcmp(user_id, ""))
    _pi.query(QString("userid='%1'").arg(user_id));
  _pi.query(QString("%1 <= check_in_time").arg(start));
  if (0 < end)
    _pi.query(QString("check_in_time <= %1").arg(end));
  _pi.limit(QString(" LIMIT %1,%2").arg(index).arg(count));
  QList<QVariant> res = sql->find(_pi);
  if (res.size() == 0) {
    return cnt;
  }

  ee_queue_t* h = head;
  ee_queue_init(h);
  web_socket_export_checkin_info_t *u;
  for (auto elem : res) {
    if (elem.canConvert<export_record_info_t>()) {
      export_record_info_t item = elem.value<export_record_info_t>();
      u = new web_socket_export_checkin_info_t();
      snprintf(u->userid, sizeof(u->userid), "%s", item.userid);
      snprintf(u->name, sizeof(u->name), "%s", item.name);
      u->check_in_time = item.check_in_time;
      // ALOGD("%s %s %d\n", u->userid, u->name, u->check_in_time);
      ee_queue_insert_tail(h, &(u->queue));
      cnt++;
    }
  }
  return cnt;
}

/**
 * db_ui_export_rec_query_end - free query result.
 * @head: a pointer of query result queue.
 *
 * Returns 0 if success, otherwise -1.
 */
int db_ui_export_rec_query_end(ee_queue_t *head) {
  if (head == nullptr) {
    return -1;
  }

  ee_queue_t* h = head, *pos, *n;
  web_socket_export_checkin_info_t *p = nullptr;
  ee_queue_for_each_safe(pos, n, h) {
    p = ee_queue_data(pos, web_socket_export_checkin_info_t, queue);
    free(p);
  }
  ee_queue_init(head);

  return 0;
}

