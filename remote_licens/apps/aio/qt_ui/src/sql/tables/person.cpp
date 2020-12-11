/*
* person.cpp - define interface function of sql for person
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
#include <QList>
#include <QDebug>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LOG_TAG "person"
#include <log/log.h>

#include "face.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "person.hpp"

Person_LZ::Person_LZ():
  _tableName(QString(EMPLOYEE_TABLE)) {
  _queryStr.clear();
}

Person_LZ& Person_LZ::query(QString key, QString value) {
  if (!_queryStr.isEmpty()) {
    _queryStr.append(" AND ");
  }
  _queryStr += QString("%1='%2'").arg(key).arg(value);

  return *this;
}

Person_LZ& Person_LZ::query(QString key, uint64_t value) {
  if (!_queryStr.isEmpty()) {
    _queryStr.append(" AND ");
  }
  _queryStr += QString("%1=%2").arg(key).arg(value);
  return *this;
}

Person_LZ& Person_LZ::query(QString query_st) {
  if (!_queryStr.isEmpty()) {
    _queryStr.append(" AND ");
  }
  _queryStr += query_st;

  return *this;
}

void Person_LZ::clearQuery() {
  _queryStr.clear();
}

void Person_LZ::setLimit(int index, int count) {
  qlimit = QString(" limit %1,%2").arg(index).arg(count);
}

QString Person_LZ::del() {
  QString del;
  if (_queryStr.isEmpty()) {
    del = QString("DELETE FROM %1").arg(_tableName);
  } else {
    del = QString("DELETE FROM %1 WHERE %2").arg(_tableName).arg(_queryStr);
  }

  return del;
}

QString Person_LZ::find() {
  QString format = QString("userid,name,pic_large,pic_small,access_card_number");
  QString select = QString("SELECT %1 FROM %2")
    .arg(format).arg(_tableName);
  if (!_queryStr.isEmpty())
    select.append(" WHERE " + _queryStr);
  select.append(" ORDER BY userid ASC");
  if (!qlimit.isEmpty())
    select.append(qlimit);
  return select;
}

QString Person_LZ::getCount() {
  QString select = QString("SELECT count(*) FROM %1")
    .arg(_tableName);
  if (!_queryStr.isEmpty())
    select.append(" WHERE " + _queryStr);

  return select;
}

void Person_LZ::setTableName(QString tableName) {
  _tableName = tableName;
}

QString Person_LZ::getTableName() {
  return _tableName;
}

QVariant Person_LZ::packageData(QList<QVariant> &metaData) {
  person_lz_t item;

  QByteArray str = metaData[0].toByteArray();
  strncpy(item.userid, str.data(), MAX_STR_LEN);
  str = metaData[1].toByteArray();
  strncpy(item.name, str.data(), MAX_STR_LEN);
  str = metaData[2].toByteArray();
  strncpy(item.pic_large, str.data(), MAX_PIC_PATH);
  str = metaData[3].toByteArray();
  strncpy(item.pic_small, str.data(), MAX_PIC_PATH);
  str = metaData[4].toByteArray();
  strncpy(item.access_card_number, str.data(), MAX_PIC_PATH);
  return QVariant::fromValue(item);
}

Person::Person(person_info_t &item) {
  lazyMode = 0;
  index = 0;
  count = 0;
  memcpy(&_rcd, &item, sizeof(person_info_t));
  if (_rcd.base.user_type == 0) {
    setTableName(EMPLOYEE_TABLE);
  } else {
    setTableName(GUEST_TABLE);
  }
}

Person::Person(person_query_t &base) {
  lazyMode = 0;
  index = 0;
  count = 0;
  memcpy(&_rcd, &base, sizeof(person_query_t));
}

void Person::setPageMode(int index, int count){
  this->index = index;
  this->count = count;
}

person_info_t Person::getOnePerson() {
  person_info_t person;

  return person;
}

Person& Person::query(QString key, QString value) {
  if (!_queryStr.isEmpty()) {
    _queryStr.append(" AND ");
  }
  _queryStr += QString("%1='%2'").arg(key).arg(value);

  return *this;
}

Person& Person::query(QString key, uint64_t value) {
  if (!_queryStr.isEmpty()) {
    _queryStr.append(" AND ");
  }
  _queryStr += QString("%1=%2").arg(key).arg(value);

  return *this;
}

void Person::clearQuery() {
  _queryStr.clear();
}

QString Person::insert() {
  QString insert = QString("INSERT INTO %1 (userid,name,user_type,face_feature,"
                     "face_key_points, face_norm0, face_norm1, pic_large,"
                     "pic_small, openvid, add_type, add_time, face_id, access_card_number)"
                     "VALUES('%2','%3', %4,'%5',"
                     "'%6', %7, %8, '%9',"
                     "'%10',%11, %12, %13, %14, %15)")
    .arg(getTableName())
    .arg(_rcd.base.userid).arg(_rcd.base.name).arg(_rcd.base.user_type).
    arg(_rcd.feat.face_feature)
    .arg(_rcd.feat.face_key_point).arg(_rcd.feat.face_norm[0]).
    arg(_rcd.feat.face_norm[1]).arg(_rcd.feat.pic_large)
    .arg(_rcd.feat.pic_small).arg(_rcd.base.openvid).
    arg(_rcd.base.add_type).arg(_rcd.base.add_time).arg(_rcd.feat.face_id);

  return insert;
}

QString Person::update() {
  QString tableName = getTableName();
  QString condition = QString("userid='%1'").arg(_rcd.base.userid);
  QString newValue = QString("name='%1',user_type='%2',face_feature='%3',"
                             "face_key_points='%4', "
                             "face_norm0=%5, face_norm1=%6, pic_large='%7', "
                             "pic_small='%8', openvid=%9, add_type=%10, "
                             "add_time=%11, face_id=%12, access_card_number=%13)")
    .arg(_rcd.base.name).arg(_rcd.base.user_type).
    arg(_rcd.feat.face_feature)
    .arg(_rcd.feat.face_key_point).arg(_rcd.feat.face_norm[0]).
    arg(_rcd.feat.face_norm[1]).arg(_rcd.feat.pic_large)
    .arg(_rcd.feat.pic_small).arg(_rcd.base.openvid).arg(_rcd.base.add_type).
    arg(_rcd.base.add_time).arg(_rcd.feat.face_id).arg(_rcd.access_card_number);
  QString update = QString("UPDATE %1 SET %2 WHERE %3")
    .arg(getTableName()).arg(newValue).arg(condition);

  return update;
}

QString Person::del() {
  QString tableName = getTableName();
  QString del;
  if (_queryStr.isEmpty()) {
    del = QString("DELETE FROM %1").arg(getTableName());
  } else {
    del = QString("DELETE FROM %1 WHERE %2").arg(getTableName()).arg(_queryStr);
  }

  return del;
}

QString Person::calc_count() {
  QString tableName = getTableName();
  QString select = QString("SELECT count(*) FROM %1")
      .arg(getTableName());
  return select;
}

QString Person::find() {
  QString tableName = getTableName();
  QString format;
  if (lazyMode) {
    format = QString("userid,name");
  } else {
    format = QString("userid,name,user_type,add_type,add_time,openvid,"
                           "face_feature,face_key_points,face_norm0,face_norm1,"
                           "pic_large,pic_small, face_id, access_card_number");
  }
  QString select = QString("SELECT %1 FROM %2")
    .arg(format).arg(getTableName());
  if (!_queryStr.isEmpty()) {
    select.append(" WHERE " + _queryStr);
  }
  if(0 != count) {
    QString order = QString(" ORDER BY add_time ASC LIMIT %1,%2")
      .arg(index).arg(count);
    select.append(order);
  }

  return select;
}

QString Person::findByLimit(int index, int count) {
  QString tableName = getTableName();
  QString format = QString("userid,name,user_type,add_type,add_time,openvid,"
                           "face_feature,face_key_points,face_norm0,face_norm1,"
                           "pic_large,pic_small, face_id, access_card_number");
  QString select = QString("SELECT %1 FROM %2")
    .arg(format).arg(getTableName());
  QString limit = QString("%1,%2").arg(index).arg(count);
  select.append(" limit " + limit);

  return select;
}

QString Person::getCount() {
  QString tableName = getTableName();
  QString select = QString("SELECT count(*) FROM %1")
    .arg(getTableName());
  if (!_queryStr.isEmpty()) {
    select.append(" WHERE " + _queryStr);
  }

  return select;
}

QString Person::replace() {
  QString replace = QString("REPLACE INTO %1 (userid,name,"
                            "user_type,face_feature,"
                            "face_key_points, face_norm0, "
                            "face_norm1, pic_large, "
                            "pic_small, openvid, add_type, "
                            "add_time, face_id, access_card_number) "
                            "VALUES('%2','%3', %4,'%5',"
                            "'%6', %7, %8, '%9',"
                            "'%10',%11, %12, %13,%14,%15)")
    .arg(getTableName())
    .arg(_rcd.base.userid).arg(_rcd.base.name).
    arg(_rcd.base.user_type).arg(_rcd.feat.face_feature)
    .arg(_rcd.feat.face_key_point).arg(_rcd.feat.face_norm[0]).
    arg(_rcd.feat.face_norm[1]).arg(_rcd.feat.pic_large)
    .arg(_rcd.feat.pic_small).arg(_rcd.base.openvid).
    arg(_rcd.base.add_type).arg(_rcd.base.add_time)
    .arg(_rcd.feat.face_id).arg(_rcd.access_card_number);

  return replace;
}

QList<QVariant> Person::insertBind() {
  QList<QVariant> res;

  QString sqlStr = QString("INSERT INTO %1 (userid,name,user_type,face_feature,"
                     "face_key_points, face_norm0, face_norm1, pic_large,"
                     "pic_small, openvid, add_type, add_time, face_id, access_card_number)"
                     "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)")
    .arg(getTableName());

  res.append(QVariant(sqlStr));
  res.append(QVariant(_rcd.base.userid));
  res.append(QVariant(_rcd.base.name));
  res.append(QVariant(_rcd.base.user_type));
  res.append(QVariant::fromValue(
    QByteArray(_rcd.feat.face_feature, FEATURE_LEN)));
  res.append(QVariant::fromValue(
    QByteArray(_rcd.feat.face_key_point, FACE_KEY_POINT)));
  res.append(QVariant(_rcd.feat.face_norm[0]));
  res.append(QVariant(_rcd.feat.face_norm[1]));
  res.append(QVariant(_rcd.feat.pic_large));
  res.append(QVariant(_rcd.feat.pic_small));
  res.append(QVariant(_rcd.base.openvid));
  res.append(QVariant(_rcd.base.add_type));
  res.append(QVariant(_rcd.base.add_time));
  res.append(QVariant(_rcd.feat.face_id));
  res.append(QVariant(_rcd.access_card_number));

  return res;
}

QList<QVariant> Person::replaceBind() {
}

QList<QVariant> Person::updateBind() {
  QList<QVariant> res;

  QString condition = QString("userid='%1' AND face_id=%2").
    arg(_rcd.base.userid).arg(_rcd.feat.face_id);
  QString newValue = QString("name=?,user_type=?,face_feature=?,"
                             "face_key_points=?, face_norm0=?, "
                             "face_norm1=?, pic_large=?, "
                             "pic_small=?, openvid=?, "
                             "add_type=?, add_time=?, access_card_number=?");
  QString update = QString("UPDATE %1 SET %2 WHERE %3")
    .arg(getTableName()).arg(newValue).arg(condition);

  res.append(QVariant(update));
  res.append(QVariant(_rcd.base.name));
  res.append(QVariant(_rcd.base.user_type));
  res.append(QVariant::fromValue(
    QByteArray(_rcd.feat.face_feature, FEATURE_LEN)));
  res.append(QVariant::fromValue(
    QByteArray(_rcd.feat.face_key_point, FACE_KEY_POINT)));
  res.append(QVariant(_rcd.feat.face_norm[0]));
  res.append(QVariant(_rcd.feat.face_norm[1]));
  res.append(QVariant(_rcd.feat.pic_large));
  res.append(QVariant(_rcd.feat.pic_small));
  res.append(QVariant(_rcd.base.openvid));
  res.append(QVariant(_rcd.base.add_type));
  res.append(QVariant(_rcd.base.add_time));
  res.append(QVariant(_rcd.access_card_number));

  return res;
}

int Person::load_db_mmc(QSqlQuery *_db, void *_mmc, int index) {
  QByteArray str;

  face_db_t *face_db = (face_db_t *)_mmc;
#if ATTENDANCE_MODE != 2
  if (_db->value(10).toByteArray().isEmpty() &&
    _db->value(11).toByteArray().isEmpty()) {
    ALOGW("%s:%d >> userid[%s]='%s' pic_large is null, loading failed!!",
      __func__, __LINE__, _db->value(0).toByteArray().data(),
      _db->value(1).toByteArray().data());
    return -1;
  }
#endif
  str = _db->value(0).toByteArray();
  strncpy(face_db->userid[index].userid, str.data(), MAX_STR_LEN);
  str = _db->value(1).toByteArray();
  strncpy(face_db->name[index].name, str.data(), PERSON_NAME_MAX);
  face_db->type[index].user_type = _db->value(2).toInt();
  str = _db->value(6).toByteArray();
  memcpy(face_db->feat[index].values, str.data(), FEATURE_LEN);
  str = _db->value(7).toByteArray();
  memcpy(face_db->kpts[index].kpts, str.data(), FACE_KEY_POINT);
  face_db->norm[index * 2] = _db->value(8).toFloat();
  face_db->norm[index * 2 + 1] = _db->value(9).toFloat();
  return 0;
}

QVariant Person::packageData(QList<QVariant> &metaData) {
  if (metaData.size() != 13) {
    // qDebug() << "metaData " << metaData.size();
  }
  person_info_t item;
  memset(&item, 0, sizeof(person_info_t));

  /*
    1-6   "userid,name,user_type,add_type,add_time,openvid,"
    7-10  "face_feature,face_key_points,face_norm0,face_norm1,"
    11-12 "pic_large,pic_small"
  */
  QByteArray str = metaData[0].toByteArray();
  strncpy(item.base.userid, str.data(), MAX_STR_LEN);
  str = metaData[1].toByteArray();
  strncpy(item.base.name, str.data(), MAX_STR_LEN);
  if (lazyMode) {
    return (QVariant::fromValue(item));
  }

  item.base.user_type = static_cast<EUserType>(metaData[2].toInt());
  item.base.add_type = static_cast<EAddType>(metaData[3].toInt());
  item.base.add_time = metaData[4].toUInt();
  item.base.openvid = metaData[5].toULongLong();

  str = metaData[6].toByteArray();
  memcpy(item.feat.face_feature, str.data(), FEATURE_LEN);

  str = metaData[7].toByteArray();
  memcpy(item.feat.face_key_point, str.data(), FACE_KEY_POINT);

  item.feat.face_norm[0] = metaData[8].toFloat();
  item.feat.face_norm[1] = metaData[9].toFloat();

  str = metaData[10].toByteArray();
  strncpy(item.feat.pic_large, str.data(), MAX_PIC_PATH);
  str = metaData[11].toByteArray();
  strncpy(item.feat.pic_small, str.data(), MAX_PIC_PATH);
  int face_id = metaData[12].toInt();
  memcpy(&item.feat.face_id, &face_id, sizeof(int));
  str = metaData[13].toByteArray();
  strncpy(item.access_card_number, str.data(), MAX_STR_LEN);

  return (QVariant::fromValue(item));
}

void Person::setTableName(QString tableName) {
  _tableName = tableName;
}

QString Person::getTableName() {
  return _tableName;
}

