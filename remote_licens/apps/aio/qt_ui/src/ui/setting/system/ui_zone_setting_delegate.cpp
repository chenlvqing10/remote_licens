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
#include "ui_zone_setting_delegate.hpp"

ui_zone_setting_delegate::ui_zone_setting_delegate() {
}
void ui_zone_setting_delegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
  if (index.isValid()) {
    painter->save();
    QVariant var = index.data(Qt::UserRole + 1);
    data_zone_setting_delegate data = var.value<data_zone_setting_delegate>();
    /**/
    QRectF rect(option.rect.x(), option.rect.y(),
    option.rect.width() - 1, option.rect.height() - 1);
    QPainterPath path;
    path.quadTo(rect.topLeft(), rect.bottomLeft());
    path.quadTo(rect.bottomLeft(), rect.bottomRight());
    path.quadTo(rect.bottomRight(), rect.topRight());
    path.quadTo(rect.topRight(), rect.topLeft());

    QRectF nameRect = QRect(rect.left() + 72, rect.top() + 17, 200, 64);
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("SourceHanSansCN-Normal", 38));
    painter->drawText(nameRect, data.text);

    data.widget->setGeometry(rect.x() + rect.width() - 72 - 56, rect.y() +
                               17, 56, 56);
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
QSize ui_zone_setting_delegate::sizeHint(
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
  Q_UNUSED(index)
  return QSize(option.rect.width(), 98);
}
