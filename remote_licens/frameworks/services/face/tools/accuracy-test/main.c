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
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>
#include "ee_list.h"
#include "file_list.h"
#include "acc_svc.h"

#define LOG_TAG "acc_test"
#include <log/log.h>
#include <signal.h>

#define C_QUALITY_THRESHOLD 20 /*0-100*/
#define C_LOGFILE "./acc-test.log"

static acc_svc_t *svc;

int cmpfunc(const void *a, const void *b)
{
	return *(float *)a < *(float *)b;
}

long long get_systime_ms(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void signal_term_handler(int num)
{
	ALOGD("signal_term_handler");
	acc_svc_deinit(svc);
}

void printprogess(int cur, int total)
{
	if (cur <= total) {
		printf("\r(%d/%d)", cur, total);
		fflush(stdout);
	} else {
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	int ret;
	face_info_t face_info;
	feats_t *base_face_feats = NULL;
	FILE *fp_log = NULL;
	char *base_path = NULL;
	char *attack_path = NULL;
	char *test_path = NULL;
	struct list_head base_file_list;
	struct list_head attack_file_list;
	struct list_head test_file_list;
	int processcnt = 0;
	int base_cnt = 0, base_cnt_noerror = 0;
	int attack_cnt = 0, attack_cnt_noerror = 0;
	int test_cnt = 0, test_cnt_noerror = 0;
	float *attack_score = NULL;
	float *test_score = NULL;
	long long start_time, end_time;

	if (argc != 4) {
		printf("Usage: %s base_dir attack_dir test_dir\n", argv[0]);
		return -1;
	} else {
		base_path = argv[1];
		attack_path = argv[2];
		test_path = argv[3];
	}

	fp_log = fopen(C_LOGFILE, "w");
	if (!fp_log) {
		printf("Open logfile failed\n");
		return -1;
	}

	/* register sigterm handler */
	signal(SIGTERM, signal_term_handler);

	svc = acc_svc_init();
	if (!svc) {
		printf("face svc init failed\n");
		fclose(fp_log);
		return -1;
	}
	/* set quality cfg */
	acc_svc_set_cfg(svc, C_QUALITY_THRESHOLD);

	/* find all jpg files */
	file_t *file, *file_temp;
	INIT_LIST_HEAD(&base_file_list);
	trave_dir(base_path, &base_file_list, &base_cnt);
	printf("base file total num %d\n", base_cnt);
	INIT_LIST_HEAD(&attack_file_list);
	trave_dir(attack_path, &attack_file_list, &attack_cnt);
	printf("attack file total num %d\n", attack_cnt);
	INIT_LIST_HEAD(&test_file_list);
	trave_dir(test_path, &test_file_list, &test_cnt);
	printf("test file total num %d\n", test_cnt);

	/* malloc */
	base_face_feats = malloc(sizeof(feats_t) * base_cnt);
	base_face_feats->values = malloc(sizeof(signed char) * FACE_FEAT_SIZE * base_cnt);
	base_face_feats->norm   = malloc(sizeof(float) * 2 * base_cnt);
	base_face_feats->kpts   = malloc(sizeof(float) * 10 * base_cnt);
	if (!base_face_feats || !base_face_feats->values || !base_face_feats->norm || !base_face_feats->kpts) {
		printf("%s malloc base_face_feats failed\n", __func__);
		fclose(fp_log);
		return -1;
	}
	attack_score = malloc(sizeof(float) * attack_cnt);
	if (!attack_score) {
		printf("%s malloc attack_score failed\n", __func__);
		fclose(fp_log);
		return -1;
	}
	memset(attack_score, 0, sizeof(float) * attack_cnt);
	test_score = malloc(sizeof(float) * test_cnt);
	if (!test_score) {
		printf("%s malloc test_score failed\n", __func__);
		fclose(fp_log);
		return -1;
	}
	memset(test_score, 0, sizeof(float) * test_cnt);

	start_time = get_systime_ms();
	fprintf(fp_log, "===================scan base pic===================\n");
	printf("===================scan base pic===================\n");
	/* calc feature for each face */
	processcnt = 1;
	list_for_each_entry_safe(file, file_temp, &base_file_list, node) {
		usleep(10);
		ret = acc_svc_get_feats(svc, file->p, &face_info);
		if (ret != 0) {
			fprintf(fp_log, "[%s],jpg decode failed\n", file->p);
		} else if (face_info.error == 0) {
			fprintf(fp_log, "[%d:%s],face quality %ld\n", base_cnt_noerror, file->p, face_info.quality_score);
			memcpy(&base_face_feats->values[FACE_FEAT_SIZE * base_cnt_noerror], face_info.values, sizeof(signed char) * FACE_FEAT_SIZE);
			memcpy(&base_face_feats->norm[2 * base_cnt_noerror], face_info.norm, sizeof(float) * 2);
			memcpy(&base_face_feats->kpts[10 * base_cnt_noerror], face_info.kpts, sizeof(float) * 10);
			base_cnt_noerror += 1;
		} else {
			fprintf(fp_log, "[%s],no face or more than 1 face detected\n", file->p);
		}
		printprogess(processcnt++, base_cnt);
	}
	base_face_feats->cnt = base_cnt_noerror;
	end_time = get_systime_ms();
	fprintf(fp_log, "===================scan base pic finished, takes %f s===================\n", (float)(end_time-start_time)/1000);
	printf("===================scan base pic finished, takes %f s===================\n", (float)(end_time-start_time)/1000);
	fprintf(fp_log, "base file total num %d,valid num %d\n", base_cnt, base_cnt_noerror);
	printf("base file total num %d,valid num %d\n", base_cnt, base_cnt_noerror);
	if (base_cnt_noerror == 0) {
		printf("%s no valid base file\n", __func__);
		fclose(fp_log);
		return -1;
	}

	/* register base */
	ret = acc_svc_register_base(svc, base_face_feats);

	/* get attack highist score */
	start_time = get_systime_ms();
	fprintf(fp_log, "===================scan attack pic===================\n");
	printf("===================scan attack pic===================\n");
	processcnt = 1;
	list_for_each_entry_safe(file, file_temp, &attack_file_list, node) {
		usleep(10);
		ret = acc_svc_get_feats(svc, file->p, &face_info);
		if (ret != 0) {
			fprintf(fp_log, "[%s],jpg decode failed\n", file->p);
		} else if (face_info.error == 0) {
			acc_svc_compare_to_base(svc, &face_info);
			fprintf(fp_log, "[%d:%s],face quality %ld,max score %f,corresponding base id %ld\n", attack_cnt_noerror, file->p, face_info.quality_score, face_info.score, face_info.max_id);
			attack_score[attack_cnt_noerror] = face_info.score;
			attack_cnt_noerror += 1;
		} else {
			fprintf(fp_log, "[%s],no face or more than 1 face detected\n", file->p);
		}
		printprogess(processcnt++, attack_cnt);
	}
	end_time = get_systime_ms();
	fprintf(fp_log, "===================scan attack pic finished, takes %f s===================\n", (float)(end_time-start_time)/1000);
	printf("===================scan attack pic finished, takes %f s===================\n", (float)(end_time-start_time)/1000);
	fprintf(fp_log, "attack file total num %d,valid num %d\n", attack_cnt, attack_cnt_noerror);
	printf("attack file total num %d,valid num %d\n", attack_cnt, attack_cnt_noerror);
	/* sort attack score */
	qsort(attack_score, attack_cnt, sizeof(float), cmpfunc);

	/* calc test score */
	start_time = get_systime_ms();
	fprintf(fp_log, "===================scan test pic===================\n");
	printf("===================scan test pic===================\n");
	processcnt = 1;
	list_for_each_entry_safe(file, file_temp, &test_file_list, node) {
		usleep(10);
		ret = acc_svc_get_feats(svc, file->p, &face_info);
		if (ret != 0) {
			fprintf(fp_log, "[%s],jpg decode failed\n", file->p);
		} else if (face_info.error == 0) {
			acc_svc_compare_to_base(svc, &face_info);
			fprintf(fp_log, "[%d:%s],face quality %ld,max score %f,corresponding base id %ld\n", test_cnt_noerror, file->p, face_info.quality_score, face_info.score, face_info.max_id);
			test_score[test_cnt_noerror] = face_info.score;
			test_cnt_noerror += 1;
		} else {
			fprintf(fp_log, "[%s],no face or more than 1 face detected\n", file->p);
		}
		printprogess(processcnt++, test_cnt);
	}
	end_time = get_systime_ms();
	fprintf(fp_log, "===================scan test pic finished, takes %f s===================\n", (float)(end_time-start_time)/1000);
	printf("===================scan test pic finished, takes %f s===================\n", (float)(end_time-start_time)/1000);
	fprintf(fp_log, "test file total num %d,valid num %d\n", test_cnt, test_cnt_noerror);
	printf("test file total num %d,valid num %d\n", test_cnt, test_cnt_noerror);
	/* sort test score */
	qsort(test_score, test_cnt, sizeof(float), cmpfunc);

	/* calc pass rate @ diff attack rate */
	fprintf(fp_log, "===================calc pass rate @ diff acc===================\n");
	printf("===================calc pass rate @ diff acc===================\n");
	for (int i = 0; i < 101; i++) {
		int attack_pass_idx = (int)(attack_cnt_noerror * 0.001 * i);
		float thr = attack_score[attack_pass_idx];
		int pass_cnt = 0;
		/* get test paas rate */
		for (int j = 0; j < test_cnt_noerror; j++) {
			if (test_score[j] > thr)
				pass_cnt += 1;
			else
				break;
		}
		fprintf(fp_log, "@acc rate %%%.3f(thr %f),test file pass num %d,pass rate %%%.3f\n", (1-0.001 * i) * 100, thr, pass_cnt, (((float)pass_cnt)/test_cnt_noerror) * 100);
		printf("@acc rate %%%.3f(thr %f),test file pass num %d,pass rate %%%.3f\n", (1-0.001 * i) * 100, thr, pass_cnt, (((float)pass_cnt)/test_cnt_noerror) * 100);
	}

	/* free res */
	usleep(1000000);
	free(attack_score);
	free(test_score);
	free(base_face_feats->values);
	free(base_face_feats->norm);
	free(base_face_feats->kpts);
	free(base_face_feats);
	list_for_each_entry_safe(file, file_temp, &base_file_list, node) {
		list_del(&file->node);
		free(file);
	}
	list_for_each_entry_safe(file, file_temp, &attack_file_list, node) {
		list_del(&file->node);
		free(file);
	}
	list_for_each_entry_safe(file, file_temp, &test_file_list, node) {
		list_del(&file->node);
		free(file);
	}
	ret = acc_svc_deinit(svc);
	if (ret) {
		ALOGE("face svc deinit failed");
		fclose(fp_log);
		return -1;
	}
	fclose(fp_log);
	return 0;
}

