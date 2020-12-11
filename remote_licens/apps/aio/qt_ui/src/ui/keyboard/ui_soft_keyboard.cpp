/*
* ui_soft_keyboard.cpp - define function of UI input method
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

#ifdef __cplusplus
extern "C" {
#endif
#define LOG_TAG     "ui_soft_keyboard"
#include <log/log.h>
#include "screen_size.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStyleOption>
#include <QPainter>
#include <QLineEdit>
#include <QFile>
#include <QDateTime>
#include <QVector>
#include <QtGui/qtextcursor.h>
#include <QInputMethodEvent>
#include <map>
#include <utility>
#include <QDebug>

#include "google_pinyin.hpp"
#include "screen_size.hpp"

#include "ui_soft_keyboard.hpp"

typedef QList<Mode> Modes;
typedef QList<Modes> ModesList;

const ModesList modeList = {
    {{Qt::Key_Q, "q", LowerCase}, {Qt::Key_Q, "Q", UpperCase},
    {Qt::Key_1, "1", SpecialChar}},
    {{Qt::Key_W, "w", LowerCase}, {Qt::Key_W, "W", UpperCase},
    {Qt::Key_2, "2", SpecialChar}},
    {{Qt::Key_E, "e", LowerCase}, {Qt::Key_E, "E", UpperCase},
    {Qt::Key_3, "3", SpecialChar}},
    {{Qt::Key_R, "r", LowerCase}, {Qt::Key_R, "R", UpperCase},
    {Qt::Key_4, "4", SpecialChar}},
    {{Qt::Key_T, "t", LowerCase}, {Qt::Key_T, "T", UpperCase},
    {Qt::Key_5, "5", SpecialChar}},
    {{Qt::Key_Y, "y", LowerCase}, {Qt::Key_Y, "Y", UpperCase},
    {Qt::Key_6, "6", SpecialChar}},
    {{Qt::Key_U, "u", LowerCase}, {Qt::Key_U, "U", UpperCase},
    {Qt::Key_7, "7", SpecialChar}},
    {{Qt::Key_I, "i", LowerCase}, {Qt::Key_I, "I", UpperCase},
    {Qt::Key_8, "8", SpecialChar}},
    {{Qt::Key_O, "o", LowerCase}, {Qt::Key_O, "O", UpperCase},
    {Qt::Key_9, "9", SpecialChar}},
    {{Qt::Key_P, "p", LowerCase}, {Qt::Key_P, "P", UpperCase},
    {Qt::Key_0, "0", SpecialChar}},

    {{Qt::Key_A, "a", LowerCase}, {Qt::Key_A, "A", UpperCase},
    {Qt::Key_unknown, "~", SpecialChar}},
    {{Qt::Key_S, "s", LowerCase}, {Qt::Key_S, "S", UpperCase},
    {Qt::Key_unknown, "!", SpecialChar}},
    {{Qt::Key_D, "d", LowerCase}, {Qt::Key_D, "D", UpperCase},
    {Qt::Key_At,      "@", SpecialChar}},
    {{Qt::Key_F, "f", LowerCase}, {Qt::Key_F, "F", UpperCase},
    {Qt::Key_NumberSign, "#", SpecialChar}},
    {{Qt::Key_G, "g", LowerCase}, {Qt::Key_G, "G", UpperCase},
    {Qt::Key_Percent, "%", SpecialChar}},
    {{Qt::Key_H, "h", LowerCase}, {Qt::Key_H, "H", UpperCase},
    {Qt::Key_unknown, "'", SpecialChar}},
    {{Qt::Key_J, "j", LowerCase}, {Qt::Key_J, "J", UpperCase},
    {Qt::Key_unknown, "&&", SpecialChar}},
    {{Qt::Key_K, "k", LowerCase}, {Qt::Key_K, "K", UpperCase},
    {Qt::Key_unknown, "*", SpecialChar}},
    {{Qt::Key_L, "l", LowerCase}, {Qt::Key_L, "L", UpperCase},
    {Qt::Key_unknown, "?", SpecialChar}},

    {{Qt::Key_Z, "z", LowerCase}, {Qt::Key_Z, "Z", UpperCase},
    {Qt::Key_ParenLeft, "(", SpecialChar}},
    {{Qt::Key_X, "x", LowerCase}, {Qt::Key_X, "X", UpperCase},
    {Qt::Key_ParenLeft, ")", SpecialChar}},
    {{Qt::Key_C, "c", LowerCase}, {Qt::Key_C, "C", UpperCase},
    {Qt::Key_Minus,     "-", SpecialChar}},
    {{Qt::Key_V, "v", LowerCase}, {Qt::Key_V, "V", UpperCase},
    {Qt::Key_unknown,   "_", SpecialChar}},
    {{Qt::Key_B, "b", LowerCase}, {Qt::Key_B, "B", UpperCase},
    {Qt::Key_unknown,   ":", SpecialChar}},
    {{Qt::Key_N, "n", LowerCase}, {Qt::Key_N, "N", UpperCase},
    {Qt::Key_Semicolon, ";", SpecialChar}},
    {{Qt::Key_M, "m", LowerCase}, {Qt::Key_M, "M", UpperCase},
    {Qt::Key_Slash,     "/", SpecialChar}},
    {{Qt::Key_CapsLock, "", Fixed}},
    {{Qt::Key_Backspace, "", Fixed}},

    {{Qt::Key_Mode_switch, ".?123", Fixed}},
    {{Qt::Key_Context1, "", Fixed}},
    {{Qt::Key_Space, "", Fixed}},
    {{Qt::Key_Enter, "", Fixed}}
};

ui_soft_keyboard::ui_soft_keyboard(QWidget* parent)
    : QWidget(parent, Qt::Tool | Qt::WindowStaysOnTopHint
              | Qt::FramelessWindowHint), last_focused_widget(0) {
  ui.setupUi(this);
  hide();
  is_chinese = false;
  is_uppercase = false;
  init_done = false;

  cand_current_pos = 0;
  cand_current_page = 0;

  pre_type = LowerCase;
  cur_type = LowerCase;

  m_btns_[0] = ui.char_q_;
  m_btns_[1] = ui.char_w_;
  m_btns_[2] = ui.char_e_;
  m_btns_[3] = ui.char_r_;
  m_btns_[4] = ui.char_t_;
  m_btns_[5] = ui.char_y_;
  m_btns_[6] = ui.char_u_;
  m_btns_[7] = ui.char_i_;
  m_btns_[8] = ui.char_o_;
  m_btns_[9] = ui.char_p_;
  m_btns_[10] = ui.char_a_;
  m_btns_[11] = ui.char_s_;
  m_btns_[12] = ui.char_d_;
  m_btns_[13] = ui.char_f_;
  m_btns_[14] = ui.char_g_;
  m_btns_[15] = ui.char_h_;
  m_btns_[16] = ui.char_j_;
  m_btns_[17] = ui.char_k_;
  m_btns_[18] = ui.char_l_;
  m_btns_[19] = ui.char_z_;
  m_btns_[20] = ui.char_x_;
  m_btns_[21] = ui.char_c_;
  m_btns_[22] = ui.char_v_;
  m_btns_[23] = ui.char_b_;
  m_btns_[24] = ui.char_n_;
  m_btns_[25] = ui.char_m_;

  m_btns_[26] = ui.btn_backspace_;
  m_btns_[27] = ui.btn_space_;
  m_btns_[28] = ui.btn_enter_;

  m_btns_[29] = ui.word_0_;
  m_btns_[30] = ui.word_1_;
  m_btns_[31] = ui.word_2_;
  m_btns_[32] = ui.word_3_;
  m_btns_[33] = ui.word_4_;
  m_btns_[34] = ui.word_5_;
  m_btns_[35] = ui.word_6_;
  m_btns_[36] = ui.word_7_;
  m_btns_[37] = ui.word_8_;
  m_btns_[38] = ui.word_9_;

  m_btns_[39] = ui.btn_dot_;

  for (int i = 0; i < 26; i++) {
    m_btns_[i]->setStyleSheet(
      "QPushButton{border-image:url(:/icon/keyboard1.png);}");
  }

  m_btns_[26]->setStyleSheet(
    "QPushButton{border-image:url(:/icon/backspace.png);}");
  m_btns_[27]->setStyleSheet(
    "QPushButton{border-image:url(:/icon/keyboard4.png);}");
  m_btns_[28]->setStyleSheet(
    "QPushButton{border-image:url(:/icon/keyboard3.png);}");
  ui.btn_caplock_->setStyleSheet(
    "QPushButton{border-image:url(:/icon/caps.png);}");
  ui.btn_special_->setStyleSheet(
    "QPushButton{border-image:url(:/icon/keyboard3.png);}");
  ui.btn_language_->setStyleSheet(
    "QPushButton{border-image:url(:/icon/keyboard3.png);}");

  ui.btn_next_->setStyleSheet("QPushButton{background-color:transparent;}");
  ui.btn_last_->setStyleSheet("QPushButton{background-color:transparent;}");

  for (int i = 29; i < 39; ++i) {
    m_btns_[i]->setStyleSheet("QPushButton{background-color:transparent;}");
  }
  ui.btn_dot_->setStyleSheet(
    "QPushButton{border-image:url(:/icon/keyboard1.png);}");

  setBtnText(cur_type);

  for (int i = 0; i < 40; i++) {
    signal_mapper.setMapping(m_btns_[i], m_btns_[i]);
    connect(m_btns_[i], SIGNAL(clicked()), &signal_mapper, SLOT(map()));
  }

  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
          this, SLOT(softKeyboardControl(QWidget*, QWidget*)));
  connect(&signal_mapper, SIGNAL(mapped(QWidget*)),
          this, SLOT(buttonClicked(QWidget*)));

  ui.widget_chinese_->hide();
  /*
  this->setStyleSheet("QWidget{background-color:#060f2b;}");
  ui.lab_pinying_->setStyleSheet("QLineEdit{color:white;}");
  */
  for (int i = 34; i <= 38; i++) {
    m_btns_[i]->hide();
  }
}

