#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "debug.h"
#include "wifi_softap.h"

typedef enum {
	AP_DISABLED = 0,
	AP_ENABLED,
} SWITCH_STATUS;

SWITCH_STATUS switch_status = AP_DISABLED;

static int set_switch_status(int enabled)
{
	switch_status = enabled;
	return 0;
}

static int is_softap_enabled(void)
{
	if (switch_status == AP_ENABLED)
		return 0;
	else
		return -1;
}

static int is_wpas_run(void)
{
	FILE *fp = NULL;
	int count = 0, size = 100;
	char buf[size];
	char cmd[100];

	sprintf(cmd, "ps -ef | grep -w %s | wc -l", "wpa_supplicant");

	fp = popen(cmd, "r");
	if (fp == NULL) {
		loge("popen err!\n");
		return -1;
	}

	if ((fgets(buf, size, fp)) != NULL)
		count = atoi(buf);

	pclose(fp);
	fp = NULL;

	if ((count - 2) == 0)
		return -1;
	else
		return 0;
}


#ifndef REALTEK_MODULE
static void softap_reload_firmware(void)
{
	system("ifconfig wlan0 down");
	system("echo /system/etc/firmware/wifi_ap.bin"
		"> /sys/module/bcmdhd/parameters/firmware_path");
}

static int is_wifi_firmware_exist(const char *firmware_file)
{
	FILE *fp = NULL;
	char buf[128] = {0};

	fp = fopen("/sys/module/bcmdhd/parameters/firmware_path", "r");
	if (fp == NULL) {
		loge("fopen error!\n");
		return -1;
	}

	fread(buf, sizeof(buf), 1, fp);

	if (strstr(buf, firmware_file) != NULL)
		return 0;
	else
		return -1;
}
#endif

int softap_set_config(AP_CONFIG *config)
{
	char buf[4096] = {0};
	int fd = -1;

	if (config == NULL) {
		loge("ap_config is invalid!\n");
		return -1;
	}

#ifdef REALTEK_MODULE
	sprintf(buf, "interface=wlan0\nctrl_interface=/var/run/hostapd\n"
			"ssid=%s\nchannel=6\ndriver=nl80211\nhw_mode=g\n"
			"ieee80211n=1\nwme_enabled=1\n"
			"ht_capab=[SHORT-GI-20][SHORT-GI-40][HT40+]\n"
			"max_num_sta=8\n", config->ssid);
#else
	sprintf(buf, "interface=wlan0\nctrl_interface=/var/run/hostapd\n"
			"ssid=%s\nchannel=6\ndriver=nl80211\n"
			"hw_mode=g\nieee80211n=1\n"
			"ignore_broadcast_ssid=0\n", config->ssid);
#endif

	switch (config->security) {
	case WIFI_OPEN:
		break;
	case WIFI_WPA_PSK:
		sprintf(buf, "%swpa=1\nwpa_pairwise=TKIP CCMP\n"
				"wpa_passphrase=%s\n"
				"wpa_key_mgmt=WPA-PSK\n", buf, config->passwd);
		break;
	case WIFI_WPA2_PSK:
		sprintf(buf, "%swpa=2\nrsn_pairwise=CCMP\nwpa_passphrase=%s\n"
				"wpa_key_mgmt=WPA-PSK\n", buf, config->passwd);
		break;
	default:
		loge("security is incorrect!\n");
		return -1;
	}

	/* update hostapd.conf */
	fd = open("/data/wifi/hostapd.conf",
			O_CREAT | O_TRUNC | O_WRONLY, 0660);
	if (fd < 0) {
		loge("can not open the conf file!\n");
		return -1;
	}

	if (write(fd, buf, strlen(buf)) < 0) {
		loge("can not write to the conf file!\n");
		return -1;
	}

	close(fd);
	return 0;
}

int softap_get_info(AP_INFO *info)
{
	char buf[4096] = {0};
	int size, len, ret = 0;
	char *buf_start, *buf_end;
	FILE *fp;

	fp = fopen("/data/wifi/hostapd.conf", "r");
	if (fp == NULL) {
		loge("can not open the conf file!\n");
		return -1;
	}

	/* obtain the configuration file length */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	if (fread(buf, sizeof(char), size, fp) < 0) {
		loge("hostapd.conf is valid!\n");
		ret = -1;
		goto exit;
	}

	buf_start = strstr(buf, "ssid=");
	if (buf_start == NULL) {
		loge("ap config is incorrect!\n");
		ret = -1;
		goto exit;
	}

	buf_start = buf_start + 5;
	buf_end = strchr(buf_start, '\n');
	len = buf_end - buf_start;
	strncpy(info->ssid, buf_start, len);
	info->ssid[len] = '\0';

	buf_start = strstr(buf, "wpa=1");
	if (buf_start != NULL) {
		info->security = WIFI_WPA_PSK;
		buf_start = strstr(buf, "wpa_passphrase=");
		if (buf_start != NULL) {
			buf_start = buf_start + 15;
			buf_end = strchr(buf_start, '\n');
			len = buf_end - buf_start;
			strncpy(info->passwd, buf_start, len);
			info->passwd[len] = '\0';
			loge("passwd: %s!\n", info->passwd);
		} else {
			loge("ap config is incorrect!\n");
			ret = -1;
			goto exit;
		}
	} else {
		buf_start = strstr(buf, "wpa=2");
		if (buf_start != NULL) {
			info->security = WIFI_WPA2_PSK;
			buf_start = strstr(buf, "wpa_passphrase=");
			if (buf_start != NULL) {
				buf_start = buf_start + 15;
				buf_end = strchr(buf_start, '\n');
				len = buf_end - buf_start;
				strncpy(info->passwd, buf_start, len);
				info->passwd[len] = '\0';
				loge("passwd: %s!\n", info->passwd);
			} else {
				loge("ap config is incorrect!\n");
				ret = -1;
				goto exit;
			}
		} else {
			info->security = WIFI_OPEN;
		}
	}

exit:
	fclose(fp);
	return ret;
}

int softap_enable(void)
{
	if (!is_softap_enabled()) {
		loge("softap has been enabled!\n");
		return 0;
	}

	/* advoid running station and softap at the same time */
	if (!is_wpas_run()) {
		logd("stop wpa_supplicant!\n");
		system("/etc/init.d/wifi_sta stop > /dev/null");
		system("killall udhcpc");
		usleep(200000);
	}

#ifndef REALTEK_MODULE
	/* check the the downloaded wifi firmware is approriate */
	if (!is_wifi_firmware_exist("wifi_sta.bin"))
		softap_reload_firmware();
#endif

	/* start hostapd and dnsmasq */
	system("/etc/init.d/wifi_ap start > /dev/null");
	system("/etc/init.d/dnsmasq start > /dev/null");

	set_switch_status(AP_ENABLED);

	return 0;
}

int softap_disable(void)
{
	if (is_softap_enabled()) {
		loge("softap has been disabled!\n");
		return 0;
	}

	/* stop hostapd and dnsmasq */
	system("/etc/init.d/wifi_ap stop > /dev/null");
	system("/etc/init.d/dnsmasq stop > /dev/null");

	set_switch_status(AP_DISABLED);

	return 0;
}
