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

#include <QDebug>
#include <QDateTime>
extern "C" {
#define LOG_TAG     "ui"
#include <log/log.h>
#include "ui_control.h"
}
#include "ui_time_setting.hpp"
#include "ui_rotate_management.hpp"

#define POS(y) (y-(720-648))

ui_time_setting::ui_time_setting() {
  QString title = tr("时间设置");
  set_title_text(title);
  center_widget->resize(1280, 648);
  QStringList name_list;
  name_list << tr("自动设置") << tr("手动设置");
  /*read from db*/
  device_time_param_t param;
  memset(&param, 0, sizeof(device_time_param_t));
  ui_control_get_time_param(&param);
  auto_enable = param.auto_set_time;
  list_view = new QListView(center_widget);
  list_view->setGeometry(QRect(0, 0, 1200, 200));
  model = new QStandardItemModel();
  for (int i = 0; i < name_list.size(); i++) {
    QStandardItem* item = new QStandardItem();
    data_time_setting_delegate data;
    data.text = name_list.at(i);
    switch (i) {
      case 0:
        label_for_auto_btn = new clicked_Label(list_view);
        label_for_auto_btn->setObjectName("time_auto_btn");
        data.widget = label_for_auto_btn;
        data.is_display = true;
        if (auto_enable) {
          label_for_auto_btn->setStyleSheet(
            "#time_auto_btn {border-image:"
                  "url(:icon/setting/turn_on.png);}");
        } else {
          label_for_auto_btn->setStyleSheet(
            "#time_auto_btn  {border-image:"
                   "url(:icon/setting/turn_off.png);}");
        }
        break;
      case 1:
        label_for_time_view = new QLabel(list_view);
        data.widget = label_for_time_view;
        data.text1 = QDateTime::currentDateTime().
        toString("yyyy/MM/dd hh:mm:ss");
        label_for_time_view->setText(data.text1);
        if (auto_enable) {
          data.is_display = false;
          label_for_time_view->hide();
        } else {
          data.is_display = true;
          break;
          }
    }
    item->setData(QVariant::fromValue(data), Qt::UserRole + 1);
    model->appendRow(item);
  }
  delegate = new ui_time_setting_delegate();
  list_view->setModel(model);
  list_view->setItemDelegate(delegate);

  label_for_year_sub = new clicked_Label(center_widget);
  label_for_year_sub->setObjectName("time_year_sub");
  label_for_year_sub->setGeometry(QRect(110, POS(482), 72, 72));

  label_for_mon_sub = new clicked_Label(center_widget);
  label_for_mon_sub->setObjectName("time_mon_sub");
  label_for_mon_sub->setGeometry(QRect(296, POS(482), 72, 72));

  label_for_day_sub = new clicked_Label(center_widget);
  label_for_day_sub->setObjectName("time_day_sub");
  label_for_day_sub->setGeometry(QRect(454, POS(482), 72, 72));

  label_for_hour_sub = new clicked_Label(center_widget);
  label_for_hour_sub->setObjectName("time_hour_sub");
  label_for_hour_sub->setGeometry(QRect(660, POS(482), 72, 72));

  label_for_min_sub = new clicked_Label(center_widget);
  label_for_min_sub->setObjectName("time_min_sub");
  label_for_min_sub->setGeometry(QRect(814, POS(482), 72, 72));

  label_for_sec_sub = new clicked_Label(center_widget);
  label_for_sec_sub->setObjectName("time_sec_sub");
  label_for_sec_sub->setGeometry(QRect(970, POS(482), 72, 72));

  label_for_year_add = new clicked_Label(center_widget);
  label_for_year_add->setObjectName("time_year_add");
  label_for_year_add->setGeometry(QRect(110, POS(306), 72, 72));


  label_for_mon_add = new clicked_Label(center_widget);
  label_for_mon_add->setObjectName("time_mon_add");
  label_for_mon_add->setGeometry(QRect(296, POS(306), 72, 72));

  label_for_day_add = new clicked_Label(center_widget);
  label_for_day_add->setObjectName("time_day_add");
  label_for_day_add->setGeometry(QRect(454, POS(306), 72, 72));

  label_for_hour_add = new clicked_Label(center_widget);
  label_for_hour_add->setObjectName("time_hour_add");
  label_for_hour_add->setGeometry(QRect(660, POS(306), 72, 72));

  label_for_min_add = new clicked_Label(center_widget);
  label_for_min_add->setObjectName("time_min_add");
  label_for_min_add->setGeometry(QRect(814, POS(306), 72, 72));

  label_for_sec_add = new clicked_Label(center_widget);
  label_for_sec_add->setObjectName("time_sec_add");
  label_for_sec_add->setGeometry(QRect(970, POS(306), 72, 72));

  edit_for_year = new QLineEdit(center_widget);
  edit_for_year->setGeometry(QRect(74, POS(396), 140, 72));
  edit_for_year->setMaxLength(4);
  edit_for_year->setAlignment(Qt::AlignCenter);
  edit_for_year->setText(QString("%1").arg(
      QDateTime::currentDateTime().date().year()));
  edit_for_year->setEnabled(false);

  edit_for_mon = new QLineEdit(center_widget);
  edit_for_mon->setGeometry(QRect(284, POS(396), 96, 72));
  edit_for_mon->setMaxLength(2);
  edit_for_mon->setAlignment(Qt::AlignCenter);
  edit_for_mon->setText(QString("%1").arg(
      QDateTime::currentDateTime().date().month()));
  edit_for_mon->setEnabled(false);

  edit_for_day = new QLineEdit(center_widget);
  edit_for_day->setGeometry(QRect(440, POS(396), 96, 72));
  edit_for_day->setMaxLength(2);
  edit_for_day->setAlignment(Qt::AlignCenter);
  edit_for_day->setText(QString("%1").arg(
      QDateTime::currentDateTime().date().day()));
  edit_for_day->setEnabled(false);

  edit_for_hour = new QLineEdit(center_widget);
  edit_for_hour->setGeometry(QRect(646, POS(396), 96, 72));
  edit_for_hour->setMaxLength(2);
  edit_for_hour->setAlignment(Qt::AlignCenter);
  edit_for_hour->setText(QString("%1").arg(
      QDateTime::currentDateTime().time().hour()));
  edit_for_hour->setEnabled(false);

  edit_for_min = new QLineEdit(center_widget);
  edit_for_min->setGeometry(QRect(802, POS(396), 96, 72));
  edit_for_min->setMaxLength(2);
  edit_for_min->setAlignment(Qt::AlignCenter);
  edit_for_min->setText(QString("%1").arg(
      QDateTime::currentDateTime().time().minute()));
  edit_for_min->setEnabled(false);


  edit_for_sec = new QLineEdit(center_widget);
  edit_for_sec->setGeometry(QRect(956, POS(396), 96, 72));
  edit_for_sec->setMaxLength(2);
  edit_for_sec->setAlignment(Qt::AlignCenter);
  edit_for_sec->setText(QString("%1").arg(
      QDateTime::currentDateTime().time().second()));
  edit_for_sec->setEnabled(false);

  label_for_year_text = new QLabel(center_widget);
  label_for_year_text->setText(tr("年"));
  label_for_year_text->setGeometry(QRect(214, POS(396), 70, 72));
  label_for_year_text->setAlignment(Qt::AlignCenter);

  label_for_mon_text = new QLabel(center_widget);
  label_for_mon_text->setText(tr("月"));
  label_for_mon_text->setGeometry(QRect(380, POS(396), 70, 72));
  label_for_mon_text->setAlignment(Qt::AlignCenter);

  label_for_day_text = new QLabel(center_widget);
  label_for_day_text->setText(tr("日"));
  label_for_day_text->setGeometry(QRect(536, POS(396), 70, 72));
  label_for_day_text->setAlignment(Qt::AlignCenter);

  label_for_hour_text = new QLabel(center_widget);
  label_for_hour_text->setText(tr(":"));
  label_for_hour_text->setGeometry(QRect(742, POS(396), 70, 72));
  label_for_hour_text->setAlignment(Qt::AlignCenter);

  label_for_min_text = new QLabel(center_widget);
  label_for_min_text->setText(tr(":"));
  label_for_min_text->setGeometry(QRect(898, POS(396), 70, 72));
  label_for_min_text->setAlignment(Qt::AlignCenter);

  save_btn = new QPushButton(center_widget);
  save_btn->setObjectName("time_btn_save");
  save_btn->setGeometry(QRect(526, POS(600), 200, 60));
  save_btn->setText(tr("确定"));

  if (auto_enable)
    hide_manual_time(auto_enable);
  else
    hide_manual_time(auto_enable);

  /**/
  connect(label_for_year_add, SIGNAL(clicked()),
  this, SLOT(slot_for_year_add()));
  connect(label_for_mon_add, SIGNAL(clicked()),
  this, SLOT(slot_for_mon_add()));
  connect(label_for_day_add, SIGNAL(clicked()),
  this, SLOT(slot_for_day_add()));
  connect(label_for_hour_add, SIGNAL(clicked()),
  this, SLOT(slot_for_hour_add()));
  connect(label_for_min_add, SIGNAL(clicked()),
  this, SLOT(slot_for_min_add()));
  connect(label_for_sec_add, SIGNAL(clicked()),
  this, SLOT(slot_for_sec_add()));

  connect(label_for_year_sub, SIGNAL(clicked()),
  this, SLOT(slot_for_year_sub()));
  connect(label_for_mon_sub, SIGNAL(clicked()),
  this, SLOT(slot_for_mon_sub()));
  connect(label_for_day_sub, SIGNAL(clicked()),
  this, SLOT(slot_for_day_sub()));
  connect(label_for_hour_sub, SIGNAL(clicked()),
  this, SLOT(slot_for_hour_sub()));
  connect(label_for_min_sub, SIGNAL(clicked()),
  this, SLOT(slot_for_min_sub()));
  connect(label_for_sec_sub, SIGNAL(clicked()),
  this, SLOT(slot_for_sec_sub()));


  connect(this, SIGNAL(signal_back_click()),
  this, SLOT(slot_for_back_click()));
  connect(label_for_auto_btn, SIGNAL(clicked()),
  this, SLOT(slot_for_auto_btn_click()));
  connect(save_btn, SIGNAL(clicked()),
  this, SLOT(slot_for_save_btn_click()));
  connect(list_view, SIGNAL(clicked(const QModelIndex&)),
  this, SLOT(slot_for_list_view_clicked(const QModelIndex&)));
}


