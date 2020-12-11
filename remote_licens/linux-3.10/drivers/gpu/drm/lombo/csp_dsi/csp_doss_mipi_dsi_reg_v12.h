/* mipi_dsi.h */

#ifndef ___MIPI_DSI___H___
#define ___MIPI_DSI___H___

#define BASE_MIPI_DSI                0x01606000

#define VA_MIPI_DSI_VER		(0x00000000 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_FTR		(0x00000004 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_MIPI_DSI_CTRL (0x00000010 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_CLK_CFG	(0x00000014 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_TX_PKT_CFG	(0x00000018 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_RX_PKT_CFG	(0x0000001C + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_VM_DPI_IF_CFG (0x00000020 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_VM_CFG0	(0x00000024 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_VM_NULL_PKT	(0x00000028 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_VM_HT	(0x0000002C + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_VM_VT	(0x00000030 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_CM_DBI_IF_CFG (0x00000034 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_CM_DCS_CMD	(0x00000038 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_CM_CFG	(0x0000003C + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_CMD_PH	(0x00000040 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_CMD_PL_DATA	(0x00000044 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_INT_EN0	(0x0000004C + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_INT_EN1	(0x00000050 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_INT_PD0	(0x00000054 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_INT_PD1	(0x00000058 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_INT_CLR0	(0x0000005C + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_INT_CLR1	(0x00000060 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PHY_CTRL	(0x00000064 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PHY_OP_TIM	(0x00000068 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PHY_CFG	(0x0000006C + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PHY_IF_CFG	(0x00000070 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PHY_STA	(0x00000074 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_CTRL	(0x00000080 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_TX_CFG	(0x00000084 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_TX_START (0x00000088 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_TX_INIT (0x0000008C + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_ERRTE	(0x00000090 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_RX_CFG	(0x00000094 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_RX_START (0x00000098 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_RX_INIT (0x0000009C + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_CHK_NUM (0x000000A0 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_ERR_NUM (0x000000A4 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_RESULT	(0x000000A8 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_ERR_NUM_CNT0 (0x000000B0 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_ERR_NUM_CNT1 (0x000000B4 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_NLST	(0x000000B8 + BASE_MIPI_DSI + VA_MIPI_DSI)
#define VA_MIPI_DSI_PRBS_ELST	(0x000000BC + BASE_MIPI_DSI + VA_MIPI_DSI)

#define DATA_MIPI_DSI_VER                        0x00001102
#define DATA_MIPI_DSI_FTR                        0x00000003
#define DATA_MIPI_DSI_MIPI_DSI_CTRL              0x00000000
#define DATA_MIPI_DSI_CLK_CFG                    0x00000000
#define DATA_MIPI_DSI_TX_PKT_CFG                 0x00000000
#define DATA_MIPI_DSI_RX_PKT_CFG                 0x00000000
#define DATA_MIPI_DSI_VM_DPI_IF_CFG              0x00000000
#define DATA_MIPI_DSI_VM_CFG0                    0x00000000
#define DATA_MIPI_DSI_VM_NULL_PKT                0x00000000
#define DATA_MIPI_DSI_VM_HT                      0x00000000
#define DATA_MIPI_DSI_VM_VT                      0x00000000
#define DATA_MIPI_DSI_CM_DBI_IF_CFG              0x00008800
#define DATA_MIPI_DSI_CM_DCS_CMD                 0x00000000
#define DATA_MIPI_DSI_CM_CFG                     0x00000000
#define DATA_MIPI_DSI_CMD_PH                     0x00000000
#define DATA_MIPI_DSI_CMD_PL_DATA                0x00000000
#define DATA_MIPI_DSI_INT_EN0                    0x00000000
#define DATA_MIPI_DSI_INT_EN1                    0x00000000
#define DATA_MIPI_DSI_INT_PD0                    0x00000000
#define DATA_MIPI_DSI_INT_PD1                    0x00000000
#define DATA_MIPI_DSI_INT_CLR0                   0x00000000
#define DATA_MIPI_DSI_INT_CLR1                   0x00000000
#define DATA_MIPI_DSI_PHY_CTRL                   0x00000000
#define DATA_MIPI_DSI_PHY_OP_TIM                 0x00000000
#define DATA_MIPI_DSI_PHY_CFG                    0x00000003
#define DATA_MIPI_DSI_PHY_IF_CFG                 0x00000000
#define DATA_MIPI_DSI_PHY_STA                    0x00001528
#define DATA_MIPI_DSI_PRBS_CTRL                  0x00000000
#define DATA_MIPI_DSI_PRBS_TX_CFG                0x00000000
#define DATA_MIPI_DSI_PRBS_TX_START              0x00000000
#define DATA_MIPI_DSI_PRBS_TX_INIT               0x00000001
#define DATA_MIPI_DSI_PRBS_ERRTE                 0x00000000
#define DATA_MIPI_DSI_PRBS_RX_CFG                0x00000000
#define DATA_MIPI_DSI_PRBS_RX_START              0x00000000
#define DATA_MIPI_DSI_PRBS_RX_INIT               0x000000FF
#define DATA_MIPI_DSI_PRBS_CHK_NUM               0x10000000
#define DATA_MIPI_DSI_PRBS_ERR_NUM               0x00000001
#define DATA_MIPI_DSI_PRBS_RESULT                0x00000000
#define DATA_MIPI_DSI_PRBS_ERR_NUM_CNT0          0x00000000
#define DATA_MIPI_DSI_PRBS_ERR_NUM_CNT1          0x00000000
#define DATA_MIPI_DSI_PRBS_NLST                  0x00000000
#define DATA_MIPI_DSI_PRBS_ELST                  0x00000000

/* Version Register */
typedef union {
	u32 val;
	struct {
	u32 ver_l:5;            /* The low 5bits of version register */
	u32 rsvd0:3;            /* - */
	u32 ver_h:3;            /* The high 3bits of version register */
	u32 rsvd1:1;            /* - */
	u32 comp:1;
	u32 rsvd2:19;           /* - */
	} bits;
} reg_mipi_dsi_ver_t;

#define MIPI_DSI_VER_COMP_0     0x0
#define MIPI_DSI_VER_COMP_1     0x1

/* Feature Register */
typedef union {
	u32 val;
	struct {
	u32 has_mipi:1;	/* Indicates that the module has MIPI DSI
			 * interface function */
	u32 has_prbs:1;	/* Indicates that the module has PRBS function */
	u32 rsvd0:30;	/* - */
	} bits;
} reg_mipi_dsi_ftr_t;

#define MIPI_DSI_FTR_HAS_MIPI_0 0x0
#define MIPI_DSI_FTR_HAS_MIPI_1 0x1
#define MIPI_DSI_FTR_HAS_PRBS_0 0x0
#define MIPI_DSI_FTR_HAS_PRBS_1 0x1

/* MIPI DSI Control Register */
typedef union {
	u32 val;
	struct {
	u32 rstn:1;             /* Reset Control */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_mipi_dsi_mipi_dsi_ctrl_t;

#define MIPI_DSI_MIPI_DSI_CTRL_RSTN_0   0x0
#define MIPI_DSI_MIPI_DSI_CTRL_RSTN_1   0x1

/* Clock Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 lp_pre_div:8;       /* LP Mode CLK Pre-Diveder.
				 * LP_CLK = hsbyteclk / LP_PRE_DIV.
				 * 0 and 1 are illegal values. */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_mipi_dsi_clk_cfg_t;

/* TX Packet Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 eotp_tx_en:1;       /* Enables EOTp transmission. */
	u32 bta_en:1;           /* Enables Bus Turn-Around request. */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_mipi_dsi_tx_pkt_cfg_t;

#define MIPI_DSI_TX_PKT_CFG_EOTP_TX_EN_0        0x0
#define MIPI_DSI_TX_PKT_CFG_EOTP_TX_EN_1        0x1
#define MIPI_DSI_TX_PKT_CFG_BTA_EN_0            0x0
#define MIPI_DSI_TX_PKT_CFG_BTA_EN_1            0x1

/* RX Packet Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 eotp_rx_en:1;       /* Enables EOTp reception. */
	u32 ecc_en:1;           /* Enables ECC error correction && reporting */
	u32 crc_en:1;           /* Enables CRC error reporting. */
	u32 rsvd0:5;            /* - */
	u32 vc_rx:2;            /* Command interface read-back VC ID. */
	u32 rsvd1:22;           /* - */
	} bits;
} reg_mipi_dsi_rx_pkt_cfg_t;

#define MIPI_DSI_RX_PKT_CFG_EOTP_RX_EN_0        0x0
#define MIPI_DSI_RX_PKT_CFG_EOTP_RX_EN_1        0x1
#define MIPI_DSI_RX_PKT_CFG_ECC_EN_0            0x0
#define MIPI_DSI_RX_PKT_CFG_ECC_EN_1            0x1
#define MIPI_DSI_RX_PKT_CFG_CRC_EN_0            0x0
#define MIPI_DSI_RX_PKT_CFG_CRC_EN_1            0x1

