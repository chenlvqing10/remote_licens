/*
* ui_face_scan.cpp - define face preview function of main
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
#define LOG_TAG   "face_scan"
#include <log/log.h>
#include "time_conv.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QWSServer>
#include <QPainter>
#include <QDebug>
#include <QFontMetrics>
#include <QSqlDatabase>
#include <QFile>
#include <QTime>
#include <QDir>

#include "ui_face_scan.hpp"
#include "ui_ui_face_scan.h"
#include "ui_ui_face_scan_v.h"
#include "QMouseEvent"
#include "ui_password_login.hpp"
#include "roll_label.hpp"

#define FACE_LINE_SIZE    5
#define NAME_FONT_SIZE    50
#define NAME_MARGIN       10
#define NAME_HEIGHT       75
#define NAME_WIDTH        400
#define DEBUG_TIME_WIDTH  250
#define HIDE_CURSOR_COUNTER  10

#ifndef PORTRAIT_SCREEN
#define MAIN_AREA_X 457
#define MAIN_AREA_Y 80
#define MAIN_AREA_W 360
#define MAIN_AREA_H 520
#else
#define MAIN_AREA_X 150
#define MAIN_AREA_Y 180
#define MAIN_AREA_W 500
#define MAIN_AREA_H 664
#endif

#define MENU_COLUM_CNT  2
#define MENU_IMD_ROW_CNT  4
#define MENU_IMD_COLUM_CNT  8
#define MENU_ICOM_WIDTH  2

#define ORDER_ROW_COLUMN(j, r, c)  \
  do{ r = j/MENU_COLUM_CNT; c = j%MENU_COLUM_CNT; }while(0)
#define ICON_R(j)  { MENU_IMD_ROW_CNT * j }
#define ICON_C(j)  { MENU_IMD_COLUM_CNT * j }
#define CONTEND_R(r)  { MENU_IMD_ROW_CNT * r + 1 }
#define CONTEND_C(c)  { MENU_IMD_COLUM_CNT * c + MENU_ICOM_WIDTH }

#define DISPLAY_QRCODE_INTERVAL_TIME 300
#define TEMP_DET  1

static QRect RECT_SCREEN(0, 0, 720, 1280);

bool ui_face_scan::isFaceServiceWorking = false;

void ui_face_scan::ui_init() {
  state_menu = {
    {":icon/name_icon.png", tr("设备名称"), "亿智电子", EN_ITEM_DEV},
    {":icon/mac_icon.png", tr("MAC地址"), "86:f2:7a:19:96:fd", EN_ITEM_MAC},
    {":icon/reco_cnt_icon.png", tr("注册人数"), "200", EN_ITEM_CNT},
    {":icon/ip_icon.png", tr("IP地址"), "192.168.19.100", EN_ITEM_IP}
  };
}

ui_face_scan::ui_face_scan(QWidget* parent) :
    QtMainWindow(parent),
    rec_time_len_(0),
    is_display_qrcode(0),
    qrcode_disp_remain_time(0) {
#ifndef PORTRAIT_SCREEN
  ui = new Ui::ui_face_scan;
#else
  ui = new Ui::ui_face_scan_v;
#endif
  rotate = ui_rotate_management::getInstance();
  rotate->is_face_det_start  = true;
  m_params = rotate->get_main_param();
  ui->setupUi(this);
#ifndef PORTRAIT_SCREEN
  ui->menu_login_btn->close();
  ui->centralwidget_1->hide();
#endif
  ui_init();
  state_map[EN_ITEM_MAC] = -1;
  state_map[EN_ITEM_CNT] = -1;
  state_map[EN_ITEM_IP] = -1;

  this->setWindowFlags(Qt::FramelessWindowHint);
  this->setAttribute(Qt::WA_TranslucentBackground);
  QWSServer::setBackground(QColor(0, 0, 0, 0));

  connect(ui->menu_login_btn, SIGNAL(clicked()),
          this, SLOT(slot_show_password_login()));

  signal_t = reinterpret_cast<SignalTable*>(signal_get_handle());

  connect(signal_t, SIGNAL(signalForRecgNotes(int, QString, QString)),
          this, SLOT(set_recg_result(int, QString, QString)));

  connect(signal_t, SIGNAL(signalForDraw()),
          this, SLOT(slot_for_draw()));

  connect(signal_t, SIGNAL(signalForQrcode(QString)),
          this, SLOT(set_qrcode_result(QString)));

  font.setFamily("SourceHanSansCN-Normal");
  font.setBold(false);
  font.setPixelSize(NAME_FONT_SIZE);
  painter = new QPainter;

  // timer of updating time
  timer = new QTimer;
  connect(timer, SIGNAL(timeout()), this, SLOT(slot_for_updatetime()));
  timer->start(1000);

  // ![debug mode code]
  is_record = false;
  ui->rec_time_label->setAlignment(Qt::AlignCenter);
  ui->rec_time_label->hide();
  ui->rec_time_label->setStyleSheet("color:red;");
  if (m_params->debug_switch.debug_switch.debug_for_record) {
    ui->record_label->setText(tr("Rec Start"));
    ui->record_label->show();
  } else {
    ui->record_label->hide();
  }
  ui->qr_label->setStyleSheet(
    "font-size:24px;color:rgb(0, 255, 255);");
  connect(ui->record_label, SIGNAL(clicked()),
          this, SLOT(slot_for_record()));
  if (m_params->debug_switch.debug_switch.debug_for_isp) {
    ui->rgb_label->setStyleSheet(
        "font-size:24px;color:rgb(0, 255, 0);");
    ui->nir_label->setStyleSheet(
        "font-size:24px;color:rgb(255, 255, 0);");
    ui->rgb_label->show();
    ui->nir_label->show();
  } else {
    ui->rgb_label->hide();
    ui->nir_label->hide();
  }

  connect(ui->test_log_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_test_log()));
  if (m_params->debug_switch.debug_switch.debug_for_time) {
    int test_log;
    face_get_test_log_param(&test_log);
    if (test_log)
      ui->test_log_btn->setText(tr("Log Stop"));
    else
      ui->test_log_btn->setText(tr("Log Start"));
    ui->test_log_btn->show();
  } else {
    ui->test_log_btn->hide();
  }

  connect(ui->capture_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_capture()));
  ui->capture_btn->setText(tr("Capture"));
  if (m_params->debug_switch.debug_switch.debug_for_record)
    ui->capture_btn->show();
  else
    ui->capture_btn->hide();
  // ![debug mode code]

  ui->eth_status_label->hide();
  ui->wifi_status_label->hide();
  ui->usb_status_label->hide();
  ui->user_name_label->hide();
  ui->user_state_label->hide();
  ui->user_name_label->setAlignment(Qt::AlignCenter);
  ui->user_state_label->setAlignment(Qt::AlignCenter);

  if (!isFaceServiceWorking && ui_control_server_running()) {
    ui->temp_box->hide();
    ui->ble_qrcode_btn->hide();
    ui->ble_qrcode_label->hide();
    set_recg_result(1, QString(), QString(tr("AI智能启动中...")));
    border_color = QColor(QString("#ffffff"));
    slot_for_dev_state();
    QTimer::singleShot(100, this, SLOT(set_starting_service()));
  } else {
    isFaceServiceWorking = true;
    if (TEMP_DET != m_params->recg_params.body_temperature_detect)
      ui->temp_box->hide();
    border_color = QColor(m_params->color);
#ifdef PORTRAIT_SCREEN
    ui->menu_login_btn->setPixmap(m_params->logo);
    ui->company_label->setText(m_params->company);
#endif
    set_recg_result(0, QString(), QString());
    update_icon_status();
  }

  this->face = NULL;
  counter_for_menu = 0;
  is_show_menu = false;
  ui_control_face_resume();
  slot_for_updatetime();
  set_qrcode_result(QString());
  connect(eth_management::getInstance(),
    SIGNAL(signal_eth_state_change(int)), this,
    SLOT(slot_for_eth_state(int)));
  connect(wifi_management::getInstance(),
    SIGNAL(signal_wifi_state_change(int)), this,
    SLOT(slot_for_wifi_state(int)));

  this->showMaximized();
}

void ui_face_scan::set_starting_service() {
  rotate->update_management();
  if (TEMP_DET == m_params->recg_params.body_temperature_detect) {
    ui->temp_box->show();
  }
  border_color = QColor(m_params->color);
#ifdef PORTRAIT_SCREEN
  ui->menu_login_btn->setPixmap(m_params->logo);
  ui->company_label->setText(m_params->company);
#endif
  update_icon_status();
}

void ui_face_scan::slot_for_eth_state(int state) {
  ALOGD("%s:%d >> recv eth state change:%d", __func__, __LINE__, state);
  update_icon_status();
}

void ui_face_scan::slot_for_wifi_state(int state) {
  ALOGD("%s:%d >> recv wifi state change:%d", __func__, __LINE__, state);
  update_icon_status();
}

void ui_face_scan::update_icon_status() {
  /* check websocket link status */
  if (rotate->get_ws_status() == 1) {
    // ui->ble_qrcode_btn->hide();
    // ui->ble_qrcode_label->hide();
    is_display_qrcode = 1;
    qrcode_disp_remain_time = 0;
  } else {
    mode_ui_param_t param;
    memset(&param, 0, sizeof(mode_ui_param_t));
    // db_mode_ui_param_query(&param);
    ui_control_get_current_mode(&param);
    /* check debug mode if not open or local mode */
#if ATTENDANCE_MODE != 2
    ui->ble_qrcode_btn->hide();
    ui->ble_qrcode_label->hide();
#else
    if ((m_params->debug_switch.debug_switch.debug_for_status == 1)) {
      ui->ble_qrcode_btn->hide();
      ui->ble_qrcode_label->hide();
    } else {
      // ui->ble_qrcode_btn->show();
      // ui->ble_qrcode_label->show();
      is_display_qrcode = 0;
      QIcon icon;
      QString wechat_url(":/icon/ble.bmp");
      icon.addFile(wechat_url);
      ui->ble_qrcode_btn->setIcon(icon);
      ui->ble_qrcode_btn->setIconSize(QSize(243, 243));
      ui->ble_qrcode_label->setText(tr("请扫描二维码配置WIFI"));
    }
#endif
  }

  if (ETH_LINK_UP <=
    eth_management::getInstance()->get_eth_state()) {
    ui->wifi_status_label->hide();
    ui->eth_status_label->show();
  } else if (WIFI_STATION_CONNECTED ==
    wifi_management::getInstance()->get_wifi_state()) {
    ui->eth_status_label->hide();
    ui->wifi_status_label->show();
  } else {
    ui->eth_status_label->hide();
    ui->wifi_status_label->hide();
  }

  /* check udisk link status */
  if (rotate->get_usb_status() == 1)
    ui->usb_status_label->show();
  else
    ui->usb_status_label->hide();
