/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS driver code for LomboTech
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

#ifndef ___VISS___CONST__H___
#define ___VISS___CONST__H___

/******************************************************************************
 * TOP
 *****************************************************************************/
enum viss_clksrc_type {
	VISS_CLKSRC_PERH0_PLL_DIV2,
	VISS_CLKSRC_PLL_MUX,
};

enum viss_mclksrc_type {
	VISS_MCLKSRC_HOSC_24M,
	VISS_MCLKSRC_PERH0_PLL_DIV2,
};

/******************************************************************************
 * VIC
 *****************************************************************************/

/* color format from sensor to VIC */
enum viss_color_fmt {
	VISS_FMT_RGB444 = 0x10,
	VISS_FMT_RGB555,
	VISS_FMT_RGB565,
	VISS_FMT_RGB666,
	VISS_FMT_RGB888,
	VISS_FMT_RGB30_LOCAL,
	VISS_FMT_YCBCR420 = 0x20,
	VISS_FMT_YCRCB420,
	VISS_FMT_YCBYCR422,
	VISS_FMT_YCRYCB422,
	VISS_FMT_CBYCRY422,
	VISS_FMT_CRYCBY422,
	VISS_FMT_YCBCR444_LOCAL,
	VISS_FMT_RAW8 = 0x40,
	VISS_FMT_RAW10,
	VISS_FMT_RAW12,
	VISS_FMT_JPEG = 0x80,
	VISS_FMT_YUYV_JPEG = 0x100,

};

#define VIC_IO_PCLK_INV			(1UL << 0)
#define VIC_IO_FIELD_INV		(1UL << 1)
#define VIC_IO_HS_INV			(1UL << 2)
#define VIC_IO_VS_INV			(1UL << 3)

enum vic_data_path {
	VIC_OUTPUT_DMA,
	VIC_OUTPUT_PIXEL_CH,
	VIC_OUTPUT_BOTH,
};

enum vic_burst_length {
	VIC_DMA_BURST_LEN_1,
	VIC_DMA_BURST_LEN_2,
	VIC_DMA_BURST_LEN_4,
	VIC_DMA_BURST_LEN_8,
};

/* dma chanel */
enum vic_dma_chanel {
	VIC_DMA_CH0,
	VIC_DMA_CH1,
	VIC_DMA_CH2,
	VIC_DMA_CH3,
};

/* output format */
enum vic_output_format {
	VIC_OUTFMT_PAST_THROUGH,
	VIC_OUTFMT_YUV422SP,
	VIC_OUTFMT_YUV420SP,
	VIC_OUTFMT_YUV422P,
	VIC_OUTFMT_YUV420P,
};

/* yuv sequence of input source  */
enum vic_yuv_seq {
	VIC_C0C1C2C3	= 0,
	VIC_C2C1C0C3	= 1,
	VIC_C1C0C3C2	= 2,
	VIC_C3C0C1C2	= 3,
};

/* field store order */
enum vic_filed_order {
	VIC_FIELD_NONE,
	VIC_FIELD_SEQ_TB,
	VIC_FIELD_SEQ_BT,
	VIC_FIELD_INTERLACED_TB,
	VIC_FIELD_INTERLACED_BT,
};

enum vic_capture_mode {
	VIC_CAP_SINGLE,
	VIC_CAP_CONT,
};

/* dma address update timing */
enum vic_update_timing {
	VIC_UD_TIMING_VBI,
	VIC_UD_TIMING_FCI,
	VIC_UD_TIMING_MANUALLY,
};

/* offset of dma_ofs and dma_size register between each dma chanel */
#define VIC_DMA_CH_WIN_OFFSET		0x20
/* offset of other register between each dma chanel */
#define VIC_DMA_CH_CFG_OFFSET		0x80

/*
 * DMA[CH] Interrupt Enable/Pending/Clear Register
 */

#define VIC_DMA_INT_FCI			(1UL << 0)
#define VIC_DMA_INT_VBI			(1UL << 1)
#define VIC_DMA_INT_LCTI		(1UL << 2)
#define VIC_DMA_INT_P0_OVF		(1UL << 8)
#define VIC_DMA_INT_P1_OVF		(1UL << 9)
#define VIC_DMA_INT_P2_OVF		(1UL << 10)

#define VIC_DMA_INT_ALL_MASK		(0xFFFFFFFFUL)
#define VIC_DMA_INT_ALL_ERR		\
		(VIC_DMA_INT_P0_OVF | VIC_DMA_INT_P1_OVF | \
		 VIC_DMA_INT_P2_OVF)

/******************************************************************************
 * MIPI_CSI
 *****************************************************************************/