/* Video Mode DPI Interface Register */
typedef union {
	u32 val;
	struct {
	u32 rgb666_l_en:1;      /* RGB666 loosely packed enable */
	u32 rsvd0:3;            /* - */
	u32 vc:2;               /* Configures the VC ID of the Video mode
				 * packets. */
	u32 cc:2;               /* Color Coding */
	u32 rsvd1:8;            /* - */
	u32 sd:1;               /* Shut Down Control */
	u32 cm:1;               /* Color Mode Control */
	u32 rsvd2:14;           /* - */
	} bits;
} reg_mipi_dsi_vm_dpi_if_cfg_t;

#define MIPI_DSI_VM_DPI_IF_CFG_RGB666_L_EN_0    0x0
#define MIPI_DSI_VM_DPI_IF_CFG_RGB666_L_EN_1    0x1
#define MIPI_DSI_VM_DPI_IF_CFG_CC_0             0x0
#define MIPI_DSI_VM_DPI_IF_CFG_CC_1             0x1
#define MIPI_DSI_VM_DPI_IF_CFG_CC_2             0x2
#define MIPI_DSI_VM_DPI_IF_CFG_CC_3             0x3
#define MIPI_DSI_VM_DPI_IF_CFG_SD_0             0x0
#define MIPI_DSI_VM_DPI_IF_CFG_SD_1             0x1
#define MIPI_DSI_VM_DPI_IF_CFG_CM_0             0x0
#define MIPI_DSI_VM_DPI_IF_CFG_CM_1             0x1

/* Video Mode Configuration 0 Register */
typedef union {
	u32 val;
	struct {
	u32 vm_en:1;            /* Enables the Video mode. */
	u32 bta_ack_en:1;       /* Enables the request for an acknowledge */
	u32 vm_pkt_seq:2;       /* Selects video mode transmission packet */
	u32 vp_px_num:11;       /* Configures the number of pixel on a
				 * single video packet.
				 * (If using 18 bit mode, this value must
				 * be a multiple of 4). */
	u32 rsvd0:1;            /* - */
	u32 vsw_mode:1;         /* Configures the mode during VSW period */
	u32 vbp_mode:1;         /* Configures the mode during VBP period */
	u32 vfp_mode:1;         /* Configures the mode during VFP period */
	u32 vact_mode:1;        /* Configures the mode during VACT period */
	u32 hbp_mode:1;         /* Configures the mode during HBP period */
	u32 hfp_mode:1;         /* Configures the transmission mode during */
	u32 rsvd1:10;           /* - */
	} bits;
} reg_mipi_dsi_vm_cfg0_t;

#define MIPI_DSI_VM_CFG0_VM_EN_0        0x0
#define MIPI_DSI_VM_CFG0_VM_EN_1        0x1
#define MIPI_DSI_VM_CFG0_BTA_ACK_EN_0   0x0
#define MIPI_DSI_VM_CFG0_BTA_ACK_EN_1   0x1
#define MIPI_DSI_VM_CFG0_VM_PKT_SEQ_0   0x0
#define MIPI_DSI_VM_CFG0_VM_PKT_SEQ_1   0x1
#define MIPI_DSI_VM_CFG0_VM_PKT_SEQ_2   0x2
#define MIPI_DSI_VM_CFG0_VM_PKT_SEQ_3   0x3
#define MIPI_DSI_VM_CFG0_VSW_MODE_0     0x0
#define MIPI_DSI_VM_CFG0_VSW_MODE_1     0x1
#define MIPI_DSI_VM_CFG0_VBP_MODE_0     0x0
#define MIPI_DSI_VM_CFG0_VBP_MODE_1     0x1
#define MIPI_DSI_VM_CFG0_VFP_MODE_0     0x0
#define MIPI_DSI_VM_CFG0_VFP_MODE_1     0x1
#define MIPI_DSI_VM_CFG0_VACT_MODE_0    0x0
#define MIPI_DSI_VM_CFG0_VACT_MODE_1    0x1
#define MIPI_DSI_VM_CFG0_HBP_MODE_0     0x0
#define MIPI_DSI_VM_CFG0_HBP_MODE_1     0x1
#define MIPI_DSI_VM_CFG0_HFP_MODE_0     0x0
#define MIPI_DSI_VM_CFG0_HFP_MODE_1     0x1
/* custom defines */
#define MIPI_DSI_VM_CFG_VM_EN_0         0x0
#define MIPI_DSI_VM_CFG_VM_EN_1         0x1
#define MIPI_DSI_VM_CFG_VM_TYPE_0       0x0
#define MIPI_DSI_VM_CFG_VM_TYPE_1       0x1
#define MIPI_DSI_VM_CFG_VM_TYPE_2       0x2
#define MIPI_DSI_VM_CFG_VM_TYPE_3       0x3
#define MIPI_DSI_VM_CFG_LP_VSA_EN_0     0x0
#define MIPI_DSI_VM_CFG_LP_VSA_EN_1     0x1
#define MIPI_DSI_VM_CFG_LP_VBP_EN_0     0x0
#define MIPI_DSI_VM_CFG_LP_VBP_EN_1     0x1
#define MIPI_DSI_VM_CFG_LP_VFP_EN_0     0x0
#define MIPI_DSI_VM_CFG_LP_VFP_EN_1     0x1
#define MIPI_DSI_VM_CFG_LP_VACT_EN_0    0x0
#define MIPI_DSI_VM_CFG_LP_VACT_EN_1    0x1
#define MIPI_DSI_VM_CFG_LP_HBP_EN_0     0x0
#define MIPI_DSI_VM_CFG_LP_HBP_EN_1     0x1
#define MIPI_DSI_VM_CFG_LP_HFP_EN_0     0x0
#define MIPI_DSI_VM_CFG_LP_HFP_EN_1     0x1
#define MIPI_DSI_VM_CFG_MULTI_PKT_EN_0  0x0
#define MIPI_DSI_VM_CFG_MULTI_PKT_EN_1  0x1
#define MIPI_DSI_VM_CFG_NULL_PKT_EN_0   0x0
#define MIPI_DSI_VM_CFG_NULL_PKT_EN_1   0x1
#define MIPI_DSI_VM_CFG_BTA_ACK_EN_0    0x0
#define MIPI_DSI_VM_CFG_BTA_ACK_EN_1    0x1

#define MIPI_DSI_DPI_CFG_CC_0           0x0
#define MIPI_DSI_DPI_CFG_CC_1           0x1
#define MIPI_DSI_DPI_CFG_CC_2           0x2
#define MIPI_DSI_DPI_CFG_CC_3           0x3
#define MIPI_DSI_DPI_CFG_DE_INV_0       0x0
#define MIPI_DSI_DPI_CFG_DE_INV_1       0x1
#define MIPI_DSI_DPI_CFG_VS_INV_0       0x0
#define MIPI_DSI_DPI_CFG_VS_INV_1       0x1
#define MIPI_DSI_DPI_CFG_HS_INV_0       0x0
#define MIPI_DSI_DPI_CFG_HS_INV_1       0x1
#define MIPI_DSI_DPI_CFG_SD_INV_0       0x0
#define MIPI_DSI_DPI_CFG_SD_INV_1       0x1
#define MIPI_DSI_DPI_CFG_CM_INV_0       0x0
#define MIPI_DSI_DPI_CFG_CM_INV_1       0x1
#define MIPI_DSI_DPI_CFG_RGB666_L_EN_0  0x0
#define MIPI_DSI_DPI_CFG_RGB666_L_EN_1  0x1
#define MIPI_DSI_DPI_CFG_SD_0           0x0
#define MIPI_DSI_DPI_CFG_SD_1           0x1
#define MIPI_DSI_DPI_CFG_CM_0           0x0
#define MIPI_DSI_DPI_CFG_CM_1           0x1

