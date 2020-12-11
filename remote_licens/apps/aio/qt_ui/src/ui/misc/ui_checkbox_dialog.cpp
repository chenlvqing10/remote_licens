/*
* ui_checkbox_dialog.cpp - define radio dialog interface function of UI
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

#include "ui_checkbox_dialog.hpp"
#include "ui_ui_radio_dialog.h"
#include "ui_rotate_management.hpp"

#define CON_CHOICE_ICON ":icon/con-choice.png"
#define CON_UNCHOICE_ICON ":icon/con-unchoice.png"

#define CHECKBOX_ROW_HEIGHT 92
#define CHECKBOX_MARGIN_L_R 42
#define CHECKBOX_MARGIN_TOP 15
#define CHECKBOX_ICON_SIZE 62

ui_checkbox_dialog::ui_checkbox_dialog(checkbox_args_t *args, QWidget *parent) :
    QtDialog(parent),
    ui(new Ui::ui_radio_dialog) {
  checkbox_info = new checkbox_args_t;
  checkbox_info->id = args->id;
  checkbox_info->title = args->title;
  checkbox_info->checkbox_list = args->checkbox_list;
  checkbox_info->default_indexs = args->default_indexs;
  checkbox_info->cb = args->cb;

  this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

  ui->radio_widget->setMinimumHeight(
      CHECKBOX_ROW_HEIGHT * checkbox_info->checkbox_list.size());

#if 0
  setAutoFillBackground(false);
  setAttribute(Qt::WA_TranslucentBackground, true);
#endif

  checkbox_item_t item_tmp;
  pModel = new QStandardItemModel();
  int count = checkbox_info->checkbox_list.size();
  for (int i = 0; i < count; ++i) {
    QStandardItem *pItem = new QStandardItem;
    item_tmp.item_title = checkbox_info->checkbox_list[i];
    if (checkbox_info->default_indexs.contains(i))
      item_tmp.is_checked = 1;
    else
      item_tmp.is_checked = 0;
    menu_checkbox << item_tmp;
    pItem->setData(QVariant::fromValue(menu_checkbox[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_checkbox_dialog_item_delegate(this);
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
  ui->tip_title_label->setText(checkbox_info->title);

  showMaxLayout();
}

ui_checkbox_dialog::~ui_checkbox_dialog() {
  delete pItemDelegate;
  delete pModel;
  delete checkbox_info;
  delete ui;
}

void ui_checkbox_dialog::slot_for_item_clicked(const QModelIndex & index) {
  int row =  index.row();

  menu_checkbox[row].is_checked = !menu_checkbox[row].is_checked;
  pModel->setData(pModel->index(row, 0),
          QVariant::fromValue(menu_checkbox[row]), Qt::UserRole + 1);

  ui->radio_listView->setModel(pModel);
  ui->radio_listView->update();
}

void ui_checkbox_dialog::slot_for_confirm() {
  if (!checkbox_info->cb)
    return;

  QVector<int> vec;
  for (int i = 0; i < checkbox_info->checkbox_list.count(); ++i) {
    if (menu_checkbox[i].is_checked)
      vec << i;
  }
  checkbox_info->cb->checkbox_callback(checkbox_info->id, vec);

  deinit();
}

void ui_checkbox_dialog::slot_for_cancel() {
  if (checkbox_info->cb)
    checkbox_info->cb->checkbox_callback(
      checkbox_info->id, checkbox_info->default_indexs);

  deinit();
}

void ui_checkbox_dialog::deinit() {
  this->deleteLater();
}

void ui_checkbox_dialog::ui_checkbox_dialog_item_delegate::paint(
    QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  int len = 400;

  if (index.isValid()) {
    QVariant var = index.data(Qt::UserRole + 1);
    checkbox_item_t itemData = var.value<checkbox_item_t>();
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

    QRectF nameRect = QRect(rect.left() + CHECKBOX_MARGIN_L_R,
        rect.top() + CHECKBOX_MARGIN_TOP, 400, CHECKBOX_ICON_SIZE);
    QRectF iconRect = QRect(rect.right() - CHECKBOX_MARGIN_L_R -
        CHECKBOX_ICON_SIZE, rect.top() + CHECKBOX_MARGIN_TOP,
        CHECKBOX_ICON_SIZE, CHECKBOX_ICON_SIZE);

    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("SourceHanSansCN-Normal", 38));
    painter->drawText(nameRect, itemData.item_title);
    if (itemData.is_checked)
      painter->drawImage(iconRect, QImage(CON_CHOICE_ICON));
    else
      painter->drawImage(iconRect, QImage(CON_UNCHOICE_ICON));

    painter->setPen(QPen(QColor("#102050")));
    if (0 == option.rect.y()) {
      painter->drawRect(rect.left() + (CHECKBOX_MARGIN_L_R - 2),
          rect.top() + 1, rect.width() - 2 * (CHECKBOX_MARGIN_L_R - 2), 1);
      painter->drawRect(rect.left() + (CHECKBOX_MARGIN_L_R - 2),
          rect.top() + 2, rect.width() - 2 * (CHECKBOX_MARGIN_L_R - 2), 1);
    }
    painter->drawRect(rect.left() + (CHECKBOX_MARGIN_L_R - 2),
        rect.bottom() - 1, rect.width() - 2 * (CHECKBOX_MARGIN_L_R - 2), 1);
    painter->drawRect(rect.left() + (CHECKBOX_MARGIN_L_R - 2),
        rect.bottom() - 2, rect.width() - 2 * (CHECKBOX_MARGIN_L_R - 2), 1);

    painter->setPen(QPen(Qt::black));
    painter->restore();
  }
}

QSize ui_checkbox_dialog::ui_checkbox_dialog_item_delegate::sizeHint(
const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  Q_UNUSED(index)

  return QSize(option.rect.width(), CHECKBOX_ROW_HEIGHT);
}

