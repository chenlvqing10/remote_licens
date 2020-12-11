/*
 * gmac.c - Driver of LomboTech GMAC Controller
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

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <linux/if_ether.h>
#include <linux/crc32.h>
#include <linux/mii.h>
#include <linux/if.h>
#include <linux/if_vlan.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/prefetch.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_net.h>
#include <linux/phy.h>

#include <mach/csp.h>
#include <mach/debug.h>

#include "gmac.h"
#if defined(CONFIG_ARCH_LOMBO_N7V1)
#include "csp/n7/gmac_const.h"
#include "csp/n7/gmac_csp.h"
#else
#error "No supported platform!"
#endif

static void lombo_gmac_tx_clean(struct lombo_gmac *lgmac);
static void lombo_gmac_tx_error(struct lombo_gmac *lgmac);
static void lombo_gamc_rx_refill(struct lombo_gmac *lgmac);
static irqreturn_t lombo_gmac_irq(int irq, void *dev_id);

/**
 * lombo_gmac_desc_tx_set_owner - set owner bit of tx desc, so DMA own the desc.
 * @desc: pointer to idma desc.
 */
static inline void lombo_gmac_desc_tx_set_owner(struct idma_desc *desc)
{
	desc->des0.tx.own = 1;
}

/**
 * lombo_gmac_desc_tx_get_owner - get owner of tx desc.
 * @desc: pointer to idma desc.
 *
 * return 0 if Controller own the desc; 1 if DMA own the desc.
 */
static inline u32 lombo_gmac_desc_tx_get_owner(struct idma_desc *desc)
{
	return desc->des0.tx.own;
}

/**
 * lombo_gmac_desc_tx_buf_len - get buffer length from tx desc.
 * @desc: pointer to idma desc.
 *
 * return buffer length of tx desc.
 */
static inline u32 lombo_gmac_desc_tx_buf_len(struct idma_desc *desc)
{
	return desc->des1.tx.buffer1_size;
}

/**
 * lombo_gmac_desc_tx_last_segment - check if tx desc is the last segment.
 * @desc: pointer to idma desc.
 *
 * return 0 if not the last segment; 1 if is the last segment.
 */
static inline u32 lombo_gmac_desc_tx_last_segment(struct idma_desc *desc)
{
	return desc->des1.tx.last_segment;
}

/**
 * lombo_gmac_desc_tx_clear_ic - clear 'interrupt on completion' bit of tx desc.
 * @desc: pointer to idma desc.
 */
static inline void lombo_gmac_desc_tx_clear_ic(struct idma_desc *desc)
{
	desc->des1.tx.interrupt = 0;
}

/**
 * lombo_gmac_desc_tx_init - init one tx desc.
 * @desc: pointer to idma desc.
 * @end: 0 means not the last one in the desc ring; !0 means the last one.
 */
static inline void lombo_gmac_desc_tx_init(struct idma_desc *desc, int end)
{
	desc->des0.tx.own = 0;
	if (end)
		desc->des1.tx.end_ring = 1;
}

/**
 * lombo_gmac_desc_tx_prepare - prepare tx desc before transmit.
 * @desc: pointer to idma desc.
 * @is_fs: 0 means the first segment; !0 means not the first segment.
 * @len: length of data to transmit.
 * @csum_flag: checksum flag.
 */
static void lombo_gmac_desc_tx_prepare(struct idma_desc *desc, int is_fs,
				       int len, int csum_flag)
{
	desc->des1.tx.first_segment = is_fs;

	if (unlikely(len >= SZ_2K)) {
		desc->des1.tx.buffer1_size = SZ_2K - 1;
		desc->des1.tx.buffer2_size =
			len - desc->des1.tx.buffer1_size;
	} else {
		desc->des1.tx.buffer1_size = len;
		desc->des1.tx.buffer2_size = 0;
	}

	/**
	 * 0: disable checksum insertion.
	 * 1: only ip header.
	 * 2: ip header but pseudoheader is not calculated.
	 * 3: ip header and pseudoheader.
	 */
	if (likely(csum_flag))
		desc->des1.tx.checksum_insertion = 3;
}

/**
 * lombo_gmac_desc_tx_close - set tx desc as the last segment for this transmit.
 * @desc: pointer to idma desc.
 */
static inline void lombo_gmac_desc_tx_close(struct idma_desc *desc)
{
	desc->des1.tx.last_segment = 1;
	desc->des1.tx.interrupt = 1;
}

/**
 * lombo_gmac_desc_tx_release - clear tx desc after transmit.
 * @desc: pointer to idma desc.
 */
static inline void lombo_gmac_desc_tx_release(struct idma_desc *desc)
{
	u32 end_ring = desc->des1.tx.end_ring;

	memset(desc, 0, offsetof(struct idma_desc, des2));
	desc->des1.tx.end_ring = end_ring;
}

/**
 * lombo_gmac_desc_tx_status - parse tx status from tx desc after transmit.
 * @desc: pointer to idma desc.
 * @lgmac: pointer to lombo gmac struct.
 * @stats: pointer to net device status struct.
 *
 * return 0 if not error occurred; !0 if any error occurred.
 */
static int lombo_gmac_desc_tx_status(struct idma_desc *desc,
				     struct lombo_gmac *lgmac,
				     struct net_device_stats *stats)
{
	if (unlikely(desc->des0.tx.error_summary)) {
		if (unlikely(desc->des0.tx.underflow_error)) {
			PRT_WARN("%s: tx underflow\n", lgmac->net_dev->name);
			stats->tx_fifo_errors++;
		}

		if (unlikely(desc->des0.tx.no_carrier)) {
			PRT_WARN("%s: tx no carrier\n", lgmac->net_dev->name);
			stats->tx_carrier_errors++;
		}

		if (unlikely(desc->des0.tx.loss_carrier)) {
			PRT_WARN("%s: tx loss carrier\n", lgmac->net_dev->name);
			stats->tx_carrier_errors++;
		}

		if (unlikely(desc->des0.tx.excessive_deferral ||
			     desc->des0.tx.excessive_collisions ||
			     desc->des0.tx.late_collision)) {
			PRT_WARN("%s: tx collision\n", lgmac->net_dev->name);
			stats->collisions += desc->des0.tx.collision_count;
		}

		return -EIO;
	}

	return 0;
}

/**
 * lombo_gmac_desc_tx_avail - get available tx desc number.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return number of available tx desc.
 */
static inline u32 lombo_gmac_desc_tx_avail(struct lombo_gmac *lgmac)
{
	return lgmac->tx_dirty + lgmac->tx_count - lgmac->tx_clean - 1;
}

/**
 * lombo_gmac_desc_rx_set_owner - set owner bit of rx desc, so DMA own the desc.
 * @desc: pointer to idma desc.
 */
static inline void lombo_gmac_desc_rx_set_owner(struct idma_desc *desc)
{
	desc->des0.rx.own = 1;
}

/**
 * lombo_gmac_desc_rx_get_owner - get owner of rx desc.
 * @desc: pointer to idma desc.
 *
 * return 0 if Controller own the desc; 1 if DMA own the desc.
 */
static inline u32 lombo_gmac_desc_rx_get_owner(struct idma_desc *desc)
{
	return desc->des0.rx.own;
}

/**
 * lombo_gmac_desc_rx_frame_len - get frame length from rx desc after receive.
 * @desc: pointer to idma desc.
 *
 * return frame length of rx desc.
 */
static inline u32 lombo_gmac_desc_rx_frame_len(struct idma_desc *desc)
{
	return desc->des0.rx.frame_length;
}

/**
 * lombo_gmac_desc_rx_init - init one rx desc.
 * @desc: pointer to idma desc.
 * @end: 0 means not the last one in the desc ring; !0 means the last one.
 */
static void lombo_gmac_desc_rx_init(struct idma_desc *desc, int end)
{
	desc->des0.rx.own = 1;
	desc->des1.rx.buffer1_size = SZ_2K - 1;
	desc->des1.rx.buffer2_size = SZ_2K - 1;
	desc->des1.rx.disable_ic = 1;
	if (end)
		desc->des1.rx.end_ring = 1;
}

/**
 * lombo_gmac_desc_rx_status - parse rx status from rx desc after receive.
 * @desc: pointer to idma desc.
 * @lgmac: pointer to lombo gmac struct.
 * @stats: pointer to net device status struct.
 *
 * return 0 if not error occurred; !0 if any error occurred.
 */
static int lombo_gmac_desc_rx_status(struct idma_desc *desc,
				     struct lombo_gmac *lgmac,
				     struct net_device_stats *stats)
{
	int ret = 0;

	if (unlikely(!desc->des0.rx.last_descriptor)) {
		PRT_WARN("%s: oversized frame spanned multiple buffers\n",
			 lgmac->net_dev->name);
		stats->rx_length_errors++;
		return LGMAC_RX_DISCARD_FRAME;
	}

	if (unlikely(desc->des0.rx.error_summary)) {
		if (unlikely(desc->des0.rx.collision)) {
			PRT_WARN("%s: rx collision\n", lgmac->net_dev->name);
			stats->collisions++;
		}

		if (unlikely(desc->des0.rx.crc_error)) {
			PRT_WARN("%s: rx crc error\n", lgmac->net_dev->name);
			stats->rx_crc_errors++;
		}

		ret = LGMAC_RX_DISCARD_FRAME;
	}

	if (unlikely(desc->des0.rx.length_error)) {
		PRT_WARN("%s: rx length error\n", lgmac->net_dev->name);
		ret = LGMAC_RX_DISCARD_FRAME;
	}

	if (unlikely(desc->des0.rx.mii_error)) {
		PRT_WARN("%s: rx mii error\n", lgmac->net_dev->name);
		ret = LGMAC_RX_DISCARD_FRAME;
	}

	return ret;
}

/**
 * lombo_gmac_desc_all_init - init all, including tx and rx, idma desc.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_desc_all_init(struct lombo_gmac *lgmac)
{
	u32 i;

	for (i = 0; i < lgmac->rx_count; i++)
		lombo_gmac_desc_rx_init(&lgmac->rx_desc[i],
					(i == (lgmac->rx_count - 1)));

	for (i = 0; i < lgmac->tx_count; i++)
		lombo_gmac_desc_tx_init(&lgmac->tx_desc[i],
					(i == (lgmac->tx_count - 1)));
}

/**
 * lombo_gmac_desc_show - dump idma desc.
 * @head: pointer to the first idma desc.
 * @size: number of idma desc to dump.
 */
static void lombo_gmac_desc_show(struct idma_desc *head, u32 size)
{
	struct idma_desc *desc = head;
	u32 i;

	for (i = 0; i < size; i++) {
		PRT_DBG("%04u [0x%08x]: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i, (u32)virt_to_phys(desc),
			desc->des0.val, desc->des1.val,
			desc->des2, desc->des3);
		desc++;
	}
}

