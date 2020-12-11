/*
 * omxrec_test.c - Testcase for recorder.
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

#define DBG_LEVEL		DBG_VERBOSE

#include <oscl.h>
#include <fcntl.h>
#include <getopt.h>
#include "dirent.h"
#include "oscl_display.h"
#include "media_common.h"
#include "video_stream.h"
#include "audio_stream.h"
#include "video_recorder.h"

#define TEST_FILENAME_LEN 128
#define TEST_DEFAULT_LOOP_CNT 1000
#define TEST_DEFAULT_LOOP_TIME 10000	/* ms */

#ifdef __linux__
#define TEST_ROOT_PATH "/mnt/card/video"
#define TEST_VIDEO_DEV0 "/dev/video3"
#define TEST_VIDEO_DEV0_WIDTH	1920
#define TEST_VIDEO_DEV0_HEIGHT	1080
#define TEST_VIDEO_DEV1 "/dev/video1"
#define TEST_VIDEO_DEV1_WIDTH	800
#define TEST_VIDEO_DEV1_HEIGHT	600
#elif defined __EOS__
#define TEST_ROOT_PATH "/mnt/sdcard/video"
#define TEST_VIDEO_DEV0 "isp"
#define TEST_VIDEO_DEV0_WIDTH	1920
#define TEST_VIDEO_DEV0_HEIGHT	1080
#define TEST_VIDEO_DEV1 "vic"
#define TEST_VIDEO_DEV1_WIDTH	1280
#define TEST_VIDEO_DEV1_HEIGHT	720
#endif

#define _FLAG_OFF 0
#define _FLAG_ON 1
#define _FLAG_ONOFF 2

typedef struct omxrec_test_cfg {
	int index;
	pthread_mutex_t lock;
	int init_flag;

	int start_flag;
	int stop_flag;
	int audio_disable;
	int video_disable;
	int preview_onoff;
	int record_onoff;
	int watermark_onoff;
	int collision_onoff;
	int collision_time;
	int loop_rec;

	int loop_total;
	int loop_time;
	char *source;
	char *recpath;

	vsrc_param_t camera_para;
	int preview_rot;
	win_para_t preview_para;
	enc_para_t enc_para;
	win_rect_t enc_rect;
	int a_encfmt;
	int a_channels;
	int a_sample_width;
	int a_sample_rate;

	void *vstream;
	void *astream;
	void *recorder;
	pthread_t test_thread;

	int loop_cnt;
	int colli_cnt;
} omxrec_test_cfg_t;

#define MAX_REC_NUM 4
omxrec_test_cfg_t omxrec[MAX_REC_NUM];

static void _get_disp_para(win_para_t *disp_para, int index)
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
		disp_para->rect.x += (index % 2) * size.width;
	} else {
		size.height /= 2;
		disp_para->rect.y += (index % 2) * size.height;
	}
	disp_para->mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
	disp_para->rect.width = size.width;
	disp_para->rect.height = size.height;

}

int init_omx_cfg(int index)
{
	omxrec_test_cfg_t *omxrec_cfg;
	DIR *dir = NULL;
	int ret = 0;

	if (index < 0 || index > MAX_REC_NUM)
		return -1;
	omxrec_cfg = &omxrec[index];
	if (omxrec_cfg->init_flag != 0)
		return 0;

	memset(omxrec_cfg, 0, sizeof(*omxrec_cfg));
	pthread_mutex_init(&omxrec_cfg->lock, NULL);
	omxrec_cfg->init_flag = 1;
	omxrec_cfg->index = index;

	omxrec_cfg->preview_onoff = _FLAG_ON;
	omxrec_cfg->record_onoff = _FLAG_ON;
	omxrec_cfg->collision_onoff = _FLAG_OFF;
	omxrec_cfg->loop_total = 1000;
	omxrec_cfg->loop_time = 10000;

	if (index%2 == 0) {
		omxrec_cfg->source = oscl_strdup(TEST_VIDEO_DEV0);
		omxrec_cfg->camera_para.width = TEST_VIDEO_DEV0_WIDTH;
		omxrec_cfg->camera_para.height = TEST_VIDEO_DEV0_HEIGHT;
	}
	if (index%2 == 1) {
		omxrec_cfg->source = oscl_strdup(TEST_VIDEO_DEV1);
		omxrec_cfg->camera_para.width = TEST_VIDEO_DEV1_WIDTH;
		omxrec_cfg->camera_para.height = TEST_VIDEO_DEV1_HEIGHT;
	}

	_get_disp_para(&omxrec_cfg->preview_para, index);

	omxrec_cfg->enc_para.width = omxrec_cfg->camera_para.width;
	omxrec_cfg->enc_para.height = omxrec_cfg->camera_para.height;
	omxrec_cfg->enc_para.bitrate = 16000000;
/*
	if (omxrec_cfg->enc_para.width == 1920)
		omxrec_cfg->enc_para.bitrate = 16000000;
	else
		omxrec_cfg->enc_para.bitrate = 8000000;
*/
	omxrec_cfg->recpath = oscl_strdup(TEST_ROOT_PATH);
	dir = opendir(omxrec_cfg->recpath);
	if (dir == NULL)
		mkdir(omxrec_cfg->recpath, 0);
	else
		closedir(dir);

	return ret;
}