#ifdef PORTRAIT_SCREEN
  /* update ethernet status */
  rotate->update_net_state();
  slot_for_dev_state();
#endif
}

void ui_face_scan::slot_for_dev_state() {
  QLabel *icons;
  QLabel *value;
  int row, column;

  if (ui->msg_layout->count() &&
    ((m_params->isMac && -1 != state_map[EN_ITEM_MAC]) ||
    (!m_params->isMac && -1 == state_map[EN_ITEM_MAC])) &&
    ((m_params->isCnt && -1 != state_map[EN_ITEM_CNT]) ||
    (!m_params->isCnt && -1 == state_map[EN_ITEM_CNT])) &&
    ((m_params->isIp && -1 != state_map[EN_ITEM_IP]) ||
    (!m_params->isIp && -1 == state_map[EN_ITEM_IP]))) {
#if 0
      for (int i = 0; i < ui->msg_layout->count(); ++i) {
        QLayoutItem *item = ui->msg_layout->itemAt(i);
        ALOGE("%s:%d >> %d:%s", __func__, __LINE__, i,
        ((QLabel *)item->widget())->text().toUtf8().data());
      }
#endif

    if (QString::compare(state_menu[EN_ITEM_DEV].value, m_params->name)) {
      state_menu[EN_ITEM_DEV].value = m_params->name;
      value = (QLabel *)ui->msg_layout->itemAt(0)->widget();
      value->setText(state_menu[EN_ITEM_DEV].value);
    }

    if (m_params->isMac &&
      QString::compare(state_menu[EN_ITEM_MAC].value, m_params->mac)) {
      state_menu[EN_ITEM_MAC].value = m_params->mac;
      value = (QLabel *)ui->msg_layout->itemAt(
        state_map[EN_ITEM_MAC])->widget();
      value->setText(state_menu[EN_ITEM_MAC].value);
    }

    if (m_params->isCnt &&
      QString::compare(state_menu[EN_ITEM_CNT].value, m_params->cnt)) {
      state_menu[EN_ITEM_CNT].value = m_params->cnt;
      value = (QLabel *)ui->msg_layout->itemAt(
        state_map[EN_ITEM_CNT])->widget();
      value->setText(state_menu[EN_ITEM_CNT].value);
    }

    if (m_params->isIp &&
      QString::compare(state_menu[EN_ITEM_IP].value, m_params->ip)) {
      state_menu[EN_ITEM_IP].value = m_params->ip;
      value = (QLabel *)ui->msg_layout->itemAt(
        state_map[EN_ITEM_IP])->widget();
      value->setText(state_menu[EN_ITEM_IP].value);
    }
  } else {
    clear_layout(ui->msg_layout);
    state_map[EN_ITEM_MAC] = -1;
    state_map[EN_ITEM_CNT] = -1;
    state_map[EN_ITEM_IP] = -1;
    state_menu[EN_ITEM_DEV].value = m_params->name;
    state_menu[EN_ITEM_MAC].value = m_params->mac;
    state_menu[EN_ITEM_CNT].value = m_params->cnt;
    state_menu[EN_ITEM_IP].value = m_params->ip;
    for (int i = 0, j = 0; i < EN_ITEM_COUNT ; ++i) {
      if ((EN_ITEM_MAC == i && !m_params->isMac) ||
        (EN_ITEM_CNT == i && !m_params->isCnt) ||
        (EN_ITEM_IP == i && !m_params->isIp))
        continue;
      icons = new QLabel(this);
      icons->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
      icons->setStyleSheet(QString("max-height:110px;\
        font-size:16px; margin-right:6px;\
        background-repeat:repeat; background-position:center;\
        background-image:url(%1)")
        .arg(state_menu[i].icon));
      icons->setText(state_menu[i].title);

      if (EN_ITEM_DEV == i &&
        state_menu[i].value.count() > 12) {
        value = new roll_Label(this);
      } else {
        value = new QLabel(this);
      }
      value->setText(state_menu[i].value);
      value->setStyleSheet("color:#FFF;font-size:24px;");

      ORDER_ROW_COLUMN(j, row, column);

      ui->msg_layout->addWidget(icons, ICON_R(row), ICON_C(column),
        4, MENU_ICOM_WIDTH);
      ui->msg_layout->addWidget(value, CONTEND_R(row), CONTEND_C(column),
        2, MENU_IMD_COLUM_CNT-MENU_ICOM_WIDTH);
      state_map[i] = ui->msg_layout->count()-1;
      j++;
    }
  }
}

