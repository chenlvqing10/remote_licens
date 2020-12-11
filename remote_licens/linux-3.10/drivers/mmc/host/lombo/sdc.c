/*
 * sdc.c - SDC host driver code for LomboTech
 * sdc driver driver code implement
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
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>

#if 0
#include <linux/regulator/consumer.h>
#endif
#include <linux/pm_runtime.h>

#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/slot-gpio.h>

#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include <linux/delay.h>

/* #define ENABLE_TRACE */
#include <mach/debug.h>
#undef MOD_NAME
#define MOD_NAME	"SDC"

#include "sdc.h"
#ifdef CONFIG_ARCH_LOMBO_N7
#include "n7/sdc_csp.h"
#include "n7/sdc_const.h"
#elif defined CONFIG_PLAT_LOMBO_N8
#include "n8/sdc_csp.h"
#include "n8/sdc_const.h"
#elif defined CONFIG_ARCH_LOMBO_N9
#include "n9/sdc_csp.h"
#include "n9/sdc_const.h"
#else
#error "No supported platform!"
#endif

#define CHK_RET_IF_GO(_ret, _lable)			\
	do {						\
		if (0 != _ret) {			\
			_ret = __LINE__;		\
			goto _lable;			\
		}					\
	} while (0)

#define DRIVER_NAME "lbsdc"

/* specify the difference for platform devices */
struct lbsdc_drv_data {
	/* TODO */
	u32 dummy;
};

static struct lbsdc_host *sdio_host;

static void lbsdc_tasklet_finish(unsigned long param);

/* finish the data request */
static void lbsdc_finish_data(struct lbsdc_host *);

/* send the mmc command */
static void lbsdc_send_command(struct lbsdc_host *, struct mmc_command *);

/* finish the mmc command */
static void lbsdc_finish_command(struct lbsdc_host *);

/* execute tuning sequence */
static int lbsdc_execute_tuning(struct mmc_host *mmc, u32 opcode);

/*
 * card detect
 * sdc internal card detect function is removed,
 * maybe has ext card detect gpio, maybe not
*/
static int lbsdc_do_get_cd(struct lbsdc_host *host);

/* runtime handler */
#ifdef CONFIG_PM_RUNTIME
static int lbsdc_runtime_pm_get(struct lbsdc_host *host);
static int lbsdc_runtime_pm_put(struct lbsdc_host *host);
#else
static inline int lbsdc_runtime_pm_get(struct lbsdc_host *host)
{
	return 0;
}
static inline int lbsdc_runtime_pm_put(struct lbsdc_host *host)
{
	return 0;
}
#endif

/**
 * lbsdc_scan_card - for sdio device driver to trigger card detect.
 * @on: 0, power off; 1, power on;
 *
 * return 0 if success; otherwise failed.
 */
int lbsdc_scan_card(u32 on)
{
	if (NULL == sdio_host)
		return -ENODEV;
	sdio_host->pw_state = on;
	mmc_detect_change(sdio_host->mmc, 0);
	return 0;
}
EXPORT_SYMBOL(lbsdc_scan_card);

/*****************************************************************************\
 *                                                                           *
 * Low-level functions                                                       *
 *                                                                           *
\*****************************************************************************/
#ifdef CONFIG_ARCH_LOMBO_N7
/**
 * lbsdc_wait_pd - enable/disable SDC clock
 * @host:	point to the sdc host structure
 * @pd_mask:	expected pending
 * @ms:		millisecond to wait
 *
 * return 0 while sucess, otherwise return error code line number
 */
static u32 lbsdc_wait_pd(struct lbsdc_host *host, u32 pd_mask, u32 ms)
{
	u32 wait_cnt = 0;
	u32 ret = 0;

	/* check whether all the expected pending is raise */
	while (csp_sdc_chk_pd(host->base, pd_mask) == 0) {
		/* we should check if there any error occur */
		if (csp_sdc_chk_err(host->base) != 0) {
			ret = __LINE__;
			break;
		}

		if (wait_cnt >= 1000 * ms) {
			PRT_WARN("%s: wait_cnt=%d\n",
				mmc_hostname(host->mmc), wait_cnt);
			/* timeout, update failed */
			ret = __LINE__;
			break;
		}
		udelay(1);
		wait_cnt++;
	}

	return ret;
}
#endif

/**
 * lbsdc_enable_clk - enable/disable SDC clock
 * @host:	point to the sdc host structure
 * @enable:	0, disable; 1, enable
 *
 * return 0 while sucess, otherwise return error code line number
 */
static u32 lbsdc_enable_clk(struct lbsdc_host *host, u32 enable)
{
	u32 ret = 0;

	csp_sdc_enable_clk(host->base, enable);
	if (!(host->flags & LBSDC_LP_DISABLED))
		csp_sdc_enable_clk_lp(host->base, enable);
#ifdef CONFIG_ARCH_LOMBO_N7
	csp_sdc_update_clk_pre(host->base);
	ret = lbsdc_wait_pd(host, SDC_INT_PD_UCF, LBSDC_WAIT_UCF_TIMEOUT);
	CHK_RET_IF_GO(ret, out);
	csp_sdc_update_clk_post(host->base);
	csp_sdc_clr_pd(host->base, SDC_INT_PD_UCF);

out:
#endif
	return ret;
}

/**
 * lbsdc_enable_clk_lp - enable/disable SDC clock's low-power mode
 * @host:	point to the sdc host structure
 * @enable:	0, disable; 1, enable
 *
 * return 0 while sucess, otherwise return error code line number
 */
static u32 lbsdc_enable_clk_lp(struct lbsdc_host *host, u32 enable)
{
	u32 ret = 0;

	csp_sdc_enable_clk_lp(host->base, enable);
#ifdef CONFIG_ARCH_LOMBO_N7
	csp_sdc_update_clk_pre(host->base);
	ret = lbsdc_wait_pd(host, SDC_INT_PD_UCF, LBSDC_WAIT_UCF_TIMEOUT);
	CHK_RET_IF_GO(ret, out);
	csp_sdc_update_clk_post(host->base);
	csp_sdc_clr_pd(host->base, SDC_INT_PD_UCF);
out:
#endif
	return ret;
}

static void lbsdc_reset_all(struct lbsdc_host *host)
{
	csp_sdc_reset_all(host->base);

#ifdef CONFIG_ARCH_LOMBO_N7
	csp_sdc_set_clk_div(host->base, 0);
	csp_sdc_set_clk_src(host->base, 0);
#endif

	/* enable SDC clock and low-power mode */
	/* TODO: we need a power cycle if enable clock failed */
	lbsdc_enable_clk(host, 1);
}

/*****************************************************************************\
 *                                                                           *
 * Core functions                                                            *
 *                                                                           *
\*****************************************************************************/

/* read one block through PIO */
static void lbsdc_read_block_pio(struct lbsdc_host *host)
{
	unsigned long flags;
	size_t blksize = host->data->blksz;
	size_t len = 0, byte_done = 0;
	u32 *buf = NULL;

	PRT_DBG("%s: PIO reading\n", mmc_hostname(host->mmc));

	/* Maybe not disable irq is ok,
	 * but we should transfer the data as for as possible to prevent
	 * host timeout or data request timeout. */
	local_irq_save(flags);

	while (byte_done < blksize) {
		/* get next data segment from sg_miter */
		if (!sg_miter_next(&host->sg_miter))
			BUG();

		len = min(host->sg_miter.length, blksize - byte_done);

		/* length that are not aligned with 32bit is ugly,
		 * currently we do not support it.
		 * TODO: support it.
		 */
		BUG_ON(len & 0x3);

		buf = host->sg_miter.addr;
#if 0
		while (len > 0) {
			/* read the sdc data register for data */
			csp_sdc_pio_read(host->base, buf, 4);

			buf++;
			len -= 4;
		}
#else
		csp_sdc_pio_read(host->base, buf, len);
#endif
		byte_done += len;
		/* info the sg_miter the bytes have been consumed. */
		host->sg_miter.consumed = len;
	}

	/* now one block has been read, stop mapping iteration */
	sg_miter_stop(&host->sg_miter);

	local_irq_restore(flags);
}

/* write one block through PIO */
static void lbsdc_write_block_pio(struct lbsdc_host *host)
{
	unsigned long flags;
	size_t blksize = host->data->blksz;
	size_t len = 0, byte_done = 0;
	u32 *buf = NULL;

	PRT_DBG("%s: PIO writing\n", mmc_hostname(host->mmc));

	local_irq_save(flags);

	while (byte_done < blksize) {
		/* get next data segment from sg_miter */
		if (!sg_miter_next(&host->sg_miter))
			BUG();

		len = min(host->sg_miter.length, blksize - byte_done);

		/* length that are not aligned with 4byte is ugly,
		 * currently we do not support it.
		 * TODO: support it.
		 */
		BUG_ON(len & 0x3);

		buf = host->sg_miter.addr;

#if 0
		while (len > 0) {
			/* write data to the sdc data register */
			csp_sdc_pio_write(host->base, buf, 4);

			buf++;
			len -= 4;
		}
#else
		csp_sdc_pio_write(host->base, buf, len);
#endif

		byte_done += len;
		/* info the sg_miter the bytes have been consumed. */
		host->sg_miter.consumed = len;
	}

	/* now one block has been written, stop mapping iteration */
	sg_miter_stop(&host->sg_miter);

	local_irq_restore(flags);
}

/* PIO data read/write entry */
static void lbsdc_transfer_pio(struct lbsdc_host *host)
{
	BUG_ON(!host->data);

	PRT_DBG("\n");

	if (host->blocks == 0)
		return;

	if (host->data->flags & MMC_DATA_READ)
		lbsdc_read_block_pio(host);
	else
		lbsdc_write_block_pio(host);

	host->blocks--;

	if ((host->data->flags & MMC_DATA_READ) &&
		(host->int_status & SDC_INT_PD_DTF)) {
		while (host->blocks != 0) {
			lbsdc_read_block_pio(host);
			host->blocks--;
		}
	}
	PRT_DBG("%s: PIO transfer complete.\n", mmc_hostname(host->mmc));
}

/**
 * lbsdc_set_idma_desc - set dual-buf mode idma descriptors for idma transfer
 * @desc:	the idma descr. address
 * @addr:	buffer addr
 * @len:	buffer len
 * @second_buf: to fill the second buffer or first buffer of the descr.
 */
static void lbsdc_set_idma_desc(struct idma_desc *desc, dma_addr_t addr,
		u32 len, u32 second_buf)
{
	if (second_buf == 0) {
		desc->des0.val = 0;
		desc->des0.bits.own = 1; /* info the descr. is own by IDMAC */
		desc->des0.bits.er = 0;  /* not ring mode */
		desc->des0.bits.dic = 0; /* not disable trans_end int */
		desc->des0.bits.ch = 0;	 /* dual buf mode */

		/* set buffer1 size */
		desc->des2.bits.bs1 = len;
		/* set buffer1 address */
		desc->des4.bap1 = addr;
	} else {
		/* set buffer2 size */
		desc->des2.bits.bs2 = len;
		/* set buffer1 address */
		desc->des6.bap2 = addr;
	}
}

/* mark the descr. as the first entry */
static void lbsdc_mark_idma_desc_first(struct idma_desc *desc)
{
	desc->des0.bits.fs = 1;
}

/* mark the descr. as the last entry */
static void lbsdc_mark_idma_desc_end(struct idma_desc *desc)
{
	desc->des0.bits.ld = 1;
}

/**
 * fill descriptors table from data's scatterlist,
 * and do mapping for dma transfer.
 */
static int lbsdc_idma_table_pre(struct lbsdc_host *host,
	struct mmc_data *data)
{
	int direction;

	struct idma_desc *desc;
	dma_addr_t addr;
	int len;

	struct scatterlist *sg;
	int i;
	u32 second_buf = 0;

	if (data->flags & MMC_DATA_READ)
		direction = DMA_FROM_DEVICE;
	else
		direction = DMA_TO_DEVICE;

#if 0
PRT_DBG("%s: opcode=%d,direction=%d\n", mmc_hostname(host->mmc),
	data->mrq->cmd->opcode, direction);
#endif

	/* we should clear the idma table first */
	memset(host->idma_desc, 0, LBSDC_MAX_DESC_CNT * IDMA_DESC_SIZE);

	/* map the data->sg here, because we will use the mapped addr below */
	host->sg_count = dma_map_sg(mmc_dev(host->mmc),
		data->sg, data->sg_len, direction);
	if (host->sg_count == 0) {
		PRT_ERR("dma_map_sg failed!\n");
		goto fail;
	}

	desc = host->idma_desc;

