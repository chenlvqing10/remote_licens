/*
 * client/cloud_biz_api.c - all functions for system configurations
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "http.h"
#include "sys_http.h"
#include "cJSON.h"

#include "recognition.h"
#include "person.h"
#include "common_record_info.h"
#include "record_info.h"
#include "lb_os_http.h"
#include "face_db.h"
#include "base64.h"
#include "face.h"

#include "net_utils.h"
#include "ip_config.h"
#include "time_conv.h"
#include "record_db.h"
#include "db_punch_record.h"
#include "config_db.h"

#include "file_ops.h"
#include "take_photo.h"
#include "web_biz_api.h"
#include "db_face_recognition_param.h"

#define LOG_TAG "cloud_biz_api"
#include <log/log.h>

#define DEV_NAME 	"eth0"

static int recv_ret;
static int reg_ret;
static int inmems_ret;
static int changed_ret;
static int upload_ret;
static int import_totalcount;
static int import_okcount;

static char *inmemsret_str;

/**
 * cloud_import_mems_ret: request for importing members result to cloud server
 *
 * Returns 0 means success, other means failure.
 */
static int cloud_import_mems_ret(void);

/**
 * str_to_json: json string parse to json object
 * @msg: json string
 * @len: string length
 * @obj_json: json object
 *
 * Returns 0 means success, other means failure.
 */
int str_to_json(char *msg, long len, cJSON **obj_json)
{
	char *p1 = NULL;

	ALOGD("[str_to_json]len = %ld\n", len);

	p1 = strstr(msg, "[{\"");
	if (p1 == NULL) {
		ALOGD("p1 == NULL\n");
		*obj_json = NULL;
		return -1;
	}
	*obj_json = cJSON_Parse(p1);
	if (*obj_json == NULL) {
		ALOGD("create obj_json fail\n");
		return -1;
	}
	return 0;
}

/**
 * remove_file: sync file
 * path: file name
 *
 * Returns 0: success, -1: failure.
 */
int truncate_file(char *path)
{
	int fd = 0;
	fd = open(path, O_WRONLY|O_TRUNC, 0666);
	if (fd <= 0) {
		ALOGE("open file error.\n");
		return -1;
	}

	ftruncate(fd, 0);
	lseek(fd, 0, SEEK_SET);

	fsync(fd);
	close(fd);

	return 0;
}

/**
 * get_mac_addr: get mac addr
 *
 * Returns mac addr.
 */
static const char *get_mac_addr(void)
{
	int ret = 0;
	unsigned char addr_str[6];
	static char mac_addr[40];

	memset(mac_addr, 0, sizeof(mac_addr));
	strcpy(mac_addr, "02:03:04:05:06:07");
	
	memset(addr_str, 0, sizeof(addr_str));
	ret = net_dev_get_mac_addr(DEV_NAME, addr_str);
	if (0 == ret)
		snprintf(mac_addr, sizeof(mac_addr),
			"%02X:%02X:%02X:%02X:%02X:%02X",
			addr_str[0],
			addr_str[1],
			addr_str[2],
			addr_str[3],
			addr_str[4],
			addr_str[5]);

	return mac_addr;
}

/**
 * cloudimportmems_recv_deal: import person information to db
 * @sockfd: socket handle
 * @msg: message string from network
 * @len: message string length
 *
 * Returns 0 means success, other means failure.
 */
