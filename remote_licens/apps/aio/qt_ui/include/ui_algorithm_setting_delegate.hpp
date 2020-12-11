/*
* ui_algorithm_setting_delegate.h -
* define algorithm setting delegate function of UI
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

#ifndef UI_ALGORITHM_SETTING_DELEGATE_H
#define UI_ALGORITHM_SETTING_DELEGATE_H

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
  QString iconPath;
  QString name;
  QWidget *widget;
  int *isDisplay;
} MuItemData_algorithm;

Q_DECLARE_METATYPE(MuItemData_algorithm)

class ui_algorithm_setting_delegate : public QStyledItemDelegate {
 public:
  explicit ui_algorithm_setting_delegate(QObject *parent = nullptr);
  void paint(QPainter *painter,
             const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;
};

#endif  //  UI_ALGORITHM_SETTING_DELEGATE_H
