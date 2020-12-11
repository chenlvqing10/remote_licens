/*
 * disp_cjson.c - Lombo disp cjson module code for LomboTech
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
#include <string.h>
#include <unistd.h>

#include "cJSON.h"
#include "disp_cjson.h"

#define GAMMA_TBL_OFST	(0)
#define GAMMA_TBL_SIZE	(1024)

#define DISP_CJSON_DBG	0
#if DISP_CJSON_DBG
#define cjson_printf	printf
#else
#define cjson_printf(format, arg...)
#endif

#define LOG_E		printf
#define LOG_I		printf
#define DISP_OK		0
#define DISP_ERROR 	1

static disp_cjson_cfg_t *disp_cjson;

#if DISP_CJSON_DBG
static const char *JSON_TYPE_STRING[7] = {
	"cJSON_False",
	"cJSON_True",
	"cJSON_NULL",
	"cJSON_Number",
	"cJSON_String",
	"cJSON_Array",
	"cJSON_Object",
};
#endif

static void disp_cjson_get_ce(cJSON *item_ce, disp_cjson_cfg_t *dcc)
{
	cJSON *item_status, *item_level, *item_min, *item_max;

	item_status = cJSON_GetObjectItem(item_ce, "status");
	if (item_status && item_status->type == cJSON_String) {
		cjson_printf("item_status:%s, type:%s\n",
			item_status->valuestring,
			JSON_TYPE_STRING[item_status->type]);
		if (strcmp(item_status->valuestring, "okay") == 0)
			dcc->ce_enable = true;
	}

	dcc->lti_level = 0;
	if (dcc->ce_enable) {
		item_level = cJSON_GetObjectItem(item_ce, "brightness");
		if (item_level && item_level->type == cJSON_Number) {
			cjson_printf("item_level:%d, type:%s\n",
				item_level->valueint,
				JSON_TYPE_STRING[item_level->type]);
			dcc->brightness = item_level->valueint;
		}

		item_level = cJSON_GetObjectItem(item_ce, "saturation");
		if (item_level && item_level->type == cJSON_Number) {
			cjson_printf("item_level:%d, type:%s\n",
				item_level->valueint,
				JSON_TYPE_STRING[item_level->type]);
			dcc->saturation = item_level->valueint;
		}

		item_level = cJSON_GetObjectItem(item_ce, "contrast");
		if (item_level && item_level->type == cJSON_Number) {
			cjson_printf("item_level:%d, type:%s\n",
				item_level->valueint,
				JSON_TYPE_STRING[item_level->type]);
			dcc->contrast = item_level->valueint;
		}

		item_min = cJSON_GetObjectItem(item_ce, "min");
		if (item_min && item_min->type == cJSON_Number) {
			cjson_printf("item_min:%d, type:%s\n",
				item_min->valueint,
				JSON_TYPE_STRING[item_min->type]);
		}

		item_max = cJSON_GetObjectItem(item_ce, "max");
		if (item_max && item_max->type == cJSON_Number) {
			cjson_printf("item_max:%d, type:%s\n",
				item_max->valueint,
				JSON_TYPE_STRING[item_max->type]);
		}
	}
}

static void disp_cjson_get_lti(cJSON *item_lti, disp_cjson_cfg_t *dcc)
{
	cJSON *item_status, *item_level, *item_min, *item_max;

	item_status = cJSON_GetObjectItem(item_lti, "status");
	if (item_status && item_status->type == cJSON_String) {
		cjson_printf("item_status:%s, type:%s\n",
			item_status->valuestring,
			JSON_TYPE_STRING[item_status->type]);
		if (strcmp(item_status->valuestring, "okay") == 0)
			dcc->lti_enable = true;
	}

	dcc->lti_level = 0;
	if (dcc->lti_enable) {
		item_level = cJSON_GetObjectItem(item_lti, "value");
		if (item_level && item_level->type == cJSON_Number) {
			cjson_printf("item_level:%d, type:%s\n",
				item_level->valueint,
				JSON_TYPE_STRING[item_level->type]);
			dcc->lti_level = item_level->valueint;
		}

		item_min = cJSON_GetObjectItem(item_lti, "min");
		if (item_min && item_min->type == cJSON_Number) {
			cjson_printf("item_min:%d, type:%s\n",
				item_min->valueint,
				JSON_TYPE_STRING[item_min->type]);
		}

		item_max = cJSON_GetObjectItem(item_lti, "max");
		if (item_max && item_max->type == cJSON_Number) {
			cjson_printf("item_max:%d, type:%s\n",
				item_max->valueint,
				JSON_TYPE_STRING[item_max->type]);
		}
	}
}

static void disp_cjson_get_peak(cJSON *item_peak, disp_cjson_cfg_t *dcc)
{
	cJSON *item_status, *item_level, *item_min, *item_max;

	item_status = cJSON_GetObjectItem(item_peak, "status");
	if (item_status && item_status->type == cJSON_String) {
		cjson_printf("item_status:%s, type:%s\n",
			item_status->valuestring,
			JSON_TYPE_STRING[item_status->type]);
		if (strcmp(item_status->valuestring, "okay") == 0)
			dcc->peak_enable = true;
	}

	dcc->peak_level = 0;
	if (dcc->peak_enable) {
		item_level = cJSON_GetObjectItem(item_peak, "value");
		if (item_level && item_level->type == cJSON_Number) {
			cjson_printf("item_level:%d, type:%s\n",
				item_level->valueint,
				JSON_TYPE_STRING[item_level->type]);
			dcc->peak_level = item_level->valueint;
		}

		item_min = cJSON_GetObjectItem(item_peak, "min");
		if (item_min && item_min->type == cJSON_Number) {
			cjson_printf("item_min:%d, type:%s\n",
				item_min->valueint,
				JSON_TYPE_STRING[item_min->type]);
		}

		item_max = cJSON_GetObjectItem(item_peak, "max");
		if (item_max && item_max->type == cJSON_Number) {
			cjson_printf("item_max:%d, type:%s\n",
				item_max->valueint,
				JSON_TYPE_STRING[item_max->type]);
		}
	}
}

static void disp_cjson_get_gamma(cJSON *item_gamma, disp_cjson_cfg_t *dcc)
{
	cJSON *item_status;
	/* cJSON *item_filepath; */

	item_status = cJSON_GetObjectItem(item_gamma, "status");
	if (item_status && item_status->type == cJSON_String) {
		cjson_printf("item_status:%s, type:%s\n",
			item_status->valuestring,
			JSON_TYPE_STRING[item_status->type]);
		if (strcmp(item_status->valuestring, "okay") == 0)
			dcc->gamma_enable = true;
	}

	#if 0
	if (dcc->gamma_enable) {
		item_filepath = cJSON_GetObjectItem(item_gamma, "filepath");
		if (item_filepath && item_filepath->type == cJSON_String) {
			cjson_printf("item_filepath:%s, type:%s\n",
				item_filepath->valuestring,
				JSON_TYPE_STRING[item_filepath->type]);
			memset(dcc->gamma_filepath, 0x00, DISP_CJSON_NAME_MAX);
			strncpy(dcc->gamma_filepath, item_filepath->valuestring,
					DISP_CJSON_NAME_MAX);
			cjson_printf("gamma_filepath:%s\n", dcc->gamma_filepath);
		}
	}
	#endif
}