int cloudimportmems_recv_deal(int sockfd, char *msg, long len)
{
	int ret = 0, failcount;
	img_info_t st_img;
	e_person_reg_code e_code = PERSON_REG_OK;
	char detail_info[64];
	char e_msg[128] = {0};

	long base64_pic_len = 0;
	char *pic = NULL;
	char *decode = NULL;
	int pic_len = 0;
	int arr_count = 0;
	int i = 0;

	cJSON *obj = NULL;
	cJSON *obj_arr = NULL;
	cJSON *obj_item = NULL;
	cJSON *obj_ret_tmp = NULL;
	cJSON *obj_ret_arr = NULL;
	cJSON *obj_ret_top = NULL;

	person_t pst_person;
	person_reg_param_t reg_param;

	memset(&reg_param, 0, sizeof(reg_param));
	person_reg_param_init(&reg_param);
	ALOGD("[cloudimportmems_recv_deal]reg_param->threshold=%f\n",
		reg_param.threshold);

	memset(&pst_person, 0, sizeof(pst_person));
	ALOGD("recv msg len = %ld\n", len);
	decode = (char *)malloc(len);
	if (decode == NULL)
		ALOGD("[%s : %d]", __func__, __LINE__);

	urldecode((unsigned char *)msg, (unsigned char *)decode);
	str_to_json(decode, len, &obj_arr);
	if (obj_arr == NULL) {
		ALOGD("create obj_arr fail\n");
		ret = -1;
		goto Exit;
	}
	arr_count = cJSON_GetArraySize(obj_arr);
	import_totalcount = arr_count;
	ALOGD("arr_count = %d\n", arr_count);

	obj_ret_arr = cJSON_CreateArray();
	if (obj_ret_arr == NULL) {
		ALOGD("create obj_ret_arr fail\n");
		ret = -1;
		goto Exit;
	}
	obj_ret_top = cJSON_CreateObject();
	if (obj_ret_top == NULL) {
		ALOGD("create obj_ret_top fail\n");
		ret = -1;
		goto Exit;
	}

	reg_ret = 0;
	for (i = 0; i < arr_count; i++) {
		obj_item = cJSON_GetArrayItem(obj_arr, i);
		if (obj_item == NULL)
			continue;

		obj = cJSON_GetObjectItem(obj_item, "employee_number");
		if (obj) {
			memset(pst_person.employee_number, 0,
				sizeof(pst_person.employee_number));
			memcpy(pst_person.employee_number, obj->valuestring,
				strlen(obj->valuestring));
			ALOGD("employee_number = %s\n", pst_person.employee_number);
		}

		obj = cJSON_GetObjectItem(obj_item, "name");
		if (obj) {
			memset(pst_person.name, 0, sizeof(pst_person.name));
			memcpy(pst_person.name, obj->valuestring,
				strlen(obj->valuestring));
			ALOGD("name = %s\n", pst_person.name);
		}

		obj = cJSON_GetObjectItem(obj_item, "gender");
		if (obj) {
			memset(pst_person.gender, 0, sizeof(pst_person.gender));
			memcpy(pst_person.gender, obj->valuestring,
				strlen(obj->valuestring));
			ALOGD("gender = %s\n", pst_person.gender);
		}

		obj = cJSON_GetObjectItem(obj_item, "nationa");
		if (obj) {
			memset(pst_person.nationa, 0, sizeof(pst_person.nationa));
			memcpy(pst_person.nationa, obj->valuestring,
				strlen(obj->valuestring));
			ALOGD("nationa = %s\n", pst_person.nationa);
		}

		obj = cJSON_GetObjectItem(obj_item, "id_card_number");
		if (obj) {
			memset(pst_person.id_card_number, 0,
				sizeof(pst_person.id_card_number));
			memcpy(pst_person.id_card_number, obj->valuestring,
				strlen(obj->valuestring));
			ALOGD("id_card_number = %s\n", pst_person.id_card_number);
		}

		obj = cJSON_GetObjectItem(obj_item, "mobile");
		if (obj) {
			memset(pst_person.mobile, 0, sizeof(pst_person.mobile));
			memcpy(pst_person.mobile, obj->valuestring,
				strlen(obj->valuestring));
			ALOGD("mobile = %s\n", pst_person.mobile);
		}

		obj = cJSON_GetObjectItem(obj_item, "access_right");
		if (obj) {
			pst_person.access_right = obj->valueint;
			ALOGD("access_right = %d\n", pst_person.access_right);
		}

		obj = cJSON_GetObjectItem(obj_item, "temporary_access_start_time");
		if (obj) {
			pst_person.temporary_access_start_time = obj->valuedouble;
			ALOGD("temporary_access_start_time = %ld\n",
				pst_person.temporary_access_start_time);
		}

		obj = cJSON_GetObjectItem(obj_item, "temporary_access_end_time");
		if (obj) {
			pst_person.temporary_access_end_time = obj->valuedouble;
			ALOGD("temporary_access_end_time = %ld\n",
				pst_person.temporary_access_end_time);
		}

		obj = cJSON_GetObjectItem(obj_item, "temporary_access_times");
		if (obj) {
			pst_person.temporary_access_times = obj->valueint;
			ALOGD("temporary_access_end_time = %d\n",
				pst_person.temporary_access_times);
		}

		obj = cJSON_GetObjectItem(obj_item, "remarks");
		if (obj) {
			memset(pst_person.remarks, 0, sizeof(pst_person.remarks));
			memcpy(pst_person.remarks, obj->valuestring,
				strlen(obj->valuestring));
			ALOGD("remarks = %s\n", pst_person.remarks);
		}

		obj = cJSON_GetObjectItem(obj_item, "base64_pic_len");
		if (obj) {
			base64_pic_len = obj->valueint;
			ALOGD("base64_pic_len = %ld\n", base64_pic_len);
		}

		obj = cJSON_GetObjectItem(obj_item, "base64_pic");
		if (obj) {
			base64_decode(obj->valuestring, strlen(obj->valuestring),
				&pic, &pic_len);
			append_pic_data_to_default_file_cloud(pic, pic_len);
			ALOGD("pic_len = %d\n", pic_len);
			if (pic)
				lb_mem_free(pic);
		}
		strcpy(st_img.pic_path, REG_PIC_DEFAULT_PATH2);
		ALOGD("[person_reg_proc]pst_person name=%s\n", pst_person.name);
		ALOGD("name = %s\n", pst_person.name);

		memset(detail_info, 0, sizeof(detail_info));
		ret = person_reg_proc(&pst_person, &st_img, detail_info,
			sizeof(detail_info), &reg_param, &e_code, 0);
		if (ret == 0)
			import_okcount += 1;

		reg_ret += ret;

		obj_ret_tmp = cJSON_CreateObject();
		cJSON_AddStringToObject(obj_ret_tmp,
			"employee_number", pst_person.employee_number);
		memset(e_msg, 0, sizeof(e_msg));
		person_reg_fail_reason(e_code, e_msg, sizeof(e_msg));
		cJSON_AddNumberToObject(obj_ret_tmp, "success", (int)e_code);
		cJSON_AddStringToObject(obj_ret_tmp, "msg", e_msg);
		cJSON_AddItemToArray(obj_ret_arr, obj_ret_tmp);

		ALOGD("[person_reg_proc]ret=%d\n", ret);
		ALOGD("[person_reg_proc]detail_info=%s\n", detail_info);
		ALOGD("[person_reg_proc]e_code=%d\n", (int)e_code);
	}

	if (face_db_update())
		ALOGE("[%s:%d]update db failed", __func__, __LINE__);

	failcount = import_totalcount - import_okcount;
	ALOGD("import_totalcount = %d\n", import_totalcount);
	ALOGD("import_failcount = %d\n", failcount);
	cJSON_AddStringToObject(obj_ret_top, "mac", get_mac_addr());
	cJSON_AddItemToObject(obj_ret_top, "data", obj_ret_arr);
	cJSON_AddNumberToObject(obj_ret_top, "totalcount", import_totalcount);
	cJSON_AddNumberToObject(obj_ret_top, "failcount", failcount);

	inmemsret_str = cJSON_PrintUnformatted(obj_ret_top);
	if (inmemsret_str)
		ALOGD("inmemsret_str=%s\n", inmemsret_str);
Exit:
	remove_file(REG_PIC_DEFAULT_PATH2);
	if (obj_arr)
		cJSON_Delete(obj_arr);
	if (obj_ret_top)
		cJSON_Delete(obj_ret_top);
	recv_ret = ret;
	return ret;
}

