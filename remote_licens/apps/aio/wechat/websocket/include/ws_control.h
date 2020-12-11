/*
 * ws_control.h - main interface of the websocket_test testcase.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef API_WEBSOCKET_SRC_INC_WS_CONTROL_H_
#define API_WEBSOCKET_SRC_INC_WS_CONTROL_H_

#include "db_all.h"

#define SORT_STR_LEN 64

#define FA_FEAT_SIZE 1024
#define FA_KPTS_SIZE (sizeof(float)*10)
#define FA_NORM_SIZE (sizeof(float)*2)
#define FA_DATA_SIZE (FA_FEAT_SIZE+FA_KPTS_SIZE+FA_NORM_SIZE)

typedef struct {
	char face_feature[1024];
	float key_points[10];
	float face_norm[2];
} _feat_local_t;

void ws_control_sort_dictionary_order(char (* str)[SORT_STR_LEN], int len);
void ws_control_sort(char *a, int len);

char *ws_control_get_timestamp(char *buf, int len);
int ws_control_set_timestamp(int sec);
char *ws_control_get_nonce(char *buf, int n);

int ws_control_get_param(websocket_client_param_t *config);

int ws_control_set_param(websocket_client_param_t *config);

int ws_control_calc_feat_from_picture(_feat_local_t *feat, char *path);

int ws_control_get_perm_version(void);
int ws_control_set_perm_version(int perm_version);

int ws_control_insert_person(websocket_person_info_t *person_info);
int ws_control_insert_rule(web_socket_rule_info_t *rule);

int ws_control_fadata_2_algdata(char *fadata,
	websocket_person_info_t *person_info);
int ws_control_algdata_2_fadata(char *alg_data, char *fadata);

/*admin info*/
int ws_control_admin_update(ee_queue_t *head);
int ws_control_admin_free(ee_queue_t *head);

/*visitor list*/
int ws_control_visitor_update(ee_queue_t *head, ee_queue_t *head_rule);
int ws_control_visitor_free(ee_queue_t *head, ee_queue_t *head_rule);

/*userinfo */
int ws_control_userinfo_update_ids(ee_queue_t *head, ee_queue_t *head_rule);
int ws_control_userinfo_update(ee_queue_t *head, ee_queue_t *head_rule);
int ws_control_userinfo_free(ee_queue_t *head, ee_queue_t *head_rule);
int ws_control_userinfo_delete_by_id(char *userid);
int ws_control_userinfo_update_one(char *userid, int type, char *feat);

int ws_control_save_file(char *file_path, char *buf, int size);
int ws_control_delete_file(char *file_path);

/*signal send*/
int ws_control_enter_page(char *open_id, char *userid);
int ws_control_exit_page(void);
int ws_control_ws_link(void);
int ws_control_ws_dis_link(void);
int ws_control_ws_bind(void);

/*user_scan*/
int ws_control_user_scan(char *user_id, int user_type);
int ws_control_qrcode(void);

/*checkin data*/
int ws_control_get_checkin_data(ee_queue_t *head);
int ws_control_get_checkin_data_by_time(
	ee_queue_t *head, long start, long end);
int ws_control_get_checkin_data_by_id_time(
	ee_queue_t *head, char *user_id, long start, long end);
int ws_control_get_checkin_count_by_id_time(
	char *user_id, long start, long end);
int ws_control_get_checkin_by_id_time_limit(
	ee_queue_t *head, char *user_id, long start, long end,
	int index, int count);
int ws_control_put_checkin_data(ee_queue_t *head);
int ws_control_set_checkin_upload(ee_queue_t *head);
int ws_control_del_checkin_data(ee_queue_t *head);

int ws_control_calc_feat_from_photo(_feat_local_t *feat, int sec);

int ws_control_calc_feat_from_photo_stop(void);

void ws_control_unbind_operation(void);

int ws_control_ws_unbind(void);

/*ws get system info*/
int ws_control_get_sn(char *sn, unsigned int len);
int ws_control_get_secret_no(char *secret_no, unsigned int len);
int ws_control_get_device_id(char *device_id, unsigned int len);
int ws_control_get_qrcode(char *qrcode, unsigned int len);

char *ws_control_get_file_name(char *path);

#endif /* API_WEBSOCKET_SRC_INC_WS_CONTROL_H_ */