int disp_cjson_get_do(const char *file_name, disp_cjson_cfg_t *dcc)
{
	int fd, read_size;
	u32 file_len;
	cJSON *json = NULL;
	char *json_str = NULL;
	cJSON *item_disp, *item_zero, *item_ce, *item_lti, *item_peak, *item_gamma;

	if (file_name == NULL) {
		LOG_E("file_name is NULL\n");
		return DISP_ERROR;
	}

	if (dcc == NULL) {
		LOG_E("dcc is NULL\n");
		return DISP_ERROR;
	}

	dcc->lti_enable = false;
	dcc->peak_enable = false;
	dcc->gamma_enable = false;

	LOG_I("file_name:%s\n", file_name);
	fd = open(file_name, O_RDONLY);
	if (fd <= 0) {
		LOG_E("open file[%s] err\n", file_name);
		return DISP_ERROR;
	}

	file_len = lseek(fd, 0, SEEK_END);

	lseek(fd, 0, SEEK_SET);

	LOG_I("file_len:%d\n", file_len);

	json_str = malloc(file_len);
	if (json_str == NULL) {
		LOG_E("json_str malloc err\n");
		return DISP_ERROR;
	}

	read_size = read(fd, json_str, file_len);
	if (read_size != file_len) {
		free(json_str);
		close(fd);
		LOG_E("read %s data err\n", file_name);
		return DISP_ERROR;
	}
	close(fd);
	LOG_I("read_size:%d\n", read_size);

	cJSON_Minify(json_str);
	/* LOG_E("json_str:%64.64s\n", json_str); */
	json = cJSON_Parse(json_str);
	if (json) {
		item_disp = cJSON_GetObjectItem(json, "disp");
		if (item_disp)
			cjson_printf("item_disp:%s, type:%s\n",
					item_disp->valuestring,
					JSON_TYPE_STRING[item_disp->type]);

		if (item_disp) {
			item_zero = cJSON_GetArrayItem(item_disp, 0);

			/* get ce config */
			item_ce = cJSON_GetObjectItem(item_zero, "ce");
			if (item_ce) {
				cjson_printf("item_ce:%s, type:%s\n",
					item_ce->valuestring,
					JSON_TYPE_STRING[item_ce->type]);
				disp_cjson_get_ce(item_ce, dcc);
			} else
				LOG_E("fail to get item_ce");

			/* get lti config */
			item_lti = cJSON_GetObjectItem(item_zero, "lti");
			if (item_lti) {
				cjson_printf("item_lti:%s, type:%s\n",
					item_lti->valuestring,
					JSON_TYPE_STRING[item_lti->type]);
				disp_cjson_get_lti(item_lti, dcc);
			} else
				LOG_E("fail to get item_lti");

			/* get peak config */
			item_peak = cJSON_GetObjectItem(item_zero, "peak");
			if (item_peak) {
				cjson_printf("item_peak:%s, type:%s\n",
					item_peak->valuestring,
					JSON_TYPE_STRING[item_peak->type]);
				disp_cjson_get_peak(item_peak, dcc);
			} else
				LOG_E("fail to get item_peak");

			/* get gamma config */
			item_gamma = cJSON_GetObjectItem(item_zero, "gamma");
			if (item_gamma) {
				cjson_printf("item_gamma:%s, type:%s\n",
					item_gamma->valuestring,
					JSON_TYPE_STRING[item_gamma->type]);
				disp_cjson_get_gamma(item_gamma, dcc);
			} else
				LOG_E("fail to get item_gamma");
		}

		cJSON_Delete(json);
	} else {
		LOG_E("cJSON_Parse err, json is null");
		free(json_str);
		return DISP_ERROR;
	}

	free(json_str);

	return DISP_OK;
}