/**
 * cloudimportmemsret_recv_deal: process result of import members
 * result from cloud server
 * @sockfd: socket handle
 * @msg: message string from network
 * @len: message string length
 *
 * Returns 0 means success, other means failure.
 */
int cloudimportmemsret_recv_deal(int sockfd, char *msg, long  len)
{
	char *p1 = NULL;
	cJSON *obj_item = NULL;
	cJSON *obj = NULL;

	ALOGD("recv msg len = %ld\n", len);
	ALOGD("recv msg = %s\n", msg);

	p1 = strstr(msg, "{\"");
	if (p1 == NULL)
		ALOGE("p1 == NULL\n");

	obj_item = cJSON_Parse(p1);
	if (obj_item == NULL) {
		ALOGE("create obj_item fail\n");
		inmems_ret = -1;
		return -1;
	}

	obj = cJSON_GetObjectItem(obj_item, "success");
	if (obj) {
		if (obj->valuestring)
			upload_ret = atoi(obj->valuestring);
		else
			upload_ret = obj->valueint;
		ALOGD("upload_ret = %d\n", upload_ret);
	}
	if (obj_item)
		cJSON_Delete(obj_item);

	return inmems_ret;

}

void write_debug_log(const char *buffer, int len)
{
	FILE *fp = fopen("/data/test.log", "ab+");
	if (fp) {
		fwrite(buffer, 1, len, fp);
		fclose(fp);
	}
}

