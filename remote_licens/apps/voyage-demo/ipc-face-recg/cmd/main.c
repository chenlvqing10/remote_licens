/*
 * main.c
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <log/log.h>
#include <signal.h>

#include "system_time.h"
#include "record_export.h"
#include "time_conv.h"
#include "config.h"
#include "db.h"
#include "app_lang.h"
#include "person_register.h"
#include "elang.h"
#include "net_wired.h"
#include "db_person_info.h"
#include "live_video.h"
#include "live_server.h"
#include "face_svc.h"
#include "media_common.h"
#include "yuv_blending.h"
#include "person_import.h"
#include "hotplug.h"
#include "http_main.h"
#include "onvif_server.h"
#include "ip_config.h"
#include "db_device_net_param.h"

#define MAX_DET_PERSON 6
static yuv_blending_t g_blending_det[MAX_DET_PERSON];
#define FACE_VIDEO_WIDTH 1280
#define FACE_VIDEO_HEIGHT 720

#define ONVIF_SERVER 1
#define RESTFUL_SERVER 0

/* #define USE_FACE_RGB_HDL_FOR_LIVE_VIDEO 1 */

int system_init(void)
{
	int ret;

	ret = system_time_set_init_value();
	if (ret)
		ALOGE("init system time failed");

	ret = net_wired_init(NULL);
	if (ret) {
		ALOGE("init wired device failed");
	}

#if RESTFUL_SERVER
	ret = http_main_start();
	if (ret)
		ALOGE("http_main_start failed");
#endif

#if	ONVIF_SERVER
	ret = onvif_nvt_start_service();
	if (ret)
		ALOGE("onvif_nvt_start_service failed");
#endif
	return 0;
}

void signal_term_handler(int num)
{
	printf("signal_term_handler");
	ALOGD("signal_term_handler");

	face_deinit();
}

void *import_result_thread_func(void *args)
{
	person_import_ctx_t ctx;
	int i = 0;

	for (i = 0; i < 100; i++) {
		memset(&ctx, 0, sizeof(person_import_ctx_t));
		if (person_import_get_ctx(&ctx) == 0) {
			if (ctx.done != EE_THREAD_RUNNING) {
				if (ctx.result_code == PERSON_IMPORT_OK
					&& ctx.person_reg_result_code == PERSON_REG_OK)
					printf("%s\n",
						elang_str("TEXT_PERSONAL_IMPORT_SUCCESSED"));
				else
					printf("%s\n",
						elang_str("TEXT_PERSONAL_IMPORT_FAILED"));

				break;
			}
		}

		sleep(1);
	}

	return NULL;
}

void *export_result_thread_func(void *args)
{
	record_export_ctx_t ctx;
	int i = 0;

	for (i = 0; i < 100; i++) {
		memset(&ctx, 0, sizeof(record_export_ctx_t));
		if (record_export_get_ctx(&ctx) == 0) {
			if (ctx.done != EE_THREAD_RUNNING) {
				if (ctx.result_code == RECORD_EXPORT_OK)
					printf("%s\n", elang_str("TEXT_REC_EXPORT_END"));
				else
					printf("%s\n",
						elang_str("TEXT_REC_EXPORT_FAILED"));

				break;
			}
		}

		sleep(1);
	}

	return NULL;
}

static void live_video_show_name_img(int index, frame_t *pframe, face_t *face)
{
	int x_start = pframe->info.video.width
			* face->rect_smooth[0] / FACE_VIDEO_WIDTH;
	int y_start = pframe->info.video.height
			* face->rect_smooth[1] / FACE_VIDEO_HEIGHT;
	int x_end = pframe->info.video.width
			* face->rect_smooth[2] / FACE_VIDEO_WIDTH;

	g_blending_det[index].source.buffer_y = pframe->info.video.addr[0];
	g_blending_det[index].source.buffer_uv = pframe->info.video.addr[1];
	g_blending_det[index].blending_x_pos = x_start
			+ (x_end - x_start - g_blending_det[index].blending_width) / 2;
	g_blending_det[index].blending_y_pos = y_start;
	g_blending_det[index].source.width_stride = pframe->info.video.width;
	g_blending_det[index].source.height_aligned = pframe->info.video.height;

	blending_operation(&g_blending_det[index]);
}

