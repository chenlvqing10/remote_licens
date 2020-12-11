/*
 * parse.c - Generic definitions for LomboTech SoCs
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
 
#include "parse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MONGOOSECONF "/etc/http.ini"
#define LISTENINGPORTS "listening_ports"
#define DOCUMENTROOT  "document_root"

/**
 *  get_parse - parse mongoose.conf to valiable
 *  @opts: the parse result where opts is saved.
 *   
 *  api of callback when parse mongoose.conf
 *
 *  Returns 0 means success, or other value means failure.
 * 
*/
int32_t get_parse(char *port, char *doc_root)
{
	int32_t ret = -2;
	FILE *fp = NULL;
	char line[512];
	char sdata[2][64];
	char *pend = NULL;
	char *p = NULL;
	int32_t count = 0;

	memset(line, 0, sizeof(line));

	fp = fopen(MONGOOSECONF, "r");
	if (!fp) {
		debug("fopen fail.\n");
		return -1;
	}

	while (1) {
		memset(line, 0, sizeof(line));
		pend = fgets(line, sizeof(line), fp);
		if (pend == NULL)
			break;

		count = 0;
		memset(sdata, 0, sizeof(sdata));
		p = strtok(line, " \r\n");
		while(p) {
			strcpy(sdata[count], p);
			count++;
			p = strtok(NULL, " \r\n");
		}

		debug("sdata[0]=%s, sdata[1]=%s\n", sdata[0], sdata[1]);

		if (strcmp(sdata[0], LISTENINGPORTS) == 0) {
			if(strlen(sdata[1]) <= 0) {
				ret = -1;
				break;
			}
			ret++;
			strcpy(port, sdata[1]);
			debug("LISTENINGPORTS\n");
		}
		else if(strcmp(sdata[0], DOCUMENTROOT) == 0) {
			if(strlen(sdata[1]) <= 0) {
				ret = -1;
				break;
			}
			ret++;
			strcpy(doc_root, sdata[1]);
			debug("DOCUMENTROOT\n");
		}
	}

	if (!fp) {
		fclose(fp);
	}

	return ret;
}

