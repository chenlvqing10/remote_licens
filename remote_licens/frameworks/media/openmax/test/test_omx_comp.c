/*
 * omx_core.c - Standard functionality for the openmax core API.
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
#define LOG_TAG			"omx_test"

#include <oscl.h>
#include <base_component.h>
#include <lb_omx_core.h>
#include "omx_test.h"
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

omx_component_tbl_t omx_reader_component = {
	"OMX.LB.SOURCE.FILE",
	1,
	file_reader_component_init, NULL, NULL,
	{
		"source.filereader",
		NULL
	}
};

omx_component_tbl_t omx_writer_component = {
	"OMX.LB.SINK.FILE",
	1,
	file_writer_component_init, NULL, NULL,
	{
		"sink.filewriter",
		NULL
	}
};

omx_component_tbl_t omx_base_component = {
	"OMX.LB.FILTER",
	1,
	filter_component_init, NULL, NULL,
	{
		"filter",
		NULL
	}
};


void creat_input_file(char *name)
{
	char *tmp_buf = (char *)oscl_malloc(1024 * 1024);
	int i;
	int fd;
	if (tmp_buf == 0)
		return;
	fd = open(name, O_WRONLY | O_CREAT);
	if (fd < 0) {
		oscl_free((void *)tmp_buf);
		return;
	}
	OSCL_LOGE("here");

	for (i = 0; i < 1 * 1024 * 1024; i++)
		tmp_buf[i] = rand();
	OSCL_LOGE("%d %d %d %d", tmp_buf[0], tmp_buf[1], tmp_buf[2], tmp_buf[3]);
	write(fd, tmp_buf, 1 * 1024 * 1024);
	for (i = 0; i < 783; i++)
		tmp_buf[i] = rand();
	write(fd, tmp_buf, 783);
	close(fd);
	oscl_free((void *)tmp_buf);
}

int compare_in_out(char *in_name, char *out_name)
{
	int infile, outfile;
	int i;
	char *in_buf;
	char *out_buf;
	int inlen, outlen;
	int ret = -1;
	int curlen;
	infile = outfile = -1;
	in_buf = (char *)oscl_zalloc(16 * 1024);
	out_buf = (char *)oscl_zalloc(16 * 1024);
	oscl_param_check_exit(((in_buf != NULL && out_buf != NULL)), -1, NULL);

	infile = open(in_name, O_RDONLY);
	outfile = open(out_name, O_RDONLY);
	oscl_param_check_exit(((infile >= 0 && outfile >= 0)), -1, NULL);
	inlen = outlen = 1 * 1024;
	ret = 0;
	curlen = 0;
	while (inlen != 0 && outlen != 0) {
		inlen = read(infile, in_buf, inlen);
		outlen = read(outfile, out_buf, outlen);
		if (inlen != outlen) {
			ret = -1;
			OSCL_LOGE("inlen:%d outlen %d", inlen, outlen);
			break;
		}
		curlen += inlen;
		if (inlen != 0)
			ret = memcmp(in_buf, out_buf, inlen);
		if (ret) {
			OSCL_LOGE("in_buf:%p out_buf %p, ret:%d", in_buf, out_buf, ret);
			OSCL_LOGE("inlen:%d curlen %d, ret:%d", inlen, curlen, ret);
			OSCL_LOGE("%d %d %d %d", out_buf[0], out_buf[1],
				out_buf[2], out_buf[3]);
			for (i = 0; i <  inlen; i++) {
				if (in_buf[i] != out_buf[i])
					OSCL_LOGE("index(%d):%d, %d\n",
						curlen - inlen + i,
						in_buf[i], out_buf[i]);
			}
			break;
		}
	}

EXIT:
	OSCL_LOGE("infile:%d outfile %d", infile, outfile);
	OSCL_LOGE("in_buf:%p out_buf %p", in_buf, out_buf);
	if (in_buf != NULL)
		oscl_free((void *)in_buf);
	if (out_buf != NULL)
		oscl_free((void *)out_buf);
	if (infile != -1)
		close(infile);
	if (outfile != -1)
		close(outfile);
	OSCL_LOGE("inputfile is %s to outputfile", (ret == 0) ? "equal" : "not equal");
	return ret;
}

int get_port_index(OMX_COMPONENTTYPE *comp, OMX_DIRTYPE dir,
	OMX_PORTDOMAINTYPE domain, int start)
{
	int i;
	int ret = -1;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;
	port_def.nVersion.nVersion = OMX_VERSION;
	for (i = start; i < OMX_PORT_NUMBER_SUPPORTED; i++) {
		port_def.nPortIndex = i;
		ret = OMX_GetParameter(comp, OMX_IndexParamPortDefinition, &port_def);
		if (ret == OMX_ErrorNone && port_def.eDir == dir
			&& port_def.eDomain == domain) {
			ret = i;
			break;
		}
	}
	OSCL_LOGE("index:%d", i);
	if (i == OMX_PORT_NUMBER_SUPPORTED)
		OSCL_LOGE("can not get port, dir:%d, domain:%d, start:%d",
			dir, domain, start);
	return ret;
}

long openmax_test_init()
{
	OMX_Init();

	omx_component_register(&omx_reader_component);
	omx_component_register(&omx_writer_component);
	omx_component_register(&omx_base_component);
	return 0;
}

void omx_show_active()
{
	OMX_HANDLETYPE *active_comp;
	int i;

	active_comp = omx_active_handle_get();
	for (i = 0; i < OMX_COMP_MAX_INST; i++) {
		if (active_comp[i] != NULL)
			base_show_component(active_comp[i]);
	}
	omx_active_handle_put();
}
void *wdg_thread(void *param)
{
	int i;
	int *exit_flag = param;
	OMX_HANDLETYPE *active_comp;
	lb_omx_component_t *component = NULL;
	int log_flag = 0;
	int count = 0;

	pthread_detach(pthread_self());
	while (1) {
		OSCL_LOGE("set flag");
		log_flag = 0;
		active_comp = omx_active_handle_get();
		for (i = 0; i < OMX_COMP_MAX_INST; i++) {
			if (active_comp[i] != NULL) {
				component = get_lb_component(active_comp[i]);
				component->dbg_wdog = 1;
			}
		}

		omx_active_handle_put();
		count = 100;
		while (count--) {
			usleep(10000);
			if (*exit_flag == 1)
				break;
		}

		if (*exit_flag == 1)
			break;

		active_comp = omx_active_handle_get();
		for (i = 0; i < OMX_COMP_MAX_INST; i++) {
			if (active_comp[i] != NULL) {
				component = get_lb_component(active_comp[i]);
				if (component->dbg_wdog == 1) {
					OSCL_LOGE("%s watchdog flag = 1",
						get_component_name(component));
					log_flag = 1;
					break;
				}
			}
		}
		omx_active_handle_put();

		if (log_flag == 1) {
			omx_show_active();
			break;
		}
	}

	pthread_exit(NULL);

	return NULL;
}

int wdg_thread_exit;

static void omx_test_help(void)
{
	#define arg_format  "  %-20s %s%s\n"

	OSCL_PRINT("usage: omxtest [argument]\n");
	OSCL_PRINT("arguments:\n");
	OSCL_PRINT(arg_format, "--rotate", "rotate component test", "");
	OSCL_PRINT(arg_format, "--vrec-vrend", "vrec and vrend component test", "");
	OSCL_PRINT(arg_format, "--untunnel", "untunel mode test", "");
	OSCL_PRINT(arg_format, "--tunnel", "tunnel mode test", "");
}

static struct option long_options[] = {
	{"help", 0, 0, 'h'},
	{"rotate", 0, 0, 'r'},
	{"vrec-vrend", 0, 0, 'v'},
	{"untunnel", 0, 0, 'u'},
	{"tunnel", 0, 0, 't'},
};

int main(int argc, char **argv)
{
	int c;
	int index;

#if 0
	int ret = -1;
	int cmd = -1;
	pthread_t wdg_handler_thread;
	wdg_thread_exit = 0;
	pthread_create(&wdg_handler_thread, NULL,
			wdg_thread, &wdg_thread_exit);
#endif

#if 0
	creat_input_file(TEST_IN_FILE);
#endif
	openmax_test_init();

	while ((c = getopt_long(argc, argv,
			"hrvut", long_options, &index)) != -1) {
		OSCL_LOGI("option:%c", c);
		OSCL_LOGI("option:%x, optarg:%s", c, (optarg == NULL) ? "null" : optarg);
		switch (c) {
		case 'h':
			omx_test_help();
			break;
		case 'r':
			openmax_test_rot_untunnel();
			break;
		case 'v':
			openmax_test_vrec_vrender_untunnel();
			break;
		case 'u':
			openmax_test_untunel();
			compare_in_out(TEST_IN_FILE, TEST_OUT_FILE);
			break;
		case 't':
			openmax_test_tunnel();
			compare_in_out(TEST_IN_FILE, TEST_OUT_FILE);
			break;
		default:
			OSCL_LOGE("Unexcepted case, please let me know, option '%c'",
				optopt);
			break;
		}
	}

	wdg_thread_exit = 1;
	return 0;
}

