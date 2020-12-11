/* ip_config.c */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <features.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <net/if_arp.h>
#include <netinet/in.h>

#define  LOG_TAG		"net_utils"
#include <log/log.h>

#include "net_utils.h"
#include "ip_config.h"
#include "if_req.h"

#define PID_FILE_NAME_LEN	(40)
#define DHCP_HOSTNAME_PRE	"hostname:"

typedef struct dhcp_handle_info {
	char dev_name[IFNAMSIZ];
	dhcp_status_handler handler;
	pid_t pid;
} dhcp_handle_info_t;

typedef struct net_priority_info {
	int ethernet_priority;
	int wifi_priority;
	int mobile_priority;
} net_priority_info_t;

net_priority_info_t priority_info = {100, 200, 300};

/**
 * obtain_one_num - get the number of '1' in the binary number.
 * @n: binary number.
 *
 * return the number of '1'.
 */
static int obtain_one_num(unsigned int n)
{
	int count = 0;
	int flag = 1;
	while (n) {
		if (n & flag)
			count++;

		n >>= 1;
	}

	return count;
}

/**
 * sockaddr_in_init - init sockaddr_in.
 * @sa: pointer to sockaddr struct.
 * @addr: address.
 */
static void sockaddr_in_init(struct sockaddr *sa, unsigned long addr)
{
	struct sockaddr_in *sin = (struct sockaddr_in *)sa;
	sin->sin_family = AF_INET;
	sin->sin_port = 0;
	sin->sin_addr.s_addr = addr;
}

/**
 * prefix_length_to_ipv4_netmask - exchange prefix length to IPv4 netmask.
 * @prefix_length: prefix length.
 *
 * return netmask.
 */
static unsigned int prefix_length_to_ipv4_netmask(int prefix_length)
{
	unsigned int mask = 0;

	/* C99 (6.5.7): shifts of 32 bits have undefined results */
	if (prefix_length <= 0 || prefix_length > 32)
		return 0;

	mask = ~mask << (32 - prefix_length);
	mask = htonl(mask);

	return mask;
}

/**
 * ifc_act_on_ipv4_route - take some action on IPv4 route.
 * @action: action.
 * @dev_name: device name.
 * @dst: destination address.
 * @prefix_length: prefix length.
 * @gw: gateway.
 *
 * return 0 if success; otherwise failed.
 */
static int ifc_act_on_ipv4_route(int action, const char *dev_name,
				 struct in_addr dst, int prefix_length,
				 struct in_addr gw)
{
	int fd, ret;
	unsigned int netmask;
	struct rtentry rt;

	memset(&rt, 0, sizeof(rt));
	rt.rt_dst.sa_family = AF_INET;
	rt.rt_dev = (void *)dev_name;

	netmask = prefix_length_to_ipv4_netmask(prefix_length);
	sockaddr_in_init(&rt.rt_genmask, netmask);
	sockaddr_in_init(&rt.rt_dst, dst.s_addr);
	rt.rt_flags = RTF_UP;

	if (prefix_length == 32)
		rt.rt_flags |= RTF_HOST;

	if (gw.s_addr != 0) {
		rt.rt_flags |= RTF_GATEWAY;
		sockaddr_in_init(&rt.rt_gateway, gw.s_addr);
	}

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		ALOGE("socket() failed: %s", strerror(errno));
		return -1;
	}

	ret = ioctl(fd, action, &rt);
	if (ret < 0) {
		if (EEXIST == errno) {
			ret = 0;
		} else {
			ret = -errno;
			ALOGE("IOCTL action(%d) failed: %s",
			      action, strerror(errno));
		}
	}

	close(fd);

	return ret;
}

/**
 * ifc_manage_ipv4_route_priority - take some action on IPv4 route with priority.
 * @action: action.
 * @dev_name: device name.
 * @dst: destination address.
 * @prefix_length: prefix length.
 * @gw: gateway.
 *
 * return 0 if success; otherwise failed.
 */