	for_each_sg(data->sg, sg, host->sg_count, i) {
		addr = sg_dma_address(sg);
		len = sg_dma_len(sg);

		/*
		 * TODO
		 * Maybe we should not treat address not 32bits aligned as bug,
		 * we can fall back pio transfer to deal with it.
		 */
		BUG_ON(addr & LBSDC_IDMA_ADDR_MASK); /* 32bits aligned */
		BUG_ON(len > MAX_IDMA_DESC_BUF_SIZE);

		/* add buffer to idma descr. */
		lbsdc_set_idma_desc(desc, addr, len, second_buf);

		second_buf++;
		BUG_ON(second_buf > 2);

		/* if last dual-buf desc is full, use new idma_desc */
		/* or use the second buf */
		if (second_buf == 2) {
			second_buf = 0;
			desc++;
		}
	}

	/*
	 * Mark first entry.
	 */
	lbsdc_mark_idma_desc_first(host->idma_desc);

	/*
	 * Mark terminating entry.
	 */
	lbsdc_mark_idma_desc_end((second_buf == 1) ? desc : (desc - 1));

	/* the iDMA descriptors have been fill, map it for idma transfer */
	host->idma_addr = dma_map_single(mmc_dev(host->mmc),
		host->idma_desc, LBSDC_MAX_DESC_CNT * IDMA_DESC_SIZE,
		DMA_TO_DEVICE);

	if (dma_mapping_error(mmc_dev(host->mmc), host->idma_addr)) {
		PRT_ERR("dma_map_single failed!\n");
		goto unmap_entries;
	}
	/* iDMA address should be 32bits aligned */
	BUG_ON(host->idma_addr & LBSDC_IDMA_ADDR_MASK);

	/* configure the idma descriptors table */
	csp_sdc_idma_desc_conf(host->base, (u32)host->idma_addr,
				0, SDC_IDMA_DESC_IN_DDR);

	return 0;

unmap_entries:
	dma_unmap_sg(mmc_dev(host->mmc), data->sg,
		data->sg_len, direction);
fail:
	return -EINVAL;
}

/* Sync cpu cache and umap the data buffers. */
static void lbsdc_idma_table_post(struct lbsdc_host *host,
	struct mmc_data *data)
{
	int direction;

	if (data->flags & MMC_DATA_READ)
		direction = DMA_FROM_DEVICE;
	else
		direction = DMA_TO_DEVICE;

#if 0
PRT_DBG("%s: opcode=%d,direction=%d\n", mmc_hostname(host->mmc),
	data->mrq->cmd->opcode, direction);
#endif

	dma_unmap_single(mmc_dev(host->mmc), host->idma_addr,
		LBSDC_MAX_DESC_CNT * IDMA_DESC_SIZE, DMA_TO_DEVICE);

	if (data->flags & MMC_DATA_READ) {
		/* after data is dma from FIFO to RAM buffers,
		 * we should sync the buffers for cpu cache. */
		dma_sync_sg_for_cpu(mmc_dev(host->mmc), data->sg,
			data->sg_len, direction);
	}

	dma_unmap_sg(mmc_dev(host->mmc), data->sg,
		data->sg_len, direction);
}

/* set proper timout value for data */
static u32 lbsdc_set_timeout(struct lbsdc_host *host, struct mmc_command *cmd)
{
	u32 count, long_step = 0;
	u32 timeout_clk;
	struct mmc_data *data = cmd->data;
	unsigned target_timeout;

	BUG_ON(!host->clock);

	timeout_clk = host->mmc->actual_clock ? host->mmc->actual_clock :
			host->clock;

	/* Unspecified timeout, assume 5s */
	/* Note: cmd_timeout_ms is set for erase command. */
	if (!data && !cmd->cmd_timeout_ms)
		target_timeout = 5000 * 1000;
	else if (!data) /* timeout in us */
		target_timeout = cmd->cmd_timeout_ms * 1000;
	else {
		target_timeout = data->timeout_ns / 1000;
		target_timeout += data->timeout_clks / timeout_clk;
	}

	/*
	 * Figure out needed cycles.
	 */
	count = (target_timeout / 1000) * (timeout_clk / 1000);

	if (count > SDC_MAX_DATA_TO_CNT) {
		long_step = 1;
		count /= SDC_LONG_TO_STEP_CNT;
		if (count > SDC_MAX_DATA_TO_CNT)
			count = SDC_MAX_DATA_TO_CNT;
	}

#if 0
PRT_DBG("%s: timeout_clk=%d,target_timeout=%d,count=%d",
	mmc_hostname(host->mmc), timeout_clk, target_timeout, count);
#endif

	csp_sdc_set_data_timeout(host->base, long_step, count);

	return count;
}

/* prepare data for transfer */
static void lbsdc_prepare_data(struct lbsdc_host *host,
		struct mmc_command *cmd)
{
	struct mmc_data *data = cmd->data;
	u32 burst_len = SDC_IDMA_BURST_1;
	int ret;

	/* Should not prepare data before last data finish. */
	if (WARN_ON(host->data)) {
		PRT_WARN("%s: last_cmd=%d,int_status=0x%08x\n",
			mmc_hostname(host->mmc), host->last_cmd,
			host->int_status);
	}

	if (data || (cmd->flags & MMC_RSP_BUSY))
		lbsdc_set_timeout(host, cmd);

	if (!data)
		return;

	/* Sanity checks */
	BUG_ON(data->blksz * data->blocks > LBSDC_MAX_DATA_SIZE);
	BUG_ON(data->blksz > host->mmc->max_blk_size);
	BUG_ON(data->blocks > LBSDC_MAX_BLOCK_CNT);

	host->data = data;
	host->data_early = 0;
	host->data->bytes_xfered = 0;

	if (host->flags & LBSDC_USE_IDMA) {
		host->flags |= LBSDC_REQ_USE_DMA;
		/* We should filled the IDMA descriptors table
		 * before IDMA transfer */
		ret = lbsdc_idma_table_pre(host, data);
		if (ret) {
			/*
			 * This only happens when someone fed
			 * us an invalid request.
			 */
			WARN_ON(1);
			/* fall back to PIO */
			host->flags &= ~LBSDC_REQ_USE_DMA;
		}
	}

	/*
	 * config the host for IDMA/PIO transfer
	 */

	/*
	 * set read/write FIFO watermark
	 */
	/* tx_th set to half FIFO depth, when FIFO empty space is lower than
	 *     this，TXDR request pending is raise,
	 *     controller's half FIFO depth is not less than block size,
	 *     so we can directly write a block to FIFO
	 * rx_th set to block_size, when the FIFO data reach not less than
	 *     this, RXDR request pending is raise,
	 *     we can directly read a block from FIFO
	 */
	csp_sdc_set_fifo_th(host->base, data->blksz / SDC_FIFO_WIDTH - 1,
			 SDC_FIFO_DEPTH / 2);

	/*
	 * set IDMA burst len
	 */
	/* IDMA's max burst len is 16, SDC_IDMA_WIDTH is 4
	 * it's always admitted that
	 *   TX_WM * SDC_FIFO_WIDTH >= burst_len * SDC_IDMA_WIDTH，
	 *   so we don't worry about FIFO overflow
	 * to avoid FIFO underflow, we should satify the following condition:
	 *  (RX_WM+1) * SDC_FIFO_WIDTH >= burst_len * SDC_IDMA_WIDTH
	 */
	if (data->blksz / SDC_IDMA_WIDTH >= 16)
		burst_len = SDC_IDMA_BURST_16;
	else if (data->blksz / SDC_IDMA_WIDTH >= 8)
		burst_len = SDC_IDMA_BURST_8;
	else if (data->blksz / SDC_IDMA_WIDTH >= 4)
		burst_len = SDC_IDMA_BURST_4;
	else
		burst_len = SDC_IDMA_BURST_1;
	csp_sdc_set_idma_burst(host->base, burst_len);

	/*
	 * set card threshold
	 */
	/* set card threshold as block size (unit in byte),
	 *   and enable read cth and write cth.
	 * guarantee that the clock won't be stopped during a block transaction
	 */
	csp_sdc_set_card_th(host->base, data->blksz, 1, 1);

	/*
	 * set the transfer mode
	 */
	csp_sdc_set_trans_mode(host->base,
		(host->flags & LBSDC_REQ_USE_DMA) ? SDC_TRANS_MODE_IDMA :
		SDC_TRANS_MODE_POLL);

	/*
	 * set the transfer byte
	 */
	csp_sdc_set_byte_cnt(host->base, data->blksz * data->blocks);

	/*
	 * set the transfer block size
	 */
	csp_sdc_set_blk_size(host->base, data->blksz);

	/* If we have to use PIO, enable the TXDR/RXDR interrupt */
	if ((host->flags & LBSDC_REQ_USE_DMA) == 0) {
		int flags;

		host->imask |= SDC_INT_PD_TXDR | SDC_INT_PD_RXDR;
		csp_sdc_enable_ints(host->base,
			SDC_INT_PD_TXDR | SDC_INT_PD_RXDR);

		flags = SG_MITER_ATOMIC;
		if (host->data->flags & MMC_DATA_READ)
			flags |= SG_MITER_TO_SG;
		else
			flags |= SG_MITER_FROM_SG;
		sg_miter_start(&host->sg_miter, data->sg, data->sg_len, flags);

		host->blocks = data->blocks;
	}
}

/* finish data */
static void lbsdc_finish_data(struct lbsdc_host *host)
{
	struct mmc_data *data;

	BUG_ON(!host->data);

#if 0
PRT_DBG("%s: opcode=%d\n", mmc_hostname(host->mmc),
	host->data->mrq->cmd->opcode);
#endif

	data = host->data;
	host->data = NULL;

	if (host->flags & LBSDC_REQ_USE_DMA)
		lbsdc_idma_table_post(host, data);

	/* We assume that nothing made it to the card in the event
	 * of an error. */
	if (data->error)
		data->bytes_xfered = 0;
	else
		data->bytes_xfered = data->blksz * data->blocks;

	/*
	 * Need to send CMD12 if -
	 * a) open-ended multiblock transfer (no CMD23)
	 * b) error in multiblock transfer
	 */
	if (data->stop &&
	    (data->error ||
	     !host->mrq->sbc)) {
		/*
		 * The controller needs a reset of internal state machines
		 * upon error conditions.
		 */
		if (data->error) {
			/* reset the whole controller */
			lbsdc_reset_all(host);
		}

		lbsdc_send_command(host, data->stop);
	} else {
#if 0
PRT_DBG("%s: opcode=%d\n", mmc_hostname(host->mmc), data->mrq->cmd->opcode);
#endif
		tasklet_schedule(&host->finish_tasklet);
	}
}

/* send mmc command */
static void lbsdc_send_command(struct lbsdc_host *host,
		struct mmc_command *cmd)
{
	u32 cconf = 0;

	if (WARN_ON(host->cmd)) {
		PRT_WARN("%s: last_cmd=%d,int_status=0x%08x\n",
			mmc_hostname(host->mmc), host->last_cmd,
			host->int_status);
	}

#ifdef CONFIG_ARCH_LOMBO_N7
	if (csp_sdc_chk_pd(host->base, SDC_INT_PD_HLWE))
		lbsdc_reset_all(host);
#endif

	/* Wait max 10s for hardware interrupts. */
	mod_timer(&host->timer, jiffies + 10 * HZ);

	host->cmd = cmd;

	if ((cmd->flags & MMC_RSP_136) && (cmd->flags & MMC_RSP_BUSY)) {
		PRT_DBG("%s: Unsupported response type!\n",
			mmc_hostname(host->mmc));
		cmd->error = -EINVAL;
		tasklet_schedule(&host->finish_tasklet);
		return;
	}

	if (cmd->flags & MMC_RSP_PRESENT)
		cconf |= SDC_CMD_RSP_EXPECT;

	if (cmd->flags & MMC_RSP_136)
		cconf |= SDC_CMD_RSP_LEN_LONG;

	if (cmd->flags & MMC_RSP_CRC)
		cconf |= SDC_CMD_CHECK_RSP_CRC;

	if (cmd->flags & MMC_RSP_BUSY)
		cconf |= SDC_CMD_BUSY_EXPT;

	if ((cmd->data != NULL) || (cmd->flags & MMC_RSP_BUSY))
		cconf |= SDC_CMD_WAIT_PRV_DATA_COM;

	if ((cmd->opcode == MMC_STOP_TRANSMISSION) ||
		(cmd->opcode == MMC_SEND_STATUS))
		cconf &= ~SDC_CMD_WAIT_PRV_DATA_COM;

