/*
 * test.c
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <execinfo.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <dirent.h>

#define LOG_TAG "gpio"
#include <log/log.h>

#include "face_svc_conf.h"
#include "face_svc.h"
#include "jpeg/jpeg_enc.h"
#include "cut.h"
#include "flip.h"
#include "recg_img.h"
#include "time_conv.h"
#include "file_ops.h"
#include "zbar_svc.h"
#include "rgb2nir.h"

#define p1

#ifdef p1
#define RGB_WIDTH 800
#define RGB_HEIGHT 1280
#define NIR_WIDTH 600
#define NIR_HEIGHT 800
#endif

#ifdef p2
#define RGB_WIDTH 1280
#define RGB_HEIGHT 720
#define NIR_WIDTH 800
#define NIR_HEIGHT 600
#endif


void *svc_hdl;

#define STAR "**************************************\n"
#define CLEAN_ERASE_H "stty erase ^H"
#define CLEAN_SCREEN  "/bin/dd if=/dev/zero of=/dev/fb0 count=1024 bs=3600"

#define GPIO_EXPORT	"/sys/class/gpio/export"
#define GPIO_UNEXPORT	"/sys/class/gpio/unexport"
#define GPIO_DIRECTION	"/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE	"/sys/class/gpio/gpio%d/value"
#define GPIO_DIR_OUT	"out"
#define GPIO_DIR_IN	"in"

#define MAX_ARGC_CNT 32
#define MAX_ARGV_LEN 256

typedef void (*test_func)(void);

typedef struct _face_svc_api_s {
	const char *name;
	test_func func;
} face_svc_api_t;

static int test_register_feat;
static float test_norm[2];
static feat_t test_feat;

int gpio_set_output_level(int gpio, int level)
{
	char str[64];
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	/* set gpio direction */
	snprintf(str, sizeof(str), GPIO_DIRECTION, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	fprintf(p, "%s", GPIO_DIR_OUT);
	fclose(p);

	/* set gpio value */
	snprintf(str, sizeof(str), GPIO_VALUE, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	if (level)
		fprintf(p, "%d", 1);
	else
		fprintf(p, "%d", 0);
	fclose(p);

	/* unexport gpio*/
	p = fopen(GPIO_UNEXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_UNEXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	return 0;
}

int gpio_set_direction(int gpio, int direction)
{
	char str[64];
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	/* set gpio direction */
	snprintf(str, sizeof(str), GPIO_DIRECTION, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	if (direction)
		fprintf(p, "%s", GPIO_DIR_IN);
	else
		fprintf(p, "%s", GPIO_DIR_OUT);
	fclose(p);

	/* unexport gpio*/
	p = fopen(GPIO_UNEXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_UNEXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	return 0;
}

int gpio_get_value(int gpio, int *value)
{
	char str[64];
	char value_str[3];
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	/* set gpio direction */
	snprintf(str, sizeof(str), GPIO_DIRECTION, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	fprintf(p, "%s", GPIO_DIR_IN);
	fclose(p);

	/* get gpio value */
	snprintf(str, sizeof(str), GPIO_VALUE, gpio);
	p = fopen(str, "r");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	if (fread(value_str, 3, 1, p) < 0) {
		ALOGE("Failed to read value!\n");
		return -1;
	}
	fclose(p);

	/* unexport gpio*/
	p = fopen(GPIO_UNEXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_UNEXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);
	*value = atoi(value_str);
	return 0;
}

int gpio_set_level(int gpio, int level)
{
	char str[64];
	FILE *p = NULL;

	/* export gpio for led conctrol */
	p = fopen(GPIO_EXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_EXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	/* set gpio value */
	snprintf(str, sizeof(str), GPIO_VALUE, gpio);
	p = fopen(str, "w");
	if (!p) {
		ALOGE("open %s failed", str);
		return -1;
	}
	if (level)
		fprintf(p, "%d", 1);
	else
		fprintf(p, "%d", 0);
	fclose(p);

	/* unexport gpio*/
	p = fopen(GPIO_UNEXPORT, "w");
	if (!p) {
		ALOGE("open %s failed", GPIO_UNEXPORT);
		return -1;
	}
	fprintf(p, "%d", gpio);
	fclose(p);

	return 0;
}

/* device name of infrared light */
#define IR_DEV_NAME \
	"/sys/class/leds/infrared_light/brightness"

/* device name of white light */
#define LIGHT_DEV_NAME \
	"/sys/class/leds/white_light/brightness"

typedef enum _LED_STATE {
	LED_OFF = 0,
	LED_ON,
} LED_STATE;

/*
 * led_ctrl - set led status
 * @node: device node name
 * @level: led status
 *
 * return 0 if success, otherwise return -1
 */
static int led_ctrl(char *node, LED_STATE state)
{
	FILE *p = NULL;

	if (NULL == node) {
		ALOGE("device node is NULL");
		return -2;
	}
	p = fopen(node, "w");
	if (!p) {
		ALOGE("open %s failed", node);
		return -1;
	}

	if (state == LED_OFF)
		fprintf(p, "%d", 0);
	else
		fprintf(p, "%d", 1);

	fclose(p);

	return 0;
}

/*
 * infrared_led_ctrl - set infrared led status
 * @state: led status
 *
 * return 0 if success, otherwise return -1
 */
int infrared_led_ctrl(LED_STATE state)
{
	return led_ctrl(IR_DEV_NAME, state);
}

/*
 * white_led_ctrl - set light led status
 * @state: led status
 *
 * return 0 if success, otherwise return -1
 */
int white_led_ctrl(LED_STATE state)
{
	return led_ctrl(LIGHT_DEV_NAME, state);
}

static long long get_systime_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

void *det_cb(face_frame_t *frame, face_rects_t *rects)
{
	int i;

	for (i = 0; i < rects->cnt; i++) {
		printf("rect x:%d, y:%d, x1:%d, y1:%d\n",
				rects->p_rect[i].rect[0],
				rects->p_rect[i].rect[1],
				rects->p_rect[i].rect[2],
				rects->p_rect[i].rect[3]);
	}
	return NULL;
}

void *recg_cb(face_frame_t *frame, face_info_t *face_info)
{
	printf("\nid: %d\nmax_id: %d\nliving: %d\nscore: %f\nfeat_version: %d\n\n",
			face_info->rect.id,
			face_info->feat.max_id,
			face_info->feat.living_body,
			face_info->feat.score,
			face_info->feat.feat_version);
	return NULL;
}

int face_img_cb(face_frame_t *frame)
{
	int ret;

	printf("enter p_face_img_cb\n");

	ret = face_return_frame(svc_hdl, frame);
	if (ret) {
		printf("face return failed");
		return -1;
	}

	return 0;
}

static void dump_feat(char *path, face_feat_t *feat)
{
	FILE *fp;

	if (!path || !feat)
		return;

	printf("feat->norm[0] = %f\n", feat->norm[0]);
	printf("feat->norm[1] = %f\n", feat->norm[1]);

	/* for test register face feats */
	test_register_feat = 1;
	test_norm[0] = feat->norm[0];
	test_norm[1] = feat->norm[1];
	memcpy(&test_feat.values, feat->values, FACE_FEAT_SIZE);

	fp = fopen(path, "wb+");
	if (!fp) {
		printf("%s open %s failed\n", __func__, path);
		return;
	}

	fwrite(feat->values, 1, FACE_FEAT_SIZE, fp);
	fclose(fp);
}

static void test_dump_frame_jpg(char *path, face_frame_t *frame)
{
	struct input_image_info input = {0};
	struct output_image_info output = {0};

	if (!path || !frame)
		return;

	input.addr[0] = frame->viraddr[0];
	input.addr[1] = frame->viraddr[1];
	input.size[0] = frame->size[0];
	input.size[1] = frame->size[1];
	input.width = frame->width;
	input.height = frame->height;
	input.format = IMG_COLOR_NV12;
	input.qualify = 100;
	input.file = path;
	if (jpeg_enc_file(&input, &output))
		ALOGE("%s encode frame to %s failed", __func__, path);
}


static sig_atomic_t s_signal_received = 0;
static void signal_handler(int sig_num)
{
	signal(sig_num, signal_handler);
	s_signal_received = sig_num;
}

/***********************************************
 * test face_svc api
 *
 ***********************************************/
 /*
 * rgb2nir
 */
void create_map_pararm(map_t *map)
{
	FILE *ini = NULL;

	ini = fopen("map_params", "wb");
	if (!ini) {
		printf("fopen map_params fail\n");
		return;
	}

	fprintf(ini,
			"[map]\n"
			"x_map_0=%lf\n"
			"x_map_1=%lf\n"
			"x_map_2=%lf\n"
			"x_map_3=%lf\n"
			"x_map_4=%lf\n"
			"y_map_0=%lf\n"
			"y_map_1=%lf\n"
			"y_map_2=%lf\n"
			"y_map_3=%lf\n"
			"y_map_4=%lf\n",
			map->x_map[0],
			map->x_map[1],
			map->x_map[2],
			map->x_map[3],
			map->x_map[4],
			map->y_map[0],
			map->y_map[1],
			map->y_map[2],
			map->y_map[3],
			map->y_map[4]
				);

			fclose(ini);
			return;
}

int cal_ord_map_params(void)
{
	printf("[%s]\n", __func__);

	map_t ord_map;
	memset(&ord_map, 0, sizeof(map_t));

	get_ord_map_t get_ord_map;
	memset(&get_ord_map, 0, sizeof(get_ord_map_t));
	get_ord_map.ord_map = &ord_map;
	get_ord_map.ord_path = "/data/ord_img";
	get_ord_map.ord_width = 6;
	get_ord_map.ord_hight = 9;
	get_ord_map.ord_grid_face_scale = 5.8;
	get_ord_map.flip = 0;

	get_ord_map_params(&get_ord_map);
	create_map_pararm(&ord_map);
	return 0;
}

static int create_dir(const char *path)
{
	if (access(path, R_OK | W_OK | X_OK))
		return mkdir(path, 0755);
	return 0;
}

static void test_ord_map_params(void)
{
	printf("[%s]\n", __func__);

	create_dir("/data/ord_img/");

	while (1) {
		printf("'q': exit and cal_ord_map_params, 't': take picture\n");
		char text[4096] = {0};
		fgets(text, 4096, stdin);
		printf("\ntext:\n%s\n", text);
		if (0 == strncmp(text, "q", 1)) {
			cal_ord_map_params();
			break;
		} else if (0 == strncmp(text, "t", 1)) {
			char rgb[256] = {0};
			char nir[256] = {0};
			long long cur_time =  get_systime_us();
			sprintf(rgb, "/data/ord_img/rgb-%lld.jpg", cur_time);
			sprintf(nir, "/data/ord_img/nir-%lld.jpg", cur_time);
			printf("rgb: %s\n", rgb);
			printf("nir: %s\n", nir);
			face_take_picture_file(FACE_CAMERA_RGB, rgb);
			face_take_picture_file(FACE_CAMERA_NIR, nir);
		}
	}
}

/*
 * face _svc
 */
static void test_face_svc_pause(void)
{
	printf("[%s]\n", __func__);
	face_svc_pause(svc_hdl);
}

static void test_face_svc_resume(void)
{
	printf("[%s]\n", __func__);
	face_svc_resume(svc_hdl);
}

static void test_face_svc_rgb_color_living_check(void)
{
	printf("[%s]\n", __func__);
	// note jpg size
	char img[][128] = {
		"debug_nir_living-20200915100359530000.jpg",
		"debug_nir_living-20200915100359670000.jpg",
	};

	for (int i = 0; i < sizeof(img) / 128; i++) {
		face_svc_rgb_color_living_check(svc_hdl, img[i]);
	}
}

static void test_face_svc_nir_living_check(void)
{
	printf("[%s]\n", __func__);
	// note jpg size
	char img[][128] = {
		"debug_nir_no_living-2020091510040021000.jpg",
		"debug_nir_no_living-20200915100400301000.jpg",
	};

	for (int i = 0; i < sizeof(img) / 128; i++) {
		face_svc_nir_living_check(svc_hdl, 2, img[i]);
	}
}

/*
 * QR
 */
int test_f_qr_scan_cb(void *arg, char *data)
{
	printf("二维码数据: %s\n", data);
	return 0;
}

static void test_face_svc_start_qrmode_2(void)
{
	printf("[%s]\n", __func__);
	int ret = 0;
	ret = face_svc_start_qrmode_set(svc_hdl, 2, test_f_qr_scan_cb, NULL);
	if (ret) {
		printf("check /etc/face_svc.conf zbar_enable = 1\n");
	}
}

static void test_face_svc_start_qrmode(void)
{
	printf("[%s]\n", __func__);
	int ret = 0;
	ret = face_svc_start_qrmode(svc_hdl, test_f_qr_scan_cb, NULL);
	if (ret) {
		printf("check /etc/face_svc.conf zbar_enable = 1\n");
	}
}

static void test_face_svc_stop_qrmode(void)
{
	printf("[%s]\n", __func__);
	face_svc_stop_qrmode(svc_hdl);
}

/*
 * face feature
 */
static void test_face_compare(void)
{
	float score = 0.0f;
	face_info_t info1;
	face_info_t info2;
	float norm = 0.0f;
	float norm_m[2] = {0};
	int j = 0;

	memset(&info1, 0, sizeof(face_info_t));
	memset(&info2, 0, sizeof(face_info_t));

	FILE *fin = NULL;
	FILE *fin1 = NULL;
	int flen = 0;
	fin = fopen("/data/compare1.bin", "rb");
	if (fin == NULL) {
		printf("reg_feat open /data/compare1.bin failed\n");
		return;
	}
	fin1 = fopen("/data/compare2.bin", "rb");
	if (fin1 == NULL) {
		printf("test_feat open /data/compare2.bin failed\n");
		return;
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);  // length=1024
	fseek(fin, 0, SEEK_SET);

	printf("flen =%d\n", flen);

	fread(info1.feat.values, 1, flen, fin);
	fread(info2.feat.values, 1, flen, fin1);

	/* norm0 */
	norm = 0.0f;
	for (j = 0; j < face_svc_conf->svc.feat_cmp_size / 2; j++) {
		norm += info1.feat.values[j] * info1.feat.values[j];
	}
	norm_m[0] = sqrtf(norm);
	/* norm1 */
	norm = 0.0f;
	for (; j < face_svc_conf->svc.feat_cmp_size; j++) {
		norm += info1.feat.values[j] * info1.feat.values[j];
	}
	norm_m[1] = sqrtf(norm);

	info1.feat.norm[0] = norm_m[0];
	info1.feat.norm[1] = norm_m[1];

	score = face_compare(svc_hdl, &info1, &info2);
	printf("## face_compare score=%f\n", score);

}

static void test_face_compare2(void)
{
	printf("[%s]\n", __func__);
	signed char *info1 = NULL;
	signed char *info2 = NULL;
	float score;

	FILE *fin = NULL;
	FILE *fin1 = NULL;
	int flen = 0;
	fin = fopen("/data/compare1.bin", "rb");
	if (fin == NULL) {
		printf("reg_feat open /data/compare1.bin failed\n");
		goto exit;
	}
	fin1 = fopen("/data/compare2.bin", "rb");
	if (fin1 == NULL) {
		printf("test_feat open /data/compare2.bin failed\n");
		goto exit;
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);  // length=1024
	fseek(fin, 0, SEEK_SET);

	printf("flen =%d\n", flen);
	info1 = malloc(sizeof(char) * flen);
	if (info1 == NULL) {
		printf("malloc failed\n");
		goto exit;
	}
	info2 = malloc(sizeof(char) * flen);
	if (info2 == NULL) {
		printf("malloc failed\n");
		goto exit;
	}
	memset(info1, 0, flen);
	memset(info2, 0, flen);
	fread(info1, 1, flen, fin);
	fread(info2, 1, flen, fin1);

	score = face_compare2(info1,info2);
	printf("## face_compare2 score=%f\n",score);
exit:
	if (info1)
		free(info1);
	if (info2)
		free(info2);
	if (fin)
		fclose(fin);
	if (fin1)
		fclose(fin1);
}

static void test_face_compare2_with_mask(void)
{
	printf("[%s]\n", __func__);
	signed char *info1 = NULL;
	signed char *info2 = NULL;
	float score;

	FILE *fin = NULL;
	FILE *fin1 = NULL;
	int flen = 0;
	fin = fopen("/data/compare1.bin", "rb");
	if (fin == NULL) {
		printf("reg_feat open /data/compare1.bin failed\n");
		goto exit;
	}
	fin1 = fopen("/data/compare2.bin", "rb");
	if (fin1 == NULL) {
		printf("test_feat open /data/compare2.bin failed\n");
		goto exit;
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);  // length=1024
	fseek(fin, 0, SEEK_SET);

	printf("flen =%d\n", flen);
	info1 = malloc(sizeof(char) * flen);
	if (info1 == NULL) {
		printf("malloc failed\n");
		goto exit;
	}
	info2 = malloc(sizeof(char) * flen);
	if (info2 == NULL) {
		printf("malloc failed\n");
		goto exit;
	}
	memset(info1, 0, flen);
	memset(info2, 0, flen);
	fread(info1, 1, flen, fin);
	fseek(fin1, 512, SEEK_SET);
	fread(info2, 1, flen - 512, fin1);

	score = face_compare2_with_mask(info1,info2);
	printf("## face_compare2_with_mask score=%f\n",score);
exit:
	if (info1)
		free(info1);
	if (info2)
		free(info2);
	if (fin)
		fclose(fin);
	if (fin1)
		fclose(fin1);
}

static void test_face_register_face_feats(void)
{
	static int db_version;

	feats_t feats;
	int ret = 0;

	printf("[%s]\n", __func__);

	if (!test_register_feat) {
		printf("test face_get_feats_from_capture first\n");
		return;
	}

	feats.cnt = 1;
	feats.feat = &test_feat;
	feats.kpts = NULL;
	feats.norm = test_norm;
	feats.version = ++db_version;

	ret = face_register_face_feats(svc_hdl, &feats);
	if (ret) {
		printf("face_register_face_feats failed\n");
		return;
	}
}

static void test_face_get_feats_from_image_file(void)
{
	printf("[%s]\n", __func__);
	int ret;

	face_feat_opt_t opt;
	memset(&opt, 0, sizeof(face_feat_opt_t));

	face_image_file_t file;
	memset(&file, 0, sizeof(face_image_file_t));
	file.hflip = 0;
	snprintf(file.image_path, FILENAME_MAX - 1, "%s", "/data/test.jpg");
	file.save_face = 1;
	snprintf(file.save_face_path, FILENAME_MAX - 1, "%s", "/data/test_face.jpg");

	ret = face_get_feats_from_image_file(svc_hdl, &file, &opt);
	if (ret == FACE_SUCCESS) {
		printf("opt.face_num = %d\n", opt.face_num);
		printf("opt.face_num = %f\n", opt.simulity);
		printf("kpts: %f:%f:%f:%f:%f:%f:%f:%f:%f:%f\n",
				opt.info.rect.kpts[0], opt.info.rect.kpts[1], opt.info.rect.kpts[2],
				opt.info.rect.kpts[3], opt.info.rect.kpts[4], opt.info.rect.kpts[5],
				opt.info.rect.kpts[6], opt.info.rect.kpts[7], opt.info.rect.kpts[8],
				opt.info.rect.kpts[9]);
		dump_feat("/data/test_file_feat.bin", &opt.info.feat);
	}

	usleep(1000 * 1000);
}

static void test_face_get_feats_from_image_data(void)
{
	printf("[%s]\n", __func__);
	int ret;

	face_feat_opt_t opt;
	memset(&opt, 0, sizeof(face_feat_opt_t));

	face_image_data_t data;
	memset(&data, 0, sizeof(face_image_data_t));

	FILE *fin = NULL;
	int flen = 0;
	fin = fopen("/data/test.jpg", "rb");
	if (fin == NULL) {
		printf("face img open failed\n");
		return;
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	data.image_ptr = malloc(flen);
	if (data.image_ptr == NULL) {
		printf("malloc failed\n");
		return;
	}
	memset(data.image_ptr, 0, flen);
	fread(data.image_ptr, 1, flen, fin);
	data.len = flen;
	fclose(fin);
	printf("data.len = %d\n", data.len);

	data.hflip = 0;
	data.save_face = 1;
	data.save_frame = 1;
	snprintf(data.save_face_path, FILENAME_MAX - 1, "%s", "/data/test_data_face.jpg");
	snprintf(data.save_frame_path, FILENAME_MAX - 1, "%s", "/data/test_data_frame.jpg");

	ret = face_get_feats_from_image_data(svc_hdl, &data, &opt);
	printf("ret = %d\n", ret);
	if (ret == FACE_SUCCESS) {
		printf("opt.face_num = %d\n", opt.face_num);
		printf("opt.face_num = %f\n", opt.simulity);
		printf("kpts: %f:%f:%f:%f:%f:%f:%f:%f:%f:%f\n",
				opt.info.rect.kpts[0], opt.info.rect.kpts[1], opt.info.rect.kpts[2],
				opt.info.rect.kpts[3], opt.info.rect.kpts[4], opt.info.rect.kpts[5],
				opt.info.rect.kpts[6], opt.info.rect.kpts[7], opt.info.rect.kpts[8],
				opt.info.rect.kpts[9]);
		dump_feat("/data/test_data_feat.bin", &opt.info.feat);
	}
	free(data.image_ptr);

	usleep(1000 * 1000);
}

static void test_face_get_feats_from_capture(void)
{
	printf("[%s]\n", __func__);
	int ret;

	face_feat_opt_t opt;
	memset(&opt, 0, sizeof(face_feat_opt_t));

	face_capture_param_t capture_param;
	memset(&opt, 0, sizeof(face_feat_opt_t));
	memset(&capture_param, 0, sizeof(face_feat_opt_t));
	capture_param.timeout_sec = 10;
	capture_param.face.head_pose_yaw = 20;
	capture_param.face.head_pose_pitch = 20;
	capture_param.face.head_pose_roll = 30;
	capture_param.face.min = 120;
	capture_param.face.max = 512;
	capture_param.face.x0 = 0;
	capture_param.face.y0 = 0;
	capture_param.face.x1 = RGB_WIDTH;
	capture_param.face.y1 = RGB_HEIGHT;
	capture_param.face.quality = 45;
	capture_param.save_face = 1;
	capture_param.save_frame = 1;
	sprintf(capture_param.save_face_path, "/data/capture_face.jpg");
	sprintf(capture_param.save_frame_path, "/data/capture_frame.jpg");
	ret = face_get_feats_from_capture(svc_hdl, &capture_param , &opt);
	printf("ret = %d\n", ret);
	if (ret == FACE_SUCCESS) {
		printf("opt.face_num = %d\n", opt.face_num);
		printf("opt.simulity = %f\n", opt.simulity);
		printf("kpts: %f:%f:%f:%f:%f:%f:%f:%f:%f:%f\n",
				opt.info.rect.kpts[0], opt.info.rect.kpts[1], opt.info.rect.kpts[2],
				opt.info.rect.kpts[3], opt.info.rect.kpts[4], opt.info.rect.kpts[5],
				opt.info.rect.kpts[6], opt.info.rect.kpts[7], opt.info.rect.kpts[8],
				opt.info.rect.kpts[9]);
		dump_feat("/data/test_capture_feat.bin", &opt.info.feat);
	}
	test_face_register_face_feats();
	usleep(1000 * 1000);
}

/*
 * media
 */
static void test_face_svc_start_preview(void)
{
	printf("[%s]\n", __func__);
	/*referenceb /etc/face_svc.conf param */
	face_svc_start_preview(FACE_CAMERA_RGB, 0, 0, RGB_HEIGHT, RGB_WIDTH, 90, 1);
	face_svc_start_preview(FACE_CAMERA_NIR, 0, 0, NIR_HEIGHT, NIR_WIDTH, 90, 1);
}

static void test_face_svc_stop_preview(void)
{
	printf("[%s]\n", __func__);
	face_svc_stop_preview(FACE_CAMERA_RGB);
	face_svc_stop_preview(FACE_CAMERA_NIR);
}

static void test_face_take_picture_file(void)
{
	printf("[%s]\n", __func__);
	long long cur_time =  get_systime_us();
	char rgb[256] = {0};
	char nir[256] = {0};
	sprintf(rgb, "/data/rgb-%lld.jpg", cur_time);
	sprintf(nir, "/data/nir-%lld.jpg", cur_time);
	face_take_picture_file(FACE_CAMERA_RGB, rgb);
	face_take_picture_file(FACE_CAMERA_NIR, nir);
}

int test_rgb_p_face_img_cb(face_frame_t *frame)
{
	printf("[%s]\n", __func__);
	long long cur_time =  get_systime_us();
	char rgb[256] = {0};
	sprintf(rgb, "/data/rgb-%lld.jpg", cur_time);
	test_dump_frame_jpg(rgb, frame);

	int ret;
	ret = face_return_frame(svc_hdl, frame);
	if (ret) {
		printf("frame return failed");
		return -1;
	}
	return 0;
}

int test_nir_p_face_img_cb(face_frame_t *frame)
{
	printf("[%s]\n", __func__);
	long long cur_time =  get_systime_us();
	char nir[256] = {0};
	sprintf(nir, "/data/nir-%lld.jpg", cur_time);
	test_dump_frame_jpg(nir, frame);

	int ret;
	ret = face_return_frame(svc_hdl, frame);
	if (ret) {
		printf("frame return failed");
		return -1;
	}
	return 0;
}

static void test_face_take_picture_cb(void)
{
	printf("[%s]\n", __func__);
	face_take_picture_cb(FACE_CAMERA_RGB, test_rgb_p_face_img_cb);
	face_take_picture_cb(FACE_CAMERA_NIR, test_nir_p_face_img_cb);
}

static void test_face_svc_start_rec(void)
{
	printf("[%s]\n", __func__);
	long long cur_time =  get_systime_us();
	char rgb[256] = {0};
	char nir[256] = {0};
	sprintf(rgb, "/data/rgb-%lld.mp4", cur_time);
	sprintf(nir, "/data/nir-%lld.mp4", cur_time);
	face_svc_start_rec(FACE_CAMERA_RGB, NULL, rgb);
	face_svc_start_rec(FACE_CAMERA_NIR, NULL, nir);
}

static void test_face_svc_stop_rec(void)
{
	printf("[%s]\n", __func__);
	face_svc_stop_rec(FACE_CAMERA_RGB);
	face_svc_stop_rec(FACE_CAMERA_NIR);
}

static char last_api[256];

static face_svc_api_t face_svc_api[] = {

	{"face_svc_start_qrmode", test_face_svc_start_qrmode},
	{"face_svc_start_qrmode_2", test_face_svc_start_qrmode_2},
	{"face_svc_stop_qrmode", test_face_svc_stop_qrmode},

	{"face_get_feats_from_capture", test_face_get_feats_from_capture},
	{"face_get_feats_from_image_file", test_face_get_feats_from_image_file},
	{"face_get_feats_from_image_data", test_face_get_feats_from_image_data},
	{"face_register_face_feats", test_face_register_face_feats},

	{"face_compare2_with_mask", test_face_compare2_with_mask},
	{"face_compare2", test_face_compare2},
	{"face_compare", test_face_compare},

	{"face_svc_start_preview", test_face_svc_start_preview},
	{"face_svc_stop_preview", test_face_svc_stop_preview},
	{"face_svc_start_rec", test_face_svc_start_rec},
	{"face_svc_stop_rec", test_face_svc_stop_rec},
	{"face_take_picture_cb", test_face_take_picture_cb},
	{"face_take_picture_file", test_face_take_picture_file},

	{"face_svc_pause", test_face_svc_pause},
	{"face_svc_resume", test_face_svc_resume},
	{"ord_map_params", test_ord_map_params},
	{"rgb_color_living_check", test_face_svc_rgb_color_living_check},
	{"nir_living_check", test_face_svc_nir_living_check},


};

#define API_CNT (sizeof(face_svc_api)/sizeof(face_svc_api[0]))

int test_face_svc_api(char *text)
{
	int not_find = 0;
	for (int i = 0; i < API_CNT; i++) {
		if (0 == strncmp(text, face_svc_api[i].name, strlen(face_svc_api[i].name))){
			face_svc_api[i].func();
			memcpy(last_api, face_svc_api[i].name,  strlen(face_svc_api[i].name) + 1);
			goto exit;
		}
	}

	not_find = 1;
exit:
	return not_find;
}

static void show_help(void)
{
	printf(STAR);
	printf("face svc api: [%s]\n\n", last_api);
	for (int i = 0; i < API_CNT; i++) {
		printf("%s\n", face_svc_api[i].name);
	}
	printf("\nExit: 'Ctrl + c' + Enter\n");
	printf(STAR);
}

int main(int argc, char *argv[])
{
	int ret;

	system(CLEAN_SCREEN);
	system(CLEAN_ERASE_H);

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	/* Turn on infra led temporarily */

#ifdef p1
	/* p1/p1s */
	gpio_set_output_level(60, 1);
	gpio_set_output_level(44, 0);
#endif

#ifdef p2
	/* p2/p2s */
	infrared_led_ctrl(LED_ON);
	white_led_ctrl(LED_OFF);
#endif

	/* init face svc */
	svc_hdl = face_svc_init();
	if (!svc_hdl) {
		printf("face svc init failed\n");
		return -1;
	}

	/* app set cb */
	face_register_det_cb(svc_hdl, det_cb);
	face_register_recg_cb(svc_hdl, recg_cb);

	/* start face svc */
	face_svc_start(svc_hdl);

	usleep(3 * 1000 * 1000);
	show_help();

	while (s_signal_received == 0) {
		char text[4096] = {0};

		printf("\napi:");
		fgets(text, 4096, stdin);
		printf("\ntext:\n%s\n", text);
		if (text[strlen(text) - 1] == '\n')
			text[strlen(text) - 1] = '\0';

		if(test_face_svc_api(text))
			show_help();
	}

	/* stop face svc */
	face_svc_stop(svc_hdl);
	face_svc_deinit(svc_hdl);

	return ret;
}
