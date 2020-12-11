/*
* ui_add_user_delegate.cpp -
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

#include <QPainter>
#include <QDebug>

#include "ui_add_user_delegate.hpp"

#ifndef PORTRAIT_SCREEN
  #define ICON_BACK_AMAGE 72
  #define ROW_PITCH  50
  #define ROW_WIDTH  98
#else
  #define ICON_BACK_AMAGE 42
  #define ROW_PITCH  42
  #define ROW_WIDTH  92
#endif

ui_add_user_delegate::
ui_add_user_delegate(QObject *parent) : QStyledItemDelegate(parent) {
}

void ui_add_user_delegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  if (index.isValid()) {
    painter->save();
    QVariant var = index.data(Qt::UserRole + 1);
    MuItemData_add_user itemData = var.value<MuItemData_add_user>();

    // item rectangle region
    QRectF rect;
    rect.setX(option.rect.x());
    rect.setY(option.rect.y());
    rect.setWidth(option.rect.width() - 1);
    rect.setHeight(option.rect.height() - 1);

    QPainterPath path;

    path.quadTo(rect.topLeft(), rect.bottomLeft());
    path.quadTo(rect.bottomLeft(), rect.bottomRight());
    path.quadTo(rect.bottomRight(), rect.topRight());
    path.quadTo(rect.topRight(), rect.topLeft());
#if 0
    if (option.state.testFlag(QStyle::State_MouseOver)) {
      painter->setPen(QPen(QColor("#000")));
      painter->setBrush(QColor("#000"));
      painter->drawPath(path);
    }
    if (option.state.testFlag(QStyle::State_Selected)) {
      painter->setPen(QPen(QColor("#000")));
      painter->setBrush(QColor("#000"));
      painter->drawPath(path);
    }
#endif
    QRectF nameRect = QRect(
      rect.left() + ICON_BACK_AMAGE, rect.top() + 18, 240, 62);

    QRectF valueRect = QRect(
      rect.right() - ICON_BACK_AMAGE - 260, rect.top() + 18, 206, 62);
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("SourceHanSansCN-Normal", 38));
    painter->drawText(nameRect, itemData.name);

    if (NULL == itemData.value) {
      itemData.widget->setGeometry(
          rect.right() - ICON_BACK_AMAGE - 200, rect.top() + 18, 200, 62);
      itemData.widget->show();
    } else {
      painter->setFont(QFont("SourceHanSansCN-Normal", 30));
      painter->drawText(valueRect, Qt::AlignRight, itemData.value);
      itemData.widget->setGeometry(
          rect.right() - ICON_BACK_AMAGE - 33, rect.top() + 30, 33, 33);
      itemData.widget->show();
    }

    painter->setPen(QPen(QColor("#102050")));
    if (0 == option.rect.y()) {
      painter->drawRect(
        rect.left() + ROW_PITCH, rect.top() + 1, rect.width() -
          ROW_PITCH * 2, 1);
      painter->drawRect(
        rect.left() + ROW_PITCH, rect.top() + 2, rect.width() -
          ROW_PITCH * 2, 1);
    }
    painter->drawRect(
      rect.left() + ROW_PITCH, rect.bottom() - 1, rect.width() -
        ROW_PITCH * 2, 1);
    painter->drawRect(
      rect.left() + ROW_PITCH, rect.bottom() - 2, rect.width() -
        ROW_PITCH * 2, 1);

    painter->setPen(QPen(Qt::black));

    painter->restore();
  }
}

QSize ui_add_user_delegate::sizeHint(
    const QStyleOptionViewItem &option, const QModelIndex &index) const {
  Q_UNUSED(index)
  return QSize(option.rect.width(), ROW_WIDTH);
}