	if (cmd->data) {
		cconf |= SDC_CMD_DATA_EXPECT;
		if (cmd->data->flags & MMC_DATA_WRITE)
			cconf |= SDC_CMD_TRANS_DIR_WRITE;
	}

	if (host->send_auto_stop)
		cconf |= SDC_CMD_SEND_AUTO_STOP;

	host->int_status = 0;

	/* expected cmd interrupt */
	host->imask = SDC_INT_ALL_CMD_ERR | SDC_INT_ALL_DATA_ERR |
		SDC_INT_PD_CMD_DONE;

	/* the data finish interrupt */
	if (cmd->flags & MMC_RSP_BUSY)
		host->imask |= SDC_INT_PD_BE;
	else if (cconf & SDC_CMD_DATA_EXPECT) {
		if (cconf & SDC_CMD_TRANS_DIR_WRITE) {
			/* we should wait busy end after all write data are
			 * transfered */
			host->imask |= SDC_INT_PD_BE;
		} else {
			host->imask |= SDC_INT_PD_DTF;
		}
	}

#if 0
PRT_INFO("%s: opcode=%d,arg=0x%08x,cconf=0x%08x\n",
	mmc_hostname(host->mmc), cmd->opcode, cmd->arg, cconf);

if (cmd->data)
	PRT_INFO("%s: data blksz=%d,blocks=%d\n",
		mmc_hostname(host->mmc), cmd->data->blksz, cmd->data->blocks);
#endif

	/* TODO */
	/* think more: whether CMD19's finish status is CMD_DONE. */
	/* other specical command: CMD11/CMD6 etc. */

	/* set irqs */
	/* csp_sdc_int_enable(host->base, 1); */
	csp_sdc_clr_pd(host->base, host->imask);
	csp_sdc_clear_set_ints(host->base,
		SDC_INT_ALL_MASK & ~SDC_INT_PD_SDIO0 , host->imask);

	/* here may enable TXDR/RXDR interrupt */
	lbsdc_prepare_data(host, cmd);

	csp_sdc_send_cmd(host->base, cmd->opcode, cmd->arg, cconf);
	host->last_cmd = cmd->opcode;
}

/* finish mmc command */
static void lbsdc_finish_command(struct lbsdc_host *host)
{
	BUG_ON(host->cmd == NULL);
#if 0
PRT_DBG("%s: opcode=%d\n", mmc_hostname(host->mmc), host->cmd->opcode);
#endif
	if (host->cmd->flags & MMC_RSP_PRESENT) {
		if (host->cmd->flags & MMC_RSP_136) {
			/* mmc core treat resp[0] as 96~127bit of long resp,
			 * but our controller's rsp0 register is 0~31bit,
			 * so we should reverse them here. */
			host->cmd->resp[0] = csp_sdc_get_rsp3(host->base);
			host->cmd->resp[1] = csp_sdc_get_rsp2(host->base);
			host->cmd->resp[2] = csp_sdc_get_rsp1(host->base);
			host->cmd->resp[3] = csp_sdc_get_rsp0(host->base);
		} else {
			host->cmd->resp[0] = csp_sdc_get_rsp0(host->base);
		}
	}

#if 0
	host->cmd->error = 0;
#endif

	/* Finished CMD23, now send actual command. */
	if (host->cmd == host->mrq->sbc) {
		host->cmd = NULL;
		lbsdc_send_command(host, host->mrq->cmd);
	} else {
		/* Processed actual command. */
		if (host->data && host->data_early)
			lbsdc_finish_data(host);

		if (!host->cmd->data)
			tasklet_schedule(&host->finish_tasklet);
		else
			host->cmd = NULL;
	}
}

/* choose best SDC module clock rate for the clock rate that expected */
#if (!(defined CONFIG_ARCH_LOMBO_N7V0_FPGA) &&	\
	!(defined CONFIG_ARCH_LOMBO_N7V1_FPGA) &&	\
	!(defined CONFIG_ARCH_LOMBO_N9V0_FPGA))
static void lbsdc_set_clock(struct lbsdc_host *host, u32 clock)
{
	u32 src = 0;
	u32 proper_src = 0;
	u32 proper_prate = 0;
	long prate = 0;

	static struct clk *clk_null;

	u32 ret = 0;

	PRT_DBG("clock=%d,host->clock=%d,host->mmc->actual_clock=%d\n",
		clock, host->clock, host->mmc->actual_clock);

	if (clock && (clock == host->clock))
		return;

	host->mmc->actual_clock = 0;

	if (clock == 0) {
		PRT_DBG("disable clock\n");
		if (host->clock != 0) {
			clk_null = clk_get(NULL, "null_clk");
			clk_set_parent(host->clk_sdc, clk_null);
			clk_put(clk_null);
			/* clk_disable(host->clk_sdc); */
			host->clock = 0;
		}
		return;
	}

	if (host->clock == 0) {
		PRT_DBG("enable clock\n");
		/* clk_enable(host->clk_sdc); */
	}

	/*
	 * set host clock with card clock
	 */
	clk_disable(host->clk_sdc);
	/* select proper parent */
	for (src = 0; src < host->clk_parents_cnt; src++) {
		PRT_DBG("src=%d,clk=0x%p\n", src, host->clk_parents[src]);
		prate = clk_get_rate(host->clk_parents[src]);
		if (clock * 2 == prate) {
			proper_src = src;
			break;
		}

		if ((proper_prate == 0) ||
		    ((prate < proper_prate) && (clock * 2 < prate)) ||
		    ((prate > proper_prate) && (clock * 2 > proper_prate))) {
			proper_src = src;
			proper_prate = prate;
		}
	}

	PRT_DBG("proper_src=%d\n", proper_src);
	ret = clk_set_parent(host->clk_sdc, host->clk_parents[proper_src]);
	if (ret != 0) {
		PRT_WARN("%s: can't set clk parent %d\n",
			mmc_hostname(host->mmc), proper_src);
		goto out;
	}

	/* the sdc working clock rate is half of the clk rate from PRCM. */
	ret = clk_set_rate(host->clk_sdc, clock * 2);
	if (ret != 0)
		PRT_WARN("%s: failed\n", mmc_hostname(host->mmc));

out:
	clk_enable(host->clk_sdc);
	host->mmc->actual_clock = clk_get_rate(host->clk_sdc) / 2;
	PRT_DBG("actual_clock=%d\n", host->mmc->actual_clock);
	host->clock = clock;
}
#else
static void lbsdc_set_clock(struct lbsdc_host *host, u32 clock)
{
	struct clk *clk_parent_null = NULL;
	u32 ret = 0;

	if (clock && (clock == host->clock))
		return;

	host->mmc->actual_clock = 0;

	if (0 == clock)
		return;

	clk_disable(host->clk_sdc);

	ret = clk_set_parent(host->clk_sdc, host->clk_parents[0]);
	if (ret != 0) {
		PRT_WARN("%s: can't set clk parent, clock=%d\n",
			mmc_hostname(host->mmc), clock);
		goto out;
	}

	ret = clk_set_rate(host->clk_sdc, 24000000);
	if (ret != 0) {
		PRT_WARN("%s: failed\n", mmc_hostname(host->mmc));
		goto out;
	}

	PRT_DBG("%s: sdc clock rate for FPGA is %lu (actual 400KHz)\n",
		mmc_hostname(host->mmc), clk_get_rate(host->clk_sdc));
	host->mmc->actual_clock = 400000 / 2;

	if (clock > 400000) {
		clk_parent_null = clk_get(NULL, "null_clk");
		if (IS_ERR_OR_NULL(clk_parent_null)) {
			PRT_ERR("%s: failed to get sdc null clock parent!\n",
				mmc_hostname(host->mmc));
			ret = PTR_ERR(clk_parent_null);
		}

		ret = clk_set_parent(host->clk_sdc, clk_parent_null);
		if (ret != 0) {
			PRT_WARN("%s: can't get set parent \"%s\"\n",
				mmc_hostname(host->mmc), "null_clk");
		}

		clk_put(clk_parent_null);

		PRT_DBG("%s: sdc clock rate for FPGA is %lu (actual 24MHz)\n",
			mmc_hostname(host->mmc), clk_get_rate(host->clk_sdc));
		host->mmc->actual_clock = 24000000 / 2;
	}

out:
	clk_enable(host->clk_sdc);
	host->clock = clock;
}
#endif

/* get maximum SDC module clock rate */
static u32 lbsdc_get_max_clock(struct lbsdc_host *host)
{
	long clk = 0;
	long max_clk = 0;
	u32 src = 0;

	for (src = 0; src < host->clk_parents_cnt; src++) {
		clk = clk_get_rate(host->clk_parents[src]);

		if (clk > max_clk)
			max_clk = clk;
	}

	return max_clk / 2;
}

/* set the card working power */
static void lbsdc_set_power(struct lbsdc_host *host, unsigned char mode,
			    unsigned short vdd)
{
#if 0
	struct mmc_host *mmc = host->mmc;

	if (!IS_ERR(mmc->supply.vmmc)) {
		spin_unlock_irq(&host->lock);
		mmc_regulator_set_ocr(mmc, mmc->supply.vmmc,
			(mode != MMC_POWER_OFF) ? vdd : 0);
		spin_lock_irq(&host->lock);
		return;
	}
#endif

	if (mode == MMC_POWER_OFF) {
		/* modify the drive strength of sdc pins */
		if (host->default_pinctrl && host->default_state) {
			pinctrl_select_state(host->default_pinctrl,
						host->default_state);
		} else
			PRT_ERR("can't set mmc:%u default_pinctrl",
				host->mmc->index);
	}

#if 1
	/* just for sd card should do this */
	if (-1 == host->pwr_gpio)
		return;

	if (mode == MMC_POWER_OFF)
		gpio_set_value(host->pwr_gpio, 0);
	else if (mode == MMC_POWER_UP)
		gpio_set_value(host->pwr_gpio, 1);
#endif
}

/*****************************************************************************\
 *                                                                           *
 * MMC callbacks                                                             *
 *                                                                           *
\*****************************************************************************/

/* start handling the mmc request */
static void lbsdc_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct lbsdc_host *host = mmc_priv(mmc);
	unsigned long flags;
	u32 present;

	lbsdc_runtime_pm_get(host);

	/* Firstly check card presence */
	present = lbsdc_do_get_cd(host);

	spin_lock_irqsave(&host->lock, flags);

	if (WARN_ON(host->mrq != NULL)) {
		PRT_WARN("%s: last_cmd=%d,int_status=0x%08x\n",
			mmc_hostname(host->mmc), host->last_cmd,
			host->int_status);
	}

	host->send_auto_stop = 0;

	/*
	 * Ensure we don't send the STOP for non-SET_BLOCK_COUNTED
	 * requests if Auto-CMD12 is enabled.
	 */
	if (!mrq->sbc && (host->flags & LBSDC_AUTO_CMD12)) {
		if (mrq->stop) {
			host->send_auto_stop = 1;
			mrq->data->stop = NULL;
			mrq->stop = NULL;
		}
	}

	host->mrq = mrq;

	/* In voltage switch procedure, we need control the clock to the card */
	if (!(host->flags & LBSDC_LP_DISABLED)) {
		if (mrq->cmd->opcode == SD_SWITCH_VOLTAGE)
			lbsdc_enable_clk_lp(host, 0);
		else if (host->last_cmd == SD_SWITCH_VOLTAGE)
			lbsdc_enable_clk_lp(host, 1);
	}

	if (!present  || (host->flags & LBSDC_DEVICE_DEAD)) {
		host->mrq->cmd->error = -ENOMEDIUM;
		tasklet_schedule(&host->finish_tasklet);
	} else {
		if (mrq->cmd)
			mrq->cmd->error = 0;

		if (mrq->stop)
			mrq->stop->error = 0;

		if (mrq->data)
			mrq->data->error = 0;

		if (mrq->sbc) {
			mrq->sbc->error = 0;
			lbsdc_send_command(host, mrq->sbc);
		} else {
			lbsdc_send_command(host, mrq->cmd);
		}
	}

	mmiowb();
	spin_unlock_irqrestore(&host->lock, flags);
}

/* config for signal timing */
static void lbsdc_set_timing(struct lbsdc_host *host, unsigned timing)
{
#if 0
	/* TODO: set proper tx-phase-shift, rx-phase-shift, rx-delay-chain */
	if ((timing == MMC_TIMING_MMC_HS200) ||
	    (timing == MMC_TIMING_UHS_SDR104)) {
		/* do someting */
	} else if (timing == MMC_TIMING_UHS_SDR12) {
		/* do someting */
	} else if (timing == MMC_TIMING_UHS_SDR25) {
		/* do someting */
	} else if (timing == MMC_TIMING_UHS_SDR50) {
		/* do someting */
	} else if (timing == MMC_TIMING_UHS_DDR50) {
		/* do someting */
	}
#endif
}