/**
 * lombo_gmac_desc_all_show - dump all, including tx and rx, idma desc.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_desc_all_show(struct lombo_gmac *lgmac)
{
	PRT_DBG("%s: rx desc list\n", lgmac->net_dev->name);
	lombo_gmac_desc_show(lgmac->rx_desc, lgmac->rx_count);

	PRT_DBG("%s: tx desc list\n", lgmac->net_dev->name);
	lombo_gmac_desc_show(lgmac->tx_desc, lgmac->tx_count);
}

/**
 * lombo_gmac_dma_init_resource - init dma resource.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_dma_init_resource(struct lombo_gmac *lgmac)
{
	u32 rx_cnt = lgmac->rx_count;
	u32 tx_cnt = lgmac->tx_count;

	PRT_DBG("%s: init dma resource\n", lgmac->net_dev->name);

	lgmac->rx_desc = dma_zalloc_coherent(lgmac->dev,
					     rx_cnt * sizeof(struct idma_desc),
					     &lgmac->rx_desc_phy,
					     GFP_KERNEL);
	if (!lgmac->rx_desc) {
		PRT_ERR("%s: failed to alloc rx desc table\n",
			lgmac->net_dev->name);
		return -ENOMEM;
	}

	lgmac->tx_desc = dma_zalloc_coherent(lgmac->dev,
					     tx_cnt * sizeof(struct idma_desc),
					     &lgmac->tx_desc_phy,
					     GFP_KERNEL);
	if (!lgmac->tx_desc) {
		PRT_ERR("%s: failed to alloc tx desc table\n",
			lgmac->net_dev->name);
		goto exit_free_rx_desc;
	}

	lgmac->rx_skb_dma = kmalloc_array(rx_cnt, sizeof(dma_addr_t),
					  GFP_KERNEL);
	if (!lgmac->rx_skb_dma) {
		PRT_ERR("%s: failed to alloc rx dma addr array\n",
			lgmac->net_dev->name);
		goto exit_free_tx_desc;
	}

	lgmac->tx_skb_dma = kmalloc_array(tx_cnt, sizeof(dma_addr_t),
					  GFP_KERNEL);
	if (!lgmac->tx_skb_dma) {
		PRT_ERR("%s: failed to alloc tx dma addr array\n",
			lgmac->net_dev->name);
		goto exit_free_rx_skb_dma;
	}

	lgmac->rx_skb = kmalloc_array(rx_cnt, sizeof(struct sk_buff *),
				      GFP_KERNEL);
	if (!lgmac->rx_skb) {
		PRT_ERR("%s: failed to alloc rx skb pointer array\n",
			lgmac->net_dev->name);
		goto exit_free_tx_skb_dma;
	}

	lgmac->tx_skb = kmalloc_array(tx_cnt, sizeof(struct sk_buff *),
				      GFP_KERNEL);
	if (!lgmac->tx_skb) {
		PRT_ERR("%s: failed to alloc tx skb pointer array\n",
			lgmac->net_dev->name);
		goto exit_free_rx_skb;
	}

	memset(lgmac->rx_skb_dma, 0, rx_cnt * sizeof(dma_addr_t));
	memset(lgmac->tx_skb_dma, 0, tx_cnt * sizeof(dma_addr_t));

	memset(lgmac->rx_skb, 0, rx_cnt * sizeof(struct sk_buff *));
	memset(lgmac->tx_skb, 0, tx_cnt * sizeof(struct sk_buff *));

	lombo_gmac_desc_all_init(lgmac);
	lombo_gmac_desc_all_show(lgmac);

	lgmac->rx_clean = 0;
	lgmac->rx_dirty = (u32)-rx_cnt;
	lombo_gamc_rx_refill(lgmac);

	lgmac->tx_dirty = 0;
	lgmac->tx_clean = 0;

	return 0;

exit_free_rx_skb:
	kfree(lgmac->rx_skb);

exit_free_tx_skb_dma:
	kfree(lgmac->tx_skb_dma);

exit_free_rx_skb_dma:
	kfree(lgmac->rx_skb_dma);

exit_free_tx_desc:
	dma_free_coherent(lgmac->dev, tx_cnt * sizeof(struct idma_desc),
			  lgmac->tx_desc, lgmac->tx_desc_phy);

exit_free_rx_desc:
	dma_free_coherent(lgmac->dev, rx_cnt * sizeof(struct idma_desc),
			  lgmac->rx_desc, lgmac->rx_desc_phy);

	return -ENOMEM;
}

/**
 * lombo_gmac_dma_free_rx_skb - free all rx socket buffer.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_dma_free_rx_skb(struct lombo_gmac *lgmac)
{
	u32 i;

	for (i = 0; i < lgmac->rx_count; i++) {
		if (lgmac->rx_skb[i]) {
			dma_unmap_single(lgmac->dev, lgmac->rx_skb_dma[i],
					 lgmac->dma_buf_sz, DMA_FROM_DEVICE);
			dev_kfree_skb_any(lgmac->rx_skb[i]);
			lgmac->rx_skb[i] = NULL;
			lgmac->rx_skb_dma[i] = 0;
		}
	}
}

/**
 * lombo_gmac_dma_free_tx_skb - free all tx socket buffer.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_dma_free_tx_skb(struct lombo_gmac *lgmac)
{
	u32 i;

	for (i = 0; i < lgmac->tx_count; i++) {
		if (lgmac->tx_skb[i]) {
			struct idma_desc *desc = lgmac->tx_desc + i;

			if (lgmac->tx_skb_dma[i])
				dma_unmap_single(lgmac->dev,
					lgmac->tx_skb_dma[i],
					lombo_gmac_desc_tx_buf_len(desc),
					DMA_TO_DEVICE);
			dev_kfree_skb_any(lgmac->tx_skb[i]);
			lgmac->tx_skb[i] = NULL;
			lgmac->tx_skb_dma[i] = 0;
		}
	}
}

/**
 * lombo_gmac_dma_free_resource - free dma resource.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_dma_free_resource(struct lombo_gmac *lgmac)
{
	PRT_DBG("%s: free dma resource\n", lgmac->net_dev->name);

	lombo_gmac_dma_free_tx_skb(lgmac);
	lombo_gmac_dma_free_rx_skb(lgmac);

	dma_free_coherent(lgmac->dev,
			  lgmac->tx_count * sizeof(struct idma_desc),
			  lgmac->tx_desc, lgmac->tx_desc_phy);
	dma_free_coherent(lgmac->dev,
			  lgmac->rx_count * sizeof(struct idma_desc),
			  lgmac->rx_desc, lgmac->rx_desc_phy);

	kfree(lgmac->tx_skb);
	kfree(lgmac->rx_skb);
	kfree(lgmac->tx_skb_dma);
	kfree(lgmac->rx_skb_dma);
}

/**
 * lombo_gmac_dma_irq_enable - enable dma interrupt.
 * @lgmac: pointer to lombo gmac struct.
 */
static inline void lombo_gmac_dma_irq_enable(struct lombo_gmac *lgmac)
{
	csp_gmac_set_dma_int_enable(lgmac->base_addr, LGMAC_DEF_DMA_INT_MASK);
}

/**
 * lombo_gmac_dma_irq_disable - disable dma interrupt.
 * @lgmac: pointer to lombo gmac struct.
 */
static inline void lombo_gmac_dma_irq_disable(struct lombo_gmac *lgmac)
{
	csp_gmac_set_dma_int_enable(lgmac->base_addr, 0);
}

/**
 * lombo_gmac_dma_set_operation_mode - set hardware dma operation mode.
 * @lgmac: pointer to lombo gmac struct.
 *
 * To set tx and rx Store-and-Forward mode, or to set dma threshold.
 */
static void lombo_gmac_dma_set_operation_mode(struct lombo_gmac *lgmac)
{
	/* enable tx and rx Store-and-Forward default */
	csp_gmac_set_rx_threshold(lgmac->base_addr, 1, 0);
	csp_gmac_set_tx_threshold(lgmac->base_addr, 1, 0);
}

/**
 * lombo_gmac_dma_soft_reset - dma software reset.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_dma_soft_reset(struct lombo_gmac *lgmac)
{
	int limit = 100;

	csp_gmac_dma_soft_reset(lgmac->base_addr);

	while (limit--) {
		if (!csp_gmac_get_dma_reset_status(lgmac->base_addr))
			break;
		mdelay(1);
	}

	if (limit < 0) {
		PRT_ERR("%s: dma software reset timeout\n",
			lgmac->net_dev->name);
		return -EBUSY;
	}

	return 0;
}

/**
 * lombo_gmac_dma_init_engine - init dma engine.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_dma_init_engine(struct lombo_gmac *lgmac)
{
	int ret = 0;

	PRT_DBG("%s: init dma engine\n", lgmac->net_dev->name);

	ret = lombo_gmac_dma_soft_reset(lgmac);
	if (ret) {
		PRT_ERR("%s: software reset dma failed (%d)\n",
			lgmac->net_dev->name, ret);
		return ret;
	}

	csp_gmac_set_rx_desc_addr(lgmac->base_addr, lgmac->rx_desc_phy);
	csp_gmac_set_tx_desc_addr(lgmac->base_addr, lgmac->tx_desc_phy);

	csp_gmac_set_dma_burst_len(lgmac->base_addr, LGMAC_DEF_DMA_PBL, 0);
	csp_gmac_set_axi_bus_mode(lgmac->base_addr, 0);

	lombo_gmac_dma_irq_enable(lgmac);

	return 0;
}

/**
 * lombo_gmac_dma_irq - DMA ISR.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_dma_irq(struct lombo_gmac *lgmac)
{
	u32 status;

	status = csp_gmac_get_dma_status(lgmac->base_addr);
	csp_gmac_clr_dma_status(lgmac->base_addr, status);

	PRT_DBG("%s: get dma interrupt status 0x%08x\n",
		lgmac->net_dev->name, status);

	/* Normal DMA Interrupt */
	if (likely(status & (LGMAC_DMA_STA_RI | LGMAC_DMA_STA_TI))) {
		if (likely(napi_schedule_prep(&lgmac->napi))) {
			lombo_gmac_dma_irq_disable(lgmac);
			__napi_schedule(&lgmac->napi);
		}
	}

	/* Abnormal DMA Interrupt */
	if (unlikely(status & (LGMAC_DMA_STA_TPS | LGMAC_DMA_STA_FBI))) {
		PRT_WARN("%s: get abnormal dma interrupt 0x%08x\n",
			 lgmac->net_dev->name, status);
		lombo_gmac_tx_error(lgmac);
	}
}

/**
 * lombo_gmac_tx_timer - timer function for tx coalesce.
 * @data: pointer to lombo gmac struct.
 */
static void lombo_gmac_tx_coal_timer(unsigned long data)
{
	struct lombo_gmac *lgmac = (struct lombo_gmac *)data;

	lombo_gmac_tx_clean(lgmac);
}

