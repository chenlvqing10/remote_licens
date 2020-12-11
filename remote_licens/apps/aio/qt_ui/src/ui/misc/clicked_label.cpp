/*
* ui_clicked_label.cpp - define label clicked interface function of UI
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

#include <QDebug>

#include "clicked_label.hpp"

clicked_Label::clicked_Label(QWidget *parent) :
    QLabel(parent) {
}

void clicked_Label::mouseReleaseEvent(QMouseEvent *ev) {
  Q_UNUSED(ev);
  emit clicked();
}

QString clicked_Label::spliteText(const QFontMetrics& font,
  const QString& text, int labelSize , int & lines) {
  int nPos = 0;
  long nOffset = 0;
  int textSize = font.width(text);

  if(textSize > labelSize){
    for (int i = 0; i < text.size(); i++){
      nOffset += font.width(text.at(i));
      if(nOffset >= labelSize){
        nPos = i;
        break;
      }
    }
    nPos = (nPos -1 < 0) ? 0 : nPos -1;
    QString strLeftData = text.left(nPos);
    QString strMidData = text.mid(nPos);
    if(lines < maxLines_)
      return strLeftData + "\n" + spliteText(font, strMidData,
        labelSize , ++lines);
    else
      return strLeftData.replace(strLeftData.size()-1, 2, "...");
  }
  return text;
}

void clicked_Label::wrapData(const QString& text , int maxlines) {
  maxLines_ = maxlines;
  QFontMetrics font(this->font());
  int textSize = font.width(text);
  int labelSize = this->width();
  int num = 1;

  if(textSize > labelSize) {
    QString qstrData = spliteText(font, text, labelSize , num);

    QLabel::setText(qstrData);
    return;
  }
  QLabel::setText(text);
}

void clicked_Label::wrapDataContainLines(const QString& text ,
  int maxlines) {
  int textSize = 0;
  int tempLines = 1;
  int labelSize = this->width();
  QList<QString> list = text.split("\n");
  QString tempString;
  QFontMetrics font(this->font());

  if(!text.contains("\n"))
    return wrapData(text , maxlines);

  maxLines_ = maxlines;
  foreach (QString text, list) {
    textSize = font.width(text);
    if (textSize > labelSize) {
      QString qstrData = spliteText(font, text, labelSize , tempLines);
      tempString.append(qstrData).append("\n");
    } else {
      tempLines++;
      if (tempLines < maxLines_)
        tempString.append(text).append("\n");
      else
        break;
    }
  }
  if (tempString.contains("\n"))
    tempString.remove(tempString.size()-1,2);
  QLabel::setText(tempString);
}

