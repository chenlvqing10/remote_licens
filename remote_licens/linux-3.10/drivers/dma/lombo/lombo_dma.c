/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * DMA driver code for LomboTech
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

#include "lombo_dma.h"
#include <linux/err.h>
#ifdef CONFIG_ARCH_LOMBO_N7
#include "csp/n7/dma_operate.h"
#else
#include "csp/n9/dma_operate.h"
#define ctl1	ctl
#endif

#define to_edma_chan(chan) container_of(chan, struct lombo_dma_chan, chan)
#define tx_to_edma_desc(tx) container_of(tx, struct lombo_dma_desc, async_tx)
#define lh_to_edma_desc(lh) container_of(lh, struct lombo_dma_desc, tx_list)
#define tn_to_edma_desc(tn) container_of(tn, struct lombo_dma_desc, node)

static int byte_to_width(struct device *dev, enum dma_slave_buswidth blen)
{
	int width;

	if (blen == DMA_SLAVE_BUSWIDTH_1_BYTE)
		width = CH_1_BYTE_WIDTH;
	else if (blen == DMA_SLAVE_BUSWIDTH_2_BYTES)
		width = CH_2_BYTES_WIDTH;
	else if (blen == DMA_SLAVE_BUSWIDTH_4_BYTES)
		width = CH_4_BYTES_WIDTH;
	else if (blen == DMA_SLAVE_BUSWIDTH_8_BYTES)
		width = CH_8_BYTES_WIDTH;
	else {
		dev_warn(dev, "warn width %d undefine, set default\n", blen);
		width = DMA_SLAVE_BUSWIDTH_1_BYTE;
	}

	return width;
}

static int width_to_byte(int width)
{
	int byte_len = 1;

	if (width == CH_1_BYTE_WIDTH)
		byte_len = 1;
	else if (width == CH_2_BYTES_WIDTH)
		byte_len = 2;
	else if (width == CH_4_BYTES_WIDTH)
		byte_len = 4;
	else if (width == CH_8_BYTES_WIDTH)
		byte_len = 8;

	return byte_len;
}

static int to_burst_len(struct device *dev, u32 burst)
{
	int burst_len;

	if (burst == 1)
		burst_len = CH_1_BURST_LEN;
	else if (burst == 4)
		burst_len = CH_4_BURST_LEN;
	else if (burst == 8)
		burst_len = CH_8_BURST_LEN;
	else {
		dev_warn(dev, "warn burst 0x%x unsupport, set default\n",
			 burst);
		burst_len = CH_8_BURST_LEN;
	}

	return burst_len;
}

static void dma_reg_tasklet(unsigned long data)
{
	struct lombo_dma_chan *echan = (struct lombo_dma_chan *)data;
	u32 half_block, blcok, transfer;
	struct lombo_dma_desc *desc, *tdesc, *head = NULL;

	spin_lock(&echan->desc_lock);

	/* abort */
	if (echan->in_terminate) {
		dev_dbg(echan->dev, "chan%d terminate end\n", echan->idx);
		/* abort channel transfer */
		csp_dma_set_channel_abort(echan->idx,
					CH_ABORT_EN, echan->base);
#ifdef CONFIG_ARCH_LOMBO_N9
		csp_dma_set_channel_abort(echan->idx,
					CH_ABORT_DIS, echan->base);
#endif
		echan->in_terminate = false;
		goto err;
	}

	/* start dma transfer */
	if (list_empty(&echan->chain_pending)) {
		dev_dbg(echan->dev, "chan%d no pending list\n", echan->idx);
		goto err;
	}

	if (!echan->idle) {
		dev_dbg(echan->dev, "chan%d is running\n", echan->idx);
		goto err;
	}

	head = lh_to_edma_desc(echan->chain_pending.next);

	/**
	 * add the new nodes submitted by client to the
	 * tail of inprogress list
	 */
	list_for_each_entry_safe(desc, tdesc, &echan->chain_pending, tx_list) {
		list_del(&desc->tx_list);
		list_add_tail(&desc->tx_list, &echan->chain_inprogress);
	}

	/* set the first desc node to dma channel */
	csp_dma_set_channel_llp(echan->idx, head->async_tx.phys,
						echan->base);
	if (echan->int_type == COMPLETE_INT) {
		half_block = CH_INT_DISABLE;
		blcok = CH_INT_DISABLE;
		transfer = CH_INT_ENABLE;
	} else if (echan->int_type == BLOCK_INT) {
		half_block = CH_INT_DISABLE;
		blcok = CH_INT_ENABLE;
		transfer = CH_INT_DISABLE;
	} else if (echan->int_type == HALF_BLOCK_INT) {
		half_block = CH_INT_ENABLE;
		blcok = CH_INT_DISABLE;
		transfer = CH_INT_DISABLE;
	} else {
		dev_warn(echan->dev, "undef int type %lu\n", echan->int_type);
		goto err;
	}

	/* clear interrupt & set interrupt enable & enable channel */
	csp_dma_clear_channel_all_int(echan->idx, echan->base);
	csp_dma_set_channel_int_en(echan->idx, half_block, transfer,
				   blcok, echan->base);
	csp_dma_set_channel_enable(echan->idx, CH_ENABLE, echan->base);

	echan->idle = false;

err:
	spin_unlock(&echan->desc_lock);
}


/* transfer dmaengine params to n7 dma controller params */
static int xlate_dma_config(struct lombo_dma_chan *echan,
				struct dma_slave_config *cfg)
{
	/* direction */
	if (cfg->direction == DMA_DEV_TO_MEM)
		echan->cfg.direction = CH_DEV_TO_MEM;
	else if (cfg->direction == DMA_MEM_TO_DEV)
		echan->cfg.direction = CH_MEM_TO_DEV;
	else if (cfg->direction == DMA_MEM_TO_MEM)
		echan->cfg.direction = CH_MEM_TO_MEM;
	else {
		dev_err(echan->dev, "direction %d unsupport\n", cfg->direction);
		return -EINVAL;
	}