/**
 * lombo_gmac_tx_coal_init - init tx coalesce.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_tx_coal_init(struct lombo_gmac *lgmac)
{
	lgmac->tx_coal_frames = LGMAC_TX_COAL_FRAME;
	lgmac->tx_coal_time = LGMAC_TX_COAL_TIMER_US;
	init_timer(&lgmac->tx_coal_timer);
	lgmac->tx_coal_timer.expires =
		jiffies + usecs_to_jiffies(lgmac->tx_coal_time);
	lgmac->tx_coal_timer.data = (unsigned long)lgmac;
	lgmac->tx_coal_timer.function = lombo_gmac_tx_coal_timer;
	add_timer(&lgmac->tx_coal_timer);
}

/**
 * lombo_gmac_eee_enable - check and enter lpi mode for eee.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_eee_enable(struct lombo_gmac *lgmac)
{
	if (lgmac->tx_dirty == lgmac->tx_clean && !lgmac->tx_lpi_enter)
		csp_gmac_set_lpi_mode(lgmac->base_addr, 1);
}

/**
 * lombo_gmac_eee_disable - leave lpi state, exit from eee.
 * @lgmac: pointer to lombo gmac struct.
 *
 * This function should be called before transmit.
 */
static void lombo_gmac_eee_disable(struct lombo_gmac *lgmac)
{
	csp_gmac_set_lpi_mode(lgmac->base_addr, 0);
	del_timer_sync(&lgmac->eee_ctrl_timer);
	lgmac->tx_lpi_enter = false;
}

/**
 * lombo_gmac_eee_ctrl_timer - timer function for eee control.
 * @data: pointer to lombo gmac struct.
 *
 * If there is no data to transfer and not in lpi state,
 * then the MAC Transmitter can be moved to lpi state.
 */
static void lombo_gmac_eee_ctrl_timer(unsigned long data)
{
	struct lombo_gmac *lgmac = (struct lombo_gmac *)data;

	lombo_gmac_eee_enable(lgmac);
	mod_timer(&lgmac->eee_ctrl_timer,
		  jiffies + msecs_to_jiffies(LGMAC_LPI_TIMER_MS));
}

/**
 * lombo_gmac_eee_init - init eee.
 * @data: pointer to lombo gmac struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_eee_init(struct lombo_gmac *lgmac)
{
	int ret = 0;

	ret = phy_init_eee(lgmac->phy_dev, 1);
	if (ret) {
		PRT_DBG("%s: phy device to init eee failed (%d)\n",
			lgmac->net_dev->name, ret);
		return ret;
	}

	if (!lgmac->eee_active) {
		lgmac->eee_active = 1;
		init_timer(&lgmac->eee_ctrl_timer);
		lgmac->eee_ctrl_timer.function = lombo_gmac_eee_ctrl_timer;
		lgmac->eee_ctrl_timer.data = (unsigned long)lgmac;
		lgmac->eee_ctrl_timer.expires =
			jiffies + msecs_to_jiffies(LGMAC_LPI_TIMER_MS);
		add_timer(&lgmac->eee_ctrl_timer);
		csp_gmac_set_lpi_timer(lgmac->base_addr,
				       LGMAC_DEF_LPI_LS_TIMER,
				       lgmac->tx_lpi_time);
	}
	csp_gmac_set_lpi_pls(lgmac->base_addr, lgmac->phy_dev->link);

	PRT_DBG("%s: eee init success\n", lgmac->net_dev->name);

	return 0;
}

/**
 * lombo_gmac_mdio_wait_busy - mdio wait busy.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return 0 if success; !0 if timeout.
 */
static int lombo_gmac_mdio_wait_busy(struct lombo_gmac *lgmac)
{
	unsigned long deadline = jiffies + 3 * HZ;

	do {
		if (csp_gmac_check_mdio_busy(lgmac->base_addr))
			cpu_relax();
		else
			return 0;
	} while (!time_after_eq(jiffies, deadline));

	return -EBUSY;
}

/**
 * lombo_gmac_mdio_read - mdio read.
 * @bus: pointer to mii bus struct.
 * @phy_id: phy device address.
 * @reg_num: phy register address.
 *
 * return >=0 the data; <0 error number.
 */
static int lombo_gmac_mdio_read(struct mii_bus *bus, int phy_id, int reg_num)
{
	struct net_device *ndev = bus->priv;
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	int data;

	if (lombo_gmac_mdio_wait_busy(lgmac))
		return -EBUSY;

	csp_gmac_config_mdio_read(lgmac->base_addr, phy_id, reg_num);

	if (lombo_gmac_mdio_wait_busy(lgmac))
		return -EBUSY;

	data = (int)csp_gmac_mdio_read_data(lgmac->base_addr);

	return data;
}

/**
 * lombo_gmac_mdio_write - mdio write.
 * @bus: pointer to mii bus struct.
 * @phy_id: phy device address.
 * @reg_num: phy register address.
 * @data: data to write.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_mdio_write(struct mii_bus *bus, int phy_id,
				 int reg_num, u16 data)
{
	struct net_device *ndev = bus->priv;
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	if (lombo_gmac_mdio_wait_busy(lgmac))
		return -EBUSY;

	csp_gmac_mdio_write_data(lgmac->base_addr, data);
	csp_gmac_config_mdio_write(lgmac->base_addr, phy_id, reg_num);

	return lombo_gmac_mdio_wait_busy(lgmac);
}

/**
 * lombo_gmac_mdio_reset - mdio reset.
 * @bus: pointer to mii bus struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_mdio_reset(struct mii_bus *bus)
{
	struct net_device *ndev = bus->priv;
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	csp_gmac_mdio_reset(lgmac->base_addr);
	csp_gmac_set_csr_clock_range(lgmac->base_addr,
				     clk_get_rate(lgmac->clk_core));

	return 0;
}

/**
 * lombo_gmac_mdio_register - register mdio bus.
 * @ndev: pointer to net device struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_mdio_register(struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	struct mii_bus *mbus;
	struct phy_device *phy_dev;
	int addr, found, ret = 0;

	mbus = mdiobus_alloc();
	if (!mbus) {
		PRT_ERR("%s: failed to alloc mdio bus\n", lgmac->net_dev->name);
		return -ENOMEM;
	}

	mbus->name = LGMAC_MII_BUS_NAME;
	mbus->read = &lombo_gmac_mdio_read;
	mbus->write = &lombo_gmac_mdio_write;
	mbus->reset = &lombo_gmac_mdio_reset;
	snprintf(mbus->id, MII_BUS_ID_SIZE, "%s-%x", mbus->name, 0);
	mbus->priv = ndev;
	mbus->irq = NULL;
	mbus->phy_mask = 0;
	mbus->parent = lgmac->dev;

	ret = mdiobus_register(mbus);
	if (ret) {
		PRT_ERR("%s: failed to register mdio bus (%d)\n",
			lgmac->net_dev->name, ret);
		goto exit_free_mdiobus;
	}

	found = 0;
	for (addr = 0; addr < PHY_MAX_ADDR; addr++) {
		phy_dev = mbus->phy_map[addr];
		if (phy_dev) {
			int active = 0;

			/**
			 * If we're going to bind the MAC to this PHY bus,
			 * and no PHY address number was provided to the MAC,
			 * use the one probed here.
			 */
			if (lgmac->phy_addr < 0)
				lgmac->phy_addr = addr;

			active = (lgmac->phy_addr == addr);

			PRT_DBG("%s: phy id:0x%08x addr:%d name:%s status:%s\n",
				lgmac->net_dev->name,
				phy_dev->phy_id, addr,
				dev_name(&phy_dev->dev),
				active ? "active" : "invalid");

			if ((phy_dev->drv) && phy_dev->drv->name) {
				PRT_DBG("%s: phy driver is [ %s ]\n",
					lgmac->net_dev->name,
					phy_dev->drv->name);
			}

			found = 1;
		}
	}

	if (!found) {
		PRT_WARN("%s: failed to found any phy device\n",
			 lgmac->net_dev->name);
		ret = -ENODEV;
		goto exit_unregister_mdiobus;
	}

	lgmac->mii_bus = mbus;

	PRT_DBG("%s: mdio bus registered\n", lgmac->net_dev->name);

	return 0;

exit_unregister_mdiobus:
	mdiobus_unregister(mbus);

exit_free_mdiobus:
	mdiobus_free(mbus);

	return ret;
}

/**
 * lombo_gmac_mdio_unregister - unregister mdio bus.
 * @ndev: pointer to net device struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_mdio_unregister(struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	PRT_DBG("%s: mdio bus unregistered\n", lgmac->net_dev->name);

	if (!lgmac->mii_bus)
		return 0;

	mdiobus_unregister(lgmac->mii_bus);
	lgmac->mii_bus->priv = NULL;
	mdiobus_free(lgmac->mii_bus);
	lgmac->mii_bus = NULL;

	return 0;
}

/**
 * lombo_gmac_set_speed - set speed in MII/RMII/RGMII interface.
 * @lgmac: pointer to lombo gmac struct.
 * @speed: speed to set, should be 10/100/1000.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_set_speed(struct lombo_gmac *lgmac, int speed)
{
	u32 tx_delay, rx_delay;

	if (speed == 10) {
		tx_delay = lgmac->tx_delay[0];
		rx_delay = lgmac->rx_delay[0];
	} else if (speed == 100) {
		tx_delay = lgmac->tx_delay[1];
		rx_delay = lgmac->rx_delay[1];
	} else if (speed == 1000) {
		tx_delay = lgmac->tx_delay[2];
		rx_delay = lgmac->rx_delay[2];
	} else {
		PRT_WARN("%s: unsupported speed %d\n",
			 lgmac->net_dev->name,
			 speed);
		return -EINVAL;
	}

	csp_gmac_set_speed(lgmac->base_addr, speed);

	/* set tx driver delay and rx sample delay */
	csp_gmac_set_tx_drv_delay(lgmac->base_addr, tx_delay);
	csp_gmac_set_rx_smp_delay(lgmac->base_addr, rx_delay);

	return 0;
}

/**
 * lombo_gmac_flow_control - set flow control parameter.
 * @lgmac: pointer to lombo gmac struct.
 */
static inline void lombo_gmac_flow_control(struct lombo_gmac *lgmac)
{
	csp_gmac_flow_control(lgmac->base_addr, lgmac->old_duplex,
			      lgmac->flow_ctrl & LGMAC_FLOW_CTRL_RX,
			      lgmac->flow_ctrl & LGMAC_FLOW_CTRL_TX,
			      LGMAC_FLOW_CTRL_PAUSE);
}

/**
 * lombo_gmac_adjust_link - to fix the link status.
 * @ndev: pointer to net device struct.
 */
static void lombo_gmac_adjust_link(struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	struct phy_device *phy_dev = lgmac->phy_dev;
	unsigned long flags;
	u32 new_status = 0;

	if (!phy_dev) {
		PRT_WARN("%s: not any phy device exist\n",
			 lgmac->net_dev->name);
		return;
	}

	PRT_DBG("%s: PHY(addr:%d link:%d speed:%d duplex:%d pause:%d)\n",
		lgmac->net_dev->name,
		lgmac->phy_addr, phy_dev->link,
		phy_dev->speed, phy_dev->duplex,
		phy_dev->pause);

	spin_lock_irqsave(&lgmac->lock, flags);

	if (phy_dev->link) {
		if (phy_dev->speed != lgmac->old_speed) {
			lgmac->old_speed = phy_dev->speed;
			lombo_gmac_set_speed(lgmac, phy_dev->speed);
			new_status = 1;
		}

		if (phy_dev->duplex != lgmac->old_duplex) {
			lgmac->old_duplex = phy_dev->duplex;
			csp_gmac_set_duplex_mode(lgmac->base_addr,
						 phy_dev->duplex);
			new_status = 1;
		}

		if (phy_dev->pause)
			lombo_gmac_flow_control(lgmac);

		if (!lgmac->old_link) {
			lgmac->old_link = 1;
			new_status = 1;
		}
	} else if (lgmac->old_link) {
		lgmac->old_link = 0;
		lgmac->old_speed = 0;
		lgmac->old_duplex = -1;
		new_status = 1;
	}

	if (new_status)
		phy_print_status(phy_dev);

	lgmac->eee_enable = !lombo_gmac_eee_init(lgmac);

	spin_unlock_irqrestore(&lgmac->lock, flags);
}

