/*
* ui_access_setting_item_delegate.cpp -
* define interface function of access setting
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
#include "ui_access_setting_item_delegate.hpp"
#include <QPainter>
#include <QDebug>

ui_access_setting_item_delegate::
ui_access_setting_item_delegate(QObject *parent) :
  QStyledItemDelegate(parent) {
}

void ui_access_setting_item_delegate::paint(QPainter *painter,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const {
  if (index.row() >= ACCESS_SETTING_ITEM_COUNT || index.row() < 0)
    return;

  if (index.isValid()) {
    painter->save();
    QVariant var = index.data(Qt::UserRole + 1);
    itemdata_access itemData = var.value<itemdata_access>();

    // item 矩形区域
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
    QRectF iconRect = QRect(rect.left() + 72, rect.top() + 17, 64, 64);
    QRectF nameRect = QRect(
      iconRect.right() + 44, rect.top() + 18, 400, 62);

    int len = 400;

    QRectF wgValueRect = QRect(
      rect.right() - 72 - len, rect.top() + 18,
      len, 62);
    QRectF valueRect = QRect(
      rect.right() - 72 - 33 - 16 - len, rect.top() + 18,
      len, 62);

    painter->drawImage(iconRect, QImage(itemData.icon_path));
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("SourceHanSansCN-Normal", 38));
    painter->drawText(nameRect, itemData.name);

    if (index.row() == 0) {
      painter->drawText(wgValueRect, Qt::AlignRight, itemData.item_value);
    } else {
      if (index.row() == 5) {
        itemData.widget->setGeometry(
            rect.right() - 184, rect.top() + 18, 102, 62);
        itemData.widget->show();
      } else {
        painter->drawText(valueRect, Qt::AlignRight, itemData.item_value);
        itemData.widget->setGeometry(
            rect.right() - 72 - 33, rect.top() + 32, 33, 33);
        itemData.widget->show();
      }
    }

    painter->setPen(QPen(QColor("#102050")));
    if (0 == option.rect.y()) {
      painter->drawRect(
        rect.left() + 50, rect.top() + 1, rect.width() - 100, 1);
      painter->drawRect(
        rect.left() + 50, rect.top() + 2, rect.width() - 100, 1);
    }
    painter->drawRect(
      rect.left() + 50, rect.bottom() - 1, rect.width() - 100, 1);
    painter->drawRect(
      rect.left() + 50, rect.bottom() - 2, rect.width() - 100, 1);

    painter->setPen(QPen(Qt::black));

    painter->restore();
  }
}

QSize ui_access_setting_item_delegate::sizeHint(
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  Q_UNUSED(index)

  return QSize(option.rect.width(), 98);
}

