/*
* ui_attendance_list.hpp define attendance list function of UI
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

#ifndef UI_ATTENDANCE_LIST_H
#define UI_ATTENDANCE_LIST_H

#include <QStandardItemModel>
#include <map>
#include <QSignalMapper>
#include <QList>

#include "QtMainWindow.hpp"
#include "db_all.h"
#include "ui_setting_item_delegate.hpp"
#include "ui_attendance_data.hpp"

namespace Ui {
class ui_attendance_list;
}

typedef enum {
    EN_GO_FIRST,  /*0*/
    EN_GO_PREV, /*1*/
    EN_GO_NEXT, /*2*/
    EN_GO_LAST, /*3*/
    EN_GO_MAX
} EN_GO_TYPE;

typedef struct {
  QString image;
  QString no;
  QString name;
  QString time;
} attendance_item_t;

Q_DECLARE_METATYPE(attendance_item_t)

class ui_attendance_list : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_attendance_list(QWidget *parent = 0);
  ~ui_attendance_list();
  void get_attendance_record(char *user_id,
    long start, long end);
  void get_attendance_list(char *user_id,
    long start, long end);
  void update_page(EN_GO_TYPE type);
  void attendance_callback(void *id, QString time);

 private slots:
  void slot_for_mainui(int event, int value);
  void slot_for_first();
  void slot_for_prev();
  void slot_for_next();
  void slot_for_last();
  void slot_for_search();
  void slot_for_output();

 private:
  class ui_list_item_delegate : public QStyledItemDelegate {
   public:
    explicit ui_list_item_delegate(QObject *parent = 0):
        QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
  };

 private:
  Ui::ui_attendance_list *ui;
  QStandardItemModel *pModel;
  ui_list_item_delegate *pItemDelegate;
  QList<attendance_item_t> record_list;
  int page = 0, pages = 0, count = 0;
  int count_height;
  char user_id[64];
  long start_date, end_date;
};

#endif  //  UI_ATTENDANCE_LIST_H