/**
 * lombo_gmac_phy_init - init phy device.
 * @ndev: pointer to net device struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_phy_init(struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	struct phy_device *phy_dev;
	int interface = lgmac->phy_interface;
	char bus_id[MII_BUS_ID_SIZE];
	char phy_id[MII_BUS_ID_SIZE + 3];

	PRT_DBG("%s: init phy device\n", lgmac->net_dev->name);

	lgmac->old_link = 0;
	lgmac->old_speed = 0;
	lgmac->old_duplex = -1;

	snprintf(bus_id, MII_BUS_ID_SIZE, "%s-%x", LGMAC_MII_BUS_NAME, 0);
	snprintf(phy_id, MII_BUS_ID_SIZE + 3, PHY_ID_FMT, bus_id,
		 lgmac->phy_addr);

	phy_dev = phy_connect(ndev, phy_id, &lombo_gmac_adjust_link, interface);
	if (IS_ERR(phy_dev)) {
		PRT_ERR("%s: failed to connect phy device (%d)\n",
			lgmac->net_dev->name, (int)PTR_ERR(phy_dev));
		return PTR_ERR(phy_dev);
	}

	/* stop advertising 1000base capability if interface is not (R)GMII */
	if (interface != PHY_INTERFACE_MODE_RGMII)
		phy_dev->advertising &=
			~(SUPPORTED_1000baseT_Half | SUPPORTED_1000baseT_Full);

	/**
	 * Broken hardware is sometimes missing the pull-up resistor on the
	 * MDIO line, which results in reads to non-existent devices returning
	 * 0 rather than 0xffff. Catch this here and treat 0 as a non-existent
	 * device as well.
	 */
	if (!phy_dev->phy_id) {
		PRT_ERR("%s: broken phy device %s\n",
			lgmac->net_dev->name,
			phy_id);
		phy_disconnect(phy_dev);
		return -ENODEV;
	}

	lgmac->phy_dev = phy_dev;

	return 0;
}

/**
 * lombo_gmac_phy_reset - phy device hardware reset or power down.
 * @lgmac: pointer to lombo gmac struct.
 * @reset: 0 means power down; !0 means reset and power on.
 */
static void lombo_gmac_phy_reset(struct lombo_gmac *lgmac, u32 reset)
{
	if (reset) {
		/* reset and power on the phy device */
		if (gpio_is_valid(lgmac->phy_rst_pin)) {
			gpio_direction_output(lgmac->phy_rst_pin,
					      lgmac->phy_rst_level);
			mdelay(5);
			gpio_direction_output(lgmac->phy_rst_pin,
					      !lgmac->phy_rst_level);
		}
		mdelay(30);
	} else {
		/* power down the phy device */
		if (gpio_is_valid(lgmac->phy_rst_pin))
			gpio_direction_output(lgmac->phy_rst_pin,
					      lgmac->phy_rst_level);
	}
}

/**
 * lombo_gmac_phy_set_interface - set phy interface type.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_phy_set_interface(struct lombo_gmac *lgmac)
{
	if (lgmac->phy_interface == PHY_INTERFACE_MODE_MII) {
		PRT_DBG("%s: phy interface type is MII\n",
			lgmac->net_dev->name);
		csp_gmac_set_phy_interface(lgmac->base_addr,
					   LGMAC_PHY_TYPE_MII_GMII);
	} else if (lgmac->phy_interface == PHY_INTERFACE_MODE_RMII) {
		PRT_DBG("%s: phy interface type is RMII\n",
			lgmac->net_dev->name);
		csp_gmac_set_phy_interface(lgmac->base_addr,
					   LGMAC_PHY_TYPE_RMII);
		if (lgmac->rmii_osc_ext) {
			csp_gmac_set_rmii_tx_clk_src(lgmac->base_addr, 1);
			csp_gmac_set_rmii_tx_clk_invert(lgmac->base_addr, 0);
		} else {
			csp_gmac_set_rmii_tx_clk_src(lgmac->base_addr, 0);
			csp_gmac_set_rmii_tx_clk_invert(lgmac->base_addr, 1);
		}
	} else if (lgmac->phy_interface == PHY_INTERFACE_MODE_RGMII) {
		PRT_DBG("%s: phy interface type is RGMII\n",
			lgmac->net_dev->name);
		csp_gmac_set_phy_interface(lgmac->base_addr,
					   LGMAC_PHY_TYPE_RGMII);
	} else {
		PRT_ERR("%s: unsupported phy interface type %d\n",
			lgmac->net_dev->name,
			lgmac->phy_interface);
		return -EPFNOSUPPORT;
	}

	return 0;
}

/**
 * lombo_gmac_check_ether_addr - verify the hardware address.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_check_ether_addr(struct lombo_gmac *lgmac)
{
	if (!is_valid_ether_addr(lgmac->net_dev->dev_addr)) {
		csp_gmac_get_mac_addr(lgmac->base_addr,
				      lgmac->net_dev->dev_addr);

		if (!is_valid_ether_addr(lgmac->net_dev->dev_addr))
			eth_hw_addr_random(lgmac->net_dev);
	}

	PRT_DBG("%s: hardware address %02x:%02x:%02x:%02x:%02x:%02x\n",
		lgmac->net_dev->name,
		lgmac->net_dev->dev_addr[0], lgmac->net_dev->dev_addr[1],
		lgmac->net_dev->dev_addr[2], lgmac->net_dev->dev_addr[3],
		lgmac->net_dev->dev_addr[4], lgmac->net_dev->dev_addr[5]);
}

/**
 * lombo_gmac_clock_get - get module clock.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_clock_get(struct lombo_gmac *lgmac)
{
	int i;
	static const char * const clk_name[] = {
		"gmac_parent_clk", "gmac_core_clk", "gmac_sclk_0",
		"gmac_sclk_1", "gmac_ahb_gate", "gmac_mem_axi_gate",
		"gmac_ahb_reset"
	};
	struct clk **clk_handle[] = {
		&lgmac->clk_parent, &lgmac->clk_core, &lgmac->clk_sclk0,
		&lgmac->clk_sclk1, &lgmac->clk_gate, &lgmac->clk_axi,
		&lgmac->clk_reset
	};

	for (i = 0; i < ARRAY_SIZE(clk_handle); i++) {
		*clk_handle[i] = devm_clk_get(lgmac->dev, clk_name[i]);
		if (IS_ERR_OR_NULL(*clk_handle[i])) {
			PRT_ERR("%s: failed to get clock %s (%d)\n",
				dev_name(lgmac->dev),
				clk_name[i],
				(int)PTR_ERR(*clk_handle[i]));
			return PTR_ERR(*clk_handle[i]);
		}
	}

	return 0;
}

/**
 * lombo_gmac_clock_init - init module clock.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_clock_init(struct lombo_gmac *lgmac)
{
	int i, ret = 0;
	static const char * const clk_name[] = {
		"gmac_core_clk", "gmac_sclk_0", "gmac_sclk_1",
		"gmac_ahb_gate", "gmac_mem_axi_gate", "gmac_ahb_reset"
	};
	struct clk **clk_handle[] = {
		&lgmac->clk_core, &lgmac->clk_sclk0, &lgmac->clk_sclk1,
		&lgmac->clk_gate, &lgmac->clk_axi, &lgmac->clk_reset
	};

	ret = clk_set_parent(lgmac->clk_core, lgmac->clk_parent);
	if (ret) {
		PRT_ERR("%s: failed to set parent for core clock (%d)\n",
			lgmac->net_dev->name, ret);
		return ret;
	}

	ret = clk_set_parent(lgmac->clk_sclk0, lgmac->clk_parent);
	if (ret) {
		PRT_ERR("%s: failed to set parent for sclk0 clock (%d)\n",
			lgmac->net_dev->name, ret);
		return ret;
	}

	ret = clk_set_parent(lgmac->clk_sclk1, lgmac->clk_parent);
	if (ret) {
		PRT_ERR("%s: failed to set parent for sclk1 clock (%d)\n",
			lgmac->net_dev->name, ret);
		return ret;
	}

	ret = clk_set_rate(lgmac->clk_core,
			   lgmac->phy_interface == PHY_INTERFACE_MODE_RGMII ?
			   250000000 : 100000000);
	if (ret) {
		PRT_ERR("%s: failed to set rate for core clock (%d)\n",
			lgmac->net_dev->name, ret);
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(clk_handle); i++) {
		ret = clk_prepare_enable(*clk_handle[i]);
		if (ret) {
			PRT_ERR("%s: failed to enable %s (%d)\n",
				lgmac->net_dev->name,
				clk_name[i],
				ret);
			goto exit_disable_clock;
		}
	}

	return 0;

exit_disable_clock:
	for (i--; i >= 0; i--)
		clk_disable_unprepare(*clk_handle[i]);

	return ret;
}

/**
 * lombo_gmac_clock_deinit - deinit module clock.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_clock_deinit(struct lombo_gmac *lgmac)
{
	struct clk *clk_null = clk_get(NULL, "null_clk");

	clk_set_parent(lgmac->clk_core, clk_null);
	clk_set_parent(lgmac->clk_sclk0, clk_null);
	clk_set_parent(lgmac->clk_sclk1, clk_null);

	clk_disable_unprepare(lgmac->clk_core);
	clk_disable_unprepare(lgmac->clk_sclk0);
	clk_disable_unprepare(lgmac->clk_sclk1);

	clk_disable_unprepare(lgmac->clk_axi);
	clk_disable_unprepare(lgmac->clk_reset);
	clk_disable_unprepare(lgmac->clk_gate);

	clk_put(clk_null);
}

/**
 * lombo_gmac_core_init - init mac core.
 * @lgmac: pointer to lombo gmac struct.
 *
 * Set default configuration and enable default interrupt.
 */
static void lombo_gmac_core_init(struct lombo_gmac *lgmac)
{
	csp_gmac_set_base_config(lgmac->base_addr, LGMAC_DEF_CONFIG);

	csp_gmac_set_int_mask(lgmac->base_addr, LGMAC_DEF_INT_MASK);
}

