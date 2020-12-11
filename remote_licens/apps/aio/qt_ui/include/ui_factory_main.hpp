/*
* ui_factory_main.h - define factory test main menu functions interface
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

#ifndef UI_FACTORY_MAIN_H
#define UI_FACTORY_MAIN_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QPushButton>
#include "QtMainWindow.hpp"

// 未测试
#define TEST_STATUS_NONE    0
// 测试正常
#define TEST_STATUS_OK      1
// 测试异常
#define TEST_STATUS_FAIL    2

typedef struct _factory_test_result {
  int lcd_status;
  int touch_status;
  int camera_status;
  int wifi_status;
  int ble_status;
  int rtc_status;
  int eth_status;
  int range_sensor_status;
  int light_sensor_status;
  int light_status;
  int ir_status;
  int speaker_status;
  int relay_status;
  int rs485_status;
  int wiegand_status;
  int alarm_status;
  int usb_status;
} factory_test_result_t;

typedef struct test_win_ {
  int test_id;
  int state;
  QString text;
  QPushButton *bt;
  const QMetaObject *win;
} test_win_t;

namespace Ui {
class ui_factory_main;
}

class ui_factory_main;

typedef void(*test_enter_fn)(ui_factory_main *factory_main);

class auto_test_thread : public QThread {
  Q_OBJECT

 public:
  explicit auto_test_thread(QObject *parent = 0);
  void set_test_ui(ui_factory_main *_test_ui);
  void auto_test_pause();
  void auto_test_resume();
  void auto_test_stop();

 protected:
  void run();

 signals:
  void auto_test_finish();

 public:
  int auto_test_run;
  bool stopped;
  bool pause;
  QMutex auto_test_mutex;
  QMutex auto_test_pause_mutex;
  QWaitCondition auto_test_cond;

 private:
  ui_factory_main *test_ui;
};

class ui_factory_main : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_factory_main(QWidget *parent = 0);
  ~ui_factory_main();
  void ui_init();

 private slots:
  void item_clicked();
  void slot_for_auto_test();
  void slot_for_update_tip();
  void slot_factory_test_back();
  void auto_test_finish();

 public:
  int is_test_ok;
  Ui::ui_factory_main *ui;

 private:
  QTimer *pTimer;
  // auto_test_thread auto_test;
  QList<test_win_t> all_test_item_t;
};

#endif  //  UI_FACTORY_MAIN_H
