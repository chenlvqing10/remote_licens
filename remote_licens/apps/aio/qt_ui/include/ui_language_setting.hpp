/*
* ui_language_setting.hpp - define language setting
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

#ifndef UI_LANGUAGE_SETTING_HPP
#define UI_LANGUAGE_SETTING_HPP

#include <QWidget>
#include <QLabel>
#include <QEventLoop>

#include "clicked_label.hpp"
class ui_language_setting : public QWidget {
  Q_OBJECT

 public:
  explicit ui_language_setting(QWidget *parent = nullptr);

 protected:
  void closeEvent(QCloseEvent *event);
  void paintEvent(QPaintEvent *);

 public:
  int exec();
  void switch_language(int type);

 private slots:
  void slot_for_english_clicked();
  void slot_for_chinese_clicked();

 private:
  QWidget *small_dlg;
  QLabel *label_for_title;
  QFrame *ll;
  QFrame *ll1;
  QLabel *label_for_english_text;
  QLabel *label_for_chinese_text;
  clicked_Label *label_for_english_select;
  clicked_Label *label_for_chinese_select;
  int current_language;
  QEventLoop *m_Loop;
};

#endif  // UI_LANGUAGE_SETTING_HPP
