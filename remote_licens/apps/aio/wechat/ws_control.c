/*
 * ws_control.c - main interface of the websocket_test testcase.
 *
 * Copyright (C) 2016 - 2019, LomboTech Co.Ltd.
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
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#define LOG_TAG "ws_app"
#include <log/log.h>

#include "ws_control.h"
#include "base64.h"
#include "db_all.h"
#include "time.h"
#include "face.h"
/*#include "signal_for_ui.hpp"*/
#include "file_ops.h"
#include "system_private.h"

#define SN_OFFSET               0
#define SECRETNO_OFFSET         0x200
#define DEVICEID_OFFSET         0x400
#define QRCODE_OFFSET           0x600

#define MAX_BUF_LEN             200

int ws_control_get_sn(char *sn, unsigned int len)
{
	int ret = 0;

	if (NULL == sn) {
		ALOGE("sn is NULL\n");
		return -1;
	}
	if (len > MAX_BUF_LEN) {
		ALOGE("sn len is %d is too big\n", len);
		return -1;
	}
	ret = system_get_private_data(SN_OFFSET, sn, len);
	if (ret) {
		ALOGE("get SN failed");
		return -1;
	}

	return ret;
}
int ws_control_get_secret_no(char *secret_no, unsigned int len)
{
	int ret = 0;

	if (NULL == secret_no) {
		ALOGE("secret_no is NULL\n");
		return -1;
	}
	if (len > MAX_BUF_LEN) {
		ALOGE("secret_no len is %d is too big\n", len);
		return -1;
	}
	ret = system_get_private_data(SECRETNO_OFFSET, secret_no, len);
	if (ret) {
		ALOGE("get secret_no failed");
		return -1;
	}

	return ret;
}
int ws_control_get_device_id(char *device_id, unsigned int len)
{
	int ret = 0;

	if (NULL == device_id) {
		ALOGE("device_id is NULL\n");
		return -1;
	}
	if (len > MAX_BUF_LEN) {
		ALOGE("device_id len is %d is too big\n", len);
		return -1;
	}
	ret = system_get_private_data(DEVICEID_OFFSET, device_id, len);
	if (ret) {
		ALOGE("get device_id failed");
		return -1;
	}

	return ret;
}
int ws_control_get_qrcode(char *qrcode, unsigned int len)
{
	int ret = 0;

	if (NULL == qrcode) {
		ALOGE("qrcode is NULL\n");
		return -1;
	}
	if (len > MAX_BUF_LEN) {
		ALOGE("qrcode len is %d is too big\n", len);
		return -1;
	}
	ret = system_get_private_data(QRCODE_OFFSET, qrcode, len);
	if (ret) {
		ALOGE("get qrcode failed");
		return -1;
	}

	return ret;
}
char *ws_control_get_timestamp(char *buf, int len)
{
	time_t now;
	time(&now);
	snprintf(buf, len - 1, "%ld", now);

	return buf;
}

int ws_control_set_timestamp(int sec)
{
	struct timeval stime;
	stime.tv_sec = sec;
	settimeofday(&stime, NULL);
	system("hwclock -w -u");

	return  0;
}

char *ws_control_get_nonce(char *buf, int n)
{
	int i = 0;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
	for (i = 0; i < n; i++) sprintf(&buf[i], "%d", (char)rand() % 9 + 1);
	buf[i] = '\0';

	return buf;
}

void ws_control_sort(char *a, int len)
{
	int i, j;
	char tmp;

	for (i = 0; i < len - 1; i++) {
		for (j = i + 1; j < len; j++) {
			if (a[i] > a[j]) {
				tmp = a[i];
				a[i] = a[j];
				a[j] = tmp;
			}
		}
	}

	return;
}

void ws_control_sort_dictionary_order(char (*str)[SORT_STR_LEN], int len)
{
	int i = 0;
	int j = 0;
	char temp[SORT_STR_LEN] = { 0 };

	for (i = 0; i < len; i++) {
		for (j = i + 1; j < len; j++) {
			if (strcmp(str[i], str[j]) > 0) {
				strcpy(temp, str[i]);
				strcpy(str[i], str[j]);
				strcpy(str[j], temp);
			}
		}
	}

	return;
}

