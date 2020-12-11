/*
 * onvif_param.c - Interface to parameters required by onvif
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

#define _XOPEN_SOURCE

#include "onvif_param.h"
#include "onvif_server.h"
#ifndef __MY_TEST__
#include "net_utils.h"
#include "ip_config.h"
#include "db_config.h"
#endif /* __MY_TEST__ */

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/ether.h>
#include <pthread.h>

#ifndef __MY_TEST__
#define LOG_TAG "ONVIF_NVT"
#include <log/log.h>
#endif /* __MY_TEST__ */

#define DBCONF_IPC_DEVICE "ipc_device"

#define DBCONF_IPC_DEV_NAME "dev_name"
#define DBCONF_IPC_DEV_NAME_DEF_VALUE "ipcam"

#define DBCONF_IPC_LOCATION "location"
#define DBCONF_IPC_LOCATION_DEF_VALUE "city/shezhen"

#define DBCONF_IPC_MANUFACTURER "manufacturer"
#define DBCONF_IPC_MANUFACTURER_DEF_VALUE "ABC tech."

#define DBCONF_IPC_MODEL "model"
#define DBCONF_IPC_MODEL_DEF_VALUE "lombo-123"

#define DBCONF_IPC_VERSION "version"
#define DBCONF_IPC_VERSION_DEF_VALUE "v1.0.0.0"

#define DBCONF_IPC_SERIAL_NO "serial_no"
#define DBCONF_IPC_SERIAL_NO_DEF_VALUE "ABCD01234"

#define DBCONF_IPC_HARDWARE_ID "hardware_id"
#define DBCONF_IPC_HARDWARE_ID_DEF_VALUE "1234"

#define DBCONF_IPC_VSRC_TOKEN "vsrc_token"
#define DBCONF_IPC_VSRC_TOKEN_DEF_VALUE "VideoSource0"

#define DBCONF_IPC_VSRC_FRAMERATE "vsrc_framerate"
#define DBCONF_IPC_VSRC_FRAMERATE_DEF_VALUE 30

#define DBCONF_IPC_VSRC_WIDTH "vsrc_width"
#define DBCONF_IPC_VSRC_WIDTH_DEF_VALUE 1920

#define DBCONF_IPC_VSRC_HEIGHT "vsrc_height"
#define DBCONF_IPC_VSRC_HEIGHT_DEF_VALUE 1080

#define DBCONF_IPC_VENC_TOKEN "venc_token"
#define DBCONF_IPC_VENC_TOKEN_DEF_VALUE "VideoEncoder"

#define DBCONF_IPC_VENC_TYPE "venc_type"
#define DBCONF_IPC_VENC_TYPE_DEF_VALUE 2

#define DBCONF_IPC_VENC_FRAMERATE "venc_framerate"
#define DBCONF_IPC_VENC_FRAMERATE_DEF_VALUE 30

#define DBCONF_IPC_VENC_BITRATE "venc_bitrate"
#define DBCONF_IPC_VENC_BITRATE_DEF_VALUE 2000

#define DBCONF_IPC_VENC_INTERVAL "venc_interval"
#define DBCONF_IPC_VENC_INTERVAL_DEF_VALUE 1

#define DBCONF_IPC_VENC_QUALITY "venc_quality"
#define DBCONF_IPC_VENC_QUALITY_DEF_VALUE 1

#define DBCONF_IPC_VENC_WIDTH "venc_width"
#define DBCONF_IPC_VENC_HEIGHT "venc_height"

#define DBCONF_IPC_VENC_GOV_LENGTH "venc_gov_length"
#define DBCONF_IPC_VENC_GOV_LENGTH_DEF_VALUE 32

#define DBCONF_IPC_VENC_PROFILE "venc_profile"
#define DBCONF_IPC_VENC_PROFILE_DEF_VALUE 0

#define DBCONF_IPC_MEDIA_PROFILE "media_profile"

#define DBCONF_IPC_MEDIA_URI "media_uri"

#define DBCONF_IPC_SNAPSHOT_URI "snapshot_uri"

#define DBCONF_IPC_HOST_NAME "dev_hostname"
#define DBCONF_IPC_HOST_NAME_DEF_VALUE "AI-IPC"

#define DBCONF_IPC_NTP_SERVER "ntp_server"
#define DBCONF_IPC_NTP_SERVER_DEF_VALUE "cn.pool.ntp.org"

#define DBCONF_IPC_VENC_GOV_LENGTH "venc_gov_length"
#define DBCONF_IPC_VENC_GOV_LENGTH_DEF_VALUE 32