/* config for driver type */
static void lbsdc_set_drv_type(struct lbsdc_host *host, unsigned drv_type)
{
	/* TODO: config the pin driver strength according drv type */
}

static void lbsdc_do_set_ios(struct lbsdc_host *host, struct mmc_ios *ios)
{
	unsigned long flags;
#if 0
	struct mmc_host *mmc = host->mmc;
#endif
	u32 sdc_bus_width = SDC_BUS_WIDTH_1;

	spin_lock_irqsave(&host->lock, flags);

	if (host->flags & LBSDC_DEVICE_DEAD) {
		spin_unlock_irqrestore(&host->lock, flags);
#if 0
		if (!IS_ERR(mmc->supply.vmmc) &&
		    ios->power_mode == MMC_POWER_OFF)
			mmc_regulator_set_ocr(mmc, mmc->supply.vmmc, 0);
#endif
		return;
	}

	/* ios->power_mode, MMC_POWER_OFF/MMC_POWER_UP */
	/*
	 * Reset the chip on each power off.
	 * Should clear out any weird states.
	 */
	if ((ios->power_mode == MMC_POWER_OFF) && !host->runtime_suspended)
		lbsdc_reset_all(host);

	spin_unlock_irqrestore(&host->lock, flags);
	/* ios->clock */
	lbsdc_set_clock(host, ios->clock);
	spin_lock_irqsave(&host->lock, flags);

	/* ios->vdd, power */
	lbsdc_set_power(host, ios->power_mode, ios->vdd);

	/* ios->bus_width, MMC_BUS_WIDTH_8, MMC_BUS_WIDTH_4, MMC_BUS_WIDTH_1 */
	if (ios->bus_width == MMC_BUS_WIDTH_8)
		sdc_bus_width = SDC_BUS_WIDTH_8;
	else if (ios->bus_width == MMC_BUS_WIDTH_4)
		sdc_bus_width = SDC_BUS_WIDTH_4;
	else
		sdc_bus_width = SDC_BUS_WIDTH_1;
	csp_sdc_set_bus_width(host->base, sdc_bus_width);
	host->bus_width = sdc_bus_width;

	/* ios->timing
	 * MMC_TIMING_LEGACY, MMC_TIMING_MMC_HS, MMC_TIMING_SD_HS
	 * MMC_TIMING_UHS_SDR12, MMC_TIMING_UHS_SDR25, MMC_TIMING_UHS_SDR50
	 * MMC_TIMING_UHS_SDR104, MMC_TIMING_UHS_DDR50
	 * set tx-phase-shift, rx-phase-shift, rx-delay-chain
	 */
	lbsdc_set_timing(host, ios->timing);

	/* ios->drv_type
	 * MMC_SET_DRIVER_TYPE_B, MMC_SET_DRIVER_TYPE_A
	 * MMC_SET_DRIVER_TYPE_C, MMC_SET_DRIVER_TYPE_D
	 */
	lbsdc_set_drv_type(host, ios->drv_type);

	mmiowb();
	spin_unlock_irqrestore(&host->lock, flags);
}

static void lbsdc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct lbsdc_host *host = mmc_priv(mmc);

	lbsdc_runtime_pm_get(host);
	lbsdc_do_set_ios(host, ios);
	lbsdc_runtime_pm_put(host);
}

/*
 * card detect
 * sdc internal card detect function is removed,
 * maybe has ext card detect gpio, maybe not
*/
static int lbsdc_do_get_cd(struct lbsdc_host *host)
{
	int gpio_cd;

	if (host->flags & LBSDC_DEVICE_DEAD)
		return 0;

	if (sdio_host == host) {
		/* already enter scan, and not to scan anymore */
		if (host->pw_state)
			host->mmc->caps |= MMC_CAP_NONREMOVABLE;
		else
			host->mmc->caps &= ~MMC_CAP_NONREMOVABLE;
		return !!host->pw_state;
	}

	/* If nonremovable, assume that the card is always present. */
	if (host->mmc->caps & MMC_CAP_NONREMOVABLE)
		return 1;

	gpio_cd = mmc_gpio_get_cd(host->mmc);
	/*
	 * Try slot gpio detect
	 */
	if (!IS_ERR_VALUE(gpio_cd))
		return !!gpio_cd;

	/* no cd-gpio */
	/* assume that the card is always present. */
	return 1;
}

static int lbsdc_get_cd(struct mmc_host *mmc)
{
	struct lbsdc_host *host = mmc_priv(mmc);
	int ret;

	lbsdc_runtime_pm_get(host);
	ret = lbsdc_do_get_cd(host);
	lbsdc_runtime_pm_put(host);
	return ret;
}

#ifdef LBSDC_ENABLE_RO_DETECT
static int lbsdc_do_get_ro(struct lbsdc_host *host)
{
	int gpio_ro;

	if (host->flags & LBSDC_DEVICE_DEAD)
		return 0;

	gpio_ro = mmc_gpio_get_ro(host->mmc);

	/* If nonremovable, assume that the card is always present. */
	if (host->mmc->caps & MMC_CAP_NONREMOVABLE)
		return 1;

	/*
	 * Try slot gpio readonly detect
	 */
	if (!IS_ERR_VALUE(gpio_ro))
		return !!gpio_ro;

	/* no ro-gpio, always not readonly. */
	return 0;
}

/* check whether card is readonly through ro-gpio. */
static int lbsdc_get_ro(struct mmc_host *mmc)
{
	struct lbsdc_host *host = mmc_priv(mmc);
	int ret;

	lbsdc_runtime_pm_get(host);
	ret = lbsdc_do_get_ro(host);
	lbsdc_runtime_pm_put(host);
	return ret;
}
#endif

static void lbsdc_enable_sdio_irq_nolock(struct lbsdc_host *host, int enable)
{
	if (host->flags & LBSDC_DEVICE_DEAD)
		goto out;

	if (enable)
		host->flags |= LBSDC_SDIO_IRQ_ENABLED;
	else
		host->flags &= ~LBSDC_SDIO_IRQ_ENABLED;

	/* SDIO IRQ will be enabled as appropriate in runtime resume */
	if (host->runtime_suspended)
		goto out;

	if (enable)
		csp_sdc_enable_ints(host->base, SDC_INT_PD_SDIO0);
	else
		csp_sdc_disable_ints(host->base, SDC_INT_PD_SDIO0);

	csp_sdc_clr_pd(host->base, SDC_INT_PD_SDIO0);

out:
	mmiowb();

}

/* enable/disable the SDIO IRQ */
static void lbsdc_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	struct lbsdc_host *host = mmc_priv(mmc);
	unsigned long flags;

	lbsdc_runtime_pm_get(host);

	spin_lock_irqsave(&host->lock, flags);
	lbsdc_enable_sdio_irq_nolock(host, enable);
	spin_unlock_irqrestore(&host->lock, flags);

	lbsdc_runtime_pm_put(host);
}

#if 0
static int lbsdc_do_start_signal_voltage_switch(struct lbsdc_host *host,
		struct mmc_ios *ios)
{
	struct mmc_host *mmc = host->mmc;
	int ret;

	switch (ios->signal_voltage) {
	case MMC_SIGNAL_VOLTAGE_330:
		csp_sdc_set_voltage(host->base, SDC_VOLT_3_3);
		if (!IS_ERR(mmc->supply.vqmmc)) {
			ret = regulator_set_voltage(mmc->supply.vqmmc, 2700000,
						    3600000);
			if (ret) {
				PRT_WARN("%s: Switching to 3.3V %s.\n",
					mmc_hostname(host->mmc),
					"signaling voltage failed");
				return -EIO;
			}
		}

		/* TODO:
		 * check whether 3.3V regulator output is stable within 5 ms */

		return 0;
#if 0
		PRT_WARN("%s: 3.3V regulator output did not became stable\n",
			mmc_hostname(host->mmc));

		return -EAGAIN;
#endif
	case MMC_SIGNAL_VOLTAGE_180:
		if (!IS_ERR(mmc->supply.vqmmc)) {
			ret = regulator_set_voltage(mmc->supply.vqmmc,
					1700000, 1950000);
			if (ret) {
				PRT_WARN("%s: Switching to 1.8V %s.\n",
					mmc_hostname(host->mmc),
					"signaling voltage failed");
				return -EIO;
			}
		}
		csp_sdc_set_voltage(host->base, SDC_VOLT_1_8);

		/* TODO:
		 * check whether 1.8V regulator output is stable within 5 ms */

		return 0;
#if 0
		PRT_WARN("%s: 1.8V regulator output did not became stable\n",
			mmc_hostname(host->mmc));

		return -EAGAIN;
#endif
	default:
		/* MMC_SIGNAL_VOLTAGE_120 is not supported. */
		/* No signal voltage switch required */
		return 0;
	}
}
#else
static int lbsdc_do_start_signal_voltage_switch(struct lbsdc_host *host,
		struct mmc_ios *ios)
{
	switch (ios->signal_voltage) {
	case MMC_SIGNAL_VOLTAGE_330:
		/* modify the drive strength of sdc pins */
		if (host->default_pinctrl && host->default_state) {
			pinctrl_select_state(host->default_pinctrl,
						host->default_state);
		} else
			PRT_ERR("can't set mmc:%u default_pinctrl",
				host->mmc->index);

		csp_sdc_set_voltage(host->base, SDC_VOLT_3_3);
		/* choose proper sau ldo according the board design */
		csp_sau_cfg_1v8_ldo(host->sidx, 0);

		break;
	case MMC_SIGNAL_VOLTAGE_180:
		/* modify the drive strength of sdc pins */
		if (host->uhs_pinctrl && host->uhs_state) {
			pinctrl_select_state(host->uhs_pinctrl,
						host->uhs_state);
		} else
			PRT_ERR("can't set mmc:%u uhs_pinctrl",
						host->mmc->index);

		csp_sdc_set_voltage(host->base, SDC_VOLT_1_8);
		/* choose proper sau ldo according the board design */
		csp_sau_cfg_1v8_ldo(host->sidx, 1);
		break;
	default:
		/* MMC_SIGNAL_VOLTAGE_120 is not supported. */
		/* No signal voltage switch required */
		break;
	}

	return 0;
}
#endif

/* switch the signal voltage to expected level. */
static int lbsdc_start_signal_voltage_switch(struct mmc_host *mmc,
		struct mmc_ios *ios)
{
	struct lbsdc_host *host = mmc_priv(mmc);
	int err;

	lbsdc_runtime_pm_get(host);
	err = lbsdc_do_start_signal_voltage_switch(host, ios);
	lbsdc_runtime_pm_put(host);
	return err;
}

/* check whether card is busy */
static int lbsdc_card_busy(struct mmc_host *mmc)
{
	struct lbsdc_host *host = mmc_priv(mmc);
	u32 sdc_status;

	lbsdc_runtime_pm_get(host);
	/* Check whether DAT[3:0] is 0000 */
	sdc_status = csp_sdc_get_status(host->base);
	PRT_DBG("sdc_status=0x%08x\n", sdc_status);
	lbsdc_runtime_pm_put(host);

	return !(sdc_status & SDC_DATA_LVL_MASK);
}

#define SDC_CALIBRATE_TSET_CYCLE	10
static int lbsdc_calibrate(struct lbsdc_host *host)
{
	u32 clbr_res = 0;
	u32 cycle = 0;
	u32 res = 0;

	for (cycle = 0; cycle < SDC_CALIBRATE_TSET_CYCLE; cycle++) {
		csp_sdc_clbr_start(host->base);
		while (1) {
			if (csp_sdc_is_clbr_done(host->base))
				break;
		}

		if (csp_sdc_is_clbr_locked(host->base)) {
			clbr_res = csp_sdc_get_clbr_status(
						host->base
				   );
		} else
			clbr_res = 0x3f;

		res += clbr_res;
		PRT_DBG("sdc calibrate round %d,val %d\n", cycle, clbr_res);
	}

	res = res / SDC_CALIBRATE_TSET_CYCLE;
	PRT_DBG("sdc calibrate result %d\n", res);

	return res;
}

#define SD_TUNING_BLK_LEN		64
const u32 sd_tuning_blk[SD_TUNING_BLK_LEN / 4] = {
	0X00FF0FFF, 0XCCC3CCFF, 0XFFCC3CC3, 0XEFFEFFFE,
	0XDDFFDFFF, 0XFBFFFBFF, 0XFF7FFFBF, 0XEFBDF777,
	0XF0FFF0FF, 0X3CCCFC0F, 0XCFCC33CC, 0XEEFFEFFF,
	0XFDFFFDFF, 0XFFBFFFDF, 0XFFF7FFBB, 0XDE7B7FF7,
};

