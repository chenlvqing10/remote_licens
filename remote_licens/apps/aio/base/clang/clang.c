/*
 * elang.c - Multi-language interface
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
#include <stdlib.h>
#include <string.h>
#include "clang.h"

typedef struct elang_header_tag {
	char flag[4];			/* name */
	unsigned int lang_num;		/* language num*/
	unsigned int str_num;		/* string num*/
	unsigned int str_map_size;	/* string map size*/
	unsigned int str_data_size;	/* string size*/
} elang_header_t;

typedef struct elang_tag {
	int cur_lang_index;		/* language index */
	elang_header_t header;		/* lang header */
	char *str_map;			/* string map for json */
	unsigned int *offset_map;	/* string offset map */
	char *str_data;			/* string data */
} elang_t;

static elang_t *g_lang;			/* global for lang*/

/**
* clang_load - load language file
* @path: file path
*
* return -1 if fail
*/
int clang_load(char *path)
{
	FILE *file = NULL;

	if (g_lang)
		clang_unload();

	g_lang = (elang_t *)malloc(sizeof(elang_t));
	if (g_lang == NULL) {
		printf("L%d: malloc fail\n", __LINE__);
		return -1;
	}
	memset(g_lang, 0, sizeof(elang_t));

	g_lang->cur_lang_index = 0;

	file = fopen(path, "rb");
	if (file == NULL)
		goto elang_load_fail;

	fread(&g_lang->header, 1, sizeof(elang_header_t), file);
	if (strcmp("EZL", g_lang->header.flag) != 0)
		goto elang_load_fail;

	g_lang->str_map = (char *)malloc(g_lang->header.str_map_size);
	if (g_lang->str_map == NULL)
		goto elang_load_fail;

	if (g_lang->header.str_map_size != fread(g_lang->str_map, 1,
		g_lang->header.str_map_size, file))
		goto elang_load_fail;

	g_lang->offset_map = (unsigned int *)malloc(4 * g_lang->header.str_num);
	if (g_lang->offset_map == NULL)
		goto elang_load_fail;

	if ((4 * g_lang->header.str_num) != fread(g_lang->offset_map, 1,
		4 * g_lang->header.str_num, file))
		goto elang_load_fail;

	g_lang->str_data = (char *)malloc(g_lang->header.str_data_size);
	if (g_lang->str_data == NULL)
		goto elang_load_fail;

	if (g_lang->header.str_data_size != fread(g_lang->str_data, 1,
		g_lang->header.str_data_size, file))
		goto elang_load_fail;

	if (file)
		fclose(file);

	return 0;

elang_load_fail:
	if (file)
		fclose(file);
	if (g_lang->str_map) {
		free(g_lang->str_map);
		g_lang->str_map = NULL;
	}
	if (g_lang->offset_map) {
		free(g_lang->offset_map);
		g_lang->offset_map = NULL;
	}
	if (g_lang->str_data) {
		free(g_lang->str_data);
		g_lang->str_data = NULL;
	}
	if (g_lang) {
		free(g_lang);
		g_lang = NULL;
	}

	return -1;
}

/**
* clang_unload - unload language
*
* return -1 if fail
*/
int clang_unload(void)
{
	if (g_lang) {
		if (g_lang->str_map) {
			free(g_lang->str_map);
			g_lang->str_map = NULL;
		}
		if (g_lang->offset_map) {
			free(g_lang->offset_map);
			g_lang->offset_map = NULL;
		}
		if (g_lang->str_data) {
			free(g_lang->str_data);
			g_lang->str_data = NULL;
		}
		free(g_lang);
		g_lang = NULL;
		return 0;
	} else {
		printf("L%d: g_lang == NULL\n", __LINE__);
		return -1;
	}
}

/**
* clang_set_language - setting language
* @langIndex: index in lang.h
*
* return -1 if fail
*/
int clang_set_language(unsigned int lang_index)
{
	if (g_lang) {
		if (lang_index >= g_lang->header.lang_num) {
			printf("L%d: lang_index >= g_lang->header.lang_num\n",
				__LINE__);
			return -1;
		}

		g_lang->cur_lang_index = lang_index;
		return 0;
	} else {
		printf("\nL%d: g_lang == NULL\n", __LINE__);
		return -1;
	}
}