/*fadata  to bindata  when userinfo sync*/
int ws_control_fadata_2_algdata(char *fadata,
				websocket_person_info_t *person_info)
{
	int len = 0;

	if (NULL == fadata || NULL == person_info) {
		ALOGE("ws_control_fadata_2_algdata -1\n");
		return -1;
	}
	ALOGD("ws_control_fadata_2_algdata fadata len = %d\n", strlen(fadata));
	char *bindata = malloc(strlen(fadata) * 3 / 4 + 1);

	if (NULL == bindata) {
		ALOGE("ws_control_fadata_2_algdata -2 \n");
		return -1;
	}
	len = base64_decode((const char *)fadata, (unsigned char *)bindata);

	if (len >= FA_DATA_SIZE) {

		memcpy(person_info->face_feature, bindata, FA_FEAT_SIZE);
		memcpy(person_info->key_points, &bindata[FA_FEAT_SIZE], FA_KPTS_SIZE);
		memcpy(person_info->face_norm, &bindata[FA_FEAT_SIZE + FA_KPTS_SIZE],
				FA_NORM_SIZE);
	}
	if (bindata) {
		free(bindata);
		bindata = NULL;
	}

	return 0;
}

int ws_control_algdata_2_fadata(char *alg_data, char *fadata)
{
	if (NULL == alg_data || NULL == fadata) {
		return -1;
	}
	base64_encode((const unsigned char *)alg_data, fadata, FA_DATA_SIZE);

	return 0;
}

int ws_control_admin_update(ee_queue_t *head)
{
	db_websocket_admin_info_del();
	db_websocket_admin_info_insert(head);
	face_db_update_admin();

	return 0;
}

int ws_control_admin_free(ee_queue_t *head)
{
	ee_queue_t *h = head, *pos, *n;
	websocket_admin_info_t *p = NULL;

	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, websocket_admin_info_t, queue);
		free(p);
	}

	ee_queue_init(h);

	return 0;
}

int ws_control_user_scan(char *user_id, int user_type)
{
	int ret = 0;
	ee_queue_t *h, head;

	websocket_admin_info_t *p = NULL;
	if (user_id == NULL) {
		return -1;
	}
	h = &head;
	ee_queue_init(h);
	db_websocket_admin_info_query_begin(h);

	ee_queue_foreach(p, h, websocket_admin_info_t, queue) {
		if (0 == strcmp(p->user_id, user_id)) {
			/*send_signal_for_websocket(WEB_SOCKET_EVENT_LOGIN);*/
			ret = 1;
			break;
		}
	}

	db_websocket_admin_info_query_end(h);

	return ret;
}

int ws_control_qrcode(void)
{
#if 0
	send_signal_for_websocket(WEB_SOCKET_EVENT_QRCODE);
#endif
	return 0;
}

/*visitor*/
int ws_control_visitor_update(ee_queue_t *head, ee_queue_t *head_rule)
{
	db_websocket_rule_info_insert(head_rule, TYPE_VISITOR);
	db_websocket_person_info_del(TYPE_VISITOR);
	db_websocket_person_info_insert(head);
	face_db_update();

	return 0;
}

static int visitor_free(ee_queue_t *head)
{
	if (NULL == head) {
		ALOGE("%s:head is null\n", __func__);
		return -1;
	}
	ee_queue_t *h = head, *pos, *n;
	websocket_person_info_t *p = NULL;

	if (ee_queue_empty(h)) {
		return 0;
	}
	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, websocket_person_info_t, queue);
		free(p);
	}

	ee_queue_init(h);

	return 0;
}

static int rules_free(ee_queue_t *head)
{
	if (NULL == head) {
		ALOGE("%s:head is null\n", __func__);
		return -1;
	}
	ee_queue_t *h = head, *pos, *n;
	web_socket_rule_info_t *p = NULL;

	if (ee_queue_empty(h)) {
		return 0;
	}
	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, web_socket_rule_info_t, queue);
		free(p);
	}

	ee_queue_init(h);

	return 0;
}

int ws_control_visitor_free(ee_queue_t *head, ee_queue_t *head_rule)
{
	visitor_free(head);
	rules_free(head_rule);

	return 0;
}

int ws_control_userinfo_delete_by_id(char *userid)
{
	db_websocket_person_info_del_by_user_id(userid);
	face_db_update();

	return 0;

}

int ws_control_userinfo_update_ids(ee_queue_t *head, ee_queue_t *head_rule)
{
	websocket_person_info_t *p = NULL;
	ee_queue_foreach(p, head, websocket_person_info_t, queue) {
		db_websocket_person_info_del_by_user_id(p->user_id);
	}
	db_websocket_person_info_insert(head);
	face_db_update();

	return 0;
}

int ws_control_userinfo_update(ee_queue_t *head, ee_queue_t *head_rule)
{
#if 0
	if (head_rule) {
		db_websocket_rule_info_insert(head_rule, TYPE_PERSON);
	}
#endif
	if (head) {
		db_websocket_person_info_del(TYPE_PERSON);
		db_websocket_person_info_insert(head);
		face_db_update();
	}

	return 0;
}

