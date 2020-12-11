/*
* ui_aboat_storage.hpp define about setting function of UI
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

#ifndef UI_ABOAT_STORAGE_H
#define UI_ABOAT_STORAGE_H

#include <QWidget>
#include <QLabel>

#include "clicked_label.hpp"
#include "my_progressbar.hpp"
#include "QtMainWindow.hpp"

namespace Ui {
class ui_aboat_storage;
}

class ui_aboat_storage : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_aboat_storage(QWidget *parent = 0);
  ~ui_aboat_storage();

 signals:

 private:
  QLabel *label_free;
  QLabel *label_free_text;
  QLabel *label_used;
  QLabel *label_used_text;

  QLabel *label_free_number;
  QLabel *label_free_text1;

  QLabel *label_used_number;
  QLabel *label_used_text1;

  QLabel *label_total_number;
  QLabel *label_total_text1;

  my_progressbar *progress;
  QWidget *center_widget;
  Ui::ui_aboat_storage *ui;
};

#endif  //  UI_ABOAT_STORAGE_H

