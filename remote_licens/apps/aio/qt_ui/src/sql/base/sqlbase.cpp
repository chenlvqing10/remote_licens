/*
* sqlbase.cpp - define interface function of sql  base class
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

#include "sqlbase.hpp"

SqlBase::SqlBase() {
  // un_yz_qDebug()<<"sqlbase init!";
}

SqlBase::~SqlBase() {
  // un_yz_qDebug()<< "sqlbase dtor";
}

QString SqlBase::insert() {
  return "SqlBase_insert cmd";
}

QString SqlBase::del() {
  return "SqlBase_del cmd";
}

QString SqlBase::update() {
  return "SqlBase_update cmd";
}

QString SqlBase::find() {
  return "SqlBase_find cmd";
}

QString SqlBase::findByLimit(int index, int count) {
  return "SqlBase_findByLimit cmd";
}

QString SqlBase::getCount() {
  return "SqlBase_getCount cmd";
}

QString SqlBase::replace() {
  return QString();
}

QString SqlBase::getTableName() {
  return QString();
}

QVariant SqlBase::packageData(QList<QVariant> &metaData) {
  metaData.clear();

  return QVariant();
}