int disp_cjson_update(void)
{
	return disp_cjson_get_do(DISP_JSON_SRC_FILE, disp_cjson);
}

void disp_cjson_init(void)
{
	#if 0
	int ret = RT_EOK;
	disp_panel_ctl_t *pctl = NULL;
	char src_filepath[DISP_CJSON_NAME_MAX] = {0};
	#endif

	printf("%s++\n", __FUNCTION__);
	disp_cjson = malloc(sizeof(disp_cjson_cfg_t));
	memset(disp_cjson, 0x00, sizeof(disp_cjson_cfg_t));
	if (disp_cjson == NULL) {
		LOG_E("disp_cjson malloc err\n");
		return;
	}

	if (access(DISP_JSON_DEBUG_FILE, 0) == 0) {
		printf("file %s is exit\n", DISP_JSON_DEBUG_FILE);
		disp_cjson_get_do(DISP_JSON_DEBUG_FILE, disp_cjson);
	} else if (access(DISP_JSON_SRC_FILE, 0) == 0) {
		printf("file %s is exit\n", DISP_JSON_SRC_FILE);
		disp_cjson_get_do(DISP_JSON_SRC_FILE, disp_cjson);
	} else {
		LOG_E("disp_config.json is not exit\n");
		return;
	}

	#if DISP_CJSON_DBG
	LOG_I("ce_enable:%d\n", disp_cjson->ce_enable);
	LOG_I("brightness:%d\n", disp_cjson->brightness);
	LOG_I("saturation:%d\n", disp_cjson->saturation);
	LOG_I("contrast:%d\n", disp_cjson->contrast);
	LOG_I("lti_enable:%d\n", disp_cjson->lti_enable);
	LOG_I("lti_level:%d\n", disp_cjson->lti_level);
	LOG_I("peak_enable:%d\n", disp_cjson->peak_enable);
	LOG_I("peak_level:%d\n", disp_cjson->peak_level);
	LOG_I("gamma_enable:%d\n", disp_cjson->gamma_enable);
	#endif
	/* LOG_I("gamma_filepath:%s", disp_cjson->gamma_filepath); */

	#if 0
	if (access(disp_cjson->gamma_filepath, 0) == 0)
		LOG_W("file %s is exit", disp_cjson->gamma_filepath);
	else {
		ret = panel_find_valid(&pctl);
		if (ret != RT_EOK) {
			LOG_E("fail to find valid panel");
			return;
		}
		sprintf(src_filepath, "%s%s%s", DISP_GAMMA_SRC_PATH,
					pctl->name, DISP_GAMMA_SRC_TAG);
		LOG_W("file %s is not exit, copy it from %s",
			disp_cjson->gamma_filepath, src_filepath);
		copy(src_filepath, disp_cjson->gamma_filepath);
	}
	#endif

	disp_cjson_gamma_get(GAMMA_TBL_SIZE);
	printf("%s--\n", __FUNCTION__);
}