#define MMC_8BIT_TUNING_BLOCK_SIZE	128
const u32 mmc_8bit_tuning_blk[MMC_8BIT_TUNING_BLOCK_SIZE / 4] = {
	0xFF00FFFF, 0x0000FFFF, 0xCCCCFFFF, 0xCCCC33CC,
	0xCC3333CC, 0xFFFFCCCC, 0xFFFFEEFF, 0xFFEEEEFF,
	0xFFDDFFFF, 0xDDDDFFFF, 0xBBFFFFFF, 0xBBFFFFFF,
	0xFFFFFFBB, 0xFFFFFF77, 0x77FF7777, 0xFFEEDDBB,

	0x00FFFFFF, 0x00FFFFFF, 0xCCFFFF00, 0xCC33CCCC,
	0x3333CCCC, 0xFFCCCCCC, 0xFFEEFFFF, 0xEEEEFFFF,
	0xDDFFFFFF, 0xDDFFFFFF, 0xFFFFFFDD, 0xFFFFFFBB,
	0xFFFFBBBB, 0xFFFF77FF, 0xFF7777FF, 0xEEDDBB77,
};

static int lbsdc_send_tuning_cmd(struct lbsdc_host *host, u32 opcode,
				void *tuning_data, const u32 *tuning_blk,
				u32 tuning_blksz)
{
	struct mmc_request mrq = {NULL};
	struct mmc_command cmd = {0};
	struct mmc_data data = {0};
	int err = 0;

	struct scatterlist sg;

	cmd.opcode = opcode;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
	cmd.retries = 0;

	data.blksz = tuning_blksz;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;
	data.sg = &sg;
	data.sg_len = 1;
	/* 100ms timeout value */
	data.timeout_ns = 100000000;

	mrq.cmd = &cmd;
	mrq.data = &data;

	sg_init_one(&sg, tuning_data, tuning_blksz);

	mmc_wait_for_req(host->mmc, &mrq);

	if ((cmd.error != 0) || (data.error != 0)) {
		err = -1;
		goto out;
	}

	err = memcmp(tuning_blk, tuning_data, tuning_blksz);

out:
	return err;
}

/* execute tuning sequence */
static int lbsdc_execute_tuning(struct mmc_host *mmc, u32 opcode)
{
	struct lbsdc_host *host = mmc_priv(mmc);
	void *tuning_data = NULL;
	const u32 *tuning_blk = NULL;
	u32 tuning_blksz = 0;
	int window_start = -1;
	int window_end = -1;
	u32 window_len = 0;
	u32 max_window_len = 0;
	u32 header_winl = 0;
	u32 rx_dly = 0;
	u32 max_rx_dly = 0;
	int best_rx_dly = -1;
	int err = 0;

	PRT_INFO("begin\n");
	switch (opcode) {
	case MMC_SEND_TUNING_BLOCK:
		tuning_blk = sd_tuning_blk;
		tuning_blksz = SD_TUNING_BLK_LEN;
		break;
	case MMC_SEND_TUNING_BLOCK_HS200:
		if (host->bus_width == SDC_BUS_WIDTH_8) {
			tuning_blk = mmc_8bit_tuning_blk;
			tuning_blksz = MMC_8BIT_TUNING_BLOCK_SIZE;
		} else {
			tuning_blk = sd_tuning_blk;
			tuning_blksz = SD_TUNING_BLK_LEN;
		}
		break;
	default:
		PRT_ERR("Unsupported tuning command (%d)\n", opcode);
		return -1;
	}

	tuning_data = kmalloc(tuning_blksz, GFP_KERNEL);
	if (IS_ERR_OR_NULL(tuning_data)) {
		PRT_ERR("Can't alloc memory for tuning data.\n");
		return -ENOMEM;
	}

	lbsdc_runtime_pm_get(host);
	disable_irq(host->irq);
	spin_lock(&host->lock);
	max_rx_dly = lbsdc_calibrate(host);
	spin_unlock(&host->lock);
	enable_irq(host->irq);

	rx_dly = 0;
	do {
		PRT_DBG("tuning delay %d start\n", rx_dly);

		disable_irq(host->irq);
		spin_lock(&host->lock);
		csp_sdc_set_rx_dly(host->base, rx_dly);
		spin_unlock(&host->lock);
		enable_irq(host->irq);

		err = lbsdc_send_tuning_cmd(host, opcode, tuning_data,
					tuning_blk, tuning_blksz);
		if (err != 0) {
			PRT_DBG("tuning failed, rx_dly=%d\n", rx_dly);
			if (window_start >= 0) {
				PRT_INFO("window [%d, %d]\n",
					window_start, window_end);
				window_len = window_end - window_start + 1;
				if (window_start == 0)
					header_winl = window_len;
				if (window_len > max_window_len) {
					max_window_len = window_len;
					best_rx_dly =
						(window_end + window_start) / 2;
				}
			}

			window_start = -1;
			window_end = -1;
			rx_dly++;
			continue;
		}

		PRT_DBG("tuning pass, rx_dly=%d\n", rx_dly);
		if (window_start < 0)
			window_start = rx_dly;
		window_end = rx_dly;

		rx_dly++;
	} while (rx_dly <= max_rx_dly);

	if (window_end == max_rx_dly) {
		PRT_INFO("window [%d, %d]\n", window_start, rx_dly - 1);
		window_len = window_end - window_start + 1 + header_winl;
		if (window_len > max_window_len) {
			max_window_len = window_len;
			best_rx_dly = (window_start + window_len / 2) %
					max_rx_dly;
		}

		if (window_len < max_rx_dly / 2) {
			PRT_WARN("tuning window maybe too small!\n");
			PRT_WARN("window_len=%d,max_rx_dly=%d\n",
				 window_len, max_rx_dly);
		}
	}

	err = 0;
	if (best_rx_dly < 0) {
		PRT_DBG("no proper rx delay point found, tuning failed!\n");
		err = -EIO;
	}
	PRT_DBG("best_rx_dly=%d\n", best_rx_dly);

	disable_irq(host->irq);
	spin_lock(&host->lock);
	csp_sdc_set_rx_dly(host->base, (u32)best_rx_dly);
	spin_unlock(&host->lock);
	enable_irq(host->irq);
	lbsdc_runtime_pm_put(host);

	kfree(tuning_data);
	PRT_INFO("end\n");

	return err;
}

/* action for card detect interrupt occur */
static void lbsdc_card_event(struct mmc_host *mmc)
{
	struct lbsdc_host *host = mmc_priv(mmc);
	unsigned long flags;
	int present;

	present = lbsdc_do_get_cd(host);

	spin_lock_irqsave(&host->lock, flags);

	/* Check host->mrq first in case we are runtime suspended */
	if (host->mrq && !present) {
		PRT_ERR("%s: Card removed during transfer!\n",
			mmc_hostname(mmc));
		PRT_ERR("%s: Resetting controller.\n",
			mmc_hostname(mmc));

		/* reset the sdc */
		lbsdc_reset_all(host);

		host->mrq->cmd->error = -ENOMEDIUM;
		tasklet_schedule(&host->finish_tasklet);
	}

	spin_unlock_irqrestore(&host->lock, flags);
}

static void lbsdc_hw_reset(struct mmc_host *mmc)
{
	struct lbsdc_host *host = mmc_priv(mmc);

	csp_sdc_disable_ints(host->base, SDC_INT_ALL_MASK);

	clk_disable(host->clk_reset);
	clk_enable(host->clk_reset);

	lbsdc_reset_all(host);
	csp_sdc_set_init_seq(host->base, 80);
	csp_sdc_int_enable(host->base, 1);

	pinctrl_select_state(host->default_pinctrl,
				host->default_state);
}

static const struct mmc_host_ops lbsdc_ops = {
	.request	= lbsdc_request,
	.set_ios	= lbsdc_set_ios,
	.get_cd		= lbsdc_get_cd,
#ifdef LBSDC_ENABLE_RO_DETECT
	.get_ro		= lbsdc_get_ro,
#endif
	.hw_reset	= lbsdc_hw_reset,

	.enable_sdio_irq = lbsdc_enable_sdio_irq,
	.start_signal_voltage_switch	= lbsdc_start_signal_voltage_switch,
	.execute_tuning			= lbsdc_execute_tuning,
	.card_event			= lbsdc_card_event,
	.card_busy	= lbsdc_card_busy,
};

/*****************************************************************************\
 *                                                                           *
 * Tasklets                                                                  *
 *                                                                           *
\*****************************************************************************/

/* finish the mrq */
static void lbsdc_tasklet_finish(unsigned long param)
{
	struct lbsdc_host *host;
	unsigned long flags;
	struct mmc_request *mrq;

	host = (struct lbsdc_host *)param;

#if 0
PRT_DBG("%s\n", mmc_hostname(host->mmc));
#endif

	spin_lock_irqsave(&host->lock, flags);
	/*
	 * If this tasklet gets rescheduled while running, it will
	 * be run again afterwards but without any active request.
	 */
	if (!host->mrq) {
		spin_unlock_irqrestore(&host->lock, flags);
		return;
	}

	del_timer(&host->timer);

	mrq = host->mrq;

	/*
	 * The controller needs a reset of internal state machines
	 * upon error conditions.
	 */
	if (!(host->flags & LBSDC_DEVICE_DEAD) &&
	    ((mrq->cmd && mrq->cmd->error) ||
	     (mrq->sbc && mrq->sbc->error) ||
	     (mrq->data && ((mrq->data->error && !mrq->data->stop) ||
			    (mrq->data->stop && mrq->data->stop->error))))) {
		/* reset the sdc */
		lbsdc_reset_all(host);
	}

	if ((mrq->data != NULL) && (mrq->data->error == -EILSEQ)) {
		switch (mrq->cmd->opcode) {
		case MMC_WRITE_BLOCK:
		case MMC_WRITE_MULTIPLE_BLOCK:
		case MMC_READ_SINGLE_BLOCK:
		case MMC_READ_MULTIPLE_BLOCK:
			if (mrq->cmd->retries == 0) {
				/* set cmd error to trigger retry */
				mrq->cmd->error = -EILSEQ;
				mrq->cmd->retries = LBSDC_TRANS_RETRIES + 1;
			} else if (mrq->cmd->retries == 1) {
				mrq->cmd->retries = 0;
				PRT_INFO("retry=%d\n", LBSDC_TRANS_RETRIES);
			} else {
				PRT_INFO("retry=%d\n",
					LBSDC_TRANS_RETRIES -
					mrq->cmd->retries + 1);
				/* set cmd error to trigger retry */
				mrq->cmd->error = -EILSEQ;
			}
			break;
		default:
			break;
		}
	}

	host->mrq = NULL;
	host->cmd = NULL;
	host->data = NULL;

	mmiowb();
	spin_unlock_irqrestore(&host->lock, flags);

	mmc_request_done(host->mmc, mrq);
	lbsdc_runtime_pm_put(host);
}

/* do things when waiting for hardware interrupts timeout. */
static void lbsdc_timeout_timer(unsigned long data)
{
	struct lbsdc_host *host;
	unsigned long flags;

	host = (struct lbsdc_host *)data;

	spin_lock_irqsave(&host->lock, flags);

	if (host->mrq) {
		PRT_ERR("%s: Timeout waiting for hardware interrupt.\n",
			mmc_hostname(host->mmc));
		PRT_ERR("%s: last_cmd=%d,int_status=0x%08x\n",
			mmc_hostname(host->mmc), host->last_cmd,
			host->int_status);
		csp_sdc_dumpregs(host->base, mmc_hostname(host->mmc));

		if (host->data) {
			host->data->error = -ETIMEDOUT;
			lbsdc_finish_data(host);
		} else {
			if (host->cmd)
				host->cmd->error = -ETIMEDOUT;
			else
				host->mrq->cmd->error = -ETIMEDOUT;

			tasklet_schedule(&host->finish_tasklet);
		}
	}

	mmiowb();
	spin_unlock_irqrestore(&host->lock, flags);
}

/*****************************************************************************\
 *                                                                           *
 * Interrupt handling                                                        *
 *                                                                           *
\*****************************************************************************/

