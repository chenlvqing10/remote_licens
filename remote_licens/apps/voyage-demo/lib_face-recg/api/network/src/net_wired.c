/*
 * net_wired.c - define interface function of operatie wired network
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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define LOG_TAG "network"
#include <log/log.h>
#include "db_device_net_param.h"
#include "net_wired.h"
/*net inc*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "net_utils.h"
#include "ip_config.h"
#include "link_handler.h"
#include "ip_config.h"

net_wired_cb net_wired_event_deal_cb[NET_WIRED_CB_NO];
static void net_wired_dhcp_callback(int status);


void net_wired_send_event(int event, int flag)
{
	int i = 0;
	for (i = 0; i < NET_WIRED_CB_NO; i++) {
		if (net_wired_event_deal_cb[i])
			net_wired_event_deal_cb[i](event, flag);
	}
}


int net_wired_link_up_deal(void)
{
	int ret;
	device_net_param_t net_info;
	net_config_para_t net_para;

	/*zeroing global paramters*/
	memset(&net_info, 0x00, sizeof(net_info));
	memset(&net_para, 0x00, sizeof(net_para));

	/*read device net parameter*/
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return -1;
	}

	if (!net_info.enable_eth_dhcp) {
		memcpy(net_para.eth_ip, net_info.eth_ip, 32);
		memcpy(net_para.eth_mask, net_info.eth_mask, 32);
		memcpy(net_para.eth_gateway, net_info.eth_gateway, 32);
		memcpy(net_para.eth_dns, net_info.eth_dns, 32);
		ret = net_wired_set_ip_para(&net_para);
		if (ret) {
			ALOGE("%s: set net para failed\n", __func__);
			return -1;
		}
	} else {
		/*start dhcp*/
		net_dev_start_dhcp(NET_DEV_NAME,
			(dhcp_status_handler)net_wired_dhcp_callback);
	}

	return ret;
}

void net_wired_link_callback(int status)
{
	int link_ret = ETH_DEV_LINK_UNKNOW;

	/*
	**ETH_DEV_LINK_UNKNOW = -1,
	**ETH_DEV_LINK_DOWN = 0,
	**ETH_DEV_LINK_UP = 1,
	*/

	switch (status) {
	case ETH_DEV_LINK_UNKNOW:
		link_ret = eth_dev_link_unknow;
		break;

	case ETH_DEV_LINK_DOWN:
		link_ret = eth_dev_link_down;
		ALOGD("%s: link down and 清除网络IP配置\n", __func__);
		net_dev_del_ip_config(NET_DEV_NAME);
		break;

	case ETH_DEV_LINK_UP:
		net_wired_link_up_deal();
		link_ret = eth_dev_link_up;
		break;
	}
	ALOGD("%s: the current net link status is %d\n", __func__, status);
	net_wired_send_event(link_ret, NET_WIRED_EVENT);
}

static void net_wired_dhcp_callback(int status)
{
	int dhcp_ret = 0;
	/*
	**DHCP_ABORT = -2,
	**DHCP_TIMEOUT = -1,
	**DHCP_SUCCESS = 0,
	*/
	switch (status) {
	case DHCP_ABORT:
		dhcp_ret = eth_dev_dhcp_abort;
		break;
	case DHCP_TIMEOUT:
		dhcp_ret = eth_dev_dhcp_timeout;
		break;
	case DHCP_SUCCESS:
		dhcp_ret = eth_dev_dhcp_success;
		break;
	}
	ALOGD("%s: the current net dhcp result is %d\n", __func__, status);

	net_wired_send_event(dhcp_ret, NET_WIRED_EVENT);
}


/**
 * register wifi event deal callback function
 * @param p a pointer of callback function
 * @return NULL
 */
void net_wired_register_event_cb(net_wired_cb p)
{
	int ret = 0;
	int i = 0;

	if (!p) {
		ALOGE("%s: p is NULL", __func__);
		return;
	}
	/*lookup cb is exist*/
	for (i = 0; i < NET_WIRED_CB_NO; i++) {
		if ((p == net_wired_event_deal_cb[i]) &&
			(net_wired_event_deal_cb[i]))
			break;
	}
	for (i = 0; i < NET_WIRED_CB_NO; i++) {
		if (net_wired_event_deal_cb[i] == NULL) {
			net_wired_event_deal_cb[i] = p;
			break;
		}
	}
	/*check net whether link up*/
	ret = net_wired_check_link_status();
	if (ret == 1) {
		/*link up*/
		ALOGD("%s: ethernet is link up", __func__);
		if (p)
			net_wired_send_event(eth_dev_link_up, NET_WIRED_EVENT);
	}
}

