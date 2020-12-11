/* ove.h */

#ifndef ___OVE___H___
#define ___OVE___H___

#define BASE_OVE                0x01530000

#define VA_OVE_CTL                       (0x00000010 + BASE_OVE + VA_OVE)
#define VA_OVE_SRAMACC                   (0x00000014 + BASE_OVE + VA_OVE)
#define VA_OVE_CFG                       (0x00000018 + BASE_OVE + VA_OVE)
#define VA_OVE_STA                       (0x0000001C + BASE_OVE + VA_OVE)
#define VA_OVE_WINSIZE                   (0x00000020 + BASE_OVE + VA_OVE)
#define VA_OVE_BKCOLOR                   (0x00000024 + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLCFG           (0x00000030 + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLCFG           (0x00000034 + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLCFG           (0x00000038 + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLCFG           (0x0000003C + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLSIZE          (0x00000040 + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLSIZE          (0x00000044 + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLSIZE          (0x00000048 + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLSIZE          (0x0000004C + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLCOOR          (0x00000050 + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLCOOR          (0x00000054 + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLCOOR          (0x00000058 + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLCOOR          (0x0000005C + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLBUFLADD       (0x00000060 + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLBUFLADD       (0x00000064 + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLBUFLADD       (0x00000068 + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLBUFLADD       (0x0000006C + BASE_OVE + VA_OVE)
#define VA_OVE_OVLBUFHADD                (0x00000070 + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLLNSTD         (0x00000080 + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLLNSTD         (0x00000084 + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLLNSTD         (0x00000088 + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLLNSTD         (0x0000008C + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLPAL0          (0x00000090 + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLPAL0          (0x000000A0 + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLPAL0          (0x000000B0 + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLPAL0          (0x000000C0 + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLPAL1          (0x00000094 + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLPAL1          (0x000000A4 + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLPAL1          (0x000000B4 + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLPAL1          (0x000000C4 + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLPAL2          (0x00000098 + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLPAL2          (0x000000A8 + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLPAL2          (0x000000B8 + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLPAL2          (0x000000C8 + BASE_OVE + VA_OVE)
#define VA_OVE_CH0_OVLPAL3          (0x0000009C + BASE_OVE + VA_OVE)
#define VA_OVE_CH1_OVLPAL3          (0x000000AC + BASE_OVE + VA_OVE)
#define VA_OVE_CH2_OVLPAL3          (0x000000BC + BASE_OVE + VA_OVE)
#define VA_OVE_CH3_OVLPAL3          (0x000000CC + BASE_OVE + VA_OVE)
#define VA_OVE_DBG0                      (0x000000F0 + BASE_OVE + VA_OVE)
#define VA_OVE_DBG1                      (0x000000F4 + BASE_OVE + VA_OVE)
#define VA_OVE_PAL                       (0x00000400 + BASE_OVE + VA_OVE)

#define DATA_OVE_CTL                     0x00000000
#define DATA_OVE_SRAMACC                 0x00000000
#define DATA_OVE_CFG                     0x00000000
#define DATA_OVE_STA                     0x00000000
#define DATA_OVE_WINSIZE                 0x00000000
#define DATA_OVE_BKCOLOR                 0x00000000
#define DATA_OVE_CH0_OVLCFG         0x00000000
#define DATA_OVE_CH1_OVLCFG         0x00000000
#define DATA_OVE_CH2_OVLCFG         0x00000000
#define DATA_OVE_CH3_OVLCFG         0x00000000
#define DATA_OVE_CH0_OVLSIZE        0x00000000
#define DATA_OVE_CH1_OVLSIZE        0x00000000
#define DATA_OVE_CH2_OVLSIZE        0x00000000
#define DATA_OVE_CH3_OVLSIZE        0x00000000
#define DATA_OVE_CH0_OVLCOOR        0x00000000
#define DATA_OVE_CH1_OVLCOOR        0x00000000
#define DATA_OVE_CH2_OVLCOOR        0x00000000
#define DATA_OVE_CH3_OVLCOOR        0x00000000
#define DATA_OVE_CH0_OVLBUFLADD     0x00000000
#define DATA_OVE_CH1_OVLBUFLADD     0x00000000
#define DATA_OVE_CH2_OVLBUFLADD     0x00000000
#define DATA_OVE_CH3_OVLBUFLADD     0x00000000
#define DATA_OVE_OVLBUFHADD              0x00000000
#define DATA_OVE_CH0_OVLLNSTD       0x00000000
#define DATA_OVE_CH1_OVLLNSTD       0x00000000
#define DATA_OVE_CH2_OVLLNSTD       0x00000000
#define DATA_OVE_CH3_OVLLNSTD       0x00000000
#define DATA_OVE_CH0_OVLPAL0        0x00000000
#define DATA_OVE_CH1_OVLPAL0        0x00000000
#define DATA_OVE_CH2_OVLPAL0        0x00000000
#define DATA_OVE_CH3_OVLPAL0        0x00000000
#define DATA_OVE_CH0_OVLPAL1        0x00000000
#define DATA_OVE_CH1_OVLPAL1        0x00000000
#define DATA_OVE_CH2_OVLPAL1        0x00000000
#define DATA_OVE_CH3_OVLPAL1        0x00000000
#define DATA_OVE_CH0_OVLPAL2        0x00000000
#define DATA_OVE_CH1_OVLPAL2        0x00000000
#define DATA_OVE_CH2_OVLPAL2        0x00000000
#define DATA_OVE_CH3_OVLPAL2        0x00000000
#define DATA_OVE_CH0_OVLPAL3        0x00000000
#define DATA_OVE_CH1_OVLPAL3        0x00000000
#define DATA_OVE_CH2_OVLPAL3        0x00000000
#define DATA_OVE_CH3_OVLPAL3        0x00000000
#define DATA_OVE_DBG0                    0x00000000
#define DATA_OVE_DBG1                    0x00000000
#define DATA_OVE_PAL                     0x00000000

