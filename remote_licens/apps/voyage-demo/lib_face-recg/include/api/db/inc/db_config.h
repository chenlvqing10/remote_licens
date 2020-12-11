/*
 * db_config.h
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

#ifndef __DB_CONFIG_H__
#define __DB_CONFIG_H__

#include "sqlite3.h"

#define DBCONF_FACE "face"
#define DBCONF_FACE_NIRBOXDEBUG "nir_box_debug"
#define DBCONF_FACE_NIRBOXDEBUG_VALUE "0"
#define DBCONF_FACE_RECGTIMEDEBUG "recg_time_debug"
#define DBCONF_FACE_RECGTIMEDEBUG_VALUE "0"
#define DBCONF_FACE_RECGTIMEOUT "recg_timeout"
#define DBCONF_FACE_RECGTIMEOUT_VALUE "3"

#define DBCONF_REGISTER "register"
#define DBCONF_REGISTER_REGQUALITY "reg_quality"
#define DBCONF_REGISTER_REGQUALITY_VALUE "0"
#define DBCONF_REGISTER_REGTHRESHOLD "reg_threshold"
#define DBCONF_REGISTER_REGTHRESHOLD_VALUE "0.5"
#define DBCONF_REGISTER_CHECKFACESIZE "check_face_size"
#define DBCONF_REGISTER_CHECKFACESIZE_VALUE "0"
#define DBCONF_REGISTER_FACEMAX "face_max"
#define DBCONF_REGISTER_FACEMAX_VALUE "512"
#define DBCONF_REGISTER_FACEMIN "face_min"
#define DBCONF_REGISTER_FACEMIN_VALUE "128"
#define DBCONF_REGISTER_REFRESHDBEACHTIME "refresh_db_each_time"
#define DBCONF_REGISTER_REFRESHDBEACHTIME_VALUE "0"
#define DBCONF_REGISTER_JPEGWSTRIDE "jpeg_w_stride"
#define DBCONF_REGISTER_JPEGWSTRIDE_VALUE "800"
#define DBCONF_REGISTER_JPEGHSTRIDE "jpeg_h_stride"
#define DBCONF_REGISTER_JPEGHSTRIDE_VALUE "1024"

#define DBCONF_SYSTEM "system"
#define DBCONF_SYSTEM_VERSION "version"
#define DBCONF_SYSTEM_VERSION_VALUE "v1.0rc3"
#define DBCONF_SYSTEM_WHITELIGHTGPIO "white_light_gpio"
#define DBCONF_SYSTEM_WHITELIGHTGPIO_VALUE "A0"
#define DBCONF_SYSTEM_HIGHBRIGHTNESS "high_brightness"
#define DBCONF_SYSTEM_HIGHBRIGHTNESS_VALUE "800"
#define DBCONF_SYSTEM_LOWBRIGHTNESS "low_brightness"
#define DBCONF_SYSTEM_LOWBRIGHTNESS_VALUE "600"
#define DBCONF_SYSTEM_DETINTERVAL "det_interval"
#define DBCONF_SYSTEM_DETINTERVAL_VALUE "1"
#define DBCONF_SYSTEM_DELAYTIME "delay_time"
#define DBCONF_SYSTEM_DELAYTIME_VALUE "15"
#define DBCONF_SYSTEM_SCREENAUTOENABLE "screen_auto_enable"
#define DBCONF_SYSTEM_SCREENAUTOENABLE_VALUE "9"
#define DBCONF_SYSTEM_SCREEN_OFF_TIME "screen_off_time"
#define DBCONF_SYSTEM_SCREEN_OFF_TIME_VALUE "15"

#define DBCONF_RECOGNITION "recognition"
#define DBCONF_RECOGNITION_RECGTHRESHOLD "recg_threshold"
#define DBCONF_RECOGNITION_RECGTHRESHOLD_VALUE "0.5"
#define DBCONF_RECOGNITION_JPEGWSTRIDE "jpeg_w_stride"
#define DBCONF_RECOGNITION_JPEGWSTRIDE_VALUE "800"
#define DBCONF_RECOGNITION_JPEGHSTRIDE "jpeg_h_stride"
#define DBCONF_RECOGNITION_JPEGHSTRIDE_VALUE "1024"

#define DBCONF_CLOUDPLATFORM "cloud_platform"
#define DBCONF_CLOUDPLATFORM_AISERVERURL "ai_server_url"
#define DBCONF_CLOUDPLATFORM_AISERVERURL_VALUE ""
#define DBCONF_CLOUDPLATFORM_ATTENDANCEURL "attendance_url"
#define DBCONF_CLOUDPLATFORM_ATTENDANCEURL_VALUE ""
#define DBCONF_CLOUDPLATFORM_FOTAURL "fota_url"
#define DBCONF_CLOUDPLATFORM_FOTAURL_VALUE ""
#define DBCONF_CLOUDPLATFORM_UPLOAD_RET_URL "cloud_upload_rec_url"
#define DBCONF_CLOUDPLATFORM_UPLOAD_RET_URL_VALUE ""
#define DBCONF_CLOUDPLATFORM_IMPORT_MEMS_URL "cloud_import_mems_url"
#define DBCONF_CLOUDPLATFORM_IMPORT_MEMS_URL_VALUE ""
#define DBCONF_CLOUDPLATFORM_IMPORT_DEL_ALL_URL "cloud_import_delete_all_url"
#define DBCONF_CLOUDPLATFORM_IMPORT_DEL_ALL_URL_VALUE ""
#define DBCONF_CLOUDPLATFORM_CHANGE_URL "cloud_ipaddr_changed_url"
#define DBCONF_CLOUDPLATFORM_CHANGE_URL_VALUE ""
#define DBCONF_CLOUDPLATFORM_IMPORT_MEMS_RET_URL "cloud_import_mems_ret_url"
#define DBCONF_CLOUDPLATFORM_IMPORT_MEMS_RET_URL_VALUE ""

#define DBCONF_DEVICEHWPARAM "device_hw_param"
#define DBCONF_DEVICEHWPARAM_VOLUME "volume"
#define DBCONF_DEVICEHWPARAM_VOLUME_VALUE "100"
#define DBCONF_DEVICEHWPARAM_AUTODISPLAYBRIGHTNESS "auto_display_brightness"
#define DBCONF_DEVICEHWPARAM_AUTODISPLAYBRIGHTNESS_VALUE "0"
#define DBCONF_DEVICEHWPARAM_DISPLAYBRIGHTNESS "display_brightness"
#define DBCONF_DEVICEHWPARAM_DISPLAYBRIGHTNESS_VALUE "100"
#define DBCONF_DEVICEHWPARAM_INFRAREDLIGHTSET "infrared_light_set"
#define DBCONF_DEVICEHWPARAM_INFRAREDLIGHTSET_VALUE "2"
#define DBCONF_DEVICEHWPARAM_LIGHTSENSITIVE "light_sensitive"
#define DBCONF_DEVICEHWPARAM_LIGHTSENSITIVE_VALUE "1"
#define DBCONF_DEVICEHWPARAM_HUMANINDUCTION "human_induction"
#define DBCONF_DEVICEHWPARAM_HUMANINDUCTION_VALUE "1"
#define DBCONF_DEVICEHWPARAM_AUTOSCREENOFF "auto_screen_off"
#define DBCONF_DEVICEHWPARAM_AUTOSCREENOFF_VALUE "100"
#define DBCONF_DEVICEHWPARAM_AUTOSCREENOFFTIMEOUT "auto_screen_off_timeout"
#define DBCONF_DEVICEHWPARAM_AUTOSCREENOFFTIMEOUT_VALUE "15"
#define DBCONF_DEVICEHWPARAM_SCREENSAVEROFF "screen_saver_off"
#define DBCONF_DEVICEHWPARAM_SCREENSAVEROFF_VALUE "100"
#define DBCONF_DEVICEHWPARAM_SCREENSAVEROFFTIMEOUT "screen_saver_off_timeout"
#define DBCONF_DEVICEHWPARAM_SCREENSAVEROFFTIMEOUT_VALUE "15"
#define DBCONF_DEVICEHWPARAM_SCREENSAVERCOUNT "screen_saver_count"
#define DBCONF_DEVICEHWPARAM_SCREENSAVERCOUNT_VALUE "3"
#define DBCONF_DEVICEHWPARAM_SCREENSAVERTIME "screen_saver_time"
#define DBCONF_DEVICEHWPARAM_SCREENSAVERTIME_VALUE "5"

#define DBCONF_DEVICENETPARAM "device_net_param"
#define DBCONF_DEVICENETPARAM_ENABLEETH "enable_eth"
#define DBCONF_DEVICENETPARAM_ENABLEETH_VALUE "4"
#define DBCONF_DEVICENETPARAM_ENABLEETHPRIORITY "enable_eth_priority"
#define DBCONF_DEVICENETPARAM_ENABLEETHPRIORITY_VALUE "255"
#define DBCONF_DEVICENETPARAM_ENABLEETHDHCP "enable_eth_dhcp"
#define DBCONF_DEVICENETPARAM_ENABLEETHDHCP_VALUE "0"
#define DBCONF_DEVICENETPARAM_ETHIP "eth_ip"
#define DBCONF_DEVICENETPARAM_ETHIP_VALUE "192.168.1.11"
#define DBCONF_DEVICENETPARAM_ETHMASK "eth_mask"
#define DBCONF_DEVICENETPARAM_ETHMASK_VALUE "255.255.255.0"
#define DBCONF_DEVICENETPARAM_ETHGATEWAY "eth_gateway"
#define DBCONF_DEVICENETPARAM_ETHGATEWAY_VALUE "192.168.1.1"
#define DBCONF_DEVICENETPARAM_ETHDNS "eth_dns"
#define DBCONF_DEVICENETPARAM_ETHDNS_VALUE "192.168.1.1"
#define DBCONF_DEVICENETPARAM_ENABLEWIFI "enable_wifi"
#define DBCONF_DEVICENETPARAM_ENABLEWIFI_VALUE "1"
#define DBCONF_DEVICENETPARAM_ENABLEHOTSPOT "enable_hotspot"
#define DBCONF_DEVICENETPARAM_ENABLEHOTSPOT_VALUE "0"
#define DBCONF_DEVICENETPARAM_HOTSPOTNAME "hotspot_name"
#define DBCONF_DEVICENETPARAM_HOTSPOTNAME_VALUE "DuDU_WiFi"
#define DBCONF_DEVICENETPARAM_HOTSPOTPASSWORD "hotspot_password"
#define DBCONF_DEVICENETPARAM_HOTSPOTPASSWORD_VALUE "12345678"

#define DBCONF_DEVICESTATUSPARAM "device_status_param"
#define DBCONF_DEVICESTATUSPARAM_COMPANYNAME "company_name"
#define DBCONF_DEVICESTATUSPARAM_COMPANYNAME_VALUE "lombo"
#define DBCONF_DEVICESTATUSPARAM_COMPANYLOGO "company_logo"
#define DBCONF_DEVICESTATUSPARAM_COMPANYLOGO_VALUE "lombo.jpg"
#define DBCONF_DEVICESTATUSPARAM_DEVICEADDRESS "device_address"
#define DBCONF_DEVICESTATUSPARAM_DEVICEADDRESS_VALUE "ZhuHai"
#define DBCONF_DEVICESTATUSPARAM_LANGUAGE "language"
#define DBCONF_DEVICESTATUSPARAM_LANGUAGE_VALUE "1"
#define DBCONF_DEVICESTATUSPARAM_PUNCHMODE "punch_mode"
#define DBCONF_DEVICESTATUSPARAM_PUNCHMODE_VALUE "0"
#define DBCONF_DEVICESTATUSPARAM_DEBUGSWITCH "debug_switch"
#define DBCONF_DEVICESTATUSPARAM_DEBUGSWITCH_VALUE "0"

#define DBCONF_FACERECOGNITIONPARAM "face_recognition_param"
#define DBCONF_FACERECOGNITIONPARAM_IDENTIFYCLOSESTDISTANCE "identify_closest_distance"
#define DBCONF_FACERECOGNITIONPARAM_IDENTIFYCLOSESTDISTANCE_VALUE "1.5"
#define DBCONF_FACERECOGNITIONPARAM_IDENTIFYFURTHESTDISTANCE "identify_furthest_distance"
#define DBCONF_FACERECOGNITIONPARAM_IDENTIFYFURTHESTDISTANCE_VALUE "3.2"
#define DBCONF_FACERECOGNITIONPARAM_IDENTIFYTHRESHOLDSCORE "identify_threshold_score"
#define DBCONF_FACERECOGNITIONPARAM_IDENTIFYTHRESHOLDSCORE_VALUE "0.5"
#define DBCONF_FACERECOGNITIONPARAM_NIRLIVEDETECT "nir_live_detect"
#define DBCONF_FACERECOGNITIONPARAM_NIRLIVEDETECT_VALUE "1"
#define DBCONF_FACERECOGNITIONPARAM_MINFACEPIXEL "min_face_pixel"
#define DBCONF_FACERECOGNITIONPARAM_MINFACEPIXEL_VALUE "80"
#define DBCONF_FACERECOGNITIONPARAM_MAXFACEPIXEL "max_face_pixel"
#define DBCONF_FACERECOGNITIONPARAM_MAXFACEPIXEL_VALUE "512"
#define DBCONF_FACERECOGNITIONPARAM_MAXTRACKFACES "max_track_faces"
#define DBCONF_FACERECOGNITIONPARAM_MAXTRACKFACES_VALUE "5"

#define DBCONF_MANAGERINFO "manager_info"
#define DBCONF_MANAGERINFO_USERNAME "user_name"
#define DBCONF_MANAGERINFO_USERNAME_VALUE "root"
#define DBCONF_MANAGERINFO_PASSWORD "user_password"
#define DBCONF_MANAGERINFO_PASSWORD_VALUE "123456"

#define DBCONF_DB "db"
#define DBCONF_DB_VERSION "version"
#define DBCONF_DB_VERSION_VALUE "v1.0"

#define DBCONF_ISP "isp"
#define DBCONF_ISP_HDR "hdr"
#define DBCONF_ISP_HDR_VALUE "0"

int db_config_get_count(sqlite3_stmt *stmt, void *value);
int db_config_get_int(char *section, char *key, int default_value);
long db_config_get_long(char *section, char *key, long default_value);
double db_config_get_double(char *section, char *key, double default_value);
int db_config_get_string(char *section, char *key, char *value, int value_size,
	char *default_value);
int db_config_set_int(char *section, char *key, int value);
int db_config_set_long(char *section, char *key, long value);
int db_config_set_double(char *section, char *key, double value);
int db_config_set_string(char *section, char *key, char *value);
int which_db_config_get_string(char *which_db, char *section, char *key,
	char *value, int value_size, char *default_value);

#endif /* __DB_CONFIG_H__ */

