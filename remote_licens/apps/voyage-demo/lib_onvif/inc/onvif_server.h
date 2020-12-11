#ifndef __ONVIF_SERVER_H__
#define __ONVIF_SERVER_H__

#define LARGE_INFO_LENGTH 1024
#define MACH_ADDR_LENGTH 32
#define INFO_LENGTH 1024
#define SMALL_INFO_LENGTH 512

#define HI_PLATFORM_ONVIF_SUBVERSION 14

#define DISC_SVR_PORT 3702
#define INTER_SVR_PORT 8000
#define DEF_SLEEP_TIME 5
#define MAX_SUPP_INTER_THRD 4

#define EXTERNAL_SVR_PORT 2085

typedef struct _HI_ONVIF_SOCKET_S_ {
	int eth0Socket;
	int wifiSocket;
} HI_ONVIF_SOCKET_S, *LPHI_ONVIF_SOCKET_S;

HI_ONVIF_SOCKET_S stOnvifSocket;

/**
 * onvif_get_ip_addr: get ip addr information
 *
 * return ip address.
 */
const char *onvif_get_ip_addr();

/**
 * onvif_get_server_port: get server port information
 *
 * return server port.
 */
int onvif_get_server_port();

/**
 * onvif_get_major_version: get onvif major version information
 *
 * return major version information.
 */
int onvif_get_major_version();

/**
 * onvif_get_minor_version: get minor version information
 *
 * return minor version.
 */
int onvif_get_minor_version();

/**
 * onvif_get_mac_addr: get mac address
 * @macaddr: a pointer to mac address 
 * @len: length of mac address buffer.
 *
 * return minor version.
 */
int onvif_get_mac_addr(char *macaddr, int len);

/**
 * get_local_ip: get local ip address
 * @iface_name: device name
 *
 * Returns ip address string.
 */
const char *get_local_ip(const char *iface_name);

/**
 * set_local_ip: set local ip address
 * @iface_name: device name
 * @ip: ip address
 *
 * Returns ip address string.
 */
int set_local_ip(const char* iface_name, const char *ip);

/**
 * onvif_nvt_start_service: start onvif nvt service
 * @NULL
 * Returns 0 means success, other means failure.
 */
int onvif_nvt_start_service();

/**
 * onvif_nvt_stop_service: stop onvif nvt service
 * @NULL
 * Returns 0 means success, other means failure.
 */
int onvif_nvt_stop_service();

#endif /* __ONVIF_SERVER_H__ */