/**
 * open wired network device
 * @param NULL
 * @return 0-success, other-failed
 */
int net_wired_open(void)
{
	int ret = 0;
	/*open net device*/
	ret = net_dev_open(NET_DEV_NAME);
	if (ret) {
		ALOGE("failed to open net device %s, error:%d", NET_DEV_NAME, ret);
		return -1;
	}
	return ret;
}

/**
 * close wired network device
 * @param NULL
 * @return 0-success, other-failed
 */
int net_wired_close(void)
{
	int ret = 0;
	/*close net device*/
	ret = net_dev_close(NET_DEV_NAME);
	if (ret) {
		ALOGE("failed to close net device %s, error:%d", NET_DEV_NAME, ret);
		return -1;
	}
	return ret;
}

/**
 * check status of wired network open
 * @param NULL
 * @return 1-open, other-failed
 */
int net_wired_check_open_status(void)
{
	return net_dev_is_open(NET_DEV_NAME);
}



/**
 * set static net device parameters
 * @param p_ip a pointer of ip address and so on
 * @return 0-success, other-failed
 */
int net_wired_set_ip_para(net_config_para_t *p_ip)
{
	int ret = 0;
	unsigned int temp_addr;
	ip_static_config_t net_para;

	if (!p_ip) {
		ALOGE("%s: para pointer is NULL, %p\n", __func__, p_ip);
		return -1;
	}
	/*init net para*/
	memset(&net_para, 0x00, sizeof(net_para));

	/*ip addr convert*/
	temp_addr = 0;
	temp_addr = inet_addr(p_ip->eth_ip);
	if (!temp_addr) {
		ALOGE("%s: input ip string is error: %s\n", __func__, p_ip->eth_ip);
		return -1;
	}
	memcpy(net_para.ip_addr, (unsigned char *)&temp_addr, 4);

	/*mask addr convert*/
	temp_addr = 0;
	temp_addr = inet_addr(p_ip->eth_mask);
	if (!temp_addr) {
		ALOGE("%s: input mask string is error: %s\n", __func__, p_ip->eth_mask);
		return -1;
	}
	memcpy(net_para.netmask, (unsigned char *)&temp_addr, 4);

	/*gateway addr convert*/
	temp_addr = 0;
	temp_addr = inet_addr(p_ip->eth_gateway);
	if (!temp_addr) {
		ALOGE("%s: input gateway string is error: %s\n", __func__,
			p_ip->eth_gateway);
		return -1;
	}
	memcpy(net_para.gateway, (unsigned char *)&temp_addr, 4);

	/*dns addr convert*/
	temp_addr = 0;
	temp_addr = inet_addr(p_ip->eth_dns);
	if (!temp_addr) {
		ALOGE("%s: input dns string is error: %s\n", __func__, p_ip->eth_dns);
		return -1;
	}
	memcpy(net_para.dns0, (unsigned char *)&temp_addr, 4);
	memcpy(net_para.dns1, (unsigned char *)&temp_addr, 4);

	/*set net para*/
	ret = net_dev_set_static_ip(NET_DEV_NAME, &net_para);
	if (ret) {
		ALOGE("failed to set static ip for net device %s, error:%d",
			NET_DEV_NAME, ret);
		return -1;
	}
	return 0;
}

/**
 * get ip address
 * @param p_ip a poniter of save ip addr parameter
 * @return NULL
 */
int net_wired_get_ip_para(net_config_para_t *p_ip)
{
	int ret = 0;
	ip_static_config_t net_para;

	ret = net_dev_get_static_ip(NET_DEV_NAME, &net_para);
	if (ret) {
		ALOGE("%s: get net para failed\n", __func__);
		return -1;
	}
	snprintf(p_ip->eth_ip, sizeof(p_ip->eth_ip), "%d.%d.%d.%d", net_para.ip_addr[0],
		net_para.ip_addr[1], net_para.ip_addr[2], net_para.ip_addr[3]);
	snprintf(p_ip->eth_mask, sizeof(p_ip->eth_mask), "%d.%d.%d.%d",
		net_para.netmask[0], net_para.netmask[1],
		net_para.netmask[2], net_para.netmask[3]);
	snprintf(p_ip->eth_gateway, sizeof(p_ip->eth_gateway), "%d.%d.%d.%d",
		net_para.gateway[0], net_para.gateway[1],
		net_para.gateway[2], net_para.gateway[3]);
	snprintf(p_ip->eth_dns, sizeof(p_ip->eth_dns), "%d.%d.%d.%d", net_para.dns0[0],
		net_para.dns0[1], net_para.dns0[2], net_para.dns0[3]);

	return 0;
}


