/*
* ui_eth_setting.cpp - define eth setting function of main
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
#define LOG_TAG     "eth_setting"
#include <log/log.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QStandardItemModel>
#include <QDebug>

#include "ui_eth_setting.hpp"
#include "ui_ui_eth_setting.h"
#include "ui_rotate_management.hpp"
#include "ui_tip_dialog.hpp"
#include "ui_eth_setting_delegate.hpp"

#define LINE_EDIT_WIDTH 300
#define CHECK_BOX_WIDTH 102

#define BORDER_EDIT "border-width:1px;border-color:#ccc;border-style:solid"
#define BORDER_STATIC "background:transparent;border-width:0;border-style:outset"
#define SWITCH_ON "QPushButton{border-image:url(:/icon/turn_on.png)}"
#define SWITCH_OFF "QPushButton{border-image:url(:/icon/turn_off.png)}"

ui_eth_setting::ui_eth_setting(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_eth_setting),
  pModel(NULL),
  pItemDelegate(NULL) {
  this->setWindowFlags(Qt::FramelessWindowHint);
  mgr = eth_management::getInstance();
  ui->setupUi(this);

  for (int i = 0; i< EDIT_ITEM_COUNT; i++) {
    net_edt[i] = NULL;
  }
  dhcp_btn = NULL;

  QStringList net_style;
  net_style << tr("IP地址") << tr("子网掩码") <<
    tr("网关地址") << tr("DNS") << tr("DHCP");
  // QStandardItemModel *pModel = new QStandardItemModel();
  pModel = new QStandardItemModel();
  for (int i = 0; i < net_style.size(); ++i) {
    QStandardItem *pItem = new QStandardItem;
    MuItemData_eth itemData;
    itemData.name = net_style.at(i);

    if (i < net_style.size() - 1) {
      net_edt[i] = new QLineEdit("");
      net_edt[i]->setAlignment(Qt::AlignHCenter);
      itemData.widget = net_edt[i];
      itemData.width = LINE_EDIT_WIDTH;
      net_edt[i]->setParent(ui->eth_listView);
    } else {
      dhcp_btn = new QPushButton("");
      dhcp_btn->setProperty("id", QString("%1").arg(i));
      connect(dhcp_btn, SIGNAL(clicked()), this,
              SLOT(slot_for_dhcp_enable()));
      itemData.widget = dhcp_btn;
      itemData.width = CHECK_BOX_WIDTH;
      dhcp_btn->setParent(ui->eth_listView);
    }
    pItem->setData(QVariant::fromValue(itemData), Qt::UserRole + 1);
    pModel->appendRow(pItem);
  }

  // ui_eth_setting_delegate *pItemDelegate =
  pItemDelegate = new ui_eth_setting_delegate(this);
  ui->eth_listView->setItemDelegate(pItemDelegate);
  ui->eth_listView->setModel(pModel);

  connect(ui->eth_setting_back_btn, SIGNAL(clicked()),
    ui_rotate_management::getInstance(), SLOT(back_activity_win()));
  connect(ui->eth_setting_saving_btn, SIGNAL(clicked()),
          this, SLOT(slot_for_save_btn()));
  connect(mgr, SIGNAL(signal_eth_state_change(int)),
        this, SLOT(slot_for_eth_state(int)));

  memset(&config, 0, sizeof(net_config_para_t));
  ui_device_eth_param_query(&config);

  net_config_para_t param;
  memset(&param, 0, sizeof(net_config_para_t));
  mgr->get_eth_para(&param);
  dhcp_state = 0 < param.dhcp;
  update_eth_show();

  ui->eth_listView->setEditTriggers(
      QAbstractItemView::NoEditTriggers);
}

ui_eth_setting::~ui_eth_setting() {
  ALOGD("ui_eth_setting delete");

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

void ui_eth_setting::slot_for_eth_state(int state) {
  update_eth_show();
}

void ui_eth_setting::update_eth_show() {
  net_config_para_t param;
  memset(&param, 0, sizeof(net_config_para_t));

  if (dhcp_state) {
    mgr->get_eth_para(&param);

    net_edt[0]->setText(param.eth_ip);
    net_edt[1]->setText(param.eth_mask);
    net_edt[2]->setText(param.eth_gateway);
    net_edt[3]->setText(param.eth_dns);

    dhcp_btn->setStyleSheet(SWITCH_ON);
    for (int i = 0; i < EDIT_ITEM_COUNT; i++) {
      net_edt[i]->setEnabled(false);
      net_edt[i]->setStyleSheet(BORDER_STATIC);
    }
  } else {
    memcpy(&param, &config, sizeof(net_config_para_t));
    net_edt[0]->setText(param.eth_ip);
    net_edt[1]->setText(param.eth_mask);
    net_edt[2]->setText(param.eth_gateway);
    net_edt[3]->setText(param.eth_dns);

    dhcp_btn->setStyleSheet(SWITCH_OFF);
    for (int i = 0; i < EDIT_ITEM_COUNT; i++) {
      net_edt[i]->setEnabled(true);
      net_edt[i]->setStyleSheet(BORDER_EDIT);
    }
  }
}

void ui_eth_setting::slot_for_dhcp_enable() {
  dhcp_state = !dhcp_state;
  update_eth_show();
}

void ui_eth_setting::slot_for_save_btn() {
  int ret = -1;
  ui_tip_dialog *w;
  net_config_para_t param;
  QString eth_ip = net_edt[0]->text();
  QString eth_mask = net_edt[1]->text();
  QString eth_gateway = net_edt[2]->text();
  QString eth_dns = net_edt[3]->text();

  if (dhcp_state) {
    ret = mgr->enable_dhcp();
    if (ret) {
      w = new ui_tip_dialog(TIP_WIN_TYPE_IP_ERROR, this);
      w->show();
      return;
    }
    config.dhcp = 1;
    ret = ui_device_eth_param_update(&config);
  } else {
    memset(&param, 0, sizeof(net_config_para_t));
    ui_device_eth_param_query(&param);
    param.dhcp = 0;
    strncpy(param.eth_ip, eth_ip.toUtf8().data(), 32);
    strncpy(param.eth_mask, eth_mask.toUtf8().data(), 32);
    strncpy(param.eth_gateway, eth_gateway.toUtf8().data(), 32);
    strncpy(param.eth_dns, eth_dns.toUtf8().data(), 32);
    ALOGD("%s:%d >> update static config:ip[%s] mask[%s] gate[%s] dns[%s]",
      __func__, __LINE__, param.eth_ip, param.eth_mask,
      param.eth_gateway, param.eth_dns);

    ret = mgr->set_eth_para(&param);
    if (ret) {
      w = new ui_tip_dialog(TIP_WIN_TYPE_IP_ERROR, this);
      w->show();
      return;
    }
    ret = ui_device_eth_param_update(&param);
  }
  ui_device_eth_param_query(&config);
  if (!ret) {
    w = new ui_tip_dialog(TIP_WIN_TYPE_SAVE_SUCCESS, this);
    update_eth_show();
  } else {
    w = new ui_tip_dialog(TIP_WIN_TYPE_SAVE_FAIL, this);
  }
  w->show();
}

