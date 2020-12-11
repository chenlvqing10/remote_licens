/*
 * sdc_const.h - SDC host driver code for LomboTech
 * sdc host driver macro define
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

#ifndef ___SDC___CONST__H___
#define ___SDC___CONST__H___

/******************************************************************************
 * sau ldo
 *****************************************************************************/
#define SAU_SDC_LDO_PD_0UA		(0)
#define SAU_SDC_LDO_PD_6MA		(1)
#define SAU_SDC_LDO_PD_12MA		(2)
#define SAU_SDC_LDO_PD_18MA		(3)

#define SAU_SDC_LDO_VOL_1V65		(0)
#define SAU_SDC_LDO_VOL_1V70		(1)
#define SAU_SDC_LDO_VOL_1V75		(2)
#define SAU_SDC_LDO_VOL_1V80		(3)
#define SAU_SDC_LDO_VOL_1V85		(4)
#define SAU_SDC_LDO_VOL_1V90		(5)
#define SAU_SDC_LDO_VOL_1V95		(6)
#define SAU_SDC_LDO_VOL_1V20		(7)

/* ***** */

#define SD_MAX_CMD_INDEX		0x3f

#define SDC_SUPPORT_INT_BTO		0

/******************************************************************************
 * feature
 *****************************************************************************/
#define SDC_FIFO_WIDTH			4
#define SDC_IDMA_WIDTH			4

/******************************************************************************
 * idma descriptor
 *****************************************************************************/
#define MAX_IDMA_DESC_BUF_SIZE		0x00008000	/* 32K */
#define IDMA_DESC_SKIP_LEN		0		/* not-fixed */
#define IDMA_DESC_SIZE			32

/*
 * descriptor's location (default in dram)
 */
#define SDC_IDMA_DESC_IN_SRAM		0
#define SDC_IDMA_DESC_IN_DDR		1

/******************************************************************************
 * Control Register
 *****************************************************************************/
/*
 * trans mode (USE_IDMAC)
 */
#define SDC_TRANS_MODE_POLL		0
#define SDC_TRANS_MODE_IDMA		1

/*
 * signal voltage (VOLT_REG0)
 */
#define SDC_VOLT_3_3			0
#define SDC_VOLT_1_8			1

/******************************************************************************
 * byte count (Byte Count Register)
 *****************************************************************************/
#define SDC_MAX_BYTE_CNT		0xffffffff

/******************************************************************************
 * timeout (Timeout Register)
 *****************************************************************************/
#define SDC_DEFAULT_RTO			0x40
#define SDC_DEFAULT_DTO_STEP		0
#define SDC_DEFAULT_DTO_CNT		0x249F00
#define SDC_LONG_TO_STEP_CNT		256
#define SDC_MAX_DATA_TO_CNT		((1 << 23) - 1)

/******************************************************************************
 * Card BUS Width Register
 *****************************************************************************/
#define SDC_BUS_WIDTH_1		0
#define SDC_BUS_WIDTH_4		1
#define SDC_BUS_WIDTH_8		2

/******************************************************************************
 * Timing Register
 *****************************************************************************/
#define SDC_RX_PHASE_0                  (0x0UL)
#define SDC_RX_PHASE_90                 (0x1UL)
#define SDC_RX_PHASE_180                (0x2UL)
#define SDC_RX_PHASE_270                (0x3UL)

#define SDC_TX_PHASE_90                 (0x1UL)
#define SDC_TX_PHASE_180                (0x2UL)

/******************************************************************************
 * Status Register
 *****************************************************************************/
#define SDC_DATA_LVL_MASK		0x000000F0

/******************************************************************************
 * Command Register
 *****************************************************************************/
