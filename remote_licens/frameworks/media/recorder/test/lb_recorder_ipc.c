/*
 * lb_recorder.c - Standard functionality for recorder.
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

#define DBG_LEVEL		DBG_INFO

#include <oscl.h>
#include <base_component.h>
#include "omx_test.h"
#include "vrender_component.h"
#include "vrec_component.h"
#include "omx_api.h"
#include "lb_recorder.h"
#include <getopt.h>
#include "recorder_private.h"
#include "dirent.h"
#include "lb_omx_core.h"
#include "oscl_ipc.h"
#include "media_debug.h"

#ifdef __EOS__
MSH_CMD_EXPORT(lbrecorder, "lombo recorder");
MSH_CMD_EXPORT(auto_lb, "auto_lombo recorder");
MSH_CMD_EXPORT(recorder_show_components, "recorder_show_components");
#else
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <signal.h>

#define MTEST_KEY 0x5678
static int g_test_flag = 1;
static void signal_handler(int sig)
{
	OSCL_LOGE("test end:%d", sig);
	g_test_flag = 0;
}
int media_test_server()
{
	umask(0);
	char buf[1024] = {0};
	int i;
	int _argc;
	char *_argv[MAX_ARGS_NUM];
	size_t s;
	int semid = -1;
	int fd = -1;

	OSCL_LOGE("test start");
	media_init();

	remove(MEDIA_TEST_PIPE);
	if (mkfifo(MEDIA_TEST_PIPE, 0777) < 0) {
		OSCL_LOGE("mkfifo MEDIA_TEST_PIPE(%s) error!", MEDIA_TEST_PIPE);
		perror("mkfifo");
		goto EXIT;
	}
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	semid = ipc_sem_init((key_t)MTEST_KEY);
	fd = open(MEDIA_TEST_PIPE, O_RDWR);
	if (fd < 0 || semid < 0) {
		OSCL_LOGE("open pipe(%s) or sem(MTEST_KEY) fail: %d, %d",
			MEDIA_TEST_PIPE, fd, semid);
		perror("open");
		goto EXIT;
	}
	while (1) {
		ipc_sem_wait(semid);
		if (g_test_flag == 0)
			break;
		memset(buf, 0, sizeof(buf));
		s = read(fd, buf, sizeof(buf)-1);
		OSCL_LOGE("==read:%s %d", buf, (int)s);
		if (s > 0) {
			buf[s] = '\0';
			OSCL_LOGE("cmd:%s", buf);
			parse_arg_buffer(buf, &_argc, _argv);
			if (_argc == 0)
				continue;
			OSCL_LOGE("_argv[0]:%s", _argv[0]);
			if (strcmp(_argv[0], "auto_lb") == 0)
				auto_lb(_argc, _argv);
			if (strcmp(_argv[0], "lbrecorder") == 0)
				lbrecorder(_argc, _argv);
			if (strcmp(_argv[0], "recorder_show_components") == 0)
				recorder_show_components(_argc, _argv);
			if (strcmp(_argv[0], "async_player") == 0)
				omxplayer_agent(_argc, _argv);
			for (i = 0; i < _argc; i++) {
				if (_argv[i])
					oscl_free(_argv[i]);
				_argv[i] = NULL;
			}
		}
	}
EXIT:
	media_deinit();
	if (fd >= 0)
		close(fd);
	if (semid >= 0)
		ipc_sem_destroy(semid);
	remove(MEDIA_TEST_PIPE);
	OSCL_LOGE("");
	return 0;
}


#ifdef _TEST_MEDIA_SERVER

int main()
{
	return media_test_server();
}
#endif


#ifdef _TEST_MEDIA_CLIENT
static pid_t __fork_mserver(int argc, char **argv)
{
	pid_t fpid = 0;
	int i;
	fpid = fork();
	if (fpid == 0) {
		prctl(PR_SET_NAME, "mediatest deamon");
		for (i = 1; i < argc; i++)
			memset(argv[i], 0, strlen(argv[i]));
		argc = 1;
		media_test_server();
		OSCL_LOGE("exit test");
		return 0;
	}
	/* sleep 500ms, wait for children process created */
	msleep(500);
	return fpid;
}

int main(int argc, char **argv)
{
	int ret;
	int semid;
	pid_t fpid = 0;

	OSCL_LOGE("test start");
	semid = semget((key_t)MTEST_KEY, 1, IPC_EXCL | 0600);
	if (semid < 0) {
		fpid = __fork_mserver(argc, argv);
		if (fpid == 0)
			return 0;
	}
	ret = send_cmd_to_pipe(MEDIA_TEST_PIPE, argc, argv);
	OSCL_LOGE("test start, ret:%d", ret);
	if (ret < 0) {
		OSCL_LOGE("open %s failed.", MEDIA_TEST_PIPE);
#if 0
		ret = send_cmd_to_pipe(MEDIA_TEST_PIPE, argc, argv);
		char in_char;
		if (strcmp(argv[0], "auto_lb") == 0)
			auto_lb(argc, argv);
		else if (strcmp(argv[0], "lbrecorder") == 0)
			lbrecorder(argc, argv);
		else if (strcmp(argv[0], "recorder_show_components") == 0)
			recorder_show_components(argc, argv);
		else if (strcmp(argv[0], "async_player") == 0)
			omxplayer_agent(argc, argv);
		else
			return 0;
		while (1) {
			scanf("%c", &in_char);
			if (in_char != 'q' && in_char != 'Q')
				printf("input 'q' to quit\n");
			else
				break;
		}
#endif
	}
	semid = ipc_sem_init((key_t)MTEST_KEY);
	ipc_sem_post(semid);
	return 0;
}
#endif

#endif

