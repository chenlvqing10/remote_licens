/*
* person.hpp - define sql type of project
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


#ifndef PERSON_H
#define PERSON_H

#include "sqlbase.hpp"

class Person_LZ : public SqlBase {
 public:
  explicit Person_LZ();
  virtual ~Person_LZ() { }

  Person_LZ& query(QString query_st);
  Person_LZ& query(QString key, QString value);
  Person_LZ& query(QString key, uint64_t value);
  void clearQuery();
  void setLimit(int index, int count);

  QString del()override;
  QString find()override;
  QString getCount();

  void setTableName(QString);
  QString getTableName()override;
  QVariant packageData(QList<QVariant>& metaData)override;

 private:
  person_lz_t _rcd;
  QString _tableName;
  QString _queryStr;
  QString qlimit;
};

class Person : public SqlBase {
 public:
  explicit Person(person_info_t&);
  explicit Person(person_query_t&);
  Person():lazyMode(0), index(0),count(0) { }
  virtual ~Person() { }

  static person_info_t getOnePerson();

  Person& query(QString key, QString value);
  Person& query(QString key, uint64_t value);
  void clearQuery();
  void setLazyMode(){
    lazyMode = 1;
  }
  void setPageMode(int index, int count);

  QString insert()override;
  QString update()override;
  QString del()override;
  QString find()override;
  QString findByLimit(int index, int count);
  QString getCount();
  QString calc_count()override;
  QString replace()override;

  QList<QVariant> insertBind()override;
  QList<QVariant> replaceBind()override;
  QList<QVariant> updateBind()override;
  int load_db_mmc(QSqlQuery *_db, void *_mmc, int index);
  QVariant packageData(QList<QVariant>& metaData)override;

  void setTableName(QString);
  QString getTableName()override;

 private:
  int lazyMode;
  int index, count;
  person_info_t _rcd;
  QString _tableName;
  QString _queryStr;
};

#endif  //  PERSON_H
