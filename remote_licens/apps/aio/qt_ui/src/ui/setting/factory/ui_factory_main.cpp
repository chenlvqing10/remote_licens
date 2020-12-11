/*
* ui_factory_main.cpp - define factory test main menu functions interface
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
#define LOG_TAG     "ui"
#include <log/log.h>
#include <iniparser.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>

#include "ui_factory_main.hpp"
#include "ui_ui_factory_main.h"
#include "ui_rotate_management.hpp"
#include "flowlayout.hpp"
#include "screen_size.hpp"
#include "ui_factory_lcd.hpp"
#include "ui_factory_speaker.hpp"
#include "ui_factory_camera.hpp"
#include "ui_factory_wifi.hpp"
#include "ui_factory_ble.hpp"
#include "ui_factory_rtc.hpp"
#include "ui_factory_eth.hpp"
#include "ui_factory_ir.hpp"
#include "ui_factory_light_sensor.hpp"
#include "ui_factory_light.hpp"
#include "ui_factory_touch.hpp"
#include "ui_factory_relay.hpp"
#include "ui_factory_rs485.hpp"
#include "ui_factory_wiegand.hpp"
#include "ui_factory_alarm.hpp"
#include "ui_factory_usb.hpp"
#include "ui_factory_range_sensor.hpp"
#include "ui_camera_calibration.hpp"

#define TEST_ITEM_MARGIN    30
#define TEST_ITEM_HSPACING  35
#define TEST_ITEM_VSPACING  28

#define INI_PATH "/etc/factory-test.ini"

#define ID_LCD 0x10
#define ID_SPEAKER 0x11
#define ID_CAMERA 0x12
#define ID_WIFI 0x13
#define ID_BLE 0x14
#define ID_RTC 0x15
#define ID_ETH 0x16
#define ID_IR 0x17
#define ID_LIGHT_SENSOR 0x18
#define ID_LIGHT 0x19
#define ID_TOUCH 0x20
#define ID_RELAY 0x21
#define ID_RS485 0x22
#define ID_WIEGAND 0x23
#define ID_ALARM 0x24
#define ID_USB 0x25
#define ID_CAMERA_CALIBRATION 0x50

static QList<test_win_t> bt_layout_t;

auto_test_thread::auto_test_thread(QObject *parent) :
    QThread(parent), test_ui(NULL) {
  this->auto_test_run = 0;
  this->stopped = true;
  this->pause = false;
}

void auto_test_thread::run() {
  QPushButton *bt;

  if (!test_ui) {
    return;
  }

  int i = 0;
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  while (rotate_signgleton->auto_test.auto_test_run) {
    rotate_signgleton->auto_test.auto_test_pause_mutex.lock();
    if (stopped)
      break;
    bt = bt_layout_t.at(i).bt;
    if (bt) {
      bt->setEnabled(true);
      bt->click();
    }

    rotate_signgleton->auto_test.auto_test_mutex.lock();
    rotate_signgleton->auto_test.auto_test_cond
        .wait(&rotate_signgleton->auto_test.auto_test_mutex);
    rotate_signgleton->auto_test.auto_test_mutex.unlock();

    i++;
    if (i == bt_layout_t.size()) {
      emit auto_test_finish();
      break;
    } else {
      if (&ui_camera_calibration::staticMetaObject == bt_layout_t.at(i).win) {
        i++;
        if (i == bt_layout_t.size()) {
          emit auto_test_finish();
          break;
        }
      }
    }
    rotate_signgleton->auto_test.auto_test_pause_mutex.unlock();
    usleep(2000 * 1000);
  }
  rotate_signgleton->auto_test.auto_test_pause_mutex.unlock();
  if (stopped) {
    rotate_signgleton->back_activity_win();
  }
  test_ui->is_test_ok = 1;
}

void auto_test_thread::auto_test_pause() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->auto_test.pause = true;
  rotate_signgleton->auto_test.auto_test_pause_mutex.lock();
}

void auto_test_thread::auto_test_resume() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->auto_test.pause = false;
  rotate_signgleton->auto_test.auto_test_pause_mutex.unlock();
}

void auto_test_thread::auto_test_stop() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->auto_test.auto_test_run = 0;
  rotate_signgleton->auto_test.auto_test_pause_mutex.lock();
  this->stopped = true;
  rotate_signgleton->auto_test.auto_test_pause_mutex.unlock();
}

void auto_test_thread::set_test_ui(ui_factory_main *_test_ui) {
  test_ui = _test_ui;
}

ui_factory_main::ui_factory_main(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_factory_main) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);
  ui_init();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  rotate_signgleton->auto_test.auto_test_mutex.lock();
  if (rotate_signgleton->auto_test.auto_test_run)
    rotate_signgleton->auto_test.auto_test_cond.wakeAll();
  rotate_signgleton->auto_test.auto_test_mutex.unlock();
  this->showMaximized();
}

ui_factory_main::~ui_factory_main() {
  delete ui;
  delete pTimer;
}

void ui_factory_main::ui_init() {
  const test_win_t *item;
  factory_test_result_t test_status;
  dictionary *ini = NULL;
  const char  *name = NULL;
  char key[128];
  int test_id;

  memset(&test_status, 0, sizeof(factory_test_result_t));
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->get_factory_test_result(&test_status);

  all_test_item_t = {
    {ID_LCD, test_status.lcd_status, tr("LCD屏测试"),
        NULL, &ui_factory_lcd::staticMetaObject},
    {ID_SPEAKER, test_status.speaker_status, tr("扬声器测试"),
        NULL, &ui_factory_speaker::staticMetaObject},
    {ID_CAMERA, test_status.camera_status, tr("摄像头测试"),
        NULL, &ui_factory_camera::staticMetaObject},
    {ID_WIFI, test_status.wifi_status, tr("WIFI测试"),
        NULL, &ui_factory_wifi::staticMetaObject},
    {ID_BLE, test_status.ble_status, tr("BT测试"),
        NULL, &ui_factory_ble::staticMetaObject},
    {ID_RTC, test_status.rtc_status, tr("RTC测试"),
        NULL, &ui_factory_rtc::staticMetaObject},
    {ID_ETH, test_status.eth_status, tr("以太网测试"),
        NULL, &ui_factory_eth::staticMetaObject},
    {ID_IR, test_status.ir_status, tr("红外补光灯测试"),
        NULL, &ui_factory_ir::staticMetaObject},
    {ID_LIGHT_SENSOR, test_status.light_sensor_status, tr("光线传感器测试"),
        NULL, &ui_factory_light_sensor::staticMetaObject},
    {ID_LIGHT, test_status.light_status, tr("白光灯测试"),
        NULL, &ui_factory_light::staticMetaObject},
    {ID_TOUCH, test_status.touch_status, tr("触摸屏测试"),
        NULL, &ui_factory_touch::staticMetaObject},
    {ID_RELAY, test_status.relay_status, tr("继电器测试"),
        NULL, &ui_factory_relay::staticMetaObject},
    {ID_RS485, test_status.rs485_status, tr("RS485测试"),
        NULL, &ui_factory_rs485::staticMetaObject},
    {ID_WIEGAND, test_status.wiegand_status, tr("韦根测试"),
        NULL, &ui_factory_wiegand::staticMetaObject},
    {ID_ALARM, test_status.alarm_status, tr("门磁报警测试"),
        NULL, &ui_factory_alarm::staticMetaObject},
    {ID_USB, test_status.usb_status, tr("USB测试"),
        NULL, &ui_factory_usb::staticMetaObject},
    {ID_CAMERA_CALIBRATION, test_status.range_sensor_status, tr("摄像机标定"),
        NULL, &ui_camera_calibration::staticMetaObject},
  };

  /* margin=30  hSpacing=35 vSpacing=28 */
  FlowLayout *flowLayout = new FlowLayout(TEST_ITEM_MARGIN,
      TEST_ITEM_HSPACING, TEST_ITEM_VSPACING);
  bt_layout_t.clear();

  ini = iniparser_load(INI_PATH);
  if (ini == NULL) {
    ALOGE("Failed to iniparser load");
    return;
  }

  for (int i = 0; i < iniparser_getnsec(ini); i++) {
    name = iniparser_getsecname(ini, i);
    sprintf(key, "%s:enable", name);
    if (iniparser_getint(ini, key, 0)) {
      sprintf(key, "%s:id", name);
      test_id = iniparser_getint(ini, key, 0);
      for (int j = 0; j < all_test_item_t.count(); ++j) {
        if (test_id == all_test_item_t[j].test_id &&
              all_test_item_t[j].bt == NULL) {
          all_test_item_t[j].bt = new QPushButton(ui->factory_flowlayout_widget);
          all_test_item_t[j].bt->setProperty("id", QString("%1")
              .arg(all_test_item_t[j].test_id));
          all_test_item_t[j].bt->setText(all_test_item_t[j].text);
          flowLayout->addWidget(all_test_item_t[j].bt);

          if (1 == all_test_item_t[j].state) {
            all_test_item_t[j].bt->setStyleSheet(
              "QPushButton{border-image:url(:/icon/factory_btn_pass.png)}");
          } else if (2 == all_test_item_t[j].state) {
            all_test_item_t[j].bt->setStyleSheet(
              "QPushButton{border-image:url(:/icon/factory_btn_abnormal.png)}");
          } else if (1 != all_test_item_t[j].state && 2 != all_test_item_t[j].state) {
            all_test_item_t[j].bt->setStyleSheet(
              "QPushButton{border-image:url(:/icon/factory_btn_normal.png)}");
          }

          if (rotate_signgleton->auto_test.auto_test_run)
            all_test_item_t[j].bt->setEnabled(false);
          connect(all_test_item_t[j].bt, SIGNAL(clicked()), this, SLOT(item_clicked()));

          bt_layout_t.append(all_test_item_t[j]);
        }
      }
    }
  }
  ui->factory_flowlayout_widget->setLayout(flowLayout);

  connect(ui->factory_main_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_factory_test_back()));
  connect(&rotate_signgleton->auto_test, SIGNAL(auto_test_finish()),
          this, SLOT(auto_test_finish()));
  connect(ui->auto_test_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_auto_test()));

  if (rotate_signgleton->auto_test.auto_test_run) {
    if (!rotate_signgleton->auto_test.pause)
      ui->auto_test_btn->setText(tr("暂停"));
  } else {
    ui->auto_test_btn->setText(tr("自动测试"));
  }

  rotate_signgleton->auto_test.set_test_ui(this);
  pTimer = new QTimer;
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_for_update_tip()));
}

