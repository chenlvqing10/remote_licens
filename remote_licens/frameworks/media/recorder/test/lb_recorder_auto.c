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

void *open_disp_engine(void)
{
	disp_hdl_t *vdisp = NULL;

	vdisp = oscl_zalloc(sizeof(disp_hdl_t));
	disp = vdisp;
	oscl_param_check((NULL != vdisp), NULL, NULL);
	vdisp->disp_device = rt_device_find("disp");
	if (vdisp->disp_device != NULL) {
		rt_device_open(vdisp->disp_device, 0);
		vdisp->disp_ops =
			(rt_device_disp_ops_t *)(vdisp->disp_device->user_data);
		RT_ASSERT(vdisp->disp_ops != NULL);
	} else {
		oscl_free(vdisp);
		OSCL_LOGE("disp_config err");
		return NULL;
	}
	return vdisp;
}
#else
unsigned int check_mem(const char *func, unsigned int pre_mem, unsigned int count)
{
	return 0;
}
#define osal_dump()
#define list_mem()
#endif

#ifdef __EOS__
void auto_lb_timeout(void *para)
{
	auto_test_para_t *testpara = (auto_test_para_t *)para;
	rt_enter_critical();
	if (testpara->loop == testpara->last_loop) {
		OSCL_LOGE("======timer(%d:%s): line:%d",
			testpara->index, testpara->source, testpara->line);
		if (testpara->auto_thread) {
			OSCL_LOGE("dump test_thread(%s)", testpara->auto_thread->name);
			dump_stack_thread(testpara->auto_thread);
		}
		OSCL_LOGE("\n===list thread===");
		list_thread();
		OSCL_LOGE("\n===list sem===");
		list_sem();
		OSCL_LOGE("\n===list mutex===");
		list_mutex();
		OSCL_LOGE("\n===dump_stack_all===");
		dump_stack_all();
		OSCL_LOGE("\n===list_timer===");
		list_timer();
		OSCL_LOGE("\n===list_mailbox===");
		list_mailbox();
		OSCL_LOGE("\n===list_msgqueue===");
		list_msgqueue();
		OSCL_LOGE("\n===list_event===");
		list_event();
		OSCL_LOGE("\n===list_mempool===");
		list_mempool();
		OSCL_LOGE("\n===omx_show_active===");
		/*
		omx_show_active();
		OSCL_LOGE("\n");
		*/
	}
	testpara->last_loop = testpara->loop;
	rt_exit_critical();
}