/* Control Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;               /* Enable Module
				 * [0x0]:Disable the whole module and gate
				 * all clocks.
				 * [0x1]:Enable the whole module. */
	u32 rsvd0:7;            /* - */
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
				 * be auto-loaded at the DC_SYNC falling edge.
				 * [0x1]: Double buffer register manual-loading
				 * mode, the double buffer register will
				 * be loaded when DBC is set. */
	u32 rsvd1:6;            /* - */
	u32 outmode:1;          /* Output mode
				 * [0x0]:output as progreesive mode
				 * [0x1]:output as interlaced mode
				 * In interlaced mode, dc will output odd/even
				 * field of image according to field signal
				 * from tcon and FIELDPOL. */
	u32 fieldpol:1;         /* Field signal polarity:
				 * [0x0]:
				 * Field 1(when low field signal) output
				 * 0/2/4/.. line
				 * Field 2(when high field signal) output
				 * 1/3/5/.. line
				 * [0x1]:
				 * Field 1(when low field signal) output
				 * 1/3/5/.. line
				 * Field 2(when high field signal) output
				 * 0/2/4/.. line */
	u32 rsvd2:12;           /* - */
	u32 dbg_ctl:1;          /* Debug register access control
				 * 0x0: Debug register can¡¯t be read or written
				 * 0x1: Debug register can be read or written */
	u32 rsvd3:1;            /* - */
	} bits;
} reg_ove_ctl_t;

#define OVE_CTL_EN_0            0x0
#define OVE_CTL_EN_1            0x1
#define OVE_CTL_DBC_0           0x0
#define OVE_CTL_DBC_1           0x1
#define OVE_CTL_DBAUTOLD_0      0x0
#define OVE_CTL_DBAUTOLD_1      0x1
#define OVE_CTL_OUTMODE_0       0x0
#define OVE_CTL_OUTMODE_1       0x1
#define OVE_CTL_FIELDPOL_0      0x0
#define OVE_CTL_FIELDPOL_1      0x1

/* Control Register */
typedef union {
	u32 val;
	struct {
	u32 sramacc:1;          /* [0x0]: SRAM accessed by hardware. When
				 * this bit is ¡®0¡¯, AHB bus write request
				 * will be ignored, and read request will
				 * return ZERO.
				 * [0x1]: SRAM accessed by AHB bus. */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_ove_sramacc_t;

#define OVE_SRAMACC_SRAMACC_0   0x0
#define OVE_SRAMACC_SRAMACC_1   0x1

/* Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 ovlen0:1;           /* Overlay 0 enable control
				 * [0x0]:Disable overlay.
				 * [0x1]:Enable overlay. */
	u32 ovlen1:1;           /* Overlay 1 enable control
				 * [0x0]:Disable overlay.
				 * [0x1]:Enable overlay. */
	u32 ovlen2:1;           /* Overlay 2 enable control
				 * [0x0]:Disable overlay.
				 * [0x1]:Enable overlay. */
	u32 ovlen3:1;           /* Overlay 3 enable control
				 * [0x0]:Disable overlay.
				 * [0x1]:Enable overlay. */
	u32 rsvd0:4;           /* - */
	u32 outsel:2;          /* Output select
			     * [0x0]:DC.
				 * [0x1]:SE0.
				 * [0x2]:SE1. */
	u32 rsvd1:22;           /* - */
	} bits;
} reg_ove_cfg_t;

#define OVE_CFG_OVLEN0_0        0x0
#define OVE_CFG_OVLEN0_1        0x1
#define OVE_CFG_OVLEN1_0        0x0
#define OVE_CFG_OVLEN1_1        0x1
#define OVE_CFG_OVLEN2_0        0x0
#define OVE_CFG_OVLEN2_1        0x1
#define OVE_CFG_OVLEN3_0        0x0
#define OVE_CFG_OVLEN3_1        0x1

/* Status Register */
typedef union {
	u32 val;
	struct {
	u32 insta:1;            /* [0x0]:Input dma is idle.
				 * [0x1]:Input dma is busy */
	u32 rsvd0:3;            /* - */
	u32 rststa:1;           /* Reset State
				 * [0x0]:Reset sucess
				 * [0x1]:Reset error */
	u32 rsvd1:11;           /* - */
	u32 framecnt:16;        /* Frame counter
				 * The frame counter is auto increased
				 * by 1 when DC_SYNC is triggered. */
	} bits;
} reg_ove_sta_t;