void ui_time_setting::slot_for_save_btn_click() {
  systemtime datetime;
  datetime.year = edit_for_year->text().toInt();
  datetime.month = edit_for_mon->text().toInt();
  datetime.day = edit_for_day->text().toInt();
  datetime.hour = edit_for_hour->text().toInt();
  datetime.min = edit_for_min->text().toInt();
  datetime.sec = edit_for_sec->text().toInt();
  ui_control_set_time(&datetime);
  QString tt = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
  label_for_time_view->setText(tt);
}
void ui_time_setting::slot_for_back_click() {
  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}
void ui_time_setting::hide_manual_time(bool enable) {
  QVariant var = model->index(1, 0).data(Qt::UserRole + 1);
  data_time_setting_delegate itemData =
      var.value<data_time_setting_delegate>();
  if (enable) {
    itemData.is_display = false;
    label_for_time_view->hide();
    label_for_year_sub->hide();
    label_for_mon_sub->hide();
    label_for_day_sub->hide();
    label_for_hour_sub->hide();
    label_for_min_sub->hide();
    label_for_sec_sub->hide();

    label_for_year_add->hide();
    label_for_mon_add->hide();
    label_for_day_add->hide();
    label_for_hour_add->hide();
    label_for_min_add->hide();
    label_for_sec_add->hide();

    edit_for_year->hide();
    edit_for_mon->hide();
    edit_for_day->hide();
    edit_for_hour->hide();
    edit_for_min->hide();
    edit_for_sec->hide();

    label_for_year_text->hide();
    label_for_mon_text->hide();
    label_for_day_text->hide();
    label_for_hour_text->hide();
    label_for_min_text->hide();
    save_btn->hide();
  } else {
      itemData.is_display = true;
      label_for_time_view->show();
      label_for_year_sub->show();
      label_for_mon_sub->show();
      label_for_day_sub->show();
      label_for_hour_sub->show();
      label_for_min_sub->show();
      label_for_sec_sub->show();

      label_for_year_add->show();
      label_for_mon_add->show();
      label_for_day_add->show();
      label_for_hour_add->show();
      label_for_min_add->show();
      label_for_sec_add->show();

      edit_for_year->show();
      edit_for_mon->show();
      edit_for_day->show();
      edit_for_hour->show();
      edit_for_min->show();
      edit_for_sec->show();

      label_for_year_text->show();
      label_for_mon_text->show();
      label_for_day_text->show();
      label_for_hour_text->show();
      label_for_min_text->show();
      save_btn->show();
  }
  model->setData(model->index(1, 0),
          QVariant::fromValue(itemData),
          Qt::UserRole + 1);

  QRegion region;
  region = region.united(list_view->frameGeometry());
  this->repaint(region);
}
void ui_time_setting:: slot_for_auto_btn_click() {
  if (auto_enable) {
    auto_enable = 0;
    label_for_auto_btn->setStyleSheet(
        "#time_auto_btn {border-image:url(:icon/setting/turn_off.png);}");
    hide_manual_time(false);
  } else {
    label_for_auto_btn->setStyleSheet(
        "#time_auto_btn {border-image: url(:icon/setting/turn_on.png);}");
    auto_enable = 1;
    hide_manual_time(true);
  }
  ui_control_set_auto_time(auto_enable);
}
void ui_time_setting::slot_for_year_add() {
  label_for_year_add->setEnabled(false);
  int year = edit_for_year->text().toInt();
  year++;
  edit_for_year->setText(QString("%1").arg(year));
  label_for_year_add->setEnabled(true);
}
void ui_time_setting::slot_for_mon_add() {
  label_for_mon_add->setEnabled(false);

  int month = edit_for_mon->text().toInt();
  month++;
  if (month > 12) month = 1;
  edit_for_mon->setText(QString("%1").arg(month));

  label_for_mon_add->setEnabled(true);
}
void ui_time_setting::slot_for_day_add() {
  label_for_day_add->setEnabled(false);

  int year = edit_for_year->text().toInt();
  int month = edit_for_mon->text().toInt();
  int day = edit_for_day->text().toInt();
  int max_day = 0;
  if (month == 1 || month == 3 || month == 5 || month == 7
  || month == 8 || month == 10 || month == 12) {
    max_day = 31;
  } else if (month == 4 || month == 6
  || month == 9 || month == 11) {
    max_day = 30;
  } else if (month == 2) {
    if (year % 4 == 0) {
      max_day = 29;
    } else {
      max_day = 28;
    }
  }
  day++;
  if (day > max_day) day = 1;
  edit_for_day->setText(QString("%1").arg(day));
  label_for_day_add->setEnabled(true);
}
void ui_time_setting::slot_for_hour_add() {
  label_for_hour_add->setEnabled(false);

  int hour = edit_for_hour->text().toInt();
  hour++;
  if (hour > 23) hour = 0;
  edit_for_hour->setText(QString("%1").arg(hour));

  label_for_hour_add->setEnabled(true);
}

