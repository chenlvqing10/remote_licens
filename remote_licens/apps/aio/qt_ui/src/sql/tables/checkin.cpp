/*
* checkin.cpp - define interface function of sql
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

#include "checkin.hpp"

Checkin::Checkin(const record_info_t &c) {
  memcpy(&_rcd, &c, sizeof(record_info_t));
  lastCount = 0;
  firstCount = 0;
}

Checkin& Checkin::query(QString key, QString value) {
  if (!_queryStr.isEmpty()) _queryStr.append(" AND ");
  _queryStr.append(QString("%1='%2'").arg(key).arg(value));

  return *this;
}

Checkin& Checkin::query(QString key, uint32_t value) {
  if (!_queryStr.isEmpty()) _queryStr.append(" AND ");
  _queryStr.append(QString("%1=%2").arg(key).arg(value));

  return *this;
}

Checkin& Checkin::query(QString query) {
  if (!_queryStr.isEmpty()) _queryStr.append(" AND ");
  _queryStr.append(query);

  return *this;
}

QString Checkin::getCount() {
  QString tableName = getTableName();
  QString select = QString("SELECT count(*) FROM %1")
    .arg(getTableName());
  if (!_queryStr.isEmpty())
    select.append(" WHERE " + _queryStr);
  if (!qlimit.isEmpty())
    select.append(qlimit);

  return select;
}

void Checkin::set_last_count(int count) {
  lastCount = count;
}

void Checkin::set_first_count(int count) {
  firstCount = count;
}

void Checkin::set_limit(int index, int count) {
  qlimit = QString(" LIMIT %1,%2").arg(index).arg(count);
}

QString Checkin::insert() {
  QString insert = QString("INSERT INTO %1 (userid,user_type,check_in_time,"
                           "is_uploaded,upload_time,face_path,name,temperature,frame_path)"
                           "VALUES('%2',%3, %4,%5,%6,'%7','%8',%9,'%10')")
    .arg(getTableName())
    .arg(_rcd.userid).arg(_rcd.user_type).arg(_rcd.check_in_time)
    .arg(_rcd.is_uploaded).arg(_rcd.upload_time).arg(_rcd.face_path)
    .arg(_rcd.name).arg(_rcd.temperature).arg(_rcd.frame_path);

  return insert;
}

QString Checkin::update() {
  QString tableName = getTableName();
  QString condition = QString("userid='%1'").arg(_rcd.userid);
  QString newValue = QString("user_type=%1, check_in_time=%2, is_uploaded=%3,"
                             "upload_time=%4, face_path='%5',"
                             "name='%6', temperature=%7, frame_path='%8'")
    .arg(_rcd.user_type).arg(_rcd.user_type).arg(_rcd.check_in_time)
    .arg(_rcd.upload_time).arg(_rcd.face_path).arg(_rcd.name)
    .arg(_rcd.temperature).arg(_rcd.frame_path);
  QString update = QString("UPDATE %1 SET %2 WHERE %3")
    .arg(getTableName()).arg(newValue).arg(condition);

  return update;
}

QString Checkin::del() {
  QString tableName = getTableName();
  QString del;
  if (!_queryStr.isEmpty()) {
    del = QString("DELETE FROM %1 WHERE %2")
      .arg(tableName).arg(_queryStr);
  } else if (0 != lastCount) {
    QString select = QString(
      "SELECT check_in_time FROM %1 ORDER BY check_in_time ASC LIMIT 0,%2")
      .arg(tableName).arg(lastCount);
    del = QString("DELETE FROM %1 WHERE check_in_time in (%2)")
      .arg(tableName).arg(select);
  } else {
    del = QString("DELETE FROM %1 WHERE check_in_time=%2")
      .arg(tableName).arg(_rcd.check_in_time);
  }

  return del;
}

QString Checkin::find() {
  QString tableName = getTableName();
  QString order;
  QString format = QString("userid,user_type,check_in_time,is_uploaded,"
                           "upload_time,face_path,name,temperature,frame_path");
  QString select = QString("SELECT %1 FROM %2")
    .arg(format).arg(getTableName());
  if (!_queryStr.isEmpty()) select.append(" WHERE " + _queryStr);
  if (0 != lastCount) {
    order = QString(" ORDER BY check_in_time ASC LIMIT 0,%1")
      .arg(lastCount);
    select.append(order);
  } else if (0 != firstCount) {
    order = QString(" ORDER BY check_in_time DESC LIMIT 0,%1")
      .arg(firstCount);
    select.append(order);
  } else {
    select.append(" ORDER BY check_in_time DESC");
    if (!qlimit.isEmpty())
      select.append(qlimit);
  }

  return select;
}

QList<QVariant> Checkin::insertBind() {
  QList<QVariant> res;

  QString sqlStr = QString("INSERT INTO %1 (userid,user_type,check_in_time,"
                           "is_uploaded,upload_time,face_path,name,temperature,frame_path)"
                           "VALUES(?,?,?,?,?,?,?,?,?)")
    .arg(getTableName());

  res.append(QVariant(sqlStr));
  res.append(QVariant(_rcd.userid));
  res.append(QVariant(_rcd.user_type));
  res.append(QVariant(_rcd.check_in_time));
  res.append(QVariant(_rcd.is_uploaded));
  res.append(QVariant(_rcd.upload_time));
  res.append(QVariant(_rcd.face_path));
  res.append(QVariant(_rcd.name));
  res.append(QVariant(_rcd.temperature));
  res.append(QVariant(_rcd.frame_path));

  return res;
}

QVariant Checkin::packageData(QList<QVariant> &metaData) {
  record_info_t item;

  /*
    1-4   "userid,rule_id,rule,effect_time"
  */
  QByteArray str = metaData[0].toByteArray();
  strncpy(item.userid, str.data(), MAX_STR_LEN);
  item.user_type = static_cast<EUserType>(metaData[1].toUInt());
  item.check_in_time = metaData[2].toInt();
  item.is_uploaded = metaData[3].toInt();
  item.upload_time = metaData[4].toInt();
  str = metaData[5].toByteArray();
  strncpy(item.face_path, str.data(), 128);
  str = metaData[6].toByteArray();
  strncpy(item.name, str.data(), MAX_STR_LEN);
  item.temperature = metaData[7].toFloat();
  str = metaData[8].toByteArray();
  strncpy(item.frame_path, str.data(), 128);

  return QVariant::fromValue(item);
}

void Checkin::clearQuery() {
  _queryStr.clear();
}

QString Checkin::getTableName() {
  return CHECKIN_TABLE;
}

