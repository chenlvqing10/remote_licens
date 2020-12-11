#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <fcntl.h>
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

#include <ifaddrs.h>
#include <net/if.h>

#include "config.h"
#include "at_parser.h"

#define  LOG_TAG		"modem_ops"
#include <log/log.h>

#define PPP_NAME		"ppp0"
#define RESET_ACTIVE		"/sys/class/mobile_pm/fg_pin/reset_active"
#define RESET_INVALID		"/sys/class/mobile_pm/fg_pin/reset_invalid"
#define PPP_PID			"/var/run/ppp0.pid"

#define CMD_LEN			128

/**
 * get_ip_addr - net device get IP address.
 *
 *@ip_addr: network interface IP value
 *
 * return 0 if success; otherwise failed.
 */
static int get_ip_addr(unsigned long *ip_addr)
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

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, PPP_NAME, 4);
	ifr.ifr_name[4] = 0;

	ret = ioctl(fd, SIOCGIFADDR, &ifr);
	if (ret < 0) {
		ALOGI("SIOCGIFADDR failed: %s", strerror(errno));
		close(fd);
		return -1;
	}

	*ip_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;

	close(fd);

	return ret;
}

/**
 * modem_open - start modem module
 *
 * if open success return 0
 */
int modem_open(void)
{
	int ret = 0;
	int len;
	int i;
	char tty_path[128] = {0};
	char cmd[CMD_LEN] = {0};
	struct common_resp common_r;
	struct ccid_resp ccid_r;
	struct cpin_resp cpin_r;
	struct csq_resp csq_r;
	struct creg_resp creg_r;
	struct cgatt_resp cgatt_r;
	struct sysinfo_resp sysinfo_r;
	unsigned long ip_addr;

	/* 1. check parameters */
	/* 1.1 whether pppd already running */
	ret = access(PPP_PID, 0);
	if (ret == 0) {
		ALOGE("modem had already running, please check.\n");
		return -1;
	}

	/* 1.2 get ttyACM path */
	ret = at_get_tty_path(tty_path, &len);
	if (ret != 0) {
		ALOGE("no find at tty device path %d\n", ret);
		return -1;
	}

	/* 1.3 adjust power control file whether exist */
	ret = access(RESET_ACTIVE, 0);
	if (ret != 0) {
		ALOGE("no find %s\n", RESET_ACTIVE);
		return -1;
	}
	ret = access(RESET_INVALID, 0);
	if (ret != 0) {
		ALOGE("no find %s\n", RESET_INVALID);
		return -1;
	}

	/* 2. power control */
	/* 2.1 reset modem and wait 3s */
	sprintf(cmd, "cat %s", RESET_ACTIVE);
	system(cmd);
	sleep(3);

	/* 2.2 active modem and wait 3s */
	memset(cmd, 0, CMD_LEN);
	sprintf(cmd, "cat %s", RESET_INVALID);
	system(cmd);
	sleep(13);

	/* 3. check ttyACM AT port whether exist */
	for (i = 0; i < 5; i++) {
		ret = access(tty_path, 0);
		if (ret != 0) {
			sleep(2);
			continue;
		}
		break;
	}

	if (i >= 5) {
		ALOGE("no find %s\n", tty_path);
		return -1;
	}

	/* 4. send AT command, init modem */
	/* 4.1 AT command */
	for (i = 0; i < 15; i++) {
		memset(&common_r, 0, sizeof(struct common_resp));
		ret = at_cmd_check_at(&common_r);
		if (ret != 0) {
			ALOGE("at_cmd_check_at fail %d\n", ret);
			return -1;
		} else {
			if (common_r.status != AT_OK) {
				sleep(1);
				continue;
			}
			break;
		}
	}

	if (i >= 15) {
		ALOGE("modem could not response AT command.\n");
		return -1;
	}

	/* 4.2 CCID command */
	for (i = 0; i < 5; i++) {
		memset(&ccid_r, 0, sizeof(struct ccid_resp));
		ret = at_cmd_get_ccid(&ccid_r);
		if (ret != 0) {
			ALOGE("at_cmd_get_ccid fail %d\n", ret);
			return -1;
		} else {
			if (ccid_r.status != AT_OK) {
				sleep(1);
				continue;
			}
			break;
		}
	}

	if (i >= 5) {
		ALOGE("modem could not response CCID command.\n");
		return -1;
	}

	/* 4.3 CPIN */
	for (i = 0; i < 5; i++) {
		memset(&cpin_r, 0, sizeof(struct cpin_resp));
		ret = at_cmd_get_cpin(&cpin_r);
		if (ret != 0) {
			ALOGE("at_cmd_get_cpin fail %d\n", ret);
			return -1;
		} else {
			if (cpin_r.status != AT_OK) {
				sleep(1);
				continue;
			}
			/* if SIM card ok, is_ready == 1 */
			if (cpin_r.is_ready != 1) {
				sleep(1);
				continue;
			}

			break;
		}
	}

	if (i >= 5) {
		ALOGE("modem could not response CPIN command.\n");
		return -1;
	}

	/* 4.4 CSQ */
	for (i = 0; i < 20; i++) {
		memset(&csq_r, 0, sizeof(struct csq_resp));
		ret = at_cmd_get_csq(&csq_r);
		if (ret != 0) {
			ALOGE("at_cmd_get_csq fail %d\n", ret);
			return -1;
		} else {
			if (csq_r.status != AT_OK) {
				sleep(1);
				continue;
			}
			/* check signal value, 99 mean no signal */
			if ((csq_r.signal == 99) && (csq_r.ber == 99)) {
				sleep(1);
				continue;
			}

			break;
		}
	}

	if (i >= 20) {
		ALOGE("modem could not response CSQ command or no signal.\n");
		return -1;
	}

	/* 4.5 CREG */
	for (i = 0; i < 20; i++) {
		memset(&creg_r, 0, sizeof(struct creg_resp));
		ret = at_cmd_get_creg(&creg_r);
		if (ret != 0) {
			ALOGE("at_cmd_get_creg fail %d\n", ret);
			return -1;
		} else {
			if (creg_r.status != AT_OK) {
				sleep(1);
				continue;
			}
			/* check network status */
			if ((creg_r.stat == 1) || (creg_r.stat == 5))
				break;

			sleep(1);
			continue;
		}
	}

	if (i >= 20) {
		ALOGE("modem could not response CREG command or no network.\n");
		return -1;
	}

	/* 4.6 CGATT */
	for (i = 0; i < 10; i++) {
		memset(&cgatt_r, 0, sizeof(struct cgatt_resp));
		ret = at_cmd_get_cgatt(&cgatt_r);
		if (ret != 0) {
			ALOGE("at_cmd_get_cgatt fail %d\n", ret);
			return -1;
		} else {
			if (cgatt_r.status != AT_OK) {
				sleep(1);
				continue;
			}
			/* check GPRS status, 0 - disconnect, 1 - connect */
			if (cgatt_r.result == 1)
				break;

			sleep(1);
			continue;
		}
	}

	if (i >= 10) {
		/* disconnect */
		memset(&cgatt_r, 0, sizeof(struct cgatt_resp));
		ret = at_cmd_set_cgatt(&cgatt_r, 0);
		if (ret != 0) {
			ALOGE("at_cmd_set_cgatt 0 fail %d\n", ret);
			return -1;
		} else {
			if (cgatt_r.status == AT_OK) {
				/* connect */
				memset(&cgatt_r, 0, sizeof(struct cgatt_resp));
				ret = at_cmd_set_cgatt(&cgatt_r, 1);
				if (ret != 0) {
					ALOGE("at_cmd_set_cgatt 1 fail %d\n",
						ret);
					return -1;
				} else {
					if (cgatt_r.result != 1) {
						ALOGE("cgatt grps fail\n");
						return -1;
					}
				}
			} else {
				ALOGE("at_cmd_set_cgatt status fail\n");
				return -1;
			}
		}
	}

	/* 4.7 MYSYSINFO */
	for (i = 0; i < 10; i++) {
		memset(&sysinfo_r, 0, sizeof(struct sysinfo_resp));
		ret = at_cmd_get_sysinfo(&sysinfo_r);
		if (ret != 0) {
			ALOGE("at_cmd_get_sysinfo fail %d\n", ret);
			return -1;
		} else {
			if (sysinfo_r.status != AT_OK) {
				ALOGE("modem could not response MYSYSINFO.\n");
				return -1;
			}

			if (sysinfo_r.mnc == 0) {
				sleep(1);
				continue;
			}
		}
	}

	/* 5. start pppd */
	if (sysinfo_r.mnc == 1) /* china mobile */
		system("pppd call mobile-dial &");
	else if (sysinfo_r.mnc == 2) /* china unicom */
		system("pppd call unicom-dial &");
	else if (sysinfo_r.mnc == 3) /* china telecom */
		system("pppd call telecom-dial &");
	else {
		 /* unknow operator number */
		ALOGE("unknow operator number %d\n", sysinfo_r.mnc);
		return -1;
	}

	/* 6. check network interface IP addr */
	sleep(3);
	for (i = 0; i < 5; i++) {
		ip_addr = 0;
		ret = get_ip_addr(&ip_addr);
		if (ret != 0) {
			sleep(2);
			continue;
		}

		if (ip_addr == 0) {
			sleep(2);
			continue;
		}

		break;
	}

	if (i >= 5)
		ALOGW("ppp0 IP addr is 0.0.0.0\n");

	return ret;
}

/**
 * modem_close - close modem module
 *
 * if open success return 0
 */
int modem_close(void)
{
	int ret = 0;

	/* 1.1 whether pppd already running */
	ret = access(PPP_PID, 0);
	if (ret != 0) {
		ALOGW("modem no running.\n");
		return -1;
	} else
		system("/etc/ppp/fg-shutdown");

	return 0;
}

/**
 * modem_set_default_gateway - set ppp0 as default gateway
 *
 */
void modem_set_default_gateway(void)
{
	int ret;
	unsigned long ip_addr = 0;
	ret = get_ip_addr(&ip_addr);

	if ((ret == 0) && (ip_addr != 0))
		system("route add default ppp0");
	else
		ALOGI("get ppp0 ip fail\n");
}