	/* src and dst width */
	echan->cfg.src_width = byte_to_width(echan->dev, cfg->src_addr_width);
	echan->cfg.dst_width = byte_to_width(echan->dev, cfg->dst_addr_width);

	/* src and dst burst */
	echan->cfg.src_burst = to_burst_len(echan->dev, cfg->src_maxburst);
	echan->cfg.dst_burst = to_burst_len(echan->dev, cfg->dst_maxburst);

	return 0;
}

static int set_lombo_dma_desc(struct lombo_dma_chan *echan, dma_addr_t addr,
				size_t len, enum dma_transfer_direction direct,
				dma_addr_t src, dma_addr_t dest,
				struct lombo_dma_desc *edesc, int two_desc)
{
	unsigned int swidth, dwidth;
	int src_ag, dst_ag;
	int t_swidth, t_dwidth;
	t_swidth = width_to_byte(echan->cfg.src_width);
	t_dwidth = width_to_byte(echan->cfg.dst_width);

	/* source and destination address */
	if (direct == DMA_DEV_TO_MEM) {
		edesc->desc.src_addr = echan->cfg.src_addr;
		edesc->desc.dst_addr = addr;
		src_ag = CH_ADDR_STA;
		dst_ag = CH_ADDR_INC;
	} else if (direct == DMA_MEM_TO_DEV) {
		edesc->desc.src_addr = addr;
		edesc->desc.dst_addr = echan->cfg.dst_addr;
		src_ag = CH_ADDR_INC;
		dst_ag = CH_ADDR_STA;
	} else if (direct == DMA_MEM_TO_MEM) {
		edesc->desc.src_addr = src;
		edesc->desc.dst_addr = dest;
		src_ag = CH_ADDR_INC;
		dst_ag = CH_ADDR_INC;
	} else {
		dev_err(echan->dev, "transfer dev2dev direction no support\n");
		return -1;
	}

	/* llp */
	edesc->desc.llp = 0;

#ifdef CONFIG_ARCH_LOMBO_N7
	/* block len */
	if ((len % t_swidth) != 0) {
		edesc->desc.block_len = 0;
		swidth = min_t(u32, echan->cfg.src_width, echan->cfg.dst_width);
		dwidth = swidth;
	} else {
		edesc->desc.block_len = len / t_swidth - 1;
		swidth = echan->cfg.src_width;
		dwidth = echan->cfg.dst_width;
	}

	/* address and width align */
	if ((edesc->desc.src_addr % (1 << swidth)) != 0) {
		dev_err(echan->dev, "src addr 0x%x no aligned with width %x\n",
			edesc->desc.src_addr, t_swidth);
		return -1;
	}

	if ((edesc->desc.dst_addr % (1 << dwidth)) != 0) {
		dev_err(echan->dev, "dst addr 0x%x no aligned with width %x\n",
			edesc->desc.dst_addr, t_dwidth);
		return -1;
	}

	/* channel control0 */
	edesc->desc.ctl0 = csp_dma_org_channel_ctrl0(src_ag, dst_ag, swidth,
						   dwidth, echan->cfg.src_burst,
						   echan->cfg.dst_burst);

	/* channel control1 */
	edesc->desc.ctl1 = csp_dma_org_channel_ctrl1();
	dev_dbg(echan->dev, "channel-%d hardware descriptor:\n"
			"src_addr: 0x%x\n"
			"dst_addr: 0x%x\n"
			"block_len: 0x%x\n"
			"llp: 0x%x\n"
			"ctl0: 0x%x\n"
			"ctl1: 0x%x\n\n",
			echan->idx, edesc->desc.src_addr, edesc->desc.dst_addr,
			edesc->desc.block_len, edesc->desc.llp,
			edesc->desc.ctl0, edesc->desc.ctl1);
#else
	edesc->desc.block_len = len;
	swidth = echan->cfg.src_width;
	dwidth = echan->cfg.dst_width;

	/**
	 * 1. if len is aligned with both t_swidth and t_dwidth,
	 *    no need to rectify the width
	 * 2. if len is not aligned with t_swidth or t_dwidth,
	 *	two cases like blow:
	 *	2.1
	 *		The len represent the second desc, we just use
	 *		the little width.
	 *	2.2
	 *		there only has one desc, we need to rectify the
	 *		width to make the len aligned with t_swidth and
	 *		t_dwidth.
	 */
	if ((len % t_swidth) != 0 || (len % t_dwidth) != 0) {
		/* len is not aligned with t_swidth or t_dwidth */
		if (two_desc) {
			swidth = min_t(u32, echan->cfg.src_width,
					echan->cfg.dst_width);
			dwidth = swidth;
		} else {
			if (len < t_swidth || len < t_dwidth) {
				/* if dirct is mem_to_mem */
				if (direct == DMA_MEM_TO_MEM) {
					while ((len % (1 << swidth)) != 0)
							--swidth;
					while ((len % (1 << dwidth)) != 0)
							--dwidth;
				} else {
					dev_err(echan->dev,
						"the width of peripheral"
						"device set wrong."
						"t_swidth: %d, "
						"t_dwidth: %d , "
						"len: %d, "
						"func: %s, "
						"line: %d\n",
						t_swidth,
						t_dwidth,
						(u32)len, __func__, __LINE__);
					return -1;
				}
			}
		}
	}

