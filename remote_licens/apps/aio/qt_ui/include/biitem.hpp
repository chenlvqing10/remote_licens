/*
* BiItem.hpp - define sql type of project
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

#ifndef BIITEM_H
#define BIITEM_H

#include "sql_head.hpp"
#include "sqlbase.hpp"

class BiItem : public SqlBase {
 public:
  BiItem();
  Q_INVOKABLE explicit BiItem(bi_item_t&);

  ~BiItem()override { }

  QString find()override;
  QString del()override;
  QString update()override;
  QString insert()override;
  QString getTableName()override;
  QVariant packageData(QList<QVariant>&)override;

  QList<QVariant> insertBind()override;
  QList<QVariant> updateBind()override;

  bi_item_t& getPtr();
  void clearyQuery();
  BiItem& query(QString key);

  bi_item_t& get();
  void setTableName(QString);

 protected:
  void setTableName();

 private:
  bi_item_t _rcd;
  QString _queryStr;
  QString _tableName;
};

class Sysinfo : public BiItem {
 public:
  Sysinfo();
  Q_INVOKABLE explicit Sysinfo(bi_item_t&);
};
class Admin : public BiItem {
 public:
  Q_INVOKABLE explicit Admin(bi_item_t&);
  Admin();
};

#endif  //  BIITEM_H

