#include "db_all.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "db.h"
#include "db_persistence.h"
#include "db_sqlite_middle.h"
#include "ee_queue.h"
#include "sql_head.h"

#ifdef _WIN32
/* //	#define NULL 0 */

#define ALOGD printf
#define ALOGE printf
#else
#include <log/log.h>
#endif

#ifndef __DB_F09

int get_int_from_text(sqlite3_stmt *stmt, int index)
{
	char tmp[64] = {0};
	db_stmt_get_text(stmt, index, tmp);
	return atoi(tmp);
}
/***************************************************************
 * For device hardware parameters update and query.
 ***************************************************************/

void device_hw_param_print(device_hw_param_t *p)
{
	printf("volume:%d\n", p->volume);
	printf("auto_display_brightness:%d\n", p->auto_display_brightness);
	printf("display_brightness:%d\n", p->display_brightness);
	printf("auto_screen_off:%d\n", p->auto_screen_off);
	printf("auto_screen_off_timeout:%d\n", p->auto_screen_off_timeout);
	printf("auto_screen_saver_timeout:%d\n", p->auto_screen_saver_timeout);
	printf("infrared_light_set:%d\n", p->infrared_light_set);
	printf("light_sensitive:%d\n", p->light_sensitive);
	printf("light_high_brightness:%d\n", p->light_high_brightness);
	printf("light_low_brightness:%d\n", p->light_low_brightness);
	printf("light_det_interval:%d\n", p->light_det_interval);
	printf("light_delay_time:%d\n", p->light_delay_time);
	printf("gpio:%d\n", p->gpio);
	printf("human_induction:%d\n", p->human_induction);
	printf("auto_screen_saver_off:%d\n", p->auto_screen_saver_off);
}

static int device_hw_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	device_hw_param_t *u = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 0:
			u->volume = (char)get_int_from_text(stmt, 2);
			break;
		case 1:
			u->auto_display_brightness = (char)get_int_from_text(stmt, 2);
			break;
		case 2:
			u->display_brightness = (char)get_int_from_text(stmt, 2);
			break;
		case 3:
			u->auto_screen_off = (char)get_int_from_text(stmt, 2);
			break;
		case 4:
			u->auto_screen_off_timeout = get_int_from_text(stmt, 2);
			break;
		case 5:
			u->auto_screen_saver_timeout = get_int_from_text(stmt, 2);
			break;
		case 6:
			u->infrared_light_set = (char)get_int_from_text(stmt, 2);
			break;
		case 7:
			u->light_sensitive = (char)get_int_from_text(stmt, 2);
			break;
		case 8:
			u->light_high_brightness = get_int_from_text(stmt, 2);
			break;
		case 9:
			u->light_low_brightness = get_int_from_text(stmt, 2);
			break;
		case 10:
			u->light_det_interval = (char)get_int_from_text(stmt, 2);
			break;
		case 11:
			u->light_delay_time = (char)get_int_from_text(stmt, 2);
			break;
		case 12:
			u->gpio = get_int_from_text(stmt, 2);
			break;
		case 13:
			u->human_induction = (char)get_int_from_text(stmt, 2);
			break;
		case 14:
			u->auto_screen_saver_off = (char)get_int_from_text(stmt, 2);
			break;
		case 302:
			db_stmt_get_text(stmt, 2, u->device_name);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 15 ? 0 : -1;
}

int db_device_hw_param_query(device_hw_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(device_hw_param_t));
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id<100;");

	return db_query_by_var_param(str_sql, device_hw_param_get_info, p_info,
								 NULL);
}
static int hw_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
							 void *arg)
{
	int rc;
	device_hw_param_t *p = arg;

	assert((stmt != NULL) && (arg != NULL));
	for (int i = 0; i <= 14; i++) {
		int bind_value = 0;
		switch (i) {
		case 0:
			bind_value = p->volume;
			break;
		case 1:
			bind_value = p->auto_display_brightness;
			break;
		case 2:
			bind_value = p->display_brightness;
			break;
		case 3:
			bind_value = p->auto_screen_off;
			break;
		/*int*/ case 4:
			bind_value = p->auto_screen_off_timeout;
			break;
		/*int*/ case 5:
			bind_value = p->auto_screen_saver_timeout;
			break;
		case 6:
			bind_value = p->infrared_light_set;
			break;
		case 7:
			bind_value = p->light_sensitive;
			break;
		/*int*/ case 8:
			bind_value = p->light_high_brightness;
			break;
		/*int*/ case 9:
			bind_value = p->light_low_brightness;
			break;
		case 10:
			bind_value = p->light_det_interval;
			break;
		case 11:
			bind_value = p->light_delay_time;
			break;
		/*int*/ case 12:
			bind_value = p->gpio;
			break;
		case 13:
			bind_value = p->human_induction;
			break;
		case 14:
			bind_value = p->auto_screen_saver_off;
			break;
		default:
			break;
		}

		char tmp[128] = {0};
		sprintf(tmp, "%d", bind_value);

		sqlite3_bind_text(stmt, 1, tmp, strlen(tmp), NULL);
		sqlite3_bind_int(stmt, 2, i);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			ALOGE("%s: Exec id:%d,value:%s error!\n", __func__, i, tmp);
			return rc;
		}
		sqlite3_reset(stmt);
	}
	return SQLITE_OK;
}
int db_device_hw_param_update(device_hw_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	/* //	memset(p_info, 0, sizeof(device_hw_param_t)); */
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, hw_info_bind_stmt, p_info);
}
/***************************************************************
 * For network info parameters update and query.
 ***************************************************************/

