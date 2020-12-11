/*
* ui_wifi_list.cpp - define interface function of UI
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
#define LOG_TAG     "wifi_list"
#include <log/log.h>
#include <sys/prctl.h>

#include "dd_md5_i.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>

#include "ui_wifi_list.hpp"
#include "ui_ui_wifi_list.h"
#include "ui_rotate_management.hpp"

#define FIELD_VALUE_SIZE 32
#define MAX_FIELD_VALUE_LEN 64
#define CURRENT_TMP_LEN 2
#define MD5_BUFFER_LEN 16

#define TRIGGER_INTERVAL 500

#ifndef PORTRAIT_SCREEN
  #define ICON_BACK_AMAGE 72
  #define ROW_PITCH  18
  #define ROW_WIDTH  98
#else
  #define ICON_BACK_AMAGE 42
  #define ROW_PITCH  0
  #define ROW_WIDTH  92
#endif
#define CHARS(qs) qs.toUtf8().data()

#define SWITCH_ON_ICON	":/icon/turn_on.png"
#define SWITCH_OFF_ICON	":/icon/turn_off.png"
#define WIFI_RSSI_STD_VALUE (-50)
#define MIN_RSSI_VALUE (-100)
#define MAX_RSSI_VALUE (-55)
#define LEVEL_RSSI_VALUE (45)

#define TEST_LEN (3)
char *TEST_SSIDS[TEST_LEN ][2] = {
  /* {ssid, pwd} SECURITY_WPA_PSK default */
  {"wifi_123", "12345678"},
  {"123456789ABCDEFGHIJKLMNOPQRSTUVW", "00000000"},
  {"Hongbang2", "12345678"}
};

const char *WIFI_RSSI[2][4] = {
  {":/icon/wifi_level_four.png", ":/icon/wifi_level_three.png",
  ":/icon/wifi_level_two.png", ":/icon/network_setting_wifi.png"},

  {":/icon/wifi_level_lock_zero.png", ":/icon/wifi_level_lock_one.png",
  ":/icon/wifi_level_lock_two.png", ":/icon/wifi_level_lock_three.png"}
};

  void ui_wifi_list::uinit(){
    treatment_type[0] = tr("禁止网络");
    treatment_type[1] = tr("删除网络");
  }
ui_wifi_list::ui_wifi_list(QWidget *parent) : QtMainWindow(parent),
  ui(new Ui::ui_wifi_list) {
  ALOGD("%s:%d", __func__, __LINE__);

  setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);
  uinit();
  mgr = wifi_management::getInstance();

  pModel = new QStandardItemModel();
  pItemDelegate = new ui_list_item_delegate(this);
  ui->wifi_show_listView->setItemDelegate(pItemDelegate);
  ui->wifi_show_listView->setModel(pModel);
  ui->wifi_show_listView->setEditTriggers(
        QAbstractItemView::NoEditTriggers);

  connect(ui->wifi_setting_back_btn, SIGNAL(clicked()),
    ui_rotate_management::getInstance(), SLOT(back_activity_win()));
  connect(ui->wifi_show_listView, SIGNAL(click(QModelIndex)),
    this, SLOT(slot_for_click_item(QModelIndex)));
  connect(ui->wifi_show_listView, SIGNAL(long_click(QModelIndex)),
    this, SLOT(slot_for_long_click_item(QModelIndex)));
  connect(mgr, SIGNAL(signal_wifi_state_change(int)), this,
    SLOT(update_wifi_state(int)));
  connect(mgr, SIGNAL(signal_wifi_scan()), this, SLOT(update_wifi_list()));

#if ATTENDANCE_MODE == 2
  ui->wifi_show_listView->hide();
  display_current_connect();
#else
  ALOGD("%s:%d", __func__, __LINE__);
  mgr->request_scan_now();
  update_wifi_list();
#endif
#if 0
  debug_test_wifi();
#endif
}

