/* vic.h */

#ifndef ___VIC___H___
#define ___VIC___H___

#define BASE_VIC                0x01410000

#define VA_VIC_VER                       (0x00000000 + BASE_VIC + VA_VIC)
#define VA_VIC_FTR                       (0x00000004 + BASE_VIC + VA_VIC)
#define VA_VIC_VIC_CTRL                  (0x00000010 + BASE_VIC + VA_VIC)
#define VA_VIC_VIC_CFG                   (0x00000014 + BASE_VIC + VA_VIC)
#define VA_VIC_IO_CFG                    (0x0000001C + BASE_VIC + VA_VIC)
#define VA_VIC_IF_CFG                    (0x00000020 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA_MULTI_CH              (0x00000030 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA_MULTI_ID              (0x00000034 + BASE_VIC + VA_VIC)
#define VA_VIC_HFB_OFS                   (0x00000080 + BASE_VIC + VA_VIC)
#define VA_VIC_HACT_WIDTH                (0x00000084 + BASE_VIC + VA_VIC)
#define VA_VIC_F1_VFB_OFS                (0x00000090 + BASE_VIC + VA_VIC)
#define VA_VIC_F1_VACT_LINE              (0x00000094 + BASE_VIC + VA_VIC)
#define VA_VIC_F2_VFB_OFS                (0x00000098 + BASE_VIC + VA_VIC)
#define VA_VIC_F2_VACT_LINE              (0x0000009C + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_OFS             (0x00000100 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_OFS             (0x00000120 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_OFS             (0x00000140 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_OFS             (0x00000160 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_SIZE            (0x00000104 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_SIZE            (0x00000124 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_SIZE            (0x00000144 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_SIZE            (0x00000164 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_MODE            (0x00000180 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_MODE            (0x00000200 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_MODE            (0x00000280 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_MODE            (0x00000300 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_CTRL            (0x00000184 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_CTRL            (0x00000204 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_CTRL            (0x00000284 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_CTRL            (0x00000304 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_UPDATE          (0x00000188 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_UPDATE          (0x00000208 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_UPDATE          (0x00000288 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_UPDATE          (0x00000308 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_ADDR0           (0x00000190 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_ADDR0           (0x00000210 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_ADDR0           (0x00000290 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_ADDR0           (0x00000310 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_ADDR1           (0x00000194 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_ADDR1           (0x00000214 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_ADDR1           (0x00000294 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_ADDR1           (0x00000314 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_ADDR2           (0x00000198 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_ADDR2           (0x00000218 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_ADDR2           (0x00000298 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_ADDR2           (0x00000318 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_LS              (0x0000019C + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_LS              (0x0000021C + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_LS              (0x0000029C + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_LS              (0x0000031C + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_BC              (0x000001B0 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_BC              (0x00000230 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_BC              (0x000002B0 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_BC              (0x00000330 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_INT_EN          (0x000001C0 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_INT_EN          (0x00000240 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_INT_EN          (0x000002C0 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_INT_EN          (0x00000340 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_INT_PEND        (0x000001C4 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_INT_PEND        (0x00000244 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_INT_PEND        (0x000002C4 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_INT_PEND        (0x00000344 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_INT_CLR         (0x000001C8 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_INT_CLR         (0x00000248 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_INT_CLR         (0x000002C8 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_INT_CLR         (0x00000348 + BASE_VIC + VA_VIC)
#define VA_VIC_DMA0_LINE_CNT        (0x000001CC + BASE_VIC + VA_VIC)
#define VA_VIC_DMA1_LINE_CNT        (0x0000024C + BASE_VIC + VA_VIC)
#define VA_VIC_DMA2_LINE_CNT        (0x000002CC + BASE_VIC + VA_VIC)
#define VA_VIC_DMA3_LINE_CNT        (0x0000034C + BASE_VIC + VA_VIC)
#define VA_VIC_DEBUG_CTRL                (0x00000600 + BASE_VIC + VA_VIC)
#define VA_VIC_TRC_DEBUG                 (0x00000608 + BASE_VIC + VA_VIC)
#define VA_VIC_SIGNAL_DEBUG              (0x0000060C + BASE_VIC + VA_VIC)
#define VA_VIC_DVP_PCLK_CNT              (0x00000610 + BASE_VIC + VA_VIC)
#define VA_VIC_DVP_VSYNC_CNT             (0x00000614 + BASE_VIC + VA_VIC)
#define VA_VIC_DVP_HS_PRD_CNT            (0x00000618 + BASE_VIC + VA_VIC)
#define VA_VIC_DVP_VS_PRD_CNT            (0x0000061C + BASE_VIC + VA_VIC)

