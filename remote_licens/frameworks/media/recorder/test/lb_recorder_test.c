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

void *disp;

auto_test_para_t g_test_para[TEST_MAX_REC_NUM];

int test_max_loop_num = 20000;
cali_out_data_t g_cali_out;
int cali_flag;
struct option long_options[] = {
	{"file", 1, 0, 'f'},
	{"filefmt", 1, 0, 'F'},
	{"vencfmt", 1, 0, 'V'},
	{"dev", 1, 0, 'd'},
	{"pixel", 1, 0, 'P'},
	{"fixtime", 0, 0, 'x'},
	{"prepare", 0, 0, 'p'},
	{"preview", 0, 0, 'v'},
	{"stop-preview", 0, 0, 't'},
	{"rec", 0, 0, 'r'},
	{"stop", 0, 0, 's'},
	{"quit", 0, 0, 'q'},
	{"help", 0, 0, 'h'},
	{"callback", 0, 0, 'c'},
	{"pano", 0, 0, 'n'},
	{"exit-pano", 0, 0, 'e'},
	{"time", 0, 0, 'm'},
	{"status", 0, 0, 'u'},
	{"auto-all", 0, 0, 'a'},
	{"pic", 1, 0, 'i'},
	{"watermark", 1, 0, 'w'},
	{"show", 0, 0, 'o'},
	{0, 0, 0, 0}
};


void init_test_para(void)
{
	DIR *dir = NULL;

	dir = opendir(VIC_PATH);
	if (dir == NULL)
		mkdir(VIC_PATH, 0);
	else
		closedir(dir);

	dir = opendir(ISP_PATH);
	if (dir == NULL)
		mkdir(ISP_PATH, 0);
	else
		closedir(dir);

	memset(g_test_para, 0, sizeof(g_test_para));
	strncpy(g_test_para[0].source, BACK_INPUT_DEVICE_NAME, TEST_DEVNAME_LEN - 1);
	g_test_para[0].index = 0;
	g_test_para[0].path = VIC_PATH;
	g_test_para[0].rec_para.bitrate = 6000000;
	g_test_para[0].rec_para.source_height = BACK_DEFAULT_HEIGHT;
	g_test_para[0].rec_para.source_width = BACK_DEFAULT_WIDTH;
	g_test_para[0].rec_para.rotate = 0;
	g_test_para[0].rec_para.enc_rot = 0;
	g_test_para[0].rec_para.height = g_test_para[0].rec_para.source_height;
	g_test_para[0].rec_para.width = g_test_para[0].rec_para.source_width;
	g_test_para[0].name_prefix = "vic";
	if (g_test_para[0].rec_para.enc_rot) {
		g_test_para[0].rec_para.height = g_test_para[0].rec_para.source_width;
		g_test_para[0].rec_para.width = g_test_para[0].rec_para.source_height;
		g_test_para[0].rec_para.height /= 16;
		g_test_para[0].rec_para.height *= 16;
	}
#ifdef __FPGA__
		g_test_para[0].rec_para.file_fmt = REC_OUTPUT_FORMAT_TS;
		g_test_para[0].rec_para.venc_format = VIDEO_ENCODER_H265;
		g_test_para[0].path = "/data/";
		strncpy(g_test_para[0].source, "/dev/video0", TEST_DEVNAME_LEN - 1);
		g_test_para[0].rec_para.source_height = 480;
		g_test_para[0].rec_para.source_width = 640;
		g_test_para[0].rec_para.height = 480;
		g_test_para[0].rec_para.width = 640;
		g_test_para[0].rec_para.bitrate = 3000000;
#endif
	sprintf(g_test_para[0].filename, "%s%s_%04d.ts", g_test_para[0].path,
		g_test_para[0].name_prefix,
		g_test_para[0].loop%70);
	g_test_para[0].loop++;

	OSCL_LOGE("0 preview rot:%d, enc rot:%d",
		g_test_para[0].rec_para.rotate, g_test_para[0].rec_para.enc_rot);

	strncpy(g_test_para[1].source, FRONT_INPUT_DEVICE_NAME, TEST_DEVNAME_LEN - 1);
	g_test_para[1].index = 1;
	g_test_para[1].path = ISP_PATH;
	g_test_para[1].rec_para.bitrate = 14000000;
	g_test_para[1].rec_para.source_height = FRONT_DEFAULT_HEIGHT;
	g_test_para[1].rec_para.source_width = FRONT_DEFAULT_WIDTH;
	g_test_para[1].rec_para.rotate = 0;
	g_test_para[1].rec_para.enc_rot = 0;
	g_test_para[1].rec_para.height = g_test_para[1].rec_para.source_height;
	g_test_para[1].rec_para.width = g_test_para[1].rec_para.source_width;
	g_test_para[1].name_prefix = "isp";
	if (g_test_para[1].rec_para.enc_rot) {
		g_test_para[1].rec_para.height = g_test_para[1].rec_para.source_width;
		g_test_para[1].rec_para.width = g_test_para[1].rec_para.source_height;
		g_test_para[1].rec_para.height /= 16;
		g_test_para[1].rec_para.height *= 16;
	}
#ifdef __FPGA__
		g_test_para[1].rec_para.file_fmt = REC_OUTPUT_FORMAT_TS;
		g_test_para[1].rec_para.venc_format = VIDEO_ENCODER_H265;
		g_test_para[1].path = "/data/";
		strncpy(g_test_para[1].source, "/dev/video0", TEST_DEVNAME_LEN - 1);
		g_test_para[1].rec_para.source_height = 480;
		g_test_para[1].rec_para.source_width = 640;
		g_test_para[1].rec_para.height = 480;
		g_test_para[1].rec_para.width = 640;
		g_test_para[1].rec_para.bitrate = 3000000;
#endif
	sprintf(g_test_para[1].filename, "%s%s_%04d.ts", g_test_para[1].path,
		g_test_para[1].name_prefix,
		g_test_para[1].loop%70);
	g_test_para[1].loop++;
	OSCL_LOGE("==1 preview rot:%d, enc rot:%d",
		g_test_para[1].rec_para.rotate, g_test_para[1].rec_para.enc_rot);
}

