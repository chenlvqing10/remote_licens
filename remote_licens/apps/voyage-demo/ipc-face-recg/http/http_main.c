/*
 * main.c - main.
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

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "restful_api.h"
#include "ws_http.h"
#include "parse.h"
#include "web_biz_api.h"
#include "web_sys_cfg.h"
#include "ws_http.h"

#include "cloud_biz_api.h"

#define LOG_TAG		"http_main"
#include <log/log.h>

#ifdef WS_GOAHEAD
#include "goahead.h"
#endif
#ifdef WS_MG
#include "mongoose.h"
#endif

static int create_http_task(void);
void *http_main();

/**
 * http_main_start: start http service
 * @
 *
 * Returns 0 if success, otherwise not 0.
 */
int http_main_start()
{
	pthread_t p;

	pthread_create(&p, NULL, http_main, NULL);
	pthread_detach(p);

	return 0;
}

/**
 * http_main: register RESTful API and start http service
 * @
 *
 * Returns 0 if success, otherwise not 0.
 */
void *http_main()
{
	int ret = 0;
	int err = 0;
	char port[32];
	char doc_root[64];
	memset(port, 0, sizeof(port));
	memset(doc_root, 0, sizeof(doc_root));
	ret = get_parse(port, doc_root);
	if (ret<0) {
		strcpy(port, "80");
		strcpy(doc_root, "webpage");
	}
	ALOGD("port = %s : doc_root = %s\n", port, doc_root);
	err = ws_http_init(port, doc_root);
	ALOGD("line = %d : err = %d\n", __LINE__, err);
	if (err != 0)
		printf("http web server init failed %d!\n", err);
#ifdef WS_GOAHEAD
	err = ws_http_register_endpoint("get_version_info", WS_EP_ASP, get_version_info);
	if (err != 0)
		printf("ASP action failed %d!\n",err);

	err = ws_http_register_endpoint("formIPSetting", WS_EP_FORM, form_ip_setting);
	if (err != 0)
		printf("Form action failed %d!\n", err);

	err = ws_http_register_endpoint("/brightsel/", WS_EP_URL, brightness_on_change);
	if (err != 0)
		printf("Url Handler /brightsel/ failed %d!\n", err);

	err = ws_http_register_endpoint("/action/login", WS_EP_URL, lb_web_login);
	err |= ws_http_register_endpoint("/action/recgcfg", WS_EP_URL, lb_web_recg_cfg);
	err |= ws_http_register_endpoint("/action/addmember", WS_EP_URL,
		lb_web_add_member);
	err |= ws_http_register_endpoint("/picvalidate", WS_EP_URL, lb_web_pic_validate);
	err |= ws_http_register_endpoint("/action/searchmember", WS_EP_URL,
		lb_web_search_member);
	err |= ws_http_register_endpoint("/action/deletemember", WS_EP_URL,
		lb_web_delete_member);
	err |= ws_http_register_endpoint("/action/searchrec", WS_EP_URL,
		lb_web_search_rec);
	err |= ws_http_register_endpoint("/action/exportrec", WS_EP_URL,
		lb_web_export_rec);
	err |= ws_http_register_endpoint("/action/lancfg", WS_EP_URL,
		lb_web_lan_cfg);

	err |= ws_http_register_endpoint("/action/wificfg", WS_EP_URL, lb_web_wifi_cfg);
	err |= ws_http_register_endpoint("/action/wifihot", WS_EP_URL,
		lb_web_get_wifi_hot);
	err |= ws_http_register_endpoint("/action/factorydefault", WS_EP_URL,
		lb_web_factory_default);
	err |= ws_http_register_endpoint("/action/fwupg", WS_EP_URL, lb_web_fw_upg);
	err |= ws_http_register_endpoint("/action/sysinfo", WS_EP_URL, lb_web_sys_info);

	err |= ws_http_register_endpoint("/action/adminpasswd", WS_EP_URL,
		lb_web_admin_passwd);
	err |= ws_http_register_endpoint("/action/systimesetting", WS_EP_URL,
		lb_web_systime_set);
	if (err != 0)
		printf("Onload Handler failed %d!\n", err);

	err |= ws_http_register_endpoint("/api/v1/sum", WS_EP_URL, restful_api_cal_sum);
	if (err != 0)
		printf("Onload Handler failed %d!\n", err);
#endif
#ifdef WS_MG
	err = ws_http_register_endpoint("/action/formIPSetting", WS_EP_FORM,
		form_ip_setting);
	if (err != 0)
		printf("Form action failed %d!\n", err);

	err = ws_http_register_endpoint("/action/login", WS_EP_URL,
		lb_web_login);
	err |= ws_http_register_endpoint("/action/logout", WS_EP_URL,
		lb_web_logout);

	/** config */
	err |= ws_http_register_endpoint("/action/getrecgcfg", WS_EP_URL,
		lb_web_get_recg_cfg);
	err |= ws_http_register_endpoint("/action/setrecgcfg", WS_EP_URL,
		lb_web_set_recg_cfg);
	err |= ws_http_register_endpoint("/action/getdevicehwparam", WS_EP_URL,
		lb_web_get_device_hwparam);
	err |= ws_http_register_endpoint("/action/setdevicehwparam", WS_EP_URL,
		lb_web_set_device_hwparam);
	err |= ws_http_register_endpoint("/action/getvolume", WS_EP_URL,
		lb_web_get_volume);
	err |= ws_http_register_endpoint("/action/setvolume", WS_EP_URL,
		lb_web_set_volume);
	err |= ws_http_register_endpoint("/action/getscreeninfo", WS_EP_URL,
		lb_web_get_screeninfo);
	err |= ws_http_register_endpoint("/action/setscreeninfo", WS_EP_URL,
		lb_web_set_screeninfo);
	err |= ws_http_register_endpoint("/action/getlightinfo", WS_EP_URL,
		lb_web_get_light_info);
	err |= ws_http_register_endpoint("/action/setlightinfo", WS_EP_URL,
		lb_web_set_light_info);
	err |= ws_http_register_endpoint("/action/getcloudplatformparam", WS_EP_URL,
		lb_web_get_cloud_platform_param);
	err |= ws_http_register_endpoint("/action/setcloudplatformparam", WS_EP_URL,
		lb_web_set_cloud_platform_param);
	err |= ws_http_register_endpoint("/action/getdevtime", WS_EP_URL,
		lb_web_get_dev_time);
	err |= ws_http_register_endpoint("/action/setdevtime", WS_EP_URL,
		lb_web_set_dev_time);
	err |= ws_http_register_endpoint("/action/setdevelopsel", WS_EP_URL,
		lb_web_set_develop_select);
	err |= ws_http_register_endpoint("/action/getdevelopsel", WS_EP_URL,
		lb_web_get_develop_select);
	err |= ws_http_register_endpoint("/action/getdevicestatus", WS_EP_URL,
		lb_web_get_device_status);
	err |= ws_http_register_endpoint("/action/setdevicestatus", WS_EP_URL,
		lb_web_set_device_status);
	err |= ws_http_register_endpoint("/action/getethparam", WS_EP_URL,
		lb_web_get_ethparam);
	err |= ws_http_register_endpoint("/action/setethparam", WS_EP_URL,
		lb_web_set_ethparam);
	err |= ws_http_register_endpoint("/action/getwifiparam", WS_EP_URL,
		lb_web_get_wifi_param);
	err |= ws_http_register_endpoint("/action/setwifiparam", WS_EP_URL,
		lb_web_set_wifi_param);
	err |= ws_http_register_endpoint("/action/getsysbaseinfo", WS_EP_URL,
		lb_web_get_sys_baseinfo);
	err |= ws_http_register_endpoint("/action/sysinfo", WS_EP_URL,
		lb_web_sys_info);

	/** record */
	err |= ws_http_register_endpoint("/action/searchrec", WS_EP_URL,
		lb_web_search_rec);
	err |= ws_http_register_endpoint("/action/exportrec", WS_EP_URL,
		lb_web_export_rec);
	err |= ws_http_register_endpoint("/action/deleterec", WS_EP_URL,
		lb_web_delete_rec);

	/** face */
	err |= ws_http_register_endpoint("/action/addmember", WS_EP_URL,
		lb_web_add_member);
	err |= ws_http_register_endpoint("/action/searchmember", WS_EP_URL,
		lb_web_search_member);
	err |= ws_http_register_endpoint("/action/updatemember", WS_EP_URL,
		lb_web_update_member);
	err |= ws_http_register_endpoint("/action/totalmember", WS_EP_URL,
		lb_web_total_member);
	err |= ws_http_register_endpoint("/action/deletemember", WS_EP_URL,
		lb_web_delete_member);
	// err |= ws_http_register_endpoint("/action/setsysadmin", WS_EP_URL,
	// 	lb_web_set_sys_admin);
	err |= ws_http_register_endpoint("/action/addmanager", WS_EP_URL,
		lb_web_add_manager);
	err |= ws_http_register_endpoint("/action/searchmanager", WS_EP_URL,
		lb_web_search_manager);
	err |= ws_http_register_endpoint("/action/updatemanager", WS_EP_URL,
		lb_web_update_manager);
	// err |= ws_http_register_endpoint("/action/delmanager", WS_EP_URL,
	// 	lb_web_delete_manager);
	err |= ws_http_register_endpoint("/action/importmems", WS_EP_URL,
		lb_web_batch_import_members);

	/** system api */
	err |= ws_http_register_endpoint("/action/getquality", WS_EP_URL,
		lb_web_get_pic_quality);
	err |= ws_http_register_endpoint("/action/restartdevice", WS_EP_URL,
		lb_web_restart_device);
	err |= ws_http_register_endpoint("/action/snapshot", WS_EP_URL,
		lb_web_snapshot);
	err |= ws_http_register_endpoint("/action/getfeature", WS_EP_URL,
		lb_web_get_feature);
	err |= ws_http_register_endpoint("/action/scanwifilist", WS_EP_URL,
		lb_web_scan_wifi_list);
	err |= ws_http_register_endpoint("/action/resetdefault", WS_EP_URL,
		lb_web_factory_default);
	err |= ws_http_register_endpoint("/action/updatefirmware", WS_EP_URL,
		lb_web_fw_upg);
	err |= ws_http_register_endpoint("/action/getwifiname", WS_EP_URL,
		lb_web_getwifiname);
	err |= ws_http_register_endpoint("/action/connectwifi", WS_EP_URL,
		lb_web_connectwifi);
	err |= ws_http_register_endpoint("/action/scandevice", WS_EP_URL,
		lb_web_scan_device);
	err |= ws_http_register_endpoint("/action/getmacaddr", WS_EP_URL,
		lb_web_get_macaddr);

	err |= ws_http_register_endpoint("/action/addmemberbyfeature",
		WS_EP_URL, lb_web_add_member_by_feature);

	if (err != 0)
		printf("Onload Handler failed %d!\n", err);
#endif

	err = create_http_task();
	if (err != 0)
		printf("create http web server task failed %d!\n", err);


	cloud_upload_rec_thr_create();

	/*
	cloud_import_mems();
	*/

	return NULL;
}

/**
 * create_http_task: start http service thread
 *
 * Returns 0 if success, otherwise not 0.
 */
static int create_http_task(void)
{
	int		err = 0;
	pthread_t	tmp_task_id;

	err = pthread_create(&tmp_task_id, NULL, (void *)ws_http_proc, NULL);
	if (err != 0) {
		printf("\n [%s] create thread error: %d!\n", __func__, err);
		return -1;
	}
	printf("\n [%s] create thread OK: %d!\n", __func__, err);

	pthread_detach((pthread_t)tmp_task_id);

	return 0;
}

