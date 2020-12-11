#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "wifi_common.h"
#include "state.h"
#include "net_conf_parser.h"

/*
 * str_to_utf8 - convert string data to utf-8 format data.
 *
 * @buf: buffer for storing string data.
 * @dst: buffer for storing utf-8 format data.
 *
 * return 0 if success; otherwise failed.
 */
int str_to_utf8(const char *buf, char *dst)
{
	char tmp_buf[1024] = {0};
	int i = 0, hex = 0;

	if ((buf == NULL) || (dst == NULL)) {
		loge("parameter is invalid!\n");
		return -1;
	}

	while (*buf != '\0') {
		if (!strncmp(buf, "\\x", 2)) {
			strncpy(tmp_buf, buf, 4);
			*tmp_buf = '0';
			*(tmp_buf + 4) = '\0';
			hex = strtoul(tmp_buf, NULL, 16);
			dst[i] = hex;
			buf += 3;
		} else if (!strncmp(buf, "\\\"", 2) ||
				!strncmp(buf, "\\\\", 2)) {
			/* compatible with '\' and '"' */
			dst[i] = *(++buf);
		} else {
			dst[i] = *buf;
		}
		i++;
		buf++;

		/* avoid memory overruns */
		if (i == 32)
			break;
	}
	dst[i] = '\0';

	return 0;
}

/**
 * traverse_netconf - traverse all netconf.
 *
 * @operation: operation performed while traversing.
 * @data: buffer for storing data obtained while traversing.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
static int traverse_netconf(NET_CONF_OPERATION operation, void *data)
{
	char cmd[128] = {0}, reply[1024] = {0}, netid[8] = {0}, tmp_ssid[64] = {0};
	char *ptr_start = NULL, *ptr_end = NULL;
	int ret = -1, netid_len = 0, cycle_flag = 0, max_priority = 0;
	int ssid_len = 0;

	sprintf(cmd, "LIST_NETWORKS");
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("list networks failed!\n");
		ret = -1;
		goto exit;
	}

	for (ptr_start = strchr(reply, '\n'); ptr_start != NULL;
		ptr_start = strchr(ptr_start, '\n')) {
		cycle_flag++;

		/* obtain netid */
		ptr_start++;
		ptr_end = strchr(ptr_start, '\t');
		if (ptr_end != NULL) {
			netid_len = ptr_end - ptr_start;
			strncpy(netid, ptr_start, netid_len);
			netid[netid_len] = '\0';
		}

		switch (operation) {
		case ENABLE:
			sprintf(cmd, "ENABLE_NETWORK %s", netid);
			ret = wifi_command(cmd, reply, sizeof(reply));
			if (ret) {
				loge("enable network failed!\n");
				ret = -1;
				goto exit;
			}
			break;

		case DISABLE:
			sprintf(cmd, "DISABLE_NETWORK %s", netid);
			ret = wifi_command(cmd, reply, sizeof(reply));
			if (ret) {
				loge("disable network failed!\n");
				ret = -1;
				goto exit;
			}
			break;

		case REMOVE:
			sprintf(cmd, "REMOVE_NETWORK %s", netid);
			ret = wifi_command(cmd, reply, sizeof(reply));
			if (ret) {
				loge("remove network failed!\n");
				ret = -1;
				goto exit;
			}
			break;

		case OBTAIN_MAX_PRIO:
			sprintf(cmd, "GET_NETWORK %s priority", netid);
			ret = wifi_command(cmd, reply, sizeof(reply));
			if (ret) {
				loge("get network priority failed!\n");
				ret = -1;
				goto exit;
			}

			if (atoi(reply) >= max_priority)
				max_priority = atoi(reply);
			*(int *)data = max_priority;
			break;

		case OBTAIN_SAVED_CONFIG_NUM:
			*(int *)data = cycle_flag;
			break;

		case OBTAIN_SAVED_CONFIG:
			((WIFI_CONFIG *)data)[cycle_flag - 1].net_id = atoi(netid);

			/* obtain ssid */
			ptr_start = ptr_end + 1;
			ptr_end = strchr(ptr_start, '\t');
			if (ptr_end != NULL) {
				ssid_len = ptr_end - ptr_start;
				strncpy(tmp_ssid, ptr_start, ssid_len);
				tmp_ssid[ssid_len] = '\0';
			}

			str_to_utf8(tmp_ssid, ((WIFI_CONFIG *)data)[cycle_flag - 1].ssid);
			loge("network ssid: %s!\n",
					((WIFI_CONFIG *)data)[cycle_flag - 1].ssid);

			/* obtain key_mgmt */
			sprintf(cmd, "GET_NETWORK %s key_mgmt", netid);
			ret = wifi_command(cmd, reply, sizeof(reply));
			if (ret) {
				loge("get network key_mgmt failed!\n");
				ret = -1;
				goto exit;
			}
			if (strstr(reply, "WPA-PSK") != NULL)
				((WIFI_CONFIG *)data)[cycle_flag - 1].security =
					SECURITY_WPA2_PSK;
			else if (strstr(reply, "NONE") != NULL)
				((WIFI_CONFIG *)data)[cycle_flag - 1].security =
					SECURITY_OPEN;

			break;

		default:
			break;
		}
	}

	if (cycle_flag == 0) {
		ret = -2;
		loge("The system does not have any network configuration!\n");
	}
