/*
* sql_head.hpp - define sql type of project
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
#ifndef SQL_HEAD_H
#define SQL_HEAD_H
#include <QObject>
#include <QVariant>
#include <QString>
#include <QSqlDriver>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QList>
#include <QDebug>

#ifdef __WIN32
#define SRC_DB_PATH "./etc"
#define WK_DB_PATH "./data"
#else
#define SRC_DB_PATH "/etc"
#define WK_DB_PATH "/data"
#endif

#define DB_FILE_NAME "/data/user.db"
#define EMPLOYEE_TABLE "employee"
#define GUEST_TABLE "guest"
#define ADMIN_TABLE "admin"
#define RULE_TABLE "rule"
#define CHECKIN_TABLE "checkin"
#define CHECKINDBG_TABLE "checkindbg"
#define SYSINFO_TABLE "sysinfo"

#define MAX_PIC_PATH 64
#define MAX_STR_LEN 64
#define MAX_VAL_LEN 256
#define FEATURE_LEN 1024
#define MAX_RULE_LEN 64
#define FACE_KEY_POINT 256
#define MAX_IMG_LEN 128


#define getVarName(x) #x

typedef enum UserType {
  EMPLOYEE = 0,
  GUEST = 1
} EUserType;

typedef enum AddType {
  LOCAL = 0,
  WECHAT = 1
}EAddType;

typedef struct {
  char userid[MAX_STR_LEN];
  char name[MAX_STR_LEN];
  EUserType user_type;
  uint64_t openvid;
  EAddType add_type;
  uint32_t add_time;
}person_query_t;

typedef struct {
  char face_feature[FEATURE_LEN];
  char face_key_point[FACE_KEY_POINT];
  float face_norm[2];
  int face_id;
  char pic_large[MAX_PIC_PATH];
  char pic_small[MAX_PIC_PATH];
}person_feat_t;

typedef struct {
  person_query_t base;
  person_feat_t feat;
  char access_card_number[MAX_STR_LEN];
}person_info_t;

typedef struct {
  char userid[MAX_STR_LEN];
  char name[MAX_STR_LEN];
  char pic_large[MAX_PIC_PATH];
  char pic_small[MAX_PIC_PATH];
  char access_card_number[MAX_STR_LEN];
}person_lz_t;

struct struct_bi_item {
  char _key[MAX_STR_LEN];
  char _value[MAX_VAL_LEN];

  struct_bi_item() : _key{0}, _value{0}
  { }

  struct_bi_item(const char* key, int value) {
    strncpy(_key, key, MAX_STR_LEN);
    memset(_value, 0x00, MAX_VAL_LEN);
    snprintf(_value, sizeof(_value), "%d", value);
  }

  struct_bi_item(const char* key, char value) {
    memset(_value, 0x00, MAX_VAL_LEN);
    snprintf(_value, sizeof(_value), "%d", value);
    strncpy(_key, key, MAX_STR_LEN);
  }

  struct_bi_item(const char* key, const char* value) {
    strncpy(_key, key, MAX_STR_LEN);
    memset(_value, 0x00, MAX_VAL_LEN);
    strncpy(_value, value, MAX_VAL_LEN);
  }
  struct_bi_item(const char* key, float value) {
    strncpy(_key, key, MAX_STR_LEN);
    memset(_value, 0x00, MAX_VAL_LEN);
    snprintf(_value, sizeof(_value), "%f", value);
  }
};

typedef struct struct_bi_item bi_item_t;

typedef struct rule_info {
  char userid[MAX_STR_LEN];
  int rule_id;
  char rule[MAX_RULE_LEN];
  int effect_time;
}rule_info_t;

typedef struct record_info {
  char userid[MAX_STR_LEN];
  int user_type;
  int check_in_time;
  int is_uploaded;
  int upload_time;
  char face_path[MAX_IMG_LEN];
  char name[MAX_STR_LEN];
  float temperature;
  char frame_path[MAX_IMG_LEN];
}record_info_t;

typedef struct export_record_info {
  char userid[MAX_STR_LEN];
  char name[MAX_STR_LEN];
  int check_in_time;
}export_record_info_t;

typedef struct record_debug {
    char userid[MAX_STR_LEN];
    int user_type;
    int check_in_time;
    int is_uploaded;
    int upload_time;
    float score;
    char pic_path[MAX_PIC_PATH];
}record_debug_t;

Q_DECLARE_METATYPE(person_query_t)
Q_DECLARE_METATYPE(person_feat_t)
Q_DECLARE_METATYPE(person_info_t)
Q_DECLARE_METATYPE(person_lz_t)
Q_DECLARE_METATYPE(bi_item_t)
Q_DECLARE_METATYPE(rule_info_t)
Q_DECLARE_METATYPE(record_info_t)
Q_DECLARE_METATYPE(export_record_info_t)
Q_DECLARE_METATYPE(record_debug_t)

#endif  //  SQL_HEAD_H
