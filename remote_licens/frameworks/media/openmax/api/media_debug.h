/*
 * media_debug.h - media debug api.
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

#ifndef __MEDIA_DBG_H__
#define __MEDIA_DBG_H__

#include "omx_api.h"

#define DBG_COMP_MAX_INST 128
#define DBG_HDL_MAX_INST 8
#define MEDIA_DBG_SEM_KEY 0x1256

#ifdef CONFIG_ARCH_LOMBO_N8V0
#define MEDIA_DBG_PIPE "/mdebugpipe"
#elif defined CONFIG_ARCH_LOMBO_N9V0
#define MEDIA_DBG_PIPE "/mdebugpipe"
#else
#define MEDIA_DBG_PIPE "/data/mdebugpipe"
#endif

typedef struct media_dbg_s {
	int init_flag;
	int stop_flag;
	int semid;
	pthread_t thread_id;

	pthread_mutex_t lock;
	void *components[DBG_COMP_MAX_INST];
	void *rec_vstream[DBG_HDL_MAX_INST];
	void *rec_astream[DBG_HDL_MAX_INST];
	void *recorder[DBG_HDL_MAX_INST];
	void *player[DBG_HDL_MAX_INST];
	void *reserve[DBG_HDL_MAX_INST];

	void *lib_test;
} mdbg_info_t;

mdbg_info_t *media_debug_get_dbginfo(void);
int print_dbg_info(mdbg_info_t *dbg_param, int argc, char **argv);

int parse_arg_buffer(char *buffer, int *argc, char **argv);
int media_debug_init();
int media_debug_deinit();
int mdebug_rm_component(comp_info_t *comp);
int mdebug_add_component(comp_info_t *comp);
int print_dbg_info(mdbg_info_t *dbg_param, int argc, char **argv);


#endif /* __MEDIA_DBG_H__ */
