/*
* QtMainWindow.hpp define a main window inherit from QMainWindow
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

#ifndef Qt_MAIN_WINDOW_H
#define Qt_MAIN_WINDOW_H

#include <QMainWindow>
#include <QObject>

class QtMainWindow : public QMainWindow
{
  Q_OBJECT
 public:
  Q_INVOKABLE explicit QtMainWindow(QWidget *parent = 0);
  ~QtMainWindow();
  void sendSignalForMainUI(int event, int value);
  virtual void setClassName() { className = metaObject()->className(); }

 public slots:
  virtual void deinit();

 protected slots:
  virtual void slot_for_mainui(int event, int value);

 signals:
  void signalForMainUI(int event, int value);

private:
  const char *className;
};

#endif // Qt_MAIN_WINDOW_H