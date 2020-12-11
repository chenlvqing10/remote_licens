/*
 * factory_test.h - For testing aio interface.
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/wait.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <signal.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "isp.h"
#include "led.h"
#include "gpio.h"
#include "sensor.h"
#define LOG_TAG	"factory_test"
#include "log/log.h"
#include "db_all.h"
#include "time_conv.h"

#include "lombo_system_sound.h"
#include "system_volume.h"
#include "wifi_station.h"
#include "net_utils.h"
#include "ip_config.h"
#include "inet_monitor.h"
#include "file_ops.h"
#include "link_handler.h"


/* audio file for test speaker */
#define FACTORY_TEST_PLAY_SOUND_PATH \
					"/usr/share/aio/res/sound/factorytest.wav"

/* date list for test rtc */
static int set_date_list[][7] = { { 2004, 2, 28, 23, 59, 59, 0 },
	{ 2000, 2, 29, 23, 59, 59, 0 },
	{ 2019, 2, 28, 23, 59, 59, 0 },
	{ 2003, 8, 30, 23, 59, 59, 0 },
	{ 1989, 7, 31, 23, 59, 59, 0 },
	{ 1999, 12, 31, 23, 59, 56, 0 },
};

extern void get_wifi_para(wifi_config_para_t *p);
extern int sync_get_scan_list(SCAN_INFO *p_ret);
extern int sync_set_wifi_enabled(char on);

/**
 * factory_test_light - test white light in factory test mode.
 * @freq_time: test time of once
 * @count: test count
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_light(int freq_time, int count)
{
	int i = 0;

	if (freq_time <= 0 || freq_time >= 10 ||
		count <= 0) {
		ALOGE("%s: param is valid\n", __func__);
		return -1;
	}

	for (i = 0; i < count; i++) {
		if ((i % 2) == 0)
			white_led_ctrl(LED_ON);
		else
			white_led_ctrl(LED_OFF);

		usleep(500 * 1000 * freq_time);
	}

	return 0;
}

/**
 * factory_test_light_infrared - test infrared light in factory test mode.
 * @freq_time: test time of once
 * @count: test count
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_light_infrared(int freq_time, int count)
{
	int i = 0;

	if (freq_time <= 0 || freq_time >= 10 ||
		count <= 0) {
		ALOGE("%s: param is valid\n", __func__);
		return -1;
	}

	for (i = 0; i < count; i++) {
		if ((i % 2) == 0)
			infrared_led_ctrl(LED_ON);
		else
			infrared_led_ctrl(LED_OFF);

		usleep(500 * 1000 * freq_time);
	}

	infrared_led_ctrl(LED_ON);

	return 0;
}

/**
 * factory_test_speaker - test speaker in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_speaker(void)
{
	int ret = 0;

	lombo_system_set_volume(80);
	ret = lombo_system_sound_play(FACTORY_TEST_PLAY_SOUND_PATH);
	if (ret)
		ALOGE("failed to play sound:%s\n",
			  FACTORY_TEST_PLAY_SOUND_PATH);
	else
		ALOGI(" play sound finished\n");

	return ret;
}

/**
 * factory_test_wifi - test wifi in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_wifi(void)
{
	int ret, i;
	SCAN_INFO *scan_res = NULL;
	wifi_config_para_t config;
	memset(&config, 0, sizeof(wifi_config_para_t));

	get_wifi_para(&config);
	ALOGD("%s:%d >> default wifi state = %d" , __func__, __LINE__, config.enable_wifi);
	if (config.enable_wifi) {
		ret = sync_set_wifi_enabled(0);
		if (ret) {
			ALOGE("reset wifi failed!\n");
			ret = -1;
			goto exit;
		}
		ret = sync_set_wifi_enabled(1);
		if (ret) {
			ALOGE("start wifi failed!\n");
			ret = -1;
			goto exit;
		}
	} else {
		ret = sync_set_wifi_enabled(1);
		if (ret) {
			ALOGE("start wifi failed!\n");
			ret = -1;
			goto exit;
		}
	}

	scan_res = calloc(sizeof(SCAN_INFO), 1);
	if (!scan_res) {
		ALOGE("%s: calloc scan_res is NULL\n", __func__);
		ret = -1;
		goto exit;
	}
	/* the list max limit number*/
	scan_res->num = 50;
	scan_res->info = calloc(sizeof(HOTSPOT_INFO), scan_res->num);
	if (!scan_res->info) {
		ALOGE("%s: calloc result.info is NULL\n", __func__);
		free(scan_res);
		scan_res = NULL;
		ret = -1;
		goto exit;
	}

	ret = sync_get_scan_list(scan_res);
	if (ret) {
		ALOGE("%s: get result is NULL\n", __func__);
		ret = -1;
		goto exit;
	}
	ALOGI("hotspot num: %d\n", scan_res->num);
	for (i = 0; i < scan_res->num; i++)
		ALOGI("ssid[%d]: %s\n", i, scan_res->info[i].ssid);

