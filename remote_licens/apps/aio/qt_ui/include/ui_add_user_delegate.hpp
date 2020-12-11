/*
* ui_add_user_delegate.h -
* define add user delegate function of UI
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

#ifndef UI_ADD_USER_DELEGATE_H
#define UI_ADD_USER_DELEGATE_H

#include <QTableWidget>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QMetaType>
#include <QPushButton>
#include <QLineEdit>

#define CTRL_TYPE_BUTTON 0
#define CTRL_TYPE_LINEEDIT 1

typedef struct {
  QString name;
  QWidget *widget;
  char *value;
} MuItemData_add_user;

Q_DECLARE_METATYPE(MuItemData_add_user)

class ui_add_user_delegate : public QStyledItemDelegate {
 public:
  explicit ui_add_user_delegate(QObject *parent = nullptr);
  void paint(QPainter *painter,
             const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;
};

#endif  //  UI_ADD_USER_DELEGATE_H
