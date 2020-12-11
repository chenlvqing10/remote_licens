#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "debug.h"
#include "wifi_common.h"
#include "net_conf_parser.h"
#include "wifi_monitor.h"
#include "state.h"
#include "wifi_station.h"

static pthread_mutex_t scan_lock = PTHREAD_MUTEX_INITIALIZER;

/*
 * is_passwd_legal - check if password is legal.
 *
 * @passwd: wifi password.
 *
 * return 0 if password is legal;
 * -1 if password is illegal.
 */
static int is_passwd_legal(const char *passwd)
{
	if ((passwd == NULL))
		return -1;

	if ((strlen(passwd) < 8))
		return -1;

	return 0;
}

/*
 * exception_handle - perform exception handling on the specified netconf.
 *
 * @netid: netid of the netconf to be operated.
 *
 * return 0 if success; otherwise failed.
 */
static int exception_handle(const char *netid)
{
	char cmd[32] = {0}, reply[32] = {0};

	if (netid == NULL) {
		loge("network id is NULL!\n");
		return -1;
	}

	sprintf(cmd, "REMOVE_NETWORK %s", netid);
	wifi_command(cmd, reply, sizeof(reply));
	sprintf(cmd, "%s", "SAVE_CONFIG");
	wifi_command(cmd, reply, sizeof(reply));

	return 0;
}

/**
 * update_priority - update priority.
 *
 * @netid: netid of the netconf to be operated.
 *
 * return 0 if success; otherwise failed.
 */
static int update_priority(const char *netid)
{
	char cmd[64] = {0}, reply[32] = {0};
	int ret = 0, max_priority = 0;

	if (netid == NULL) {
		loge("network id is NULL!\n");
		return -1;
	}

	obtain_max_priority(&max_priority);

	sprintf(cmd, "SET_NETWORK %s priority %d", netid, (max_priority+1));
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("set network priority failed!\n");
		exception_handle(netid);
		return -1;
	}

	return 0;
}

/*
 * obtain_channel - obtain the channel of the corresponding frequency.
 *
 * @frequency: channel frequency.
 * @channel: buffer for storing channel.
 *
 * return 0 if success; otherwise failed.
 */
static int obtain_channnel(int frequency, int *channel)
{
	int ret = 0;

	if (channel == NULL) {
		loge("wifi channel is invalid!\n");
		return -1;
	}

	switch (frequency) {
	case 2412:
		*channel = 1;
		break;

	case 2417:
		*channel = 2;
		break;

	case 2422:
		*channel = 3;
		break;

	case 2427:
		*channel = 4;
		break;

	case 2432:
		*channel = 5;
		break;

	case 2437:
		*channel = 6;
		break;

	case 2442:
		*channel = 7;
		break;

	case 2447:
		*channel = 8;
		break;

	case 2452:
		*channel = 9;
		break;

	case 2457:
		*channel = 10;
		break;

	case 2462:
		*channel = 11;
		break;

	case 2467:
		*channel = 12;
		break;

	case 2472:
		*channel = 13;
		break;

	case 5180:
		*channel = 36;
		break;

	case 5200:
		*channel = 40;
		break;

	case 5220:
		*channel = 44;
		break;

	case 5240:
		*channel = 48;
		break;

	case 5260:
		*channel = 52;
		break;

	case 5280:
		*channel = 56;
		break;

	case 5300:
		*channel = 60;
		break;

	case 5320:
		*channel = 64;
		break;

	case 5500:
		*channel = 100;
		break;

	case 5520:
		*channel = 104;
		break;

	case 5540:
		*channel = 108;
		break;

	case 5560:
		*channel = 112;
		break;

	case 5580:
		*channel = 116;
		break;

	case 5600:
		*channel = 120;
		break;

	case 5620:
		*channel = 124;
		break;

	case 5640:
		*channel = 128;
		break;

	case 5660:
		*channel = 132;
		break;

	case 5680:
		*channel = 136;
		break;

	case 5700:
		*channel = 140;
		break;

	case 5720:
		*channel = 144;
		break;

	case 5745:
		*channel = 149;
		break;

	case 5765:
		*channel = 153;
		break;

	case 5785:
		*channel = 157;
		break;

	case 5805:
		*channel = 161;
		break;

	case 5825:
		*channel = 165;
		break;

	default:
		ret = -1;
	}

	return ret;
}

/**
 * parse_scan_result - parse scan result.
 *
 * @result: scan result.
 * @scan_info: buffer for storing parsed scan information.
 *
 * return 0 if success; otherwise failed.
 */
