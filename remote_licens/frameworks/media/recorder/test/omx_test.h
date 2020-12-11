/*
 * oscl.h - common lib api used by lombo media player.
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

#ifndef __LB_OMX_TEST_H__
#define __LB_OMX_TEST_H__
#include <oscl.h>
#include <base_component.h>
#include "vrender_component.h"
#include "vrec_component.h"
#include "omx_api.h"
#include "lb_recorder.h"
#include <getopt.h>
#include "recorder_private.h"
#include "dirent.h"
#include "lb_omx_core.h"
#include "recorder_pano.h"

#ifdef __linux__
#define TEST_ROOT_PATH "/mnt/card/"
#define VIC_PATH "/mnt/card/VIDEO_R/"
#define ISP_PATH "/mnt/card/VIDEO_F/"

#ifdef CONFIG_ARCH_LOMBO_N8V0
#define FRONT_INPUT_DEVICE_NAME	"/dev/video0"
#define FRONT_DEFAULT_WIDTH	1280
#define FRONT_DEFAULT_HEIGHT	720
#define BACK_INPUT_DEVICE_NAME	"/dev/video0"
#define BACK_DEFAULT_WIDTH	1280
#define BACK_DEFAULT_HEIGHT	720
#define MEDIA_TEST_PIPE "/mtestpipe"
#elif defined CONFIG_ARCH_LOMBO_N9V0
#define FRONT_INPUT_DEVICE_NAME	"/dev/video0"
#define FRONT_DEFAULT_WIDTH	1280
#define FRONT_DEFAULT_HEIGHT	720
#define BACK_INPUT_DEVICE_NAME	"/dev/video0"
#define BACK_DEFAULT_WIDTH	1280
#define BACK_DEFAULT_HEIGHT	720
#define MEDIA_TEST_PIPE "/mtestpipe"
#undef VIC_PATH
#undef ISP_PATH
#define VIC_PATH "/data/VIDEO_R/"
#define ISP_PATH "/data/VIDEO_F/"
#else
#define FRONT_INPUT_DEVICE_NAME	"/dev/video3"
#define FRONT_DEFAULT_WIDTH	1920
#define FRONT_DEFAULT_HEIGHT	1080
#define BACK_INPUT_DEVICE_NAME	"/dev/video1"
#define BACK_DEFAULT_WIDTH	800
#define BACK_DEFAULT_HEIGHT	600
#define MEDIA_TEST_PIPE "/data/mtestpipe"
#endif
#define DEFAULT_ROTATE 0
#elif defined __EOS__
#define TEST_ROOT_PATH "/mnt/sdcard/"
#define VIC_PATH "/mnt/sdcard/VIDEO_R/"
#define ISP_PATH "/mnt/sdcard/VIDEO_F/"
#define FRONT_INPUT_DEVICE_NAME	"isp"
#define FRONT_DEFAULT_WIDTH	1920
#define FRONT_DEFAULT_HEIGHT	1080
#define BACK_INPUT_DEVICE_NAME	"vic"
#define BACK_DEFAULT_WIDTH	1280
#define BACK_DEFAULT_HEIGHT	720
#define DEFAULT_ROTATE 1
#endif

#define TEST_MAX_REC_NUM (4)
#define TEST_DEVNAME_LEN (64)
#define TEST_FILENAME_LEN (64)

typedef struct auto_test_para {
	int index;
	int watermark_enable;
	int watermark;
	int stop_flag;
	int preview_flag;
	int record_flag;
	win_para_t disp_para;
	rec_param_t rec_para;
	char *name_prefix;
	char source[TEST_DEVNAME_LEN];
	char filename[TEST_FILENAME_LEN];
	char pic_name[TEST_FILENAME_LEN];
	char *path;
	void *recorder;
	pthread_t auto_thread;
#ifdef __EOS__
	rt_timer_t timer;
#else
#endif
	int line;
	int last_loop;
	int loop;
	char cmd_line[16];
	void *(*test_func)(void *);
} auto_test_para_t;
#ifndef FINSH_ARG_MAX
#define FINSH_ARG_MAX 16
#endif
typedef struct test_cmd {
	char str[64];
	char argc;
	char argv[FINSH_ARG_MAX][MAX_ARGS_LEN];
} test_cmd_t;

int g_exit_flag;

auto_test_para_t g_test_para[TEST_MAX_REC_NUM];
int test_max_loop_num;
int test_all_flag;
cali_out_data_t g_cali_out;
int cali_flag;
void *disp;

int watermark_test_source(void *rec);
int watermark_test(void *rec, int number);
long auto_lb(int argc, char **argv);
long lbrecorder(int argc, char **argv);
int recorder_show_components(int argc, char **argv);
void auto_test_all(void);
int pano_test(void *erecorder);
void pano_exit(void *erecorder);
int cb_file_closed(void *hdl, char *file_name);
int cb_get_next_file(void *hdl, char *next_file);
int cb_get_next_jpg(void *hdl, char *next_file);
void init_test_para(void);
void _get_config_disp_para(win_para_t *disp_para, int index);
int omxplayer_test(int argc, char *argv[]);
int omxplayer_agent(int argc, char *argv[]);

#endif /* __LB_OSCL_H__ */
