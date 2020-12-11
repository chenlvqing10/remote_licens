/*
 * person_output.c - Writing personnel registration information to Excel.
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wchar.h>
#include <errno.h>
#include <memory.h>
#include <dirent.h>
#include <string.h>
#include <sys/prctl.h>

#include "hotplug.h"
#include "person.h"
#include "file_ops.h"
#include "char_conv.h"
#include "time_conv.h"
#include "person_import.h"
#include "excel_file_read.h"
#include "excel_person_public.h"
#include "face.h"
#include "db_all.h"
#include "ui_control.h"
#include "xlslib.h"
#include "person_output.h"
#include "thread_ops.h"
#include "excel_file_read.h"

#define LOG_TAG "person_output"
#include "log/log.h"

/**
 * Number of columns in each person registration information
 * when registering in bulk
 */
#define PERSON_OUTPUT_INFO_COL 14
#define PERSON_INFO_STEP 20

static output_cb g_cb;
static int th_living = 0;
static pthread_t th1;
static void *g_win = NULL;

static char *head[] = {
	"人员编号",
	"姓名",
	"性别",
	"民族",
	"身份证号码",
	"电话号码",
	"部门名称",
	"人员通行权限",
	"临时通行开始时间",
	"临时通行截止时间",
	"临时通行次数",
	"门禁卡 卡号",
	"备注",
	"登记照片名"
};

int getSheetRow()
{
	int ret = 0, i = 0;
	int sheet_count = 0;
	int row_count = 0;
	xlsWorkBook *wb = NULL;
	xlsWorkSheet *ws = NULL;

	if (is_file_exist(PERSON_OUTPUT_EXCEL_FILE)){
		ALOGD("file[%s] not exit.",PERSON_OUTPUT_EXCEL_FILE);
		return row_count;
	}
	/* Open the Excel file */
	ret = excel_file_read_open(PERSON_OUTPUT_EXCEL_FILE, EXCEL_CHAR_SET, &wb);
	if (ret != 0) {
		ALOGE("Can't open %s\n", PERSON_OUTPUT_EXCEL_FILE);
		return row_count;
	}
	/* sheets: Get the worksheet and then parse */
	sheet_count = (int)wb->sheets.count;

	for (i = 0; i < sheet_count; i++) {
		if (0 == strcmp(wb->sheets.sheet[i].name,
			PERSON_OUTPUT_EXCEL_SHEET)){
			ws = xls_getWorkSheet(wb, i);
			row_count = ws->rows.lastrow;
			ALOGD("sheet row_count=%d", row_count);
			xls_close_WS(ws);
			break;
		}
	}
	/* Close the Excel file */
	excel_file_read_close(wb);
	return row_count;
}

int copy_register_pic(local_person_info_t *p_info)
{
	int ret = 0;
	char *p;
	char _pic[FILENAME_MAX] = {0};
	char _dcim[FILENAME_MAX] = {0};

	create_dir(PERSON_OUTPUT_DCIM_PATH);

	ret = is_file_exist(p_info->pic_large);
	if (0 == ret) {
		strcpy(_dcim, PERSON_OUTPUT_DCIM_PATH);
		strcpy(_pic, (p = strrchr(p_info->pic_large, '/'))?
			p+1:p_info->pic_large);
		strcat(_dcim, _pic);
		ret = file_copy(p_info->pic_large, _dcim);
		strncpy(p_info->pic_large, _pic, sizeof(_pic));
	} else {
		memset(p_info->pic_large, 0, sizeof(p_info->pic_large));
	}
	return ret;
}

void excel_write_person_row(worksheet *p_write_ws,
	int row_index, person_t *p_info)
{
	char temp[16] = {0};

	xlsWorksheetLabel(p_write_ws, row_index, 0,
				p_info->employee_number, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 1,
				p_info->name, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 2,
				p_info->gender, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 3,
				p_info->nationa, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 4,
				p_info->id_card_number, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 5,
				p_info->mobile, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 6,
				p_info->department_name, NULL);

	sprintf(temp, "%d", p_info->access_right);
	xlsWorksheetLabel(p_write_ws, row_index, 7,
				temp, NULL);

	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%ld", p_info->temporary_access_start_time);
	xlsWorksheetLabel(p_write_ws, row_index, 8,
				temp, NULL);

	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%ld", p_info->temporary_access_start_time);
	xlsWorksheetLabel(p_write_ws, row_index, 9,
				temp, NULL);

	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%d", p_info->temporary_access_times);
	xlsWorksheetLabel(p_write_ws, row_index, 10,
				temp, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 11,
				p_info->access_card_number, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 12,
				p_info->remarks, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 13,
				p_info->regist_photo_name, NULL);
}

