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

#include "sql_head.hpp"
#include "checkinDbg.hpp"

CheckinDbg::CheckinDbg(const record_debug_t &c) {
  memcpy(&_rcd, &c, sizeof(record_debug_t));
}

CheckinDbg& CheckinDbg::query(QString key, QString value) {
  if (!_queryStr.isEmpty()) _queryStr.append(" AND ");
  _queryStr.append(QString("%1='%2'").arg(key).arg(value));

  return *this;
}

CheckinDbg& CheckinDbg::query(QString key, uint32_t value) {
  if (!_queryStr.isEmpty()) _queryStr.append(" AND ");
  _queryStr.append(QString("%1=%2").arg(key).arg(value));

  return *this;
}

QString CheckinDbg::insert() {
  QString insert = QString("INSERT INTO %1 (userid,"
                           "user_type,check_in_time,is_uploaded, "
                           "upload_time, score, pic_path)"
                           "VALUES('%2',%3, %4,%5,%6, %7, %8)")
    .arg(getTableName())
    .arg(_rcd.userid).arg(_rcd.user_type).arg(_rcd.check_in_time)
    .arg(_rcd.is_uploaded).arg(_rcd.upload_time)
    .arg(_rcd.score).arg(_rcd.pic_path);

  return insert;
}

QString CheckinDbg::update() {
  QString tableName = getTableName();
  QString condition = QString("userid='%1'").arg(_rcd.userid);
  QString newValue = QString("user_type=%1,"
                             "check_in_time=%2,is_uploaded=%3, upload_time=%4,"
                             "score=%5, pic_path=%6")
    .arg(_rcd.user_type).arg(_rcd.check_in_time)
    .arg(_rcd.is_uploaded).arg(_rcd.upload_time)
    .arg(_rcd.score).arg(_rcd.pic_path);
  QString update = QString("UPDATE %1 SET %2 WHERE %3")
    .arg(getTableName()).arg(newValue).arg(condition);

  return update;
}

QString CheckinDbg::del() {
  QString tableName = getTableName();
  if (_queryStr.isEmpty()) {
    _queryStr = QString("1=1");
  }
  QString del = QString("DELETE FROM %1 WHERE %2").
    arg(getTableName()).arg(_queryStr);

  return del;
}

QString CheckinDbg::find() {
  QString tableName = getTableName();
  QString format = QString("userid,user_type,"
                           "check_in_time,is_uploaded, upload_time,"
                           "score, pic_path");
  QString select = QString("SELECT %1 FROM %2 ")
    .arg(format).arg(getTableName());
  if (!_queryStr.isEmpty()) select.append("WHERE " + _queryStr);

  return select;
}

QList<QVariant> CheckinDbg::insertBind() {
  QList<QVariant> res;

  QString sqlStr = QString("INSERT INTO %1 (userid,user_type,"
                           "check_in_time,is_uploaded, upload_time,"
                           "score, pic_path) "
                           "VALUES(?,?,?,?,?,?,?)")
    .arg(getTableName());

  res.append(QVariant(sqlStr));
  res.append(QVariant(_rcd.userid));
  res.append(QVariant(_rcd.user_type));
  res.append(QVariant(_rcd.check_in_time));
  res.append(QVariant(_rcd.is_uploaded));
  res.append(QVariant(_rcd.upload_time));
  res.append(QVariant(_rcd.score));
  res.append(QVariant(_rcd.pic_path));

  return res;
}

QVariant CheckinDbg::packageData(QList<QVariant> &metaData) {
  record_debug_t item;

  /*
    1-6   "userid,rule_id,rule,effect_time,score,pic_path"
  */
  QByteArray str = metaData[0].toByteArray();
  strncpy(item.userid, str.data(), MAX_STR_LEN);
  item.user_type = static_cast<EUserType>(metaData[1].toUInt());
  item.check_in_time = metaData[2].toInt();
  item.is_uploaded = metaData[3].toInt();
  item.upload_time = metaData[4].toInt();
  item.score = metaData[5].toFloat();
  strncpy(item.pic_path, metaData[6].toByteArray().data(), MAX_PIC_PATH);
  return QVariant::fromValue(item);
}

void CheckinDbg::clearQuery() {
  _queryStr.clear();
}

QString CheckinDbg::getTableName() {
  return CHECKINDBG_TABLE;
}