exit:
	if (!config.enable_wifi) {
		ret = sync_set_wifi_enabled(0);
		if (ret) {
			ALOGE("%s: recover the default state of wifi failed!\n",
				__func__);
			ret = -1;
		}
	}

	if (scan_res) {
		if (scan_res->info) {
			free(scan_res->info);
			scan_res->info = NULL;
		}
		free(scan_res);
		scan_res = NULL;
	}
	return ret;
}

/**
 * factory_test_illuminance - test light sensor in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_illuminance(void)
{
	int ret = -1;
	int value = 0;
	int last_value = 0;
	int count = 0;
	int i = 0;

	for (i = 0; i < 10; i++) {
		if (sensor_get_illuminance(&value) == 0) {
			if (last_value != 0 &&
				abs(value - last_value) >= 100) {
				if (count++ >= 2)
					return 0;
			}
			last_value = value;
		}
		usleep(1000 * 1000 * 1);
	}

	return ret;
}

/**
 * factory_test_get_illuminance_value - get value of light sensor
 * in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_get_illuminance_value(int *value)
{
	if (value == NULL)
		return -1;

	*value = isp_get_brightness(FACE_CAMERA_RGB);

	return 0;
}

/**
 * factory_test_rtc - test rtc in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_rtc(void)
{
	int fd = 0;
	int ret = 0;
	int i = 0, list_num = 0;
	char *dev = "/dev/rtc0";
#if 0
	int pending = 0;
	unsigned long counter = 0;
	struct rtc_time alm_tm;
#endif
	struct rtc_time rtc_tm, rtc_set;

	ALOGD("open rtc dev:%s.\n", dev);

	fd = open(dev, O_RDONLY);
	if (fd < 0) {
		perror(dev);
		ALOGE("%s:%d >> Failed to open %s", __func__, __LINE__, dev);
		ret = __LINE__;
		return ret;
	}

	/* Read the RTC time/date */
	ret = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (ret < 0) {
		perror("ioctl: read time");
		ALOGE("%s:%d >> Failed to read time", __func__, __LINE__);
		ret = __LINE__;
		goto out;
	}
	ALOGD("Current RTC date/time is %d-%d-%d,%02d:%02d:%02d.\n",
		   rtc_tm.tm_year + 1900, rtc_tm.tm_mon + 1, rtc_tm.tm_mday,
		   rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	/* Set the RTC time/date */
	list_num = sizeof(set_date_list) / sizeof(set_date_list[0]);
	for (i = 0; i < list_num; i++) {
		rtc_set.tm_year = set_date_list[i][0] - 1900;
		rtc_set.tm_mon = set_date_list[i][1] - 1;
		rtc_set.tm_mday = set_date_list[i][2];
		rtc_set.tm_hour = set_date_list[i][3];
		rtc_set.tm_min = set_date_list[i][4];
		rtc_set.tm_sec = set_date_list[i][5];
		ret = ioctl(fd, RTC_SET_TIME, &rtc_set);
		if (ret != set_date_list[i][6]) {
			if (ret < 0) {
				perror("ioctl: set time");
				ALOGE("%s:%d >> Failed to set time", __func__, __LINE__);
				ret = __LINE__;
				goto out;
			} else {
				ALOGE("Failed to set rtc time!\n");
				ALOGD("the setted time is: %d-%d-%d,%d:%d:%d\n",
					   set_date_list[i][0], set_date_list[i][1],
					   set_date_list[i][2], set_date_list[i][3],
					   set_date_list[i][4], set_date_list[i][5]);
				ret = __LINE__;
				goto out;
			}
		}

		sleep(1);
		memset(&rtc_tm, 0, sizeof(struct rtc_time));
		ret = ioctl(fd, RTC_RD_TIME, &rtc_tm);
		if (ret < 0) {
			perror("ioctl: read time");
			ALOGE("%s:%d >> Failed to read time", __func__, __LINE__);
			ret = __LINE__;
			goto out;
		}

		switch (i) {
		case 0:
			if ((rtc_set.tm_year != rtc_tm.tm_year) ||
				(rtc_set.tm_mon != rtc_tm.tm_mon) ||
				(29 != rtc_tm.tm_mday) ||
				(0 != rtc_tm.tm_hour) ||
				(0 != rtc_tm.tm_min)) {
				ALOGD("the %d times rtc test failed!\n", i + 1);
				ALOGD("rtc failed: time != setting time!\n");
				ALOGD("Setting date/time is %d-%d-%d,%02d:%02d:%02d.\n",
					   rtc_set.tm_year + 1900,
					   rtc_set.tm_mon + 1,
					   rtc_set.tm_mday,
					   rtc_set.tm_hour,
					   rtc_set.tm_min,
					   rtc_set.tm_sec);
				ALOGD("Cur RTC date-time is %d-%d-%d,%02d:%02d:%02d\n",
					   rtc_tm.tm_year + 1900,
					   rtc_tm.tm_mon + 1,
					   rtc_tm.tm_mday,
					   rtc_tm.tm_hour,
					   rtc_tm.tm_min,
					   rtc_tm.tm_sec);
				ret = __LINE__;
				goto out;
			}
			break;
		case 1:
		case 2:
			if ((rtc_set.tm_year != rtc_tm.tm_year) ||
				 (2 != rtc_tm.tm_mon) ||
				 (1 != rtc_tm.tm_mday) ||
				 (0 != rtc_tm.tm_hour) ||
				 (0 != rtc_tm.tm_min)) {
				ALOGD("the %d times rtc test failed!\n", i + 1);
				ALOGD("rtc failed: time != setting time!\n");
				ALOGD("Set date/time is %d-%d-%d,%02d:%02d:%02d.\n",
					   rtc_set.tm_year + 1900,
					   rtc_set.tm_mon + 1,
					   rtc_set.tm_mday,
					   rtc_set.tm_hour,
					   rtc_set.tm_min,
					   rtc_set.tm_sec);
				ALOGD("Cur RTC date/time is %d-%d-%d,%02d:%02d:%02d.\n",
					   rtc_tm.tm_year + 1900,
					   rtc_tm.tm_mon + 1,
					   rtc_tm.tm_mday,
					   rtc_tm.tm_hour,
					   rtc_tm.tm_min,
					   rtc_tm.tm_sec);
				ret = __LINE__;
				goto out;
			}
			break;
		case 3:
			if ((rtc_set.tm_year != rtc_tm.tm_year) ||
				(7 != rtc_tm.tm_mon) ||
				(31 != rtc_tm.tm_mday) ||
				(0 != rtc_tm.tm_hour) ||
				(0 != rtc_tm.tm_min)) {
				ALOGD("the %d times rtc test failed!\n", i + 1);
				ALOGD("rtc failed:time != setting time!\n");
				ALOGD("Setting date/time is %d-%d-%d,%02d:%02d:%02d.\n",
					   rtc_set.tm_year + 1900,
					   rtc_set.tm_mon + 1,
					   rtc_set.tm_mday,
					   rtc_set.tm_hour,
					   rtc_set.tm_min,
					   rtc_set.tm_sec);
				ALOGD("Cur RTC date/time is %d-%d-%d,%02d:%02d:%02d.\n",
					   rtc_tm.tm_year + 1900,
					   rtc_tm.tm_mon + 1,
					   rtc_tm.tm_mday,
					   rtc_tm.tm_hour,
					   rtc_tm.tm_min,
					   rtc_tm.tm_sec);
				ret = __LINE__;
				goto out;
			}
			break;
		case 4:
			if ((rtc_set.tm_year != rtc_tm.tm_year) ||
				(7 != rtc_tm.tm_mon) ||
				(1 != rtc_tm.tm_mday) ||
				(0 != rtc_tm.tm_hour) ||
				(0 != rtc_tm.tm_min)) {
				ALOGD("the %d times rtc test failed!\n", i + 1);
				ALOGD("rtc failed: time != setting time!\n");
				ALOGD("Setting date/time is %d-%d-%d,%02d:%02d:%02d.\n",
					   rtc_set.tm_year + 1900,
					   rtc_set.tm_mon + 1,
					   rtc_set.tm_mday,
					   rtc_set.tm_hour,
					   rtc_set.tm_min,
					   rtc_set.tm_sec);
				ALOGD("Cur RTC date/time is %d-%d-%d,%02d:%02d:%02d.\n",
					   rtc_tm.tm_year + 1900,
					   rtc_tm.tm_mon + 1,
					   rtc_tm.tm_mday,
					   rtc_tm.tm_hour,
					   rtc_tm.tm_min,
					   rtc_tm.tm_sec);
				ret = __LINE__;
				goto out;
			}
			break;
		case 5:
			ALOGD("After set,cur RTC date/time is %d-%d-%d,%02d:%02d:%02d.\n",
				   rtc_tm.tm_year + 1900,
				   rtc_tm.tm_mon + 1,
				   rtc_tm.tm_mday,
				   rtc_tm.tm_hour,
				   rtc_tm.tm_min,
				   rtc_tm.tm_sec);
			break;
		}
	}

	close(fd);
	ALOGD("----rtc test success!----\n");

	return 0;
out:
	ALOGE("----rtc test failed!----\n");
	ALOGE("error at %d.\n", ret);
	close(fd);

	return ret;
}