void ui_face_scan::clear_layout(QLayout *layout) {
  QLayoutItem *item;
  while ((item = layout->takeAt(0)) !=0) {
    if (item->widget()) {
      delete item->widget();
    }
    if (item->layout()) {
      clear_layout(item->layout());
    }
    delete item;
  }
}

void ui_face_scan::update_temp_state(face_t *face) {
  if (NULL == face || face->rect_smooth[0] < MAIN_AREA_X ||
    face->rect_smooth[2] > MAIN_AREA_X + MAIN_AREA_W ||
    face->rect_smooth[1] < MAIN_AREA_Y ||
    face->rect_smooth[3] > MAIN_AREA_Y + MAIN_AREA_H) {
    if (ui->temp_box->isChecked())
      ui->temp_box->setCheckState(Qt::Unchecked);
  } else {
    if (!ui->temp_box->isChecked())
      ui->temp_box->setCheckState(Qt::Checked);
  }
}

void ui_face_scan::set_recg_result(int is_pass,
  QString name, QString statu) {
  static int t_pass = 0;
  static QString t_name;
  static QString t_statu;
  ALOGD("[%s:%d] is_pass: %d, name: %s, statu: %s",
    __func__, __LINE__, is_pass, name.toUtf8().data(), statu.toUtf8().data());

  if (name.compare(t_name) != 0 || statu.compare(t_statu) != 0) {
    if (name.isEmpty()) {
      if (!statu.isEmpty()) {
        ui->user_state_label_2->setText(statu);
        ui->stackedWidget->setCurrentIndex(1);
      }
    } else {
      ui->user_name_label->setText(name);
      ui->user_state_label->setText(statu);
      ui->user_name_label->show();
      ui->user_state_label->show();
      ui->stackedWidget->setCurrentIndex(0);
    }
  }

  if (2 == is_pass ||
    (0 == is_pass && name.isEmpty() && statu.isEmpty())) {
    ui->stackedWidget->hide();
  } else {
    ui->stackedWidget->show();
  }

  if (1 == is_pass) {
    if (t_pass != is_pass) {
      ui->user_name_label->setStyleSheet("background-color:#00d2cc;");
      ui->user_state_label->setStyleSheet("background-color:#00d2cc;");
      ui->user_state_label_2->setStyleSheet("background-color:#00d2cc;");
      ui->name_state_frame->setStyleSheet("background-color:#00d2cc;");
      ui->line->setStyleSheet("background:rgba(255,255,255,50%);");
    }
  } else {
    if (t_pass != is_pass) {
      ui->user_name_label->setStyleSheet("background-color:#f2313f;");
      ui->user_state_label->setStyleSheet("background-color:#f2313f;");
      ui->user_state_label_2->setStyleSheet("background-color:#f2313f;");
      ui->name_state_frame->setStyleSheet("background-color:#f2313f;");
      ui->line->setStyleSheet("background:rgba(255,255,255,50%);");
    }
  }
  t_pass = is_pass;
  t_name = name;
  t_statu = statu;
}