static int ifc_manage_ipv4_route_priorty(int action, const char *dev_name,
					 struct in_addr dst, int prefix_length,
					 struct in_addr gw)
{
	int fd, ret;
	unsigned int netmask;
	struct rtentry rt;

	memset(&rt, 0, sizeof(rt));
	rt.rt_dst.sa_family = AF_INET;
	rt.rt_dev = (void *)dev_name;

	if (!strcmp(dev_name, "eth0"))
		rt.rt_metric = priority_info.ethernet_priority + 1;
	else if (!strcmp(dev_name, "wlan0"))
		rt.rt_metric = priority_info.wifi_priority + 1;
	else if (!strcmp(dev_name, "ppp0"))
		rt.rt_metric = priority_info.mobile_priority + 1;

	netmask = prefix_length_to_ipv4_netmask(prefix_length);
	sockaddr_in_init(&rt.rt_genmask, netmask);
	sockaddr_in_init(&rt.rt_dst, dst.s_addr);
	rt.rt_flags = RTF_UP;

	if (prefix_length == 32)
		rt.rt_flags |= RTF_HOST;

	if (gw.s_addr != 0) {
		rt.rt_flags |= RTF_GATEWAY;
		sockaddr_in_init(&rt.rt_gateway, gw.s_addr);
	}

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		ALOGE("socket() failed: %s", strerror(errno));
		return -1;
	}

	ret = ioctl(fd, action, &rt);
	if (ret < 0) {
		if (EEXIST == errno) {
			ret = 0;
		} else {
			ret = -errno;
			ALOGE("IOCTL action(%d) failed: %s",
			      action, strerror(errno));
		}
	}

	close(fd);

	return ret;
}

/**
 * create_udhcpc_pid_file_name - create udhcpc pid file name.
 * @dev_name: device name.
 * @file_name: file name.
 */
static void create_udhcpc_pid_file_name(const char *dev_name, char *file_name)
{
	char tmp_buf[IFNAMSIZ];

	memset(tmp_buf, 0, IFNAMSIZ);
	strncpy(tmp_buf, dev_name, IFNAMSIZ);
	tmp_buf[IFNAMSIZ - 1] = 0;

	memset(file_name, 0, PID_FILE_NAME_LEN);
	sprintf(file_name, "/var/run/udhcpc_%s.pid", tmp_buf);
	file_name[PID_FILE_NAME_LEN - 1] = 0;
}

/**
 * dhcp_handle_thread - DHCP handle thread function.
 * @args: pointer to DHCP handle info struct.
 *
 * return NULL.
 */
static void *dhcp_handle_thread(void *args)
{
	int status;
	dhcp_handle_info_t *handle_info;

	pthread_detach(pthread_self());

	handle_info = (dhcp_handle_info_t *)args;

	waitpid(handle_info->pid, &status, 0);

	ALOGD("wait dhcp process pid %d get status 0x%08x",
	      handle_info->pid, status);

	if (WIFEXITED(status)) {
		if (!WEXITSTATUS(status)) {
			ALOGI("dhcp for %s success", handle_info->dev_name);

			handle_info->handler(DHCP_SUCCESS);
		} else {
			ALOGI("dhcp for %s timeout", handle_info->dev_name);

			handle_info->handler(DHCP_TIMEOUT);
		}
	} else {
		ALOGI("dhcp for %s abort", handle_info->dev_name);

		handle_info->handler(DHCP_ABORT);
	}

	free(handle_info);

	pthread_exit(NULL);
}