#define DBCONF_IPC_ONVIF_PORT "onvif_port"
#define DBCONF_IPC_ONVIF_PORT_DEF_VALUE 2085

#define DBCONF_IPC_HTTP_PORT "http_port"
#define DBCONF_IPC_HTTP_PORT_DEF_VALUE 8085

#define DBCONF_IPC_RTSP_PORT "rtsp_port"
#define DBCONF_IPC_RTSP_PORT_DEF_VALUE 554

#define DBCONF_IPC_USER_NAME "user_name"
#define DBCONF_IPC_USER_NAME_DEF_VALUE ""

#define DBCONF_IPC_PASSWORD "password"
#define DBCONF_IPC_PASSWORD_DEF_VALUE ""

#define DBCONF_IPC_USER_LEVEL "user_level"
#define DBCONF_IPC_USER_LEVEL_DEF_VALUE 2

#define DBCONF_IPC_TIMEZONE_INDEX "timezone_index"
#define DBCONF_IPC_TIMEZONE_INDEX_DEF_VALUE 100

#ifdef __MY_TEST__
/* device name */
static char s_dev_name[MAX_NAME_LEN];

/* location information */
static char s_location[MAX_NAME_LEN];
#endif /* __MY_TEST__ */

/* net parameter */
static net_param_t s_net_param;

#ifdef __MY_TEST__
/* host name */
static char s_hostname[MAX_NAME_LEN];

/* ntp server */
static char s_ntp_server[MAX_NAME_LEN];

/* http port */
static int s_http_port;

/* rtsp port */
static int s_rtsp_port;

/* timezone index */
static int s_tz_index;
#endif /* __MY_TEST__ */

/* user list */
static user_info_t s_user_list[MAX_USER_NUM];

