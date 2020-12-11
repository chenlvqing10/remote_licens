/*
 * file_http_download.h
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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


#ifndef __FILE_HTTP_DOWNLOAD_H__
#define __FILE_HTTP_DOWNLOAD_H__

struct download_handler_s {
	void *hh;
	int progress;
	int loading;
	int (*set_url)(void *handler, char *url, char *storage_path, char *file_name);
	int (*start)(void *handler);
	int (*pause)(void *handler);
	int (*resume)(void *handler);
	int (*stop)(void *handler); /* release resources byself */
	int (*set_auto_free_res)(void *handler); /* finish download auto release resources */
	int (*set_finish_cb)(void *handler, void (*finish_cb)(void *, int), void *data, int size); /* finish download cb to app */
};

struct download_handler_s *http_download_create(void);
#endif //__FILE_HTTP_DOWNLOAD_H__

