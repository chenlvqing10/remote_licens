/* prcm.h */

#ifndef ___PRCM___H___
#define ___PRCM___H___

#define BASE_PRCM                0x0400A000

#define VA_PRCM_VER                          (0x00000000 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AHB_GAT0                     (0x00000200 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AHB_GAT1                     (0x00000204 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AHB_RST0                     (0x00000220 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AHB_RST1                     (0x00000224 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_APB_GAT0                     (0x00000280 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_APB_RST0                     (0x000002A0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_APB_GAT1                     (0x000002C0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_APB_RST1                     (0x000002E0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AXI_GAT                      (0x00000340 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_HFEOSC_CTRL                  (0x00000420 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_MBIAS_CTRL                   (0x00000424 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_PLL_EN                   (0x00000460 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_PLL_FAC                  (0x00000468 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_PLL_TUNE0                (0x0000046C + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_PLL_TEN                  (0x00000474 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_PLL_STAT                 (0x00000478 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_PLL_MOD                  (0x0000047C + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_PLL_NFRAC                (0x00000480 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_PLL_TUNE2                (0x0000048C + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_EN                 (0x000004A0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_FAC                (0x000004A4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_TUNE0              (0x000004A8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_TEN                (0x000004AC + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_STAT               (0x000004B0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_MOD                (0x000004B4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_NFRAC              (0x000004B8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_TUNE1              (0x000004BC + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_PLL_TUNE2              (0x000004C0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH0_PLL_EN                 (0x000004E0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH0_PLL_FAC                (0x000004E4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH0_PLL_TUNE0              (0x000004E8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH0_PLL_TEN                (0x000004EC + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH0_PLL_STAT               (0x000004F0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH0_PLL_MOD                (0x000004F4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH0_PLL_NFRAC              (0x000004F8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AUDIO_PLL_EN                 (0x00000520 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AUDIO_PLL_FAC                (0x00000524 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AUDIO_PLL_TUNE0              (0x00000528 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AUDIO_PLL_TEN                (0x0000052C + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AUDIO_PLL_STAT               (0x00000530 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AUDIO_PLL_MOD                (0x00000534 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AUDIO_PLL_NFRAC              (0x00000538 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VC_PLL_EN                    (0x000005A0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VC_PLL_FAC                   (0x000005A4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VC_PLL_TUNE0                 (0x000005A8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VC_PLL_TEN                   (0x000005AC + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VC_PLL_STAT                  (0x000005B0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VC_PLL_MOD                   (0x000005B4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VC_PLL_NFRAC                 (0x000005B8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH1_PLL_EN                 (0x000005E0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH1_PLL_FAC                (0x000005E4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH1_PLL_TUNE0              (0x000005E8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH1_PLL_TEN                (0x000005EC + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH1_PLL_STAT               (0x000005F0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH1_PLL_MOD                (0x000005F4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH1_PLL_NFRAC              (0x000005F8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PERH1_PLL_TUNE1              (0x000005FC + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VIDEO_PLL_EN                 (0x00000620 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VIDEO_PLL_FAC                (0x00000624 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VIDEO_PLL_TUNE0              (0x00000628 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VIDEO_PLL_TEN                (0x0000062C + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VIDEO_PLL_STAT               (0x00000630 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VIDEO_PLL_MOD                (0x00000634 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VIDEO_PLL_NFRAC              (0x00000638 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VIDEO_PLL_TUNE1              (0x0000063C + BASE_PRCM + VA_PRCM)
#define VA_PRCM_TP0_PLL_EN                   (0x00000760 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_TP0_PLL_FAC                  (0x00000764 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_TP0_PLL_FRAC                 (0x00000768 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_TP0_PLL_STAT                 (0x0000076C + BASE_PRCM + VA_PRCM)
#define VA_PRCM_PLL_TBLK                     (0x00000860 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_CPU_AXI_CLK_CTRL             (0x00000900 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AHB_APB_CLK_CTRL             (0x00000904 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_MAXI_CLK_CTRL                (0x00000920 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_CLK_CTRL               (0x00000940 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_RST                    (0x00000944 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDRAM_BM_CLK_CTRL            (0x00000948 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDC0_CLK_CTRL                (0x00000960 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SDC1_CLK_CTRL                (0x00000964 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SPI0_CLK_CTRL                (0x00000970 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SPI1_CLK_CTRL                (0x00000974 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_SPI2_CLK_CTRL                (0x00000978 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VC_CLK_CTRL                  (0x000009C0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_VISS_CLK_CTRL                (0x000009D0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_DPU_SCLK0_CLK_CTRL           (0x00000AD0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_DPU_SCLK1_CLK_CTRL           (0x00000AD4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_DPU_SCLK2_CLK_CTRL           (0x00000AD8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_DOSS_CLK_CTRL                (0x00000AE0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AX_ENU_CLK_CTRL              (0x00000BD0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AX_CVU_CLK_CTRL              (0x00000BD4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_I2S_CLK_CTRL                 (0x00000BE0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_I2C0_CLK_CTRL                (0x00000CA0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_I2C1_CLK_CTRL                (0x00000CA4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_I2C2_CLK_CTRL                (0x00000CA8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_I2C3_CLK_CTRL                (0x00000CAC + BASE_PRCM + VA_PRCM)
#define VA_PRCM_UART0_CLK_CTRL               (0x00000CC0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_UART1_CLK_CTRL               (0x00000CC4 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_UART2_CLK_CTRL               (0x00000CC8 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_UART3_CLK_CTRL               (0x00000CCC + BASE_PRCM + VA_PRCM)
#define VA_PRCM_USB_PHY_RST                  (0x00000CE0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_MBIST_CLK_CTRL               (0x00000CF0 + BASE_PRCM + VA_PRCM)
#define VA_PRCM_AES_CLK_CTRL                 (0x00000D00 + BASE_PRCM + VA_PRCM)

#define DATA_PRCM_VER                        0x00001100
#define DATA_PRCM_AHB_GAT0                   0x00000000
#define DATA_PRCM_AHB_GAT1                   0x00000000
#define DATA_PRCM_AHB_RST0                   0x00000000
#define DATA_PRCM_AHB_RST1                   0x00000000
#define DATA_PRCM_APB_GAT0                   0x00000000
#define DATA_PRCM_APB_RST0                   0x00000000
#define DATA_PRCM_APB_GAT1                   0x00000000
#define DATA_PRCM_APB_RST1                   0x00000000
#define DATA_PRCM_AXI_GAT                    0x00000000
#define DATA_PRCM_HFEOSC_CTRL                0x00000031
#define DATA_PRCM_MBIAS_CTRL                 0x00000101
#define DATA_PRCM_CPU_PLL_EN                 0x00000000
#define DATA_PRCM_CPU_PLL_FAC                0x00001400
#define DATA_PRCM_CPU_PLL_TUNE0              0x03078E18
#define DATA_PRCM_CPU_PLL_TEN                0x00000000
#define DATA_PRCM_CPU_PLL_STAT               0x00000000
#define DATA_PRCM_CPU_PLL_MOD                0x00000000
#define DATA_PRCM_CPU_PLL_NFRAC              0x00000000
#define DATA_PRCM_CPU_PLL_TUNE2              0x00000000
#define DATA_PRCM_SDRAM_PLL_EN               0x00000000
#define DATA_PRCM_SDRAM_PLL_FAC              0x00001100
#define DATA_PRCM_SDRAM_PLL_TUNE0            0x03061E18
#define DATA_PRCM_SDRAM_PLL_TEN              0x00000000
#define DATA_PRCM_SDRAM_PLL_STAT             0x00000000
#define DATA_PRCM_SDRAM_PLL_MOD              0x00000000
#define DATA_PRCM_SDRAM_PLL_NFRAC            0x00000000
#define DATA_PRCM_SDRAM_PLL_TUNE1            0x00000000
#define DATA_PRCM_SDRAM_PLL_TUNE2            0x00000000
#define DATA_PRCM_PERH0_PLL_EN               0x00000000
#define DATA_PRCM_PERH0_PLL_FAC              0x00006311
#define DATA_PRCM_PERH0_PLL_TUNE0            0x0020C3D8
#define DATA_PRCM_PERH0_PLL_TEN              0x00000000
#define DATA_PRCM_PERH0_PLL_STAT             0x00000000
#define DATA_PRCM_PERH0_PLL_MOD              0x00000000
#define DATA_PRCM_PERH0_PLL_NFRAC            0x00000000
#define DATA_PRCM_AUDIO_PLL_EN               0x00000000
#define DATA_PRCM_AUDIO_PLL_FAC              0x00002B21
#define DATA_PRCM_AUDIO_PLL_TUNE0            0x0060F1D8
#define DATA_PRCM_AUDIO_PLL_TEN              0x00000000
#define DATA_PRCM_AUDIO_PLL_STAT             0x00000000
#define DATA_PRCM_AUDIO_PLL_MOD              0x00000000
#define DATA_PRCM_AUDIO_PLL_NFRAC            0x00000000
#define DATA_PRCM_VC_PLL_EN                  0x00000000
#define DATA_PRCM_VC_PLL_FAC                 0x00003C11
#define DATA_PRCM_VC_PLL_TUNE0               0x0020F1D8
#define DATA_PRCM_VC_PLL_TEN                 0x00000000
#define DATA_PRCM_VC_PLL_STAT                0x00000000
#define DATA_PRCM_VC_PLL_MOD                 0x00000000
#define DATA_PRCM_VC_PLL_NFRAC               0x00000000
#define DATA_PRCM_PERH1_PLL_EN               0x00000000
#define DATA_PRCM_PERH1_PLL_FAC              0x00006311
#define DATA_PRCM_PERH1_PLL_TUNE0            0x0060F1D8
#define DATA_PRCM_PERH1_PLL_TEN              0x00000000
#define DATA_PRCM_PERH1_PLL_STAT             0x00000000
#define DATA_PRCM_PERH1_PLL_MOD              0x00000000
#define DATA_PRCM_PERH1_PLL_NFRAC            0x00000000
#define DATA_PRCM_PERH1_PLL_TUNE1            0x00000000
#define DATA_PRCM_VIDEO_PLL_EN               0x00000000
#define DATA_PRCM_VIDEO_PLL_FAC              0x00006331
#define DATA_PRCM_VIDEO_PLL_TUNE0            0x0060F1D8
#define DATA_PRCM_VIDEO_PLL_TEN              0x00000000
#define DATA_PRCM_VIDEO_PLL_STAT             0x00000000
#define DATA_PRCM_VIDEO_PLL_MOD              0x00000000
#define DATA_PRCM_VIDEO_PLL_NFRAC            0x00000000
#define DATA_PRCM_VIDEO_PLL_TUNE1            0x00000000
#define DATA_PRCM_TP0_PLL_EN                 0x00000003
#define DATA_PRCM_TP0_PLL_FAC                0x00111101
#define DATA_PRCM_TP0_PLL_FRAC               0x00000003
#define DATA_PRCM_TP0_PLL_STAT               0x00000000
#define DATA_PRCM_PLL_TBLK                   0x03040000
#define DATA_PRCM_CPU_AXI_CLK_CTRL           0x00010000
#define DATA_PRCM_AHB_APB_CLK_CTRL           0x00000000
#define DATA_PRCM_MAXI_CLK_CTRL              0x00000000
#define DATA_PRCM_SDRAM_CLK_CTRL             0x00000000
#define DATA_PRCM_SDRAM_RST                  0x00000000
#define DATA_PRCM_SDRAM_BM_CLK_CTRL          0x00000000
#define DATA_PRCM_SDC0_CLK_CTRL              0x00000000
#define DATA_PRCM_SDC1_CLK_CTRL              0x00000000
#define DATA_PRCM_SPI0_CLK_CTRL              0x00000001
#define DATA_PRCM_SPI1_CLK_CTRL              0x00000000
#define DATA_PRCM_SPI2_CLK_CTRL              0x00000000
#define DATA_PRCM_VC_CLK_CTRL                0x00000000
#define DATA_PRCM_VISS_CLK_CTRL              0x00000000
#define DATA_PRCM_DPU_SCLK0_CLK_CTRL         0x00000000
#define DATA_PRCM_DPU_SCLK1_CLK_CTRL         0x00000000
#define DATA_PRCM_DPU_SCLK2_CLK_CTRL         0x00000000
#define DATA_PRCM_DOSS_CLK_CTRL              0x00000000
#define DATA_PRCM_AX_ENU_CLK_CTRL            0x00000000
#define DATA_PRCM_AX_CVU_CLK_CTRL            0x00000000
#define DATA_PRCM_I2S_CLK_CTRL               0x00000000
#define DATA_PRCM_I2C0_CLK_CTRL              0x00000000
#define DATA_PRCM_I2C1_CLK_CTRL              0x00000000
#define DATA_PRCM_I2C2_CLK_CTRL              0x00000000
#define DATA_PRCM_I2C3_CLK_CTRL              0x00000000
#define DATA_PRCM_UART0_CLK_CTRL             0x00000000
#define DATA_PRCM_UART1_CLK_CTRL             0x00000000
#define DATA_PRCM_UART2_CLK_CTRL             0x00000000
#define DATA_PRCM_UART3_CLK_CTRL             0x00000000
#define DATA_PRCM_USB_PHY_RST                0x00000000
#define DATA_PRCM_MBIST_CLK_CTRL             0x80000001
#define DATA_PRCM_AES_CLK_CTRL               0x00000000

/* CLOCK Version Register */
typedef union {
	u32 val;
	struct {
	u32 ver_l:5;            /* The low 5bits of version register */
	u32 rsvd0:3;            /* - */
	u32 ver_h:3;            /* The high 3bits of version register */
	u32 rsvd1:1;            /* - */
	u32 comp:1;             /* Backward Compatibility
				 * 0: Incompatible to last version of hardware
				 * 1: Compatible to last version of hardware */
	u32 rsvd2:19;           /* - */
	} bits;
} reg_prcm_ver_t;

/* AHB BUS Clock Gating Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dma:1;              /* DMA bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 ptimer:1;           /* PTIMER bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd0:4;            /* - */
	u32 aes:1;              /* AES bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd1:9;            /* - */
	u32 sdram:1;            /* SDRAM Controller bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 sdc0:1;             /* SDC0 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 sdc1:1;             /* SDC1 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd2:2;            /* - */
	u32 spi0:1;             /* SPI0(SSPI) bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on.
				 * BOOT PIN SEL XIPÆô¶¯Ê±, ¸ÄbitÓ²¼þ×Ô¶¯ÖÃ1.
				 * ·ÇXIPÆô¶¯Ê±, Ä¬ÈÏ0. */
	u32 spi1:1;             /* SPI1(Communicate) bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 spi2:1;             /* SPI2(3-wire) bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd3:8;            /* - */
	} bits;
} reg_prcm_ahb_gat0_t;

#define PRCM_AHB_GAT0_DMA_0     0x0
#define PRCM_AHB_GAT0_DMA_1     0x1
#define PRCM_AHB_GAT0_PTIMER_0  0x0
#define PRCM_AHB_GAT0_PTIMER_1  0x1
#define PRCM_AHB_GAT0_AES_0     0x0
#define PRCM_AHB_GAT0_AES_1     0x1
#define PRCM_AHB_GAT0_SDRAM_0   0x0
#define PRCM_AHB_GAT0_SDRAM_1   0x1
#define PRCM_AHB_GAT0_SDC0_0    0x0
#define PRCM_AHB_GAT0_SDC0_1    0x1
#define PRCM_AHB_GAT0_SDC1_0    0x0
#define PRCM_AHB_GAT0_SDC1_1    0x1
#define PRCM_AHB_GAT0_SPI0_0    0x0
#define PRCM_AHB_GAT0_SPI0_1    0x1
#define PRCM_AHB_GAT0_SPI1_0    0x0
#define PRCM_AHB_GAT0_SPI1_1    0x1
#define PRCM_AHB_GAT0_SPI2_0    0x0
#define PRCM_AHB_GAT0_SPI2_1    0x1

/* AHB BUS Clock Gating Register 1 */
typedef union {
	u32 val;
	struct {
	u32 viss:1;             /* VISS bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd0:1;            /* - */
	u32 dpu:1;              /* DPU bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd1:1;            /* - */
	u32 doss:1;             /* DOSS bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd2:7;            /* - */
	u32 vc:1;               /* VC bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 ax:1;               /* AX bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd3:2;            /* - */
	u32 usb:1;              /* USB bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd4:15;           /* - */
	} bits;
} reg_prcm_ahb_gat1_t;

#define PRCM_AHB_GAT1_VISS_0    0x0
#define PRCM_AHB_GAT1_VISS_1    0x1
#define PRCM_AHB_GAT1_DPU_0     0x0
#define PRCM_AHB_GAT1_DPU_1     0x1
#define PRCM_AHB_GAT1_DOSS_0    0x0
#define PRCM_AHB_GAT1_DOSS_1    0x1
#define PRCM_AHB_GAT1_VC_0      0x0
#define PRCM_AHB_GAT1_VC_1      0x1
#define PRCM_AHB_GAT1_AX_0      0x0
#define PRCM_AHB_GAT1_AX_1      0x1
#define PRCM_AHB_GAT1_USB_0     0x0
#define PRCM_AHB_GAT1_USB_1     0x1

/* AHB BUS Reset Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dma:1;              /* DMA bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 ptimer:1;           /* PTIMER bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd0:4;            /* - */
	u32 aes:1;              /* AES bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd1:9;            /* - */
	u32 sdram:1;            /* SDRAM Controller bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 sdc0:1;             /* SDC0 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 sdc1:1;             /* SDC1 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd2:2;            /* - */
	u32 spi0:1;             /* SPI0(SSPI) bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset.
				 * BOOT PIN SEL XIPÆô¶¯Ê±, ¸ÄbitÓ²¼þ×Ô¶¯ÖÃ1.
				 * ·ÇXIPÆô¶¯Ê±, Ä¬ÈÏ0. */
	u32 spi1:1;             /* SPI1(Communicate) bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 spi2:1;             /* SPI2(3-wire) bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd3:8;            /* - */
	} bits;
} reg_prcm_ahb_rst0_t;

#define PRCM_AHB_RST0_DMA_0     0x0
#define PRCM_AHB_RST0_DMA_1     0x1
#define PRCM_AHB_RST0_PTIMER_0  0x0
#define PRCM_AHB_RST0_PTIMER_1  0x1
#define PRCM_AHB_RST0_AES_0     0x0
#define PRCM_AHB_RST0_AES_1     0x1
#define PRCM_AHB_RST0_SDRAM_0   0x0
#define PRCM_AHB_RST0_SDRAM_1   0x1
#define PRCM_AHB_RST0_SDC0_0    0x0
#define PRCM_AHB_RST0_SDC0_1    0x1
#define PRCM_AHB_RST0_SDC1_0    0x0
#define PRCM_AHB_RST0_SDC1_1    0x1
#define PRCM_AHB_RST0_SPI0_0    0x0
#define PRCM_AHB_RST0_SPI0_1    0x1
#define PRCM_AHB_RST0_SPI1_0    0x0
#define PRCM_AHB_RST0_SPI1_1    0x1
#define PRCM_AHB_RST0_SPI2_0    0x0
#define PRCM_AHB_RST0_SPI2_1    0x1

/* AHB BUS Reset Register 1 */
typedef union {
	u32 val;
	struct {
	u32 viss:1;             /* VISS bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd0:1;            /* - */
	u32 dpu:1;              /* DPU bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd1:1;            /* - */
	u32 doss:1;             /* DOSS bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd2:7;            /* - */
	u32 vc:1;               /* VC bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 ax:1;               /* AX bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd3:2;            /* - */
	u32 usb:1;              /* USB bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd4:15;           /* - */
	} bits;
} reg_prcm_ahb_rst1_t;

#define PRCM_AHB_RST1_VISS_0    0x0
#define PRCM_AHB_RST1_VISS_1    0x1
#define PRCM_AHB_RST1_DPU_0     0x0
#define PRCM_AHB_RST1_DPU_1     0x1
#define PRCM_AHB_RST1_DOSS_0    0x0
#define PRCM_AHB_RST1_DOSS_1    0x1
#define PRCM_AHB_RST1_VC_0      0x0
#define PRCM_AHB_RST1_VC_1      0x1
#define PRCM_AHB_RST1_AX_0      0x0
#define PRCM_AHB_RST1_AX_1      0x1
#define PRCM_AHB_RST1_USB_0     0x0
#define PRCM_AHB_RST1_USB_1     0x1

/* APB BUS Gating Register 0 */
typedef union {
	u32 val;
	struct {
	u32 i2c0:1;             /* I2C0 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 i2c1:1;             /* I2C1 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 i2c2:1;             /* I2C2 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 i2c3:1;             /* I2C3 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd0:4;            /* - */
	u32 uart0:1;            /* UART0 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 uart1:1;            /* UART1 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 uart2:1;            /* UART2 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 uart3:1;            /* UART3 bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd1:20;           /* - */
	} bits;
} reg_prcm_apb_gat0_t;

#define PRCM_APB_GAT0_I2C0_0    0x0
#define PRCM_APB_GAT0_I2C0_1    0x1
#define PRCM_APB_GAT0_I2C1_0    0x0
#define PRCM_APB_GAT0_I2C1_1    0x1
#define PRCM_APB_GAT0_I2C2_0    0x0
#define PRCM_APB_GAT0_I2C2_1    0x1
#define PRCM_APB_GAT0_I2C3_0    0x0
#define PRCM_APB_GAT0_I2C3_1    0x1
#define PRCM_APB_GAT0_UART0_0   0x0
#define PRCM_APB_GAT0_UART0_1   0x1
#define PRCM_APB_GAT0_UART1_0   0x0
#define PRCM_APB_GAT0_UART1_1   0x1
#define PRCM_APB_GAT0_UART2_0   0x0
#define PRCM_APB_GAT0_UART2_1   0x1
#define PRCM_APB_GAT0_UART3_0   0x0
#define PRCM_APB_GAT0_UART3_1   0x1

/* APB0 BUS Reset Register 0 */
typedef union {
	u32 val;
	struct {
	u32 i2c0:1;             /* I2C0 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 i2c1:1;             /* I2C1 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 i2c2:1;             /* I2C2 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 i2c3:1;             /* I2C3 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd0:4;            /* - */
	u32 uart0:1;            /* UART0 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 uart1:1;            /* UART1 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 uart2:1;            /* UART2 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 uart3:1;            /* UART3 bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd1:20;           /* - */
	} bits;
} reg_prcm_apb_rst0_t;

#define PRCM_APB_RST0_I2C0_0    0x0
#define PRCM_APB_RST0_I2C0_1    0x1
#define PRCM_APB_RST0_I2C1_0    0x0
#define PRCM_APB_RST0_I2C1_1    0x1
#define PRCM_APB_RST0_I2C2_0    0x0
#define PRCM_APB_RST0_I2C2_1    0x1
#define PRCM_APB_RST0_I2C3_0    0x0
#define PRCM_APB_RST0_I2C3_1    0x1
#define PRCM_APB_RST0_UART0_0   0x0
#define PRCM_APB_RST0_UART0_1   0x1
#define PRCM_APB_RST0_UART1_0   0x0
#define PRCM_APB_RST0_UART1_1   0x1
#define PRCM_APB_RST0_UART2_0   0x0
#define PRCM_APB_RST0_UART2_1   0x1
#define PRCM_APB_RST0_UART3_0   0x0
#define PRCM_APB_RST0_UART3_1   0x1