	/* address and width align */
	if ((edesc->desc.src_addr % (1 << swidth)) != 0
		|| (edesc->desc.src_addr % (1 << dwidth)) != 0) {
		dev_err(echan->dev, "src addr 0x%x no aligned with swidth %x or "
					"dwidth %x\n",
			edesc->desc.src_addr, (1 << swidth), (1 << dwidth));
		return -1;
	}

	if ((edesc->desc.dst_addr % (1 << swidth)) != 0) {
		dev_err(echan->dev, "dst addr 0x%x no aligned with swidth %x\n",
			edesc->desc.dst_addr, (1 << swidth));
		return -1;
	}

	edesc->desc.ctl = csp_dma_org_channel_ctrl(src_ag, dst_ag, swidth,
						dwidth,
						echan->cfg.src_burst,
						echan->cfg.dst_burst);
	dev_dbg(echan->dev, "channel-%d hardware descriptor:\n"
			"src_addr: 0x%x\n"
			"dst_addr: 0x%x\n"
			"block_len: 0x%x\n"
			"llp: 0x%x\n"
			"ctl: 0x%x\n",
			echan->idx, edesc->desc.src_addr, edesc->desc.dst_addr,
			edesc->desc.block_len, edesc->desc.llp,
			edesc->desc.ctl);
#endif
	return 0;
}

/**
 * dma_tx_submit - add tx descriptor list to channel pending list
 * @tx: the tx descriptor to submit
 *
 * return dma_cookie_t if success
 */
static dma_cookie_t dma_tx_submit(struct dma_async_tx_descriptor *tx)
{
	struct dma_chan *chan = tx->chan;
	struct lombo_dma_chan *echan = to_edma_chan(chan);
	struct lombo_dma_desc *desc = tx_to_edma_desc(tx);
	struct lombo_dma_desc *child, *tail;
	dma_cookie_t cookie = -EBUSY;

	spin_lock(&echan->desc_lock);

	list_for_each_entry(child, &desc->tx_node, node) {
		cookie = dma_cookie_assign(&child->async_tx);
	}

	if (!list_empty(&echan->chain_pending)) {
		tail = lh_to_edma_desc(echan->chain_pending.prev);
		tail = tn_to_edma_desc(tail->tx_node.prev);
		tail->desc.llp = desc->async_tx.phys;
		csp_dma_set_channel_lli_last(&tail->desc.ctl1, false);

	}

	list_add_tail(&desc->tx_list, &echan->chain_pending);

	spin_unlock(&echan->desc_lock);

	return cookie;
}

/**
 * dma_alloc_descriptor - alloc descriptor for channel
 * @chan: the channel to alloc descriptor
 *
 * return lombo_dma_desc pointer if success, NULL if error
 */
static struct lombo_dma_desc *dma_alloc_descriptor(struct lombo_dma_chan *chan)
{
	struct lombo_dma_desc *desc;
	dma_addr_t desc_add;

	desc = dma_pool_alloc(chan->desc_pool, GFP_ATOMIC, &desc_add);
	if (!desc) {
		dev_err(chan->dev, "not memory for descriptor alloc\n");
		return NULL;
	}

	memset(desc, 0, sizeof(*desc));
	INIT_LIST_HEAD(&desc->tx_list);
	INIT_LIST_HEAD(&desc->node);
	INIT_LIST_HEAD(&desc->tx_node);
	dma_async_tx_descriptor_init(&desc->async_tx, &chan->chan);

	desc->async_tx.tx_submit = dma_tx_submit;
	desc->async_tx.phys = desc_add;
	return desc;
}

/**
 * lombo_dma_alloc_chan_res - alloc resources for channel
 * @chan: the channel to alloc resources
 *
 * return 1 if success, otherwise error
 */
static int lombo_dma_alloc_chan_res(struct dma_chan *chan)
{
	struct lombo_dma_chan *echan = to_edma_chan(chan);

	if (echan->desc_pool)
		return 1;

	echan->desc_pool = dma_pool_create(dev_name(&chan->dev->device),
					   echan->dev,
					   sizeof(struct lombo_dma_desc),
					   64,
					   0);

	if (!echan->desc_pool) {
		dev_err(echan->dev, "unable to alloc descriptor pool\n");
		return -ENOMEM;
	}

	echan->idle = true;

	return 1;
}

/**
 * lombo_dma_free_desc_list - free descriptor list resources
 * @echan: the channel to free descriptor list
 * @list: which descriptor list
 */
static void lombo_dma_free_desc_list(struct lombo_dma_chan *echan,
				     struct list_head *list)
{
	struct lombo_dma_desc *desc, *tdesc;

	list_for_each_entry_safe(desc, tdesc, list, node) {
		list_del(&desc->node);
		dma_pool_free(echan->desc_pool, desc, desc->async_tx.phys);
	}
}

static void lombo_dma_free_tx_list(struct lombo_dma_chan *echan,
					struct list_head *list)
{
	struct lombo_dma_desc *desc, *tdesc;

	list_for_each_entry_safe(desc, tdesc, list, tx_list) {
		lombo_dma_free_desc_list(echan, &desc->tx_node);
	}
	INIT_LIST_HEAD(list);
}

/**
 * lombo_dma_free_chan_res - free channel resources
 * @chan: the channel to free resources
 */
static void lombo_dma_free_chan_res(struct dma_chan *chan)
{
	struct lombo_dma_chan *echan = to_edma_chan(chan);
	unsigned long flags;

	spin_lock_irqsave(&echan->desc_lock, flags);
	lombo_dma_free_tx_list(echan, &echan->chain_pending);
	lombo_dma_free_tx_list(echan, &echan->chain_inprogress);
	echan->idle = true;
	/* we should clear the cookie of this channel */
	dma_cookie_init(&echan->chan);
	spin_unlock_irqrestore(&echan->desc_lock, flags);
	dma_pool_destroy(echan->desc_pool);
	echan->desc_pool = NULL;
}