void auto_test_starttimer(auto_test_para_t *testpara, int timeout)
{
	if (testpara->timer == NULL) {
		OSCL_LOGE("====creat and start timer %d", testpara->index);
		testpara->timer = rt_timer_create("autolb",
			auto_lb_timeout, testpara,
			timeout * RT_TICK_PER_SECOND,
			RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
		rt_timer_start(testpara->timer);
	}
}

void auto_stop_timer(auto_test_para_t *testpara)
{
	OSCL_LOGE("====stop and del timer %d", testpara->index);
	if (testpara->timer) {
		rt_timer_stop(testpara->timer);
		rt_timer_delete(testpara->timer);
		testpara->timer = NULL;
	}
}
#else
void auto_lb_timeout(void *para)
{
}
void auto_test_starttimer(auto_test_para_t *testpara, int timeout)
{
}
void auto_stop_timer(auto_test_para_t *testpara)
{
}
#endif

static int init_dvr_para()
{
	int i;
	auto_test_para_t *test_para;

	g_test_para[0].name_prefix = "dvr0";
	g_test_para[1].name_prefix = "dvr1";
	g_test_para[2].name_prefix = "dvr2";
	g_test_para[3].name_prefix = "dvr3";
	for (i = 0; i < TEST_MAX_REC_NUM; i++) {
		test_para = &g_test_para[i];
		strncpy(test_para->source, FRONT_INPUT_DEVICE_NAME,
			TEST_DEVNAME_LEN - 1);

		if (i != 1) {
			test_para->preview_flag = 0;
			memcpy(&test_para->rec_para, &g_test_para[1].rec_para,
				sizeof(rec_param_t));
			test_para->path = g_test_para[1].path;
			strncpy(test_para->source, g_test_para[1].source,
				TEST_DEVNAME_LEN - 1);
		}
		test_para->rec_para.height = test_para->rec_para.source_height / 2;
		test_para->rec_para.width = test_para->rec_para.source_width / 2;
		test_para->rec_para.enc_rect.x = (i)%2*test_para->rec_para.width;
		test_para->rec_para.enc_rect.y = (i)/2*test_para->rec_para.height;
		test_para->rec_para.enc_rect.width = test_para->rec_para.width;
		test_para->rec_para.enc_rect.height = test_para->rec_para.height;
		test_para->rec_para.bitrate = 6000000;
	}
	return 0;
}

static void wait_test_finished(int index)
{
	if (g_test_para[index].auto_thread) {
		pthread_join(g_test_para[index].auto_thread, NULL);
		g_test_para[index].auto_thread = 0;
	}
}

static void mdelay_with_flag(int delay_ms, int *stop_flag)
{
	int start_time = 0;
	start_time = oscl_get_msec();
	while ((stop_flag == NULL) || ((*stop_flag) == 0)) {
		oscl_mdelay(100);
		if ((oscl_get_msec() - start_time) >= delay_ms)
			break;
	}
}

void *auto_test_preview(void *param)
{
	void *recorder = NULL;
	int count = 0;
	rec_param_t recorder_para;
	char filename[50] = {0};
	auto_test_para_t *test_para = param;
	unsigned int start_mem = 0, pre_mem = 0;

	auto_test_starttimer(test_para, 30);
	OSCL_LOGI("======start preview test======");
	start_mem = check_mem(__func__, 0, 0);
	memset(&recorder_para, 0, sizeof(rec_param_t));
	recorder = test_para->recorder;
	if (test_para->recorder == NULL) {
		test_para->recorder = lb_recorder_creat();
		recorder = test_para->recorder;
	}
	lb_recorder_ctrl(recorder, LB_REC_SET_VIDEO_SOURCE, test_para->source);
	lb_recorder_ctrl(recorder, LB_REC_SET_PARA, &test_para->rec_para);
	lb_recorder_ctrl(recorder, LB_REC_SET_DISP_PARA, &test_para->disp_para);
	lb_recorder_ctrl(recorder, LB_REC_PREPARE, 0);

	if (test_para->record_flag) {
		cb_get_next_file(recorder, filename);
		lb_recorder_ctrl(recorder, LB_REC_SET_OUTPUT_FILE, filename);
		lb_recorder_ctrl(recorder, LB_REC_START, 0);
		OSCL_LOGI("Start rec....");
	}
	oscl_mdelay(1000);
	while ((!test_para->stop_flag) && (count < test_max_loop_num)) {
		count++;
		test_para->loop++;
		recorder_para.rotate = count%4;
		lb_recorder_ctrl(recorder, LB_REC_PREVIEW, 0);
		mdelay_with_flag(2000, &test_para->stop_flag);
		OSCL_LOGI("LB_REC_STOP_PREVIEW");
		lb_recorder_ctrl(recorder, LB_REC_STOP_PREVIEW, 0);
		OSCL_LOGI("======stop count:%d =========", count);
		OSCL_LOGI("==============rotate:%d w,h:%d-%d=============",
			recorder_para.rotate, recorder_para.source_width,
			recorder_para.source_height);
		pre_mem = check_mem(__func__, pre_mem, count);
		oscl_mdelay(500);
	}
	if (test_para->record_flag)
		lb_recorder_ctrl(recorder, LB_REC_STOP, 0);
	lb_recorder_release(recorder);
	test_para->recorder = NULL;
	recorder = NULL;
	oscl_mdelay(500);
	check_mem(__func__, start_mem, 0);
	OSCL_LOGI("======preview test complete======");
	auto_stop_timer(test_para);
	return 0;
}

void *auto_test_recording(void *param)
{
	void *recorder;
	int count = 0;
	rec_param_t recorder_para;
	char filename[50];
	auto_test_para_t *test_para = param;
	unsigned int start_mem = 0, pre_mem = 0;

	auto_test_starttimer(test_para, 30);
	OSCL_LOGI("======start recording test======");
	start_mem = check_mem(__func__, 0, 0);
	if (test_para->recorder == NULL)
		test_para->recorder = lb_recorder_creat();
	recorder = test_para->recorder;
	memset(&recorder_para, 0, sizeof(rec_param_t));
	lb_recorder_ctrl(recorder, LB_REC_SET_VIDEO_SOURCE, test_para->source);
	lb_recorder_ctrl(recorder, LB_REC_SET_PARA, &test_para->rec_para);
	lb_recorder_ctrl(recorder, LB_REC_SET_DISP_PARA, &test_para->disp_para);
	lb_recorder_ctrl(recorder, LB_REC_PREPARE, 0);
	if (test_para->preview_flag) {
		OSCL_LOGI("Start preview.....");
		lb_recorder_ctrl(recorder, LB_REC_PREVIEW, 0);
	}
	while ((!test_para->stop_flag) && (count < test_max_loop_num)) {
		count++;
		recorder_para.rotate = count%4;
		memset(&recorder_para, 0, sizeof(rec_param_t));
		cb_get_next_file(recorder, filename);
		OSCL_LOGE("======start count===:%d %s=========", count, filename);
/*
		if (count%2) {
			recorder_para.source_height = 640;
			recorder_para.source_width = 480;
		}
*/
		lb_recorder_ctrl(recorder, LB_REC_SET_OUTPUT_FILE, filename);
		lb_recorder_ctrl(recorder, LB_REC_START, 0);
		mdelay_with_flag(2000, &test_para->stop_flag);
		lb_recorder_ctrl(recorder, LB_REC_STOP, 0);
		oscl_mdelay(100);
		pre_mem = check_mem(__func__, pre_mem, count);
		OSCL_LOGE("======stop count:%d %s=========", count, filename);
		OSCL_LOGE("==============rotate:%d w,h:%d-%d=============",
			recorder_para.rotate, recorder_para.source_width,
			recorder_para.source_height);
	}
	if (test_para->preview_flag)
		lb_recorder_ctrl(recorder, LB_REC_STOP_PREVIEW, 0);
	lb_recorder_release(recorder);
	recorder = NULL;
	test_para->recorder = NULL;
	oscl_mdelay(500);
	check_mem(__func__, start_mem, 0);
	OSCL_LOGI("======recording test complete======");
	auto_stop_timer(test_para);

	return 0;
}

void *auto_test_recorder(void *param)
{
	void *recorder;
	char filename[50];
	auto_test_para_t *test_para = param;
	unsigned int start_mem = 0, pre_mem = 0;

	auto_test_starttimer(test_para, 30);
	OSCL_LOGI("======start recorder test======");
	start_mem = check_mem(__func__, 0, 0);
	while ((!test_para->stop_flag) && (test_para->loop < test_max_loop_num)) {
		OSCL_LOGE("\n\n\n======start count===:%d=========", test_para->loop);
		if (test_para->record_flag == 0)
			test_para->loop++;
		if (test_para->recorder == NULL)
			test_para->recorder = lb_recorder_creat();
		recorder = test_para->recorder;
		lb_recorder_ctrl(recorder, LB_REC_SET_VIDEO_SOURCE, test_para->source);
		lb_recorder_ctrl(recorder, LB_REC_SET_PARA, &test_para->rec_para);
		lb_recorder_ctrl(recorder, LB_REC_SET_ROTATE, (void *)1);
		lb_recorder_ctrl(recorder, LB_REC_SET_DISP_PARA, &test_para->disp_para);
		lb_recorder_ctrl(recorder,
				LB_REC_SET_DISP_SCALE_MODE, (void *)SCALE_MODE_OFFLINE);
		lb_recorder_ctrl(recorder, LB_REC_PREPARE, 0);
		OSCL_LOGI("record_flag:%d, preview_flag:%d",
			test_para->record_flag, test_para->preview_flag);
		if (test_para->preview_flag) {
			OSCL_LOGI("Start preview.....");
			lb_recorder_ctrl(recorder, LB_REC_PREVIEW, 0);
		}
		if (test_para->record_flag) {
			cb_get_next_file(recorder, filename);
			lb_recorder_ctrl(recorder, LB_REC_SET_OUTPUT_FILE, filename);
			lb_recorder_ctrl(recorder, LB_REC_START, 0);
			cb_get_next_jpg(recorder, filename);
			oscl_mdelay(1000);
			OSCL_LOGE("take pic:%s", filename);
			lb_recorder_ctrl(recorder, LB_REC_TAKE_PICTURE, filename);
		}

		mdelay_with_flag(2000, &test_para->stop_flag);
		if (test_para->record_flag)
			lb_recorder_ctrl(recorder, LB_REC_STOP, 0);
		if (test_para->preview_flag)
			lb_recorder_ctrl(recorder, LB_REC_STOP_PREVIEW, 0);
		lb_recorder_release(recorder);
		pre_mem = check_mem(__func__, pre_mem, test_para->loop);
		OSCL_LOGE("======stop count:%d %s=========", test_para->loop, filename);
		OSCL_LOGE("=====preview rot:%d, enc rot:%d, w,h:%d-%d======",
			test_para->rec_para.rotate, test_para->rec_para.enc_rot,
			test_para->rec_para.source_width,
			test_para->rec_para.source_height);
		recorder = NULL;
		test_para->recorder = NULL;
	}
	oscl_mdelay(500);
	check_mem(__func__, start_mem, 0);
	OSCL_LOGI("======recorder complete======");
	auto_stop_timer(test_para);

	return 0;
}

void *auto_test_fix_duration(void *param)
{
	void *recorder;
	int count = 0;
	char filename[50];
	auto_test_para_t *test_para = param;
	fix_duration_param_t fix_duration_param;
	unsigned int start_mem = 0;

	auto_test_starttimer(test_para, 90);
	OSCL_LOGI("======start fix duration test======");
	oscl_mdelay(500);
	start_mem = check_mem(__func__, 0, 0);
	if (test_para->recorder == NULL)
		test_para->recorder = lb_recorder_creat();
	recorder = test_para->recorder;
	lb_recorder_ctrl(recorder, LB_REC_SET_VIDEO_SOURCE, test_para->source);
	lb_recorder_ctrl(recorder, LB_REC_SET_PARA, &test_para->rec_para);
	lb_recorder_ctrl(recorder, LB_REC_SET_DISP_PARA, &test_para->disp_para);
	lb_recorder_ctrl(recorder, LB_REC_PREPARE, 0);
	if (test_para->preview_flag)
		lb_recorder_ctrl(recorder, LB_REC_PREVIEW, 0);
	cb_get_next_file(recorder, filename);
	OSCL_LOGE("======start count===:%d %s=========", count, filename);
	lb_recorder_ctrl(recorder, LB_REC_SET_OUTPUT_FILE, filename);
	lb_recorder_ctrl(recorder, LB_REC_START, 0);
	fix_duration_param.file_duration = 60;
	fix_duration_param.cb_get_next_file = cb_get_next_file;
	fix_duration_param.cb_file_closed = cb_file_closed;
	lb_recorder_ctrl(recorder, LB_REC_SET_FIX_DURATION_PARA, &fix_duration_param);

	while ((test_para->loop < test_max_loop_num) && (!test_para->stop_flag))
		mdelay_with_flag(fix_duration_param.file_duration, &test_para->stop_flag);

	lb_recorder_ctrl(recorder, LB_REC_STOP, 0);
	if (test_para->preview_flag)
		lb_recorder_ctrl(recorder, LB_REC_STOP_PREVIEW, 0);
	lb_recorder_release(recorder);
	oscl_mdelay(500);
	check_mem(__func__, start_mem, 0);
	auto_stop_timer(test_para);
	OSCL_LOGI("======fix duration complete======");
	recorder = NULL;
	test_para->recorder = NULL;
	test_para->loop = 0;

	return 0;
}

void *auto_test_pano(void *param)
{
	void *recorder[2] = {NULL, NULL};
	int count = 0;
	char filename[50] = {0};
	auto_test_para_t *test_para = param;
	unsigned int start_mem = 0, pre_mem = 0;

	auto_test_starttimer(test_para, 30);
	OSCL_LOGI("======start pano test======");
	oscl_mdelay(500);
	start_mem = check_mem(__func__, 0, 0);
	/* 1. create recorder */
	recorder[0] = lb_recorder_creat();

	/* 2. set video input source */
	lb_recorder_ctrl(recorder[0], LB_REC_SET_VIDEO_SOURCE, BACK_INPUT_DEVICE_NAME);

	/* 3. set rotate angle */
	lb_recorder_ctrl(recorder[0], LB_REC_SET_ROTATE, (void *)1);

	/* 4. recorder prepare */
	lb_recorder_ctrl(recorder[0], LB_REC_PREPARE, NULL);
	memset(&g_cali_out, 0, sizeof(g_cali_out));
	if (1 == test_para->record_flag) {
		snprintf(filename, sizeof(filename), VIC_PATH"test_pano_0.mp4");
		lb_recorder_ctrl(recorder[0], LB_REC_SET_OUTPUT_FILE, filename);
		lb_recorder_ctrl(recorder[0], LB_REC_START, 0);
		OSCL_LOGI("Start rec1....");
	} else if (2 == test_para->record_flag) {
		snprintf(filename, sizeof(filename), VIC_PATH"test_pano_0.mp4");
		lb_recorder_ctrl(recorder[0], LB_REC_SET_OUTPUT_FILE, filename);
		lb_recorder_ctrl(recorder[0], LB_REC_START, 0);
		OSCL_LOGI("Start rec1....");

		recorder[1] = lb_recorder_creat();
		lb_recorder_ctrl(recorder[1], LB_REC_SET_VIDEO_SOURCE,
				 FRONT_INPUT_DEVICE_NAME);
		lb_recorder_ctrl(recorder[1], LB_REC_SET_ROTATE, (void *)1);
		lb_recorder_ctrl(recorder[1], LB_REC_PREPARE, NULL);
		snprintf(filename, sizeof(filename), ISP_PATH"test_pano_1.mp4");
		lb_recorder_ctrl(recorder[1], LB_REC_SET_OUTPUT_FILE, filename);
		lb_recorder_ctrl(recorder[1], LB_REC_START, 0);
		OSCL_LOGI("Start rec2....");
	}
	oscl_mdelay(1000);
	while ((!test_para->stop_flag) && (count < test_max_loop_num)) {
		count++;
		test_para->loop++;

		/* 5. create pano test */
		pano_test(recorder[0]);
		oscl_mdelay(2000);

		/* 6. release pano test */
		pano_exit(recorder[0]);
		oscl_mdelay(500);
		pre_mem = check_mem(__func__, pre_mem, count);
		OSCL_LOGI("\n======stop count:%d=========\n", count);
	}
	if (test_para->record_flag) {
		lb_recorder_ctrl(recorder[0], LB_REC_STOP, 0);
		if (recorder[1])
			lb_recorder_ctrl(recorder[1], LB_REC_STOP, 0);
	}
	if (recorder[0])
		lb_recorder_release(recorder[0]);
	if (recorder[1])
		lb_recorder_release(recorder[1]);
	recorder[0] = NULL;
	recorder[1] = NULL;
	if (g_cali_out.data)
		oscl_free(g_cali_out.data);
	memset(&g_cali_out, 0, sizeof(g_cali_out));
	cali_flag = 0;
	oscl_mdelay(500);
	check_mem(__func__, start_mem, 0);
	auto_stop_timer(test_para);
	OSCL_LOGI("======pano test complete======");

	return 0;
}

void *auto_test_pic(void *param)
{
	void *recorder;
	int count = 0;
	rec_param_t recorder_para;
	char filename[50];
	auto_test_para_t *test_para = param;
	unsigned int start_mem = 0, pre_mem = 0;

	auto_test_starttimer(test_para, 30);
	OSCL_LOGI("======start take pic test======");
	start_mem = check_mem(__func__, 0, 0);
	if (test_para->recorder == NULL)
		test_para->recorder = lb_recorder_creat();
	recorder = test_para->recorder;
	memset(&recorder_para, 0, sizeof(rec_param_t));
	lb_recorder_ctrl(recorder, LB_REC_SET_VIDEO_SOURCE, test_para->source);
	lb_recorder_ctrl(recorder, LB_REC_SET_PARA, &test_para->rec_para);
	lb_recorder_ctrl(recorder, LB_REC_SET_DISP_PARA, &test_para->disp_para);
	lb_recorder_ctrl(recorder, LB_REC_SET_ROTATE, (void *)test_para->rec_para.rotate);
	lb_recorder_ctrl(recorder, LB_REC_PREPARE, 0);
	while ((!test_para->stop_flag) && (count < test_max_loop_num)) {
		if (count == test_max_loop_num / 3) {
			OSCL_LOGE("Start preview.....");
			lb_recorder_ctrl(recorder, LB_REC_PREVIEW, 0);
			test_para->preview_flag = 1;
		}
		if (count == test_max_loop_num / 3) {
			OSCL_LOGE("Start rec.....");
			cb_get_next_file(recorder, filename);
			lb_recorder_ctrl(recorder, LB_REC_SET_OUTPUT_FILE, filename);
			lb_recorder_ctrl(recorder, LB_REC_START, 0);
			test_para->record_flag = 1;
		}
		count++;
		recorder_para.rotate = count%4;
		memset(&recorder_para, 0, sizeof(rec_param_t));
		sprintf(filename, "%s%s_%08d.jpg", test_para->path,
			test_para->name_prefix, count);

		OSCL_LOGE("======start count===:%d %s=========", count, filename);
		lb_recorder_ctrl(recorder, LB_REC_TAKE_PICTURE, filename);
		pre_mem = check_mem(__func__, pre_mem, count);
		OSCL_LOGE("======stop count:%d %s=========", count, filename);
		OSCL_LOGE("==============rotate:%d w,h:%d-%d=============",
			recorder_para.rotate, recorder_para.source_width,
			recorder_para.source_height);
	}
	if (test_para->preview_flag)
		lb_recorder_ctrl(recorder, LB_REC_STOP_PREVIEW, 0);
	if (test_para->record_flag)
		lb_recorder_ctrl(recorder, LB_REC_STOP, 0);
	lb_recorder_release(recorder);
	recorder = NULL;
	test_para->recorder = NULL;
	oscl_mdelay(500);
	check_mem(__func__, start_mem, 0);
	OSCL_LOGI("======recording test complete======");
	auto_stop_timer(test_para);

	return 0;
}

void *auto_test_drm(void *param)
{
	void *disp_hdl = NULL;
	void *disp_layer = NULL;
	unsigned char *buffer[8];
	OMX_S32 i = 0;
	OMX_S32 w, h;
	OMX_U32 size[3];
	OMX_S32 align = 4 * 1024;
	OMX_S32 count = 0;
	layer_data_t data;

	h = 1080;
	w = 1920;
	system("mount -t debugfs none /tmp/");
	calc_frame_size(OMX_COLOR_FormatYUV420SemiPlanar, w, h, size, align);

	while (1) {
		OSCL_LOGI("test drm use memory. %d, %d %d %d",
				count++, size[0], size[1], size[2]);
		for (i = 0; i < 8; i++) {
			buffer[i] = oscl_malloc_align(size[0] + size[1] + size[2], align);
			if (buffer[i] == NULL) {
				OSCL_LOGE("malloc fail.");
				goto EXIT;
			}
		}
		disp_hdl = oscl_open_disp_engine();
		if (disp_hdl == NULL) {
			OSCL_LOGE("oscl_open_disp_engine fail.");
			return NULL;
		}
		disp_layer = oscl_disp_request_layer(disp_hdl);
		if (disp_layer)	{
			for (i = 0; i < 8; i++) {
				memset(&data, 0, sizeof(data));
				data.win_rect.left = 0;
				data.win_rect.top = 0;
				data.win_rect.width = 1280;
				data.win_rect.height = 720;

				data.src_size.height = h;
				data.src_size.width = w;

				data.crop_rect.width = w;
				data.crop_rect.height = h;
				data.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
				data.size[0] = size[0];
				data.size[1] = size[1];
				data.size[2] = size[2];
				data.addr[0] = buffer[i];
				if (size[1])
					data.addr[1] =	buffer[i] + size[0];
				if (size[2]) {
					data.addr[2] =
						buffer[i] + size[0] + size[1];
				}
				oscl_disp_update_layer_data(disp_hdl,
							   disp_layer,
							   &data);
			}
			oscl_disp_release_layer(disp_hdl, &disp_layer);
		} else
			OSCL_LOGE("oscl_disp_request_layer fail.");
		oscl_close_disp_engine(disp_hdl);
		for (i = 0; i < 8; i++) {
			if (buffer[i] != NULL)
				oscl_free_align(buffer[i]);
		}
		system("cat /tmp/ion/heaps/carvout");
		oscl_mdelay(5000);
	}

EXIT:
	for (i = 0; i < 8; i++) {
		if (buffer[i])
			oscl_free_align(buffer[i]);
	}
	return NULL;
}

void *auto_test_thread(void *param)
{
	auto_test_para_t *test_para = param;
	void *retval = NULL;
	OSCL_LOGE("test %d command:%s", test_para->index, test_para->cmd_line);
	OSCL_LOGE("test file saved path:%s, prefix:%s, source dev:%s, outfmt:%d",
		test_para->path, test_para->name_prefix, test_para->source,
		test_para->rec_para.file_fmt);
	OSCL_LOGE("test para, preview flag:%d, recorder flag:%d",
		test_para->preview_flag, test_para->record_flag);
	OSCL_LOGE("source para, frame rate:%d, size(w-h): %d-%d",
		test_para->rec_para.frame_rate,
		test_para->rec_para.source_width, test_para->rec_para.source_height);
	OSCL_LOGE("display para, rotate:%d, mode:%d, rect: %d-%d-%d-%d",
		test_para->rec_para.rotate, test_para->disp_para.mode,
		test_para->disp_para.rect.x, test_para->disp_para.rect.y,
		test_para->disp_para.rect.width, test_para->disp_para.rect.height);
	OSCL_LOGE("audio para, channels:%d, sample_width:%d, sample rate:%d, enc fmt:%d",
		test_para->rec_para.audio_channels,
		test_para->rec_para.audio_sample_width,
		test_para->rec_para.audio_sample_rate,
		test_para->rec_para.aenc_format);
	OSCL_LOGE("venc para, rot:%d, fmt:%d, rect:%d-%d-%d-%d, size:%d-%d, bitrate:%d",
		test_para->rec_para.enc_rot,
		test_para->rec_para.venc_format,
		test_para->rec_para.enc_rect.x, test_para->rec_para.enc_rect.y,
		test_para->rec_para.enc_rect.width, test_para->rec_para.enc_rect.height,
		test_para->rec_para.width, test_para->rec_para.height,
		test_para->rec_para.bitrate);
	if (test_para) {
		prctl(PR_SET_NAME, test_para->cmd_line);
		if (test_para->test_func)
			retval = test_para->test_func(param);
	}
	return retval;
}


long auto_lb(int argc, char **argv)
{
	int index = 0;
	int total = 1;
	int i;
	auto_test_para_t tmp_para;
	pthread_attr_t thread_attr;
	void *(*test_func)(void *) = NULL;
	int _argv_int[20];

	if (g_test_para[0].source[0] == 0)
		init_test_para();
	if (disp == NULL)
		disp = oscl_open_disp_engine();

	if (argc >= 2) {
		if (argv[1][0] == '1')
			index = 1;
		if (argv[1][0] == 'a') {
			index = 0;
			total = 2;
		}
	}

	memset(&tmp_para, 0, sizeof(tmp_para));
	memset(_argv_int, 0, sizeof(_argv_int));
	for (i = 3; i < argc; i++)
		_argv_int[i] = atoi(argv[i]);

	if (argc >= 3 && !strncmp(argv[2], "stop", 4)) {
		if (total > 1)
			total = TEST_MAX_REC_NUM - index;
		for (i = index; i < index + total; i++)
			g_test_para[i].stop_flag = 1;
		for (i = index; i < index + total; i++)
			wait_test_finished(i);
		return 0;
	} else if (argc >= 4 && !strncmp(argv[2], "setcnt", 4)) {
		test_max_loop_num = _argv_int[3];
		return 0;
	} else if (argc >= 4 && !strncmp(argv[2], "preview", 4)) {
		test_func = auto_test_preview;
		tmp_para.record_flag = _argv_int[3];
	} else if (argc >= 4 && !strncmp(argv[2], "rec", 4)) {
		test_func = auto_test_recording;
		tmp_para.preview_flag = _argv_int[3];
	} else if (argc >= 5 && !strncmp(argv[2], "recorder", 4)) {
		test_func = auto_test_recorder;
		tmp_para.preview_flag = _argv_int[3];
		tmp_para.record_flag = _argv_int[4];
	} else if (argc >= 3 && !strncmp(argv[2], "dvr", 3)) {
		index = 0;
		total = 3;
		test_func = auto_test_recorder;
		tmp_para.preview_flag = _argv_int[3];
		tmp_para.record_flag = _argv_int[4];
	} else if (argc >= 4 && !strncmp(argv[2], "pano", 4)) {
		test_func = auto_test_pano;
		tmp_para.record_flag = _argv_int[3];
	} else if (argc >= 3 && !strncmp(argv[2], "fixtime", 4)) {
		test_func = auto_test_fix_duration;
		tmp_para.preview_flag = _argv_int[3];
	} else if (argc >= 3 && !strncmp(argv[2], "pic", 4)) {
		test_func = auto_test_pic;
	} else if (argc >= 3 && !strncmp(argv[2], "drm", 4)) {
		test_func = auto_test_drm;
	} else {
		OSCL_LOGE("Input para error.");
		return -1;
	}

	for (i = index; i < index + total; i++) {
		g_test_para[i].preview_flag = tmp_para.preview_flag;
		g_test_para[i].record_flag = tmp_para.record_flag;

		g_test_para[i].cmd_line[0] = '0' + i;
		strncpy(&g_test_para[i].cmd_line[1],  argv[2], 15);
		g_test_para[i].cmd_line[15] = 0;
		_get_config_disp_para(&g_test_para[i].disp_para, i);

		OSCL_LOGE("creat test thread!");
		g_test_para[i].stop_flag = 0;
		pthread_attr_init(&thread_attr);
		pthread_attr_setstacksize(&thread_attr, 0x2000);
		g_test_para[i].test_func = test_func;
		pthread_create(&g_test_para[i].auto_thread, &thread_attr,
			       auto_test_thread, &g_test_para[i]);
	}
	if (!strncmp(argv[2], "dvr", 3))
		init_dvr_para();

	return 0;
}

test_cmd_t rec_cmd[] = {
	{
		"auto_lb 0 preview 0",
		4,
		{"auto_lb", "0", "preview", "0"},
	},
	{
		"auto_lb 0 preview 1",
		4,
		{"auto_lb", "0", "preview", "1"},
	},
	{
		"auto_lb 0 rec 0",
		4,
		{"auto_lb", "0", "rec", "0"},
	},
	{
		"auto_lb 0 rec 1",
		4,
		{"auto_lb", "0", "rec", "1"},
	},
	{
		"auto_lb 0 recorder 0 0",
		5,
		{"auto_lb", "0", "recorder", "0", "0"},
	},
	{
		"auto_lb 0 recorder 0 1",
		5,
		{"auto_lb", "0", "recorder", "0", "1"},
	},
	{
		"auto_lb 0 recorder 1 1",
		5,
		{"auto_lb", "0", "recorder", "1", "1"},
	},
	{
		"auto_lb 0 fixtime",
		3,
		{"auto_lb", "0", "fixtime"},
	},
	{
		"auto_lb a preview 0",
		4,
		{"auto_lb", "a", "preview", "0"},
	},
	{
		"auto_lb a preview 1",
		4,
		{"auto_lb", "a", "preview", "1"},
	},
	{
		"auto_lb a rec 0",
		4,
		{"auto_lb", "a", "rec", "0"},
	},
	{
		"auto_lb a rec 1",
		4,
		{"auto_lb", "a", "rec", "1"},
	},
	{
		"auto_lb a recorder 0 0",
		5,
		{"auto_lb", "a", "recorder", "0", "0"},
	},
	{
		"auto_lb a recorder 0 1",
		5,
		{"auto_lb", "a", "recorder", "0", "1"},
	},
	{
		"auto_lb a recorder 1 1",
		5,
		{"auto_lb", "a", "recorder", "1", "1"},
	},
	{
		"auto_lb a fixtime",
		3,
		{"auto_lb", "a", "fixtime"},
	},
/*
	{
		"auto_lb 0 pano 1",
		4,
		{"auto_lb", "0", "pano", "1"},
	},
	{
		"auto_lb 0 pano 2",
		4,
		{"auto_lb", "0", "pano", "2"},
	}
*/
};

void auto_test_all(void)
{
	int argc;
	int i = 0, j = 0;
	int cmd_num = ARRAY_SIZE(rec_cmd);
	char *argv[FINSH_ARG_MAX];
	unsigned int start_mem = 0;

	start_mem = check_mem(__func__, 0, 0);
	memset(argv, 0, sizeof(argv));
	for (i = 0; i < FINSH_ARG_MAX; i++) {
		argv[i] = oscl_zalloc(MAX_ARGS_LEN);
		OSCL_LOGI("argv[%d]:%p", i, argv[i]);
		if (NULL == argv[i]) {
			OSCL_LOGE("Malloc fail.");
			return;
		}
	}
	test_max_loop_num = 3;
	for (i = 0; i < cmd_num; i++) {
		printf("\n*****************************************************\n");
		printf("Test command(%d) [%s]\n", i, rec_cmd[i].str);
		list_mem();
		printf("*****************************************************\n");
		argc = rec_cmd[i].argc;
		for (j = 0; j < argc; j++)
			strncpy(argv[j], rec_cmd[i].argv[j], MAX_ARGS_LEN);
		auto_lb(argc, argv);
		for (j = 0; j < TEST_MAX_REC_NUM; j++)
			wait_test_finished(j);
	}
	for (i = 0; i < FINSH_ARG_MAX; i++) {
		if (argv[i])
			oscl_free(argv[i]);
	}
	check_mem(__func__, start_mem, 0);
}


