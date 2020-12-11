/*
* ui_checkbox_dialog.cpp - define raido dialog interface function of UI
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

#ifndef UI_CHECKBOX_DIALOG_H
#define UI_CHECKBOX_DIALOG_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QMetaType>
#include "QtDialog.hpp"

typedef struct {
  QString item_title;
  int is_checked;
} checkbox_item_t;

Q_DECLARE_METATYPE(checkbox_item_t)

namespace Ui {
class ui_radio_dialog;
}

class ui_checkbox_callback {
 public:
  virtual void checkbox_callback(void *id, QVector<int> indexs) = 0;
};

typedef struct checkbox_args {
  void *id;
  QString title;
  QStringList checkbox_list;
  QVector<int> default_indexs;
  ui_checkbox_callback *cb;
} checkbox_args_t;

class ui_checkbox_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_checkbox_dialog(checkbox_args_t *args, QWidget* parent = 0);
  ~ui_checkbox_dialog();
  void deinit();

 private slots:
  void slot_for_confirm();
  void slot_for_cancel();
  void slot_for_item_clicked(const QModelIndex & index);

 private:
  class ui_checkbox_dialog_item_delegate : public QStyledItemDelegate {
   public:
    explicit ui_checkbox_dialog_item_delegate(QObject *parent = 0):
        QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
  };

 private:
  Ui::ui_radio_dialog* ui;
  checkbox_args_t *checkbox_info;
  QList<checkbox_item_t> menu_checkbox;
  QStandardItemModel *pModel;
  ui_checkbox_dialog_item_delegate *pItemDelegate;
};

#endif  //  UI_CHECKBOX_DIALOG_H