#ifdef __EOS__
unsigned int check_mem(const char *func, rt_uint32_t pre_mem, rt_int32_t count)
{
	unsigned int total = 0;
	unsigned int used = 0;
	unsigned int max_used = 0;

	rt_memory_info(&total, &used, &max_used);
	if (pre_mem > 0) {
		if (pre_mem != used) {
			OSCL_LOGE(">>>>>>>>>>>>>>>[%s %d %d]test fail.<<<<<<<<<<<<<<<",
					func, count, (used - pre_mem));
		} else {
			OSCL_LOGI(">>>>>>>>>>>>>>>[%s %d %d]test success.<<<<<<<<<<<<<<<",
					func, count, (used - pre_mem));
		}
	}
	OSCL_LOGI("total memory: %d\n", total);
	OSCL_LOGI("used memory : %d\n", used);
	OSCL_LOGI("maximum allocated memory: %d\n", max_used);

	return used;
}
#else
#define check_mem(func, pre_mem, count) 0
#define osal_dump()
#define list_mem()
#endif

static void lbrecorder_help(void)
{
	#define arg_format "  %-20s %s%s"

	OSCL_PRINT("usage: lbrecorder [argument]");
	OSCL_PRINT("arguments:");
	OSCL_PRINT(arg_format, "--file filename", "set out filename", "");
	OSCL_PRINT(arg_format, "--filefmt", "set filefmt: 1-mp4, 3-raw, 4-ts", "");
	OSCL_PRINT(arg_format, "--vencfmt",
		"set video enc format:1-h264, 2-mjpeg, 3-265", "");
	OSCL_PRINT(arg_format, "--dev devname", "set device(before dev start)", "");
	OSCL_PRINT(arg_format, "--pixel height", "set source pixel(480, 720, 1080)", "");
	OSCL_PRINT(arg_format, "--preview", "start preview", "");
	OSCL_PRINT(arg_format, "--stop-preview", "stop preview", "");
	OSCL_PRINT(arg_format, "--rec", "start recorder", "");
	OSCL_PRINT(arg_format, "--stop", "stop recorder", "");
	OSCL_PRINT(arg_format, "--fixtime", "record recyle with each file 30s", "");
	OSCL_PRINT(arg_format, "--callback", "callback test", "");
	OSCL_PRINT(arg_format, "--pano", "pano test. Support in eos", "");
	OSCL_PRINT(arg_format, "--exit-pano", "exit pano test. Support in eos", "");
	OSCL_PRINT(arg_format, "--auto-all", "run all auto test case", "");
	OSCL_PRINT(arg_format, "--pic", "take picture", "");
	OSCL_PRINT(arg_format, "--watermark number", "set watermark(num 0-9999).", "");
	OSCL_PRINT(arg_format, "--time", "show rec time", "");
	OSCL_PRINT(arg_format, "--status", "show statuc", "");
	OSCL_PRINT(arg_format, "--show", "show active components.", "");
	OSCL_PRINT(arg_format, "--quit", "quit lb recorder test", "");
	OSCL_PRINT(arg_format, "--help", "show this help info", "");
}

