/*
* eth_management.cpp - define eth manager of project
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

#define LOG_TAG     "eth_mgr"
#include <log/log.h>

#include "net_utils.h"
#include "ip_config.h"
#include "link_handler.h"
#include "ip_config.h"
#include "inet_monitor.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <sys/prctl.h>
#include <arpa/inet.h>
#include "eth_management.hpp"

#define NET_DEV_NAME "eth0"

#if NETWORK_DEBUG == 1
#define NATIVE_ETH(func) do { \
  ALOGD("%s:%d >> native debug start!\n", __func__, __LINE__); \
  func \
  ALOGD("%s:%d >> native debug finish!\n", __func__, __LINE__); \
} while (0)

#else
#define NATIVE_ETH(func) do { \
  func \
} while (0)
#endif

eth_management *eth_management::instance = nullptr;

#if __cplusplus
extern "C" {
#endif

int get_eth_para(net_config_para_t *p_ip) {
  ALOGD("%s:%d >>", __func__, __LINE__);
  eth_management *mgr = eth_management::getInstance();
  return mgr->get_eth_para(p_ip);
}

int sync_set_eth_para(net_config_para_t *p_ip)
{
  int ret = 0;
  ALOGD("%s:%d", __func__, __LINE__);
  eth_management *mgr = eth_management::getInstance();
#if 0
  p_ip->dhcp = 0;
  memcpy(p_ip->eth_ip, "192.168.3.19", 32);
  memcpy(p_ip->eth_mask, "255.255.255.0", 32);
  memcpy(p_ip->eth_gateway, "192.168.3.1", 32);
  memcpy(p_ip->eth_dns, "192.168.3.1", 32);
#endif
  ret = mgr->set_eth_para(p_ip);
  if (ret) {
    ALOGD("%s:%d >> set eth failed!%d", __func__, __LINE__, ret);
    return ret;
  }
  while (ETH_LINK_UP == mgr->get_eth_state())
    usleep(200 * 1000);
  ret = mgr->get_eth_state();
  ALOGD("%s:%d >> eth state=%d", __func__, __LINE__, ret);
  if (ETH_DHCPC_SUCCESS == ret || ETH_STATIC_SUCCESS == ret)
    ret = 0;
  else
    ret = -1;
  return ret;
}

int net_get_info(char *ip, int i_size, char *mac, int m_size) {
  int ret = 0;
  unsigned char mac_addr[6];
  ip_static_config_t ip_config;
  memset(&ip_config, 0, sizeof(ip_config));
  memset(mac_addr, 0, sizeof(mac_addr));

  ret = net_dev_get_static_ip("eth0", &ip_config);
  if (ret) {
    ret = net_dev_get_static_ip("wlan0", &ip_config);
    if (ret) {
      ALOGD("%s:%d >> network does not connect!", __func__, __LINE__);
      memset(ip, 0, i_size);
      memset(mac, 0, m_size);
      return 0;
    } else {
      ret = net_dev_get_mac_addr("wlan0", mac_addr);
      if (ret) {
        ALOGE("failed to get mac addr of device wlan0, ret: %d", ret);
        return -1;
      } else {
        sprintf(ip, "%d.%d.%d.%d", ip_config.ip_addr[0], ip_config.ip_addr[1],
          ip_config.ip_addr[2], ip_config.ip_addr[3]);
        sprintf(mac, "%x:%x:%x:%x:%x:%x", mac_addr[0], mac_addr[1], mac_addr[2],
          mac_addr[3], mac_addr[4], mac_addr[5]);
        return 0;
      }
    }
  } else {
    ret = net_dev_get_mac_addr("eth0", mac_addr);
    if (ret) {
      ALOGE("failed to get mac addr of device eth0, ret: %d", ret);
      return -1;
    } else {
      sprintf(ip, "%d.%d.%d.%d", ip_config.ip_addr[0], ip_config.ip_addr[1],
        ip_config.ip_addr[2], ip_config.ip_addr[3]);
      sprintf(mac, "%x:%x:%x:%x:%x:%x", mac_addr[0], mac_addr[1], mac_addr[2],
        mac_addr[3], mac_addr[4], mac_addr[5]);
      return 0;
    }
  }

#if 0
  FILE *stream;

  memset(ip, 0, i_size);
  memset(mac, 0, m_size);
  stream = popen(
    "ifconfig eth0 | grep 'inet addr' |awk '{print $2}'| tr -d 'addr:'", "r");
  if (!stream) {
    ALOGE("%s >> popen failed, errno: %d (%s)\n",
      __func__, errno, strerror(errno));
    return -1;
  }
  fread(ip, 1, i_size - 1, stream);
  fflush(stream);
  pclose(stream);
  if (strlen(ip) > 0) {
    if (ip[strlen(ip) - 1] == '\n')
      ip[strlen(ip) - 1] = '\0';
    stream = popen(
      "ifconfig eth0 | grep 'HWaddr' |awk '{print $5}'", "r");
    if (!stream) {
      ALOGE("%s >> popen failed, errno: %d (%s)\n",
        __func__, errno, strerror(errno));
      return -1;
    }
    fread(mac, 1, m_size - 1, stream);
    if (strlen(mac) > 0 && mac[strlen(mac) - 1] == '\n')
      mac[strlen(mac) - 1] = '\0';
    fflush(stream);
    pclose(stream);
  } else {
    memset(ip, 0, i_size);
    memset(mac, 0, m_size);
    stream = popen(
      "ifconfig wlan0 | grep 'inet addr' |awk '{print $2}'| tr -d 'addr:'", "r");
    if (!stream) {
      ALOGE("%s >> popen failed, errno: %d (%s)\n",
        __func__, errno, strerror(errno));
      return -1;
    }
    fread(ip, 1, i_size - 1, stream);
    fflush(stream);
    pclose(stream);
    if (strlen(ip) > 0) {
      if (ip[strlen(ip) - 1] == '\n')
        ip[strlen(ip) - 1] = '\0';
      stream = popen(
        "ifconfig wlan0 | grep 'HWaddr' |awk '{print $5}'", "r");
      if (!stream) {
        ALOGE("%s >> popen failed, errno: %d (%s)\n",
          __func__, errno, strerror(errno));
        return -1;
      }
      fread(mac, 1, m_size - 1, stream);
      if (strlen(mac) > 0 && mac[strlen(mac) - 1] == '\n')
        mac[strlen(mac) - 1] = '\0';
      fflush(stream);
      pclose(stream);
    }
  }
  return 0;
#endif
}

#if __cplusplus
}
#endif

static void *net_eth_init_task(void *p)
{
  eth_management *mgr = eth_management::getInstance();
  mgr->eth_task_proc();
  return NULL;
}

static void net_eth_link_callback(int status)
{
  eth_management *mgr = eth_management::getInstance();
  mgr->eth_link_callback(status);
}

static void net_eth_dhcp_callback(int status)
{
  eth_management *mgr = eth_management::getInstance();
  mgr->eth_dhcp_callback(status);
}

eth_management::eth_management(QObject *parent) :
  QObject(parent), cur_state(ETH_IDLE), looping(false) {
  memset(&eth_info, 0, sizeof(net_config_para_t));
  if (pthread_mutex_init(&loop_lock, NULL)) {
    ALOGE("%s >> init loop failed, errno: %d (%s)\n",
      __func__, errno, strerror(errno));
  }

  if (pthread_cond_init(&loop_cond, NULL)) {
    ALOGE("%s >> init loop_cond failed, errno: %d (%s)\n",
      __func__, errno, strerror(errno));
  }
}

eth_management::eth_management(const eth_management&) {}

eth_management& eth_management::operator = (const eth_management&) {}

eth_management* eth_management::getInstance() {
  if (!instance) {
    instance = new eth_management();
  }
  return instance;
}

void eth_management::init_management(net_config_para_t *net_param) {
  ALOGD("%s:%d >> init management", __func__, __LINE__);
  if (looping)
    return;
  looping = true;
  memset(&eth_info, 0, sizeof(net_config_para_t));
  memcpy(&eth_info, net_param, sizeof(net_config_para_t));
  if (eth_info.enable) {
    pthread_create(&proc_thr, NULL, net_eth_init_task, this);
  } else {
    ALOGI("%s:%d >> eth eth disenable!", __func__, __LINE__);
  }
}

void eth_management::deinit_management() {
  int ret = 0;
  ALOGD("%s:%d >> deinit management", __func__, __LINE__);
  if (!looping)
    return;
  looping = false;
  pthread_join(proc_thr, NULL);
  NATIVE_ETH(ret = net_dev_is_open(NET_DEV_NAME););
  if (0 == ret) {
    notify_eth_state_change(ETH_IDLE);
    return;
  }
  /*stop dhcp*/
  NATIVE_ETH(net_dev_stop_dhcp(NET_DEV_NAME););
  /*stop detected of net link*/
  NATIVE_ETH(ret = eth_dev_stop_link_detect(NET_DEV_NAME););
  if (ret) {
    ALOGE("%s: failed to started", __func__);
  }

  /*close net device*/
  NATIVE_ETH(ret = net_dev_close(NET_DEV_NAME););
  if (ret) {
    ALOGE("failed to open net device %s, error:%d", NET_DEV_NAME, ret);
  }
}