void ui_time_setting::slot_for_min_add() {
  label_for_min_add->setEnabled(false);

  int min = edit_for_min->text().toInt();
  min++;
  if (min > 59) min = 0;
  edit_for_min->setText(QString("%1").arg(min));

  label_for_min_add->setEnabled(true);
}
void ui_time_setting::slot_for_sec_add() {
  label_for_sec_add->setEnabled(false);

  int sec = edit_for_sec->text().toInt();
  sec++;
  if (sec > 59) sec = 0;
  edit_for_sec->setText(QString("%1").arg(sec));

  label_for_sec_add->setEnabled(true);
}

void ui_time_setting::slot_for_year_sub() {
  label_for_year_sub->setEnabled(false);

  int year = edit_for_year->text().toInt();
  year--;
  edit_for_year->setText(QString("%1").arg(year));

  label_for_year_sub->setEnabled(true);
}
void ui_time_setting::slot_for_mon_sub() {
  label_for_mon_sub->setEnabled(false);

  int month = edit_for_mon->text().toInt();
  month--;
  if (month == 0) month = 12;
  edit_for_mon->setText(QString("%1").arg(month));
  label_for_mon_sub->setEnabled(true);
}
void ui_time_setting::slot_for_day_sub() {
  label_for_day_sub->setEnabled(false);

  int year = edit_for_year->text().toInt();
  int month = edit_for_mon->text().toInt();
  int day = edit_for_day->text().toInt();
  int max_day = 0;
  if (month == 1 || month == 3 || month == 5 || month == 7
  || month == 8 || month == 10 || month == 12) {
    max_day = 31;
  } else if (month == 4 || month == 6 ||
  month == 9 || month == 11) {
    max_day = 30;
  } else if (month == 2) {
    if (year % 4 == 0) {
      max_day = 29;
    } else {
      max_day = 28;
    }
  }
  day--;
  if (day == 0) day = max_day;
  edit_for_day->setText(QString("%1").arg(day));

  label_for_day_sub->setEnabled(true);
}
void ui_time_setting::slot_for_hour_sub() {
  label_for_hour_sub->setEnabled(false);
  int hour = edit_for_hour->text().toInt();
  hour--;
  if (hour < 0) hour = 23;
  edit_for_hour->setText(QString("%1").arg(hour));
  label_for_hour_sub->setEnabled(true);
}
void ui_time_setting::slot_for_min_sub() {
  label_for_min_sub->setEnabled(false);

  int min = edit_for_min->text().toInt();
  min--;
  if (min < 0) min = 59;
  edit_for_min->setText(QString("%1").arg(min));

  label_for_min_sub->setEnabled(true);
}
void ui_time_setting::slot_for_sec_sub() {
  label_for_sec_sub->setEnabled(false);

  int sec = edit_for_sec->text().toInt();
  sec--;
  if (sec < 0) sec = 59;
  edit_for_sec->setText(QString("%1").arg(sec));

  label_for_sec_sub->setEnabled(true);
}

void ui_time_setting::slot_for_list_view_clicked(const QModelIndex& index) {
}