/* APB1 BUS Gating Register 1 */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:2;            /* - */
	u32 i2s:1;              /* I2S bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd1:3;            /* - */
	u32 gpadc:1;            /* GPADC bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd2:2;            /* - */
	u32 gpio:1;             /* GPIO bus clock gating.
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted. Clock on. */
	u32 rsvd3:22;           /* - */
	} bits;
} reg_prcm_apb_gat1_t;

#define PRCM_APB_GAT1_I2S_0     0x0
#define PRCM_APB_GAT1_I2S_1     0x1
#define PRCM_APB_GAT1_GPADC_0   0x0
#define PRCM_APB_GAT1_GPADC_1   0x1
#define PRCM_APB_GAT1_GPIO_0    0x0
#define PRCM_APB_GAT1_GPIO_1    0x1

/* APB1 BUS Reset Register 1 */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:2;            /* - */
	u32 i2s:1;              /* I2S bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd1:3;            /* - */
	u32 gpadc:1;            /* GPADC bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd2:2;            /* - */
	u32 gpio:1;             /* GPIO bus reset.
				 * [1¡¯b0]: de-asserted. Reset.
				 * [1¡¯b1]: asserted. Not reset. */
	u32 rsvd3:22;           /* - */
	} bits;
} reg_prcm_apb_rst1_t;

#define PRCM_APB_RST1_I2S_0     0x0
#define PRCM_APB_RST1_I2S_1     0x1
#define PRCM_APB_RST1_GPADC_0   0x0
#define PRCM_APB_RST1_GPADC_1   0x1
#define PRCM_APB_RST1_GPIO_0    0x0
#define PRCM_APB_RST1_GPIO_1    0x1

/* AXI BUS Clock Gating Register */
typedef union {
	u32 val;
	struct {
	u32 dma:1;              /* DMA master clock gating
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted */
	u32 vc:1;               /* VC master clock gating
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted */
	u32 rsvd0:2;            /* - */
	u32 viss0:1;            /* VISS0 master clock gating
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted */
	u32 viss1:1;            /* VISS1 master clock gating
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted */
	u32 rsvd1:2;            /* - */
	u32 dpu:1;              /* DPU master clock gating
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted */
	u32 rsvd2:1;            /* - */
	u32 doss:1;             /* DOSS master clock gating
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted */
	u32 rsvd3:1;            /* - */
	u32 ax:1;               /* AX master clock gating
				 * [1¡¯b0]: de-asserted. Clock off.
				 * [1¡¯b1]: asserted */
	u32 rsvd4:19;           /* - */
	} bits;
} reg_prcm_axi_gat_t;

#define PRCM_AXI_GAT_DMA_0      0x0
#define PRCM_AXI_GAT_DMA_1      0x1
#define PRCM_AXI_GAT_VC_0       0x0
#define PRCM_AXI_GAT_VC_1       0x1
#define PRCM_AXI_GAT_VISS0_0    0x0
#define PRCM_AXI_GAT_VISS0_1    0x1
#define PRCM_AXI_GAT_VISS1_0    0x0
#define PRCM_AXI_GAT_VISS1_1    0x1
#define PRCM_AXI_GAT_DPU_0      0x0
#define PRCM_AXI_GAT_DPU_1      0x1
#define PRCM_AXI_GAT_DOSS_0     0x0
#define PRCM_AXI_GAT_DOSS_1     0x1
#define PRCM_AXI_GAT_AX_0       0x0
#define PRCM_AXI_GAT_AX_1       0x1

/* HFEOSC XTAL Control Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;               /* xtal_en.
				 * Xtal enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 src_sel:2;          /* Xtal source select status.
				 * [2¡¯b00]: HFEOSC_SYS in system domain
				 * [2¡¯b01]: HFEOSC_PIN_INPUT from PIN
				 * [2¡¯b10]: HFEOSC_RTC in rtc domain
				 * Others are reserved. */
	u32 xtal_start:1;       /* xtal_start.
				 * ¸ßÆµ¾§ÕñÆô¶¯¼ÓËÙ¿ØÖÆ.
				 * ÖÃ1ºó¾­¹ý5msÖÃ0.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 tune:3;             /* xtal_tune[2:0].
				 * Xtal tune.
				 * [3¡¯b000]: ºÄµç250uA
				 * [3¡¯b001]: ºÄµç375uA
				 * [3¡¯b010]: ºÄµç500uA
				 * [3¡¯b011]: ºÄµç625uA
				 * [3¡¯b100]: ºÄµç750uA
				 * [3¡¯b101]: ºÄµç875uA
				 * [3¡¯b110]: ºÄµç1mA
				 * [3¡¯b111]: ºÄµç1.1mA */
	u32 rsvd0:25;           /* - */
	} bits;
} reg_prcm_hfeosc_ctrl_t;

#define PRCM_HFEOSC_CTRL_EN_0           0x0
#define PRCM_HFEOSC_CTRL_EN_1           0x1
#define PRCM_HFEOSC_CTRL_SRC_SEL_0      0x0
#define PRCM_HFEOSC_CTRL_SRC_SEL_1      0x1
#define PRCM_HFEOSC_CTRL_SRC_SEL_2      0x2
#define PRCM_HFEOSC_CTRL_XTAL_START_0   0x0
#define PRCM_HFEOSC_CTRL_XTAL_START_1   0x1
#define PRCM_HFEOSC_CTRL_TUNE_0         0x0
#define PRCM_HFEOSC_CTRL_TUNE_1         0x1
#define PRCM_HFEOSC_CTRL_TUNE_2         0x2
#define PRCM_HFEOSC_CTRL_TUNE_3         0x3
#define PRCM_HFEOSC_CTRL_TUNE_4         0x4
#define PRCM_HFEOSC_CTRL_TUNE_5         0x5
#define PRCM_HFEOSC_CTRL_TUNE_6         0x6
#define PRCM_HFEOSC_CTRL_TUNE_7         0x7

/* MBIAS Control Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;               /* mbias_en.
				 * Mbias enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * MbiasÎªËùÓÐPLLÌá¹©Æ«ÖÃµçÁ÷£¬µ±ÈÎÒâÒ»PLLÊ¹ÓÃ»ò
				 * HFEOSC´¦ÓÚ»î¶¯×´Ì¬£¬±ØÐëÊ¹ÄÜmbias */
	u32 rsvd0:3;            /* - */
	u32 tst_en0:1;          /* mbias_ten 0.
				 * MbiasÆ«ÖÃµçÁ÷²âÊÔÊ¹ÄÜ¡£
				 * ²âÊÔÊ±½«pad_anatestÉÏÀ­ÖÁ3.3V£¬²âµçÁ÷£¬
				 * µçÁ÷Ô¼5uA¡£MbiasÕý³£¹¤×÷Ê±±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 tst_en1:1;          /* mbias_ten 1.
				 * MbiasÆ«ÖÃµçÑ¹²âÊÔÊ¹ÄÜ¡£
				 * ²âÊÔÊ±²âpad_anatestµ½GNDµçÑ¹£¬Ô¼1.1V¡£M
				 * biasÕý³£¹¤×÷Ê±±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 tst_en2:1;          /* mbias_ten 2.
				 * PLLÆ«ÖÃµçÁ÷²âÊÔÊ¹ÄÜ¡£
				 * ²âÊÔÊ±½«pad_anatestÏÂÀ­µ½GND£¬²âµçÁ÷£¬µ
				 * çÁ÷Ô¼5uA¡£Õý³£¹¤×÷Ê±±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 tst_en3:1;          /* mbias_ten 3.
				 * MbiasÊä³öLDOµçÑ¹²âÊÔÊ¹ÄÜ¡£
				 * ²âÊÔÊ±²âpad_anatestµ½GNDµçÑ¹£¬Ô¼1.1V¡£Õ
				 * ý³£¹¤×÷Ê±±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 tune2_0:3;          /* mbias_tune[2:0].
				 * MbiasÖÐLDOÊä³öµçÑ¹µ÷½ÚÐÅºÅ¡£
				 * [3¡¯b000]: 1.05V
				 * [3¡¯b001]: 1.10V
				 * [3¡¯b010]: 1.15V
				 * [3¡¯b011]: 1.20V
				 * [3¡¯b100]: 1.25V
				 * [3¡¯b101]: 1.30V
				 * [3¡¯b110]: 1.35V
				 * [3¡¯b111]: 1.40V */
	u32 tune3:1;            /* mbias_tune[3].
				 * MbiasÖÐLDOÊäÈëµçÑ¹Ô´ÐÅºÅÑ¡Ôñ¡£
				 * [1¡¯b0]: µç×è·ÖÑ¹
				 * [1¡¯b1]: ´øÏ¶»ù×¼ */
	u32 tune4:1;            /* mbias_tune[4].
				 * MbiasÆ«ÖÃµçÁ÷Ô´Ñ¡Ôñ¡£
				 * [1¡¯b0]: ´øÏ¶»ù×¼²úÉúµÄPATHµçÁ÷
				 * [1¡¯b1]: µç×èÓëMOS¹Ü²úÉúµÄÆ«ÖÃµçÁ÷ */
	u32 rsvd1:19;           /* - */
	} bits;
} reg_prcm_mbias_ctrl_t;

#define PRCM_MBIAS_CTRL_EN_0            0x0
#define PRCM_MBIAS_CTRL_EN_1            0x1
#define PRCM_MBIAS_CTRL_TST_EN0_0       0x0
#define PRCM_MBIAS_CTRL_TST_EN0_1       0x1
#define PRCM_MBIAS_CTRL_TST_EN1_0       0x0
#define PRCM_MBIAS_CTRL_TST_EN1_1       0x1
#define PRCM_MBIAS_CTRL_TST_EN2_0       0x0
#define PRCM_MBIAS_CTRL_TST_EN2_1       0x1
#define PRCM_MBIAS_CTRL_TST_EN3_0       0x0
#define PRCM_MBIAS_CTRL_TST_EN3_1       0x1
#define PRCM_MBIAS_CTRL_TUNE2_0_0       0x0
#define PRCM_MBIAS_CTRL_TUNE2_0_1       0x1
#define PRCM_MBIAS_CTRL_TUNE2_0_2       0x2
#define PRCM_MBIAS_CTRL_TUNE2_0_3       0x3
#define PRCM_MBIAS_CTRL_TUNE2_0_4       0x4
#define PRCM_MBIAS_CTRL_TUNE2_0_5       0x5
#define PRCM_MBIAS_CTRL_TUNE2_0_6       0x6
#define PRCM_MBIAS_CTRL_TUNE2_0_7       0x7
#define PRCM_MBIAS_CTRL_TUNE3_0         0x0
#define PRCM_MBIAS_CTRL_TUNE3_1         0x1
#define PRCM_MBIAS_CTRL_TUNE4_0         0x0
#define PRCM_MBIAS_CTRL_TUNE4_1         0x1

/* CPU_PLL Enable Register */
typedef union {
	u32 val;
	struct {
	u32 enp:1;              /* cpu_enp&aud_enc.
				 * PLLÊ¹ÄÜÐÅºÅ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÕý³£¹¤×÷Ðè1ms¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 enm:1;              /* cpu_enm.
				 * PLLÆ«ÖÃµçÂ·Ê¹ÄÜ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÆ«ÖÃµçÂ·Õý³£¹¤×÷Ðè100us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 oen:1;              /* cpu_oen.
				 * PLL Êä³ögating¡£
				 * OEN = 0Ê±PLL¸ßÆµÊ±ÖÓ²»Êä³ö£¬¼´cpu_ckhsº
				 * Ícpu_cklsÊä³öÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ren:1;              /* cpu_ren.
				 * PLLÊä³öÄ£ÄâÊ±ÖÓÊ¹ÄÜ¡£
				 * RGATÖÃ0Ê±cpu_ckanaÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * For test, */
	u32 rsvd1:27;           /* - */
	} bits;
} reg_prcm_cpu_pll_en_t;

#define PRCM_CPU_PLL_EN_ENP_0   0x0
#define PRCM_CPU_PLL_EN_ENP_1   0x1
#define PRCM_CPU_PLL_EN_ENM_0   0x0
#define PRCM_CPU_PLL_EN_ENM_1   0x1
#define PRCM_CPU_PLL_EN_OEN_0   0x0
#define PRCM_CPU_PLL_EN_OEN_1   0x1
#define PRCM_CPU_PLL_EN_REN_0   0x0
#define PRCM_CPU_PLL_EN_REN_1   0x1

/* CPU_PLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 pre_div:2;          /* cpu_prediv[1:0].
				 * PLL pre-divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4
				 * Ò»°ãÇé¿öPRE_DIVÓ¦±£³ÖÖÃ2¡¯b00.ÉÙÊýÇé¿öP
				 * RE_DIVÊ¹ÓÃ2¡¯b01/2¡¯b10/2¡¯b11. */
	u32 rsvd0:2;            /* - */
	u32 post_div:2;         /* cpu_postdiv[1:0].
				 * PLL post divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4 */
	u32 rsvd1:2;            /* - */
	u32 n:8;                /* cpu_nint[7:0].
				 * PLL n factor.
				 * Valid value = N.
				 * Õý³£Ê¹ÓÃÊ±£¬valid value±ØÐë´óÓÚ10.ÇÒN=0
				 * ½ûÖ¹Ê¹ÓÃ¡£ */
	u32 rsvd2:16;           /* - */
	} bits;
} reg_prcm_cpu_pll_fac_t;

#define PRCM_CPU_PLL_FAC_PRE_DIV_0      0x0
#define PRCM_CPU_PLL_FAC_PRE_DIV_1      0x1
#define PRCM_CPU_PLL_FAC_PRE_DIV_2      0x2
#define PRCM_CPU_PLL_FAC_PRE_DIV_3      0x3
#define PRCM_CPU_PLL_FAC_POST_DIV_0     0x0
#define PRCM_CPU_PLL_FAC_POST_DIV_1     0x1
#define PRCM_CPU_PLL_FAC_POST_DIV_2     0x2
#define PRCM_CPU_PLL_FAC_POST_DIV_3     0x3

/* CPU_PLL Tune0 Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:3;            /* - */
	u32 tune1:3;            /* cpu_tune[2:0].
				 * PLL LDOµçÑ¹µ÷½Ú¡£
				 * µçÑ¹¹ýµÍÓÐ¿ÉÄÜÓ°ÏìÊä³öÊ±ÖÓµÄÕ¼¿Õ±È¡£
				 * [3¡¯b000]: 0.95V
				 * [3¡¯b001]: 1.00V
				 * [3¡¯b010]: 1.05V
				 * [3¡¯b011]: 1.10V
				 * [3¡¯b100]: 1.15V
				 * [3¡¯b101]: 1.20V
				 * [3¡¯b110]: 1.25V
				 * [3¡¯b111]: 1.30V */
	u32 rsvd1:3;            /* - */
	u32 tune2:5;            /* cpu_tune[7:3].
				 * PLLÄÚ²¿cpµçÁ÷µ÷½Ú¡£
				 * µçÁ÷Ô½´ó£¬PLL»·Â·´ø¿íÔ½´ó¡£
				 * I = (TUNE13_9 + 1) * 0.5uA. */
	u32 tune3:1;            /* cpu_tune[8].
				 * PLLÆ«ÖÃÄ£Ê½¡£
				 * [1¡¯b0]: ÊÖ¶¯Æ«ÖÃÄ£Ê½
				 * [1¡¯b1]: ×ÔÆ«ÖÃÄ£Ê½
				 * Ñ¡Ôñ×ÔÆ«ÖÃÄ£Ê½£¬»·Â·´ø¿í²»Ëæn factor±ä»
				 * ¯¶ø±ä»¯¡£ */
	u32 tune4:3;            /* cpu_tune[11:9].
				 * PLL VCOÔöÒæµ÷½Ú¡£
				 * [3¡¯b000]: 1.6GHz/V
				 * [3¡¯b001]: 1.8GHz/V
				 * [3¡¯b010]: 2.2GHz/V
				 * [3¡¯b011]: 2.6GHz/V
				 * [3¡¯b100]: 3.0GHz/V
				 * [3¡¯b101]: 3.4GHz/V
				 * [3¡¯b110]: 3.8GHz/V
				 * [3¡¯b111]: 4.2GHz/V
				 * ¸ù¾ÝPLL¹¤×÷ÆµÂÊµ÷½Ú¿ÉµÃµ½½ÏºÃÔëÉùÐÔÄÜ¡£ */
	u32 tune5:1;            /* cpu_tune[12].
				 * PLL n factorÒì³£¼à²âÊ¹ÄÜ¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜÊ±£¬Èôn³öÏÖÒì³£PLL×Ô¶¯´¦Àí£¬·ÀÖ¹PLL
				 * ½øÈëÒì³£¹¤×÷×´Ì¬¡£ */
	u32 tune6:1;            /* cpu_tune[13].
				 * ckbkÂö¿íÔö¼Óµ÷½ÚÐÅºÅ.
				 * [1¡¯b0]: ²»Ôö¼Ó
				 * [1¡¯b1]: Ôö¼Ó */
	u32 tune7:1;            /* cpu_tune[14].
				 * PLL¶¯Ì¬µ÷ÆµÊ±ÖÓÏàÎ»µ÷½Ú¡£
				 * [1¡¯b0]: ·´ÏàÊä³ö
				 * [1¡¯b1]: ²»·´ÏàÊä³ö
				 * ¾ö¶¨cksdmÊÇ·ñ·´ÏàÊä³öÖÁCMU. */
	u32 tune8:1;            /* cpu_tune[16].
				 * PLLÕûÊý·ÖÆµµ÷ÆµÄ£Ê½Ñ¡Ôñ¡£
				 * [1¡¯b0]: Ö±½ÓÌø±ä
				 * [1¡¯b1]: Öð½¥Ìø±ä */
	u32 tune9:3;            /* cpu_tune[19:17].
				 * PLLÕûÊý·ÖÆµÖð½¥Ìø±äµ÷ÆµÄ£Ê½µÄÌø±äÐ±ÂÊ¿ØÖÆ£¬·ñÔò¸Ãbit
				 * fieldÎÞÒâÒå¡£
				 * ¹«Ê½:
				 *  */
	u32 tune10:1;           /* cpu_tune[20].
				 * LSB ditherÆô¶¯ÐÅºÅ¡£¸ÃÑ¡ÏîÖ÷ÒªÓÃÓÚ¸ÄÉÆÐ
				 * ¡Êý·ÖÆµ¡¢Õ¹Æµ¼°ÏßÐÔµ÷ÆµµÄÐÔÄÜ£¬¶ÔÕûÊý·Ö
				 * ÆµÃ»ÓÐ×÷ÓÃ¡£
				 * [1¡¯b0]: ²»Æô¶¯
				 * [1¡¯b1]: Æô¶¯ */
	u32 tune11:2;           /* cpu_tune[22:21].
				 * PLL¶¯Ì¬µ÷Æµµ÷ÖÆ·½Ê½¡£
				 * [2¡¯b00]: Ò»½×¼ÓÍÌÂö³å
				 * [2¡¯b01]: Ò»½×¼Óµ÷½Ún
				 * [2¡¯b10]: MASH111µ÷ÖÆ
				 * [2¡¯b11]: ÕûÊý·ÖÆµ */
	u32 tune12:1;           /* cpu_tune[15].
				 * Êý×ÖÄ£¿éÊ±ÖÓÔ´Ñ¡Ôñ.
				 * [1¡¯b0]: cksdm
				 * [1¡¯b1]: dck */
	u32 rsvd2:3;            /* - */
	} bits;
} reg_prcm_cpu_pll_tune0_t;

#define PRCM_CPU_PLL_TUNE0_TUNE1_0      0x0
#define PRCM_CPU_PLL_TUNE0_TUNE1_1      0x1
#define PRCM_CPU_PLL_TUNE0_TUNE1_2      0x2
#define PRCM_CPU_PLL_TUNE0_TUNE1_3      0x3
#define PRCM_CPU_PLL_TUNE0_TUNE1_4      0x4
#define PRCM_CPU_PLL_TUNE0_TUNE1_5      0x5
#define PRCM_CPU_PLL_TUNE0_TUNE1_6      0x6
#define PRCM_CPU_PLL_TUNE0_TUNE1_7      0x7
#define PRCM_CPU_PLL_TUNE0_TUNE3_0      0x0
#define PRCM_CPU_PLL_TUNE0_TUNE3_1      0x1
#define PRCM_CPU_PLL_TUNE0_TUNE4_0      0x0
#define PRCM_CPU_PLL_TUNE0_TUNE4_1      0x1
#define PRCM_CPU_PLL_TUNE0_TUNE4_2      0x2
#define PRCM_CPU_PLL_TUNE0_TUNE4_3      0x3
#define PRCM_CPU_PLL_TUNE0_TUNE4_4      0x4
#define PRCM_CPU_PLL_TUNE0_TUNE4_5      0x5
#define PRCM_CPU_PLL_TUNE0_TUNE4_6      0x6
#define PRCM_CPU_PLL_TUNE0_TUNE4_7      0x7
#define PRCM_CPU_PLL_TUNE0_TUNE5_0      0x0
#define PRCM_CPU_PLL_TUNE0_TUNE5_1      0x1
#define PRCM_CPU_PLL_TUNE0_TUNE6_0      0x0
#define PRCM_CPU_PLL_TUNE0_TUNE6_1      0x1
#define PRCM_CPU_PLL_TUNE0_TUNE7_0      0x0
#define PRCM_CPU_PLL_TUNE0_TUNE7_1      0x1
#define PRCM_CPU_PLL_TUNE0_TUNE8_0      0x0
#define PRCM_CPU_PLL_TUNE0_TUNE8_1      0x1
#define PRCM_CPU_PLL_TUNE0_TUNE10_0     0x0
#define PRCM_CPU_PLL_TUNE0_TUNE10_1     0x1
#define PRCM_CPU_PLL_TUNE0_TUNE11_0     0x0
#define PRCM_CPU_PLL_TUNE0_TUNE11_1     0x1
#define PRCM_CPU_PLL_TUNE0_TUNE11_2     0x2
#define PRCM_CPU_PLL_TUNE0_TUNE11_3     0x3
#define PRCM_CPU_PLL_TUNE0_TUNE12_0     0x0
#define PRCM_CPU_PLL_TUNE0_TUNE12_1     0x1

/* CPU_PLL Test Enable Register */
typedef union {
	u32 val;
	struct {
	u32 ten0:1;             /* cpu_ten[0].
				 * PLLÊý×ÖLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÊý×ÖLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:2;            /* - */
	u32 ten1:1;             /* cpu_ten[1].
				 * PLLÊä³öÊ±ÖÓ²âÊÔÊ¹ÄÜ¡£
				 * ÖÃ1Ê±PLLÊä³öÊ±ÖÓÊä³öÖÁtblk¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd1:28;           /* - */
	} bits;
} reg_prcm_cpu_pll_ten_t;

