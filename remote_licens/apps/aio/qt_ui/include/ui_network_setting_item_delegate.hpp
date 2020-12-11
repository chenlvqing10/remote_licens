/*
* ui_network_setting_item_delegate.h - define network setting item delegate function of UI
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

#ifndef UI_NETWORK_SETTING_ITEM_DELEGATE_H
#define UI_NETWORK_SETTING_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QMetaType>

typedef struct {
  QString iconPath;
  QString network_style;
} MuItemData_network;

Q_DECLARE_METATYPE(MuItemData_network)

class ui_network_setting_item_delegate : public QStyledItemDelegate {
 public:
  ui_network_setting_item_delegate(QObject *parent = nullptr);
  void paint(QPainter *painter,
              const QStyleOptionViewItem &option,
              const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};

#endif  //  UI_NETWORK_SETTING_ITEM_DELEGATE_H