#define OVE_STA_INSTA_0         0x0
#define OVE_STA_INSTA_1         0x1
#define OVE_STA_RSTSTA_0        0x0
#define OVE_STA_RSTSTA_1        0x1

/* Window Size Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Window width in pixels
				 * Actual window width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Window height in pixels
				 * Actual window height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_winsize_t;

/* Back Color Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_bkcolor_t;

/* Overlay Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:4;            /* - */
	u32 wkmod:1;            /* Overlay work mode
				 * [2¡¯b00]:normal mode
				 * [2¡¯b01]:palette mode */
	u32 rsvd1:3;            /* - */
	u32 fmt:5;              /* Overlay buffer input image pixel data format
				 * Normal mode:
				 * [5¡¯b00000]:ARGB8888
				 * [5¡¯b00001]:ABGR8888
				 * [5¡¯b00010]:BGRA8888
				 * [5¡¯b00011]:RGBA8888
				 * [5¡¯b00100]:ARGB4444
				 * [5¡¯b00101]:ABGR4444
				 * [5¡¯b00110]:BGRA4444
				 * [5¡¯b00111]:RGBA4444
				 * [5¡¯b01000]:ARGB1555
				 * [5¡¯b01001]:ABGR1555
				 * [5¡¯b01010]:BGRA5551
				 * [5¡¯b01011]:RGBA5551
				 * [5¡¯b01100]:RGB565
				 * [5¡¯b01101]:BGR565
				 * [5¡¯b01110]:BGR565
				 * [5¡¯b01111]:RGB565
				 * [5¡¯b10000]:RGB888
				 * [5¡¯b10001]:BGR888
				 * [5¡¯b10010]:BGR888
				 * [5¡¯b10011]:RGB888
				 * (bit0: BR Swap, bit1: Component Swap)
				 * Palette mode:
				 * 5¡¯b00000: 1bpp
				 * 5¡¯b00001: 2bpp
				 * 5¡¯b00010: 4bpp
				 * 5¡¯b00011: 8bpp
				 * Other: Reserved */
	u32 rsvd2:3;            /* - */
	u32 colorctl:2;         /* Overlay color value control
				 * [2¡¯b00]: color = original color
				 * [2¡¯b01]: color = color * pixel alpha value
				 * [2¡¯b10]: color = color * plane alpha value
				 * [2¡¯b11]: color = color * pixel alpha
				 * value * plane alpha value */
	u32 rsvd3:2;            /* - */
	u32 alphactl:2;         /* Overlay alpha value control
				 * [2¡¯b00]: alpha = pixel alpha value
				 * [2¡¯b01]: alpha = plane alpha value
				 * [2¡¯b10]: alpha = pixel alpha value
				 * * plane alpha value
				 * [2¡¯b11]: reserved */
	u32 rsvd4:2;            /* - */
	u32 plaalpha:8;         /* Plane alpha value */
	} bits;
} reg_ove_ch0_ovlcfg_t;

#define OVE_CH0_OVLCFG_WKMOD_0          0x0
#define OVE_CH0_OVLCFG_WKMOD_1          0x1
#define OVE_CH0_OVLCFG_FMT_0            0x0
#define OVE_CH0_OVLCFG_FMT_1            0x1
#define OVE_CH0_OVLCFG_FMT_2            0x2
#define OVE_CH0_OVLCFG_FMT_3            0x3
#define OVE_CH0_OVLCFG_FMT_4            0x4
#define OVE_CH0_OVLCFG_FMT_5            0x5
#define OVE_CH0_OVLCFG_FMT_6            0x6
#define OVE_CH0_OVLCFG_FMT_7            0x7
#define OVE_CH0_OVLCFG_FMT_8            0x8
#define OVE_CH0_OVLCFG_FMT_9            0x9
#define OVE_CH0_OVLCFG_FMT_A            0xA
#define OVE_CH0_OVLCFG_FMT_B            0xB
#define OVE_CH0_OVLCFG_FMT_C            0xC
#define OVE_CH0_OVLCFG_FMT_D            0xD
#define OVE_CH0_OVLCFG_FMT_E            0xE
#define OVE_CH0_OVLCFG_FMT_F            0xF
#define OVE_CH0_OVLCFG_FMT_10           0x10
#define OVE_CH0_OVLCFG_FMT_11           0x11
#define OVE_CH0_OVLCFG_FMT_12           0x12
#define OVE_CH0_OVLCFG_FMT_13           0x13
#define OVE_CH0_OVLCFG_COLORCTL_0       0x0
#define OVE_CH0_OVLCFG_COLORCTL_1       0x1
#define OVE_CH0_OVLCFG_COLORCTL_2       0x2
#define OVE_CH0_OVLCFG_COLORCTL_3       0x3
#define OVE_CH0_OVLCFG_ALPHACTL_0       0x0
#define OVE_CH0_OVLCFG_ALPHACTL_1       0x1
#define OVE_CH0_OVLCFG_ALPHACTL_2       0x2
#define OVE_CH0_OVLCFG_ALPHACTL_3       0x3