/**
 * clouduploadrec_recv_deal: process result of upload
 * punch record to cloud server
 * @sockfd: socket handle
 * @msg: message string from network
 * @len: message string length
 *
 * Returns 0 means success, other means failure.
 */
int clouduploadrec_recv_deal(int sockfd, char *msg, long  len)
{
	char *p1 = NULL;
	cJSON *obj_item = NULL;
	cJSON *obj = NULL;
	char *decode = NULL;

	write_debug_log("recv mesg:\n", 20);
	write_debug_log(msg, len);

	ALOGD("recv msg len = %ld\n", len);
	ALOGD("recv msg = %s\n", msg);

	decode = (char *)malloc(len + 1);
	if (NULL == decode) {
		upload_ret = -1;
		return -1;
	}

	memset(decode, 0, len + 1);
	urldecode((unsigned char *)msg, (unsigned char *)decode);
	/*
	write_debug_log(decode, strlen(decode));
	*/
	p1 = strstr(decode, "{\"");
	if (p1 == NULL)
		ALOGD("p1 == NULL\n");

	obj_item = cJSON_Parse(p1);
	if (obj_item == NULL) {
		ALOGD("create obj_item fail\n");
		free(decode);
		upload_ret = -1;
		return -1;
	}

	obj = cJSON_GetObjectItem(obj_item, "success");
	if (obj) {
		if (obj->valuestring)
			upload_ret = atoi(obj->valuestring);
		else
			upload_ret = obj->valueint;
		ALOGD("upload_ret = %d\n", upload_ret);
	}
	if (obj_item)
		cJSON_Delete(obj_item);

	free(decode);

	return upload_ret;
}

/**
 * get_ip_addr: get ip addr
 *
 * Returns ip addr.
 */
extern int net_dev_get_ip_addr(const char *dev_name, unsigned long *ip_addr);
static const char *get_ip_addr(void)
{
	static char ip_addr_str[20];
	int ret;
	unsigned long ip_addr;

	strcpy(ip_addr_str, "192.168.1.11");
	
	ret = net_dev_get_ip_addr(DEV_NAME, &ip_addr);
	if (0 == ret)
		snprintf(ip_addr_str, sizeof(ip_addr_str),
			"%d.%d.%d.%d",
			(unsigned int)(ip_addr & 0xff),
			(unsigned int)((ip_addr >> 8) & 0xff),
			(unsigned int)((ip_addr >> 16) & 0xff),
			(unsigned int)((ip_addr >> 24) & 0xff));
	/*
	printf("current device ip_addr %s\n", ip_addr_str);
	*/
	return ip_addr_str;
}

/**
 * cloud_ipaddr_changed_recv_deal: process result of notify
 * ipaddr changed to cloud server
 * @sockfd: socket handle
 * @msg: message string from network
 * @len: message string length
 *
 * Returns 0 means success, other means failure.
 */
int cloud_ipaddr_changed_recv_deal(int sockfd, char *msg, long  len)
{
	char *p1 = NULL;
	cJSON *obj_item = NULL;
	cJSON *obj = NULL;

	write_debug_log("recv mesg:\n", 20);
	write_debug_log(msg, len);

	ALOGD("recv msg len = %ld\n", len);
	ALOGD("recv msg = %s\n", msg);

	p1 = strstr(msg, "{\"");
	if (p1 == NULL)
		ALOGD("p1 == NULL\n");

	obj_item = cJSON_Parse(p1);
	if (obj_item == NULL) {
		ALOGD("create obj_item fail\n");
		changed_ret = -1;
		return -1;
	}

	obj = cJSON_GetObjectItem(obj_item, "success");
	if (obj) {
		if (obj->valuestring)
			changed_ret = atoi(obj->valuestring);
		else
			changed_ret = obj->valueint;
		ALOGD("changed_ret = %d\n", changed_ret);
	}
	if (obj_item)
		cJSON_Delete(obj_item);

	return changed_ret;
}

/**
 * cloud_ipaddr_changed: notify for device ip addr has been changed.
 *
 * Returns 0 means success, other means failure.
 */
