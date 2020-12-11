/*
 * time_ntp.c - Get time from NTP server.
 *
 * Copyright (C) 2020, LomboTech Co.Ltd.
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

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "time_ntp.h"

#define LOG_TAG	"time_ntp"
#include <log/log.h>

#define NTP_TIME_OUT	10
/* NTP server */
#define NTP_SERVER "211.68.71.26"
static pthread_t p;
static int flag;
static int count;
static int check;
static int check_now;
#if 0
void *set_network_time_proc(void *arg)
{
	int ret = 0;
	char cmd[256];
	int time = 0;
	int count = 0;
	while (flag) {
		ret = 0;
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ntpd -p %s -qNn", NTP_SERVER);
		ret = system(cmd);
		if (0 == ret && count == 0) {
			ALOGD("%s\n", cmd);
			system("hwclock -w -u");
			break;
		} else
			ALOGE("%s Get ntp time error,return=%d\n", __func__, ret);

		time++;
		if (time > NTP_TIME_OUT) {
			ALOGE("%s Get time from ntp server fail:%s\n", __func__,
				 NTP_SERVER);
			time = 0;
		}
		sleep(1);
		count++;
		if (count == 3600)
			count = 0;
	}

	return NULL;
}

/*
* set_network_time - set time from NTP server
* @none.
* Return 0 if success, otherwise return -1.
*/
int ntp_cal_time(void)
{
	int ret = -1;
	if(flag == 1) return 0;
	flag = 1;

	ret = pthread_create(&p, NULL, set_network_time_proc, NULL);
	if (ret != 0) {
		ALOGE("[%s:%d]pthread_create error\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

int exit_ntp_cal_time(void)
{
	if (flag == 0)
		return 0;
	flag = 0;
	pthread_join(p, NULL);
	return 0;
}
#endif

static void *ntp_proc(void *arg)
{
	int ret = 0;
	char cmd[256];
	prctl(PR_SET_NAME, "ntp_proc");

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ntpd -p %s -qNn", NTP_SERVER);
	ALOGD("%s: enter flag = %d check = %d\n",
		  __func__, flag, check);
	while (flag) {
		if(check) {
			if((count % 18000) == 0) {
				check_now = 1;
			}
			if(check_now) {
				if(0 == system(cmd)) {
					check_now = 0;
					system("hwclock -w -u");
				}else {
					ALOGE("%s Get ntp time error,return=%d\n", __func__, ret);
				}
			}
			count++;
		}
		usleep(100*1000);
	}

	return NULL;
}
int ntp_start(void)
{
	check = 1;
	check_now = 1;
	ALOGD("%s: flag = %d check = %d\n",
		  __func__, flag, check);
	return 0;
}
int ntp_stop(void)
{
	check = 0;
	ALOGD("%s: flag = %d check = %d\n",
		  __func__, flag, check);
	return 0;
}
int ntp_checktime(void){

	count = 0;
	return 0;
}
int ntp_time_init(int check_flag)
{
	int ret = -1;
	flag = 1;
	check = check_flag;
	ret = pthread_create(&p, NULL, ntp_proc, NULL);
	if (ret != 0) {
		ALOGE("[%s:%d]pthread_create error\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

int ntp_time_deinit(void)
{
	if (flag == 0)
		return 0;
	flag = 0;
	pthread_join(p, NULL);

	return 0;
}