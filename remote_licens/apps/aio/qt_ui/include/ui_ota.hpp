/*
* ui_ota.hpp - define ota server in ui
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

#ifndef UI_OTA_H
#define UI_OTA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ota_svc.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <pthread.h>
#include <QObject>
#include <QTimer>
#include "ui_tip_dialog.hpp"

class ui_ota;

class state : public QObject,
  public ui_tip_callback {
  Q_OBJECT

 public:
  explicit state(ui_ota *svc,
    ota_state id, QObject *parent = nullptr);
  ~state();
  virtual int action_asyn() = 0;
  void mark_err_flag() { err_flag = 1; };
  char is_err() { return err_flag; };
  virtual int tip_callback(tip_args_t *args, bool isCancel);

 public slots:
  virtual void slot_ui();
  virtual void update();
  virtual int slot_confirm();
  virtual int slot_cancel();

 protected:
  ui_ota *ota;
  ui_tip_dialog *dialog;
  static char *dots[];
  char err_flag;

 public:
  ota_state ID;
};

class search_state : public state {
  Q_OBJECT

 public:
  explicit search_state(ui_ota *svc, QObject *parent = nullptr);
  ~search_state();
  int action_asyn();

 public slots:
  void slot_ui();
  void update();
  int slot_cancel();

 public:
  static char isShow;
};

class prompt_state : public state {
  Q_OBJECT

 public:
  explicit prompt_state(ui_ota *svc, QObject *parent = nullptr);
  ~prompt_state();
  int action_asyn();

 public slots:
  void slot_ui();
  int slot_confirm();
};

class loading_state : public state {
  Q_OBJECT

 public:
  explicit loading_state(ui_ota *svc, QObject *parent = nullptr);
  ~loading_state();
  int action_asyn();

 public slots:
  void slot_ui();
  void update();
  int slot_cancel();

 private:
  struct download_handler_s *http_download;
  int pos;
};

class checking_state: public state {
  Q_OBJECT

 public:
  explicit checking_state(ui_ota *svc, QObject *parent = nullptr);
  ~checking_state();
  int action_asyn();

 public slots:
  void slot_ui();
  void update();
};

class reboot_state: public state {
  Q_OBJECT

 public:
  explicit reboot_state(ui_ota *svc, QObject *parent = nullptr);
  ~reboot_state();
  int action_asyn();

 public slots:
  void slot_ui();
  void update();
  int slot_confirm();
  int slot_cancel();
};

class ui_ota : public QObject, public ui_tip_callback {
  Q_OBJECT

 public:
  explicit ui_ota(QObject *parent = nullptr);
  ~ui_ota();
  void svc_proc_impl();

  void try_ota_visit(char isShow = 0);
  void state_callback(ota_state state);
  void send_signal_ui();
  void timer_update_ui(char isStart);
  void test_ota();
  void debug_test_ota();
  int tip_callback(tip_args_t *args, bool isCancel);
  ui_tip_dialog *get_dialog();

 signals:
  void signalUI();

 private slots:
  void slot_update();
  void slot_show_ui();

 private:
  void change_state(state *_state);

 private:
  state *m_state;
  ui_tip_dialog *dialog;
  pthread_mutex_t state_lock;
  char looping;
  pthread_t svc_th;
  pthread_mutex_t loop_lock;
  pthread_cond_t loop_cond;
  QList<ota_state> loops;
  QTimer *pTimer;
};

#endif  //  UI_OTA_H
  