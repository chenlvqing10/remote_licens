/*
* ui_user_view_list.cpp define user view list function of UI
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
#include "person_output.h"
#include "hotplug.h"
#include "file_ops.h"
#include "screen_size.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QObject>
#include <QDebug>
#include <QModelIndex>

#include "ui_rotate_management.hpp"
#include "ui_user_view_list.hpp"
#include "ui_ui_user_view_list.h"
#include "ui_setting_item_delegate.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_ui_tip_dialog.h"

#ifndef PORTRAIT_SCREEN
  #define TOP_BUTTOM 151
  #define ROW_WIDTH  98
#else
  #define TOP_BUTTOM 192
  #define ROW_WIDTH  92
#endif

#if ATTENDANCE_MODE == 2
#define BT_DEL_ICON ""
#else
#define BT_DEL_ICON ":/icon/del.png"
#endif

#define EVENT_UPDATE_DB 0x01
#define EVENT_LOAD_DB 0x02

void local_output_cb(void *win, EN_OUTPUT_TYPE state, int step) {
#if 0
  ui_user_view_list *g_win = (ui_user_view_list *)win;
  g_win->output_user_cb(state, step);
#endif
  send_signal_for_output((int)state, step);
}

static void * threadLoadDB(void *arg) {
  ui_user_view_list *win = (ui_user_view_list *)arg;
  win->get_user_list();
  win->sendSignalForMainUI(EVENT_LOAD_DB, 1);
  return NULL;
}

static void * threadUpdateDB(void *arg) {
  ui_user_view_list *win = (ui_user_view_list *)arg;
  face_db_update();
  win->sendSignalForMainUI(EVENT_UPDATE_DB, 1);
  return NULL;
}

ui_user_view_list::ui_user_view_list(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_user_view_list),
  pModel(NULL),
  pItemDelegate(NULL) {
  item_t item;
  item.type = EN_ITEM_IMAGE;
  page = 0;
  pages = 0;
  count = 0;
  select_index = -1;

  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  pModel = new QStandardItemModel();
  count_height = (get_screen_height() - TOP_BUTTOM) / ROW_WIDTH;
  for (int i = 0; i < count_height; ++i) {
    QStandardItem *pItem = new QStandardItem;
    pItem->setData(QVariant::fromValue(item), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_setting_item_delegate(this);
  ui->user_view_listView->setItemDelegate(pItemDelegate);
  ui->user_view_listView->setModel(pModel);

  ui->rec_tip_label->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
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

  SignalTable *signal_table =
    reinterpret_cast<SignalTable *>(signal_get_handle());
  connect(signal_table, SIGNAL(signalForOutput(int, int)),
    this, SLOT(output_user_cb(int, int)));
  connect(ui->search_btn, SIGNAL(clicked()),
    this, SLOT(slot_for_search()));

#if ATTENDANCE_MODE == 2
  ui->output_btn->setText("");
  ui->output_btn->setFlat(true);
  ui->output_btn->setFocusPolicy(Qt::NoFocus);
  ui->output_btn->setStyleSheet("background-color:transparent");
#else
  connect(ui->user_view_listView, SIGNAL(clicked(QModelIndex)),
    this, SLOT(slot_for_del_user_item(QModelIndex)));
  connect(ui->output_btn, SIGNAL(clicked()),
    this, SLOT(slot_for_output()));
#endif
  memset(user_id, 0, sizeof(user_id));
  memset(user_name, 0, sizeof(user_name));
  count = db_ui_local_person_get_count(user_id, user_name);
  pages = ceil((double)count/count_height);

#if 0
  if (count < 500) {
    get_user_list();
    update_page(BT_GO_FIRST);
  } else {
    sendSignalForMainUI(EVENT_LOAD_DB, 0);
  }
#endif
  update_page(BT_GO_FIRST);

  ui->user_view_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_user_view_list::~ui_user_view_list() {
  ALOGD("%s delete", __func__);

  user_list.clear();

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

void ui_user_view_list::get_user_list() {
  int ret = 0;
  ee_queue_t _head;
  ee_queue_t *h_person = &_head;
  local_person_info_t *p = NULL;
  item_t tmp;
  ALOGD("%s:%d >> start load db.", __func__, __LINE__);
  user_list.clear();
#if 0
  ret = 0;
  while (ret < count) {
    ret += ui_control_get_person_txt_begin(h_person, ret, 500);
    ee_queue_foreach(p, h_person,
      local_person_info_t, queue) {
      tmp.icon = QString(p->user_id);
      tmp.title = QString(p->name);
      tmp.value = BT_DEL_ICON;
      tmp.type = EN_ITEM_IMAGE;
      user_list.append(tmp);
    }
    ui_control_local_person_get_end(h_person);
  }
#endif
  if (strlen(user_id) > 0 || strlen(user_name) > 0) {
    ret = db_ui_local_person_query_search_begin(h_person, user_id, user_name,
      count_height * page, count_height);
  } else {
    ret = ui_control_local_person_get_begin(h_person,
      count_height * page, count_height);
  }
  ee_queue_foreach(p, h_person,
    local_person_info_t, queue) {
    tmp.icon = QString(p->user_id);
    tmp.title = QString(p->name);
    tmp.value = BT_DEL_ICON;
    tmp.type = EN_ITEM_IMAGE;
    user_list.append(tmp);
  }
  ui_control_local_person_get_end(h_person);

  h_person = NULL;
  if (ret <= 0) {
    user_list.clear();
  }
  ALOGD("%s:%d >> end load db.", __func__, __LINE__);
#if 0
  for (int i = 0; i < user_list.size(); ++i) {
    tmp = user_list.at(i);
    ALOGE("%s[%d] >> %s, %s, %s", __func__, i,
      tmp->icon, tmp->title, tmp->value);
  }
#endif
}

void ui_user_view_list::slot_for_mainui(int event, int value) {
  /* run in main thread */
  static ui_tip_dialog *loading_w;
  pthread_t tidp = 0;

  ALOGD("%s:%d >> event=%d, value=%d",
    __func__, __LINE__, event, value);
  if (EVENT_UPDATE_DB == event) {
    if (0 == value) {
      if (count < 500) {
        face_db_update();
      } else {
          if (!loading_w) {
            loading_w = new ui_tip_dialog(TIP_WIN_TYPE_NOTE, this);
            loading_w->set_content(tr("正在删除用户注册数据...").
              toUtf8().data());
            loading_w->show();
          }
          if (-1 == pthread_create(&tidp, NULL, threadUpdateDB,
            (void *)this)) {
            ALOGE("pthread_create test thread failed!!\n");
            loading_w->start_count_exit();
            return;
          }
          pthread_detach(tidp);
      }
    } else {
        if (loading_w) {
          loading_w->start_count_exit();
          loading_w = NULL;
        }
        file_system_sync();
    }
  } else if (EVENT_LOAD_DB == event) {
    if (0 == value) {
      if (!loading_w)
        loading_w = new ui_tip_dialog(TIP_WIN_TYPE_NOTE, this);
        loading_w->set_content(tr("正在加载用户数据...").toUtf8().data());
        loading_w->show();
      if (-1 == pthread_create(&tidp, NULL, threadLoadDB, (void *)this)) {
        ALOGE("pthread_create test thread failed!!\n");
        loading_w->start_count_exit();
        return;
      }
      pthread_detach(tidp);
    } else {
        if (loading_w) {
          loading_w->start_count_exit();
          loading_w = NULL;
        }
        update_page(BT_GO_FIRST);
        file_system_sync();
    }
  }
}

