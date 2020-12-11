/*
* biitem.cpp - define interface function of db
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

#include "biitem.hpp"

BiItem::BiItem() {
  setTableName();
}

BiItem::BiItem(bi_item_t &_c) {
  memset(&_rcd, 0x00, sizeof(bi_item_t));
  memcpy(_rcd._key, _c._key, MAX_STR_LEN);
  memcpy(_rcd._value, _c._value, MAX_VAL_LEN);
  setTableName();
}

QString BiItem::find() {
  QString tableName = getTableName();

  QString condition, select;
  if (_queryStr.isEmpty()) select = QString("SELECT key,value FROM %1 ")
      .arg(getTableName());
  else
    select = QString("SELECT key,value FROM %1 WHERE %2")
      .arg(getTableName()).arg(_queryStr);

  return select;
}

QString BiItem::del() {
  QString tableName = getTableName();
  QString del;
  if (_queryStr.isEmpty()) del = QString("DELETE FROM %1")
      .arg(getTableName());
  else
    del = QString("SELECT key,value FROM %1 WHERE %2")
      .arg(getTableName()).arg(_queryStr);

  return del;
}

QString BiItem::update() {
  QString tableName = getTableName();
  QString condition = QString("key='%1'").arg(_rcd._key);
  QString newValue = QString("value='%1'").arg(_rcd._value);
  QString update = QString("UPDATE %1 SET %2 WHERE %3")
    .arg(getTableName()).arg(newValue).arg(condition);

  return update;
}

QString BiItem::insert() {
  QString tableName = getTableName();

  QString insert = QString("INSERT INTO %1 (key,value) VALUES('%2','%3')")
    .arg(getTableName()).arg(_rcd._key).arg(_rcd._value);

  return insert;
}

QString BiItem::getTableName() {
  return _tableName;
}

QVariant BiItem::packageData(QList<QVariant> &metaData) {
  if (metaData.size() != 2) {
    // qDebug() << "BiItem::packageData()";
  }
  bi_item_t item;

  /*
    1-2   "key,value"
  */
  QByteArray str = metaData[0].toByteArray();
  strncpy(item._key, str.data(), MAX_STR_LEN);
  str = metaData[1].toByteArray();
  strncpy(item._value, str.data(), MAX_VAL_LEN);

  return QVariant::fromValue(item);
}

QList<QVariant> BiItem::insertBind() {
  QList<QVariant> res;
  QString sqlStr = QString("INSERT INTO %1 (key,value) VALUES(?,?)").
    arg(getTableName());

  res.append(QVariant(sqlStr));
  res.append(QVariant(_rcd._key));
  res.append(QVariant(_rcd._value));

  return res;
}

QList<QVariant> BiItem::updateBind() {
  QString tableName = getTableName();
  QString condition = QString("key='%1'").arg(_rcd._key);
  QString update = QString("UPDATE %1 SET value=? WHERE %2")
    .arg(getTableName()).arg(condition);

  QList<QVariant> res;
  res.append(QVariant(update));
  res.append(QVariant(_rcd._value));

  return res;
}

bi_item_t& BiItem::getPtr() {
  return _rcd;
}

void BiItem::clearyQuery() {
  _queryStr.clear();
}

BiItem& BiItem::query(QString key) {
  if (!_queryStr.isEmpty()) _queryStr.append(" OR ");
  _queryStr.append(QString("key='%1'").arg(key));

  return *this;
}

bi_item_t& BiItem::get() {
  return _rcd;
}

void BiItem::setTableName(QString tableName) {
  _tableName = tableName;
}

void BiItem::setTableName() {
  _tableName = "BiItem";
}

Sysinfo::Sysinfo() : BiItem() {
  setTableName(SYSINFO_TABLE);
}

Sysinfo::Sysinfo(bi_item_t &item) : BiItem(item) {
  setTableName(SYSINFO_TABLE);
}

Admin::Admin(bi_item_t &item) : BiItem(item) {
  setTableName(ADMIN_TABLE);
}

Admin::Admin() : BiItem() {
  setTableName(ADMIN_TABLE);
}

