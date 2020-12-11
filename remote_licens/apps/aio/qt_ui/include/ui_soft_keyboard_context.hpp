/*
* ui_soft_keyboard_contxt.hpp - define interface function of UI input method
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
#ifndef UI_SOFT_KEYBOARD_CONTEXT_H
#define UI_SOFT_KEYBOARD_CONTEXT_H

#include <QtGui/QInputContext>

#include "ui_soft_keyboard.hpp"

class MainWindow;

class ui_soft_keyboard_context : public QInputContext {
  Q_OBJECT

 public:
  ui_soft_keyboard_context();
  ~ui_soft_keyboard_context();

  bool filterEvent(const QEvent* event);
  QString identifierName();
  QString language();
  bool isComposing() const;
  void reset();

 private slots:
  void sendCharacter(int key);
  void sendString(QString text, QList<QInputMethodEvent::Attribute> attributes);
  void sendStringToOld(QString text,
      QList<QInputMethodEvent::Attribute> attributes, QWidget *w);

 private:
  void updatePosition();

 private:
  ui_soft_keyboard* keyboard_;
};

#endif