int deinit_omx_cfg(int index)
{
	omxrec_test_cfg_t *omxrec_cfg;

	if (index < 0 || index > MAX_REC_NUM)
		return -1;
	omxrec_cfg = &omxrec[index];
	if (omxrec_cfg->recpath) {
		oscl_free(omxrec_cfg->recpath);
		omxrec_cfg->recpath = NULL;
	}
	if (omxrec_cfg->source) {
		oscl_free(omxrec_cfg->source);
		omxrec_cfg->source = NULL;
	}
	pthread_mutex_destroy(&omxrec_cfg->lock);
	memset(omxrec_cfg, 0, sizeof(*omxrec_cfg));
	return 0;
}
#define REC_CFG_SET_INT(src, dst, type) \
do {\
	if (src->type != -1) \
		dst->type = src->type; \
} while (0)

#define REC_CFG_DUP_STR(src, dst, str) \
do {\
	if (src->str != NULL) { \
		if (dst->str) \
			oscl_free(dst->str); \
		dst->str = oscl_strdup(src->str); \
		oscl_free(src->str); \
		src->str = NULL; \
	} \
} while (0)

#define REC_CFG_PRINT_INT(cfg, type) \
	OSCL_LOGE("%s: %d", #type, cfg->type)

static void _print_recorder_cfg(omxrec_test_cfg_t *rec_cfg, int line)
{
	OSCL_LOGE("_print_recorder_cfg: %d", line);
	REC_CFG_PRINT_INT(rec_cfg, index);
	REC_CFG_PRINT_INT(rec_cfg, init_flag);
	REC_CFG_PRINT_INT(rec_cfg, start_flag);
	REC_CFG_PRINT_INT(rec_cfg, stop_flag);
	REC_CFG_PRINT_INT(rec_cfg, preview_onoff);
	REC_CFG_PRINT_INT(rec_cfg, record_onoff);
	REC_CFG_PRINT_INT(rec_cfg, watermark_onoff);
	REC_CFG_PRINT_INT(rec_cfg, collision_onoff);
	REC_CFG_PRINT_INT(rec_cfg, collision_time);
	REC_CFG_PRINT_INT(rec_cfg, loop_rec);
	REC_CFG_PRINT_INT(rec_cfg, loop_cnt);
	REC_CFG_PRINT_INT(rec_cfg, loop_total);
	REC_CFG_PRINT_INT(rec_cfg, loop_time);
	OSCL_LOGE("source: %s", rec_cfg->source);
	OSCL_LOGE("recpath: %s", rec_cfg->recpath);
}
static int _set_recorder_cfg(omxrec_test_cfg_t *rec_cfg, omxrec_test_cfg_t *cmd_cfg)
{
	if (rec_cfg->init_flag == 0)
		init_omx_cfg(rec_cfg->index);
	pthread_mutex_lock(&rec_cfg->lock);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, start_flag);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, stop_flag);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, preview_onoff);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, record_onoff);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, watermark_onoff);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, collision_onoff);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, collision_time);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, loop_rec);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, loop_cnt);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, loop_total);
	REC_CFG_SET_INT(cmd_cfg, rec_cfg, loop_time);

	REC_CFG_DUP_STR(cmd_cfg, rec_cfg, source);
	REC_CFG_DUP_STR(cmd_cfg, rec_cfg, recpath);
	pthread_mutex_unlock(&rec_cfg->lock);

	return 0;
}

