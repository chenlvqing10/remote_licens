/*
 * onvif_param.h - Interface to parameters required by onvif
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

#ifndef __ONVIF_PARAM_H__
#define __ONVIF_PARAM_H__

#define MAX_NAME_LEN		64
#define MAX_NAME_SIZE		64
#define MAX_URI_LEN		256
#define MAX_ENCODER_NUM		2
#define MAX_IP_ADDR_LEN		20
#define MAX_USER_NUM		8

/*
#define __MY_TEST__
*/

typedef struct _st_ptz_config
{
	unsigned char	pan_tilt_x;
	unsigned char	pan_tilt_y;
	unsigned short	default_ptz_time_out;

	unsigned char	pan_tilt_limits_x_min;
	unsigned char	pan_tilt_limits_x_max;
	unsigned char	pan_tilt_limits_y_min;
	unsigned char	pan_tilt_limits_y_max;

	unsigned char	zoom_limits_min;
	unsigned char	zoom_limits_max;
	unsigned short	move_ramp;

	unsigned short	preset_ramp;
	unsigned short	preset_tour_ramp;

	unsigned char	zoom_x;
	unsigned char	reserved[3];

} ptz_config_t, *pptz_config_t;

typedef struct _st_preset
{
	char	name[MAX_NAME_SIZE];
	char	token[MAX_NAME_SIZE];

	unsigned char	pan_tilt_x;
	unsigned char	pan_tilt_y;
	unsigned char	zoom_x;
	unsigned char	reserved;

} preset_t, *ppreset_t;

/* voide source information */
typedef struct tag_video_source {
	char	token[MAX_NAME_LEN]; /* token */
	int	frame_rate; /* frame rate */
	int	width; /* width of resolution */
	int	height; /* height of resolution */
} video_source_t;

/* voide encoder information */
typedef struct tag_video_encoder {
	char	token[MAX_NAME_LEN]; /* token */
	int	encode_type; /* encode type:
				JPEG = 0,
				MPEG4 = 1,
				H264 = 2
				*/
	int	frame_rate; /* frame rate */
	int	bit_rate; /* bit rate */
	int	encode_interval; /* encode interval */
	int	quality; /* quality */
	int	width; /* width of resolution */
	int	height; /* height of resolution */
	int	gov_length; /* GOVLength*/
	int	profile; /* profile:
				H264 profile:
					Baseline = 0,
					Main = 1,
					Extended = 2,
					High = 3
				*/
} video_encoder_t;

/* net parameter information */
typedef struct tag_net_param {
	int	enable_dhcp; /* dhcp status: 1/0: enable/disable */
	char	ip_addr[MAX_IP_ADDR_LEN]; /* ip address */
	char	mask[MAX_IP_ADDR_LEN]; /* submask */
	char	gateway[MAX_IP_ADDR_LEN]; /* gateway */
	char	dns1[MAX_IP_ADDR_LEN]; /* dns1 server */
	char	dns2[MAX_IP_ADDR_LEN]; /* dns2 server */
} net_param_t;

/* user information */
typedef struct tag_user_info {
	char	username[MAX_NAME_LEN]; /* username */
	char	password[MAX_NAME_LEN]; /* password */
	int	level; /*level:
			0: tt__UserLevel__Administrator,
			1: tt__UserLevel__Operator,
			2: tt__UserLevel__User,
			3: tt__UserLevel__Anonymous,
			4: tt__UserLevel__Extended
			*/
} user_info_t;