#define MIPI_DSI_DBI_CFG_IN_FMT_0       0x0
#define MIPI_DSI_DBI_CFG_IN_FMT_1       0x1
#define MIPI_DSI_DBI_CFG_IN_FMT_2       0x2
#define MIPI_DSI_DBI_CFG_IN_FMT_3       0x3
#define MIPI_DSI_DBI_CFG_IN_FMT_4       0x4
#define MIPI_DSI_DBI_CFG_IN_FMT_5       0x5
#define MIPI_DSI_DBI_CFG_IN_FMT_6       0x6
#define MIPI_DSI_DBI_CFG_IN_FMT_7       0x7
#define MIPI_DSI_DBI_CFG_IN_FMT_8       0x8
#define MIPI_DSI_DBI_CFG_IN_FMT_9       0x9
#define MIPI_DSI_DBI_CFG_IN_FMT_A       0xA
#define MIPI_DSI_DBI_CFG_IN_FMT_B       0xB
#define MIPI_DSI_DBI_CFG_IN_FMT_C       0xC
#define MIPI_DSI_DBI_CFG_LUT_SIZE_0     0x0
#define MIPI_DSI_DBI_CFG_LUT_SIZE_1     0x1
#define MIPI_DSI_DBI_CFG_LUT_SIZE_2     0x2
#define MIPI_DSI_DBI_CFG_LUT_SIZE_3     0x3
#define MIPI_DSI_DBI_CFG_PART_EN_0      0x0
#define MIPI_DSI_DBI_CFG_PART_EN_1      0x1
#define MIPI_DSI_DBI_CFG_OUT_FMT_0      0x0
#define MIPI_DSI_DBI_CFG_OUT_FMT_1      0x1
#define MIPI_DSI_DBI_CFG_OUT_FMT_2      0x2
#define MIPI_DSI_DBI_CFG_OUT_FMT_3      0x3
#define MIPI_DSI_DBI_CFG_OUT_FMT_4      0x4
#define MIPI_DSI_DBI_CFG_OUT_FMT_5      0x5
#define MIPI_DSI_DBI_CFG_OUT_FMT_6      0x6
#define MIPI_DSI_DBI_CFG_OUT_FMT_7      0x7
#define MIPI_DSI_DBI_CFG_OUT_FMT_8      0x8
#define MIPI_DSI_DBI_CFG_OUT_FMT_9      0x9
#define MIPI_DSI_DBI_CFG_OUT_FMT_A      0xA
#define MIPI_DSI_DBI_CFG_OUT_FMT_B      0xB
#define MIPI_DSI_DBI_CFG_OUT_FMT_C      0xC
/* end custom defines */

/* Video Mode NULL Packet Register */
typedef union {
	u32 val;
	struct {
	u32 multi_pkt_en:1;             /*  */
	u32 null_pkt_en:1;              /* */
	u32 pkt_num:10;                 /* Configures the packet number
					 * during a Line period. This packet
					 * number contain the video packets
					 * and the null packets. */
	u32 null_pkt_size:10;           /* Configures the number of the
					 * null packet. */
	u32 rsvd0:10;                   /* - */
	} bits;
} reg_mipi_dsi_vm_null_pkt_t;

#define MIPI_DSI_VM_NULL_PKT_MULTI_PKT_EN_0     0x0
#define MIPI_DSI_VM_NULL_PKT_MULTI_PKT_EN_1     0x1
#define MIPI_DSI_VM_NULL_PKT_NULL_PKT_EN_0      0x0
#define MIPI_DSI_VM_NULL_PKT_NULL_PKT_EN_1      0x1

/* custom define */
#define HS	(0)
#define LP	(1)

#define DSI_REG_NON_BURST_PULSE		MIPI_DSI_VM_CFG0_VM_PKT_SEQ_0
#define DSI_REG_NON_BURST_EVENT		MIPI_DSI_VM_CFG0_VM_PKT_SEQ_1
#define DSI_REG_BURST_PULSE		MIPI_DSI_VM_CFG0_VM_PKT_SEQ_2

#define DSI_REG_DPI_RGB888		MIPI_DSI_VM_DPI_IF_CFG_CC_0
#define DSI_REG_DPI_RGB666		MIPI_DSI_VM_DPI_IF_CFG_CC_1
#define DSI_REG_DPI_RGB565		MIPI_DSI_VM_DPI_IF_CFG_CC_2

#define DSI_REG_DBI_RGB565		MIPI_DSI_CM_DBI_IF_CFG_IN_FMT_0
#define DSI_REG_DBI_RGB666		MIPI_DSI_CM_DBI_IF_CFG_IN_FMT_1
#define DSI_REG_DBI_GB888		MIPI_DSI_CM_DBI_IF_CFG_IN_FMT_3

#define DSI_REG_DBI_LUT_16BPP		MIPI_DSI_CM_DBI_IF_CFG_LUT_SIZE_0
#define DSI_REG_DBI_LUT_18BPP		MIPI_DSI_CM_DBI_IF_CFG_LUT_SIZE_1
#define DSI_REG_DBI_LUT_24BPP		MIPI_DSI_CM_DBI_IF_CFG_LUT_SIZE_2

/* Video Mode Horizontal Timing Register */
typedef union {
	u32 val;
	struct {
	u32 hsw:9;      /* Configures the Horizontal Synchronism Active
			 * period in lane byte clock cycles. */
	u32 hbp:9;      /* Configures the Horizontal Back Porch period
			 * in lane byte clock cycles. */
	u32 ht:14;      /* Configures the size of the total line counted
			 * in lane byte cycles. */
	} bits;
} reg_mipi_dsi_vm_ht_t;

/* Video Mode Vertical Timing Register */
typedef union {
	u32 val;
	struct {
	u32 vsw:4;              /* Configures the Vertical Synchronism
				 * Active period measured in horizontal lines */
	u32 vbp:6;              /* Configures the Vertical Back Porch period
				 * measured in horizontal lines. */
	u32 vfp:6;              /* Configures the Vertical Front Porch
				 * period measured in horizontal lines. */
	u32 vact:11;            /* Configures the Vertical Active period
				 * measured in horizontal lines. */
	u32 rsvd0:5;            /* - */
	} bits;
} reg_mipi_dsi_vm_vt_t;

/* Command Mode DBI Interface Register */
typedef union {
	u32 val;
	struct {
	u32 wr_cont_en:1;       /* Enables the write_memory_continue commands */
	u32 rsvd0:3;            /* - */
	u32 vc:2;               /* Configures the VC ID of the command
				 * mode packets. */
	u32 lut_size:2;         /* Configures the size used to transport */
	u32 in_fmt:2;           /* Input pixel data format */
	u32 rsvd1:2;            /* - */
	u32 out_fmt:2;          /* Output pixel data format */
	u32 rsvd2:18;           /* - */
	} bits;
} reg_mipi_dsi_cm_dbi_if_cfg_t;

#define MIPI_DSI_CM_DBI_IF_CFG_WR_CONT_EN_0     0x0
#define MIPI_DSI_CM_DBI_IF_CFG_WR_CONT_EN_1     0x1
#define MIPI_DSI_CM_DBI_IF_CFG_LUT_SIZE_0       0x0
#define MIPI_DSI_CM_DBI_IF_CFG_LUT_SIZE_1       0x1
#define MIPI_DSI_CM_DBI_IF_CFG_LUT_SIZE_2       0x2
#define MIPI_DSI_CM_DBI_IF_CFG_LUT_SIZE_3       0x3
#define MIPI_DSI_CM_DBI_IF_CFG_IN_FMT_0         0x0
#define MIPI_DSI_CM_DBI_IF_CFG_IN_FMT_1         0x1
#define MIPI_DSI_CM_DBI_IF_CFG_IN_FMT_2         0x2
#define MIPI_DSI_CM_DBI_IF_CFG_IN_FMT_3         0x3
#define MIPI_DSI_CM_DBI_IF_CFG_OUT_FMT_0        0x0
#define MIPI_DSI_CM_DBI_IF_CFG_OUT_FMT_1        0x1
#define MIPI_DSI_CM_DBI_IF_CFG_OUT_FMT_2        0x2
#define MIPI_DSI_CM_DBI_IF_CFG_OUT_FMT_3        0x3

/* Command Mode DCS Command Register */
typedef union {
	u32 val;
	struct {
	u32 whole_cmd_size:16;          /* Configures the size of whole
					 * DCS write memory commands.
					 * WHOLE _CMD_SIZE = payload size -1 */
	u32 one_cmd_size:16;            /* Configures the size of one DCS
					 * write memory command. If a DCS
					 * command size exceed the CMD_SIZE,
					 * the DCS command will partition
					 * into several write memory continues.
					 * ONE_CMD_SIZE = payload size -1 */
	} bits;
} reg_mipi_dsi_cm_dcs_cmd_t;

/* Command Mode Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 cm_en:1;            /* Enables the Command Mode. */
	u32 ack_rq_en:1;        /* Enables the acknowledge request after */
	u32 tear_en:1;          /* Enables the tearing effect acknowledge */
	u32 rsvd0:5;            /* - */
	u32 cmd_sw_0p:1;        /* Configures the transmission mode of
				 * the Short Write Packet with 0 Parameters. */
	u32 cmd_sw_1p:1;        /* Configures the transmission mode of
				 * the Short Write Packet with 1 Parameters. */
	u32 cmd_sw_2p:1;        /* Configures the transmission mode of
				 * the Short Write Packet with 2 Parameters.*/
	u32 cmd_sr_0p:1;        /* Configures the transmission mode of
				 * the Short Read Packet with 0 Parameters. */
	u32 cmd_sr_1p:1;        /* Configures the transmission mode of
				 * the Short Read Packet with 1 Parameters. */
	u32 cmd_sr_2p:1;        /* Configures the transmission mode of
				 * the Short Read Packet with. 2 Parameters. */
	u32 dcs_sw_0p:1;        /* Configures the transmission mode of
				 * the DCS Short Write Packet with 0 Param. */
	u32 dcs_sw_1p:1;        /* Configures the transmission mode of
				 * the DCS Short Write Packet with 1 Param. */
	u32 dcs_sr_0p:1;        /* Configures the transmission mode of
				 * the DCS Short Read Packet with 0 Param. */
	u32 max_rd_pkt:1;       /* Configures the transmission mode of
				 * the Maximum Read Packet. */
	u32 cmd_lw:1;           /* Configures the transmission mode of
				 * the Long Write Packet. */
	u32 dcs_lw:1;           /* Configures the transmission mode of
				 * the DCS Long Write Packet. */
	u32 rsvd1:12;           /* - */
	} bits;
} reg_mipi_dsi_cm_cfg_t;