/**
 * lombo_dma_tx_status - get channel transfer status
 */
static enum dma_status lombo_dma_tx_status(struct dma_chan *chan,
					   dma_cookie_t cookie,
					   struct dma_tx_state *txstate)
{
	enum dma_status ret;

	ret = dma_cookie_status(chan, cookie, txstate);
	return ret;
}

/**
 * lombo_dma_issue_pending - issue dma channel transfer
 */
static void lombo_dma_issue_pending(struct dma_chan *chan)
{
	struct lombo_dma_chan *echan = to_edma_chan(chan);
	unsigned long flags;

	spin_lock_irqsave(&echan->desc_lock, flags);
	tasklet_schedule(&echan->reg_tasklet);
	spin_unlock_irqrestore(&echan->desc_lock, flags);
}

static int dma_breakup_block(int s_width, int d_width, size_t len,
						size_t *tlen0, size_t *tlen1)
{
	int bigwidth;

	if (!d_width) {
		/* N7 branch */
		if ((len % s_width) == 0) {
			*tlen0 = len;
			return 0;
		} else if (((len % s_width) != 0) && (len > s_width)) {
			*tlen1 = len % s_width;
			*tlen0 = len - *tlen1;
			return 1;
		} else if (((len % s_width) != 0) && (len < s_width)) {
			*tlen0 = len;
			return 0;
		}
	} else {
		/* N9 branch */
		/* not aligned with s_width or d_width */
		if (((len % s_width) != 0 || (len % d_width) != 0)
			&& (len > s_width)
			&& (len > d_width)) {
			if ((len % s_width) != 0 && (len % d_width) != 0) {
				pr_err(
					"dma len %d is "
					"not aligned with s_width: %d, and "
					"d_width: %d, "
					"func: %s, line: %d\n",
					(u32)len,
					s_width,
					d_width,
					__func__, __LINE__);
				return -1;
			}
			bigwidth = max_t(u32, s_width, d_width);

			*tlen1 = len % bigwidth;
			*tlen0 = len - *tlen1;
			return 1;
		} else {
			/**
			 * there has three case here:
			 * 1. len is aligned with both s_width and d_width
			 * 2. len is not aligned with s_width or d_width,
			 *    and len < s_width
			 * 3. len is not aligned with s_width or d_width,
			 *    and len < d_width
			 * all the three cases above, we all set *tlen0 = len,
			 * and we deal with the three cases in func
			 * set_lombo_dma_desc.
			 */
			*tlen0 = len;
			return 0;
		}
	}
	return 0;
}

/**
 * lombo_dma_prep_memcpy - get memcpy tx descriptor
 */
static struct dma_async_tx_descriptor *lombo_dma_prep_memcpy(
							struct dma_chan *chan,
							dma_addr_t dest,
							dma_addr_t src,
							size_t len,
							unsigned long flags)
{
	struct lombo_dma_chan *echan = to_edma_chan(chan);
	struct lombo_dma_desc *first = NULL, *prev = NULL, *desc = NULL;
	dma_addr_t dest_addr, src_addr;
	size_t copy = 0;
	size_t tlen0 = 0, tlen1 = 0, avail = 0;
	int two_desc;
	int bwidth, dwidth;

	if (!len) {
		dev_err(echan->dev, "%s bad params\n", __func__);
		return NULL;
	}

	/* transfer complete interrupt */
	echan->int_type = COMPLETE_INT;
	echan->trans_type = MEMORY_TRANS;

	bwidth = width_to_byte(echan->cfg.src_width);
	dwidth = width_to_byte(echan->cfg.dst_width);
	dest_addr = dest;
	src_addr = src;
	avail = len;

	do {
		copy = min_t(size_t, avail, DMA_MAX_DESC_BYTES);
#ifdef CONFIG_ARCH_LOMBO_N7
		two_desc = dma_breakup_block(bwidth, 0,
						copy, &tlen0, &tlen1);
#else
		two_desc = dma_breakup_block(bwidth,
					dwidth, copy, &tlen0, &tlen1);
#endif
		if (two_desc == -1)
			goto fail;

		desc = dma_alloc_descriptor(echan);
		if (!desc) {
			dev_err(echan->dev, "alloc desc fail\n");
			goto fail;
		}

		if (set_lombo_dma_desc(echan, 0, tlen0,
			DMA_MEM_TO_MEM, src_addr, dest_addr, desc, 0)) {
			dma_pool_free(echan->desc_pool, desc,
						desc->async_tx.phys);
			goto fail;
		}

		if (!first)
			first = desc;
		else
			prev->desc.llp = desc->async_tx.phys;

		desc->async_tx.cookie = 0;
		async_tx_ack(&desc->async_tx);
		prev = desc;
		list_add_tail(&desc->node, &first->tx_node);
		dest_addr += tlen0;
		src_addr += tlen0;
		avail -= tlen0;

		/* set second descriptor if exist */
		if (two_desc == 1) {
			desc = dma_alloc_descriptor(echan);
			if (!desc) {
				dev_err(echan->dev, "alloc desc fail\n");
				goto fail;
			}

			if (set_lombo_dma_desc(echan, 0, tlen1,
					DMA_MEM_TO_MEM, src_addr,
					dest_addr, desc, 1))
				goto fail;
			prev->desc.llp = desc->async_tx.phys;
			desc->async_tx.cookie = 0;
			async_tx_ack(&desc->async_tx);
			prev = desc;
			list_add_tail(&desc->node, &first->tx_node);
			dest_addr += tlen1;
			src_addr += tlen1;
			avail -= tlen1;
		}
	} while (avail);

	/* set last descriptor lli to last */
	csp_dma_set_channel_lli_last(&desc->desc.ctl1, true);

	first->async_tx.flags = flags;

	return &first->async_tx;

fail:
	if (first)
		lombo_dma_free_desc_list(echan, &first->tx_node);
	return NULL;
}