void disp_cjson_exit(void)
{
	printf("%s++\n", __FUNCTION__);
	if (disp_cjson == NULL) {
		LOG_E("please init disp_cjson first\n");
		return;
	}

	free(disp_cjson->gamma_tbl);
	free(disp_cjson);
	printf("%s--\n", __FUNCTION__);
}

disp_cjson_cfg_t *disp_cjson_get(void)
{
	return disp_cjson;
}

int disp_cjson_gamma_get(u32 table_size)
{
	int fd, read_size;
	u32 file_len;
	disp_cjson_cfg_t *dcc;
	u32 *tbl;
	const char *file_name;

	dcc = disp_cjson_get();
	if (dcc == NULL) {
		LOG_E("please init disp_cjson first\n");
		return DISP_ERROR;
	}

	/* LOG_I("gamma_filepath:%s\n", dcc->gamma_filepath); */
	if (!dcc->gamma_enable) {
		LOG_E("cjson gamma is not enable\n");
		return DISP_ERROR;
	}

	if (access(DISP_GAMMA_DEBUG_FILE, 0) == 0) {
		printf("file %s is exit\n", DISP_GAMMA_DEBUG_FILE);
		file_name = DISP_GAMMA_DEBUG_FILE;
	} else if (access(DISP_GAMMA_SRC_FILE, 0) == 0) {
		printf("file %s is exit\n", DISP_GAMMA_SRC_FILE);
		file_name = DISP_GAMMA_SRC_FILE;
	} else {
		LOG_E("disp_gamma.bin is not exit\n");
		return DISP_ERROR;
	}

	if (table_size != GAMMA_TBL_SIZE) {
		LOG_E("cjson gamma table_size[%d] should be %d\n",
			table_size, GAMMA_TBL_SIZE);
		return DISP_ERROR;
	}

	if (dcc->gamma_tbl == NULL) {
		dcc->gamma_tbl = malloc(table_size);
		if (dcc->gamma_tbl == NULL) {
			LOG_E("fail to malloc gamma_tbl\n");
			return DISP_ERROR;
		}
		dcc->gamma_tbl_size = table_size;
	}

	tbl = (u32 *)dcc->gamma_tbl;

	fd = open(file_name, O_RDONLY);
	if (fd <= 0) {
		LOG_E("open file[%s] err\n", file_name);
		return DISP_ERROR;
	}

	file_len = lseek(fd, 0, SEEK_END);
	if (file_len != table_size) {
		LOG_E("file_len[%d] is not equal to GAMMA_TBL_SIZE[%d]\n",
			table_size, GAMMA_TBL_SIZE);
		close(fd);
		return DISP_ERROR;
	}

	lseek(fd, 0, SEEK_SET);
	read_size = read(fd, (char *)tbl, file_len);
	if (read_size != file_len) {
		close(fd);
		LOG_E("read %s data err\n", file_name);
		return DISP_ERROR;
	}
	LOG_I("read_size:%d\n", read_size);
	close(fd);

	return DISP_OK;
}

int disp_cjson_gamma_update(void)
{
	return disp_cjson_gamma_get(GAMMA_TBL_SIZE);
}