int ws_control_userinfo_free(ee_queue_t *head, ee_queue_t *head_rule)
{
	if (head) {
		visitor_free(head);
	}
#if 0
	if (head_rule) {
		rules_free(head_rule);
	}
#endif

	return 0;
}

int ws_control_userinfo_update_one(char *userid, int type, char *feat)
{
	int ret = 0;
	websocket_person_info_t *person = NULL;

	if (NULL == userid || NULL == feat) {
		return -1;
	}
	person = calloc(sizeof(websocket_person_info_t), 1);
	if (NULL == person) {
		return -2;
	}
	strncpy(person->user_id, userid, sizeof(person->user_id));
	person->user_type = type;
	memcpy(person->face_feature, feat, FA_FEAT_SIZE);
	memcpy(person->key_points, &feat[FA_FEAT_SIZE], FA_KPTS_SIZE);
	memcpy(person->face_norm, &feat[FA_FEAT_SIZE + FA_KPTS_SIZE],
			FA_NORM_SIZE);
	memset(person->access_card_number, 0,
		sizeof(person->access_card_number));

	ret = db_websocket_person_info_update_feat(person);
	if (person) {
		free(person);
		person = NULL;
	}
	face_capture_ok_sound_play();
	face_db_update();

	return ret;
}

int ws_control_get_perm_version(void)
{
	return db_perm_version_query();
}

int ws_control_set_perm_version(int version)
{
	return db_perm_version_update(version);
}

int ws_control_calc_feat_from_photo(_feat_local_t *feat, int sec)
{
	int ret = 0;
	face_info_t info;

	memset(&info, 0, sizeof(face_info_t));
	ret = face_photo_2_feat(&info, sec);
	if (ret == 0 && feat) {
		memcpy(feat->face_feature, info.feat.values, FA_FEAT_SIZE);
		memcpy(feat->key_points, info.rect.kpts, FA_KPTS_SIZE);
		memcpy(feat->face_norm, info.feat.norm, FA_NORM_SIZE);
	}

	return ret;
}

int ws_control_calc_feat_from_photo_stop(void)
{
	return face_stop_photo_2_feat();
}

int ws_control_calc_feat_from_picture(_feat_local_t *feat, char *path)
{
	int ret = 0;
	face_info_t info;

	memset(&info, 0, sizeof(face_info_t));
	ret = face_picture_2_feat(path, &info);
	if (ret == 0 && feat) {
		memcpy(feat->face_feature, info.feat.values, FA_FEAT_SIZE);
		memcpy(feat->key_points, info.rect.kpts, FA_KPTS_SIZE);
		memcpy(feat->face_norm, info.feat.norm, FA_NORM_SIZE);
	}

	return ret;
}

int ws_control_delete_file(char *file_path)
{
	return remove_pic_by_path(file_path);
}

int ws_control_save_file(char *file_path, char *buf, int size)
{
	return file_write(file_path, buf, size);
}

int ws_control_enter_page(char *open_id, char *userid)
{
#if 0
	send_signal_for_websocket(WEB_SOCKET_EVENT_ENTER_PAGE);
#endif
	return 0;
}

int ws_control_exit_page(void)
{
#if 0
	send_signal_for_websocket(WEB_SOCKET_EVENT_EXIT_PAGE);
#endif
	return 0;
}

int ws_control_ws_link(void)
{
#if 0
	send_signal_for_websocket(WEB_SOCKET_EVENT_LINKED);
#endif
	return 0;
}

int ws_control_ws_dis_link(void)
{
#if 0
	send_signal_for_websocket(WEB_SOCKET_EVENT_DIS_LINKED);
#endif
	return 0;
}

int ws_control_ws_bind(void)
{
#if 0
	websocket_sound_play(sound_bind);
	send_signal_for_websocket(WEB_SOCKET_EVENT_BIND);
#endif
	return 0;
}

int ws_control_ws_unbind(void)
{
#if 0
	websocket_sound_play(sound_unbind);
	send_signal_for_websocket(WEB_SOCKET_EVENT_UNBIND);
#endif
	return 0;
}

