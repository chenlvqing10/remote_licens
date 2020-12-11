/*
* ui_main_setting.cpp - define interface function of UI main setting
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
#define LOG_TAG     "ui"
#include <log/log.h>
#include "ui_control.h"
#include "hotplug.h"
#include "file_ops.h"
#include "system_info.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QDebug>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QFile>

#include "roll_label.hpp"
#include "clicked_label.hpp"
#include "ui_main_setting.hpp"
#include "ui_ui_main_setting.h"
#include "ui_rotate_management.hpp"
#include "ui_volum_setting.hpp"
#include "ui_language_setting.hpp"
#include "ui_setting_item_delegate.hpp"

#define LOGO_NEW_PATH "/mnt/usb/logo.png"
#define LOGO_PATH "/data/logo.png"
#define COMPANY_NAME_MAX_LEN 127

#define COMPANY_NAME_ROW 0
#define COMPANY_LOGO_ROW 1
#define DEVICE_NAME_ROW 2
#define FACE_BOX_COLOR_ROW 3
#define NAME_ENABLE_ROW 4
#define NAME_FORMAT_ROW 5
#define MAC_ENABLE_ROW 6
#define IP_ENABLE_ROW 7
#define REGISTERED_COUNT_ROW 8

void ui_main_setting::ui_init() {
  face_box_color_steps[0] = tr("白色");
  face_box_color_steps[1] = tr("绿色");
  face_box_color_steps[2] = tr("红色");

  color_steps[0] = "#ffffff";
  color_steps[1] = "#00ff00";
  color_steps[2] = "#ff0000";

  name_format_steps[0] = tr("全名");
  name_format_steps[1] = tr("姓**");

  menu_main = {
    {" ", tr("公司名称"), "", EN_ITEM_LABEL, ""},  // COMPANY_NAME_ROW
    {" ", tr("上传logo"), "", EN_ITEM_LABEL, ":/icon/add_logo.png"},  // COMPANY_LOGO_ROW
    {" ", tr("设备名称"), "", EN_ITEM_LABEL, ""},  // DEVICE_NAME_ROW
    {" ", tr("人脸框颜色"), "", EN_ITEM_LABEL, ""},  // FACE_BOX_COLOR_ROW
    {" ", tr("显示人名"), "1", EN_ITEM_SWITCH, ""},  // NAME_ENABLE_ROW
    {" ", tr("人名格式"), "", EN_ITEM_LABEL, ""},  // NAME_FORMAT_ROW
    {" ", tr("显示MAC地址"), "1", EN_ITEM_SWITCH, ""},  // MAC_ENABLE_ROW
    {" ", tr("显示IP地址"), "1", EN_ITEM_SWITCH, ""},  // IP_ENABLE_ROW
    {" ", tr("显示已注册人数"), "1", EN_ITEM_SWITCH, ""}  // REGISTERED_COUNT_ROW
  };
#if 0
  menu_main[COMPANY_NAME_ROW].widget = new QLabel(ui->main_setting_listView);
  menu_main[DEVICE_NAME_ROW].widget = new QLabel(ui->main_setting_listView);
  qobject_cast<QLabel *>(menu_main[COMPANY_NAME_ROW].widget)
      ->setAlignment(Qt::AlignRight);
  qobject_cast<QLabel *>(menu_main[DEVICE_NAME_ROW].widget)
      ->setAlignment(Qt::AlignRight);
  // menu_main[COMPANY_NAME_ROW].widget->setParent(ui->main_setting_listView);
  // menu_main[DEVICE_NAME_ROW].widget->setParent(ui->main_setting_listView);
  // menu_main[COMPANY_NAME_ROW].widget->setFocusProxy(ui->main_setting_listView);
  // menu_main[DEVICE_NAME_ROW].widget->setFocusProxy(ui->main_setting_listView);
#endif
}

ui_main_setting::ui_main_setting(QWidget* parent) :
    QtMainWindow(parent), ui(new Ui::ui_main_setting),
    pModel(NULL), pItemDelegate(NULL) {
  ui_rotate_management* rotate_signgleton = ui_rotate_management::getInstance();
  this->setWindowFlags(Qt::FramelessWindowHint);
  ui->setupUi(this);

  ui_init();

  memset(&main_setting_param, 0, sizeof(main_setting_param_t));
  ui_control_get_main_setting_param(&main_setting_param);
  int color_key = 0;
  for (auto iter = color_steps.begin(); iter != color_steps.end(); ++iter) {
    if (!iter.value().compare(QString(main_setting_param.face_border_color)))
      color_key = iter.key();
  }

  pModel = new QStandardItemModel();
  for (int i = 0; i < menu_main.count(); ++i) {
    QStandardItem *pItem = new QStandardItem;

    switch (i) {
      case COMPANY_NAME_ROW:
        /*
        qobject_cast<QLabel *>(menu_main[i].widget)
          ->setText(QString(main_setting_param.company_name));
        */
        menu_main[i].value = QString(main_setting_param.company_name);
        break;
      case DEVICE_NAME_ROW:
        /*
        qobject_cast<QLabel *>(menu_main[i].widget)
            ->setText(QString(main_setting_param.device_name));
        */
        menu_main[i].value = QString(main_setting_param.device_name);
        if (menu_main[i].value.isEmpty()) {
          build_info_t build_info;
          memset(&build_info, 0, sizeof(build_info_t));
          system_info_get_build_info(&build_info);
          menu_main[i].value = QString(build_info.device_name);
        }
        break;
      case FACE_BOX_COLOR_ROW:
        menu_main[i].value = face_box_color_steps[color_key];
        break;
      case NAME_ENABLE_ROW:
        menu_main[i].value = QString::number(main_setting_param.name_display);
        break;
      case NAME_FORMAT_ROW:
        menu_main[i].value = name_format_steps[main_setting_param.name_format];
        break;
      case MAC_ENABLE_ROW:
        menu_main[i].value = QString::number(main_setting_param.mac_display);
        break;
      case IP_ENABLE_ROW:
        menu_main[i].value = QString::number(main_setting_param.ip_display);
        break;
      case REGISTERED_COUNT_ROW:
        menu_main[i].value = QString::number(
            main_setting_param.register_count_display);
        break;
      default:
        break;
    }
    pItem->setData(QVariant::fromValue(menu_main[i]), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }
  pItemDelegate = new ui_setting_item_delegate(this);
  ui->main_setting_listView->setItemDelegate(pItemDelegate);
  ui->main_setting_listView->setModel(pModel);

  connect(ui->main_setting_back_btn, SIGNAL(clicked()),
      rotate_signgleton, SLOT(back_activity_win()));
  connect(ui->main_setting_listView, SIGNAL(clicked(QModelIndex)),
      this, SLOT(slot_for_item_clicked(QModelIndex)));

  ui->main_setting_listView->setEditTriggers(
    QAbstractItemView::NoEditTriggers);
}