void ui_face_scan::set_qrcode_result(QString data) {
  if (!data.isEmpty()) {
    ui->qr_label->wrapData(data, 200);
    ui->qr_label->show();
  } else {
    ui->qr_label->hide();
    ui->qr_label->setText("");
  }
}

QRegion ui_face_scan::cal_min_region(faces_t* face) {
  int i;
  QRect face_rect, name_rect, debug_time_rect;
  QRegion region = this->last_draw_region;
  QRegion tmp_region;

  if (face == NULL || face->face_num == 0) {
    this->last_draw_region = tmp_region;
    return region;
  }

  for (i = 0; i < face->face_num; i++) {
    face_rect = QRect(face->faces[i].rect[0] - FACE_LINE_SIZE,
                      face->faces[i].rect[1] - FACE_LINE_SIZE,
                      face->faces[i].rect[2] - face->faces[i].rect[0]
                      + FACE_LINE_SIZE * 2,
                      face->faces[i].rect[3] - face->faces[i].rect[1]
                      + FACE_LINE_SIZE * 2);

    if (face->faces[i].rect[1] < (NAME_HEIGHT + NAME_MARGIN))
      name_rect = QRect(face->faces[i].rect[0] - FACE_LINE_SIZE,
                        face->faces[i].rect[3],
                        NAME_WIDTH + 2 * FACE_LINE_SIZE, NAME_HEIGHT);
    else
      name_rect = QRect(face->faces[i].rect[0] - FACE_LINE_SIZE,
                        face->faces[i].rect[1] - NAME_MARGIN - NAME_HEIGHT,
                        NAME_WIDTH + 2 * FACE_LINE_SIZE, NAME_HEIGHT);

    region = region.united(face_rect);
    region = region.united(name_rect);
    tmp_region = tmp_region.united(face_rect);
    tmp_region = tmp_region.united(name_rect);
  }

  /* debug mode region */
  if (m_params->debug_switch.debug_switch.debug_for_time) {
    for (i = 0; i < face->face_num; i++) {
      face_rect = QRect(face->faces[i].nir_rect[0] - FACE_LINE_SIZE,
                        face->faces[i].nir_rect[1] - FACE_LINE_SIZE,
                        face->faces[i].nir_rect[2]
                        - face->faces[i].nir_rect[0] + FACE_LINE_SIZE * 2,
                        face->faces[i].nir_rect[3]
                        - face->faces[i].nir_rect[1] + FACE_LINE_SIZE * 2);

      region = region.united(face_rect);
      tmp_region = tmp_region.united(face_rect);

      debug_time_rect = QRect(face->faces[i].rect[0] - FACE_LINE_SIZE,
                              face->faces[i].rect[1] - FACE_LINE_SIZE,
                              DEBUG_TIME_WIDTH, 2 * NAME_HEIGHT);

      region = region.united(debug_time_rect);
      tmp_region = tmp_region.united(debug_time_rect);
    }
  }

  this->last_draw_region = tmp_region;

  return region;
}