int _psr_dev_idx(char *arg, int *dev_index)
{
	int i;
	int ndev = 0;
	if (arg == NULL || dev_index == NULL)
		return -1;
	dev_index[0] = -1;
	if (arg)
		OSCL_TRACE("%s", arg);
	if (arg[0] == 'a') {
		OSCL_LOGE("dev all");
		for (i = 0; i < MAX_REC_NUM; i++)
			dev_index[i] = i;
		return MAX_REC_NUM;
	}
	for (i = 0; (i < 256) && (ndev < MAX_REC_NUM); i++) {
		OSCL_TRACE("%c", arg[i]);
		if (arg[i] >= '0' && arg[i] <= '9') {
			dev_index[ndev] = arg[i] - '0';
			ndev++;
		}
		if (arg[i] == 0)
			break;
	}
	return ndev;
}

static void omxrec_help(void)
{
	#define arg_format "  %-20s %s%s"

	OSCL_PRINT("usage: omxrec [argument]");
	OSCL_PRINT("arguments:");
	OSCL_PRINT(arg_format, "--audio_disable", "disable audio stream", "");
	OSCL_PRINT(arg_format, "--collision [time]", "collision onoff", "collison cache");
	OSCL_PRINT(arg_format, "--dev devname", "choise device id to config(run)", "");
	OSCL_PRINT(arg_format, "--fixtime", "set recyle recorder time limit", "");
	OSCL_PRINT(arg_format, "--looprec", "set test loop time", "");
	OSCL_PRINT(arg_format, "--looptotal", "set test loop count", "");
	OSCL_PRINT(arg_format, "--preview", "preview onoff enable", "");
	OSCL_PRINT(arg_format, "--rec", "rec onoff enable", "");
	OSCL_PRINT(arg_format, "--start", "start test thread", "");
	OSCL_PRINT(arg_format, "--source", "set test source dev", "");
	OSCL_PRINT(arg_format, "--stop", "stop test thread", "");
	OSCL_PRINT(arg_format, "--time", "set loop time of one test case", "");
	OSCL_PRINT(arg_format, "--watermark", "watermark setting", "");
	OSCL_PRINT(arg_format, "--help", "show this help info", "");
}