void eth_management::eth_task_proc() {
  int ret = 0;
  int check_cnt = 0;
  prctl(PR_SET_NAME, "eth_thr");

  /*open net device*/
  NATIVE_ETH(ret = net_dev_open(NET_DEV_NAME););
  if (ret) {
    ALOGE("failed to open net device %s, error:%d\n", NET_DEV_NAME, ret);
    return;
  }

  for (; check_cnt < 5; ++check_cnt) {
    sleep(1);
    NATIVE_ETH(ret = eth_dev_link_is_up(NET_DEV_NAME););
    if (ret >= 0) {
      break;
    } else {
      ALOGW("%s: failed to get link state", __func__);
      continue;
    }
  }
  if (1 == ret) {
    notify_eth_state_change(ETH_LINK_UP);
    set_eth_para(&eth_info);
  } else if (0 == ret){
    notify_eth_state_change(ETH_LINK_DOWN);
  } else {
    notify_eth_state_change(ETH_IDLE);
  }

  /*start detected of net link*/
  NATIVE_ETH(ret = eth_dev_start_link_detect(NET_DEV_NAME););
  if (ret)
    ALOGE("%s: failed to started", __func__);
  /*register link status change callback*/
  NATIVE_ETH(ret = eth_dev_add_link_handler( NET_DEV_NAME,
    (eth_dev_link_handler)net_eth_link_callback););
  if (ret)
    ALOGE("%s: failed to register\n", __func__);

  while (looping) {
    pthread_mutex_lock(&loop_lock);
    pthread_cond_wait(&loop_cond, &loop_lock);
    pthread_mutex_unlock(&loop_lock);
    sleep(5);
    /*start dhcp when linkup*/
    NATIVE_ETH(ret = eth_dev_link_is_up(NET_DEV_NAME););
    if (1 == ret) {
      notify_eth_state_change(ETH_LINK_UP);
      if (eth_info.dhcp)
        NATIVE_ETH(net_dev_start_dhcp(NET_DEV_NAME,
          (dhcp_status_handler)net_eth_dhcp_callback););
      else
        set_eth_para(&eth_info);
    } else if (0 == ret) {
      notify_eth_state_change(ETH_LINK_DOWN);
    }
  }

  return;
}