/**
 * lombo_gmac_open - open entry point of the driver.
 * @ndev: pointer to net device struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_open(struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	int ret;

	PRT_DBG("%s: open...\n", lgmac->net_dev->name);

	netif_carrier_off(ndev);

	ret = lombo_gmac_clock_init(lgmac);
	if (ret) {
		PRT_ERR("%s: failed to init module clock (%d)\n",
			lgmac->net_dev->name, ret);
		return ret;
	}

	csp_gmac_enable_clk_gate(lgmac->base_addr, LGMAC_CLK_GATE_ALL);

	ret = lombo_gmac_phy_set_interface(lgmac);
	if (ret) {
		PRT_ERR("%s: failed to set phy interface (%d)\n",
			lgmac->net_dev->name, ret);
		goto exit_disable_clock_gate;
	}

	lombo_gmac_phy_reset(lgmac, 1);

	if (!lgmac->mii_bus) {
		ret = lombo_gmac_mdio_register(ndev);
		if (ret) {
			PRT_ERR("%s: failed to register mdio bus (%d)\n",
				lgmac->net_dev->name, ret);
			goto exit_reset_phy;
		}
	}

	ret = lombo_gmac_phy_init(ndev);
	if (ret) {
		PRT_ERR("%s: failed to init phy device (%d)\n",
			lgmac->net_dev->name, ret);
		goto exit_unregister_mdio;
	}

	lgmac->tx_count = L1_CACHE_ALIGN(LGMAC_TX_DESC_CNT);
	lgmac->rx_count = L1_CACHE_ALIGN(LGMAC_RX_DESC_CNT);
	lgmac->dma_buf_sz = L1_CACHE_ALIGN(LGMAC_DMA_BUF_SIZE);
	ret = lombo_gmac_dma_init_resource(lgmac);
	if (ret) {
		PRT_ERR("%s: failed to init dma resource (%d)\n",
			lgmac->net_dev->name, ret);
		goto exit_disconnect_phy;
	}

	ret = lombo_gmac_dma_init_engine(lgmac);
	if (ret) {
		PRT_ERR("%s: failed to init dma engine (%d)\n",
			lgmac->net_dev->name, ret);
		goto exit_free_dma_resource;
	}

	ret = request_irq(ndev->irq, lombo_gmac_irq, IRQF_SHARED,
			  ndev->name, ndev);
	if (unlikely(ret < 0)) {
		PRT_ERR("%s: failed to request irq (%d)\n",
			lgmac->net_dev->name, ret);
		goto exit_disable_dma_irq;
	}

	lombo_gmac_check_ether_addr(lgmac);
	csp_gmac_set_mac_addr(lgmac->base_addr, ndev->dev_addr);

	lombo_gmac_dma_set_operation_mode(lgmac);

	csp_gmac_dma_start_rx(lgmac->base_addr);
	csp_gmac_dma_start_tx(lgmac->base_addr);

	lombo_gmac_core_init(lgmac);
	csp_gmac_set_mac_enable(lgmac->base_addr, 1);

	lgmac->tx_lpi_time = LGMAC_DEF_LPI_TW_TIMER;
	lgmac->eee_enable = !lombo_gmac_eee_init(lgmac);

	lombo_gmac_tx_coal_init(lgmac);

	lgmac->rx_wdt_cnt = LGMAC_MAX_RX_WDT_CNT;
	csp_gmac_set_rx_watchdog(lgmac->base_addr, LGMAC_MAX_RX_WDT_CNT);

	if (lgmac->phy_dev)
		phy_start(lgmac->phy_dev);

	napi_enable(&lgmac->napi);

	netif_start_queue(ndev);

	PRT_INFO("%s: open success\n", lgmac->net_dev->name);

	return 0;

exit_disable_dma_irq:
	lombo_gmac_dma_irq_disable(lgmac);

exit_free_dma_resource:
	lombo_gmac_dma_free_resource(lgmac);

exit_disconnect_phy:
	if (lgmac->phy_dev) {
		phy_stop(lgmac->phy_dev);
		phy_disconnect(lgmac->phy_dev);
		lgmac->phy_dev = NULL;
	}

exit_unregister_mdio:
	if (lgmac->mii_bus)
		lombo_gmac_mdio_unregister(ndev);

exit_reset_phy:
	lombo_gmac_phy_reset(lgmac, 0);

exit_disable_clock_gate:
	csp_gmac_disable_clk_gate(lgmac->base_addr, LGMAC_CLK_GATE_ALL);

	lombo_gmac_clock_deinit(lgmac);

	return ret;
}

/**
 * lombo_gmac_stop - close entry point of the driver.
 * @ndev: pointer to net device struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_stop(struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	PRT_DBG("%s: close...\n", lgmac->net_dev->name);

	lombo_gmac_dma_irq_disable(lgmac);

	if (lgmac->eee_enable)
		del_timer_sync(&lgmac->eee_ctrl_timer);

	if (lgmac->phy_dev) {
		phy_stop(lgmac->phy_dev);
		phy_disconnect(lgmac->phy_dev);
		lgmac->phy_dev = NULL;
	}

	netif_stop_queue(ndev);

	napi_disable(&lgmac->napi);

	del_timer_sync(&lgmac->tx_coal_timer);

	free_irq(ndev->irq, ndev);

	csp_gmac_dma_stop_tx(lgmac->base_addr);
	csp_gmac_dma_stop_rx(lgmac->base_addr);

	csp_gmac_set_mac_enable(lgmac->base_addr, 0);
	lombo_gmac_dma_free_resource(lgmac);

	if (lgmac->mii_bus)
		lombo_gmac_mdio_unregister(ndev);

	lombo_gmac_phy_reset(lgmac, 0);

	csp_gmac_disable_clk_gate(lgmac->base_addr, LGMAC_CLK_GATE_ALL);

	lombo_gmac_clock_deinit(lgmac);

	netif_carrier_off(ndev);

	PRT_INFO("%s: close success\n", lgmac->net_dev->name);

	return 0;
}

/**
 * lombo_gmac_start_xmit - tx entry point of the driver.
 * @skb: pointer to the socket buffer.
 * @ndev: pointer to net device struct.
 *
 * return 0 if success; otherwise failed.
 */
static netdev_tx_t lombo_gmac_start_xmit(struct sk_buff *skb,
					 struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	struct idma_desc *first, *desc;
	const skb_frag_t *frag;
	int i, len, csum_insertion = 0;
	u32 nfrags = skb_shinfo(skb)->nr_frags;
	u32 tx_cnt = lgmac->tx_count;
	u32 entry, nopaged_len = skb_headlen(skb);

	PRT_DBG("%s: transmit start...\n", lgmac->net_dev->name);

	spin_lock(&lgmac->tx_lock);

	if (unlikely(lombo_gmac_desc_tx_avail(lgmac) < (nfrags + 1))) {
		if (!netif_queue_stopped(ndev)) {
			netif_stop_queue(ndev);
			PRT_ERR("%s: tx ring full when queue awake\n",
				lgmac->net_dev->name);
		}
		spin_unlock(&lgmac->tx_lock);
		return NETDEV_TX_BUSY;
	}

	if (lgmac->tx_lpi_enter)
		lombo_gmac_eee_disable(lgmac);

	csum_insertion = (skb->ip_summed == CHECKSUM_PARTIAL);

	entry = lgmac->tx_clean % tx_cnt;
	first = lgmac->tx_desc + entry;
	desc = first;
	lgmac->tx_skb[entry] = skb;

	desc->des2 = dma_map_single(lgmac->dev, skb->data, nopaged_len,
				    DMA_TO_DEVICE);
	lgmac->tx_skb_dma[entry] = desc->des2;
	lombo_gmac_desc_tx_prepare(desc, 1, nopaged_len, csum_insertion);

	for (i = 0; i < nfrags; i++) {
		frag = &skb_shinfo(skb)->frags[i];
		len = skb_frag_size(frag);

		entry = (++lgmac->tx_clean) % tx_cnt;
		desc = lgmac->tx_desc + entry;

		desc->des2 = skb_frag_dma_map(lgmac->dev, frag, 0, len,
					      DMA_TO_DEVICE);
		lgmac->tx_skb_dma[entry] = desc->des2;
		lgmac->tx_skb[entry] = NULL;
		lombo_gmac_desc_tx_prepare(desc, 0, len, csum_insertion);
		wmb();
		lombo_gmac_desc_tx_set_owner(desc);
		wmb();
	}

	/* finalize the latest segment */
	lombo_gmac_desc_tx_close(desc);
	wmb();

	/* According to the coalesce parameter the IC bit of the latest
	 * segment could be reset and the timer re-started to invoke the
	 * lombo tx function. This approach takes care about the fragments.
	 */
	lgmac->tx_coal_cnt += nfrags + 1;
	if (lgmac->tx_coal_frames > lgmac->tx_coal_cnt) {
		lombo_gmac_desc_tx_clear_ic(desc);
		mod_timer(&lgmac->tx_coal_timer,
			  jiffies + usecs_to_jiffies(lgmac->tx_coal_time));
	} else {
		lgmac->tx_coal_cnt = 0;
	}

	lombo_gmac_desc_tx_set_owner(first);
	wmb();

	lgmac->tx_clean++;
	if (unlikely(lombo_gmac_desc_tx_avail(lgmac) <= (MAX_SKB_FRAGS + 1)))
		netif_stop_queue(ndev);

	ndev->stats.tx_bytes += skb->len;

	skb_tx_timestamp(skb);

	csp_gmac_dma_tx_poll_demand(lgmac->base_addr);

	spin_unlock(&lgmac->tx_lock);

	PRT_DBG("%s: transmit success\n", lgmac->net_dev->name);

	return NETDEV_TX_OK;
}

/**
 * lombo_gmac_tx_clean - reclaim resource after transmit completed.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_tx_clean(struct lombo_gmac *lgmac)
{
	struct sk_buff *skb;
	struct idma_desc *desc;
	u32 last, entry, tx_error, tx_cnt = lgmac->tx_count;

	spin_lock(&lgmac->tx_lock);

	while (lgmac->tx_dirty != lgmac->tx_clean) {
		entry = lgmac->tx_dirty % tx_cnt;
		skb = lgmac->tx_skb[entry];
		desc = lgmac->tx_desc + entry;

		/* check if the desc is owned by DMA */
		if (lombo_gmac_desc_tx_get_owner(desc))
			break;

		/* check if any tx error by looking at the last segment */
		last = lombo_gmac_desc_tx_last_segment(desc);
		if (likely(last)) {
			tx_error = lombo_gmac_desc_tx_status(desc, lgmac,
					&lgmac->net_dev->stats);
			if (likely(!tx_error))
				lgmac->net_dev->stats.tx_packets++;
			else
				lgmac->net_dev->stats.tx_errors++;
		}

		PRT_DBG("%s: tx_clean %u, tx_dirty %u\n",
			lgmac->net_dev->name,
			lgmac->tx_clean, lgmac->tx_dirty);

		if (likely(lgmac->tx_skb_dma[entry])) {
			dma_unmap_single(lgmac->dev,
					 lgmac->tx_skb_dma[entry],
					 lombo_gmac_desc_tx_buf_len(desc),
					 DMA_TO_DEVICE);
			lgmac->tx_skb_dma[entry] = 0;
		}

		if (unlikely(desc->des3))
			desc->des3 = 0;

		if (likely(skb)) {
			dev_kfree_skb(skb);
			lgmac->tx_skb[entry] = NULL;
		}

		lombo_gmac_desc_tx_release(desc);

		lgmac->tx_dirty++;
	}

	if (unlikely(netif_queue_stopped(lgmac->net_dev) &&
		     lombo_gmac_desc_tx_avail(lgmac) >
		     LGMAC_TX_DESC_THRESH(lgmac))) {
		netif_tx_lock(lgmac->net_dev);
		if (netif_queue_stopped(lgmac->net_dev) &&
		    lombo_gmac_desc_tx_avail(lgmac) >
			LGMAC_TX_DESC_THRESH(lgmac)) {
			PRT_DBG("%s: restart transmit\n", lgmac->net_dev->name);
			netif_wake_queue(lgmac->net_dev);
		}
		netif_tx_unlock(lgmac->net_dev);
	}

	if (lgmac->eee_enable && !lgmac->tx_lpi_enter) {
		lombo_gmac_eee_enable(lgmac);
		mod_timer(&lgmac->eee_ctrl_timer,
			  jiffies + msecs_to_jiffies(LGMAC_LPI_TIMER_MS));
	}

	spin_unlock(&lgmac->tx_lock);
}

