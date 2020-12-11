/*
 * net_wired.h - export some interface function of operatie wired network
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

#ifndef _NET_WIRED_H
#define _NET_WIRED_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define NET_DEV_NAME		"eth0"
#define NET_WIRED_EVENT 0xFE
#define NET_WIRED_CB_NO 5
typedef void (*net_wired_cb)(int, int);

typedef enum {
	eth_dev_link_unknow = 0,
	eth_dev_link_down,
	eth_dev_link_up,
	eth_dev_dhcp_abort,
	eth_dev_dhcp_timeout,
	eth_dev_dhcp_success,
} eth_dev_event;

typedef struct tag_net_para {
	/*net enable*/
	char enable;

	/* Eth priority
	 * Options:
	 *	0: Eth
	 *	1: WiFi
	 */
	char priority;

	/* DHCP
	 * Options:
	 *	0: Disable
	 *	1: Enable
	 */
	char dhcp;

	/* Eth ip address */
	char eth_ip[32];

	/* Eth subnet mask */
	char eth_mask[32];

	/* Eth gateway */
	char eth_gateway[32];

	/* Eth DNS */
	char eth_dns[32];
} net_config_para_t;

/**
 * register wifi event deal callback function
 * @param p a pointer of callback function
 * @return NULL
 */
void net_wired_register_event_cb(net_wired_cb p);


/**
 * open wired network device
 * @param NULL
 * @return 0-success, other-failed
 */
int net_wired_open(void);

/**
 * close wired network device
 * @param NULL
 * @return 0-success, other-failed
 */
int net_wired_close(void);

/**
 * set static net device parameters
 * @param p_ip a pointer of ip address
 * @param p_mask apointer of submask address
 * @param p_gateway apointer of gateway address
 * @param p_dns apointer of dns address
 * @return 0-success, other-failed
 */
int net_wired_set_ip_para(net_config_para_t *p_ip);

/**
 * get ip address
 * @param p_ip a poniter of saveing ip addr parameter
 * @return NULL
 */
int net_wired_get_ip_para(net_config_para_t *p_ip);

/**
 * start net dhcp
 * @param p_cb a pointer of dhcp callback funtion
 * @return NULL
 */
void net_wired_start_dhcp(void *p_cb);

/**
 * close net dhcp
 * @param NULL
 * @return NULL
 */
void net_wired_stop_dhcp(void);

/**
 * check the status of net link status
 * @param NULL
 * @return 0-disconnect, 1-connect, -1-check failed
 */
int net_wired_check_link_status(void);

/**
 * start detect of net link status
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wired_start_link_detect(void);

/**
 * stop detect of net link status
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wired_stop_link_detect(void);


/**
 * get device net parameters
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wired_get_config_para(net_config_para_t *p_net);

/**
 * update dev net parameters
 * @param p a pointer of net parameters
 * @return 0-successful,other-failed
 */
int net_wired_set_config_para(net_config_para_t *p_net);

/**
 * init net mode
 * @param NULL
 * @return  0-success, other-failed
 */
int net_wired_init(void *p);

/**
 * deinit net mode
 * @param net_name a poniter of net device name
 * @return  0-success, other-failed
 */
int net_wired_deinit(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