/* Overlay Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:4;            /* - */
	u32 wkmod:1;            /* Overlay work mode
				 * [2¡¯b00]:normal mode
				 * [2¡¯b01]:palette mode */
	u32 rsvd1:3;            /* - */
	u32 fmt:5;              /* Overlay buffer input image pixel data format
				 * Normal mode:
				 * [5¡¯b00000]:ARGB8888
				 * [5¡¯b00001]:ABGR8888
				 * [5¡¯b00010]:BGRA8888
				 * [5¡¯b00011]:RGBA8888
				 * [5¡¯b00100]:ARGB4444
				 * [5¡¯b00101]:ABGR4444
				 * [5¡¯b00110]:BGRA4444
				 * [5¡¯b00111]:RGBA4444
				 * [5¡¯b01000]:ARGB1555
				 * [5¡¯b01001]:ABGR1555
				 * [5¡¯b01010]:BGRA5551
				 * [5¡¯b01011]:RGBA5551
				 * [5¡¯b01100]:RGB565
				 * [5¡¯b01101]:BGR565
				 * [5¡¯b01110]:BGR565
				 * [5¡¯b01111]:RGB565
				 * [5¡¯b10000]:RGB888
				 * [5¡¯b10001]:BGR888
				 * [5¡¯b10010]:BGR888
				 * [5¡¯b10011]:RGB888
				 * (bit0: BR Swap, bit1: Component Swap)
				 * Palette mode:
				 * 5¡¯b00000: 1bpp
				 * 5¡¯b00001: 2bpp
				 * 5¡¯b00010: 4bpp
				 * 5¡¯b00011: 8bpp
				 * Other: Reserved */
	u32 rsvd2:3;            /* - */
	u32 colorctl:2;         /* Overlay color value control
				 * [2¡¯b00]: color = original color
				 * [2¡¯b01]: color = color * pixel alpha value
				 * [2¡¯b10]: color = color * plane alpha value
				 * [2¡¯b11]: color = color * pixel alpha
				 * value * plane alpha value */
	u32 rsvd3:2;            /* - */
	u32 alphactl:2;         /* Overlay alpha value control
				 * [2¡¯b00]: alpha = pixel alpha value
				 * [2¡¯b01]: alpha = plane alpha value
				 * [2¡¯b10]: alpha = pixel alpha value
				 * * plane alpha value
				 * [2¡¯b11]: reserved */
	u32 rsvd4:2;            /* - */
	u32 plaalpha:8;         /* Plane alpha value */
	} bits;
} reg_ove_ch1_ovlcfg_t;

#define OVE_CH1_OVLCFG_WKMOD_0          0x0
#define OVE_CH1_OVLCFG_WKMOD_1          0x1
#define OVE_CH1_OVLCFG_FMT_0            0x0
#define OVE_CH1_OVLCFG_FMT_1            0x1
#define OVE_CH1_OVLCFG_FMT_2            0x2
#define OVE_CH1_OVLCFG_FMT_3            0x3
#define OVE_CH1_OVLCFG_FMT_4            0x4
#define OVE_CH1_OVLCFG_FMT_5            0x5
#define OVE_CH1_OVLCFG_FMT_6            0x6
#define OVE_CH1_OVLCFG_FMT_7            0x7
#define OVE_CH1_OVLCFG_FMT_8            0x8
#define OVE_CH1_OVLCFG_FMT_9            0x9
#define OVE_CH1_OVLCFG_FMT_A            0xA
#define OVE_CH1_OVLCFG_FMT_B            0xB
#define OVE_CH1_OVLCFG_FMT_C            0xC
#define OVE_CH1_OVLCFG_FMT_D            0xD
#define OVE_CH1_OVLCFG_FMT_E            0xE
#define OVE_CH1_OVLCFG_FMT_F            0xF
#define OVE_CH1_OVLCFG_FMT_10           0x10
#define OVE_CH1_OVLCFG_FMT_11           0x11
#define OVE_CH1_OVLCFG_FMT_12           0x12
#define OVE_CH1_OVLCFG_FMT_13           0x13
#define OVE_CH1_OVLCFG_COLORCTL_0       0x0
#define OVE_CH1_OVLCFG_COLORCTL_1       0x1
#define OVE_CH1_OVLCFG_COLORCTL_2       0x2
#define OVE_CH1_OVLCFG_COLORCTL_3       0x3
#define OVE_CH1_OVLCFG_ALPHACTL_0       0x0
#define OVE_CH1_OVLCFG_ALPHACTL_1       0x1
#define OVE_CH1_OVLCFG_ALPHACTL_2       0x2
#define OVE_CH1_OVLCFG_ALPHACTL_3       0x3