void ui_factory_main::item_clicked() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  QPushButton *chk_btn = reinterpret_cast <QPushButton *>(sender());
  QString id = chk_btn->property("id").toString();
  foreach(test_win_t item, bt_layout_t) {
    if (id.compare(item.bt->property("id").toString()) == 0) {
      rotate_signgleton->start_activity_win(item.win);
      break;
    }
  }
}

void ui_factory_main::slot_for_auto_test() {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();

  if (bt_layout_t.count() == 0)
    return;

  if (!rotate_signgleton->auto_test.auto_test_run) {
    is_test_ok = 0;
    rotate_signgleton->auto_test.auto_test_run = 1;
    rotate_signgleton->auto_test.stopped = false;

    pTimer->start(1000);
    rotate_signgleton->auto_test.start();
  } else {
    if (!rotate_signgleton->auto_test.pause) {
      ui->auto_test_btn->setText(tr("继续"));
      rotate_signgleton->auto_test.auto_test_pause();
    } else {
      ui->auto_test_btn->setText(tr("暂停"));
      rotate_signgleton->auto_test.auto_test_resume();
    }
  }
}

void ui_factory_main::slot_for_update_tip() {
  if (is_test_ok) {
    pTimer->stop();
    ui->factory_main_back_btn->setEnabled(true);
    ui->auto_test_btn->setEnabled(true);
    ui->auto_test_btn->setStyleSheet("background:#00d5cf");
  }
}

void ui_factory_main::slot_factory_test_back() {
  ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();

  if (rotate_signgleton->auto_test.auto_test_run) {
    if (!rotate_signgleton->auto_test.pause) {
      rotate_signgleton->auto_test.auto_test_pause();
    }
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACTORY_AUTO_TEST, this);
    w->show();
  } else {
    rotate_signgleton->back_activity_win();
  }
}

void ui_factory_main::auto_test_finish() {
  ui_rotate_management *rotate_signgleton =
      ui_rotate_management::getInstance();

  rotate_signgleton->auto_test.auto_test_run = 0;
  ui->auto_test_btn->setText(tr("自动测试"));
  foreach(test_win_t item, bt_layout_t) {
    item.bt->setEnabled(true);
  }
}