static int device_net_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	device_net_param_t *u = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 100:
			u->enable_eth = (char)get_int_from_text(stmt, 2);
			break;
		case 101:
			u->enable_eth_priority = (char)get_int_from_text(stmt, 2);
			break;
		case 102:
			u->enable_eth_dhcp = (char)get_int_from_text(stmt, 2);
			break;
		case 103:
			db_stmt_get_text(stmt, 2, u->eth_ip);
			break;
		case 104:
			db_stmt_get_text(stmt, 2, u->eth_mask);
			break;
		case 105:
			db_stmt_get_text(stmt, 2, u->eth_gateway);
			break;
		case 106:
			db_stmt_get_text(stmt, 2, u->eth_dns);
			break;
		case 107:
			u->enable_wifi = (char)get_int_from_text(stmt, 2);
			break;
		case 108:
			u->enable_hotspot = (char)get_int_from_text(stmt, 2);
			break;
		case 109:
			db_stmt_get_text(stmt, 2, u->hotspot_name);
			break;
		case 110:
			db_stmt_get_text(stmt, 2, u->hotspot_password);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 11 ? 0 : -1;
}

int db_device_net_param_query(device_net_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(device_net_param_t));
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id>99 AND id <200;");

	return db_query_by_var_param(str_sql, device_net_param_get_info, p_info,
								 NULL);
}

static int net_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
							  void *arg)
{
	int rc;
	device_net_param_t *p = arg;

	assert((stmt != NULL) && (arg != NULL));

	for (int i = 100; i <= 110; i++) {
		int bind_value = 0;
		char *bind_ptr = NULL;
		switch (i) {
		case 100:
			bind_value = p->enable_eth;
			break;
		case 101:
			bind_value = p->enable_eth_priority;
			break;
		case 102:
			bind_value = p->enable_eth_dhcp;
			break;
		case 103:
			bind_ptr = p->eth_ip;
			break;
		case 104:
			bind_ptr = p->eth_mask;
			break;
		case 105:
			bind_ptr = p->eth_gateway;
			break;
		case 106:
			bind_ptr = p->eth_dns;
			break;
		case 107:
			bind_value = p->enable_wifi;
			break;
		case 108:
			bind_value = p->enable_hotspot;
			break;
		case 109:
			bind_ptr = p->hotspot_name;
			break;
		case 110:
			bind_ptr = p->hotspot_password;
			break;
		default:
			break;
		}
		if (!bind_ptr) {
			char tmp[128] = {0};
			sprintf(tmp, "%d", bind_value);
			bind_ptr = tmp;
		}
		sqlite3_bind_text(stmt, 1, bind_ptr, strlen(bind_ptr), NULL);
		sqlite3_bind_int(stmt, 2, i);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("exec id:%d,value:%s error!\n", i, bind_ptr);
			return rc;
		}
		sqlite3_reset(stmt);
	}
	return SQLITE_OK;
}

int db_device_net_param_update(device_net_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	/* //	memset(p_info, 0, sizeof(device_hw_param_t)); */
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, net_info_bind_stmt, p_info);
}

/***************************************************************
 * For face recg parameters update and query.
 ***************************************************************/

static int face_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
							   void *arg)
{
	int rc;
	face_recognition_param_t *p = arg;

	assert((stmt != NULL) && (arg != NULL));

	for (int i = 200; i <= 203; i++) {
		char tmp[128] = {0};
		switch (i) {
		case 200:
			sprintf(tmp, "%f", p->identify_closest_distance);
			break;
		case 201:
			sprintf(tmp, "%f", p->identify_furthest_distance);
			break;
		case 202:
			sprintf(tmp, "%f", p->identify_threshold_score);
			break;
		case 203:
			sprintf(tmp, "%d", p->nir_live_detect);
			break;
		default:
			break;
		}

		sqlite3_bind_text(stmt, 1, tmp, strlen(tmp), NULL);
		sqlite3_bind_int(stmt, 2, i);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("exec id:%d,value:%s error!\n", i, tmp);
			return rc;
		}
		sqlite3_reset(stmt);
	}
	return SQLITE_OK;
}
int db_face_recognition_param_update(face_recognition_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	/* //	memset(p_info, 0, sizeof(device_hw_param_t)); */
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, face_info_bind_stmt, p_info);
}
static int device_face_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	face_recognition_param_t *u = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 200:
			u->identify_closest_distance = db_stmt_get_double(stmt, 2);
			break;
		case 201:
			u->identify_furthest_distance = db_stmt_get_double(stmt, 2);
			break;
		case 202:
			u->identify_threshold_score = db_stmt_get_double(stmt, 2);
			break;
		case 203:
			u->nir_live_detect = (char)(char)get_int_from_text(stmt, 2);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 4 ? 0 : -1;
}
int db_face_recognition_param_query(face_recognition_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(face_recognition_param_t));
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id>199 AND id <300;");

	return db_query_by_var_param(str_sql, device_face_param_get_info, p_info,
								 NULL);
}
/***************************************************************
 * For device info parameters update and query.
 ***************************************************************/

