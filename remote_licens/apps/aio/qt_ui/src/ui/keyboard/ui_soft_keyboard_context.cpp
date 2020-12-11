/*
* ui_soft_keyboard_context.cpp - define function of UI input method
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

#include <QtCore>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define LOG_TAG     "ui_soft_keyboard_context"
#include <log/log.h>
#include "screen_size.h"
#include "ui_control.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "screen_size.hpp"

#include "ui_soft_keyboard_context.hpp"

ui_soft_keyboard_context::ui_soft_keyboard_context() {
  keyboard_ = new ui_soft_keyboard();
  connect(keyboard_, SIGNAL(characterGenerated(int)), SLOT(sendCharacter(int)));
  connect(keyboard_, SIGNAL(stringGenerated(QString,
      QList<QInputMethodEvent::Attribute>)),
      SLOT(sendString(QString, QList<QInputMethodEvent::Attribute>)));
  connect(keyboard_, SIGNAL(stringGeneratedToOld(QString,
      QList<QInputMethodEvent::Attribute>, QWidget*)),
      SLOT(sendStringToOld(QString,
      QList<QInputMethodEvent::Attribute>, QWidget*)));
}

ui_soft_keyboard_context::~ui_soft_keyboard_context() {
}

bool ui_soft_keyboard_context::filterEvent(const QEvent* event) {
  if (event->type() == QEvent::RequestSoftwareInputPanel
      && !keyboard_->isVisible()) {
    keyboard_->showSoftKeyboard();
    return true;
  } else if (event->type() == QEvent::CloseSoftwareInputPanel
      && keyboard_->isVisible()) {
    keyboard_->hideSoftKeyboard();
    return true;
  }
  return false;
}

QString ui_soft_keyboard_context::identifierName() {
  return "ui_soft_keyboard_context";
}

void ui_soft_keyboard_context::reset() {
}

bool ui_soft_keyboard_context::isComposing() const {
  return false;
}

QString ui_soft_keyboard_context::language() {
  return "en_US";
}

void ui_soft_keyboard_context::sendCharacter(int key) {
  QPointer<QWidget> w = focusWidget();

  if (w) {
    QKeyEvent keyPress(QEvent::KeyPress, key, Qt::NoModifier, QString(key));
    QApplication::sendEvent(w, &keyPress);
  }
}

void ui_soft_keyboard_context::sendString(QString text,
    QList<QInputMethodEvent::Attribute> attributes) {
  QPointer<QWidget> w = focusWidget();

  if (w) {
    QInputMethodEvent event(text, attributes);
    QApplication::sendEvent(w, &event);
  }
}

void ui_soft_keyboard_context::sendStringToOld(QString text,
    QList<QInputMethodEvent::Attribute> attributes, QWidget *w) {
  QPointer<QWidget> widget = w;

  if (widget) {
    QInputMethodEvent event("", attributes);
    QApplication::sendEvent(widget, &event);
  }
}

void ui_soft_keyboard_context::updatePosition() {
  QWidget* widget = focusWidget();

  if (!widget)
    return;

  keyboard_->move(0, get_screen_height() - keyboard_->height());
}