#define DATA_VIC_VER                     0x00001101
#define DATA_VIC_FTR                     0x000F0103
#define DATA_VIC_VIC_CTRL                0x00000000
#define DATA_VIC_VIC_CFG                 0x00000000
#define DATA_VIC_IO_CFG                  0x00000000
#define DATA_VIC_IF_CFG                  0x00000000
#define DATA_VIC_DMA_MULTI_CH            0x00000000
#define DATA_VIC_DMA_MULTI_ID            0x03020100
#define DATA_VIC_HFB_OFS                 0x00000000
#define DATA_VIC_HACT_WIDTH              0x00000000
#define DATA_VIC_F1_VFB_OFS              0x00000000
#define DATA_VIC_F1_VACT_LINE            0x00000000
#define DATA_VIC_F2_VFB_OFS              0x00000000
#define DATA_VIC_F2_VACT_LINE            0x00000000
#define DATA_VIC_DMA0_OFS           0x00000000
#define DATA_VIC_DMA1_OFS           0x00000000
#define DATA_VIC_DMA2_OFS           0x00000000
#define DATA_VIC_DMA3_OFS           0x00000000
#define DATA_VIC_DMA0_SIZE          0x00000000
#define DATA_VIC_DMA1_SIZE          0x00000000
#define DATA_VIC_DMA2_SIZE          0x00000000
#define DATA_VIC_DMA3_SIZE          0x00000000
#define DATA_VIC_DMA0_MODE          0x00000200
#define DATA_VIC_DMA1_MODE          0x00000200
#define DATA_VIC_DMA2_MODE          0x00000200
#define DATA_VIC_DMA3_MODE          0x00000200
#define DATA_VIC_DMA0_CTRL          0x00000000
#define DATA_VIC_DMA1_CTRL          0x00000000
#define DATA_VIC_DMA2_CTRL          0x00000000
#define DATA_VIC_DMA3_CTRL          0x00000000
#define DATA_VIC_DMA0_UPDATE        0x00000000
#define DATA_VIC_DMA1_UPDATE        0x00000000
#define DATA_VIC_DMA2_UPDATE        0x00000000
#define DATA_VIC_DMA3_UPDATE        0x00000000
#define DATA_VIC_DMA0_ADDR0         0x00000000
#define DATA_VIC_DMA1_ADDR0         0x00000000
#define DATA_VIC_DMA2_ADDR0         0x00000000
#define DATA_VIC_DMA3_ADDR0         0x00000000
#define DATA_VIC_DMA0_ADDR1         0x00000000
#define DATA_VIC_DMA1_ADDR1         0x00000000
#define DATA_VIC_DMA2_ADDR1         0x00000000
#define DATA_VIC_DMA3_ADDR1         0x00000000
#define DATA_VIC_DMA0_ADDR2         0x00000000
#define DATA_VIC_DMA1_ADDR2         0x00000000
#define DATA_VIC_DMA2_ADDR2         0x00000000
#define DATA_VIC_DMA3_ADDR2         0x00000000
#define DATA_VIC_DMA0_LS            0x00000000
#define DATA_VIC_DMA1_LS            0x00000000
#define DATA_VIC_DMA2_LS            0x00000000
#define DATA_VIC_DMA3_LS            0x00000000
#define DATA_VIC_DMA0_BC            0x00000000
#define DATA_VIC_DMA1_BC            0x00000000
#define DATA_VIC_DMA2_BC            0x00000000
#define DATA_VIC_DMA3_BC            0x00000000
#define DATA_VIC_DMA0_INT_EN        0x00000000
#define DATA_VIC_DMA1_INT_EN        0x00000000
#define DATA_VIC_DMA2_INT_EN        0x00000000
#define DATA_VIC_DMA3_INT_EN        0x00000000
#define DATA_VIC_DMA0_INT_PEND      0x00000000
#define DATA_VIC_DMA1_INT_PEND      0x00000000
#define DATA_VIC_DMA2_INT_PEND      0x00000000
#define DATA_VIC_DMA3_INT_PEND      0x00000000
#define DATA_VIC_DMA0_INT_CLR       0x00000000
#define DATA_VIC_DMA1_INT_CLR       0x00000000
#define DATA_VIC_DMA2_INT_CLR       0x00000000
#define DATA_VIC_DMA3_INT_CLR       0x00000000
#define DATA_VIC_DMA0_LINE_CNT      0x00000000
#define DATA_VIC_DMA1_LINE_CNT      0x00000000
#define DATA_VIC_DMA2_LINE_CNT      0x00000000
#define DATA_VIC_DMA3_LINE_CNT      0x00000000
#define DATA_VIC_DEBUG_CTRL              0x00000000
#define DATA_VIC_TRC_DEBUG               0x00000000
#define DATA_VIC_SIGNAL_DEBUG            0x00000000
#define DATA_VIC_DVP_PCLK_CNT            0x0000FFFF
#define DATA_VIC_DVP_VSYNC_CNT           0x00000000
#define DATA_VIC_DVP_HS_PRD_CNT          0x00000000
#define DATA_VIC_DVP_VS_PRD_CNT          0x00000000

/* Version Register */
typedef union {
	u32 val;
	struct {
	u32 ver_l:5;           /* */
	u32 rsvd0:3;           /* */
	u32 ver_h:3;           /* */
	u32 rsvd1:1;           /* */
	u32 comp:1;            /* */
	u32 rsvd2:19;          /* */
	} bits;
} reg_vic_ver_t;

/* Feature Register */
typedef union {
	u32 val;
	struct {
	u32 has_yc_intl:1;             /* */
	u32 has_yc_sp:1;               /* */
	u32 rsvd0:6;                   /* */
	u32 has_multi_ch:1;            /* */
	u32 rsvd1:7;                   /* */
	u32 has_dma0:1;                /* */
	u32 has_dma1:1;                /* */
	u32 has_dma2:1;                /* */
	u32 has_dma3:1;                /* */
	u32 rsvd2:12;                  /* */
	} bits;
} reg_vic_ftr_t;

#define VIC_FTR_HAS_YC_INTL_0   0x0
#define VIC_FTR_HAS_YC_INTL_1   0x1
#define VIC_FTR_HAS_YC_SP_0     0x0
#define VIC_FTR_HAS_YC_SP_1     0x1
#define VIC_FTR_HAS_MULTI_CH_0  0x0
#define VIC_FTR_HAS_MULTI_CH_1  0x1
#define VIC_FTR_HAS_DMA0_0      0x0
#define VIC_FTR_HAS_DMA0_1      0x1
#define VIC_FTR_HAS_DMA1_0      0x0
#define VIC_FTR_HAS_DMA1_1      0x1
#define VIC_FTR_HAS_DMA2_0      0x0
#define VIC_FTR_HAS_DMA2_1      0x1
#define VIC_FTR_HAS_DMA3_0      0x0
#define VIC_FTR_HAS_DMA3_1      0x1

/* VIC Control Register */
typedef union {
	u32 val;
	struct {
	u32 vic_en:1;          /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_vic_ctrl_t;

#define VIC_VIC_CTRL_VIC_EN_0   0x0
#define VIC_VIC_CTRL_VIC_EN_1   0x1

/* VIC Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:1;           /* */
	u32 raw_msb:1;         /* */
	u32 data_8bit:1;       /* */
	u32 rsvd1:1;           /* */
	u32 dvp_path:2;        /* */
	u32 rsvd2:25;          /* */
	u32 busy:1;            /* */
	} bits;
} reg_vic_vic_cfg_t;

#define VIC_VIC_CFG_RAW_MSB_0   0x0
#define VIC_VIC_CFG_RAW_MSB_1   0x1
#define VIC_VIC_CFG_DATA_8BIT_0 0x0
#define VIC_VIC_CFG_DATA_8BIT_1 0x1
#define VIC_VIC_CFG_DVP_PATH_0  0x0
#define VIC_VIC_CFG_DVP_PATH_1  0x1
#define VIC_VIC_CFG_DVP_PATH_2  0x2
#define VIC_VIC_CFG_DVP_PATH_3  0x3
#define VIC_VIC_CFG_BUSY_0      0x0
#define VIC_VIC_CFG_BUSY_1      0x1

/* IO Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 pclk_inv:1;                /* */
	u32 field_inv:1;               /* */
	u32 hs_inv:1;                  /* */
	u32 vs_inv:1;                  /* */
	u32 rsvd0:4;                   /* */
	u32 pclk_phase:6;              /* */
	u32 rsvd1:18;                  /* */
	} bits;
} reg_vic_io_cfg_t;

#define VIC_IO_CFG_PCLK_INV_0   0x0
#define VIC_IO_CFG_PCLK_INV_1   0x1
#define VIC_IO_CFG_FIELD_INV_0  0x0
#define VIC_IO_CFG_FIELD_INV_1  0x1
#define VIC_IO_CFG_HS_INV_0     0x0
#define VIC_IO_CFG_HS_INV_1     0x1
#define VIC_IO_CFG_VS_INV_0     0x0
#define VIC_IO_CFG_VS_INV_1     0x1

/* Interface Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 data_ptcl:4;       /* */
	u32 rsvd0:4;           /* */
	u32 yc_seq:2;          /* */
	u32 yc_sync:2;         /* */
	u32 rsvd1:4;           /* */
	u32 sync_ptcl:4;       /* */
	u32 field_det:2;       /* */
	u32 rsvd2:2;           /* */
	u32 v_mode:1;          /* */
	u32 rsvd3:7;           /* */
	} bits;
} reg_vic_if_cfg_t;

