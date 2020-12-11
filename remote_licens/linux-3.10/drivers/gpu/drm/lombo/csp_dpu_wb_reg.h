/* wb.h */

#ifndef ___WB___H___
#define ___WB___H___

#define BASE_WB                0x015A0000

#define VA_WB_VER                    (0x00000000 + BASE_WB + VA_WB)
#define VA_WB_FTR                    (0x00000004 + BASE_WB + VA_WB)
#define VA_WB_CTL                    (0x00000010 + BASE_WB + VA_WB)
#define VA_WB_STRMCTL                (0x00000014 + BASE_WB + VA_WB)
#define VA_WB_CFG                    (0x00000018 + BASE_WB + VA_WB)
#define VA_WB_STA                    (0x0000001C + BASE_WB + VA_WB)
#define VA_WB_INTEN                  (0x00000020 + BASE_WB + VA_WB)
#define VA_WB_INTPD                  (0x00000024 + BASE_WB + VA_WB)
#define VA_WB_INTCLR                 (0x00000028 + BASE_WB + VA_WB)
#define VA_WB_SRCSIZE                (0x00000030 + BASE_WB + VA_WB)
#define VA_WB_CROPSIZE               (0x00000034 + BASE_WB + VA_WB)
#define VA_WB_CROPCOOR               (0x00000038 + BASE_WB + VA_WB)
#define VA_WB_DSTSIZE0               (0x00000040 + BASE_WB + VA_WB)
#define VA_WB_DSTSIZE1               (0x00000044 + BASE_WB + VA_WB)
#define VA_WB_CH0_BUFADD        (0x00000050 + BASE_WB + VA_WB)
#define VA_WB_CH1_BUFADD        (0x00000054 + BASE_WB + VA_WB)
#define VA_WB_CH0_BUFLNSTD      (0x00000060 + BASE_WB + VA_WB)
#define VA_WB_CH1_BUFLNSTD      (0x00000064 + BASE_WB + VA_WB)
#define VA_WB_CSCCOEF                (0x00000070 + BASE_WB + VA_WB)
#define VA_WB_SCLRX0                 (0x000000A0 + BASE_WB + VA_WB)
#define VA_WB_SCLRY0                 (0x000000A4 + BASE_WB + VA_WB)
#define VA_WB_SCLRX1                 (0x000000A8 + BASE_WB + VA_WB)
#define VA_WB_SCLRY1                 (0x000000AC + BASE_WB + VA_WB)
#define VA_WB_OFSTX0                 (0x000000B0 + BASE_WB + VA_WB)
#define VA_WB_OFSTY0                 (0x000000B4 + BASE_WB + VA_WB)
#define VA_WB_OFSTX1                 (0x000000B8 + BASE_WB + VA_WB)
#define VA_WB_OFSTY1                 (0x000000BC + BASE_WB + VA_WB)
#define VA_WB_SHIFTCTL               (0x000000C0 + BASE_WB + VA_WB)
#define VA_WB_DBG0                   (0x000000F0 + BASE_WB + VA_WB)
#define VA_WB_DBG1                   (0x000000F4 + BASE_WB + VA_WB)
#define VA_WB_DBG2                   (0x000000F8 + BASE_WB + VA_WB)
#define VA_WB_CH0XCOEF0              (0x00000400 + BASE_WB + VA_WB)
#define VA_WB_CH0YCOEF0              (0x00000500 + BASE_WB + VA_WB)
#define VA_WB_CH1XCOEF0              (0x00000600 + BASE_WB + VA_WB)
#define VA_WB_CH1YCOEF0              (0x00000700 + BASE_WB + VA_WB)
#define VA_WB_GAMMA                  (0x00000800 + BASE_WB + VA_WB)

