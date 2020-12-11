/*
 * sdc.h - SDC host driver code for LomboTech
 * sdc driver structure and interface
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
#ifndef ____SDC___H__
#define ____SDC___H__

#define LBSDC_WAIT_UCF_TIMEOUT	1

#define LBSDC_MAX_BLK_SIZE	(512)
#define LBSDC_MAX_BLOCK_CNT	(4096)
#define LBSDC_MAX_DATA_SIZE	(LBSDC_MAX_BLK_SIZE * LBSDC_MAX_BLOCK_CNT)
#define LBSDC_IDMA_DESC_TABLE_SIZE	(4096)
#define LBSDC_MAX_DESC_CNT	(LBSDC_IDMA_DESC_TABLE_SIZE / IDMA_DESC_SIZE)

#define LBSDC_MAX_REQ_SIZE	(524288)

#define LBSDC_IDMA_ADDR_MASK	0x3

#define LBSDC_CD_NONE		0
#define LBSDC_CD_GPIO		1
#define LBSDC_CD_PERMANENT	2

#define LBSDC_TRANS_RETRIES	2

/* idma descriptor的desc0 */
union idma_des0 {
	u32 val;
	struct {
		u32	resv0:1;
		u32	dic:1;
		u32	ld:1;
		u32	fs:1;
		u32	ch:1;
		u32	er:1;
		u32	resv1:24;
		u32	ces:1;
		u32	own:1;
	} bits;
};

/* idma descriptor的desc2 */
union idma_des2 {
	u32 val;
	struct {
		u32	bs1:16;
		u32	bs2:16;
	} bits;
};

/* idma descriptor的desc4 */
struct idma_des4 {
	u32		bap1;
};

/* idma descriptor的desc5 */
struct idma_des5 {
	u32		bap1;
};

/* idma descriptor的desc6 */
union idma_des6 {
	u32		bap2;
	u32		nda;
};

/* idma descriptor的desc7 */
union idma_des7 {
	u32		bap2;
	u32		nda;
};

struct idma_desc {
	union idma_des0		des0;
	u32			des1;	/* rsvd */
	union idma_des2		des2;
	u32			des3;	/* rsvd */
	struct idma_des4	des4;
	struct idma_des5	des5;	/* rsvd */
	union idma_des6		des6;
	union idma_des7		des7;	/* rsvd */
};


/* host flags */
#define LBSDC_USE_IDMA		(1UL << 0)	/* Host is prefer to use iDMA */
#define LBSDC_REQ_USE_DMA	(1UL << 2)	/* Use DMA for this req. */
#define LBSDC_DEVICE_DEAD	(1UL << 3)	/* Device unresponsive */
#define LBSDC_AUTO_CMD12	(1UL << 6)	/* Auto CMD12 support */
#define LBSDC_SDIO_IRQ_ENABLED	(1UL << 9)	/* SDIO irq enabled */
#define LBSDC_LP_DISABLED	(1UL << 10)	/* low power mode disabled */
#define LBSDC_SDIO_MODE		(1UL << 16)	/* SDIO host for SDIO card */
#define LBSDC_CAP_UHS		(1UL << 17)	/* Host support UHS */
#define LBSDC_CAP_HS200		(1UL << 18)	/* Host support HS200 */


#define SDC_MAX_CLK_PARENTS_CNT		4

struct lbsdc_host {
	struct device		*dev;
	struct mmc_host		*mmc;

	void __iomem		*base;		/* iomem base */

	struct clk		*clk_sdc;	/* sdc module clock */
	struct clk		*clk_gate;	/* sdc ahb clock gate */
	struct clk		*clk_reset;	/* sdc ahb clock reset */

	/* sdc module clock parents */
	struct pinctrl		*default_pinctrl;
	struct pinctrl_state    *default_state;
	struct pinctrl          *uhs_pinctrl;
	struct pinctrl_state    *uhs_state;

	struct clk		*clk_parents[SDC_MAX_CLK_PARENTS_CNT];
#if 0
	u32			clk_prate[SDC_MAX_CLK_PARENTS_CNT];
#endif
	u32			clk_parents_cnt;

	spinlock_t		lock;
	int			irq;		/* sdc irq number */

	u32 runtime_suspended;	/* Host is runtime suspended */

	u32			flags;
	u32			cd_gpio;
	s32			pwr_gpio;
	u32			pw_state;
	u32			sidx;

	u32			clock;		/* Current card clock (MHz) */
	u32			bus_width;

	u32			send_auto_stop;
	u32			imask;
	u32			int_status;

	u32			data_early;	/* DTF before CMD_DONE */
	u32			last_cmd;	/* store the last cmd code */

	struct mmc_request	*mrq;		/* current mmc request */
	struct mmc_command	*cmd;		/* current mmc command */
	struct mmc_data		*data;		/* current mmc data */

	struct sg_mapping_iter	sg_miter;	/* SG state for PIO */
	u32			blocks;		/* remaining PIO blocks */

	u32			sg_count;	/* Mapped sg entries */

	struct idma_desc	*idma_desc;	/* iDMA descr. table */
	size_t			idma_table_sz;	/* iDMA descr. table size*/

	dma_addr_t		idma_addr;	/* Mapped iDMA descr. table */

	struct tasklet_struct finish_tasklet;

	struct timer_list	timer;		/* timer for waiting
						  * hardware interrupt */
};

#endif