#define VIC_IF_CFG_DATA_PTCL_0  0x0
#define VIC_IF_CFG_DATA_PTCL_1  0x1
#define VIC_IF_CFG_DATA_PTCL_2  0x2
#define VIC_IF_CFG_DATA_PTCL_3  0x3
#define VIC_IF_CFG_DATA_PTCL_5  0x5
#define VIC_IF_CFG_YC_SEQ_0     0x0
#define VIC_IF_CFG_YC_SEQ_1     0x1
#define VIC_IF_CFG_YC_SEQ_2     0x2
#define VIC_IF_CFG_YC_SEQ_3     0x3
#define VIC_IF_CFG_YC_SYNC_0    0x0
#define VIC_IF_CFG_YC_SYNC_1    0x1
#define VIC_IF_CFG_SYNC_PTCL_0  0x0
#define VIC_IF_CFG_SYNC_PTCL_1  0x1
#define VIC_IF_CFG_SYNC_PTCL_2  0x2
#define VIC_IF_CFG_SYNC_PTCL_3  0x3
#define VIC_IF_CFG_SYNC_PTCL_4  0x4
#define VIC_IF_CFG_SYNC_PTCL_5  0x5
#define VIC_IF_CFG_FIELD_DET_0  0x0
#define VIC_IF_CFG_FIELD_DET_1  0x1
#define VIC_IF_CFG_FIELD_DET_2  0x2
#define VIC_IF_CFG_FIELD_DET_3  0x3
#define VIC_IF_CFG_V_MODE_0     0x0
#define VIC_IF_CFG_V_MODE_1     0x1

/* DMA Multi-Channel Register */
typedef union {
	u32 val;
	struct {
	u32 num:2;             /* */
	u32 rsvd0:2;           /* */
	u32 en:1;              /* */
	u32 rsvd1:27;          /* */
	} bits;
} reg_vic_dma_multi_ch_t;

#define VIC_DMA_MULTI_CH_NUM_0  0x0
#define VIC_DMA_MULTI_CH_NUM_1  0x1
#define VIC_DMA_MULTI_CH_NUM_2  0x2
#define VIC_DMA_MULTI_CH_NUM_3  0x3
#define VIC_DMA_MULTI_CH_EN_0   0x0
#define VIC_DMA_MULTI_CH_EN_1   0x1

/* DMA Multi-Channel ID Register */
typedef union {
	u32 val;
	struct {
	u32 ch0_id:4;          /* */
	u32 rsvd0:4;           /* */
	u32 ch1_id:4;          /* */
	u32 rsvd1:4;           /* */
	u32 ch2_id:4;          /* */
	u32 rsvd2:4;           /* */
	u32 ch3_id:4;          /* */
	u32 rsvd3:4;           /* */
	} bits;
} reg_vic_dma_multi_id_t;

/* Horizontal Front Blanking Offset Register */
typedef union {
	u32 val;
	struct {
	u32 hoffset:16;        /* */
	u32 rsvd0:16;          /* */
	} bits;
} reg_vic_hfb_ofs_t;

/* Horizontal Active Width Register */
typedef union {
	u32 val;
	struct {
	u32 haw:32;            /* */
	} bits;
} reg_vic_hact_width_t;

/* Field1 Vertical Front Blanking Offset Register */
typedef union {
	u32 val;
	struct {
	u32 f1_voffset:10;             /* */
	u32 rsvd0:22;                  /* */
	} bits;
} reg_vic_f1_vfb_ofs_t;

/* Field1 Vertical Active Line Register */
typedef union {
	u32 val;
	struct {
	u32 f1_vact_line:11;           /* */
	u32 rsvd0:21;                  /* */
	} bits;
} reg_vic_f1_vact_line_t;

/* Field2 Vertical Front Blanking Offset Register */
typedef union {
	u32 val;
	struct {
	u32 f2_voffset:10;             /* */
	u32 rsvd0:22;                  /* */
	} bits;
} reg_vic_f2_vfb_ofs_t;

/* Field2 Vertical Active Line Register */
typedef union {
	u32 val;
	struct {
	u32 f2_vact_line:11;           /* */
	u32 rsvd0:21;                  /* */
	} bits;
} reg_vic_f2_vact_line_t;

/* DMA[CH] Offset Register */
typedef union {
	u32 val;
	struct {
	u32 xs:15;             /* */
	u32 rsvd0:1;           /* */
	u32 ys:15;             /* */
	u32 rsvd1:1;           /* */
	} bits;
} reg_vic_dma0_ofs_t;

/* DMA[CH] Offset Register */
typedef union {
	u32 val;
	struct {
	u32 xs:15;             /* */
	u32 rsvd0:1;           /* */
	u32 ys:15;             /* */
	u32 rsvd1:1;           /* */
	} bits;
} reg_vic_dma1_ofs_t;

/* DMA[CH] Offset Register */
typedef union {
	u32 val;
	struct {
	u32 xs:15;             /* */
	u32 rsvd0:1;           /* */
	u32 ys:15;             /* */
	u32 rsvd1:1;           /* */
	} bits;
} reg_vic_dma2_ofs_t;

/* DMA[CH] Offset Register */
typedef union {
	u32 val;
	struct {
	u32 xs:15;             /* */
	u32 rsvd0:1;           /* */
	u32 ys:15;             /* */
	u32 rsvd1:1;           /* */
	} bits;
} reg_vic_dma3_ofs_t;

/* DMA[CH] Size Register */
typedef union {
	u32 val;
	struct {
	u32 x:15;              /* */
	u32 rsvd0:1;           /* */
	u32 y:15;              /* */
	u32 rsvd1:1;           /* */
	} bits;
} reg_vic_dma0_size_t;

/* DMA[CH] Size Register */
typedef union {
	u32 val;
	struct {
	u32 x:15;              /* */
	u32 rsvd0:1;           /* */
	u32 y:15;              /* */
	u32 rsvd1:1;           /* */
	} bits;
} reg_vic_dma1_size_t;

/* DMA[CH] Size Register */
typedef union {
	u32 val;
	struct {
	u32 x:15;              /* */
	u32 rsvd0:1;           /* */
	u32 y:15;              /* */
	u32 rsvd1:1;           /* */
	} bits;
} reg_vic_dma2_size_t;

/* DMA[CH] Size Register */
typedef union {
	u32 val;
	struct {
	u32 x:15;              /* */
	u32 rsvd0:1;           /* */
	u32 y:15;              /* */
	u32 rsvd1:1;           /* */
	} bits;
} reg_vic_dma3_size_t;