#define DATA_WB_VER                  0x00001100
#define DATA_WB_FTR                  0x00000000
#define DATA_WB_CTL                  0x00000000
#define DATA_WB_STRMCTL              0x00000000
#define DATA_WB_CFG                  0x00000000
#define DATA_WB_STA                  0x00000000
#define DATA_WB_INTEN                0x00000000
#define DATA_WB_INTPD                0x00000000
#define DATA_WB_INTCLR               0x00000000
#define DATA_WB_SRCSIZE              0x00000000
#define DATA_WB_CROPSIZE             0x00000000
#define DATA_WB_CROPCOOR             0x00000000
#define DATA_WB_DSTSIZE0             0x00000000
#define DATA_WB_DSTSIZE1             0x00000000
#define DATA_WB_CH0_BUFADD      0x00000000
#define DATA_WB_CH1_BUFADD      0x00000000
#define DATA_WB_CH0_BUFLNSTD    0x00000000
#define DATA_WB_CH1_BUFLNSTD    0x00000000
#define DATA_WB_CSCCOEF              0x00000000
#define DATA_WB_SCLRX0               0x00000000
#define DATA_WB_SCLRY0               0x00000000
#define DATA_WB_SCLRX1               0x00000000
#define DATA_WB_SCLRY1               0x00000000
#define DATA_WB_OFSTX0               0x00000000
#define DATA_WB_OFSTY0               0x00000000
#define DATA_WB_OFSTX1               0x00000000
#define DATA_WB_OFSTY1               0x00000000
#define DATA_WB_SHIFTCTL             0x00000000
#define DATA_WB_DBG0                 0x0000FFFF
#define DATA_WB_DBG1                 0x00000000
#define DATA_WB_DBG2                 0x00000000
#define DATA_WB_CH0XCOEF0            0x00000000
#define DATA_WB_CH0YCOEF0            0x00000000
#define DATA_WB_CH1XCOEF0            0x00000000
#define DATA_WB_CH1YCOEF0            0x00000000
#define DATA_WB_GAMMA                0x00000000

/* Version Register */
typedef union {
	u32 val;
	struct {
	u32 ver_l:5;            /* The low 5bits of version register */
	u32 rsvd0:3;            /* - */
	u32 ver_h:3;            /* The high 3bits of version register */
	u32 rsvd1:1;            /* - */
	u32 comp:1;             /* Backward Compatibility
				 * [1¡¯b0]: Incompatible to last version
				 * of hardware
				 * [1¡¯b1]: Compatible to last version
				 * of hardware */
	u32 rsvd2:19;           /* - */
	} bits;
} reg_wb_ver_t;

#define WB_VER_COMP_0   0x0
#define WB_VER_COMP_1   0x1

/* Feature Register */
typedef union {
	u32 val;
	struct {
	u32 has_ftr0:1;         /* Support Gamma
				 * [0x0]: Non-support
				 * [0x1]: Support */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_wb_ftr_t;

#define WB_FTR_HAS_FTR0_0       0x0
#define WB_FTR_HAS_FTR0_1       0x1

/* Control Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;               /* Enable control
				 * Write 0 to this bit will disable the
				 * whole module and gate of all clocks.
				 * Write 1 to this bit will enable the
				 * clock of this module and reset the interrupt
				 * pending and timer status. */
	u32 rsvd0:3;            /* - */
	u32 continuemode:1;           /* Continue mode enable
				 * [0x0]: Single mode,WB will stop when
				 * current frame is finished.
				 * [0x1]: Continue mode, WB will start a new.
				 * frame when current frame is finished. */
	u32 rsvd1:3;            /* - */
	u32 dbc:1;              /* Double buffer register control, valid
				 * when DBAUTOLD is 1.
				 * [0x0]: Ignore
				 * [0x1]: Double buffer register will be
				 * loaded immediately when this bit is
				 * set to 1, and this bit will be clear
				 * to 0 when all double buffer registers
				 * have been loaded. When DBAUTOLD is 0,
				 * this bit will be ignored. */
	u32 dbautold:1;         /* Double buffer register auto-loading
				 * function disable
				 * [0x0]: Double buffer register auto-loading
				 * mode, the double buffer register will
				 * be auto-loaded when DC_SYNC is triggered.
				 * [0x1]: Double buffer register manual-loading
				 * mode, the double buffer register will
				 * be loaded when DBC is set. */
	u32 rsvd2:2;            /* - */
	u32 sramacc:1;          /* [0x0]: SRAM accessed by hardware. When
				 * this bit is ¡®0¡¯, AHB bus write request
				 * will be ignored, and read request will
				 * return ZERO.
				 * [0x1]: SRAM accessed by AHB bus. */
	u32 rsvd3:4;            /* - */
	u32 fieldsta:1;         /* Field signal state control
				 * [0x0]: Output low field signal to front
				 * module.
				 * [0x1]: Output high field signal to front
				 * module */
	u32 rsvd4:12;           /* - */
	u32 dbg_ctl:1;          /* Debug register access control
				 * 0x0: Debug register can¡¯t be read or written
				 * 0x1: Debug register can be read or written */
	u32 rst:1;              /* Write ¡°0¡± to this bit will reset the
				 * module.
				 * Write ¡°1¡± to this bit will release
				 * the reset signal. */
	} bits;
} reg_wb_ctl_t;