#define MIPI_DSI_CM_CFG_CM_EN_0         0x0
#define MIPI_DSI_CM_CFG_CM_EN_1         0x1
#define MIPI_DSI_CM_CFG_ACK_RQ_EN_0     0x0
#define MIPI_DSI_CM_CFG_ACK_RQ_EN_1     0x1
#define MIPI_DSI_CM_CFG_TEAR_EN_0       0x0
#define MIPI_DSI_CM_CFG_TEAR_EN_1       0x1
#define MIPI_DSI_CM_CFG_CMD_SW_0P_0     0x0
#define MIPI_DSI_CM_CFG_CMD_SW_0P_1     0x1
#define MIPI_DSI_CM_CFG_CMD_SW_1P_0     0x0
#define MIPI_DSI_CM_CFG_CMD_SW_1P_1     0x1
#define MIPI_DSI_CM_CFG_CMD_SW_2P_0     0x0
#define MIPI_DSI_CM_CFG_CMD_SW_2P_1     0x1
#define MIPI_DSI_CM_CFG_CMD_SR_0P_0     0x0
#define MIPI_DSI_CM_CFG_CMD_SR_0P_1     0x1
#define MIPI_DSI_CM_CFG_CMD_SR_1P_0     0x0
#define MIPI_DSI_CM_CFG_CMD_SR_1P_1     0x1
#define MIPI_DSI_CM_CFG_CMD_SR_2P_0     0x0
#define MIPI_DSI_CM_CFG_CMD_SR_2P_1     0x1
#define MIPI_DSI_CM_CFG_DCS_SW_0P_0     0x0
#define MIPI_DSI_CM_CFG_DCS_SW_0P_1     0x1
#define MIPI_DSI_CM_CFG_DCS_SW_1P_0     0x0
#define MIPI_DSI_CM_CFG_DCS_SW_1P_1     0x1
#define MIPI_DSI_CM_CFG_DCS_SR_0P_0     0x0
#define MIPI_DSI_CM_CFG_DCS_SR_0P_1     0x1
#define MIPI_DSI_CM_CFG_MAX_RD_PKT_0    0x0
#define MIPI_DSI_CM_CFG_MAX_RD_PKT_1    0x1
#define MIPI_DSI_CM_CFG_CMD_LW_0        0x0
#define MIPI_DSI_CM_CFG_CMD_LW_1        0x1
#define MIPI_DSI_CM_CFG_DCS_LW_0        0x0
#define MIPI_DSI_CM_CFG_DCS_LW_1        0x1

