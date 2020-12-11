/*
* ui_user_view.cpp - define interface function of UI
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

#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QStringList>
#include <QString>
#include <QPushButton>
#include <QDebug>

#include "ui_user_view.hpp"
#include "ui_ui_user_view.h"
#include "ui_user_view_item_delegate.hpp"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"

#define TABLE_VIEW_WIDTH 700
#define TABLE_VIEW_HEIGHT 480

#define TABLE_ROW_HEIGHT 81  // 68

#define DEL_BUTTON_WIDTH 64
#define DEL_BUTTON_HEIGHT 63

#define TABLE_COL_WIDTH 300
#define MIN_TABLE_COL_WIDTH 160

#define TIMER_INTERVAL_COUNT 10

#define ONE_PAGE_REC_NUM 6  // 7

ui_user_view::ui_user_view(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_user_view),
    rec_count_(0),
    index_(0),
    tableModel(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);

  memset(user_id, 0, sizeof(user_id));
#if 0
  ui->user_tableView->setMinimumSize(TABLE_VIEW_WIDTH, TABLE_VIEW_HEIGHT);
  ui->user_tableView->verticalHeader()->hide();
#endif
  ui->user_tableView->setItemDelegate(
        new ui_user_view_item_delegate(ui->user_tableView));
#if 0
  ui->extract_btn->hide();
  ui->clean_btn->hide();
#endif
  connect(ui->user_view_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  connect(ui->first_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_first()));
  connect(ui->prev_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_prev()));
  connect(ui->next_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_next()));
  connect(ui->last_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_last()));
#if 0
  ui->first_btn->setStyleSheet(
        "QPushButton{border-image:url(:/icon/first.png);max-width:72px;}");
  ui->prev_btn->setStyleSheet(
        "QPushButton{border-image:url(:/icon/prev.png);max-width:72px;}");
  ui->next_btn->setStyleSheet(
        "QPushButton{border-image:url(:/icon/next.png);max-width:72px;}");
  ui->last_btn->setStyleSheet(
        "QPushButton{border-image:url(:/icon/last.png);max-width:72px;}");
#endif
  ui->rec_tip_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_call_function()));
#if 0
  int mode;
  ui_get_ui_mode(&mode);
  if (mode == MODE_FACTORY) {
    ui->clean_btn->setEnabled(false);
    ui->clean_btn->setStyleSheet("background:#454A60");
  } else {
    ui->clean_btn->hide();
  }
#endif
  pTimer->start(TIMER_INTERVAL_COUNT);
  this->showMaximized();
  load_user_from_db();
}

ui_user_view::~ui_user_view() {
  delete ui;
  delete pTimer;
  if (tableModel) {
    tableModel->clear();
    delete tableModel;
    tableModel = NULL;
  }
}

void ui_user_view::onEditTableBtnClicked() {
  QPushButton *edit_btn = reinterpret_cast <QPushButton *>(sender());
  qDebug() << edit_btn->property("id").toString() << endl;
}

void ui_user_view::onDelTableBtnClicked() {
  int found = 0;
  QPushButton *del_btn = reinterpret_cast <QPushButton *>(sender());
  QString id = del_btn->property("id").toString();
  QStandardItemModel * model =
      reinterpret_cast<QStandardItemModel *>(ui->user_tableView->model());
  for (int i = 0; i < model->rowCount(); i++) {
    QString str = model->index(i, 1).data(Qt::DisplayRole).value<QString>();
    if (str == id) {
#if 0
      model->removeRow(i);
      if ((i != 0) && (i >= model->rowCount())) {
        i--;
      }
      ui->user_tableView->selectRow(i);
      ui_control_person_info_del_by_user_id(id.toUtf8().data());
#endif
      snprintf(user_id, sizeof(user_id), "%s", id.toStdString().data());
      found = 1;
      break;
    }
  }

  if (0 ==found) {
    return;
  }

  ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_DELETE_CONFRIM, this);
  w->set_user_win(this);
  w->show();
}

void ui_user_view::delete_selected_user() {
  int found = 0;
  QStandardItemModel * model =
      reinterpret_cast<QStandardItemModel *>(ui->user_tableView->model());
  for (int i = 0; i < model->rowCount(); i++) {
    QString str = model->index(i, 1).data(Qt::DisplayRole).value<QString>();
    if (str.compare(user_id) == 0) {
      model->removeRow(i);
      if ((i != 0) && (i >= model->rowCount())) {
        i--;
      }
      ui->user_tableView->selectRow(i);
      ui_control_person_info_del_by_user_id(user_id);
      found = 1;
      break;
    }
  }

  if (1 == found) {
    rec_count_--;
    if (rec_count_ < 0) {
      rec_count_ = 0;
    }
    if (rec_count_ <= ONE_PAGE_REC_NUM) {
      index_ = 0;
    }
    load_user_from_db();
  }
}

void ui_user_view::clean_all_items() {
  QStandardItemModel * model =
      reinterpret_cast<QStandardItemModel *>(ui->user_tableView->model());
  model->clear();
}

void ui_user_view::slot_for_call_function() {
    pTimer->stop();
    // load_user_from_db();
}

void ui_user_view::load_user_from_db() {
  if ((rec_count_ == 0) && (index_ == 0)) {
    /*
    ee_queue_t head_person;
    ee_queue_init(&head_person);
    rec_count_ = ui_control_person_info_query_begin(&head_person,
                      TYPE_PERSON);
    ui_control_person_info_query_end(&head_person, TYPE_PERSON);
    */
    rec_count_ = ui_control_person_info_get_count();
  }

  display_users();
  int page_index, pages;
  if (rec_count_ > 0) {
    page_index = index_ / ONE_PAGE_REC_NUM + 1;
    pages = ((rec_count_ % ONE_PAGE_REC_NUM) > 0) ?
            (rec_count_ / ONE_PAGE_REC_NUM + 1) :
            (rec_count_ / ONE_PAGE_REC_NUM);
    QString str = QString(tr("第%1页,共%2页"))
                      .arg(page_index)
                      .arg(pages);
    ui->rec_tip_label->setText(str);
  }
}