int get_rec_config(int argc, char **argv, omxrec_test_cfg_t *test_cfg)
{
	int opt;
	int i;
	int index;
	int cnt;
	int dev_idx[MAX_REC_NUM];
	omxrec_test_cfg_t cmd_cfg;
	static const char *omx_rec_opts = "hacd:f:l:L:prsS:tT:w";

	struct option omx_rec_long_options[] = {
		{"help", 0, 0, 'h'},
		{"audio_disable", 0, 0, 'a'},
		{"collision", 1, 0, 'c'},
		{"dev", 1, 0, 'd'},
		{"fixtime", 1, 0, 'f'},
		{"looprec", 1, 0, 'l'},
		{"looptotal", 1, 0, 'L'},
		{"preview", 0, 0, 'p'},
		{"rec", 0, 0, 'r'},
		{"start", 0, 0, 's'},
		{"source", 1, 0, 'S'},
		{"stop", 0, 0, 't'},
		{"time", 1, 0, 'T'},
		{"watermark", 0, 0, 'w'},
		{0, 0, 0, 0}
	};

	OSCL_TRACE("");
	memset(&cmd_cfg, -1, sizeof(omxrec_test_cfg_t));
	memset(&dev_idx, -1, sizeof(dev_idx));
	cmd_cfg.source = NULL;
	cmd_cfg.recpath = NULL;
	cmd_cfg.recorder = NULL;

	while (1) {
		opt = getopt_long(argc, argv, omx_rec_opts, omx_rec_long_options, &index);
		if (opt == -1) {
			OSCL_LOGE("get args end, exit!\n");
			optind = 0;
			break;
		}
		OSCL_LOGI("option:%d", opt);
		OSCL_LOGI("option:%c, optarg:%s", opt, (optarg == NULL) ? "nul" : optarg);
		switch (opt) {
		case 'h':
			omxrec_help();
			break;
		case 'a':
			cmd_cfg.audio_disable = 1;
			break;
		case 'c':
			cmd_cfg.collision_onoff = 1;
			if (optarg != NULL)
				cmd_cfg.collision_time = atoi(optarg);
			break;
		case 'd':
			_psr_dev_idx(optarg, dev_idx);
			break;
		case 'l':
			if (optarg != NULL)
				cmd_cfg.loop_rec = atoi(optarg);
			break;
		case 'L':
			if (optarg != NULL)
				cmd_cfg.loop_total = atoi(optarg);
			break;
		case 'p':
			cmd_cfg.preview_onoff = 1;
			break;
		case 'r':
			cmd_cfg.record_onoff = 1;
			break;
		case 's':
			cmd_cfg.start_flag = 1;
			break;
		case 'S':
			if (optarg != NULL)
				cmd_cfg.source = oscl_strdup(optarg);
			break;
		case 't':
			cmd_cfg.stop_flag = 1;
			break;
		case 'T':
			if (optarg != NULL)
				cmd_cfg.loop_time = atoi(optarg);
			break;
		case 'w':
			cmd_cfg.watermark_onoff = 1;
			break;
		default:
			break;
		}
	}
	OSCL_TRACE("");
	cnt = 0;
	for (i = 0; i < MAX_REC_NUM; i++) {
		index = dev_idx[i];
		if (index >= 0 && index < MAX_REC_NUM) {
			OSCL_TRACE("");
			cnt++;
			test_cfg[index].index = index;
			_set_recorder_cfg(&test_cfg[index], &cmd_cfg);
		}
	}
	if (cnt == 0) {
		OSCL_TRACE("");
		test_cfg[0].index = 0;
		_set_recorder_cfg(&test_cfg[0], &cmd_cfg);
	}
	if (cmd_cfg.start_flag != 1 && cmd_cfg.stop_flag != 1)
		_print_recorder_cfg(&cmd_cfg, __LINE__);
	OSCL_TRACE("");
	return 0;
}

void *disp;

int _fallocate(char *filename, int size)
{
	int fd = -1;
	int ret;
	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, 0666);
		if (fd < 0)
			return 0;
		ret = posix_fallocate(fd, 0, size);
		if (ret)
			OSCL_LOGE("fallocate failed:%d, %s!", ret, strerror(errno));
	}
	close(fd);
	return 0;

}

int _get_filename(void *hdl, char *next_file)
{
	omxrec_test_cfg_t *cfg = hdl;
	sprintf(next_file, "%s/dev%d_%04d.mp4", cfg->recpath,
		cfg->index, cfg->loop_cnt%70);
	cfg->loop_cnt++;

#if 0
	_fallocate(next_file, 125*1024*1024);
#endif
	return 0;
}
int _get_colli_filename(omxrec_test_cfg_t *cfg, char *next_file)
{
	sprintf(next_file, "%s/dev%d_%04d_co.mp4", cfg->recpath,
		cfg->index, cfg->colli_cnt%70);
	cfg->colli_cnt++;
#if 0
	_fallocate(next_file, 25*1024*1024);
#endif

	return 0;
}
int _file_closed(void *hdl, char *file_name)
{
	OSCL_LOGE("close hdl:%p, file %s closed", hdl, file_name);
	return 0;
}

static int _init_vstream(omxrec_test_cfg_t *cfg)
{
	int ret = 0;

	if (cfg->video_disable == 1 && cfg->preview_onoff == _FLAG_OFF)
		return 0;

	if (cfg->vstream == NULL)
		cfg->vstream = video_stream_create();
	oscl_param_check_exit(cfg->vstream != NULL, -1, NULL);

	if (cfg->source)
		ret = video_stream_set_camera_source(cfg->vstream, cfg->source);
	oscl_param_check_exit(ret == 0, -1, NULL);
	ret = video_stream_set_src_para(cfg->vstream, &cfg->camera_para);
	oscl_param_check_exit(ret == 0, -1, NULL);
	ret = video_stream_set_disp_para(cfg->vstream,
		&cfg->preview_para, cfg->preview_rot);
EXIT:
	return ret;
}

