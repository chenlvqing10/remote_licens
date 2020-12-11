#ifndef __AX_REG_H__
#define __AX_REG_H__

#ifdef CONFIG_ARCH_LOMBO_N7V1

/* TOP base registers */
#define AX_BASE_ADDR         0x00000000 /* 0x01700000 */
#define AX_TOP_VER           (AX_BASE_ADDR + 0x00)
#define AX_TOP_FTR           (AX_BASE_ADDR + 0x04)

/* CVU registers */
#define CVU_TOP_VER          (AX_BASE_ADDR + 0x200)
#define CVU_TOP_FTR          (AX_BASE_ADDR + 0x204)

#define CVU_ADPLL_CTRL       (AX_BASE_ADDR + 0xb4)
#define CVU_ADPLL_FAC        (AX_BASE_ADDR + 0xb8)
#define CVU_ADPLL_TUNE0      (AX_BASE_ADDR + 0xbc)
#define CVU_ADPLL_TUNE1      (AX_BASE_ADDR + 0xc0)
#define CVU_ADPLL_STAT       (AX_BASE_ADDR + 0xc4)

#define CVU_CTL              (AX_BASE_ADDR + 0x210)
#define CVU_RST              (AX_BASE_ADDR + 0x210)
#define CVU_STAT             (AX_BASE_ADDR + 0x218)
#define CVU_INT_EN           (AX_BASE_ADDR + 0x220)
#define CVU_INT_PD           (AX_BASE_ADDR + 0x224)
#define CVU_INT_CLR          (AX_BASE_ADDR + 0x228)

#define CVU_DIN_CFG0         (AX_BASE_ADDR + 0x230)
#define CVU_DIN_CFG1         (AX_BASE_ADDR + 0x234)
#define CVU_DIN_CFG2         (AX_BASE_ADDR + 0x238)
#define CVU_DIN_YADDR        (AX_BASE_ADDR + 0x23C)
#define CVU_DIN_UVADDR       (AX_BASE_ADDR + 0x240)
#define CVU_DOUT_CFG0        (AX_BASE_ADDR + 0x250)
#define CVU_DOUT_CFG1        (AX_BASE_ADDR + 0x254)
#define CVU_DOUT_PYM0_CFG    (AX_BASE_ADDR + 0x260)
#define CVU_DOUT_PYM1_CFG    (AX_BASE_ADDR + 0x264)
#define CVU_DOUT_PYM2_CFG    (AX_BASE_ADDR + 0x268)
#define CVU_DOUT_PYM3_CFG    (AX_BASE_ADDR + 0x26C)
#define CVU_DOUT_PYM4_CFG    (AX_BASE_ADDR + 0x270)
#define CVU_DOUT_PYM5_CFG    (AX_BASE_ADDR + 0x274)
#define CVU_DOUT_PYM6_CFG    (AX_BASE_ADDR + 0x278)
#define CVU_DOUT_PYM7_CFG    (AX_BASE_ADDR + 0x27C)
#define CVU_DOUT_ADDR        (AX_BASE_ADDR + 0x280)

#define CVU_RSZ_CFG0         (AX_BASE_ADDR + 0x290)
#define CVU_RSZ_CFG1         (AX_BASE_ADDR + 0x294)
#define CVU_RSZ_CFG2         (AX_BASE_ADDR + 0x298)
#define CVU_RSZ_CFG3         (AX_BASE_ADDR + 0x29C)

#define CVU_RSZ_PYM0_CFG0    (AX_BASE_ADDR + 0x2A0)
#define CVU_RSZ_PYM0_CFG1    (AX_BASE_ADDR + 0x2A4)
#define CVU_RSZ_PYM1_CFG0    (AX_BASE_ADDR + 0x2A8)
#define CVU_RSZ_PYM1_CFG1    (AX_BASE_ADDR + 0x2AC)
#define CVU_RSZ_PYM2_CFG0    (AX_BASE_ADDR + 0x2B0)
#define CVU_RSZ_PYM2_CFG1    (AX_BASE_ADDR + 0x2B4)
#define CVU_RSZ_PYM3_CFG0    (AX_BASE_ADDR + 0x2B8)
#define CVU_RSZ_PYM3_CFG1    (AX_BASE_ADDR + 0x2BC)
#define CVU_RSZ_PYM4_CFG0    (AX_BASE_ADDR + 0x2C0)
#define CVU_RSZ_PYM4_CFG1    (AX_BASE_ADDR + 0x2C4)
#define CVU_RSZ_PYM5_CFG0    (AX_BASE_ADDR + 0x2C8)
#define CVU_RSZ_PYM5_CFG1    (AX_BASE_ADDR + 0x2CC)
#define CVU_RSZ_PYM6_CFG0    (AX_BASE_ADDR + 0x2D0)
#define CVU_RSZ_PYM6_CFG1    (AX_BASE_ADDR + 0x2D4)
#define CVU_RSZ_PYM7_CFG0    (AX_BASE_ADDR + 0x2D8)
#define CVU_RSZ_PYM7_CFG1    (AX_BASE_ADDR + 0x2DC)

