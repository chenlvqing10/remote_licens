/*
 * mem_info.c
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
#include "mem_info.h"

typedef struct MEM_PACKED {
	char name[20];
	unsigned long total;
	char name2[20];
} MEM_OCCUPY;

MEM_PACK get_memoccupy()
{
	FILE *fd;
	double mem_total, mem_used_rate;
	char buff[256];

	MEM_OCCUPY m;
	MEM_PACK p;

	fd = fopen("/proc/meminfo", "r");

	fgets(buff, sizeof(buff), fd);

	sscanf(buff, "%s %lu %s\n", m.name, &m.total, m.name2);

	mem_total = m.total;

	fgets(buff, sizeof(buff), fd);

	sscanf(buff, "%s %lu %s\n", m.name, &m.total, m.name2);

	mem_used_rate = (1 - m.total / mem_total) * 100;

	mem_total = mem_total / (1024 * 1024);

	p.total = mem_total;
	p.used_rate = mem_used_rate;

	fclose(fd);

	return p;
}