static int _init_astream(omxrec_test_cfg_t *cfg)
{
	int ret = 0;

	if (cfg->audio_disable == 1)
		return 0;

	oscl_param_check_exit(cfg->astream == NULL, -1, NULL);
	cfg->astream = audio_stream_creat();
	oscl_param_check_exit(cfg->astream != NULL, -1, NULL);

	if (cfg->a_encfmt)
		ret = audio_stream_set_coding_type(cfg->astream, cfg->a_encfmt);
	if (cfg->a_channels)
		ret = audio_stream_set_channels(cfg->astream, cfg->a_channels);
	if (cfg->a_sample_width) {
		ret = audio_stream_set_sample_width(cfg->astream,
			cfg->a_sample_width);
	}
	if (cfg->a_sample_rate) {
		ret = audio_stream_set_sample_rate(cfg->astream,
			cfg->a_sample_rate);
	}
EXIT:
	return ret;
}

static int _init_recorder(omxrec_test_cfg_t *cfg)
{
	int ret = 0;

	if (cfg->record_onoff == _FLAG_OFF)
		return 0;
	cfg->recorder = video_rec_creat();
	oscl_param_check_exit(cfg->recorder != NULL, -1, NULL);
	if (cfg->vstream)
		video_rec_set_video_stream(cfg->recorder, cfg->vstream);
	if (cfg->astream)
		video_rec_set_audio_stream(cfg->recorder, cfg->astream);

	ret = video_rec_set_para(cfg->recorder, &cfg->enc_para);
	oscl_param_check_exit(ret == 0, -1, NULL);
	ret = video_rec_set_enc_area(cfg->recorder, &cfg->enc_rect);
	oscl_param_check_exit((ret == 0), -1, NULL);
EXIT:
	return ret;
}

static void mdelay_with_exitflag(int delay_ms, int *stop_flag)
{
	int start_time = 0;
	start_time = oscl_get_msec();
	while ((stop_flag == NULL) || ((*stop_flag) == 0)) {
		oscl_mdelay(100);
		if ((oscl_get_msec() - start_time) >= delay_ms)
			break;
	}
}