/**
 * start net dhcp
 * @param p_cb a pointer of dhcp callback funtion
 * @return NULL
 */
void net_wired_start_dhcp(void *p_cb)
{

	net_dev_start_dhcp(NET_DEV_NAME, (dhcp_status_handler)p_cb);

}

/**
 * close net dhcp
 * @param NULL
 * @return NULL
 */
void net_wired_stop_dhcp(void)
{
	net_dev_stop_dhcp(NET_DEV_NAME);
}

/**
 * check the status of net link status
 * @param NULL
 * @return 0-disconnect, 1-connect, -1-check failed
 */
int net_wired_check_link_status(void)
{
	int ret = 0;
	/*close net device*/
	ret = eth_dev_link_is_up(NET_DEV_NAME);
	if (ret < 0) {
		ALOGE("%s: failed to checked", __func__);
		return -1;
	}
	return ret;
}
/**
 * start detect of net link status
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wired_start_link_detect(void)
{
	int ret = 0;
	/*close net device*/
	ret = eth_dev_start_link_detect(NET_DEV_NAME);
	if (ret) {
		ALOGE("%s: failed to started", __func__);
		return -1;
	}
	return ret;
}

/**
 * stop detect of net link status
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wired_stop_link_detect(void)
{
	int ret = 0;
	/*close net device*/
	ret = eth_dev_stop_link_detect(NET_DEV_NAME);
	if (ret) {
		ALOGE("%s: failed to stop", __func__);
		return -1;
	}
	return ret;
}

/**
 * get device net parameters
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wired_get_config_para(net_config_para_t *p_net)
{
	device_net_param_t net_info;
	int ret = 0;

	/*verify parameters*/
	if (!p_net) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}
	/*parameter init*/
	memset(&net_info, 0x00, sizeof(net_info));

	/*read device net parameter*/
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return -1;
	}

	/*data copy*/
	p_net->enable = net_info.enable_eth;
	p_net->priority = net_info.enable_eth_priority;
	p_net->dhcp = net_info.enable_eth_dhcp;

	memcpy(p_net->eth_ip, net_info.eth_ip, 32);
	memcpy(p_net->eth_mask, net_info.eth_mask, 32);
	memcpy(p_net->eth_gateway, net_info.eth_gateway, 32);
	memcpy(p_net->eth_dns, net_info.eth_dns, 32);

	return 0;
}
/**
 * update dev net parameters
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wired_set_config_para(net_config_para_t *p_net)
{
	int ret = 0;
	device_net_param_t net_info;

	/*verify parameters*/
	if (!p_net) {
		ALOGE("%s: parameter pointer is NULL\n", __func__);
		return -1;
	}

	/*read device net parameter*/
	memset(&net_info, 0x00, sizeof(net_info));
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return -1;
	}

	if ((net_dev_is_open(NET_DEV_NAME) == 0) && (p_net->enable)) {
		ret = net_dev_open(NET_DEV_NAME);
		if (ret) {
			ALOGE("%s: open net device failed\n", __func__);
			return -1;
		}
	} else if (net_dev_is_open(NET_DEV_NAME) && (!p_net->enable)) {
		net_dev_stop_dhcp(NET_DEV_NAME);
		net_wired_link_callback(ETH_DEV_LINK_DOWN);
		ret = net_dev_close(NET_DEV_NAME);
		if (ret) {
			ALOGE("%s: close net device failed\n", __func__);
			return -1;
		}
		eth_dev_del_link_handler(NET_DEV_NAME, net_wired_link_callback);
		eth_dev_stop_link_detect(NET_DEV_NAME);
	}

	if ((net_dev_is_open(NET_DEV_NAME) && p_net->enable)) {
		/*set para to net module*/
		if (p_net->dhcp)
			net_dev_start_dhcp(NET_DEV_NAME, net_wired_dhcp_callback);
		else {
			/*set ip and others parameters to net device*/
			ret = net_wired_set_ip_para(p_net);
			if (ret) {
				ALOGE("%s: update net parameter failed\n", __func__);
				return -1;
			}
			/*copy ip and other config parameters to net_info for saving*/
			snprintf(net_info.eth_ip, sizeof(p_net->eth_ip),
				"%s", p_net->eth_ip);
			snprintf(net_info.eth_mask, sizeof(p_net->eth_mask),
				"%s", p_net->eth_mask);
			snprintf(net_info.eth_gateway, sizeof(p_net->eth_gateway),
				"%s", p_net->eth_gateway);
			snprintf(net_info.eth_dns, sizeof(p_net->eth_dns),
				"%s", p_net->eth_dns);
		}
		/*start net status detects*/
		ret = eth_dev_start_link_detect(NET_DEV_NAME);
		if (ret) {
			ALOGE("%s: start link status detecte failed\n", __func__);
			return -1;
		}
		/*register link status callback function*/
		ret = eth_dev_add_link_handler(NET_DEV_NAME,
			net_wired_link_callback);
		if (ret) {
			ALOGE("%s: set link status cb failed\n", __func__);
			return -1;
		}
	}
	/*save parameter to database*/
	net_info.enable_eth = p_net->enable;
	net_info.enable_eth_dhcp = p_net->dhcp;
	net_info.enable_eth_priority = p_net->priority;
	ret = db_device_net_param_update(&net_info);
	if (ret) {
		ALOGE("%s: save para failed(%d)\n", __func__, ret);
		return ret;
	}

	return ret;
}


