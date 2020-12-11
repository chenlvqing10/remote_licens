/*
* rule.cpp - define interface function of sql for rule
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

#include "rule.hpp"

Rule::Rule(const rule_info_t &c) {
  memcpy(&_rcd, &c, sizeof(rule_info_t));
}

Rule& Rule::query(QString key, QString value) {
  if (!_queryStr.isEmpty()) {
    _queryStr.append(" AND ");
  }
  _queryStr.append(QString("%1='%2'").arg(key).arg(value));

  return *this;
}

Rule& Rule::query(QString key, uint32_t value) {
  if (!_queryStr.isEmpty()) {
    _queryStr.append(" AND ");
  }
  _queryStr.append(QString("%1=%2").arg(key).arg(value));

  return *this;
}

QString Rule::insert() {
  QString insert = QString("INSERT INTO %1 (userid,rule_id,rule,effect_time)"
                           "VALUES('%2',%3, '%4',%5)")
    .arg(getTableName())
    .arg(_rcd.userid).arg(_rcd.rule_id).arg(_rcd.rule).arg(_rcd.effect_time);

  return insert;
}

QString Rule::update() {
  QString tableName = getTableName();
  QString condition = QString("userid='%1'").arg(_rcd.userid);
  QString newValue = QString("rule_id=%1,rule='%2',effect_time=%3")
    .arg(_rcd.rule_id).arg(_rcd.rule).arg(_rcd.effect_time);
  QString update = QString("UPDATE %1 SET %2 WHERE %3")
    .arg(getTableName()).arg(newValue).arg(condition);

  return update;
}

QString Rule::del() {
  QString tableName = getTableName();
  if (_queryStr.isEmpty()) {
    // qDebug() << "Rule::del()";
  }
  QString del = QString("DELETE FROM %1 WHERE %2").
      arg(getTableName()).arg(_queryStr);

  return del;
}

QString Rule::find() {
  QString tableName = getTableName();
  QString format = QString("userid,rule_id,rule,effect_time");
  QString select = QString("SELECT %1 FROM %2 ")
    .arg(format).arg(getTableName());
  if (!_queryStr.isEmpty()) {
    select.append("WHERE " + _queryStr);
  }

  return select;
}

QList<QVariant> Rule::insertBind() {
  QList<QVariant> res;

  QString sqlStr = QString("INSERT INTO %1 (userid,rule_id,rule,effect_time)"
                           "VALUES(?,?,?,?)")
    .arg(getTableName());

  res.append(QVariant(sqlStr));
  res.append(QVariant(_rcd.userid));
  res.append(QVariant(_rcd.rule_id));
  res.append(QVariant(_rcd.rule));
  res.append(QVariant(_rcd.effect_time));

  return res;
}

QList<QVariant> Rule::updateBind() {
  QList<QVariant> res;
  QString condition = QString("userid='%1'").arg(_rcd.userid);
  QString newValue = QString("rule_id=?,rule=?,effect_time=?");
  QString sqlStr = QString("UPDATE %1 SET %2 WHERE %3")
    .arg(getTableName()).arg(newValue).arg(condition);

  res.append(QVariant(sqlStr));
  res.append(QVariant(_rcd.rule_id));
  res.append(QVariant(_rcd.rule));
  res.append(QVariant(_rcd.effect_time));

  return res;
}

QVariant Rule::packageData(QList<QVariant> &metaData) {
  if (metaData.size() != 4) {
    // qDebug() << "Rule::packageData()";
  }
  rule_info_t item;

  /*
    1-4   "userid,rule_id,rule,effect_time"
  */
  QByteArray str = metaData[0].toByteArray();
  strncpy(item.userid, str.data(), MAX_STR_LEN);
  item.rule_id = metaData[1].toInt();

  str = metaData[2].toByteArray();
  strncpy(item.rule, str.data(), MAX_RULE_LEN);

  item.effect_time = metaData[3].toInt();

  return QVariant::fromValue(item);
}

void Rule::clearQuery() {
  _queryStr.clear();
}

QString Rule::getTableName() {
  return RULE_TABLE;
}

