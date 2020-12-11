/*
 * api/sys_cfg.c - all functions for system configurations
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

#ifndef _WEB_SYS_CFG_H_
#define _WEB_SYS_CFG_H_

#ifdef WS_GOAHEAD
void form_ip_setting(Webs *wp);
int brightness_on_change(Webs *wp, char *prefix, char *dir, int flags);

int lb_web_lan_cfg(Webs *wp, char *prefix, char *dir, int flags);
int lb_web_wifi_cfg(Webs *wp, char *prefix, char *dir, int flags);
int lb_web_get_wifi_hot(Webs *wp, char *prefix, char *dir, int flags);
int lb_web_volume_setting(Webs *wp, char *prefix, char *dir, int flags);
int lb_web_bright_setting(Webs *wp, char *prefix, char *dir, int flags);
int lb_web_light_comp(Webs *wp, char *prefix, char *dir, int flags);
int lb_web_factory_default(Webs *wp, char *prefix, char *dir, int flags);
int lb_web_fw_upg(Webs *wp, char *prefix, char *dir, int flags);
int lb_web_sys_info(Webs *wp, char *prefix, char *dir, int flags);
void lb_web_systime_set(Webs *wp, char *prefix, char *dir, int flags);

#endif

#ifdef WS_MG
void form_ip_setting(struct mg_connection *nc, int ev, void *p);

void lb_web_factory_default(struct mg_connection *nc, int ev, void *p);
void lb_web_fw_upg(struct mg_connection *nc, int ev, void *p);
void lb_web_sys_info(struct mg_connection *nc, int ev, void *p);
void lb_web_set_sys_admin(struct mg_connection *nc, int ev, void *p);
void lb_web_get_dev_time(struct mg_connection *nc, int ev, void *p);
void lb_web_set_dev_time(struct mg_connection *nc, int ev, void *p);
void lb_web_restart_device(struct mg_connection *nc, int ev, void *p);
void lb_web_add_manager(struct mg_connection *nc, int ev, void *p);
void lb_web_search_manager(struct mg_connection *nc, int ev, void *p);
void lb_web_update_manager(struct mg_connection *nc, int ev, void *p);
void lb_web_delete_manager(struct mg_connection *nc, int ev, void *p);
void lb_web_get_develop_select(struct mg_connection *nc, int ev, void *p);
void lb_web_set_develop_select(struct mg_connection *nc, int ev, void *p);
void lb_web_get_device_hwparam(struct mg_connection *nc, int ev, void *p);
void lb_web_set_device_hwparam(struct mg_connection *nc, int ev, void *p);
void lb_web_get_volume(struct mg_connection *nc, int ev, void *p);
void lb_web_set_volume(struct mg_connection *nc, int ev, void *p);
void lb_web_get_screeninfo(struct mg_connection *nc, int ev, void *p);
void lb_web_set_screeninfo(struct mg_connection *nc, int ev, void *p);
void lb_web_get_device_status(struct mg_connection *nc, int ev, void *p);
void lb_web_set_device_status(struct mg_connection *nc, int ev, void *p);
void lb_web_get_ethparam(struct mg_connection *nc, int ev, void *p);
void lb_web_set_ethparam(struct mg_connection *nc, int ev, void *p);
void lb_web_get_sys_baseinfo(struct mg_connection *nc, int ev, void *p);
void lb_web_get_wifi_param(struct mg_connection *nc, int ev, void *p);
void lb_web_set_wifi_param(struct mg_connection *nc, int ev, void *p);
void lb_web_scan_wifi_list(struct mg_connection *nc, int ev, void *p);
void lb_web_get_light_info(struct mg_connection *nc, int ev, void *p);
void lb_web_set_light_info(struct mg_connection *nc, int ev, void *p);
void lb_web_scan_device(struct mg_connection *nc, int ev, void *p);
void lb_web_get_macaddr(struct mg_connection *nc, int ev, void *p);
#endif

#endif
