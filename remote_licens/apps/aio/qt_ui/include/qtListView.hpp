/*
* qtListView.hpp - define qt listview interface function of UI
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

#ifndef QT_LIST_VIEW_H
#define QT_LIST_VIEW_H

#include <QListView>
#include <QTime>

class qtListView : public QListView {
  Q_OBJECT

 public:
  explicit qtListView(QWidget *parent = 0);

 protected:
  virtual void mousePressEvent(QMouseEvent *ev);
  virtual void mouseReleaseEvent(QMouseEvent *ev);
  virtual void mouseMoveEvent(QMouseEvent *ev);
  virtual void wheelEvent(QWheelEvent *e);

 signals:
  void click(const QModelIndex &index) const;
  void long_click(const QModelIndex &index) const;

 private:
  void notify_onclicked(const QModelIndex &index) const;
  void notify_longclicked(const QModelIndex &index) const;

 private:
  QTime *ptime;
  QPoint pressPos;
  QPoint lastPos;
  QScrollBar *p_srcoll;
};

#endif  //  QT_LIST_VIEW_H