/*
* ui_add_user.cpp - define add user function of main
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
#include "file_ops.h"
#include "face.h"
#include "person_import.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QDebug>

#include "ui_rotate_management.hpp"
#include "ui_add_user.hpp"
#include "ui_ui_add_user.h"
#include "ui_tip_dialog.hpp"
#include "ui_add_user_delegate.hpp"

#define FACE_INPUT_ITEM 2
#define FIELD_NAME_LEN 30

ui_add_user::ui_add_user(QWidget *parent) :
    QtMainWindow(parent),
    ui(new Ui::ui_add_user),
    pModel(NULL),
    pItemDelegate(NULL) {
    user_information = 0;
  this->setWindowFlags(Qt::FramelessWindowHint);
  memset(&user_info, 0, sizeof(user_information_param_t));

  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  ui->setupUi(this);
  userid_step = ui_get_userid_max() + 1;

  for (int i = 0; i < UESR_ITEM_COUNT; i++) {
    user_edt[i] = NULL;
  }
  face_input_btn = NULL;
  memset(finput_value, 0, sizeof(finput_value));

  QStringList user_item;
  user_item << tr("工号") << tr("姓名") << tr("人脸录入");
  // QStandardItemModel *pModel = new QStandardItemModel();
  pModel = new QStandardItemModel();
  for (int i = 0; i < user_item.size(); ++i) {
    QStandardItem *pItem = new QStandardItem;
    MuItemData_add_user itemData;
    itemData.name = user_item.at(i);

    if (i < user_item.size() - 1) {
      user_edt[i] = new QLineEdit("");
      user_edt[i]->setAlignment(Qt::AlignHCenter);
      if (0 == i)
        user_edt[i]->setPlaceholderText(QString::number(userid_step));
      itemData.widget = user_edt[i];
      itemData.value = NULL;
      user_edt[i]->setParent(ui->user_add_listView);
    } else {
      snprintf(finput_value, sizeof(finput_value), "%s",
        tr("未录入").toUtf8().data());
      face_input_btn = new QPushButton("");
      face_input_btn->setProperty("id", QString("%1").arg(i));
      connect(face_input_btn, SIGNAL(clicked()), this,
              SLOT(slot_for_face_input()));
      itemData.widget = face_input_btn;
      face_input_btn->setStyleSheet(
              "QPushButton{border-image:url(:/icon/enter.png)}");
      face_input_btn->setParent(ui->user_add_listView);
    }
    pItem->setData(QVariant::fromValue(itemData), Qt::UserRole + 1);
    pModel->appendRow(pItem);
    itemData.value = finput_value;
  }

  // ui_add_user_delegate *pItemDelegate =
  pItemDelegate = new ui_add_user_delegate(this);
  ui->user_add_listView->setItemDelegate(pItemDelegate);
  ui->user_add_listView->setModel(pModel);
  update_user_info(rotate_signgleton->get_user_info());

  connect(ui->user_add_listView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(slot_for_user_add_listview(QModelIndex)));

  connect(ui->user_add_back_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_cancel_save()));

  connect(ui->user_add_confirm_save, SIGNAL(clicked()),
          this, SLOT(slot_for_confirm_save()));

  ui->user_add_listView->setEditTriggers(
          QAbstractItemView::NoEditTriggers);
}

ui_add_user::~ui_add_user() {
  for (int i = 0; i < UESR_ITEM_COUNT; i++) {
    if (user_edt[i]) {
      delete user_edt[i];
      user_edt[i] = NULL;
    }
  }

  if (face_input_btn) {
    delete face_input_btn;
    face_input_btn = NULL;
  }

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

void ui_add_user::update_user_info(user_information_param_t *usr_info) {
  if (!usr_info) {
    return;
  }
  memcpy(&user_info, usr_info, sizeof(user_information_param_t));
  user_edt[0]->setText(usr_info->employee_number);
  user_edt[1]->setText(usr_info->employee_name);
  if ((user_info.face_norm[0] != 0) || (user_info.face_norm[1] != 0)) {
    snprintf(finput_value, sizeof(finput_value), "%s",
      tr("已录入").toUtf8().data());
  } else {
    snprintf(finput_value, sizeof(finput_value), "%s",
      tr("未录入").toUtf8().data());
  }
}

void ui_add_user::deinit() {
  user_information_param_t tmp_usr_info;
  QtMainWindow::deinit();
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  if (user_information == 1) {
    memcpy(&tmp_usr_info, &user_info, sizeof(user_information_param_t));
    QString id = user_edt[0]->text();
    strncpy(tmp_usr_info.employee_number, id.toUtf8().data(),
            sizeof(tmp_usr_info.employee_number));
    QString name = user_edt[1]->text();
    strncpy(tmp_usr_info.employee_name, name.toUtf8().data(),
            sizeof(tmp_usr_info.employee_name));
    rotate_signgleton->set_user_info(&tmp_usr_info);
  } else {
    user_edt[0]->setText("");
    user_edt[1]->setText("");
    snprintf(finput_value, sizeof(finput_value), "%s",
      tr("未录入").toUtf8().data());
    QString id_null = user_edt[0]->text();
    strncpy(tmp_usr_info.employee_number, id_null.toUtf8().data(),
            sizeof(tmp_usr_info.employee_number));
    QString name_null = user_edt[1]->text();
    strncpy(tmp_usr_info.employee_name, name_null.toUtf8().data(),
            sizeof(tmp_usr_info.employee_name));
    memset(tmp_usr_info.face_norm, 0, sizeof(tmp_usr_info.face_norm));
    rotate_signgleton->set_user_info(&tmp_usr_info);
  }
    rotate_signgleton->set_recg_cnt();
}

void ui_add_user::slot_for_user_add_listview(const QModelIndex &index) {
  if (index.row() == FACE_INPUT_ITEM) {
    slot_for_face_input();
  }
}

void ui_add_user::slot_for_face_input() {
  user_information = 1;
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->start_activity_win(
    &ui_face_authentication::staticMetaObject);
  ((ui_face_authentication *)
    rotate_signgleton->get_current_win())->set_type(0);
}

void ui_add_user::slot_for_cancel_save() {
  user_information = 0;
  remove_pic_by_path(user_info.save_frame_path);
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  rotate_signgleton->back_activity_win();
}

void ui_add_user::slot_for_confirm_save() {
  int ret = 0;
  user_information_param_t tmp_usr_info;
  ui_rotate_management *rotate_signgleton =
    ui_rotate_management::getInstance();
  memcpy(&tmp_usr_info, &user_info, sizeof(user_information_param_t));
  QString id = user_edt[0]->text();
  if (id.compare("") == 0)
    id = QString::number(userid_step);
  strncpy(tmp_usr_info.employee_number, id.toUtf8().data(),
          sizeof(tmp_usr_info.employee_number));
  QString name = user_edt[1]->text();
  strncpy(tmp_usr_info.employee_name, name.toUtf8().data(),
          sizeof(tmp_usr_info.employee_name));

  ret = check_id_is_validate(tmp_usr_info.employee_number);
  if (-1 == ret) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(
      tr("工号未填写！").toUtf8().data());
    w->show();
    return;
  } else if (-2 == ret) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(
      tr("工号格式错误,"
      "请输入0-2147483647以内的数字!").toUtf8().data());
    w->show();
    return;
  } else if (-3 == ret) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(
      tr("您输入的工号错误，"
      "请重新输入工号！").toUtf8().data());
    w->show();
    // user_edt[0]->setFocus();
    return;
  }

  if (db_ui_local_person_get_count(tmp_usr_info.employee_number, "")) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(tr("工号已被注册，请重新输入工号！").toUtf8().data());
    w->show();
    return;
  }

  ret = check_name_is_validate(tmp_usr_info.employee_name);
  if (-1 == ret) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(
      tr("请输入姓名！").toUtf8().data());
    w->show();
    return;
  } else if (-2 == ret) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(
      tr("您输入的姓名长度超过限制(中文不能超过"
        "10个,字母不超过30个),请重新输入姓名!").toUtf8().data());
    w->show();
    return;
  } else if (-3 == ret) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
    w->set_content(
      tr("格式错误，请输入中文或字母").toUtf8().data());
    w->show();
    return;
  }

  if (tmp_usr_info.face_norm[0] == 0.0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_FACE_NO_INPUT, this);
    w->show();
    return;
  }

  ret = ui_insert_user(&tmp_usr_info);
  if (ret == 0) {
    ui_tip_dialog *w = new ui_tip_dialog(TIP_WIN_TYPE_SAVE_SUCCESS, this);
    w->show();
    ret = id.toInt();
    if (ret >= userid_step && ret < INT_MAX -1)
      userid_step = ret + 1;
    ret = 0;
  } else if (ret == -3) {
    ui_tip_dialog *w =
        new ui_tip_dialog(TIP_WIN_TYPE_FACE_SIMILARITY_TOO_HIGH, this);
    w->show();
    return;
  } else {
    ui_tip_dialog *w;
    if (MAX_FEAT_CNT <= db_ui_local_person_get_count(NULL, NULL)) {
      w = new ui_tip_dialog(TIP_WIN_TYPE_TOAST, this);
      w->set_content(QString(tr("抱歉，人脸库容超过最高限制[%1]人！"))
        .arg(MAX_FEAT_CNT).toUtf8().data());
    } else {
      w = new ui_tip_dialog(TIP_WIN_TYPE_SAVE_FAIL, this);
    }
    w->show();
  }

  user_edt[0]->setText("");
  user_edt[1]->setText("");
  user_edt[0]->setPlaceholderText(QString::number(userid_step));
  snprintf(finput_value, sizeof(finput_value), "%s",
    tr("未录入").toUtf8().data());
  QString id_null = user_edt[0]->text();
  strncpy(tmp_usr_info.employee_number, id_null.toUtf8().data(),
          sizeof(tmp_usr_info.employee_number));
  QString name_null = user_edt[1]->text();
  strncpy(tmp_usr_info.employee_name, name_null.toUtf8().data(),
          sizeof(tmp_usr_info.employee_name));
  memset(user_info.face_norm, 0, sizeof(user_info.face_norm));
  memset(tmp_usr_info.face_norm, 0, sizeof(tmp_usr_info.face_norm));
  memset(user_info.save_frame_path, 0, sizeof(user_info.save_frame_path));
  rotate_signgleton->set_user_info(&tmp_usr_info);
  QRegion region;
  region = region.united(ui->user_add_listView->frameGeometry());
  this->repaint(region);
}