static int buf_handle(void *lbrecorder, frame_t *frame)
{
	OSCL_LOGE("fame:%p:%p, time:%d, size(w-h):%d,%d, pri:%p, %p", frame,
		  frame->info.video.addr[0], (int)frame->info.video.time_stamp,
		  frame->info.video.width, frame->info.video.height,
		  frame->priv_data, frame->header);
	lb_recorder_ctrl(lbrecorder, LB_REC_FREE_FRAME, (void *)frame);
	OSCL_LOGE("free end");
	return 0;
}

void _get_config_disp_para(win_para_t *disp_para, int index)
{
	disp_size_t size;
	void *disp_dev;

	memset(&size, 0, sizeof(disp_size_t));
	disp_dev = oscl_open_disp_engine();
	if (disp_dev)
		oscl_disp_get_screen_size(disp_dev, &size);
	oscl_close_disp_engine(disp_dev);
	disp_para->rect.x = 0;
	disp_para->rect.y = 0;
	if (size.width > size.height) {
		size.width /= 2;
		disp_para->rect.x += index * size.width;
	} else {
		size.height /= 2;
		disp_para->rect.y += index * size.height;
	}
	disp_para->mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
	disp_para->rect.width = size.width;
	disp_para->rect.height = size.height;

}

int pano_test(void *erecorder)
{
	cali_param_t cali_para;
	win_para_t win_para;
	pano_param_t init_para;
	cali_contex_t cali_ctx;
	vsize_t prev_size;
	vsize_t size;
	int ret = 0;

	if (NULL == erecorder)
		return -1;
	memset(&cali_para, 0, sizeof(cali_para));
	memset(&win_para, 0, sizeof(win_para));
	memset(&init_para, 0, sizeof(init_para));
	memset(&size, 0, sizeof(size));

	ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_CREAT, NULL);
	if (0 == cali_flag) {
		/* 1. set calibration para */
		cali_para.box_rows = 5;
		cali_para.box_cols = 11;
		cali_para.box_width = 20;
		cali_para.box_height = 20;
		cali_para.dist_2_rear = 80;
		cali_para.car_width = 180;
		cali_para.car_length = 460;
		cali_para.front_dist = 100;
		cali_para.rear_dist = 500;
		cali_para.align = -1;
		cali_para.use_ext_cali_img = 1; /* use extern calibration image */
		cali_para.ext_cali_img.width = 1280;
		cali_para.ext_cali_img.height = 720;
		strncpy(cali_para.ext_cali_img.format, "nv12",
				sizeof(cali_para.ext_cali_img.format) - 1);
		strncpy(cali_para.ext_cali_img.path, TEST_ROOT_PATH"cali_yuv.bin",
				sizeof(cali_para.ext_cali_img.path) - 1);
		ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_SET_CALI_PARA,
				(void *)&cali_para);
	}
	/* 2. set init pano para */
	init_para.in_gps = 0;
	init_para.in_obd = 0;
	init_para.car_para_en = 1;
	init_para.car_width = cali_para.car_width;
	init_para.data_format = NULL;
	init_para.carboard_img.width = 82;
	init_para.carboard_img.height = 209;

	init_para.use_ext_cutline = 0; /* use extern cutline value */
	init_para.culine = 300;

	strncpy(init_para.carboard_img.format, "nv12",
			sizeof(init_para.carboard_img.format) - 1);
	strncpy(init_para.carboard_img.path, TEST_ROOT_PATH"car_yuv.bin",
			sizeof(init_para.carboard_img.path) - 1);
	ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_SET_INIT_PARA, (void *)&init_para);


	/* 4. set pano display mode */
	win_para.mode = VDISP_WINDOW_USERDEF;
	win_para.rect.x = 0;
	win_para.rect.y = 100;
	win_para.rect.width = 320;
	win_para.rect.height = 160;
	ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_SET_DISP_MODE, (void *)&win_para);

	prev_size.width = 160;
	prev_size.height = 320;
	ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_SET_PREVIEW_SIZE,
				(void *)&prev_size);

	if (cali_flag) {
		ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_SET_CALI_DATA, &g_cali_out);
		if (g_cali_out.data)
			oscl_free(g_cali_out.data);
		memset(&g_cali_out, 0, sizeof(g_cali_out));
	}
	/* 5. enable pano stream */
	ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_START, NULL);

	/* 6. enable calibration */
	if (0 == cali_flag) {
		memset(&cali_ctx, 0, sizeof(cali_ctx));
		ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_CALI_PROCESS, &cali_ctx);
		if (0 == ret) {
			OSCL_LOGI("cutline_dnthr:%d", cali_ctx.cutline_dnthr);
			OSCL_LOGI("cutline_upthr:%d", cali_ctx.cutline_upthr);
			OSCL_LOGI("car_rect.x:%d", cali_ctx.car_rect.x);
			OSCL_LOGI("car_rect.y:%d", cali_ctx.car_rect.y);
			OSCL_LOGI("car_rect.width:%d", cali_ctx.car_rect.width);
			OSCL_LOGI("car_rect.height:%d", cali_ctx.car_rect.height);
		}
	}
	return ret;
}