/**
 * net_dev_set_ip_addr - net device set IP address.
 * @dev_name: device name.
 * @ip_addr: IP address to set.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_set_ip_addr(const char *dev_name, unsigned long ip_addr)
{
	int fd, ret;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		ALOGE("socket() failed: %s", strerror(errno));
		return -1;
	}

	ifreq_init(dev_name, &ifr);
	sockaddr_in_init(&ifr.ifr_addr, ip_addr);

	ret = ioctl(fd, SIOCSIFADDR, &ifr);
	if (ret < 0) {
		ALOGE("SIOCSIFADDR failed: %s", strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

/**
 * net_dev_get_ip_addr - net device get IP address.
 * @dev_name: device name.
 * @ip_addr: the IP address we get.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_get_ip_addr(const char *dev_name, unsigned long *ip_addr)
{
	int fd, ret = 0;
	struct ifreq ifr;

	if (!ip_addr) {
		ALOGE("invalid IP address buffer");
		return -1;
	}

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		ALOGE("socket() failed: %s", strerror(errno));
		return -1;
	}

	ifreq_init(dev_name, &ifr);

	ret = ioctl(fd, SIOCGIFADDR, &ifr);
	if (ret < 0) {
		ALOGE("SIOCGIFADDR failed: %s", strerror(errno));
		close(fd);
		return -1;
	}

	*ip_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;

	close(fd);

	return 0;
}

/**
 * net_dev_del_ip_config - net device delete IP config.
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_del_ip_config(const char *dev_name)
{
	char cmd[64];

	net_dev_set_ip_addr(dev_name, 0UL);

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(cmd, "/etc/init.d/ip_drop %s", dev_name);
	system(cmd);

	return 0;
}

/**
 * net_dev_set_netmask - net device set netmask.
 * @dev_name: device name.
 * @netmask: netmask to set.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_set_netmask(const char *dev_name, unsigned long netmask)
{
	int fd, ret;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		ALOGE("socket() failed: %s", strerror(errno));
		return -1;
	}

	ifreq_init(dev_name, &ifr);
	sockaddr_in_init(&ifr.ifr_netmask, netmask);

	ret = ioctl(fd, SIOCSIFNETMASK, &ifr);
	if (ret) {
		ALOGE("SIOCSIFNETMASK failed: %s", strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

/**
 * net_dev_get_netmask - net device get netmask.
 * @dev_name: device name.
 * @netmask: the netmask we get.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_get_netmask(const char *dev_name, unsigned long *netmask)
{
	int fd, ret = 0;
	struct ifreq ifr;

	if (!netmask) {
		ALOGE("invalid netmask buffer");
		return -1;
	}

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		ALOGE("socket() failed: %s", strerror(errno));
		return -1;
	}

	ifreq_init(dev_name, &ifr);

	ret = ioctl(fd, SIOCGIFNETMASK, &ifr);
	if (ret < 0) {
		ALOGE("SIOCGIFNETMASK failed: %s", strerror(errno));
		close(fd);
		return -1;
	}

	*netmask = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;

	close(fd);

	return 0;
}

/**
 * net_dev_remove_gateway - net device remove default gateway.
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_remove_gateway(const char *dev_name)
{
	int fd, ret;
	struct rtentry rt;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		ALOGE("socket() failed: %s", strerror(errno));
		return -1;
	}

	memset(&rt, 0, sizeof(rt));
	rt.rt_dev = (void *)dev_name;
	rt.rt_flags = RTF_UP|RTF_GATEWAY;
	sockaddr_in_init(&rt.rt_dst, 0);

	ret = ioctl(fd, SIOCDELRT, &rt);
	if ((ret < 0) && (errno != ESRCH)) {
		ALOGE("SIOCDELRT failed: %s", strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

/**
 * net_dev_set_gateway - net device set default gateway.
 * @dev_name: device name.
 * @gateway: gateway to set.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_set_gateway(const char *dev_name, unsigned long gateway)
{
	int ret;
	struct in_addr in_dst, in_gw;

	in_dst.s_addr = 0;
	in_gw.s_addr = gateway;

	ret = net_dev_remove_gateway(dev_name);
	if (ret) {
		ALOGE("failed to remove default route for net device %s",
		      dev_name);
		return -1;
	}

	ret = ifc_act_on_ipv4_route(SIOCADDRT, dev_name, in_dst, 0, in_gw);
	if (ret) {
		ALOGE("failed to set default route for net device %s",
		      dev_name);
		return -1;
	}

	return 0;
}

/**
 * net_dev_get_gateway - net device get default gateway.
 * @dev_name: device name.
 * @gateway: the gateway we get.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_get_gateway(const char *dev_name, unsigned long *gateway)
{
	FILE *fp;
	char buf[256];
	char iface[IFNAMSIZ];
	unsigned long dest_addr, gate_addr;

	*gateway = INADDR_NONE;

	fp = fopen("/proc/net/route", "r");
	if (!fp) {
		ALOGE("failed to open file /proc/net/route");
		return -1;
	}

	/* skip title line */
	fgets(buf, sizeof(buf), fp);
	while (fgets(buf, sizeof(buf), fp)) {
		if (sscanf(buf, "%s\t%lx\t%lx",
		    iface, &dest_addr, &gate_addr) != 3)
			continue;
		if (dest_addr != 0)
			continue;
		if (strcmp(dev_name, iface))
			continue;
		*gateway = gate_addr;
		break;
	}

	fclose(fp);

	if (INADDR_NONE == (*gateway)) {
		ALOGE("failed to found default route of net device %s",
		      dev_name);
		return -1;
	}

	return 0;
}