void ui_soft_keyboard::setBtnText(Type type) {
  for (int i = 0; i < 26; i++) {
    for (int j = 0; j < modeList[i].count(); j++) {
      if (modeList[i][j].type == type)
        m_btns_[i]->setText(modeList[i][j].display);
    }
  }
}

bool ui_soft_keyboard::event(QEvent* e) {
  switch (e->type()) {
    case QEvent::WindowActivate:
      if (last_focused_widget)
        last_focused_widget->activateWindow();
      break;
    default:
      break;
  }

  return QWidget::event(e);
}

void ui_soft_keyboard::softKeyboardControl(QWidget* oldFocus,
      QWidget* newFocus) {
  if (newFocus != 0 && !this->isAncestorOf(newFocus)) {
    last_focused_widget = newFocus;

    if (newFocus->objectName().compare(QString("ui_face_input")) == 0)
      resetSoftKeyboard();

    if (newFocus->inherits("QLineEdit")
          || newFocus->inherits("QTextEdit")
          || newFocus->inherits("QPlainTextEdit")
          || newFocus->inherits("QTextBrowser")
          || newFocus->inherits("QSpinBox")
          || newFocus->inherits("QDoubleSpinBox")
          || newFocus->inherits("QDateEdit")
          || newFocus->inherits("QTimeEdit")
          || newFocus->inherits("QDateTimeEdit")) {
      showSoftKeyboard();
    } else {
#if 0
      if (oldFocus->inherits("QLineEdit")
            || newFocus->inherits("QTextEdit")
            || newFocus->inherits("QPlainTextEdit")
            || newFocus->inherits("QTextBrowser")
            || newFocus->inherits("QSpinBox")
            || newFocus->inherits("QDoubleSpinBox")
            || newFocus->inherits("QDateEdit")
            || newFocus->inherits("QTimeEdit")
            || newFocus->inherits("QDateTimeEdit")) {
        QList<QInputMethodEvent::Attribute> attributes;
        QTextCharFormat textFormat;
        textFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        attributes.append(QInputMethodEvent::Attribute(
            QInputMethodEvent::TextFormat, 0, 0, textFormat));
        emit stringGeneratedToOld("", attributes, oldFocus);
        // QInputMethodEvent event("", attributes);
        // QApplication::postEvent(newFocus, &event);
      }
#endif
      hideSoftKeyboard();
    }
  }
}