static int device_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
								 void *arg)
{
	int rc;
	device_system_info_t *p = arg;
	assert((stmt != NULL) && (arg != NULL));

	for (int i = 300; i <= 302; i++) {
		char *bind_ptr = NULL;
		switch (i) {
		case 300:
			bind_ptr = p->face_recg_alg_version;
			break;
		case 301:
			bind_ptr = p->firmware_version;
			break;
		case 302:
			bind_ptr = p->device_name;
			break;
		default:
			break;
		}

		sqlite3_bind_text(stmt, 1, bind_ptr, strlen(bind_ptr), NULL);
		sqlite3_bind_int(stmt, 2, i);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("exec id:%d,value:%s error!\n", i, bind_ptr);
			return rc;
		}
		sqlite3_reset(stmt);
	}
	return SQLITE_OK;
}

int db_device_system_info_update(device_system_info_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	/* //	memset(p_info, 0, sizeof(device_hw_param_t)); */
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, device_info_bind_stmt, p_info);
}

static int device_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	device_system_info_t *u = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 300:
			db_stmt_get_text(stmt, 2, u->face_recg_alg_version);
			break;
		case 301:
			db_stmt_get_text(stmt, 2, u->firmware_version);
			break;
		case 302:
			db_stmt_get_text(stmt, 2, u->device_name);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 3 ? 0 : -1;
}
int db_device_system_info_query(device_system_info_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(device_system_info_t));
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id>299 AND id <400;");

	return db_query_by_var_param(str_sql, device_param_get_info, p_info, NULL);
}
/***************************************************************
 * For websockets client parameters update and query.
 ***************************************************************/

static int websocket_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
									void *arg)
{
	int rc;

	assert((stmt != NULL) && (arg != NULL));
	websocket_client_param_t *p = arg;
	char tmp[128] = {0};
	for (int i = 400; i <= 409; i++) {
		char *bind_ptr = NULL;
		switch (i) {
		case 400:
			bind_ptr = p->sn;
			break;
		case 401:
			bind_ptr = p->seed_secret_no;
			break;
		case 402:
			bind_ptr = p->websocket_url;
			break;
		case 403:
			bind_ptr = p->device_id;
			break;
		case 404:
			bind_ptr = p->static_qrcode_url;
			break;
		case 405:
			bind_ptr = p->secret_no;
			break;
		case 406:
			bind_ptr = p->active_code;
			break;
		case 407:
			bind_ptr = p->firmware_verson;
			break;
		case 408:
			sprintf(tmp, "%d", p->perm_version);
			bind_ptr = tmp;
			break;
		case 409:
			bind_ptr = p->secret;
			break;
		default:
			break;
		}
		sqlite3_bind_text(stmt, 1, bind_ptr, strlen(bind_ptr), NULL);
		sqlite3_bind_int(stmt, 2, i);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("exec id:%d,value:%s error!\n", i, bind_ptr);
			return rc;
		}
		sqlite3_reset(stmt);
	}
	return SQLITE_OK;
}
int db_websocket_client_param_update(websocket_client_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, websocket_info_bind_stmt, p_info);
}

static int websocket_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	websocket_client_param_t *u = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 400:
			db_stmt_get_text(stmt, 2, u->sn);
			break;
		case 401:
			db_stmt_get_text(stmt, 2, u->seed_secret_no);
			break;
		case 402:
			db_stmt_get_text(stmt, 2, u->websocket_url);
			break;
		case 403:
			db_stmt_get_text(stmt, 2, u->device_id);
			break;
		case 404:
			db_stmt_get_text(stmt, 2, u->static_qrcode_url);
			break;
		case 405:
			db_stmt_get_text(stmt, 2, u->secret_no);
			break;
		case 406:
			db_stmt_get_text(stmt, 2, u->active_code);
			break;
		case 407:
			db_stmt_get_text(stmt, 2, u->firmware_verson);
			break;
		case 408:
			u->perm_version = db_stmt_get_int(stmt, 2);
			break;
		case 409:
			db_stmt_get_text(stmt, 2, u->secret);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 10 ? 0 : -1;
}
int db_websocket_client_param_query(websocket_client_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(websocket_client_param_t));
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id>399 AND id <500;");

	return db_query_by_var_param(str_sql, websocket_param_get_info, p_info,
								 NULL);
}

static int perm_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
							   void *arg)
{
	int rc;

	assert((stmt != NULL) && (arg != NULL));
	int *p = arg;
	char tmp[128] = {0};
	sprintf(tmp, "%d", *p);
	sqlite3_bind_text(stmt, 1, tmp, strlen(tmp), NULL);
	sqlite3_bind_int(stmt, 2, 408);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		printf("exec id:408,value:%s error!\n", tmp);
		return rc;
	}
	sqlite3_reset(stmt);

	return SQLITE_OK;
}
int db_perm_version_update(int version)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, perm_info_bind_stmt, &version);
}
static int perm_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	int *perm_ptr = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 408:
			*perm_ptr = db_stmt_get_int(stmt, 2);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 1 ? 0 : -1;
}
int db_perm_version_query(void)
{
	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id=408;");
	int perm_version = 0;
	int ret = db_query_by_var_param(str_sql, perm_param_get_info, &perm_version,
									NULL);
	if (ret == 0)
		return perm_version;
	return -1;
}