/**
 * net_dev_set_dns_ip - net device set dns server's IP address.
 * @dev_name: device name.
 * @dns0: first choice dns server's IP address.
 * @dns1: additional dns server's IP address.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_set_dns_ip(const char *dev_name, unsigned long dns0,
		       unsigned long dns1)
{
	char cmd[256];

	memset(cmd, 0x00, sizeof(cmd));

	if (dns0 || dns1) {
		if (dns0 && dns1) {
			sprintf(cmd, "/etc/init.d/dns_config %s %d.%d.%d.%d "
				"%d.%d.%d.%d", dev_name,
				(int)(dns0&0xFF),
				(int)((dns0 >> 8)&0xFF),
				(int)((dns0 >> 16)&0xFF),
				(int)((dns0 >> 24)&0xFF),
				(int)(dns1&0xFF),
				(int)((dns1 >> 8)&0xFF),
				(int)((dns1 >> 16)&0xFF),
				(int)((dns1 >> 24)&0xFF));
		} else if (dns0) {
			sprintf(cmd, "/etc/init.d/dns_config %s %d.%d.%d.%d",
				dev_name,
				(int)(dns0&0xFF),
				(int)((dns0 >> 8)&0xFF),
				(int)((dns0 >> 16)&0xFF),
				(int)((dns0 >> 24)&0xFF));
		} else {
			sprintf(cmd, "/etc/init.d/dns_config %s %d.%d.%d.%d",
				dev_name,
				(int)(dns1&0xFF),
				(int)((dns1 >> 8)&0xFF),
				(int)((dns1 >> 16)&0xFF),
				(int)((dns1 >> 24)&0xFF));
		}

		system(cmd);
	}

	return 0;
}

/**
 * net_dev_set_dns_ip - net device get dns server's IP address.
 * @dev_name: device name.
 * @dns0: first choice dns server's IP address.
 * @dns1: additional dns server's IP address.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_get_dns_ip(const char *dev_name, unsigned long *dns0,
		       unsigned long *dns1)
{
	FILE *fp;
	char tmp, title[256], buf[256], iface[IFNAMSIZ];
	int ip0, ip1, ip2, ip3;

	*dns0 = 0;
	*dns1 = 0;

	fp = fopen("/etc/resolv.conf", "r");
	if (!fp) {
		ALOGE("failed to open file /etc/resolv.conf");
		return -1;
	}

	while (fgets(buf, sizeof(buf), fp)) {
		if (sscanf(buf, "%s\t%d.%d.%d.%d\t%c\t%s",
		    title, &ip0, &ip1, &ip2, &ip3, &tmp, iface) != 7)
			continue;
		if (strcmp("nameserver", title))
			continue;
		if (strcmp(dev_name, iface))
			continue;
		if ('#' != tmp)
			continue;
		if (!(*dns0)) {
			*dns0 = (unsigned long)((ip0 & 0xFF) |
				((ip1 & 0xFF) << 8) | ((ip2 & 0xFF) << 16) |
				((ip3 & 0xFF) << 24));
		} else {
			*dns1 = (unsigned long)((ip0 & 0xFF) |
				((ip1 & 0xFF) << 8) | ((ip2 & 0xFF) << 16) |
				((ip3 & 0xFF) << 24));
			break;
		}
	}

	fclose(fp);

	return 0;
}

/**
 * net_dev_change_gateway_priority - net device change priority of gateway.
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
static int net_dev_change_gateway_priority(const char *dev_name)
{
	unsigned long gateway;
	int ret;
	struct in_addr in_dst, in_gw;

	ret = net_dev_get_gateway(dev_name, &gateway);
	if (ret) {
		ALOGE("failed to get gateway for %s\n", dev_name);
		goto exit;
	}

	ret = net_dev_remove_gateway(dev_name);
	if (ret) {
		ALOGE("failed to remove default route for net device %s",
		      dev_name);
		goto exit;
	}

	in_dst.s_addr = 0;
	in_gw.s_addr = gateway;

	ret = ifc_manage_ipv4_route_priorty(SIOCADDRT, dev_name,
					    in_dst, 0, in_gw);
	if (ret) {
		ALOGE("failed to set priority of gateway for net device %s",
			dev_name);
		goto exit;
	}

exit:
	return ret;
}

/**
 * net_dev_change_route_priority - net device change priority of route.
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
static int net_dev_change_route_priority(const char *dev_name)
{
	int ret, prefix_length, division_result, remainder, tmp;
	unsigned long ip_addr, netmask;
	struct in_addr in_dst, in_gw;

	ret = net_dev_get_ip_addr(dev_name, &ip_addr);
	if (ret) {
		ALOGE("failed to get IP Address for %s", dev_name);
		goto exit;
	}

	ret = net_dev_get_netmask(dev_name, &netmask);
	if (ret) {
		ALOGE("failed to get netmask for %s", dev_name);
		goto exit;
	}

	/* obtain network address */
	prefix_length = obtain_one_num(netmask);
	division_result = (32 - prefix_length) / 8;
	remainder = (32 - prefix_length) % 8;
	switch (division_result) {
	case 0:
		tmp = (0xFF000000 << remainder) | (0xFFFFFF);
		break;
	case 1:
		tmp = (0xFF0000 << remainder) | (0xFFFF);
		break;
	case 2:
		tmp = (0xFF00 << remainder) | (0xFF);
		break;
	case 3:
		tmp = 0xFF << remainder;
		break;
	default:
		ALOGE("failed to obtain network address %s", dev_name);
		goto exit;
	}
	in_dst.s_addr = ip_addr & tmp;

	in_gw.s_addr = 0;

	ret = ifc_act_on_ipv4_route(SIOCDELRT, dev_name, in_dst,
				    prefix_length, in_gw);
	if (ret) {
		ALOGE("failed to reomve the route for net device %s",
		      dev_name);
		goto exit;
	}

	ret = ifc_manage_ipv4_route_priorty(SIOCADDRT, dev_name, in_dst,
					    prefix_length, in_gw);
	if (ret) {
		ALOGE("failed to set priority of route for net device %s",
		      dev_name);
		goto exit;
	}