void pano_exit(void *erecorder)
{
	int ret = 0;
	cali_out_data_t out;

	if (NULL == erecorder)
		return;
#if 1
	ret = lb_recorder_ctrl(erecorder, LB_REC_PANO_GET_CALI_DATA, &out);
	OSCL_LOGE("get cali out data, ret:%d", ret);
	if ((0 == ret) && (out.data)) {
		g_cali_out.data = oscl_zalloc(out.data_size);
		if (NULL == g_cali_out.data) {
			OSCL_LOGE("Malloc fail...");
		} else {
			memcpy(g_cali_out.data, out.data, out.data_size);
			g_cali_out.data_size = out.data_size;
			cali_flag = 1;
		}
		oscl_free(out.data);
	}
#endif
	lb_recorder_ctrl(erecorder, LB_REC_PANO_STOP, NULL);
	lb_recorder_ctrl(erecorder, LB_REC_PANO_RELEASE, NULL);
}

int cb_get_next_file(void *hdl, char *next_file)
{
	int idx = 0;
	int i;

	for (i = 0; i < TEST_MAX_REC_NUM; i++) {
		if (hdl == g_test_para[i].recorder) {
			g_test_para[i].loop++;
			idx = i;
		}
	}

	sprintf(next_file, "%s%s_%04d.mp4", g_test_para[idx].path,
		g_test_para[idx].name_prefix,
		g_test_para[idx].loop%70);
	OSCL_LOGE("get new file : %s\n", next_file);
	return 0;
}
int cb_get_next_jpg(void *hdl, char *next_file)
{
	int idx = 0;
	int i;

	for (i = 0; i < TEST_MAX_REC_NUM; i++) {
		if (hdl == g_test_para[i].recorder) {
			g_test_para[i].loop++;
			idx = i;
		}
	}

	sprintf(next_file, "%s%s_%04d.jpg", g_test_para[idx].path,
		g_test_para[idx].name_prefix, g_test_para[idx].loop%70);
	OSCL_LOGE("new jpeg file : %s\n", next_file);
	return 0;
}