exit:
	if (ret != -1) {
		if ((operation != OBTAIN_MAX_PRIO) &&
			(operation != DISABLE) && (operation != OBTAIN_SAVED_CONFIG_NUM) &&
			(operation != OBTAIN_SAVED_CONFIG) && (ret == 0)) {
			sprintf(cmd, "%s", "SAVE_CONFIG");
			ret = wifi_command(cmd, reply, sizeof(reply));
			if (ret) {
				loge("save config failed!\n");
				ret = -1;
			}
		}
	}

	return ret;
}

/**
 * enable_all_netconf - enable all netconf.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int enable_all_netconf(void)
{
	return traverse_netconf(ENABLE, NULL);
}

/**
 * disable_all_netconf - disable all netconf.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int disable_all_netconf(void)
{
	return traverse_netconf(DISABLE, NULL);
}

/**
 * remove_all_netconf - remove all netconf.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int remove_all_netconf(void)
{
	return traverse_netconf(REMOVE, NULL);
}

/**
 * obtain_max_priority - obtain the highest priority.
 *
 * @max_priority: buffer for storing the highest priority.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int obtain_max_priority(int *max_priority)
{
	return traverse_netconf(OBTAIN_MAX_PRIO, max_priority);
}

/**
 * obtain_saved_config_num - obtain the number of saved wifi configuration.
 *
 * @saved_config_num: number of saved hotspot.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int obtain_saved_config_num(int *saved_config_num)
{
	return traverse_netconf(OBTAIN_SAVED_CONFIG_NUM, saved_config_num);
}

/**
 * obtain_saved_config - obtain all saved wifi configuration.
 *
 * @save_config: configuration of saved hotspot.
 *
 * return 0 if success; -1 if it is failed to obtain netconf;
 * -2 if no netconf exists.
 */
int obtain_saved_config(WIFI_CONFIG *saved_config)
{
	return traverse_netconf(OBTAIN_SAVED_CONFIG, saved_config);
}

/**
 * is_sta_connected - check if station is connected.
 *
 * return 0 if station is not connected; 1 if station is connected.
 */
int is_sta_connected(void)
{
	char cmd[16] = {0}, reply[1024] = {0};
	int ret = -1;

	set_status_flag(ON);
	sprintf(cmd, "STATUS");
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("get status failed!\n");
		return -1;
	}

	if (strstr(reply, "wpa_state=COMPLETED") != NULL) {
		logd("trigger connected event by status command!\n");
		return 1;
	} else {
		set_status_flag(OFF);
		return 0;
	}
}

/**
 * is_netconf_exist - check if the specified netconf exists.
 *
 * @ssid: ssid for the specified netconf.
 * @security: security for the specified netconf.
 * @netid: buffer for storing netid if the specified netconf exists.
 *
 * return 0 if the specified netconf does not exists;
 * 1 if the specified netconf exists;
 * -1 if it is failed to obtain netconf.
 */
int is_netconf_exist(const char *ssid, SECURITY security, char *netid)
{
	char cmd[128] = {0}, reply[1024] = {0}, expected_key[16] = {0};
	char real_ssid[64] = {0}, real_netid[8] = {0}, real_key[16] = {0};
	char tmp_ssid[64] = {0};
	char *ptr_start = NULL, *ptr_end = NULL;
	int ret = -1, ssid_len = 0, netid_len = 0;

	sprintf(cmd, "LIST_NETWORKS");
	ret = wifi_command(cmd, reply, sizeof(reply));
	if (ret) {
		loge("list networks failed!\n");
		ret = -1;
		goto exit;
	}

	switch (security) {
	case SECURITY_OPEN:
		sprintf(expected_key, "NONE");
		break;

	case SECURITY_WPA_PSK:

	case SECURITY_WPA2_PSK:
		sprintf(expected_key, "WPA-PSK");
		break;

	default:
		loge("security is illegal!\n");
	}

	for (ptr_start = strchr(reply, '\n'); ptr_start != NULL;
		ptr_start = strchr(ptr_start, '\n')) {
		ptr_start++;
		/* obtain real netid */
		ptr_end = strchr(ptr_start, '\t');
		if (ptr_end != NULL) {
			netid_len = ptr_end - ptr_start;
			strncpy(real_netid, ptr_start, netid_len);
			real_netid[netid_len] = '\0';
		}

		sprintf(cmd, "GET_NETWORK %s key_mgmt", real_netid);
		ret = wifi_command(cmd, real_key, sizeof(real_key));
		if (ret) {
			loge("get network key_mgmt failed!\n");
			ret = -1;
			goto exit;
		}
		/* logd("real_netid: %s, real_key: %s, expected_key: %s!\n",
				real_netid, real_key, expected_key); */

		if (strncmp(real_key, expected_key,
			strlen(expected_key)) == 0) {
			/* obtain real ssid */
			ptr_start = ptr_end + 1;
			ptr_end = strchr(ptr_start, '\t');
			if (ptr_end != NULL) {
				ssid_len = ptr_end - ptr_start;
				strncpy(tmp_ssid, ptr_start, ssid_len);
				tmp_ssid[ssid_len] = '\0';
			}

			str_to_utf8(tmp_ssid, real_ssid);

			/*  logd("real_ssid: %s, expected_ssid: %s!\n",
					real_ssid, ssid); */
			if (strcmp(real_ssid, ssid) == 0) {
				strncpy(netid, real_netid, netid_len);
				netid[netid_len] = '\0';
				/* logd("netid: %s!\n", netid); */
				ret = 1;
				break;
			}
		}
	}

exit:
	return ret;
}
