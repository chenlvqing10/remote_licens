/*
* ui_radio_dialog.cpp - define raido dialog interface function of UI
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

#ifndef UI_RADIO_DIALOG_H
#define UI_RADIO_DIALOG_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QMetaType>
#include <QListView>
#include "QtDialog.hpp"

typedef struct {
  QString item_title;
  int is_checked;
} radio_item_t;

Q_DECLARE_METATYPE(radio_item_t)

namespace Ui {
class ui_radio_dialog;
}

class ui_radio_callback {
 public:
  virtual void radio_callback(void *id, int index,
    bool isCancel = false) = 0;
};

typedef struct radio_args {
  void *id;
  QString title;
  QStringList radio_list;
  int default_index;
  ui_radio_callback *cb;
} radio_args_t;

class ui_radio_dialog : public QtDialog {
  Q_OBJECT

 public:
  explicit ui_radio_dialog(radio_args_t *args, QWidget* parent = 0);
  ~ui_radio_dialog();
  void set_dialog(QString title,
    QStringList radio_list, int default_index);
  void deinit();

 private slots:
  void slot_for_confirm();
  void slot_for_cancel();
  void group_item_clicked(int index);
  void slot_for_item_clicked(const QModelIndex & index);

 private:
  class ui_radio_dialog_item_delegate : public QStyledItemDelegate {
   public:
    explicit ui_radio_dialog_item_delegate(QObject *parent = 0):
        QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
  };

 private:
  Ui::ui_radio_dialog* ui;
  // QButtonGroup *m_group;
  radio_args_t *radio_info;
  QList<radio_item_t> menu_radio;
  QStandardItemModel *pModel;
  ui_radio_dialog_item_delegate *pItemDelegate;
};

#endif  //  UI_RADIO_DIALOG_H