/**
 * lombo_dma_prep_slave_sg - get slave sg tx descriptor
 */
static struct dma_async_tx_descriptor *lombo_dma_prep_slave_sg(
					struct dma_chan *chan,
					struct scatterlist *sgl,
					unsigned int sg_len,
					enum dma_transfer_direction direction,
					unsigned long flags,
					void *context)
{
	struct lombo_dma_chan *echan = to_edma_chan(chan);
	struct lombo_dma_desc *first = NULL, *prev = NULL, *desc = NULL;
	struct scatterlist *sg;
	dma_addr_t desc_addr;
	size_t len, avail;
	size_t tlen0 = 0, tlen1 = 0;
	int two_desc;
	int bwidth, dwidth;
	int i;

	if ((sgl == NULL) || (sg_len == 0)) {
		dev_err(echan->dev, "%s bad params\n", __func__);
		return NULL;
	}

	/* transfer complete interrupt */
	echan->int_type = COMPLETE_INT;
	echan->trans_type = SG_TRANS;

	bwidth = width_to_byte(echan->cfg.src_width);
	dwidth = width_to_byte(echan->cfg.dst_width);

	for_each_sg(sgl, sg, sg_len, i) {
		desc_addr = sg_dma_address(sg);
		avail = sg_dma_len(sg);
		two_desc = -1;

		do {
			len = min_t(size_t, avail, DMA_MAX_DESC_BYTES);

#ifdef CONFIG_ARCH_LOMBO_N7
			two_desc = dma_breakup_block(bwidth, 0,
							len, &tlen0, &tlen1);
#else
			two_desc = dma_breakup_block(bwidth, dwidth,
							len, &tlen0, &tlen1);
#endif
			if (two_desc == -1)
				goto fail;

			/* set first descriptor */
			desc = dma_alloc_descriptor(echan);
			if (!desc) {
				dev_err(echan->dev, "alloc desc fail\n");
				goto fail;
			}

			if (set_lombo_dma_desc(echan, desc_addr, tlen0,
						direction, 0, 0, desc, 0)) {
				dma_pool_free(echan->desc_pool,
						desc, desc->async_tx.phys);
				goto fail;
			}

			if (!first)
				first = desc;
			else
				prev->desc.llp = desc->async_tx.phys;

			desc->async_tx.cookie = 0;
			async_tx_ack(&desc->async_tx);
			prev = desc;
			list_add_tail(&desc->node, &first->tx_node);
			desc_addr += tlen0;
			avail -= tlen0;

			/* set second descriptor if exist */
			if (two_desc == 1) {
				desc = dma_alloc_descriptor(echan);
				if (!desc) {
					dev_err(echan->dev, "alloc desc fail\n");
					goto fail;
				}

				if (set_lombo_dma_desc(echan, desc_addr, tlen1,
						direction, 0, 0, desc, 1))
					goto fail;
				prev->desc.llp = desc->async_tx.phys;
				desc->async_tx.cookie = 0;
				async_tx_ack(&desc->async_tx);
				prev = desc;
				list_add_tail(&desc->node, &first->tx_node);
				desc_addr += tlen1;
				avail -= tlen1;
			}
		} while (avail);
	}

	/* set last descriptor lli to last */
	csp_dma_set_channel_lli_last(&desc->desc.ctl1, true);

	first->async_tx.flags = flags;

	return &first->async_tx;

fail:
	if (first)
		lombo_dma_free_desc_list(echan, &first->tx_node);
	return NULL;
}

struct dma_async_tx_descriptor *lombo_dma_prep_cyclic(
					struct dma_chan *chan,
					dma_addr_t buf_addr,
					size_t buf_len,
					size_t period_len,
					enum dma_transfer_direction direction,
					unsigned long flags,
					void *context)
{
	struct lombo_dma_chan *echan = to_edma_chan(chan);
	struct lombo_dma_desc *first = NULL, *prev = NULL, *desc = NULL;
	dma_addr_t addr;
	int i, slot;

	if ((buf_len == 0) || (period_len == 0)) {
		dev_err(echan->dev, "%s bad params\n", __func__);
		return NULL;
	}

	if ((buf_len % period_len) != 0) {
		dev_err(echan->dev, "period should be multi of buffer len\n");
		return NULL;
	}

	/* block complete interrupt */
	echan->int_type = BLOCK_INT;
	echan->trans_type = CYCLIC_TRANS;

	slot = buf_len / period_len;
	dev_dbg(echan->dev, "buf len: 0x%x, period len: 0x%x, slot: %d\n",
		(u32)buf_len, (u32)period_len, slot);
	for (i = 0; i < slot; i++) {
		addr = buf_addr + i * period_len;

		/* set first descriptor */
		desc = dma_alloc_descriptor(echan);
		if (!desc) {
			dev_err(echan->dev, "%s desc fail\n", __func__);
			goto fail;
		}

		if (set_lombo_dma_desc(echan, addr, period_len, direction,
				   0, 0, desc, 0)) {
			dma_pool_free(echan->desc_pool, desc,
						desc->async_tx.phys);
			goto fail;
		}
		if (!first)
			first = desc;
		else
			prev->desc.llp = desc->async_tx.phys;

		desc->async_tx.cookie = 0;
		async_tx_ack(&desc->async_tx);
		prev = desc;
		list_add_tail(&desc->node, &first->tx_node);
	}

	/* set last descriptor lli to no last, create cyclic transfer */
	csp_dma_set_channel_lli_last(&desc->desc.ctl1, false);
	desc->desc.llp = first->async_tx.phys;

	first->async_tx.flags = flags;

	return &first->async_tx;

fail:
	if (first)
		lombo_dma_free_desc_list(echan, &first->tx_node);
	return NULL;
}