/**
 * onvif_get_dev_name: get device name
 * @dev_name: a pointer to device name.
 * @len: length of device name buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_dev_name(char *dev_name, int len)
{
	if (!dev_name || len < MAX_NAME_LEN)
		return -1;
#ifdef __MY_TEST__
	if (!strlen(s_dev_name))
		strcpy(s_dev_name, "ipcam");
	strcpy(dev_name, s_dev_name);

	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_DEV_NAME,
			dev_name, len,
			DBCONF_IPC_DEV_NAME_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_set_dev_name: set device name
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_dev_name(const char *dev_name)
{
	if (!dev_name)
		return -1;
#ifdef __MY_TEST__
	snprintf(s_dev_name, sizeof(s_dev_name), "%s", dev_name);

	return 0;
#else
	return db_config_set_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_DEV_NAME,
			(char *)dev_name);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_location: get location
 * @location: a pointer to location information.
 * @len: length of location information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_location(char *location, int len)
{
	if (!location || len < MAX_NAME_LEN)
		return -1;
#ifdef __MY_TEST__
	if (!strlen(s_location))
		strcpy(s_location, "city/shenzhen");
	strcpy(location, s_location);

	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_LOCATION,
			location, len,
			DBCONF_IPC_LOCATION_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_set_location: set location
 * @location: location information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_location(const char *location)
{
	if (!location)
		return -1;
#ifdef __MY_TEST__
	snprintf(s_location, sizeof(s_location), "%s", location);

	return 0;
#else
	return db_config_set_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_LOCATION,
			(char *)location);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_manufacturer: get manufacturer
 * @mfacture: a pointer to manufacturer information.
 * @len: length of manufacturer information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_manufacturer(char *mfacture, int len)
{
	if (!mfacture || len < MAX_NAME_LEN)
		return -1;
#ifdef __MY_TEST__
	strcpy(mfacture, "ABC tech.");

	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_MANUFACTURER,
			mfacture, len,
			DBCONF_IPC_MANUFACTURER_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_device_model: get deivce model
 * @model: a pointer to model.
 * @len: length of model buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_device_model(char *model, int len)
{
	if (!model || len < MAX_NAME_LEN)
		return -1;
#ifdef __MY_TEST__
	strcpy(model, "lombo-123");
	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_MODEL,
			model, len,
			DBCONF_IPC_MODEL_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_device_version: get deivce version
 * @version: a pointer to version information.
 * @len: length of version infromation buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_device_version(char *version, int len)
{
	if (!version || len < MAX_NAME_LEN)
		return -1;
#ifdef __MY_TEST__
	strcpy(version, "v1.0.0.0");
	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_VERSION,
			version, len,
			DBCONF_IPC_VERSION_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_serial_no: get serial no
 * @serial_no: a pointer to serial no.
 * @len: length of serial no buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_serial_no(char *serial_no, int len)
{
	if (!serial_no || len < MAX_NAME_LEN)
		return -1;
#ifdef __MY_TEST__
	strcpy(serial_no, "ABCD01234");
	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_SERIAL_NO,
			serial_no, len,
			DBCONF_IPC_SERIAL_NO_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_hardware_id: get hardware id
 * @hardware_id: a pointer to hardware id.
 * @len: length of hardware id buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_hardware_id(char *hardware_id, int len)
{
	if (!hardware_id || len < MAX_NAME_LEN)
		return -1;
#ifdef __MY_TEST__
	strcpy(hardware_id, "1234");
	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_HARDWARE_ID,
			hardware_id, len,
			DBCONF_IPC_HARDWARE_ID_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_video_source_info: get video source info
 * @vsource: a pointer to video source information.
 * @len: length of video source information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_video_source_info(video_source_t *vsource, int len)
{
	if (!vsource || (len < sizeof(video_source_t)))
		return -1;
#ifdef __MY_TEST__
	strcpy(vsource->token, "VideoSource0");
	vsource->frame_rate = 30;
	vsource->width = 1920;
	vsource->height = 1080;

	return 0;
#else
	int ret;
	ret = db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_VSRC_TOKEN,
			vsource->token, sizeof(vsource->token),
			DBCONF_IPC_VSRC_TOKEN_DEF_VALUE);

	vsource->frame_rate = db_config_get_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_VSRC_FRAMERATE,
			DBCONF_IPC_VSRC_FRAMERATE_DEF_VALUE);

	vsource->width = db_config_get_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_VSRC_WIDTH,
			DBCONF_IPC_VSRC_WIDTH_DEF_VALUE);

	vsource->height = db_config_get_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_VSRC_HEIGHT,
			DBCONF_IPC_VSRC_HEIGHT_DEF_VALUE);

	return ret;
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_video_encoder_info: get video encoder info
 * @index: encoder no.
 * @vencoder: a pointer to video encoder information.
 * @len: length of video encoder information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_video_encoder_info(int index,
		video_encoder_t *vencoder, int len)
{
	if ((index < 0 || index > MAX_ENCODER_NUM)
		|| !vencoder || (len < sizeof(video_encoder_t)))
		return -1;
#ifdef __MY_TEST__
	switch (index) {
	case 0:
		strcpy(vencoder->token, "VideoEncoder0");
		vencoder->encode_type = 2;
		vencoder->frame_rate = 30;
		vencoder->bit_rate = 2000;
		vencoder->encode_interval = 1;
		vencoder->quality = 1;
		vencoder->width = 1920;
		vencoder->height = 1080;
		vencoder->gov_length = 32;
		vencoder->profile = 0;
		break;

	default:
		strcpy(vencoder->token, "VideoEncoder1");
		vencoder->encode_type = 2;
		vencoder->frame_rate = 30;
		vencoder->bit_rate = 1000;
		vencoder->encode_interval = 1;
		vencoder->quality = 1;
		vencoder->width = 1280;
		vencoder->height = 720;
		vencoder->gov_length = 32;
		vencoder->profile = 0;
		break;
	}

	return 0;
#else
	int ret, def_intvalue;
	char key[256], def_value[256];
	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_TOKEN, index);
	snprintf(def_value, sizeof(def_value), "VideoEncoder%d", index);
	ret = db_config_get_string(DBCONF_IPC_DEVICE,
			key,
			vencoder->token, sizeof(vencoder->token),
			def_value);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_TYPE, index);
	vencoder->encode_type = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			DBCONF_IPC_VENC_TYPE_DEF_VALUE);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_FRAMERATE, index);
	vencoder->frame_rate = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			DBCONF_IPC_VENC_TYPE_DEF_VALUE);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_BITRATE, index);
	def_intvalue = index == 0 ? 2000 : 1000;
	vencoder->bit_rate = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			def_intvalue);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_INTERVAL, index);
	vencoder->encode_interval = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			DBCONF_IPC_VENC_INTERVAL_DEF_VALUE);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_QUALITY, index);
	vencoder->quality = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			DBCONF_IPC_VENC_QUALITY_DEF_VALUE);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_WIDTH, index);
	def_intvalue = index == 0 ? 1920 : 1280;
	vencoder->width = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			def_intvalue);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_HEIGHT, index);
	def_intvalue = index == 0 ? 1080 : 720;
	vencoder->height = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			def_intvalue);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_GOV_LENGTH, index);
	vencoder->gov_length = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			DBCONF_IPC_VENC_GOV_LENGTH_DEF_VALUE);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_VENC_PROFILE, index);
	vencoder->gov_length = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			DBCONF_IPC_VENC_PROFILE_DEF_VALUE);

	return ret;
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_media_profile: get media profile
 * @index: encoder no.
 * @profile: a pointer to media profile information.
 * @len: length of media profile information buffer.

 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_media_profile(int index, char *profile, int len)
{
	if ((index < 0 || index > MAX_ENCODER_NUM)
		|| !profile || (len < MAX_NAME_LEN))
		return -1;
#ifdef __MY_TEST__
	switch (index) {
	case 0:
		strcpy(profile, "MajorStream");
		break;

	default:
		strcpy(profile, "MinorStream");
		break;
	}

	return 0;
#else
	char key[256], def_value[256];
	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_MEDIA_PROFILE, index);
	if (0 == index)
		snprintf(def_value, sizeof(def_value), "%s", "MajorStream");
	else
		snprintf(def_value, sizeof(def_value), "%s", "MinorStream");

	return db_config_get_string(DBCONF_IPC_DEVICE,
				key,
				profile, len,
				def_value);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_media_uri: get media uri
 * @profile: media profile.
 * @uri: a pointer to media uri information.
 * @len: length of media uri information buffer.
 *
 * return 0 if success; otherwise failed.
 */