exit:
	return ret;
}

/**
 * net_dev_set_static_ip - set static IP configuration.
 * @dev_name: device name.
 * @ip_config: static IP configuration.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_set_static_ip(const char *dev_name,
			  const ip_static_config_t *ip_config)
{
	int ret;
	unsigned long ip_addr, netmask, gateway, dns0, dns1;

	if (!dev_name) {
		ALOGE("invalid net device name");
		return -1;
	}

	if (!ip_config) {
		ALOGE("invalid static IP config");
		return -1;
	}

	ip_addr = ((ip_config->ip_addr)[3] << 24) |
		  ((ip_config->ip_addr)[2] << 16) |
		  ((ip_config->ip_addr)[1] << 8) |
		  (ip_config->ip_addr)[0];
	netmask = ((ip_config->netmask)[3] << 24) |
		  ((ip_config->netmask)[2] << 16) |
		  ((ip_config->netmask)[1] << 8) |
		  (ip_config->netmask)[0];
	gateway = ((ip_config->gateway)[3] << 24) |
		  ((ip_config->gateway)[2] << 16) |
		  ((ip_config->gateway)[1] << 8) |
		  (ip_config->gateway)[0];
	dns0    = ((ip_config->dns0)[3] << 24) |
		  ((ip_config->dns0)[2] << 16) |
		  ((ip_config->dns0)[1] << 8) |
		  (ip_config->dns0)[0];
	dns1    = ((ip_config->dns1)[3] << 24) |
		  ((ip_config->dns1)[2] << 16) |
		  ((ip_config->dns1)[1] << 8) |
		  (ip_config->dns1)[0];

	ALOGD("ipconfig ip:%d.%d.%d.%d mask:%d.%d.%d.%d route:%d.%d.%d.%d"
	      " dns0:%d.%d.%d.%d dns1:%d.%d.%d.%d",
	      (ip_config->ip_addr)[0], (ip_config->ip_addr)[1],
	      (ip_config->ip_addr)[2], (ip_config->ip_addr)[3],
	      (ip_config->netmask)[0], (ip_config->netmask)[1],
	      (ip_config->netmask)[2], (ip_config->netmask)[3],
	      (ip_config->gateway)[0], (ip_config->gateway)[1],
	      (ip_config->gateway)[2], (ip_config->gateway)[3],
	      (ip_config->dns0)[0], (ip_config->dns0)[1],
	      (ip_config->dns0)[2], (ip_config->dns0)[3],
	      (ip_config->dns1)[0], (ip_config->dns1)[1],
	      (ip_config->dns1)[2], (ip_config->dns1)[3]);

	if (ip_addr) {
		ret = net_dev_set_ip_addr(dev_name, ip_addr);
		if (ret) {
			ALOGE("failed to set IP Address for %s", dev_name);
			return -1;
		}
	}

	if (netmask) {
		ret = net_dev_set_netmask(dev_name, netmask);
		if (ret) {
			ALOGE("failed to set netmask for %s", dev_name);
			return -1;
		}
	}

	if (gateway) {
		ret = net_dev_set_gateway(dev_name, gateway);
		if (ret) {
			ALOGE("failed to set gateway for %s", dev_name);
			return -1;
		}
	}

	if (dns0 || dns1) {
		ret = net_dev_set_dns_ip(dev_name, dns0, dns1);
		if (ret) {
			ALOGE("failed to set dns server's IP for %s", dev_name);
			return -1;
		}
	}

	return 0;
}

/**
 * net_dev_get_static_ip - get static IP configuration.
 * @dev_name: device name.
 * @ip_config: static IP configuration.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_get_static_ip(const char *dev_name, ip_static_config_t *ip_config)
{
	int ret;
	unsigned long ip_addr, netmask, gateway, dns0, dns1;

	if (!dev_name) {
		ALOGE("invalid net device name");
		return -1;
	}

	if (!ip_config) {
		ALOGE("invalid static IP config");
		return -1;
	}

	ret = net_dev_get_ip_addr(dev_name, &ip_addr);
	if (ret) {
		ALOGE("failed to get IP Address for %s", dev_name);
		return -1;
	}

	ret = net_dev_get_netmask(dev_name, &netmask);
	if (ret) {
		ALOGE("failed to get netmask for %s", dev_name);
		return -1;
	}

	ret = net_dev_get_gateway(dev_name, &gateway);
	if (ret) {
		ALOGE("failed to get gateway for %s", dev_name);
		return -1;
	}

	ret = net_dev_get_dns_ip(dev_name, &dns0, &dns1);
	if (ret)
		ALOGD("not dns server's IP address for %s was set", dev_name);

	(ip_config->ip_addr)[3] = (ip_addr >> 24) & 0xFF;
	(ip_config->ip_addr)[2] = (ip_addr >> 16) & 0xFF;
	(ip_config->ip_addr)[1] = (ip_addr >> 8) & 0xFF;
	(ip_config->ip_addr)[0] = ip_addr & 0xFF;

	(ip_config->netmask)[3] = (netmask >> 24) & 0xFF;
	(ip_config->netmask)[2] = (netmask >> 16) & 0xFF;
	(ip_config->netmask)[1] = (netmask >> 8) & 0xFF;
	(ip_config->netmask)[0] = netmask & 0xFF;

	(ip_config->gateway)[3] = (gateway >> 24) & 0xFF;
	(ip_config->gateway)[2] = (gateway >> 16) & 0xFF;
	(ip_config->gateway)[1] = (gateway >> 8) & 0xFF;
	(ip_config->gateway)[0] = gateway & 0xFF;

	(ip_config->dns0)[3]	= (dns0 >> 24) & 0xFF;
	(ip_config->dns0)[2]	= (dns0 >> 16) & 0xFF;
	(ip_config->dns0)[1]	= (dns0 >> 8) & 0xFF;
	(ip_config->dns0)[0]	= dns0 & 0xFF;

	(ip_config->dns1)[3]	= (dns1 >> 24) & 0xFF;
	(ip_config->dns1)[2]	= (dns1 >> 16) & 0xFF;
	(ip_config->dns1)[1]	= (dns1 >> 8) & 0xFF;
	(ip_config->dns1)[0]	= dns1 & 0xFF;

	ALOGD("ipconfig ip:%d.%d.%d.%d mask:%d.%d.%d.%d route:%d.%d.%d.%d"
	      " dns0:%d.%d.%d.%d dns1:%d.%d.%d.%d",
	      (ip_config->ip_addr)[0], (ip_config->ip_addr)[1],
	      (ip_config->ip_addr)[2], (ip_config->ip_addr)[3],
	      (ip_config->netmask)[0], (ip_config->netmask)[1],
	      (ip_config->netmask)[2], (ip_config->netmask)[3],
	      (ip_config->gateway)[0], (ip_config->gateway)[1],
	      (ip_config->gateway)[2], (ip_config->gateway)[3],
	      (ip_config->dns0)[0],    (ip_config->dns0)[1],
	      (ip_config->dns0)[2],    (ip_config->dns0)[3],
	      (ip_config->dns1)[0],    (ip_config->dns1)[1],
	      (ip_config->dns1)[2],    (ip_config->dns1)[3]);

	return 0;
}

/**
 * net_dev_change_priority - net device change priority.
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
int net_dev_change_priority(const char *dev_name)
{
	int ret;

	ret = net_dev_change_gateway_priority(dev_name);
	if (ret) {
		ALOGE("failed to change gateway priority for %s\n", dev_name);
		goto exit;
	}

	ret = net_dev_change_route_priority(dev_name);
	if (ret) {
		ALOGE("failed to change route priority for %s\n", dev_name);
		goto exit;
	}

exit:
	return ret;
}

/**
 * net_dev_start_dhcp - start DHCP.
 * @dev_name: device name.
 * @handler: DHCP status handler.
 */
