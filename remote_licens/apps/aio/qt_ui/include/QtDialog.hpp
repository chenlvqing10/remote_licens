/*
* QtDialog.hpp define a main window inherit from QMainWindow
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

#ifndef Qt_DIALOG_H
#define Qt_DIALOG_H

#include <QMainWindow>
#include <QObject>

class QtDialog : public QMainWindow
{
  Q_OBJECT
 public:
  Q_INVOKABLE explicit QtDialog(QWidget *parent = 0);
  ~QtDialog();
  virtual void setClassName() { className = metaObject()->className(); }
  virtual void showMaxLayout();

private:
  const char *className;
};

#endif // Qt_DIALOG_H