/* DMA[CH] Mode Register */
typedef union {
	u32 val;
	struct {
	u32 out_fmt:4;                 /* */
	u32 rsvd0:4;                   /* */
	u32 burst:2;                   /* */
	u32 rsvd1:4;                   /* */
	u32 filed_strg:2;              /* */
	u32 fci:1;                     /* */
	u32 vbi:1;                     /* */
	u32 lcti:1;                    /* */
	u32 rsvd2:1;                   /* */
	u32 ud:2;                      /* */
	u32 rsvd3:9;                   /* */
	u32 cap:1;                     /* */
	} bits;
} reg_vic_dma0_mode_t;

#define VIC_DMA0_MODE_OUT_FMT_0         0x0
#define VIC_DMA0_MODE_OUT_FMT_1         0x1
#define VIC_DMA0_MODE_OUT_FMT_2         0x2
#define VIC_DMA0_MODE_OUT_FMT_3         0x3
#define VIC_DMA0_MODE_OUT_FMT_4         0x4
#define VIC_DMA0_MODE_BURST_0           0x0
#define VIC_DMA0_MODE_BURST_1           0x1
#define VIC_DMA0_MODE_BURST_2           0x2
#define VIC_DMA0_MODE_BURST_3           0x3
#define VIC_DMA0_MODE_FILED_STRG_0      0x0
#define VIC_DMA0_MODE_FILED_STRG_1      0x1
#define VIC_DMA0_MODE_FILED_STRG_2      0x2
#define VIC_DMA0_MODE_FILED_STRG_3      0x3
#define VIC_DMA0_MODE_FCI_0             0x0
#define VIC_DMA0_MODE_FCI_1             0x1
#define VIC_DMA0_MODE_VBI_0             0x0
#define VIC_DMA0_MODE_VBI_1             0x1
#define VIC_DMA0_MODE_LCTI_0            0x0
#define VIC_DMA0_MODE_LCTI_1            0x1
#define VIC_DMA0_MODE_UD_0              0x0
#define VIC_DMA0_MODE_UD_1              0x1
#define VIC_DMA0_MODE_UD_2              0x2
#define VIC_DMA0_MODE_UD_3              0x3
#define VIC_DMA0_MODE_CAP_0             0x0
#define VIC_DMA0_MODE_CAP_1             0x1

/* DMA[CH] Mode Register */
typedef union {
	u32 val;
	struct {
	u32 out_fmt:4;                 /* */
	u32 rsvd0:4;                   /* */
	u32 burst:2;                   /* */
	u32 rsvd1:4;                   /* */
	u32 filed_strg:2;              /* */
	u32 fci:1;                     /* */
	u32 vbi:1;                     /* */
	u32 lcti:1;                    /* */
	u32 rsvd2:1;                   /* */
	u32 ud:2;                      /* */
	u32 rsvd3:9;                   /* */
	u32 cap:1;                     /* */
	} bits;
} reg_vic_dma1_mode_t;

#define VIC_DMA1_MODE_OUT_FMT_0         0x0
#define VIC_DMA1_MODE_OUT_FMT_1         0x1
#define VIC_DMA1_MODE_OUT_FMT_2         0x2
#define VIC_DMA1_MODE_OUT_FMT_3         0x3
#define VIC_DMA1_MODE_OUT_FMT_4         0x4
#define VIC_DMA1_MODE_BURST_0           0x0
#define VIC_DMA1_MODE_BURST_1           0x1
#define VIC_DMA1_MODE_BURST_2           0x2
#define VIC_DMA1_MODE_BURST_3           0x3
#define VIC_DMA1_MODE_FILED_STRG_0      0x0
#define VIC_DMA1_MODE_FILED_STRG_1      0x1
#define VIC_DMA1_MODE_FILED_STRG_2      0x2
#define VIC_DMA1_MODE_FILED_STRG_3      0x3
#define VIC_DMA1_MODE_FCI_0             0x0
#define VIC_DMA1_MODE_FCI_1             0x1
#define VIC_DMA1_MODE_VBI_0             0x0
#define VIC_DMA1_MODE_VBI_1             0x1
#define VIC_DMA1_MODE_LCTI_0            0x0
#define VIC_DMA1_MODE_LCTI_1            0x1
#define VIC_DMA1_MODE_UD_0              0x0
#define VIC_DMA1_MODE_UD_1              0x1
#define VIC_DMA1_MODE_UD_2              0x2
#define VIC_DMA1_MODE_UD_3              0x3
#define VIC_DMA1_MODE_CAP_0             0x0
#define VIC_DMA1_MODE_CAP_1             0x1

/* DMA[CH] Mode Register */
typedef union {
	u32 val;
	struct {
	u32 out_fmt:4;                 /* */
	u32 rsvd0:4;                   /* */
	u32 burst:2;                   /* */
	u32 rsvd1:4;                   /* */
	u32 filed_strg:2;              /* */
	u32 fci:1;                     /* */
	u32 vbi:1;                     /* */
	u32 lcti:1;                    /* */
	u32 rsvd2:1;                   /* */
	u32 ud:2;                      /* */
	u32 rsvd3:9;                   /* */
	u32 cap:1;                     /* */
	} bits;
} reg_vic_dma2_mode_t;

#define VIC_DMA2_MODE_OUT_FMT_0         0x0
#define VIC_DMA2_MODE_OUT_FMT_1         0x1
#define VIC_DMA2_MODE_OUT_FMT_2         0x2
#define VIC_DMA2_MODE_OUT_FMT_3         0x3
#define VIC_DMA2_MODE_OUT_FMT_4         0x4
#define VIC_DMA2_MODE_BURST_0           0x0
#define VIC_DMA2_MODE_BURST_1           0x1
#define VIC_DMA2_MODE_BURST_2           0x2
#define VIC_DMA2_MODE_BURST_3           0x3
#define VIC_DMA2_MODE_FILED_STRG_0      0x0
#define VIC_DMA2_MODE_FILED_STRG_1      0x1
#define VIC_DMA2_MODE_FILED_STRG_2      0x2
#define VIC_DMA2_MODE_FILED_STRG_3      0x3
#define VIC_DMA2_MODE_FCI_0             0x0
#define VIC_DMA2_MODE_FCI_1             0x1
#define VIC_DMA2_MODE_VBI_0             0x0
#define VIC_DMA2_MODE_VBI_1             0x1
#define VIC_DMA2_MODE_LCTI_0            0x0
#define VIC_DMA2_MODE_LCTI_1            0x1
#define VIC_DMA2_MODE_UD_0              0x0
#define VIC_DMA2_MODE_UD_1              0x1
#define VIC_DMA2_MODE_UD_2              0x2
#define VIC_DMA2_MODE_UD_3              0x3
#define VIC_DMA2_MODE_CAP_0             0x0
#define VIC_DMA2_MODE_CAP_1             0x1

/* DMA[CH] Mode Register */
typedef union {
	u32 val;
	struct {
	u32 out_fmt:4;                 /* */
	u32 rsvd0:4;                   /* */
	u32 burst:2;                   /* */
	u32 rsvd1:4;                   /* */
	u32 filed_strg:2;              /* */
	u32 fci:1;                     /* */
	u32 vbi:1;                     /* */
	u32 lcti:1;                    /* */
	u32 rsvd2:1;                   /* */
	u32 ud:2;                      /* */
	u32 rsvd3:9;                   /* */
	u32 cap:1;                     /* */
	} bits;
} reg_vic_dma3_mode_t;

