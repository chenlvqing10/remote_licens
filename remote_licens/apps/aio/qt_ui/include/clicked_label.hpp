/*
* ui_clicked_label.h - define label clicked interface function of UI
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

#ifndef CLICKED_LABEL_H
#define CLICKED_LABEL_H

#include <QLabel>

class clicked_Label : public QLabel {
  Q_OBJECT

 public:
  explicit clicked_Label(QWidget *parent = 0);
  void wrapData(const QString& text , int lines = 0);
  void wrapDataContainLines(const QString& text , int lines = 0);

 protected:
  virtual void mouseReleaseEvent(QMouseEvent *ev);

 private:
  QString spliteText(const QFontMetrics& font, const QString& text,
    int labelSize , int  & lines);

 signals:
  void clicked();
 public slots:
 private:
  int maxLines_ = 0;
};

#endif  //  CLICKED_LABEL_H