static int parse_scan_result(const char *result, SCAN_INFO *scan_info)
{
	char parse_field[64] = {0}, tmp_buf[256] = {0};
	char *ptr_start = NULL, *ptr_end = NULL, *tmp_ptr = NULL;
	HOTSPOT_INFO *tmp_info = NULL;
	int num = 0, hidden_num = 0, empty_num = 0, len = 0, frequency = 0;
	int i = 0;

	if ((result == NULL) || (strchr(result, '\n') == NULL)) {
		loge("scan result is empty!\n");
		return -1;
	}

	if (scan_info == NULL) {
		loge("scan info is NULL!\n");
		return -1;
	}

	/* record number of hotspot */
	for (ptr_start = strchr(result, '\n'); ptr_start != NULL;
		ptr_start = strchr(ptr_start, '\n')) {
		ptr_start++;
		num++;
	}

	/* record number of hidden hotspot */
	tmp_ptr = strchr(result, '\n');
	if (tmp_ptr != NULL) {
		tmp_ptr++;
		while (1) {
			ptr_start = tmp_ptr;
			tmp_ptr = strstr(ptr_start, "\\x00\n");
			if (tmp_ptr != NULL) {
				tmp_ptr = tmp_ptr + 5;
				hidden_num++;
				continue;
			} else {
				tmp_ptr = strstr(ptr_start, "\\x00\n");
				if (tmp_ptr != NULL)
					hidden_num++;
				break;
			}
		}
	}

	/* record number of hotspot that ssid is empty */
	tmp_ptr = strchr(result, '\n');
	if (tmp_ptr != NULL) {
		tmp_ptr++;
		while (1) {
			ptr_start = tmp_ptr;
			tmp_ptr = strstr(ptr_start, "\t\n");
			if (tmp_ptr != NULL) {
				tmp_ptr = tmp_ptr + 3;
				empty_num++;
				continue;
			} else if (*(strchr(ptr_start, '\0') - 1) == '\t') {
				empty_num++;
			}
			break;
		}
	}

	scan_info->num = num - hidden_num - empty_num;
	num = 0;

	if (scan_info->num) {
		tmp_info = (HOTSPOT_INFO *)malloc(sizeof(HOTSPOT_INFO) *
				scan_info->num);
		if (tmp_info == NULL) {
			loge("malloc error!\n");
			return -1;
		}
	} else {
		logi("There is no valid hotspot in the environment!\n");
		scan_info->info = NULL;
		return 0;
	}
	memset(tmp_info, 0, sizeof(HOTSPOT_INFO) * scan_info->num);

	for (ptr_start = strchr(result, '\n'); ptr_start != NULL;
		ptr_start = strchr(ptr_start, '\n')) {
		ptr_start++;

		ptr_end = strchr(ptr_start, '\n');
		if (ptr_end == NULL) {
			ptr_end = strchr(ptr_start, '\0');
			if (ptr_end == NULL) {
				break;
			} else {
				len = ptr_end - ptr_start;
				strncpy(tmp_buf, ptr_start, len);
				tmp_buf[len] = '\0';
			}
		} else {
			len = ptr_end - ptr_start;
			strncpy(tmp_buf, ptr_start, len);
			tmp_buf[len] = '\0';
		}

		/* ignore hidden hotspot */
		if (strstr(tmp_buf, "\\x00") != NULL) {
			logd("ignore hidden hotspot!\n");
			continue;
		}

		/* ignore the hotspot that ssid is empty */
		if (*(strchr(tmp_buf, '\0') - 1) == '\t') {
			logd("ignore the hotspot that ssid is empty!\n");
			continue;
		}

		/* parse bssid */
		ptr_end = strchr(ptr_start, '\t');
		if (ptr_end != NULL) {
			len = ptr_end - ptr_start;
			strncpy(tmp_info[num].bssid, ptr_start, len);
			tmp_info[num].bssid[len] = '\0';
		}

		/* parse channel */
		ptr_start = ptr_end + 1;
		ptr_end = strchr(ptr_start, '\t');
		if (ptr_end != NULL) {
			len = ptr_end - ptr_start;
			strncpy(parse_field, ptr_start, len);
			parse_field[len] = '\0';
			frequency = atoi(parse_field);
			if (obtain_channnel(frequency,
				&(tmp_info[num].channel))) {
				loge("obtain channel failed!\n");
				free(tmp_info);
				return -1;
			}
		}

		/* parse rssi */
		ptr_start = ptr_end + 1;
		ptr_end = strchr(ptr_start, '\t');
		if (ptr_end != NULL) {
			len = ptr_end - ptr_start;
			strncpy(parse_field, ptr_start, len);
			parse_field[len] = '\0';
			tmp_info[num].rssi = atoi(parse_field);
		}

		/* parse security */
		ptr_start = ptr_end + 1;
		ptr_end = strchr(ptr_start, '\t');
		if (ptr_end != NULL) {
			len = ptr_end - ptr_start;
			strncpy(parse_field, ptr_start, len);
			parse_field[len] = '\0';
			if (strstr(parse_field, "WPA2-PSK") != NULL)
				tmp_info[num].security = SECURITY_WPA2_PSK;
			else if (strstr(parse_field, "WPA-PSK") != NULL)
				tmp_info[num].security = SECURITY_WPA_PSK;
			else if (strstr(parse_field, "ESS") != NULL)
				tmp_info[num].security = SECURITY_OPEN;
		}

		/* parse ssid */
		ptr_start = ptr_end + 1;

		ptr_end = strchr(ptr_start, '\n');
		if (ptr_end == NULL) {
			ptr_end = strchr(ptr_start, '\0');
			if (ptr_end == NULL) {
				break;
			} else {
				len = ptr_end - ptr_start;
				strncpy(tmp_buf, ptr_start, len);
				tmp_buf[len] = '\0';

				/* compatible with chinese ssid */
				str_to_utf8(tmp_buf, tmp_info[num].ssid);
			}
		} else {
			len = ptr_end - ptr_start;
			strncpy(tmp_buf, ptr_start, len);
			tmp_buf[len] = '\0';

			/* compatible with chinese ssid */
			str_to_utf8(tmp_buf, tmp_info[num].ssid);
		}

		/* ignore the hotspot with the same name */
		for (i = 0; i < num; i++) {
			if (!strcmp(tmp_info[i].ssid, tmp_info[num].ssid)) {
				logd("ignore the hotspot with the same name!\n");
				if (tmp_info[i].rssi < tmp_info[num].rssi) {
					logd("replace hotsplot with less intense ones!\n");
					tmp_info[i].channel = tmp_info[num].channel;
					tmp_info[i].rssi = tmp_info[num].rssi;
					tmp_info[i].security = tmp_info[num].security;
					strcpy(tmp_info[i].bssid, tmp_info[num].bssid);
					strcpy(tmp_info[i].ssid, tmp_info[num].ssid);
				}
				break;
			}
		}

		if (i == num) {
			num++;
			logd("add new hotsplot item\n");
		}
	}

	if ((scan_info->info != NULL))
		free(scan_info->info);

	scan_info->info = tmp_info;
	scan_info->num = num;
	return 0;
}