void ui_soft_keyboard::buttonClicked(QWidget* w) {
  QString text;
  int offset = 0;

  offset = getLastFocusCursorIndex();
  if (!w)
    return;

  /* return button clicked handle */
  if (w == m_btns_[28]) {
    returnClick();
    return;
  }

  if (!is_chinese || cur_type == SpecialChar) {
    /* english or special char input handle */
    if (getBtnText(w, text, 0, 26) && text.size() > 0) {
      whole_pre_str = whole_pre_str.insert(offset, text);
      emit characterGenerated(text[0].unicode());
    } else if (w == m_btns_[26]) {
      // offset = backspaceClick();
      if (offset == 0)
        return;
      whole_pre_str = whole_pre_str.remove(offset - 1, 1);
      emit characterGenerated(Qt::Key_Backspace);
    } else if (w == m_btns_[27]) {
      whole_pre_str = whole_pre_str.insert(offset, " ");
      emit characterGenerated(Qt::Key_Space);
    } else if (w == m_btns_[39]) {
      whole_pre_str = whole_pre_str.insert(offset, ui.btn_dot_->text());
      emit characterGenerated(ui.btn_dot_->text()[0].unicode());
    }
  } else {
    /* chinese input handle */
    if (getBtnText(w, text, 0, 26) && text.size() > 0)
      a2zClick(text, offset);
    else if (getBtnText(w, text, 29, 10) && text.size() > 0)
      candidateClick(text, offset);
    else if (w == m_btns_[26])
      backspaceClick(offset);
    else if (w == m_btns_[27])
      spaceClick(offset);
    else if (w == m_btns_[39]) {
      whole_pre_str = whole_pre_str.insert(offset, ui.btn_dot_->text());
      emit characterGenerated(ui.btn_dot_->text()[0].unicode());
    }
  }

  ui.lab_pinying_->setText(whole_pre_str);
  // setTextFocusedWidget(offset, text);
}

