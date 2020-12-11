/*
* ui_ota.cpp - define ota server in ui
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
#define LOG_TAG     "ui_ota"
#include <log/log.h>
#include <stdio.h>
#include "file_ops.h"
#include "ota_svc.h"
#include "file_http_download.h"
#include "update_svc.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <sys/prctl.h>
#include <QDebug>
#include "ui_ota.hpp"
#include "ui_ui_tip_dialog.h"
#include "ui_rotate_management.hpp"

#define OTA_PATH "/data/ota"

char search_state::isShow = 0;
char *state::dots[] = {"   ", ".  ", ".. ", "..."};

static pthread_mutex_t ui_lock;

state::state(ui_ota *svc,
  ota_state id, QObject *parent):
  ota(svc), ID(id), QObject(parent),
  err_flag(0), dialog(NULL) {
};

state::~state() {
  ALOGD("state::%s:%d", __func__, __LINE__);
  ota = NULL;
};

int state::tip_callback(tip_args_t *args, bool isCancel) {
  return isCancel?slot_cancel():slot_confirm();
}

void state::slot_ui() {}

void state::update() {}

int state::slot_confirm() {
  ALOGD("state::%s:%d", __func__, __LINE__);
  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return 0;
  }
  ota->timer_update_ui(0);
  pthread_mutex_unlock(&ui_lock);
  return 0;
}

int state::slot_cancel() {
  ALOGD("state::%s:%d", __func__, __LINE__);
  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return 0;
  }

  ota->timer_update_ui(0);
  pthread_mutex_unlock(&ui_lock);
  ota->state_callback(OTA_STATE_IDEL);
  usleep(100*1000);
  return 0;
}

search_state::search_state(ui_ota *svc, QObject *parent):
    state(svc, OTA_STATE_SEARCH, parent) {
  //todo
  ALOGD("search_state::%s >> ID=%d, isShow=%d", __func__, ID, isShow);
}

search_state::~search_state() {
  //todo
  ALOGD("search_state::%s:%d", __func__, __LINE__);
  ota->timer_update_ui(0);
}

int search_state::action_asyn() {
#if 1
  return ota_http_check_updateinfo();
#else
  int rand_t = rand()%10+1;
  ALOGD("search_state::%s:%d >> rand_t = %d", __func__, __LINE__, rand_t);
  sleep(rand_t);
  return -1;
#endif
}

void search_state::slot_ui() {
  Ui::ui_tip_dialog *ui;

  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }
  if (0 == isShow) {
    ALOGD("%s:%d >> visit in background.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }

  dialog = ota->get_dialog();
  ui = dialog->get_ui();
  ui->tip_confirm_btn->hide();
  ui->tip_title_label->setText(
    QString(tr("版本查询")));
  ui->tip_content_label->setText(
    QString(tr("查询最新版本信息中")).append(dots[0]));

  dialog->show();
  ota->timer_update_ui(1);
  pthread_mutex_unlock(&ui_lock);
}

void search_state::update() {
  static int tmp = 0;

  pthread_mutex_lock(&ui_lock);
  if (dialog) {
    Ui::ui_tip_dialog *ui = dialog->get_ui();
    if (!err_flag) {
      ui->tip_content_label->setText(
        QString(tr("查询最新版本信息中")).append(dots[tmp++%4]));
      pthread_mutex_unlock(&ui_lock);
    } else {
      pthread_mutex_unlock(&ui_lock);
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST);
      w->set_content(tr("查询最新版本操作异常！").toUtf8().data());
      w->show();
      slot_cancel();
    }
  }
  if (65532 == tmp)
    tmp = 0;
}

int search_state::slot_cancel() {
  ALOGD("search_state::%s:%d", __func__, __LINE__);
  cancle_ota_http_get();
  return state::slot_cancel();
}

prompt_state::prompt_state(ui_ota *svc, QObject *parent):
    state(svc, OTA_STATE_PROMPT, parent) {
  //todo
  ALOGD("prompt_state::%s >> ID=%d", __func__, ID);
}

prompt_state::~prompt_state() {
  //todo
  ALOGD("prompt_state::%s:%d", __func__, __LINE__);
  ota->timer_update_ui(0);
}

int prompt_state::action_asyn() {
  ALOGD("prompt_state::%s:%d", __func__, __LINE__);
  return 0;
}

void prompt_state::slot_ui() {
  Ui::ui_tip_dialog *ui;

  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }
  ota->timer_update_ui(0);
  char ota_firmware_version[32] = {0};
  get_ota_firmware_version(ota_firmware_version,
    sizeof(ota_firmware_version));

  dialog = ota->get_dialog();
  ui = dialog->get_ui();
  ui->tip_confirm_btn->show();
  ui->tip_cancel_btn->show();
  ui->tip_title_label->setText(
    QString(tr("OTA升级")));
  ui->tip_content_label->setText(QString(tr("发现新版本<%1>,是否升级？"))
    .arg(ota_firmware_version));

  dialog->show();
  pthread_mutex_unlock(&ui_lock);
 }

int prompt_state::slot_confirm() {
  ALOGD("prompt_state::%s:%d", __func__, __LINE__);
  state::slot_confirm();
  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return 0;
  }
  pthread_mutex_unlock(&ui_lock);
  ota->state_callback(OTA_STATE_DOWNING);
  return 0;
}

loading_state::loading_state(ui_ota *svc, QObject *parent):
    state(svc, OTA_STATE_DOWNING, parent), pos(0),
    http_download(NULL) {
  //todo
  ALOGD("loading_state::%s >> ID=%d", __func__, ID);
}

loading_state::~loading_state() {
  //todo
  ALOGD("loading_state::%s:%d", __func__, __LINE__);
  ota->timer_update_ui(0);
}

int loading_state::action_asyn() {
  ALOGD("loading_state::%s:%d", __func__, __LINE__);
  ui_rotate_management *mgr = ui_rotate_management::getInstance();
  if (!mgr->is_net_connected()) {
    ALOGW("loading_state >> %s: network disconnected!\n", __func__);
    return -1;
  }
  http_download = http_download_create();
  return ota_http_start_update(http_download);
}

void loading_state::slot_ui() {
  Ui::ui_tip_dialog *ui;

  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }

  dialog = ota->get_dialog();
  ui = dialog->get_ui();
  ui->tip_title_label->setText(
    QString(tr("OTA下载")));
  ui->tip_content_label->setText(
    QString(tr("下载新版固件，完成[%1%]")).arg(0).append(dots[0]));
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->show();
  dialog->show();
  pthread_mutex_unlock(&ui_lock);
  ota->timer_update_ui(1);
}

void loading_state::update() {
  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }
  static int tmp = 0;

  if (http_download) {
    if(!err_flag) {
      if (http_download->loading) {
        pos = http_download->progress;
      } else {
        pthread_mutex_unlock(&ui_lock);
        ota->tip_callback(NULL, true);
        ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST);
        w->set_content(tr("连接超时，下载失败！").toUtf8().data());
        w->show();
        return;
      }
    } else {
      pthread_mutex_unlock(&ui_lock);
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST);
      w->set_content(tr("下载最新版本操作异常！").toUtf8().data());
      w->show();
      slot_cancel();
      return;
    }
  }
  if (dialog) {
    Ui::ui_tip_dialog *ui = dialog->get_ui();
    ui->tip_content_label->setText(
      QString(tr("下载新版固件，完成[%1%]")).arg(pos)
      .append(dots[tmp++%4]));
  }
  pthread_mutex_unlock(&ui_lock);
  if (65532 == tmp)
    tmp = 0;
}

int loading_state::slot_cancel() {
  ALOGD("loading_state::%s:%d", __func__, __LINE__);
  if (http_download)
    http_download->stop(http_download);
  http_download = NULL;
  return state::slot_cancel();
}

checking_state::checking_state(ui_ota *svc, QObject *parent):
    state(svc, OTA_STATE_CHECKING, parent) {
  //todo
  ALOGD("checking_state::%s >> ID=%d", __func__, ID);
}

checking_state::~checking_state() {
  ALOGD("checking_state::%s:%d", __func__, __LINE__);
  ota->timer_update_ui(0);
}

int checking_state::action_asyn() {
  int ret = -1;
  ALOGD("checking_state::%s:%d", __func__, __LINE__);
  ret = update_device_copy_pkg(OTA_PATH"/");
  ALOGD("checking_state::%s:%d >> ret =%d", __func__, __LINE__, ret);
  remove(OTA_PATH);
  if (ret == 0)
    ota->state_callback(OTA_STATE_REBOOT);
  else if(ret == -1)
    ALOGE("checking_state::%s:%d >> md5 cmp fail!",
      __func__, __LINE__);
  return ret;
}

void checking_state::slot_ui() {
  Ui::ui_tip_dialog *ui;

  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }

  dialog = ota->get_dialog();
  ui = dialog->get_ui();
  ui->tip_title_label->setText(
    QString(tr("OTA校验")));
  ui->tip_content_label->setText(
    QString(tr("正在检验固件完整性")).append(dots[0]));
  ui->tip_confirm_btn->hide();
  ui->tip_cancel_btn->hide();
  dialog->show();
  pthread_mutex_unlock(&ui_lock);
  ota->timer_update_ui(1);
}

void checking_state::update() {
  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }
  static int tmp = 0;

  if (dialog) {
    Ui::ui_tip_dialog *ui = dialog->get_ui();
    if (!err_flag) {
      ui->tip_content_label->setText(
        QString(tr("正在检验固件完整性")).append(dots[tmp++%4]));
      pthread_mutex_unlock(&ui_lock);
    } else {
      pthread_mutex_unlock(&ui_lock);
      ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST);
      w->set_content(tr("非法固件！").toUtf8().data());
      w->show();
      slot_cancel();
    }
  }
  if (65532 == tmp)
    tmp = 0;
}

reboot_state::reboot_state(ui_ota *svc, QObject *parent):
    state(svc, OTA_STATE_REBOOT, parent) {
  //todo
  ALOGD("reboot_state::%s >> ID=%d", __func__, ID);
}

reboot_state::~reboot_state() {
  //todo
  ALOGD("reboot_state::%s:%d", __func__, __LINE__);
  ota->timer_update_ui(0);
}

int reboot_state::action_asyn() {
  ALOGD("reboot_state::%s:%d", __func__, __LINE__);
  return 0;
}

void reboot_state::slot_ui() {
  Ui::ui_tip_dialog *ui;
  ui_control_wakeup_screen();

  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }

  ota->timer_update_ui(0);
  dialog = ota->get_dialog();
  ui = dialog->get_ui();
  ui->tip_title_label->setText(
    QString(tr("升级安装")));
  ui->tip_content_label->setText(
    QString(tr("10秒后重启进入升级模式，点击确定立即升级。")));
  ui->tip_confirm_btn->show();
  ui->tip_cancel_btn->show();
  dialog->show();
  pthread_mutex_unlock(&ui_lock);
  ota->timer_update_ui(1);
}

void reboot_state::update() {
  pthread_mutex_lock(&ui_lock);
  if (!ota) {
    ALOGW("%s:%d >> state ota had been released.", __func__, __LINE__);
    pthread_mutex_unlock(&ui_lock);
    return;
  }
  static int tmp = 10;

  if (dialog) {
    Ui::ui_tip_dialog *ui = dialog->get_ui();
    ui->tip_content_label->setText(
      QString(tr("%1秒后重启进入升级模式，点击确定立即升级。")).arg(--tmp));
  }
  pthread_mutex_unlock(&ui_lock);
  if (0 == tmp)
    slot_confirm();
}

int reboot_state::slot_confirm() {
  ALOGD("state::%s:%d", __func__, __LINE__);
  ota->timer_update_ui(0);
  pthread_mutex_lock(&ui_lock);
  if (dialog) {
    Ui::ui_tip_dialog *ui = dialog->get_ui();
    ui->tip_confirm_btn->hide();
    ui->tip_cancel_btn->hide();
    ui->tip_content_label->setText(
      QString(tr("重启安装中...")));
  }
  pthread_mutex_unlock(&ui_lock);
  ALOGD("%s:%d >> start to update ota", __func__, __LINE__);
  update_device_start();
  ALOGD("%s:%d >> system reboot finished", __func__, __LINE__);
  return 1;
}

int reboot_state::slot_cancel() {
  ALOGD("state::%s:%d", __func__, __LINE__);
  update_device_cancel();
  return state::slot_cancel();
}

void *svc_proc(void *arg) {
  prctl(PR_SET_NAME, "ota_thread");

  ui_ota *svc = (ui_ota *)arg;
  svc->svc_proc_impl();
  return NULL;
}

void svc_state_callback(void *arg, ota_state state) {
  ALOGD("%s:%d >> state=%d", __func__, __LINE__, state);
  ui_ota *svc = (ui_ota *)arg;
  svc->state_callback(state);
}

ui_ota::ui_ota(QObject *parent) :
    QObject(parent), m_state(NULL), dialog(NULL) {
  ALOGD("%s:%d", __func__, __LINE__);
  loops.clear();
  connect(this, SIGNAL(signalUI()), this, SLOT(slot_show_ui()));
  pTimer = new QTimer();
  connect(pTimer, SIGNAL(timeout()), this, SLOT(slot_update()));
  set_state_callback(this, svc_state_callback);
  init_ota_url();

  if (pthread_mutex_init(&ui_lock, NULL)) {
    ALOGE("init ui lock failed, errno: %d (%s)\n",
      errno, strerror(errno));
  }

  if (pthread_mutex_init(&state_lock, NULL)) {
    ALOGE("init state failed, errno: %d (%s)\n",
      errno, strerror(errno));
  }

  if (pthread_mutex_init(&loop_lock, NULL)) {
    ALOGE("init loop failed, errno: %d (%s)\n",
      errno, strerror(errno));
  }

  if (pthread_cond_init(&loop_cond, NULL)) {
    ALOGE("init loop_cond failed, errno: %d (%s)\n",
      errno, strerror(errno));
  }

  if (pthread_create(&svc_th, NULL, svc_proc, (void *)this)) {
    ALOGE("create loop thread failed, errno: %d (%s)",
      errno, strerror(errno));
  }
}

ui_ota::~ui_ota() {
  ALOGD("%s", __func__);
  looping = 0;
  pthread_mutex_lock(&loop_lock);
  loops.append(OTA_STATE_IDEL);
  pthread_cond_signal(&loop_cond);
  pthread_mutex_unlock(&loop_lock);
  pthread_join(svc_th, NULL);
  disconnect(SIGNAL(signalUI()));
  pTimer->stop();
  disconnect(pTimer, SIGNAL(timeout()), this, SLOT(slot_update()));
  delete pTimer;
  pTimer = NULL;
  pthread_mutex_lock(&ui_lock);
  if (dialog) {
    dialog->deleteLater();
    dialog = NULL;
  }
  pthread_mutex_unlock(&ui_lock);
  pthread_mutex_destroy(&ui_lock);
  pthread_mutex_destroy(&state_lock);
  pthread_mutex_destroy(&loop_lock);
}

void ui_ota::svc_proc_impl() {
  ota_state ret;
  looping = 1;
  state *_state;

  while(looping) {
    _state = NULL;
    pthread_mutex_lock(&loop_lock);
    if (loops.isEmpty())
      pthread_cond_wait(&loop_cond, &loop_lock);
    ret = loops.first();
    loops.pop_front();
    pthread_mutex_unlock(&loop_lock);
    if (NULL != m_state && ret == m_state->ID)
      continue;
    ALOGD("%s recv state=%d.", __func__, ret);

    switch (ret) {
      case OTA_STATE_SEARCH:
        _state = new search_state(this);
        ALOGD("%s:%d create state[%d]",
          __func__, __LINE__, _state->ID);
        break;
      case OTA_STATE_PROMPT:
        _state = new prompt_state(this);
        ALOGD("%s:%d create state[%d]",
          __func__, __LINE__, _state->ID);
        break;
      case OTA_STATE_DOWNING:
        _state = new loading_state(this);
        ALOGD("%s:%d create state[%d]",
          __func__, __LINE__, _state->ID);
        break;
      case OTA_STATE_CHECKING:
        _state = new checking_state(this);
        ALOGD("%s:%d create state[%d]",
          __func__, __LINE__, _state->ID);
        break;
      case OTA_STATE_REBOOT:
        _state = new reboot_state(this);
        ALOGD("%s:%d create state[%d]",
          __func__, __LINE__, _state->ID);
        break;
      default: //OTA_STATE_IDEL
        ALOGW("%s recv state not found.", __func__);
        break;
    }
    change_state(_state);
    if (m_state) {
      send_signal_ui();
      int tmp = m_state->action_asyn();
      if (tmp != 0) {
        ALOGE("%s: state[%d] aticon failed! >> tmp = %d",
          __func__, m_state->ID, tmp);
        m_state->mark_err_flag();
      }
    }
  }
}

void ui_ota::send_signal_ui() {
  ALOGD("%s: notify for show ui!", __func__);
  emit signalUI();
}

void ui_ota::slot_update() {
  pthread_mutex_lock(&state_lock);
  ALOGD("%s:%d", __func__, __LINE__);
  if (m_state) {
    m_state->update();
  }
  pthread_mutex_unlock(&state_lock);
}

void ui_ota::slot_show_ui() {
  pthread_mutex_lock(&state_lock);
  ALOGD("%s:%d >> %p", __func__, __LINE__, m_state);
  if (m_state) {
    ALOGD("%s:%d >> %d", __func__, __LINE__, m_state->ID);
    m_state->slot_ui();
  }
  pthread_mutex_unlock(&state_lock);
}

void ui_ota::timer_update_ui(char isStart) {
  if (isStart)
    pTimer->start(1000);
  else
    pTimer->stop();
}

void ui_ota::change_state(state *_state) {
  pthread_mutex_lock(&state_lock);
  if (NULL != m_state) {
    ALOGD("%s %d >> %d", __func__, m_state->ID, _state?_state->ID:0);
    delete m_state;
    m_state = NULL;
  } else {
    ALOGD("%s null >> %d", __func__, _state?_state->ID:0);
  }
  m_state = _state;
  pthread_mutex_unlock(&state_lock);
}

void ui_ota::try_ota_visit(char isShow) {
  ALOGD("%s >> %d", __func__, isShow);
  search_state::isShow = isShow;
  pthread_mutex_lock(&state_lock);
  if (m_state) {
    if (OTA_STATE_SEARCH == m_state->ID) {
      ALOGI("%s: remove current search state!", __func__);
      m_state->slot_cancel();
    } else if (OTA_STATE_SEARCH < m_state->ID) {
      ALOGD("%s:%d >> state=%d return", __func__, __LINE__, m_state->ID);
      pthread_mutex_unlock(&state_lock);
      return;
    }
  }
  pthread_mutex_unlock(&state_lock);

  pthread_mutex_lock(&loop_lock);
  if (!loops.contains(OTA_STATE_SEARCH)) {
    loops.append(OTA_STATE_SEARCH);
    pthread_cond_signal(&loop_cond);
  } else {
    ALOGW("%s: had contain search event!", __func__);
  }
  pthread_mutex_unlock(&loop_lock);
}

void ui_ota::state_callback(ota_state state) {
  ALOGD("%s:%d >> state=%d", __func__, __LINE__, state);

  pthread_mutex_lock(&loop_lock);
  if (!loops.contains(state)) {
    loops.append(state);
    pthread_cond_signal(&loop_cond);
  } else {
    ALOGW("%s: had contain state = %d!", __func__, state);
  }
  pthread_mutex_unlock(&loop_lock);
}

ui_tip_dialog *ui_ota::get_dialog() {
  if (!dialog) {
    timer_update_ui(0);
    tip_args_t tip_info;
    tip_info.title = QString(tr(" "));
    tip_info.content = QString(tr(" "));
    tip_info.cb = this;

    dialog = new ui_tip_dialog(&tip_info);
  }
  return dialog;
}

int ui_ota::tip_callback(tip_args_t *args, bool isCancel) {
  int ret = 0;
  if (m_state) {
    ret = m_state->tip_callback(args, isCancel);
    if (!isCancel || 0 != ret) {
      return 1;
    }
  }
  timer_update_ui(0);
  pthread_mutex_lock(&ui_lock);
  if (dialog) {
    dialog->deleteLater();
    dialog = NULL;
  }
  pthread_mutex_unlock(&ui_lock);
  return 0;
}




void ui_ota::test_ota() {
  int rand_t = 0;
  while(1) {

#define STATE_LOCK() pthread_mutex_lock(&state_lock);
#define STATE_UNLOCK() pthread_mutex_unlock(&state_lock);

    try_ota_visit(rand()%3>1?1:0);
    ALOGD("%s:%d >> watch OTA_STATE_SEARCH. >> id=%d",
      __func__, __LINE__, m_state?m_state->ID:OTA_STATE_IDEL);

    if (m_state && OTA_STATE_SEARCH == m_state->ID) {
      rand_t = rand()%11;
      ALOGD("%s:%d >> watch OTA_STATE_SEARCH sleep %ds.",
        __func__, __LINE__, rand_t);
      sleep(rand_t);
	  STATE_LOCK();
      if (m_state && OTA_STATE_SEARCH == m_state->ID) {
        ALOGD("%s:%d >> OTA_STATE_SEARCH test slot_cancel.",
          __func__, __LINE__);
        m_state->slot_cancel();
		STATE_UNLOCK();
        usleep(100*1000);
        continue;
      }
	  STATE_UNLOCK();
    }

    ALOGD("%s:%d >> watch OTA_STATE_PROMPT. >> id=%d",
      __func__, __LINE__, m_state?m_state->ID:OTA_STATE_IDEL);
    if (m_state && OTA_STATE_PROMPT == m_state->ID) {
      rand_t = rand()%11;
      ALOGD("%s:%d >> watch OTA_STATE_PROMPT sleep %ds.",
        __func__, __LINE__, rand_t);
      sleep(rand_t);
	  STATE_LOCK();
      if (m_state && OTA_STATE_PROMPT == m_state->ID) {
        if(rand()%2) {

          ALOGD("%s:%d >> OTA_STATE_PROMPT test slot_confirm.",
            __func__, __LINE__);
          m_state->slot_confirm();
		  STATE_UNLOCK();
          usleep(100*1000);
        } else {
          ALOGD("%s:%d >> OTA_STATE_PROMPT test slot_cancel.",
            __func__, __LINE__);
          m_state->slot_cancel();
		  STATE_UNLOCK();
          usleep(100*1000);
          continue;
        }
      }
    }

    ALOGD("%s:%d >> watch OTA_STATE_DOWNING. >> id=%d",
      __func__, __LINE__, m_state?m_state->ID:OTA_STATE_IDEL);
    if (m_state && OTA_STATE_DOWNING == m_state->ID) {
      rand_t = rand()%20;
      ALOGD("%s:%d >> watch OTA_STATE_DOWNING sleep %ds.",
        __func__, __LINE__, rand_t);
      sleep(rand_t);
	  STATE_LOCK();
      if (m_state && OTA_STATE_DOWNING == m_state->ID) {
        ALOGD("%s:%d >> OTA_STATE_DOWNING test slot_cancel.",
          __func__, __LINE__);
        m_state->slot_cancel();
		STATE_UNLOCK();
        usleep(100*1000);
        continue;
      }
	  STATE_UNLOCK();
    }
    ALOGD("%s:%d >> watch OTA_STATE_CHECKING. >> id=%d",
      __func__, __LINE__, m_state?m_state->ID:OTA_STATE_IDEL);
    while (m_state && OTA_STATE_CHECKING == m_state->ID) {
      usleep(100*1000);
    }
    ALOGD("%s:%d >> watch OTA_STATE_REBOOT. >> id=%d",
      __func__, __LINE__, m_state?m_state->ID:OTA_STATE_IDEL);
    if (m_state && OTA_STATE_REBOOT == m_state->ID) {
      rand_t = rand()%10;
      ALOGD("%s:%d >> watch OTA_STATE_REBOOT sleep %ds.",
        __func__, __LINE__, rand_t);
      sleep(rand_t);
	  STATE_LOCK();
      if (m_state && OTA_STATE_REBOOT == m_state->ID) {
        ALOGD("%s:%d >> OTA_STATE_REBOOT test slot_cancel.",
          __func__, __LINE__);
        m_state->slot_cancel();
		STATE_UNLOCK();
        usleep(100*1000);
        continue;
      }
	  STATE_UNLOCK();
    }
    rand_t = rand()%6;
    ALOGD("%s:%d >> watch test loop: sleep %ds.",
      __func__, __LINE__, rand_t);
    sleep(rand_t);
  }
}

static void *test_proc(void *arg) {
  prctl(PR_SET_NAME, "test_ota_th");
  ALOGD("%s:%d >> test thread start.", __func__, __LINE__);
#if 0
  sleep(5);
  ui_ota *svc = (ui_ota *)arg;
  svc->try_ota_visit(1);
  sleep(3);
  svc_state_callback((void *)svc, OTA_STATE_DOWNING);
  sleep(11);
  svc_state_callback((void *)svc, OTA_STATE_CHECKING);
  sleep(10);
  svc_state_callback((void *)svc, OTA_STATE_REBOOT);
  sleep(3);
#endif
  ui_ota *svc = (ui_ota *)arg;
  svc->test_ota();
  ALOGD("%s:%d >> test thread end.", __func__, __LINE__);
  return NULL;
}

void ui_ota::debug_test_ota() {
  ALOGD("%s:%d", __func__, __LINE__);
  pthread_t test_th;
  if (pthread_create(&test_th, NULL, test_proc, (void *)this)) {
    ALOGE("create test ota thread failed, errno: %d (%s)",
      errno, strerror(errno));
  }
}