#define WB_CTL_ONLINE_0         0x0
#define WB_CTL_ONLINE_1         0x1
#define WB_CTL_DBC_0            0x0
#define WB_CTL_DBC_1            0x1
#define WB_CTL_DBAUTOLD_0       0x0
#define WB_CTL_DBAUTOLD_1       0x1
#define WB_CTL_SRAMACC_0        0x0
#define WB_CTL_SRAMACC_1        0x1
#define WB_CTL_FIELDSTA_0       0x0
#define WB_CTL_FIELDSTA_1       0x1

/* Stream Control Register */
typedef union {
	u32 val;
	struct {
	u32 start:1;            /* Stream Start control
				 * Write 1 to this bit will start streaming
				 * frames immediately in offline mode,
				 * or will auto start at the DC_SYNC falling
				 * edge in online mode.
				 * Self clear when Wb starts process. */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_wb_strmctl_t;

/* Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 fmt:2;              /* WB format select:
				 * [0x0]:ARGB8888
				 * [0x1]:RGB565
				 * [0x2]:YUV422SP
				 * [0x3]: YUV420SP */
	u32 rsvd0:14;           /* - */
	u32 rsmpen:1;           /* Resample function enable
				 * [0x0]: Disable resample function.
				 * [0x1]: Enable resample function. */
	u32 cscen:1;            /* Color space conversion function enable
				 * [0x0]: Disable.
				 * [0x1]: Enable. */
	u32 rsvd1:14;           /* - */
	} bits;
} reg_wb_cfg_t;

#define WB_CFG_FMT_0    0x0
#define WB_CFG_FMT_1    0x1
#define WB_CFG_FMT_2    0x2
#define WB_CFG_FMT_3    0x3
#define WB_CFG_RSMPEN_0 0x0
#define WB_CFG_RSMPEN_1 0x1
#define WB_CFG_CSCEN_0  0x0
#define WB_CFG_CSCEN_1  0x1

/* Status Register */
typedef union {
	u32 val;
	struct {
	u32 sta:1;              /* [0x0]: Dma is idle.
				 * [0x1]: Dma is busy */
	u32 rsvd0:7;            /* - */
	u32 to:1;               /* [0x0]:Write back finish in time
				 * [0x1]:Write back timeout. It shows that
				 * the time write back takes is more than
				 * WBTHR in offline mode, or write back
				 * has not finished at the DC_SYNC rising edge.
				 */
	u32 ovfl:1;             /* [0x0]:Write back fifo is normal
				 * [0x1]:Write back fifo is overflow */
	u32 rsvd1:22;           /* - */
	} bits;
} reg_wb_sta_t;

#define WB_STA_STA_0    0x0
#define WB_STA_STA_1    0x1
#define WB_STA_TO_0     0x0
#define WB_STA_TO_1     0x1
#define WB_STA_OVFL_0   0x0
#define WB_STA_OVFL_1   0x1

/* Interrupt Enable Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:16;           /* - */
	u32 wberr:1;            /* Write back error interrupt enable
				 * [0x0]: Disabled
				 * [0x1]: Enabled */
	u32 rsvd1:14;           /* - */
	u32 wbfnsh:1;           /* Write back finish interrupt enable
				 * [0x0]: Disabled
				 * [0x1]: Enabled */
	} bits;
} reg_wb_inten_t;

#define WB_INTEN_WBERR_0        0x0
#define WB_INTEN_WBERR_1        0x1
#define WB_INTEN_WBFNSH_0       0x0
#define WB_INTEN_WBFNSH_1       0x1