#define SDC_CMD_RSP_EXPECT                      (1UL << 6)
#define SDC_CMD_RSP_LEN_LONG                    (1UL << 7)
#define SDC_CMD_CHECK_RSP_CRC                   (1UL << 8)
#define SDC_CMD_DATA_EXPECT                     (1UL << 9)
#define SDC_CMD_TRANS_DIR_WRITE                 (1UL << 10)
#define SDC_CMD_TRANS_MODE_STREAM               (1UL << 11)
#define SDC_CMD_SEND_AUTO_STOP                  (1UL << 12)
#define SDC_CMD_WAIT_PRV_DATA_COM               (1UL << 13)
#define SDC_CMD_STOP_ABORT_CMD                  (1UL << 14)
#define SDC_CMD_SEND_INIT                       (1UL << 15)
#define SDC_CMD_UPDATE_CLK_REG_ONLY             (1UL << 21)
#define SDC_CMD_BUSY_EXPT                       (1UL << 23)
#define SDC_CMD_EN_BOOT                         (1UL << 24)
#define SDC_CMD_EXPECT_BOOT_ACK                 (1UL << 25)
#define SDC_CMD_DIS_BOOT                        (1UL << 26)
#define SDC_CMD_BOOT_MODE_ALTER                 (1UL << 27)
#define SDC_CMD_VOLT_SWITCH                     (1UL << 28)
#define SDC_CMD_USE_HOLD_REG                    (1UL << 29)
#define SDC_CMD_START_CMD                       (1UL << 31)

#define SDC_CMD_NO_RSP_CFG	(0)
#define	SDC_CMD_RSP_R1_CFG	(SDC_CMD_RSP_EXPECT|	\
			SDC_CMD_CHECK_RSP_CRC|SDC_CMD_WAIT_PRV_DATA_COM)
#define SDC_CMD_RSP_R1_RX_CFG	(SDC_CMD_RSP_R1_CFG|SDC_CMD_DATA_EXPECT)
#define SDC_CMD_RSP_R1_TX_CFG	(SDC_CMD_RSP_R1_CFG|	\
				  SDC_CMD_DATA_EXPECT|SDC_CMD_TRANS_DIR_WRITE)
#define	SDC_CMD_RSP_R2_CFG	\
			(SDC_CMD_RSP_EXPECT|SDC_CMD_RSP_LEN_LONG|	\
			 SDC_CMD_CHECK_RSP_CRC|SDC_CMD_WAIT_PRV_DATA_COM)
#define	SDC_CMD_RSP_R3_CFG	(SDC_CMD_RSP_EXPECT|SDC_CMD_WAIT_PRV_DATA_COM)
#define	SDC_CMD_RSP_R1B_CFG	(SDC_CMD_RSP_R1_CFG|SDC_CMD_BUSY_EXPT)
#define SDC_CMD_RSP_R6_CFG	SDC_CMD_RSP_R1_CFG
#define SDC_CMD_RSP_R7_CFG	SDC_CMD_RSP_R1_CFG

#define SDC_CMD_MASK		\
			(SDC_CMD_RSP_EXPECT|SDC_CMD_RSP_LEN_LONG|	\
			 SDC_CMD_CHECK_RSP_CRC|SDC_CMD_DATA_EXPECT|	\
			 SDC_CMD_TRANS_DIR_WRITE|SDC_CMD_TRANS_MODE_STREAM| \
			 SDC_CMD_SEND_AUTO_STOP|SDC_CMD_WAIT_PRV_DATA_COM|  \
			 SDC_CMD_STOP_ABORT_CMD|SDC_CMD_SEND_INIT|	\
			 SDC_CMD_UPDATE_CLK_REG_ONLY|SDC_CMD_BUSY_EXPT|	\
			 SDC_CMD_EN_BOOT|SDC_CMD_EXPECT_BOOT_ACK|	\
			 SDC_CMD_DIS_BOOT|SDC_CMD_BOOT_MODE_ALTER|	\
			 SDC_CMD_VOLT_SWITCH)

/******************************************************************************
 * Interrupt Pending register
 *****************************************************************************/