/**
 * factory_test_camera - test camera in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_camera(void)
{
	return 0;
}

/**
 * ping_work - ping WLAN of test eth in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
static int ping_work(void)
{
	char *hostnames[] = {
		"www.qq.com",
		"www.163.com",
		"www.sohu.com",
	};

	for (int i = 0; i < 3; i++) {
		if (net_is_connect("eth0", hostnames[i]) == 1)
			return 0;
	}

	return -1;
}

/**
 * factory_test_eth - test eth in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_eth()
{
	int32_t ret = -1;
	char *dev = "eth0";
	ALOGD("start test eth:%s.\n", dev);

	ret = net_dev_open(dev);
	if (ret) {
		ALOGE("failed to open net device %s, error:%d",
			  "eth0", ret);
		goto exit;
	}
	/*check net whether link up*/
	ret = eth_dev_link_is_up(dev);
	if (ret <= 0) {
		ALOGE("%s: failed to checked", __func__);
		ret = -1;
	} else if (ret == 1) {
		/*link up*/
		ret = ping_work();
		if (ret)
			ALOGE("%s: failed to ping", __func__);
	}
exit:
	ALOGD("test eth finish: ret=%d.\n", ret);
	return ret;
}

/**
 * @brief  ble test
 * @note
 * @param  fd:
 * @retval None
 */