/* Command Packet Header Register */
typedef union {
	u32 val;
	struct {
	u32 di:8;               /* Configures the DI of the packet header.
				 * Writing to this register will trigger
				 * packet transmission. */
	u32 ph_data:16;         /* Configures the packet header data. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_mipi_dsi_cmd_ph_t;

/* Command Payload Data Register */
typedef union {
	u32 val;
	struct {
	u32 data:32;            /* Writes the packet payload.
				 * Reads the incoming command read data. */
	} bits;
} reg_mipi_dsi_cmd_pl_data_t;

/* Interrupt Enable 0 Register */
typedef union {
	u32 val;
	struct {
	u32 err_ack_sot:1;              /* SoT Error from Display Acknowledge
					 * Error Report. */
	u32 err_ack_sots:1;             /* SoT Sync Error from Display
					 * Acknowledge Error Report. */
	u32 err_ack_eots:1;             /* EoT Sync Error from Display
					 * Acknowledge Error Report. */
	u32 err_ack_ecmd:1;             /* Escape Mode Entry Command Error
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_lps:1;              /* Low-Power Transmit Sync Error
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_hsto:1;             /* HS Receive Timeout Error from
					 * Display Acknowledge Error Report. */
	u32 err_ack_fc:1;               /* False Control Error from Display
					 * Acknowledge Error Report. */
	u32 err_ack_rsvd00:1;           /* Reserved (specific to device)
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_ecc1:1;             /* ECC Error, single-bit (detected
					 * and corrected) from Display
					 * Acknowledge Error Report. */
	u32 err_ack_eccm:1;             /* ECC Error, multi-bit (detected,
					 * not corrected) from Display
					 * Acknowledge Error Report. */
	u32 err_ack_cks:1;              /* Checksum Error (Long packet
					 * only) from Display Acknowledge
					 * Error Report. */
	u32 err_ack_dt:1;               /* DSI Data Type Not Recognized
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_vid:1;              /* DSI VC ID Invalid from Display
					 * Acknowledge Error Report. */
	u32 err_ack_tl:1;               /* Invalid Transmission Length
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_rsvd11:1;           /* Reserved (specific to device)
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_pv:1;               /* DSI Protocol Violation from
					 * Display Acknowledge Error Report. */
	u32 err_entry_cmd:1;            /* Escape Entry Command Error from
					 * Lane 0. */
	u32 err_esc_sync:1;             /* Low-Power Data Transmission
					 * Synchronization Error from Lane 0. */
	u32 err_ctl:1;                  /* Control Error from Lane 0. */
	u32 err_lp0:1;                  /* LP0 Contention Error from Lane 0. */
	u32 err_lp1:1;                  /* LP1 Contention Error from Lane 0. */
	u32 rsvd2:11;                   /* - */
	} bits;
} reg_mipi_dsi_int_en0_t;

#define MIPI_DSI_INT_EN0_ERR_ACK_SOT_0          0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_SOT_1          0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_SOTS_0         0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_SOTS_1         0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_EOTS_0         0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_EOTS_1         0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_ECMD_0         0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_ECMD_1         0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_LPS_0          0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_LPS_1          0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_HSTO_0         0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_HSTO_1         0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_FC_0           0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_FC_1           0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_RSVD0_0        0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_RSVD0_1        0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_ECC1_0         0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_ECC1_1         0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_ECCM_0         0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_ECCM_1         0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_CKS_0          0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_CKS_1          0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_DT_0           0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_DT_1           0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_VID_0          0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_VID_1          0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_TL_0           0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_TL_1           0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_RSVD1_0        0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_RSVD1_1        0x1
#define MIPI_DSI_INT_EN0_ERR_ACK_PV_0           0x0
#define MIPI_DSI_INT_EN0_ERR_ACK_PV_1           0x1
#define MIPI_DSI_INT_EN0_ERR_ENTRY_CMD_0        0x0
#define MIPI_DSI_INT_EN0_ERR_ENTRY_CMD_1        0x1
#define MIPI_DSI_INT_EN0_ERR_ESC_SYNC_0         0x0
#define MIPI_DSI_INT_EN0_ERR_ESC_SYNC_1         0x1
#define MIPI_DSI_INT_EN0_ERR_CTL_0              0x0
#define MIPI_DSI_INT_EN0_ERR_CTL_1              0x1
#define MIPI_DSI_INT_EN0_ERR_LP0_0              0x0
#define MIPI_DSI_INT_EN0_ERR_LP0_1              0x1
#define MIPI_DSI_INT_EN0_ERR_LP1_0              0x0
#define MIPI_DSI_INT_EN0_ERR_LP1_1              0x1

/* Interrupt Enable 1 Register */
typedef union {
	u32 val;
	struct {
	u32 err_hstx_to:1;              /* High Speed Transmission Time
					 * Out Counter error. */
	u32 err_lprx_to:1;              /* Low Power Reception Time Out
					 * Counter error. */
	u32 err_ecc_single:1;           /* ECC single error. */
	u32 err_ecc_mult:1;             /* ECC multiple error. */
	u32 err_crc:1;                  /* CRC error.*/
	u32 err_pkt_size:1;             /* Rx receive packet size error. */
	u32 err_eotp:1;                 /* EOTp Packet not received error. */
	u32 err_dpi_wr:1;               /* DPI pixel line payload FIFO
					 * full error. */
	u32 err_ccmd_wr:1;              /* Command FIFO of command interface
					 * full error. */
	u32 err_cpl_wr:1;               /* Payload data FIFO of command
					 * interface full error. */
	u32 err_cpl_tx:1;               /* Command packet build FIFO empty
					 * error. */
	u32 err_cpl_rd:1;               /* Command packet read data payload
					 * FIFO empty error. */
	u32 err_cpl_rx:1;               /* Command packet read back FIFO
					 * full error. */
	u32 err_cmd_wr:1;               /* DBI command FIFO full error. */
	u32 err_pl_wr:1;                /* Write payload data DBI FIFO
					 * full error. */
	u32 err_pl_rd:1;                /* Payload DBI FIFO empty error,
					 * when sending a DCS read command. */
	u32 err_pl_rx:1;                /* DBI read back packet payload
					 * FIFO full error. */
	u32 err_ill_cmd:1;              /* Error attempt to write an illegal
					 * command on DBI. */
	u32 rsvd0:14;                   /* - */
	} bits;
} reg_mipi_dsi_int_en1_t;

#define MIPI_DSI_INT_EN1_ERR_HSTX_TO_0          0x0
#define MIPI_DSI_INT_EN1_ERR_HSTX_TO_1          0x1
#define MIPI_DSI_INT_EN1_ERR_LPRX_TO_0          0x0
#define MIPI_DSI_INT_EN1_ERR_LPRX_TO_1          0x1
#define MIPI_DSI_INT_EN1_ERR_ECC_SINGLE_0       0x0
#define MIPI_DSI_INT_EN1_ERR_ECC_SINGLE_1       0x1
#define MIPI_DSI_INT_EN1_ERR_ECC_MULT_0         0x0
#define MIPI_DSI_INT_EN1_ERR_ECC_MULT_1         0x1
#define MIPI_DSI_INT_EN1_ERR_CRC_0              0x0
#define MIPI_DSI_INT_EN1_ERR_CRC_1              0x1
#define MIPI_DSI_INT_EN1_ERR_PKT_SIZE_0         0x0
#define MIPI_DSI_INT_EN1_ERR_PKT_SIZE_1         0x1
#define MIPI_DSI_INT_EN1_ERR_EOTP_0             0x0
#define MIPI_DSI_INT_EN1_ERR_EOTP_1             0x1
#define MIPI_DSI_INT_EN1_ERR_DPI_WR_0           0x0
#define MIPI_DSI_INT_EN1_ERR_DPI_WR_1           0x1
#define MIPI_DSI_INT_EN1_ERR_CCMD_WR_0          0x0
#define MIPI_DSI_INT_EN1_ERR_CCMD_WR_1          0x1
#define MIPI_DSI_INT_EN1_ERR_CPL_WR_0           0x0
#define MIPI_DSI_INT_EN1_ERR_CPL_WR_1           0x1
#define MIPI_DSI_INT_EN1_ERR_CPL_TX_0           0x0
#define MIPI_DSI_INT_EN1_ERR_CPL_TX_1           0x1
#define MIPI_DSI_INT_EN1_ERR_CPL_RD_0           0x0
#define MIPI_DSI_INT_EN1_ERR_CPL_RD_1           0x1
#define MIPI_DSI_INT_EN1_ERR_CPL_RX_0           0x0
#define MIPI_DSI_INT_EN1_ERR_CPL_RX_1           0x1
#define MIPI_DSI_INT_EN1_ERR_CMD_WR_0           0x0
#define MIPI_DSI_INT_EN1_ERR_CMD_WR_1           0x1
#define MIPI_DSI_INT_EN1_ERR_PL_WR_0            0x0
#define MIPI_DSI_INT_EN1_ERR_PL_WR_1            0x1
#define MIPI_DSI_INT_EN1_ERR_PL_RD_0            0x0
#define MIPI_DSI_INT_EN1_ERR_PL_RD_1            0x1
#define MIPI_DSI_INT_EN1_ERR_PL_RX_0            0x0
#define MIPI_DSI_INT_EN1_ERR_PL_RX_1            0x1
#define MIPI_DSI_INT_EN1_ERR_ILL_CMD_0          0x0
#define MIPI_DSI_INT_EN1_ERR_ILL_CMD_1          0x1

/* Interrupt Pending 0 Register */
typedef union {
	u32 val;
	struct {
	u32 err_ack_sot:1;              /* Retrieves SoT Error from Display
					 * Acknowledge Error Report. */
	u32 err_ack_sots:1;             /* Retrieves SoT Sync Error from
					 * Display Acknowledge Error Report. */
	u32 err_ack_eots:1;             /* Retrieves EoT Sync Error from
					 * Display Acknowledge Error Report. */
	u32 err_ack_ecmd:1;             /* Retrieves Escape Mode Entry
					 * Command Error from Display
					 * Acknowledge Error Report. */
	u32 err_ack_lps:1;              /* Retrieves Low-Power Transmit
					 * Sync Error from Display Acknowledge
					 * Error Report. */
	u32 err_ack_hsto:1;             /* Retrieves HS Receive Timeout
					 * Error from Display Acknowledge
					 * Error Report. */
	u32 err_ack_fc:1;               /* Retrieves False Control Error
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_rsvd00:1;           /* Retrieves Reserved (specific
					 * to device) from Display Acknowledge
					 * Error Report. */
	u32 err_ack_ecc1:1;             /* Retrieves ECC Error, single-bit
					 * (detected and corrected) from
					 * Display Acknowledge Error Report. */
	u32 err_ack_eccm:1;             /* Retrieves ECC Error, multi-bit
					 * (detected, not corrected) from
					 * Display Acknowledge Error Report. */
	u32 err_ack_cks:1;              /* Retrieves Checksum Error (Long
					 * packet only) from Display Acknowledge
					 * Error Report. */
	u32 err_ack_dt:1;               /* Retrieves DSI Data Type Not
					 * Recognized from Display Acknowledge
					 * Error Report. */
	u32 err_ack_vid:1;              /* Retrieves DSI VC ID Invalid
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_tl:1;               /* Retrieves Invalid Transmission
					 * Length from Display Acknowledge
					 * Error Report. */
	u32 err_ack_rsvd11:1;           /* Retrieves Reserved (specific
					 * to device) from Display Acknowledge
					 * Error Report. */
	u32 err_ack_pv:1;               /* Retrieves DSI Protocol Violation
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_entry_cmd:1;            /* Escape Entry Command Error from
					 * Lane 0. */
	u32 err_esc_sync:1;             /* Low-Power Data Transmission
					 * Synchronization Error from Lane 0. */
	u32 err_ctl:1;                  /* Control Error from Lane 0. */
	u32 err_lp0:1;                  /* LP0 Contention Error from Lane 0. */
	u32 err_lp1:1;                  /* LP1 Contention Error from Lane 0. */
	u32 rsvd2:11;                   /* - */
	} bits;
} reg_mipi_dsi_int_pd0_t;

/* Interrupt Pending 1 Register */
typedef union {
	u32 val;
	struct {
	u32 err_hstx_to:1;              /* High Speed Transmission Time
					 * Out Counter reached the end and
					 * Contention Detection is detected. */
	u32 err_lprx_to:1;              /* Low Power Reception Time Out
					 * Counter reached the end and
					 * Contention Detection as been
					 * detected. */
	u32 err_ecc_single:1;           /* ECC single error is detected and
					 * corrected in a received packet. */
	u32 err_ecc_mult:1;             /* ECC multiple error is detected
					 * in a received packet. */
	u32 err_crc:1;                  /* CRC error is detected in the
					 * received packet payload. */
	u32 err_pkt_size:1;             /* Rx receive packet size error is
					 * detected during packet reception. */
	u32 err_eotp:1;                 /* EOTp Packet not received at
					 * the end of the incoming peripheral
					 * transmission. */
	u32 err_dpi_wr:1;               /* During a DPI pixel line storage
					 * the payload FIFO becomes full
					 * and data stored is corrupted. */
	u32 err_ccmd_wr:1;              /* System tries to write a command
					 * through the command interface
					 * and FIFO is full, therefore
					 * the command is not written. */
	u32 err_cpl_wr:1;               /* System tries to write a payload
					 * data through the command interface
					 * and FIFO is full, therefore
					 * the payload is not written. */
	u32 err_cpl_tx:1;               /* During a command interface packet
					 * build, the payload FIFO becomes
					 * empty and corrupt data is sent. */
	u32 err_cpl_rd:1;               /* During a command interface packet
					 * read back, the payload FIFO
					 * becomes empty and data is sent
					 * to the interface is corrupted. */
	u32 err_cpl_rx:1;               /* During a command interface packet
					 * read back, the payload FIFO
					 * becomes full and received data
					 * is corrupted. */
	u32 err_cmd_wr:1;               /* System tries to write a command
					 * through the DBI but the command
					 * FIFO is full, therefore the
					 * command is not written. */
	u32 err_pl_wr:1;                /* System tries to write payload
					 * data through the DBI interface
					 * and the FIFO is full, therefore
					 * the Command is not written. */
	u32 err_pl_rd:1;                /* During a DCS read data, the
					 * payload FIFO goes empty and
					 * data sent to the interface is
					 * corrupted. */
	u32 err_pl_rx:1;                /* During a DBI read back packet,
					 * the payload FIFO becomes full
					 * and received data is corrupted. */
	u32 err_ill_cmd:1;              /* Attempt to write an illegal
					 * command on the DBI interface
					 * and core blocked by transmission. */
	u32 rsvd0:14;                   /* - */
	} bits;
} reg_mipi_dsi_int_pd1_t;

/* Interrupt Clear 0 Register */
typedef union {
	u32 val;
	struct {
	u32 err_ack_sot:1;              /* SoT Error from Display Acknowledge
					 * Error Report. */
	u32 err_ack_sots:1;             /* SoT Sync Error from Display
					 * Acknowledge Error Report. */
	u32 err_ack_eots:1;             /* EoT Sync Error from Display
					 * Acknowledge Error Report.*/
	u32 err_ack_ecmd:1;             /* Escape Mode Entry Command Error
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_lps:1;              /* Low-Power Transmit Sync Error
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_hsto:1;             /* HS Receive Timeout Error from
					 * Display Acknowledge Error Report. */
	u32 err_ack_fc:1;               /* False Control Error from Display
					 * Acknowledge Error Report. */
	u32 err_ack_rsvd00:1;           /* Reserved (specific to device)
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_ecc1:1;             /* ECC Error, single-bit (detected
					 * and corrected) from Display
					 * Acknowledge Error Report. */
	u32 err_ack_eccm:1;             /* ECC Error, multi-bit (detected,
					 * not corrected) from Display
					 * Acknowledge Error Report. */
	u32 err_ack_cks:1;              /* Checksum Error (Long packet
					 * only) from Display Acknowledge */
	u32 err_ack_dt:1;               /* DSI Data Type Not Recognized
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_vid:1;              /* DSI VC ID Invalid from Display
					 * Acknowledge Error Report. */
	u32 err_ack_tl:1;               /* Invalid Transmission Length
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_rsvd11:1;           /* Reserved (specific to device)
					 * from Display Acknowledge Error
					 * Report. */
	u32 err_ack_pv:1;               /* DSI Protocol Violation from
					 * Display Acknowledge Error Report. */
	u32 err_entry_cmd:1;            /* Escape Entry Command Error from
					 * Lane 0. */
	u32 err_esc_sync:1;             /* Low-Power Data Transmission
					 * Synchronization Error from Lane 0. */
	u32 err_ctl:1;                  /* Control Error from Lane 0. */
	u32 err_lp0:1;                  /* LP0 Contention Error from Lane 0. */
	u32 err_lp1:1;                  /* LP1 Contention Error from Lane 0. */
	u32 rsvd2:11;                   /* - */
	} bits;
} reg_mipi_dsi_int_clr0_t;

#define MIPI_DSI_INT_CLR0_ERR_ACK_SOT_0         0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_SOT_1         0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_SOTS_0        0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_SOTS_1        0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_EOTS_0        0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_EOTS_1        0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_ECMD_0        0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_ECMD_1        0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_LPS_0         0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_LPS_1         0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_HSTO_0        0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_HSTO_1        0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_FC_0          0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_FC_1          0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_RSVD0_0       0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_RSVD0_1       0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_ECC1_0        0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_ECC1_1        0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_ECCM_0        0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_ECCM_1        0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_CKS_0         0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_CKS_1         0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_DT_0          0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_DT_1          0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_VID_0         0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_VID_1         0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_TL_0          0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_TL_1          0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_RSVD1_0       0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_RSVD1_1       0x1
#define MIPI_DSI_INT_CLR0_ERR_ACK_PV_0          0x0
#define MIPI_DSI_INT_CLR0_ERR_ACK_PV_1          0x1
#define MIPI_DSI_INT_CLR0_ERR_ENTRY_CMD_0       0x0
#define MIPI_DSI_INT_CLR0_ERR_ENTRY_CMD_1       0x1
#define MIPI_DSI_INT_CLR0_ERR_ESC_SYNC_0        0x0
#define MIPI_DSI_INT_CLR0_ERR_ESC_SYNC_1        0x1
#define MIPI_DSI_INT_CLR0_ERR_CTL_0             0x0
#define MIPI_DSI_INT_CLR0_ERR_CTL_1             0x1
#define MIPI_DSI_INT_CLR0_ERR_LP0_0             0x0
#define MIPI_DSI_INT_CLR0_ERR_LP0_1             0x1
#define MIPI_DSI_INT_CLR0_ERR_LP1_0             0x0
#define MIPI_DSI_INT_CLR0_ERR_LP1_1             0x1

/* Interrupt Clear 1 Register */
typedef union {
	u32 val;
	struct {
	u32 err_hstx_to:1;              /* High Speed Transmission Time
					 * Out Counter error. */
	u32 err_lprx_to:1;              /* Low Power Reception Time Out
					 * Counter error. */
	u32 err_ecc_single:1;           /* ECC single error. */
	u32 err_ecc_mult:1;             /* ECC multiple error. */
	u32 err_crc:1;                  /* CRC error. */
	u32 err_pkt_size:1;             /* Rx receive Packet size error. */
	u32 err_eotp:1;                 /* EOTp Packet not received error. */
	u32 err_dpi_wr:1;               /* DPI pixel line payload FIFO
					 * full error. */
	u32 err_ccmd_wr:1;              /* Command FIFO of command interface
					 * full error. */
	u32 err_cpl_wr:1;               /* Payload data FIFO of command
					 * interface full error. */
	u32 err_cpl_tx:1;               /* Command interface packet build
					 * FIFO empty error. */
	u32 err_cpl_rd:1;               /* Command interface read data
					 * payload FIFO empty error. */
	u32 err_cpl_rx:1;               /* Command interface packet read
					 * back FIFO full error. */
	u32 err_cmd_wr:1;               /* DBI command FIFO full error. */
	u32 err_pl_wr:1;                /* Write payload data DBI FIFO
					 * full error. */
	u32 err_pl_rd:1;                /* Payload DBI FIFO empty error. */
	u32 err_pl_rx:1;                /* DBI read back packet payload
					 * FIFO full error. */
	u32 err_ill_cmd:1;              /* Error attempt to write an illegal
					 * command on DBI. */
	u32 rsvd0:14;                   /* - */
	} bits;
} reg_mipi_dsi_int_clr1_t;

#define MIPI_DSI_INT_CLR1_ERR_HSTX_TO_0         0x0
#define MIPI_DSI_INT_CLR1_ERR_HSTX_TO_1         0x1
#define MIPI_DSI_INT_CLR1_ERR_LPRX_TO_0         0x0
#define MIPI_DSI_INT_CLR1_ERR_LPRX_TO_1         0x1
#define MIPI_DSI_INT_CLR1_ERR_ECC_SINGLE_0      0x0
#define MIPI_DSI_INT_CLR1_ERR_ECC_SINGLE_1      0x1
#define MIPI_DSI_INT_CLR1_ERR_ECC_MULT_0        0x0
#define MIPI_DSI_INT_CLR1_ERR_ECC_MULT_1        0x1
#define MIPI_DSI_INT_CLR1_ERR_CRC_0             0x0
#define MIPI_DSI_INT_CLR1_ERR_CRC_1             0x1
#define MIPI_DSI_INT_CLR1_ERR_PKT_SIZE_0        0x0
#define MIPI_DSI_INT_CLR1_ERR_PKT_SIZE_1        0x1
#define MIPI_DSI_INT_CLR1_ERR_EOTP_0            0x0
#define MIPI_DSI_INT_CLR1_ERR_EOTP_1            0x1
#define MIPI_DSI_INT_CLR1_ERR_DPI_WR_0          0x0
#define MIPI_DSI_INT_CLR1_ERR_DPI_WR_1          0x1
#define MIPI_DSI_INT_CLR1_ERR_CCMD_WR_0         0x0
#define MIPI_DSI_INT_CLR1_ERR_CCMD_WR_1         0x1
#define MIPI_DSI_INT_CLR1_ERR_CPL_WR_0          0x0
#define MIPI_DSI_INT_CLR1_ERR_CPL_WR_1          0x1
#define MIPI_DSI_INT_CLR1_ERR_CPL_TX_0          0x0
#define MIPI_DSI_INT_CLR1_ERR_CPL_TX_1          0x1
#define MIPI_DSI_INT_CLR1_ERR_CPL_RD_0          0x0
#define MIPI_DSI_INT_CLR1_ERR_CPL_RD_1          0x1
#define MIPI_DSI_INT_CLR1_ERR_CPL_RX_0          0x0
#define MIPI_DSI_INT_CLR1_ERR_CPL_RX_1          0x1
#define MIPI_DSI_INT_CLR1_ERR_CMD_WR_0          0x0
#define MIPI_DSI_INT_CLR1_ERR_CMD_WR_1          0x1
#define MIPI_DSI_INT_CLR1_ERR_PL_WR_0           0x0
#define MIPI_DSI_INT_CLR1_ERR_PL_WR_1           0x1
#define MIPI_DSI_INT_CLR1_ERR_PL_RD_0           0x0
#define MIPI_DSI_INT_CLR1_ERR_PL_RD_1           0x1
#define MIPI_DSI_INT_CLR1_ERR_PL_RX_0           0x0
#define MIPI_DSI_INT_CLR1_ERR_PL_RX_1           0x1
#define MIPI_DSI_INT_CLR1_ERR_ILL_CMD_0         0x0
#define MIPI_DSI_INT_CLR1_ERR_ILL_CMD_1         0x1

/* PHY Control Register */
typedef union {
	u32 val;
	struct {
	u32 phy_shut:1;              /* D-PHY Shutdown */
	u32 phy_rstn:1;              /* D-PHY Reset */
	u32 phy_clk_en:1;            /* Enables D-PHY Clock Lane Module. */
	u32 hs_clk_en:1;             /* Enable the HS clock. */
	u32 ulps_rq_clk:1;           /* Requests ULPS mode on Clock Lane. */
	u32 ulps_exit_clk:1;         /* Exits ULPS mode on Clock Lane. */
	u32 ulps_rq_dat:1;           /* Requests ULPS mode on all data lanes. */
	u32 ulps_exit_dat:1;         /* Exits ULPS mode on all data lanes. */
	u32 tx_trig_mode:4;          /* Controls the trigger mode.
					 * [0x1]: Reset-Trigger
					 * [0x2]: Unknown-3
					 * [0x4]: Unknown-4
					 * [0x8]: Unknown-5
					 * Others: Reserved */
	u32 rsvd0:20;                /* - */
	} bits;
} reg_mipi_dsi_phy_ctrl_t;

#define MIPI_DSI_PHY_CTRL_PHY_SHUT_0            0x0
#define MIPI_DSI_PHY_CTRL_PHY_SHUT_1            0x1
#define MIPI_DSI_PHY_CTRL_PHY_RSTN_0            0x0
#define MIPI_DSI_PHY_CTRL_PHY_RSTN_1            0x1
#define MIPI_DSI_PHY_CTRL_PHY_CLK_EN_0          0x0
#define MIPI_DSI_PHY_CTRL_PHY_CLK_EN_1          0x1
#define MIPI_DSI_PHY_CTRL_TX_TRIG_MODE_1        0x1
#define MIPI_DSI_PHY_CTRL_TX_TRIG_MODE_2        0x2
#define MIPI_DSI_PHY_CTRL_TX_TRIG_MODE_4        0x4
#define MIPI_DSI_PHY_CTRL_TX_TRIG_MODE_8        0x8

/* PHY Operation Timing Register */
typedef union {
	u32 val;
	struct {
	u32 bta_cnt:12;         /* Configures the maximum time required
				 * to perform the Bus Turn Around operation.
				 * This time is count by hsbyteclk. */
	u32 lp2hs_cnt:8;        /* Configures the maximum time that the
				 * PHY takes to go from LP to HS transmission.
				 * This time is count by hsbyteclk. */
	u32 hs2lp_cnt:8;        /* Configures the maximum time that the
				 * PHY takes to go from HS to LP transmission.
				 * This time is count by hsbyteclk. */
	u32 rsvd0:4;            /* - */
	} bits;
} reg_mipi_dsi_phy_op_tim_t;

/* PHY Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 lane_num:2;         /* Number of active data lanes. */
	u32 stop_wt:8;          /* Configures wait cycles to request another
				 * HS packet.
				 * This time is count by hsbitclk. */
	u32 rsvd0:22;           /* - */
	} bits;
} reg_mipi_dsi_phy_cfg_t;