/**
 * lombo_dma_control - dma command control
 */
static int lombo_dma_control(struct dma_chan *chan, enum dma_ctrl_cmd cmd,
			     unsigned long arg)
{
	struct lombo_dma_chan *echan = to_edma_chan(chan);
	struct device *dev = echan->dev;
	struct dma_slave_config *cfg = (void *)arg;
	unsigned long flags;
	int ret = 0;

	switch (cmd) {
	case DMA_TERMINATE_ALL:
		spin_lock_irqsave(&echan->desc_lock, flags);
		echan->in_terminate = true;
		tasklet_schedule(&echan->reg_tasklet);
		lombo_dma_free_tx_list(echan, &echan->chain_pending);
		lombo_dma_free_tx_list(echan, &echan->chain_inprogress);
		echan->idle = true;
		spin_unlock_irqrestore(&echan->desc_lock, flags);
		break;
	case DMA_PAUSE:
		csp_dma_halt_channel_trans(echan->idx, echan->base);
		break;
	case DMA_RESUME:
		/* resume block chain lli transfer */
		csp_dma_resume_channel_trans(echan->idx, echan->base);
		break;
	case DMA_SLAVE_CONFIG:
		dev_dbg(dev, "ch%d slave config: src_addr: 0x%p\n"
			"dst_addr: 0x%p\n"
			"direction: 0x%x\n"
			"src addr width: 0x%ux\n"
			"dst addr width: 0x%ux\n"
			"src burst: 0x%ux\n"
			"dst burst: 0x%ux\n",
			echan->idx,
			(void *)cfg->src_addr, (void *)cfg->dst_addr,
			cfg->direction,	cfg->src_addr_width,
			cfg->dst_addr_width, cfg->src_maxburst,
			cfg->dst_maxburst);

		echan->cfg.src_addr = cfg->src_addr;
		echan->cfg.dst_addr = cfg->dst_addr;

		/* get dma channel config */
		ret = xlate_dma_config(echan, cfg);
		if (ret < 0)
			return ret;

		/* set channel cfg */
		csp_dma_set_channel_cfg(echan->idx, echan->cfg.direction,
					echan->priority, echan->slave_id,
					echan->base);
		break;
	default:
		return -ENOSYS;
	}

	return ret;
}

static irqreturn_t lombo_dma_int_handler(int irq, void *dev_id)
{
	struct lombo_dma_device *edev = dev_id;
	u32 err_int;
	u32 i, int_num = 0;
	struct lombo_dma_desc *desc, *tdesc;
	struct dma_async_tx_descriptor *txd;

	for (i = 0; i < edev->nr_chan; i++) {
		if (csp_dma_is_ch_comb_int_pd(i, edev->base)) {
			int_num++;
			/* trans interrupt */
			if (csp_dma_is_ch_tran_int_pd(i, edev->base))
				csp_dma_clear_ch_tran_int_pd(i, edev->base);
			/* block interrupt */
			if (csp_dma_is_ch_blk_int_pd(i, edev->base))
				csp_dma_clear_ch_blk_int_pd(i, edev->base);
			/* half block interrupt */
			if (csp_dma_is_ch_hblk_int_pd(i, edev->base))
				csp_dma_clear_ch_hblk_int_pd(i, edev->base);
			/* halt interrput */
			if (csp_dma_is_ch_halt_int_pd(i, edev->base)) {
				csp_dma_clear_ch_halt_int_pd(i, edev->base);
				continue;
			}
			/* error interrupt */
			err_int = csp_dma_is_ch_err_int_pd(i, edev->base);
			if (err_int) {
				csp_dma_clear_ch_err_int_pd(i, edev->base);
				dev_err(edev->dev, "ch%d err %x\n", i, err_int);
				continue;
			}

			/* cyclic transfer callback */
			if (edev->echan[i].trans_type == CYCLIC_TRANS) {
				list_for_each_entry_safe(desc, tdesc,
					&edev->echan[i].chain_inprogress,
					tx_list) {
					txd = &desc->async_tx;
					if (txd->callback)
						txd->callback(
						txd->callback_param);
				}
			} else if (edev->echan[i].trans_type == MEMORY_TRANS ||
				edev->echan[i].trans_type == SG_TRANS) {
				/* tasklet schedule callback */
				tasklet_schedule(&edev->echan[i].tasklet);
			}
		}
	}

	if (int_num)
		return IRQ_HANDLED;
	else
		return IRQ_NONE;
}

static void dma_do_tasklet(unsigned long data)
{
	struct lombo_dma_chan *echan = (struct lombo_dma_chan *)data;
	struct lombo_dma_desc *desc, *tdesc;
	struct dma_async_tx_descriptor *txd;
	enum dma_status ret;

	spin_lock(&echan->desc_lock);
	echan->idle = true;

	if (!list_empty(&echan->chain_inprogress)) {
		desc = lh_to_edma_desc(echan->chain_inprogress.prev);

		/* this desc has been completed*/
		desc = tn_to_edma_desc(desc->tx_node.prev);
		dma_cookie_complete(&desc->async_tx);
	}

	list_for_each_entry_safe(desc, tdesc,
				&echan->chain_inprogress, tx_list) {
		txd = &desc->async_tx;

		ret = dma_cookie_status(txd->chan, txd->cookie, NULL);
		if (ret != DMA_SUCCESS)
			continue;

		list_del(&desc->tx_list);

		if (txd->callback) {
			spin_unlock(&echan->desc_lock);
			txd->callback(txd->callback_param);
			spin_lock(&echan->desc_lock);
		}

		/* free the descs */
		lombo_dma_free_desc_list(echan, &desc->tx_node);
	}

	if (list_empty(&echan->chain_inprogress))
		INIT_LIST_HEAD(&echan->chain_inprogress);
	spin_unlock(&echan->desc_lock);
}