extern int net_dev_get_ip_addr(const char *dev_name, unsigned long *ip_addr);
int onvif_get_media_uri(const char *profile, char *uri, int len)
{
#ifndef __MY_TEST__
	int rtsp_port;
	unsigned long ip_addr;
	char key[256], flag[256], def_flag[256];
#endif
	if (!profile || !uri || (len < MAX_URI_LEN))
		return -1;
#ifndef __MY_TEST__
	rtsp_port = db_config_get_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_RTSP_PORT,
			DBCONF_IPC_RTSP_PORT_DEF_VALUE);
	net_dev_get_ip_addr("eth0", &ip_addr);
	if (!strcmp(profile, "MajorStream")) {
		snprintf(key, sizeof(key), "%s0", DBCONF_IPC_MEDIA_URI);
		if (554 == rtsp_port)
			snprintf(def_flag, sizeof(def_flag),
				"/%s", "live");
		else
			snprintf(def_flag, sizeof(def_flag),
				":%d/%s", rtsp_port, "live");
		db_config_get_string(DBCONF_IPC_DEVICE, key,
				flag, sizeof(key), def_flag);

	} else {
		snprintf(key, sizeof(key), "%s1", DBCONF_IPC_MEDIA_URI);
		if (554 == rtsp_port)
			snprintf(def_flag, sizeof(def_flag),
				"/%s", "live");
		else
			snprintf(def_flag, sizeof(def_flag),
				":%d/%s", rtsp_port, "live");
		db_config_get_string(DBCONF_IPC_DEVICE, key,
				flag, sizeof(key), def_flag);
	}
	sprintf(uri, "rtsp://%d.%d.%d.%d%s",
			(unsigned int)(ip_addr & 0xff),
			(unsigned int)((ip_addr >> 8) & 0xff),
			(unsigned int)((ip_addr >> 16) & 0xff),
			(unsigned int)((ip_addr >> 24) & 0xff),
			flag);
#else
#if 0
	if (!strcmp(profile, "MajorStream"))
		snprintf(uri, len, "rtsp://%s/%s",
			"192.168.39.132",
			"test1.264");
	else
		snprintf(uri, len, "rtsp://%s/%s",
			"192.168.39.132",
			"test2.264");
#else
	snprintf(uri, len, "rtsp://%s/live", onvif_get_ip_addr());
#endif
#endif /* __MY_TEST__ */
	return 0;
}

/**
 * onvif_get_snapshot_uri: get snapshot uri
 * @profile: snapshot profile.
 * @uri: a pointer to snapshot uri information.
 * @len: length of snapshot uri information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_snapshot_uri(const char *profile, char *uri, int len)
{
#ifndef __MY_TEST__
	unsigned long ip_addr;
	char key[256], flag[256], def_flag[256];
#endif /* __MY_TEST__ */
	if (!profile || !uri || (len < MAX_URI_LEN))
		return -1;