#ifdef DRAG_MODE
void ui_soft_keyboard::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    pt_press = event->globalPos();
    left_btn_press = true;
  }
  event->ignore();
}

void ui_soft_keyboard::mouseMoveEvent(QMouseEvent * event) {
  if (left_btn_press) {
    move(pos() + event->globalPos() - pt_press);
    pt_press = event->globalPos();
  }
  event->ignore();
}

void ui_soft_keyboard::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton)
    left_btn_press = false;
  event->ignore();
}
#endif

/*
void ui_soft_keyboard::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.fillRect(rect(), QColor(153, 153, 153, 127));
}
*/

void ui_soft_keyboard::on_btn_language__clicked() {
  pinyin_search = "";
  is_chinese = !is_chinese;

  if (!init_done) {
    bool ret = initChinese();
    if (ret)
      init_done = true;
  }

  ui.btn_language_->setText(is_chinese ? tr("中/英") : tr("英/中"));
  if (is_chinese) {
    ui.btn_dot_->setText("。");
  } else {
    ui.widget_chinese_->hide();
    ui.btn_dot_->setText(".");
  }

  int offset = getLastFocusCursorIndex();
  whole_pre_str = whole_pre_str.remove(offset, py_pre_str.size());
  QList<QInputMethodEvent::Attribute> attributes;
  QTextCharFormat textFormat;
  textFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
  attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
      0, 0, textFormat));
  emit stringGenerated("", attributes);

  py_pre_str = "";
  pinyin_search = "";
  ui.lab_pinying_->setText(whole_pre_str);
}

void ui_soft_keyboard::on_btn_caplock__clicked() {
  if (cur_type == SpecialChar)
    return;

  pre_type = cur_type;
  is_uppercase = !is_uppercase;
  cur_type = is_uppercase ? UpperCase : LowerCase;

  setBtnText(cur_type);
}

void ui_soft_keyboard::on_btn_special__clicked() {
  if (cur_type == SpecialChar) {
    cur_type = pre_type;
    pre_type = SpecialChar;
  } else {
    pre_type = cur_type;
    cur_type = SpecialChar;
  }

  if (cur_type == SpecialChar)
    ui.btn_special_->setText("ABC");
  else
    ui.btn_special_->setText("?123");

  setBtnText(cur_type);
  ui.widget_chinese_->hide();

  int offset = getLastFocusCursorIndex();
  whole_pre_str = whole_pre_str.remove(offset, py_pre_str.size());
  QList<QInputMethodEvent::Attribute> attributes;
  QTextCharFormat textFormat;
  textFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
  attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
      0, 0, textFormat));
  emit stringGenerated("", attributes);

  py_pre_str = "";
  pinyin_search = "";
  ui.lab_pinying_->setText(whole_pre_str);
}

void ui_soft_keyboard::on_btn_next__clicked() {
  if (!is_chinese)
    return;

  showChinese(cand_current_page + 1, 0);
}

void ui_soft_keyboard::on_btn_last__clicked() {
  if (!is_chinese)
    return;

  showChinese(cand_current_page - 1, 0);
}