int cb_file_closed(void *hdl, char *file_name)
{
	OSCL_LOGE("close new file hdl:%p, file %s closed", hdl, file_name);
	return 0;
}

void init_recorder(auto_test_para_t *test_para)
{
	void *recorder;
	vsize_t size;
	win_para_t disp_para;

	recorder = test_para->recorder;
	if (recorder == NULL) {
		recorder = lb_recorder_creat();
		test_para->recorder = recorder;
	}
	if (recorder) {
		memset(&size, 0, sizeof(size));
		lb_recorder_ctrl(recorder, LB_REC_SET_VIDEO_SOURCE,
			(void *)test_para->source);

		_get_config_disp_para(&disp_para, test_para->index);
		lb_recorder_ctrl(recorder, LB_REC_SET_DISP_PARA, &disp_para);
		OSCL_LOGE("cur_player:%d  preview rot:%d, enc rot:%d",
			test_para->index,
			test_para->rec_para.rotate,
			test_para->rec_para.enc_rot);
		lb_recorder_ctrl(recorder, LB_REC_SET_PARA,
			&test_para->rec_para);
		lb_recorder_ctrl(recorder, LB_REC_SET_ROTATE,
			(void *)test_para->rec_para.rotate);
		lb_recorder_ctrl(recorder, LB_REC_PREPARE, NULL);
		lb_recorder_ctrl(recorder, LB_REC_SET_OUTPUT_FILE, test_para->filename);
	}
}

static void lbrecorder_exec_cmds(int *cmds, int num_cmd, auto_test_para_t *test_para)
{
	int i;
	int ret = -1;
	void *recorder;
	void *cmd_para;
	int time = 0;
	int status = 0;
	app_frame_cb_t cb;
	static int cb_cnt;

	fix_duration_param_t fix_duration_param;
	if (test_para->recorder == NULL)
		init_recorder(test_para);
	recorder = test_para->recorder;
	if (recorder == NULL) {
		OSCL_LOGE("init recorder(%s) failed!", test_para->source);
		return;
	}

	for (i = 0; (i < num_cmd) && (cmds[i] != -1); i++) {
		OSCL_LOGE("recorder:%s cmd:%s", test_para->source,
			media_cmd_as_string(cmds[i]));
		cmd_para = NULL;
		switch (cmds[i]) {
		case LB_REC_PREPARE:
		case LB_REC_PREVIEW:
		case LB_REC_STOP_PREVIEW:
		case LB_REC_START:
		case LB_REC_STOP:
			break;
		case LB_REC_TAKE_PICTURE:
			cmd_para = test_para->pic_name;
			break;
		case LB_REC_GET_TIME:
			time = 0;
			cmd_para = &time;
			break;
		case LB_REC_GET_STATUS:
			status = 0;
			cmd_para = &status;
			break;
		case LB_REC_SET_FIX_DURATION_PARA:
			fix_duration_param.file_duration = 30;
			fix_duration_param.cb_get_next_file = cb_get_next_file;
			fix_duration_param.cb_file_closed = cb_file_closed;
			cmd_para = &fix_duration_param;
			break;
		case LB_REC_SET_CB_FRAME: {
			cb.app_data = recorder;
			cb.buf_handle = buf_handle;
			cb.type = VS_VIDEO_ROT_FRAME;
			if (cb_cnt++ % 3 == 0)
				cb.buf_handle = NULL;
			if (cb.type == VS_VIDEO_ROT_FRAME) {
				long enc_rot = test_para->rec_para.enc_rot;
				lb_recorder_ctrl(recorder, LB_REC_SET_CB_ROT,
					(void *)enc_rot);
			}
			cmd_para = &cb;
			break;
		}
		case LB_REC_SET_WATERMARK_INDEX: {
			if (test_para->watermark_enable == 0) {
				watermark_test_source(recorder);
				test_para->watermark_enable = 1;
			}
			ret = watermark_test(recorder, test_para->watermark);
			cmds[i] = -1;
			break;
		}
		case LB_REC_SET_OUTPUT_FILE: {
			cmd_para = test_para->filename;
			break;
		}
		case LB_REC_SET_PARA: {
			cmd_para = &test_para->rec_para;
			break;
		}
		default:
			OSCL_LOGE("unknown cmd:%d", cmds[i]);
			break;
		}
		if (cmds[i] != -1) {
			ret = lb_recorder_ctrl(recorder, cmds[i], (void *)cmd_para);
			if (cmds[i] == LB_REC_GET_TIME || cmds[i] == LB_REC_GET_STATUS) {
				OSCL_LOGE("%s:%d", media_cmd_as_string(cmds[i]),
					*(int *)cmd_para);
			}
			OSCL_LOGE("recorder %s:%p cmd:%s, ret:%x", test_para->source,
				recorder, media_cmd_as_string(cmds[i]), ret);
		}
	}
}