/**
 * lombo_gmac_tx_error - clean tx desc and restart transmit in case of errors.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gmac_tx_error(struct lombo_gmac *lgmac)
{
	u32 i, tx_cnt = lgmac->tx_count;

	PRT_DBG("%s: transmit errors\n", lgmac->net_dev->name);

	netif_stop_queue(lgmac->net_dev);

	csp_gmac_dma_stop_tx(lgmac->base_addr);
	lombo_gmac_dma_free_tx_skb(lgmac);
	for (i = 0; i < tx_cnt; i++)
		lombo_gmac_desc_tx_init(&lgmac->tx_desc[i],
					(i == (tx_cnt - 1)));
	lgmac->tx_dirty = 0;
	lgmac->tx_clean = 0;
	csp_gmac_dma_start_tx(lgmac->base_addr);

	lgmac->net_dev->stats.tx_errors++;
	netif_wake_queue(lgmac->net_dev);
}

/**
 * lombo_gmac_tx_timeout - tx timeout entry point of the driver.
 * @ndev: pointer to net device struct.
 *
 * This function is called when a packet transmission fails to
 * complete within a reasonable time.
 * The driver will mark the error in the netdev structure and arrange
 * for the device to be reset to a sane state in order to transmit a new packet.
 */
static void lombo_gmac_tx_timeout(struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	lombo_gmac_tx_error(lgmac);
}

/**
 * lombo_gamc_rx_refill - refill used skb preallocated buffer.
 * @lgmac: pointer to lombo gmac struct.
 */
static void lombo_gamc_rx_refill(struct lombo_gmac *lgmac)
{
	u32 entry, rx_cnt = lgmac->rx_count;
	struct idma_desc *desc;
	struct sk_buff *skb;

	for (; (lgmac->rx_clean - lgmac->rx_dirty) > 0; lgmac->rx_dirty++) {
		entry = lgmac->rx_dirty % rx_cnt;
		desc = lgmac->rx_desc + entry;

		if (likely(!lgmac->rx_skb[entry])) {
			skb = netdev_alloc_skb_ip_align(lgmac->net_dev,
							lgmac->dma_buf_sz);
			if (unlikely(!skb)) {
				PRT_WARN("%s: failed to alloc rx skb\n",
					 lgmac->net_dev->name);
				break;
			}

			lgmac->rx_skb[entry] = skb;
			lgmac->rx_skb_dma[entry] =
				dma_map_single(lgmac->dev, skb->data,
					       lgmac->dma_buf_sz,
					       DMA_FROM_DEVICE);
			desc->des2 = lgmac->rx_skb_dma[entry];
		}
		wmb();

		lombo_gmac_desc_rx_set_owner(desc);
		wmb();
	}
}

/**
 * lombo_gmac_rx - rx entry point of the driver.
 * @lgmac: pointer to lombo gmac struct.
 * @limit: napi bugget.
 *
 * This function called by the napi poll method.
 * It gets all the frames inside the ring.
 */
static int lombo_gmac_rx(struct lombo_gmac *lgmac, int limit)
{
	u32 rx_cnt = lgmac->rx_count;
	u32 entry = lgmac->rx_clean % rx_cnt;
	u32 next_entry, count = 0;
	int status, frame_len;
	struct idma_desc *desc;
	struct sk_buff *skb;

	PRT_DBG("%s: receive start...\n", lgmac->net_dev->name);

	while (count < limit) {
		desc = lgmac->rx_desc + entry;

		/* check if the desc is owned by DMA */
		if (lombo_gmac_desc_rx_get_owner(desc))
			break;

		count++;

		next_entry = (++lgmac->rx_clean) % rx_cnt;
		prefetch(lgmac->rx_desc + next_entry);

		/* read the status of the incoming frame */
		status = lombo_gmac_desc_rx_status(desc, lgmac,
						   &lgmac->net_dev->stats);
		if (unlikely(status == LGMAC_RX_DISCARD_FRAME)) {
			lgmac->net_dev->stats.rx_errors++;
		} else {
			/**
			 * Automatic Pad or FCS Stripping is set,
			 * GMAC core strips PAD/FCS for IEEE 802.3
			 * Type frames(LLC/LLC-SNAP).
			 */
			frame_len = lombo_gmac_desc_rx_frame_len(desc);
			if (unlikely(status != LGMAC_RX_LLC_SNAP))
				frame_len -= ETH_FCS_LEN;

			skb = lgmac->rx_skb[entry];
			if (unlikely(!skb)) {
				PRT_ERR("%s: inconsistent rx desc chain\n",
					lgmac->net_dev->name);
				lgmac->net_dev->stats.rx_dropped++;
				break;
			}
			prefetch(skb->data - NET_IP_ALIGN);
			lgmac->rx_skb[entry] = NULL;

			skb_put(skb, frame_len);
			dma_unmap_single(lgmac->dev,
					 lgmac->rx_skb_dma[entry],
					 lgmac->dma_buf_sz,
					 DMA_FROM_DEVICE);

			skb->protocol = eth_type_trans(skb, lgmac->net_dev);

			skb->ip_summed = CHECKSUM_UNNECESSARY;

			napi_gro_receive(&lgmac->napi, skb);

			lgmac->net_dev->stats.rx_packets++;
			lgmac->net_dev->stats.rx_bytes += frame_len;
		}
		entry = next_entry;
	}

	lombo_gamc_rx_refill(lgmac);

	PRT_DBG("%s: receive success\n", lgmac->net_dev->name);

	return count;
}

/**
 * lombo_gmac_poll - poll method (NAPI).
 * @lgmac: pointer to napi struct.
 * @budget: max number of packets that the current CPU can
 *          receive from all interfaces.
 *
 * Look at the incoming frames and clear the tx resources.
 */
static int lombo_gmac_poll(struct napi_struct *napi, int budget)
{
	int work_done = 0;
	struct lombo_gmac *lgmac = container_of(napi, struct lombo_gmac, napi);

	lombo_gmac_tx_clean(lgmac);

	work_done = lombo_gmac_rx(lgmac, budget);
	if (work_done < budget) {
		napi_complete(napi);
		lombo_gmac_dma_irq_enable(lgmac);
	}

	return work_done;
}

/**
 * lombo_gmac_set_config - set config entry point of the driver.
 * @ndev: pointer to net device struct.
 * @map: pointer to device mapping struct.
 */
static int lombo_gmac_set_config(struct net_device *ndev, struct ifmap *map)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	/* can't act on a running interface */
	if (ndev->flags & IFF_UP) {
		PRT_WARN("%s: net interface is running\n",
			 lgmac->net_dev->name);
		return -EBUSY;
	}

	/* do not allow changing the I/O address */
	if (map->base_addr != ndev->base_addr) {
		PRT_WARN("%s: can not change I/O address\n",
			 lgmac->net_dev->name);
		return -EOPNOTSUPP;
	}

	/* do not allow changing the IRQ number */
	if (map->irq != ndev->irq) {
		PRT_WARN("%s: can not change IRQ number\n",
			 lgmac->net_dev->name);
		return -EOPNOTSUPP;
	}

	return 0;
}

/**
 * lombo_gmac_set_frame_filter - set frame filter multicast addressing.
 * @ndev: pointer to net device struct.
 */
static void lombo_gmac_set_frame_filter(struct net_device *ndev)
{
	void *base_addr = (void *)ndev->base_addr;
	struct netdev_hw_addr *ha;
	int mc_count = netdev_mc_count(ndev);
	u32 bit_nr, value = 0;
	u32 mc_filter[2];

	if (ndev->flags & IFF_PROMISC) {
		/* pass all packets */
		value = LGMAC_FFILTER_PR;
	} else if (mc_count > LGMAC_HASH_TABLE_SIZE ||
		   ndev->flags & IFF_ALLMULTI) {
		/* pass all multicast packets */
		value = LGMAC_FFILTER_PM;

		csp_gmac_set_filter_hash_hi(base_addr, 0xFFFFFFFF);
		csp_gmac_set_filter_hash_lo(base_addr, 0xFFFFFFFF);
	} else if (!netdev_mc_empty(ndev)) {
		/* hash filter for multicast packets */
		value = LGMAC_FFILTER_HMC;

		memset(mc_filter, 0, sizeof(mc_filter));
		netdev_for_each_mc_addr(ha, ndev) {
			/* The upper 6 bits of the calculated CRC are used to
			 * index the content of the hash table.
			 */
			bit_nr = bitrev32(~crc32_le(~0, ha->addr, 6)) >> 26;
			/* The most significant bit determines the register to
			 * use (H/L) while the other 5 bits determine the bit
			 * within the register.
			 */
			mc_filter[bit_nr >> 5] |= BIT(bit_nr & 31);
		}

		csp_gmac_set_filter_hash_hi(base_addr, mc_filter[1]);
		csp_gmac_set_filter_hash_lo(base_addr, mc_filter[0]);
	}

	csp_gmac_set_frame_filter(base_addr, value);
}

/**
 * lombo_gmac_set_rx_mode - entry point for multicast addressing.
 * @ndev: pointer to net device struct.
 *
 * This function is called by the kernel whenever multicast address
 * must be enable or disable.
 */
static void lombo_gmac_set_rx_mode(struct net_device *ndev)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	spin_lock(&lgmac->lock);
	lombo_gmac_set_frame_filter(ndev);
	spin_unlock(&lgmac->lock);
}

/**
 * lombo_gmac_change_mtu - entry point to change MTU of the net device.
 * @ndev: pointer to net device struct.
 * @new_mtu: MTU value to set.
 *
 * The Maximum Transfer Unit (MTU) is used by the network layer
 * to drive packet transmission. Ethernet has an MTU of 1500 octets
 * (ETH_DATA_LEN). This value can be changed with ifconfig.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_change_mtu(struct net_device *ndev, int new_mtu)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	int max_mtu;

	if (netif_running(ndev)) {
		PRT_WARN("%s: net interface is running\n",
			 lgmac->net_dev->name);
		return -EBUSY;
	}

	max_mtu = SKB_MAX_HEAD(NET_SKB_PAD + NET_IP_ALIGN);
	if (new_mtu < 46 || new_mtu > max_mtu) {
		PRT_ERR("%s: invalid mtu %d, max mtu %d\n",
			lgmac->net_dev->name, new_mtu, max_mtu);
		return -EINVAL;
	}

	ndev->mtu = new_mtu;
	netdev_update_features(ndev);

	return 0;
}

/**
 * lombo_gmac_fix_features - fix features entry point of the driver.
 * @ndev: pointer to net device struct.
 * @features: net device features.
 *
 * return new net device features value.
 */
