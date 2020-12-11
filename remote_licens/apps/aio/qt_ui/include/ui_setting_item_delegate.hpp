/*
* ui_setting_item_delegate.h -
* define setting delegate function of UI
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

#ifndef UI_SETTING_ITEM_DELEGATE_H
#define UI_SETTING_ITEM_DELEGATE_H

#include <QTableWidget>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QMetaType>
#include <QPushButton>

#define ATTENDANCE_SETTING_ITEM_COUNT 3

typedef enum {
  EN_ITEM_LABEL,  /*0*/
  EN_ITEM_SWITCH, /*1*/
  EN_ITEM_LABEL_ONLY, /*2*/
  EN_ITEM_BT, /*3*/
  EN_ITEM_IMAGE, /*4*/
  EN_ITEM_EDIT,
} EN_ITEM_TYPE;

typedef struct {
  QString icon;
  QString title;
  QString value;
  EN_ITEM_TYPE type;
  QString icon_end;
  QWidget *widget;
} item_t;

Q_DECLARE_METATYPE(item_t)

class ui_setting_item_delegate : public QStyledItemDelegate {
 public:
  explicit ui_setting_item_delegate(QObject *parent = nullptr);
  void paint(QPainter *painter,
             const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;
};

#endif  //  UI_SETTING_ITEM_DELEGATE_H
