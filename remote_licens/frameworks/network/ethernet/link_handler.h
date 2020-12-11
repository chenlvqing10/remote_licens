/* link_handler.h */

#ifndef __LINK_HANDLER_H__
#define __LINK_HANDLER_H__

/**
 * enum eth_dev_link_status - ethernet device link status.
 * @ETH_DEV_LINK_UNKNOW: link status unknow.
 * @ETH_DEV_LINK_DOWN: link is down.
 * @ETH_DEV_LINK_UP: link is up.
 */
typedef enum eth_dev_link_status {
	ETH_DEV_LINK_UNKNOW = -1,
	ETH_DEV_LINK_DOWN = 0,
	ETH_DEV_LINK_UP = 1,
} eth_dev_link_status_t;

/**
 * eth_dev_link_handler - ethernet device link status callback handler.
 * @status: link status.
 */
typedef void (*eth_dev_link_handler)(eth_dev_link_status_t status);

/**
 * eth_dev_link_is_up - check if ethernet device is link up.
 * @dev_name: device name.
 *
 * return 0 if device is link down; 1 if device is link up;
 * otherwise some error happened.
 */
int eth_dev_link_is_up(const char *dev_name);

/**
 * eth_dev_start_link_detect - start link status detect process.
 * @dev_name: device name.
 *
 * This function should be called before you first call any function to
 * add, delete or clear link status handler.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_start_link_detect(const char *dev_name);

/**
 * eth_dev_stop_link_detect - stop link status detect process.
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_stop_link_detect(const char *dev_name);

/**
 * eth_dev_add_link_handler - add link status callback handler.
 * @dev_name: dev_name.
 * @handler: callback handler.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_add_link_handler(const char *dev_name,
			     eth_dev_link_handler handler);

/**
 * eth_dev_del_link_handler - delete link status callback handler.
 * @dev_name: dev_name.
 * @handler: callback handler.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_del_link_handler(const char *dev_name,
			     eth_dev_link_handler handler);

/**
 * eth_dev_clr_link_handler - delete all link status callback handler.
 * @dev_name: dev_name.
 *
 * return 0 if success; otherwise failed.
 */
int eth_dev_clr_link_handler(const char *dev_name);

#endif /* __LINK_HANDLER_H__ */
