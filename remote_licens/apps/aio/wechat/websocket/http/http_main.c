/*
 * http_main.c - define interface function of http server main.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <iniparser.h>
#include "http_main.h"
#include "ws_http.h"
#include "web_biz_api.h"
#include <sys/prctl.h>

#define LOG_TAG "http_main"
#include <log/log.h>

#define WEB_SERVER_PORT "80"
#define WEB_SERVER_DOC "/usr/share/aio/webpage"
#define WEB_SERVER_INI "/usr/share/aio/http.ini"


static web_thread_t web_register;

static void *web_thread_proc(void *arg)
{
	ALOGD("In web main thread Runnig.");
	prctl(PR_SET_NAME, "web_server_thread");

	web_register.isRunnig = 1;

	while(web_register.isRunnig) {
		ws_http_poll();
	}
	ws_http_close();
	ALOGI("exit web main thread.");
	return NULL;
}

/**
 * http_send_checkin - send checkin data to remote.
 *
 */
void http_send_checkin(const char *url)
{
	client_send_checkin(url);
}

/**
 * http_set_web_handle_cb - set http webserver callback.
 *
 */
void http_set_web_handle_cb(web_handler_cb cb)
{
	web_handle_cb(cb);
}

/** 
 * http_stop_web_server - stop webserver for register user.
 *
 * Returns 0 success,other failed.
 */
int http_stop_web_server(void)
{
	if(web_register.isRunnig) {
		ALOGI("wait web thread quit.");
		web_register.isRunnig = 0;
		pthread_join(web_register.tid, NULL);
	}
	ws_http_close();
	return 0;
}

/** 
 * http_start_web_server - start webserver for register user.
 *
 * Returns 0 success,other failed.
 */
int http_start_web_server(void)
{
	ALOGD("%s: init web register", __func__);
	int err = 0;
	const char *port, *doc;
	dictionary *ini_server;

	http_stop_web_server();
	ini_server = iniparser_load(WEB_SERVER_INI);
	if (!ini_server) {
		ALOGE("failed to load app config file:%s", WEB_SERVER_INI);
		return -1;
	}
	port = iniparser_getstring(ini_server, "listening_ports", WEB_SERVER_PORT);
	doc = iniparser_getstring(ini_server, "document_root", WEB_SERVER_DOC);

	err = ws_http_init(port, doc);
	if (err != 0) {
		ALOGE("http web server init failed %d!\n", err);
		return err;
	}

	/**** add http server action point ****/
	err = ws_http_register_endpoint("/deviceLogin", WS_EP_URL, lb_web_login);
	err |= ws_http_register_endpoint("/heartBeat", WS_EP_URL, lb_web_set_dev_hearbeat);
	err |= ws_http_register_endpoint("/getDeviceParameter", WS_EP_URL, lb_web_get_dev_params);
	err |= ws_http_register_endpoint("/setDeviceParameter", WS_EP_URL, lb_web_set_dev_params);
	err |= ws_http_register_endpoint("/restoreDeviceDefaultParameter", WS_EP_URL, lb_web_restore_dev_params);
	err |= ws_http_register_endpoint("/getDeviceTime", WS_EP_URL, lb_web_get_dev_time);
	err |= ws_http_register_endpoint("/setDeviceTime", WS_EP_URL, lb_web_set_dev_time);
	err |= ws_http_register_endpoint("/uploadUpgradeAPKFile", WS_EP_URL, lb_web_upload_upgrade_file);
	err |= ws_http_register_endpoint("/setDeviceReboot", WS_EP_URL, lb_web_set_dev_reboot);
	err |= ws_http_register_endpoint("/setDeviceRemoteOpen", WS_EP_URL, lb_web_set_dev_remoteopnen);
	err |= ws_http_register_endpoint("/getDeviceVersion", WS_EP_URL, lb_web_get_dev_version);
	err |= ws_http_register_endpoint("/getDeviceSnapPicture", WS_EP_URL, lb_web_get_snap_picture);
	err |= ws_http_register_endpoint("/setDeviceLogo", WS_EP_URL, lb_web_set_dev_logo);
	err |= ws_http_register_endpoint("/restoreDeviceLogo", WS_EP_URL, lb_web_restore_dev_logo);
	err |= ws_http_register_endpoint("/setDeviceNetwork", WS_EP_URL, lb_web_set_dev_network);
	err |= ws_http_register_endpoint("/getDeviceSnapFace", WS_EP_URL, lb_web_get_snap_face);
	err |= ws_http_register_endpoint("/deleteAllDeviceAdvertPicture", WS_EP_URL, lb_web_del_advert_pngs);
	err |= ws_http_register_endpoint("/addDeviceAdvertPicture", WS_EP_URL, lb_web_add_advert_png);
	err |= ws_http_register_endpoint("/getDeviceLog", WS_EP_URL, lb_web_get_dev_log);
	err |= ws_http_register_endpoint("/deleteDeviceLog", WS_EP_URL, lb_web_del_dev_log);
	err |= ws_http_register_endpoint("/addDeviceWhiteList", WS_EP_URL, lb_web_add_white_list);
	err |= ws_http_register_endpoint("/deleteDeviceWhiteList", WS_EP_URL, lb_web_del_white_list);
	err |= ws_http_register_endpoint("/deleteDeviceAllWhiteList", WS_EP_URL, lb_web_remove_white_lists);
	err |= ws_http_register_endpoint("/getAllDeviceIdWhiteList", WS_EP_URL, lb_web_get_white_list);
	err |= ws_http_register_endpoint("/getDeviceWhiteListDetailByIdNum", WS_EP_URL, lb_web_get_white_member);
	err |= ws_http_register_endpoint("/setIdentifyCallBck", WS_EP_URL, lb_web_set_identify_callback);
	err |= ws_http_register_endpoint("/setDeviceTimeAccessGroups", WS_EP_URL, lb_web_set_access_groups);
	err |= ws_http_register_endpoint("/setDeviceTimeAccessSpecialTimeGroups", WS_EP_URL, lb_web_set_special_groups);
	err |= ws_http_register_endpoint("/IpSetting", WS_EP_URL, lb_web_ip_setting);
	err |= ws_http_register_endpoint("/setWifiState", WS_EP_URL, lb_web_set_wifi_state);
	err |= ws_http_register_endpoint("/getScanWifiResults", WS_EP_URL, lb_web_get_scan_wifi_results);
	err |= ws_http_register_endpoint("/connectWifi", WS_EP_URL, lb_web_connect_wifi);
	err |= ws_http_register_endpoint("/factoryDefault", WS_EP_URL, lb_web_factory_default);
	err |= ws_http_register_endpoint("/algSetting", WS_EP_URL, lb_web_alg_setting);
	err |= ws_http_register_endpoint("/webUpgrade", WS_EP_URL, lb_web_upgrade);
	err |= ws_http_register_endpoint("/getWifiState", WS_EP_URL, lb_web_get_wifi_state);

	


	err |= ws_http_register_endpoint("/recvTest", WS_EP_URL, lb_web_recv_test);
	if (err != 0) {
		ALOGE("Onload Handler failed %d!\n", err);
		goto err_exit;
	}

	err = pthread_create(&web_register.tid, NULL, web_thread_proc, NULL);
	if(0 != err) {
		web_register.isRunnig = 0;
		ALOGE("create web register thread failed!");
		goto err_exit;
	}
	pthread_detach(web_register.tid);
	if(ini_server)
		iniparser_freedict(ini_server);

	return err;

err_exit:

	http_stop_web_server();
	if(ini_server)
		iniparser_freedict(ini_server);
	return err;

}