bool ui_soft_keyboard::initChinese() {
  google_pinyin = new GooglePinyin;

  bool ret = google_pinyin->init(QString(DICT_PATH) + "/dict");
  if (!ret)
    ALOGE("%s: init google pinyin service failed!", __func__);
  else
    ALOGD("%s: init google pinyin service success!", __func__);

  return ret;
}

bool ui_soft_keyboard::getBtnText(QWidget* widget,
      QString& text, int start, int count) {
  text = "";

  for (int i = start; i < start + count; ++i) {
    if (widget == m_btns_[i]) {
      text = m_btns_[i]->text();
      return true;
    }
  }

  return false;
}

void ui_soft_keyboard::showChinese(int page, int pos) {
  QString str_cand;
  int cand_max_count = 5;

  if (page < 0)
    page = 0;
  if (pos < 0 || pos > 9)
    pos = 0;

  for (int i = 0; i < cand_max_count; ++i) {
    if (page * cand_max_count + i > cand_num)
      str_cand = "";
    str_cand = google_pinyin->getCandidate(page * cand_max_count + i);
    if (str_cand.size() > CAND_MAX_LEN)
      str_cand = str_cand.mid(0, CAND_MAX_LEN);
    m_btns_[29 + i]->setText(str_cand);
  }

  if (m_btns_[29 + pos]->text().isEmpty())
    pos = cand_current_pos;

  cand_current_page = page;
  cand_current_pos = pos;
}

QString ui_soft_keyboard::getPrePinyin() {
  QString surface = google_pinyin->pinyinString(false);
  QVector<int> splStart = google_pinyin->spellingStartPositions();
  int fixedLen = google_pinyin->fixedLength();
  QString composingStrDisplay;
  int surfaceDecodedLen = google_pinyin->pinyinStringLength(true);

  if (!surfaceDecodedLen) {
    composingStrDisplay = surface.mid(splStart[fixedLen + 1]);
  } else {
    for (int pos = fixedLen + 1; pos < splStart.size() - 1; pos++) {
      composingStrDisplay += surface.mid(splStart[pos],
          splStart[pos + 1] - splStart[pos]);
      if (splStart[pos + 1] < surfaceDecodedLen)
        composingStrDisplay += QLatin1String("'");
    }
    if (surfaceDecodedLen < surface.length())
      composingStrDisplay += surface.mid(surfaceDecodedLen).toLower();
  }

  return composingStrDisplay;
}

void ui_soft_keyboard::hideSoftKeyboard() {
  if (!this->isVisible())
    return;

  google_pinyin->flushCache();
  if (ui.widget_chinese_->isVisible())
    ui.widget_chinese_->hide();

  int offset = getLastFocusCursorIndex();
  whole_pre_str = whole_pre_str.remove(offset, py_pre_str.size());
  QList<QInputMethodEvent::Attribute> attributes;
  QTextCharFormat textFormat;
  textFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
  attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
      0, 0, textFormat));
  emit stringGenerated("", attributes);

  pinyin_search = "";
  cand_current_pos = -1;
  cand_current_page = -1;
  py_pre_str = "";
  cand_num = 0;

  this->hide();
}

bool ui_soft_keyboard::isLastFocusVisibleAll() {
  bool ret = false;
  QPoint r = last_focused_widget->mapToGlobal(QPoint(0, 0));
  int x = r.x() + last_focused_widget->width();
  int y = r.y() + last_focused_widget->height();
  ret = this->x() < x && this->y() < y;

  return !ret;
}