/***************************************************************
 * For mode ui parameters update and query.
 ***************************************************************/

static int mode_ui_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
								  void *arg)
{
	int rc;

	assert((stmt != NULL) && (arg != NULL));
	mode_ui_param_t *p = arg;
	for (int i = 500; i <= 501; i++) {
		char tmp[128] = {0};

		switch (i) {
		case 500:
			sprintf(tmp, "%d", p->mode);
			break;
		case 501:
			sprintf(tmp, "%d", p->rotate);
			break;
		default:
			break;
		}

		sqlite3_bind_text(stmt, 1, tmp, strlen(tmp), NULL);
		sqlite3_bind_int(stmt, 2, i);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("exec id:%d,value:%s error!\n", i, tmp);
			return rc;
		}
		sqlite3_reset(stmt);
	}
	return SQLITE_OK;
}

int db_mode_ui_param_update(mode_ui_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, mode_ui_info_bind_stmt, p_info);
}

static int mode_ui_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	mode_ui_param_t *u = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 500:
			u->mode = db_stmt_get_int(stmt, 2);
			break;
		case 501:
			u->rotate = db_stmt_get_int(stmt, 2);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 2 ? 0 : -1;
}

int db_mode_ui_param_query(mode_ui_param_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	assert(p_info != NULL);

	memset(p_info, 0, sizeof(mode_ui_param_t));
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id>499 AND id <600;");

	return db_query_by_var_param(str_sql, mode_ui_param_get_info, p_info, NULL);
}

/***************************************************************
 * For factory mode parameters update and query.
 ***************************************************************/
static int factory_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
								  void *arg)
{
	int rc;

	assert((stmt != NULL) && (arg != NULL));
	int *p = arg;
	char tmp[128] = {0};
	sprintf(tmp, "%d", *p);
	sqlite3_bind_text(stmt, 1, tmp, strlen(tmp), NULL);
	sqlite3_bind_int(stmt, 2, 600);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		printf("exec id:600,value:%s error!\n", tmp);
		return rc;
	}
	sqlite3_reset(stmt);

	return SQLITE_OK;
}
int db_factory_test_update(int version)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, factory_info_bind_stmt, &version);
}
static int factory_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	int *factory_ptr = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 600:
			*factory_ptr = db_stmt_get_int(stmt, 2);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 1 ? 0 : -1;
}
int db_factory_test_query(void)
{
	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id=600;");
	int factory = 0;
	int ret =
		db_query_by_var_param(str_sql, factory_param_get_info, &factory, NULL);
	if (ret == 0)
		return factory;
	return -1;
}
/***************************************************************
 * For debug switch parameters update and query.
 ***************************************************************/
static int debug_info_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
								void *arg)
{
	int rc;

	assert((stmt != NULL) && (arg != NULL));
	int *p = arg;
	char tmp[128] = {0};
	sprintf(tmp, "%d", *p);
	sqlite3_bind_text(stmt, 1, tmp, strlen(tmp), NULL);
	sqlite3_bind_int(stmt, 2, 601);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		printf("exec id:601,value:%s error!\n", tmp);
		return rc;
	}
	sqlite3_reset(stmt);

	return SQLITE_OK;
}
int db_debug_switch_update(int version)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, debug_info_bind_stmt, &version);
}
static int debug_param_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	int *factory_ptr = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 601:
			*factory_ptr = db_stmt_get_int(stmt, 2);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 1 ? 0 : -1;
}
int db_debug_switch_query(void)
{
	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id=601;");
	int factory = 0;
	int ret =
		db_query_by_var_param(str_sql, debug_param_get_info, &factory, NULL);
	if (ret == 0)
		return factory;
	return -1;
}
/***************************************************************
 * For for query sysinfo table of delay time.
 ***************************************************************/
static int delay_time_bind_stmt(sqlite3_stmt *stmt, unsigned int index,
								void *arg)
{
	int rc;

	assert((stmt != NULL) && (arg != NULL));
	int *p = arg;
	char tmp[128] = {0};
	sprintf(tmp, "%d", *p);
	sqlite3_bind_text(stmt, 1, tmp, strlen(tmp), NULL);
	sqlite3_bind_int(stmt, 2, 602);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		printf("exec id:602,value:%s error!\n", tmp);
		return -1;
	}
	sqlite3_reset(stmt);

	return SQLITE_OK;
}
int db_delay_time_update(int relay_time)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE sysinfo SET value=? WHERE id=?;");

	return db_non_query_operator(str_sql, delay_time_bind_stmt, &relay_time);
}
static int delay_time_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	int *factory_ptr = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		int id = db_stmt_get_int(stmt, 0);
		switch (id) {
		case 602:
			*factory_ptr = db_stmt_get_int(stmt, 2);
			break;
		default:
			break;
		}
		count++;
	}

	return count == 1 ? 0 : -1;
}
int db_delay_time_query()
{
	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE id=602;");
	int delay_time = 0;
	int ret =
		db_query_by_var_param(str_sql, delay_time_get_info, &delay_time, NULL);
	if (ret == 0)
		return delay_time;
	return -1;
}