void eth_management::eth_link_callback(int state) {
  ALOGD("%s: the current net link status is %d\n", __func__, cur_state);

  switch (state) {
  case ETH_DEV_LINK_UNKNOW:
    notify_eth_state_change(ETH_IDLE);
    break;

  case ETH_DEV_LINK_DOWN:
    if (eth_info.dhcp)
      NATIVE_ETH(net_dev_stop_dhcp(NET_DEV_NAME););
    NATIVE_ETH(net_dev_del_ip_config(NET_DEV_NAME););
    notify_eth_state_change(ETH_LINK_DOWN);
    break;

  case ETH_DEV_LINK_UP:
    notify_eth_state_change(ETH_LINK_UP);
    if (set_eth_para(&eth_info) < 0) {
      pthread_mutex_lock(&loop_lock);
      pthread_cond_signal(&loop_cond);
      pthread_mutex_unlock(&loop_lock);
    }
    break;
  }
}

void eth_management::eth_dhcp_callback(int state) {
  int ret = 0;
  ALOGD("%s: the current net dhcp result is %d\n", __func__, state);
  switch (state) {
  case DHCP_ABORT:
    notify_eth_state_change(ETH_DHCPC_ABORT);
    break;
  case DHCP_TIMEOUT:
    pthread_mutex_lock(&loop_lock);
    pthread_cond_signal(&loop_cond);
    pthread_mutex_unlock(&loop_lock);
    notify_eth_state_change(ETH_DHCPC_TIMEOUT);
    break;
  case DHCP_SUCCESS:
    get_eth_para(&eth_info);
    ALOGD("Trun to eth connect first!");
    NATIVE_ETH(ret = net_dev_change_priority(NET_DEV_NAME););
    if (ret < 0)
      ALOGE("change_priority failed");
    notify_eth_state_change(ETH_DHCPC_SUCCESS);
    break;
  }
}