ui_wifi_list::~ui_wifi_list() {
  ALOGD("%s:%d", __func__, __LINE__);
  wifi_list.clear();

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

void ui_wifi_list::deinit() {
  ALOGD("%s:%d", __func__, __LINE__);
  QtMainWindow::deinit();
  mgr->save_config();
}

void ui_wifi_list::update_wifi_state(int state) {
  wifi_state stat = (wifi_state)state;
  ALOGD("%s:%d >> state = %d", __func__, __LINE__, state);
  display_current_connect();
#if ATTENDANCE_MODE != 2
  ALOGD("%s:%d", __func__, __LINE__);
  if (WIFI_STATION_CLOSING == state)
    update_wifi_list_imp(false);
  else if (WIFI_STATION_CONNECTING == state)
    return;
  else
    update_wifi_list();
#endif
}

void ui_wifi_list::update_wifi_list_imp(bool ishow) {
  ALOGD("%s:%d", __func__, __LINE__);
  int ret = 0;
  wifi_dot item;
  QStandardItem *pItem;
  pModel->clear();

  //wifi switch
  memset(&item, 0, sizeof(wifi_dot));
  strncpy(item.ssid, "Wi-Fi", WIFI_NAME_LEN);
  item.netid = -1;
  item.rssi = mgr->is_wifi_enabled()?1:0;
  pItem = new QStandardItem;
  pItem->setData(QVariant::fromValue(item), Qt::UserRole + 1);
  pModel->appendRow(pItem);

  if (ishow) {
    ret = mgr->get_scan_list(&wifi_list);
    ALOGD("%s:%d >> ret = %d", __func__, __LINE__, ret);
    if (ret) {
      memset(&item, 0, sizeof(wifi_dot));
      ret = mgr->get_alive_connect(&item);
      if (0 == ret) {
        //current connected wifi
        pItem = new QStandardItem;
        pItem->setData(QVariant::fromValue(item), Qt::UserRole + 1);
        pModel->appendRow(pItem);
      }
      ALOGD("%s:%d >> count=%d", __func__, __LINE__, wifi_list.count());
      for (int i = 0; i < wifi_list.count(); i++) {
        if (0 == strcmp(wifi_list.at(i).ssid, item.ssid))
          continue;
        pItem = new QStandardItem;
        pItem->setData(QVariant::fromValue(wifi_list.at(i)),
          Qt::UserRole + 1);
        pModel->appendRow(pItem);
      }
    } else {
      ALOGW("%s:%d >> wifi manager wifi list fail!",
        __func__, __LINE__);
      wifi_list.clear();
    }
  } else {
    ALOGW("%s:%d >> wifi disable or get wifi manager wifi list fail!",
      __func__, __LINE__);
    wifi_list.clear();
  }
  ui->wifi_show_listView->update();
}

void ui_wifi_list::update_wifi_list() {
  ALOGD("%s:%d", __func__, __LINE__);
  update_wifi_list_imp(mgr->is_wifi_enabled());
  display_current_connect();
}

void ui_wifi_list::display_current_connect() {
  wifi_dot dot;

  wifi_state state = mgr->get_wifi_state();
  ALOGD("%s:%d >> state=%d", __func__, __LINE__, state);
  ui->connect_label->setText("");
  ui->connect_label->show();

  if (WIFI_STATION_STARTING == state)
    ui->connect_label->setText(QString(tr("正在启动wifi ......")));
  else if(WIFI_STATION_CLOSING == state)
    ui->connect_label->setText(QString(tr("正在关闭wifi ......")));
  else if(WIFI_STATION_SCANNING == state) {
    if (wifi_list.count() == 0)
      ui->connect_label->setText(QString(tr("正在扫描wifi ......")));
  }
#if ATTENDANCE_MODE == 2
  else if(WIFI_STATION_CONNECTED == state) {
    char ip[32] = {0};
    char mac[32] = {0};
    memset(&dot, 0, sizeof(wifi_dot));
    mgr->get_alive_connect(&dot);
    net_get_info(ip, sizeof(ip), mac, sizeof(mac));
    ui->connect_label->setText(QString(tr("当前WIFI连接: ssid %1     ip %2"))
      .arg(dot.ssid).arg(ip));
  }
  else if(WIFI_STATION_UNAVAILABLE == state)
    ui->connect_label->setText(QString(tr("网络不可达！")));
  else if(WIFI_STATION_FAILED == state)
    ui->connect_label->setText(QString(tr("网络连接失败！")));
#endif
  else {
    ALOGD("%s:%d >> recv state=%d", __func__, __LINE__, state);
    ui->connect_label->hide();
    return;
  }
}

void ui_wifi_list::edit_callback(edit_args_t *args) {
  if (mgr->is_wifi_enabled()) {
    ALOGD("%s:%d >> %s | %s", __func__, __LINE__,
      args->key.toUtf8().data(),
      args->value.toUtf8().data());
    wifi_dot dot;
    memset(&dot, 0, sizeof(wifi_dot));
    if (mgr->get_wifi_dot(&dot, args->key) == 0) {
      strncpy(dot.pwd, args->value.toUtf8().data(), WIFI_PWD_LEN);
      ALOGD("%s:%d >> ssid=%s pwd=%s", __func__, __LINE__,
        dot.ssid, dot.pwd);
      mgr->connect_network(&dot);
    }
  }
}

int ui_wifi_list::tip_callback(tip_args_t *args, bool isCancel) {
  if (isCancel) return 0;
  if (mgr->is_wifi_enabled()) {
    ALOGD("%s:%d >> %s", __func__, __LINE__,
      args->value.toUtf8().data());
    wifi_dot dot;
    memset(&dot, 0, sizeof(wifi_dot));
    if (mgr->get_wifi_dot(&dot, args->value) == 0) {
      ALOGD("%s:%d >> id=%d ssid=%s", __func__, __LINE__,
        (int)args->id, dot.ssid);
      if (ACT_WIFI_DISABLE == (int)args->id)
        mgr->disable_network(&dot);
      else if (ACT_WIFI_DELDOT == (int)args->id)
        mgr->remove_network(&dot);
    }
  }
  return 0;
}

void ui_wifi_list::slot_for_click_item (QModelIndex index) {
  radio_args_t *radio_info;
  ui_radio_dialog *w;
  ALOGD("%s:%d >> row = %d, state = %s", __func__, __LINE__, index.row(),
    CHARS(mgr->get_state_notes(WIFI_STATION_CONNECTING)));
  if (0 == index.row()) {
    bool enabled = !mgr->is_wifi_enabled();
    mgr->set_wifi_enabled(enabled);
    mgr->save_config();
    if (!enabled)
      update_wifi_list_imp(false);
  } else {
    wifi_dot cur_cn;
    QVariant var = index.data(Qt::UserRole + 1);
    wifi_dot it = var.value<wifi_dot>();
    ALOGD("%s:%d >> row = %d. ssid=%s", __func__, __LINE__,
      index.row(), it.ssid);
    memset(&cur_cn, 0, sizeof(wifi_dot));
    if (mgr->get_alive_connect(&cur_cn) != 0)
      memset(&cur_cn, 0, sizeof(wifi_dot));

    if (0 == strcmp(it.ssid, cur_cn.ssid) &&
      WIFI_STATION_CONNECTED == mgr->get_wifi_state()) {
#if 0
      tip_args_t *tip_info = new tip_args_t;
      tip_info->id = reinterpret_cast<void *>(ACT_WIFI_DISABLE);
      tip_info->title = QString(tr("禁止网络"));
      tip_info->content = QString(tr("是否禁止“%1”连接?").arg(it.ssid))      ;
      tip_info->value = QString(it.ssid);
      tip_info->cb = this;
      ui_tip_dialog *w = new ui_tip_dialog(tip_info, this);
      w->show();
      delete tip_info;
#endif
      ssid = it.ssid;
      radio_info = new radio_args_t;
      radio_info->title = QString(it.ssid);
      for (auto iter = treatment_type.begin() ;
        iter != treatment_type.end(); ++iter) {
        radio_info->radio_list << iter.value();
      }
      radio_info->default_index = 0;
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
    } else {
      wifi_state state = mgr->get_wifi_state();
      if (0 == strcmp(it.ssid, cur_cn.ssid) &&
        (WIFI_STATION_CONNECTING == state ||
        WIFI_STATION_DHCPCING == state)) {
        ALOGD("ssid[%s] is connecting, ignore click.", it.ssid);
        return;
      }
      int ret = mgr->connect_network(&it);
      ALOGD("%s:%d >> get network id = %d , ret = %d", __func__, __LINE__,
        it.netid, ret);
      if (ret) {
        edit_args_t *edit_info = new edit_args_t;
        edit_info->id = reinterpret_cast<void *>(this);
        edit_info->title = QString(tr("%1 网络").arg(it.ssid));
        edit_info->key = QString(it.ssid);
        edit_info->cb = this;
        ui_edit_dialog *w = new ui_edit_dialog(edit_info, this);
        w->show();
        delete edit_info;
      }
    }
  }
}

void ui_wifi_list::slot_for_long_click_item (QModelIndex index) {
  ALOGD("%s:%d >> row = %d", __func__, __LINE__, index.row());
  if (0 == index.row()) {
    bool enabled = !mgr->is_wifi_enabled();
    mgr->set_wifi_enabled(enabled);
    mgr->save_config();
    if (!enabled)
      update_wifi_list_imp(false);
  } else {
    QVariant var = index.data(Qt::UserRole + 1);
    wifi_dot it = var.value<wifi_dot>();
    ALOGD("%s:%d >> row = %d. ssid=%s", __func__, __LINE__, index.row(), it.ssid);
#if 0
    tip_args_t *tip_info = new tip_args_t;
    tip_info->id = reinterpret_cast<void *>(ACT_WIFI_DELDOT);
    tip_info->title = QString(tr("删除网络"));
    tip_info->content = QString(tr("是否删除“%1”网络?").arg(it.ssid))      ;
    tip_info->value = QString(it.ssid);
    tip_info->cb = this;
    ui_tip_dialog *w = new ui_tip_dialog(tip_info, this);
    w->show();
    delete tip_info;
#endif
  }
}

ui_wifi_list::ui_list_item_delegate::ui_list_item_delegate(
  QObject *parent): QStyledItemDelegate(parent) {
  mgr = wifi_management::getInstance();
  cur_cn = new wifi_dot;
}

ui_wifi_list::ui_list_item_delegate::~ui_list_item_delegate() {
  delete cur_cn;
}

void ui_wifi_list::ui_list_item_delegate::paint(
    QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  int len = 400;
  wifi_dot itemData;

  if (index.isValid()) {
    painter->save();
    QVariant var = index.data(Qt::UserRole + 1);
    itemData = var.value<wifi_dot>();
    if (0 == index.row()) {
      memset(cur_cn, 0, sizeof(wifi_dot));
      if (mgr->get_alive_connect(cur_cn) != 0)
        memset(cur_cn, 0, sizeof(wifi_dot));
    }

    // item 矩形区域
    QRectF rect;
    rect.setX(option.rect.x());
    rect.setY(option.rect.y());
    rect.setWidth(option.rect.width() - 0);
    rect.setHeight(option.rect.height() - 0);

    QPainterPath path;
    path.moveTo(rect.topRight());
    path.lineTo(rect.topLeft());
    path.quadTo(rect.topLeft(), rect.topLeft());
    path.lineTo(rect.bottomLeft());
    path.quadTo(rect.bottomLeft(), rect.bottomLeft());
    path.lineTo(rect.bottomRight());
    path.quadTo(rect.bottomRight(), rect.bottomRight());
    path.lineTo(rect.topRight());
    path.quadTo(rect.topRight(), rect.topRight());

    if(0 == index.row())
      paint_wifi_switch(painter, rect, itemData);
    else
      paint_wifi_item(painter, rect, itemData);

    painter->setPen(QPen(QColor("#102050")));
    if (0 == option.rect.y()) {
      painter->drawRect(
      rect.left() + ROW_PITCH, rect.top() + 1, rect.width() -
        ROW_PITCH * 2, 1);
      painter->drawRect(
      rect.left() + ROW_PITCH, rect.top() + 2, rect.width() -
        ROW_PITCH * 2, 1);
    }
    painter->drawRect(
      rect.left() + ROW_PITCH, rect.bottom() - 1, rect.width() -
        ROW_PITCH * 2, 1);
    painter->drawRect(
      rect.left() + ROW_PITCH, rect.bottom() - 2, rect.width() -
        ROW_PITCH * 2, 1);

    painter->setPen(QPen(Qt::black));
    painter->restore();

  }
}

void ui_wifi_list::ui_list_item_delegate::paint_wifi_switch(
  QPainter *painter, QRectF rect, const wifi_dot &it) const {
  QRectF titleRect = QRect(
    rect.left() + ICON_BACK_AMAGE, rect.top() + 18, 400, 62);
  QRectF swRect = QRect(
    rect.right() - ICON_BACK_AMAGE-102, rect.top() + 18, 102, 62);

  painter->setPen(QPen(Qt::white));
  painter->setFont(QFont("SourceHanSansCN-Normal", 38));
  painter->drawText(titleRect, it.ssid);

  if (it.rssi) {
    // be checked
    painter->drawImage(swRect, QImage(SWITCH_ON_ICON));
  } else {
    // be unchecked
    painter->drawImage(swRect, QImage(SWITCH_OFF_ICON));
  }
}

void ui_wifi_list::ui_list_item_delegate::paint_wifi_item(
  QPainter *painter, QRectF rect, const wifi_dot &it) const {
  static int step = (MAX_RSSI_VALUE - MIN_RSSI_VALUE) / 3;
  int level;
  if (it.rssi <= MIN_RSSI_VALUE) {
    level = 0;
  } else if (it.rssi >= MAX_RSSI_VALUE) {
    level = 3;
  } else {
    level = (it.rssi - MIN_RSSI_VALUE) / step;
  }

  int len = strlen(it.ssid);
  QRectF iconRect = QRect(rect.right() - (ICON_BACK_AMAGE + 64),
    rect.top() + 17, 64, 64);
  QRectF nameRect = QRect(rect.left() + ROW_PITCH,
    rect.top() + 18, len * 38, 62);

  painter->setPen(QPen(Qt::white));
  if (0 == strcmp(it.ssid, cur_cn->ssid)) {
    wifi_state state = mgr->get_wifi_state();
    QRectF stateRect = QRect(rect.left() + ROW_PITCH,
      rect.top() + 4, 380, 30);
    painter->setFont(QFont("SourceHanSansCN-Normal", 24));
#if 0
    if (WIFI_STATION_CONNECTED == state) {
      painter->drawText(stateRect, tr("已连接"));
    } else if (WIFI_STATION_CONNECTING == state) {
      painter->drawText(stateRect, tr("正在连接"));
    } else if (WIFI_STATION_DISCONNECTING == state) {
      painter->drawText(stateRect, tr("正在断开"));
    } else if (WIFI_STATION_DISCONNECTED == state) {
      painter->drawText(stateRect, tr("已断开"));
    } else if (WIFI_STATION_AUTHENTICATING_ERROR == state) {
      painter->drawText(stateRect, tr("密码错误"));
    } else if (WIFI_STATION_UNAVAILABLE == state) {
      painter->drawText(stateRect, tr("网络不可达"));
    } else if (WIFI_STATION_FAILED == state) {
      painter->drawText(stateRect, tr("连接失败"));
    }
#endif
    painter->drawText(stateRect, mgr->get_state_notes(state));
    nameRect = QRect(rect.left() + ROW_PITCH,
        rect.top() + 32, len * 38, 54);
    painter->setPen(QPen(QColor("#00d5cf")));
  }

  if (len < 28)
    painter->setFont(QFont("SourceHanSansCN-Normal", 38));
  else {
    nameRect = QRect(rect.left() + ROW_PITCH,
      rect.top() + 32, len * 32, 54);
    painter->setFont(QFont("SourceHanSansCN-Normal", 32));
  }
  painter->drawText(nameRect, it.ssid);
  painter->drawImage(iconRect,
    QImage(WIFI_RSSI[it.security>0?1:0][level]));
}

QSize ui_wifi_list::ui_list_item_delegate::sizeHint(
const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  Q_UNUSED(index)

  return QSize(option.rect.width(), ROW_WIDTH);
}

void ui_wifi_list::slot_for_mainui(int event, int value) {
  ALOGD("[ui_wifi_list] %s:%d >> event=%d, value=%d",
    __func__, __LINE__, event, value);
  if (-1 == event) {
    if (1 == value)
      test_wifi();
    else if (2 == value)
      ui_rotate_management::getInstance()->back_activity_win();
    else if (0 == value)
      ui_control_wakeup_screen();
  }
}

void ui_wifi_list::test_wifi() {
  ALOGD("%s:%d >> test wifi state.", __func__, __LINE__);
  ui_control_wakeup_screen();
  //slot_for_click_item row 0
  bool enabled = !mgr->is_wifi_enabled();
  mgr->set_wifi_enabled(enabled);
  mgr->save_config();
  if (!enabled)
    update_wifi_list_imp(false);
}

static void *test_proc(void *arg) {
  prctl(PR_SET_NAME, "test_wifi_th");
  ALOGD("%s:%d >> test thread start.", __func__, __LINE__);
  int rand_t = 0, cnt = 0, ret = 0;
  ui_wifi_list *ui_wifi = (ui_wifi_list *)arg;

  ui_wifi->sendSignalForMainUI(-1, 0);
  rand_t = rand()%3 + 1;
  ALOGD("%s:%d >> wait start sleep %ds.",
    __func__, __LINE__, rand_t);
  sleep(rand_t);

#if 0
  cnt = rand()%10 + 1;
  for (int i = 0; i < cnt; ++i) {
    ui_wifi->sendSignalForMainUI(-1, 1);
    rand_t = rand()%10 + 1;
    ALOGD("%s:%d >> wait click sleep %ds.",
      __func__, __LINE__, rand_t);
    sleep(rand_t);
  }
  ui_wifi->sendSignalForMainUI(-1, 2);
#endif
  cnt = rand()%20 + 1;
  wifi_management *mgr = wifi_management::getInstance();
  for (int i = 0; i < cnt; ++i) {
    if (rand()%2 && ui_wifi->get_wifi_list().count() > 0) {
      rand_t = rand()%TEST_LEN;
      ALOGD("%s:%d >> test ssid[%d]={%s, %s}.",
        __func__, __LINE__, rand_t, TEST_SSIDS[rand_t][0],
        TEST_SSIDS[rand_t][1]);
      wifi_dot dot;
      memset(&dot, 0, sizeof(dot));
      strncpy(dot.ssid, TEST_SSIDS[rand_t][0], WIFI_NAME_LEN);
      strncpy(dot.pwd, TEST_SSIDS[rand_t][1], WIFI_PWD_LEN);
      dot.security = SECURITY_WPA_PSK;
      if (mgr->get_alive_ssid().compare(TEST_SSIDS[rand_t][0]) == 0) {
        if (rand()%2) {
          /* disable_network */
          ALOGD("%s:%d >> disable_network ssid[%d]{%s, %s}",
            __func__, __LINE__, rand_t, TEST_SSIDS[rand_t][0],
            TEST_SSIDS[rand_t][1]);
          ret = mgr->disable_network(&dot);
          ALOGD("%s:%d >> disable_network ssid[%d]{%s, %s} = %d",
            __func__, __LINE__, rand_t, TEST_SSIDS[rand_t][0],
            TEST_SSIDS[rand_t][1], ret);
        } else {
          /* remove_network */
          ALOGD("%s:%d >> remove_network ssid[%d]{%s, %s}",
            __func__, __LINE__, rand_t, TEST_SSIDS[rand_t][0],
            TEST_SSIDS[rand_t][1]);
          ret = mgr->remove_network(&dot);
          ALOGD("%s:%d >> remove_network ssid[%d]{%s, %s} = %d",
            __func__, __LINE__, rand_t, TEST_SSIDS[rand_t][0],
            TEST_SSIDS[rand_t][1], ret);
        }
      } else {
        /* sync_connect_network */
        ALOGD("%s:%d >> connect ssid[%d]{%s, %s}",
          __func__, __LINE__, rand_t, TEST_SSIDS[rand_t][0],
          TEST_SSIDS[rand_t][1]);
        ret = mgr->sync_connect_network(&dot);
        ALOGD("%s:%d >> connect ssid[%d]{%s, %s} = %d",
          __func__, __LINE__, rand_t, TEST_SSIDS[rand_t][0],
          TEST_SSIDS[rand_t][1], ret);
      }
      usleep(100*1000);
    } else {
      ui_wifi->sendSignalForMainUI(-1, 1);
      rand_t = rand()%10 + 1;
      ALOGD("%s:%d >> wait click sleep %ds.", __func__, __LINE__, rand_t);
      sleep(rand_t);
    }
  }
  ui_wifi->sendSignalForMainUI(-1, 2);
  ALOGD("%s:%d >> test thread end.", __func__, __LINE__);
  return NULL;
}

void ui_wifi_list::debug_test_wifi() {
  ALOGD("%s:%d", __func__, __LINE__);
  pthread_t test_th;
  if (pthread_create(&test_th, NULL, test_proc, (void *)this)) {
    ALOGE("create test wifi thread failed, errno: %d (%s)",
      errno, strerror(errno));
  }
}

void ui_wifi_list::radio_callback(void *id, int count,
  bool isCancel) {
  if (!ssid.isEmpty()) {
    if (!isCancel && mgr->is_wifi_enabled()) {
      wifi_dot dot;
      memset(&dot, 0, sizeof(wifi_dot));
      mgr->get_wifi_dot(&dot, ssid);
      if (0 == count) {
        mgr->disable_network(&dot);
      } else {
        mgr->remove_network(&dot);
      }
    }
    ssid.clear();
  }
}