/* action for the command related status. */
static void lbsdc_cmd_irq(struct lbsdc_host *host, u32 int_status)
{
	BUG_ON(int_status == 0);

#if 0
PRT_DBG("%s: int_status=0x%08x.\n", mmc_hostname(host->mmc), int_status);
#endif

	if (!host->cmd) {
		PRT_ERR("%s: Got command interrupt 0x%08x even though %s.\n",
			mmc_hostname(host->mmc), int_status,
			"no command operation was in progress");
		PRT_ERR("%s: last_cmd=%d,int_status=0x%08x\n",
			mmc_hostname(host->mmc), host->last_cmd,
			host->int_status);
		csp_sdc_dumpregs(host->base, mmc_hostname(host->mmc));
		return;
	}

	if (int_status & SDC_INT_PD_RTO) {
		PRT_DBG("%s: Response Timeout(RTO)! opcode=%d\n",
			mmc_hostname(host->mmc), host->cmd->opcode);
		host->cmd->error = -ETIMEDOUT;
	}

	if (int_status & SDC_INT_PD_RSP_ERR) {
		PRT_ERR("%s: Response Error(RSP_ERR)! opcode=%d\n",
			mmc_hostname(host->mmc), host->cmd->opcode);
		host->cmd->error = -EILSEQ;
	}

	if (int_status & SDC_INT_PD_CRC7E) {
		PRT_ERR("%s: Response CRC7 Error(CRC7E)! opcode=%d\n",
			mmc_hostname(host->mmc), host->cmd->opcode);
		host->cmd->error = -EILSEQ;
	}

	if (host->cmd->error) {
		PRT_DBG("host->int_status=0x%8x\n", host->int_status);
		/* error catched, disable all interrupts */
		csp_sdc_disable_ints(host->base, host->imask);
		csp_sdc_dumpregs(host->base, mmc_hostname(host->mmc));
		tasklet_schedule(&host->finish_tasklet);
		return;
	}

	/*
	 * for command that with busy fallow, is finished by SDC_INT_PD_BE,
	 * which is handle in lbsdc_data_irq()
	 */
	if ((host->cmd->flags & MMC_RSP_BUSY) &&
		((int_status & SDC_INT_DATA_MASK) ||
		!(host->int_status & SDC_INT_PD_BE)))
		return;

	/* CMD_DONE means command transfer finished. */
	if (int_status & SDC_INT_PD_CMD_DONE)
		lbsdc_finish_command(host);
}

static void lbsdc_transfer_finish(struct lbsdc_host *host)
{
	u32 dfi = 0;

	if (((host->flags & LBSDC_REQ_USE_DMA) == 0) &&
		(host->data->flags == MMC_DATA_READ) &&
		(host->blocks == 0)) {
		BUG_ON(host->cmd);
		BUG_ON(csp_sdc_get_fifo_cnt(host->base));
	}

	if (host->data->flags == MMC_DATA_READ) {
		dfi = SDC_INT_PD_DTF;
	} else {
#if 0
		/* when need send stop for mult-block write,
		 * busy may occur after stop cmd is sent. */
		if (host->data->stop && !host->mrq->sbc)
			dfi = SDC_INT_PD_DTF;
		else
			dfi = SDC_INT_PD_DTF | SDC_INT_PD_BE;
#else
		/*
		 * for n7, we need wait BE before send stop, or we may be
		 * confuse that: BE --> (stop) CMD_DONE --> BE
		 */
		dfi = SDC_INT_PD_DTF | SDC_INT_PD_BE;
#endif
	}

	if (host->send_auto_stop)
		dfi |= SDC_INT_PD_ACD;

	/* data transfer complete */
	if ((host->int_status & dfi) == dfi) {
		if (host->cmd) {
			/*
			 * Data managed to finish before the
			 * command completed. Make sure we do
			 * things in the proper order.
			 * (eg. SDIO cmd to read only a few bytes)
			 */
			host->data_early = 1;
		} else {
			lbsdc_finish_data(host);
		}
	}
}

/* action for the data related status. */
static void lbsdc_data_irq(struct lbsdc_host *host, u32 int_status)
{
	BUG_ON(int_status == 0);

#if 0
PRT_DBG("%s: int_status=0x%08x.\n", mmc_hostname(host->mmc), int_status);
#endif

	/* TODO: for CMD19 */

	if (!host->data) {
		if (host->cmd && (host->cmd->flags & MMC_RSP_BUSY)) {
#if SDC_SUPPORT_INT_BTO
			if (int_status & SDC_INT_PD_BTO) {
				host->cmd->error = -ETIMEDOUT;
				lbsdc_finish_command(host);
				return;
			}
#endif

			if ((int_status & SDC_INT_PD_BE) &&
				(host->int_status & SDC_INT_PD_CMD_DONE)) {
				lbsdc_finish_command(host);
				return;
			}
		} else {
			PRT_ERR("%s: Got data interrupt 0x%08x though %s.\n",
				mmc_hostname(host->mmc), int_status,
				"no data operation was in progress");
			PRT_ERR("%s: last_cmd=%d,int_status=0x%08x\n",
				mmc_hostname(host->mmc), host->last_cmd,
				host->int_status);
			csp_sdc_dumpregs(host->base, mmc_hostname(host->mmc));
		}

		return;
	}

#if SDC_SUPPORT_INT_BTO
	if (int_status & SDC_INT_PD_BTO) {
		PRT_WARN("%s: Busy Timeout(BTO)! opcode=%d\n",
			mmc_hostname(host->mmc), host->data->mrq->cmd->opcode);
		host->data->error = -ETIMEDOUT;
	}
#endif

	if (int_status & SDC_INT_PD_DRTO) {
		PRT_WARN("%s: Data request Timeout(DRTO)! opcode=%d\n",
			mmc_hostname(host->mmc), host->data->mrq->cmd->opcode);
		host->data->error = -ETIMEDOUT;
	}

	if (int_status & SDC_INT_PD_HTO) {
		PRT_WARN("%s: Host request Timeout(HTO)! opcode=%d\n",
			mmc_hostname(host->mmc), host->data->mrq->cmd->opcode);
		host->data->error = -ETIMEDOUT;
	}

	if (int_status & SDC_INT_PD_WNC) {
		PRT_WARN("%s: SDC Write with No CRC(WNC)! opcode=%d\n",
			mmc_hostname(host->mmc), host->data->mrq->cmd->opcode);
		host->data->error = -EILSEQ;
	}

	if (int_status & SDC_INT_PD_SBE) {
		PRT_WARN("%s: Start Bits Error(SBE)! opcode=%d\n",
			mmc_hostname(host->mmc), host->data->mrq->cmd->opcode);
		host->data->error = -EILSEQ;
	}

	if (int_status & SDC_INT_PD_EBE) {
		PRT_WARN("%s: End Bits Error(EBE)! opcode=%d\n",
			mmc_hostname(host->mmc), host->data->mrq->cmd->opcode);
		host->data->error = -EILSEQ;
	}

	if (int_status & SDC_INT_PD_CRC16E) {
		PRT_WARN("%s: Data CRC16 Error(CRC16E)! opcode=%d\n",
			mmc_hostname(host->mmc), host->data->mrq->cmd->opcode);
		host->data->error = -EILSEQ;
	}

	if (host->data->error) {
		PRT_WARN("%s: Data Error!\n", mmc_hostname(host->mmc));
		/* error catched, disable all interrupts */
		csp_sdc_disable_ints(host->base, host->imask);
		csp_sdc_dumpregs(host->base, mmc_hostname(host->mmc));
		lbsdc_finish_data(host);
	} else {
		if (!(host->flags & LBSDC_REQ_USE_DMA)) {
			if (int_status & (SDC_INT_PD_TXDR | SDC_INT_PD_RXDR) ||
				((host->data->flags & MMC_DATA_READ) &&
				(int_status & SDC_INT_PD_DTF))) {

#ifdef SDC_PIO_TASKLET
				tasklet_schedule(&host->pio_tasklet);
#else
				lbsdc_transfer_pio(host);
#endif

				/*
				 * TXDR/RXDR may trigger again and again
				 * during data transfer perio,
				 * clear it for accepting next TXDR/RXDR.
				 */
				csp_sdc_clr_pd(host->base,
					SDC_INT_PD_TXDR | SDC_INT_PD_RXDR);
				host->int_status &=
					~(SDC_INT_PD_TXDR | SDC_INT_PD_RXDR);
			}
		}

#if 0
		if (!(host->flags & LBSDC_REQ_USE_DMA))
			lbsdc_transfer_finish(host);
#else
		lbsdc_transfer_finish(host);
#endif
	}
}

/* SDC IRQ handle entry */
static irqreturn_t lbsdc_irq(int irq, void *dev_id)
{
	irqreturn_t result;
	struct lbsdc_host *host = dev_id;
	u32 int_status, unexpected = 0;
	int cardint = 0, max_loops = 16;

	PRT_DBG("%s\n", mmc_hostname(host->mmc));

	spin_lock(&host->lock);

	if (host->runtime_suspended) {
		spin_unlock(&host->lock);
		PRT_WARN("%s: got irq while runtime suspended\n",
			mmc_hostname(host->mmc));
		return IRQ_HANDLED;
	}

	int_status = csp_sdc_get_int_pd(host->base);
	if (!int_status) {
		PRT_WARN("%s: no sdc interrupt pendings\n",
			mmc_hostname(host->mmc));
		result = IRQ_NONE;
		goto out;
	}

again:
	host->int_status |= int_status;
#if 0
	PRT_DBG("%s: *** got interrupt: 0x%08x\n",
		mmc_hostname(host->mmc), int_status);
#endif

	/* action for cmd */
	if (int_status & SDC_INT_CMD_MASK) {
		/*
		 * clear the pending bits that to be handled
		 */
		csp_sdc_clr_pd(host->base,
			int_status & SDC_INT_CMD_MASK);
		csp_sdc_disable_ints(host->base,
			int_status & SDC_INT_CMD_MASK);
		lbsdc_cmd_irq(host, int_status);
	}

	/* action for data */
	if (int_status & SDC_INT_DATA_MASK) {
		/*
		 * clear the pending bits that to be handled
		 */
		csp_sdc_clr_pd(host->base,
			int_status & SDC_INT_DATA_MASK);
		csp_sdc_disable_ints(host->base,
			int_status & SDC_INT_DATA_MASK);
		lbsdc_data_irq(host, int_status & SDC_INT_DATA_MASK);
	}

#if 0	/* for debug */
	if (host->data && (host->flags & LBSDC_REQ_USE_DMA)) {
		u32 idma_int_pd;

		if (host->data->flags & MMC_DATA_READ)
			idma_int_pd = SDC_IDMA_INT_STA_RI;
		else
			idma_int_pd = SDC_IDMA_INT_STA_TI;

		if (csp_sdc_chk_idma_pd(host->base, idma_int_pd) == 1)
			lbsdc_transfer_finish(host);
	}
#endif

	if (int_status & SDC_INT_PD_SDIO0)
		cardint = 1;

	result = IRQ_HANDLED;

	int_status = csp_sdc_get_int_pd(host->base);
	/* interrupt pending status has changed, deal with it at once. */
	if (((host->cmd == NULL) || (host->cmd->error == 0)) &&
		((host->data == NULL) || (host->data->error == 0)) &&
		(((int_status & host->int_status) != int_status) &&
			--max_loops))
		goto again;

out:
	spin_unlock(&host->lock);

	/* It's abnormal that unexpected status occur. */
	unexpected = host->int_status &
		~(SDC_INT_CMD_MASK|SDC_INT_DATA_MASK|SDC_INT_PD_SDIO0);
#if (!SDC_SUPPORT_INT_BTO)
	unexpected &= ~(SDC_INT_PD_BTO);
#endif

#if !(defined CONFIG_ARCH_LOMBO_N7) && !(defined CONFIG_PLAT_LOMBO_N8)
	unexpected &= ~(SDC_INT_PD_FSM_FINISH);
#endif

	if (unexpected) {
		PRT_ERR("%s: Unexpected interrupt 0x%08x.\n",
			mmc_hostname(host->mmc), unexpected);
		PRT_ERR("%s: last_cmd=%d,int_status=0x%08x\n",
			mmc_hostname(host->mmc), host->last_cmd,
			host->int_status);
		csp_sdc_dumpregs(host->base, mmc_hostname(host->mmc));
		csp_sdc_clr_pd(host->base, unexpected);
	}

	/*
	 * We have to delay this as it calls back into the driver.
	 */
	if (cardint && host->mmc->sdio_irq_thread)
		mmc_signal_sdio_irq(host->mmc);

	if (host->blocks > 0) {
		csp_sdc_enable_ints(host->base,
			SDC_INT_PD_TXDR | SDC_INT_PD_RXDR);
	}

	return result;
}

/*****************************************************************************\
 *                                                                           *
 * Suspend/resume                                                            *
 *                                                                           *
\*****************************************************************************/