/* Interrupt Pending Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:16;           /* - */
	u32 wberr:1;            /* Write back error interrupt pending
				 * [0x0]: No write back error happens
				 * [0x1]: WBOVFL or WBTO happens */
	u32 rsvd1:14;           /* - */
	u32 wbfnsh:1;           /* Write back finish interrupt pending
				 * [0x0]: Write back has not finished
				 * [0x1]: Write back has finished */
	} bits;
} reg_wb_intpd_t;

#define WB_INTPD_WBERR_0        0x0
#define WB_INTPD_WBERR_1        0x1
#define WB_INTPD_WBFNSH_0       0x0
#define WB_INTPD_WBFNSH_1       0x1

/* Interrupt Clear Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:16;           /* - */
	u32 wberr:1;            /* Write ¡®1¡¯ to clear INTPD.WBERR
				 * Write ¡®0¡¯ will be ignored */
	u32 rsvd1:14;           /* - */
	u32 wbfnsh:1;           /* Write ¡®1¡¯ to clear INTPD.WBFNSH
				 * Write ¡®0¡¯ will be ignored */
	} bits;
} reg_wb_intclr_t;

/* Source Size Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Input imge width in pixels
				 * Actual input width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Input image height in pixels
				 * Actual input height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_wb_srcsize_t;

/* Crop Size Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Crop width in pixels
				 * Actual input width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Crop height in pixels
				 * Actual input height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_wb_cropsize_t;

/* Crop Coordinate Register */
typedef union {
	u32 val;
	struct {
	u32 x:13;               /* X coordinate of crop area in pixels,
				 * Only support positive coordinate. The
				 * value is the x location of crop area
				 * in the input image. */
	u32 rsvd0:3;            /* - */
	u32 y:13;               /* Y coordinate of crop area in pixels,
				 * Only support positive coordinate.  The
				 * value is the y location of crop area
				 * in the input image. */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_wb_cropcoor_t;

/* Destination Size0 Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Output imge width in pixels
				 * Actual input width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Output image height in pixels
				 * Actual input height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_wb_dstsize0_t;

/* Destination Size1 Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Output imge width in pixels
				 * Actual input width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Output image height in pixels
				 * Actual input height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_wb_dstsize1_t;

/* Output Buffer Address Register */
typedef union {
	u32 val;
	struct {
	u32 add:32;     /* Output buffer start address in bytes for channel
			 * [CH]. */
	} bits;
} reg_wb_ch0_bufadd_t;

/* Output Buffer Address Register */
typedef union {
	u32 val;
	struct {
	u32 add:32;     /* Output buffer start address in bytes for channel
			 * [CH]. */
	} bits;
} reg_wb_ch1_bufadd_t;

/* Output Buffer Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 lnstd:32;           /* Output frame buffer line stride in bytes
				 * for channel [CH]. */
	} bits;
} reg_wb_ch0_buflnstd_t;

/* Output Buffer Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 lnstd:32;           /* Output frame buffer line stride in bytes
				 * for channel [CH]. */
	} bits;
} reg_wb_ch1_buflnstd_t;

/* CSC Coefficient Register */
typedef union {
	u32 val;
	struct {
	u32 coef:15;            /* Color space conversion coefficient */
	u32 rsvd0:17;           /* - */
	} bits;
} reg_wb_csccoef_t;

/* Scaling X Ratio 0 Register */
typedef union {
	u32 val;
	struct {
	u32 sclr:22;            /* Scaling ratio in x-direction for channel 0 */
	u32 rsvd0:10;           /* - */
	} bits;
} reg_wb_sclrx0_t;

/* Scaling Y Ratio 0 Register */
typedef union {
	u32 val;
	struct {
	u32 sclr:22;            /* Scaling ratio in y-direction for channel 0 */
	u32 rsvd0:10;           /* - */
	} bits;
} reg_wb_sclry0_t;

/* Scaling X Ratio 1 Register */
typedef union {
	u32 val;
	struct {
	u32 sclr:22;            /* Scaling ratio in x-direction for channel 1 */
	u32 rsvd0:10;           /* - */
	} bits;
} reg_wb_sclrx1_t;

/* Scaling Y Ratio 1 Register */
typedef union {
	u32 val;
	struct {
	u32 sclr:22;            /* Scaling ratio in y-direction for channel 1 */
	u32 rsvd0:10;           /* - */
	} bits;
} reg_wb_sclry1_t;

