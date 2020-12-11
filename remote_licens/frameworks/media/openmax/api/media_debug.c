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
#include <dlfcn.h>
#include <oscl.h>
#include <omx_api.h>
#include <media_debug.h>
#include <oscl_ipc.h>

#define MEDIA_MAX_DBG_FUNC 30

#define MEDIA_IPC_DEBUG
#define M_NAME_LEN 16
typedef struct media_dbg_func {
	char name[M_NAME_LEN];
	int (*func)(mdbg_info_t *dbg, int argc, char **argv);
	char *info;
} media_dbg_func_t;

media_dbg_func_t mdbg_func[MEDIA_MAX_DBG_FUNC] = {
	{"print", print_dbg_info, "print dbg info"},
};

static mdbg_info_t m_dbg;

int parse_arg_buffer(char *buffer, int *argc, char **argv)
{
	const char *split = " ";
	char *p;
	int total;
	int i = 0;

	p = strtok(buffer, split);
	if (p == NULL) {
		OSCL_LOGE("not find # in %s\n", buffer);
		return -1;
	}
	total = atoi(p);
	if (total < 0 || total > MAX_ARGS_NUM)
		return -1;
	*argc = total;
	i = 0;
	while (p != NULL && i < total) {
		p = strtok(NULL, split);
		if (p) {
			argv[i] = oscl_zalloc(MAX_ARGS_LEN);
			strncpy(argv[i], p, MAX_ARGS_LEN - 1);
			i++;
		}
	}
	return 0;
}

mdbg_info_t *media_debug_get_dbginfo(void)
{
	if (m_dbg.init_flag == 0)
		media_debug_init();
	return &m_dbg;
}

void media_debug_help(void)
{
	int i;
	for (i = 0; i < MEDIA_MAX_DBG_FUNC; i++) {
		if (mdbg_func[i].name[0])
			OSCL_LOGE("%s: %s", mdbg_func[i].name, mdbg_func[i].info);
	}
}

#ifdef MEDIA_IPC_DEBUG
void *media_debug_daemon(void *param)
{
	char buf[1024];
	int i;
	int _argc;
	char *_argv[MAX_ARGS_NUM];
	size_t s;
	mdbg_info_t *dbg_param = param;

	prctl(PR_SET_NAME, "media_dgb_daemon");
	oscl_param_check(dbg_param != NULL, NULL, NULL);

	int fd = open(MEDIA_DBG_PIPE, O_RDONLY);
	if (fd < 0) {
		OSCL_LOGE("open MEDIA_DBG_PIPE fail");
		goto EXIT;
	}

	while (dbg_param->stop_flag == 0) {
		ipc_sem_wait(dbg_param->semid);
		memset(buf, 0, sizeof(buf));
		s = read(fd, buf, sizeof(buf) - 1);
		if (s <= 0)
			continue;
		OSCL_LOGI("==get dbg command(len:%d):%s", (int)s, buf);
		_argc = 0;
		parse_arg_buffer(buf, &_argc, _argv);
		if (_argc <= 1) {
			media_debug_help();
			continue;
		}
		for (i = 0; i < MEDIA_MAX_DBG_FUNC; i++) {
			if (mdbg_func[i].func &&
				strncmp(_argv[1], mdbg_func[i].name, M_NAME_LEN) == 0) {
				OSCL_LOGI("start dbg cmd:%p", mdbg_func[i].func);
				mdbg_func[i].func(dbg_param, _argc, _argv);
				break;
			}
		}
		if (i == MEDIA_MAX_DBG_FUNC)
			media_debug_help();
		for (i = 0; i < _argc; i++) {
			if (_argv[i])
				oscl_free(_argv[i]);
			_argv[i] = NULL;
		}
	}
	OSCL_LOGI("exit");
EXIT:
	if (dbg_param->semid != -1) {
		ipc_sem_destroy(dbg_param->semid);
		dbg_param->semid = -1;
	}
	OSCL_LOGE("exit");
	return NULL;
}

#ifdef CONFIG_MTEST
#define _TEST_LIB_NAME "libmedia_test.so"
void _libtest_init(mdbg_info_t *dbg_param)
{
	int i;
	int (*media_test)(mdbg_info_t *dbg, int argc, char **argv) = NULL;
	if (dbg_param->lib_test != NULL)
		goto EXIT;
	dbg_param->lib_test = dlopen(_TEST_LIB_NAME, RTLD_LAZY);
	if (dbg_param->lib_test == NULL) {
		OSCL_LOGE("Failed to open lib:%s: %s", _TEST_LIB_NAME, dlerror());
		goto EXIT;
	}
	media_test = dlsym(dbg_param->lib_test, "media_test");
	if (media_test == NULL) {
		dlclose(dbg_param->lib_test);
		OSCL_LOGE("%s dlsym fail:%s", _TEST_LIB_NAME, dlerror());
		goto EXIT;
	}
	for (i = 0; i < MEDIA_MAX_DBG_FUNC; i++) {
		if (mdbg_func[i].func == NULL) {
			OSCL_LOGE("add dbg func:%p", media_test);
			mdbg_func[i].func = media_test;
			strcpy(mdbg_func[i].name, "media_test");
			mdbg_func[i].info = "test command for media";
			break;
		}
	}
EXIT:
	return;
}

