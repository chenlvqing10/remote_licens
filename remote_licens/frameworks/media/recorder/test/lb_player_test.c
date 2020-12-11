/*
 * lb_player_test.c - Standard functionality for recorder.
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
#include "oscl.h"
#include "omx_test.h"
#include "omx_mediaplayer.h"
#include "signal.h"

static pthread_t player_thread;

void _get_win(omxmp_win_t  *win, disp_size_t  *scn_size)
{
	win_para_t disp_para;
	int index = 0;
	if (g_test_para[0].recorder != NULL)
		index = 1;
	_get_config_disp_para(&disp_para, index);
	if (win) {
		win->left = disp_para.rect.x;
		win->top  = disp_para.rect.y;
		win->width = disp_para.rect.width;
		win->height = disp_para.rect.height;
	}
}

void *player_test_thread(void *param)
{
	int i;
	test_cmd_t *player_cmd = param;
	char *argv[FINSH_ARG_MAX];
	for (i = 0; i < player_cmd->argc; i++) {
		argv[i] = player_cmd->argv[i];
		OSCL_LOGE("%d:%p, ", i, argv[i]);
	}
	if (player_cmd)
		omxplayer_test(player_cmd->argc, argv);
	free(player_cmd);
	return NULL;
}

int omxplayer_agent(int argc, char *argv[])
{
	int ret = -1;
	int i;
	test_cmd_t *player_cmd;

	if (player_thread) {
		ret = pthread_kill(player_thread, 0);
		if (ret == ESRCH)
			player_thread = 0;
	}

	if (player_thread) {
		if (argc > FINSH_ARG_MAX)
			argc = FINSH_ARG_MAX;
		player_cmd = malloc(sizeof(test_cmd_t));
		player_cmd->argc = argc;
		for (i = 0; i < argc; i++) {
			strncpy(player_cmd->argv[i], argv[i], MAX_ARGS_LEN);
			OSCL_LOGE("%d:%s, ", i, player_cmd->argv[i]);
		}
		ret = pthread_create(&player_thread, NULL,
					player_test_thread, player_cmd);
		if (ret != 0)
			free(player_cmd);
	} else {
		ret = omxplayer_test(argc, argv);
	}
	return ret;
}