/***************************************************************
 * For for query sysinfo table by key.
 ***************************************************************/
static int query_param_by_key_get_info(sqlite3_stmt *stmt, void *p_info)
{
	int count = 0;

	query_type_t *type_ptr = p_info;

	assert((stmt != NULL) && (p_info != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		switch (*type_ptr) {
		case TYPE_INT: {
			int *int_ptr = p_info;
			*int_ptr = db_stmt_get_int(stmt, 2);
		} break;
		case TYPE_FLT: {
			float *flt_ptr = p_info;
			*flt_ptr = (float)db_stmt_get_double(stmt, 2);
		} break;
		case TYPE_STR: {
			char *chr_ptr = p_info;
			db_stmt_get_text(stmt, 2, chr_ptr);
		} break;
		}
		count++;
	}
	return count == 1 ? 0 : -1;
}
int db_query_sysinfo_by_key(char *key, query_type_t type, void *value)
{
	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "SELECT id,key,value FROM sysinfo WHERE key='%s';", key);
	query_type_t *para_type = value;
	*para_type = type;
	return db_query_by_var_param(str_sql, query_param_by_key_get_info, value,
								 NULL);
}
/***************************************************************
 * Admin table related
 ***************************************************************/
static int admin_query_info(sqlite3_stmt *stmt, void *ee_head)
{
	int count = 0;

	ee_queue_t *h = ee_head;

	assert((stmt != NULL) && (ee_head != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		/* // int id = db_stmt_get_int(stmt,0); */
		websocket_admin_info_t *u = malloc(sizeof(websocket_admin_info_t));
		memset(u, 0x00, sizeof(websocket_admin_info_t));
		db_stmt_get_text(stmt, 0, u->user_id);
		db_stmt_get_text(stmt, 1, u->name);
		ee_queue_insert_tail(h, &(u->queue));
		count++;
	}
	return count;
}
int db_websocket_admin_info_query_begin(ee_queue_t *head)
{
	if (head == NULL)
		return -1;
	ee_queue_t *h = head;
	ee_queue_init(h);

	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql), "SELECT key,value FROM admin;");

	return db_query_by_var_param(str_sql, admin_query_info, h, NULL);
}

int db_websocket_admin_info_query_end(ee_queue_t *head)
{
	if (head == NULL)
		return -1;
	ee_queue_t *h = head, *pos, *n;

	websocket_admin_info_t *p = NULL;
	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, websocket_admin_info_t, queue);
		free(p);
	}
	ee_queue_init(head);
	return 0;
}

int db_websocket_admin_info_del()
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql), "DELETE FROM admin;");

	return db_non_query_operator(str_sql, NULL, NULL);
}
static int admin_insert_info(sqlite3_stmt *stmt, unsigned int index,
							 void *ee_head)
{
	assert((stmt != NULL) && (ee_head != NULL));

	ee_queue_t *h = (ee_queue_t *)ee_head, *pos;
	/* // ee_queue_init(h); */
	int count = 0;
	int rc = SQLITE_OK;
	websocket_admin_info_t *p = NULL;
	ee_queue_for_each(pos, h) {
		p = ee_queue_data(pos, websocket_admin_info_t, queue);
		ALOGD("exec %s:%s...\n", p->user_id, p->name);
		sqlite3_bind_text(stmt, 1, p->user_id, strlen(p->user_id), NULL);
		sqlite3_bind_text(stmt, 2, p->name, strlen(p->name), NULL);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			ALOGD("exec %s:%s error!\n", p->user_id, p->name);
			return rc;
		}
		count++;
		sqlite3_reset(stmt);
	}
	return rc;
}
int db_websocket_admin_info_insert(ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));
	db_websocket_admin_info_del();
	snprintf(str_sql, sizeof(str_sql),
			 "INSERT INTO admin (key,value) VALUES(?,?);");

	return db_non_query_operator(str_sql, admin_insert_info, head);
}