void ui_face_scan::slot_for_draw() {
  QRegion region;
  if (!isFaceServiceWorking) {
    isFaceServiceWorking = true;
    set_recg_result(0, QString(), QString());
  }

  /* Wait for completion of last drawing */
  /* ALOGD("slot_for_draw %08x time:%lld\n", this->face, time_get_us()); */
  if (this->face != NULL)
    return;

  this->face = ui_get_faces();
  if (TEMP_DET == m_params->recg_params.body_temperature_detect) {
    if (face->face_num > 0)
      update_temp_state(&(face->faces[0]));
    else
      update_temp_state(NULL);
  }
  if (this->face != NULL && this->face->face_num > 0)
    region = cal_min_region(this->face);
  else
    region = this->last_draw_region;

  if (region.isEmpty()) {
    ALOGD("Refresh region is empty, refresh full screen!");
    region = region.united(RECT_SCREEN);
  }
  this->repaint(region);
}

ui_face_scan::~ui_face_scan() {
  ALOGD("delete ui_face_scan");
  if (is_record) {
    ui_control_record_stop();
  }
  delete timer;
  delete painter;
  delete ui;
}

void ui_face_scan::deinit() {
  QtMainWindow::deinit();

  disconnect(signal_t, SIGNAL(signalForDraw()),
               this, SLOT(slot_for_draw()));

  ALOGD("ui_face_scan::deinit() is_wait_clock_win_opened()");
  if (rotate->is_wait_clock_win_opened() == 0) {
    ALOGD("ui_face_scan::deinit() ui_control_face_pause()");
    ui_control_face_pause();
  }
  rotate->is_face_det_start  = false;
}

#ifndef PORTRAIT_SCREEN
void ui_face_scan::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    if (is_show_menu) {
      ui->menu_login_btn->hide();
      is_show_menu = false;
    } else {
      ui->menu_login_btn->show();
      is_show_menu = true;
      counter_for_menu = 10;
    }
  }
}
#endif

void ui_face_scan::draw_face_sim(int left, int top,
  int right, int bottom) {
  int width = right - left;
  int height = bottom - top;

  painter->setPen(QPen(border_color, FACE_LINE_SIZE));

  // left-top
  painter->drawLine(QPoint(left, top), QPoint(left + width / 8, top));
  painter->drawLine(QPoint(left, top), QPoint(left, top + height / 8));
  // right-top
  painter->drawLine(QPoint(right, top), QPoint(right - width / 8, top));
  painter->drawLine(QPoint(right, top), QPoint(right, top + height / 8));
  // left-bottom
  painter->drawLine(QPoint(left, bottom), QPoint(left + width / 8, bottom));
  painter->drawLine(QPoint(left, bottom), QPoint(left, bottom - height / 8));
  // right-bottom
  painter->drawLine(QPoint(right, bottom), QPoint(right - width / 8, bottom));
  painter->drawLine(QPoint(right, bottom), QPoint(right, bottom - height / 8));
}