/* Overlay Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:4;            /* - */
	u32 wkmod:1;            /* Overlay work mode
				 * [2¡¯b00]:normal mode
				 * [2¡¯b01]:palette mode */
	u32 rsvd1:3;            /* - */
	u32 fmt:5;              /* Overlay buffer input image pixel data format
				 * Normal mode:
				 * [5¡¯b00000]:ARGB8888
				 * [5¡¯b00001]:ABGR8888
				 * [5¡¯b00010]:BGRA8888
				 * [5¡¯b00011]:RGBA8888
				 * [5¡¯b00100]:ARGB4444
				 * [5¡¯b00101]:ABGR4444
				 * [5¡¯b00110]:BGRA4444
				 * [5¡¯b00111]:RGBA4444
				 * [5¡¯b01000]:ARGB1555
				 * [5¡¯b01001]:ABGR1555
				 * [5¡¯b01010]:BGRA5551
				 * [5¡¯b01011]:RGBA5551
				 * [5¡¯b01100]:RGB565
				 * [5¡¯b01101]:BGR565
				 * [5¡¯b01110]:BGR565
				 * [5¡¯b01111]:RGB565
				 * [5¡¯b10000]:RGB888
				 * [5¡¯b10001]:BGR888
				 * [5¡¯b10010]:BGR888
				 * [5¡¯b10011]:RGB888
				 * (bit0: BR Swap, bit1: Component Swap)
				 * Palette mode:
				 * 5¡¯b00000: 1bpp
				 * 5¡¯b00001: 2bpp
				 * 5¡¯b00010: 4bpp
				 * 5¡¯b00011: 8bpp
				 * Other: Reserved */
	u32 rsvd2:3;            /* - */
	u32 colorctl:2;         /* Overlay color value control
				 * [2¡¯b00]: color = original color
				 * [2¡¯b01]: color = color * pixel alpha value
				 * [2¡¯b10]: color = color * plane alpha value
				 * [2¡¯b11]: color = color * pixel alpha
				 * value * plane alpha value */
	u32 rsvd3:2;            /* - */
	u32 alphactl:2;         /* Overlay alpha value control
				 * [2¡¯b00]: alpha = pixel alpha value
				 * [2¡¯b01]: alpha = plane alpha value
				 * [2¡¯b10]: alpha = pixel alpha value
				 * * plane alpha value
				 * [2¡¯b11]: reserved */
	u32 rsvd4:2;            /* - */
	u32 plaalpha:8;         /* Plane alpha value */
	} bits;
} reg_ove_ch2_ovlcfg_t;

#define OVE_CH2_OVLCFG_WKMOD_0          0x0
#define OVE_CH2_OVLCFG_WKMOD_1          0x1
#define OVE_CH2_OVLCFG_FMT_0            0x0
#define OVE_CH2_OVLCFG_FMT_1            0x1
#define OVE_CH2_OVLCFG_FMT_2            0x2
#define OVE_CH2_OVLCFG_FMT_3            0x3
#define OVE_CH2_OVLCFG_FMT_4            0x4
#define OVE_CH2_OVLCFG_FMT_5            0x5
#define OVE_CH2_OVLCFG_FMT_6            0x6
#define OVE_CH2_OVLCFG_FMT_7            0x7
#define OVE_CH2_OVLCFG_FMT_8            0x8
#define OVE_CH2_OVLCFG_FMT_9            0x9
#define OVE_CH2_OVLCFG_FMT_A            0xA
#define OVE_CH2_OVLCFG_FMT_B            0xB
#define OVE_CH2_OVLCFG_FMT_C            0xC
#define OVE_CH2_OVLCFG_FMT_D            0xD
#define OVE_CH2_OVLCFG_FMT_E            0xE
#define OVE_CH2_OVLCFG_FMT_F            0xF
#define OVE_CH2_OVLCFG_FMT_10           0x10
#define OVE_CH2_OVLCFG_FMT_11           0x11
#define OVE_CH2_OVLCFG_FMT_12           0x12
#define OVE_CH2_OVLCFG_FMT_13           0x13
#define OVE_CH2_OVLCFG_COLORCTL_0       0x0
#define OVE_CH2_OVLCFG_COLORCTL_1       0x1
#define OVE_CH2_OVLCFG_COLORCTL_2       0x2
#define OVE_CH2_OVLCFG_COLORCTL_3       0x3
#define OVE_CH2_OVLCFG_ALPHACTL_0       0x0
#define OVE_CH2_OVLCFG_ALPHACTL_1       0x1
#define OVE_CH2_OVLCFG_ALPHACTL_2       0x2
#define OVE_CH2_OVLCFG_ALPHACTL_3       0x3

/* Overlay Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:4;            /* - */
	u32 wkmod:1;            /* Overlay work mode
				 * [2¡¯b00]:normal mode
				 * [2¡¯b01]:palette mode */
	u32 rsvd1:3;            /* - */
	u32 fmt:5;              /* Overlay buffer input image pixel data format
				 * Normal mode:
				 * [5¡¯b00000]:ARGB8888
				 * [5¡¯b00001]:ABGR8888
				 * [5¡¯b00010]:BGRA8888
				 * [5¡¯b00011]:RGBA8888
				 * [5¡¯b00100]:ARGB4444
				 * [5¡¯b00101]:ABGR4444
				 * [5¡¯b00110]:BGRA4444
				 * [5¡¯b00111]:RGBA4444
				 * [5¡¯b01000]:ARGB1555
				 * [5¡¯b01001]:ABGR1555
				 * [5¡¯b01010]:BGRA5551
				 * [5¡¯b01011]:RGBA5551
				 * [5¡¯b01100]:RGB565
				 * [5¡¯b01101]:BGR565
				 * [5¡¯b01110]:BGR565
				 * [5¡¯b01111]:RGB565
				 * [5¡¯b10000]:RGB888
				 * [5¡¯b10001]:BGR888
				 * [5¡¯b10010]:BGR888
				 * [5¡¯b10011]:RGB888
				 * (bit0: BR Swap, bit1: Component Swap)
				 * Palette mode:
				 * 5¡¯b00000: 1bpp
				 * 5¡¯b00001: 2bpp
				 * 5¡¯b00010: 4bpp
				 * 5¡¯b00011: 8bpp
				 * Other: Reserved */
	u32 rsvd2:3;            /* - */
	u32 colorctl:2;         /* Overlay color value control
				 * [2¡¯b00]: color = original color
				 * [2¡¯b01]: color = color * pixel alpha value
				 * [2¡¯b10]: color = color * plane alpha value
				 * [2¡¯b11]: color = color * pixel alpha
				 * value * plane alpha value */
	u32 rsvd3:2;            /* - */
	u32 alphactl:2;         /* Overlay alpha value control
				 * [2¡¯b00]: alpha = pixel alpha value
				 * [2¡¯b01]: alpha = plane alpha value
				 * [2¡¯b10]: alpha = pixel alpha value
				 * * plane alpha value
				 * [2¡¯b11]: reserved */
	u32 rsvd4:2;            /* - */
	u32 plaalpha:8;         /* Plane alpha value */
	} bits;
} reg_ove_ch3_ovlcfg_t;