static void set_adv_data(int fd)
{
	le_set_advertising_data_cp cmd;
	int i = 0;
	int n;
	const char *name = "ADVTEST";

	cmd.length = 0;
	memset(cmd.data, 0, sizeof(cmd.data));
	/* set adv data */
	cmd.data[i] = 0x02; /* Field length */
	cmd.length += (1 + cmd.data[i++]);
	cmd.data[i++] = 0x01; /* Flags */
	/* LE General Discoverable Mode, BR/EDR Not Supported */
	cmd.data[i++] = (0x02 | 0x04);

	cmd.data[i] = 0x03;
	cmd.length += (1 + cmd.data[i++]);
	cmd.data[i++] = 0x03; /* complete list of 15-bit service class uuids */
	cmd.data[i++] = 0x0a;
	cmd.data[i++] = 0xa0;


	n = strlen(name);
	cmd.data[i] = 1 + n;
	cmd.length += (1 + cmd.data[i++]);
	cmd.data[i++] = 0x09; /* complete local name */
	memcpy(&cmd.data[i], name, n);
	i += n;

	if (hci_send_cmd(fd, OGF_LE_CTL, OCF_LE_SET_ADVERTISING_DATA,
			 sizeof(cmd), &cmd)) {
		perror("Send cmd for set adv data error");
		ALOGE("%s >> Send cmd for set adv data error!", __func__);
	}

}