void ui_user_view::display_users() {
  if (tableModel) {
    tableModel->clear();
    delete tableModel;
    tableModel = NULL;
  }

  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();

  if (width > height) {
    ui->user_tableView->setMinimumSize(TABLE_VIEW_WIDTH, TABLE_VIEW_HEIGHT);
  } else {
    ui->user_tableView->setMinimumSize(width - 0.1 * width, TABLE_VIEW_HEIGHT);
  }
  ui->user_tableView->verticalHeader()->hide();

  tableModel = new QStandardItemModel(this);
  ui->user_tableView->setModel(tableModel);

  QStringList columnTitles;
  columnTitles << "" << tr("id") << tr("姓名") << "" << tr("删除");
  tableModel->setHorizontalHeaderLabels(columnTitles);

  int index, mode, count;
  websocket_person_info_t *p = NULL;

  ui_get_ui_mode(&mode);

  ee_queue_t head_person;
  ee_queue_init(&head_person);

  count = ui_control_person_info_get_begin(
              &head_person, index_, ONE_PAGE_REC_NUM);
  if (count > 0) {
    index = 0;
    ee_queue_foreach(p, &head_person, websocket_person_info_t, queue) {
      tableModel->setItem(index, 0, new QStandardItem(""));
      tableModel->setItem(index, 1, new QStandardItem(p->user_id));
      tableModel->setItem(index, 2, new QStandardItem(p->name));
      tableModel->item(index, 2)->setTextAlignment(Qt::AlignCenter);
      tableModel->setItem(index, 3, new QStandardItem(""));

      if (mode == MODE_FACTORY) {
        // add button to the last column
        QPushButton *del_btn = new QPushButton("");

        del_btn->setProperty("id", p->user_id);
        // set custom property
        del_btn->setProperty("index", QString("%1").arg(index));
        del_btn->setProperty("status", "normal");
        del_btn->setFixedSize(DEL_BUTTON_WIDTH, DEL_BUTTON_HEIGHT);
        connect(del_btn, SIGNAL(clicked()), this,
                SLOT(onDelTableBtnClicked()));
        del_btn->setStyleSheet(
          "QPushButton{border-image:url(:/icon/del.png);max-width:64px;}");

        // notice every time insert the button at the last line
        ui->user_tableView->setIndexWidget(
            tableModel->index(tableModel->rowCount() - 1, 4), del_btn);
        // tableModel->setData(tableModel->index(tableModel->rowCount() - 1, 4),
        //    Qt::AlignRight, Qt::TextAlignmentRole);
      }

      // set row height
      ui->user_tableView->setRowHeight(index, TABLE_ROW_HEIGHT);

      // set column width
#if 0
      ui->user_tableView->setColumnWidth(0, static_cast<int>(0.2 * width));
      ui->user_tableView->setColumnWidth(1, static_cast<int>(0.3 * width));
      ui->user_tableView->setColumnWidth(2, static_cast<int>(0.2 * width));
      ui->user_tableView->setColumnWidth(3, static_cast<int>(0.3 * width));
#else
      if (width > height) {
        ui->user_tableView->setColumnWidth(0, static_cast<int>(0.019 * width));
        ui->user_tableView->setColumnWidth(1, static_cast<int>(0.231 * width));
        ui->user_tableView->setColumnWidth(2, static_cast<int>(0.425 * width));
        ui->user_tableView->setColumnWidth(3, static_cast<int>(0.15 * width));
        ui->user_tableView->setColumnWidth(4, static_cast<int>(0.1 * width));
      } else {
        ui->user_tableView->setColumnWidth(0, static_cast<int>(0.019 * width));
        ui->user_tableView->setColumnWidth(1, static_cast<int>(0.231 * width));
        ui->user_tableView->setColumnWidth(2, static_cast<int>(0.425 * width));
        ui->user_tableView->setColumnWidth(3, static_cast<int>(0.05 * width));
        ui->user_tableView->setColumnWidth(4, static_cast<int>(0.2 * width));
      }
#endif
      ui->user_tableView->setFocusPolicy(Qt::NoFocus);
      ui->user_tableView->setShowGrid(false);

      ui->user_tableView->horizontalHeader()->setVisible(false);
      ui->user_tableView->verticalHeader()->setVisible(false);

      //  set select one row
      ui->user_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
      ui->user_tableView->setSelectionMode(QAbstractItemView::NoSelection);

      //  set only read
      ui->user_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

      index++;
    }
    // ui->user_tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  } else {
    ui->user_tableView->horizontalHeader()->setVisible(false);
    ui->user_tableView->verticalHeader()->setVisible(false);
  }

  ui_control_person_info_get_end(&head_person);
#if 0
  if (mode == MODE_FACTORY) {
    ui->clean_btn->setEnabled(true);
    ui->clean_btn->setStyleSheet("background:#00d5cf");
  } else {
    ui->clean_btn->hide();
  }
#endif
}

void ui_user_view::slot_for_first() {
  index_ = 0;
  load_user_from_db();
}

void ui_user_view::slot_for_prev() {
  index_ -= ONE_PAGE_REC_NUM;
  if (index_ < 0) {
    index_ = 0;
  }
  load_user_from_db();
}

void ui_user_view::slot_for_next() {
  index_ += ONE_PAGE_REC_NUM;
  if (index_ >= rec_count_) {
    if (rec_count_ % ONE_PAGE_REC_NUM > 0) {
      index_ = rec_count_ - (rec_count_ % ONE_PAGE_REC_NUM);
    } else {
      index_ = rec_count_ - ONE_PAGE_REC_NUM;
    }
  }
  load_user_from_db();
}

void ui_user_view::slot_for_last() {
  if (rec_count_ % ONE_PAGE_REC_NUM > 0) {
    index_ = rec_count_ - (rec_count_ % ONE_PAGE_REC_NUM);
  } else {
    index_ = rec_count_ - ONE_PAGE_REC_NUM;
  }
  load_user_from_db();
}