void *recorder_test_thread(void *param)
{
	int ret = 0;
	char filename[50];
	omxrec_test_cfg_t *cfg = param;
	oscl_param_check(param != NULL, NULL, NULL);

	_print_recorder_cfg(cfg, __LINE__);
	ret = _init_vstream(cfg);
	oscl_param_check_exit((ret == 0), -1, NULL);
	ret = _init_astream(cfg);
	oscl_param_check_exit((ret == 0), -1, NULL);
	ret = _init_recorder(cfg);
	oscl_param_check_exit((ret == 0), -1, NULL);

	if (cfg->preview_onoff == _FLAG_ON) {
		OSCL_LOGI("====dev %d start preview =", cfg->index);
		video_stream_set_layer_level(cfg->vstream, VIDEO_LAYER_TOP);
		ret = video_stream_start_preview(cfg->vstream);
		OSCL_LOGI("====dev %d start preview =", cfg->index);
	}
	oscl_param_check_exit((ret == 0), -1, NULL);
	if (cfg->record_onoff == _FLAG_ON) {
		fix_duration_param_t fix_duartion;
		OSCL_LOGI("====video_rec_collision_enable,%p =", cfg);
		fix_duartion.file_duration = 30;
		fix_duartion.cb_get_next_file = _get_filename;
		fix_duartion.cb_file_closed = _file_closed;
		video_rec_set_fix_duration_param(cfg->recorder, cfg, &fix_duartion);

		oscl_param_check_exit((ret == 0), -1, NULL);
		OSCL_LOGI("=dev %d start rec:%s =", cfg->index, filename);
		oscl_param_check_exit((ret == 0), -1, NULL);
		ret = video_rec_start(cfg->recorder);
		oscl_param_check_exit((ret == 0), -1, NULL);
	}

	while ((cfg->stop_flag == 0) && (cfg->loop_cnt < cfg->loop_total)) {
		OSCL_LOGI("====dev %d start test:%d =======", cfg->index, cfg->loop_cnt);
		if (cfg->preview_onoff == _FLAG_ONOFF) {
			OSCL_LOGI("====dev %d start preview =", cfg->index);
			ret = video_stream_start_preview(cfg->vstream);
			video_stream_set_layer_level(cfg->vstream, VIDEO_LAYER_TOP);
		}
		oscl_param_check_exit((ret == 0), -1, NULL);
		if (cfg->record_onoff == _FLAG_ONOFF) {
			OSCL_LOGI("====video_rec_collision_enable =");
			ret = _get_filename(cfg, filename);
			oscl_param_check_exit((ret == 0), -1, NULL);
			OSCL_LOGI("=dev %d start rec:%s =", cfg->index, filename);
			ret = video_rec_set_output_file(cfg->recorder, filename);
			oscl_param_check_exit((ret == 0), -1, NULL);
			ret = video_rec_start(cfg->recorder);
			oscl_param_check_exit((ret == 0), -1, NULL);
		}

		mdelay_with_exitflag(cfg->loop_time, &cfg->stop_flag);

		if (cfg->record_onoff == _FLAG_ONOFF) {
			ret = video_rec_stop(cfg->recorder);
			oscl_param_check_exit((ret == 0), -1, NULL);
		}
		if (cfg->preview_onoff == _FLAG_ONOFF)
			ret = video_stream_stop_preview(cfg->vstream);
		OSCL_LOGI("====dev %d start test:%d =======", cfg->index, cfg->loop_cnt);
	}


EXIT:
	if (cfg->recorder) {
		video_rec_stop(cfg->recorder);
		video_rec_release(cfg->recorder);
		cfg->recorder = NULL;
	}
	if (cfg->astream) {
		audio_stream_release(cfg->astream);
		cfg->astream = NULL;
	}
	if (cfg->vstream) {
		video_stream_stop_preview(cfg->vstream);
		video_stream_release(cfg->vstream);
		cfg->vstream = NULL;
	}
	if (cfg->stop_flag == 0) {
		pthread_detach(cfg->test_thread);
		cfg->test_thread = 0;
	}
	return NULL;
}

static int _create_recorder_test_thread(omxrec_test_cfg_t *cfg)
{
	int ret = 0;
	pthread_attr_t thread_attr;

	pthread_attr_init(&thread_attr);
	pthread_attr_setstacksize(&thread_attr, 0x2000);
	ret = pthread_create(&cfg->test_thread, &thread_attr,
		       recorder_test_thread, cfg);
	if (ret != 0) {
		OSCL_LOGE("create thread %d failed", cfg->index);
		cfg->test_thread = 0;
	}
	return ret;
}

int omx_recorder_test(int argc, char **argv)
{
	int ret = 0;
	int i;

	OSCL_TRACE("");
	if (disp == NULL) {
		disp = oscl_open_disp_engine();
		if (disp == NULL)
			return -1;
	}
	get_rec_config(argc, argv, omxrec);

	for (i = 0; i < MAX_REC_NUM; i++) {
		if (omxrec[i].start_flag == 1 && omxrec[i].test_thread == 0) {
			OSCL_LOGE("start test stream %d", i);
			omxrec[i].stop_flag = 0;
			omxrec[i].start_flag = 0;
			ret = _create_recorder_test_thread(&omxrec[i]);
		}
		if (omxrec[i].stop_flag == 1) {
			OSCL_LOGE("stop test stream %d", i);
			if (omxrec[i].test_thread)
				pthread_join(omxrec[i].test_thread, NULL);
			omxrec[i].test_thread = 0;
			omxrec[i].stop_flag = 0;
		}
	}
	return ret;
}

int media_test(void *dbg_param, int argc, char **argv)
{
	OSCL_TRACE("");
	omx_recorder_test(argc, argv);
	return 0;
}