/**
 * parse_status_result - parse the return result by status command.
 *
 * @result: return result by status command.
 * @connect_info: buffer for storing parsed connection status information.
 *
 * return 0 if success; otherwise failed.
 */
static int parse_status_result(const char *result, CONNECT_INFO *connect_info)
{
	char tmp_buf[256] = {0};
	char *ptr_start = NULL, *ptr_end = NULL;
	int len = 0;

	if ((result == NULL) || (strchr(result, '\n') == NULL)) {
		loge("status result is empty!\n");
		return -1;
	}

	if (connect_info == NULL) {
		loge("connect info is NULL!\n");
		return -1;
	}

	/* parse bssid */
	ptr_start = strstr(result, "bssid=");
	if (ptr_start != NULL) {
		ptr_start = ptr_start + 6;
		ptr_end = strchr(ptr_start, '\n');
		len = ptr_end - ptr_start;
		strncpy(connect_info->bssid, ptr_start, len);
		connect_info->bssid[len] = '\0';
	} else {
		return -1;
	}

	/* parse ssid */
	ptr_start = strstr(ptr_start, "ssid=");
	if (ptr_start != NULL) {
		ptr_start = ptr_start + 5;
		ptr_end = strchr(ptr_start, '\n');
		len = ptr_end - ptr_start;
		strncpy(tmp_buf, ptr_start, len);
		tmp_buf[len] = '\0';

		/* compatible with chinese ssid */
		str_to_utf8(tmp_buf, connect_info->ssid);
	} else {
		return -1;
	}

	/* parse network id */
	ptr_start = strstr(ptr_start, "id=");
	if (ptr_start != NULL) {
		ptr_start = ptr_start + 3;
		ptr_end = strchr(ptr_start, '\n');
		len = ptr_end - ptr_start;
		strncpy(tmp_buf, ptr_start, len);
		tmp_buf[len] = '\0';
		connect_info->id = atoi(tmp_buf);
	} else {
		return -1;
	}

	return 0;
}

#ifndef REALTEK_MODULE
/**
 * sta_reload_firmware - reload station firmware for some wifi modules.
 */
static void sta_reload_firmware(void)
{
	system("ifconfig wlan0 down");
	system("echo /system/etc/firmware/wifi_sta.bin"
		"> /sys/module/bcmdhd/parameters/firmware_path");
}

/**
 * is_wifi_firmware_exist - check that wifi firmware have been downloaded.
 *
 * @firmware_file: wifi firmware file name.
 *
 * return 0 if success; otherwise failed.
 */