void ui_face_scan::draw_face(int left, int top, int right,
                              int bottom, int type, QString name) {
  int width = right - left;
  int height = bottom - top;

  if (type == 1) {
    painter->setPen(QPen(Qt::green, FACE_LINE_SIZE));
  } else if (type == 0) {
    painter->setPen(QPen(Qt::red, FACE_LINE_SIZE));
  } else {
    painter->setPen(QPen(Qt::yellow, FACE_LINE_SIZE));
  }
  // left-top
  painter->drawLine(QPoint(left, top), QPoint(left + width / 8, top));
  painter->drawLine(QPoint(left, top), QPoint(left, top + height / 8));
  // right-top
  painter->drawLine(QPoint(right, top), QPoint(right - width / 8, top));
  painter->drawLine(QPoint(right, top), QPoint(right, top + height / 8));
  // left-bottom
  painter->drawLine(QPoint(left, bottom), QPoint(left + width / 8, bottom));
  painter->drawLine(QPoint(left, bottom), QPoint(left, bottom - height / 8));
  // right-bottom
  painter->drawLine(QPoint(right, bottom), QPoint(right - width / 8, bottom));
  painter->drawLine(QPoint(right, bottom), QPoint(right, bottom - height / 8));
  // name
  QFontMetrics fm = painter->fontMetrics();
  if (top < (NAME_HEIGHT + NAME_MARGIN))
    painter->drawText(QRect(left, bottom, NAME_WIDTH,
      fm.ascent() + fm.descent()), name);
  else
    painter->drawText(QRect(left, top - fm.ascent()
                        - fm.descent() - NAME_MARGIN,
                        NAME_WIDTH, fm.ascent() + fm.descent()), name);
}

void ui_face_scan::draw_debug_info() {
  char info[18];
  int timeout = 0;
  int64_t diff_ts, cur_ts = time_get_us();
  int recg_timeout = face_get_recg_timeout() * 1000;

  for (int i = 0; i < face->face_num; i++) {
    if (cur_ts - face->faces[i].det_ts_begin > recg_timeout)
      timeout = 1;
    /* draw nir rect */
    draw_face(face->faces[i].nir_rect[0], face->faces[i].nir_rect[1],
              face->faces[i].nir_rect[2],
              face->faces[i].nir_rect[3], 2, QString(" "));

    if (face->faces[i].recg_ts_end) {
      diff_ts = face->faces[i].recg_ts_end - face->faces[i].det_ts_begin;
      if (diff_ts > recg_timeout)
        snprintf(info, sizeof(info), "%s", "R:timeout  ");
      else
        snprintf(info, sizeof(info), "R:%2.3f",
                 static_cast<double>(diff_ts) / 1000000);
      painter->drawText(QPoint(face->faces[i].rect[0], face->faces[i].rect[1]
                               + NAME_MARGIN + NAME_FONT_SIZE), QString(info));
    } else if (timeout) {
      snprintf(info, sizeof(info), "%s", "R:timeout  ");
      painter->drawText(QPoint(face->faces[i].rect[0], face->faces[i].rect[1]
                               + NAME_MARGIN + NAME_FONT_SIZE), QString(info));
    }

    if (face->faces[i].living_ts_end) {
      diff_ts = face->faces[i].living_ts_end - face->faces[i].det_ts_begin;
      if (diff_ts > recg_timeout)
        snprintf(info, sizeof(info), "%s", "L:timeout");
      else
        snprintf(info, sizeof(info), "L:%2.3f",
                 static_cast<double>(diff_ts) / 1000000);
      painter->drawText(QPoint(face->faces[i].rect[0], face->faces[i].rect[1]
                        + 2 * (NAME_MARGIN + NAME_FONT_SIZE)), QString(info));
    } else if (timeout) {
      snprintf(info, sizeof(info), "%s", "L:timeout");
      painter->drawText(QPoint(face->faces[i].rect[0], face->faces[i].rect[1]
                        + 2 * (NAME_MARGIN + NAME_FONT_SIZE)), QString(info));
    }
  }
}

