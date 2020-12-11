/*
* ui_face_input.cpp - define face preview function of main
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
#define LOG_TAG   "ui"
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

#include "ui_face_input.hpp"
#include "ui_ui_face_input.h"
#include "ui_ui_face_input_v.h"
#include "ui_rotate_management.hpp"
#include "QMouseEvent"

#define FACE_LINE_SIZE    5
#define NAME_FONT_SIZE    50
#define NAME_MARGIN       10
#define NAME_HEIGHT       75
#define NAME_WIDTH        400
#define DEBUG_TIME_WIDTH  250
#define HIDE_CURSOR_COUNTER  10

#define DISPLAY_QRCODE_INTERVAL_TIME 300

static QRect RECT_SCREEN(0, 0, 720, 1280);

ui_face_input::ui_face_input(QWidget* parent) :
    QMainWindow(parent),
    rec_time_len_(0),
    is_display_qrcode(0),
    qrcode_disp_remain_time(0) {
#ifndef PORTRAIT_SCREEN
    ui = new Ui::ui_face_input;
#else
    ui = new Ui::ui_face_input_v;
#endif
  memset(&debug_switch, 0, sizeof(un_debug_switch_t));
  if (ui_control_get_debug_switch(&debug_switch) < 0) {
    ALOGE("ui_control_get_debug_switch failed\n");
  }
  memset(&p_info, 0, sizeof(algorithm_system_info_t));
  if (ui_get_algorithm_system_info(&p_info) < 0) {
    ALOGE("ui_get_algorithm_system_info failed\n");
  }

  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();
  rotate_signgleton->is_face_det_start  = true;
  ui->setupUi(this);
#ifndef PORTRAIT_SCREEN
  ui->menu_login_btn->close();
#endif
  // ui->face_input_listView->hide();
  ui->title_face_input_label->hide();
  this->setWindowFlags(Qt::FramelessWindowHint);
  this->setAttribute(Qt::WA_TranslucentBackground);
  QWSServer::setBackground(QColor(0, 0, 0, 0));
  connect(ui->menu_login_btn, SIGNAL(clicked()),
          this, SLOT(slot_show_password_login()));

  signal_t = reinterpret_cast<SignalTable*>(signal_get_handle());
#if 0
  connect(signal_t, SIGNAL(signalForNetStatus(int, int)),
          this, SLOT(slot_for_net_status(int, int)));
  connect(signal_t, SIGNAL(signalForWebSocket(int)),
          this, SLOT(slot_for_web_socket_status(int)));
  connect(signal_t, SIGNAL(signalForStorage(int, int)),
          this, SLOT(slot_for_storage(int, int)));
#endif
  connect(signal_t, SIGNAL(signalForDraw()),
          this, SLOT(slot_for_draw()));

  font.setFamily("SourceHanSansCN-Normal");
  font.setBold(false);
  font.setPixelSize(NAME_FONT_SIZE);
  painter = new QPainter;

  // timer of updating time
  timer = new QTimer;
  connect(timer, SIGNAL(timeout()), this, SLOT(slot_for_updatetime()));
  timer->start(1000);

  ui->ble_qrcode_btn->setStyleSheet(
    "QPushButton{border-image:url(:/icon/ble.bmp)}");

  ui->usb_status_label->setPixmap(QPixmap(":/icon/usb.png"));
  ui->eth_status_label->setPixmap(QPixmap(":/icon/network_setting_eth.png"));
  ui->wifi_status_label->setPixmap(QPixmap(":/icon/network_setting_wifi.png"));

  // ![debug mode code]
  is_record = false;
  ui->rec_time_label->setAlignment(Qt::AlignCenter);
  ui->rec_time_label->hide();
  ui->rec_time_label->setStyleSheet("color:red;");
  if (debug_switch.debug_switch.debug_for_record) {
    ui->record_label->setText(tr("Rec Start"));
    ui->record_label->show();
  } else {
    ui->record_label->hide();
  }
  connect(ui->record_label, SIGNAL(clicked()),
          this, SLOT(slot_for_record()));
  if (debug_switch.debug_switch.debug_for_isp) {
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
  if (debug_switch.debug_switch.debug_for_time) {
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
  if (debug_switch.debug_switch.debug_for_record)
    ui->capture_btn->show();
  else
    ui->capture_btn->hide();
  // ![debug mode code]

  ui->eth_status_label->hide();
  ui->wifi_status_label->hide();
  ui->usb_status_label->hide();

  update_icon_status();

  this->face = NULL;
  counter_for_menu = 0;
  is_show_menu = false;
  ui_control_face_resume();
  slot_for_updatetime();

  this->showMaximized();
}

void ui_face_input::update_icon_status() {
  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();

  /* check websocket link status */
  if (rotate_signgleton->get_ws_status() == 1) {
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
    if ((debug_switch.debug_switch.debug_for_status == 1)) {
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

  eth_management *eth_mgr = eth_management::getInstance();
  wifi_management *wifi_mgr = wifi_management::getInstance();
  if (ETH_LINK_UP == eth_mgr->get_eth_state()) {
    ui->wifi_status_label->hide();
    ui->eth_status_label->show();
  } else if (WIFI_STATION_CONNECTED == wifi_mgr->get_wifi_state()) {
    ui->eth_status_label->hide();
    ui->wifi_status_label->show();
  } else {
    ui->eth_status_label->hide();
    ui->wifi_status_label->hide();
  }

  /* check udisk link status */
  if (rotate_signgleton->get_usb_status() == 1)
    ui->usb_status_label->show();
  else
    ui->usb_status_label->hide();

#if 0
  /* check ethernet link status */
  if (rotate_signgleton->get_eth_status() == 1) {
    ui->eth_status_label->setPixmap(QPixmap(""));
    ui->eth_status_label->setPixmap(QPixmap(":/icon/network_setting_eth.png"));
    /* check udisk link status */
    ui->wifi_status_label->setPixmap(QPixmap(""));
    if (rotate_signgleton->get_usb_status() == 1) {
      ui->wifi_status_label->setPixmap(QPixmap(":/icon/usb.png"));
    }
  } else {
    /* check wifi link status */
    if (rotate_signgleton->get_wifi_status() == 1) {
      ui->eth_status_label->setPixmap(QPixmap(""));
      ui->eth_status_label->setPixmap(
            QPixmap(":/icon/network_setting_wifi.png"));
      /* check udisk link status */
      ui->wifi_status_label->setPixmap(QPixmap(""));
      if (rotate_signgleton->get_usb_status() == 1) {
        ui->wifi_status_label->setPixmap(QPixmap(":/icon/usb.png"));
      }
    } else {
      /* check udisk link status */
      ui->eth_status_label->setPixmap(QPixmap(""));
      if (rotate_signgleton->get_usb_status() == 1) {
        ui->eth_status_label->setPixmap(QPixmap(":/icon/usb.png"));
      }
      ui->wifi_status_label->setPixmap(QPixmap(""));
    }
  }
  ui->eth_status_label->show();
  ui->wifi_status_label->show();
  ui->usb_status_label->hide();
#endif
}

QRegion ui_face_input::cal_min_region(faces_t* face) {
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
  if (debug_switch.debug_switch.debug_for_time) {
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

void ui_face_input::slot_for_draw() {
  QRegion region;

  /* Wait for completion of last drawing */
  //ALOGD("slot_for_draw %08x time:%lld\n", this->face, time_get_us());

  if (this->face != NULL)
    return;

  this->face = ui_get_faces();

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

ui_face_input::~ui_face_input() {
  ALOGD("delete ui_face_input");
  if (is_record) {
    ui_control_record_stop();
  }
  delete timer;
  delete painter;
  delete ui;
}

void ui_face_input::deinit() {
  disconnect(signal_t, SIGNAL(signalForDraw()),
               this, SLOT(slot_for_draw()));
#if 0
  disconnect(signal_t, SIGNAL(signalForNetStatus(int, int)),
               this, SLOT(slot_for_net_status(int, int)));
#endif
  QtMainWindow::deinit();

  ui_rotate_management* rotate_signgleton =
      ui_rotate_management::getInstance();
  ALOGD("ui_face_input::deinit() is_wait_clock_win_opened()");
  if (rotate_signgleton->is_wait_clock_win_opened() == 0) {
    ALOGD("ui_face_input::deinit() ui_control_face_pause()");
    ui_control_face_pause();
  }
  rotate_signgleton->is_face_det_start  = false;
}

#ifndef PORTRAIT_SCREEN
void ui_face_input::mousePressEvent(QMouseEvent* event) {
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

void ui_face_input::draw_face(int left, int top, int right,
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

void ui_face_input::draw_debug_info() {
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

void ui_face_input::paintEvent(QPaintEvent* e) {
  faces_t* face = this->face;
  QString name;
  int fontWidth;
  if (NULL == face) return;
  /*QPixmap pixmap(size());
  painter->begin(&pixmap);*/
  painter->begin(this);
  painter->setFont(font);
  QFontMetrics fm = painter->fontMetrics();
  for (int i = 0; i < face->face_num; i++) {
    if (p_info.living_nir) {
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
  if (debug_switch.debug_switch.debug_for_time)
    draw_debug_info();

  ui_put_faces(face);
  this->face = NULL;
  painter->end();
/*
  painter->begin(this);
  painter->drawPixmap(0, 0, pixmap);
  painter->end();
*/
}

void ui_face_input::slot_for_updatetime() {
  char time_str[40];
  time_t cur_time;
  struct tm* tm;
  struct tm now_time;
  cur_time = time(NULL);
  tm = localtime_r(&cur_time, &now_time);
  // tm = localtime(&cur_time);
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm);
#ifndef PORTRAIT_SCREEN
  ui->dtime_show_label->setText(time_str);
#else
  QDateTime date_time = QDateTime::currentDateTime();
  QString curretn_time = date_time.toString("hh:mm:ss");
  QString curretn_date = QLocale()
        .toString(date_time, QString("yyyy/MM/dd dddd"));
  ui->dtime_show_label->setText(curretn_time);
  ui->date_show_label->setText(curretn_date);
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

  if (debug_switch.debug_switch.debug_for_isp == 1) {
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

void ui_face_input::slot_for_web_socket_status(int event) {
  ui_rotate_management *rotate_signgleton;
  switch (event) {
  case WEB_SOCKET_EVENT_LINKED:
    if (debug_switch.debug_switch.debug_for_status == 0) {
      ui->ble_qrcode_btn->hide();
      ui->ble_qrcode_label->hide();
      update_icon_status();
    }
    break;

  case WEB_SOCKET_EVENT_DIS_LINKED:
    if (debug_switch.debug_switch.debug_for_status == 0) {
      ui->ble_qrcode_btn->show();
      ui->ble_qrcode_label->show();
    }
    break;

  case WEB_SOCKET_EVENT_UNBIND:
#if 0
    rotate_signgleton = ui_rotate_management::getInstance();
    rotate_signgleton->slot_bind_choose();
    deinit();
    // emit deinit();
#endif
    break;

  default:
    break;
  }
}

void ui_face_input::slot_for_storage(int event, int type) {
  ui_rotate_management* rotate_signgleton =
    ui_rotate_management::getInstance();
  if (event == STORAGE_EVENT_UDISK) {
    if (type == STORAGE_TYPE_CONNECT) {
      rotate_signgleton->set_usb_status(1);
    }

    if (type == STORAGE_TYPE_DISCONN) {
      rotate_signgleton->set_usb_status(0);
    }
  }

  update_icon_status();
}

void ui_face_input::slot_for_record() {
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

void ui_face_input::slot_show_password_login() {
  ui_rotate_management* rotate_signgleton =
    ui_rotate_management::getInstance();

  rotate_signgleton->start_activity_win(
    &ui_password_login::staticMetaObject);
}

void ui_face_input::slot_for_test_log() {
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

void ui_face_input::slot_for_capture() {
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