void eth_management::notify_eth_state_change(eth_state state) {
  ALOGD("%s: state=%d, cur_state=%d", __func__, state, cur_state);
  if (state == cur_state) return;
  cur_state = state;
  emit signal_eth_state_change((int)state);
}

int eth_management::set_eth_para(net_config_para_t *p_ip)
{
  int ret = 0;
  unsigned int temp_addr = 0;
  ip_static_config_t net_para;

  if (!p_ip) {
    ALOGE("%s: para pointer is NULL, %p\n", __func__, p_ip);
    return -ENO_NULL;
  }
  ALOGD("%s:%d >> net_config_para_t[%d %d %d %s %s %s %s]",
    __func__, __LINE__, p_ip->enable, p_ip->priority, p_ip->dhcp,
    p_ip->eth_ip, p_ip->eth_mask, p_ip->eth_gateway,
    p_ip->eth_dns);

  /*set net para*/
  if (p_ip->dhcp) {
    ALOGD("%s: start eth dhcp >> %d\n", __func__, cur_state);

    if (ETH_LINK_UP < cur_state)
      notify_eth_state_change(ETH_LINK_UP);
    NATIVE_ETH(net_dev_start_dhcp(NET_DEV_NAME,
          (dhcp_status_handler)net_eth_dhcp_callback););
    eth_info.dhcp = true;
  } else { //static ip config
    /*init net para*/
    memset(&net_para, 0x00, sizeof(net_para));

    /*ip addr convert*/
    temp_addr = inet_addr(p_ip->eth_ip);
    if (INADDR_NONE == temp_addr) {
      ALOGE("%s: input ip string is error: %s\n", __func__, p_ip->eth_ip);
      return -ENO_IP;
    }
    memcpy(net_para.ip_addr, (unsigned char *)&temp_addr, 4);

    /*mask addr convert*/
    temp_addr = inet_addr(p_ip->eth_mask);
    if (INADDR_NONE == temp_addr) {
      ALOGE("%s: input mask string is error: %s\n", __func__, p_ip->eth_mask);
      return -ENO_MASK;
    }
    memcpy(net_para.netmask, (unsigned char *)&temp_addr, 4);

    /*gateway addr convert*/
    temp_addr = inet_addr(p_ip->eth_gateway);
    if (INADDR_NONE == temp_addr) {
      ALOGE("%s: input gateway string is error: %s\n", __func__,
        p_ip->eth_gateway);
      return -ENO_GATEWAY;
    }
    memcpy(net_para.gateway, (unsigned char *)&temp_addr, 4);

    /*dns addr convert*/
    temp_addr = inet_addr(p_ip->eth_dns);
    if (INADDR_NONE == temp_addr) {
      ALOGE("%s: input dns string is error: %s\n", __func__, p_ip->eth_dns);
      return -ENO_DNS;
    }
    memcpy(net_para.dns0, (unsigned char *)&temp_addr, 4);
    memcpy(net_para.dns1, (unsigned char *)&temp_addr, 4);

    if (eth_info.dhcp)
      NATIVE_ETH(net_dev_stop_dhcp(NET_DEV_NAME););

    ALOGD("%s: start set eth static ip >> %d\n", __func__, cur_state);
    if (ETH_LINK_UP < cur_state)
      notify_eth_state_change(ETH_LINK_UP);
    NATIVE_ETH(ret = net_dev_set_static_ip(NET_DEV_NAME, &net_para););
    if (ret) {
      ALOGE("failed to set static ip for net device %s, error:%d",
        NET_DEV_NAME, ret);
      NATIVE_ETH(net_dev_del_ip_config(NET_DEV_NAME););
      return -ENO_SET;
    } else {
      net_config_para_t tmp_para;
      memset(&tmp_para, 0, sizeof(net_config_para_t));
      ret = get_eth_para(&tmp_para);
      if (ret) {
        ALOGE("%s: get net para failed\n", __func__);
      }
      if (0 == strcmp(tmp_para.eth_ip, p_ip->eth_ip)) {
        notify_eth_state_change(ETH_STATIC_SUCCESS);
      } else {
        ALOGW("%s: id diff[%s] -> [%s]\n", __func__,
          p_ip->eth_ip, tmp_para.eth_ip);
        notify_eth_state_change(ETH_STATIC_FAIL);
        return -ENO_SET;
      }
    }
    memcpy(&eth_info, p_ip, sizeof(net_config_para_t));
  }

  return 0;
}