static int person_query_info(sqlite3_stmt *stmt, void *ee_head)
{
	int count = 0;

	ee_queue_t *h = ee_head;

	assert((stmt != NULL) && (ee_head != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		websocket_person_info_t *u = malloc(sizeof(websocket_person_info_t));
		memset(u, 0x00, sizeof(websocket_person_info_t));
		/*userid,name,user_type,add_type,add_time,openvid,
		face_feature,face_key_points,face_norm0,face_norm1,pic_large
		pic_small, face_id */
		/*
	char user_id[64];
	char name[32];
	int  user_type;
	long long openvid;
	char pic_large[32];
	char pic_small[32];
	int  add_source;
	int  add_time;
	int  face_id;
	char face_feature[1024];
	float key_points[10];
	float face_norm[2];
	*/
		db_stmt_get_text_v2(stmt, 0, u->user_id, 64);
		db_stmt_get_text(stmt, 1, u->name);
		/* db_stmt_get_text_v2(stmt,1,u->name, 32); */
		u->user_type = db_stmt_get_int(stmt, 2);
		u->add_source = db_stmt_get_int(stmt, 3);
		u->add_time = db_stmt_get_int(stmt, 4);
		u->openvid = db_stmt_get_int64(stmt, 5);
		db_stmt_get_blob_v2(stmt, 6, (unsigned char *)u->face_feature,
							1024 * sizeof(char));
		/* //db_stmt_get_blob(stmt,6,(unsigned char*)u->face_feature); */
		db_stmt_get_blob_v2(stmt, 7, (unsigned char *)u->key_points,
							10 * sizeof(float));
		db_stmt_get_blob_v2(stmt, 8, (unsigned char *)&u->face_norm[0],
							sizeof(float));
		db_stmt_get_blob_v2(stmt, 9, (unsigned char *)&u->face_norm[1],
							sizeof(float));
		db_stmt_get_text_v2(stmt, 10, u->pic_large, 32);
		db_stmt_get_text_v2(stmt, 11, u->pic_small, 32);
		u->face_id = db_stmt_get_int(stmt, 12);
		ee_queue_insert_tail(h, &(u->queue));
		count++;
	}
	return count;
}
int db_websocket_person_info_query_begin(ee_queue_t *head, int type)
{
	if (head == NULL)
		return -1;

	ee_queue_t *h = head;
	ee_queue_init(h);

	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
			 "SELECT userid,name,user_type,add_type,add_time,openvid,"
			 "face_feature,face_key_points,face_norm0,face_norm1,pic_large,"
			 "pic_small, face_id FROM %s;",
			 type == TYPE_PERSON ? EMPLOYEE_TABLE : GUEST_TABLE);

	return db_query_by_var_param(str_sql, person_query_info, h, NULL);
}
int db_websocket_person_info_query_end(ee_queue_t *head, int type)
{
	if (head == NULL)
		return -1;
	ee_queue_t *h = head, *pos, *n;

	websocket_person_info_t *p = NULL;
	ee_queue_for_each_safe(pos, n, h) {
		/* //		  p = ((websocket_admin_info_t *) ((char *)pos -
		// offsetof(websocket_admin_info_t, queue))); */
		p = ee_queue_data(pos, websocket_person_info_t, queue);
		free(p);
	}
	ee_queue_init(head);
	return 0;
}
int db_websocket_person_info_del_by_user_id(char *user_id)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "DELETE FROM employee WHERE userid='%s';", user_id);

	return db_non_query_operator(str_sql, NULL, NULL);
}
int db_websocket_person_info_del(int type)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql), "DELETE FROM %s;",
			 type == TYPE_PERSON ? EMPLOYEE_TABLE : GUEST_TABLE);

	return db_non_query_operator(str_sql, NULL, NULL);
}
static int person_insert_info(sqlite3_stmt *stmt, unsigned int index,
							  void *ee_head)
{
	assert((stmt != NULL) && (ee_head != NULL));

	ee_queue_t *h = (ee_queue_t *)ee_head, *pos;
	/* // ee_queue_init(h); */
	int count = 0;
	int rc;
	websocket_person_info_t *p = NULL;
	ee_queue_for_each(pos, h) {
		/* //		  p = ((websocket_admin_info_t *) ((char *)pos -
		// offsetof(websocket_admin_info_t, queue))); */
		p = ee_queue_data(pos, websocket_person_info_t, queue);
		/*	   "INSERT INTO %s (userid,name,user_type,face_feature,"
		 "face_key_points, face_norm0, face_norm1, pic_large, "
		 "pic_small, openvid, add_type, add_time, face_id) "
		 "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)"*/

		sqlite3_bind_text(stmt, 1, p->user_id, strlen(p->user_id), NULL);
		sqlite3_bind_text(stmt, 2, p->name, strlen(p->name), NULL);
		sqlite3_bind_int(stmt, 3, p->user_type);
		sqlite3_bind_blob(stmt, 4, p->face_feature, 1024, SQLITE_STATIC);
		sqlite3_bind_blob(stmt, 5, p->key_points, sizeof(float) * 10,
						  SQLITE_STATIC);
		sqlite3_bind_double(stmt, 6, (double)p->face_norm[0]);
		sqlite3_bind_double(stmt, 7, (double)p->face_norm[1]);

		sqlite3_bind_text(stmt, 8, p->pic_large, strlen(p->pic_large), NULL);
		sqlite3_bind_text(stmt, 9, p->pic_small, strlen(p->pic_small), NULL);
		sqlite3_bind_int64(stmt, 10, p->openvid);
		sqlite3_bind_int(stmt, 11, p->add_source);
		sqlite3_bind_int(stmt, 12, p->add_time);
		sqlite3_bind_int(stmt, 13, p->face_id);

		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("exec %s:%s error!\n", p->user_id, p->name);
			return rc;
		}
		count++;
		sqlite3_reset(stmt);
	}
	return rc;
}
int db_websocket_person_info_insert(ee_queue_t *head)
{
	char str_sql[DB_SQL_MAX_LEN];
	int type =
		ee_queue_data(head->next, websocket_person_info_t, queue)->user_type;
	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "INSERT INTO %s (userid,name,user_type,face_feature,"
			 "face_key_points, face_norm0, face_norm1, pic_large, "
			 "pic_small, openvid, add_type, add_time, face_id) "
			 "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)",
			 type == TYPE_PERSON ? EMPLOYEE_TABLE : GUEST_TABLE);
	ALOGD("[%s:%d] %s\n", __func__, __LINE__, str_sql);
	return db_non_query_operator(str_sql, person_insert_info, head);
}
static int person_update_feat(sqlite3_stmt *stmt, unsigned int index,
							void *p_head)
{
	assert((stmt != NULL) && (p_head != NULL));
	int rc = SQLITE_OK;
	websocket_person_info_t *p = p_head;
	sqlite3_bind_blob(stmt, 1, p->face_feature, 1024, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 2, p->key_points, sizeof(float) * 10,
					  SQLITE_STATIC);
	sqlite3_bind_double(stmt, 3, (double)p->face_norm[0]);
	sqlite3_bind_double(stmt, 4, (double)p->face_norm[1]);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		printf("exec %s:%s error!\n", p->user_id, p->name);
		return rc;
	}
	sqlite3_reset(stmt);
	return rc;
}
int db_websocket_person_info_update_feat(websocket_person_info_t *p_info)
{
	assert(p_info != NULL);
	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
			 "UPDATE %s SET face_feature=?, face_key_points=?, face_norm0=?,"
			 "face_norm1=? WHERE userid='%s'",
			 EMPLOYEE_TABLE, p_info->user_id);
	return db_non_query_operator(str_sql, person_update_feat, p_info);
}
int db_websocket_person_info_query_begin_by_name(char *user_name,
		ee_queue_t *head)
{
	if (head == NULL)
		return -1;

	ee_queue_t *h = head;
	ee_queue_init(h);

	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
			 "SELECT userid,name,user_type,add_type,add_time,openvid,"
			 "face_feature,face_key_points,face_norm0,face_norm1,pic_large,"
			 "pic_small, face_id  FROM %s WHERE name='%s';",
			 EMPLOYEE_TABLE, user_name);

	return db_query_by_var_param(str_sql, person_query_info, h, NULL);
}
/*//person
int db_websocket_person_info_del_by_user_id(char *user_id);
//visitor  1 2
int db_websocket_person_info_del(int type);
//
int db_websocket_person_info_insert(ee_queue_t *head);
int db_websocket_person_info_update_feat(websocket_person_info_t *p_info)*/