static bool lombo_filter_fn(struct dma_chan *chan, void *fn_param)
{
	struct lombo_dma_chan *echan = to_edma_chan(chan);
	struct lombo_dma_data *data = fn_param;

	echan->slave_id = data->slave_id;
	echan->priority = data->priority;

	dev_info(echan->dev, "request channel-%d slave id %d, priority %d\n",
		echan->idx, echan->slave_id, echan->priority);
	return true;
}

static struct dma_chan *lombo_dma_xlate(struct of_phandle_args *dma_spec,
					struct of_dma *ofdma)
{
	struct lombo_dma_device *dev = ofdma->of_dma_data;
	dma_cap_mask_t mask = dev->device.cap_mask;
	struct lombo_dma_data data;

	if (dma_spec->args_count != 2) {
		dev_err(dev->dev, "device dma of param error\n");
		return NULL;
	}

	data.slave_id = dma_spec->args[0];
	data.priority = dma_spec->args[1];

	return dma_request_channel(mask, lombo_filter_fn, &data);
}

static int lombo_dma_remove(struct platform_device *op)
{
	struct lombo_dma_device *dev = platform_get_drvdata(op);

	/* release clk and reset */
	clk_disable_unprepare(dev->axi_gate);
	clk_disable_unprepare(dev->ahb_gate);
	clk_disable_unprepare(dev->ahb_reset);

	devm_clk_put(dev->dev, dev->axi_gate);
	devm_clk_put(dev->dev, dev->ahb_gate);
	devm_clk_put(dev->dev, dev->ahb_reset);
#ifndef CONFIG_ARCH_LOMBO_N7
	clk_disable_unprepare(dev->dma_clk);
	clk_disable_unprepare(dev->parent);

	devm_clk_put(dev->dev, dev->dma_clk);
	devm_clk_put(dev->dev, dev->parent);
#endif
	dma_async_device_unregister(&dev->device);
	return 0;
}

static struct of_device_id lombo_dma_dt_ids[] = {
	{ .compatible = "lombo,n7-dma", },
	{ .compatible = "lombo,n9-dma", },
	{}
};
MODULE_DEVICE_TABLE(of, lombo_dma_dt_ids);