#define PRCM_CPU_PLL_TEN_TEN0_0 0x0
#define PRCM_CPU_PLL_TEN_TEN0_1 0x1
#define PRCM_CPU_PLL_TEN_TEN1_0 0x0
#define PRCM_CPU_PLL_TEN_TEN1_1 0x1

/* CPU_PLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 dlock:1;            /* PLL digital lock status.
				 * When PLL reaches stable, lock bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 asteady:1;          /* cpu_steady.
				 * PLL analog steady status.
				 * [1¡¯b0]: not steady
				 * [1¡¯b1]: steady
				 * Factor N±ä»¯ÎÈ¶¨ÐÅºÅ, ¿ÉÓÃÓÚÅÐ¶ÏÏßÐÔµ÷Æ
				 * µµ÷ÆµºÍÕûÊý·ÖÆµÖð½¥µ÷Æµ½áÊø */
	u32 alock:1;            /* cpu_lock.
				 * PLL analog lock status.
				 * When PLL reaches stable, analog lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 rsvd0:29;           /* - */
	} bits;
} reg_prcm_cpu_pll_stat_t;

#define PRCM_CPU_PLL_STAT_DLOCK_0       0x0
#define PRCM_CPU_PLL_STAT_DLOCK_1       0x1
#define PRCM_CPU_PLL_STAT_ASTEADY_0     0x0
#define PRCM_CPU_PLL_STAT_ASTEADY_1     0x1
#define PRCM_CPU_PLL_STAT_ALOCK_0       0x0
#define PRCM_CPU_PLL_STAT_ALOCK_1       0x1

/* CPU_PLL Mode Register */
typedef union {
	u32 val;
	struct {
	u32 mod:2;              /* cpu_msel[1:0].
				 * PLL¹¤×÷Ä£Ê½Ñ¡Ôñ¡£
				 * [2¡¯b00]: ÕûÊý·ÖÆµ
				 * [2¡¯b01]: -
				 * [2¡¯b10]: -
				 * [2¡¯b11]: ÏßÐÔµ÷Æµ
				 * PLLÖ»ÄÜ¹¤×÷ÔÚÆäÖÐÒ»ÖÖÄ£Ê½£¬¶ÔÏßÐÔµ÷Æµ£¬
				 * ¿ÉÔÚTUNE¿ØÖÆ¿ªÆôÏßÐÔµ÷Æµµþ¼ÓÕ¹Æµ¡£ */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_prcm_cpu_pll_mod_t;

#define PRCM_CPU_PLL_MOD_MOD_0  0x0
#define PRCM_CPU_PLL_MOD_MOD_1  0x1
#define PRCM_CPU_PLL_MOD_MOD_2  0x2
#define PRCM_CPU_PLL_MOD_MOD_3  0x3

/* CPU_PLL N Factor Fraction Register */
typedef union {
	u32 val;
	struct {
	u32 nfrac:18;           /* cpu_nfrac[17:0].
				 * PLL nÒò×Ó·ÖÊý²¿·Ö¡£Ö»ÓÐÔÚÐ¡Êý·ÖÆµ¡¢Õ¹Æµ
				 * ÒÔ¼°ÏßÐÔµ÷ÆµÄ£Ê½ÏÂÓÐÐ§¡£
				 * ¹«Ê½£º
				 *  */
	u32 rsvd0:14;           /* - */
	} bits;
} reg_prcm_cpu_pll_nfrac_t;

/* CPU_PLL Tune2 Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;               /* cpu_tune[47].
				 * ÏßÐÔµ÷Æµ¹ý³ÌÖÐÊÇ·ñ¿ªÆôÕ¹Æµ¡£
				 * ½öÏßÐÔµ÷ÆµÄ£Ê½ÓÐÐ§¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:15;           /* - */
	u32 step:14;            /* cpu_tune[61:48].
				 * PLLÏßÐÔµ÷Æµ²½³¤µ÷½Ú£¬µ÷½Ú¸ÃÑ¡Ïî¿Éµ÷½ÚÏß
				 * ÐÔµ÷ÆµµÄÐ±ÂÊ¡£
				 * ¹«Ê½:
				 * 
				 * ½öÓÃÓÚÏßÐÔµ÷Æµ. */
	u32 rsvd1:2;            /* - */
	} bits;
} reg_prcm_cpu_pll_tune2_t;

#define PRCM_CPU_PLL_TUNE2_EN_0         0x0
#define PRCM_CPU_PLL_TUNE2_EN_1         0x1

/* SDRAM_PLL Enable Register */
typedef union {
	u32 val;
	struct {
	u32 enp:1;              /* sdram_enp&enc.
				 * PLLÊ¹ÄÜÐÅºÅ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÕý³£¹¤×÷Ðè10us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 enm:1;              /* sdram_enm.
				 * PLLÆ«ÖÃµçÂ·Ê¹ÄÜ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÆ«ÖÃµçÂ·Õý³£¹¤×÷Ðè100us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 oen:1;              /* sdram_oen.
				 * PLL Êä³ögating¡£
				 * OEN = 0Ê±PLL¸ßÆµÊ±ÖÓ²»Êä³ö£¬¼´ddr_ckhsº
				 * Íddr_cklsÊä³öÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ren:1;              /* sdram_ren.
				 * PLLÊä³öÄ£ÄâÊ±ÖÓÊ¹ÄÜ¡£
				 * RGATÖÃ0Ê±ckanaÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * For test. */
	u32 rsvd1:27;           /* - */
	} bits;
} reg_prcm_sdram_pll_en_t;

#define PRCM_SDRAM_PLL_EN_ENP_0         0x0
#define PRCM_SDRAM_PLL_EN_ENP_1         0x1
#define PRCM_SDRAM_PLL_EN_ENM_0         0x0
#define PRCM_SDRAM_PLL_EN_ENM_1         0x1
#define PRCM_SDRAM_PLL_EN_OEN_0         0x0
#define PRCM_SDRAM_PLL_EN_OEN_1         0x1
#define PRCM_SDRAM_PLL_EN_REN_0         0x0
#define PRCM_SDRAM_PLL_EN_REN_1         0x1

/* SDRAM_PLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 pre_div:2;          /* sdram_prediv[1:0].
				 * PLL pre-divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4
				 * Ò»°ãÇé¿öPRE_DIVÓ¦±£³ÖÖÃ2¡¯b00.ÉÙÊýÇé¿öP
				 * RE_DIVÊ¹ÓÃ2¡¯b01/2¡¯b10/2¡¯b11. */
	u32 rsvd0:2;            /* - */
	u32 post_div:2;         /* sdram_postdiv[1:0].
				 * PLL post divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4 */
	u32 rsvd1:2;            /* - */
	u32 n:8;                /* sdram_nint[7:0].
				 * PLL n factor.
				 * Valid value = N.
				 * Õý³£Ê¹ÓÃÊ±£¬valid value±ØÐë´óÓÚ10.ÇÒN=0
				 * ½ûÖ¹Ê¹ÓÃ¡£ */
	u32 rsvd2:16;           /* - */
	} bits;
} reg_prcm_sdram_pll_fac_t;

#define PRCM_SDRAM_PLL_FAC_PRE_DIV_0    0x0
#define PRCM_SDRAM_PLL_FAC_PRE_DIV_1    0x1
#define PRCM_SDRAM_PLL_FAC_PRE_DIV_2    0x2
#define PRCM_SDRAM_PLL_FAC_PRE_DIV_3    0x3
#define PRCM_SDRAM_PLL_FAC_POST_DIV_0   0x0
#define PRCM_SDRAM_PLL_FAC_POST_DIV_1   0x1
#define PRCM_SDRAM_PLL_FAC_POST_DIV_2   0x2
#define PRCM_SDRAM_PLL_FAC_POST_DIV_3   0x3

/* SDRAM_PLL Tune0 Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:3;            /* - */
	u32 tune1:3;            /* sdram_tune[2:0].
				 * PLLÊý×ÖºÍÄ£ÄâLDOµçÑ¹µ÷½Ú¡£
				 * µçÑ¹¹ýµÍÓÐ¿ÉÄÜÓ°ÏìÊä³öÊ±ÖÓµÄÕ¼¿Õ±È¡£
				 * [3¡¯b000]: 0.95V
				 * [3¡¯b001]: 1.00V
				 * [3¡¯b010]: 1.05V
				 * [3¡¯b011]: 1.10V
				 * [3¡¯b100]: 1.15V
				 * [3¡¯b101]: 1.20V
				 * [3¡¯b110]: 1.25V
				 * [3¡¯b111]: 1.30V */
	u32 rsvd1:3;            /* - */
	u32 tune3:5;            /* sdram_tune[7:3].
				 * PLLÄÚ²¿cpµçÁ÷µ÷½Ú¡£
				 * µçÁ÷Ô½´ó£¬PLL»·Â·´ø¿íÔ½´ó¡£
				 * I = (TUNE13_9 + 1) * 0.5uA. */
	u32 tune4:1;            /* sdram_tune[8].
				 * PLLÆ«ÖÃÄ£Ê½¡£
				 * [1¡¯b0]: ÊÖ¶¯Æ«ÖÃÄ£Ê½
				 * [1¡¯b1]: ×ÔÆ«ÖÃÄ£Ê½
				 * Ñ¡Ôñ×ÔÆ«ÖÃÄ£Ê½£¬»·Â·´ø¿í²»Ëæn factor±ä»
				 * ¯¶ø±ä»¯¡£ */
	u32 tune5:3;            /* sdram_tune[11:9].
				 * PLL VCOÔöÒæµ÷½Ú¡£
				 * [3¡¯b000]: 1.6GHz/V
				 * [3¡¯b001]: 1.8GHz/V
				 * [3¡¯b010]: 2.2GHz/V
				 * [3¡¯b011]: 2.6GHz/V
				 * [3¡¯b100]: 3.0GHz/V
				 * [3¡¯b101]: 3.4GHz/V
				 * [3¡¯b110]: 3.8GHz/V
				 * [3¡¯b111]: 4.2GHz/V
				 * ¸ù¾ÝPLL¹¤×÷ÆµÂÊµ÷½Ú¿ÉµÃµ½½ÏºÃÔëÉùÐÔÄÜ¡£ */
	u32 tune6:1;            /* sdram_tune[12].
				 * PLL n factorÒì³£¼à²âÊ¹ÄÜ¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜÊ±£¬Èôn³öÏÖÒì³£PLL×Ô¶¯´¦Àí£¬·ÀÖ¹PLL
				 * ½øÈëÒì³£¹¤×÷×´Ì¬¡£ */
	u32 tune7:1;            /* sdram_tune[13].
				 * ckbkÂö¿íÔö¼Óµ÷½ÚÐÅºÅ.
				 * [1¡¯b0]: ²»Ôö¼Ó
				 * [1¡¯b1]: Ôö¼Ó */
	u32 tune8:1;            /* sdram_tune[14].
				 * PLL¶¯Ì¬µ÷ÆµÊ±ÖÓÏàÎ»µ÷½Ú¡£
				 * [1¡¯b0]: ·´ÏàÊä³ö
				 * [1¡¯b1]: ²»·´ÏàÊä³ö
				 * ¾ö¶¨ddr_cksdmÊÇ·ñ·´ÏàÊä³öÖÁCMU. */
	u32 tune9:1;            /* sdram_tune[16].
				 * PLLÕûÊý·ÖÆµµ÷ÆµÄ£Ê½Ñ¡Ôñ¡£
				 * [1¡¯b0]: Ö±½ÓÌø±ä
				 * [1¡¯b1]: Öð½¥Ìø±ä */
	u32 tune10:3;           /* sdram_tune[19:17].
				 * PLLÕûÊý·ÖÆµÖð½¥Ìø±äµ÷ÆµÄ£Ê½µÄÌø±äÐ±ÂÊ¿ØÖÆ¡£
				 * ¹«Ê½:
				 *  */
	u32 tune11:1;           /* sdram_tune[20].
				 * LSB ditherÆô¶¯ÐÅºÅ¡£¸ÃÑ¡ÏîÖ÷ÒªÓÃÓÚ¸ÄÉÆÐ
				 * ¡Êý·ÖÆµ¡¢Õ¹Æµ¼°ÏßÐÔµ÷ÆµµÄÐÔÄÜ£¬¶ÔÕûÊý·Ö
				 * ÆµÃ»ÓÐ×÷ÓÃ¡£
				 * [1¡¯b0]: ²»Æô¶¯
				 * [1¡¯b1]: Æô¶¯ */
	u32 tune12:2;           /* sdram_tune[22:21].
				 * PLL¶¯Ì¬µ÷Æµµ÷ÖÆ·½Ê½¡£
				 * [2¡¯b00]: Ò»½×¼ÓÍÌÂö³å
				 * [2¡¯b01]: Ò»½×¼Óµ÷½Ún
				 * [2¡¯b10]: MASH111µ÷ÖÆ
				 * [2¡¯b11]: ÕûÊý·ÖÆµ */
	u32 tune13:1;           /* sdram_tune[15].
				 * Êý×ÖÄ£¿éÊ±ÖÓÔ´Ñ¡Ôñ.
				 * [1¡¯b0]: cksdm
				 * [1¡¯b1]: dck */
	u32 rsvd2:3;            /* - */
	} bits;
} reg_prcm_sdram_pll_tune0_t;

#define PRCM_SDRAM_PLL_TUNE0_TUNE1_0    0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE1_1    0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE1_2    0x2
#define PRCM_SDRAM_PLL_TUNE0_TUNE1_3    0x3
#define PRCM_SDRAM_PLL_TUNE0_TUNE1_4    0x4
#define PRCM_SDRAM_PLL_TUNE0_TUNE1_5    0x5
#define PRCM_SDRAM_PLL_TUNE0_TUNE1_6    0x6
#define PRCM_SDRAM_PLL_TUNE0_TUNE1_7    0x7
#define PRCM_SDRAM_PLL_TUNE0_TUNE4_0    0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE4_1    0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE5_0    0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE5_1    0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE5_2    0x2
#define PRCM_SDRAM_PLL_TUNE0_TUNE5_3    0x3
#define PRCM_SDRAM_PLL_TUNE0_TUNE5_4    0x4
#define PRCM_SDRAM_PLL_TUNE0_TUNE5_5    0x5
#define PRCM_SDRAM_PLL_TUNE0_TUNE5_6    0x6
#define PRCM_SDRAM_PLL_TUNE0_TUNE5_7    0x7
#define PRCM_SDRAM_PLL_TUNE0_TUNE6_0    0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE6_1    0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE7_0    0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE7_1    0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE8_0    0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE8_1    0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE9_0    0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE9_1    0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE11_0   0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE11_1   0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE12_0   0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE12_1   0x1
#define PRCM_SDRAM_PLL_TUNE0_TUNE12_2   0x2
#define PRCM_SDRAM_PLL_TUNE0_TUNE12_3   0x3
#define PRCM_SDRAM_PLL_TUNE0_TUNE13_0   0x0
#define PRCM_SDRAM_PLL_TUNE0_TUNE13_1   0x1

/* SDRAM_PLL Test Enable Register */
typedef union {
	u32 val;
	struct {
	u32 ten0:1;             /* sdram_ten[0].
				 * PLLÊý×ÖLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÊý×ÖLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 ten1:1;             /* sdram_ten[1].
				 * PLLÄ£ÄâLDO0Ä£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÄ£ÄâLDO0µçÑ¹£¬
				 * Ô¼2.2V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 ten2:1;             /* sdram_ten[2].
				 * PLLÄ£ÄâLDO1Ä£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÄ£ÄâLDO1µçÑ¹£¬
				 * Ô¼2.2V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 ten3:1;             /* sdram_ten[3].
				 * PLLÊä³öÊ±ÖÓ²âÊÔÊ¹ÄÜ¡£
				 * ÖÃ1Ê±PLLÊä³öÊ±ÖÓÊä³öÖÁtblk¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:28;           /* - */
	} bits;
} reg_prcm_sdram_pll_ten_t;

#define PRCM_SDRAM_PLL_TEN_TEN0_0       0x0
#define PRCM_SDRAM_PLL_TEN_TEN0_1       0x1
#define PRCM_SDRAM_PLL_TEN_TEN1_0       0x0
#define PRCM_SDRAM_PLL_TEN_TEN1_1       0x1
#define PRCM_SDRAM_PLL_TEN_TEN2_0       0x0
#define PRCM_SDRAM_PLL_TEN_TEN2_1       0x1
#define PRCM_SDRAM_PLL_TEN_TEN3_0       0x0
#define PRCM_SDRAM_PLL_TEN_TEN3_1       0x1

/* SRAM_PLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 dlock:1;            /* PLL digital lock status.
				 * When PLL reaches stable, digital lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 asteady:1;          /* sdram_steady.
				 * PLL analog steady status.
				 * [1¡¯b0]: not steady
				 * [1¡¯b1]: steady
				 * Factor N±ä»¯ÎÈ¶¨ÐÅºÅ, ¿ÉÓÃÓÚÅÐ¶ÏÏßÐÔµ÷Æ
				 * µµ÷ÆµºÍÕûÊý·ÖÆµÖð½¥µ÷Æµ½áÊø */
	u32 alock:1;            /* sdram_lock.
				 * PLL analog lock status.
				 * When PLL reaches stable, analog lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 rsvd0:29;           /* - */
	} bits;
} reg_prcm_sdram_pll_stat_t;

#define PRCM_SDRAM_PLL_STAT_DLOCK_0     0x0
#define PRCM_SDRAM_PLL_STAT_DLOCK_1     0x1
#define PRCM_SDRAM_PLL_STAT_ASTEADY_0   0x0
#define PRCM_SDRAM_PLL_STAT_ASTEADY_1   0x1
#define PRCM_SDRAM_PLL_STAT_ALOCK_0     0x0
#define PRCM_SDRAM_PLL_STAT_ALOCK_1     0x1

/* SDRAM_PLL Mode Register */
typedef union {
	u32 val;
	struct {
	u32 mod:2;              /* sdram_msel[1:0].
				 * PLL¹¤×÷Ä£Ê½Ñ¡Ôñ¡£
				 * [2¡¯b00]: ÕûÊý·ÖÆµ
				 * [2¡¯b01]: Ð¡Êý·ÖÆµ
				 * [2¡¯b10]: Õ¹Æµ
				 * [2¡¯b11]: ÏßÐÔµ÷Æµ
				 * PLLÖ»ÄÜ¹¤×÷ÔÚÆäÖÐÒ»ÖÖÄ£Ê½£¬¶ÔÏßÐÔµ÷Æµ£¬
				 * ¿ÉÔÚTUNE¿ØÖÆ¿ªÆôÏßÐÔµ÷Æµµþ¼ÓÕ¹Æµ¡£ */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_prcm_sdram_pll_mod_t;

#define PRCM_SDRAM_PLL_MOD_MOD_0        0x0
#define PRCM_SDRAM_PLL_MOD_MOD_1        0x1
#define PRCM_SDRAM_PLL_MOD_MOD_2        0x2
#define PRCM_SDRAM_PLL_MOD_MOD_3        0x3

/* SDRAM_PLL N Factor Fraction Register */
typedef union {
	u32 val;
	struct {
	u32 nfrac:18;           /* sdram_nfrac[17:0].
				 * PLL nÒò×Ó·ÖÊý²¿·Ö¡£Ö»ÓÐÔÚÕ¹ÆµÒÔ¼°ÏßÐÔµ÷
				 * ÆµÄ£Ê½ÏÂÓÐÐ§¡£
				 * ¹«Ê½£º
				 *  */
	u32 rsvd0:14;           /* - */
	} bits;
} reg_prcm_sdram_pll_nfrac_t;

/* SDRAM_PLL Tune1 Register */
typedef union {
	u32 val;
	struct {
	u32 period:12;          /* sdram_tune[34:23].
				 * PLLÕ¹ÆµÖÜÆÚµ÷½Ú¡£
				 * ¹«Ê½:
				 * PLL²Î¿¼ÆµÂÊ / Õ¹ÆµÆµÂÊ / 2.
				 * ÀýÈç:
				 * Ðè30KHzµÄÕ¹ÆµÆµÂÊ£¬ÊäÈëµ½PLLµÄ²Î¿¼ÆµÂÊÎ
				 * ª24MHz£¬ÔòÖÜÆÚÎª:
				 * 24MHz / 30KHz / 2 = 400.
				 * ½öÓÃÓÚÕ¹ÆµºÍÏßÐÔµ÷Æµ. */
	u32 rsvd0:4;            /* - */
	u32 amplitude:12;       /* sdram_tune[46:35].
				 * PLLÕ¹Æµ·ù¶È²½³¤µ÷½Ú.
				 * ¹«Ê½:
				 * 
				 * ½öÓÃÓÚÕ¹ÆµºÍÏßÐÔµ÷Æµ. */
	u32 rsvd1:4;            /* - */
	} bits;
} reg_prcm_sdram_pll_tune1_t;

/* SDRAM_PLL Tune2 Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;               /* sdram_tune[47].
				 * ÏßÐÔµ÷Æµ¹ý³ÌÖÐÊÇ·ñ¿ªÆôÕ¹Æµ¡£
				 * ½öÏßÐÔµ÷ÆµÄ£Ê½ÓÐÐ§¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:15;           /* - */
	u32 step:14;            /* sdram_tune[61:48].
				 * PLLÏßÐÔµ÷Æµ²½³¤µ÷½Ú£¬µ÷½Ú¸ÃÑ¡Ïî¿Éµ÷½ÚÏß
				 * ÐÔµ÷ÆµµÄÐ±ÂÊ¡£
				 * ¹«Ê½:
				 * 
				 * ½öÓÃÓÚÏßÐÔµ÷Æµ. */
	u32 rsvd1:2;            /* - */
	} bits;
} reg_prcm_sdram_pll_tune2_t;

#define PRCM_SDRAM_PLL_TUNE2_EN_0       0x0
#define PRCM_SDRAM_PLL_TUNE2_EN_1       0x1

/* PERH0_PLL Enable Register */
typedef union {
	u32 val;
	struct {
	u32 enp:1;              /* peri_enp&enc.
				 * PLLÊ¹ÄÜÐÅºÅ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÕý³£¹¤×÷Ðè10us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 enm:1;              /* peri_enm.
				 * PLLÆ«ÖÃµçÂ·Ê¹ÄÜ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÆ«ÖÃµçÂ·Õý³£¹¤×÷Ðè100us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 oen:1;              /* peri_oen.
				 * PLL Êä³ögating¡£
				 * OEN = 0Ê±PLL¸ßÆµÊ±ÖÓ²»Êä³ö£¬¼´peri_ckhs
				 * ºÍperi_cklsÊä³öÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ren:1;              /* peri_ren.
				 * PLLÊä³öÄ£ÄâÊ±ÖÓÊ¹ÄÜ¡£
				 * RGATÖÃ0Ê±peri_ckanaÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * For test. */
	u32 rsvd1:27;           /* - */
	} bits;
} reg_prcm_perh0_pll_en_t;

#define PRCM_PERH0_PLL_EN_ENP_0         0x0
#define PRCM_PERH0_PLL_EN_ENP_1         0x1
#define PRCM_PERH0_PLL_EN_ENM_0         0x0
#define PRCM_PERH0_PLL_EN_ENM_1         0x1
#define PRCM_PERH0_PLL_EN_OEN_0         0x0
#define PRCM_PERH0_PLL_EN_OEN_1         0x1
#define PRCM_PERH0_PLL_EN_REN_0         0x0
#define PRCM_PERH0_PLL_EN_REN_1         0x1