#ifndef __MY_TEST__
	net_dev_get_ip_addr("eth0", &ip_addr);

	if (!strcmp(profile, "MajorStream")) {
		snprintf(key, sizeof(key), "%s0", DBCONF_IPC_SNAPSHOT_URI);
		snprintf(def_flag, sizeof(def_flag), ":554/%s", "MajorStream");
		db_config_get_string(DBCONF_IPC_DEVICE, key,
				flag, sizeof(key), def_flag);

	} else {
		snprintf(key, sizeof(key), "%s1", DBCONF_IPC_SNAPSHOT_URI);
		snprintf(def_flag, sizeof(def_flag), ":554/%s", "MinorStream");
		db_config_get_string(DBCONF_IPC_DEVICE, key,
				flag, sizeof(key), def_flag);
	}
	sprintf(uri, "rtsp://%d.%d.%d.%d%s",
			(unsigned int)(ip_addr & 0xff),
			(unsigned int)((ip_addr >> 8) & 0xff),
			(unsigned int)((ip_addr >> 16) & 0xff),
			(unsigned int)((ip_addr >> 24) & 0xff),
			flag);
#else
#if 0
	if (!strcmp(profile, "MajorStream"))
		snprintf(uri, len, "rtsp://%s/%s",
			"192.168.39.132",
			"MajorStream");
	else
		snprintf(uri, len, "rtsp://%s/%s",
			"192.168.39.132",
			"MinorStream");
#else
	snprintf(uri, len, "rtsp://%s/live", onvif_get_ip_addr());
#endif
#endif /* __MY_TEST__ */
	return 0;
}

/**
 * onvif_get_net_param: get net param
 * @is_wifi: if wifi.
 * @net_param: a pointer to net parameter information.
 * @len: length of net parameter information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_net_param(int is_wifi, net_param_t *net_param, int len)
{
#ifndef __MY_TEST__
	ip_static_config_t ip_config;
#endif /* __MY_TEST__ */
	if ((NULL == net_param) || len < sizeof(net_param_t))
		return -1;
#ifndef __MY_TEST__
	if (is_wifi)
		net_dev_get_static_ip("wlan0", &ip_config);
	else
		net_dev_get_static_ip("eth0", &ip_config);

	if (!strlen(s_net_param.ip_addr))
		net_param->enable_dhcp = 1;
	else
		net_param->enable_dhcp = 0;
	sprintf(net_param->ip_addr, "%d.%d.%d.%d",
			ip_config.ip_addr[0], ip_config.ip_addr[1],
			ip_config.ip_addr[2], ip_config.ip_addr[3]);
	sprintf(net_param->mask, "%d.%d.%d.%d",
			ip_config.netmask[0], ip_config.netmask[1],
			ip_config.netmask[2], ip_config.netmask[3]);
	sprintf(net_param->gateway, "%d.%d.%d.%d",
			ip_config.gateway[0], ip_config.gateway[1],
			ip_config.gateway[2], ip_config.gateway[3]);
	sprintf(net_param->dns1, "%d.%d.%d.%d",
			ip_config.dns0[0], ip_config.dns0[1],
			ip_config.dns0[2], ip_config.dns0[3]);
	sprintf(net_param->dns2, "%d.%d.%d.%d",
			ip_config.dns1[0], ip_config.dns1[1],
			ip_config.dns1[2], ip_config.dns1[3]);
#else
	net_param->enable_dhcp = 1;
	sprintf(net_param->ip_addr, "%s", "192.168.39.249");
	sprintf(net_param->mask, "%s", "255.255.255.0");
	sprintf(net_param->gateway, "%s", "192.168.39.254");
	sprintf(net_param->dns1, "%s", "192.168.16.1");
	sprintf(net_param->dns2, "%s", "192.168.16.2");
#endif /* __MY_TEST__ */
	return 0;
}

/**
 * onvif_set_net_param: set net param
 * @is_wifi: if wifi.
 * @net_param: net parameter information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_net_param(int is_wifi, net_param_t *net_param)
{
#ifndef __MY_TEST__
	int ipaddr[4], netmask[4], gateway[4];
	int i, dns0[4], dns1[4];
	ip_static_config_t ip_config;
#endif /* __MY_TEST__ */
	if ((NULL == net_param))
		return -1;