ui_main_setting::~ui_main_setting() {
  ALOGD("[%s][%d] ui delete", __func__, __LINE__);

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

void ui_main_setting::deinit() {
  QtMainWindow::deinit();

  ui_rotate_management* rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->set_main_control();
}

void ui_main_setting::slot_for_item_clicked(const QModelIndex& index) {
  int row = index.row();
  int ind = 0;
  radio_args_t *radio_info;
  input_args_t *input_info;
  ui_radio_dialog *w;
  ui_input_dialog *w_input;
  ui_tip_dialog *w_tip;
  ui_tip_dialog *update_tip;
  tip_args_t *tip_info;
  display_control_t display_ctl;
  ui_rotate_management* rotate_signgleton = ui_rotate_management::getInstance();

  switch (row) {
    case COMPANY_NAME_ROW:
    case DEVICE_NAME_ROW:
      input_info = new input_args_t;
      input_info->id = reinterpret_cast<void *>(row);
      input_info->title = menu_main[row].title;
      input_info->default_input = menu_main[row].value;
      input_info->echo_mode = QLineEdit::Normal;
      input_info->max_len = COMPANY_NAME_MAX_LEN;
      input_info->cb = this;
      w_input = new ui_input_dialog(input_info, this);
      w_input->show();
      delete input_info;
      break;
    case COMPANY_LOGO_ROW:
      tip_info = new tip_args_t;
      tip_info->id = reinterpret_cast<void *>(COMPANY_LOGO_ROW);
      tip_info->title = QString(tr("更新logo"));
      tip_info->content = QString(tr("是否更新logo?"))    ;
      tip_info->value = QString("");
      tip_info->cb = this;

      if (udisk_is_ok() != 0) {
        w_tip = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
        w_tip->set_content(tr("未检测到u盘插入").toUtf8().data());
      } else if (is_file_exist(LOGO_NEW_PATH) != 0) {
        w_tip = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
        w_tip->set_content(tr("未读取到logo.png文件").toUtf8().data());
      } else {
        update_tip = new ui_tip_dialog(tip_info, this);
      }
      delete tip_info;
      break;
    case FACE_BOX_COLOR_ROW:
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(row);
      radio_info->title = menu_main[row].title;
      for (auto item : face_box_color_steps) {
        radio_info->radio_list << item;
        if (menu_main[row].value.compare(item) == 0)
          radio_info->default_index = ind;
        ind++;
      }
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
      break;
    case NAME_ENABLE_ROW:
      main_setting_param.name_display = !main_setting_param.name_display;
      ui_control_set_main_setting_param(&main_setting_param);
      display_ctl.name_display = main_setting_param.name_display;
      display_ctl.name_format = main_setting_param.name_format;
      set_display_control(&display_ctl);
      rotate_signgleton->set_main_control();
      switch_clicked(row);
      break;
    case NAME_FORMAT_ROW:
      radio_info = new radio_args_t;
      radio_info->id = reinterpret_cast<void *>(row);
      radio_info->title = menu_main[row].title;
      for (auto iter = name_format_steps.begin();
          iter != name_format_steps.end(); ++iter) {
        radio_info->radio_list << iter.value();
        if (menu_main[row].value.compare(iter.value()) == 0)
          radio_info->default_index = ind;
        ind++;
      }
      radio_info->cb = this;
      w = new ui_radio_dialog(radio_info, this);
      w->show();
      delete radio_info;
      break;
    case MAC_ENABLE_ROW:
      main_setting_param.mac_display = !main_setting_param.mac_display;
      ui_control_set_main_setting_param(&main_setting_param);
      switch_clicked(row);
      break;
    case IP_ENABLE_ROW:
      main_setting_param.ip_display = !main_setting_param.ip_display;
      ui_control_set_main_setting_param(&main_setting_param);
      switch_clicked(row);
      break;
    case REGISTERED_COUNT_ROW:
      main_setting_param.register_count_display =
          !main_setting_param.register_count_display;
      ui_control_set_main_setting_param(&main_setting_param);
      switch_clicked(row);
      break;
    default:
      break;
  }
}

void ui_main_setting::radio_callback(void *id, int index,
  bool isCancel) {
  int row = (int)id;
  int ret;
  display_control_t display_ctl;
  ui_rotate_management* rotate_signgleton = ui_rotate_management::getInstance();

  if (row == FACE_BOX_COLOR_ROW) {
    if (menu_main[row].value.compare(face_box_color_steps[index]) == 0) {
      ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
      return;
    }

    memcpy(main_setting_param.face_border_color,
        color_steps[index].toUtf8().data(), 16);
    ret = ui_control_set_main_setting_param(&main_setting_param);
    if (ret) {
      ALOGD("%s[%d] set main ui parameter fail!", __func__, __LINE__);
      return;
    }
    rotate_signgleton->set_border_color();
    menu_main[row].value = face_box_color_steps[index];
  } else if (row == NAME_FORMAT_ROW) {
    if (menu_main[row].value.compare(name_format_steps[index]) == 0) {
      ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
      return;
    }

    main_setting_param.name_format = index;
    ret = ui_control_set_main_setting_param(&main_setting_param);
    if (ret) {
      ALOGD("%s[%d] set main ui parameter fail!", __func__, __LINE__);
      return;
    }
    display_ctl.name_display = main_setting_param.name_display;
    display_ctl.name_format = main_setting_param.name_format;
    set_display_control(&display_ctl);
    rotate_signgleton->set_main_control();
    menu_main[row].value = name_format_steps[index];
  }

  update_listview(row);
}

void ui_main_setting::input_callback(void *id, QString text) {
  int row = (int)id;
  int ret;
  ui_rotate_management* rotate_signgleton = ui_rotate_management::getInstance();

  if (menu_main[row].value.compare(text) == 0) {
      ALOGD("[%s:%d] parameter no change", __func__, __LINE__);
      return;
  }

  if (row == COMPANY_NAME_ROW) {
    memcpy(main_setting_param.company_name, text.toUtf8().data(), 128);
    ret = ui_control_set_main_setting_param(&main_setting_param);
    rotate_signgleton->set_company_name();
  } else if (row == DEVICE_NAME_ROW) {
    memcpy(main_setting_param.device_name, text.toUtf8().data(), 128);
    ret = ui_control_set_main_setting_param(&main_setting_param);
  }

  if (ret) {
    ALOGD("%s[%d] set main ui parameter fail!", __func__, __LINE__);
    return;
  }

  menu_main[row].value = text;
  update_listview(row);
}

int ui_main_setting::tip_callback(tip_args_t *args, bool isCancle) {
  if (isCancle)
    return 0;

  ui_tip_dialog *w_tip = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
  ui_rotate_management* rotate_signgleton = ui_rotate_management::getInstance();

  if (file_copy(LOGO_NEW_PATH, LOGO_PATH)) {
    w_tip->set_content(tr("更新失败").toUtf8().data());
    w_tip->show();
  } else {
    w_tip->set_content(tr("更新成功").toUtf8().data());
    w_tip->show();
    rotate_signgleton->set_logo_pixmap();
  }

  return 0;
}

void ui_main_setting::switch_clicked(int index) {
  if (menu_main[index].value.compare("1") == 0) {
    menu_main[index].value = "0";
  } else {
    menu_main[index].value = "1";
  }

  update_listview(index);
}

void ui_main_setting::update_listview(int row) {
  pModel->setData(pModel->index(row, 0),
  QVariant::fromValue(menu_main[row]), Qt::UserRole + 1);
  ui->main_setting_listView->setModel(pModel);
  ui->main_setting_listView->update();
}