/**
 * @brief  do leadv
 * @note   for bluetooth test
 * @retval
 */
static int do_leadv(void)
{
	struct hci_request rq;
	le_set_advertise_enable_cp advertise_cp;
	le_set_advertising_parameters_cp adv_params_cp;
	uint8_t status;
	int dd, ret;
	int hdev = -1;
	bdaddr_t ba;

	if (hdev != -1 && hci_devba(hdev, &ba) < 0) {
		printf("Device is not available!\n");
		ret = -1;
		goto done;
	}

	if (hdev < 0)
		hdev = hci_get_route(NULL);

	dd = hci_open_dev(hdev);
	if (dd < 0) {
		perror("Could not open device");
		ALOGE("%s >> Could not open device!", __func__);
		ret = -1;
		goto done;
	}

	memset(&adv_params_cp, 0, sizeof(adv_params_cp));
	adv_params_cp.min_interval = htobs(0x0800);
	adv_params_cp.max_interval = htobs(0x0800);
	adv_params_cp.advtype = 0;
	adv_params_cp.chan_map = 7;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISING_PARAMETERS;
	rq.cparam = &adv_params_cp;
	rq.clen = LE_SET_ADVERTISING_PARAMETERS_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, 1000);
	if (ret < 0)
		goto done;

	set_adv_data(dd);

	memset(&advertise_cp, 0, sizeof(advertise_cp));
	advertise_cp.enable = 0x01;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
	rq.cparam = &advertise_cp;
	rq.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, 1000);

	hci_close_dev(dd);

done:
	if (ret < 0) {
		printf("set ble advertising failed\n");
		return -1;
	}

	return 0;
}

/**
 * @brief  bluetooth test interface
 * @note
 * @retval
 */
int factory_test_bluetooth(void)
{
	return do_leadv();
}

/**
 * @brief  usb test interface
 * @note
 * @retval
 */
int factory_test_usb(void)
{
	char *usb_path = "/mnt/usb";
	char *usb_file_path = "/mnt/usb/usb_test_%s";
	char str_time[32] = {0};
	char file_name[128] = {0};
	char buf[128] = "test usb...";
	int ret = -1;

	ret = is_dir_exist(usb_path);
	if (ret) {
		ALOGE("%s >> file[%s] not exits! test failed!",
			__func__, usb_path);
		return -1;
	}

	time_second_to_file_name_str(time(NULL),
		str_time, sizeof(str_time));
	snprintf(file_name, sizeof(file_name),
		usb_file_path, str_time);

	ret = file_write(file_name, buf, 128);
	if (ret) {
		ALOGE("%s >> write file[%s] failed!",
			__func__, file_name);
		return -1;
	}

	ret = file_remove(file_name);
	if (ret) {
		ALOGE("%s >> delete file[%s] failed!",
			__func__, file_name);
		return -1;
	}

	return 0;
}

