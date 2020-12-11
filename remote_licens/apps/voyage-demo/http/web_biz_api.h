/*
 * api/web_biz_api.h - all functions for RESTful.
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

#ifndef _WEB_BIZ_API_H_
#define _WEB_BIZ_API_H_

#include "mongoose.h"

void lb_web_recg_cfg(struct mg_connection *nc, int ev, void *p);
void lb_web_bright_setting(struct mg_connection *nc, int ev, void *p);
void lb_web_sys_info(struct mg_connection *nc, int ev, void *p);

#endif

