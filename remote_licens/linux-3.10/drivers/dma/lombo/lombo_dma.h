/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * DMA definitions for LomboTech SoCs
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

#ifndef __LOMBO_DMA_H
#define __LOMBO_DMA_H

#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/dmapool.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>
#include <linux/of.h>
#include <linux/clk.h>

#include "../dmaengine.h"

/* interrupt type */
#define HALF_BLOCK_INT	0x1
#define BLOCK_INT	0x2
#define COMPLETE_INT	0x4
#define SRC_COMP_INT	0x8
#define DST_COMP_INT	0xF

/* transfer type */
#define MEMORY_TRANS	0x1
#define SG_TRANS	0x2
#define CYCLIC_TRANS	0x4

/**
 * lombo_dma_chan_cfg - record lombo channel config param
 * @direction: trans direction
 * @src_addr: sources address
 * @dst_addr: destination address
 * @src_width: sources bus width bits
 * @dst_width: destination bus width bits
 * @src_burst: sources bus burst length
 * @dst_burst: destination burst length
 */
struct lombo_dma_chan_cfg {
	u32 direction;
	u32 src_addr;
	u32 dst_addr;
	u32 src_width;
	u32 dst_width;
	u32 src_burst;
	u32 dst_burst;
};

/**
 * lombo_dma_chan - lombo physical dma channel descriptor
 * @idx: channel index
 * @dev: dma channel controller
 * @chan: dmaengine channel struct
 * @desc: async transfer descriptor
 * @dir: transfer direction
 * @base: dma controller base address
 * @tasklet: task process
 * @slave_id: dma slave id, for identify SRP/DRP
 * @priority: dma channel priority
 * @int_type: interrupt type(half/block/transfer)
 * @trans_type: transfer type(memory/cyclic/sg)
 * @desc_lock: operation lock
 * @chain_pending: dma pending descriptor list
 * @idle: channel state
 * @desc_pool: descriptor pool
 */
struct lombo_dma_chan {
	int				idx;
	struct device			*dev;
	struct dma_chan			chan;
	struct dma_async_tx_descriptor	desc;
	struct lombo_dma_chan_cfg	cfg;
	void __iomem			*base;

	/* channel's basic info */
	struct tasklet_struct		tasklet;
	int				slave_id;
	int				priority;
	unsigned long			int_type;
	unsigned long			trans_type;

	/* list for desc */
	spinlock_t			desc_lock;
	struct list_head		chain_pending;
	struct list_head		chain_inprogress;
	bool				idle;

	struct dma_pool			*desc_pool;

	/* reg write async */
	struct tasklet_struct		reg_tasklet;
	bool				in_terminate;
};

/**
 * lombo_dma_hw_desc - lombo dma physical descriptor
 * @src_addr: source address
 * @dst_addr: destination address
 * @block_len: the transfer block length
 * @llp: link list pointer
 * @ctl0: controller register 0
 * @ctl1: controller register 1
 */
struct lombo_dma_hw_desc {
	u32	src_addr;
	u32	res1;
	u32	dst_addr;
	u32	res2;
	u32	block_len;
#ifdef CONFIG_ARCH_LOMBO_N7
	u32	res3;
	u32	llp;
	u32	res4;
	u32	ctl0;
	u32	ctl1;
	u32	res[6];
#else
	u32	ctl;
	u32	llp;
	u32	res3;
#endif
};

/**
 * lombo_dma_desc - lombo physical dma channel descriptor
 * @desc: dma controller identify dma physical descriptor
 * @node: descriptor list
 * @tx_node: descriptor list link
 * @tx_list: the descriptor list for process
 * @async_tx: async transfer descriptor
 */
struct lombo_dma_desc {
	struct lombo_dma_hw_desc	desc;
	struct list_head		node;
	struct list_head		tx_node;
	struct list_head		tx_list;
	struct dma_async_tx_descriptor	async_tx;
};

/**
 * lombo_dma_device - lombo dma controller struct
 * @nr_chan: physical channel number
 * @base: dma controller base address
 * @dev: dma controller device struct
 * @device: dmaengine dma_device struct
 * @echan: lombo physical channel pointer
 * @irq: dma controller irq
 * @axi_gate: axi dma master clk gating
 * @ahb_gate: ahb dma bus clk gating
 * @ahb_reset: ahb dma bus reset
 */
struct lombo_dma_device {
	int			nr_chan;
	void __iomem		*base;
	struct device		*dev;
	struct dma_device	device;
	struct lombo_dma_chan	*echan;
	int			irq;
	struct clk		*axi_gate;
	struct clk		*ahb_gate;
	struct clk		*ahb_reset;
#ifndef CONFIG_ARCH_LOMBO_N7
	u32			frequency;
	struct clk		*dma_clk;
	struct clk		*parent;
#endif
};

/**
 * lombo_dma_data - lombo dma client param
 * @dma_request: request periperal number
 * @priority: dma channel priority, 0 is the lowest, from 0 to 7
 */
struct lombo_dma_data {
	int slave_id;
	int priority;
};

#endif /* __LOMBO_DMA_H */

