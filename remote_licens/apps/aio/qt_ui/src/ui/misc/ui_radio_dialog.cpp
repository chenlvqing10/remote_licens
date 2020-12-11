/*
* ui_radio_dialog.cpp - define radio dialog interface function of UI
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

#include <QObject>
#include <QDebug>
#include <QModelIndex>

#include "ui_radio_dialog.hpp"
#include "ui_ui_radio_dialog.h"
#include "ui_rotate_management.hpp"

#define CHOICE_ICON ":icon/setting/choice.png"

#define RADIO_ROW_HEIGHT 92
#define RADIO_MARGIN_L_R 42
#define RADIO_MARGIN_TOP 18
#define RADIO_ICON_SIZE 56

ui_radio_dialog::ui_radio_dialog(radio_args_t *args, QWidget *parent) :
    QtDialog(parent),
    ui(new Ui::ui_radio_dialog) {
  radio_info = new radio_args_t;
  radio_info->id = args->id;
  radio_info->title = args->title;
  radio_info->radio_list = args->radio_list;
  radio_info->default_index = args->default_index;
  radio_info->cb = args->cb;

  this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

  ui->radio_widget->setMinimumHeight(
      RADIO_ROW_HEIGHT * radio_info->radio_list.size());

#if 0
  setAutoFillBackground(false);
  setAttribute(Qt::WA_TranslucentBackground, true);
#endif

  radio_item_t item_tmp;
  pModel = new QStandardItemModel();
  int count = radio_info->radio_list.size();
  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
    item_tmp.item_title = radio_info->radio_list[i];
    if (i == radio_info->default_index)
      item_tmp.is_checked = 1;
    else
      item_tmp.is_checked = 0;
    menu_radio << item_tmp;
    pItem->setData(QVariant::fromValue(menu_radio[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_radio_dialog_item_delegate(this);
  ui->radio_listView->setItemDelegate(pItemDelegate);
  ui->radio_listView->setModel(pModel);

  connect(ui->radio_listView, SIGNAL(clicked(QModelIndex)),
      this, SLOT(slot_for_item_clicked(QModelIndex)));

  connect(ui->radio_confirm_btn, SIGNAL(clicked()),
      this, SLOT(slot_for_confirm()));
  connect(ui->radio_cancel_btn, SIGNAL(clicked()),
      this, SLOT(slot_for_cancel()));

  ui->radio_cancel_btn->setStyleSheet(
      "QPushButton{border-image:url(:/icon/tip_confirm.png)}");

  ui->tip_title_label->setAlignment(Qt::AlignCenter);
  ui->tip_title_label->setText(radio_info->title);

  /*
  m_group = new QButtonGroup(this);
  m_group->setExclusive(true);
  for (int i = 0; i < radio_info->radio_list.size(); ++i) {
    p_bt = new QRadioButton(this);
    p_bt->setText(radio_info->radio_list.at(i));
    p_bt->setChecked(radio_info->default_index == i);
    ui->radioLayout->addWidget(p_bt);
    m_group->addButton(p_bt, i);
  }
  connect(m_group, SIGNAL(buttonClicked(int)),
    this, SLOT(group_item_clicked(int)));
  */
  showMaxLayout();
}

ui_radio_dialog::~ui_radio_dialog() {
  delete pItemDelegate;
  delete pModel;
  delete radio_info;
  delete ui;
}

void ui_radio_dialog::group_item_clicked(int index) {
  ALOGD("%s[%d] oncliked index = %d", __func__, __LINE__, index);
}

void ui_radio_dialog::slot_for_item_clicked(const QModelIndex & index) {
  int row =  index.row();

  for (int i = 0; i < radio_info->radio_list.count(); ++i) {
    if (row == i && menu_radio[i].is_checked)
      return;
    if (row == i) {
      menu_radio[i].is_checked = 1;
      pModel->setData(pModel->index(i, 0),
          QVariant::fromValue(menu_radio[i]), Qt::UserRole + 1);
    }
    if (row != i && menu_radio[i].is_checked) {
      menu_radio[i].is_checked = 0;
      pModel->setData(pModel->index(i, 0),
          QVariant::fromValue(menu_radio[i]), Qt::UserRole + 1);
    }
  }

  ui->radio_listView->setModel(pModel);
  ui->radio_listView->update();
}

void ui_radio_dialog::slot_for_confirm() {
  if (!radio_info->cb)
    return;

  for (int i = 0; i < radio_info->radio_list.count(); ++i) {
    if (menu_radio[i].is_checked)
      radio_info->cb->radio_callback(radio_info->id, i);
  }

  deinit();
}

void ui_radio_dialog::slot_for_cancel() {
  if (radio_info->cb)
    radio_info->cb->radio_callback(
      radio_info->id, radio_info->default_index, true);
  deinit();
}

void ui_radio_dialog::deinit() {
  this->deleteLater();
}

void ui_radio_dialog::ui_radio_dialog_item_delegate::paint(
    QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  int len = 400;

  if (index.isValid()) {
    QVariant var = index.data(Qt::UserRole + 1);
    radio_item_t itemData = var.value<radio_item_t>();
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

    QRectF nameRect = QRect(rect.left() + RADIO_MARGIN_L_R, rect.top() +
        RADIO_MARGIN_TOP, 400, RADIO_ICON_SIZE);
    QRectF iconRect = QRect(rect.right() - RADIO_MARGIN_L_R - RADIO_ICON_SIZE,
        rect.top() + RADIO_MARGIN_TOP, RADIO_ICON_SIZE, RADIO_ICON_SIZE);

    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("SourceHanSansCN-Normal", 38));
    painter->drawText(nameRect, itemData.item_title);
    if (itemData.is_checked)
      painter->drawImage(iconRect, QImage(CHOICE_ICON));

    painter->setPen(QPen(QColor("#102050")));
    if (0 == option.rect.y()) {
      painter->drawRect(rect.left() + (RADIO_MARGIN_L_R - 2),
          rect.top() + 1, rect.width() - 2 * (RADIO_MARGIN_L_R - 2), 1);
      painter->drawRect(rect.left() + (RADIO_MARGIN_L_R - 2),
          rect.top() + 2, rect.width() - 2 * (RADIO_MARGIN_L_R - 2), 1);
    }
    painter->drawRect(rect.left() + (RADIO_MARGIN_L_R - 2),
        rect.bottom() - 1, rect.width() - 2 * (RADIO_MARGIN_L_R - 2), 1);
    painter->drawRect(rect.left() + (RADIO_MARGIN_L_R - 2),
        rect.bottom() - 2, rect.width() - 2 * (RADIO_MARGIN_L_R - 2), 1);

    painter->setPen(QPen(Qt::black));
    painter->restore();
  }
}

QSize ui_radio_dialog::ui_radio_dialog_item_delegate::sizeHint(
const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  Q_UNUSED(index)

  return QSize(option.rect.width(), RADIO_ROW_HEIGHT);
}

