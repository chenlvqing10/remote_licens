/*
* exportcheckin.cpp - define interface function of sql
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

#include "exportcheckin.hpp"

ExportCheckin::ExportCheckin(const export_record_info_t &c) {
  memcpy(&_rcd, &c, sizeof(export_record_info_t));
}

ExportCheckin& ExportCheckin::query(int index, int count) {
  if (!_queryStr.isEmpty()) _queryStr.append("LIMIT %1,%2");
  _queryStr.append(QString("LIMIT %1,%2").arg(index).arg(count));

  return *this;
}

ExportCheckin& ExportCheckin::query(QString query) {
  if (_queryStr.isEmpty())
    _queryStr = QString(" WHERE ");
  else
    _queryStr.append(" AND ");
  _queryStr.append(query);

  return *this;
}

ExportCheckin& ExportCheckin::limit(QString limit) {
  _queryStr.append(limit);

  return *this;
}

QString ExportCheckin::find() {
  //TODO: add output face_path | frame_path
  QString select = QString("SELECT userid,name,check_in_time FROM "CHECKIN_TABLE);
  if (!_queryStr.isEmpty())
    select.append(_queryStr);

  return select;
}

QString ExportCheckin::getCount() {
  QString select = QString("SELECT count(*) FROM "CHECKIN_TABLE);
  if (!_queryStr.isEmpty())
    select.append(_queryStr);

  return select;
}

QVariant ExportCheckin::packageData(QList<QVariant> &metaData) {
  export_record_info_t item;

  QByteArray str = metaData[0].toByteArray();
  strncpy(item.userid, str.data(), MAX_STR_LEN);
  QByteArray str1 = metaData[1].toByteArray();
  strncpy(item.name, str1.data(), MAX_STR_LEN);
  item.check_in_time = metaData[2].toInt();

  return QVariant::fromValue(item);
}

void ExportCheckin::clearQuery() {
  _queryStr.clear();
}

QString ExportCheckin::getTableName() {
  return CHECKIN_TABLE;
}

