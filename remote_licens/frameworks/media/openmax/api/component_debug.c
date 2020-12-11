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
#include <omx_api.h>
#include <getopt.h>
#include <media_debug.h>

typedef struct comp_dgb_log {
	int detail;
	int dbg_comp_index;
	int help;
} _dgb_log_cmd_t;

typedef struct comp_dgb_info {
	int detail;
	int dbg_comp_index;
	int help;
} comp_dgb_info_t;

static const char *omx_log_opts = "c:dh";

struct option omx_log_long_options[] = {
	{"comp", 1, 0, 'c'},
	{"detail", 0, 0, 'd'},
	{"help", 0, 0, 'h'},
	{0, 0, 0, 0}
};
int get_log_config(int argc, char **argv, _dgb_log_cmd_t *cmd_cfg)
{
	int          c;
	int          index;
	while (1) {
		c = getopt_long(argc, argv, omx_log_opts, omx_log_long_options, &index);
		if (c == -1) {
			OSCL_LOGI("get args end, exit!\n");
			optind = 0;
			break;
		}
		OSCL_LOGI("option:%c", c);
		OSCL_LOGI("option:%c, optarg:%s", c, (optarg == NULL) ? "null" : optarg);
		switch (c) {
		case 'd':
			cmd_cfg->detail = 1;
			break;
		case 'c':
			if (optarg != NULL && optarg[0] == 'a')
				cmd_cfg->dbg_comp_index = DBG_COMP_MAX_INST;
			else
				cmd_cfg->dbg_comp_index = atoi(optarg);
			break;
		case 'h':
			cmd_cfg->help = 1;
			break;
		default:
			break;
		}
	}
	return 0;
}

int _print_comp_list(mdbg_info_t *dbg_param)
{
	int i;
	comp_info_t *comp;
	oscl_param_check(dbg_param != NULL, -1, NULL);

	OSCL_PRINT("  %-8s %-30s\n", "index", "component name");
	for (i = 0; i < DBG_COMP_MAX_INST; i++) {
		comp = dbg_param->components[i];
		if (comp)
			OSCL_PRINT("  %-8d %-30s %p\n", i, comp->name, comp);
	}
	return 0;
}

int _print_port_info(port_info_t *port_info)
{
	int i;
	OSCL_PRINT("index: %d\n", port_info->index);
	OSCL_PRINT("state:%x, edir:%d, domain:%d\n",
		port_info->state, port_info->state, port_info->domain);
	OSCL_PRINT("connected omxcomp:%p, port:%p\n",
		port_info->tunnel_hdl, port_info->connect_port);
	OSCL_PRINT("nbuffers:%d, size:%d k\n",
		port_info->nbuffer, port_info->buf_size / 1024);
	if (port_info->hold_map) {
		OSCL_PRINT("port hold buffers:\n");
		for (i = 0; i < port_info->nbuffer; i++)
			if (port_info->hold_map[i] >= 0)
				OSCL_PRINT("%d(%d), ", i, port_info->hold_map[i]);
		OSCL_PRINT("\n");
	}
	if (port_info->header) {
		OSCL_PRINT("port buffer headers: %p\n", port_info->header);
		for (i = 0; i < port_info->nbuffer; i++)
			if (port_info->header[i])
				OSCL_PRINT("%d(%p:%p), ", i, port_info->header[i],
					port_info->header[i]->pBuffer);
		OSCL_PRINT("\n");
	}
	if (port_info->cb_async.buf_handle)
		OSCL_PRINT("cb async info: type-%d, appdata-%p, buf-handle:%p\n",
			port_info->cb_async.type, port_info->cb_async.app_data,
			port_info->cb_async.buf_handle);
	if (port_info->cb_filter.buf_handle)
		OSCL_PRINT("cb filter info: type-%d, appdata-%p, buf-handle:%p\n",
			port_info->cb_filter.type, port_info->cb_filter.app_data,
			port_info->cb_filter.buf_handle);
	if (port_info->priv_data && (port_info->priv_data->port == port_info) &&
		port_info->priv_data->cb_frame) {
		OSCL_PRINT("callback frame:");
		for (i = 0; i < port_info->nbuffer; i++)
			OSCL_PRINT(" %p,", port_info->priv_data->cb_frame[i].header);
		OSCL_PRINT("\n");
	}
	return 0;
}