static int blending_handle(face_t *face, frame_t *pframe)
{
	float threshold = face_recg_threshold();
	if (face->score > threshold) {
		if (strcmp(face->name, "A") == 0)
			live_video_show_name_img(0, pframe, face);
		else if (strcmp(face->name, "B") == 0)
			live_video_show_name_img(1, pframe, face);
		else if (strcmp(face->name, "C") == 0)
			live_video_show_name_img(2, pframe, face);
		else if (strcmp(face->name, "D") == 0)
			live_video_show_name_img(3, pframe, face);
		else if (strcmp(face->name, "E") == 0)
			live_video_show_name_img(4, pframe, face);
		else if (strcmp(face->name, "F") == 0)
			live_video_show_name_img(5, pframe, face);
		else
			printf("name is %s\n", face->name);
	}

	return 0;
}

static void blending_init()
{
	for (int i = 0; i < MAX_DET_PERSON; i++) {
		FILE *fp_blending;

		char filename[128];

		sprintf(filename, "/usr/share/ipc-face-recg/res/img/%d.rgb", i + 1);

		fp_blending = fopen(filename, "rb");
		if (fp_blending == NULL) {
			printf("open file error\n");
			continue;
		}

		g_blending_det[i].colorspace = RGBA8888;
		g_blending_det[i].text_color = unknow_text;
		g_blending_det[i].blending_width = 64;
		g_blending_det[i].blending_height = 64;
		g_blending_det[i].blending_stride = g_blending_det[i].blending_width
				* 4;
		unsigned int blending_size = 4 * g_blending_det[i].blending_width
				* g_blending_det[i].blending_height;
		g_blending_det[i].blending_data = (unsigned char *) malloc(
				blending_size);
		fread(g_blending_det[i].blending_data, 1, blending_size, fp_blending);

		fclose(fp_blending);
	}

}
static void blending_deinit()
{
	for (int i = 0; i < MAX_DET_PERSON; i++) {
		if (g_blending_det[i].blending_data != NULL) {
			free(g_blending_det[i].blending_data);
			g_blending_det[i].blending_data = NULL;
		}
	}
}

void app_main_storage_dev_send_status_cb(int event, unsigned int para)
{
	if (event == HOTPLUG_TYPE_UDISK) {
		if (para == HOTPLUG_STATE_DISCONNECT)
			live_video_stop_main_rec();
		else if (para == HOTPLUG_STATE_CONNECT)
			live_video_start_main_rec();
	}
}

static int usb_hotplug_init(void)
{
	int ret;

	/* initialize hotplug manager */
	ret = hotplug_mgr_init();
	if (ret) {
		ALOGE("hotplug mgr init failed");
		return -1;
	}

	/* register device change callback */
	ret = hotplug_register_change_cb(
		(hotplug_change_cb)app_main_storage_dev_send_status_cb);
	if (ret) {
		ALOGE("udisk_register_change_cb failed");
		return -1;
	}

	return 0;
}

void app_main_net_send_status_cb(int event, unsigned int para)
{
	if (para == NET_WIRED_EVENT) {
		if (event != eth_dev_dhcp_success && event != eth_dev_link_up) {
			device_net_param_t net_info;
			net_config_para_t net_para;

			/*zeroing global paramters*/
			memset(&net_info, 0x00, sizeof(net_info));
			memset(&net_para, 0x00, sizeof(net_para));
			if (!db_device_net_param_query(&net_info)) {
				return;
			}

			memcpy(net_para.eth_ip, net_info.eth_ip, 32);
			memcpy(net_para.eth_mask, net_info.eth_mask, 32);
			memcpy(net_para.eth_gateway, net_info.eth_gateway, 32);
			memcpy(net_para.eth_dns, net_info.eth_dns, 32);
			net_wired_set_ip_para(&net_para);
		}
	}
}

