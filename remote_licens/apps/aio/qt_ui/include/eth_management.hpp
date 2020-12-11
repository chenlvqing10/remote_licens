/*
* eth_management.hpp - define eth manager of project
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

#ifndef ETH_MANAGEMENT_H
#define ETH_MANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "db_all.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QObject>
#include <pthread.h>

typedef enum {
  ETH_IDLE = 0,
  ETH_LINK_DOWN,  //1
  ETH_LINK_UP,  //2
  ETH_DHCPC_ABORT,  //3
  ETH_DHCPC_TIMEOUT,  //4
  ETH_DHCPC_SUCCESS,  //5
  ETH_STATIC_SUCCESS,  //6
  ETH_STATIC_FAIL  //7
} eth_state;

typedef enum {
  ENO_NULL = 1,
  ENO_IP,  //2
  ENO_MASK,  //3
  ENO_GATEWAY,  //4
  ENO_DNS,  //5
  ENO_SET  //6
} eth_error;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int net_get_info(char *ip, int i_size,
  char *mac, int m_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

class eth_management : public QObject {
  Q_OBJECT

 public:
  static eth_management *getInstance();
  void init_management(net_config_para_t *net_param);
  void deinit_management();
  int eth_link_up_deal();
  void eth_task_proc();
  void eth_link_callback(int state);
  void eth_dhcp_callback(int state);
  void notify_eth_state_change(eth_state state);

  //system api
  int set_eth_para(net_config_para_t *p_ip);
  int get_eth_para(net_config_para_t *p_ip);
  int enable_dhcp();

  /* int get_ip_mac(char *ip, int i_size, char *mac, int m_size); */
  eth_state get_eth_state() {
    return cur_state;
  }

 signals:
  void signal_eth_state_change(int state);

 private:
  eth_management &operator =(const eth_management&);
  eth_management(const eth_management&);
  explicit eth_management(QObject *parent = nullptr);


 private:
  static eth_management *instance;
  eth_state cur_state;
  net_config_para_t eth_info;
  pthread_mutex_t loop_lock;
  pthread_cond_t loop_cond;
  pthread_t proc_thr;
  bool looping;

};

#endif  //  ETH_MANAGEMENT_H