int _print_comp_info(comp_info_t *comp_info, int detail_flag)
{
	int tmp;
	/* param check*/
	oscl_param_check(comp_info != NULL, -1, NULL);

	OSCL_PRINT("comp(%p-%p): %s\n", comp_info, comp_info->cmp_hdl,
		comp_info->name ? comp_info->name : "NULL");
	OSCL_PRINT("state:%x\n", comp_info->state);
	if (pthread_mutex_trylock(&comp_info->state_lock) == 0) {
		pthread_mutex_unlock(&comp_info->state_lock);
		OSCL_PRINT("state_lock is free\n");
	} else
		OSCL_PRINT("state_lock is busy\n");

	if (comp_info->sem_cmd) {
		sem_getvalue(comp_info->sem_cmd, &tmp);
		OSCL_PRINT("sem_cmd val:%d\n", tmp);
	}
	if (comp_info->priv_data)
		OSCL_PRINT("private data:%p\n", comp_info->priv_data);
	OSCL_PRINT("number of ports:%d\n", comp_info->num_port);
	for (tmp = 0; tmp < comp_info->num_port; tmp++) {
		OSCL_PRINT("port %d: %p\n", tmp, &comp_info->port_info[tmp]);
		_print_port_info(&comp_info->port_info[tmp]);
	}
	if (detail_flag)
		base_show_component(comp_info->cmp_hdl);
	return 0;
}

int print_dbg_info(mdbg_info_t *dbg_param, int argc, char **argv)
{
	int i;
	_dgb_log_cmd_t cmd;

	oscl_param_check(dbg_param != NULL, -1, NULL);

	pthread_mutex_lock(&dbg_param->lock);
	memset(&cmd, 0, sizeof(cmd));
	cmd.dbg_comp_index = -1;
	get_log_config(argc, argv, &cmd);
	if (cmd.dbg_comp_index >= 0 && cmd.dbg_comp_index < DBG_COMP_MAX_INST) {
		OSCL_PRINT("==component %d==\n", cmd.dbg_comp_index);
		_print_comp_info(dbg_param->components[cmd.dbg_comp_index], cmd.detail);
	} else if (cmd.dbg_comp_index == DBG_COMP_MAX_INST) {
		for (i = 0; i < DBG_COMP_MAX_INST; i++) {
			if (dbg_param->components[i]) {
				OSCL_PRINT("==component %d==\n", i);
				_print_comp_info(dbg_param->components[i], cmd.detail);
			}
		}
	} else
		_print_comp_list(dbg_param);
	pthread_mutex_unlock(&dbg_param->lock);

	return 0;
}

int mdebug_add_component(comp_info_t *comp)
{
	int i;
	int ret = -1;
	mdbg_info_t *dbg_param = media_debug_get_dbginfo();

	oscl_param_check(comp != NULL, -1, NULL);
	oscl_param_check(dbg_param != NULL, -1, NULL);
	pthread_mutex_lock(&dbg_param->lock);
	for (i = 0; i < DBG_COMP_MAX_INST; i++) {
		if (dbg_param->components[i] == NULL) {
			dbg_param->components[i] = comp;
			ret = 0;
			break;
		}
	}
	pthread_mutex_unlock(&dbg_param->lock);
	if (i == DBG_COMP_MAX_INST)
		OSCL_LOGE("exceed max components instance");
	return ret;
}

int mdebug_rm_component(comp_info_t *comp)
{
	int i;
	int ret = -1;
	mdbg_info_t *dbg_param = media_debug_get_dbginfo();

	oscl_param_check(comp != NULL, -1, NULL);
	oscl_param_check(dbg_param != NULL, -1, NULL);

	pthread_mutex_lock(&dbg_param->lock);
	for (i = 0; i < DBG_COMP_MAX_INST; i++) {
		if (dbg_param->components[i] == comp) {
			dbg_param->components[i] = NULL;
			ret = 0;
			break;
		}
	}
	pthread_mutex_unlock(&dbg_param->lock);
	if (i == DBG_COMP_MAX_INST)
		OSCL_LOGE("can not find comp:%p in dbg list", comp);
	return ret;
}

