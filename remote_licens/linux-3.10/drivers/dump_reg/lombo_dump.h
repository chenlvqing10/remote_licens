/*
 * lombo_dump.h - head file of LomboTech dump module
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

#ifndef __LOMBO_DUMP_H__
#define __LOMBO_DUMP_H__

#define DUMP_MAX_NUM	2
#define DUMP_MOD_ONE	0x0
#define DUMP_MOD_MUL	0x01
#define DUMP_MOD_GROUP	0x02

struct dump_para {
	u32 mode;
	void __iomem *paddr[2];
	void __iomem *addr[DUMP_MAX_NUM];
};

struct write_para {
	void __iomem *addr;
	void __iomem *paddr;
	u32 chg_val;
	u32 bef_val;
};

#endif