void excel_write_person_row_simple(worksheet *p_write_ws,
	int row_index, local_person_info_t *p_info)
{
	xlsWorksheetLabel(p_write_ws, row_index, 0,
				p_info->user_id, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 1,
				p_info->name, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 7,
				"0", NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 11,
				p_info->access_card_number, NULL);

	xlsWorksheetLabel(p_write_ws, row_index, 13,
				p_info->pic_large, NULL);
}


void *thread_proc(void *arg)
{
	int ret = 0, tmp = 0, count = 0, cnt = 0;
	int i = 0, row_count = 0, row_index = 0;
	ee_queue_t head_person;
	workbook *p_write_wb = NULL;
	worksheet *p_write_ws = NULL;
	local_person_info_t *_p = NULL;

	prctl(PR_SET_NAME, "person_output");
	system("rm -rf "PERSON_OUTPUT_FILE_PATH" && sync");

	row_count = getSheetRow();
	ALOGD("%s:%d >> row_count=%d",
		__func__, __LINE__, row_count);

	count = db_ui_person_info_get_count(TYPE_PERSON);
	if (0 == count) {
		ALOGI("had no found register person!");
		if (g_cb)
			g_cb(g_win, EN_OUTPUT_OK, 0);
		th_living = 0;
		return NULL;
	}

	create_dir(PERSON_OUTPUT_FILE_PATH);
	p_write_wb = xlsNewWorkbook();
	p_write_ws = xlsWorkbookSheet(p_write_wb,
		PERSON_OUTPUT_EXCEL_SHEET);

	for (i = 0; i < PERSON_OUTPUT_INFO_COL; i++) {
		xlsWorksheetLabel(p_write_ws, 0, i, head[i], NULL);
	}

	cnt = ceil((double)count/PERSON_INFO_STEP);
	for (i = 0; i < cnt; i++) {
		if ( 0 == th_living || udisk_is_insert() != 1) {
			ALOGW("%s[%d] udisk_is_insert[%d] is not ok or thread is stop.",
				__func__, __LINE__, udisk_is_insert());
			ret = -1;
			break;
		}
		ee_queue_init(&head_person);
		ret = ui_control_local_person_get_begin(
					&head_person, i * PERSON_INFO_STEP, PERSON_INFO_STEP);
		if (ret > 0) {
			row_index = row_count + i * PERSON_INFO_STEP;
			ee_queue_foreach(_p, &head_person, local_person_info_t, queue) {
				if (0 == th_living || udisk_is_insert() != 1) {
					ALOGD("%s[%d] thread is stop.", __func__, __LINE__);
					ret = -1;
					break;
				}
				if (copy_register_pic(_p)) {
					ALOGE("%s[%d] copy register file error.", __func__, __LINE__);
				}
				excel_write_person_row_simple(p_write_ws, row_index + 1, _p);
				row_index++;
				if (g_cb && row_index%50 == 0) {
					g_cb(g_win, EN_OUTPUT_WORKING, row_index*100/count);
				}
			}
		}
		ui_control_local_person_get_end(&head_person);
		ret = 0;
	}
	/* Close the Excel file */
	tmp = xlsWorkbookDump(p_write_wb, PERSON_OUTPUT_EXCEL_FILE);
	ALOGD("%s:%d >> xlsWorkbookDump = %d", __func__, __LINE__, tmp);
	xlsDeleteWorkbook(p_write_wb);
	file_system_sync();
	if (g_cb) {
		if (0 == ret && 0 == tmp)
			g_cb(g_win, EN_OUTPUT_OK, 100);
		else
			g_cb(g_win, EN_OUTPUT_ERROR, row_index*100/count);
	}

	th_living = 0;
	return NULL;
}

/**
 * person_output_stop - Stop to output person info
 * None
 *
 * Returns None.
 */
void person_output_stop(output_cb cb)
{
	ALOGD("person output thread stop!");
	th_living = 0;
	pthread_join(th1, NULL);
}

/**
 * person_output_start - Start to output person info in thread
 * None
 *
 * Returns None.
 */
void person_output_start(void *win, output_cb cb)
{
	g_cb = cb;
	g_win = win;
	if (th_living) {
		ALOGW("Output thread is running");
		return;
	}
	if (g_cb && db_ui_person_info_get_count(TYPE_PERSON) > 200)
		g_cb(g_win, EN_OUTPUT_WORKING, 0);

	th_living = 1;
	pthread_create(&th1, NULL, thread_proc, NULL);
}