/***************************************************
 *  rules
 **************************************************/
static int rule_query_info(sqlite3_stmt *stmt, void *ee_head)
{
	int count = 0;

	ee_queue_t *h = ee_head;

	assert((stmt != NULL) && (ee_head != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		web_socket_rule_info_t *u = malloc(sizeof(web_socket_rule_info_t));
		memset(u, 0x00, sizeof(web_socket_rule_info_t));
		db_stmt_get_text_v2(stmt, 0, u->user_id, 64);
		u->rule_id = db_stmt_get_int(stmt, 1);
		db_stmt_get_text_v2(stmt, 2, u->pass_rule, 64);
		u->effect_time = db_stmt_get_int(stmt, 3);
		ee_queue_insert_tail(h, &(u->queue));
		count++;
	}
	return count;
}
int db_websocket_rule_info_query_begin(ee_queue_t *head)
{
	if (head == NULL)
		return -1;

	ee_queue_t *h = head;
	ee_queue_init(h);

	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
			 "SELECT userid,rule_id,rule,effect_time "
			 "FROM %s",
			 RULE_TABLE);

	return db_query_by_var_param(str_sql, rule_query_info, h, NULL);
}
int db_websocket_rule_info_query_end(ee_queue_t *head)
{
	if (head == NULL)
		return -1;
	ee_queue_t *h = head, *pos, *n;

	web_socket_rule_info_t *p = NULL;
	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, web_socket_rule_info_t, queue);
		free(p);
	}
	ee_queue_init(head);
	return 0;
}

int db_websocket_rule_info_del_by_user_id(char *user_id)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql), "DELETE FROM %s WHERE userid='%d';",
			 RULE_TABLE, user_id);

	return db_non_query_operator(str_sql, NULL, NULL);
}
static int rule_insert_info(sqlite3_stmt *stmt, unsigned int index,
							void *ee_head)
{
	assert((stmt != NULL) && (ee_head != NULL));

	ee_queue_t *h = (ee_queue_t *)ee_head, *pos;
	/* // ee_queue_init(h); */
	int count = 0;
	int rc;
	web_socket_rule_info_t *p = NULL;
	ee_queue_for_each(pos, h) {
		p = ee_queue_data(pos, web_socket_rule_info_t, queue);

		sqlite3_bind_text(stmt, 1, p->user_id, strlen(p->user_id), NULL);
		sqlite3_bind_int64(stmt, 2, p->rule_id);
		sqlite3_bind_text(stmt, 3, p->pass_rule, strlen(p->pass_rule), NULL);
		sqlite3_bind_int64(stmt, 4, p->effect_time);

		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			printf("exec %s:%s error!\n", p->user_id, p->pass_rule);
			return rc;
		}
		count++;
		sqlite3_reset(stmt);
	}
	return rc;
}
int db_websocket_rule_info_insert(ee_queue_t *head, int type)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql),
			 "INSERT INTO %s (userid,name,user_type,face_feature,"
			 "face_key_points, face_norm0, face_norm1, pic_large, "
			 "pic_small, openvid, add_type, add_time, face_id) "
			 "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)",
			 RULE_TABLE);
	ALOGD("[%s:%d] %s\n", __func__, __LINE__, str_sql);
	return db_non_query_operator(str_sql, rule_insert_info, head);
}

