/*
* sqlapi.cpp - define interface function of sql
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

#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>
#include <QFile>
#include <algorithm>

#include "db_all.h"
#include "ee_queue.h"
#include "sqlapi.hpp"

SqlApi *SqlApi::_instance = nullptr;

#define PER_NUM_FOR_PERSON 200

SqlApi::SqlApi() {
#if 0
  QFile dbFile("./data/user.db");
  if (!dbFile.exists()) {
    QString srcFileName = QString(SRC_DB_PATH) + "/" + DB_FILE_NAME;
    QString dstFileName = QString(WK_DB_PATH) + "/" + DB_FILE_NAME;
  }
#endif
  db = QSqlDatabase::addDatabase("QSQLITE", "app");
  db.setDatabaseName(DB_FILE_NAME);
}

/*
 * singleton class, don't need copy constructor& assignment constructor
SqlApi::SqlApi(const SqlApi &)
{
    // un_yz_qDebug()<<"copy SqlAPi constructor";
}

const SqlApi &SqlApi::operator=(const SqlApi &)
{
    // un_yz_qDebug()<< "assignment SqlApi constructor";
    return *this;
}
*/

SqlApi* SqlApi::Instance() {
  if (_instance == nullptr) {
    _instance = new SqlApi();
  }

  return _instance;
}

SqlApi::~SqlApi() {
  if (_instance != nullptr) {
    delete _instance;
    // QSqlDatabase::removeDatabase("app");
    _instance = NULL;
  }
}

/*****************************************************
 * @brief delete one record from database
 * delete one record from database
 *
 * @param[in] item
 * this parameter has the information of the func need.
 *
 *
 * @return the numbers of record had effected
 *
*****************************************************/
int SqlApi::del(SqlBase &item) {
  QMutexLocker locker(&dbLock);
  bool state = true;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);

  state &= query.exec(item.del());
  if (query.lastError().isValid()) {
    qDebug() << "SqlApi::del err " << query.lastError();
  }
  int numRowsAffect = query.numRowsAffected();
  query.finish();

  return numRowsAffect;
}

int SqlApi::insertBind(const SqlBase &item) {
  QMutexLocker locker(&dbLock);
  bool state = true;
  Db_manage dm(db, true, state);
  QSqlQuery query(db);
  QList<QVariant> variList = const_cast<SqlBase&>(item).insertBind();
  QString sqlStr;
  if (variList.first().canConvert<QString>()) {
    sqlStr = variList.first().value<QString>();
    variList.pop_front();
  }
  query.prepare(sqlStr);
  int cnt = 0;
  for (auto elem : variList) {
    query.addBindValue(elem);
  }
  state = query.exec();
  if (query.lastError().isValid()) {
    qDebug() << "SqlApi::insertBind err " << query.lastError();
    cnt = -1;
  }
  query.finish();

  return cnt;
}

int SqlApi::replaceBind(const SqlBase &item) {
  QMutexLocker locker(&dbLock);
  bool state = true;
  Db_manage dm(db, true, state);
  QSqlQuery query(db);
  QList<QVariant> variList = const_cast<SqlBase&>(item).updateBind();
  QString sqlStr;
  if (variList.first().canConvert<QString>()) {
    sqlStr = variList.first().value<QString>();
    variList.pop_front();
  }
  query.prepare(sqlStr);
  int cnt = 0;
  for (auto elem : variList) {
    cnt++;
    query.addBindValue(elem);
  }

  state = query.exec();
  if (query.lastError().isValid()) {
    // qDebug() << "SqlApi::replaceBind()";
    qDebug() << "SqlApi::replaceBind err " << query.lastError();
  }
  query.finish();

  return query.lastError().driverText().toInt();
}

int SqlApi::insertBind(const QList<SqlBase *> &item_list, bool trans) {
  QMutexLocker locker(&dbLock);
  bool state = true;
  Db_manage dm(db, trans, state);
  int cnt = 0;
  for ( auto elem : item_list ) {
    QSqlQuery query(db);
    QList<QVariant> variList = elem->insertBind();
    QString sqlStr;
    if (variList.first().canConvert<QString>()) {
      sqlStr = variList.first().value<QString>();
      variList.pop_front();
    }
    query.prepare(sqlStr);
    for (auto elem : variList) {
      query.addBindValue(elem);
    }
    state &= query.exec();
    if (query.lastError().isValid()) {
      qDebug() << "SqlApi::insertBind err " << query.lastError();
      cnt = -1;
      break;
    }
    cnt++;
  }

  return cnt;
}

int SqlApi::delBind(const SqlBase &item) {
  bool state = true;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);
  QList<QVariant> variList = const_cast<SqlBase&>(item).insertBind();
  QString sqlStr;
  if (variList.first().canConvert<QString>()) {
    sqlStr = variList.first().value<QString>();
    variList.pop_front();
  }
  query.prepare(sqlStr);
  int cnt = 0;
  for (auto elem : variList) {
    cnt++;
    query.addBindValue(elem);
  }
  state &= query.exec();
  int numRowsAffect = query.numRowsAffected();
  query.finish();

  return state ? numRowsAffect : -1;
}

