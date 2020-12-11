/*
* ui_attendance_list.cpp - define interface function of attendance list
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
#include "face.h"
#include "time_conv.h"
#include "ws_control.h"
#include "screen_size.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>
#include <QModelIndex>

#include "ui_rotate_management.hpp"
#include "ui_attendance_list.hpp"
#include "ui_ui_attendance_list.h"
#include "ui_attendance_setting.hpp"
#include "ui_attendance_search.hpp"

#ifndef PORTRAIT_SCREEN
  #define ICON_BACK_AMAGE 72
  #define TOP_BUTTOM 151
  #define ROW_PITCH  50
  #define ROW_WIDTH  98
#else
  #define ICON_BACK_AMAGE 42
  #define TOP_BUTTOM 192
  #define ROW_PITCH  42
  #define ROW_WIDTH  92
#endif

ui_attendance_list::ui_attendance_list(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_attendance_list),
  pModel(NULL),
  pItemDelegate(NULL) {
  attendance_item_t item;

  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);
#if ATTENDANCE_MODE == 0
  ui->attendance_title_label->setText(tr("查看通行记录"));
#else
  ui->attendance_title_label->setText(tr("查看考勤记录"));
#endif
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  pModel = new QStandardItemModel();
  count_height = (get_screen_height() - TOP_BUTTOM) / ROW_WIDTH;
  for (int i = 0; i < count_height; ++i) {
    QStandardItem *pItem = new QStandardItem;
    pItem->setData(QVariant::fromValue(item), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_list_item_delegate(this);
  ui->attendance_listView->setItemDelegate(pItemDelegate);
  ui->attendance_listView->setModel(pModel);

  ui->rec_tip_label->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

  connect(ui->attendance_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  connect(ui->first_btn, SIGNAL(clicked()),  this, SLOT(slot_for_first()));
  connect(ui->prev_btn, SIGNAL(clicked()), this, SLOT(slot_for_prev()));
  connect(ui->next_btn, SIGNAL(clicked()), this, SLOT(slot_for_next()));
  connect(ui->last_btn, SIGNAL(clicked()), this, SLOT(slot_for_last()));
  connect(ui->search_btn, SIGNAL(clicked()), this, SLOT(slot_for_search()));
  connect(ui->output_btn, SIGNAL(clicked()), this, SLOT(slot_for_output()));

  memset(user_id, 0, sizeof(user_id));
  memcpy(user_id, rotate_signgleton->attendance_num.toUtf8().data(),
    sizeof(user_id));
  start_date = rotate_signgleton->attendance_start;
  end_date = rotate_signgleton->attendance_end;

#if 0
  get_attendance_record(user_id, start_date, end_date);
#endif

  count = ws_control_get_checkin_count_by_id_time(user_id,
    start_date, end_date);
  pages = ceil((double)count/count_height);
  update_page(EN_GO_FIRST);

  ui->attendance_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_attendance_list::~ui_attendance_list() {
  ALOGD("%s delete", __func__);

  record_list.clear();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  if (-1 != rotate_signgleton->attendance_start) {
    rotate_signgleton->attendance_num = QString("");
    rotate_signgleton->attendance_start = -1;
    rotate_signgleton->attendance_end = 0;
  }

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }

  delete ui;
}

void ui_attendance_list::slot_for_mainui(int event, int value) {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
#if 0
  get_attendance_record(
    rotate_signgleton->attendance_num.toUtf8().data(),
    rotate_signgleton->attendance_start,
    rotate_signgleton->attendance_end);
#endif
  memset(user_id, 0, sizeof(user_id));
  memcpy(user_id, rotate_signgleton->attendance_num.toUtf8().data(),
    sizeof(user_id));
  start_date = rotate_signgleton->attendance_start;
  end_date = rotate_signgleton->attendance_end;
  count = ws_control_get_checkin_count_by_id_time(user_id,
    start_date, end_date);
  pages = ceil((double)count/count_height);
  update_page(EN_GO_FIRST);
}

void ui_attendance_list::get_attendance_record(char *user_id,
    long start, long end) {
    int ret = 0;
    ee_queue_t _head;
    ee_queue_t *h_checkin = &_head;
    web_socket_checkin_info_t *p = NULL;
    attendance_item_t tmp;
    char buf[64];
    record_list.clear();
    ret = ws_control_get_checkin_data_by_id_time(
        h_checkin, user_id, start, end);
    if (ret == 0) {
        ALOGI("%s:no checkin data\n", __func__);
        h_checkin = NULL;
        return;
    }

    ee_queue_foreach(p, h_checkin,
                    web_socket_checkin_info_t, queue) {
        tmp.image = QString(p->face_path);
        tmp.no = QString(p->user_id);
        tmp.name = QString(p->name);
        memset(buf, 0, sizeof(buf));
        time_min_to_str(p->timestamp, buf, 64);
        tmp.time = QString(buf);
        record_list.append(tmp);
    }

    ws_control_put_checkin_data(h_checkin);
    h_checkin = NULL;
    if (ret < 0) {
        record_list.clear();
    }
    count = record_list.size();
    pages = ceil((double)count/count_height);
#if 0
    for (int i = 0; i < record_list.size(); ++i) {
        tmp = record_list.at(i);
        ALOGE("%s[%d] >> %s, %s, %s", __func__, i,
            tmp.icon, tmp.title, tmp.value);
    }
#endif
}

void ui_attendance_list::get_attendance_list(char *user_id,
  long start, long end) {
  int ret = 0;
  ee_queue_t _head;
  ee_queue_t *h_checkin = &_head;
  web_socket_checkin_info_t *p = NULL;
  attendance_item_t tmp;
  char buf[64];
  ALOGD("%s:%d >> start get limit list.", __func__, __LINE__);
  record_list.clear();
  ret = ws_control_get_checkin_by_id_time_limit(h_checkin,
    user_id, start, end, count_height * page, count_height);
  ee_queue_foreach(p, h_checkin,
    web_socket_checkin_info_t, queue) {
    if (strlen(p->face_path))
      tmp.image = QString(p->face_path);
    else
      tmp.image = QString(p->frame_path);
    tmp.no = QString(p->user_id);
    if (0 == strcmp(p->user_id, "-1")) {
      tmp.name = QString(tr("陌生人"));
    } else {
      tmp.name = QString(p->name);
    }
    memset(buf, 0, sizeof(buf));
    time_min_to_str(p->timestamp, buf, 64);
    tmp.time = QString(buf);
    record_list.append(tmp);
  }
  ws_control_put_checkin_data(h_checkin);

  h_checkin = NULL;
  if (ret <= 0) {
    record_list.clear();
  }
  ALOGD("%s:%d >> end load db.", __func__, __LINE__);
#if 0
  for (int i = 0; i < user_list.size(); ++i) {
    tmp = record_list.at(i);
    ALOGE("%s[%d] >> %s, %s, %s", __func__, i,
      tmp->icon, tmp->title, tmp->value);
  }
#endif
}


void ui_attendance_list::update_page(EN_GO_TYPE type) {
  item_t item;
  item.type = EN_ITEM_IMAGE;
  if (0 == count) {
    QString str = QString(tr("第%1/%2页"))
      .arg(0).arg(0);
    for (int i = 0; i < count_height; i++) {
      pModel->setData(pModel->index(i, 0),
        QVariant::fromValue(item), Qt::UserRole + 1);
    }
    ui->rec_tip_label->setText(str);
    return;
  }
  switch (type) {
    case EN_GO_FIRST:
      page = 0;
      break;
    case EN_GO_PREV:
      if (0 < page)
          page -= 1;
      else
          return;
      break;
    case EN_GO_NEXT:
      if (page < pages - 1)
          page += 1;
      else
          return;
      break;
    case EN_GO_LAST:
      page = pages - 1;
      break;
  }
  get_attendance_list(user_id, start_date, end_date);
  QString str = QString(tr("第%1/%2页"))
    .arg(page+1).arg(pages);
  ui->rec_tip_label->setText(str);
  for (int i = 0; i < count_height; i++) {
    if (i + count_height * page < count)
      pModel->setData(pModel->index(i, 0),
        QVariant::fromValue(record_list.at(i)), Qt::UserRole + 1);
    else
      pModel->setData(pModel->index(i, 0),
        QVariant::fromValue(item), Qt::UserRole + 1);
  }
  ui->attendance_listView->update();
}

void ui_attendance_list::slot_for_first() {
  update_page(EN_GO_FIRST);
}

void ui_attendance_list::slot_for_prev() {
  update_page(EN_GO_PREV);
}

void ui_attendance_list::slot_for_next() {
  update_page(EN_GO_NEXT);
}

void ui_attendance_list::slot_for_last() {
  update_page(EN_GO_LAST);
}

void ui_attendance_list::slot_for_search() {
#ifndef PORTRAIT_SCREEN
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->start_activity_win(
    &ui_attendance_data::staticMetaObject);
#else
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->start_activity_win(
    &ui_attendance_search::staticMetaObject);
#endif
}

void ui_attendance_list::slot_for_output() {
#if ATTENDANCE_MODE != 2
  ui_tip_dialog *tip = new ui_tip_dialog(TIP_WIN_TYPE_EXPORT_RECORD, this);
  tip->show();
#endif
  send_signal_for_websocket(WEB_SOCKET_EVENT_RECORD_EXPORT);
}

void ui_attendance_list::attendance_callback(void *id, QString time) {
  sendSignalForMainUI(0, 0);
}

void ui_attendance_list::ui_list_item_delegate::paint(
    QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  int len = 400;

  if (index.isValid()) {
    QVariant var = index.data(Qt::UserRole + 1);
    attendance_item_t itemData = var.value<attendance_item_t>();
    painter->save();

    QRectF rect;
    rect.setX(option.rect.x());
    rect.setY(option.rect.y());
    rect.setWidth(option.rect.width() - 1);
    rect.setHeight(option.rect.height() - 1);

    QPainterPath path;
    path.quadTo(rect.topLeft(), rect.bottomLeft());
    path.quadTo(rect.bottomLeft(), rect.bottomRight());
    path.quadTo(rect.bottomRight(), rect.topRight());
    path.quadTo(rect.topRight(), rect.topLeft());

    QRectF imageRect = QRect(rect.left() + ICON_BACK_AMAGE,
      rect.top() + 17, 76, 76);
    QRectF numRect = QRect(rect.left() + ICON_BACK_AMAGE + 88,
      rect.top() + 17, 106, 64);
    QRectF nameRect = QRect(rect.left() + ICON_BACK_AMAGE + 194,
      rect.top() + 17, 172, 64);
    QRectF timeRect = QRect(rect.right() - ICON_BACK_AMAGE - 340,
      rect.top() + 17, 340, 64);

    painter->drawImage(imageRect, QImage(itemData.image));
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("SourceHanSansCN-Normal", 38));
    painter->drawText(numRect, itemData.no);
    painter->drawText(nameRect, itemData.name);
    painter->drawText(timeRect, itemData.time);

    painter->setPen(QPen(QColor("#102050")));
    if (0 == option.rect.y()) {
      painter->drawRect(
      rect.left() + ROW_PITCH, rect.top() + 1, rect.width() -
        ROW_PITCH * 2, 1);
      painter->drawRect(
      rect.left() + ROW_PITCH, rect.top() + 2, rect.width() -
        ROW_PITCH * 2, 1);
    }
    painter->drawRect(
      rect.left() + ROW_PITCH, rect.bottom() - 1, rect.width() -
        ROW_PITCH * 2, 1);
    painter->drawRect(
      rect.left() + ROW_PITCH, rect.bottom() - 2, rect.width() -
        ROW_PITCH * 2, 1);
      painter->setPen(QPen(Qt::black));
      painter->restore();
  }
}

QSize ui_attendance_list::ui_list_item_delegate::sizeHint(
const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  Q_UNUSED(index)

  return QSize(option.rect.width(), ROW_WIDTH);
}