#define SDC_INT_PD_CMD_DONE     (1UL << 0)	/* Cmd done */
#define SDC_INT_PD_DTF          (1UL << 1)	/* Data transfer finish */
#define SDC_INT_PD_BE           (1UL << 2)	/* Busy end */
#define SDC_INT_PD_ACD          (1UL << 3)	/* Auto stop cmd done */
#define SDC_INT_PD_RSP_ERR      (1UL << 8)	/* Response error */
#define SDC_INT_PD_CRC7E        (1UL << 9)	/* Response CRC error */
#define SDC_INT_PD_CRC16E       (1UL << 10)	/* Data CRC error */
#define SDC_INT_PD_WNC          (1UL << 11)	/* Write no CRC-status */
#define SDC_INT_PD_SBE          (1UL << 12)	/* Start-bit error */
#define SDC_INT_PD_EBE          (1UL << 13)	/* End-bit error (read) */
/* Hardware locked write error */
#define SDC_INT_PD_HLWE         (1UL << 14)
#define SDC_INT_PD_RTO          (1UL << 16)	/* Response timeout */
#define SDC_INT_PD_BTO          (1UL << 17)	/* Busy timeout */
#define SDC_INT_PD_DRTO         (1UL << 18)	/* Data read timeout */
/* Data starvation-by-host timeout */
#define SDC_INT_PD_HTO          (1UL << 19)
#define SDC_INT_PD_TXDR         (1UL << 20)	/* Tx FIFO data request */
#define SDC_INT_PD_RXDR         (1UL << 21)	/* Rx FIFO data request */
#define SDC_INT_PD_FIFO_RUN     (1UL << 22)	/* FIFO under/overrun error */
#define SDC_INT_PD_SDIO0        (1UL << 24)	/* SDIO0 Interrupt */
#define SDC_INT_PD_BAR          (1UL << 28)	/* Boot Ack Received */
#define SDC_INT_PD_BDS          (1UL << 29)	/* Boot Data Start */
#define SDC_INT_PD_VS           (1UL << 30)	/* Volt switch */
#define SDC_INT_PD_UCF          (1UL << 31)	/* Update clock finished */

#define SDC_INT_ALL_MASK	(0xFFFFFFFFUL)

#define	SDC_INT_ALL_CMD_ERR	\
		(SDC_INT_PD_RSP_ERR|SDC_INT_PD_CRC7E|SDC_INT_PD_RTO)
#define SDC_INT_CMD_MASK	(SDC_INT_ALL_CMD_ERR|SDC_INT_PD_CMD_DONE)

/* Busy is treated as data */
#if SDC_SUPPORT_INT_BTO
#define SDC_INT_ALL_DATA_ERR	\
		(SDC_INT_PD_CRC16E|SDC_INT_PD_BTO|			\
		 SDC_INT_PD_WNC|SDC_INT_PD_SBE|SDC_INT_PD_EBE|		\
		 SDC_INT_PD_DRTO|SDC_INT_PD_HTO|SDC_INT_PD_FIFO_RUN)
#else
#define SDC_INT_ALL_DATA_ERR	\
		(SDC_INT_PD_CRC16E|			\
		 SDC_INT_PD_WNC|SDC_INT_PD_SBE|SDC_INT_PD_EBE|		\
		 SDC_INT_PD_DRTO|SDC_INT_PD_HTO|SDC_INT_PD_FIFO_RUN)
#endif

#define SDC_INT_DATA_MASK	\
		(SDC_INT_ALL_DATA_ERR|SDC_INT_PD_DTF|SDC_INT_PD_BE|	\
		 SDC_INT_PD_TXDR|SDC_INT_PD_RXDR)

#define SDC_INT_ALL_ERR		\
		(SDC_INT_ALL_CMD_ERR|SDC_INT_ALL_DATA_ERR|SDC_INT_PD_HLWE)

#define SDC_INT_EXPECTED	\
		(SDC_INT_PD_CMD_DONE|SDC_INT_PD_DTF|SDC_INT_PD_BE)

/******************************************************************************
 * FIFO threshold register
 *****************************************************************************/
#define SDC_FIFO_DEPTH			(512UL * 2 / 4)

#define SDC_IDMA_BURST_1		(0x0UL)
#define SDC_IDMA_BURST_4		(0x1UL)
#define SDC_IDMA_BURST_8		(0x2UL)
#define SDC_IDMA_BURST_16		(0x3UL)

/******************************************************************************
 * Internal DMAC Status Register
 *****************************************************************************/
#define SDC_IDMA_INT_STA_TI                     (1UL << 0)
#define SDC_IDMA_INT_STA_RI                     (1UL << 1)
#define SDC_IDMA_INT_STA_FBE                    (1UL << 2)
#define SDC_IDMA_INT_STA_DU                     (1UL << 4)
#define SDC_IDMA_INT_STA_CES                    (1UL << 5)
#define SDC_IDMA_INT_STA_NIS                    (1UL << 8)
#define SDC_IDMA_INT_STA_AIS                    (1UL << 9)

#define SDC_IDMA_INT_ALL_ERR	\
		(SDC_IDMA_INT_STA_FBE|SDC_IDMA_INT_STA_DU|		\
		 SDC_IDMA_INT_STA_CES|SDC_IDMA_INT_STA_AIS)

#endif /* ___SDC___CONST__H___ */