#define MAX_TEST_CMDS 10
long lbrecorder(int argc, char **argv)
{
	int c;
	int index;
	int test_cmds[MAX_TEST_CMDS];
	int num_cmd = 0;
	int *pcmd = NULL;
	char **tmp_argv;
	int tmp_argc;
	void *recorder;
	auto_test_para_t *test_para;
	int arg_num;

	if (disp == NULL) {
		disp = oscl_open_disp_engine();
		OSCL_LOGE("disp:%p", disp);
	}

	if (g_test_para[0].source[0] == 0)
		init_test_para();

	tmp_argc = argc;
	tmp_argv = argv;
	test_para = &g_test_para[1];
	if (argc >= 2) {
		if (argv[1][0] == '0') {
			tmp_argc--;
			tmp_argv = &tmp_argv[1];
			test_para = &g_test_para[0];
		} else if (argv[1][0] == '1') {
			tmp_argc--;
			tmp_argv = &tmp_argv[1];
		}
	}
	recorder = test_para->recorder;

	OSCL_LOGI("lbrecorder start............");
	list_mem();

	memset(test_cmds, -1, sizeof(test_cmds));
	OSCL_LOGE("recorder:%p argc:%d, argv:%s", recorder, argc, *argv);
	while ((num_cmd <= MAX_TEST_CMDS)) {
		c = getopt_long(argc, argv, "fFVdPxpvtrsqhcnwemuaio",
			long_options, &index);
		if (c < 0)
			break;
		pcmd = &test_cmds[num_cmd];
		*pcmd = -1;
		OSCL_LOGE("option:%c", c);
		OSCL_LOGE("option:%x, optarg:%s", c, (optarg == NULL) ? "null" : optarg);
		switch (c) {
		case 'H':
		case 'h':
			lbrecorder_help();
			break;
		case 'p': {
			*pcmd = LB_REC_PREPARE;
			break;
		}
		case 'v': {
			*pcmd = LB_REC_PREVIEW;
			break;
		}
		case 't': {
			*pcmd = LB_REC_STOP_PREVIEW;
			break;
		}
		case 'r': {
			*pcmd = LB_REC_START;
			break;
		}
		case 's': {
			*pcmd = LB_REC_STOP;
			break;
		}
		case 'i': {
			*pcmd = LB_REC_TAKE_PICTURE;
			strncpy(test_para->filename, optarg, TEST_FILENAME_LEN - 1);
			break;
		}
		case 'w': {
			test_para->watermark = atoi(optarg);
			*pcmd = LB_REC_SET_WATERMARK_INDEX;
			break;
		}
		case 'x': {
			*pcmd = LB_REC_SET_FIX_DURATION_PARA;
			break;
		}
		case 'c': {
			*pcmd = LB_REC_SET_CB_FRAME;
			break;
		}
		case 'm': {
			*pcmd = LB_REC_GET_TIME;
			break;
		}
		case 'u': {
			*pcmd = LB_REC_GET_STATUS;
			break;
		}
		case 'f':
			strncpy(test_para->filename, optarg, TEST_FILENAME_LEN - 1);
			if (recorder)
				*pcmd = LB_REC_SET_OUTPUT_FILE;
			break;
		case 'F': {
			arg_num = atoi(optarg);
			if (arg_num > REC_OUTPUT_FORMAT_NULL &&
				arg_num < REC_OUTPUT_FORMAT_NONE)
				test_para->rec_para.file_fmt = arg_num;
			*pcmd = LB_REC_SET_PARA;
			break;
		}
		case 'V': {
			arg_num = atoi(optarg);
			if (arg_num > VIDEO_ENCODER_INVALID &&
				arg_num < VIDEO_ENCODER_NONE)
				test_para->rec_para.venc_format = arg_num;
			*pcmd = LB_REC_SET_PARA;
			break;
		}
		case 'd':
			strncpy(test_para->source, optarg, TEST_DEVNAME_LEN - 1);
			break;
		case 'P': {
			arg_num = atoi(optarg);
			if (arg_num == 1080) {
				test_para->rec_para.source_height = 1080;
				test_para->rec_para.source_width = 1920;
				test_para->rec_para.height = 1080;
				test_para->rec_para.width = 1920;
				test_para->rec_para.bitrate = 14000000;
			} else if (arg_num == 720) {
				test_para->rec_para.source_height = 720;
				test_para->rec_para.source_width = 1280;
				test_para->rec_para.height = 720;
				test_para->rec_para.width = 1280;
				test_para->rec_para.bitrate = 6000000;
			} else if (arg_num == 480) {
				test_para->rec_para.source_height = 480;
				test_para->rec_para.source_width = 640;
				test_para->rec_para.height = 480;
				test_para->rec_para.width = 640;
				test_para->rec_para.bitrate = 3000000;
			} else
				OSCL_LOGE("unknown height:%s", optarg);
			*pcmd = LB_REC_SET_PARA;
			break;
		}
		case 'o': {
			recorder_show_components(0, NULL);
			break;
		}
		case 'n': {
			pano_test(recorder);
			break;
		}
		case 'e': {
			pano_exit(recorder);
			break;
		}
		case 'q': {
			lb_recorder_release(recorder);
			oscl_close_disp_engine(disp);
			disp = NULL;
			recorder = NULL;
			test_para->recorder = NULL;
			break;
		}
		case 'a': {
			if (g_test_para[0].recorder)
				lb_recorder_release(g_test_para[0].recorder);
			if (g_test_para[1].recorder)
				lb_recorder_release(g_test_para[1].recorder);
			recorder = NULL;
			g_test_para[0].recorder = NULL;
			g_test_para[1].recorder = NULL;
			oscl_mdelay(500);
			list_mem();
			OSCL_LOGI("lb_recorder_release....");
			auto_test_all();
			break;
		}
		default:
			OSCL_LOGE("Unexcepted case, please let me know, option '%c'",
				optopt);
			break;
		}
		if (*pcmd != -1)
			num_cmd++;
	}
	if (num_cmd)
		lbrecorder_exec_cmds(test_cmds, num_cmd, test_para);
	optind = 0;
	return 0;

}