/***************************************************
 *  checkins
 **************************************************/
static int checkin_query_info(sqlite3_stmt *stmt, void *ee_head)
{
	int count = 0;

	ee_queue_t *h = ee_head;

	assert((stmt != NULL) && (ee_head != NULL));

	while (SQLITE_ROW == sqlite3_step(stmt)) {
		web_socket_checkin_info_t *u =
			malloc(sizeof(web_socket_checkin_info_t));
		memset(u, 0x00, sizeof(web_socket_checkin_info_t));

		db_stmt_get_text_v2(stmt, 0, u->user_id, 64);
		u->user_type = db_stmt_get_int(stmt, 1);
		u->timestamp = db_stmt_get_int(stmt, 2);
		u->is_upload = db_stmt_get_int(stmt, 3);
		u->upload_time = db_stmt_get_int(stmt, 4);
		db_stmt_get_text_v2(stmt, 5, u->face_path, 128);
		ee_queue_insert_tail(h, &(u->queue));
		count++;
	}
	return count;
}
int db_websocket_checkin_info_query_begin(ee_queue_t *head)
{
	if (head == NULL)
		return -1;

	ee_queue_t *h = head;
	ee_queue_init(h);

	char str_sql[DB_SQL_MAX_LEN];
	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
			 "SELECT userid,user_type,check_in_time,is_uploaded,upload_time,face_path"
			 "FROM %s LIMIT 199 OFFSET 0",
			 CHECKIN_TABLE);

	return db_query_by_var_param(str_sql, checkin_query_info, h, NULL);
}
int db_websocket_checkin_info_query_end(ee_queue_t *head)
{
	if (head == NULL)
		return -1;
	ee_queue_t *h = head, *pos, *n;

	web_socket_checkin_info_t *p = NULL;
	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, web_socket_checkin_info_t, queue);
		free(p);
	}
	ee_queue_init(head);
	return 0;
}

/* // int db_websocket_checkin_info_del_by_user_id(char *user_id); */
int db_websocket_checkin_info_del_by_day(int days)
{
	return 0;
}

int db_websocket_checkin_info_del_by_time(int timestamp)
{
	return 0;
}

int db_websocket_checkin_info_del_table()
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql), "DELETE FROM %s;", CHECKIN_TABLE);

	return db_non_query_operator(str_sql, NULL, NULL);
}
static int checkin_insert_info(sqlite3_stmt *stmt, unsigned int index,
								void *p_info)
{
	assert((stmt != NULL) && (p_info != NULL));

	int rc = SQLITE_OK;
	web_socket_checkin_info_t *p = p_info;
	/* //"SELECT userid,user_type,check_in_time,is_uploaded, upload_time," */
	sqlite3_bind_text(stmt, 1, p->user_id, strlen(p->user_id), NULL);
	sqlite3_bind_int(stmt, 2, p->user_type);
	sqlite3_bind_int(stmt, 3, p->timestamp);
	sqlite3_bind_int(stmt, 4, p->is_upload);
	sqlite3_bind_int(stmt, 5, p->upload_time);
	sqlite3_bind_text(stmt, 6, p->face_path, strlen(p->face_path), NULL);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		ALOGD("exec %s:%d error!\n", p->user_id, p->timestamp);
		return rc;
	}

	sqlite3_reset(stmt);

	return rc;
}
int db_websocket_checkin_info_insert(web_socket_checkin_info_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));
	snprintf(str_sql, sizeof(str_sql),
			 "INSERT INTO %s (userid,user_type,check_in_time,is_uploaded,"
			 "upload_time,face_path) VALUES(?,?,?,?,?,?);",
			 CHECKIN_TABLE);

	return db_non_query_operator(str_sql, checkin_insert_info, p_info);
}

int db_websocket_checkin_info_del(web_socket_checkin_info_t *p_info)
{
	char str_sql[DB_SQL_MAX_LEN];

	memset(str_sql, 0, sizeof(str_sql));

	snprintf(str_sql, sizeof(str_sql), "DELETE FROM %s WHERE userid='%s';",
			 CHECKIN_TABLE, p_info->user_id);

	return db_non_query_operator(str_sql, NULL, NULL);
}

void db_all_init()
{
	ALOGD(
		"%s, this function is deprecated,"
		"call db_init() alternatively!",
		__func__);
	db_init();
}

void db_deinit()
{
	ALOGD(
		"%s, With this version c api,"
		"this function is no need to be called!",
		__func__);
}

#endif
