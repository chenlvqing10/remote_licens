/*
 * omx_dbg.c - Standard functionality for the openmax core API.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <sys/sem.h>
#include <oscl.h>
#include <oscl_ipc.h>
#include <media_debug.h>
#include <signal.h>
#include <unistd.h>

static int g_test_flag = 1;
static void signal_handler(int sig)
{
	OSCL_LOGE("test end:%d", sig);
	g_test_flag = 0;
}
int main(int argc, char **argv)
{
	int ret;
	int semid;
	pid_t fpid = 0;
	int i;

	semid = semget((key_t)MEDIA_DBG_SEM_KEY, 1, IPC_EXCL | 0600);
	OSCL_LOGE("%d", semid);
	if (semid < 0) {
		fpid = fork();
		if (fpid == 0) {
			prctl(PR_SET_NAME, "omx_dbg deamon");
			for (i = 1; i < argc; i++)
				memset(argv[i], 0, strlen(argv[i]));
			argc = 1;
			media_init();
			signal(SIGINT, signal_handler);
			signal(SIGTERM, signal_handler);
			while (g_test_flag)
				oscl_mdelay(1000);
			media_deinit();
			semctl((key_t)MEDIA_DBG_SEM_KEY, 1, IPC_RMID, 0);
			OSCL_LOGE("exit test");
			return 0;
		} else
			msleep(500);
	}

	ret = send_cmd_to_pipe(MEDIA_DBG_PIPE, argc, argv);
	if (ret < 0)
		OSCL_LOGE("send cmd to pipe failed!");
	semid = ipc_sem_init((key_t)MEDIA_DBG_SEM_KEY);
	ipc_sem_post(semid);
	return 0;
}