void ui_face_scan::paintMoreInfo() {
  faces_t* face = this->face;
  QString name;
  int fontWidth;

  painter->begin(this);
  painter->setFont(font);
  QFontMetrics fm = painter->fontMetrics();

  for (int i = 0; i < face->face_num; i++) {
#if 0
    ALOGE("%s:%d > %s > %d > %d > %d", __func__, __LINE__,
      face->faces[i].name, m_params->recg_params.living_nir,
      face->faces[i].recg, face->faces[i].living);
#endif
    if (m_params->recg_params.living_nir) {
      if (face->faces[i].recg) {
        name = QString::fromUtf8(face->faces[i].name);
        fontWidth = fm.width(name);
        if (fontWidth > NAME_WIDTH)
          name = fm.elidedText(name, Qt::ElideRight, NAME_WIDTH);
        if (face->faces[i].living) {
          draw_face(face->faces[i].rect[0], face->faces[i].rect[1],
                face->faces[i].rect[2], face->faces[i].rect[3], 1, name);
        } else {
          draw_face(face->faces[i].rect[0], face->faces[i].rect[1],
                face->faces[i].rect[2], face->faces[i].rect[3], 0, name);
        }
      } else {
        if (face->faces[i].living) {
#ifndef RELEASE
          draw_face(face->faces[i].rect[0], face->faces[i].rect[1],
                  face->faces[i].rect[2], face->faces[i].rect[3], 1,
                  QString(" "));
#else
          draw_face(face->faces[i].rect[0], face->faces[i].rect[1],
                  face->faces[i].rect[2], face->faces[i].rect[3], 0,
                  QString(" "));
#endif
        } else {
          draw_face(face->faces[i].rect[0], face->faces[i].rect[1],
                  face->faces[i].rect[2], face->faces[i].rect[3], 0,
                  QString(" "));
        }
      }
    } else {
      if (face->faces[i].recg) {
        name = QString::fromUtf8(face->faces[i].name);
        fontWidth = fm.width(name);
        if (fontWidth > NAME_WIDTH)
          name = fm.elidedText(name, Qt::ElideRight, NAME_WIDTH);
        draw_face(face->faces[i].rect[0], face->faces[i].rect[1],
                face->faces[i].rect[2], face->faces[i].rect[3], 1, name);
      } else {
        draw_face(face->faces[i].rect[0], face->faces[i].rect[1],
                face->faces[i].rect[2], face->faces[i].rect[3], 0,
                QString(" "));
      }
    }
  }
    /* show debug info */
  if (m_params->debug_switch.debug_switch.debug_for_time)
    draw_debug_info();
  painter->end();
}

void ui_face_scan::paintEvent(QPaintEvent* e) {
  faces_t* face = this->face;
  if (NULL == face) return;

#ifndef PORTRAIT_SCREEN
  paintMoreInfo();
#else
#if ATTENDANCE_MODE == 2
  paintMoreInfo();
#else
  if (m_params->debug_switch.debug_switch.debug_for_time)
    paintMoreInfo();
  else if (TEMP_DET != m_params->recg_params.body_temperature_detect) {
    painter->begin(this);
    painter->setFont(font);
    QFontMetrics fm = painter->fontMetrics();
    for (int i = 0; i < face->face_num; i++) {
      draw_face_sim(face->faces[i].rect[0], face->faces[i].rect[1],
        face->faces[i].rect[2], face->faces[i].rect[3]);
    }
    painter->end();
  }
#endif
#endif
  ui_put_faces(face);
  this->face = NULL;
}

void ui_face_scan::slot_for_updatetime() {
  char time_str[40];
  time_t cur_time;
  struct tm* tm;
  struct tm now_time;
  cur_time = time(NULL);
  tm = localtime_r(&cur_time, &now_time);
  // tm = localtime(&cur_time);
  strftime(time_str, sizeof(time_str), "%Y/%m/%d %H:%M", tm);
#ifndef PORTRAIT_SCREEN
  ui->dtime_show_label->setText(time_str);
#else
  ui->main_clock->setText(time_str);
#endif

  if (is_record) {
    rec_time_len_++;
    snprintf(time_str, sizeof(time_str), "%02d:%02d",
             rec_time_len_ / 60,
             rec_time_len_ % 60);
    ui->rec_time_label->setText(time_str);
  }

#ifndef PORTRAIT_SCREEN
  if (counter_for_menu != 0) {
    counter_for_menu--;
    if (counter_for_menu == 0) {
      ui->menu_login_btn->hide();
      is_show_menu = false;
    }
  }
#endif

  if (m_params->debug_switch.debug_switch.debug_for_isp == 1) {
    memset(&exif[0], 0, sizeof(viss_isp_exif));
    memset(&exif[1], 0, sizeof(viss_isp_exif));
    if (ui_control_get_isp_data(&exif[0], &exif[1]) < 0) {
      ALOGE("ui_control_get_isp_data failed\n");
      return;
    }
    /*foramt string*/
    isp_rgb_str = QString("rgb isp:\n"
                          "exposure_time=%1\n"
                          "brightness=%2\n"
                          "iso_speed=%3\n"
                          "back_score=%4\n"
                          "res0:%5\nres1:%6\nres2:%7\nres4:%8")
        .arg(exif[0].exposure_time)
        .arg(exif[0].brightness)
        .arg(exif[0].iso_speed)
        .arg(exif[0].back_score)
        .arg(exif[0].res[0])
        .arg(exif[0].res[1])
        .arg(exif[0].res[2])
        .arg(exif[0].res[4]);
    ui->rgb_label->setText(isp_rgb_str);
    isp_nir_str = QString("nir isp:\n"
                          "exposure_time=%1\n"
                          "brightness=%2\n"
                          "iso_speed=%3\n"
                          "back_score=%4\n"
                          "res0:%5\nres1:%6\nres2:%7\nres4:%8")
        .arg(exif[1].exposure_time)
        .arg(exif[1].brightness)
        .arg(exif[1].iso_speed)
        .arg(exif[1].back_score)
        .arg(exif[1].res[0])
        .arg(exif[1].res[1])
        .arg(exif[1].res[2])
        .arg(exif[1].res[4]);
    ui->nir_label->setText(isp_nir_str);
  }

  if (is_display_qrcode) {
    if (0 == qrcode_disp_remain_time) {
      qrcode_disp_remain_time = DISPLAY_QRCODE_INTERVAL_TIME;
      char path[256] = "/data/wechat.bmp";
      if (ui_get_qrcode_path(path) == 0) {
        ui->ble_qrcode_label->setText(tr("使用企业微信扫描录入"));
        #if 0
        ui->ble_qrcode_btn->setStyleSheet(
          "QPushButton{border-image:url(/data/wechat.bmp);"
          "background-color:rgb(255,255,255);}");
        #else
        QIcon icon;
        QString wechat_url(path);
        icon.addFile(wechat_url);
        ui->ble_qrcode_btn->setIcon(icon);
        ui->ble_qrcode_btn->setIconSize(QSize(234, 234));
        #endif
      }
    } else {
      qrcode_disp_remain_time--;
    }
  }
}

