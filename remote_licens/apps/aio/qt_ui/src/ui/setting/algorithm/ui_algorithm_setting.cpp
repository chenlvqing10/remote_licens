/*
* ui_algorithm_setting.cpp - define algorithm_setting function of main
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
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QDebug>

#include "ui_algorithm_setting.hpp"
#include "ui_ui_algorithm_setting.h"
#include "ui_tip_dialog.hpp"
#include "ui_debug_switch.hpp"

#define MAX_CLICKED_COUNT   3
#define MAX_THRESHOLD_VALUE_LEN 2
#define DEBUG_SWITCH_ITEM 4

#define RECG_ENABLE_ROW 0
#define INFRARED_IMAGE_ENABLE_ROW 1
#define IDENTIFY_THRESHOLD_SCORE_ROW  2
#define NIR_LIVE_DETECT_ROW 3
#define LIVING_DETECT_THRESHOLD_SCORE_ROW 4
#define MASK_DETECT_ENABLE_ROW  5
#define MASK_RECG_ENABLE_ROW  6
#define MASK_RECG_THRESHOLD_SCORE_ROW 7
#define RECG_TIMEOUT_ROW  8
#define REPEAT_RECG_ENABLE_ROW  9
#define RECG_INTERVAL_ROW 10
#define RECG_DISTANCE_ROW 11
#define VOICE_MODE_ROW  12
#define GRETTINGS_ROW 13
#define STRANGER_VOICE_ROW  14
#define QR_CODE_RECG_ROW  15
#define BODY_TEMPERATURE_DETECT_ROW 16
#define TEMPERATURE_COMPENSATION_ROW  17
#define ALARM_TEMPERATURE_ROW 18
#define EXPOSURE_VALUES_ROW 19
#define DEVELOPER_MODE_ROW  20

#define SET_BIT(x, y) (x |= (1u << y))
#define GET_BIT(x, y) (x >> y & 1)

void ui_algorithm_setting::ui_init() {
  voice_mode_steps[0] = tr("播报名字");
  voice_mode_steps[1] = tr("播放问候语");

  grettings_steps[0] = tr("请通行");
  grettings_steps[1] = tr("欢迎光临");
  grettings_steps[2] = tr("时间问候语");

  stranger_voice_steps[0] = tr("播报假体");
  stranger_voice_steps[1] = tr("播报陌生人");

  menu_algorithm = {
    {" ", tr("人脸识别"), "1", EN_ITEM_SWITCH},  // RECG_ENABLE_ROW
    {" ", tr("红外图像"), "1", EN_ITEM_SWITCH},  // INFRARED_IMAGE_ENABLE_ROW
    {" ", tr("识别阈值"), "54", EN_ITEM_LABEL},  // IDENTIFY_THRESHOLD_SCORE_ROW
    {" ", tr("活体检测"), "1", EN_ITEM_SWITCH},  // NIR_LIVE_DETECT_ROW
    {" ", tr("活体阈值"), "10", EN_ITEM_LABEL},  // LIVING_DETECT_THRESHOLD_SCORE_ROW
    {" ", tr("口罩检测"), "0", EN_ITEM_SWITCH},  // MASK_DETECT_ENABLE_ROW
    {" ", tr("口罩识别"), "1", EN_ITEM_SWITCH},  // MASK_RECG_ENABLE_ROW
    {" ", tr("口罩阈值"), "53", EN_ITEM_LABEL},  // MASK_RECG_THRESHOLD_SCORE_ROW
    {" ", tr("识别超时"), "1000ms", EN_ITEM_LABEL},  // RECG_TIMEOUT_ROW
    {" ", tr("重复识别"), "1", EN_ITEM_SWITCH},  // REPEAT_RECG_ENABLE_ROW
    {" ", tr("识别间隔"), "1000ms", EN_ITEM_LABEL},  // RECG_INTERVAL_ROW
    {" ", tr("识别距离"), "150cm", EN_ITEM_LABEL},  // RECG_DISTANCE_ROW

    {" ", tr("语音模式"), "", EN_ITEM_LABEL},  // VOICE_MODE_ROW
    {" ", tr("问候语"), "", EN_ITEM_LABEL},  // GRETTINGS_ROW
    {" ", tr("陌生人语音"), "", EN_ITEM_LABEL},  // STRANGER_VOICE_ROW

    {" ", tr("二维码识别"), "0", EN_ITEM_SWITCH},  // QR_CODE_RECG_ROW
    {" ", tr("体温检测"), "0", EN_ITEM_SWITCH},  // BODY_TEMPERATURE_DETECT_ROW
    {" ", tr("温度补偿"), "0", EN_ITEM_LABEL},  // TEMPERATURE_COMPENSATION_ROW
    {" ", tr("报警温度"), "37.3", EN_ITEM_LABEL},  // ALARM_TEMPERATURE_ROW
    {" ", tr("曝光补偿"), "0", EN_ITEM_LABEL},  // EXPOSURE_VALUES_ROW

    {" ", tr("开发者模式"), "", EN_ITEM_LABEL},  // DEVELOPER_MODE_ROW
  };
}

ui_algorithm_setting::ui_algorithm_setting(QWidget *parent) :
    QtMainWindow(parent), ui(new Ui::ui_algorithm_setting),
    pModel(NULL), pItemDelegate(NULL) {
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);
  this->setWindowFlags(Qt::FramelessWindowHint);

  ui_init();

  clicked_count = 0;
  memset(&debug_sw, 0, sizeof(un_debug_switch_t));
  ui_control_get_debug_switch(&debug_sw);

  memset(&alg_info, 0, sizeof(algorithm_system_info_t));
  ui_get_algorithm_system_info(&alg_info);

  pModel = new QStandardItemModel();
  int count = menu_algorithm.count();
  for (int i = 0; i < count - 1; ++i) {
    QStandardItem *pItem = new QStandardItem;

    switch (i) {
      case RECG_ENABLE_ROW:
        menu_algorithm[i].value = QString::number(alg_info.recg_enable);
        break;
      case INFRARED_IMAGE_ENABLE_ROW:
        menu_algorithm[i].value = QString::number(
          alg_info.infrared_image_enable);
        break;
      case IDENTIFY_THRESHOLD_SCORE_ROW:
        menu_algorithm[i].value = QString("%1")
          .arg(alg_info.face_threshold_value * 100);
        break;
      case NIR_LIVE_DETECT_ROW:
        menu_algorithm[i].value = QString::number(alg_info.living_nir);
        break;
      case LIVING_DETECT_THRESHOLD_SCORE_ROW:
        menu_algorithm[i].value = QString("%1")
          .arg(alg_info.living_detect_threshold_score * 100);
        break;
      case MASK_DETECT_ENABLE_ROW:
        menu_algorithm[i].value = QString::number(alg_info.mask_detect_enable);
        break;
      case MASK_RECG_ENABLE_ROW:
        menu_algorithm[i].value = QString::number(alg_info.mask_enable);
        break;
      case MASK_RECG_THRESHOLD_SCORE_ROW:
        menu_algorithm[i].value = QString("%1")
          .arg(alg_info.mask_threshold_value * 100);
        break;
      case RECG_TIMEOUT_ROW:
        menu_algorithm[i].value = QString::number(alg_info.recg_timeout) + "ms";
        break;
      case REPEAT_RECG_ENABLE_ROW:
        menu_algorithm[i].value = QString::number(alg_info.repeat_recg_enable);
        break;
      case RECG_INTERVAL_ROW:
        menu_algorithm[i].value =
            QString::number(alg_info.recg_interval) + "ms";
        break;
      case RECG_DISTANCE_ROW:
        menu_algorithm[i].value =
            QString::number(alg_info.recg_distance) + "cm";
        break;
      case VOICE_MODE_ROW:
        break;
      case GRETTINGS_ROW:
        menu_algorithm[i].value = grettings_steps[alg_info.grettings];
        break;
      case STRANGER_VOICE_ROW:
        break;
      case QR_CODE_RECG_ROW:
        menu_algorithm[i].value = QString::number(alg_info.qr_code_recg);
        break;
      case BODY_TEMPERATURE_DETECT_ROW:
        menu_algorithm[i].value =
            QString::number(alg_info.body_temperature_detect);
        break;
      case TEMPERATURE_COMPENSATION_ROW:
        menu_algorithm[i].value = QString("%1")
          .arg(alg_info.temperature_compensation);
        break;
      case ALARM_TEMPERATURE_ROW:
        menu_algorithm[i].value = QString("%1")
          .arg(alg_info.alarm_temperature);
        break;
      case EXPOSURE_VALUES_ROW:
        menu_algorithm[i].value = QString::number(alg_info.exposure_values);
        break;
      default:
        break;
    }

    pItem->setData(QVariant::fromValue(menu_algorithm[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_setting_item_delegate(this);
  ui->algorithm_listView->setItemDelegate(pItemDelegate);
  ui->algorithm_listView->setModel(pModel);

  connect(ui->algorithm_listView, SIGNAL(click(QModelIndex)),
    this, SLOT(slot_for_algorithm_setting_item(QModelIndex)));
  connect(ui->algorithm_listView, SIGNAL(long_click(QModelIndex)),
    this, SLOT(slot_for_algorithm_setting_item(QModelIndex)));


  connect(ui->algorithm_setting_back_btn, SIGNAL(clicked()),
          rotate_signgleton, SLOT(back_activity_win()));

  ui->algorithm_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_algorithm_setting::~ui_algorithm_setting() {
  ALOGD("%s[%d] ui delete", __func__, __LINE__);

  if (pModel) {
    pModel->clear();
    delete pModel;
    pModel = NULL;
  }

  if (pItemDelegate) {
    delete pItemDelegate;
    pItemDelegate = NULL;
  }

  delete ui;
}

void ui_algorithm_setting::deinit() {
  QtMainWindow::deinit();

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->set_algorithm_params();
  rotate_signgleton->set_debug_switch();
}

void ui_algorithm_setting::slot_for_algorithm_setting_item(
    const QModelIndex &index) {
  int i = index.row();
  int ind = 0;
  int state;
  QString str;
  QMap<int, QString> m_tmp;
  radio_args_t *radio_info;
  count_args_t *count_info;
  checkbox_args_t *checkbox_info;
  ui_radio_dialog *w;
  ui_count_dialog *w_count;
  ui_checkbox_dialog *w_checkbox;
  ui_rotate_management *rotate_signgleton = ui_rotate_management::getInstance();

  switch (i) {
    case RECG_ENABLE_ROW:
      alg_info.recg_enable = !alg_info.recg_enable;
      ui_set_algorithm_system_info(&alg_info);
      switch_clicked(i);
      if (!alg_info.recg_enable && alg_info.mask_enable) {
        alg_info.mask_enable = !alg_info.mask_enable;
        ui_set_algorithm_system_info(&alg_info);
        switch_clicked(MASK_RECG_ENABLE_ROW);
      }
      break;
    case INFRARED_IMAGE_ENABLE_ROW:
      alg_info.infrared_image_enable = !alg_info.infrared_image_enable;
      ui_set_algorithm_system_info(&alg_info);
      switch_clicked(i);
      break;
    case IDENTIFY_THRESHOLD_SCORE_ROW:
    case LIVING_DETECT_THRESHOLD_SCORE_ROW:
    case MASK_RECG_THRESHOLD_SCORE_ROW:
    case EXPOSURE_VALUES_ROW:
      count_info = new count_args_t;
      count_info->id = reinterpret_cast<void *>(i);
      count_info->title = menu_algorithm[i].title;
      if (i == EXPOSURE_VALUES_ROW) {
        count_info->min = -3;
        count_info->max = 3;
      } else {
        count_info->min = 1;
        count_info->max = 99;
      }
      count_info->default_value = menu_algorithm[i].value.toInt();
      count_info->cb = this;
      w_count = new ui_count_dialog(count_info, this);
      w_count->show();
      delete count_info;
      break;
    case NIR_LIVE_DETECT_ROW:
      alg_info.living_nir = !alg_info.living_nir;
      ui_set_algorithm_system_info(&alg_info);
      switch_clicked(i);
      break;
    case MASK_DETECT_ENABLE_ROW:
      alg_info.mask_detect_enable = !alg_info.mask_detect_enable;
      ui_set_algorithm_system_info(&alg_info);
      switch_clicked(i);
      break;
    case MASK_RECG_ENABLE_ROW:
      alg_info.mask_enable = !alg_info.mask_enable;
      ui_set_algorithm_system_info(&alg_info);
      switch_clicked(i);
      if (alg_info.mask_enable && !alg_info.recg_enable) {
        alg_info.recg_enable = !alg_info.recg_enable;
        ui_set_algorithm_system_info(&alg_info);
        switch_clicked(RECG_ENABLE_ROW);
      }
      break;
    case RECG_TIMEOUT_ROW:
    case RECG_INTERVAL_ROW:
    case RECG_DISTANCE_ROW:
      count_info = new count_args_t;
      count_info->id = reinterpret_cast<void *>(i);
      count_info->title = menu_algorithm[i].title;
      if (i == RECG_DISTANCE_ROW) {
        count_info->min = 50;
        count_info->max = 150;
      } else {
        count_info->min = 0;
        count_info->max = 5000;
      }
      str = menu_algorithm[i].value;
      count_info->default_value = str.left(str.length() - 2).toInt();
      count_info->cb = this;
      w_count = new ui_count_dialog(count_info, this);
      w_count->show();
      delete count_info;
      break;
    case REPEAT_RECG_ENABLE_ROW:
      alg_info.repeat_recg_enable = !alg_info.repeat_recg_enable;
      ui_set_algorithm_system_info(&alg_info);
      switch_clicked(i);
      break;
    case VOICE_MODE_ROW:
    case STRANGER_VOICE_ROW:
      if (i == VOICE_MODE_ROW) {
        state = alg_info.voice_mode;
        m_tmp = voice_mode_steps;
      } else {
        state = alg_info.stranger_voice;
        m_tmp = stranger_voice_steps;
      }
      checkbox_info = new checkbox_args_t;
      checkbox_info->id = reinterpret_cast<void *>(i);
      checkbox_info->title = menu_algorithm[i].title;
      for (auto iter = m_tmp.begin();
          iter != m_tmp.end(); ++iter) {
        checkbox_info->checkbox_list << iter.value();
        if (GET_BIT(state, iter.key()))
          checkbox_info->default_indexs << iter.key();
      }
      checkbox_info->cb = this;
      w_checkbox = new ui_checkbox_dialog(checkbox_info, this);
      w_checkbox->show();
      delete checkbox_info;
      break;
    case GRETTINGS_ROW:
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(i);
      radio_info->title = menu_algorithm[i].title;
      for (auto iter = grettings_steps.begin();
          iter != grettings_steps.end(); ++iter) {
        radio_info->radio_list << iter.value();
        if (menu_algorithm[i].value.compare(iter.value()) == 0)
          radio_info->default_index = ind;
        ind++;
      }
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
      break;
    case QR_CODE_RECG_ROW:
      alg_info.qr_code_recg = !alg_info.qr_code_recg;
      ui_set_algorithm_system_info(&alg_info);
      switch_clicked(i);
      break;
    case BODY_TEMPERATURE_DETECT_ROW:
      alg_info.body_temperature_detect = !alg_info.body_temperature_detect;
      ui_set_algorithm_system_info(&alg_info);
      switch_clicked(i);
      break;
    case TEMPERATURE_COMPENSATION_ROW:
    case ALARM_TEMPERATURE_ROW:
      count_info = new count_args_t;
      count_info->id = reinterpret_cast<void *>(i);
      count_info->title = menu_algorithm[i].title;
      if (i == TEMPERATURE_COMPENSATION_ROW) {
        count_info->min = 0;
        count_info->max = 10;
      } else {
        count_info->min = 300;
        count_info->max = 450;
      }
      count_info->default_value = menu_algorithm[i].value.toFloat() * 10;
      count_info->cb = this;
      w_count = new ui_count_dialog(count_info, this, 1, 1);
      w_count->show();
      delete count_info;
      break;
    case DEVELOPER_MODE_ROW:
      rotate_signgleton->start_activity_win(
        &ui_debug_switch::staticMetaObject);
    default:
      break;
  }
}

void ui_algorithm_setting::switch_clicked(int index) {
  if (menu_algorithm[index].value.compare("1") == 0) {
    menu_algorithm[index].value = "0";
  } else {
    menu_algorithm[index].value = "1";
  }

  update_listview(index);
}

void ui_algorithm_setting::update_listview(int row) {
  pModel->setData(pModel->index(row, 0),
  QVariant::fromValue(menu_algorithm[row]), Qt::UserRole + 1);
  ui->algorithm_listView->setModel(pModel);
  ui->algorithm_listView->update();
}

void ui_algorithm_setting::radio_callback(void *id, int index,
  bool isCancel) {
  int row = (int)id;
  int ret;

  if (menu_algorithm[row].value.compare(grettings_steps[index]) == 0) {
    ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
    return;
  }

  alg_info.grettings = index;
  ret = ui_set_algorithm_system_info(&alg_info);
  if (ret) {
    ALOGD("%s[%d] set algorithm parameter fail!", __func__, __LINE__);
    return;
  }

  menu_algorithm[row].value = grettings_steps[index];

  update_listview(row);
}

void ui_algorithm_setting::count_callback(void *id, int count) {
  int ret;
  int row = (int)id;
  QString str;

  switch (row) {
    case IDENTIFY_THRESHOLD_SCORE_ROW:
    case LIVING_DETECT_THRESHOLD_SCORE_ROW:
    case MASK_RECG_THRESHOLD_SCORE_ROW:
    case EXPOSURE_VALUES_ROW:
      if (menu_algorithm[row].value.toInt() == count) {
        ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
        return;
      }

      if (row == IDENTIFY_THRESHOLD_SCORE_ROW)
        alg_info.face_threshold_value = count / 100.0;
      else if (row == LIVING_DETECT_THRESHOLD_SCORE_ROW)
        alg_info.living_detect_threshold_score = count / 100.0;
      else if (row == MASK_RECG_THRESHOLD_SCORE_ROW)
        alg_info.mask_threshold_value = count / 100.0;
      else if (row == EXPOSURE_VALUES_ROW)
        alg_info.exposure_values = count;

      ret = ui_set_algorithm_system_info(&alg_info);
      if (ret) {
        ALOGD("%s[%d] set algorithm parameter fail!", __func__, __LINE__);
        return;
      }

      menu_algorithm[row].value = QString::number(count);

      break;
    case RECG_TIMEOUT_ROW:
    case RECG_INTERVAL_ROW:
    case RECG_DISTANCE_ROW:
      str = menu_algorithm[row].value;
      if (str.left(str.length() - 2).toInt() == count) {
        ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
        return;
      }

      if (row == RECG_TIMEOUT_ROW)
        alg_info.recg_timeout = count;
      else if (row == RECG_INTERVAL_ROW)
        alg_info.recg_interval = count;
      else if (row == RECG_DISTANCE_ROW)
        alg_info.recg_distance = count;

      ret = ui_set_algorithm_system_info(&alg_info);
      if (ret) {
        ALOGD("%s[%d] set algorithm parameter fail!", __func__, __LINE__);
        return;
      }

      if (row == RECG_TIMEOUT_ROW || row == RECG_INTERVAL_ROW)
        menu_algorithm[row].value = QString::number(count) + "ms";
      else if (row == RECG_DISTANCE_ROW)
        menu_algorithm[row].value = QString::number(count) + "mc";

      break;
    case TEMPERATURE_COMPENSATION_ROW:
    case ALARM_TEMPERATURE_ROW:
      if (menu_algorithm[row].value.toFloat() == count / 10.0) {
        ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
        return;
      }

      if (row == TEMPERATURE_COMPENSATION_ROW)
        alg_info.temperature_compensation = count / 10.0;
      else
        alg_info.alarm_temperature = count / 10.0;

      ret = ui_set_algorithm_system_info(&alg_info);
      if (ret) {
        ALOGD("%s[%d] set algorithm parameter fail!", __func__, __LINE__);
        return;
      }

      menu_algorithm[row].value = QString("%1").arg(count / 10.0);

      break;
    default:
      break;
  }

  update_listview(row);
}

void ui_algorithm_setting::checkbox_callback(void *id, QVector<int> counts) {
  int ret, state = 0;
  int row = (int)id;

  for (auto item : counts) {
    SET_BIT(state, item);
  }

  if (row == VOICE_MODE_ROW)
    alg_info.voice_mode = state;
  else if (row == STRANGER_VOICE_ROW)
    alg_info.stranger_voice = state;

  ret = ui_set_algorithm_system_info(&alg_info);
  if (ret) {
    ALOGD("%s[%d] set algorithm parameter fail!", __func__, __LINE__);
    return;
  }
}

void ui_algorithm_setting::update_debug_item_info() {
  un_debug_switch_t debug_sw;
  memset(&debug_sw, 0, sizeof(un_debug_switch_t));
  ui_control_get_debug_switch(&debug_sw);

  if (debug_sw.debug_switch.debug_for_status) {
    QStandardItem *pItem = new QStandardItem;
    pItem->setData(QVariant::fromValue(menu_algorithm[DEVELOPER_MODE_ROW]),
      Qt::UserRole + 1);
    pModel->appendRow(pItem);
  } else {
    if (pModel->rowCount() - 1 == DEVELOPER_MODE_ROW) {
      pModel->removeRow(DEVELOPER_MODE_ROW);
    }
  }

  ui->algorithm_listView->setModel(pModel);
  ui->algorithm_listView->update();
}

int ui_algorithm_setting::check_str_is_validate(const char *str) {
  if (NULL == str) {
    return -1;
  }

  if ((strlen(str) > MAX_THRESHOLD_VALUE_LEN) || (strlen(str) == 0)) {
    return -1;
  }

  for (int i = 0; i < strlen(str); i++) {
    if ((*(str + i) < '0') || (*(str + i) > '9')) {
      return -1;
    }
  }

  return 0;
}

void ui_algorithm_setting::mouseReleaseEvent(QMouseEvent * event) {
  int status = ui_control_get_development_status();
  if (0 == status) {
    return;
  }

  if (clicked_count == 0) {
    begin_time = time(NULL);
  }
  clicked_count++;
  if (clicked_count == MAX_CLICKED_COUNT) {
    if (time(NULL) - begin_time <= 1) {
      un_debug_switch_t debug_sw;
      ui_control_get_debug_switch(&debug_sw);
      debug_sw.debug_switch.debug_for_status =
          ~debug_sw.debug_switch.debug_for_status;
      if (debug_sw.debug_switch.debug_for_status == 0) {
        memset(&debug_sw, 0, sizeof(un_debug_switch_t));
      }
      ui_control_set_debug_switch(&debug_sw);
      update_debug_item_info();
      clicked_count = 0;
    } else {
      clicked_count = 0;
    }
  }
}

