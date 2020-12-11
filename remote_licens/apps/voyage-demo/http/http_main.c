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

#include "ws_http.h"
#include "web_biz_api.h"
#include "mongoose.h"

int create_http_task(void)
{
	int err = 0;
	pthread_t tmp_task_id;

	err = pthread_create(&tmp_task_id, NULL, (void *)ws_http_proc, NULL);
	if (err != 0) {
		printf("\n [%s] create thread error: %d!\n", __func__, err);
		return -1;
	}
	printf("\n [%s] create thread OK: %d!\n", __func__, err);

	pthread_detach((pthread_t)tmp_task_id);

	return 0;
}

int main()
{
	int err = 0;
	char port[32];
	char doc_root[128];
	memset(port, 0, sizeof(port));
	memset(doc_root, 0, sizeof(doc_root));
	strcpy(port, "80");
	strcpy(doc_root, "/etc/webpage");
	printf("port=%s, doc_root=%s\n", port, doc_root);
	err = ws_http_init(port, doc_root);
	if (err != 0) {
		printf("http web server init failed %d!\n", err);
		return err;
	}

	err = ws_http_register_endpoint("/action/recgcfg", WS_EP_URL, lb_web_recg_cfg);

	err |= ws_http_register_endpoint("/action/brightsetting", WS_EP_URL,
		lb_web_bright_setting);

	err |= ws_http_register_endpoint("/action/sysinfo", WS_EP_URL, lb_web_sys_info);

	if (err != 0)
		printf("Onload Handler failed %d!\n", err);

	err = create_http_task();
	if (err != 0)
		printf("create http web server task failed %d!\n", err);

	while (1)
		usleep(50000);

	return err;
}