int cloud_ipaddr_changed()
{
	int ret;
	char str_url[256] = {0};
	cJSON *obj;
	char *request_str = NULL;
	static char ip_addr[20] = {0};
	char new_ip_addr[20];

	memset(new_ip_addr, 0, sizeof(new_ip_addr));
	strcpy(new_ip_addr, get_ip_addr());
	if (0 == strcmp(new_ip_addr, ip_addr))
		return -1;

#if 0
	get_ini_value("cloud_ipaddr_changed_url", str_url, sizeof(str_url));
#else
	get_cloud_ipaddr_changed_url(str_url, sizeof(str_url));
#endif
	ALOGD("[%s:%d]cloud_ipaddr_changed_url = %s\n",
		__func__, __LINE__, str_url);

	obj = cJSON_CreateObject();

	cJSON_AddStringToObject(obj, "mac", get_mac_addr());
	cJSON_AddStringToObject(obj, "ipaddr", new_ip_addr);
	request_str = cJSON_PrintUnformatted(obj);
	if (request_str) {
		ret = send_request(str_url, request_str,
			strlen(request_str),
			NULL);
		if (0 == ret)
			strcpy(ip_addr, new_ip_addr);
		free(request_str);
	}

	if (obj)
		cJSON_Delete(obj);

	return 0;
}

/**
 * cloud_import_mems: request for importing members from cloud server
 *
 * Returns 0 means success, other means failure.
 */
int cloud_import_mems(void)
{
	char str_url[256] = {0};

	import_totalcount = 0;
	import_okcount = 0;

#if 0
	get_ini_value("cloud_import_mems_url", str_url, sizeof(str_url));
#else
	get_cloud_import_mems_url(str_url, sizeof(str_url));
#endif
	ALOGD("[cloud_import_mems]cloud_import_mems_url = %s\n", str_url);

#if 1
	cJSON *obj;
	char *request_str = NULL;

	obj = cJSON_CreateObject();

	cJSON_AddStringToObject(obj, "mac", get_mac_addr());
	cJSON_AddNumberToObject(obj, "last_employee_number", 0);
	request_str = cJSON_PrintUnformatted(obj);
	if (request_str) {
		send_request(str_url, request_str,
			strlen(request_str), cloudimportmems_recv_deal);
		free(request_str);
	}

	if (obj)
		cJSON_Delete(obj);

	ALOGD("[cloud_import_mems]reg_ret=%d\n", reg_ret);
	cloud_import_mems_ret();
#else
	send_request(str_url, "", 0, cloudimportmems_recv_deal);
	ALOGD("[cloud_import_mems]reg_ret=%d\n", reg_ret);
#endif

	return reg_ret;
}

/**
 * cloud_import_mems_ret: request for importing members result to cloud server
 *
 * Returns 0 means success, other means failure.
 */
int cloud_import_mems_ret(void)
{
	char str_url[64] = {0};
	int ret = 0;

#if 0
	get_ini_value("cloud_import_mems_ret_url", str_url, sizeof(str_url));
#else
	get_cloud_import_mems_ret_url(str_url, sizeof(str_url));
#endif
	ALOGD("[cloud_import_mems_ret]cloud_import_mems_ret_url = %s\n", str_url);

	if (inmemsret_str) {
		ret = send_request(str_url, inmemsret_str, strlen(inmemsret_str),
			cloudimportmemsret_recv_deal);
		free(inmemsret_str);
	}

	if (ret != 0)
		inmems_ret = -1;

	ALOGD("[cloud_import_mems]inmems_ret=%d\n", inmems_ret);

	return inmems_ret;
}

/**
 * get_cloud_import_mems_count: get result of imprting member information
 * @totalcount: total members
 * @okcount: count of members have been importing db
 *
 * Returns 0 means success, other means failure.
 */
int get_cloud_import_mems_count(int *totalcount, int *okcount)
{
	*totalcount = import_totalcount;
	*okcount = import_okcount;

	return 0;
}

/**
 * cloud_upload_rec: send punch record information to cloud server
 * @msg: message string of punch record
 * @len: message string length
 *
 * Returns 0 means success, other means failure.
 */