#ifndef __MY_TEST__
	if (net_param->enable_dhcp) {
		;
	} else {
		if (0 == is_wifi) {
			db_config_set_string(DBCONF_DEVICENETPARAM,
				DBCONF_DEVICENETPARAM_ETHIP,
				net_param->ip_addr);
			db_config_set_string(DBCONF_DEVICENETPARAM,
				DBCONF_DEVICENETPARAM_ETHMASK,
				net_param->mask);
			db_config_set_string(DBCONF_DEVICENETPARAM,
				DBCONF_DEVICENETPARAM_ETHGATEWAY,
				net_param->gateway);
			db_config_set_string(DBCONF_DEVICENETPARAM,
				DBCONF_DEVICENETPARAM_ETHDNS,
				net_param->dns1);
		}
		s_net_param.enable_dhcp = net_param->enable_dhcp;
		strcpy(s_net_param.ip_addr, net_param->ip_addr);
		sscanf(net_param->ip_addr, "%d.%d.%d.%d",
				&ipaddr[0], &ipaddr[1],
				&ipaddr[2], &ipaddr[3]);
		sscanf(net_param->mask, "%d.%d.%d.%d",
				&netmask[0], &netmask[1],
				&netmask[2], &netmask[3]);
		sscanf(net_param->gateway, "%d.%d.%d.%d",
				&gateway[0], &gateway[1],
				&gateway[2], &gateway[3]);
		sscanf(net_param->dns1, "%d.%d.%d.%d",
				&dns0[0], &dns0[1],
				&dns0[2], &dns0[3]);
		sscanf(net_param->dns2, "%d.%d.%d.%d",
				&dns1[0], &dns1[1],
				&dns1[2], &dns1[3]);
		for (i = 0; i < 4; i++) {
			ip_config.ip_addr[i] = (unsigned char) ipaddr[i];
			ip_config.netmask[i] = (unsigned char) netmask[i];
			ip_config.gateway[i] = (unsigned char) gateway[i];
			ip_config.dns0[i] = (unsigned char) dns0[i];
			ip_config.dns1[i] = (unsigned char) dns1[i];
		}

		if (is_wifi)
			net_dev_set_static_ip("wlan0", &ip_config);
		else
			net_dev_set_static_ip("eth0", &ip_config);
	}
#else

#endif /* __MY_TEST__ */
	return 0;
}

/**
 * onvif_get_hostname: get host name
 * @hostname: a pointer to host name information.
 * @len: length of host name information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_hostname(char *hostname, int len)
{
	if (!hostname || (len < MAX_NAME_LEN))
		return -1;
#ifdef __MY_TEST__
	if (0 == strlen(s_hostname))
		strcpy(s_hostname, "AI-IPC");

	snprintf(hostname, len, "%s", s_hostname);

	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_HOST_NAME,
			hostname, len,
			DBCONF_IPC_HOST_NAME_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_set_hostname: set host name
 * @hostname: host name information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_hostname(char *hostname)
{
	if (!hostname)
		return -1;
#ifdef __MY_TEST__
	snprintf(s_hostname, sizeof(s_hostname), "%s", hostname);

	return 0;
#else
	return db_config_set_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_HOST_NAME,
			hostname);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_ntp_server: get ntp server
 * @ntp_server: a pointer to ntp server information.
 * @len: length of ntp server information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_ntp_server(char *ntp_server, int len)
{
	if (!ntp_server || (len < MAX_NAME_LEN))
		return -1;
#ifdef __MY_TEST__
	if (0 == strlen(s_ntp_server))
		strcpy(s_ntp_server, "192.168.43.1");

	snprintf(ntp_server, len, "%s", s_ntp_server);

	return 0;
#else
	return db_config_get_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_NTP_SERVER,
			ntp_server, len,
			DBCONF_IPC_NTP_SERVER_DEF_VALUE);
#endif /* __MY_TEST__ */
}

/**
 * onvif_set_ntp_server: set ntp server
 * @ntp_server: ntp server information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_ntp_server(char *ntp_server)
{
	if (!ntp_server)
		return -1;
#ifdef __MY_TEST__
	snprintf(s_ntp_server, sizeof(s_ntp_server), "%s", ntp_server);

	return 0;
#else
	return db_config_set_string(DBCONF_IPC_DEVICE,
			DBCONF_IPC_NTP_SERVER,
			ntp_server);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_onvif_port: get onvif port
 * @port: a pointer to onvif port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_onvif_port(int *port)
{
	if (!port)
		return -1;
#ifndef __MY_TEST__
	*port = db_config_get_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_ONVIF_PORT,
			DBCONF_IPC_ONVIF_PORT_DEF_VALUE);
#else
	*port = DBCONF_IPC_ONVIF_PORT_DEF_VALUE;
#endif /* __MY_TEST__ */
	return 0;
}

