/*
* ExportCheckin.hpp - define sql type of project
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

#ifndef EXPORT_CHECKIN_H
#define EXPORT_CHECKIN_H

#include "sql_head.hpp"
#include "sqlbase.hpp"

class ExportCheckin : public SqlBase {
 public:
  explicit ExportCheckin(const export_record_info_t&);
  ExportCheckin() { }
  virtual ~ExportCheckin() { }

  ExportCheckin& query(int index, int count);
  ExportCheckin& query(QString query);
  ExportCheckin& limit(QString limit);

  QString find()override;
  QString getCount();

  QVariant packageData(QList<QVariant>& metaData)override;
  void clearQuery();
  QString getTableName()override;

 private:
  export_record_info_t _rcd;
  QString _queryStr;
};

#endif  //  EXPORT_CHECKIN_H