int cloud_upload_rec(char *msg, int len)
{
	char str_url[256] = {0};
#if 0
	get_ini_value("cloud_upload_rec_url", str_url, sizeof(str_url));
#else
	get_cloud_upload_rec_url(str_url, sizeof(str_url));
#endif
	ALOGD("[%s]cloud_import_mems_url = %s\n", __func__, str_url);
	upload_ret = -1;
	send_request(str_url, msg, len, clouduploadrec_recv_deal);

	ALOGD("[%s]upload_ret=%d\n", __func__, upload_ret);

	return upload_ret;
}

/**
 * print_upload_record: print one punch record
 * @rec: one punch record
 *
 * Returns None.
 */
void print_upload_record(full_punch_rec_info_t_of_upload *rec)
{
	char buf[256];

	if (NULL == rec)
		return;

	ALOGD("id --> %d", rec->id);
	ALOGD("match_score --> %f", rec->match_score);
	ALOGD("address --> %s", rec->address);

	memset(buf, 0, sizeof(buf));
	time_second_to_str((rec->punch_time / 1000), buf, sizeof(buf));
	ALOGD("punch_time --> %s", buf);

	ALOGD("punch_live_large_pic_name --> %s",
			rec->punch_live_large_pic_name);
	ALOGD("punch_live_small_pic_name --> %s",
			rec->punch_live_small_pic_name);

	ALOGD("punch_pass --> %d", rec->punch_pass);
	ALOGD("employee_number --> %s", rec->employee_number);
	ALOGD("name --> %s", rec->name);
	ALOGD("gender --> %s", rec->gender);
	ALOGD("nationa --> %s", rec->nationa);
	ALOGD("id_card_number --> %s", rec->id_card_number);
	ALOGD("mobile --> %ld", rec->mobile);
	ALOGD("department_name --> %s", rec->department_name);
	ALOGD("access_right --> %d", rec->access_right);

	memset(buf, 0, sizeof(buf));
	time_second_to_str(
			(rec->temporary_access_start_time / 1000),
			buf, sizeof(buf));
	ALOGD("temporary_access_start_time --> %s", buf);

	memset(buf, 0, sizeof(buf));
	time_second_to_str(
			(rec->temporary_access_end_time / 1000),
			buf, sizeof(buf));
	ALOGD("temporary_access_end_time --> %s", buf);

	ALOGD("access_card_number --> %s", rec->access_card_number);
}

/**
 * fill_record_data_to_json: fill one punch record data to json object
 * @rec: one punch record
 * @obj: json object
 *
 * Returns 0 means success, other means failure.
 */
int fill_record_data_to_json(full_punch_rec_info_t_of_upload *rec, cJSON *obj)
{
	char buf[256];
	int ret, rlen = 0;
	unsigned char *pbin = NULL;
	char *base64_str = NULL;
	int base64_len = 0;

	if ((NULL == rec) || (NULL == obj))
		return -1;

	cJSON_AddStringToObject(obj, "mac", get_mac_addr());
	cJSON_AddNumberToObject(obj, "id", rec->id);
	cJSON_AddNumberToObject(obj, "match_score", rec->match_score);
	cJSON_AddStringToObject(obj, "address", rec->address);

	memset(buf, 0, sizeof(buf));
	time_second_to_str((rec->punch_time / 1000), buf, sizeof(buf));
	cJSON_AddStringToObject(obj, "punch_time", buf);

	cJSON_AddStringToObject(obj,
		"punch_live_large_pic_name",
		rec->punch_live_large_pic_name);
	cJSON_AddStringToObject(obj,
		"punch_live_small_pic_name",
		rec->punch_live_small_pic_name);

	ret = get_file_bin_data(
		rec->punch_live_small_pic_name,
		&pbin, &rlen);
	if (0 == ret && rlen > 0) {
		ret = base64_encode((const char *)pbin, rlen,
			&base64_str, &base64_len);
		if (0 != ret)
			ALOGE("base64_encode err!\n");
		cJSON_AddNumberToObject(obj,
			"base64_pic_len",
			base64_len);
		cJSON_AddStringToObject(obj,
			"base64_pic",
			base64_str);
	}

	cJSON_AddNumberToObject(obj, "punch_pass", rec->punch_pass);
	cJSON_AddStringToObject(obj, "employee_number", rec->employee_number);
	cJSON_AddStringToObject(obj, "name", rec->name);
	cJSON_AddStringToObject(obj, "gender", rec->gender);
	cJSON_AddStringToObject(obj, "nationa", rec->nationa);
	cJSON_AddStringToObject(obj, "id_card_number", rec->id_card_number);
	cJSON_AddNumberToObject(obj, "mobile", rec->mobile);
	cJSON_AddStringToObject(obj, "department_name", rec->department_name);
	cJSON_AddNumberToObject(obj, "access_right", rec->access_right);
	cJSON_AddStringToObject(obj,
		"access_card_number",
		rec->access_card_number);

	memset(buf, 0, sizeof(buf));
	time_second_to_str(
		(rec->temporary_access_start_time / 1000),
		buf, sizeof(buf));
	cJSON_AddStringToObject(obj,
		"temporary_access_start_time", buf);

	memset(buf, 0, sizeof(buf));
	time_second_to_str(
		(rec->temporary_access_end_time / 1000),
		buf, sizeof(buf));
	cJSON_AddStringToObject(obj,
		"temporary_access_end_time", buf);

	if (pbin)
		lb_mem_free(pbin);
	if (base64_str)
		lb_mem_free(base64_str);

	return 0;
}