void ui_soft_keyboard::showSoftKeyboard() {
  ui.btn_language_->setText(is_chinese ? tr("中/英") : tr("英/中"));
  if (!this->isVisible()) {
    this->move(0, get_screen_height() - this->height());
    this->resize(get_screen_width(), this->height());
    this->show();
  }

  QString focus_text;
  QString str_validator;
  QString str = QString(QLatin1String(
      last_focused_widget->metaObject()->className()));

  if (str.compare(QString("QLineEdit")) == 0) {
    QLineEdit *last_focused_edit =
        qobject_cast<QLineEdit *>(last_focused_widget);
    if (last_focused_edit->validator())
      str_validator = QString(QLatin1String(
          last_focused_edit->validator()->metaObject()->className()));

    if (last_focused_edit->echoMode() == QLineEdit::Password ||
        str_validator.compare(QString("QIntValidator")) == 0 ||
        str_validator.compare(QString("QDoubleValidator")) == 0) {
      if (is_chinese)
        on_btn_language__clicked();
      ui.btn_language_->setEnabled(false);
    } else {
      ui.btn_language_->setEnabled(true);
    }

    if (last_focused_edit->echoMode() == QLineEdit::Password)
      ui.lab_pinying_->setEchoMode(QLineEdit::Password);
    else
      ui.lab_pinying_->setEchoMode(QLineEdit::Normal);

    focus_text = last_focused_edit->text();
  } else if (str.compare(QString("QTextEdit")) == 0) {
    QTextEdit *last_focused_edit =
        qobject_cast<QTextEdit *>(last_focused_widget);
    focus_text = last_focused_edit->toPlainText();
  } else if (str.compare(QString("QSpinBox")) == 0) {
    QSpinBox *last_focused_edit = qobject_cast<QSpinBox *>(last_focused_widget);
    focus_text = last_focused_edit->text();
  } else if (str.compare(QString("QDoubleSpinBox")) == 0) {
    QDoubleSpinBox *last_focused_edit =
        qobject_cast<QDoubleSpinBox *>(last_focused_widget);
    focus_text = last_focused_edit->text();
  } else if (str.compare(QString("QDateEdit")) == 0) {
    QDateEdit *last_focused_edit =
        qobject_cast<QDateEdit *>(last_focused_widget);
    focus_text = last_focused_edit->text();
  } else if (str.compare(QString("QTimeEdit")) == 0) {
    QTimeEdit *last_focused_edit =
        qobject_cast<QTimeEdit *>(last_focused_widget);
    focus_text = last_focused_edit->text();
  } else if (str.compare(QString("QDateTimeEdit")) == 0) {
    QDateTimeEdit *last_focused_edit =
        qobject_cast<QDateTimeEdit *>(last_focused_widget);
    focus_text = last_focused_edit->text();
  }

  if (last_focused_widget->isVisible() && isLastFocusVisibleAll())
    ui.lab_pinying_->hide();
  else
    ui.lab_pinying_->show();

  ui.lab_pinying_->setText(focus_text);
  whole_pre_str = focus_text;
}

void ui_soft_keyboard::resetSoftKeyboard() {
  whole_pre_str = "";

  if (cur_type == SpecialChar)
    on_btn_special__clicked();

  if (is_uppercase)
    on_btn_caplock__clicked();

  if (is_chinese)
    on_btn_language__clicked();

  if (init_done) {
    google_pinyin->deinit();
    init_done = false;
    ALOGD("%s: google pinyin deinit!", __func__);
  }
}

void ui_soft_keyboard::a2zClick(QString text, int offset) {
  int split_num = 0;

  for (int i = 0; i < py_pre_str.size(); i++) {
    if (QString("'").compare(QString(py_pre_str[i])) == 0)
      split_num++;
  }

  if (pinyin_search.size() >= 20 || split_num >= 8)
    return;
  if (!ui.widget_chinese_->isVisible())
    ui.widget_chinese_->show();

  pinyin_search = pinyin_search + text[0].toLower();
  cand_num = google_pinyin->search(pinyin_search);

  whole_pre_str = whole_pre_str.remove(offset, py_pre_str.size());
  QList<QInputMethodEvent::Attribute> attributes;
  QTextCharFormat textFormat;
  textFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
  attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
      0, 0, textFormat));
  emit stringGenerated("", attributes);

  py_pre_str = getPrePinyin();
  whole_pre_str = whole_pre_str.insert(offset, py_pre_str);

  attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
      0, py_pre_str.length(), textFormat));
  emit stringGenerated(py_pre_str, attributes);

  showChinese(0, 0);
}

void ui_soft_keyboard::candidateClick(QString text, int offset) {
  whole_pre_str = whole_pre_str.remove(offset, py_pre_str.size());
  QList<QInputMethodEvent::Attribute> attributes;
  QTextCharFormat textFormat;
  textFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
  attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
      0, 0, textFormat));
  emit stringGenerated("", attributes);

  whole_pre_str = whole_pre_str.insert(offset, text);
  for (int i = 0; i < text.size(); i++) {
    emit characterGenerated(text[i].unicode());
  }
  py_pre_str = "";
  pinyin_search = "";
}

