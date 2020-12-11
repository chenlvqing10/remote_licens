/*
* ui_eth_setting_delegate.h -
* define ethernet setting delegate function of UI
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

#ifndef UI_ETH_SETTING_DELEGATE_H
#define UI_ETH_SETTING_DELEGATE_H

#include <QTableWidget>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QMetaType>
#include <QPushButton>
#include <QLineEdit>

#define CTRL_TYPE_BUTTON 0
#define CTRL_TYPE_LINEEDIT 1

typedef struct {
  QString name;
  QWidget *widget;
  int width;
} MuItemData_eth;

Q_DECLARE_METATYPE(MuItemData_eth)

class ui_eth_setting_delegate : public QStyledItemDelegate {
 public:
  void set_draw_bottom_line_status(int status);

 public:
  explicit ui_eth_setting_delegate(QObject *parent = nullptr);
  void paint(QPainter *painter,
             const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;

 private:
  int is_draw_bottom_line;
};

#endif  //  UI_ETH_SETTING_DELEGATE_H