#define MIPI_DSI_PHY_CFG_LANE_NUM_0     0x0
#define MIPI_DSI_PHY_CFG_LANE_NUM_1     0x1
#define MIPI_DSI_PHY_CFG_LANE_NUM_2     0x2
#define MIPI_DSI_PHY_CFG_LANE_NUM_3     0x3

/* PHY Interface Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 send:1;             /* Triggers a read/write operation on the
				 * phy interface.,aute cleared by hardware
				 * when the operation has been finished. */
	u32 rw:1;               /* Reads or writes operation on the phy
				 * interface. */
	u32 rsvd0:5;            /* - */
	u32 phy_if_rst:1;       /* PHY Interface reset. */
	u32 phy_add:8;          /* The phy register address to be R/W. */
	u32 wr_data:8;          /* Data write into the phy register. */
	u32 rd_data:8;          /* Data read back from the phy register. */
	} bits;
} reg_mipi_dsi_phy_if_cfg_t;

#define MIPI_DSI_PHY_IF_CFG_RW_0                0x0
#define MIPI_DSI_PHY_IF_CFG_RW_1                0x1
#define MIPI_DSI_PHY_IF_CFG_PHY_IF_RST_0        0x0
#define MIPI_DSI_PHY_IF_CFG_PHY_IF_RST_1        0x1