#ifdef CONFIG_PM
int lbsdc_suspend_host(struct lbsdc_host *host)
{
	struct clk *clk_null = NULL;
	int ret;
	u32 i;

	ret = mmc_suspend_host(host->mmc);
	if (ret)
		return ret;

	csp_sdc_disable_ints(host->base, SDC_INT_ALL_MASK);

	clk_null = clk_get(NULL, "null_clk");
	clk_set_parent(host->clk_sdc, clk_null);
	clk_put(clk_null);

	clk_disable_unprepare(host->clk_sdc);
	clk_disable_unprepare(host->clk_reset);
	clk_disable_unprepare(host->clk_gate);

	for (i = 0; i < host->clk_parents_cnt; i++)
		clk_unprepare(host->clk_parents[i]);

	free_irq(host->irq, host);
	return ret;

}

int lbsdc_resume_host(struct lbsdc_host *host)
{

	int ret;
	u32 i;

	ret = request_irq(host->irq, lbsdc_irq, IRQF_SHARED,
			  mmc_hostname(host->mmc), host);
	if (ret)
		return ret;

	/* prepare_enable clocks */
	for (i = 0; i < host->clk_parents_cnt; i++)
		clk_prepare(host->clk_parents[i]);

	clk_prepare_enable(host->clk_gate);
	clk_prepare_enable(host->clk_reset);
	clk_prepare_enable(host->clk_sdc);

	/* re-configure the io settings for sdio host */
	if (sdio_host == host) {
		lbsdc_do_start_signal_voltage_switch(host, &host->mmc->ios);
		/* Force clock and power re-program */
		host->clock = 0;
		lbsdc_do_set_ios(host, &host->mmc->ios);
	}

	lbsdc_reset_all(host);
	csp_sdc_set_init_seq(host->base, 80);
	csp_sdc_int_enable(host->base, 1);

	mmiowb();
	return mmc_resume_host(host->mmc);

}
#endif

#ifdef CONFIG_PM_RUNTIME

static int lbsdc_runtime_pm_get(struct lbsdc_host *host)
{
	return pm_runtime_get_sync(host->mmc->parent);
}

static int lbsdc_runtime_pm_put(struct lbsdc_host *host)
{
	pm_runtime_mark_last_busy(host->mmc->parent);
	return pm_runtime_put_autosuspend(host->mmc->parent);
}

int lbsdc_runtime_suspend_host(struct lbsdc_host *host)
{
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);
#if 0	/* TODO: determine which INT_PD we should disable */
	csp_sdc_disable_ints(host, SDC_INT_PD_SDIO0);
#else
	csp_sdc_disable_ints(host->base, SDC_INT_ALL_MASK);
#endif
	spin_unlock_irqrestore(&host->lock, flags);

	synchronize_irq(host->irq);

	spin_lock_irqsave(&host->lock, flags);
	host->runtime_suspended = true;
	spin_unlock_irqrestore(&host->lock, flags);

	return 0;
}

int lbsdc_runtime_resume_host(struct lbsdc_host *host)
{
	unsigned long flags;

	lbsdc_do_start_signal_voltage_switch(host, &host->mmc->ios);
	/* Force clock and power re-program */
	host->clock = 0;
	lbsdc_do_set_ios(host, &host->mmc->ios);

	spin_lock_irqsave(&host->lock, flags);

	host->runtime_suspended = false;

	/* Enable SDIO IRQ */
	if ((host->flags & LBSDC_SDIO_IRQ_ENABLED))
		lbsdc_enable_sdio_irq_nolock(host, true);

	spin_unlock_irqrestore(&host->lock, flags);

	return 0;
}

#endif

/******************************************************************************
 *
 ******************************************************************************/

static const struct of_device_id lbsdc_dt_match[];

static inline struct lbsdc_drv_data *lbsdc_get_driver_data(
			struct platform_device *pdev)
{
	if (pdev->dev.of_node) {
		const struct of_device_id *match;
		match = of_match_node(lbsdc_dt_match, pdev->dev.of_node);
		return (struct lbsdc_drv_data *)match->data;
	}

	return NULL;
}

#define LBSDC_VOLT_SUPP_3V3		0
#define LBSDC_VOLT_SUPP_1V8		1
#define LBSDC_VOLT_SUPP_BOTH		2

#define LBSDC_INVALID_SIDX		(0xFFFF)
/* parse sdc config form device tree */
static int lbsdc_parse_dt(struct device *dev, struct lbsdc_host *host)
{
	u32 volt_supp = 0;
	u32 volt_sidx = LBSDC_INVALID_SIDX;
	struct device_node *node = dev->of_node;
	enum of_gpio_flags flags;
	int len, ret, gpio;

	mmc_of_parse(host->mmc);

	host->flags |= LBSDC_USE_IDMA;
	if (of_find_property(node, "use_pio", &len))
		host->flags &= ~LBSDC_USE_IDMA;

	if (of_find_property(node, "sdio_mode", &len))
		host->flags |= LBSDC_SDIO_MODE;

	if (of_property_read_u32(node, "volt-supp", &volt_supp) < 0) {
		PRT_DBG("%s: \"volt-supp\" is missing, assuming 3.3v.\n",
			mmc_hostname(host->mmc));
		volt_supp = LBSDC_VOLT_SUPP_3V3;
	}

	if (volt_supp > LBSDC_VOLT_SUPP_BOTH) {
		PRT_ERR("%s: \"volt-supp\" value is invalid\n",
			mmc_hostname(host->mmc));
		volt_supp = LBSDC_VOLT_SUPP_3V3;
	} else if (volt_supp == LBSDC_VOLT_SUPP_BOTH) {
		if (of_property_read_u32(node, "volt-sidx", &volt_sidx) < 0) {
			PRT_DBG("%s: \"volt_sidx\" is missing\n",
				mmc_hostname(host->mmc));
			volt_supp = LBSDC_VOLT_SUPP_3V3;
		}
	}
	host->sidx = volt_sidx;

#if 1
	/* for sd card,it should request pwr-gpios */
	host->pwr_gpio = -1;
	gpio = of_get_named_gpio_flags(node, "pwr-gpios", 0, &flags);
	if (gpio_is_valid(gpio)) {
		ret = devm_gpio_request(dev, gpio, "pwr-gpios");
		if (ret < 0) {
			PRT_ERR("gpio:%d request failed\n", gpio);
			return -EBUSY;
		}

		ret = gpio_direction_output(gpio, 1);
		if (ret < 0) {
			PRT_ERR("gpio:%d set output failed\n", gpio);
			return -EBUSY;
		}

		gpio_set_value(gpio, 0);

		host->pwr_gpio = gpio;
	}
#endif

	if (volt_supp > LBSDC_VOLT_SUPP_3V3) {
		if (of_find_property(node, "cap_uhs", &len))
			host->flags |= LBSDC_CAP_UHS;

		if (of_find_property(node, "cap_hs200", &len))
			host->flags |= LBSDC_CAP_HS200;
	}

	if (of_find_property(node, "no_lp", &len))
		host->flags |= LBSDC_LP_DISABLED;

	/* TODO: Add more */

	return 0;
}

/* get sdc uhs_pinctrl*/
static int lbsdc_uhs_pinctrl_get(struct device *dev, struct lbsdc_host *host)
{
	struct pinctrl *pinctrl;
	struct pinctrl_state *state;

	pinctrl = devm_pinctrl_get(dev);
	if (IS_ERR_OR_NULL(pinctrl)) {
		PRT_ERR("failed to get sdc pinctrl\n");
		return 1;
	}
	host->uhs_pinctrl = pinctrl;

	state = pinctrl_lookup_state(pinctrl, "uhs_pinctrl");
	if (IS_ERR_OR_NULL(state)) {
		PRT_ERR("failed to look up state uhs_pinctrl\n");
		return 1;
	}
	host->uhs_state = state;

	return 0;
}

/* get sdc default_pinctrl */
static int lbsdc_default_pinctrl_get(
			struct device *dev, struct lbsdc_host *host)
{
	struct pinctrl *pinctrl;
	struct pinctrl_state *state;

	pinctrl = devm_pinctrl_get(dev);
	if (IS_ERR_OR_NULL(pinctrl)) {
		PRT_ERR("failed to get sdc pinctrl\n");
		return 1;
	}
	host->default_pinctrl = pinctrl;

	state = pinctrl_lookup_state(pinctrl, "default");
	if (IS_ERR_OR_NULL(state)) {
		PRT_ERR("failed to look up state default_pinctrl\n");
		return 1;
	}
	host->default_state = state;

	return 0;
}

/******************************************************************************
 *
 ******************************************************************************/

static int lbsdc_probe(struct platform_device *pdev)
{
	struct lbsdc_drv_data *drv_data;
	struct lbsdc_host *host;
	struct mmc_host	*mmc;
	struct resource *res;
	char clk_pname[16] = {0};
	struct clk *parent_clk = NULL;
	int irq;
	u32 ocr_avail;
	u32 i;
	int ret;

	if (!pdev->dev.of_node) {
		dev_err(&pdev->dev, "%s %d: no device data specified\n",
			__func__, __LINE__);
		return -ENOENT;
	}

	mmc = mmc_alloc_host(sizeof(struct lbsdc_host), &pdev->dev);
	if (!mmc)
		return -ENOMEM;

	host = mmc_priv(mmc);
	host->mmc = mmc;

	/*********************************************************************
	 * host resources
	 ********************************************************************/
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		PRT_ERR("%s: no irq specified!\n", mmc_hostname(mmc));
		return irq;
	}
	host->irq = irq;

	PRT_DBG("%s: irq=%d\n", mmc_hostname(mmc), irq);

	host->clk_gate = devm_clk_get(&pdev->dev, "sdc_ahb_gate");
	if (IS_ERR_OR_NULL(host->clk_gate)) {
		PRT_ERR("%s: failed to get ahb sdc clock gate!\n",
			mmc_hostname(mmc));
		ret = PTR_ERR(host->clk_gate);
		goto err_dt_clk;
	}

	host->clk_reset = devm_clk_get(&pdev->dev, "sdc_ahb_reset");
	if (IS_ERR_OR_NULL(host->clk_reset)) {
		PRT_ERR("%s: failed to get ahb sdc clock reset!\n",
			mmc_hostname(mmc));
		ret = PTR_ERR(host->clk_reset);
		goto err_dt_clk;
	}

	host->clk_sdc = devm_clk_get(&pdev->dev, "sdc_module_clk");
	if (IS_ERR_OR_NULL(host->clk_sdc)) {
		PRT_ERR("%s: failed to get sdc module clock!\n",
			mmc_hostname(mmc));
		ret = PTR_ERR(host->clk_sdc);
		goto err_dt_clk;
	}

	while (host->clk_parents_cnt < SDC_MAX_CLK_PARENTS_CNT) {
		sprintf(clk_pname, "sdc_clk_p%u", host->clk_parents_cnt);

		parent_clk = devm_clk_get(&pdev->dev, clk_pname);
		host->clk_parents_cnt++;
		if (IS_ERR_OR_NULL(parent_clk)) {
			PRT_DBG("can't get parent clock \"%s\"\n",
				clk_pname);
			host->clk_parents_cnt--;
			break;
		}
		host->clk_parents[host->clk_parents_cnt - 1] = parent_clk;
		clk_prepare(parent_clk);
	}

	if (host->clk_parents_cnt == 0) {
		/* make sure that we have specified "clock-parents" in dts */
		PRT_ERR("%s: can't get clock parents for sdc module!\n",
			mmc_hostname(mmc));
		ret = PTR_ERR(host->clk_parents[0]);
		goto err_dt_clk;
	}

	clk_prepare_enable(host->clk_gate);
	clk_prepare_enable(host->clk_reset);
	clk_prepare_enable(host->clk_sdc);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	host->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(host->base)) {
		ret = PTR_ERR(host->base);
		goto err_ioremap;
	}

	PRT_DBG("%s: base=0x%p\n", mmc_hostname(mmc), host->base);

	/*********************************************************************
	 * set host parameters.
	 ********************************************************************/
	ret = lbsdc_parse_dt(&pdev->dev, host);
	if (ret)
		goto err_dt_clk;

	/* TODO: use drv_data for difference. */
	drv_data = lbsdc_get_driver_data(pdev);

	host->dev = &pdev->dev;

	platform_set_drvdata(pdev, host);
	mmc->ops = &lbsdc_ops;

	if (mmc->f_max == 0)
		mmc->f_max = lbsdc_get_max_clock(host);

#if (defined CONFIG_ARCH_LOMBO_N7V0_FPGA) ||	\
	(defined CONFIG_ARCH_LOMBO_N7V1_FPGA) ||	\
	(defined CONFIG_ARCH_LOMBO_N9V0_FPGA)
	/* in FPGA, frequency is max to 12M */
	mmc->f_max = 12000000;
