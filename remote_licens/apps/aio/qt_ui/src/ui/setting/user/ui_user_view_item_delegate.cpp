/*
* ui_user_view_item_delegate.cpp - define interface function of user view
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

#include <QPainter>
#include <QDebug>

#include "ui_user_view_item_delegate.hpp"

ui_user_view_item_delegate::
ui_user_view_item_delegate(QTableView *tableView) {
  int gridHint = tableView->style()->styleHint(
    QStyle::SH_Table_GridLineColor, new QStyleOptionViewItemV4());
  QColor gridColor = static_cast<QRgb>(gridHint);
  pen = QPen(gridColor, 0, tableView->gridStyle());
  view = tableView;
}

void ui_user_view_item_delegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  QStyledItemDelegate::paint(painter, option, index);
  QPen oldPen = painter->pen();
  painter->setPen(pen);

  painter->setPen(QPen(QColor("#102050")));
  if (option.rect.y() == 0) {
    painter->drawRect(option.rect.x(), option.rect.y(),
                      option.rect.width(), 1);
    painter->drawRect(option.rect.x(), option.rect.y() + 1,
                      option.rect.width(), 1);
  }
  painter->drawRect(option.rect.x(), option.rect.bottom()-1,
                    option.rect.width(), 1);
  painter->drawRect(option.rect.x(), option.rect.bottom(),
                    option.rect.width(), 1);
  painter->setPen(oldPen);
}

