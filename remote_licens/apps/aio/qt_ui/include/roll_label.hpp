/*
* roll_Label.h - define label roll interface function of UI
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

#ifndef ROLL_LABEL_H
#define ROLL_LABEL_H

#include <QLabel>
#include <QTimerEvent>

class roll_Label : public QLabel
{
    Q_OBJECT
public:
    explicit roll_Label(QWidget *parent = nullptr);

public:
    void setInterVal(int nInterVal){startTimer(nInterVal);}
protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
private:
    int m_nCharWidth = 0;
    int m_nCurrentIndex = 0;
    QString m_strText;
    QString m_strDrawText;
};

#endif // ROLL_LABEL_H