/* PHY Status Register */
typedef union {
	u32 val;
	struct {
	u32 lock:1;             /* Reports status of phylock D-PHY pin. */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_mipi_dsi_phy_sta_t;

/* PRBS Control Register */
typedef union {
	u32 val;
	struct {
	u32 s2prbs:1;           /* MIPI D-PHY switch to PRBS mode. */
	u32 state:1;            /* Config MIPI D-PHY state. */
	u32 rsvd0:2;            /* - */
	u32 basdir_d0:1;        /* Direction of data lane0. */
	u32 basdir_d1:1;        /* Direction of data lane1. */
	u32 basdir_d2:1;        /* Direction of data lane2. */
	u32 basdir_d3:1;        /* Direction of data lane3. */
	u32 rsvd1:24;           /* - */
	} bits;
} reg_mipi_dsi_prbs_ctrl_t;

#define MIPI_DSI_PRBS_CTRL_S2PRBS_0     0x0
#define MIPI_DSI_PRBS_CTRL_S2PRBS_1     0x1
#define MIPI_DSI_PRBS_CTRL_STATE_0      0x0
#define MIPI_DSI_PRBS_CTRL_STATE_1      0x1
#define MIPI_DSI_PRBS_CTRL_BASDIR_D0_0  0x0
#define MIPI_DSI_PRBS_CTRL_BASDIR_D0_1  0x1
#define MIPI_DSI_PRBS_CTRL_BASDIR_D1_0  0x0
#define MIPI_DSI_PRBS_CTRL_BASDIR_D1_1  0x1
#define MIPI_DSI_PRBS_CTRL_BASDIR_D2_0  0x0
#define MIPI_DSI_PRBS_CTRL_BASDIR_D2_1  0x1
#define MIPI_DSI_PRBS_CTRL_BASDIR_D3_0  0x0
#define MIPI_DSI_PRBS_CTRL_BASDIR_D3_1  0x1

/* PRBS TX Config Register */
typedef union {
	u32 val;
	struct {
	u32 tx_order:1;         /* PRBS TX order configuration. */
	u32 tx_type:3;          /* PRBS TX type configuration. */
	u32 rsvd0:3;            /* - */
	u32 tx_enable:1;        /* PRBS TX enable. */
	u32 rsvd1:24;           /* - */
	} bits;
} reg_mipi_dsi_prbs_tx_cfg_t;

#define MIPI_DSI_PRBS_TX_CFG_TX_ORDER_0         0x0
#define MIPI_DSI_PRBS_TX_CFG_TX_ORDER_1         0x1
#define MIPI_DSI_PRBS_TX_CFG_TX_TYPE_0          0x0
#define MIPI_DSI_PRBS_TX_CFG_TX_ENABLE_0        0x0
#define MIPI_DSI_PRBS_TX_CFG_TX_ENABLE_1        0x1

/* PRBS TX Start Register */
typedef union {
	u32 val;
	struct {
	u32 tx_start:1;         /* Start PRBS TX. */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_mipi_dsi_prbs_tx_start_t;

#define MIPI_DSI_PRBS_TX_START_TX_START_0       0x0
#define MIPI_DSI_PRBS_TX_START_TX_START_1       0x1

/* PRBS TX Initialization Register */
typedef union {
	u32 val;
	struct {
	u32 tx_init:8;	/* PRBS TX initial value, which should not be 0. */
	u32 rsvd0:24;	/* - */
	} bits;
} reg_mipi_dsi_prbs_tx_init_t;

/* PRBS Errte Register */
typedef union {
	u32 val;
	struct {
	u32 errte:1;            /* PRBS error TX enable. */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_mipi_dsi_prbs_errte_t;

#define MIPI_DSI_PRBS_ERRTE_ERRTE_0     0x0
#define MIPI_DSI_PRBS_ERRTE_ERRTE_1     0x1

/* PRBS RX Config Register */
typedef union {
	u32 val;
	struct {
	u32 rx_order:1;         /* PRBS RX order configuration. */
	u32 rx_type:3;          /* PRBS RX type configuration. */
	u32 dual_sync:1;        /* Common header dual sync mode. */
	u32 rsvd0:2;            /* - */
	u32 rx_enable:1;        /* PRBS RX enable. */
	u32 rsvd1:24;           /* - */
	} bits;
} reg_mipi_dsi_prbs_rx_cfg_t;

#define MIPI_DSI_PRBS_RX_CFG_RX_ORDER_0         0x0
#define MIPI_DSI_PRBS_RX_CFG_RX_ORDER_1         0x1
#define MIPI_DSI_PRBS_RX_CFG_RX_TYPE_0          0x0
#define MIPI_DSI_PRBS_RX_CFG_DUAL_SYNC_0        0x0
#define MIPI_DSI_PRBS_RX_CFG_DUAL_SYNC_1        0x1
#define MIPI_DSI_PRBS_RX_CFG_RX_ENABLE_0        0x0
#define MIPI_DSI_PRBS_RX_CFG_RX_ENABLE_1        0x1

/* PRBS RX Start Register */
typedef union {
	u32 val;
	struct {
	u32 rx_start:1;         /* Start PRBS RX.
				 * [0x0]: Stop
				 * [0x1]: Start */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_mipi_dsi_prbs_rx_start_t;

#define MIPI_DSI_PRBS_RX_START_RX_START_0       0x0
#define MIPI_DSI_PRBS_RX_START_RX_START_1       0x1

/* PRBS RX Initialization Register */
typedef union {
	u32 val;
	struct {
	u32 rx_init:8;          /* PRBS initial value, which should not be 0. */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_mipi_dsi_prbs_rx_init_t;

/* PRBS Check Number Register */
typedef union {
	u32 val;
	struct {
	u32 cnum:32;            /* PRBS check number. */
	} bits;
} reg_mipi_dsi_prbs_chk_num_t;

/* PRBS Error Number Register */
typedef union {
	u32 val;
	struct {
	u32 ernum:16;           /* PRBS error number. */
	u32 rsvd0:16;           /* - */
	} bits;
} reg_mipi_dsi_prbs_err_num_t;

/* PRBS Result Register */
typedef union {
	u32 val;
	struct {
	u32 dl0_sd:1;           /* Indicate data lane 0 sync done state. */
	u32 dl1_sd:1;           /* Indicate data lane 1 sync done state. */
	u32 dl2_sd:1;           /* Indicate data lane 2 sync done state. */
	u32 dl3_sd:1;           /* Indicate data lane 3 sync done state. */
	u32 dl0_se:1;           /* Indicate data lane 0 sync error state. */
	u32 dl1_se:1;           /* Indicate data lane 1 sync error state. */
	u32 dl2_se:1;           /* Indicate data lane 2 sync error state. */
	u32 dl3_se:1;           /* Indicate data lane 3 sync error state. */
	u32 dl0_cd:1;           /* Indicate data lane 0 check done state. */
	u32 dl1_cd:1;           /* Indicate data lane 1 check done state. */
	u32 dl2_cd:1;           /* Indicate data lane 2 check done state. */
	u32 dl3_cd:1;           /* Indicate data lane 3 check done state. */
	u32 dl0_ce:1;           /* Indicate data lane 0 check error state. */
	u32 dl1_ce:1;           /* Indicate data lane 1 check error state. */
	u32 dl2_ce:1;           /* Indicate data lane 2 check error state. */
	u32 dl3_ce:1;           /* Indicate data lane 3 check error state. */
	u32 dl0_cr:1;           /* Indicate data lane 0 check result. */
	u32 dl1_cr:1;           /* Indicate data lane 1 check result. */
	u32 dl2_cr:1;           /* Indicate data lane 2 check result. */
	u32 dl3_cr:1;           /* Indicate data lane 3 check result. */
	u32 rsvd0:12;           /* - */
	} bits;
} reg_mipi_dsi_prbs_result_t;

/* PRBS Error Number Counter Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dl0:16;     /* Indicate data lane 0 error number. */
	u32 dl1:16;     /* Indicate data lane 1 error number. */
	} bits;
} reg_mipi_dsi_prbs_err_num_cnt0_t;

/* PRBS Error Number Counter Register 1 */
typedef union {
	u32 val;
	struct {
	u32 dl2:16;     /* Indicate data lane 2 error number. */
	u32 dl3:16;     /* Indicate data lane 3 error number. */
	} bits;
} reg_mipi_dsi_prbs_err_num_cnt1_t;

/* PRBS Normal Last Register */
typedef union {
	u32 val;
	struct {
	u32 dl0:8;      /* Indicate data lane 0 normal last data. */
	u32 dl1:8;      /* Indicate data lane 1 normal last data. */
	u32 dl2:8;      /* Indicate data lane 2 normal last data. */
	u32 dl3:8;      /* Indicate data lane 3 normal last data. */
	} bits;
} reg_mipi_dsi_prbs_nlst_t;

/* PRBS Error Last Register */
typedef union {
	u32 val;
	struct {
	u32 dl0:8;      /* Indicate data lane 0 error last data. */
	u32 dl1:8;      /* Indicate data lane 1 error last data. */
	u32 dl2:8;      /* Indicate data lane 2 error last data. */
	u32 dl3:8;      /* Indicate data lane 3 error last data. */
	} bits;
} reg_mipi_dsi_prbs_elst_t;

/* reg_mipi_dsi_t bank */
typedef struct tag_mipi_dsi {
	reg_mipi_dsi_ver_t                 ver;                /* 0000 */
	reg_mipi_dsi_ftr_t                 ftr;                /* 0004 */
	u32                                res0[2];
	reg_mipi_dsi_mipi_dsi_ctrl_t       mipi_dsi_ctrl;      /* 0010 */
	reg_mipi_dsi_clk_cfg_t             clk_cfg;            /* 0014 */
	reg_mipi_dsi_tx_pkt_cfg_t          tx_pkt_cfg;         /* 0018 */
	reg_mipi_dsi_rx_pkt_cfg_t          rx_pkt_cfg;         /* 001C */
	reg_mipi_dsi_vm_dpi_if_cfg_t       vm_dpi_if_cfg;      /* 0020 */
	reg_mipi_dsi_vm_cfg0_t             vm_cfg0;            /* 0024 */
	reg_mipi_dsi_vm_null_pkt_t         vm_null_pkt;        /* 0028 */
	reg_mipi_dsi_vm_ht_t               vm_ht;              /* 002C */
	reg_mipi_dsi_vm_vt_t               vm_vt;              /* 0030 */
	reg_mipi_dsi_cm_dbi_if_cfg_t       cm_dbi_if_cfg;      /* 0034 */
	reg_mipi_dsi_cm_dcs_cmd_t          cm_dcs_cmd;         /* 0038 */
	reg_mipi_dsi_cm_cfg_t              cm_cfg;             /* 003C */
	reg_mipi_dsi_cmd_ph_t              cmd_ph;             /* 0040 */
	reg_mipi_dsi_cmd_pl_data_t         cmd_pl_data;        /* 0044 */
	u32                                res1[1];
	reg_mipi_dsi_int_en0_t             int_en0;            /* 004C */
	reg_mipi_dsi_int_en1_t             int_en1;            /* 0050 */
	reg_mipi_dsi_int_pd0_t             int_pd0;            /* 0054 */
	reg_mipi_dsi_int_pd1_t             int_pd1;            /* 0058 */
	reg_mipi_dsi_int_clr0_t            int_clr0;           /* 005C */
	reg_mipi_dsi_int_clr1_t            int_clr1;           /* 0060 */
	reg_mipi_dsi_phy_ctrl_t            phy_ctrl;           /* 0064 */
	reg_mipi_dsi_phy_op_tim_t          phy_op_tim;         /* 0068 */
	reg_mipi_dsi_phy_cfg_t             phy_cfg;            /* 006C */
	reg_mipi_dsi_phy_if_cfg_t          phy_if_cfg;         /* 0070 */
	reg_mipi_dsi_phy_sta_t             phy_sta;            /* 0074 */
	u32                                res2[2];
	reg_mipi_dsi_prbs_ctrl_t           prbs_ctrl;          /* 0080 */
	reg_mipi_dsi_prbs_tx_cfg_t         prbs_tx_cfg;        /* 0084 */
	reg_mipi_dsi_prbs_tx_start_t       prbs_tx_start;      /* 0088 */
	reg_mipi_dsi_prbs_tx_init_t        prbs_tx_init;       /* 008C */
	reg_mipi_dsi_prbs_errte_t          prbs_errte;         /* 0090 */
	reg_mipi_dsi_prbs_rx_cfg_t         prbs_rx_cfg;        /* 0094 */
	reg_mipi_dsi_prbs_rx_start_t       prbs_rx_start;      /* 0098 */
	reg_mipi_dsi_prbs_rx_init_t        prbs_rx_init;       /* 009C */
	reg_mipi_dsi_prbs_chk_num_t        prbs_chk_num;       /* 00A0 */
	reg_mipi_dsi_prbs_err_num_t        prbs_err_num;       /* 00A4 */
	reg_mipi_dsi_prbs_result_t         prbs_result;        /* 00A8 */
	u32                                res3[1];
	reg_mipi_dsi_prbs_err_num_cnt0_t   prbs_err_num_cnt0;  /* 00B0 */
	reg_mipi_dsi_prbs_err_num_cnt1_t   prbs_err_num_cnt1;  /* 00B4 */
	reg_mipi_dsi_prbs_nlst_t           prbs_nlst;          /* 00B8 */
	reg_mipi_dsi_prbs_elst_t           prbs_elst;          /* 00BC */
} reg_mipi_dsi_t;

#endif /* ___MIPI_DSI___H___ */