/**
 * init net mode
 * @param NULL
 * @return  0-success, other-failed
 */
void *net_wired_init_task(void *p)
{
	int check_cnt = 0;
	int ret;
	device_net_param_t net_info;
	net_config_para_t net_para;

	/*zeroing global paramters*/
	memset(&net_info, 0x00, sizeof(net_info));
	memset(&net_para, 0x00, sizeof(net_para));
	net_wired_register_event_cb((net_wired_cb)p);

	/*read device net parameter*/
	ret = db_device_net_param_query(&net_info);
	if (!ret) {
		ALOGE("%s: read device net para failed(%d)\n", __func__, ret);
		return NULL;
	}

	/*open net device*/
	if (!net_info.enable_eth) {
		ALOGD("%s: eth network is not been enable\n", __func__);
		return NULL;
	}
	ret = net_dev_open(NET_DEV_NAME);
	if (ret) {
		ALOGE("failed to open net device %s, error:%d\n", NET_DEV_NAME, ret);
		return NULL;
	}
	while (1) {
		sleep(1);
		ret = eth_dev_link_is_up(NET_DEV_NAME);
		if (ret <= 0) {
			ALOGE("%s: net wire is not linked", __func__);
			goto exit;
		} else
			break;
		ALOGD("%s: check_cnt = %d\n", __func__, check_cnt);
		check_cnt++;
		if (check_cnt > 5)
			goto exit;
	}
	if (!net_info.enable_eth_dhcp) {
		memcpy(net_para.eth_ip, net_info.eth_ip, 32);
		memcpy(net_para.eth_mask, net_info.eth_mask, 32);
		memcpy(net_para.eth_gateway, net_info.eth_gateway, 32);
		memcpy(net_para.eth_dns, net_info.eth_dns, 32);
		ret = net_wired_set_ip_para(&net_para);
		if (ret) {
			ALOGE("%s: set net para failed\n", __func__);
			goto exit;
		}
	} else {
		/*start dhcp*/
		net_dev_start_dhcp(NET_DEV_NAME,
			(dhcp_status_handler)net_wired_dhcp_callback);
	}

exit:
	/*start detected of net link*/
	ret = eth_dev_start_link_detect(NET_DEV_NAME);
	if (ret) {
		ALOGE("%s: failed to started", __func__);
		return NULL;
	}
	/*register link status change callback*/
	ret = eth_dev_add_link_handler(NET_DEV_NAME,
		(eth_dev_link_handler)net_wired_link_callback);
	if (ret) {
		ALOGE("%s: failed to register\n", __func__);
		return NULL;
	}
	return NULL;
}
int net_wired_init(void *p)
{
	pthread_t thread_wired_init;

	pthread_create(&thread_wired_init, NULL, net_wired_init_task, p);
	pthread_detach(thread_wired_init);

	return 0;
}

/**
 * deinit net mode
 * @param net_name a poniter of net device name
 * @return  0-success, other-failed
 */
int net_wired_deinit(void)
{
	int ret = 0;
	int i = 0;
	for (i = 0; i < NET_WIRED_CB_NO; i++)
		net_wired_event_deal_cb[i] = NULL;
	if (net_wired_check_open_status() == 0)
		return 0;
	/*stop dhcp*/
	net_dev_stop_dhcp(NET_DEV_NAME);
	/*stop detected of net link*/
	ret = eth_dev_stop_link_detect(NET_DEV_NAME);
	if (ret) {
		ALOGE("%s: failed to started", __func__);
		return -1;
	}
	/*close net device*/
	ret = net_dev_close(NET_DEV_NAME);
	if (ret) {
		ALOGE("failed to open net device %s, error:%d", NET_DEV_NAME, ret);
		return -1;
	}
	return ret;
}