/* PERH0_PLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 pre_div:2;          /* peri_prediv[1:0].
				 * PLL pre-divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4
				 * Ò»°ãÇé¿öPRE_DIVÓ¦±£³ÖÖÃ2¡¯b00.ÉÙÊýÇé¿öP
				 * RE_DIVÊ¹ÓÃ2¡¯b01/2¡¯b10/2¡¯b11. */
	u32 rsvd0:2;            /* - */
	u32 post_div:2;         /* peri_postdiv[1:0].
				 * PLL post divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4 */
	u32 rsvd1:2;            /* - */
	u32 n:8;                /* peri_nint[7:0].
				 * PLL n factor.
				 * Valid value = N.
				 * Õý³£Ê¹ÓÃÊ±£¬valid value±ØÐë´óÓÚ10. ÇÒN=
				 * 0½ûÖ¹Ê¹ÓÃ¡£ */
	u32 rsvd2:16;           /* - */
	} bits;
} reg_prcm_perh0_pll_fac_t;

#define PRCM_PERH0_PLL_FAC_PRE_DIV_0    0x0
#define PRCM_PERH0_PLL_FAC_PRE_DIV_1    0x1
#define PRCM_PERH0_PLL_FAC_PRE_DIV_2    0x2
#define PRCM_PERH0_PLL_FAC_PRE_DIV_3    0x3
#define PRCM_PERH0_PLL_FAC_POST_DIV_0   0x0
#define PRCM_PERH0_PLL_FAC_POST_DIV_1   0x1
#define PRCM_PERH0_PLL_FAC_POST_DIV_2   0x2
#define PRCM_PERH0_PLL_FAC_POST_DIV_3   0x3

/* PERH0_PLL Tune0 Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:3;            /* - */
	u32 tune1:3;            /* peri_tune[2:0].
				 * PLLÊý×ÖºÍÄ£ÄâLDOµçÑ¹µ÷½Ú¡£
				 * µçÑ¹¹ýµÍÓÐ¿ÉÄÜÓ°ÏìÊä³öÊ±ÖÓµÄÕ¼¿Õ±È¡£
				 * [3¡¯b000]: 0.95V
				 * [3¡¯b001]: 1.00V
				 * [3¡¯b010]: 1.05V
				 * [3¡¯b011]: 1.10V
				 * [3¡¯b100]: 1.15V
				 * [3¡¯b101]: 1.20V
				 * [3¡¯b110]: 1.25V
				 * [3¡¯b111]: 1.30V */
	u32 tune2:5;            /* peri_tune[7:3].
				 * PLLÄÚ²¿cpµçÁ÷µ÷½Ú¡£
				 * µçÁ÷Ô½´ó£¬PLL»·Â·´ø¿íÔ½´ó¡£
				 * I = (TUNE13_9 + 1) * 0.5uA. */
	u32 tune3:1;            /* peri_tune[8].
				 * PLLÆ«ÖÃÄ£Ê½¡£
				 * [1¡¯b0]: ÊÖ¶¯Æ«ÖÃÄ£Ê½
				 * [1¡¯b1]: ×ÔÆ«ÖÃÄ£Ê½
				 * Ñ¡Ôñ×ÔÆ«ÖÃÄ£Ê½£¬»·Â·´ø¿í²»Ëæn factor±ä»
				 * ¯¶ø±ä»¯¡£ */
	u32 tune4:3;            /* peri_tune[11:9].
				 * PLL VCOÔöÒæµ÷½Ú¡£
				 * [3¡¯b000]: 1.6GHz/V
				 * [3¡¯b001]: 1.8GHz/V
				 * [3¡¯b010]: 2.2GHz/V
				 * [3¡¯b011]: 2.6GHz/V
				 * [3¡¯b100]: 3.0GHz/V
				 * [3¡¯b101]: 3.4GHz/V
				 * [3¡¯b110]: 3.8GHz/V
				 * [3¡¯b111]: 4.2GHz/V
				 * ¸ù¾ÝPLL¹¤×÷ÆµÂÊµ÷½Ú¿ÉµÃµ½½ÏºÃÔëÉùÐÔÄÜ¡£ */
	u32 tune5:1;            /* peri_tune[12].
				 * PLL n factorÒì³£¼à²âÊ¹ÄÜ¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜÊ±£¬Èôn³öÏÖÒì³£PLL×Ô¶¯´¦Àí£¬·ÀÖ¹PLL
				 * ½øÈëÒì³£¹¤×÷×´Ì¬¡£ */
	u32 tune6:1;            /* peri_tune[13].
				 * ckbkÂö¿íÔö¼Óµ÷½ÚÐÅºÅ.
				 * [1¡¯b0]: ²»Ôö¼Ó
				 * [1¡¯b1]: Ôö¼Ó */
	u32 tune7:1;            /* peri_tune[14].
				 * PLL¶¯Ì¬µ÷ÆµÊ±ÖÓÏàÎ»µ÷½Ú¡£
				 * [1¡¯b0]: ·´ÏàÊä³ö
				 * [1¡¯b1]: ²»·´ÏàÊä³ö
				 * ¾ö¶¨peri_cksdmÊÇ·ñ·´ÏàÊä³öÖÁCMU. */
	u32 tune8:1;            /* peri_tune[16].
				 * PLLÕûÊý·ÖÆµµ÷ÆµÄ£Ê½Ñ¡Ôñ¡£
				 * [1¡¯b0]: Ö±½ÓÌø±ä
				 * [1¡¯b1]: Öð½¥Ìø±ä */
	u32 tune9:3;            /* peri_tune[19:17].
				 * PLLÕûÊý·ÖÆµÖð½¥Ìø±äµ÷ÆµÄ£Ê½µÄÌø±äÐ±ÂÊ¿ØÖÆ¡£
				 * ¹«Ê½:
				 *  */
	u32 rsvd1:3;            /* - */
	u32 tune10:1;           /* peri_tune[15].
				 * Êý×ÖÄ£¿éÊ±ÖÓÔ´Ñ¡Ôñ.
				 * [1¡¯b0]: cksdm
				 * [1¡¯b1]: dck */
	u32 rsvd2:6;            /* - */
	} bits;
} reg_prcm_perh0_pll_tune0_t;

#define PRCM_PERH0_PLL_TUNE0_TUNE1_0    0x0
#define PRCM_PERH0_PLL_TUNE0_TUNE1_1    0x1
#define PRCM_PERH0_PLL_TUNE0_TUNE1_2    0x2
#define PRCM_PERH0_PLL_TUNE0_TUNE1_3    0x3
#define PRCM_PERH0_PLL_TUNE0_TUNE1_4    0x4
#define PRCM_PERH0_PLL_TUNE0_TUNE1_5    0x5
#define PRCM_PERH0_PLL_TUNE0_TUNE1_6    0x6
#define PRCM_PERH0_PLL_TUNE0_TUNE1_7    0x7
#define PRCM_PERH0_PLL_TUNE0_TUNE3_0    0x0
#define PRCM_PERH0_PLL_TUNE0_TUNE3_1    0x1
#define PRCM_PERH0_PLL_TUNE0_TUNE4_0    0x0
#define PRCM_PERH0_PLL_TUNE0_TUNE4_1    0x1
#define PRCM_PERH0_PLL_TUNE0_TUNE4_2    0x2
#define PRCM_PERH0_PLL_TUNE0_TUNE4_3    0x3
#define PRCM_PERH0_PLL_TUNE0_TUNE4_4    0x4
#define PRCM_PERH0_PLL_TUNE0_TUNE4_5    0x5
#define PRCM_PERH0_PLL_TUNE0_TUNE4_6    0x6
#define PRCM_PERH0_PLL_TUNE0_TUNE4_7    0x7
#define PRCM_PERH0_PLL_TUNE0_TUNE5_0    0x0
#define PRCM_PERH0_PLL_TUNE0_TUNE5_1    0x1
#define PRCM_PERH0_PLL_TUNE0_TUNE6_0    0x0
#define PRCM_PERH0_PLL_TUNE0_TUNE6_1    0x1
#define PRCM_PERH0_PLL_TUNE0_TUNE7_0    0x0
#define PRCM_PERH0_PLL_TUNE0_TUNE7_1    0x1
#define PRCM_PERH0_PLL_TUNE0_TUNE8_0    0x0
#define PRCM_PERH0_PLL_TUNE0_TUNE8_1    0x1
#define PRCM_PERH0_PLL_TUNE0_TUNE10_0   0x0
#define PRCM_PERH0_PLL_TUNE0_TUNE10_1   0x1

/* PERH0_PLL Test Enable Register */
typedef union {
	u32 val;
	struct {
	u32 ten0:1;             /* peri_ten[0].
				 * PLLÊý×ÖLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÊý×ÖLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 ten1:1;             /* peri_ten[1].
				 * PLLÄ£ÄâLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÄ£ÄâLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 ten3:1;             /* peri_ten[3].
				 * PLLÊä³öÊ±ÖÓ²âÊÔÊ¹ÄÜ¡£
				 * ÖÃ1Ê±PLLÊä³öÊ±ÖÓÊä³öÖÁtblk¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 ten2:1;             /* peri_ten[2].
				 * PLLÄ£ÄâLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÄ£ÄâLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:28;           /* - */
	} bits;
} reg_prcm_perh0_pll_ten_t;

#define PRCM_PERH0_PLL_TEN_TEN0_0       0x0
#define PRCM_PERH0_PLL_TEN_TEN0_1       0x1
#define PRCM_PERH0_PLL_TEN_TEN1_0       0x0
#define PRCM_PERH0_PLL_TEN_TEN1_1       0x1
#define PRCM_PERH0_PLL_TEN_TEN3_0       0x0
#define PRCM_PERH0_PLL_TEN_TEN3_1       0x1
#define PRCM_PERH0_PLL_TEN_TEN2_0       0x0
#define PRCM_PERH0_PLL_TEN_TEN2_1       0x1

/* PERH0_PLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 dlock:1;            /* PLL digital lock status.
				 * When PLL reaches stable, digital lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 asteady:1;          /* peri_steady.
				 * PLL analog steady status.
				 * [1¡¯b0]: not steady
				 * [1¡¯b1]: steady
				 * Factor N±ä»¯ÎÈ¶¨ÐÅºÅ, ¿ÉÓÃÓÚÅÐ¶ÏÏßÐÔµ÷Æ
				 * µµ÷ÆµºÍÕûÊý·ÖÆµÖð½¥µ÷Æµ½áÊø */
	u32 alock:1;            /* peri_lock.
				 * PLL analog lock status.
				 * When PLL reaches stable, analog lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 rsvd0:29;           /* - */
	} bits;
} reg_prcm_perh0_pll_stat_t;

#define PRCM_PERH0_PLL_STAT_DLOCK_0     0x0
#define PRCM_PERH0_PLL_STAT_DLOCK_1     0x1
#define PRCM_PERH0_PLL_STAT_ASTEADY_0   0x0
#define PRCM_PERH0_PLL_STAT_ASTEADY_1   0x1
#define PRCM_PERH0_PLL_STAT_ALOCK_0     0x0
#define PRCM_PERH0_PLL_STAT_ALOCK_1     0x1

/* PERH0_PLL Mode Register */
typedef union {
	u32 val;
	struct {
	u32 mod:2;              /* PLL¹¤×÷Ä£Ê½Ñ¡Ôñ¡£
				 * [2¡¯b00]: ÕûÊý·ÖÆµ
				 * [2¡¯b01]: -
				 * [2¡¯b10]: -
				 * [2¡¯b11]: -
				 * PLLÖ»ÄÜ¹¤×÷ÔÚÆäÖÐÒ»ÖÖÄ£Ê½£¬¶ÔÏßÐÔµ÷Æµ£¬
				 * ¿ÉÔÚTUNE¿ØÖÆ¿ªÆôÏßÐÔµ÷Æµµþ¼ÓÕ¹Æµ¡£ */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_prcm_perh0_pll_mod_t;

#define PRCM_PERH0_PLL_MOD_MOD_0        0x0
#define PRCM_PERH0_PLL_MOD_MOD_1        0x1
#define PRCM_PERH0_PLL_MOD_MOD_2        0x2
#define PRCM_PERH0_PLL_MOD_MOD_3        0x3

/* PERH0_PLL N Factor Fraction Register */
typedef union {
	u32 val;
	struct {
	u32 nfrac:18;           /* peri_nfrac[17:0].
				 * PLL nÒò×Ó·ÖÊý²¿·Ö¡£Ö»ÓÐÔÚÐ¡Êý·ÖÆµ¡¢Õ¹Æµ
				 * ÒÔ¼°ÏßÐÔµ÷ÆµÄ£Ê½ÏÂÓÐÐ§¡£
				 * ¹«Ê½£º
				 *  */
	u32 rsvd0:14;           /* - */
	} bits;
} reg_prcm_perh0_pll_nfrac_t;

/* AUDIO_PLL Enable Register */
typedef union {
	u32 val;
	struct {
	u32 enp:1;              /* aud_enp&aud_enc.
				 * PLLÊ¹ÄÜÐÅºÅ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÕý³£¹¤×÷Ðè10us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 enm:1;              /* aud_enm.
				 * PLLÆ«ÖÃµçÂ·Ê¹ÄÜ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÆ«ÖÃµçÂ·Õý³£¹¤×÷Ðè100us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 oen:1;              /* aud_oen.
				 * PLL Êä³ögating¡£
				 * OEN = 0Ê±PLL¸ßÆµÊ±ÖÓ²»Êä³ö£¬¼´audio_ckh
				 * sºÍaudio_cklsÊä³öÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ren:1;              /* aud_ren.
				 * PLLÊä³öÄ£ÄâÊ±ÖÓÊ¹ÄÜ¡£
				 * RGATÖÃ0Ê±audio_ckanaÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * For test, */
	u32 rsvd1:3;            /* - */
	u32 div_endiv17:1;      /* div_clodiv17Êä³öÊ¹ÄÜÐÅºÅ.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 div_endiv7:1;       /* div_clodiv7Êä³öÊ¹ÄÜÐÅºÅ.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd2:22;           /* - */
	} bits;
} reg_prcm_audio_pll_en_t;

#define PRCM_AUDIO_PLL_EN_ENP_0         0x0
#define PRCM_AUDIO_PLL_EN_ENP_1         0x1
#define PRCM_AUDIO_PLL_EN_ENM_0         0x0
#define PRCM_AUDIO_PLL_EN_ENM_1         0x1
#define PRCM_AUDIO_PLL_EN_OEN_0         0x0
#define PRCM_AUDIO_PLL_EN_OEN_1         0x1
#define PRCM_AUDIO_PLL_EN_REN_0         0x0
#define PRCM_AUDIO_PLL_EN_REN_1         0x1
#define PRCM_AUDIO_PLL_EN_DIV_ENDIV17_0 0x0
#define PRCM_AUDIO_PLL_EN_DIV_ENDIV17_1 0x1
#define PRCM_AUDIO_PLL_EN_DIV_ENDIV7_0  0x0
#define PRCM_AUDIO_PLL_EN_DIV_ENDIV7_1  0x1

/* AUDIO_PLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 pre_div:2;          /* aud_prediv[1:0].
				 * PLL pre-divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4
				 * Ò»°ãÇé¿öPRE_DIVÓ¦±£³ÖÖÃ2¡¯b00.ÉÙÊýÇé¿öP
				 * RE_DIVÊ¹ÓÃ2¡¯b01/2¡¯b10/2¡¯b11. */
	u32 rsvd0:2;            /* - */
	u32 post_div:2;         /* aud_postdiv[1:0].
				 * PLL post divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4 */
	u32 rsvd1:2;            /* - */
	u32 n:8;                /* aud_nint[7:0].
				 * PLL n factor.
				 * Valid value = N.
				 * Õý³£Ê¹ÓÃÊ±£¬valid value±ØÐë´óÓÚ10. ÇÒN=
				 * 0½ûÖ¹Ê¹ÓÃ¡£ */
	u32 rsvd2:16;           /* - */
	} bits;
} reg_prcm_audio_pll_fac_t;

#define PRCM_AUDIO_PLL_FAC_PRE_DIV_0    0x0
#define PRCM_AUDIO_PLL_FAC_PRE_DIV_1    0x1
#define PRCM_AUDIO_PLL_FAC_PRE_DIV_2    0x2
#define PRCM_AUDIO_PLL_FAC_PRE_DIV_3    0x3
#define PRCM_AUDIO_PLL_FAC_POST_DIV_0   0x0
#define PRCM_AUDIO_PLL_FAC_POST_DIV_1   0x1
#define PRCM_AUDIO_PLL_FAC_POST_DIV_2   0x2
#define PRCM_AUDIO_PLL_FAC_POST_DIV_3   0x3

/* AUDIO_PLL Tune0 Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:3;            /* - */
	u32 tune1:3;            /* aud_tune[2:0].
				 * PLL LDOµçÑ¹µ÷½Ú¡£
				 * µçÑ¹¹ýµÍÓÐ¿ÉÄÜÓ°ÏìÊä³öÊ±ÖÓµÄÕ¼¿Õ±È¡£
				 * [3¡¯b000]: 0.95V
				 * [3¡¯b001]: 1.00V
				 * [3¡¯b010]: 1.05V
				 * [3¡¯b011]: 1.10V
				 * [3¡¯b100]: 1.15V
				 * [3¡¯b101]: 1.20V
				 * [3¡¯b110]: 1.25V
				 * [3¡¯b111]: 1.30V */
	u32 tune2:5;            /* aud_tune[7:3].
				 * PLLÄÚ²¿cpµçÁ÷µ÷½Ú¡£
				 * µçÁ÷Ô½´ó£¬PLL»·Â·´ø¿íÔ½´ó¡£
				 * I = (TUNE2 + 1) * 0.5uA. */
	u32 tune3:1;            /* aud_tune[8].
				 * PLLÆ«ÖÃÄ£Ê½¡£
				 * [1¡¯b0]: ÊÖ¶¯Æ«ÖÃÄ£Ê½
				 * [1¡¯b1]: ×ÔÆ«ÖÃÄ£Ê½
				 * Ñ¡Ôñ×ÔÆ«ÖÃÄ£Ê½£¬»·Â·´ø¿í²»Ëæn factor±ä»
				 * ¯¶ø±ä»¯¡£ */
	u32 tune4:3;            /* aud_tune[11:9].
				 * PLL VCOÔöÒæµ÷½Ú¡£
				 * [3¡¯b000]: 1.6GHz/V
				 * [3¡¯b001]: 1.8GHz/V
				 * [3¡¯b010]: 2.2GHz/V
				 * [3¡¯b011]: 2.6GHz/V
				 * [3¡¯b100]: 3.0GHz/V
				 * [3¡¯b101]: 3.4GHz/V
				 * [3¡¯b110]: 3.8GHz/V
				 * [3¡¯b111]: 4.2GHz/V
				 * ¸ù¾ÝPLL¹¤×÷ÆµÂÊµ÷½Ú¿ÉµÃµ½½ÏºÃÔëÉùÐÔÄÜ¡£ */
	u32 tune5:1;            /* aud_tune[12].
				 * PLL n factorÒì³£¼à²âÊ¹ÄÜ¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜÊ±£¬Èôn³öÏÖÒì³£PLL×Ô¶¯´¦Àí£¬·ÀÖ¹PLL
				 * ½øÈëÒì³£¹¤×÷×´Ì¬¡£ */
	u32 tune6:1;            /* aud_tune[13].
				 * audpll aud_ckbkÂö¿íÔö¼Óµ÷½ÚÐÅºÅ.
				 * [1¡¯b0]: ²»Ôö¼Ó
				 * [1¡¯b1]: Ôö¼Ó */
	u32 tune7:1;            /* aud_tune[14].
				 * PLL¶¯Ì¬µ÷ÆµÊ±ÖÓÏàÎ»µ÷½Ú¡£
				 * [1¡¯b0]: ·´ÏàÊä³ö
				 * [1¡¯b1]: ²»·´ÏàÊä³ö
				 * ¾ö¶¨audio_cksdmÊÇ·ñ·´ÏàÊä³öÖÁCMU. */
	u32 tune8:1;            /* aud_tune[16].
				 * PLLsÕûÊý·ÖÆµµ÷ÆµÄ£Ê½Ñ¡Ôñ¡£
				 * [1¡¯b0]: Ö±½ÓÌø±ä
				 * [1¡¯b1]: Öð½¥Ìø±ä */
	u32 tune9:3;            /* aud_tune[19:17].
				 * PLLÕûÊý·ÖÆµÖð½¥Ìø±äµ÷ÆµÄ£Ê½µÄÌø±äÐ±ÂÊ¿ØÖÆ¡£
				 * ¹«Ê½:
				 *  */
	u32 tune10:1;           /* aud_tune[20].
				 * LSB ditherÆô¶¯ÐÅºÅ¡£¸ÃÑ¡ÏîÖ÷ÒªÓÃÓÚ¸ÄÉÆÐ
				 * ¡Êý·ÖÆµ¡¢Õ¹Æµ¼°ÏßÐÔµ÷ÆµµÄÐÔÄÜ£¬¶ÔÕûÊý·Ö
				 * ÆµÃ»ÓÐ×÷ÓÃ¡£
				 * [1¡¯b0]: ²»Æô¶¯
				 * [1¡¯b1]: Æô¶¯ */
	u32 tune11:2;           /* aud_tune[22:21].
				 * PLL¶¯Ì¬µ÷Æµµ÷ÖÆ·½Ê½¡£
				 * [2¡¯b00]: Ò»½×¼ÓÍÌÂö³å
				 * [2¡¯b01]: Ò»½×¼Óµ÷½Ún
				 * [2¡¯b10]: MASH111µ÷ÖÆ
				 * [2¡¯b11]: ÕûÊý·ÖÆµ */
	u32 tune12:1;           /* aud_tune[15].
				 * Êý×ÖÄ£¿éÊ±ÖÓÔ´Ñ¡Ôñ.
				 * [1¡¯b0]: cksdm
				 * [1¡¯b1]: dck */
	u32 rsvd1:6;            /* - */
	} bits;
} reg_prcm_audio_pll_tune0_t;

#define PRCM_AUDIO_PLL_TUNE0_TUNE1_0    0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE1_1    0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE1_2    0x2
#define PRCM_AUDIO_PLL_TUNE0_TUNE1_3    0x3
#define PRCM_AUDIO_PLL_TUNE0_TUNE1_4    0x4
#define PRCM_AUDIO_PLL_TUNE0_TUNE1_5    0x5
#define PRCM_AUDIO_PLL_TUNE0_TUNE1_6    0x6
#define PRCM_AUDIO_PLL_TUNE0_TUNE1_7    0x7
#define PRCM_AUDIO_PLL_TUNE0_TUNE3_0    0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE3_1    0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE4_0    0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE4_1    0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE4_2    0x2
#define PRCM_AUDIO_PLL_TUNE0_TUNE4_3    0x3
#define PRCM_AUDIO_PLL_TUNE0_TUNE4_4    0x4
#define PRCM_AUDIO_PLL_TUNE0_TUNE4_5    0x5
#define PRCM_AUDIO_PLL_TUNE0_TUNE4_6    0x6
#define PRCM_AUDIO_PLL_TUNE0_TUNE4_7    0x7
#define PRCM_AUDIO_PLL_TUNE0_TUNE5_0    0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE5_1    0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE6_0    0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE6_1    0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE7_0    0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE7_1    0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE8_0    0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE8_1    0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE10_0   0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE10_1   0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE11_0   0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE11_1   0x1
#define PRCM_AUDIO_PLL_TUNE0_TUNE11_2   0x2
#define PRCM_AUDIO_PLL_TUNE0_TUNE11_3   0x3
#define PRCM_AUDIO_PLL_TUNE0_TUNE12_0   0x0
#define PRCM_AUDIO_PLL_TUNE0_TUNE12_1   0x1

