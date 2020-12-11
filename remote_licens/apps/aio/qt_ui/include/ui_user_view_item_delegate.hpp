/*
* ui_user_view_item_delegate.h - define user view item delegate function of UI
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

#ifndef UI_USER_VIEW_ITEM_DELEGATE_H
#define UI_USER_VIEW_ITEM_DELEGATE_H

#include <QTableWidget>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QMetaType>

typedef struct {
  QString id;
  QString name;
  QString limit;
  QString edt_iconPath;
  QString del_iconPath;
} MuItemData_user;

Q_DECLARE_METATYPE(MuItemData_user)

class ui_user_view_item_delegate : public QStyledItemDelegate {
 public:
#if 0
  ui_user_view_item_delegate(QObject *parent = nullptr);
  void paint(QPainter *painter,
             const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;
#else
 public:
  ui_user_view_item_delegate(QTableView* tableView);

 protected:
  void paint(QPainter *painter,
              const QStyleOptionViewItem &option,
              const QModelIndex &index) const;

 private:
  QPen pen;
  QTableView *view;
#endif
};

#endif  //  UI_USER_VIEW_ITEM_DELEGATE_H