int ws_control_get_private_info(websocket_client_param_t *config)
{
	int ret = 0;
	ret = ws_control_get_sn(
		config->sn,
		sizeof(config->sn));
	if (ret < 0)
		return ret;
	ret = ws_control_get_secret_no(
		config->secret_no,
		sizeof(config->secret_no));
	if (ret < 0)
		return ret;
	ret = ws_control_get_device_id(
		config->device_id,
		sizeof(config->device_id));
	if (ret < 0)
		return ret;
	ret = ws_control_get_qrcode(
		config->static_qrcode_url,
		sizeof(config->static_qrcode_url));
	if (ret < 0)
		return ret;

	return ret;
}
#if 0
int ws_control_get_private_info(websocket_client_param_t *config)
{
	int ret = 0;

	ret = system_get_private_data(
		SN_OFFSET,
		config->sn,
		sizeof(config->sn));
	if (ret) {
		ALOGE("get SN failed");
		return -1;
	}
	/*config->sn[10] = 0;*/
	ALOGD("SN:%s\n", config->sn);

	ret = system_get_private_data(
		SECRETNO_OFFSET,
		config->secret_no,
		sizeof(config->secret_no));
	if (ret) {
		ALOGE("get SECRETNO failed");
		return -1;
	}
	/*config->secret_no[32] = 0;*/

	ALOGD("SECRETNO:%s\n", config->secret_no);

	ret = system_get_private_data(
		DEVICEID_OFFSET,
		config->device_id,
		sizeof(config->device_id));
	if (ret) {
		ALOGE("get DEVICEID failed");
		return -1;
	}
	/*config->device_id[19] = 0;*/
	ALOGD("DEVICEID:%s\n", config->device_id);

	ret = system_get_private_data(
		QRCODE_OFFSET,
		config->static_qrcode_url,
		sizeof(config->static_qrcode_url));
	if (ret) {
		ALOGE("get QRCODE failed");
		return -1;
	}
	/*config->static_qrcode_url[100] = 0;*/
	ALOGD("QRCODE:%s", config->static_qrcode_url);

	return 0;
}
#endif
int ws_control_get_param(websocket_client_param_t *config)
{
	int ret = 0;

	if (NULL == config) {
		return -1;
	}
	ret = db_websocket_client_param_query(config);
	if (ret != 0) {
		ALOGE("db_websocket_client_param_query failed\n");
	}
	ALOGD("web url = %s\n", config->websocket_url);
	ret = ws_control_get_private_info(config);

	return ret;
}

int ws_control_set_param(websocket_client_param_t *config)
{
	if (NULL == config) {
		return -1;
	}

	return db_websocket_client_param_update(config);
}

/*checkin */
int ws_control_get_checkin_data(ee_queue_t *head)
{
#if ATTENDANCE_MODE > 0
	return db_websocket_checkin_info_query_begin(head, 199);
#else
	return db_websocket_checkin_info_query_begin(head, 10);
#endif
}

/* checkin */
int ws_control_get_checkin_data_by_time(
	ee_queue_t *head, long start, long end)
{
	return db_websocket_checkin_info_query_by_time_begin(
		head, start, end);
}

/* checkin */
int ws_control_get_checkin_data_by_id_time(
	ee_queue_t *head, char *user_id, long start, long end)
{
	return db_websocket_checkin_info_query_by_id_time_begin(
		head, user_id, start, end);
}

int ws_control_get_checkin_count_by_id_time(
	char *user_id, long start, long end)
{
	return db_websocket_checkin_info_count_by_id_time(
		user_id, start, end);
}

int ws_control_get_checkin_by_id_time_limit(
	ee_queue_t *head, char *user_id, long start, long end,
	int index, int count)
{
	return db_websocket_checkin_query_limit_begin(
		head, user_id, start, end, index, count);
}


int ws_control_put_checkin_data(ee_queue_t *head)
{
	return db_websocket_checkin_info_query_end(head);
}

int ws_control_set_checkin_upload(ee_queue_t *head)
{
	web_socket_checkin_info_t *p = NULL;

	ee_queue_foreach(p, head, web_socket_checkin_info_t, queue) {
		p->is_upload = 1;
	}

	return 0;
}

int ws_control_del_checkin_data(ee_queue_t *head)
{
	web_socket_checkin_info_t *p = NULL;

	ee_queue_foreach(p, head, web_socket_checkin_info_t, queue) {
		if (p->is_upload)
			db_websocket_checkin_info_del(p);
	}

	return 0;
}

void ws_control_unbind_operation(void)
{
	db_websocket_admin_info_del();
	db_websocket_person_info_del(TYPE_PERSON);
	db_websocket_person_info_del(TYPE_VISITOR);
	db_websocket_checkin_info_del_table();
	face_db_update();
}
/**
 *
 *
 * @author
 *
 * @param path
 *
 * @return char*
 */
char *ws_control_get_file_name(char *path)
{
	int i = 0;
	int len = 0;
	if (NULL == path) {
		ALOGE("%s:path is null\n", __func__);
		return NULL;
	}
	len = strlen(path);
	for (i = len; i >= 0; i--) {
		if (path[i] == '/')
			return &path[i + 1];
	}
	return NULL;
}