/* Initial Phase Xoffset0 Register */
typedef union {
	u32 val;
	struct {
	u32 ofst:19;            /* Initial Phase offset in x-direction
				 * for channel 0. */
	u32 rsvd0:13;           /* - */
	} bits;
} reg_wb_ofstx0_t;

/* Initial Phase Yoffset0 Register */
typedef union {
	u32 val;
	struct {
	u32 ofst:19;            /* Initial Phase offset in y-direction
				 * for channel 0. */
	u32 rsvd0:13;           /* - */
	} bits;
} reg_wb_ofsty0_t;

/* Initial Phase Xoffset1 Register */
typedef union {
	u32 val;
	struct {
	u32 ofst:19;            /* Initial Phase offset in x-direction
				 * for channel 1. */
	u32 rsvd0:13;           /* - */
	} bits;
} reg_wb_ofstx1_t;

/* Initial Phase Yoffset1 Register */
typedef union {
	u32 val;
	struct {
	u32 ofst:19;            /* Initial Phase offset in y-direction
				 * for channel 1. */
	u32 rsvd0:13;           /* - */
	} bits;
} reg_wb_ofsty1_t;

/* Shift Control Register */
typedef union {
	u32 val;
	struct {
	u32 x0:2;               /* Shift for x-direction of channel 0/3. */
	u32 y0:2;               /* Shift for y-direction of channel 0/3. */
	u32 x1:2;               /* Shift for x-direction of channel 1/2. */
	u32 y1:2;               /* Shift for y-direction of channel 1/2. */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_wb_shiftctl_t;

/* Debug 0 Register */
typedef union {
	u32 val;
	struct {
	u32 wbthr:16;           /* When wb finished, if WBTMR excess this
				 * threshold, timeout flag will be set. */
	u32 wbtmr:16;           /* Elapse time of the last process in timer
				 * clock cycles. The value will be reset
				 * to 0 when module enable or wb start,
				 * and will be updated after wb finished.
				 * Its clock parent is module core clock. */
	} bits;
} reg_wb_dbg0_t;

/* Debug 1 Register */
typedef union {
	u32 val;
	struct {
	u32 bstlen:2;           /* Max burst length, the exact number of
				 * transfers in a burst for write back.
				 * [0x0]: 2 burst
				 * [0x1]: 4 burst
				 * [0x2]: 8 burst
				 * [0x3]: 16 burst */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_wb_dbg1_t;

#define WB_DBG1_BSTLEN_0        0x0
#define WB_DBG1_BSTLEN_1        0x1
#define WB_DBG1_BSTLEN_2        0x2
#define WB_DBG1_BSTLEN_3        0x3

/* Debug 2 Register */
typedef union {
	u32 val;
	struct {
	u32 x:13;               /* Output x coordinate of in pixels for
				 * channel 0. It shows the x location of
				 * current processing pixels. */
	u32 rsvd0:3;            /* - */
	u32 y:13;               /* Output y coordinate of in pixels for
				 * channel 0. It shows the y location of
				 * current processing pixels. */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_wb_dbg2_t;

/* CH0 X-direction Filter Coefficient 0 Register */
typedef union {
	u32 val;
	struct {
	u32 c0:8;       /* Scaling filter coefficient0 in x-direction for
			 * channel 0. */
	u32 c1:8;       /* Scaling filter coefficient1 in x-direction for
			 * channel 0. */
	u32 c2:8;       /* Scaling filter coefficient2 in x-direction for
			 * channel 0. */
	u32 c3:8;       /* Scaling filter coefficient3 in x-direction for
			 * channel 0. */
	} bits;
} reg_wb_ch0xcoef0_t;

/* CH0 Y-direction Filter Coefficient 0 Register */
typedef union {
	u32 val;
	struct {
	u32 c0:8;       /* Scaling filter coefficient0 in y-direction for
			 * channel 0. */
	u32 c1:8;       /* Scaling filter coefficient1 in y-direction for
			 * channel 0. */
	u32 c2:8;       /* Scaling filter coefficient2 in y-direction for
			 * channel 0. */
	u32 c3:8;       /* Scaling filter coefficient3 in y-direction for
			 * channel 0. */
	} bits;
} reg_wb_ch0ycoef0_t;

/* CH1 X-direction Filter Coefficient 0 Register */
typedef union {
	u32 val;
	struct {
	u32 c0:8;       /* Scaling filter coefficient0 in x-direction for
			 * channel 1. */
	u32 c1:8;       /* Scaling filter coefficient1 in x-direction for
			 * channel 1. */
	u32 c2:8;       /* Scaling filter coefficient2 in x-direction for
			 * channel 1. */
	u32 c3:8;       /* Scaling filter coefficient3 in x-direction for
			 * channel 1. */
	} bits;
} reg_wb_ch1xcoef0_t;

/* CH1 Y-direction Filter Coefficient 0 Register */
typedef union {
	u32 val;
	struct {
	u32 c0:8;       /* Scaling filter coefficient0 in y-direction for
			 * channel 1. */
	u32 c1:8;       /* Scaling filter coefficient1 in y-direction for
			 * channel 1. */
	u32 c2:8;       /* Scaling filter coefficient2 in y-direction for
			 * channel 1. */
	u32 c3:8;       /* Scaling filter coefficient3 in y-direction for
			 * channel 1. */
	} bits;
} reg_wb_ch1ycoef0_t;

/* reg_wb_t bank */
typedef struct tag_wb {
	reg_wb_ver_t           ver;            /* 0000 */
	reg_wb_ftr_t           ftr;            /* 0004 */
	u32                    res0[2];
	reg_wb_ctl_t           ctl;            /* 0010 */
	reg_wb_strmctl_t       strmctl;        /* 0014 */
	reg_wb_cfg_t           cfg;            /* 0018 */
	reg_wb_sta_t           sta;            /* 001C */
	reg_wb_inten_t         inten;          /* 0020 */
	reg_wb_intpd_t         intpd;          /* 0024 */
	reg_wb_intclr_t        intclr;         /* 0028 */
	u32                    res1[1];
	reg_wb_srcsize_t       srcsize;        /* 0030 */
	reg_wb_cropsize_t      cropsize;       /* 0034 */
	reg_wb_cropcoor_t      cropcoor;       /* 0038 */
	u32                    res2[1];
	reg_wb_dstsize0_t      dstsize0;       /* 0040 */
	reg_wb_dstsize1_t      dstsize1;       /* 0044 */
	u32                    res3[2];
	reg_wb_ch0_bufadd_t    ch0_bufadd;     /* 0050 */
	reg_wb_ch1_bufadd_t    ch1_bufadd;     /* 0054 */
	u32                    res4[2];
	reg_wb_ch0_buflnstd_t  ch0_buflnstd;   /* 0060 */
	reg_wb_ch1_buflnstd_t  ch1_buflnstd;   /* 0064 */
	u32                    res5[2];
	reg_wb_csccoef_t       csccoef;        /* 0070 */
	u32                    res6[11];
	reg_wb_sclrx0_t        sclrx0;         /* 00A0 */
	reg_wb_sclry0_t        sclry0;         /* 00A4 */
	reg_wb_sclrx1_t        sclrx1;         /* 00A8 */
	reg_wb_sclry1_t        sclry1;         /* 00AC */
	reg_wb_ofstx0_t        ofstx0;         /* 00B0 */
	reg_wb_ofsty0_t        ofsty0;         /* 00B4 */
	reg_wb_ofstx1_t        ofstx1;         /* 00B8 */
	reg_wb_ofsty1_t        ofsty1;         /* 00BC */
	reg_wb_shiftctl_t      shiftctl;       /* 00C0 */
	u32                    res7[11];
	reg_wb_dbg0_t          dbg0;           /* 00F0 */
	reg_wb_dbg1_t          dbg1;           /* 00F4 */
	reg_wb_dbg2_t          dbg2;           /* 00F8 */
	u32                    res8[193];
	reg_wb_ch0xcoef0_t     ch0xcoef0;      /* 0400 */
	u32                    res9[63];
	reg_wb_ch0ycoef0_t     ch0ycoef0;      /* 0500 */
	u32                    res10[63];
	reg_wb_ch1xcoef0_t     ch1xcoef0;      /* 0600 */
	u32                    res11[63];
	reg_wb_ch1ycoef0_t     ch1ycoef0;      /* 0700 */
	u32                    res12[63];
	u32                    prv_0800;       /* 0800 */
} reg_wb_t;

#endif /* ___WB___H___ */