enum mcsi_dt {
	/* short packet */
	MCSI_FS			= 0x00,
	MCSI_FE			= 0x01,
	MCSI_LS			= 0x02,
	MCSI_LE			= 0x03,
	MCSI_GSP0		= 0x08,
	MCSI_GSP1		= 0x09,
	MCSI_GSP2		= 0x0a,
	MCSI_GSP3		= 0x0b,
	MCSI_GSP4		= 0x0c,
	MCSI_GSP5		= 0x0d,
	MCSI_GSP6		= 0x0e,
	MCSI_GSP7		= 0x0f,
	/* long packet */
	MCSI_NULL		= 0x10,
	MCSI_BLK		= 0x11,
	MCSI_EMBD		= 0x12,
	MCSI_YUV420		= 0x18,
	MCSI_YUV420_10BIT	= 0x19,
	MCSI_YUV420_LGY		= 0x1a,
	MCSI_YUV420_CSPS	= 0x1c,
	MCSI_YUV420_CSPS_10BIT	= 0x1d,
	MCSI_YUV422		= 0x1e,
	MCSI_YUV422_10BIT	= 0x1f,
	MCSI_RGB565		= 0x22,
	MCSI_RGB888		= 0x24,
	MCSI_RAW8		= 0x2a,
	MCSI_RAW10		= 0x2b,
	MCSI_RAW12		= 0x2c,
	MCSI_UD0		= 0x30,
	MCSI_UD1		= 0x31,
	MCSI_UD2		= 0x32,
	MCSI_UD3		= 0x33,
	MCSI_UD4		= 0x34,
	MCSI_UD5		= 0x35,
	MCSI_UD6		= 0x36,
	MCSI_UD7		= 0x37,
};

enum mcsi_vc {
	MCSI_VC0 = 0,
	MCSI_VC1 = 1,
	MCSI_VC2 = 2,
	MCSI_VC3 = 3,
};

enum mcsi_id {
	MCSI_ID0 = 0,
	MCSI_ID1 = 1,
	MCSI_ID2 = 2,
	MCSI_ID3 = 3,
};


enum mcsi_data_path {
	MCSI_OUTPUT_DMA,
	MCSI_OUTPUT_PIXEL_CH,
	MCSI_OUTPUT_BOTH,
};

enum mcsi_burst_length {
	MCSI_DMA_BURST_LEN_1,
	MCSI_DMA_BURST_LEN_2,
	MCSI_DMA_BURST_LEN_4,
	MCSI_DMA_BURST_LEN_8,
};

/* output format */
enum mcsi_output_format {
	MCSI_OUTFMT_PAST_THROUGH,
	MCSI_OUTFMT_YUV422SP,
	MCSI_OUTFMT_YUV420SP,
	MCSI_OUTFMT_YUV422P,
	MCSI_OUTFMT_YUV420P,
};

/* yuv sequence of input source  */
enum mcsi_yuv_seq {
	MCSI_C0C1C2C3	= 0,
	MCSI_C2C1C0C3	= 1,
	MCSI_C1C0C3C2	= 2,
	MCSI_C3C0C1C2	= 3,
};

enum mcsi_capture_mode {
	MCSI_CAP_SINGLE,
	MCSI_CAP_CONT,
};

/* dma address update timing */
enum mcsi_update_timing {
	MCSI_UD_TIMING_VBI,
	MCSI_UD_TIMING_FCI,
	MCSI_UD_TIMING_MANUALLY,
};

/* Interrupt gate */
enum mcsi_int {
	MCSI_INT_DMA,
	MCSI_INT_MIPI,
};

/*
 * DMA Interrupt Enable/Pending/Clear Register
 */

#define MCSI_DMA_INT_FCI		(1UL << 0)
#define MCSI_DMA_INT_VBI		(1UL << 1)
#define MCSI_DMA_INT_LCTI		(1UL << 2)
#define MCSI_DMA_INT_P0_OVF		(1UL << 8)
#define MCSI_DMA_INT_P1_OVF		(1UL << 9)
#define MCSI_DMA_INT_P2_OVF		(1UL << 10)
#define MCSI_DMA_INT_LB_OR_EN           (1UL << 13)

#define MCSI_DMA_INT_ALL_MASK		(0xFFFFFFFFUL)
#define MCSI_DMA_INT_ALL_ERR		\
		(MCSI_DMA_INT_P0_OVF | MCSI_DMA_INT_P1_OVF | \
		 MCSI_DMA_INT_P2_OVF)

/******************************************************************************
 * ISP
 *****************************************************************************/




/******************************************************************************
 * ISP-LITE
 *****************************************************************************/


#endif /* ___VISS___CONST__H___ */
