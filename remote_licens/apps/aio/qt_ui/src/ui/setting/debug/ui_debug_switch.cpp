/*
* ui_debug_switch.hpp - define debug switch function of main
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
#endif /* __cplusplus */
#include "ui_control.h"
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QDebug>

#include "ui_debug_switch.hpp"
#include "ui_ui_debug_switch.h"
#include "ui_rotate_management.hpp"
#include "ui_debug_switch_delegate.hpp"
#include "ui_batch_recognition.hpp"

ui_debug_switch::ui_debug_switch(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_debug_switch),
    isp_btn(NULL),
    rec_btn(NULL),
    time_btn(NULL),
    pModel(NULL),
    pItemDelegate(NULL) {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);
  this->setWindowFlags(Qt::FramelessWindowHint);

  QStringList net_style;
  net_style << tr("显示ISP信息") << tr("录像按钮") << tr("显示时间") << tr("批量用户识别");
  // QStandardItemModel *pModel = new QStandardItemModel();
  pModel = new QStandardItemModel();
  for (int i = 0; i < net_style.size(); ++i) {
    QStandardItem *pItem = new QStandardItem;
    MuItemData_debug itemData;
    itemData.name = net_style.at(i);

    if (i < net_style.size() - 1) {
      QPushButton *chk_btn = new QPushButton("");
      chk_btn->setProperty("id", QString("%1").arg(i));
      // set custom property
      chk_btn->setProperty("name",
                           QString("my listview %1").arg(i));
      chk_btn->setProperty("status", "normal");
      connect(chk_btn, SIGNAL(clicked()), this,
              SLOT(slot_for_check_btn()));
      update_debug_icon_status(i, chk_btn);
      itemData.btn = chk_btn;
      chk_btn->setParent(ui->debug_listView);
    } else {
      itemData.btn = NULL;
    }
    pItem->setData(QVariant::fromValue(itemData), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }

  // ui_debug_switch_delegate *pItemDelegate =
  pItemDelegate = new ui_debug_switch_delegate(this);
  ui->debug_listView->setItemDelegate(pItemDelegate);
  ui->debug_listView->setModel(pModel);

  connect(ui->debug_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_debug_listview(QModelIndex)));

  connect(ui->debug_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));
}

ui_debug_switch::~ui_debug_switch() {
  delete ui;

  if (isp_btn) {
    delete isp_btn;
    isp_btn = NULL;
  }

  if (rec_btn) {
    delete rec_btn;
    rec_btn = NULL;
  }

  if (time_btn) {
    delete time_btn;
    time_btn = NULL;
  }

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }
}

void ui_debug_switch::update_debug_icon_status(int index, QPushButton *btn) {
  un_debug_switch_t debug_sw;

  if (NULL == btn) {
    return;
  }

  memset(&debug_sw, 0, sizeof(un_debug_switch_t));
  ui_control_get_debug_switch(&debug_sw);
  switch (index) {
    case 0:
      if (debug_sw.debug_switch.debug_for_isp) {
        btn->setStyleSheet(
          "QPushButton{border-image:url(:/icon/turn_on.png)}");
      } else {
        btn->setStyleSheet(
          "QPushButton{border-image:url(:/icon/turn_off.png)}");
      }
      isp_btn = btn;
      break;

    case 1:
      if (debug_sw.debug_switch.debug_for_record) {
        btn->setStyleSheet(
          "QPushButton{border-image:url(:/icon/turn_on.png)}");
      } else {
        btn->setStyleSheet(
          "QPushButton{border-image:url(:/icon/turn_off.png)}");
      }
      rec_btn = btn;
      break;

    case 2:
      if (debug_sw.debug_switch.debug_for_time) {
        btn->setStyleSheet(
          "QPushButton{border-image:url(:/icon/turn_on.png)}");
      } else {
        btn->setStyleSheet
          ("QPushButton{border-image:url(:/icon/turn_off.png);}");
      }
      time_btn = btn;
      break;
  }
}

void ui_debug_switch::deinit() {
  QtMainWindow::deinit();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->set_debug_switch();
}

void ui_debug_switch::slot_for_debug_listview(const QModelIndex &index) {
  // qDebug() << "index " << index.row();
  if (index.row() == 3) {
    ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();
    rotate_signgleton->start_activity_win(
      &ui_batch_recognition::staticMetaObject);
  }
}

void ui_debug_switch::slot_for_check_btn() {
  QPushButton *chk_btn = reinterpret_cast <QPushButton *>(sender());
  QString id = chk_btn->property("id").toString();
  qDebug() << "slot_for_check_btn() id " << id;

  un_debug_switch_t debug_sw;
  memset(&debug_sw, 0, sizeof(un_debug_switch_t));
  ui_control_get_debug_switch(&debug_sw);
  if (0 == id.compare("0")) {
    debug_sw.debug_switch.debug_for_isp =
      ~debug_sw.debug_switch.debug_for_isp;
    ui_control_set_debug_switch(&debug_sw);
    update_debug_icon_status(0, isp_btn);
  }

  if (0 == id.compare("1")) {
    debug_sw.debug_switch.debug_for_record =
      ~debug_sw.debug_switch.debug_for_record;
    ui_control_set_debug_switch(&debug_sw);
    update_debug_icon_status(1, rec_btn);
  }

  if (0 == id.compare("2")) {
    debug_sw.debug_switch.debug_for_time =
      ~debug_sw.debug_switch.debug_for_time;
    ui_control_set_debug_switch(&debug_sw);
    update_debug_icon_status(2, time_btn);
  }
}