#define OVE_CH3_OVLCFG_WKMOD_0          0x0
#define OVE_CH3_OVLCFG_WKMOD_1          0x1
#define OVE_CH3_OVLCFG_FMT_0            0x0
#define OVE_CH3_OVLCFG_FMT_1            0x1
#define OVE_CH3_OVLCFG_FMT_2            0x2
#define OVE_CH3_OVLCFG_FMT_3            0x3
#define OVE_CH3_OVLCFG_FMT_4            0x4
#define OVE_CH3_OVLCFG_FMT_5            0x5
#define OVE_CH3_OVLCFG_FMT_6            0x6
#define OVE_CH3_OVLCFG_FMT_7            0x7
#define OVE_CH3_OVLCFG_FMT_8            0x8
#define OVE_CH3_OVLCFG_FMT_9            0x9
#define OVE_CH3_OVLCFG_FMT_A            0xA
#define OVE_CH3_OVLCFG_FMT_B            0xB
#define OVE_CH3_OVLCFG_FMT_C            0xC
#define OVE_CH3_OVLCFG_FMT_D            0xD
#define OVE_CH3_OVLCFG_FMT_E            0xE
#define OVE_CH3_OVLCFG_FMT_F            0xF
#define OVE_CH3_OVLCFG_FMT_10           0x10
#define OVE_CH3_OVLCFG_FMT_11           0x11
#define OVE_CH3_OVLCFG_FMT_12           0x12
#define OVE_CH3_OVLCFG_FMT_13           0x13
#define OVE_CH3_OVLCFG_COLORCTL_0       0x0
#define OVE_CH3_OVLCFG_COLORCTL_1       0x1
#define OVE_CH3_OVLCFG_COLORCTL_2       0x2
#define OVE_CH3_OVLCFG_COLORCTL_3       0x3
#define OVE_CH3_OVLCFG_ALPHACTL_0       0x0
#define OVE_CH3_OVLCFG_ALPHACTL_1       0x1
#define OVE_CH3_OVLCFG_ALPHACTL_2       0x2
#define OVE_CH3_OVLCFG_ALPHACTL_3       0x3

