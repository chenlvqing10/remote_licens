/*
* qtListView.cpp - define qt listview interface function of UI
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

#include <QMouseEvent>
#include "qtListView.hpp"
#include <QScrollBar>

qtListView::qtListView(QWidget *parent) :
  QListView(parent) {
  ptime = new QTime(0, 0, 0, 0);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  p_srcoll = verticalScrollBar();
}

 void qtListView::mousePressEvent(QMouseEvent *ev) {
  Q_UNUSED(ev);

  /* pressPos = QCursor::pos(); */
  if(pressPos.isNull()) {
    ptime->start();
    pressPos = ev->pos();
    lastPos = ev->pos();
  } else {
    pressPos = QPoint(0, 0);
    ptime->start();
  }
  QListView::mousePressEvent(ev);

#if 0
  if (itemAt(mapFromGlobal(QCursor::pos())) ! = NULL) {
    itemAt(mapFromGlobal(QCursor::pos()))->setSelected(true);
  }
#endif
}

void qtListView::mouseReleaseEvent(QMouseEvent *ev) {
  Q_UNUSED(ev);
#if 0
  QList<QListWidgetItem *> list = selectedItems();
  if (list.size() == 0) return;
#endif

  if (pressPos.isNull() || ptime->elapsed() < 80 ||
    (ev->pos() - pressPos).manhattanLength() > 5) {
    ptime->start();
    pressPos = QPoint(0, 0);
    QListView::mouseReleaseEvent(ev);
    return;
  }

  QModelIndex index = indexAt(pressPos);
  if (index.isValid()) {
    if (ptime->elapsed() > 500) {
      notify_longclicked(index);
    } else {
      notify_onclicked(index);
    }
  }

  ptime->start();
  pressPos = QPoint(0, 0);
  QListView::mouseReleaseEvent(ev);
}

void qtListView::mouseMoveEvent(QMouseEvent *ev) {
  Q_UNUSED(ev);

  int dy = lastPos.y() - ev->pos().y();
  int ads_dy = qAbs(dy);
  if (150 < ads_dy)
    dy *= 3 / 2;
  else if (100 < ads_dy)
    dy *= 4 / 3;
  else if (50 < ads_dy)
    dy *= 6 / 5;
  p_srcoll->setValue(p_srcoll->value() + dy);
  lastPos = ev->pos();
  ptime->start();
  /* QListView::mouseMoveEvent(ev); */
}

void qtListView::wheelEvent(QWheelEvent *e) {
  Q_UNUSED(e);
  /* ignore wheel event */
  e->accept();
  /* QListView::wheelEvent(e); */
}

void qtListView::notify_onclicked(const QModelIndex &index) const {
  emit click(index);
}

void qtListView::notify_longclicked(const QModelIndex &index) const {
  emit long_click(index);
}