int eth_management::get_eth_para(net_config_para_t *p_ip)
{
  int ret = 0;
  ip_static_config_t net_para;

  if (NULL == p_ip) {
    ALOGE("net_wired_get_ip_para p_ip is NULL\n");
    return -1;
  }
  memset(&net_para, 0, sizeof(ip_static_config_t));
  ret = net_dev_get_static_ip(NET_DEV_NAME, &net_para);
  if (ret) {
    ALOGE("%s: get net para failed\n", __func__);
    return -1;
  }
  memcpy(p_ip, &eth_info, sizeof(net_config_para_t));
  snprintf(p_ip->eth_ip, sizeof(p_ip->eth_ip),
    "%d.%d.%d.%d", net_para.ip_addr[0], net_para.ip_addr[1],
    net_para.ip_addr[2], net_para.ip_addr[3]);
  snprintf(p_ip->eth_mask, sizeof(p_ip->eth_mask),
    "%d.%d.%d.%d", net_para.netmask[0], net_para.netmask[1],
    net_para.netmask[2], net_para.netmask[3]);
  snprintf(p_ip->eth_gateway, sizeof(p_ip->eth_gateway),
    "%d.%d.%d.%d", net_para.gateway[0], net_para.gateway[1],
    net_para.gateway[2], net_para.gateway[3]);
  snprintf(p_ip->eth_dns, sizeof(p_ip->eth_dns),
    "%d.%d.%d.%d", net_para.dns0[0], net_para.dns0[1],
    net_para.dns0[2], net_para.dns0[3]);
  ALOGD("%s:%d >> net_config_para_t[%d %d %d %s %s %s %s]",
    __func__, __LINE__, p_ip->enable, p_ip->priority, p_ip->dhcp,
    p_ip->eth_ip, p_ip->eth_mask, p_ip->eth_gateway,
    p_ip->eth_dns);
  return ret;
}

int eth_management::enable_dhcp()
{
  eth_info.dhcp = true;
  return set_eth_para(&eth_info);
}