/* AUDIO_PLL Test Enable Register */
typedef union {
	u32 val;
	struct {
	u32 ten0:1;             /* aud_ten[1].
				 * PLLÊý×ÖLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÊý×ÖLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ten1:1;             /* aud_ten[1].
				 * PLLÊä³öÊ±ÖÓ²âÊÔÊ¹ÄÜ¡£
				 * ÖÃ1Ê±PLLÊä³öÊ±ÖÓÊä³öÖÁtblk¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd1:29;           /* - */
	} bits;
} reg_prcm_audio_pll_ten_t;

#define PRCM_AUDIO_PLL_TEN_TEN0_0       0x0
#define PRCM_AUDIO_PLL_TEN_TEN0_1       0x1
#define PRCM_AUDIO_PLL_TEN_TEN1_0       0x0
#define PRCM_AUDIO_PLL_TEN_TEN1_1       0x1

/* AUDIO_PLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 dlock:1;            /* PLL digital lock status.
				 * When PLL reaches stable, digital lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 asteady:1;          /* aud_steady.
				 * PLL analog steady status.
				 * [1¡¯b0]: not steady
				 * [1¡¯b1]: steady
				 * Factor N±ä»¯ÎÈ¶¨ÐÅºÅ, ¿ÉÓÃÓÚÅÐ¶ÏÏßÐÔµ÷Æ
				 * µµ÷ÆµºÍÕûÊý·ÖÆµÖð½¥µ÷Æµ½áÊø */
	u32 alock:1;            /* aud_lock.
				 * PLL analog lock status.
				 * When PLL reaches stable, analog lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 rsvd0:29;           /* - */
	} bits;
} reg_prcm_audio_pll_stat_t;

#define PRCM_AUDIO_PLL_STAT_DLOCK_0     0x0
#define PRCM_AUDIO_PLL_STAT_DLOCK_1     0x1
#define PRCM_AUDIO_PLL_STAT_ASTEADY_0   0x0
#define PRCM_AUDIO_PLL_STAT_ASTEADY_1   0x1
#define PRCM_AUDIO_PLL_STAT_ALOCK_0     0x0
#define PRCM_AUDIO_PLL_STAT_ALOCK_1     0x1

/* AUDIO_PLL Mode Register */
typedef union {
	u32 val;
	struct {
	u32 mod:2;              /* aud_msel[1:0].
				 * PLL¹¤×÷Ä£Ê½Ñ¡Ôñ¡£
				 * [2¡¯b00]: ÕûÊý·ÖÆµ
				 * [2¡¯b01]: Ð¡Êý·ÖÆµ
				 * [2¡¯b10]: -
				 * [2¡¯b11]: -
				 * PLLÖ»ÄÜ¹¤×÷ÔÚÆäÖÐÒ»ÖÖÄ£Ê½£¬¶ÔÏßÐÔµ÷Æµ£¬
				 * ¿ÉÔÚTUNE¿ØÖÆ¿ªÆôÏßÐÔµ÷Æµµþ¼ÓÕ¹Æµ¡£ */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_prcm_audio_pll_mod_t;

#define PRCM_AUDIO_PLL_MOD_MOD_0        0x0
#define PRCM_AUDIO_PLL_MOD_MOD_1        0x1
#define PRCM_AUDIO_PLL_MOD_MOD_2        0x2
#define PRCM_AUDIO_PLL_MOD_MOD_3        0x3

/* AUDIO_PLL N Factor Fraction Register */
typedef union {
	u32 val;
	struct {
	u32 nfrac:18;           /* aud_nfrac[17:0].
				 * PLL nÒò×Ó·ÖÊý²¿·Ö¡£Ö»ÓÐÔÚÐ¡Êý·ÖÆµ¡¢Õ¹Æµ
				 * ¡¢ÏßÐÔµ÷ÆµÄ£Ê½ÏÂÏÂÓÐÐ§¡£
				 * ¹«Ê½£º
				 *  */
	u32 rsvd0:14;           /* - */
	} bits;
} reg_prcm_audio_pll_nfrac_t;

/* VC_PLL Enable Register */
typedef union {
	u32 val;
	struct {
	u32 enp:1;              /* vc_enp&enc.
				 * PLLÊ¹ÄÜÐÅºÅ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÕý³£¹¤×÷Ðè10us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 enm:1;              /* vc_enm.
				 * PLLÆ«ÖÃµçÂ·Ê¹ÄÜ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÆ«ÖÃµçÂ·Õý³£¹¤×÷Ðè100us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 oen:1;              /* vc_oen.
				 * PLL Êä³ögating¡£
				 * OEN = 0Ê±PLL¸ßÆµÊ±ÖÓ²»Êä³ö£¬¼´audio_ckh
				 * sºÍaudio_cklsÊä³öÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ren:1;              /* vc_ren.
				 * PLLÊä³öÄ£ÄâÊ±ÖÓÊ¹ÄÜ¡£
				 * RGATÖÃ0Ê±audio_ckanaÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * For test. */
	u32 rsvd1:27;           /* - */
	} bits;
} reg_prcm_vc_pll_en_t;

#define PRCM_VC_PLL_EN_ENP_0    0x0
#define PRCM_VC_PLL_EN_ENP_1    0x1
#define PRCM_VC_PLL_EN_ENM_0    0x0
#define PRCM_VC_PLL_EN_ENM_1    0x1
#define PRCM_VC_PLL_EN_OEN_0    0x0
#define PRCM_VC_PLL_EN_OEN_1    0x1
#define PRCM_VC_PLL_EN_REN_0    0x0
#define PRCM_VC_PLL_EN_REN_1    0x1

/* VC_PLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 pre_div:2;          /* vc_prediv[1:0].
				 * PLL pre-divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4
				 * ·ÇÌØÊâÇé¿ö£¬PRE_DIVÓ¦±£³ÖÖÃ0. */
	u32 rsvd0:2;            /* - */
	u32 post_div:2;         /* vc_postdiv[1:0].
				 * PLL post divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4 */
	u32 rsvd1:2;            /* - */
	u32 n:8;                /* vc_nint[7:0].
				 * PLL n factor.
				 * Valid value = N.
				 * Õý³£Ê¹ÓÃÊ±£¬valid value±ØÐë´óÓÚ10. ÇÒN=
				 * 0½ûÖ¹Ê¹ÓÃ¡£ */
	u32 rsvd2:16;           /* - */
	} bits;
} reg_prcm_vc_pll_fac_t;

#define PRCM_VC_PLL_FAC_PRE_DIV_0       0x0
#define PRCM_VC_PLL_FAC_PRE_DIV_1       0x1
#define PRCM_VC_PLL_FAC_PRE_DIV_2       0x2
#define PRCM_VC_PLL_FAC_PRE_DIV_3       0x3
#define PRCM_VC_PLL_FAC_POST_DIV_0      0x0
#define PRCM_VC_PLL_FAC_POST_DIV_1      0x1
#define PRCM_VC_PLL_FAC_POST_DIV_2      0x2
#define PRCM_VC_PLL_FAC_POST_DIV_3      0x3

/* VC_PLL Tune0 Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:3;            /* - */
	u32 tune1:3;            /* vc_tune[2:0].
				 * PLLÊý×ÖLDOµçÑ¹µ÷½Ú¡£
				 * µçÑ¹¹ýµÍÓÐ¿ÉÄÜÓ°ÏìÊä³öÊ±ÖÓµÄÕ¼¿Õ±È¡£
				 * [3¡¯b000]: 0.95V
				 * [3¡¯b001]: 1.00V
				 * [3¡¯b010]: 1.05V
				 * [3¡¯b011]: 1.10V
				 * [3¡¯b100]: 1.15V
				 * [3¡¯b101]: 1.20V
				 * [3¡¯b110]: 1.25V
				 * [3¡¯b111]: 1.30V */
	u32 tune2:5;            /* vc_tune[7:3].
				 * PLLÄÚ²¿cpµçÁ÷µ÷½Ú¡£
				 * µçÁ÷Ô½´ó£¬PLL»·Â·´ø¿íÔ½´ó¡£
				 * I = (TUNE13_9 + 1) * 0.5uA. */
	u32 tune3:1;            /* vc_tune[8].
				 * PLLÆ«ÖÃÄ£Ê½¡£
				 * [1¡¯b0]: ÊÖ¶¯Æ«ÖÃÄ£Ê½
				 * [1¡¯b1]: ×ÔÆ«ÖÃÄ£Ê½
				 * Ñ¡Ôñ×ÔÆ«ÖÃÄ£Ê½£¬»·Â·´ø¿í²»Ëæn factor±ä»
				 * ¯¶ø±ä»¯¡£ */
	u32 tune4:3;            /* vc_tune[11:9].
				 * PLL VCOÔöÒæµ÷½Ú¡£
				 * [3¡¯b000]: 1.6GHz/V
				 * [3¡¯b001]: 1.8GHz/V
				 * [3¡¯b010]: 2.2GHz/V
				 * [3¡¯b011]: 2.6GHz/V
				 * [3¡¯b100]: 3.0GHz/V
				 * [3¡¯b101]: 3.4GHz/V
				 * [3¡¯b110]: 3.8GHz/V
				 * [3¡¯b111]: 4.2GHz/V
				 * ¸ù¾ÝPLL¹¤×÷ÆµÂÊµ÷½Ú¿ÉµÃµ½½ÏºÃÔëÉùÐÔÄÜ¡£ */
	u32 tune5:1;            /* vc_tune[12].
				 * PLL n factorÒì³£¼à²âÊ¹ÄÜ¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜÊ±£¬Èôn³öÏÖÒì³£PLL×Ô¶¯´¦Àí£¬·ÀÖ¹PLL
				 * ½øÈëÒì³£¹¤×÷×´Ì¬¡£ */
	u32 tune6:1;            /* vc_tune[13].
				 * ckbkÂö¿íÔö¼Óµ÷½ÚÐÅºÅ.
				 * [1¡¯b0]: ²»Ôö¼Ó
				 * [1¡¯b1]: Ôö¼Ó */
	u32 tune7:1;            /* vc_tune[14].
				 * PLL¶¯Ì¬µ÷ÆµÊ±ÖÓÏàÎ»µ÷½Ú¡£
				 * [1¡¯b0]: ·´ÏàÊä³ö
				 * [1¡¯b1]: ²»·´ÏàÊä³ö
				 * ¾ö¶¨vc_cksdmÊÇ·ñ·´ÏàÊä³öÖÁCMU. */
	u32 tune8:1;            /* vc_tune[16].
				 * PLLÕûÊý·ÖÆµµ÷ÆµÄ£Ê½Ñ¡Ôñ¡£
				 * [1¡¯b0]: Ö±½ÓÌø±ä
				 * [1¡¯b1]: Öð½¥Ìø±ä */
	u32 tune9:3;            /* vc_tune[19:17].
				 * PLLÕûÊý·ÖÆµÖð½¥Ìø±äµ÷ÆµÄ£Ê½µÄÌø±äÐ±ÂÊ¿ØÖÆ¡£
				 * ¹«Ê½:
				 *  */
	u32 rsvd1:3;            /* - */
	u32 tune10:1;           /* vc_tune[15].
				 * Êý×ÖÄ£¿éÊ±ÖÓÔ´Ñ¡Ôñ.
				 * [1¡¯b0]: cksdm
				 * [1¡¯b1]: dck */
	u32 rsvd2:6;            /* - */
	} bits;
} reg_prcm_vc_pll_tune0_t;

#define PRCM_VC_PLL_TUNE0_TUNE1_0       0x0
#define PRCM_VC_PLL_TUNE0_TUNE1_1       0x1
#define PRCM_VC_PLL_TUNE0_TUNE1_2       0x2
#define PRCM_VC_PLL_TUNE0_TUNE1_3       0x3
#define PRCM_VC_PLL_TUNE0_TUNE1_4       0x4
#define PRCM_VC_PLL_TUNE0_TUNE1_5       0x5
#define PRCM_VC_PLL_TUNE0_TUNE1_6       0x6
#define PRCM_VC_PLL_TUNE0_TUNE1_7       0x7
#define PRCM_VC_PLL_TUNE0_TUNE3_0       0x0
#define PRCM_VC_PLL_TUNE0_TUNE3_1       0x1
#define PRCM_VC_PLL_TUNE0_TUNE4_0       0x0
#define PRCM_VC_PLL_TUNE0_TUNE4_1       0x1
#define PRCM_VC_PLL_TUNE0_TUNE4_2       0x2
#define PRCM_VC_PLL_TUNE0_TUNE4_3       0x3
#define PRCM_VC_PLL_TUNE0_TUNE4_4       0x4
#define PRCM_VC_PLL_TUNE0_TUNE4_5       0x5
#define PRCM_VC_PLL_TUNE0_TUNE4_6       0x6
#define PRCM_VC_PLL_TUNE0_TUNE4_7       0x7
#define PRCM_VC_PLL_TUNE0_TUNE5_0       0x0
#define PRCM_VC_PLL_TUNE0_TUNE5_1       0x1
#define PRCM_VC_PLL_TUNE0_TUNE6_0       0x0
#define PRCM_VC_PLL_TUNE0_TUNE6_1       0x1
#define PRCM_VC_PLL_TUNE0_TUNE7_0       0x0
#define PRCM_VC_PLL_TUNE0_TUNE7_1       0x1
#define PRCM_VC_PLL_TUNE0_TUNE8_0       0x0
#define PRCM_VC_PLL_TUNE0_TUNE8_1       0x1
#define PRCM_VC_PLL_TUNE0_TUNE10_0      0x0
#define PRCM_VC_PLL_TUNE0_TUNE10_1      0x1

/* VC_PLL Test Enable Register */
typedef union {
	u32 val;
	struct {
	u32 ten0:1;             /* vc_ten[0].
				 * PLLÊý×ÖLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÊý×ÖLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ten1:1;             /* vc_ten[1].
				 * PLLÊä³öÊ±ÖÓ²âÊÔÊ¹ÄÜ¡£
				 * ÖÃ1Ê±PLLÊä³öÊ±ÖÓÊä³öÖÁtblk¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd1:29;           /* - */
	} bits;
} reg_prcm_vc_pll_ten_t;

#define PRCM_VC_PLL_TEN_TEN0_0  0x0
#define PRCM_VC_PLL_TEN_TEN0_1  0x1
#define PRCM_VC_PLL_TEN_TEN1_0  0x0
#define PRCM_VC_PLL_TEN_TEN1_1  0x1

/* VC_PLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 dlock:1;            /* PLL digital lock status.
				 * When PLL reaches stable, digital lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 asteady:1;          /* PLL analog steady status.
				 * [1¡¯b0]: not steady
				 * [1¡¯b1]: steady
				 * Factor N±ä»¯ÎÈ¶¨ÐÅºÅ, ¿ÉÓÃÓÚÅÐ¶ÏÏßÐÔµ÷Æ
				 * µµ÷ÆµºÍÕûÊý·ÖÆµÖð½¥µ÷Æµ½áÊø */
	u32 alock:1;            /* aud_lock.
				 * PLL analog lock status.
				 * When PLL reaches stable, analog lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 rsvd0:29;           /* - */
	} bits;
} reg_prcm_vc_pll_stat_t;

#define PRCM_VC_PLL_STAT_DLOCK_0        0x0
#define PRCM_VC_PLL_STAT_DLOCK_1        0x1
#define PRCM_VC_PLL_STAT_ASTEADY_0      0x0
#define PRCM_VC_PLL_STAT_ASTEADY_1      0x1
#define PRCM_VC_PLL_STAT_ALOCK_0        0x0
#define PRCM_VC_PLL_STAT_ALOCK_1        0x1

/* VC_PLL Mode Register */
typedef union {
	u32 val;
	struct {
	u32 mod:2;              /* PLL¹¤×÷Ä£Ê½Ñ¡Ôñ¡£
				 * [2¡¯b00]: ÕûÊý·ÖÆµ
				 * [2¡¯b01]: Ð¡Êý·ÖÆµ
				 * [2¡¯b10]: -
				 * [2¡¯b11]: -
				 * PLLÖ»ÄÜ¹¤×÷ÔÚÆäÖÐÒ»ÖÖÄ£Ê½£¬¶ÔÏßÐÔµ÷Æµ£¬
				 * ¿ÉÔÚTUNE¿ØÖÆ¿ªÆôÏßÐÔµ÷Æµµþ¼ÓÕ¹Æµ¡£ */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_prcm_vc_pll_mod_t;

#define PRCM_VC_PLL_MOD_MOD_0   0x0
#define PRCM_VC_PLL_MOD_MOD_1   0x1
#define PRCM_VC_PLL_MOD_MOD_2   0x2
#define PRCM_VC_PLL_MOD_MOD_3   0x3

/* VC_PLL N Factor Fraction Register */
typedef union {
	u32 val;
	struct {
	u32 nfrac:18;           /* vc_nfrac[17:0].
				 * PLL nÒò×Ó·ÖÊý²¿·Ö¡£Ö»ÓÐÔÚÐ¡Êý·ÖÆµ¡¢Õ¹Æµ
				 * ¡¢ÏßÐÔµ÷ÆµÄ£Ê½ÏÂÏÂÓÐÐ§¡£
				 * ¹«Ê½£º
				 *  */
	u32 rsvd0:14;           /* - */
	} bits;
} reg_prcm_vc_pll_nfrac_t;

/* PERH1_PLL Enable Register */
typedef union {
	u32 val;
	struct {
	u32 enp:1;              /* peri1_enp&peri1_enc.
				 * PLLÊ¹ÄÜÐÅºÅ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÕý³£¹¤×÷Ðè10us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 enm:1;              /* peri1_enm.
				 * PLLÆ«ÖÃµçÂ·Ê¹ÄÜ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÆ«ÖÃµçÂ·Õý³£¹¤×÷Ðè100us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 oen:1;              /* peri1_oen.
				 * PLL Êä³ögating¡£
				 * OEN = 0Ê±PLL¸ßÆµÊ±ÖÓ²»Êä³ö£¬¼´peri_ckhs
				 * ºÍperi_cklsÊä³öÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ren:1;              /* peri1_ren.
				 * PLLÊä³öÄ£ÄâÊ±ÖÓÊ¹ÄÜ¡£
				 * RGATÖÃ0Ê±peri_ckanaÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * For test. */
	u32 rsvd1:27;           /* - */
	} bits;
} reg_prcm_perh1_pll_en_t;

#define PRCM_PERH1_PLL_EN_ENP_0         0x0
#define PRCM_PERH1_PLL_EN_ENP_1         0x1
#define PRCM_PERH1_PLL_EN_ENM_0         0x0
#define PRCM_PERH1_PLL_EN_ENM_1         0x1
#define PRCM_PERH1_PLL_EN_OEN_0         0x0
#define PRCM_PERH1_PLL_EN_OEN_1         0x1
#define PRCM_PERH1_PLL_EN_REN_0         0x0
#define PRCM_PERH1_PLL_EN_REN_1         0x1

/* PERH1_PLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 pre_div:2;          /* peri1_prediv[1:0].
				 * PLL pre-divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4
				 * ·ÇÌØÊâÇé¿ö£¬PRE_DIVÓ¦±£³ÖÖÃ0. */
	u32 rsvd0:2;            /* - */
	u32 post_div:2;         /* peri1_postdiv[1:0].
				 * PLL post divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4 */
	u32 rsvd1:2;            /* - */
	u32 n:8;                /* peri1_nint[7:0].
				 * PLL n factor.
				 * Valid value = N.
				 * Õý³£Ê¹ÓÃÊ±£¬valid value±ØÐë´óÓÚ10. ÇÒN=
				 * 0½ûÖ¹Ê¹ÓÃ¡£ */
	u32 rsvd2:16;           /* - */
	} bits;
} reg_prcm_perh1_pll_fac_t;

#define PRCM_PERH1_PLL_FAC_PRE_DIV_0    0x0
#define PRCM_PERH1_PLL_FAC_PRE_DIV_1    0x1
#define PRCM_PERH1_PLL_FAC_PRE_DIV_2    0x2
#define PRCM_PERH1_PLL_FAC_PRE_DIV_3    0x3
#define PRCM_PERH1_PLL_FAC_POST_DIV_0   0x0
#define PRCM_PERH1_PLL_FAC_POST_DIV_1   0x1
#define PRCM_PERH1_PLL_FAC_POST_DIV_2   0x2
#define PRCM_PERH1_PLL_FAC_POST_DIV_3   0x3

