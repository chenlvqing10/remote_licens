/*
 * cpu_info.c
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
#include <unistd.h>

typedef struct {
	char name[20];
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
	unsigned int iowait;
	unsigned int irq;
	unsigned int softirq;
} CPU_OCCUPY;

static double cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n)
{
	double od, nd;
	double id, sd;
	double cpu_use;

	od = (double) (o->user + o->nice + o->system + o->idle + o->softirq
			+ o->iowait + o->irq);
	nd = (double) (n->user + n->nice + n->system + n->idle + n->softirq
			+ n->iowait + n->irq);

	id = (double) (n->idle);
	sd = (double) (o->idle);

	if ((nd - od) != 0)
		cpu_use = 100 - (id - sd) / (nd - od) * 100.0;
	else
		cpu_use = 0;
	return cpu_use;
}

static void get_cpuoccupy(CPU_OCCUPY *cpust)
{
	FILE *fd;
	char buff[256];
	CPU_OCCUPY *cpu_occupy = cpust;

	fd = fopen("/proc/stat", "r");
	fgets(buff, sizeof(buff), fd);

	sscanf(buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user,
			&cpu_occupy->nice, &cpu_occupy->system, &cpu_occupy->idle,
			&cpu_occupy->iowait, &cpu_occupy->irq, &cpu_occupy->softirq);

	fclose(fd);
}

double getCpuRate()
{
	CPU_OCCUPY cpu_stat1, cpu_stat2;

	get_cpuoccupy(&cpu_stat1);

	sleep(1);

	get_cpuoccupy(&cpu_stat2);

	return cal_cpuoccupy(&cpu_stat1, &cpu_stat2);
}