/**
 * onvif_get_dev_name: get device name
 * @dev_name: a pointer to device name.
 * @len: length of device name buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_dev_name(char *dev_name, int len);

/**
 * onvif_set_dev_name: set device name
 * @dev_name: device name.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_dev_name(const char *dev_name);

/**
 * onvif_get_location: get location
 * @location: a pointer to location information.
 * @len: length of location information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_location(char *location, int len);

/**
 * onvif_set_location: set location
 * @location: location information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_location(const char *location);

/**
 * onvif_get_manufacturer: get manufacturer
 * @mfacture: a pointer to manufacturer information.
 * @len: length of manufacturer information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_manufacturer(char *mfacture, int len);

/**
 * onvif_get_device_model: get deivce model
 * @model: a pointer to model.
 * @len: length of model buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_device_model(char *model, int len);

/**
 * onvif_get_device_version: get deivce version
 * @version: a pointer to version information.
 * @len: length of version infromation buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_device_version(char *version, int len);

/**
 * onvif_get_serial_no: get serial no
 * @serial_no: a pointer to serial no.
 * @len: length of serial no buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_serial_no(char *serial_no, int len);

/**
 * onvif_get_hardware_id: get hardware id
 * @hardware_id: a pointer to hardware id.
 * @len: length of hardware id buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_hardware_id(char *hardware_id, int len);

/**
 * onvif_get_video_source_info: get video source info
 * @vsource: a pointer to video source information.
 * @len: length of video source information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_video_source_info(video_source_t *vsource, int len);

/**
 * onvif_get_video_encoder_info: get video encoder info
 * @index: encoder no.
 * @vencoder: a pointer to video encoder information.
 * @len: length of video encoder information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_video_encoder_info(int index,
		video_encoder_t *vencoder, int len);

/**
 * onvif_get_media_profile: get media profile
 * @index: encoder no.
 * @profile: a pointer to media profile information.
 * @len: length of media profile information buffer.

 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_media_profile(int index, char *profile, int len);

/**
 * onvif_get_media_uri: get media uri
 * @profile: media profile.
 * @uri: a pointer to media uri information.
 * @len: length of media uri information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_media_uri(const char *profile, char *uri, int len);

/**
 * onvif_get_snapshot_uri: get snapshot uri
 * @profile: snapshot profile.
 * @uri: a pointer to snapshot uri information.
 * @len: length of snapshot uri information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_snapshot_uri(const char *profile, char *uri, int len);

/**
 * onvif_get_net_param: get net param
 * @is_wifi: if wifi.
 * @net_param: a pointer to net parameter information.
 * @len: length of net parameter information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_net_param(int is_wifi, net_param_t *net_param, int len);

/**
 * onvif_set_net_param: set net param
 * @is_wifi: if wifi.
 * @net_param: net parameter information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_net_param(int is_wifi, net_param_t *net_param);

/**
 * onvif_get_hostname: get host name
 * @hostname: a pointer to host name information.
 * @len: length of host name information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_hostname(char *hostname, int len);

/**
 * onvif_set_hostname: set host name
 * @hostname: host name information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_hostname(char *hostname);

/**
 * onvif_get_ntp_server: get ntp server
 * @ntp_server: a pointer to ntp server information.
 * @len: length of ntp server information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_ntp_server(char *ntp_server, int len);

/**
 * onvif_set_ntp_server: set ntp server
 * @ntp_server: ntp server information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_ntp_server(char *ntp_server);

/**
 * onvif_get_onvif_port: get onvif port
 * @port: a pointer to onvif port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_onvif_port(int *port);

/**
 * onvif_set_onvif_port: set onvif port
 * @port: onvif port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_onvif_port(int port);

/**
 * onvif_get_http_port: get http port
 * @port: a pointer to http port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_http_port(int *port);

/**
 * onvif_set_http_port: set http port
 * @port: http port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_http_port(int port);

/**
 * onvif_get_rtsp_port: get rtsp port
 * @port: a pointer to rtsp port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_rtsp_port(int *port);

/**
 * onvif_set_rtsp_port: set rtsp port
 * @port: rtsp port information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_rtsp_port(int port);

/**
 * onvif_get_user_info: get user information
 * @index: user serial no.
 * @user: a pointer to user information.
 * @len: length of user information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_user_info(int index, user_info_t *user, int len);

/**
 * onvif_set_user_info: set user information
 * @username: user name.
 * @user: user information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_user_info(const char *username, user_info_t *user);

/**
 * onvif_delete_user_info: delete user information
 * @username: user name.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_delete_user_info(const char *username);

/**
 * onvif_add_user_info: add user information
 * @user: user information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_add_user_info(user_info_t *user);

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
 * convert_time_str_to_sec - Convert time string to seconds.
 * @str_time A pointer to char time
 * @sec Out, time seconds
 *
 * Returns 0 if valid, otherwise not 0.
 */
int convert_time_str_to_sec(const char *str_time, double *out_sec);

/**
 * onvif_get_timezone_index: get time zone index
 * @indx: a pointer to time zone index information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_timezone_index(int *indx);

/**
 * onvif_set_timezone_index: set time zone index
 * @indx: timezone index information.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_set_timezone_index(int indx);

/**
 * onvif_get_dev_guid: get device guid
 * @guid: a pointer to guid information.
 * @len: length of guid information buffer.
 *
 * return 0 if success; otherwise failed.
 */
int onvif_get_dev_guid(char *guid, int len);

int init_ptz();

#endif /* __ONVIF_PARAM_H__ */