/* PERH1_PLL Tune0 Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:3;            /* - */
	u32 tune1:3;            /* peri1_tune[2:0].
				 * PLLÊý×ÖºÍÄ£ÄâLDOµçÑ¹µ÷½Ú¡£
				 * µçÑ¹¹ýµÍÓÐ¿ÉÄÜÓ°ÏìÊä³öÊ±ÖÓµÄÕ¼¿Õ±È¡£
				 * [3¡¯b000]: 0.95V
				 * [3¡¯b001]: 1.00V
				 * [3¡¯b010]: 1.05V
				 * [3¡¯b011]: 1.10V
				 * [3¡¯b100]: 1.15V
				 * [3¡¯b101]: 1.20V
				 * [3¡¯b110]: 1.25V
				 * [3¡¯b111]: 1.30V */
	u32 tune2:5;            /* peri1_tune[7:3].
				 * PLLÄÚ²¿cpµçÁ÷µ÷½Ú¡£
				 * µçÁ÷Ô½´ó£¬PLL»·Â·´ø¿íÔ½´ó¡£
				 * I = (TUNE2 + 1) * 0.5uA. */
	u32 tune3:1;            /* peri1_tune[8].
				 * PLLÆ«ÖÃÄ£Ê½¡£
				 * [1¡¯b0]: ÊÖ¶¯Æ«ÖÃÄ£Ê½
				 * [1¡¯b1]: ×ÔÆ«ÖÃÄ£Ê½
				 * Ñ¡Ôñ×ÔÆ«ÖÃÄ£Ê½£¬»·Â·´ø¿í²»Ëæn factor±ä»
				 * ¯¶ø±ä»¯¡£ */
	u32 tune4:3;            /* peri1_tune[11:9].
				 * PLL VCOÔöÒæµ÷½Ú¡£
				 * [3¡¯b000]: 1.6GHz/V
				 * [3¡¯b001]: 1.8GHz/V
				 * [3¡¯b010]: 2.2GHz/V
				 * [3¡¯b011]: 2.6GHz/V
				 * [3¡¯b100]: 3.0GHz/V
				 * [3¡¯b101]: 3.4GHz/V
				 * [3¡¯b110]: 3.8GHz/V
				 * [3¡¯b111]: 4.2GHz/V
				 * ¸ù¾ÝPLL¹¤×÷ÆµÂÊµ÷½Ú¿ÉµÃµ½½ÏºÃÔëÉùÐÔÄÜ¡£ */
	u32 tune5:1;            /* peri1_tune[12].
				 * PLL n factorÒì³£¼à²âÊ¹ÄÜ¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜÊ±£¬Èôn³öÏÖÒì³£PLL×Ô¶¯´¦Àí£¬·ÀÖ¹PLL
				 * ½øÈëÒì³£¹¤×÷×´Ì¬¡£ */
	u32 tune6:1;            /* peri1_tune[13].
				 * ckbkÂö¿íÔö¼Óµ÷½ÚÐÅºÅ.
				 * [1¡¯b0]: ²»Ôö¼Ó
				 * [1¡¯b1]: Ôö¼Ó */
	u32 tune7:1;            /* peri1_tune[14].
				 * PLL¶¯Ì¬µ÷ÆµÊ±ÖÓÏàÎ»µ÷½Ú¡£
				 * [1¡¯b0]: ·´ÏàÊä³ö
				 * [1¡¯b1]: ²»·´ÏàÊä³ö
				 * ¾ö¶¨peri_cksdmÊÇ·ñ·´ÏàÊä³öÖÁCMU. */
	u32 tune8:1;            /* peri1_tune[16].
				 * PLLÕûÊý·ÖÆµµ÷ÆµÄ£Ê½Ñ¡Ôñ¡£
				 * [1¡¯b0]: Ö±½ÓÌø±ä
				 * [1¡¯b1]: Öð½¥Ìø±ä */
	u32 tune9:3;            /* peri1_tune[19:17].
				 * PLLÕûÊý·ÖÆµÖð½¥Ìø±äµ÷ÆµÄ£Ê½µÄÌø±äÐ±ÂÊ¿ØÖÆ¡£
				 * ¹«Ê½:
				 *  */
	u32 tune10:1;           /* peri1_tune[20].
				 * LSB ditherÆô¶¯ÐÅºÅ¡£¸ÃÑ¡ÏîÖ÷ÒªÓÃÓÚ¸ÄÉÆÐ
				 * ¡Êý·ÖÆµ¡¢Õ¹Æµ¼°ÏßÐÔµ÷ÆµµÄÐÔÄÜ£¬¶ÔÕûÊý·Ö
				 * ÆµÃ»ÓÐ×÷ÓÃ¡£
				 * [1¡¯b0]: ²»Æô¶¯
				 * [1¡¯b1]: Æô¶¯ */
	u32 tune11:2;           /* peri1_tune[22:21].
				 * PLL¶¯Ì¬µ÷Æµµ÷ÖÆ·½Ê½¡£
				 * [2¡¯b00]: Ò»½×¼ÓÍÌÂö³å
				 * [2¡¯b01]: Ò»½×¼Óµ÷½Ún
				 * [2¡¯b10]: MASH111µ÷ÖÆ
				 * [2¡¯b11]: ÕûÊý·ÖÆµ */
	u32 tune12:1;           /* peri1_tune[15].
				 * Êý×ÖÄ£¿éÊ±ÖÓÔ´Ñ¡Ôñ.
				 * [1¡¯b0]: cksdm
				 * [1¡¯b1]: dck */
	u32 rsvd1:6;            /* - */
	} bits;
} reg_prcm_perh1_pll_tune0_t;

#define PRCM_PERH1_PLL_TUNE0_TUNE1_0    0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE1_1    0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE1_2    0x2
#define PRCM_PERH1_PLL_TUNE0_TUNE1_3    0x3
#define PRCM_PERH1_PLL_TUNE0_TUNE1_4    0x4
#define PRCM_PERH1_PLL_TUNE0_TUNE1_5    0x5
#define PRCM_PERH1_PLL_TUNE0_TUNE1_6    0x6
#define PRCM_PERH1_PLL_TUNE0_TUNE1_7    0x7
#define PRCM_PERH1_PLL_TUNE0_TUNE3_0    0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE3_1    0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE4_0    0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE4_1    0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE4_2    0x2
#define PRCM_PERH1_PLL_TUNE0_TUNE4_3    0x3
#define PRCM_PERH1_PLL_TUNE0_TUNE4_4    0x4
#define PRCM_PERH1_PLL_TUNE0_TUNE4_5    0x5
#define PRCM_PERH1_PLL_TUNE0_TUNE4_6    0x6
#define PRCM_PERH1_PLL_TUNE0_TUNE4_7    0x7
#define PRCM_PERH1_PLL_TUNE0_TUNE5_0    0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE5_1    0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE6_0    0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE6_1    0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE7_0    0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE7_1    0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE8_0    0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE8_1    0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE10_0   0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE10_1   0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE11_0   0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE11_1   0x1
#define PRCM_PERH1_PLL_TUNE0_TUNE11_2   0x2
#define PRCM_PERH1_PLL_TUNE0_TUNE11_3   0x3
#define PRCM_PERH1_PLL_TUNE0_TUNE12_0   0x0
#define PRCM_PERH1_PLL_TUNE0_TUNE12_1   0x1

/* PERH1_PLL Test Enable Register */
typedef union {
	u32 val;
	struct {
	u32 ten0:1;             /* peri1_ten[0].
				 * PLLÊý×ÖLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÊý×ÖLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ten1:1;             /* peri1_ten[1].
				 * PLLÊä³öÊ±ÖÓ²âÊÔÊ¹ÄÜ¡£
				 * ÖÃ1Ê±PLLÊä³öÊ±ÖÓÊä³öÖÁtblk¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd1:29;           /* - */
	} bits;
} reg_prcm_perh1_pll_ten_t;

#define PRCM_PERH1_PLL_TEN_TEN0_0       0x0
#define PRCM_PERH1_PLL_TEN_TEN0_1       0x1
#define PRCM_PERH1_PLL_TEN_TEN1_0       0x0
#define PRCM_PERH1_PLL_TEN_TEN1_1       0x1

/* PERH1_PLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 dlock:1;            /* PLL digital lock status.
				 * When PLL reaches stable, digital lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 asteady:1;          /* peri1_steady.
				 * PLL analog steady status.
				 * [1¡¯b0]: not steady
				 * [1¡¯b1]: steady
				 * Factor N±ä»¯ÎÈ¶¨ÐÅºÅ, ¿ÉÓÃÓÚÅÐ¶ÏÏßÐÔµ÷Æ
				 * µµ÷ÆµºÍÕûÊý·ÖÆµÖð½¥µ÷Æµ½áÊø */
	u32 alock:1;            /* peri1_lock.
				 * PLL analog lock status.
				 * When PLL reaches stable, analog lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 rsvd0:29;           /* - */
	} bits;
} reg_prcm_perh1_pll_stat_t;

#define PRCM_PERH1_PLL_STAT_DLOCK_0     0x0
#define PRCM_PERH1_PLL_STAT_DLOCK_1     0x1
#define PRCM_PERH1_PLL_STAT_ASTEADY_0   0x0
#define PRCM_PERH1_PLL_STAT_ASTEADY_1   0x1
#define PRCM_PERH1_PLL_STAT_ALOCK_0     0x0
#define PRCM_PERH1_PLL_STAT_ALOCK_1     0x1

/* PERH1_PLL Mode Register */
typedef union {
	u32 val;
	struct {
	u32 mod:2;              /* peri1_msel[1:0].
				 * PLL¹¤×÷Ä£Ê½Ñ¡Ôñ¡£
				 * [2¡¯b00]: ÕûÊý·ÖÆµ
				 * [2¡¯b01]: -
				 * [2¡¯b10]: Õ¹Æµ
				 * [2¡¯b11]: -
				 * PLLÖ»ÄÜ¹¤×÷ÔÚÆäÖÐÒ»ÖÖÄ£Ê½£¬¶ÔÏßÐÔµ÷Æµ£¬
				 * ¿ÉÔÚTUNE¿ØÖÆ¿ªÆôÏßÐÔµ÷Æµµþ¼ÓÕ¹Æµ¡£ */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_prcm_perh1_pll_mod_t;

#define PRCM_PERH1_PLL_MOD_MOD_0        0x0
#define PRCM_PERH1_PLL_MOD_MOD_1        0x1
#define PRCM_PERH1_PLL_MOD_MOD_2        0x2
#define PRCM_PERH1_PLL_MOD_MOD_3        0x3

/* PERH1_PLL N Factor Fraction Register */
typedef union {
	u32 val;
	struct {
	u32 nfrac:18;           /* peri1_nfrac[17:0].
				 * PLL nÒò×Ó·ÖÊý²¿·Ö¡£Ö»ÓÐÔÚÐ¡Êý·ÖÆµ¡¢Õ¹Æµ
				 * ÒÔ¼°ÏßÐÔµ÷ÆµÄ£Ê½ÏÂÓÐÐ§¡£
				 * ¹«Ê½£º
				 *  */
	u32 rsvd0:14;           /* - */
	} bits;
} reg_prcm_perh1_pll_nfrac_t;

/* PERH1_PLL Tune1 Register */
typedef union {
	u32 val;
	struct {
	u32 period:12;          /* peri1_tune[34:23].
				 * PLLÕ¹ÆµÖÜÆÚµ÷½Ú¡£
				 * ¹«Ê½:
				 * PLL²Î¿¼ÆµÂÊ / Õ¹ÆµÆµÂÊ / 2.
				 * ÀýÈç:
				 * Ðè30KHzµÄÕ¹ÆµÆµÂÊ£¬ÊäÈëµ½PLLµÄ²Î¿¼ÆµÂÊÎ
				 * ª24MHz£¬ÔòÖÜÆÚÎª:
				 * 24MHz / 30KHz / 2 = 400.
				 * ½öÓÃÓÚÕ¹ÆµºÍÏßÐÔµ÷Æµ. */
	u32 rsvd0:4;            /* - */
	u32 amplitude:12;       /* peri1_tune[46:35].
				 * PLLÕ¹Æµ·ù¶È²½³¤µ÷½Ú.
				 * ¹«Ê½:
				 * 
				 * ½öÓÃÓÚÕ¹ÆµºÍÏßÐÔµ÷Æµ. */
	u32 rsvd1:4;            /* - */
	} bits;
} reg_prcm_perh1_pll_tune1_t;

/* VIDEO_PLL Enable Register */
typedef union {
	u32 val;
	struct {
	u32 enp:1;              /* vid_enp&enc.
				 * PLLÊ¹ÄÜÐÅºÅ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÕý³£¹¤×÷Ðè10us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 enm:1;              /* vid_enm.
				 * PLLÆ«ÖÃµçÂ·Ê¹ÄÜ¡£
				 * ÏÈENM = 1ºóENP = 1.
				 * EnableÉúÐ§ºóµ½PLLÆ«ÖÃµçÂ·Õý³£¹¤×÷Ðè100us¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜPLLÊä³öÐèÏÈÊ¹ÄÜ ENM£¬ºóÊ¹ÄÜENP¡£ */
	u32 oen:1;              /* vid_oen.
				 * PLL Êä³ögating¡£
				 * OEN = 0Ê±PLL¸ßÆµÊ±ÖÓ²»Êä³ö£¬¼´peri_ckhs
				 * ºÍperi_cklsÊä³öÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ren:1;              /* vid_ren.
				 * PLLÊä³öÄ£ÄâÊ±ÖÓÊ¹ÄÜ¡£
				 * RGATÖÃ0Ê±peri_ckanaÎª0£¬²»Ó°ÏìPLLÕý³£¹¤×÷¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * For test. */
	u32 rsvd1:27;           /* - */
	} bits;
} reg_prcm_video_pll_en_t;

#define PRCM_VIDEO_PLL_EN_ENP_0         0x0
#define PRCM_VIDEO_PLL_EN_ENP_1         0x1
#define PRCM_VIDEO_PLL_EN_ENM_0         0x0
#define PRCM_VIDEO_PLL_EN_ENM_1         0x1
#define PRCM_VIDEO_PLL_EN_OEN_0         0x0
#define PRCM_VIDEO_PLL_EN_OEN_1         0x1
#define PRCM_VIDEO_PLL_EN_REN_0         0x0
#define PRCM_VIDEO_PLL_EN_REN_1         0x1

/* VIDEO_PLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 pre_div:2;          /* vid_prediv[1:0].
				 * PLL pre-divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4
				 * ·ÇÌØÊâÇé¿ö£¬PRE_DIVÓ¦±£³ÖÖÃ0. */
	u32 rsvd0:2;            /* - */
	u32 post_div:2;         /* vid_postdiv[1:0].
				 * PLL post divider factor.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /3
				 * [2¡¯b11]: /4 */
	u32 rsvd1:2;            /* - */
	u32 n:8;                /* vid_nint[7:0].
				 * PLL n factor.
				 * Valid value = N.
				 * Õý³£Ê¹ÓÃÊ±£¬valid value±ØÐë´óÓÚ10. ÇÒN=
				 * 0½ûÖ¹Ê¹ÓÃ¡£ */
	u32 rsvd2:16;           /* - */
	} bits;
} reg_prcm_video_pll_fac_t;

#define PRCM_VIDEO_PLL_FAC_PRE_DIV_0    0x0
#define PRCM_VIDEO_PLL_FAC_PRE_DIV_1    0x1
#define PRCM_VIDEO_PLL_FAC_PRE_DIV_2    0x2
#define PRCM_VIDEO_PLL_FAC_PRE_DIV_3    0x3
#define PRCM_VIDEO_PLL_FAC_POST_DIV_0   0x0
#define PRCM_VIDEO_PLL_FAC_POST_DIV_1   0x1
#define PRCM_VIDEO_PLL_FAC_POST_DIV_2   0x2
#define PRCM_VIDEO_PLL_FAC_POST_DIV_3   0x3

/* VIDEO_PLL Tune0 Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:3;            /* - */
	u32 tune1:3;            /* vid_tune[2:0].
				 * PLLÊý×ÖºÍÄ£ÄâLDOµçÑ¹µ÷½Ú¡£
				 * µçÑ¹¹ýµÍÓÐ¿ÉÄÜÓ°ÏìÊä³öÊ±ÖÓµÄÕ¼¿Õ±È¡£
				 * [3¡¯b000]: 0.95V
				 * [3¡¯b001]: 1.00V
				 * [3¡¯b010]: 1.05V
				 * [3¡¯b011]: 1.10V
				 * [3¡¯b100]: 1.15V
				 * [3¡¯b101]: 1.20V
				 * [3¡¯b110]: 1.25V
				 * [3¡¯b111]: 1.30V */
	u32 tune2:5;            /* vid_tune[7:3].
				 * PLLÄÚ²¿cpµçÁ÷µ÷½Ú¡£
				 * µçÁ÷Ô½´ó£¬PLL»·Â·´ø¿íÔ½´ó¡£
				 * I = (TUNE2 + 1) * 0.5uA. */
	u32 tune3:1;            /* vid_tune[8].
				 * PLLÆ«ÖÃÄ£Ê½¡£
				 * [1¡¯b0]: ÊÖ¶¯Æ«ÖÃÄ£Ê½
				 * [1¡¯b1]: ×ÔÆ«ÖÃÄ£Ê½
				 * Ñ¡Ôñ×ÔÆ«ÖÃÄ£Ê½£¬»·Â·´ø¿í²»Ëæn factor±ä»
				 * ¯¶ø±ä»¯¡£ */
	u32 tune4:3;            /* vid_tune[11:9].
				 * PLL VCOÔöÒæµ÷½Ú¡£
				 * [3¡¯b000]: 1.6GHz/V
				 * [3¡¯b001]: 1.8GHz/V
				 * [3¡¯b010]: 2.2GHz/V
				 * [3¡¯b011]: 2.6GHz/V
				 * [3¡¯b100]: 3.0GHz/V
				 * [3¡¯b101]: 3.4GHz/V
				 * [3¡¯b110]: 3.8GHz/V
				 * [3¡¯b111]: 4.2GHz/V
				 * ¸ù¾ÝPLL¹¤×÷ÆµÂÊµ÷½Ú¿ÉµÃµ½½ÏºÃÔëÉùÐÔÄÜ¡£ */
	u32 tune5:1;            /* vid_tune[12].
				 * PLL n factorÒì³£¼à²âÊ¹ÄÜ¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Ê¹ÄÜÊ±£¬Èôn³öÏÖÒì³£PLL×Ô¶¯´¦Àí£¬·ÀÖ¹PLL
				 * ½øÈëÒì³£¹¤×÷×´Ì¬¡£ */
	u32 tune6:1;            /* vid_tune[13].
				 * ckbkÂö¿íÔö¼Óµ÷½ÚÐÅºÅ.
				 * [1¡¯b0]: ²»Ôö¼Ó
				 * [1¡¯b1]: Ôö¼Ó */
	u32 tune7:1;            /* vid_tune[14].
				 * PLL¶¯Ì¬µ÷ÆµÊ±ÖÓÏàÎ»µ÷½Ú¡£
				 * [1¡¯b0]: ·´ÏàÊä³ö
				 * [1¡¯b1]: ²»·´ÏàÊä³ö
				 * ¾ö¶¨peri_cksdmÊÇ·ñ·´ÏàÊä³öÖÁCMU. */
	u32 tune8:1;            /* vid_tune[16].
				 * PLLÕûÊý·ÖÆµµ÷ÆµÄ£Ê½Ñ¡Ôñ¡£
				 * [1¡¯b0]: Ö±½ÓÌø±ä
				 * [1¡¯b1]: Öð½¥Ìø±ä */
	u32 tune9:3;            /* vid_tune[19:17].
				 * PLLÕûÊý·ÖÆµÖð½¥Ìø±äµ÷ÆµÄ£Ê½µÄÌø±äÐ±ÂÊ¿ØÖÆ¡£
				 * ¹«Ê½:
				 *  */
	u32 tune10:1;           /* vid_tune[20].
				 * LSB ditherÆô¶¯ÐÅºÅ¡£¸ÃÑ¡ÏîÖ÷ÒªÓÃÓÚ¸ÄÉÆÐ
				 * ¡Êý·ÖÆµ¡¢Õ¹Æµ¼°ÏßÐÔµ÷ÆµµÄÐÔÄÜ£¬¶ÔÕûÊý·Ö
				 * ÆµÃ»ÓÐ×÷ÓÃ¡£
				 * [1¡¯b0]: ²»Æô¶¯
				 * [1¡¯b1]: Æô¶¯ */
	u32 tune11:2;           /* vid_tune[22:21].
				 * PLL¶¯Ì¬µ÷Æµµ÷ÖÆ·½Ê½¡£
				 * [2¡¯b00]: Ò»½×¼ÓÍÌÂö³å
				 * [2¡¯b01]: Ò»½×¼Óµ÷½Ún
				 * [2¡¯b10]: MASH111µ÷ÖÆ
				 * [2¡¯b11]: ÕûÊý·ÖÆµ */
	u32 tune12:1;           /* vid_tune[15].
				 * Êý×ÖÄ£¿éÊ±ÖÓÔ´Ñ¡Ôñ.
				 * [1¡¯b0]: cksdm
				 * [1¡¯b1]: dck */
	u32 rsvd1:6;            /* - */
	} bits;
} reg_prcm_video_pll_tune0_t;

#define PRCM_VIDEO_PLL_TUNE0_TUNE1_0    0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE1_1    0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE1_2    0x2
#define PRCM_VIDEO_PLL_TUNE0_TUNE1_3    0x3
#define PRCM_VIDEO_PLL_TUNE0_TUNE1_4    0x4
#define PRCM_VIDEO_PLL_TUNE0_TUNE1_5    0x5
#define PRCM_VIDEO_PLL_TUNE0_TUNE1_6    0x6
#define PRCM_VIDEO_PLL_TUNE0_TUNE1_7    0x7
#define PRCM_VIDEO_PLL_TUNE0_TUNE3_0    0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE3_1    0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE4_0    0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE4_1    0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE4_2    0x2
#define PRCM_VIDEO_PLL_TUNE0_TUNE4_3    0x3
#define PRCM_VIDEO_PLL_TUNE0_TUNE4_4    0x4
#define PRCM_VIDEO_PLL_TUNE0_TUNE4_5    0x5
#define PRCM_VIDEO_PLL_TUNE0_TUNE4_6    0x6
#define PRCM_VIDEO_PLL_TUNE0_TUNE4_7    0x7
#define PRCM_VIDEO_PLL_TUNE0_TUNE5_0    0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE5_1    0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE6_0    0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE6_1    0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE7_0    0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE7_1    0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE8_0    0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE8_1    0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE10_0   0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE10_1   0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE11_0   0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE11_1   0x1
#define PRCM_VIDEO_PLL_TUNE0_TUNE11_2   0x2
#define PRCM_VIDEO_PLL_TUNE0_TUNE11_3   0x3
#define PRCM_VIDEO_PLL_TUNE0_TUNE12_0   0x0
#define PRCM_VIDEO_PLL_TUNE0_TUNE12_1   0x1

/* VIDEO_PLL Test Enable Register */
typedef union {
	u32 val;
	struct {
	u32 ten0:1;             /* vid_ten[0].
				 * PLLÊý×ÖLDOÄ£Äâ²âÊÔÊ¹ÄÜÐÅºÅ¡£
				 * ÖÃ1Ê±pad_anatestÉÏÊä³öPLLÊý×ÖLDOµçÑ¹£¬Ô
				 * ¼1.1V¡£
				 * PLLÕý³£¹¤×÷Ê±£¬±ØÐëdisable¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:1;            /* - */
	u32 ten1:1;             /* vid_ten[1].
				 * PLLÊä³öÊ±ÖÓ²âÊÔÊ¹ÄÜ¡£
				 * ÖÃ1Ê±PLLÊä³öÊ±ÖÓÊä³öÖÁtblk¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd1:29;           /* - */
	} bits;
} reg_prcm_video_pll_ten_t;

#define PRCM_VIDEO_PLL_TEN_TEN0_0       0x0
#define PRCM_VIDEO_PLL_TEN_TEN0_1       0x1
#define PRCM_VIDEO_PLL_TEN_TEN1_0       0x0
#define PRCM_VIDEO_PLL_TEN_TEN1_1       0x1

/* VIDEO_PLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 dlock:1;            /* PLL digital lock status.
				 * When PLL reaches stable, digital lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 asteady:1;          /* vid_steady.
				 * PLL analog steady status.
				 * [1¡¯b0]: not steady
				 * [1¡¯b1]: steady
				 * Factor N±ä»¯ÎÈ¶¨ÐÅºÅ, ¿ÉÓÃÓÚÅÐ¶ÏÏßÐÔµ÷Æ
				 * µµ÷ÆµºÍÕûÊý·ÖÆµÖð½¥µ÷Æµ½áÊø */
	u32 alock:1;            /* vid_lock.
				 * PLL analog lock status.
				 * When PLL reaches stable, analog lock
				 * bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 rsvd0:29;           /* - */
	} bits;
} reg_prcm_video_pll_stat_t;

