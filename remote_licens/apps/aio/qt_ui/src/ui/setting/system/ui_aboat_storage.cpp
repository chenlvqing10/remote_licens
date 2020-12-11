/*
* ui_aboat_storage.cpp - define aboat storage of project
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
#define LOG_TAG     "ui"
#include <log/log.h>
#include "ui_control.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QtDebug>

#include "ui_rotate_management.hpp"
#include "ui_aboat_storage.hpp"
#include "ui_ui_aboat_storage.h"

#define POS(y) (y-(720-648))

ui_aboat_storage::ui_aboat_storage(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_aboat_storage) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  center_widget = ui->storage_center_widget;

  /*read storage info*/
  memory mem;
  memset(&mem, 0, sizeof(memory));
  ui_control_sta_mem(&mem);
#ifndef PORTRAIT_SCREEN
  progress = new my_progressbar(center_widget);
  progress->setObjectName("storage_progress");
  progress->setGeometry(QRect(116, POS(134), 1048, 42));
  progress->setRange(0, 1000);
  int value = mem.used * 1000 / mem.total;
  progress->setValue(value);
  progress->setTextVisible(false);
  progress->setInvertedAppearance(false);
  label_free = new QLabel(center_widget);
  label_free->setObjectName("storage_label_free");
  label_free->setGeometry(QRect(116, POS(198), 24, 24));

  label_free_text = new QLabel(center_widget);
  label_free_text->setObjectName("storage_label_free_text");
  label_free_text->setGeometry(QRect(150, POS(198), 160, 24));
  label_free_text->setText(tr("剩余空间"));

  label_used = new QLabel(center_widget);
  label_used->setObjectName("storage_label_used");
  label_used->setGeometry(QRect(314, POS(198), 24, 24));

  label_used_text = new QLabel(center_widget);
  label_used_text->setObjectName("storage_label_used_text");
  label_used_text->setGeometry(QRect(348, POS(198), 160, 24));
  label_used_text->setText(tr("已用空间"));
  /**/

  label_used_number = new QLabel(center_widget);
  label_used_number->setObjectName("storage_label_used_number");
  label_used_number->setGeometry(QRect(182, POS(308), 164, 164));
  label_used_number->setText(QString("%1M").arg(mem.used));
  label_used_number->setAlignment(Qt::AlignCenter);

  label_used_text1 = new QLabel(center_widget);
  label_used_text1->setGeometry(QRect(184, POS(502), 152, 48));
  label_used_text1->setText(tr("已用空间"));
  label_used_text1->setAlignment(Qt::AlignCenter);

  label_free_number = new QLabel(center_widget);
  label_free_number->setObjectName("storage_label_free_number");
  label_free_number->setGeometry(QRect(558, POS(308), 164, 164));
  label_free_number->setText(QString("%1M").arg(mem.free));
  label_free_number->setAlignment(Qt::AlignCenter);

  label_free_text1 = new QLabel(center_widget);
  label_free_text1->setGeometry(QRect(568, POS(502), 152, 48));
  label_free_text1->setText(tr("剩余空间"));
  label_free_text1->setAlignment(Qt::AlignCenter);

  label_total_number = new QLabel(center_widget);
  label_total_number->setObjectName("storage_label_total_number");
  label_total_number->setGeometry(QRect(934, POS(308), 164, 164));
  label_total_number->setText(QString("%1M").arg(mem.total));
  label_total_number->setAlignment(Qt::AlignCenter);

  label_total_text1 = new QLabel(center_widget);
  label_total_text1->setGeometry(QRect(955, POS(502), 132, 48));
  label_total_text1->setText(tr("总空间"));
  label_total_text1->setAlignment(Qt::AlignCenter);
#else

  label_total_text1 = new QLabel(center_widget);
  label_total_text1->setObjectName("storage_label_total_text1");
  label_total_text1->setGeometry(QRect(60, 168, 132, 24));
  label_total_text1->setText(tr("总空间"));

  label_total_number = new QLabel(center_widget);
  label_total_number->setObjectName("storage_label_total_number");
  label_total_number->setGeometry(QRect(132, 168, 160, 24));
  label_total_number->setText(QString("%1G").arg(QString::
    number((float)mem.total/1024, 'f', 2)));

  progress = new my_progressbar(center_widget);
  progress->setObjectName("storage_progress");
  progress->setGeometry(QRect(60, 212, 680, 42));
  progress->setRange(0, 1000);
  int value = mem.used * 1000 / mem.total;
  progress->setValue(value);
  progress->setTextVisible(false);

  label_free = new QLabel(center_widget);
  label_free->setObjectName("storage_label_free");
  label_free->setGeometry(QRect(60, 274, 24, 24));

  label_free_text = new QLabel(center_widget);
  label_free_text->setObjectName("storage_label_free_text");
  label_free_text->setGeometry(QRect(90, 274, 160, 24));
  label_free_text->setText(tr("剩余空间"));

  label_used = new QLabel(center_widget);
  label_used->setObjectName("storage_label_used");
  label_used->setGeometry(QRect(256, 274, 24, 24));

  label_used_text = new QLabel(center_widget);
  label_used_text->setObjectName("storage_label_used_text");
  label_used_text->setGeometry(QRect(286, 274, 160, 24));
  label_used_text->setText(tr("已用空间"));

  label_used_number = new QLabel(center_widget);
  label_used_number->setObjectName("storage_label_used_number");
  label_used_number->setGeometry(QRect(62, 408, 228, 228));
  label_used_number->setText(QString("%1G").arg(QString::
      number((float)mem.used/1024, 'f', 2)));
  label_used_number->setAlignment(Qt::AlignCenter);

  label_used_text1 = new QLabel(center_widget);
  label_used_text1->setGeometry(QRect(104, 660, 152, 48));
  label_used_text1->setText(tr("已用空间"));
  label_used_text1->setAlignment(Qt::AlignCenter);

  label_free_number = new QLabel(center_widget);
  label_free_number->setObjectName("storage_label_free_number");
  label_free_number->setGeometry(QRect(508, 408, 228, 228));
  label_free_number->setText(QString("%1G").arg(QString::
      number((float)mem.free/1024, 'f', 2)));
  label_free_number->setAlignment(Qt::AlignCenter);

  label_free_text1 = new QLabel(center_widget);
  label_free_text1->setGeometry(QRect(550, 660, 152, 48));
  label_free_text1->setText(tr("剩余空间"));
  label_free_text1->setAlignment(Qt::AlignCenter);
#endif

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  QObject::connect(ui->storage_back_btn, SIGNAL(clicked()),
                   rotate_signgleton, SLOT(back_activity_win()));
}

ui_aboat_storage::~ui_aboat_storage() {
  delete ui;
}