/**
 * onvif_set_onvif_port: set onvif port
 * @port: onvif port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_onvif_port(int port)
{
#ifndef __MY_TEST__
	return db_config_set_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_ONVIF_PORT,
			port);
#else
	return 0;
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_http_port: get http port
 * @port: a pointer to http port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_http_port(int *port)
{
	if (!port)
		return -1;
#ifdef __MY_TEST__
	if (0 == s_http_port)
		s_http_port = DBCONF_IPC_HTTP_PORT_DEF_VALUE;

	*port = s_http_port;

	return 0;
#else
	*port = db_config_get_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_HTTP_PORT,
			DBCONF_IPC_HTTP_PORT_DEF_VALUE);

	return 0;
#endif /* __MY_TEST__ */
}

/**
 * onvif_set_http_port: set http port
 * @port: http port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_http_port(int port)
{
#ifdef __MY_TEST__
	s_http_port = port;

	return 0;
#else
	return db_config_set_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_HTTP_PORT,
			port);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_rtsp_port: get rtsp port
 * @port: a pointer to rtsp port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_rtsp_port(int *port)
{
	if (!port)
		return -1;
#ifdef __MY_TEST__
	if (0 == s_rtsp_port)
		s_rtsp_port = 554;

	*port = s_rtsp_port;

	return 0;
#else
	*port = db_config_get_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_RTSP_PORT,
			DBCONF_IPC_RTSP_PORT_DEF_VALUE);

	return 0;
#endif /* __MY_TEST__ */
}

/**
 * onvif_set_rtsp_port: set rtsp port
 * @port: rtsp port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_rtsp_port(int port)
{
#ifdef __MY_TEST__
	s_rtsp_port = port;

	return 0;
#else
	return db_config_set_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_RTSP_PORT,
			port);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_user_info: get user information
 * @index: user serial no.
 * @user: a pointer to user information.
 * @len: length of user information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_user_info(int index, user_info_t *user, int len)
{
#ifndef __MY_TEST__
	char key[256];
#endif /* __MY_TEST__ */
	if (index < 0 || index >= MAX_USER_NUM)
		return -1;

	if (!user || (len < sizeof(user_info_t)))
		return -2;
#ifndef __MY_TEST__
	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_USER_NAME, index);
	db_config_get_string(DBCONF_IPC_DEVICE,
			key,
			s_user_list[index].username,
			sizeof(s_user_list[index].username),
			DBCONF_IPC_USER_NAME_DEF_VALUE);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_PASSWORD, index);
	db_config_get_string(DBCONF_IPC_DEVICE,
			key,
			s_user_list[index].password,
			sizeof(s_user_list[index].password),
			DBCONF_IPC_PASSWORD_DEF_VALUE);

	snprintf(key, sizeof(key), "%s%d", DBCONF_IPC_USER_LEVEL, index);
	s_user_list[index].level = db_config_get_int(DBCONF_IPC_DEVICE,
			key,
			DBCONF_IPC_USER_LEVEL_DEF_VALUE);
#endif /* __MY_TEST__ */
	memcpy(user, &s_user_list[index], sizeof(user_info_t));

	return 0;
}

/**
 * onvif_set_user_info: set user information
 * @username: user name.
 * @user: user information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_user_info(const char *username, user_info_t *user)
{
#ifndef __MY_TEST__
	char key[256];
#endif /* __MY_TEST__ */
	int i, ret = -1;
	if (!username || !user)
		return -1;

	for (i = 0; i < MAX_USER_NUM; i++) {
		if ((strlen(s_user_list[i].username) > 0)
				&& (0 == strcmp(
				s_user_list[i].username,
				username))) {
			memcpy(&s_user_list[i],
				user, sizeof(user_info_t));
#ifndef __MY_TEST__
			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_USER_NAME, i);
			ret = db_config_set_string(DBCONF_IPC_DEVICE,
					key,
					s_user_list[i].username);

			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_PASSWORD, i);
			ret += db_config_set_string(DBCONF_IPC_DEVICE,
					key,
					s_user_list[i].password);

			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_USER_LEVEL, i);
			ret += db_config_get_int(DBCONF_IPC_DEVICE,
					key,
					s_user_list[i].level);
#endif /* __MY_TEST__ */
			break;
		}
	}

	return ret;
}