int ui_soft_keyboard::setLastFocusCursorIndex(int index) {
  int ret = -1;
  QString str = QString(QLatin1String(
      last_focused_widget->metaObject()->className()));

  if (str.compare(QString("QLineEdit")) == 0) {
    QLineEdit *last_focused_edit =
        qobject_cast<QLineEdit *>(last_focused_widget);
    last_focused_edit->setCursorPosition(index);
    ret = 0;
  } else if (str.compare(QString("QTextEdit")) == 0) {
    QTextEdit *last_focused_edit =
        qobject_cast<QTextEdit *>(last_focused_widget);
    QTextCursor tmpCursor = last_focused_edit->textCursor();
    tmpCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, index);
    last_focused_edit->setTextCursor(tmpCursor);
    ret = 0;
  } else if (str.compare(QString("QPlainTextEdit")) == 0) {
    QPlainTextEdit *last_focused_edit =
        qobject_cast<QPlainTextEdit *>(last_focused_widget);
    QTextCursor tmpCursor = last_focused_edit->textCursor();
    tmpCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, index);
    last_focused_edit->setTextCursor(tmpCursor);
    ret = 0;
  } else if (str.compare(QString("QTextBrowser")) == 0) {
    QTextBrowser *last_focused_edit =
        qobject_cast<QTextBrowser *>(last_focused_widget);
    QTextCursor tmpCursor = last_focused_edit->textCursor();
    tmpCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, index);
    last_focused_edit->setTextCursor(tmpCursor);
    ret = 0;
  }

  return ret;
}

int ui_soft_keyboard::getLastFocusCursorIndex() {
  int index = -1;
  QString str = QString(QLatin1String(
      last_focused_widget->metaObject()->className()));

  if (str.compare(QString("QLineEdit")) == 0) {
    QLineEdit *last_focused_edit =
        qobject_cast<QLineEdit *>(last_focused_widget);
    index = last_focused_edit->cursorPosition();
  } else if (str.compare(QString("QTextEdit")) == 0) {
    QTextEdit *last_focused_edit =
        qobject_cast<QTextEdit *>(last_focused_widget);
    QTextCursor tmpCursor = last_focused_edit->textCursor();
    index = tmpCursor.position();
  } else if (str.compare(QString("QPlainTextEdit")) == 0) {
    QPlainTextEdit *last_focused_edit =
        qobject_cast<QPlainTextEdit *>(last_focused_widget);
    QTextCursor tmpCursor = last_focused_edit->textCursor();
    index = tmpCursor.position();
  } else if (str.compare(QString("QTextBrowser")) == 0) {
    QTextBrowser *last_focused_edit =
        qobject_cast<QTextBrowser *>(last_focused_widget);
    QTextCursor tmpCursor = last_focused_edit->textCursor();
    index = tmpCursor.position();
  } else if (str.compare(QString("QSpinBox")) == 0) {
    // Fixme: could not cursor position
    index = whole_pre_str.size();
  } else if (str.compare(QString("QDoubleSpinBox")) == 0) {
    // Fixme: could not cursor position
    index = whole_pre_str.size();
  } else if (str.compare(QString("QDateEdit")) == 0) {
    // Fixme: could not cursor position
    index = whole_pre_str.size();
  } else if (str.compare(QString("QTimeEdit")) == 0) {
    // Fixme: could not cursor position
    index = whole_pre_str.size();
  } else if (str.compare(QString("QDateTimeEdit")) == 0) {
    // Fixme: could not cursor position
    index = whole_pre_str.size();
  } else {
    index = whole_pre_str.size();
  }

  return index;
}

void ui_soft_keyboard::backspaceClick(int offset) {
  if (pinyin_search.size() > 0) {
    size_t pos = pinyin_search.size() - 1;
    pinyin_search = pinyin_search.left(pinyin_search.size() - 1);
    cand_num = google_pinyin->deleteSearch(pos, false, false);

    whole_pre_str = whole_pre_str.remove(offset, py_pre_str.size());
    py_pre_str = py_pre_str.left(py_pre_str.size() - 1);
    if (py_pre_str.endsWith("'"))
      py_pre_str = py_pre_str.left(py_pre_str.size() - 1);
    whole_pre_str = whole_pre_str.insert(offset, py_pre_str);

    QList<QInputMethodEvent::Attribute> attributes;
    QTextCharFormat textFormat;
    textFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    attributes.append(QInputMethodEvent::Attribute(
        QInputMethodEvent::TextFormat, 0, 0, textFormat));
    emit stringGenerated("", attributes);
    attributes.append(QInputMethodEvent::Attribute(
        QInputMethodEvent::TextFormat, 0, py_pre_str.length(), textFormat));
    emit stringGenerated(py_pre_str, attributes);

    showChinese(0, 0);
  } else {
      if (offset == 0)
        return;
      whole_pre_str = whole_pre_str.remove(offset - 1, 1);
      emit characterGenerated(Qt::Key_Backspace);
  }

  return;
}

