/*
* ui_base_widget.cpp - define ui_base_widget of project
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
#include "ui_aboat_device_main_delegate.hpp"

ui_aboat_device_main_delegate::ui_aboat_device_main_delegate() {
}
void ui_aboat_device_main_delegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
  if (index.isValid()) {
    painter->save();
    QVariant var = index.data(Qt::UserRole + 1);
    data_aboat_setting_delegate itemData =
        var.value<data_aboat_setting_delegate>();

    // current item if not display
    if (itemData.is_display == false) {
      painter->restore();
      return;
    }

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

    QRectF iconRect = QRect(rect.left() + 72, rect.top() + 17, 64, 64);
    QRectF nameRect = QRect(
        iconRect.right() + 44, rect.top() + 18, 240, 62);

    int len = 400;
    QRectF valueRect = QRect(
        rect.right() - 72 - len, rect.top() + 18,
        len, 62);
    QRectF enterRect = QRect(rect.right() - 72, rect.top() + 18, 42, 42);
    painter->drawImage(iconRect, QImage(itemData.icon_path));
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("SourceHanSansCN-Normal", 38));
    painter->drawText(nameRect, itemData.name);
    switch (index.row()) {
      case 0:
      case 1:
      case 2:
        painter->drawText(valueRect, Qt::AlignRight, itemData.text);
        break;
      case 3:
      case 4:
      case 5:
      case 6:
        painter->drawImage(enterRect, QImage(":icon/setting/enter.png"));
        break;
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
QSize ui_aboat_device_main_delegate::sizeHint(
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
  Q_UNUSED(index)
  return QSize(option.rect.width(), 98);
}