int SqlApi::updateBind(const SqlBase &item) {
  QMutexLocker locker(&dbLock);
  bool state = true;
  Db_manage dm(db, true, state);
  QSqlQuery query(db);
  QList<QVariant> variList = const_cast<SqlBase&>(item).updateBind();
  QString sqlStr;
  if (variList.first().canConvert<QString>()) {
    sqlStr = variList.first().value<QString>();
    variList.pop_front();
  }
  query.prepare(sqlStr);
  int cnt = 0;
  for (auto elem : variList) {
    cnt++;
    query.addBindValue(elem);
  }
  state &= query.exec();
  int numRowsAffect = query.numRowsAffected();
  query.finish();

  return state ? numRowsAffect : -1;
}

int SqlApi::updateBind(const QList<SqlBase *> &item_list, bool trans) {
  QMutexLocker locker(&dbLock);
  bool state = true;
  Db_manage dm(db, trans, state);
  int cnt = 0;
  for (auto elem : item_list) {
    QSqlQuery query(db);
    QList<QVariant> variList = elem->updateBind();
    QString sqlStr;
    if (variList.first().canConvert<QString>()) {
      sqlStr = variList.first().value<QString>();
      variList.pop_front();
    }
    query.prepare(sqlStr);
    int cnt = 0;
    for (auto elem : variList) {
      cnt++;
      query.addBindValue(elem);
    }
    state &= query.exec();
    if (query.lastError().isValid()) {
      break;
    }
    cnt++;
  }

  return cnt;
}

int SqlApi::calc_count(SqlBase &parent) {
  int count = 0;
  QMutexLocker locker(&dbLock);
  bool state;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);

  query.setForwardOnly(true);
  state &= query.exec(parent.calc_count());
  while (query.next()) {
    count =  query.value(0).toInt();
  }
  query.clear();
  return count;
}
QList<QVariant> SqlApi::find_by_page(SqlBase & parent) {
  int count = 0;
  int c_index = 0;
  int c_r = 0;
  QList<QVariant> res;
  QMutexLocker locker(&dbLock);
  bool state;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);
  query.setForwardOnly(true);
  state &= query.exec(parent.calc_count());
  while (query.next()) {
    count = query.value(0).toInt();
    break;
  }
  if (count == 0) {
    query.clear();
    return res;
  }
  c_index = count / PER_NUM_FOR_PERSON;
  c_r = count % PER_NUM_FOR_PERSON;
  query.exec("PRAGMA cache_size = 10;");
  for (int n = 0; n < c_index; n++) {
    state &= query.exec(
          parent.findByLimit(n * PER_NUM_FOR_PERSON, PER_NUM_FOR_PERSON));
    while (query.next()) {
      QList<QVariant> metaData;
      QSqlRecord rec = query.record();
      for (int i = 0; i < rec.count(); ++i) {
        metaData.append(query.value(i));
      }
      res.push_back(parent.packageData(metaData));
    }
  }
  if (c_r != 0) {
    state &= query.exec(parent.findByLimit(c_index * PER_NUM_FOR_PERSON, c_r));
    while (query.next()) {
      QList<QVariant> metaData;
      QSqlRecord rec = query.record();
      for (int i = 0; i < rec.count(); ++i) {
        metaData.append(query.value(i));
      }
      res.push_back(parent.packageData(metaData));
    }
  }
  query.clear();
  return res;
}

QList<QVariant> SqlApi::find(SqlBase &parent) {
  QList<QVariant> res;
  QMutexLocker locker(&dbLock);
  bool state;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);

  // query.exec("PRAGMA cache_size = 10;");

  query.setForwardOnly(true);
  state &= query.exec(parent.find());
  while (query.next()) {
    QList<QVariant> metaData;
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) {
      metaData.append(query.value(i));
    }
    res.push_back(parent.packageData(metaData));
  }

  query.finish();

  return res;
}

int SqlApi::load_person(Person *_pi, void *face_db,
  int feat_cnt, int index) {
  int ret = 0, idx = index;
  bool state;

  if (index >= feat_cnt)
    return ret;

  QMutexLocker locker(&dbLock);
  Db_manage dm(db, false, state);
  QSqlQuery query(db);

  query.setForwardOnly(true);
  state &= query.exec(_pi->find());
  while (query.next()) {
    ret = _pi->load_db_mmc(&query, face_db, idx);
    if (0 == ret)
      idx++;
    if (idx >= feat_cnt)
      break;
  }

  query.finish();
  return idx - index;
}

QList<QVariant> SqlApi::findBind(SqlBase &item) {
  QList<QVariant> res;
  QMutexLocker locker(&dbLock);
  bool state;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);

  QList<QVariant> variList = item.updateBind();
  QString sqlStr;
  if (variList.first().canConvert<QString>()) {
    sqlStr = variList.first().value<QString>();
    variList.pop_front();
  }
  query.prepare(sqlStr);
  int cnt = 0;
  for (auto elem : variList) {
    cnt++;
    query.addBindValue(elem);
  }
  state &= query.exec();
  while (query.next()) {
    QList<QVariant> metaData;
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) {
      metaData.append(query.value(i));
    }
    res.push_back(item.packageData(metaData));
  }

  query.finish();

  return res;
}