/**
* clang_get_utf8_string - get utf8 string
* @strIndex: index in lang.h
*
* return -1 if fail, don't free the pointer
*/
const char *clang_get_utf8_string(unsigned int str_index)
{
	unsigned int offset;
	int cnt = 0;
	const char *ret_str;

	if (g_lang == NULL) {
		printf("L%d: g_lang == NULL\n", __LINE__);
		return NULL;
	}

	if (str_index >= g_lang->header.str_num) {
		printf("L%d: str_index >= g_lang->header.str_num\n",
			__LINE__);
		return NULL;
	}

	offset = g_lang->offset_map[str_index];
	ret_str = (const char *)(g_lang->str_data + offset);
	while (cnt < g_lang->cur_lang_index) {
		while (*ret_str++ != '\0')
			;
		cnt++;
	}

	return ret_str;
}

/**
* clang_str - get utf8 string
* @json_str: string in json
*
* return -1 if fail, don't free the pointer
*/
char *clang_str(const char *json_str)
{
	unsigned int str_index = 0;
	unsigned int offset;
	int cnt = 0;
	char *ret_str;

	if (g_lang == NULL) {
		printf("L%d: g_lang == NULL\n", __LINE__);
		return NULL;
	}

	str_index = 0;
	offset = 0;
	while (strcmp(json_str, (const char *)
		(g_lang->str_map + offset)) != 0) {
		offset += (strlen((const char *)
			(g_lang->str_map + offset))+1);
		str_index++;

		if (str_index >= g_lang->header.str_num) {
			printf("L%d: don't fined string, %s\n",
				__LINE__, json_str);
			return NULL;
		}
	}

	offset = g_lang->offset_map[str_index];
	ret_str = (char *)(g_lang->str_data + offset);
	while (cnt < g_lang->cur_lang_index) {
		while (*ret_str++ != '\0')
			;
		cnt++;
	}

	return ret_str;
}

void clang_test(void)
{
	int i;

	if (-1 == clang_load("lang.el"))
		return;

	clang_set_language(ELANG_ENGLISH);

	for (i = 0; i < ESTR_LANGUAGE_MAX; i++)
		printf("%s\n", clang_get_utf8_string(i));

	printf("%s\n", clang_str("STR_MONDAY"));
	printf("%s\n", clang_str("STR_TUESDAY"));
	printf("%s\n", clang_str("STR_WEDNESDAY"));
	printf("%s\n", clang_str("STR_THURSDAY"));
	printf("%s\n", clang_str("STR_FRIDAY"));
	printf("%s\n", clang_str("STR_SATURDAY"));
	printf("%s\n", clang_str("STR_SUNDAY"));


	clang_set_language(ELANG_SIMPLE_CHINESE);

	for (i = 0; i < ESTR_LANGUAGE_MAX; i++)
		printf("%s\n", clang_get_utf8_string(i));

	printf("%s\n", clang_str("STR_TAB_INPUT"));
	printf("%s\n", clang_str("STR_TAB_LIST"));
	printf("%s\n", clang_str("STR_TAB_CHART"));
	printf("%s\n", clang_str("STR_TAB_Setting"));
	printf("%s\n", clang_str("STR_TAB_lang"));

	clang_set_language(ELANG_TRADITION_CHEINESE);

	for (i = 0; i < ESTR_LANGUAGE_MAX; i++)
		printf("%s\n", clang_get_utf8_string(i));

	printf("%s\n", clang_str("STR_TAB_INPUT"));
	printf("%s\n", clang_str("STR_TAB_LIST"));
	printf("%s\n", clang_str("STR_TAB_CHART"));
	printf("%s\n", clang_str("STR_TAB_Setting"));
	printf("%s\n", clang_str("STR_TAB_lang"));

	clang_unload();
}