static netdev_features_t lombo_gmac_fix_features(struct net_device *ndev,
						 netdev_features_t features)
{
	return features;
}

/**
 * lombo_gmac_irq - GMAC ISR.
 * @irq: interrupt number.
 * @dev_id: pointer to net device struct.
 *
 * This is the main driver interrupt service routine.
 * It calls the DMA ISR and also the Core ISR to manage other interrupts.
 *
 * return irqreturn_t value.
 */
static irqreturn_t lombo_gmac_irq(int irq, void *dev_id)
{
	struct net_device *ndev = (struct net_device *)dev_id;
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	u32 status, lpi_sta, rgmii_sta;

	if (unlikely(!ndev)) {
		PRT_ERR("%s: net device not exist\n", lgmac->net_dev->name);
		return IRQ_NONE;
	}

	status = csp_gmac_get_int_status(lgmac->base_addr);

	PRT_DBG("%s: get interrupt status 0x%08x\n",
		lgmac->net_dev->name, status);

	/* LPI interrupt */
	if (status & LGMAC_INT_LPI) {
		lpi_sta = csp_gmac_get_lpi_ctrl_status(lgmac->base_addr);
		PRT_DBG("%s: get LPI status 0x%08x\n",
			lgmac->net_dev->name, lpi_sta);

		if (lpi_sta & LGMAC_LPI_TX_LPI_ENTRY) {
			PRT_DBG("%s: tramsit enter in LPI\n",
				lgmac->net_dev->name);
			lgmac->tx_lpi_enter = true;
		}

		if (lpi_sta & LGMAC_LPI_TX_LPI_EXIT) {
			PRT_DBG("%s: tramsit exit from LPI\n",
				lgmac->net_dev->name);
			lgmac->tx_lpi_enter = false;
		}
	}

	/* RGMII interrupt */
	if (status & LGMAC_INT_RGMII)
		rgmii_sta = csp_gmac_get_rgmii_status(lgmac->base_addr);

	/* DMA interrupt */
	lombo_gmac_dma_irq(lgmac);

	return IRQ_HANDLED;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/**
 * lombo_gmac_poll_controller - polling receive.
 * @ndev: pointer to net device struct.
 *
 * Used by NETCONSOLE and other diagnostic tools to allow network I/O
 * with interrupts disabled.
 */
static void lombo_gmac_poll_controller(struct net_device *ndev)
{
	disable_irq(ndev->irq);
	lombo_gmac_irq(ndev->irq, ndev);
	enable_irq(ndev->irq);
}
#endif

/**
 * lombo_gmac_do_ioctl - IOCTL entry point of the driver.
 * @ndev: pointer to net device struct.
 * @rq: an IOCTL specefic structure, that can contain a pointer to
 *      a proprietary structure used to pass information to the driver.
 * @cmd: IOCTL command
 */
static int lombo_gmac_do_ioctl(struct net_device *ndev,
			       struct ifreq *rq, int cmd)
{
	int ret = -EOPNOTSUPP;
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	if (!netif_running(ndev))
		return -EINVAL;

	switch (cmd) {
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		if (!lgmac->phy_dev)
			return -EINVAL;
		ret = phy_mii_ioctl(lgmac->phy_dev, rq, cmd);
		break;
	default:
		break;
	}

	return ret;
}

static const struct net_device_ops lombo_gmac_netdev_ops = {
	.ndo_open		= lombo_gmac_open,
	.ndo_stop		= lombo_gmac_stop,
	.ndo_start_xmit		= lombo_gmac_start_xmit,
	.ndo_set_rx_mode	= lombo_gmac_set_rx_mode,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_do_ioctl		= lombo_gmac_do_ioctl,
	.ndo_set_config		= lombo_gmac_set_config,
	.ndo_change_mtu		= lombo_gmac_change_mtu,
	.ndo_tx_timeout		= lombo_gmac_tx_timeout,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= lombo_gmac_poll_controller,
#endif
	.ndo_fix_features	= lombo_gmac_fix_features,
};

/**
 * lombo_gmac_ethtool_getsettings - ethtool get settings.
 * @ndev: pointer to net device struct.
 * @cmd: pointer to ethtool cmd struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_ethtool_getsettings(struct net_device *ndev,
					  struct ethtool_cmd *cmd)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	struct phy_device *phy_dev = ndev->phydev;
	int ret = 0;

	if (!phy_dev) {
		PRT_WARN("%s: PHY device not exist\n", lgmac->net_dev->name);
		return -ENODEV;
	}

	if (!netif_running(ndev)) {
		PRT_WARN("%s: net interface is disabled\n",
			 lgmac->net_dev->name);
		return -EBUSY;
	}

	cmd->transceiver = XCVR_INTERNAL;
	spin_lock_irq(&lgmac->lock);
	ret = phy_ethtool_gset(phy_dev, cmd);
	spin_unlock_irq(&lgmac->lock);

	return ret;
}

/**
 * lombo_gmac_ethtool_setsettings - ethtool set settings.
 * @ndev: pointer to net device struct.
 * @cmd: pointer to ethtool cmd struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_ethtool_setsettings(struct net_device *ndev,
					  struct ethtool_cmd *cmd)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	struct phy_device *phy_dev = ndev->phydev;
	int ret = 0;

	spin_lock(&lgmac->lock);
	ret = phy_ethtool_sset(phy_dev, cmd);
	spin_unlock(&lgmac->lock);

	return ret;
}

/**
 * lombo_gmac_ethtool_getdrvinfo - ethtool get driver info.
 * @ndev: pointer to net device struct.
 * @info: pointer to ethtool driver info struct.
 */
static void lombo_gmac_ethtool_getdrvinfo(struct net_device *ndev,
					  struct ethtool_drvinfo *info)
{
	strlcpy(info->driver, LGMAC_DRV_NAME, sizeof(info->driver));
	strlcpy(info->version, LGMAC_DRV_VERSION, sizeof(info->version));
	info->fw_version[0] = '\0';
}

/**
 * lombo_gmac_ethtool_get_coalesce - ethtool get interrupt coalescing parameter.
 * @ndev: pointer to net device struct.
 * @coalesce: pointer to ethtool coalesce struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_ethtool_get_coalesce(struct net_device *ndev,
					   struct ethtool_coalesce *coalesce)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	unsigned long clk_rate = clk_get_rate(lgmac->clk_core);

	coalesce->tx_max_coalesced_frames = lgmac->tx_coal_frames;
	coalesce->tx_coalesce_usecs = lgmac->tx_coal_time;
	coalesce->rx_coalesce_usecs = clk_rate ?
		(256 * lgmac->rx_wdt_cnt) / (clk_rate / 1000000) : 0;

	return 0;
}

/**
 * lombo_gmac_ethtool_set_coalesce - ethtool set interrupt coalescing parameter.
 * @ndev: pointer to net device struct.
 * @coalesce: pointer to ethtool coalesce struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_ethtool_set_coalesce(struct net_device *ndev,
					   struct ethtool_coalesce *coalesce)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	unsigned long clk_rate = clk_get_rate(lgmac->clk_core);
	u32 rx_wdt_cnt;

	if (coalesce->rx_max_coalesced_frames ||
	    coalesce->rx_coalesce_usecs_irq ||
	    coalesce->rx_max_coalesced_frames_irq ||
	    coalesce->tx_coalesce_usecs_irq ||
	    coalesce->use_adaptive_rx_coalesce ||
	    coalesce->use_adaptive_tx_coalesce ||
	    coalesce->pkt_rate_low ||
	    coalesce->rx_coalesce_usecs_low ||
	    coalesce->rx_max_coalesced_frames_low ||
	    coalesce->tx_coalesce_usecs_high ||
	    coalesce->tx_max_coalesced_frames_low ||
	    coalesce->pkt_rate_high ||
	    coalesce->tx_coalesce_usecs_low ||
	    coalesce->rx_coalesce_usecs_high ||
	    coalesce->rx_max_coalesced_frames_high ||
	    coalesce->tx_max_coalesced_frames_irq ||
	    coalesce->stats_block_coalesce_usecs ||
	    coalesce->tx_max_coalesced_frames_high ||
	    coalesce->rate_sample_interval)
		return -EOPNOTSUPP;

	if (!coalesce->rx_coalesce_usecs)
		return -EINVAL;

	if (!coalesce->tx_coalesce_usecs &&
	    !coalesce->tx_max_coalesced_frames)
		return -EINVAL;

	if (coalesce->tx_coalesce_usecs > LGMAC_TX_COAL_TIMER_US ||
	    coalesce->tx_max_coalesced_frames > LGMAC_TX_COAL_FRAME_MAX)
		return -EINVAL;

	rx_wdt_cnt = clk_rate ?
		(coalesce->rx_coalesce_usecs * (clk_rate / 1000000)) / 256 : 0;

	if (rx_wdt_cnt > LGMAC_MAX_RX_WDT_CNT ||
	    rx_wdt_cnt < LGMAC_MIN_RX_WDT_CNT)
		return -EINVAL;
	else
		return -EOPNOTSUPP;

	lgmac->tx_coal_frames = coalesce->tx_max_coalesced_frames;
	lgmac->tx_coal_time = coalesce->tx_coalesce_usecs;
	lgmac->rx_wdt_cnt = rx_wdt_cnt;
	csp_gmac_set_rx_watchdog(lgmac->base_addr, lgmac->rx_wdt_cnt);

	return 0;
}

/**
 * lombo_gmac_ethtool_set_pauseparam - ethtool get flow control parameter.
 * @ndev: pointer to net device struct.
 * @pause: pointer to ethtool flow control parameter struct.
 */
static void lombo_gmac_ethtool_get_pauseparam(struct net_device *ndev,
					      struct ethtool_pauseparam *param)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	param->rx_pause = 0;
	param->tx_pause = 0;

	if (!(ndev->phydev->supported & SUPPORTED_Pause) ||
	    !(ndev->phydev->supported & SUPPORTED_Asym_Pause))
		return;

	spin_lock(&lgmac->lock);

	param->autoneg = lgmac->phy_dev->autoneg;

	if (lgmac->flow_ctrl & LGMAC_FLOW_CTRL_RX)
		param->rx_pause = 1;

	if (lgmac->flow_ctrl & LGMAC_FLOW_CTRL_TX)
		param->tx_pause = 1;

	spin_unlock(&lgmac->lock);
}