int SqlApi::findPersonInfo(void *data, SqlBase &parent) {
  int i, ret = 0;
  ee_queue_t *h;
  tag_websocket_person_info *u;
  QMutexLocker locker(&dbLock);
  bool state;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);

  query.exec("PRAGMA cache_size = 10;");
  // state &= query.exec("PRAGMA cache_size = 10;");

  // state &= query.exec("PRAGMA cache_size;");
  // state &= query.exec("PRAGMA page_size;");
  // state &= query.exec("PRAGMA synchronous;");
  // state &= query.exec("PRAGMA temp_store;");
  state &= query.exec(parent.find());

  h =  reinterpret_cast<ee_queue_t *>(data);
  i = 0;
  while (query.next()) {
#if 0
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) {
      qDebug() << i << " " << query.value(i);
    }
#endif

#if 1
    QList<QVariant> metaData;
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) {
      metaData.append(query.value(i));
    }

    u = reinterpret_cast<tag_websocket_person_info *>
          (malloc(sizeof(tag_websocket_person_info)));
    if (NULL == u) {
      break;
    }

    memset(u, 0, sizeof(tag_websocket_person_info));
    QByteArray str = metaData[0].toByteArray();
    snprintf(u->user_id, sizeof(u->user_id), "%s", str.data());
    str = metaData[1].toByteArray();
    snprintf(u->name, sizeof(u->name), "%s", str.data());
    u->user_type = metaData[2].toInt();
    u->add_source = metaData[3].toInt();
    u->add_time = metaData[4].toUInt();
    u->openvid = metaData[5].toULongLong();
    str = metaData[6].toByteArray();
    memcpy(u->face_feature, str.data(), FEATURE_LEN);
    str = metaData[7].toByteArray();
    memcpy(u->key_points, str.data(), 10 * sizeof(float));
    u->face_norm[0] = metaData[8].toFloat();
    u->face_norm[1] = metaData[9].toFloat();

    str = metaData[10].toByteArray();
    snprintf(u->pic_large, sizeof(u->pic_large), "%s", str.data());
    str = metaData[11].toByteArray();
    snprintf(u->pic_small, sizeof(u->pic_small), "%s", str.data());
    u->face_id = metaData[12].toInt();
    ee_queue_insert_tail(h, &(u->queue));

    metaData.clear();
    rec.clear();
#endif
    i++;
  }

  query.finish();
  query.clear();

  return i;
}

int SqlApi::findByLimit(void *data, SqlBase &parent, int index, int count) {
  int i, ret = 0;
  ee_queue_t *h;
  tag_websocket_person_info *u;
  QMutexLocker locker(&dbLock);
  bool state;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);

  // query.exec("PRAGMA cache_size = 10;");
  query.setForwardOnly(true);
  state &= query.exec(parent.findByLimit(index, count));

  h = reinterpret_cast<ee_queue_t *>(data);
  i = 0;
  while (query.next()) {
    QList<QVariant> metaData;
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) {
      metaData.append(query.value(i));
    }

    u = reinterpret_cast<tag_websocket_person_info *>
          (malloc(sizeof(tag_websocket_person_info)));
    if (NULL == u) {
      break;
    }

    memset(u, 0, sizeof(tag_websocket_person_info));
    QByteArray str = metaData[0].toByteArray();
    snprintf(u->user_id, sizeof(u->user_id), "%s", str.data());
    str = metaData[1].toByteArray();
    snprintf(u->name, sizeof(u->name), "%s", str.data());
    u->user_type = metaData[2].toInt();
    u->add_source = metaData[3].toInt();
    u->add_time = metaData[4].toUInt();
    u->openvid = metaData[5].toULongLong();
    str = metaData[6].toByteArray();
    memcpy(u->face_feature, str.data(), FEATURE_LEN);
    str = metaData[7].toByteArray();
    memcpy(u->key_points, str.data(), 10 * sizeof(float));
    u->face_norm[0] = metaData[8].toFloat();
    u->face_norm[1] = metaData[9].toFloat();

    str = metaData[10].toByteArray();
    snprintf(u->pic_large, sizeof(u->pic_large), "%s", str.data());
    str = metaData[11].toByteArray();
    snprintf(u->pic_small, sizeof(u->pic_small), "%s", str.data());
    u->face_id = metaData[12].toInt();
    ee_queue_insert_tail(h, &(u->queue));

    metaData.clear();
    rec.clear();

    i++;
  }

  query.finish();
  query.clear();

  return i;
}

int SqlApi::getCount(SqlBase &parent) {
  int count = 0;
  QMutexLocker locker(&dbLock);
  bool state;
  Db_manage dm(db, false, state);
  QSqlQuery query(db);

  // query.exec("PRAGMA cache_size = 10;");
  query.setForwardOnly(true);
  state &= query.exec(parent.getCount());

  while (query.next()) {
    QSqlRecord rec = query.record();
    count = query.value(0).toInt();
    //qDebug() << "SqlApi::getCount() count " << count;
  }

  query.finish();
  query.clear();

  return count;
}

