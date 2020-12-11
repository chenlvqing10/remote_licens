/*
* sqlapi.hpp - define sql type of project
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

#ifndef SQLAPI_H
#define SQLAPI_H

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include "sqlbase.hpp"
#include "person.hpp"

class SqlBase;

/*****************************************************
 *
 *
*****************************************************/
/*****************************************************
 * @brief class SqlApi is for database CURD ops
 * This class is a Singleton class, so you can get an
 * instance by static method " SqlApi::Instance()" to
 * get one instance.
*****************************************************/
class SqlApi {
 public:
  static SqlApi* Instance();

  virtual ~SqlApi();

  int del(SqlBase&);

  int insertBind(const SqlBase& item);
  int replaceBind(const SqlBase& item);
  int insertBind(const QList<SqlBase*>& item_list, bool trans = true);
  int delBind(const SqlBase& item);
  int updateBind(const SqlBase& item);
  int updateBind(const QList<SqlBase*>& item_list, bool trans = true);

  QList<QVariant> find(SqlBase&);
  QList<QVariant> find_by_page(SqlBase&);
  QList<QVariant> findBind(SqlBase&);
  int load_person(Person *_pi, void *face_db,
    int feat_cnt, int index);
  int calc_count(SqlBase&);
  int findPersonInfo(void *data, SqlBase&);
  int findByLimit(void *data, SqlBase&, int, int);
  int getCount(SqlBase&);

 protected:
  SqlApi();
  SqlApi(const SqlApi&) = delete;
  const SqlApi& operator=(const SqlApi&) = delete;
  SqlApi(SqlApi&&) = delete;

 private:
  static SqlApi* _instance;  //  类静态成员的需初始化
  QSqlDatabase db;
  QMutex dbLock;
  class Db_manage {
   public:
    explicit Db_manage(const QSqlDatabase& db, bool isTrans, const bool& state)
        : _db(const_cast<QSqlDatabase *>(&db)),
          _trans(isTrans), _state(const_cast<bool*>(&state)) {
      if (!_db->isOpen()) {
        _db->open();
      }
      if (_trans) {
        _db->transaction();
      }
    }

    ~Db_manage() {
      if (_trans) {
        if (*_state) {
          _db->commit();
        } else {
          _db->rollback();
        }
      }
      if (_db->isOpen())
        _db->close();
    }

   private:
    QSqlDatabase* _db;
    bool _trans;
    bool* _state;
  };
};

#endif  //  SQLAPI_H
