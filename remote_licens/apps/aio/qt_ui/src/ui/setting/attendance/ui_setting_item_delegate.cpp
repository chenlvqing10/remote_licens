/*
* ui_setting_item_delegate.cpp -
* define interface function of setting item
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
#include "ui_setting_item_delegate.hpp"
#include <QPainter>
#include <QDebug>

#ifdef __cplusplus
extern "C" {
#endif
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */
#ifndef PORTRAIT_SCREEN
  #define ICON_BACK_AMAGE 72
  #define ROW_PITCH  50
  #define ROW_WIDTH  98
#else
  #define ICON_BACK_AMAGE 42
  #define ROW_PITCH  42
  #define ROW_WIDTH  92
#endif

#define SWITCH_ON_ICON ":/icon/turn_on.png"
#define SWITCH_OFF_ICON ":/icon/turn_off.png"
#define BT_ENTER_ICON ":/icon/enter.png"
#define BT_EDIT_ICON ":/icon/edit.png"

ui_setting_item_delegate::
ui_setting_item_delegate(QObject *parent) :
  QStyledItemDelegate(parent) {
}

void ui_setting_item_delegate::paint(QPainter *painter,
  const QStyleOptionViewItem &option,
  const QModelIndex &index) const {
  int len = 400;

  if (index.isValid()) {
    QVariant var = index.data(Qt::UserRole + 1);
    item_t itemData = var.value<item_t>();
    painter->save();

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
  /* painter->drawRect(rect.right() - 184, rect.top() + 8, 76, 76); */

    QRectF iconRect = QRect(rect.left() + ICON_BACK_AMAGE,
      rect.top() + 17, 64, 64);
    QRectF numRect = QRect(rect.left() + ICON_BACK_AMAGE,
        rect.top() + 17, 460, 64);
    QRectF nameRect;
    if (!QString(" ").compare(itemData.icon)) {
      nameRect = QRect(
        rect.left() + ICON_BACK_AMAGE, rect.top() + 18, 400, 62);
    } else {
      nameRect = QRect(
        iconRect.right() + 44, rect.top() + 18, 400, 62);
    }
    QRectF swRect = QRect(
      rect.right() - ICON_BACK_AMAGE-102, rect.top() + 18, 102, 62);
    QRectF enterRect = QRect(
      rect.right() - ICON_BACK_AMAGE - 33, rect.top() + 32, 33, 33);
    QRectF valueRect = QRect(
      rect.right() - ICON_BACK_AMAGE - 33 - 16 - len, rect.top() + 18,
      len, 62);
    QRectF valueRectLeft = QRect(
      rect.right() - ICON_BACK_AMAGE - len, rect.top() + 18,
      len, 62);
    QRectF btRect = QRect(
      rect.right() - ICON_BACK_AMAGE - 102, rect.top() + 18, 62, 62);
    QRectF imageRect = QRect(
      rect.right() - ICON_BACK_AMAGE-102, rect.top() + 8, 76, 76);
    QRect editRect = QRect(
      rect.right() - ICON_BACK_AMAGE - 250, rect.top() + 10, 200, 72);
    QRectF iconEndRect = QRect(
      rect.right() - ICON_BACK_AMAGE - 84, rect.top() + 10, 84, 72);

  switch (itemData.type) {
    case EN_ITEM_LABEL:
      painter->drawImage(iconRect, QImage(itemData.icon));
      painter->setPen(QPen(Qt::white));
      painter->setFont(QFont("SourceHanSansCN-Normal", 38));
      painter->drawText(nameRect, itemData.title);

      painter->drawText(valueRect, Qt::AlignRight, itemData.value);
      if (itemData.icon_end.isNull() || !itemData.icon_end.compare(""))
        painter->drawImage(enterRect, QImage(BT_ENTER_ICON));
      else
        painter->drawImage(iconEndRect, QImage(itemData.icon_end));
      break;
    case EN_ITEM_SWITCH:
      painter->drawImage(iconRect, QImage(itemData.icon));
      painter->setPen(QPen(Qt::white));
      painter->setFont(QFont("SourceHanSansCN-Normal", 38));
      painter->drawText(nameRect, itemData.title);

      if (0 == itemData.value.compare("1")) {
      // be checked
        painter->drawImage(swRect, QImage(SWITCH_ON_ICON));
      } else {
        // be unchecked
        painter->drawImage(swRect, QImage(SWITCH_OFF_ICON));
      }
      break;
    case EN_ITEM_LABEL_ONLY:
      painter->drawImage(iconRect, QImage(itemData.icon));
      painter->setPen(QPen(Qt::white));
      painter->setFont(QFont("SourceHanSansCN-Normal", 38));
      painter->drawText(nameRect, itemData.title);
      painter->drawText(valueRectLeft, Qt::AlignRight, itemData.value);
      break;
    case EN_ITEM_BT:
      painter->setPen(QPen(Qt::white));
      painter->setFont(QFont("SourceHanSansCN-Normal", 38));
      painter->drawText(iconRect, itemData.icon);
      painter->drawText(nameRect, itemData.title);

      painter->drawImage(btRect, QImage(BT_EDIT_ICON));
      break;
    case EN_ITEM_IMAGE:
      painter->setPen(QPen(Qt::white));
      painter->setFont(QFont("SourceHanSansCN-Normal", 38));
      painter->drawText(numRect, itemData.icon);
      painter->drawText(rect, Qt::AlignCenter, itemData.title);
      painter->drawImage(imageRect, QImage(itemData.value));
      break;
    case EN_ITEM_EDIT:
      painter->drawImage(iconRect, QImage(itemData.icon));
      painter->setPen(QPen(Qt::white));
      painter->setFont(QFont("SourceHanSansCN-Normal", 38));
      painter->drawText(nameRect, itemData.title);
      painter->drawText(valueRect, Qt::AlignRight, itemData.value);
      if (NULL != itemData.widget) {
        itemData.widget->setGeometry(editRect);
        itemData.widget->show();
      }
      painter->drawImage(enterRect, QImage(BT_ENTER_ICON));
      break;
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

QSize ui_setting_item_delegate::sizeHint(
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  Q_UNUSED(index)

  return QSize(option.rect.width(), ROW_WIDTH);
}