void ui_soft_keyboard::spaceClick(int offset) {
  if (py_pre_str.isEmpty()) {
    whole_pre_str = whole_pre_str.insert(offset, " ");
    emit characterGenerated(Qt::Key_Space);
  } else {
    whole_pre_str = whole_pre_str.remove(offset, py_pre_str.size());
    QList<QInputMethodEvent::Attribute> attributes;
    QTextCharFormat textFormat;
    textFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    attributes.append(QInputMethodEvent::Attribute(
        QInputMethodEvent::TextFormat, 0, 0, textFormat));
    emit stringGenerated("", attributes);

    QString text = ui.word_0_->text();
    if (!text.isEmpty()) {
      whole_pre_str = whole_pre_str.insert(offset, text);
      for (int i = 0; i < text.size(); i++) {
        emit characterGenerated(text[i].unicode());
      }
    }

    pinyin_search = "";
    py_pre_str = "";
  }
}

void ui_soft_keyboard::setTextFocusedWidget(int offset, QString text) {
  int key = -1;

  if (isLastFocusVisibleAll()) {
    for (int i = 0; i < text.size(); i++) {
      key = text[i].unicode();
      emit characterGenerated(key);
    }

    return;
  }

  QString str = QString(QLatin1String(
      last_focused_widget->metaObject()->className()));

  if (str.compare(QString("QLineEdit")) == 0) {
    QLineEdit *last_focused_edit =
        qobject_cast<QLineEdit *>(last_focused_widget);
    last_focused_edit->clear();
  } else if (str.compare(QString("QTextEdit")) == 0) {
    QTextEdit *last_focused_edit =
        qobject_cast<QTextEdit *>(last_focused_widget);
    last_focused_edit->clear();
  } else if (str.compare(QString("QPlainTextEdit")) == 0) {
    QPlainTextEdit *last_focused_edit =
        qobject_cast<QPlainTextEdit *>(last_focused_widget);
    last_focused_edit->clear();
  } else if (str.compare(QString("QTextBrowser")) == 0) {
    QTextBrowser *last_focused_edit =
        qobject_cast<QTextBrowser *>(last_focused_widget);
    last_focused_edit->clear();
  } else if (str.compare(QString("QSpinBox")) == 0) {
    QSpinBox *last_focused_edit =
        qobject_cast<QSpinBox *>(last_focused_widget);
    last_focused_edit->clear();
  } else if (str.compare(QString("QDoubleSpinBox")) == 0) {
    QDoubleSpinBox *last_focused_edit =
        qobject_cast<QDoubleSpinBox *>(last_focused_widget);
    last_focused_edit->clear();
  } else if (str.compare(QString("QDateEdit")) == 0) {
    QDateEdit *last_focused_edit =
        qobject_cast<QDateEdit *>(last_focused_widget);
    last_focused_edit->clear();
  } else if (str.compare(QString("QTimeEdit")) == 0) {
    QTimeEdit *last_focused_edit =
        qobject_cast<QTimeEdit *>(last_focused_widget);
    last_focused_edit->clear();
  } else if (str.compare(QString("QDateTimeEdit")) == 0) {
    QDateTimeEdit *last_focused_edit =
        qobject_cast<QDateTimeEdit *>(last_focused_widget);
    last_focused_edit->clear();
  }
  for (int i = 0; i < whole_pre_str.size() - py_pre_str.size(); i++) {
    key = whole_pre_str[i].unicode();
    emit characterGenerated(key);
  }

  while (offset-- > 0) {
    emit characterGenerated(Qt::Key_Left);
  }
}

void ui_soft_keyboard::returnClick() {
  // setTextFocusedWidget(0, "");

  whole_pre_str = "";
  pinyin_search = "";
  py_pre_str = "";

  hideSoftKeyboard();
}

QWidget* ui_soft_keyboard::getLastFocus() {
  return this->last_focused_widget;
}