void ui_user_view_list::delete_selected_user() {
  if (-1 != select_index) {
    count--;
    item_t select = user_list.at(select_index);
    ui_control_person_info_del_by_user_id(
      select.icon.toLatin1().data());
    face_db_delete(select.icon.toLatin1().data());
    user_list.removeAt(select_index);
    pages = ceil((double)count/count_height);
    page = (select_index+count_height * page)/count_height;
    refresh_page();
  }
  select_index = -1;
}

void ui_user_view_list::refresh_page() {
  item_t item;
  item.type = EN_ITEM_IMAGE;
  QString str;

  get_user_list();
  if (0 == count) {
    str = QString(tr("第%1/%2页"))
      .arg(0).arg(0);
    for (int i = 0; i < count_height; i++) {
      pModel->setData(pModel->index(i, 0),
        QVariant::fromValue(item),
        Qt::UserRole + 1);
    }
  } else {
    str = QString(tr("第%1/%2页"))
      .arg(page+1).arg(pages);
    for (int i = 0; i < count_height; i++) {
      if (i + count_height * page < count)
        pModel->setData(pModel->index(i, 0),
          QVariant::fromValue(
#if 0
          user_list.at(i + count_height * page)),
#endif
          user_list.at(i)),
          Qt::UserRole + 1);
      else
        pModel->setData(pModel->index(i, 0),
          QVariant::fromValue(item),
          Qt::UserRole + 1);
    }
  }
  ui->rec_tip_label->setText(str);
  ui->user_view_listView->update();
}