#define PRCM_VIDEO_PLL_STAT_DLOCK_0     0x0
#define PRCM_VIDEO_PLL_STAT_DLOCK_1     0x1
#define PRCM_VIDEO_PLL_STAT_ASTEADY_0   0x0
#define PRCM_VIDEO_PLL_STAT_ASTEADY_1   0x1
#define PRCM_VIDEO_PLL_STAT_ALOCK_0     0x0
#define PRCM_VIDEO_PLL_STAT_ALOCK_1     0x1

/* VIDEO_PLL Mode Register */
typedef union {
	u32 val;
	struct {
	u32 mod:2;              /* vid_msel[1:0].
				 * PLL¹¤×÷Ä£Ê½Ñ¡Ôñ¡£
				 * [2¡¯b00]: ÕûÊý·ÖÆµ
				 * [2¡¯b01]: Ð¡Êý·ÖÆµ
				 * [2¡¯b10]: Õ¹Æµ
				 * [2¡¯b11]: -
				 * PLLÖ»ÄÜ¹¤×÷ÔÚÆäÖÐÒ»ÖÖÄ£Ê½£¬¶ÔÏßÐÔµ÷Æµ£¬
				 * ¿ÉÔÚTUNE¿ØÖÆ¿ªÆôÏßÐÔµ÷Æµµþ¼ÓÕ¹Æµ¡£ */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_prcm_video_pll_mod_t;

#define PRCM_VIDEO_PLL_MOD_MOD_0        0x0
#define PRCM_VIDEO_PLL_MOD_MOD_1        0x1
#define PRCM_VIDEO_PLL_MOD_MOD_2        0x2
#define PRCM_VIDEO_PLL_MOD_MOD_3        0x3

/* VIDEO_PLL N Factor Fraction Register */
typedef union {
	u32 val;
	struct {
	u32 nfrac:18;           /* vid_nfrac[17:0].
				 * PLL nÒò×Ó·ÖÊý²¿·Ö¡£Ö»ÓÐÔÚÐ¡Êý·ÖÆµ¡¢Õ¹Æµ
				 * ÒÔ¼°ÏßÐÔµ÷ÆµÄ£Ê½ÏÂÓÐÐ§¡£
				 * ¹«Ê½£º
				 *  */
	u32 rsvd0:14;           /* - */
	} bits;
} reg_prcm_video_pll_nfrac_t;

/* VIDEO_PLL Tune1 Register */
typedef union {
	u32 val;
	struct {
	u32 period:12;          /* vid_tune[34:23].
				 * PLLÕ¹ÆµÖÜÆÚµ÷½Ú¡£
				 * ¹«Ê½:
				 * PLL²Î¿¼ÆµÂÊ / Õ¹ÆµÆµÂÊ / 2.
				 * ÀýÈç:
				 * Ðè30KHzµÄÕ¹ÆµÆµÂÊ£¬ÊäÈëµ½PLLµÄ²Î¿¼ÆµÂÊÎ
				 * ª24MHz£¬ÔòÖÜÆÚÎª:
				 * 24MHz / 30KHz / 2 = 400.
				 * ½öÓÃÓÚÕ¹ÆµºÍÏßÐÔµ÷Æµ. */
	u32 rsvd0:4;            /* - */
	u32 amplitude:12;       /* vid_tune[46:35].
				 * PLLÕ¹Æµ·ù¶È²½³¤µ÷½Ú.
				 * ¹«Ê½:
				 * 
				 * ½öÓÃÓÚÕ¹ÆµºÍÏßÐÔµ÷Æµ. */
	u32 rsvd1:4;            /* - */
	} bits;
} reg_prcm_video_pll_tune1_t;

/* TP0_PLL Enable Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;                       /* PLLÆô¶¯ÐÅºÅ¡£¼´PLLÊ¹ÄÜ¡£
					 * [1¡¯b0]: enable
					 * [1¡¯b1]: disable */
	u32 gat:1;                      /* PLLÊä³öºó³ýÆµÊ±ÖÓgatingÐÅºÅ¡£
					 * [1¡¯b0]: Êä³öºó³ýÆµÊ±ÖÓ
					 * [1¡¯b1]: ²»Êä³öºó³ýÆµÊ±ÖÓ */
	u32 bypass:1;                   /* µ±sdram_bypass=1Ê±£¬sdram_foutp
					 * ostdiv»á±ä³Ésdram_fref.
					 * [1¡¯b0]: not bypass
					 * [1¡¯b1]: byass */
	u32 fout4phasepd:1;             /* fout4phasepd.
					 * PLLÊä³ö4ÏàÎ»Ê±ÖÓ¡£
					 * [1¡¯b0]: Êä³ö4ÏàÎ»Ê±ÖÓ
					 * [1¡¯b1]: ²»Êä³ö4ÏàÎ»Ê±ÖÓ */
	u32 foutvcopd:1;                /* sdram_pll foutvcopd.
					 * [1¡¯b0]: fan-out vco clock
					 * [1¡¯b1]: not fan-out vco clock */
	u32 rsvd0:27;                   /* - */
	} bits;
} reg_prcm_tp0_pll_en_t;

#define PRCM_TP0_PLL_EN_EN_0            0x0
#define PRCM_TP0_PLL_EN_EN_1            0x1
#define PRCM_TP0_PLL_EN_GAT_0           0x0
#define PRCM_TP0_PLL_EN_GAT_1           0x1
#define PRCM_TP0_PLL_EN_BYPASS_0        0x0
#define PRCM_TP0_PLL_EN_BYPASS_1        0x1
#define PRCM_TP0_PLL_EN_FOUT4PHASEPD_0  0x0
#define PRCM_TP0_PLL_EN_FOUT4PHASEPD_1  0x1
#define PRCM_TP0_PLL_EN_FOUTVCOPD_0     0x0
#define PRCM_TP0_PLL_EN_FOUTVCOPD_1     0x1

/* TP0_PLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 refdiv:6;           /* PLLÇ°³ýÆµÒò×Ó¡£
				 * Valid value = REFDIV.
				 * 0 is not valid for REFDIV. */
	u32 rsvd0:2;            /* - */
	u32 postdiv1:3;         /* PLLºó³ýÆµÒò×Ó1.
				 * Valid value = POSTDIV1.
				 * 0 is not valid for POSTDIV1. */
	u32 rsvd1:1;            /* - */
	u32 postdiv2:3;         /* PLLºó³ýÆµÒò×Ó2.
				 * Valid value = POSTDIV2.
				 * 0 is not valid for POSTDIV2. */
	u32 rsvd2:1;            /* - */
	u32 fbdiv:12;           /* PLL±¶ÆµÒò×Ó¡£
				 * Valid value = FBDIV.
				 * 0 is not valid for FBDIV. */
	u32 rsvd3:4;            /* - */
	} bits;
} reg_prcm_tp0_pll_fac_t;

/* TP0_PLL Fractional Register */
typedef union {
	u32 val;
	struct {
	u32 dsmpd:1;            /* µ±peri_dsmpd=0Ê±£¬pllÄÚ²¿delta sigmaµ÷Ö
				 * ÆÆô¶¯£¬¸Ã¹¦ÄÜÖ÷ÒªÓÃÓÚÐ¡Êý·ÖÆµµÈ¶¯Ì¬µ÷Æµ
				 * £¬ÕûÊýµ÷ÆµÎÞÐ§¡£
				 * [1¡¯b0]: enable dsm
				 * [1¡¯b1]: disable dsm */
	u32 dacpd:1;            /* µ±peri_dacpd=0Ê±£¬pllÄÚ²¿²¹³¥dacÆô¶¯£¬¸
				 * Ã¹¦ÄÜÖ÷ÒªÓÃÓÚÐ¡Êý·ÖÆµµÈ¶¯Ì¬µ÷Æµ£¬ÕûÊýµ÷
				 * ÆµÎÞÐ§¡£
				 * [1¡¯b0]: enable dac
				 * [1¡¯b1]: disable dac */
	u32 rsvd0:6;            /* - */
	u32 frac:24;            /* PLLÐ¡Êý·ÖÆµÒò×Ó¡£
				 * ¹«Ê½:
				 * fbdiv + (frac / 2^24). */
	} bits;
} reg_prcm_tp0_pll_frac_t;

#define PRCM_TP0_PLL_FRAC_DSMPD_0       0x0
#define PRCM_TP0_PLL_FRAC_DSMPD_1       0x1
#define PRCM_TP0_PLL_FRAC_DACPD_0       0x0
#define PRCM_TP0_PLL_FRAC_DACPD_1       0x1

/* TP0_PLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 lock:1;             /* PLL Lock status.
				 * When PLL reaches stable, lock bit is set.
				 * [1¡¯b0]: unlocked
				 * [1¡¯b1]: locked */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_prcm_tp0_pll_stat_t;

#define PRCM_TP0_PLL_STAT_LOCK_0        0x0
#define PRCM_TP0_PLL_STAT_LOCK_1        0x1

/* PLL Test Block Control Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;               /* tblk_en.
				 * Test block enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 div_endio:1;        /* div_endio.
				 * pad_cktestÊä³öÊ¹ÄÜÐÅºÅ¡£
				 * ¿ØÖÆÊä³ötest blockÊ±ÖÓtblk_cktd¡£
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	u32 rsvd0:2;            /* - */
	u32 sel0:4;             /* tblk_sel[3:0].
				 * Test block source select.
				 * [4¡¯b0000]: fref
				 * [4¡¯b0001]: aud_cktest
				 * [4¡¯b0010]: cpu_cktest
				 * [4¡¯b0011]: peri_cktest
				 * [4¡¯b0100]: peri1_cktest
				 * [4¡¯b0101]: sdram_cktest
				 * [4¡¯b0110]: vc_cktest
				 * [4¡¯b0111]: vid_cktest
				 * Others are reserved. */
	u32 sel1:4;             /* tblk_sel[7:4].
				 * Test block source select.
				 * [4¡¯b0000]: fref
				 * [4¡¯b0001]: aud_cktest
				 * [4¡¯b0010]: cpu_cktest
				 * [4¡¯b0011]: peri_cktest
				 * [4¡¯b0100]: peri1_cktest
				 * [4¡¯b0101]: sdram_cktest
				 * [4¡¯b0110]: vc_cktest
				 * [4¡¯b0111]: vid_cktest
				 * Others are reserved. */
	u32 rsvd1:4;            /* - */
	u32 div0:3;             /* tblk_div[2:0].
				 * Test block divider.
				 * [3¡¯b000]: /1
				 * [3¡¯b001]: /2
				 * [3¡¯b010]: /4
				 * [3¡¯b011]: /8
				 * [3¡¯b100]: /16
				 * [3¡¯b101]: /32
				 * [3¡¯b110]: /64
				 * [3¡¯b111]: /128 */
	u32 rsvd2:5;            /* - */
	u32 div1:2;             /* PLL LVDS output test divider.
				 * [2¡¯b00]: /1
				 * [2¡¯b01]: /2
				 * [2¡¯b10]: /4
				 * [2¡¯b11]: /8
				 * TP0PLL¾­¹ý´Ëdivider´ÓLVDS·Å³ö. */
	u32 rsvd3:6;            /* - */
	} bits;
} reg_prcm_pll_tblk_t;

#define PRCM_PLL_TBLK_EN_0              0x0
#define PRCM_PLL_TBLK_EN_1              0x1
#define PRCM_PLL_TBLK_DIV_ENDIO_0       0x0
#define PRCM_PLL_TBLK_DIV_ENDIO_1       0x1
#define PRCM_PLL_TBLK_SEL0_0            0x0
#define PRCM_PLL_TBLK_SEL0_1            0x1
#define PRCM_PLL_TBLK_SEL0_2            0x2
#define PRCM_PLL_TBLK_SEL0_3            0x3
#define PRCM_PLL_TBLK_SEL0_4            0x4
#define PRCM_PLL_TBLK_SEL0_5            0x5
#define PRCM_PLL_TBLK_SEL0_6            0x6
#define PRCM_PLL_TBLK_SEL0_7            0x7
#define PRCM_PLL_TBLK_SEL1_0            0x0
#define PRCM_PLL_TBLK_SEL1_1            0x1
#define PRCM_PLL_TBLK_SEL1_2            0x2
#define PRCM_PLL_TBLK_SEL1_3            0x3
#define PRCM_PLL_TBLK_SEL1_4            0x4
#define PRCM_PLL_TBLK_SEL1_5            0x5
#define PRCM_PLL_TBLK_SEL1_6            0x6
#define PRCM_PLL_TBLK_SEL1_7            0x7
#define PRCM_PLL_TBLK_DIV0_0            0x0
#define PRCM_PLL_TBLK_DIV0_1            0x1
#define PRCM_PLL_TBLK_DIV0_2            0x2
#define PRCM_PLL_TBLK_DIV0_3            0x3
#define PRCM_PLL_TBLK_DIV0_4            0x4
#define PRCM_PLL_TBLK_DIV0_5            0x5
#define PRCM_PLL_TBLK_DIV0_6            0x6
#define PRCM_PLL_TBLK_DIV0_7            0x7
#define PRCM_PLL_TBLK_DIV1_0            0x0
#define PRCM_PLL_TBLK_DIV1_1            0x1
#define PRCM_PLL_TBLK_DIV1_2            0x2
#define PRCM_PLL_TBLK_DIV1_3            0x3

/* CPU_AXI Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 cpu_src_sel:2;      /* CPU clock source select.
				 * [2¡¯b00]: HFEOSC
				 * [2¡¯b01]: RTCCLK
				 * [2¡¯b10]: CPU_PLL
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 cpu_src_div:2;      /* CPU clock source divider.
				 * CPU Clock = CPU_SRC_SEL / CPU_SRC_DIV.
				 * [2¡¯b00]: 1
				 * [2¡¯b01]: 2
				 * [2¡¯b10]: 4
				 * Others are reserved.
				 * µ±CPUÐèÇó240MÒÔÏÂÆµÂÊÊ±²Å»áÊ¹ÓÃCPU_SRC_DIV. */
	u32 rsvd1:10;           /* - */
	u32 axi_div:2;          /* AXI clock divider.
				 * AXI Clock = CPU_CLK / AXI_DIV.
				 * [2¡¯b00]: 1
				 * [2¡¯b01]: 2
				 * [2¡¯b10]: 3
				 * [2¡¯b11]: 4 */
	u32 rsvd2:14;           /* - */
	} bits;
} reg_prcm_cpu_axi_clk_ctrl_t;

#define PRCM_CPU_AXI_CLK_CTRL_CPU_SRC_SEL_0     0x0
#define PRCM_CPU_AXI_CLK_CTRL_CPU_SRC_SEL_1     0x1
#define PRCM_CPU_AXI_CLK_CTRL_CPU_SRC_SEL_2     0x2
#define PRCM_CPU_AXI_CLK_CTRL_CPU_SRC_DIV_0     0x0
#define PRCM_CPU_AXI_CLK_CTRL_CPU_SRC_DIV_1     0x1
#define PRCM_CPU_AXI_CLK_CTRL_CPU_SRC_DIV_2     0x2
#define PRCM_CPU_AXI_CLK_CTRL_AXI_DIV_0         0x0
#define PRCM_CPU_AXI_CLK_CTRL_AXI_DIV_1         0x1
#define PRCM_CPU_AXI_CLK_CTRL_AXI_DIV_2         0x2
#define PRCM_CPU_AXI_CLK_CTRL_AXI_DIV_3         0x3

/* AHB_APB Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* AHB Bus clock source select.
				 * [2¡¯b00]: HFEOSC
				 * [2¡¯b01]: RTCCLK
				 * [2¡¯b10]: PERH0_PLL_DIV2
				 * [2¡¯b11]: - */
	u32 rsvd0:2;            /* - */
	u32 ahb_div:2;          /* AHB Bus clock divider.
				 * AHB Clock = SRC_SEL/ AHB_DIV.
				 * APB Clock = AHB_CLK / hard-coded 2.
				 * Valid value = DIV + 1. */
	u32 rsvd1:26;           /* - */
	} bits;
} reg_prcm_ahb_apb_clk_ctrl_t;

#define PRCM_AHB_APB_CLK_CTRL_SRC_SEL_0 0x0
#define PRCM_AHB_APB_CLK_CTRL_SRC_SEL_1 0x1
#define PRCM_AHB_APB_CLK_CTRL_SRC_SEL_2 0x2
#define PRCM_AHB_APB_CLK_CTRL_SRC_SEL_3 0x3

/* MEMORY AXI BUS Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Bus clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: RTCCLK
				 * [2¡¯b11]: PERH0_PLL_VCO
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:2;              /* Bus clock divider.
				 * [2¡¯b00]: 1
				 * [2¡¯b01]: 2
				 * [2¡¯b10]: 3
				 * [2¡¯b11]: 4 */
	u32 rsvd1:25;           /* - */
	u32 en:1;               /* BUS clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_maxi_clk_ctrl_t;

#define PRCM_MAXI_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_MAXI_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_MAXI_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_MAXI_CLK_CTRL_SRC_SEL_3    0x3
#define PRCM_MAXI_CLK_CTRL_DIV_0        0x0
#define PRCM_MAXI_CLK_CTRL_DIV_1        0x1
#define PRCM_MAXI_CLK_CTRL_DIV_2        0x2
#define PRCM_MAXI_CLK_CTRL_DIV_3        0x3
#define PRCM_MAXI_CLK_CTRL_EN_0         0x0
#define PRCM_MAXI_CLK_CTRL_EN_1         0x1

/* SDRAM Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: SDRAM_PLL
				 * [2¡¯b10]: TP0_PLL
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:2;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * [2¡¯b00]: 1
				 * [2¡¯b01]: 2
				 * [2¡¯b10]: 4
				 * Others are reserved. */
	u32 rsvd1:25;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_sdram_clk_ctrl_t;

#define PRCM_SDRAM_CLK_CTRL_SRC_SEL_0   0x0
#define PRCM_SDRAM_CLK_CTRL_SRC_SEL_1   0x1
#define PRCM_SDRAM_CLK_CTRL_SRC_SEL_2   0x2
#define PRCM_SDRAM_CLK_CTRL_DIV_0       0x0
#define PRCM_SDRAM_CLK_CTRL_DIV_1       0x1
#define PRCM_SDRAM_CLK_CTRL_DIV_2       0x2
#define PRCM_SDRAM_CLK_CTRL_EN_0        0x0
#define PRCM_SDRAM_CLK_CTRL_EN_1        0x1

/* SDRAM Reset Control Register */
typedef union {
	u32 val;
	struct {
	u32 sdramc:1;           /* SDRAM Controller Reset
				 * [1¡¯b0]: asserted. reset.
				 * [1¡¯b1]: de-asserted. not reset. */
	u32 pub:1;              /* PUB Reset
				 * [1¡¯b0]: asserted. reset.
				 * [1¡¯b1]: de-asserted. not reset. */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_prcm_sdram_rst_t;

#define PRCM_SDRAM_RST_SDRAMC_0 0x0
#define PRCM_SDRAM_RST_SDRAMC_1 0x1
#define PRCM_SDRAM_RST_PUB_0    0x0
#define PRCM_SDRAM_RST_PUB_1    0x1

/* SDRAM Bandwidth Monitor Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select
				 * [2¡¯b00]: HFEOSC
				 * Others are reserved. */
	u32 rsvd0:29;           /* - */
	u32 en:1;               /* Clock Enable
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_sdram_bm_clk_ctrl_t;

#define PRCM_SDRAM_BM_CLK_CTRL_SRC_SEL_0        0x0
#define PRCM_SDRAM_BM_CLK_CTRL_EN_0             0x0
#define PRCM_SDRAM_BM_CLK_CTRL_EN_1             0x1

/* SDC0 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:3;          /* Clock source select.
				 * [3¡¯b000]: -
				 * [3¡¯b001]: HFEOSC
				 * [3¡¯b010]: PERH0_PLL_VCO
				 * [3¡¯b011]: PERH1_PLL
				 * Others are reserved. */
	u32 rsvd0:1;            /* - */
	u32 div0:4;             /* Clock source divider 0.
				 * Valid value = DIV0 + 1. */
	u32 div1:4;             /* Clock source divider 1.
				 * Valid value = DIV1 + 1.
				 * Clock = SRC_SEL / DIV0 / DIV1. */
	u32 rsvd1:19;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_sdc0_clk_ctrl_t;

#define PRCM_SDC0_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_SDC0_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_SDC0_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_SDC0_CLK_CTRL_SRC_SEL_3    0x3
#define PRCM_SDC0_CLK_CTRL_EN_0         0x0
#define PRCM_SDC0_CLK_CTRL_EN_1         0x1

/* SDC1 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:3;          /* Clock source select.
				 * [3¡¯b000]: -
				 * [3¡¯b001]: HFEOSC
				 * [3¡¯b010]: PERH0_PLL_VCO
				 * [3¡¯b011]: PERH1_PLL
				 * Others are reserved. */
	u32 rsvd0:1;            /* - */
	u32 div0:4;             /* Clock source divider 0.
				 * Valid value = DIV0 + 1. */
	u32 div1:4;             /* Clock source divider 1.
				 * Valid value = DIV1 + 1.
				 * Clock = SRC_SEL / DIV0 / DIV1. */
	u32 rsvd1:19;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_sdc1_clk_ctrl_t;

#define PRCM_SDC1_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_SDC1_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_SDC1_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_SDC1_CLK_CTRL_SRC_SEL_3    0x3
#define PRCM_SDC1_CLK_CTRL_EN_0         0x0
#define PRCM_SDC1_CLK_CTRL_EN_1         0x1

/* SPI0 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:3;          /* Clock source select.
				 * [3¡¯b000]: -
				 * [3¡¯b001]: HFEOSC
				 * [3¡¯b010]: PERH0_PLL_DIV2
				 * [3¡¯b011]: PERH1_PLL
				 * Others are reserved. */
	u32 rsvd0:1;            /* - */
	u32 div0:3;             /* Clock source divider 0.
				 * Valid value = DIV0 + 1. */
	u32 rsvd1:1;            /* - */
	u32 div1:4;             /* Clock source divider 1.
				 * Valid value = DIV1 + 1.
				 * Clock = SRC_SEL / DIV0 / DIV1. */
	u32 rsvd2:19;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * BOOT PIN SEL XIPÆô¶¯Ê±, ¸ÃbitÓ²¼þ×Ô¶¯ÖÃ1.
				 * ·ÇXIPÆô¶¯Ê±, Ä¬ÈÏ0. */
	} bits;
} reg_prcm_spi0_clk_ctrl_t;

#define PRCM_SPI0_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_SPI0_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_SPI0_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_SPI0_CLK_CTRL_SRC_SEL_3    0x3
#define PRCM_SPI0_CLK_CTRL_EN_0         0x0
#define PRCM_SPI0_CLK_CTRL_EN_1         0x1

/* SPI1 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:3;          /* Clock source select.
				 * [3¡¯b000]: -
				 * [3¡¯b001]: HFEOSC
				 * [3¡¯b010]: PERH0_PLL_DIV2
				 * [3¡¯b011]: PERH1_PLL
				 * Others are reserved. */
	u32 rsvd0:1;            /* - */
	u32 div0:3;             /* Clock source divider 0.
				 * Valid value = DIV0 + 1. */
	u32 rsvd1:1;            /* - */
	u32 div1:4;             /* Clock source divider 1.
				 * Valid value = DIV1 + 1.
				 * Clock = SRC_SEL / DIV0 / DIV1. */
	u32 rsvd2:19;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_spi1_clk_ctrl_t;