#define VIC_DMA3_MODE_OUT_FMT_0         0x0
#define VIC_DMA3_MODE_OUT_FMT_1         0x1
#define VIC_DMA3_MODE_OUT_FMT_2         0x2
#define VIC_DMA3_MODE_OUT_FMT_3         0x3
#define VIC_DMA3_MODE_OUT_FMT_4         0x4
#define VIC_DMA3_MODE_BURST_0           0x0
#define VIC_DMA3_MODE_BURST_1           0x1
#define VIC_DMA3_MODE_BURST_2           0x2
#define VIC_DMA3_MODE_BURST_3           0x3
#define VIC_DMA3_MODE_FILED_STRG_0      0x0
#define VIC_DMA3_MODE_FILED_STRG_1      0x1
#define VIC_DMA3_MODE_FILED_STRG_2      0x2
#define VIC_DMA3_MODE_FILED_STRG_3      0x3
#define VIC_DMA3_MODE_FCI_0             0x0
#define VIC_DMA3_MODE_FCI_1             0x1
#define VIC_DMA3_MODE_VBI_0             0x0
#define VIC_DMA3_MODE_VBI_1             0x1
#define VIC_DMA3_MODE_LCTI_0            0x0
#define VIC_DMA3_MODE_LCTI_1            0x1
#define VIC_DMA3_MODE_UD_0              0x0
#define VIC_DMA3_MODE_UD_1              0x1
#define VIC_DMA3_MODE_UD_2              0x2
#define VIC_DMA3_MODE_UD_3              0x3
#define VIC_DMA3_MODE_CAP_0             0x0
#define VIC_DMA3_MODE_CAP_1             0x1

