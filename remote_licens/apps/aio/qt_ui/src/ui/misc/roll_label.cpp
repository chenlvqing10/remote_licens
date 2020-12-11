/*
* roll_Label.cpp - define label roll interface function of UI
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
#include <roll_label.hpp>
#include <QPainter>
#include <QDebug>

roll_Label::roll_Label(QWidget *parent)
    : QLabel(parent)
{
    this->startTimer(300);
}

void roll_Label::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if(0 == m_nCharWidth)
        return ;

    QPainter painter(this);
    m_strDrawText = m_strText.mid(m_nCurrentIndex);
    while((m_strDrawText.size() * m_nCharWidth) <= width()){
        m_strDrawText += m_strText + " ";
    }
    painter.drawText(rect(), alignment(), m_strDrawText);
}

void roll_Label::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    m_strText = text();
    if(!m_strText.isEmpty()){
        m_strText.append("    ");
        int nSumWidth = fontMetrics().width(m_strText);
        m_nCharWidth = nSumWidth/m_strText.size();
    }

    m_nCurrentIndex++;
    if(m_nCurrentIndex > m_strText.size()){
        m_nCurrentIndex = 0;
    }

    update();
}