#define PRCM_SPI1_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_SPI1_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_SPI1_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_SPI1_CLK_CTRL_SRC_SEL_3    0x3
#define PRCM_SPI1_CLK_CTRL_EN_0         0x0
#define PRCM_SPI1_CLK_CTRL_EN_1         0x1

/* SPI2 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:3;          /* Clock source select.
				 * [3¡¯b000]: -
				 * [3¡¯b001]: HFEOSC
				 * [3¡¯b010]: PERH0_PLL_DIV2
				 * [3¡¯b011]: PERH1_PLL
				 * Others are reserved. */
	u32 rsvd0:1;            /* - */
	u32 div0:3;             /* Clock source divider 0.
				 * Valid value = DIV0 + 1. */
	u32 rsvd1:1;            /* - */
	u32 div1:4;             /* Clock source divider 1.
				 * Valid value = DIV1 + 1.
				 * Clock = SRC_SEL / DIV0 / DIV1. */
	u32 rsvd2:19;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_spi2_clk_ctrl_t;

#define PRCM_SPI2_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_SPI2_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_SPI2_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_SPI2_CLK_CTRL_SRC_SEL_3    0x3
#define PRCM_SPI2_CLK_CTRL_EN_0         0x0
#define PRCM_SPI2_CLK_CTRL_EN_1         0x1

/* VC Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:3;          /* Clock source select.
				 * [3¡¯b000]: -
				 * [3¡¯b001]: VC_PLL
				 * Others are reserved. */
	u32 rsvd0:1;            /* - */
	u32 div0:4;             /* Clock source divider 0.
				 * Valid value = DIV0 + 1. */
	u32 rsvd1:23;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_vc_clk_ctrl_t;

#define PRCM_VC_CLK_CTRL_SRC_SEL_0      0x0
#define PRCM_VC_CLK_CTRL_SRC_SEL_1      0x1
#define PRCM_VC_CLK_CTRL_EN_0           0x0
#define PRCM_VC_CLK_CTRL_EN_1           0x1

/* VISS Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 sclk0_en:1;         /* VISS sclk0 enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable.
				 * clock source is PERH_PLL_DIV2. */
	u32 rsvd0:7;            /* - */
	u32 sclk1_en:1;         /* VISS sclk1 enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable.
				 * clock source is VC_PLL. */
	u32 rsvd1:7;            /* - */
	u32 sclk2_en:1;         /* VISS sclk2 enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable.
				 * clock source is HFEOSC. */
	u32 rsvd2:15;           /* - */
	} bits;
} reg_prcm_viss_clk_ctrl_t;

#define PRCM_VISS_CLK_CTRL_SCLK0_EN_0   0x0
#define PRCM_VISS_CLK_CTRL_SCLK0_EN_1   0x1
#define PRCM_VISS_CLK_CTRL_SCLK1_EN_0   0x0
#define PRCM_VISS_CLK_CTRL_SCLK1_EN_1   0x1
#define PRCM_VISS_CLK_CTRL_SCLK2_EN_0   0x0
#define PRCM_VISS_CLK_CTRL_SCLK2_EN_1   0x1

/* DPU SCLK0 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: PERH0_PLL_DIV2
				 * [2¡¯b10]: VC_PLL
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:4;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:23;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_dpu_sclk0_clk_ctrl_t;

#define PRCM_DPU_SCLK0_CLK_CTRL_SRC_SEL_0       0x0
#define PRCM_DPU_SCLK0_CLK_CTRL_SRC_SEL_1       0x1
#define PRCM_DPU_SCLK0_CLK_CTRL_SRC_SEL_2       0x2
#define PRCM_DPU_SCLK0_CLK_CTRL_EN_0            0x0
#define PRCM_DPU_SCLK0_CLK_CTRL_EN_1            0x1

/* DPU SCLK1 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: VIDEO_PLL
				 * [2¡¯b10]: VC_PLL
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:4;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:23;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_dpu_sclk1_clk_ctrl_t;

#define PRCM_DPU_SCLK1_CLK_CTRL_SRC_SEL_0       0x0
#define PRCM_DPU_SCLK1_CLK_CTRL_SRC_SEL_1       0x1
#define PRCM_DPU_SCLK1_CLK_CTRL_SRC_SEL_2       0x2
#define PRCM_DPU_SCLK1_CLK_CTRL_EN_0            0x0
#define PRCM_DPU_SCLK1_CLK_CTRL_EN_1            0x1

/* DPU SCLK2 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:31;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable.
				 * clock source is HFEOSC. */
	} bits;
} reg_prcm_dpu_sclk2_clk_ctrl_t;

#define PRCM_DPU_SCLK2_CLK_CTRL_EN_0    0x0
#define PRCM_DPU_SCLK2_CLK_CTRL_EN_1    0x1

/* DOSS Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 hfeosc_en:1;        /* DOSS HFEOSC clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable.
				 * clock source is HFEOSC. */
	u32 rsvd0:7;            /* - */
	u32 sclk0_en:1;         /* DOSS sclk0 enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable.
				 * clock source is VIDEO_PLL. */
	u32 rsvd1:23;           /* - */
	} bits;
} reg_prcm_doss_clk_ctrl_t;

#define PRCM_DOSS_CLK_CTRL_HFEOSC_EN_0  0x0
#define PRCM_DOSS_CLK_CTRL_HFEOSC_EN_1  0x1
#define PRCM_DOSS_CLK_CTRL_SCLK0_EN_0   0x0
#define PRCM_DOSS_CLK_CTRL_SCLK0_EN_1   0x1

/* AX ENU Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: -
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_ax_enu_clk_ctrl_t;

#define PRCM_AX_ENU_CLK_CTRL_SRC_SEL_0  0x0
#define PRCM_AX_ENU_CLK_CTRL_SRC_SEL_1  0x1
#define PRCM_AX_ENU_CLK_CTRL_EN_0       0x0
#define PRCM_AX_ENU_CLK_CTRL_EN_1       0x1

/* AX CVU Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: -
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_ax_cvu_clk_ctrl_t;

#define PRCM_AX_CVU_CLK_CTRL_SRC_SEL_0  0x0
#define PRCM_AX_CVU_CLK_CTRL_SRC_SEL_1  0x1
#define PRCM_AX_CVU_CLK_CTRL_EN_0       0x0
#define PRCM_AX_CVU_CLK_CTRL_EN_1       0x1

/* I2S Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: AUDIO_PLL_DIVM
				 * [2¡¯b10]:AUDIO_PLL_DIV7(for 24.571M)
				 * [2¡¯b11]:AUDIO_PLL_DIV17(for 22.5882M)
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_i2s_clk_ctrl_t;

#define PRCM_I2S_CLK_CTRL_SRC_SEL_0     0x0
#define PRCM_I2S_CLK_CTRL_SRC_SEL_1     0x1
#define PRCM_I2S_CLK_CTRL_SRC_SEL_2     0x2
#define PRCM_I2S_CLK_CTRL_SRC_SEL_3     0x3
#define PRCM_I2S_CLK_CTRL_EN_0          0x0
#define PRCM_I2S_CLK_CTRL_EN_1          0x1

/* I2C0 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV8
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_i2c0_clk_ctrl_t;

#define PRCM_I2C0_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_I2C0_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_I2C0_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_I2C0_CLK_CTRL_EN_0         0x0
#define PRCM_I2C0_CLK_CTRL_EN_1         0x1

/* I2C1 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV8
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_i2c1_clk_ctrl_t;

#define PRCM_I2C1_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_I2C1_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_I2C1_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_I2C1_CLK_CTRL_EN_0         0x0
#define PRCM_I2C1_CLK_CTRL_EN_1         0x1

/* I2C2 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV8
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_i2c2_clk_ctrl_t;

#define PRCM_I2C2_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_I2C2_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_I2C2_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_I2C2_CLK_CTRL_EN_0         0x0
#define PRCM_I2C2_CLK_CTRL_EN_1         0x1

/* I2C3 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV8
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_i2c3_clk_ctrl_t;

#define PRCM_I2C3_CLK_CTRL_SRC_SEL_0    0x0
#define PRCM_I2C3_CLK_CTRL_SRC_SEL_1    0x1
#define PRCM_I2C3_CLK_CTRL_SRC_SEL_2    0x2
#define PRCM_I2C3_CLK_CTRL_EN_0         0x0
#define PRCM_I2C3_CLK_CTRL_EN_1         0x1

/* UART0 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV8
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_uart0_clk_ctrl_t;

#define PRCM_UART0_CLK_CTRL_SRC_SEL_0   0x0
#define PRCM_UART0_CLK_CTRL_SRC_SEL_1   0x1
#define PRCM_UART0_CLK_CTRL_SRC_SEL_2   0x2
#define PRCM_UART0_CLK_CTRL_EN_0        0x0
#define PRCM_UART0_CLK_CTRL_EN_1        0x1

/* UART1 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV8
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_uart1_clk_ctrl_t;

#define PRCM_UART1_CLK_CTRL_SRC_SEL_0   0x0
#define PRCM_UART1_CLK_CTRL_SRC_SEL_1   0x1
#define PRCM_UART1_CLK_CTRL_SRC_SEL_2   0x2
#define PRCM_UART1_CLK_CTRL_EN_0        0x0
#define PRCM_UART1_CLK_CTRL_EN_1        0x1

/* UART2 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV8
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_uart2_clk_ctrl_t;

#define PRCM_UART2_CLK_CTRL_SRC_SEL_0   0x0
#define PRCM_UART2_CLK_CTRL_SRC_SEL_1   0x1
#define PRCM_UART2_CLK_CTRL_SRC_SEL_2   0x2
#define PRCM_UART2_CLK_CTRL_EN_0        0x0
#define PRCM_UART2_CLK_CTRL_EN_1        0x1

/* UART3 Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV8
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:3;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:24;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_uart3_clk_ctrl_t;

#define PRCM_UART3_CLK_CTRL_SRC_SEL_0   0x0
#define PRCM_UART3_CLK_CTRL_SRC_SEL_1   0x1
#define PRCM_UART3_CLK_CTRL_SRC_SEL_2   0x2
#define PRCM_UART3_CLK_CTRL_EN_0        0x0
#define PRCM_UART3_CLK_CTRL_EN_1        0x1

/* USB PHY Reset Control Register */
typedef union {
	u32 val;
	struct {
	u32 rst:1;              /* USB PHY reset.
				 * [1¡¯b0]: asserted. reset.
				 * [1¡¯b1]: de-asserted. not reset. */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_prcm_usb_phy_rst_t;

#define PRCM_USB_PHY_RST_RST_0  0x0
#define PRCM_USB_PHY_RST_RST_1  0x1

/* Memory Bist Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 src_sel:2;          /* Clock source select.
				 * [2¡¯b00]: -
				 * [2¡¯b01]: HFEOSC
				 * [2¡¯b10]: PERH0_PLL_DIV2
				 * Others are reserved. */
	u32 rsvd0:2;            /* - */
	u32 div:4;              /* Clock source divider.
				 * Clock = SRC_SEL / DIV.
				 * Valid value = DIV + 1. */
	u32 rsvd1:23;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable */
	} bits;
} reg_prcm_mbist_clk_ctrl_t;

#define PRCM_MBIST_CLK_CTRL_SRC_SEL_0   0x0
#define PRCM_MBIST_CLK_CTRL_SRC_SEL_1   0x1
#define PRCM_MBIST_CLK_CTRL_SRC_SEL_2   0x2
#define PRCM_MBIST_CLK_CTRL_EN_0        0x0
#define PRCM_MBIST_CLK_CTRL_EN_1        0x1

/* AES Clock Control Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:31;           /* - */
	u32 en:1;               /* Clock enable.
				 * [1¡¯b0]: disable
				 * [1¡¯b1]: enable
				 * Clock source is HFEOSC. */
	} bits;
} reg_prcm_aes_clk_ctrl_t;

#define PRCM_AES_CLK_CTRL_EN_0          0x0
#define PRCM_AES_CLK_CTRL_EN_1          0x1

/* reg_prcm_t bank */
typedef struct tag_prcm {
	reg_prcm_ver_t                 ver;                /* 0000 */
	u32                            res0[127];
	reg_prcm_ahb_gat0_t            ahb_gat0;           /* 0200 */
	reg_prcm_ahb_gat1_t            ahb_gat1;           /* 0204 */
	u32                            res1[6];
	reg_prcm_ahb_rst0_t            ahb_rst0;           /* 0220 */
	reg_prcm_ahb_rst1_t            ahb_rst1;           /* 0224 */
	u32                            res2[22];
	reg_prcm_apb_gat0_t            apb_gat0;           /* 0280 */
	u32                            res3[7];
	reg_prcm_apb_rst0_t            apb_rst0;           /* 02A0 */
	u32                            res4[7];
	reg_prcm_apb_gat1_t            apb_gat1;           /* 02C0 */
	u32                            res5[7];
	reg_prcm_apb_rst1_t            apb_rst1;           /* 02E0 */
	u32                            res6[23];
	reg_prcm_axi_gat_t             axi_gat;            /* 0340 */
	u32                            res7[55];
	reg_prcm_hfeosc_ctrl_t         hfeosc_ctrl;        /* 0420 */
	reg_prcm_mbias_ctrl_t          mbias_ctrl;         /* 0424 */
	u32                            res8[14];
	reg_prcm_cpu_pll_en_t          cpu_pll_en;         /* 0460 */
	u32                            res9[1];
	reg_prcm_cpu_pll_fac_t         cpu_pll_fac;        /* 0468 */
	reg_prcm_cpu_pll_tune0_t       cpu_pll_tune0;      /* 046C */
	u32                            res10[1];
	reg_prcm_cpu_pll_ten_t         cpu_pll_ten;        /* 0474 */
	reg_prcm_cpu_pll_stat_t        cpu_pll_stat;       /* 0478 */
	reg_prcm_cpu_pll_mod_t         cpu_pll_mod;        /* 047C */
	reg_prcm_cpu_pll_nfrac_t       cpu_pll_nfrac;      /* 0480 */
	u32                            res11[2];
	reg_prcm_cpu_pll_tune2_t       cpu_pll_tune2;      /* 048C */
	u32                            res12[4];
	reg_prcm_sdram_pll_en_t        sdram_pll_en;       /* 04A0 */
	reg_prcm_sdram_pll_fac_t       sdram_pll_fac;      /* 04A4 */
	reg_prcm_sdram_pll_tune0_t     sdram_pll_tune0;    /* 04A8 */
	reg_prcm_sdram_pll_ten_t       sdram_pll_ten;      /* 04AC */
	reg_prcm_sdram_pll_stat_t      sdram_pll_stat;     /* 04B0 */
	reg_prcm_sdram_pll_mod_t       sdram_pll_mod;      /* 04B4 */
	reg_prcm_sdram_pll_nfrac_t     sdram_pll_nfrac;    /* 04B8 */
	reg_prcm_sdram_pll_tune1_t     sdram_pll_tune1;    /* 04BC */
	reg_prcm_sdram_pll_tune2_t     sdram_pll_tune2;    /* 04C0 */
	u32                            res13[7];
	reg_prcm_perh0_pll_en_t        perh0_pll_en;       /* 04E0 */
	reg_prcm_perh0_pll_fac_t       perh0_pll_fac;      /* 04E4 */
	reg_prcm_perh0_pll_tune0_t     perh0_pll_tune0;    /* 04E8 */
	reg_prcm_perh0_pll_ten_t       perh0_pll_ten;      /* 04EC */
	reg_prcm_perh0_pll_stat_t      perh0_pll_stat;     /* 04F0 */
	reg_prcm_perh0_pll_mod_t       perh0_pll_mod;      /* 04F4 */
	reg_prcm_perh0_pll_nfrac_t     perh0_pll_nfrac;    /* 04F8 */
	u32                            res14[9];
	reg_prcm_audio_pll_en_t        audio_pll_en;       /* 0520 */
	reg_prcm_audio_pll_fac_t       audio_pll_fac;      /* 0524 */
	reg_prcm_audio_pll_tune0_t     audio_pll_tune0;    /* 0528 */
	reg_prcm_audio_pll_ten_t       audio_pll_ten;      /* 052C */
	reg_prcm_audio_pll_stat_t      audio_pll_stat;     /* 0530 */
	reg_prcm_audio_pll_mod_t       audio_pll_mod;      /* 0534 */
	reg_prcm_audio_pll_nfrac_t     audio_pll_nfrac;    /* 0538 */
	u32                            res15[25];
	reg_prcm_vc_pll_en_t           vc_pll_en;          /* 05A0 */
	reg_prcm_vc_pll_fac_t          vc_pll_fac;         /* 05A4 */
	reg_prcm_vc_pll_tune0_t        vc_pll_tune0;       /* 05A8 */
	reg_prcm_vc_pll_ten_t          vc_pll_ten;         /* 05AC */
	reg_prcm_vc_pll_stat_t         vc_pll_stat;        /* 05B0 */
	reg_prcm_vc_pll_mod_t          vc_pll_mod;         /* 05B4 */
	reg_prcm_vc_pll_nfrac_t        vc_pll_nfrac;       /* 05B8 */
	u32                            res16[9];
	reg_prcm_perh1_pll_en_t        perh1_pll_en;       /* 05E0 */
	reg_prcm_perh1_pll_fac_t       perh1_pll_fac;      /* 05E4 */
	reg_prcm_perh1_pll_tune0_t     perh1_pll_tune0;    /* 05E8 */
	reg_prcm_perh1_pll_ten_t       perh1_pll_ten;      /* 05EC */
	reg_prcm_perh1_pll_stat_t      perh1_pll_stat;     /* 05F0 */
	reg_prcm_perh1_pll_mod_t       perh1_pll_mod;      /* 05F4 */
	reg_prcm_perh1_pll_nfrac_t     perh1_pll_nfrac;    /* 05F8 */
	reg_prcm_perh1_pll_tune1_t     perh1_pll_tune1;    /* 05FC */
	u32                            res17[8];
	reg_prcm_video_pll_en_t        video_pll_en;       /* 0620 */
	reg_prcm_video_pll_fac_t       video_pll_fac;      /* 0624 */
	reg_prcm_video_pll_tune0_t     video_pll_tune0;    /* 0628 */
	reg_prcm_video_pll_ten_t       video_pll_ten;      /* 062C */
	reg_prcm_video_pll_stat_t      video_pll_stat;     /* 0630 */
	reg_prcm_video_pll_mod_t       video_pll_mod;      /* 0634 */
	reg_prcm_video_pll_nfrac_t     video_pll_nfrac;    /* 0638 */
	reg_prcm_video_pll_tune1_t     video_pll_tune1;    /* 063C */
	u32                            res18[72];
	reg_prcm_tp0_pll_en_t          tp0_pll_en;         /* 0760 */
	reg_prcm_tp0_pll_fac_t         tp0_pll_fac;        /* 0764 */
	reg_prcm_tp0_pll_frac_t        tp0_pll_frac;       /* 0768 */
	reg_prcm_tp0_pll_stat_t        tp0_pll_stat;       /* 076C */
	u32                            res19[60];
	reg_prcm_pll_tblk_t            pll_tblk;           /* 0860 */
	u32                            res20[39];
	reg_prcm_cpu_axi_clk_ctrl_t    cpu_axi_clk_ctrl;   /* 0900 */
	reg_prcm_ahb_apb_clk_ctrl_t    ahb_apb_clk_ctrl;   /* 0904 */
	u32                            res21[6];
	reg_prcm_maxi_clk_ctrl_t       maxi_clk_ctrl;      /* 0920 */
	u32                            res22[7];
	reg_prcm_sdram_clk_ctrl_t      sdram_clk_ctrl;     /* 0940 */
	reg_prcm_sdram_rst_t           sdram_rst;          /* 0944 */
	reg_prcm_sdram_bm_clk_ctrl_t   sdram_bm_clk_ctrl;  /* 0948 */
	u32                            res23[5];
	reg_prcm_sdc0_clk_ctrl_t       sdc0_clk_ctrl;      /* 0960 */
	reg_prcm_sdc1_clk_ctrl_t       sdc1_clk_ctrl;      /* 0964 */
	u32                            res24[2];
	reg_prcm_spi0_clk_ctrl_t       spi0_clk_ctrl;      /* 0970 */
	reg_prcm_spi1_clk_ctrl_t       spi1_clk_ctrl;      /* 0974 */
	reg_prcm_spi2_clk_ctrl_t       spi2_clk_ctrl;      /* 0978 */
	u32                            res25[17];
	reg_prcm_vc_clk_ctrl_t         vc_clk_ctrl;        /* 09C0 */
	u32                            res26[3];
	reg_prcm_viss_clk_ctrl_t       viss_clk_ctrl;      /* 09D0 */
	u32                            res27[63];
	reg_prcm_dpu_sclk0_clk_ctrl_t  dpu_sclk0_clk_ctrl; /* 0AD0 */
	reg_prcm_dpu_sclk1_clk_ctrl_t  dpu_sclk1_clk_ctrl; /* 0AD4 */
	reg_prcm_dpu_sclk2_clk_ctrl_t  dpu_sclk2_clk_ctrl; /* 0AD8 */
	u32                            res28[1];
	reg_prcm_doss_clk_ctrl_t       doss_clk_ctrl;      /* 0AE0 */
	u32                            res29[59];
	reg_prcm_ax_enu_clk_ctrl_t     ax_enu_clk_ctrl;    /* 0BD0 */
	reg_prcm_ax_cvu_clk_ctrl_t     ax_cvu_clk_ctrl;    /* 0BD4 */
	u32                            res30[2];
	reg_prcm_i2s_clk_ctrl_t        i2s_clk_ctrl;       /* 0BE0 */
	u32                            res31[47];
	reg_prcm_i2c0_clk_ctrl_t       i2c0_clk_ctrl;      /* 0CA0 */
	reg_prcm_i2c1_clk_ctrl_t       i2c1_clk_ctrl;      /* 0CA4 */
	reg_prcm_i2c2_clk_ctrl_t       i2c2_clk_ctrl;      /* 0CA8 */
	reg_prcm_i2c3_clk_ctrl_t       i2c3_clk_ctrl;      /* 0CAC */
	u32                            res32[4];
	reg_prcm_uart0_clk_ctrl_t      uart0_clk_ctrl;     /* 0CC0 */
	reg_prcm_uart1_clk_ctrl_t      uart1_clk_ctrl;     /* 0CC4 */
	reg_prcm_uart2_clk_ctrl_t      uart2_clk_ctrl;     /* 0CC8 */
	reg_prcm_uart3_clk_ctrl_t      uart3_clk_ctrl;     /* 0CCC */
	u32                            res33[4];
	reg_prcm_usb_phy_rst_t         usb_phy_rst;        /* 0CE0 */
	u32                            res34[3];
	reg_prcm_mbist_clk_ctrl_t      mbist_clk_ctrl;     /* 0CF0 */
	u32                            res35[3];
	reg_prcm_aes_clk_ctrl_t        aes_clk_ctrl;       /* 0D00 */
} reg_prcm_t;

#endif /* ___PRCM___H___ */