int main(int argc, char **argv)
{
	int ret;

	/* register sigterm handler */
	signal(SIGTERM, signal_term_handler);

	/* Initialize config */
	ret = config_init();
	if (ret) {
		ALOGE("config init failed");
		return -1;
	}

	/* Check and initialize database */
	ret = db_init("ipc-face-recg");
	if (ret) {
		ALOGE("db init failed");
		return -1;
	}

	ret = lang_init();
	if (ret) {
		ALOGE("lang init failed\n");
		return -1;
	}

	/* Initialize net, time, */
	ret = system_init();
	if (ret) {
		ALOGE("system init failed");
		return ret;
	}

	net_wired_register_event_cb((net_wired_cb)app_main_net_send_status_cb);

#ifdef USE_FACE_RGB_HDL_FOR_LIVE_VIDEO
	/* Initialize, config, start face service */
	face_svc_set_conf_file_path("/etc/ipc_face_svc.conf");
	ret = face_init();
	if (ret) {
		ALOGE("face init failed");
		return -1;
	}

	ret = face_set_cb(live_video_face_cb);
	if (ret) {
		ALOGE("set face cb failed");
		return -1;
	}

	void *rgb_hdl = face_rgb_hdl(get_face_handle());
	ret = live_video_init(rgb_hdl);
	if (ret)
		ALOGE("Init live video failed");
#else
	ret = live_video_init(NULL);
	if (ret)
		ALOGE("Init live video failed");

	/* Initialize, config, start face service */
	face_svc_set_conf_file_path("/etc/ipc_face_svc.conf");
	ret = face_init();
	if (ret) {
		ALOGE("face init failed");
		return -1;
	}

	ret = face_set_cb(live_video_face_cb);
	blending_init();

	if (ret) {
		ALOGE("set face cb failed");
		return -1;
	}
#endif

	live_video_set_blending_callback(blending_handle);

	ret = rtsp_server_init();
	if (ret)
		ALOGE("init rtsp server failed");

	ret = usb_hotplug_init();
	if (ret) {
		ALOGE("hotplug mgr init failed");
		return -1;
	}

	/*waitting message and deal message and loop*/
	while (1) {
		char cmd[256];

		printf("\n>");
		fgets(cmd, 256, stdin);

		printf("Command is %s\n", cmd);
		if (strncmp(cmd, "delete", 6) == 0) {
			char name[64];
			printf("\nname>");
			fgets(name, sizeof(name), stdin);

			printf("name is %s\n", name);
			int len = strlen(name);
			if (len > 0 && name[len - 1] == '\n') {
				name[len - 1] = 0;
				len--;
			}

			if (len <= 0) {
				printf(
					"Input cannot be empty,please input correctly(eg: name or all)!\n");
				continue;
			} else if (strncmp(name, "all", 3) == 0) {
				if (db_person_info_delete_all() == 0) {
					face_clear_face_list();
					face_db_update();
					printf("Successfully deleted all registrants!\n");
				} else {
					printf("Deleting registered personnel failed!\n");
				}
			} else {
				ee_queue_t head;
				int count = 0;

				count = db_person_info_query_begin_by_name(name, &head);
				db_person_info_query_end(&head);
				if (count <= 0) {
					printf("%s\n",
						elang_str("TEXT_PERSON_REC_HAVE_NOT"));
					continue;
				}

				if (db_person_info_delete(name) == 0) {
					face_clear_face_list_by_name(name);
					face_db_update();
					printf("Successfully deleted %s!\n", name);
				} else {
					printf("Deleting %s failed!\n", name);
				}
			}
		} else if (strncmp(cmd, "add", 3) == 0) {
			char name[64];
			printf("\nname>");
			fgets(name, sizeof(name), stdin);

			printf("name is %s\n", name);
			int len = strlen(name);
			if (len > 1) {
				if (name[len - 1] == '\n')
					name[len - 1] = 0;
				app_management_reg_start(name);
			} else
				printf("%s\n", elang_str("TEXT_EEROR_INPUT_NAME_TIP"));

		} else if (strncmp(cmd, "exit", 4) == 0) {
			break;
		} else if (strncmp(cmd, "import", 6) == 0) {
			person_import_thread();

			pthread_t import_result_thread;
			pthread_create(&import_result_thread, NULL,
				import_result_thread_func, NULL);
			pthread_detach(import_result_thread);
		} else if (strncmp(cmd, "export", 6) == 0) {
			export_query_condition_t export_p;
			time_t t;
			time_get_current_sec(&t);

			printf("\nname>");
			fgets(export_p.user_name, sizeof(export_p.user_name), stdin);

			printf("name is %s\n", export_p.user_name);
			int len = strlen(export_p.user_name);
			if (export_p.user_name[len - 1] == '\n')
				export_p.user_name[len - 1] = 0;

			export_p.rec_pass = 1;
			export_p.rec_start_time = 0;
			export_p.rec_end_time = (double)t * 1000;

			record_export_thread(&export_p);

			pthread_t result_thread;
			pthread_create(&result_thread, NULL, export_result_thread_func,
				NULL);
			pthread_detach(result_thread);
		} else {
			system(cmd);
		}
	}

	rtsp_server_deinit();

	live_video_deinit();

	blending_deinit();

	/* deinit eth device */
	net_wired_deinit();
	lang_deinit();

	/* deinit face service */
	face_deinit();

	config_deinit();

	return 0;
}

