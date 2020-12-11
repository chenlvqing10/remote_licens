/*
* ui_wifi_list.hpp - define wifi list function of UI
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

#ifndef UI_WIFI_LIST_H
#define UI_WIFI_LIST_H

#include <QStandardItemModel>
#include <QSignalMapper>
#include <QList>


#include <QTableWidget>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QMetaType>
#include <QPushButton>
#include <QMap>

#include "QtMainWindow.hpp"
#include "wifi_management.hpp"
#include "ui_edit_dialog.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_radio_dialog.hpp"

namespace Ui {
class ui_wifi_list;
}

Q_DECLARE_METATYPE(wifi_dot)

class ui_wifi_list : public QtMainWindow,
  ui_edit_callback, ui_tip_callback, ui_radio_callback{
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_wifi_list(QWidget *parent = 0);
  ~ui_wifi_list();
  void handle_click_event(bool time_out);
  void display_current_connect();
  void edit_callback(edit_args_t *args);
  int tip_callback(tip_args_t *args, bool isCancel = false);
  void radio_callback(void *id, int index, bool isCancel = false);
  void test_wifi();
  void debug_test_wifi();
  QList<wifi_dot> get_wifi_list() {
    return wifi_list;
  }
  void uinit();

 public slots:
  void deinit();

 protected slots:
  void slot_for_mainui(int event, int value);

 private:
  void update_wifi_list_imp(bool ishow);

 private slots:
  void slot_for_click_item (QModelIndex index);
  void slot_for_long_click_item (QModelIndex index);
  void update_wifi_list();
  void update_wifi_state(int state);

 private:
  class ui_list_item_delegate : public QStyledItemDelegate {
   public:
    explicit ui_list_item_delegate(QObject *parent = 0);
    ~ui_list_item_delegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
      const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
      const QModelIndex &index) const override;

    void paint_wifi_switch(QPainter *painter,
      QRectF rect, const wifi_dot &it) const;

    void paint_wifi_item(QPainter *painter, QRectF rect,
      const wifi_dot &it) const;

    private:
      wifi_management *mgr;
      wifi_dot *cur_cn;
  };

 private:
  Ui::ui_wifi_list *ui;
  QStandardItemModel *pModel;
  ui_list_item_delegate *pItemDelegate;
  QList<wifi_dot> wifi_list;
  wifi_management *mgr;
  QString ssid;
  QMap<int, QString> treatment_type;
};

#endif  //  UI_WIFI_LIST_H