void net_dev_start_dhcp(const char *dev_name, dhcp_status_handler handler)
{
	pid_t pid;
	char file_name[PID_FILE_NAME_LEN];
	char host_name[64], temp_buf[32];
	unsigned char mac_addr[6];

	ALOGD("start dhcp for device %s", dev_name);

	create_udhcpc_pid_file_name(dev_name, file_name);

	/* hostname is just as: virgo-9a5e68e5f66a */
	memset(host_name, 0, sizeof(host_name));
	memset(temp_buf, 0, sizeof(temp_buf));
	memset(mac_addr, 0, sizeof(mac_addr));

	if (gethostname(temp_buf, sizeof(temp_buf))) {
		ALOGW("failed to get host name");
		strcpy(temp_buf, "unknown");
	}

	if (net_dev_get_mac_addr(dev_name, mac_addr))
		ALOGW("failed to get mac address");

	strcpy(host_name, DHCP_HOSTNAME_PRE);
	strcat(host_name, temp_buf);
	memset(temp_buf, 0 ,sizeof(temp_buf));
	sprintf(temp_buf, "-%02x%02x%02x%02x%02x%02x",
		mac_addr[0], mac_addr[1], mac_addr[2],
		mac_addr[3], mac_addr[4], mac_addr[5]);
	strcat(host_name, temp_buf);

	net_dev_stop_dhcp(dev_name);

	pid = vfork();
	if (pid < 0) {
		ALOGE("failed to vfork() for udhcpc");

		handler(DHCP_ERROR);
	} else if (pid > 0) {
		pthread_t tid;
		dhcp_handle_info_t *handle_info;

		handle_info = malloc(sizeof(dhcp_handle_info_t));
		memset(handle_info->dev_name, 0, sizeof(handle_info->dev_name));
		strncpy(handle_info->dev_name, dev_name, IFNAMSIZ);
		handle_info->dev_name[IFNAMSIZ - 1] = 0;
		handle_info->handler = handler;
		handle_info->pid = pid;

		pthread_create(&tid, NULL, &dhcp_handle_thread, handle_info);

#if 0
		/* delay for the pid file to be created */
		usleep(1);
#endif
	} else {
#if 0
		/* Our original intention is to make sure the udhcpc process
		 * exits when the parent process exits.
		 * But PR_SET_PDEATHSIG may be not a good choice, because the
		 * udhcpc process will be killed when the thread that called
		 * this function(net_dev_start_dhcp) exit. */
		prctl(PR_SET_PDEATHSIG, SIGHUP);
#endif

		/* udhcpc -n -q -T 3 -t 5 -i <dev_name> -p <file_name> */
		execl("/sbin/udhcpc", "udhcpc", "-n", "-q", "-T", "3", "-t",
		      "5", "-i", dev_name, "-p", file_name, "-x", host_name,
		      NULL);
	}
}

/**
 * net_dev_stop_dhcp - stop DHCP.
 * @dev_name: device name.
 */
void net_dev_stop_dhcp(const char *dev_name)
{
	int fd, pid, ret;
	char pid_str[16];
	char file_name[PID_FILE_NAME_LEN];

	create_udhcpc_pid_file_name(dev_name, file_name);

	fd = open(file_name, O_RDONLY);
	if (fd < 0) {
		ALOGD("file %s not exist", file_name);
		return;
	}

	unlink(file_name);

	memset(pid_str, 0, sizeof(pid_str));
	ret = read(fd, pid_str, sizeof(pid_str));
	if (ret < 0) {
		ALOGE("failed to read file %s", file_name);
		goto exit_close_file;
	}

	pid_str[sizeof(pid_str) - 1] = 0;
	pid = atoi(pid_str);
	if (pid) {
		ALOGD("kill udhcpc process, dev_name %s, pid %d",
		      dev_name, pid);
		kill(pid, SIGKILL);
	}

exit_close_file:
	close(fd);
}