static int is_wifi_firmware_exist(const char *firmware_file)
{
	FILE *fp = NULL;
	char buf[128] = {0};

	if (firmware_file == NULL) {
		loge("wifi firmware file is invalid!\n");
		return -1;
	}

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

/**
 * sta_enable - enable station.
 *
 * @handler: registered callback function.
 *
 * return 0 if success; otherwise failed.
 */
int sta_enable(STA_EVENT_HANDLER handler)
{
	int ret = -1, wpas_start_flag = 0;

	if (init_sta_info() == -1) {
		loge("init station information failed!\n");
		ret = -1;
		goto exit;
	}

	if (!is_wifi_enabled()) {
		loge("wifi has been enabled!\n");
		ret = 0;
		goto exit;
	}

	if (handler == NULL) {
		loge("callback function is invalid!\n");
		ret = -1;
		goto exit;
	}

	set_switch_status(STA_ENABLING);

	/* advoid running station and softap at the same time */
	if (!is_proc_run("hostapd")) {
		logd("stop hostapd!\n");
		system("/etc/init.d/wifi_ap stop > /dev/null");
		system("/etc/init.d/dnsmasq stop > /dev/null");
		usleep(200000);
	}

#ifndef REALTEK_MODULE
	/* check the the downloaded wifi firmware is approriate */
	if (!is_wifi_firmware_exist("wifi_ap.bin"))
		sta_reload_firmware();
#endif

	logd("start enable station!\n");

	if (is_proc_run("wpa_supplicant")) {
		logd("start wpa_supplicant!\n");
		wifi_start_supplicant();
		wpas_start_flag = 1;
	}

	ret = wifi_connect_to_supplicant();
	if (ret) {
		loge("connect to wpas error!\n");
		ret = -1;
		goto exit;
	}

	register_event_handler(handler);
	start_monitor();
	set_switch_status(STA_ENABLED);

	/* check if station is connected */
	if ((wpas_start_flag == 1) && (is_sta_connected() == 1)) {
		logd("wifi is already connected!\n");
		set_sta_state(STA_CONNECTED_STATE);
	} else {
		set_sta_state(STA_DISCONNECTED_STATE);
	}
	usleep(100000);

exit:
	return ret;
}

/**
 * sta_disable - disable station.
 *
 * return 0 if success; otherwise failed.
 */
int sta_disable(void)
{
	int ret = 0, i = 0, j = 0;

	if (is_wifi_enabled()) {
		loge("wifi has been disabled!\n");
		wifi_stop_supplicant();
		ret = 0;
		goto exit;
	}

	logd("start disable station!\n");
	set_switch_status(STA_DISABLING);

	while ((get_send_command_flag() == ON) && (i < 10)) {
		loge("waiting for sending command is done!\n");
		i++;
		usleep(100000);
	}

	while ((get_scan_result_flag() == ON) && (j < 10)) {
		loge("waiting for obtain scan result!\n");
		j++;
		usleep(100000);
	}

	wifi_close_supplicant_connection();
	wifi_stop_supplicant();
	unregister_event_handler();
	set_switch_status(STA_DISABLED);

	if (uninit_sta_info()) {
		loge("uninit station information failed!\n");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

/**
 * sta_connect - connect the visible hotspot.
 *
 * @ssid: hotspot name.
 * @passwd: hotspot password.
 * @security: hotspot encryption mode.
 *
 * return 0 if success; otherwise failed.
 */
int sta_connect(const char *ssid, const char *passwd, SECURITY security)
{
	char cmd[128] = {0}, reply[32] = {0};
	char used_netid[8] = {0}, new_netid[8] = {0};
	int i = 0, ret = 0, exist_flag = 0, sta_event = 0, connect_fail = 0;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	if (!ssid) {
		loge("ssid is NULL!\n");
		call_event_handler(STA_UNKNOWN_EVENT);
		return -1;
	}

	while (get_delete_netconf_flag() == ON) {
		logd("waiting for the other delete netconf operation!\n");
		usleep(100000);
	}

	pthread_mutex_lock(&scan_lock);

	/* make sure that wifi is still on when the thread is woken up */
	if (is_wifi_enabled()) {
		loge("wifi is not enabled, switch_status: %d", get_switch_status());
		return -1;
	}

	/* check if the corresponding netconf exists */
	exist_flag = is_netconf_exist(ssid, security, used_netid);

	sprintf(cmd, "%s", "ADD_NETWORK");
	ret = wifi_command(cmd, new_netid, sizeof(new_netid));
	if (ret) {
		loge("add network failed!\n");
		ret = -1;
		goto exit;
	}

	sprintf(cmd, "SET_NETWORK %s ssid \"%s\"", new_netid, ssid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("set network ssid failed!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}

	if (security == SECURITY_OPEN) {
		sprintf(cmd, "SET_NETWORK %s key_mgmt NONE", new_netid);
		ret = wifi_command(cmd, reply, sizeof(reply));
		if (ret) {
			loge("set network key_mgmt failed!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}
	} else if (security == SECURITY_WPA_PSK ||
		 security == SECURITY_WPA2_PSK) {
		ret = is_passwd_legal(passwd);
		if (ret) {
			loge("wpa-psk passwd is illegal!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}

		sprintf(cmd, "SET_NETWORK %s psk \"%s\"", new_netid, passwd);
		ret = wifi_command(cmd, reply, sizeof(reply));
		if (ret) {
			loge("set network psk failed!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}
	} else {
		loge("security is not support!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}

	/* update the priority of new netconf */
	ret = update_priority(new_netid);
	if (ret) {
		loge("update priority failed!\n");
		ret = -1;
		goto exit;
	}

	set_connect_flag(ON);

	/* start connection */
	sprintf(cmd, "SELECT_NETWORK %s", new_netid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("select network failed!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}
	set_sta_state(STA_DISCONNECTED_STATE);

	/* wait for connection to end */
	do {
		usleep(100000);
		i++;
	} while ((i < 200) && (get_sta_state() == STA_DISCONNECTED_STATE) &&
				(get_connect_event() == UNKNOWN_EVENT));

	if (i == 200) {
		set_sta_state(STA_DISCONNECTED_STATE);
		set_connect_event(NETWORK_NOT_EXIST);
	}

	if (get_sta_state() == STA_DISCONNECTED_STATE) {
		if (get_connect_event() == NETWORK_NOT_EXIST)
			call_event_handler(STA_NETWORK_NOT_EXIST);
		else if (get_connect_event() == PASSWD_ERROR)
			call_event_handler(STA_PASSWD_ERROR);
		else if (get_connect_event() == CONNECT_REJECT)
			call_event_handler(STA_CONNECT_REJECT);
		else if (get_connect_event() == CONNECT_ABORT)
			call_event_handler(STA_CONNECT_ABORT);

		connect_fail = 1;
		set_connect_event(UNKNOWN_EVENT);
		set_connect_flag(OFF);
		exception_handle(new_netid);
		ret = -1;
		goto remove;
	}

	read_event(&sta_event);
	call_event_handler(sta_event);

remove:
	if (exist_flag == 1) {
		logd("remove the netconf with the same ssid");
		/* netconf already exists */
		sprintf(cmd, "REMOVE_NETWORK %s", used_netid);
		wifi_command(cmd, reply, sizeof(reply));
	}

	sprintf(cmd, "%s", "SAVE_CONFIG");
	wifi_command(cmd, reply, sizeof(reply));

	if (enable_all_netconf() == -1) {
		loge("enable all networks error!\n");
		ret = -1;
	}

	/* make sure that the abnormal event count is reset */
	reset_abnormal_event_count();

exit:
	if (ret) {
		set_sta_state(STA_DISCONNECTED_STATE);
		if (connect_fail == 0)
			call_event_handler(STA_UNKNOWN_EVENT);
	}

	pthread_mutex_unlock(&scan_lock);
	return ret;
}

/**
 * sta_connect_hidden - connect the invisible hotspot.
 *
 * @ssid: hotspot name.
 * @passwd: hotspot password.
 * @security: hotspot encryption method.
 *
 * return 0 if success; otherwise failed.
 */
int sta_connect_hidden(const char *ssid, const char *passwd, SECURITY security)
{
	char cmd[128] = {0}, reply[32] = {0};
	char used_netid[8] = {0}, new_netid[8] = {0};
	int i = 0, ret = 0, exist_flag = 0, sta_event = 0, connect_fail = 0;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	if (!ssid) {
		loge("ssid is NULL!\n");
		call_event_handler(STA_UNKNOWN_EVENT);
		return -1;
	}

	while (get_delete_netconf_flag() == ON) {
		logd("waiting for the other delete netconf operation!\n");
		usleep(100000);
	}

	pthread_mutex_lock(&scan_lock);

	/* make sure that wifi is still on when the thread is woken up */
	if (is_wifi_enabled()) {
		loge("wifi is not enabled, switch_status: %d", get_switch_status());
		return -1;
	}

	/* check if the corresponding netconf exists */
	exist_flag = is_netconf_exist(ssid, security, used_netid);

	sprintf(cmd, "%s", "ADD_NETWORK");
	ret = wifi_command(cmd, new_netid, sizeof(new_netid));
	if (ret) {
		loge("add network failed!\n");
		ret = -1;
		goto exit;
	}

	sprintf(cmd, "SET_NETWORK %s ssid \"%s\"", new_netid, ssid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("set network ssid failed!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}

	if (security == SECURITY_OPEN) {
		sprintf(cmd, "SET_NETWORK %s key_mgmt NONE", new_netid);
		ret = wifi_command(cmd, reply, sizeof(reply));
		if (ret) {
			loge("set network key_mgmt failed!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}
	} else if (security == SECURITY_WPA_PSK ||
		 security == SECURITY_WPA2_PSK) {
		ret = is_passwd_legal(passwd);
		if (ret) {
			loge("wpa-psk passwd is illegal!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}

		sprintf(cmd, "SET_NETWORK %s psk \"%s\"", new_netid, passwd);
		ret = wifi_command(cmd, reply, sizeof(reply));
		if (ret) {
			loge("set network psk failed!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}
	} else {
		loge("security is not support!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}

	sprintf(cmd, "SET_NETWORK %s scan_ssid 1", new_netid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("set network scan_ssid failed!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}

	/* update the priority of new netconf */
	ret = update_priority(new_netid);
	if (ret) {
		loge("update priority failed!\n");
		ret = -1;
		goto exit;
	}

	set_connect_flag(ON);

	/* start connection */
	sprintf(cmd, "SELECT_NETWORK %s", new_netid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("select network failed!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}
	set_sta_state(STA_DISCONNECTED_STATE);

	/* wait for connection to end */
	do {
		usleep(100000);
		i++;
	} while ((i < 200) && (get_sta_state() == STA_DISCONNECTED_STATE) &&
			 (get_connect_event() == UNKNOWN_EVENT));

	if (i == 200) {
		set_sta_state(STA_DISCONNECTED_STATE);
		set_connect_event(NETWORK_NOT_EXIST);
	}

	if (get_sta_state() == STA_DISCONNECTED_STATE) {
		if (get_connect_event() == NETWORK_NOT_EXIST)
			call_event_handler(STA_NETWORK_NOT_EXIST);
		else if (get_connect_event() == PASSWD_ERROR)
			call_event_handler(STA_PASSWD_ERROR);
		else if (get_connect_event() == CONNECT_REJECT)
			call_event_handler(STA_CONNECT_REJECT);
		else if (get_connect_event() == CONNECT_ABORT)
			call_event_handler(STA_CONNECT_ABORT);

		connect_fail = 1;
		set_connect_event(UNKNOWN_EVENT);
		set_connect_flag(OFF);
		exception_handle(new_netid);
		ret = -1;
		goto remove;
	}

	read_event(&sta_event);
	call_event_handler(sta_event);

remove:
	if (exist_flag == 1) {
		logd("remove the netconf with the same ssid");
		/* netconf already exists */
		sprintf(cmd, "REMOVE_NETWORK %s", used_netid);
		wifi_command(cmd, reply, sizeof(reply));
	}

	sprintf(cmd, "%s", "SAVE_CONFIG");
	wifi_command(cmd, reply, sizeof(reply));

	if (enable_all_netconf() == -1) {
		loge("enable all networks error!\n");
		ret = -1;
	}

	/* make sure that the abnormal event count is reset */
	reset_abnormal_event_count();

exit:
	if (ret) {
		set_sta_state(STA_DISCONNECTED_STATE);
		if (connect_fail == 0)
			call_event_handler(STA_UNKNOWN_EVENT);
	}

	pthread_mutex_unlock(&scan_lock);
	return ret;
}

/**
 * sta_connect_with_bssid - connect to a hotspot for a specific bssid.
 *
 * @ssid: hotspot name.
 * @passwd: hotspot password.
 * @bssid: hotapot bssid.
 * @security: hotspot encryption mode.
 *
 * return 0 if success; otherwise failed.
 */
int sta_connect_with_bssid(const char *ssid, const char *passwd,
			   SECURITY security, const char *bssid)
{
	char cmd[128] = {0}, reply[32] = {0};
	char used_netid[8] = {0}, new_netid[8] = {0};
	int i = 0, ret = 0, exist_flag = 0, sta_event = 0, connect_fail = 0;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	if (!ssid) {
		loge("ssid is NULL!\n");
		call_event_handler(STA_UNKNOWN_EVENT);
		return -1;
	}

	while (get_delete_netconf_flag() == ON) {
		logd("waiting for the other delete netconf operation!\n");
		usleep(100000);
	}

	pthread_mutex_lock(&scan_lock);

	/* make sure that wifi is still on when the thread is woken up */
	if (is_wifi_enabled()) {
		loge("wifi is not enabled, switch_status: %d", get_switch_status());
		return -1;
	}

	/* check if the corresponding netconf exists */
	exist_flag = is_netconf_exist(ssid, security, used_netid);

	sprintf(cmd, "%s", "ADD_NETWORK");
	ret = wifi_command(cmd, new_netid, sizeof(new_netid));
	if (ret) {
		loge("add network failed!\n");
		ret = -1;
		goto exit;
	}

	sprintf(cmd, "SET_NETWORK %s ssid \"%s\"", new_netid, ssid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("set network ssid failed!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}

	sprintf(cmd, "SET_NETWORK %s bssid \%s\ ", new_netid, bssid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("set network bssid failed!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}

	if (security == SECURITY_OPEN) {
		sprintf(cmd, "SET_NETWORK %s key_mgmt NONE", new_netid);
		ret = wifi_command(cmd, reply, sizeof(reply));
		if (ret) {
			loge("set network key_mgmt failed!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}
	} else if (security == SECURITY_WPA_PSK ||
		 security == SECURITY_WPA2_PSK) {
		ret = is_passwd_legal(passwd);
		if (ret) {
			loge("wpa-psk passwd is illegal!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}

		sprintf(cmd, "SET_NETWORK %s psk \"%s\"", new_netid, passwd);
		ret = wifi_command(cmd, reply, sizeof(reply));
		if (ret) {
			loge("set network psk failed!\n");
			exception_handle(new_netid);
			ret = -1;
			goto exit;
		}
	} else {
		loge("security is not support!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}

	/* update the priority of new netconf */
	ret = update_priority(new_netid);
	if (ret) {
		loge("update priority failed!\n");
		ret = -1;
		goto exit;
	}

	set_connect_flag(ON);

	/* start connection */
	sprintf(cmd, "SELECT_NETWORK %s", new_netid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("select network failed!\n");
		exception_handle(new_netid);
		ret = -1;
		goto exit;
	}
	set_sta_state(STA_DISCONNECTED_STATE);

	/* wait for connection to end */
	do {
		usleep(100000);
		i++;
	} while ((i < 200) && (get_sta_state() == STA_DISCONNECTED_STATE) &&
				(get_connect_event() == UNKNOWN_EVENT));

	if (i == 200) {
		set_sta_state(STA_DISCONNECTED_STATE);
		set_connect_event(NETWORK_NOT_EXIST);
	}

	if (get_sta_state() == STA_DISCONNECTED_STATE) {
		if (get_connect_event() == NETWORK_NOT_EXIST)
			call_event_handler(STA_NETWORK_NOT_EXIST);
		else if (get_connect_event() == PASSWD_ERROR)
			call_event_handler(STA_PASSWD_ERROR);
		else if (get_connect_event() == CONNECT_REJECT)
			call_event_handler(STA_CONNECT_REJECT);
		else if (get_connect_event() == CONNECT_ABORT)
			call_event_handler(STA_CONNECT_ABORT);

		connect_fail = 1;
		set_connect_event(UNKNOWN_EVENT);
		set_connect_flag(OFF);
		exception_handle(new_netid);
		ret = -1;
		goto remove;
	}

	read_event(&sta_event);
	call_event_handler(sta_event);

remove:
	if (exist_flag == 1) {
		logd("remove the netconf with the same ssid");
		/* netconf already exists */
		sprintf(cmd, "REMOVE_NETWORK %s", used_netid);
		wifi_command(cmd, reply, sizeof(reply));
	}

	sprintf(cmd, "%s", "SAVE_CONFIG");
	wifi_command(cmd, reply, sizeof(reply));

	if (enable_all_netconf() == -1) {
		loge("enable all networks error!\n");
		ret = -1;
	}

	/* make sure that the abnormal event count is reset */
	reset_abnormal_event_count();

exit:
	if (ret) {
		set_sta_state(STA_DISCONNECTED_STATE);
		if (connect_fail == 0)
			call_event_handler(STA_UNKNOWN_EVENT);
	}

	pthread_mutex_unlock(&scan_lock);
	return ret;
}

/**
 * sta_connect_with_id - connect AP by using network id.
 *
 * @netid: network id.
 *
 * return 0 if success; otherwise failed.
 */
int sta_connect_with_id(int netid)
{
	char cmd[128] = {0}, reply[32] = {0}, used_netid[8] = {0};
	int i = 0, ret = 0, sta_event = 0, connect_fail = 0;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	while (get_delete_netconf_flag() == ON) {
		logd("waiting for the other delete netconf operation!\n");
		usleep(100000);
	}

	pthread_mutex_lock(&scan_lock);

	/* make sure that wifi is still on when the thread is woken up */
	if (is_wifi_enabled()) {
		loge("wifi is not enabled, switch_status: %d", get_switch_status());
		return -1;
	}

	sprintf(used_netid, "%d", netid);

	/* update the priority of new netconf */
	ret = update_priority(used_netid);
	if (ret) {
		loge("update priority failed!\n");
		ret = -1;
		goto exit;
	}

	set_connect_flag(ON);

	/* start connection */
	sprintf(cmd, "SELECT_NETWORK %s", used_netid);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("select network failed!\n");
		exception_handle(used_netid);
		ret = -1;
		goto exit;
	}
	set_sta_state(STA_DISCONNECTED_STATE);

	/* wait for connection to end */
	do {
		usleep(100000);
		i++;
	} while ((i < 200) && (get_sta_state() == STA_DISCONNECTED_STATE) &&
				(get_connect_event() == UNKNOWN_EVENT));

	if (i == 200) {
		set_sta_state(STA_DISCONNECTED_STATE);
		set_connect_event(NETWORK_NOT_EXIST);
	}

	if (get_sta_state() == STA_DISCONNECTED_STATE) {
		if (get_connect_event() == NETWORK_NOT_EXIST) {
			call_event_handler(STA_NETWORK_NOT_EXIST);
			exception_handle(used_netid);
		} else if (get_connect_event() == PASSWD_ERROR) {
			call_event_handler(STA_PASSWD_ERROR);
			exception_handle(used_netid);
		} else if (get_connect_event() == CONNECT_REJECT) {
			call_event_handler(STA_CONNECT_REJECT);
			exception_handle(used_netid);
		} else if (get_connect_event() == CONNECT_ABORT) {
			call_event_handler(STA_CONNECT_ABORT);
		}

		connect_fail = 1;
		set_connect_event(UNKNOWN_EVENT);
		set_connect_flag(OFF);
		ret = -1;
		goto enable;
	} else if (get_sta_state() == STA_CONNECTED_STATE) {
		sprintf(cmd, "%s", "SAVE_CONFIG");
		wifi_command(cmd, reply, sizeof(reply));
		ret = 0;
	}

	read_event(&sta_event);
	call_event_handler(sta_event);

enable:
	if (enable_all_netconf() == -1) {
		loge("enable all networks error!\n");
		ret = -1;
	}

	/* make sure that the abnormal event count is reset */
	reset_abnormal_event_count();

exit:
	if (ret) {
		set_sta_state(STA_DISCONNECTED_STATE);
		if (connect_fail == 0)
			call_event_handler(STA_UNKNOWN_EVENT);
	}

	pthread_mutex_unlock(&scan_lock);

	return ret;
}

/**
 * sta_disconnect - disconnect from AP.
 *
 * return 0 if success; otherwise failed.
 */
int sta_disconnect(void)
{
	char cmd[16] = {0}, reply[16] = {0};
	int ret = -1, sta_event;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	if (get_sta_state() != STA_CONNECTED_STATE) {
		loge("station is not connected!\n");
		return -1;
	}

	set_disconnect_flag(ON);

	sprintf(cmd, "%s", "DISCONNECT");
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("disconnect failed!\n");
		ret = -1;
		goto exit;
	}

	read_event(&sta_event);
	call_event_handler(sta_event);

exit:
	if (ret) {
		call_event_handler(STA_UNKNOWN_EVENT);
		set_disconnect_flag(OFF);
	}

	return ret;
}

/**
 * sta_scan - perform scan operation once.
 *
 * return 0 if success; otherwise failed.
 */
int sta_scan(void)
{
	char cmd[16] = {0}, reply[4096] = {0};
	int ret = -1, sta_event, i;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	pthread_mutex_lock(&scan_lock);

	/* make sure that wifi is still on when the thread is woken up */
	if (is_wifi_enabled()) {
		loge("wifi is not enabled, switch_status: %d", get_switch_status());
		return -1;
	}

	set_scan_flag(ON);

	sprintf(cmd, "%s", "SCAN");
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		/* make sure that the scan_result event is not mishandled */
		set_scan_flag(OFF);

		/* check if wifi is busy */
		if ((strncmp(reply, "FAIL-BUSY", 9) == 0)) {
			logd("wifi is scanning because the wifi is trying to connect \
					automatically!\n");
			if (disable_all_netconf() == -1) {
				loge("disable all networks error!\n");
				ret = -1;
				goto exit;
			}

			sprintf(cmd, "%s", "SCAN");
			for (i = 0; i < 20; i++) {
				if (is_wifi_enabled()) {
					loge("wifi is disabled");
					ret = -1;
					goto exit;
				}

				set_scan_flag(ON);
				ret = wifi_command(cmd, reply, sizeof(reply));
				if (ret) {
					/* make sure that the scan_result event is not mishandled */
					set_scan_flag(OFF);

					/*  check if wifi is busy */
					if ((i != 19) && (strncmp(reply,
						"FAIL-BUSY", 9) == 0)) {
						logw("wifi is busy, retry to send command!");
						usleep(300000);
						continue;
					} else {
						loge("scan failed");
						ret = -1;
						goto exit;
					}
				}
				break;
			}

			if (enable_all_netconf() == -1) {
				loge("enable all networks error!\n");
				ret = -1;
				goto exit;
			}
		} else {
			loge("scan failed!\n");
			ret = -1;
			goto exit;
		}
	}

	read_event(&sta_event);
	call_event_handler(sta_event);

exit:
	if (ret) {
		call_event_handler(STA_UNKNOWN_EVENT);
		set_scan_flag(OFF);
	}

	pthread_mutex_unlock(&scan_lock);

	return ret;
}

/**
 * sta_scan_results - obtain scan results.
 *
 * @scan_info: buffer for storing scan information.
 *
 * return 0 if success; otherwise failed.
 */
int sta_scan_results(SCAN_INFO *scan_info)
{
	char cmd[16] = {0}, reply[4096] = {0};
	int ret = -1;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	if (scan_info == NULL) {
		loge("scan info is NULL!\n");
		call_event_handler(STA_UNKNOWN_EVENT);
		return -1;
	}

	set_scan_result_flag(ON);

	sprintf(cmd, "%s", "SCAN_RESULTS");
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("obtain scan results failed!\n");
		ret = -1;
		goto exit;
	}

	set_scan_result_flag(OFF);

	ret = parse_scan_result(reply, scan_info);
	if (ret) {
		loge("parse scan results failed!\n");
		ret = -1;
		goto exit;
	}

exit:
	if (ret == -1) {
		scan_info->num = 0;
		scan_info->info = NULL;
		call_event_handler(STA_UNKNOWN_EVENT);
	}

	return ret;
}

/**
 * sta_get_info - obtain current connection status information.
 *
 * @connect_info: buffer for storing connection status information.
 *
 * return 0 if success; otherwise failed.
 */
int sta_get_connect_info(CONNECT_INFO *connect_info)
{
	char cmd[16] = {0}, reply[1024] = {0};
	int ret = -1;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	if (connect_info == NULL) {
		loge("connect info is NULL!\n");
		call_event_handler(STA_UNKNOWN_EVENT);
		return -1;
	}

	pthread_mutex_lock(&scan_lock);

	/* make sure that wifi is still on when the thread is woken up */
	if (is_wifi_enabled()) {
		loge("wifi is not enabled, switch_status: %d", get_switch_status());
		return -1;
	}

	set_status_flag(ON);
	sprintf(cmd, "STATUS");
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("obtain current connection status information failed!\n");
		call_event_handler(STA_UNKNOWN_EVENT);
		ret = -1;
		goto exit;
	}

	if (strstr(reply, "wpa_state=COMPLETED") != NULL) {
		logd("trigger connected event by status command!\n");
		ret = parse_status_result(reply, connect_info);
		if (ret) {
			loge("parse return reply by status command failed!\n");
			call_event_handler(STA_UNKNOWN_EVENT);
			ret = -1;
		} else {
			ret = 0;
		}
	} else {
		loge("station is not connected!\n");
		set_status_flag(OFF);
		ret = -1;
	}

exit:
	if (ret == -1)
		memset(connect_info, 0, sizeof(CONNECT_INFO));

	pthread_mutex_unlock(&scan_lock);
	return ret;
}

/**
 * sta_disable_net_conf - disable network configuration.
 *
 * @net_id: network id.
 *
 * return 0 if success; otherwise failed.
 */
int sta_disable_net_conf(int net_id)
{
	char cmd[32] = {0}, reply[1024] = {0};
	int ret = -1;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	sprintf(cmd, "DISABLE_NETWORK %d", net_id);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("disable network config failed!\n");
		ret = -1;
		goto exit;
	}

exit:
	if (ret)
		call_event_handler(STA_UNKNOWN_EVENT);

	return ret;
}

/**
 * sta_delete_net_conf - delete network configuration.
 *
 * @net_id: network id.
 *
 * return 0 if success; otherwise failed.
 */
int sta_delete_net_conf(int net_id)
{
	char cmd[32] = {0}, reply[1024] = {0};
	int ret = -1;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	set_delete_netconf_flag(ON);
	sprintf(cmd, "REMOVE_NETWORK %d", net_id);
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("remove network config failed!\n");
		ret = -1;
		goto exit;
	}

	sprintf(cmd, "%s", "SAVE_CONFIG");
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("save config failed!\n");
		ret = -1;
		goto exit;
	}

exit:
	if (ret)
		call_event_handler(STA_UNKNOWN_EVENT);

	set_delete_netconf_flag(OFF);
	return ret;
}

/**
 * sta_delete_all_net_conf - delete all network configuration.
 *
 * return 0 if success; otherwise failed.
 */
int sta_delete_all_net_conf()
{
	int ret = -1;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	ret = remove_all_netconf();
	if (ret) {
		loge("remove all network config failed!\n");
		ret = -1;
		goto exit;
	}

exit:
	if (ret)
		call_event_handler(STA_UNKNOWN_EVENT);

	return ret;
}

/**
 * sta_disable_all_net_conf - disable all network configuration.
 *
 * return 0 if success; otherwise failed.
 */
int sta_disable_all_net_conf()
{
	int ret = 0;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	if (get_connect_flag() == ON) {
		if (disable_all_netconf() == -1) {
			loge("disable all networks error!\n");
			ret = -1;
			goto exit;
		} else {
			logi("connection is aborted!\n");
			set_sta_state(STA_DISCONNECTED_STATE);
			set_connect_event(CONNECT_ABORT);
		}
	}

exit:
	if (ret)
		call_event_handler(STA_UNKNOWN_EVENT);

	return ret;
}


/**
 * sta_get_net_id - obtain network id.
 *
 * @ssid: ssid for the specified netconf.
 * @security: security for the specified netconf.
 * @netid: buffer for storing netid if the specified netconf exists.
 *
 * return 0 if success; otherwise failed.
 */
int sta_get_net_id(const char *ssid, SECURITY security, int *netid)
{
	int ret = -1, exist_flag = 0;
	char tmp_netid[8] = {0};

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	pthread_mutex_lock(&scan_lock);

	/* make sure that wifi is still on when the thread is woken up */
	if (is_wifi_enabled()) {
		loge("wifi is not enabled, switch_status: %d", get_switch_status());
		return -1;
	}

	exist_flag = is_netconf_exist(ssid, security, tmp_netid);
	if (exist_flag == 1) {
		ret = 0;
		*netid = atoi(tmp_netid);
	} else {
		loge("obtain network configuration failed!");
		ret = -1;
	}

exit:
	if (exist_flag == -1)
		call_event_handler(STA_UNKNOWN_EVENT);

	pthread_mutex_unlock(&scan_lock);

	return ret;
}

/**
 * sta_get_save_config - obtain all saved wifi configuration information.
 *
 * @saved_wifi_config: buffer for storing saved wifi configuration.
 *
 * return 0 if success; otherwise failed.
 */
int sta_get_saved_config(SAVED_WIFI_CONFIG *saved_wifi_config)
{
	int ret = -1;
	WIFI_CONFIG *tmp_config = NULL;

	if (is_wifi_enabled()) {
		loge("wifi is not enabled");
		return -1;
	}

	if (saved_wifi_config == NULL) {
		loge("saved_wifi_config is NULL!\n");
		call_event_handler(STA_UNKNOWN_EVENT);
		return -1;
	}

	pthread_mutex_lock(&scan_lock);

	/* make sure that wifi is still on when the thread is woken up */
	if (is_wifi_enabled()) {
		loge("wifi is not enabled, switch_status: %d", get_switch_status());
		return -1;
	}

	ret = obtain_saved_config_num(&(saved_wifi_config->num));
	if (ret == -1) {
		loge("obtain saved config num failed!\n");
		goto exit;
	}
	logi("save_wifi_config->num: %d!\n", saved_wifi_config->num);

	if (saved_wifi_config->num) {
		tmp_config = (WIFI_CONFIG *)malloc(sizeof(WIFI_CONFIG) *
				saved_wifi_config->num);
		if (tmp_config == NULL) {
			loge("malloc error!\n");
			goto exit;
		}
	} else {
		logi("There is no saved wifi config in the device!\n");
		if ((saved_wifi_config->config != NULL))
			free(saved_wifi_config->config);
		saved_wifi_config->config = NULL;
		ret = 0;
		goto exit;
	}
	memset(tmp_config, 0, sizeof(WIFI_CONFIG) * saved_wifi_config->num);

	ret = obtain_saved_config(tmp_config);
	if (ret == -1) {
		loge("obtain saved config failed!\n");
		goto exit;
	}

	if ((saved_wifi_config->config != NULL))
		free(saved_wifi_config->config);

	saved_wifi_config->config = tmp_config;

exit:
	if (ret == -1) {
		saved_wifi_config->num = 0;
		saved_wifi_config = NULL;
		call_event_handler(STA_UNKNOWN_EVENT);
	}

	pthread_mutex_unlock(&scan_lock);

	return ret;
}