#define CVU_RSZ_ADDR         (AX_BASE_ADDR + 0x2E0)

#define CVU_PERF_CNT         (AX_BASE_ADDR + 0x2E4)
#define CVU_BW_STAT_EN       (AX_BASE_ADDR + 0x2E8)
#define CVU_BW_R_STAT        (AX_BASE_ADDR + 0x2EC)
#define CVU_BW_W_STAT        (AX_BASE_ADDR + 0x2F0)
#define CVU_RST_STAT         (AX_BASE_ADDR + 0x2FC)

/* ENU Registers */
#define ENU_RST              (AX_BASE_ADDR + 0x10)
#define ENU_ADPLL_CTRL       (AX_BASE_ADDR + 0xa0)
#define ENU_ADPLL_FAC        (AX_BASE_ADDR + 0xa4)
#define ENU_ADPLL_TUNE0      (AX_BASE_ADDR + 0xa8)
#define ENU_ADPLL_TUNE1      (AX_BASE_ADDR + 0xac)
#define ENU_ADPLL_STAT       (AX_BASE_ADDR + 0xb0)

#define ENU_VER              (AX_BASE_ADDR + 0x100)
#define ENU_FTR              (AX_BASE_ADDR + 0x104)
#define ENU_CTL              (AX_BASE_ADDR + 0x110)
#define ENU_STAT0            (AX_BASE_ADDR + 0x118)
#define ENU_STAT1            (AX_BASE_ADDR + 0x11C)
#define ENU_CMDQ_INT_EN      (AX_BASE_ADDR + 0x120)
#define ENU_CMDQ_INT_PD      (AX_BASE_ADDR + 0x124)
#define ENU_CMDQ_INT_CLR     (AX_BASE_ADDR + 0x128)
#define ENU_CMDQ_ADDR        (AX_BASE_ADDR + 0x130)

#define ENU_CMDQ_INT_EN0     (AX_BASE_ADDR + 0x134)
#define ENU_CMDQ_INT_EN1     (AX_BASE_ADDR + 0x138)
#define ENU_CMDQ_INT_PD0     (AX_BASE_ADDR + 0x13C)
#define ENU_CMDQ_INT_PD1     (AX_BASE_ADDR + 0x140)
#define ENU_CMDQ_INT_CLR0    (AX_BASE_ADDR + 0x144)
#define ENU_CMDQ_INT_CLR1    (AX_BASE_ADDR + 0x148)

#define ENU_CMDQ_BOX         (AX_BASE_ADDR + 0x14C)
#define ENU_CMDQ_PS_L32      (AX_BASE_ADDR + 0x150)
#define ENU_CMDQ_PS_H32      (AX_BASE_ADDR + 0x154)

#define ENU_PERF_CNT         (AX_BASE_ADDR + 0x1E4)
#define ENU_BW_STAT_EN       (AX_BASE_ADDR + 0x1E8)
#define ENU_BW_R_STAT        (AX_BASE_ADDR + 0x1EC)
#define ENU_BW_W_STAT        (AX_BASE_ADDR + 0x1F0)
#define ENU_RST_STAT         (AX_BASE_ADDR + 0x1FC)

#define PRCM_BASE 0xF400A000

#define AHB_GAT1             (PRCM_BASE + 0x204)
#define AHB_RST1             (PRCM_BASE + 0x224)
#define AXI_GAT              (PRCM_BASE + 0x340)

#define AX_PLL_ENB           (PRCM_BASE + 0x6A0)
#define AX_PLL_FAC           (PRCM_BASE + 0x6A8)
#define AX_PLL_TUN0          (PRCM_BASE + 0x6AC)

#define AX_PLL_STAT          (PRCM_BASE + 0x6B8)
#define AX_PLL_MODE          (PRCM_BASE + 0x6BC)

#define AX_PLL_NFRAC         (PRCM_BASE + 0x6C0)
#define AX_PLL_TUN2          (PRCM_BASE + 0x6CC)

#define AX_PLL_CTL           (PRCM_BASE + 0xBD0)

#endif

#endif
