/*
* ui_soft_keyboard.hpp - define interface function of UI input method
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

#ifndef UI_SOFT_KEYBOARD_H
#define UI_SOFT_KEYBOARD_H

#include <QtGui>
#include <QtCore>
#include <QPoint>
#include <QPushButton>
#include <map>
#include <vector>

#include "google_pinyin.hpp"
#include "ui_ui_soft_keyboard.h"

#define SCREEN_HEIGHT   720
#define BTN_NUM         40
#define CAND_MAX_LEN    5
#define DICT_PATH       "/usr/share/aio/res"

enum Type {LowerCase, UpperCase, SpecialChar, Fixed};

struct Mode {
  int key;          /* Qt::Key */
  QString display;  /* display text */
  Type type;        /* default: LowerCase */
};

class ui_soft_keyboard : public QWidget {
  Q_OBJECT

 public:
  explicit ui_soft_keyboard(QWidget* parent = 0);
  void hideSoftKeyboard();
  void showSoftKeyboard();
  void resetSoftKeyboard();
  QWidget* getLastFocus();

 signals:
  void characterGenerated(int character);
  void stringGenerated(QString text,
      QList<QInputMethodEvent::Attribute> attributes);
  void stringGeneratedToOld(QString text,
      QList<QInputMethodEvent::Attribute> attributes, QWidget *w);

 protected:
  bool event(QEvent* e);
#ifdef DRAG_MODE
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
#endif
  // void paintEvent(QPaintEvent*);

 private slots:
  void softKeyboardControl(QWidget* oldFocus, QWidget* newFocus);
  void buttonClicked(QWidget* w);
  void on_btn_language__clicked();
  void on_btn_caplock__clicked();
  void on_btn_next__clicked();
  void on_btn_last__clicked();
  void on_btn_special__clicked();

 private:
  bool initChinese();
  bool getBtnText(QWidget* widget, QString& text, int start, int count);
  void showChinese(int page, int pos);
  void setBtnText(Type type);
  QString getPrePinyin();
  void a2zClick(QString text, int offset);
  void candidateClick(QString text, int offset);
  void backspaceClick(int offset);
  void spaceClick(int offset);
  void returnClick();
  void setTextFocusedWidget(int offset, QString text);
  int getLastFocusCursorIndex();
  int setLastFocusCursorIndex(int index);
  bool isLastFocusVisibleAll();

 private:
  Ui::ui_soft_keyboard ui;
  QWidget* last_focused_widget;
  QSignalMapper signal_mapper;
  QPushButton* m_btns_[BTN_NUM];

  /* drag soft_keyboard */
  QPoint pt_press;

  /* mouse left key press */
  bool left_btn_press;

  /* false: english，true：chinese */
  bool is_chinese;

  /* false: Lowercase，true：Uppercase */
  bool is_uppercase;

  /* pinying of search */
  QString pinyin_search;

  /* candidate current position */
  int cand_current_pos;

  /* candidate current page */
  int cand_current_page;

  /* previous mode */
  Type pre_type;

  /* current mode */
  Type cur_type;

  /* whole preview string */
  QString whole_pre_str;

  /* chinese init */
  bool init_done;

  /* preview pinyin */
  QString py_pre_str;

  /* candidate count */
  unsigned cand_num;

  /* googlepinyin service */
  GooglePinyin *google_pinyin;
};
#endif  //  UI_SOFT_KEYBOARD_H