#endif

	/* minimum clk freq set to 24M / 12 / 10. */
	mmc->f_min = 100000;

	/*********************************************************************
	 * host capability
	 ********************************************************************/

	mmc->caps |= MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED;
	mmc->caps |= MMC_CAP_CMD23;
	if (host->flags & LBSDC_CAP_UHS) {
		mmc->caps |= MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 |
			MMC_CAP_UHS_SDR50 | MMC_CAP_UHS_SDR104;
	}

#if 0
	mmc->caps |= MMC_CAP_HW_RESET;
#endif

	if (host->flags & LBSDC_CAP_HS200)
		mmc->caps2 |= MMC_CAP2_HS200_1_8V_SDR;

	/*********************************************************************
	 * pinctrl
	 ********************************************************************/
	ret = lbsdc_default_pinctrl_get(&pdev->dev, host);
	if (ret) {
		PRT_ERR("%s:can't get default_pinctrl for sdc module\n",
			mmc_hostname(mmc));
	} else {
		PRT_INFO("%s:get default_pinctrl for sdc module\n",
			mmc_hostname(mmc));
	}

	if ((host->flags & LBSDC_CAP_UHS) || (host->flags & LBSDC_CAP_HS200)) {
		/* make sure that we have specified "uhs_pinctrl" in dts */
		ret = lbsdc_uhs_pinctrl_get(&pdev->dev, host);
		if (ret) {
			PRT_ERR("%s:can't get uhs_pinctrl for sdc module\n",
				mmc_hostname(mmc));
			goto err_dt_clk;
		} else {
			PRT_INFO("%s:get uhs_pinctrl for sdc module\n",
				mmc_hostname(mmc));
		}
	}

#if 0
	mmc->caps |= MMC_CAP_DRIVER_TYPE_A | MMC_CAP_DRIVER_TYPE_C |
				MMC_CAP_DRIVER_TYPE_D;
#endif

	pm_runtime_enable(&pdev->dev);
	pm_runtime_set_autosuspend_delay(&pdev->dev, 50);
	pm_runtime_use_autosuspend(&pdev->dev);
	pm_suspend_ignore_children(&pdev->dev, 1);

	/*********************************************************************
	 * power
	 ********************************************************************/

#if 0
	/* If vqmmc regulator and no 1.8V signalling, then there's no UHS */
	mmc->supply.vqmmc = regulator_get(mmc_dev(mmc), "vqmmc");
	if (IS_ERR_OR_NULL(mmc->supply.vqmmc)) {
		if (PTR_ERR(mmc->supply.vqmmc) < 0) {
			PRT_INFO("%s: no vqmmc regulator found!\n",
				mmc_hostname(mmc));
			mmc->supply.vqmmc = NULL;
		}
	} else {
		ret = regulator_enable(host->mmc->supply.vqmmc);
		if (!regulator_is_supported_voltage(host->mmc->supply.vqmmc,
			1700000, 1950000))
			mmc->caps &= ~(MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 |
				MMC_CAP_UHS_SDR50 | MMC_CAP_UHS_SDR104 |
				MMC_CAP_UHS_DDR50);
		if (ret) {
			PRT_INFO("%s: Failed to enable vqmmc regulator: %d!\n",
				mmc_hostname(mmc), ret);
			mmc->supply.vqmmc = NULL;
		}
	}

	/* TODO: configure for tuning */

	/*******/

	mmc->supply.vmmc = regulator_get(mmc_dev(mmc), "vmmc");
	if (IS_ERR_OR_NULL(mmc->supply.vmmc)) {
		if (PTR_ERR(mmc->supply.vmmc) < 0) {
			PRT_INFO("%s: no vmmc regulator found!\n",
				mmc_hostname(mmc));
			mmc->supply.vmmc = NULL;
		}
	}
#endif

	/* TODO: get max current */

	/*******/

#if 1
	ocr_avail = MMC_VDD_27_28 | MMC_VDD_28_29 |
			MMC_VDD_29_30 | MMC_VDD_30_31 |
			MMC_VDD_31_32 | MMC_VDD_32_33 |
			MMC_VDD_32_33 | MMC_VDD_33_34 |
			MMC_VDD_34_35 | MMC_VDD_35_36;
#else
	ocr_avail	= MMC_VDD_32_33 | MMC_VDD_33_34;
#endif

	mmc->ocr_avail = ocr_avail;
	mmc->ocr_avail_sdio = ocr_avail;
	mmc->ocr_avail_sd = ocr_avail;
	mmc->ocr_avail_mmc = ocr_avail | MMC_VDD_165_195;

	if (mmc->ocr_avail == 0) {
		PRT_ERR("%s: Hardware doesn't report any support voltages.\n",
			mmc_hostname(mmc));
		return -ENODEV;
	}

	/*********************************************************************
	 * data i/o configure
	 ********************************************************************/
	spin_lock_init(&host->lock);

	/*
	 * Maximum segments count.
	 * Depend on max IDMA descriptors count (64 * 2).
	 */
	mmc->max_segs = LBSDC_MAX_DESC_CNT * 2;

	/*
	 * Maximum number of sectors in one transfer. Limited by DMA boundary
	 * size (512KiB).
	 */
	mmc->max_req_size = LBSDC_MAX_REQ_SIZE;

	/*
	 * Maximum segments size.
	 * Depend on max IDMA descriptor's max buffer size.
	 */
	mmc->max_seg_size = MAX_IDMA_DESC_BUF_SIZE; /* 32K */

	/*
	 * Maximum block size.
	 */
	mmc->max_blk_size = LBSDC_MAX_BLK_SIZE;

	/*
	 * Maximum block count.
	 */
	mmc->max_blk_count = LBSDC_MAX_BLOCK_CNT;

	tasklet_init(&host->finish_tasklet,
		lbsdc_tasklet_finish, (unsigned long)host);

	setup_timer(&host->timer, lbsdc_timeout_timer, (unsigned long)host);

	ret = request_irq(host->irq, lbsdc_irq, IRQF_SHARED,
		mmc_hostname(mmc), host);
	if (ret) {
		PRT_ERR("%s: Failed to request IRQ %d: %d.\n",
			mmc_hostname(mmc), host->irq, ret);
		goto untasklet;
	}

	/* reset the whole controller */
	lbsdc_reset_all(host);

	csp_sdc_set_init_seq(host->base, 80);
	csp_sdc_int_enable(host->base, 1);

	mmiowb();

	host->clock = mmc->f_min;

	if (host->flags & LBSDC_SDIO_MODE) {
		sdio_host = host;
		/* do not periodically detect whether sdio card alive */
		host->mmc->caps &= ~MMC_CAP_NEEDS_POLL;
	}

	ret = mmc_add_host(mmc);
	if (ret) {
		PRT_ERR("%s: mmc_add_host() failed.\n", mmc_hostname(mmc));
		goto err_add_host;
	}

	if (host->flags & LBSDC_USE_IDMA) {
		host->idma_desc = kzalloc(LBSDC_MAX_DESC_CNT * IDMA_DESC_SIZE,
					GFP_KERNEL);
		if (host->idma_desc == NULL)
			host->flags &= ~LBSDC_USE_IDMA;
	}

	dev_info(&pdev->dev, "%s - using %s, %s SDIO IRQ, host bw %d\n",
		mmc_hostname(mmc),
		 (host->flags & LBSDC_USE_IDMA) ? "dma" : "pio",
		 mmc->caps & MMC_CAP_SDIO_IRQ ? "hw" : "sw",
		 mmc->caps & MMC_CAP_8_BIT_DATA ? 8 :
		 (mmc->caps & MMC_CAP_4_BIT_DATA ? 4 : 1));

	return 0;

err_add_host:
	sdio_host = NULL;
	pm_runtime_forbid(&pdev->dev);
	pm_runtime_get_noresume(&pdev->dev);

untasklet:
	tasklet_kill(&host->finish_tasklet);

err_ioremap:
	clk_disable_unprepare(host->clk_sdc);
	clk_disable_unprepare(host->clk_reset);
	clk_disable_unprepare(host->clk_gate);
	for (i = 0; i < host->clk_parents_cnt; i++)
		clk_unprepare(host->clk_parents[i]);

err_dt_clk:
	mmc_free_host(host->mmc);

	return ret;
}

static int lbsdc_remove(struct platform_device *pdev)
{
	struct lbsdc_host *host = platform_get_drvdata(pdev);
	unsigned long flags;
	u32 i;

	spin_lock_irqsave(&host->lock, flags);

	host->flags |= LBSDC_DEVICE_DEAD;

	if (host->mrq) {
		PRT_ERR("%s: Controller removed during transfer!\n",
			mmc_hostname(host->mmc));

		host->mrq->cmd->error = -ENOMEDIUM;
		tasklet_schedule(&host->finish_tasklet);
	}

	spin_unlock_irqrestore(&host->lock, flags);

	mmc_remove_host(host->mmc);

	csp_sdc_disable_ints(host->base, SDC_INT_ALL_MASK);
	free_irq(host->irq, host);

	del_timer_sync(&host->timer);

	tasklet_kill(&host->finish_tasklet);

#if 0
	if (host->mmc->supply.vmmc) {
		regulator_disable(host->mmc->supply.vmmc);
		regulator_put(host->mmc->supply.vmmc);
	}

	if (host->mmc->supply.vqmmc) {
		regulator_disable(host->mmc->supply.vqmmc);
		regulator_put(host->mmc->supply.vqmmc);
	}
#endif

	kfree(host->idma_desc);
	host->idma_desc = NULL;

	pm_runtime_dont_use_autosuspend(&pdev->dev);
	pm_runtime_disable(&pdev->dev);

	clk_disable_unprepare(host->clk_sdc);
	clk_disable_unprepare(host->clk_reset);
	clk_disable_unprepare(host->clk_gate);
	for (i = 0; i < host->clk_parents_cnt; i++)
		clk_unprepare(host->clk_parents[i]);

	mmc_free_host(host->mmc);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int lbsdc_suspend(struct device *dev)
{
	struct lbsdc_host *host = dev_get_drvdata(dev);

	return lbsdc_suspend_host(host);
}

static int lbsdc_resume(struct device *dev)
{
	struct lbsdc_host *host = dev_get_drvdata(dev);

	return lbsdc_resume_host(host);
}
#endif

#ifdef CONFIG_PM_RUNTIME
static int lbsdc_runtime_suspend(struct device *dev)
{
	struct lbsdc_host *host = dev_get_drvdata(dev);
	u32 i;
	int ret;

	ret = lbsdc_runtime_suspend_host(host);

	clk_disable_unprepare(host->clk_sdc);
	clk_disable_unprepare(host->clk_gate);
	for (i = 0; i < host->clk_parents_cnt; i++)
		clk_unprepare(host->clk_parents[i]);
	return ret;
}

static int lbsdc_runtime_resume(struct device *dev)
{
	struct lbsdc_host *host = dev_get_drvdata(dev);
	u32 i;
	int ret;

	for (i = 0; i < host->clk_parents_cnt; i++)
		clk_prepare(host->clk_parents[i]);
	clk_prepare_enable(host->clk_gate);
	clk_prepare_enable(host->clk_sdc);
	ret = lbsdc_runtime_resume_host(host);
	return ret;
}
#endif

#ifdef CONFIG_PM
static const struct dev_pm_ops lbsdc_pmops = {
	SET_SYSTEM_SLEEP_PM_OPS(lbsdc_suspend, lbsdc_resume)
	SET_RUNTIME_PM_OPS(lbsdc_runtime_suspend, lbsdc_runtime_resume,
			   NULL)
};

#define LBSDC_PMOPS (&lbsdc_pmops)

#else
#define LBSDC_PMOPS NULL
#endif

#ifdef CONFIG_OF
static const struct of_device_id lbsdc_dt_match[] = {
	{ .compatible = "lombo,n7-sdc", },
	{ .compatible = "lombo,n8-sdc", },
	{ .compatible = "lombo,n9-sdc", },
	{},
};
MODULE_DEVICE_TABLE(of, lbsdc_dt_match);
#endif

static struct platform_driver lbsdc_driver = {
	.probe		= lbsdc_probe,
	.remove		= lbsdc_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "lombo-sdc",
		.of_match_table = of_match_ptr(lbsdc_dt_match),
#ifdef CONFIG_ARCH_LOMBO_N7V1
		.pm	= LBSDC_PMOPS,
#endif
	},
};

module_platform_driver(lbsdc_driver);

MODULE_DESCRIPTION("LOMBO SD/MMC Card Interface driver");
MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:lombo-sdc");
