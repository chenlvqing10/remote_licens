/*
 * oscl_debug_ipc.c - queue api used by lombo media framework.
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

struct semun {
	int val;
};

int ipc_sem_init(key_t sem_key)
{
	int semid;
	semid = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0600);
	if (semid == -1) {
		semid = semget(sem_key, 1, IPC_CREAT | 0600);
		if (semid == -1)
			OSCL_LOGE("_sem_init err!");
	} else {
		struct semun a;
		a.val = 0;
		if (semctl(semid, 0, SETVAL, a) == -1)
			OSCL_LOGE("semctl init error");
	}
	return semid;
}

void ipc_sem_wait(int semid)
{
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;

	if (semop(semid, &buf, 1) == -1)
		OSCL_LOGE("_sem_wait error");
}

void ipc_sem_post(int semid)
{
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO;
	if (semop(semid, &buf, 1) == -1)
		perror("_sem_post error");
}

void ipc_sem_destroy(int semid)
{
	if (semctl(semid, 0, IPC_RMID) == -1)
		perror("semctl destroy error");
}

int send_cmd_to_pipe(char *pipe, int argc, char **argv)
{
	char buf[1024] = {0};
	int fd;
	int i;

	if (argc > MAX_ARGS_NUM) {
		OSCL_LOGE("argc > %d, error!", MAX_ARGS_NUM);
		return 0;
	}
	fd = open(pipe, O_WRONLY);
	if (fd < 0) {
		OSCL_LOGE("open MEDIA_TEST_PIPE failed");
		perror("open");
		return -1;
	}

	fflush(stdout);
	sprintf(buf, "%d", argc);
	OSCL_LOGI("buffer:%s, %d", buf, (int)strlen(buf));
	write(fd, buf, strlen(buf));

	i = 0;
	while (i < argc) {
		if (strlen(argv[i]) < MAX_ARGS_LEN - 1) {
			OSCL_LOGI("argv[%d]:%s, %d", i, argv[i], (int)strlen(argv[i]));
			write(fd, " ", 1);
			write(fd, argv[i], strlen(argv[i]));
			i++;
		} else {
			OSCL_LOGI("argv[%d]:%s is too long!!", i, argv[i]);
		}
	}
	write(fd, '\0', 1);
	fflush(stdout);
	close(fd);
	OSCL_LOGI("exit");
	return 0;
}

