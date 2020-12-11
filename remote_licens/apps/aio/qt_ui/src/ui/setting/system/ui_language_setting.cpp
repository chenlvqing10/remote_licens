/*
* ui_base_widget.cpp - define ui_base_widget of project
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
#include <QCloseEvent>
#include <QPainter>
#include <QTranslator>
#include <QApplication>
extern "C" {
#define LOG_TAG     "ui"
#include <log/log.h>
#include "ui_control.h"
}
#include "ui_language_setting.hpp"
#include "ui_rotate_management.hpp"

ui_language_setting::ui_language_setting(QWidget* parent) : QWidget(parent) {
  setFixedSize(1280, 720);
  setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_DeleteOnClose);

  small_dlg = new QWidget(this);
  small_dlg->setGeometry(QRect(220, 168, 840, 380));
  small_dlg->setAttribute(Qt::WA_StyledBackground, true);
  small_dlg->setObjectName("language_small_dlg");

  label_for_title = new QLabel(small_dlg);
  label_for_title->setGeometry(QRect(330, 32, 200, 40));
  label_for_title->setAlignment(Qt::AlignCenter);
  label_for_title->setObjectName("language_label_for_title");
  label_for_title->setText(tr("语言设置"));
  ll = new QFrame(small_dlg);
  ll->setObjectName("language_ll");
  ll->setStyleSheet("#language_ll {border:2px solid #102050;}");
  ll->setGeometry(QRect(0, 94, 840, 4));

  ll1 = new QFrame(small_dlg);
  ll1->setObjectName("language_ll1");
  ll1->setStyleSheet("#language_ll1 {border:2px solid #102050;}");
  ll1->setGeometry(QRect(50, 190, 720, 4));

  label_for_english_text = new QLabel(small_dlg);
  label_for_english_text->setObjectName("language_label_for_english_text");
  label_for_english_text->setGeometry(QRect(60, 112, 200, 72));
  label_for_english_text->setText(tr("中文"));

  label_for_chinese_text = new QLabel(small_dlg);
  label_for_chinese_text->setObjectName("language_label_for_chinese_text");
  label_for_chinese_text->setGeometry(QRect(60, 210, 200, 72));
  label_for_chinese_text->setText(tr("英文"));

  label_for_chinese_select = new clicked_Label(small_dlg);
  label_for_chinese_select->setObjectName("language_label_for_chinese_select");
  label_for_chinese_select->setGeometry(QRect(840 - 50 - 56, 112, 56, 56));

  label_for_english_select = new clicked_Label(small_dlg);
  label_for_english_select->setObjectName("language_label_for_english_select");
  label_for_english_select->setGeometry(QRect(840 - 50 - 56, 210, 56, 56));
  /**/
  device_lang_param_t info;
  memset(&info, 0, sizeof(device_lang_param_t));

  ui_control_get_lang(&info);

  current_language = info.lang;

  if (current_language == 0) {
    label_for_chinese_select->setStyleSheet(
        "#language_label_for_chinese_select "
        "{border-image: url(:icon/setting/choice.png);}");
  } else {
    label_for_english_select->setStyleSheet(
        "#language_label_for_english_select "
        "{border-image: url(:icon/setting/choice.png);}");
  }
  connect(label_for_english_select, SIGNAL(clicked()),
        this, SLOT(slot_for_english_clicked()));
  connect(label_for_chinese_select, SIGNAL(clicked()),
        this, SLOT(slot_for_chinese_clicked()));
}

void ui_language_setting::slot_for_english_clicked() {
  switch_language(1);
  label_for_chinese_select->setStyleSheet(
    "{border-image: "
    "url(:icon/setting/choice1.png);}");
  label_for_english_select->setStyleSheet(
      "#language_label_for_english_select "
      "{border-image: url(:icon/setting/choice.png);}");
  close();
}
void ui_language_setting::slot_for_chinese_clicked() {
  switch_language(0);
  label_for_english_select->setStyleSheet(
      "{border-image: url(:icon/setting/choice1.png);}");
  label_for_chinese_select->setStyleSheet(
      "language_label_for_chinese_select "
      "{border-image: url(:icon/setting/choice.png);}");
  close();
}
void ui_language_setting::closeEvent(QCloseEvent* event) {
  if (m_Loop != NULL) {
    m_Loop->exit();
  }
  event->accept();
}
int ui_language_setting::exec() {
  this->show();
  m_Loop = new QEventLoop(this);
  m_Loop->exec();
  return current_language;
}
void ui_language_setting::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.fillRect(this->rect(), QColor(0, 0, 0, 200));
  QWidget::paintEvent(event);
}
void ui_language_setting::switch_language(int type) {
  current_language = type;
  ui_control_set_lang(type);

  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();

  if (type == 0) {
    qApp->removeTranslator(&rotate_signgleton->translator);
  } else {
    rotate_signgleton->translator.load(":icon/setting/qt_gui_en_US.qm");
    qApp->installTranslator(&rotate_signgleton->translator);
  }
}