static int lombo_dma_probe(struct platform_device *op)
{
	struct lombo_dma_device *dev;
	struct lombo_dma_chan *echan;
	const struct of_device_id *of_id;
	struct resource *iores;
	struct device_node *np;
	int i, irq;
	int ret = 0;
	int dma_channels = 0;

	np = op->dev.of_node;
	if (!np) {
		dev_err(&op->dev, "%s dma dt node null error\n", __func__);
		return -EINVAL;
	}

	dev = devm_kzalloc(&op->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		dev_err(&op->dev, "%s alloc lombo_dma_device fail\n", __func__);
		return -ENOMEM;
	}
	dev->dev = &op->dev;

	/* clk res and reset */
	dev->axi_gate = devm_clk_get(dev->dev, "axi_dma_gate");
	dev->ahb_gate = devm_clk_get(dev->dev, "ahb_dma_gate");
	dev->ahb_reset = devm_clk_get(dev->dev, "ahb_dma_reset");

#ifndef CONFIG_ARCH_LOMBO_N7
	dev->dma_clk = devm_clk_get(dev->dev, "dma_clk");
	dev->parent = devm_clk_get(dev->dev, "dma_clk_parent");

	if (IS_ERR(dev->parent)) {
		pr_err("dma clk get parent clk failed\n");
		return -EINVAL;
	}

	if ((IS_ERR(dev->dma_clk))) {
		dev_err(&op->dev, "get dma_clk failed\n");
		return -EINVAL;
	}

	ret = of_property_read_u32(op->dev.of_node, "default_frequency",
					&dev->frequency);
	if (ret < 0) {
		dev_err(&op->dev, "invalid frequency failed\n");
		return -EINVAL;
	}

	/* set the secondary clock */
	ret = clk_set_parent(dev->dma_clk, dev->parent);
	/* set the clock frequency */
	clk_set_rate(dev->dma_clk, dev->frequency);
	if (ret) {
		dev_err(&op->dev, "dma clk set parent clk failed\n");
		return ret;
	}

	ret = clk_prepare_enable(dev->dma_clk);
	if (ret) {
		dev_err(&op->dev, "enable dma_clk failed\n");
		return ret;
	}
#endif
	if ((IS_ERR(dev->axi_gate)) || (IS_ERR(dev->ahb_gate)) ||
		(IS_ERR(dev->ahb_reset))) {
		dev_err(&op->dev, "get dma gate/reset clk failed\n");
		return -EINVAL;
	}

	ret = clk_prepare_enable(dev->axi_gate);
	if (ret) {
		dev_err(&op->dev, "enable dma axi clk failed\n");
		return ret;
	}

	ret = clk_prepare_enable(dev->ahb_gate);
	if (ret) {
		dev_err(&op->dev, "enable dma ahb clk failed\n");
		return ret;
	}

	ret = clk_prepare_enable(dev->ahb_reset);
	if (ret) {
		dev_err(&op->dev, "enable dma ahb reset failed\n");
		return ret;
	}

	platform_set_drvdata(op, dev);
	iores = platform_get_resource(op, IORESOURCE_MEM, 0);
	if (!iores) {
		dev_err(&op->dev, "%s get resources fail\n", __func__);
		return -EINVAL;
	}

	dev->base = devm_ioremap_resource(dev->dev, iores);
	if (IS_ERR(dev->base)) {
		dev_err(&op->dev, "%s get io map fail\n", __func__);
		return PTR_ERR(dev->base);
	}

	of_id = of_match_device(lombo_dma_dt_ids, dev->dev);
	if (of_id)
		of_property_read_u32(dev->dev->of_node, "dma-channels",
				     &dma_channels);
	else
		dma_channels = 8;

	dev->nr_chan = dma_channels;

	dev->echan = devm_kzalloc(dev->dev,
				  dma_channels * sizeof(struct lombo_dma_chan),
				  GFP_KERNEL);
	if (!dev->echan) {
		dev_err(&op->dev, "%s alloc lombo_dma_chan fail\n", __func__);
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&dev->device.channels);

	/* all channel share one irq, get and register */
	irq = platform_get_irq(op, 0);
	ret = request_threaded_irq(irq, NULL, lombo_dma_int_handler,
					IRQF_ONESHOT, "dma", dev);
	if (ret < 0) {
		dev_err(&op->dev, "request irq %d fail, err %d\n", irq, ret);
		return ret;
	}

	/* init each channel */
	for (i = 0; i < dma_channels; i++) {
		echan = &dev->echan[i];
		echan->idx = i;
		echan->base = dev->base;
		spin_lock_init(&echan->desc_lock);
		echan->dev = dev->dev;
		echan->chan.device = &dev->device;
		dma_cookie_init(&echan->chan);
		tasklet_init(&echan->tasklet, dma_do_tasklet,
			     (unsigned long)echan);
		tasklet_init(&echan->reg_tasklet, dma_reg_tasklet,
			     (unsigned long)echan);
		INIT_LIST_HEAD(&echan->chain_pending);
		INIT_LIST_HEAD(&echan->chain_inprogress);

		/* register chan to dmaengine */
		list_add_tail(&echan->chan.device_node, &dev->device.channels);
	}

	/* set dma capability */
	dma_cap_zero(dev->device.cap_mask);
	dma_cap_set(DMA_MEMCPY, dev->device.cap_mask);
	dma_cap_set(DMA_SLAVE, dev->device.cap_mask);
	dma_cap_set(DMA_CYCLIC, dev->device.cap_mask);

	dev->device.dev = &op->dev;
	dev->device.device_alloc_chan_resources = lombo_dma_alloc_chan_res;
	dev->device.device_free_chan_resources = lombo_dma_free_chan_res;
	dev->device.device_tx_status = lombo_dma_tx_status;
	dev->device.device_issue_pending = lombo_dma_issue_pending;
	dev->device.device_control = lombo_dma_control;
	dev->device.device_prep_dma_memcpy = lombo_dma_prep_memcpy;
	dev->device.device_prep_slave_sg = lombo_dma_prep_slave_sg;
	dev->device.device_prep_dma_cyclic = lombo_dma_prep_cyclic;

	/* register dma controller to dmaengine */
	ret = dma_async_device_register(&dev->device);
	if (ret) {
		dev_err(&op->dev, "%s: dmaengine register fail\n", __func__);
		return ret;
	}

	/* register dma of controller helper */
	ret = of_dma_controller_register(np, lombo_dma_xlate, dev);
	if (ret) {
		dev_err(&op->dev, "failed to register dma of controller\n");
		dma_async_device_unregister(&dev->device);
		return ret;
	}

	return 0;
}

#ifdef CONFIG_PM
static int lombo_dma_suspend(struct device *_dev)
{
	struct platform_device *pdev;
	struct lombo_dma_device *dev;

	pdev = to_platform_device(_dev);
	dev = platform_get_drvdata(pdev);

	clk_disable_unprepare(dev->ahb_reset);
	clk_disable_unprepare(dev->ahb_gate);
	clk_disable_unprepare(dev->axi_gate);

	/* Do other things about your module's suspend */
	return 0;
}

static int lombo_dma_resume(struct device *_dev)
{
	struct platform_device *pdev;
	struct lombo_dma_device *dev;
	int ret;

	pdev = to_platform_device(_dev);
	dev = platform_get_drvdata(pdev);

	ret = clk_prepare_enable(dev->axi_gate);
	ret = clk_prepare_enable(dev->ahb_gate);
	ret = clk_prepare_enable(dev->ahb_reset);
	/* Do others things about your module's resume */

	return 0;
}

static const struct dev_pm_ops lbdma_pmops = {
	SET_SYSTEM_SLEEP_PM_OPS(lombo_dma_suspend, lombo_dma_resume)
};
#endif/* #ifdef CONFIG_PM  */

static struct platform_driver lombo_dma_driver = {
	.driver = {
		.name = "lombo-dma",
		.owner = THIS_MODULE,
		.of_match_table = lombo_dma_dt_ids,
		.pm = &lbdma_pmops,
	},
	.probe = lombo_dma_probe,
	.remove = lombo_dma_remove,
};

static int __init lombo_dma_init(void)
{
	return platform_driver_register(&lombo_dma_driver);
}
subsys_initcall(lombo_dma_init);

MODULE_DESCRIPTION("LomboTech DMA Driver");
MODULE_AUTHOR("LomboTech Co.Ltd");
MODULE_LICENSE("GPL v2");