void ui_user_view_list::update_page(BT_GO_TYPE type) {
  switch (type) {
    case BT_GO_FIRST:
      page = 0;
      break;
    case BT_GO_PREV:
      if (0 < page)
        page -= 1;
      else
        return;
      break;
    case BT_GO_NEXT:
      if (page < pages - 1)
        page += 1;
      else
        return;
      break;
    case BT_GO_LAST:
      page = pages - 1;
      break;
  }
  refresh_page();
}

void ui_user_view_list::output_user_cb(
  int state, int step) {
  static ui_tip_dialog *loading_w;
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->output_btn->setEnabled(true);
  ALOGD("%s:%d >> state=%d, step=%d", __func__, __LINE__,
    state, step);
  if (EN_OUTPUT_WORKING == state) {
    rotate_signgleton->set_setting_count_state(TIMER_COUNTS_STOP);
    ALOGD("user list outputing[%d'%']", step);
    if (!loading_w)
      loading_w = new ui_tip_dialog(TIP_WIN_TYPE_NOTE, this);
    QString loading =
      QString(tr("正在导出，请勿拔出U盘,完成%1%"))
      .arg(step);
    loading_w->set_content(loading.toUtf8().data());
    loading_w->show();
  } else if (EN_OUTPUT_OK == state) {
    if (!loading_w)
      loading_w = new ui_tip_dialog(TIP_WIN_TYPE_NOTE, this);
    loading_w->set_content(tr("导出用户成功").toUtf8().data());
    loading_w->start_count_exit();
    loading_w = NULL;
  } else if (EN_OUTPUT_ERROR == state) {
      if (!loading_w)
        loading_w = new ui_tip_dialog(TIP_WIN_TYPE_NOTE, this);
      loading_w->set_content(tr("导出用户失败, 请检查U盘是否正常！").toUtf8().data());
      loading_w->start_count_exit();
      loading_w = NULL;
  }
  rotate_signgleton->set_setting_count_state(TIMER_COUNTS_START);
}

void ui_user_view_list::slot_for_search() {
  user_search_args_t *time_info;
  ui_user_search_dialog *w;
  time_info = new user_search_args_t;
  time_info->cb = (ui_user_search_callback *)this;
  w = new ui_user_search_dialog(time_info, this);
  w->show();
  delete time_info;
}

void ui_user_view_list::slot_for_del_user_item(
  const QModelIndex &index) {
  ui_tip_dialog *w;
  Ui::ui_tip_dialog* ui;
  QVariant var = index.data(Qt::UserRole + 1);
  item_t itemData = var.value<item_t>();

  if (itemData.title != NULL) {
    select_index = index.row();
    w = new ui_tip_dialog(TIP_WIN_TYPE_DELETE_CONFRIM, this);
    ui = w->get_ui();
    ui->tip_content_label->setText(
      QString(tr("是否删除用户“%1”?"))      .arg(itemData.title));
    w->set_user_win(this);
    w->show();
  }
}

void ui_user_view_list::slot_for_first() {
  update_page(BT_GO_FIRST);
}

void ui_user_view_list::slot_for_prev() {
  update_page(BT_GO_PREV);
}

void ui_user_view_list::slot_for_next() {
  update_page(BT_GO_NEXT);
}

void ui_user_view_list::slot_for_last() {
  update_page(BT_GO_LAST);
}

void ui_user_view_list::slot_for_output() {
  if (udisk_is_ok() != 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(tr("未检测到u盘插入").toUtf8().data());
    w->show();
  } else {
    ui->output_btn->setEnabled(false);
    person_output_start((void *)this, local_output_cb);
  }
}

void ui_user_view_list::user_search_callback(QString id, QString name) {
  memset(user_id, 0, sizeof(user_id));
  memset(user_name, 0, sizeof(user_id));
  if (!id.isEmpty())
    memcpy(user_id, id.toUtf8().data(), sizeof(user_id));
  if (!name.isEmpty())
    memcpy(user_name, name.toUtf8().data(), sizeof(user_name));
  count = db_ui_local_person_get_count(user_id, user_name);
  pages = ceil((double)count/count_height);
  slot_for_first();
}