/**
 * proc_upload_record: query punch records, and upload
 *
 * Returns None.
 */
void proc_upload_record(void)
{
	int query_count = 0;
	ee_queue_t head;
	face_recognition_param_t face_recg_param;
	full_punch_rec_info_t_of_upload *q = NULL;
	int ret = 0;

	cJSON *obj = NULL;
	char *pjson = NULL;
	int count = 0;

	memset(&face_recg_param, 0, sizeof(face_recognition_param_t));
	db_face_recognition_param_query(&face_recg_param);

	count = db_punch_record_count_all();
	query_count = db_punch_record_full_info_query_cloud_begin_for_cloud(
				count, &head);
	ALOGD("[%d] query_count = %d\n", __LINE__, query_count);
	if (query_count <= 0) {
		ALOGE("[%s : %d]", __func__, __LINE__);
		db_punch_record_full_info_query_end_for_cloud(&head);
		return;
	}

	ee_queue_foreach(q, &head,
			full_punch_rec_info_t_of_upload, queue) {
#ifdef DEBUG
		print_upload_record(q);
#endif
		if (q->match_score < face_recg_param.identify_threshold_score)
			q->punch_pass = 0;

		/** pack data */
		obj = cJSON_CreateObject();
		fill_record_data_to_json(q, obj);
		pjson = cJSON_PrintUnformatted(obj);
		if (pjson) {
			/** send data */
			/*
			write_debug_log("send mesg begin...\n", 20);
			*/
			ret = cloud_upload_rec(pjson, strlen(pjson));
			/*
			write_debug_log("send mesg end.\n", 20);
			*/
		}
		/** set uploaf flag */
		if (0 == ret) {
			ALOGD("clound_upload_rec ok.\n");
			ret = file_remove(q->punch_live_large_pic_name);
			if (ret)
				ALOGE("%s: failed to remove file(%s)",
				__func__, q->punch_live_large_pic_name);
			ret = file_remove(q->punch_live_small_pic_name);
			if (ret)
				ALOGE("%s: failed to remove file(%s)",
				__func__, q->punch_live_small_pic_name);
				db_punch_record_delete_by_id(q->id);
		} else
			ALOGD("clound_upload_rec fail.\n");

		/** deinit */
		if (NULL != obj) {
			cJSON_Delete(obj);
			obj  = NULL;
		}

		if (NULL != pjson) {
			lb_mem_free(pjson);
			pjson = NULL;
		}
	}
	db_punch_record_full_info_query_end_for_cloud(&head);
}

/**
 * cloud_upload_rec_thr_proc: a scheduled query punch records, and upload
 *
 * Returns 0 means success.
 */
void *cloud_upload_rec_thr_proc(void *param)
{
#if 0
	int count;
	sleep(15);
	
	count = 0;
#endif

	while (1) {
		cloud_ipaddr_changed();
#if 0
		cloud_import_mems();
		count++;
		count = 13;
		if (count >= 12) {
			proc_upload_record();
			count = 0;
		}
#else
		proc_upload_record();
#endif
		sleep(1);
	}

	return NULL;
}

/**
 * cloud_upload_rec_thr_create: create a query punch record thread
 *
 * Returns 0 means success.
 */
int cloud_upload_rec_thr_create()
{
	pthread_t p;

	pthread_create(&p, NULL, cloud_upload_rec_thr_proc, NULL);
	pthread_detach(p);

	return 0;
}

