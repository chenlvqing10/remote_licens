/*
* SqlBase.hpp - define sql type of project
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

#ifndef SQLBASE_H
#define SQLBASE_H

#include "sql_head.hpp"

class SqlBase {
 public:
  SqlBase();
  virtual ~SqlBase();
  virtual QString insert();
  virtual QString del();
  virtual QString update();
  virtual QString find();
  virtual QString findByLimit(int index, int count);
  virtual QString getCount();
  virtual QString calc_count() {return NULL;}

  virtual QList<QVariant> insertBind() { return QList<QVariant>();}
  virtual QList<QVariant> delBind() { return QList<QVariant>();}
  virtual QList<QVariant> updateBind() { return QList<QVariant>();}
  virtual QList<QVariant> findBind() { return QList<QVariant>();}
  virtual QList<QVariant> replaceBind() { return QList<QVariant>();}
  virtual QString replace();
  virtual QString getTableName();
  virtual QVariant packageData(QList<QVariant>&);
};

#endif  //  SQLBASE_H