void _libtest_deinit(mdbg_info_t *dbg_param)
{
	int i;
	if (dbg_param->lib_test == NULL)
		return;

	for (i = 0; i < MEDIA_MAX_DBG_FUNC; i++) {
		if (mdbg_func[i].func &&
			strncmp("media_test", mdbg_func[i].name, M_NAME_LEN) == 0) {
			OSCL_LOGE("rm dbg func:%p", mdbg_func[i].func);
			mdbg_func[i].func = NULL;
			break;
		}
	}
	dlclose(dbg_param->lib_test);
	dbg_param->lib_test = NULL;
	return;
}
#else
void _libtest_init(mdbg_info_t *dbg_param)
{
	OSCL_LOGE("CONFIG_MTEST undefined. do nothing");
	return;
}
void _libtest_deinit(mdbg_info_t *dbg_param)
{
	return;
}
#endif

int media_debug_init()
{
	int ret = 0;
	mdbg_info_t *dbg_param = &m_dbg;

	OSCL_LOGI("enter");
	if (dbg_param->init_flag) {
		OSCL_LOGE("dbg_param already init");
		goto EXIT;
	}
	memset(dbg_param, 0, sizeof(mdbg_info_t));

	dbg_param->init_flag = 1;
	pthread_mutex_init(&dbg_param->lock, NULL);

	umask(0);
	remove(MEDIA_DBG_PIPE);
	if (mkfifo(MEDIA_DBG_PIPE, 0777) < 0) {
		OSCL_LOGE("mkfifo %s error!", MEDIA_DBG_PIPE);
		ret = -1;
		goto EXIT;
	}

	dbg_param->semid = semget((key_t)MEDIA_DBG_SEM_KEY, 1, IPC_EXCL | 0600);
	if (dbg_param->semid >= 0)
		OSCL_LOGE("semid is %d, maybe media quit unnormally", dbg_param->semid);
	else {
		dbg_param->semid = ipc_sem_init((key_t)MEDIA_DBG_SEM_KEY);
		if (dbg_param->semid == -1) {
			OSCL_LOGE("init ipc sem failed!");
			goto EXIT;
		}
	}

	_libtest_init(dbg_param);

	ret = pthread_create(&dbg_param->thread_id, NULL, media_debug_daemon, dbg_param);
	if (ret != 0) {
		OSCL_LOGE("pthread_create media_debug_daemon error!");
		dbg_param->thread_id = 0;
		ipc_sem_destroy(dbg_param->semid);
		dbg_param->semid = -1;
		ret = -1;
		goto EXIT;
	}
EXIT:
	return ret;
}

int media_debug_deinit()
{
	mdbg_info_t *dbg_param = &m_dbg;
	if (dbg_param->init_flag == 0) {
		OSCL_LOGE("dbg_param not init");
		goto EXIT;
	}
	if (dbg_param->thread_id) {
		dbg_param->stop_flag = 1;
		ipc_sem_post(dbg_param->semid);
		send_cmd_to_pipe(MEDIA_DBG_PIPE, 0, NULL);
		pthread_join(dbg_param->thread_id, NULL);
		dbg_param->thread_id = 0;
	}

	if (dbg_param->semid != -1) {
		ipc_sem_destroy(dbg_param->semid);
		dbg_param->semid = -1;
	}
	_libtest_deinit(dbg_param);

	pthread_mutex_destroy(&dbg_param->lock);
	memset(dbg_param, 0, sizeof(mdbg_info_t));

	remove(MEDIA_DBG_PIPE);
	OSCL_LOGE("exit");
EXIT:
	return 0;
}
#else
int media_debug_init()
{
	mdbg_info_t *dbg_param = &m_dbg;
	if (dbg_param->init_flag) {
		OSCL_LOGE("dbg_param already init");
		goto EXIT;
	}
	memset(dbg_param, 0, sizeof(mdbg_info_t));

	dbg_param->init_flag = 1;
	pthread_mutex_init(&dbg_param->lock, NULL);
EXIT:
	return 0;
}
int media_debug_deinit()
{
	mdbg_info_t *dbg_param = &m_dbg;
	if (dbg_param->init_flag == 0) {
		OSCL_LOGE("dbg_param not init");
		goto EXIT;
	}
	pthread_mutex_destroy(&dbg_param->lock);
	memset(dbg_param, 0, sizeof(mdbg_info_t));
EXIT:
	return 0;
}

#endif