/* Overlay Size Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Overlay width in pixels
				 * Actual overlay width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Overlay height in pixels
				 * Actual overlay height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_ch0_ovlsize_t;

/* Overlay Size Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Overlay width in pixels
				 * Actual overlay width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Overlay height in pixels
				 * Actual overlay height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_ch1_ovlsize_t;

/* Overlay Size Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Overlay width in pixels
				 * Actual overlay width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Overlay height in pixels
				 * Actual overlay height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_ch2_ovlsize_t;

/* Overlay Size Register */
typedef union {
	u32 val;
	struct {
	u32 w:13;               /* Overlay width in pixels
				 * Actual overlay width is W + 1 */
	u32 rsvd0:3;            /* - */
	u32 h:13;               /* Overlay height in pixels
				 * Actual overlay height is H + 1 */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_ch3_ovlsize_t;

/* Overlay Coordinate Register */
typedef union {
	u32 val;
	struct {
	u32 x:13;               /* Overlay x coordinate in pixels, only
				 * support positive coordinate. The value
				 * is the x location of overlay in the
				 * whole window. */
	u32 rsvd0:3;            /* - */
	u32 y:13;               /* Overlay y coordinate in pixels, only
				 * support positive coordinate. The value
				 * is the y location of overlay in the
				 * whole window. */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_ch0_ovlcoor_t;

/* Overlay Coordinate Register */
typedef union {
	u32 val;
	struct {
	u32 x:13;               /* Overlay x coordinate in pixels, only
				 * support positive coordinate. The value
				 * is the x location of overlay in the
				 * whole window. */
	u32 rsvd0:3;            /* - */
	u32 y:13;               /* Overlay y coordinate in pixels, only
				 * support positive coordinate. The value
				 * is the y location of overlay in the
				 * whole window. */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_ch1_ovlcoor_t;

/* Overlay Coordinate Register */
typedef union {
	u32 val;
	struct {
	u32 x:13;               /* Overlay x coordinate in pixels, only
				 * support positive coordinate. The value
				 * is the x location of overlay in the
				 * whole window. */
	u32 rsvd0:3;            /* - */
	u32 y:13;               /* Overlay y coordinate in pixels, only
				 * support positive coordinate. The value
				 * is the y location of overlay in the
				 * whole window. */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_ch2_ovlcoor_t;

/* Overlay Coordinate Register */
typedef union {
	u32 val;
	struct {
	u32 x:13;               /* Overlay x coordinate in pixels, only
				 * support positive coordinate. The value
				 * is the x location of overlay in the
				 * whole window. */
	u32 rsvd0:3;            /* - */
	u32 y:13;               /* Overlay y coordinate in pixels, only
				 * support positive coordinate. The value
				 * is the y location of overlay in the
				 * whole window. */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_ch3_ovlcoor_t;

/* Overlay Buffer Low 32bit Address Register */
typedef union {
	u32 val;
	struct {
	u32 add:32;     /* Overlay frame buffer start address in bits. */
	} bits;
} reg_ove_ch0_ovlbufladd_t;

/* Overlay Buffer Low 32bit Address Register */
typedef union {
	u32 val;
	struct {
	u32 add:32;     /* Overlay frame buffer start address in bits. */
	} bits;
} reg_ove_ch1_ovlbufladd_t;

/* Overlay Buffer Low 32bit Address Register */
typedef union {
	u32 val;
	struct {
	u32 add:32;     /* Overlay frame buffer start address in bits. */
	} bits;
} reg_ove_ch2_ovlbufladd_t;

/* Overlay Buffer Low 32bit Address Register */
typedef union {
	u32 val;
	struct {
	u32 add:32;     /* Overlay frame buffer start address in bits. */
	} bits;
} reg_ove_ch3_ovlbufladd_t;

/* Overlay Buffer High 4bit Address Register */
typedef union {
	u32 val;
	struct {
	u32 ovl0add:4;	/* Overlay0 frame buffer start address in bits. */
	u32 rsvd0:4;	/* - */
	u32 ovl1add:4;	/* Overlay1 frame buffer start address in bits. */
	u32 rsvd1:4;	/* - */
	u32 ovl2add:4;	/* Overlay2 frame buffer start address in bits. */
	u32 rsvd2:4;	/* - */
	u32 ovl3add:4;	/* Overlay3 frame buffer start address in bits. */
	u32 rsvd3:4;	/* - */
	} bits;
} reg_ove_ovlbufhadd_t;

/* Overlay Buffer Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 lnstd:32;           /* Overlay frame buffer line stride in bits. */
	} bits;
} reg_ove_ch0_ovllnstd_t;

/* Overlay Buffer Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 lnstd:32;           /* Overlay frame buffer line stride in bits. */
	} bits;
} reg_ove_ch1_ovllnstd_t;

/* Overlay Buffer Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 lnstd:32;           /* Overlay frame buffer line stride in bits. */
	} bits;
} reg_ove_ch2_ovllnstd_t;

/* Overlay Buffer Line Stride Register */
typedef union {
	u32 val;
	struct {
	u32 lnstd:32;           /* Overlay frame buffer line stride in bits. */
	} bits;
} reg_ove_ch3_ovllnstd_t;

/* Overlay Palette 0 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch0_ovlpal0_t;

/* Overlay Palette 0 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch1_ovlpal0_t;

/* Overlay Palette 0 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch2_ovlpal0_t;

/* Overlay Palette 0 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch3_ovlpal0_t;

/* Overlay Palette 1 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch0_ovlpal1_t;

/* Overlay Palette 1 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch1_ovlpal1_t;

/* Overlay Palette 1 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch2_ovlpal1_t;

/* Overlay Palette 1 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch3_ovlpal1_t;

/* Overlay Palette 2 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch0_ovlpal2_t;

/* Overlay Palette 2 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch1_ovlpal2_t;

/* Overlay Palette 2 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch2_ovlpal2_t;

/* Overlay Palette 2 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch3_ovlpal2_t;

/* Overlay Palette 3 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch0_ovlpal3_t;

/* Overlay Palette 3 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch1_ovlpal3_t;

/* Overlay Palette 3 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch2_ovlpal3_t;

/* Overlay Palette 3 Register */
typedef union {
	u32 val;
	struct {
	u32 b:8;        /* Blue value */
	u32 g:8;        /* Green value */
	u32 r:8;        /* Red value */
	u32 a:8;        /* Alpha value */
	} bits;
} reg_ove_ch3_ovlpal3_t;

/* Debug 0 Register */
typedef union {
	u32 val;
	struct {
	u32 bstlen:2;           /* Max burst length, the exact number of
				 * transfers in a burs.
				 * [0x0]: 2 burst
				 * [0x1]: 4 burst
				 * [0x2]: 8 burst
				 * [0x3]: 16 burst */
	u32 rsvd0:29;           /* - */
	u32 rst:1;              /* Soft Reset the module
				 * [0x0]: Release
				 * [0x1]: Reset the module core, and clear
				 * status register. */
	} bits;
} reg_ove_dbg0_t;

#define OVE_DBG0_BSTLEN_0       0x0
#define OVE_DBG0_BSTLEN_1       0x1
#define OVE_DBG0_BSTLEN_2       0x2
#define OVE_DBG0_BSTLEN_3       0x3
#define OVE_DBG0_RST_0          0x0
#define OVE_DBG0_RST_1          0x1

/* Debug 1 Register */
typedef union {
	u32 val;
	struct {
	u32 x:13;               /* Window x coordinate of in pixels. It
				 * shows the x location of current processing
				 * pixels. */
	u32 rsvd0:3;            /* - */
	u32 y:13;               /* Window y coordinate of in pixels. It
				 * shows the y location of current processing
				 * pixels. */
	u32 rsvd1:3;            /* - */
	} bits;
} reg_ove_dbg1_t;

/* reg_ove_t bank */
typedef struct tag_ove {
	u32                        res[4];
	reg_ove_ctl_t              ctl;            /* 0010 */
	reg_ove_sramacc_t          sramacc;        /* 0014 */
	reg_ove_cfg_t              cfg;            /* 0018 */
	reg_ove_sta_t              sta;            /* 001C */
	reg_ove_winsize_t          winsize;        /* 0020 */
	reg_ove_bkcolor_t          bkcolor;        /* 0024 */
	u32                        res0[2];
	reg_ove_ch0_ovlcfg_t       ch0_ovlcfg;     /* 0030 */
	reg_ove_ch1_ovlcfg_t       ch1_ovlcfg;     /* 0034 */
	reg_ove_ch2_ovlcfg_t       ch2_ovlcfg;     /* 0038 */
	reg_ove_ch3_ovlcfg_t       ch3_ovlcfg;     /* 003C */
	reg_ove_ch0_ovlsize_t      ch0_ovlsize;    /* 0040 */
	reg_ove_ch1_ovlsize_t      ch1_ovlsize;    /* 0044 */
	reg_ove_ch2_ovlsize_t      ch2_ovlsize;    /* 0048 */
	reg_ove_ch3_ovlsize_t      ch3_ovlsize;    /* 004C */
	reg_ove_ch0_ovlcoor_t      ch0_ovlcoor;    /* 0050 */
	reg_ove_ch1_ovlcoor_t      ch1_ovlcoor;    /* 0054 */
	reg_ove_ch2_ovlcoor_t      ch2_ovlcoor;    /* 0058 */
	reg_ove_ch3_ovlcoor_t      ch3_ovlcoor;    /* 005C */
	reg_ove_ch0_ovlbufladd_t   ch0_ovlbufladd; /* 0060 */
	reg_ove_ch1_ovlbufladd_t   ch1_ovlbufladd; /* 0064 */
	reg_ove_ch2_ovlbufladd_t   ch2_ovlbufladd; /* 0068 */
	reg_ove_ch3_ovlbufladd_t   ch3_ovlbufladd; /* 006C */
	reg_ove_ovlbufhadd_t       ovlbufhadd;     /* 0070 */
	u32                        res1[3];
	reg_ove_ch0_ovllnstd_t     ch0_ovllnstd;   /* 0080 */
	reg_ove_ch1_ovllnstd_t     ch1_ovllnstd;   /* 0084 */
	reg_ove_ch2_ovllnstd_t     ch2_ovllnstd;   /* 0088 */
	reg_ove_ch3_ovllnstd_t     ch3_ovllnstd;   /* 008C */
	reg_ove_ch0_ovlpal0_t      ch0_ovlpal0;    /* 0090 */
	reg_ove_ch0_ovlpal1_t      ch0_ovlpal1;    /* 0094 */
	reg_ove_ch0_ovlpal2_t      ch0_ovlpal2;    /* 0098 */
	reg_ove_ch0_ovlpal3_t      ch0_ovlpal3;    /* 009C */
	reg_ove_ch1_ovlpal0_t      ch1_ovlpal0;    /* 00A0 */
	reg_ove_ch1_ovlpal1_t      ch1_ovlpal1;    /* 00A4 */
	reg_ove_ch1_ovlpal2_t      ch1_ovlpal2;    /* 00A8 */
	reg_ove_ch1_ovlpal3_t      ch1_ovlpal3;    /* 00AC */
	reg_ove_ch2_ovlpal0_t      ch2_ovlpal0;    /* 00B0 */
	reg_ove_ch2_ovlpal1_t      ch2_ovlpal1;    /* 00B4 */
	reg_ove_ch2_ovlpal2_t      ch2_ovlpal2;    /* 00B8 */
	reg_ove_ch2_ovlpal3_t      ch2_ovlpal3;    /* 00BC */
	reg_ove_ch3_ovlpal0_t      ch3_ovlpal0;    /* 00C0 */
	reg_ove_ch3_ovlpal1_t      ch3_ovlpal1;    /* 00C4 */
	reg_ove_ch3_ovlpal2_t      ch3_ovlpal2;    /* 00C8 */
	reg_ove_ch3_ovlpal3_t      ch3_ovlpal3;    /* 00CC */
	u32                        res2[8];
	reg_ove_dbg0_t             dbg0;           /* 00F0 */
	reg_ove_dbg1_t             dbg1;           /* 00F4 */
	u32                        res3[194];
	u32                        pal;       /* 0400 */
} reg_ove_t;

#endif /* ___OVE___H___ */