void ui_face_scan::slot_for_web_socket_status(int event) {
  switch (event) {
  case WEB_SOCKET_EVENT_LINKED:
    if (m_params->debug_switch.debug_switch.debug_for_status == 0) {
      ui->ble_qrcode_btn->hide();
      ui->ble_qrcode_label->hide();
      update_icon_status();
    }
    break;

  case WEB_SOCKET_EVENT_DIS_LINKED:
    if (m_params->debug_switch.debug_switch.debug_for_status == 0) {
      ui->ble_qrcode_btn->show();
      ui->ble_qrcode_label->show();
    }
    break;

  case WEB_SOCKET_EVENT_UNBIND:
#if 0
    rotate->slot_bind_choose();
    deinit();
    // emit deinit();
#endif
    break;

  default:
    break;
  }
}

void ui_face_scan::slot_for_storage(int event, int type) {
  if (event == STORAGE_EVENT_UDISK) {
    if (type == STORAGE_TYPE_CONNECT) {
      rotate->set_usb_status(1);
    }

    if (type == STORAGE_TYPE_DISCONN) {
      rotate->set_usb_status(0);
    }
  }
  update_icon_status();
}

void ui_face_scan::slot_show_password_login() {
  if (!isFaceServiceWorking) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST);
    w->set_content(tr("设备数据初始化中，暂停登录！").toUtf8().data());
    w->show();
    return;
  }
  rotate->start_activity_win(
    &ui_password_login::staticMetaObject);
}

void ui_face_scan::slot_for_record() {
  if (is_record == false) {
    rec_time_len_ = 0;
    ui->rec_time_label->setText("00:00");
    ui->rec_time_label->show();
    ui->record_label->setText(tr("Rec Stop"));
    ui_control_record_start();
    is_record =  true;
  } else {
    ui->rec_time_label->hide();
    ui->record_label->setText(tr("Rec Start"));
    ui_control_record_stop();
    is_record =  false;
  }
}

void ui_face_scan::slot_for_test_log() {
  int test_log;
  face_get_test_log_param(&test_log);

  test_log = !test_log;
  if (test_log) {
    ui->test_log_btn->setText(tr("Log Stop"));
  } else {
    ui->test_log_btn->setText(tr("Log Start"));
  }

  face_set_test_log_param(test_log);
}

void ui_face_scan::slot_for_capture() {
  char file_name[MAX_BUFFER_SIZE];
  char dir_name[MAX_BUFFER_SIZE] = {'\0'};

  QDir *dir = new QDir;
  snprintf(dir_name, sizeof(dir_name), "%s", "/data/face_record/capture");
  bool exist = dir->exists(dir_name);
  if (exist == false) {
    dir->mkdir(dir_name);
  }

  QDateTime time = QDateTime::currentDateTime();
  QString date_time = time.toString("yyyy-MM-dd-hh-mm-ss");

  memset(file_name, 0, sizeof(file_name));
  snprintf(file_name, sizeof(file_name),
       "/data/face_record/capture/%s_rgb.jpg", date_time.toLatin1().data());
  ui_control_snapshot(0, file_name);
  snprintf(file_name, sizeof(file_name),
       "/data/face_record/capture/%s_nir.jpg", date_time.toLatin1().data());
  ui_control_snapshot(1, file_name);
}

