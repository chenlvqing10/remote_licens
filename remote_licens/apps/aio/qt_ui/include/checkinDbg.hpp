/*
* CheckinDbg.hpp - define sql type of project
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

#ifndef CHECKINDBG_H
#define CHECKINDBG_H

#include "sql_head.hpp"
#include "sqlbase.hpp"

class CheckinDbg : public SqlBase {
 public:
  explicit CheckinDbg(const record_debug_t&);
  CheckinDbg() { }
  virtual ~CheckinDbg() { }

  CheckinDbg& query(QString key, QString value);
  CheckinDbg& query(QString key, uint32_t value);

  QString insert()override;
  QString update()override;
  QString del()override;
  QString find()override;

  QList<QVariant> insertBind()override;

  QVariant packageData(QList<QVariant>& metaData)override;
  void clearQuery();
//    void setTableName(QString&);
  QString getTableName()override;

 private:
  record_debug_t _rcd;
  QString _queryStr;
};

#endif  //  CHECKINDBG_H
