/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS_ISP driver code for LomboTech
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

#include "viss-m2m.h"

struct list_head *offline_buf_pending[VISS_IS_MAX_NUM];
struct list_head offline_fifo;

struct offline_buf offline_buf_pool[VISS_IS_MAX_NUM][RAW_BUF_NUM];

bool isp_offline_busy;
bool isp_offline;
int isp_offline_id;
int isp_offline_set;
struct timespec isp_offline_ts;

int offline_buf_init(struct device *dev, int w, int h, int d, int id)
{
	int i;

	for (i = 0; i < RAW_BUF_NUM; i++) {
		offline_buf_pool[id][i].dma.size = w * h * d;
		offline_buf_pool[id][i].dma.vaddr = dma_alloc_coherent(dev,
			offline_buf_pool[id][i].dma.size,
			&offline_buf_pool[id][i].dma.paddr, GFP_KERNEL);
		if (!offline_buf_pool[id][i].dma.vaddr) {
			PRT_ERR("memory alloc failed, size %d\n",
				offline_buf_pool[id][i].dma.size);
			return -ENOMEM;
		}
		offline_buf_add(offline_buf_pending[id],
			&offline_buf_pool[id][i]);

		offline_buf_pool[id][i].win.left = 0;
		offline_buf_pool[id][i].win.top = 0;
		offline_buf_pool[id][i].win.width = w;
		offline_buf_pool[id][i].win.height = h;
		offline_buf_pool[id][i].raw_depth = d;
		offline_buf_pool[id][i].isp_id = id;
	}
	return 0;
}

void offline_buf_exit(struct device *dev, int id)
{
	int i;

	for (i = 0; i < RAW_BUF_NUM; i++) {
		dma_free_coherent(dev, offline_buf_pool[id][i].dma.size,
			offline_buf_pool[id][i].dma.vaddr,
			offline_buf_pool[id][i].dma.paddr);
	}
}