/**
 * onvif_delete_user_info: delete user information
 * @username: user name.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_delete_user_info(const char *username)
{
	int i, ret = 0;
#ifndef __MY_TEST__
	char key[256];
#endif /* __MY_TEST__ */
	if (!username)
		return -1;

	for (i = 0; i < MAX_USER_NUM; i++) {
		if ((strlen(s_user_list[i].username) > 0)
				&& (0 == strcmp(
				s_user_list[i].username,
				username))) {
			memset(&s_user_list[i], 0, sizeof(user_info_t));
#ifndef __MY_TEST__
			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_USER_NAME, i);
			ret = db_config_set_string(DBCONF_IPC_DEVICE, key, "");

			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_PASSWORD, i);
			ret += db_config_set_string(DBCONF_IPC_DEVICE, key, "");

			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_USER_LEVEL, i);
			ret += db_config_get_int(DBCONF_IPC_DEVICE, key, 2);
#endif /* __MY_TEST__ */
			break;
		}
	}

	return ret;
}

/**
 * onvif_add_user_info: add user information
 * @user: user information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_add_user_info(user_info_t *user)
{
#ifndef __MY_TEST__
	char key[256];
#endif /* __MY_TEST__ */
	int i, ret = -1;

	if (!user)
		return -1;

	for (i = 0; i < MAX_USER_NUM; i++) {
		if (0 == strlen(s_user_list[i].username)) {
			memcpy(&s_user_list[i],
				user, sizeof(user_info_t));
#ifndef __MY_TEST__
			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_USER_NAME, i);
			ret = db_config_set_string(DBCONF_IPC_DEVICE,
					key,
					s_user_list[i].username);

			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_PASSWORD, i);
			ret += db_config_set_string(DBCONF_IPC_DEVICE,
					key,
					s_user_list[i].password);

			snprintf(key, sizeof(key), "%s%d",
					DBCONF_IPC_USER_LEVEL, i);
			ret += db_config_get_int(DBCONF_IPC_DEVICE,
					key,
					s_user_list[i].level);
#endif /* __MY_TEST__ */
			break;
		}
	}

	return ret;
}

/**
 * convert_time_str_to_sec - Convert time string to seconds.
 * @str_time A pointer to char time
 * @sec Out, time seconds
 *
 * Returns 0 if valid, otherwise not 0.
 */
int convert_time_str_to_sec(const char *str_time, double *out_sec)
{
	int ret = 0;
	time_t t = 0;
	struct tm tmp_time;

	if (str_time == NULL || out_sec == NULL)
		return -1;

	/* Note: must be initialized, otherwise tm_isdst will be an arbitrary value */
	memset(&tmp_time, 0, sizeof(struct tm));
	strptime(str_time, "%Y/%m/%d %H:%M:%S", &tmp_time);
	t = mktime(&tmp_time);
	if (t != 0) {
		*out_sec = (long)(t);
		ret = 0;
	} else
		ret = -1;

	return ret;
}

/**
 * onvif_get_timezone_index: get time zone index
 * @indx: a pointer to time zone index information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_timezone_index(int *indx)
{
	if (!indx)
		return -1;
#ifdef __MY_TEST__
	if (0 == s_tz_index)
		s_tz_index = 100;

	*indx = s_tz_index;

	return 0;
#else
	*indx = db_config_get_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_TIMEZONE_INDEX,
			DBCONF_IPC_TIMEZONE_INDEX_DEF_VALUE);

	return 0;
#endif /* __MY_TEST__ */
}

/**
 * onvif_set_timezone_index: set time zone index
 * @indx: timezone index information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_timezone_index(int indx)
{
#ifdef __MY_TEST__
	s_tz_index = indx;

	return 0;
#else
	return db_config_set_int(DBCONF_IPC_DEVICE,
			DBCONF_IPC_TIMEZONE_INDEX,
			indx);
#endif /* __MY_TEST__ */
}

/**
 * onvif_get_dev_guid: get device guid
 * @guid: a pointer to guid information.
 * @len: length of guid information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_dev_guid(char *guid, int len)
{
	char macaddr[6];
	if (!guid || (len < MAX_URI_LEN))
		return -1;

	onvif_get_mac_addr(macaddr, 6);

	snprintf(guid, len,
		"urn:uuid:20200108-15-16-30-%02X%02X%02X%02X%02X%02X",
		macaddr[0], macaddr[1], macaddr[2],
		macaddr[3], macaddr[4], macaddr[5]);
	return 0;
}