/* DMA[CH] Control Register */
typedef union {
	u32 val;
	struct {
	u32 start:1;           /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_dma0_ctrl_t;

/* DMA[CH] Control Register */
typedef union {
	u32 val;
	struct {
	u32 start:1;           /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_dma1_ctrl_t;

/* DMA[CH] Control Register */
typedef union {
	u32 val;
	struct {
	u32 start:1;           /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_dma2_ctrl_t;

/* DMA[CH] Control Register */
typedef union {
	u32 val;
	struct {
	u32 start:1;           /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_dma3_ctrl_t;

/* DMA[CH] Update Register */
typedef union {
	u32 val;
	struct {
	u32 ud_man:1;          /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_dma0_update_t;

/* DMA[CH] Update Register */
typedef union {
	u32 val;
	struct {
	u32 ud_man:1;          /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_dma1_update_t;

/* DMA[CH] Update Register */
typedef union {
	u32 val;
	struct {
	u32 ud_man:1;          /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_dma2_update_t;

/* DMA[CH] Update Register */
typedef union {
	u32 val;
	struct {
	u32 ud_man:1;          /* */
	u32 rsvd0:31;          /* */
	} bits;
} reg_vic_dma3_update_t;

/* DMA[CH] Component0 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma0_addr0_t;

/* DMA[CH] Component0 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma1_addr0_t;

/* DMA[CH] Component0 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma2_addr0_t;

/* DMA[CH] Component0 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma3_addr0_t;

/* DMA[CH] Component1 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma0_addr1_t;

/* DMA[CH] Component1 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma1_addr1_t;

/* DMA[CH] Component1 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma2_addr1_t;

/* DMA[CH] Component1 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma3_addr1_t;

/* DMA[CH] Component2 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma0_addr2_t;

/* DMA[CH] Component2 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma1_addr2_t;

/* DMA[CH] Component2 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma2_addr2_t;

/* DMA[CH] Component2 Output Address Register */
typedef union {
	u32 val;
	struct {
	u32 addr:32;           /* */
	} bits;
} reg_vic_dma3_addr2_t;

/* DMA[CH] Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 y:16;              /* */
	u32 c:15;              /* */
	u32 rsvd0:1;           /* */
	} bits;
} reg_vic_dma0_ls_t;

/* DMA[CH] Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 y:16;              /* */
	u32 c:15;              /* */
	u32 rsvd0:1;           /* */
	} bits;
} reg_vic_dma1_ls_t;

/* DMA[CH] Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 y:16;              /* */
	u32 c:15;              /* */
	u32 rsvd0:1;           /* */
	} bits;
} reg_vic_dma2_ls_t;

/* DMA[CH] Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 y:16;              /* */
	u32 c:15;              /* */
	u32 rsvd0:1;           /* */
	} bits;
} reg_vic_dma3_ls_t;

/* DMA[CH] Byte Counter Register */
typedef union {
	u32 val;
	struct {
	u32 bc:32;             /* */
	} bits;
} reg_vic_dma0_bc_t;

/* DMA[CH] Byte Counter Register */
typedef union {
	u32 val;
	struct {
	u32 bc:32;             /* */
	} bits;
} reg_vic_dma1_bc_t;

/* DMA[CH] Byte Counter Register */
typedef union {
	u32 val;
	struct {
	u32 bc:32;             /* */
	} bits;
} reg_vic_dma2_bc_t;

/* DMA[CH] Byte Counter Register */
typedef union {
	u32 val;
	struct {
	u32 bc:32;             /* */
	} bits;
} reg_vic_dma3_bc_t;

/* DMA[CH] Interrupt Enable Register */
typedef union {
	u32 val;
	struct {
	u32 fci_en:1;          /* */
	u32 vbi_en:1;          /* */
	u32 lcti_en:1;         /* */
	u32 rsvd0:5;           /* */
	u32 p0_ovf_en:1;       /* */
	u32 p1_ovf_en:1;       /* */
	u32 p2_ovf_en:1;       /* */
	u32 rsvd1:21;          /* */
	} bits;
} reg_vic_dma0_int_en_t;

#define VIC_DMA0_INT_EN_FCI_EN_0        0x0
#define VIC_DMA0_INT_EN_FCI_EN_1        0x1
#define VIC_DMA0_INT_EN_VBI_EN_0        0x0
#define VIC_DMA0_INT_EN_VBI_EN_1        0x1
#define VIC_DMA0_INT_EN_LCTI_EN_0       0x0
#define VIC_DMA0_INT_EN_LCTI_EN_1       0x1
#define VIC_DMA0_INT_EN_P0_OVF_EN_0     0x0
#define VIC_DMA0_INT_EN_P0_OVF_EN_1     0x1
#define VIC_DMA0_INT_EN_P1_OVF_EN_0     0x0
#define VIC_DMA0_INT_EN_P1_OVF_EN_1     0x1
#define VIC_DMA0_INT_EN_P2_OVF_EN_0     0x0
#define VIC_DMA0_INT_EN_P2_OVF_EN_1     0x1

/* DMA[CH] Interrupt Enable Register */
typedef union {
	u32 val;
	struct {
	u32 fci_en:1;          /* */
	u32 vbi_en:1;          /* */
	u32 lcti_en:1;         /* */
	u32 rsvd0:5;           /* */
	u32 p0_ovf_en:1;       /* */
	u32 p1_ovf_en:1;       /* */
	u32 p2_ovf_en:1;       /* */
	u32 rsvd1:21;          /* */
	} bits;
} reg_vic_dma1_int_en_t;

#define VIC_DMA1_INT_EN_FCI_EN_0        0x0
#define VIC_DMA1_INT_EN_FCI_EN_1        0x1
#define VIC_DMA1_INT_EN_VBI_EN_0        0x0
#define VIC_DMA1_INT_EN_VBI_EN_1        0x1
#define VIC_DMA1_INT_EN_LCTI_EN_0       0x0
#define VIC_DMA1_INT_EN_LCTI_EN_1       0x1
#define VIC_DMA1_INT_EN_P0_OVF_EN_0     0x0
#define VIC_DMA1_INT_EN_P0_OVF_EN_1     0x1
#define VIC_DMA1_INT_EN_P1_OVF_EN_0     0x0
#define VIC_DMA1_INT_EN_P1_OVF_EN_1     0x1
#define VIC_DMA1_INT_EN_P2_OVF_EN_0     0x0
#define VIC_DMA1_INT_EN_P2_OVF_EN_1     0x1

/* DMA[CH] Interrupt Enable Register */
typedef union {
	u32 val;
	struct {
	u32 fci_en:1;          /* */
	u32 vbi_en:1;          /* */
	u32 lcti_en:1;         /* */
	u32 rsvd0:5;           /* */
	u32 p0_ovf_en:1;       /* */
	u32 p1_ovf_en:1;       /* */
	u32 p2_ovf_en:1;       /* */
	u32 rsvd1:21;          /* */
	} bits;
} reg_vic_dma2_int_en_t;

#define VIC_DMA2_INT_EN_FCI_EN_0        0x0
#define VIC_DMA2_INT_EN_FCI_EN_1        0x1
#define VIC_DMA2_INT_EN_VBI_EN_0        0x0
#define VIC_DMA2_INT_EN_VBI_EN_1        0x1
#define VIC_DMA2_INT_EN_LCTI_EN_0       0x0
#define VIC_DMA2_INT_EN_LCTI_EN_1       0x1
#define VIC_DMA2_INT_EN_P0_OVF_EN_0     0x0
#define VIC_DMA2_INT_EN_P0_OVF_EN_1     0x1
#define VIC_DMA2_INT_EN_P1_OVF_EN_0     0x0
#define VIC_DMA2_INT_EN_P1_OVF_EN_1     0x1
#define VIC_DMA2_INT_EN_P2_OVF_EN_0     0x0
#define VIC_DMA2_INT_EN_P2_OVF_EN_1     0x1

/* DMA[CH] Interrupt Enable Register */
typedef union {
	u32 val;
	struct {
	u32 fci_en:1;          /* */
	u32 vbi_en:1;          /* */
	u32 lcti_en:1;         /* */
	u32 rsvd0:5;           /* */
	u32 p0_ovf_en:1;       /* */
	u32 p1_ovf_en:1;       /* */
	u32 p2_ovf_en:1;       /* */
	u32 rsvd1:21;          /* */
	} bits;
} reg_vic_dma3_int_en_t;

#define VIC_DMA3_INT_EN_FCI_EN_0        0x0
#define VIC_DMA3_INT_EN_FCI_EN_1        0x1
#define VIC_DMA3_INT_EN_VBI_EN_0        0x0
#define VIC_DMA3_INT_EN_VBI_EN_1        0x1
#define VIC_DMA3_INT_EN_LCTI_EN_0       0x0
#define VIC_DMA3_INT_EN_LCTI_EN_1       0x1
#define VIC_DMA3_INT_EN_P0_OVF_EN_0     0x0
#define VIC_DMA3_INT_EN_P0_OVF_EN_1     0x1
#define VIC_DMA3_INT_EN_P1_OVF_EN_0     0x0
#define VIC_DMA3_INT_EN_P1_OVF_EN_1     0x1
#define VIC_DMA3_INT_EN_P2_OVF_EN_0     0x0
#define VIC_DMA3_INT_EN_P2_OVF_EN_1     0x1

/* DMA[CH] Interrupt Pending Register */
typedef union {
	u32 val;
	struct {
	u32 fci_pend:1;                /* */
	u32 vbi_pend:1;                /* */
	u32 lcti_pend:1;               /* */
	u32 rsvd0:5;                   /* */
	u32 p0_ovf_pend:1;             /* */
	u32 p1_ovf_pend:1;             /* */
	u32 p2_ovf_pend:1;             /* */
	u32 rsvd1:21;                  /* */
	} bits;
} reg_vic_dma0_int_pend_t;

/* DMA[CH] Interrupt Pending Register */
typedef union {
	u32 val;
	struct {
	u32 fci_pend:1;                /* */
	u32 vbi_pend:1;                /* */
	u32 lcti_pend:1;               /* */
	u32 rsvd0:5;                   /* */
	u32 p0_ovf_pend:1;             /* */
	u32 p1_ovf_pend:1;             /* */
	u32 p2_ovf_pend:1;             /* */
	u32 rsvd1:21;                  /* */
	} bits;
} reg_vic_dma1_int_pend_t;

/* DMA[CH] Interrupt Pending Register */
typedef union {
	u32 val;
	struct {
	u32 fci_pend:1;                /* */
	u32 vbi_pend:1;                /* */
	u32 lcti_pend:1;               /* */
	u32 rsvd0:5;                   /* */
	u32 p0_ovf_pend:1;             /* */
	u32 p1_ovf_pend:1;             /* */
	u32 p2_ovf_pend:1;             /* */
	u32 rsvd1:21;                  /* */
	} bits;
} reg_vic_dma2_int_pend_t;

/* DMA[CH] Interrupt Pending Register */
typedef union {
	u32 val;
	struct {
	u32 fci_pend:1;                /* */
	u32 vbi_pend:1;                /* */
	u32 lcti_pend:1;               /* */
	u32 rsvd0:5;                   /* */
	u32 p0_ovf_pend:1;             /* */
	u32 p1_ovf_pend:1;             /* */
	u32 p2_ovf_pend:1;             /* */
	u32 rsvd1:21;                  /* */
	} bits;
} reg_vic_dma3_int_pend_t;

/* DMA[CH] Interrupt Clear Register */
typedef union {
	u32 val;
	struct {
	u32 fci_clr:1;                 /* */
	u32 vbi_clr:1;                 /* */
	u32 lcti_clr:1;                /* */
	u32 rsvd0:5;                   /* */
	u32 p0_ovf_clr:1;              /* */
	u32 p1_ovf_clr:1;              /* */
	u32 p2_ovf_clr:1;              /* */
	u32 rsvd1:21;                  /* */
	} bits;
} reg_vic_dma0_int_clr_t;

#define VIC_DMA0_INT_CLR_FCI_CLR_0      0x0
#define VIC_DMA0_INT_CLR_FCI_CLR_1      0x1
#define VIC_DMA0_INT_CLR_VBI_CLR_0      0x0
#define VIC_DMA0_INT_CLR_VBI_CLR_1      0x1
#define VIC_DMA0_INT_CLR_LCTI_CLR_0     0x0
#define VIC_DMA0_INT_CLR_LCTI_CLR_1     0x1
#define VIC_DMA0_INT_CLR_P0_OVF_CLR_0   0x0
#define VIC_DMA0_INT_CLR_P0_OVF_CLR_1   0x1
#define VIC_DMA0_INT_CLR_P1_OVF_CLR_0   0x0
#define VIC_DMA0_INT_CLR_P1_OVF_CLR_1   0x1
#define VIC_DMA0_INT_CLR_P2_OVF_CLR_0   0x0
#define VIC_DMA0_INT_CLR_P2_OVF_CLR_1   0x1

/* DMA[CH] Interrupt Clear Register */
typedef union {
	u32 val;
	struct {
	u32 fci_clr:1;                 /* */
	u32 vbi_clr:1;                 /* */
	u32 lcti_clr:1;                /* */
	u32 rsvd0:5;                   /* */
	u32 p0_ovf_clr:1;              /* */
	u32 p1_ovf_clr:1;              /* */
	u32 p2_ovf_clr:1;              /* */
	u32 rsvd1:21;                  /* */
	} bits;
} reg_vic_dma1_int_clr_t;

#define VIC_DMA1_INT_CLR_FCI_CLR_0      0x0
#define VIC_DMA1_INT_CLR_FCI_CLR_1      0x1
#define VIC_DMA1_INT_CLR_VBI_CLR_0      0x0
#define VIC_DMA1_INT_CLR_VBI_CLR_1      0x1
#define VIC_DMA1_INT_CLR_LCTI_CLR_0     0x0
#define VIC_DMA1_INT_CLR_LCTI_CLR_1     0x1
#define VIC_DMA1_INT_CLR_P0_OVF_CLR_0   0x0
#define VIC_DMA1_INT_CLR_P0_OVF_CLR_1   0x1
#define VIC_DMA1_INT_CLR_P1_OVF_CLR_0   0x0
#define VIC_DMA1_INT_CLR_P1_OVF_CLR_1   0x1
#define VIC_DMA1_INT_CLR_P2_OVF_CLR_0   0x0
#define VIC_DMA1_INT_CLR_P2_OVF_CLR_1   0x1

/* DMA[CH] Interrupt Clear Register */
typedef union {
	u32 val;
	struct {
	u32 fci_clr:1;                 /* */
	u32 vbi_clr:1;                 /* */
	u32 lcti_clr:1;                /* */
	u32 rsvd0:5;                   /* */
	u32 p0_ovf_clr:1;              /* */
	u32 p1_ovf_clr:1;              /* */
	u32 p2_ovf_clr:1;              /* */
	u32 rsvd1:21;                  /* */
	} bits;
} reg_vic_dma2_int_clr_t;

#define VIC_DMA2_INT_CLR_FCI_CLR_0      0x0
#define VIC_DMA2_INT_CLR_FCI_CLR_1      0x1
#define VIC_DMA2_INT_CLR_VBI_CLR_0      0x0
#define VIC_DMA2_INT_CLR_VBI_CLR_1      0x1
#define VIC_DMA2_INT_CLR_LCTI_CLR_0     0x0
#define VIC_DMA2_INT_CLR_LCTI_CLR_1     0x1
#define VIC_DMA2_INT_CLR_P0_OVF_CLR_0   0x0
#define VIC_DMA2_INT_CLR_P0_OVF_CLR_1   0x1
#define VIC_DMA2_INT_CLR_P1_OVF_CLR_0   0x0
#define VIC_DMA2_INT_CLR_P1_OVF_CLR_1   0x1
#define VIC_DMA2_INT_CLR_P2_OVF_CLR_0   0x0
#define VIC_DMA2_INT_CLR_P2_OVF_CLR_1   0x1

/* DMA[CH] Interrupt Clear Register */
typedef union {
	u32 val;
	struct {
	u32 fci_clr:1;                 /* */
	u32 vbi_clr:1;                 /* */
	u32 lcti_clr:1;                /* */
	u32 rsvd0:5;                   /* */
	u32 p0_ovf_clr:1;              /* */
	u32 p1_ovf_clr:1;              /* */
	u32 p2_ovf_clr:1;              /* */
	u32 rsvd1:21;                  /* */
	} bits;
} reg_vic_dma3_int_clr_t;

#define VIC_DMA3_INT_CLR_FCI_CLR_0      0x0
#define VIC_DMA3_INT_CLR_FCI_CLR_1      0x1
#define VIC_DMA3_INT_CLR_VBI_CLR_0      0x0
#define VIC_DMA3_INT_CLR_VBI_CLR_1      0x1
#define VIC_DMA3_INT_CLR_LCTI_CLR_0     0x0
#define VIC_DMA3_INT_CLR_LCTI_CLR_1     0x1
#define VIC_DMA3_INT_CLR_P0_OVF_CLR_0   0x0
#define VIC_DMA3_INT_CLR_P0_OVF_CLR_1   0x1
#define VIC_DMA3_INT_CLR_P1_OVF_CLR_0   0x0
#define VIC_DMA3_INT_CLR_P1_OVF_CLR_1   0x1
#define VIC_DMA3_INT_CLR_P2_OVF_CLR_0   0x0
#define VIC_DMA3_INT_CLR_P2_OVF_CLR_1   0x1

/* DMA[CH] Line Counter Register */
typedef union {
	u32 val;
	struct {
	u32 ls_trig:12;        /* */
	u32 rsvd0:4;           /* */
	u32 ls_crnt:12;        /* */
	u32 rsvd1:4;           /* */
	} bits;
} reg_vic_dma0_line_cnt_t;

/* DMA[CH] Line Counter Register */
typedef union {
	u32 val;
	struct {
	u32 ls_trig:12;        /* */
	u32 rsvd0:4;           /* */
	u32 ls_crnt:12;        /* */
	u32 rsvd1:4;           /* */
	} bits;
} reg_vic_dma1_line_cnt_t;

/* DMA[CH] Line Counter Register */
typedef union {
	u32 val;
	struct {
	u32 ls_trig:12;        /* */
	u32 rsvd0:4;           /* */
	u32 ls_crnt:12;        /* */
	u32 rsvd1:4;           /* */
	} bits;
} reg_vic_dma2_line_cnt_t;

/* DMA[CH] Line Counter Register */
typedef union {
	u32 val;
	struct {
	u32 ls_trig:12;        /* */
	u32 rsvd0:4;           /* */
	u32 ls_crnt:12;        /* */
	u32 rsvd1:4;           /* */
	} bits;
} reg_vic_dma3_line_cnt_t;

/* reg_vic_t bank */
typedef struct tag_vic {
	reg_vic_ver_t              ver;            /* 0000 */
	reg_vic_ftr_t              ftr;            /* 0004 */
	u32                        res0[2];
	reg_vic_vic_ctrl_t         vic_ctrl;       /* 0010 */
	reg_vic_vic_cfg_t          vic_cfg;        /* 0014 */
	u32                        res1[1];
	reg_vic_io_cfg_t           io_cfg;         /* 001C */
	reg_vic_if_cfg_t           if_cfg;         /* 0020 */
	u32                        res2[3];
	reg_vic_dma_multi_ch_t     dma_multi_ch;   /* 0030 */
	reg_vic_dma_multi_id_t     dma_multi_id;   /* 0034 */
	u32                        res3[18];
	reg_vic_hfb_ofs_t          hfb_ofs;        /* 0080 */
	reg_vic_hact_width_t       hact_width;     /* 0084 */
	u32                        res4[2];
	reg_vic_f1_vfb_ofs_t       f1_vfb_ofs;     /* 0090 */
	reg_vic_f1_vact_line_t     f1_vact_line;   /* 0094 */
	reg_vic_f2_vfb_ofs_t       f2_vfb_ofs;     /* 0098 */
	reg_vic_f2_vact_line_t     f2_vact_line;   /* 009C */
	u32                        res5[24];
	reg_vic_dma0_ofs_t         dma0_ofs;       /* 0100 */
	reg_vic_dma0_size_t        dma0_size;      /* 0104 */
	u32                        res6[6];
	reg_vic_dma1_ofs_t         dma1_ofs;       /* 0120 */
	reg_vic_dma1_size_t        dma1_size;      /* 0124 */
	u32                        res7[6];
	reg_vic_dma2_ofs_t         dma2_ofs;       /* 0140 */
	reg_vic_dma2_size_t        dma2_size;      /* 0144 */
	u32                        res8[6];
	reg_vic_dma3_ofs_t         dma3_ofs;       /* 0160 */
	reg_vic_dma3_size_t        dma3_size;      /* 0164 */
	u32                        res9[6];
	reg_vic_dma0_mode_t        dma0_mode;      /* 0180 */
	reg_vic_dma0_ctrl_t        dma0_ctrl;      /* 0184 */
	reg_vic_dma0_update_t      dma0_update;    /* 0188 */
	u32                        res10[1];
	reg_vic_dma0_addr0_t       dma0_addr0;     /* 0190 */
	reg_vic_dma0_addr1_t       dma0_addr1;     /* 0194 */
	reg_vic_dma0_addr2_t       dma0_addr2;     /* 0198 */
	reg_vic_dma0_ls_t          dma0_ls;        /* 019C */
	u32                        res11[4];
	reg_vic_dma0_bc_t          dma0_bc;        /* 01B0 */
	u32                        res12[3];
	reg_vic_dma0_int_en_t      dma0_int_en;    /* 01C0 */
	reg_vic_dma0_int_pend_t    dma0_int_pend;  /* 01C4 */
	reg_vic_dma0_int_clr_t     dma0_int_clr;   /* 01C8 */
	reg_vic_dma0_line_cnt_t    dma0_line_cnt;  /* 01CC */
	u32                        res13[12];
	reg_vic_dma1_mode_t        dma1_mode;      /* 0200 */
	reg_vic_dma1_ctrl_t        dma1_ctrl;      /* 0204 */
	reg_vic_dma1_update_t      dma1_update;    /* 0208 */
	u32                        res14[1];
	reg_vic_dma1_addr0_t       dma1_addr0;     /* 0210 */
	reg_vic_dma1_addr1_t       dma1_addr1;     /* 0214 */
	reg_vic_dma1_addr2_t       dma1_addr2;     /* 0218 */
	reg_vic_dma1_ls_t          dma1_ls;        /* 021C */
	u32                        res15[4];
	reg_vic_dma1_bc_t          dma1_bc;        /* 0230 */
	u32                        res16[3];
	reg_vic_dma1_int_en_t      dma1_int_en;    /* 0240 */
	reg_vic_dma1_int_pend_t    dma1_int_pend;  /* 0244 */
	reg_vic_dma1_int_clr_t     dma1_int_clr;   /* 0248 */
	reg_vic_dma1_line_cnt_t    dma1_line_cnt;  /* 024C */
	u32                        res17[12];
	reg_vic_dma2_mode_t        dma2_mode;      /* 0280 */
	reg_vic_dma2_ctrl_t        dma2_ctrl;      /* 0284 */
	reg_vic_dma2_update_t      dma2_update;    /* 0288 */
	u32                        res18[1];
	reg_vic_dma2_addr0_t       dma2_addr0;     /* 0290 */
	reg_vic_dma2_addr1_t       dma2_addr1;     /* 0294 */
	reg_vic_dma2_addr2_t       dma2_addr2;     /* 0298 */
	reg_vic_dma2_ls_t          dma2_ls;        /* 029C */
	u32                        res19[4];
	reg_vic_dma2_bc_t          dma2_bc;        /* 02B0 */
	u32                        res20[3];
	reg_vic_dma2_int_en_t      dma2_int_en;    /* 02C0 */
	reg_vic_dma2_int_pend_t    dma2_int_pend;  /* 02C4 */
	reg_vic_dma2_int_clr_t     dma2_int_clr;   /* 02C8 */
	reg_vic_dma2_line_cnt_t    dma2_line_cnt;  /* 02CC */
	u32                        res21[12];
	reg_vic_dma3_mode_t        dma3_mode;      /* 0300 */
	reg_vic_dma3_ctrl_t        dma3_ctrl;      /* 0304 */
	reg_vic_dma3_update_t      dma3_update;    /* 0308 */
	u32                        res22[1];
	reg_vic_dma3_addr0_t       dma3_addr0;     /* 0310 */
	reg_vic_dma3_addr1_t       dma3_addr1;     /* 0314 */
	reg_vic_dma3_addr2_t       dma3_addr2;     /* 0318 */
	reg_vic_dma3_ls_t          dma3_ls;        /* 031C */
	u32                        res23[4];
	reg_vic_dma3_bc_t          dma3_bc;        /* 0330 */
	u32                        res24[3];
	reg_vic_dma3_int_en_t      dma3_int_en;    /* 0340 */
	reg_vic_dma3_int_pend_t    dma3_int_pend;  /* 0344 */
	reg_vic_dma3_int_clr_t     dma3_int_clr;   /* 0348 */
	reg_vic_dma3_line_cnt_t    dma3_line_cnt;  /* 034C */
	u32                        res25[172];
	u32                        prv_0600;       /* 0600 */
	u32                        res26[1];
	u32                        prv_0608;       /* 0608 */
	u32                        prv_060C;       /* 060C */
	u32                        prv_0610;       /* 0610 */
	u32                        prv_0614;       /* 0614 */
	u32                        prv_0618;       /* 0618 */
	u32                        prv_061C;       /* 061C */
} reg_vic_t;

#endif /* ___VIC___H___ */