/**
 * lombo_gmac_ethtool_set_pauseparam - ethtool set flow control parameter.
 * @ndev: pointer to net device struct.
 * @pause: pointer to ethtool flow control parameter struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_ethtool_set_pauseparam(struct net_device *ndev,
					     struct ethtool_pauseparam *param)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);
	struct phy_device *phy_dev = lgmac->phy_dev;
	int ret = 0;

	spin_lock(&lgmac->lock);

	phy_dev->autoneg = param->autoneg;

	lgmac->flow_ctrl = 0;
	if (param->rx_pause)
		lgmac->flow_ctrl |= LGMAC_FLOW_CTRL_RX;
	if (param->tx_pause)
		lgmac->flow_ctrl |= LGMAC_FLOW_CTRL_TX;

	if (param->autoneg) {
		if (netif_running(ndev))
			ret = phy_start_aneg(phy_dev);
	} else {
		lombo_gmac_flow_control(lgmac);
	}

	spin_unlock(&lgmac->lock);

	return ret;
}

/**
 * lombo_gmac_check_if_running - ethtool begin, check if net device is running.
 * @ndev: pointer to net device struct.
 *
 * return 0 if device is running; otherwise not running.
 */
static int lombo_gmac_ethtool_begin(struct net_device *ndev)
{
	if (!netif_running(ndev))
		return -EBUSY;

	return 0;
}

/**
 * lombo_gmac_ethtool_get_eee - ethtool get EEE supported and status.
 * @ndev: pointer to net device struct.
 * @eee: pointer to ethtool eee struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_ethtool_get_eee(struct net_device *ndev,
				      struct ethtool_eee *eee)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	eee->eee_enabled = lgmac->eee_enable;
	eee->eee_active = lgmac->eee_active;
	eee->tx_lpi_timer = lgmac->tx_lpi_time;

	return phy_ethtool_get_eee(ndev->phydev, eee);
}

/**
 * lombo_gmac_ethtool_set_eee - ethtool set EEE supported and status.
 * @ndev: pointer to net device struct.
 * @eee: pointer to ethtool eee struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_ethtool_set_eee(struct net_device *ndev,
				      struct ethtool_eee *eee)
{
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	lgmac->eee_enable = eee->eee_enabled;

	if (lgmac->eee_enable) {
		/**
		 * We are asking for enabling the EEE but it is safe
		 * to verify all by invoking the eee_init function.
		 * In case of failure it will return an error.
		 */
		lgmac->eee_enable = lombo_gmac_eee_init(lgmac);
		if (!lgmac->eee_enable)
			return -EOPNOTSUPP;

		/* Do not change tx_lpi_timer in case of failure */
		lgmac->tx_lpi_time = eee->tx_lpi_timer;
	} else
		lombo_gmac_eee_disable(lgmac);

	return phy_ethtool_set_eee(lgmac->phy_dev, eee);
}

static const struct ethtool_ops lombo_gmac_ethtool_ops = {
	.get_settings	= lombo_gmac_ethtool_getsettings,
	.set_settings	= lombo_gmac_ethtool_setsettings,
	.get_drvinfo	= lombo_gmac_ethtool_getdrvinfo,
	.get_link	= ethtool_op_get_link,
	.get_coalesce	= lombo_gmac_ethtool_get_coalesce,
	.set_coalesce	= lombo_gmac_ethtool_set_coalesce,
	.get_pauseparam	= lombo_gmac_ethtool_get_pauseparam,
	.set_pauseparam	= lombo_gmac_ethtool_set_pauseparam,
	.begin		= lombo_gmac_ethtool_begin,
	.get_eee	= lombo_gmac_ethtool_get_eee,
	.set_eee	= lombo_gmac_ethtool_set_eee,
};

/**
 * lombo_gmac_parse_dt - parse config from device tree.
 * @lgmac: pointer to lombo gmac struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_parse_dt(struct lombo_gmac *lgmac)
{
	int ret;
	enum of_gpio_flags flags;

	/* get phy interface */
	lgmac->phy_interface = of_get_phy_mode(lgmac->dev->of_node);
	if (lgmac->phy_interface < 0) {
		PRT_ERR("%s: failed to get phy interface type (%d)\n",
			dev_name(lgmac->dev),
			lgmac->phy_interface);
		return lgmac->phy_interface;
	}

	/* supported phy interface: MII, RMII, RGMII */
	if (lgmac->phy_interface != PHY_INTERFACE_MODE_MII &&
	    lgmac->phy_interface != PHY_INTERFACE_MODE_RMII &&
	    lgmac->phy_interface != PHY_INTERFACE_MODE_RGMII) {
		PRT_ERR("%s: unsupported phy interface %d\n",
			dev_name(lgmac->dev),
			lgmac->phy_interface);
		return -EPFNOSUPPORT;
	}

	/* get rmii clock source */
	if (lgmac->phy_interface == PHY_INTERFACE_MODE_RMII) {
		ret = of_property_read_u32(lgmac->dev->of_node,
					   "rmii_osc_ext",
					   &lgmac->rmii_osc_ext);
		if (ret) {
			PRT_ERR("%s: failed to get rmii tx clk src (%d)\n",
				dev_name(lgmac->dev), ret);
			return ret;
		}
	}

	/* get phy reset pin and reset level */
	lgmac->phy_rst_pin = of_get_named_gpio_flags(lgmac->dev->of_node,
						     "phy_rst_pin", 0, &flags);
	if (gpio_is_valid(lgmac->phy_rst_pin))
		lgmac->phy_rst_level = flags;
	else
		PRT_WARN("%s: failed to get phy reset pin (%d)\n",
			 dev_name(lgmac->dev), lgmac->phy_rst_pin);

	/* get tx delay */
	ret = of_property_read_u32_array(lgmac->dev->of_node,
					 "tx_delay", lgmac->tx_delay, 3);
	if (ret)
		PRT_WARN("%s: failed to get tx delay parameters (%d)\n",
			 dev_name(lgmac->dev), ret);

	/* get rx delay */
	ret = of_property_read_u32_array(lgmac->dev->of_node,
					 "rx_delay", lgmac->rx_delay, 3);
	if (ret)
		PRT_WARN("%s: failed to get rx delay parameters (%d)\n",
			 dev_name(lgmac->dev), ret);

	return 0;
}

/**
 * lombo_gmac_probe - probe entry point of the driver.
 * @pdev: pointer to platform device struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct net_device *ndev = NULL;
	struct lombo_gmac *lgmac = NULL;
	struct device *dev = &pdev->dev;
	int ret = 0;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		PRT_ERR("%s: failed to get memory resource\n", dev_name(dev));
		return -ENODEV;
	}

	ndev = alloc_etherdev(sizeof(struct lombo_gmac));
	if (!ndev) {
		PRT_ERR("%s: failed to alloc net device\n", dev_name(dev));
		return -ENOMEM;
	}

	ether_setup(ndev);
	SET_NETDEV_DEV(ndev, dev);

	lgmac = netdev_priv(ndev);
	lgmac->net_dev = ndev;
	lgmac->dev = dev;

	lgmac->base_addr = devm_ioremap_resource(dev, res);
	if (IS_ERR(lgmac->base_addr)) {
		ret = PTR_ERR(lgmac->base_addr);
		PRT_ERR("%s: failed to ioremap (%d)\n", dev_name(dev), ret);
		goto exit_free_net_device;
	}

	ndev->irq = platform_get_irq(pdev, 0);
	if (ndev->irq < 0) {
		ret = ndev->irq;
		PRT_ERR("%s: failed to get irq number (%d)\n",
			dev_name(dev), ret);
		goto exit_free_net_device;
	}

	ret = lombo_gmac_parse_dt(lgmac);
	if (ret) {
		PRT_ERR("%s: failed to parse device tree (%d)\n",
			dev_name(dev), ret);
		goto exit_free_net_device;
	}

	lgmac->flow_ctrl = LGMAC_FLOW_CTRL_OFF;
	lgmac->phy_addr = -1;

	spin_lock_init(&lgmac->lock);
	spin_lock_init(&lgmac->tx_lock);

	ndev->hw_features    = NETIF_F_SG | NETIF_F_IP_CSUM |
			       NETIF_F_IPV6_CSUM | NETIF_F_RXCSUM;
	ndev->features	    |= ndev->hw_features | NETIF_F_HIGHDMA;
	ndev->base_addr	     = (unsigned long)lgmac->base_addr;
	ndev->watchdog_timeo = msecs_to_jiffies(LGMAC_TX_TIMEOUT_MS);

	ndev->netdev_ops     = &lombo_gmac_netdev_ops;
	SET_ETHTOOL_OPS(ndev, &lombo_gmac_ethtool_ops);

	csp_gmac_default_mac_addr(lgmac->base_addr, ndev->dev_addr);

	netif_napi_add(ndev, &lgmac->napi, lombo_gmac_poll, 64);

	if (gpio_is_valid(lgmac->phy_rst_pin)) {
		ret = devm_gpio_request(lgmac->dev, lgmac->phy_rst_pin,
					"phy_rst_pin");
		if (ret) {
			PRT_ERR("%s: failed to request phy reset pin (%d)\n",
				dev_name(dev), ret);
			goto exit_delete_napi;
		}
	}

	ret = lombo_gmac_clock_get(lgmac);
	if (ret) {
		PRT_ERR("%s: failed to get module clock (%d)\n",
			dev_name(dev), ret);
		goto exit_delete_napi;
	}

	ret = register_netdev(ndev);
	if (ret) {
		PRT_ERR("%s: failed to register net device (%d)\n",
			dev_name(dev), ret);
		goto exit_delete_napi;
	}

	netif_carrier_off(ndev);

	platform_set_drvdata(pdev, lgmac->net_dev);

	PRT_INFO("%s: driver probe success\n", lgmac->net_dev->name);

	return 0;

exit_delete_napi:
	netif_napi_del(&lgmac->napi);

exit_free_net_device:
	free_netdev(ndev);

	return ret;
}

/**
 * lombo_gmac_remove - remove entry point of the driver.
 * @pdev: pointer to platform device struct.
 *
 * return 0 if success; otherwise failed.
 */
static int lombo_gmac_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct lombo_gmac *lgmac = netdev_priv(ndev);

	PRT_INFO("%s: driver remove\n", lgmac->net_dev->name);

	netif_carrier_off(ndev);
	netif_napi_del(&lgmac->napi);
	unregister_netdev(ndev);
	free_netdev(ndev);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

#ifdef CONFIG_PM
static int lombo_gmac_suspend(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);

	if (!ndev || !netif_running(ndev))
		return 0;

	return lombo_gmac_stop(ndev);
}

static int lombo_gmac_resume(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);

	if (!ndev || !netif_running(ndev))
		return 0;

	return lombo_gmac_open(ndev);
}

static const struct dev_pm_ops lombo_gmac_pm_ops = {
	.suspend = lombo_gmac_suspend,
	.resume  = lombo_gmac_resume,
};
#endif

static const struct of_device_id lombo_gmac_match[] = {
	{ .compatible = "lombo,n7v1-gmac", },
	{},
};

MODULE_DEVICE_TABLE(of, lombo_gmac_match);

static struct platform_driver lombo_gmac_driver = {
	.probe  = lombo_gmac_probe,
	.remove = lombo_gmac_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name  = "lombo-gmac",
#ifdef CONFIG_PM
		.pm    = &lombo_gmac_pm_ops,
#endif
		.of_match_table = lombo_gmac_match,
	},
};

module_platform_driver(lombo_gmac_driver);

MODULE_DESCRIPTION("Driver for LomboTech GMAC Controller");
MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_LICENSE("GPL v2");
