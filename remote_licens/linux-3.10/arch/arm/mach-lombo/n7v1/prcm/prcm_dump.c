/* prcm_dump.c */

#ifndef ___PRCM___DUMP__C___
#define ___PRCM___DUMP__C___

reg_info_t prcm_info[] = {
	{VA_PRCM_VER, DATA_PRCM_VER, dump_prcm_ver, "PRCM_VER", 0},
	{VA_PRCM_AHB_GAT0, DATA_PRCM_AHB_GAT0, dump_prcm_ahb_gat0, "PRCM_AHB_GAT0", 0},
	{VA_PRCM_AHB_GAT1, DATA_PRCM_AHB_GAT1, dump_prcm_ahb_gat1, "PRCM_AHB_GAT1", 0},
	{VA_PRCM_AHB_RST0, DATA_PRCM_AHB_RST0, dump_prcm_ahb_rst0, "PRCM_AHB_RST0", 0},
	{VA_PRCM_AHB_RST1, DATA_PRCM_AHB_RST1, dump_prcm_ahb_rst1, "PRCM_AHB_RST1", 0},
	{VA_PRCM_APB_GAT0, DATA_PRCM_APB_GAT0, dump_prcm_apb_gat0, "PRCM_APB_GAT0", 0},
	{VA_PRCM_APB_RST0, DATA_PRCM_APB_RST0, dump_prcm_apb_rst0, "PRCM_APB_RST0", 0},
	{VA_PRCM_APB_GAT1, DATA_PRCM_APB_GAT1, dump_prcm_apb_gat1, "PRCM_APB_GAT1", 0},
	{VA_PRCM_APB_RST1, DATA_PRCM_APB_RST1, dump_prcm_apb_rst1, "PRCM_APB_RST1", 0},
	{VA_PRCM_AXI_GAT, DATA_PRCM_AXI_GAT, dump_prcm_axi_gat, "PRCM_AXI_GAT", 0},
	{VA_PRCM_HFEOSC_CTRL, DATA_PRCM_HFEOSC_CTRL, dump_prcm_hfeosc_ctrl, "PRCM_HFEOSC_CTRL", 0},
	{VA_PRCM_MBIAS_CTRL, DATA_PRCM_MBIAS_CTRL, dump_prcm_mbias_ctrl, "PRCM_MBIAS_CTRL", 0},
	{VA_PRCM_PLL_DBG, DATA_PRCM_PLL_DBG, dump_prcm_pll_dbg, "PRCM_PLL_DBG", 0},
	{VA_PRCM_CPU_PLL_EN, DATA_PRCM_CPU_PLL_EN, dump_prcm_cpu_pll_en, "PRCM_CPU_PLL_EN", 0},
	{VA_PRCM_CPU_PLL_FAC, DATA_PRCM_CPU_PLL_FAC, dump_prcm_cpu_pll_fac, "PRCM_CPU_PLL_FAC", 0},
	{VA_PRCM_CPU_PLL_TUNE0, DATA_PRCM_CPU_PLL_TUNE0, dump_prcm_cpu_pll_tune0, "PRCM_CPU_PLL_TUNE0", 0},
	{VA_PRCM_CPU_PLL_TEN, DATA_PRCM_CPU_PLL_TEN, dump_prcm_cpu_pll_ten, "PRCM_CPU_PLL_TEN", 0},
	{VA_PRCM_CPU_PLL_STAT, DATA_PRCM_CPU_PLL_STAT, dump_prcm_cpu_pll_stat, "PRCM_CPU_PLL_STAT", 0},
	{VA_PRCM_CPU_PLL_MOD, DATA_PRCM_CPU_PLL_MOD, dump_prcm_cpu_pll_mod, "PRCM_CPU_PLL_MOD", 0},
	{VA_PRCM_CPU_PLL_NFRAC, DATA_PRCM_CPU_PLL_NFRAC, dump_prcm_cpu_pll_nfrac, "PRCM_CPU_PLL_NFRAC", 0},
	{VA_PRCM_CPU_PLL_TUNE2, DATA_PRCM_CPU_PLL_TUNE2, dump_prcm_cpu_pll_tune2, "PRCM_CPU_PLL_TUNE2", 0},
	{VA_PRCM_SDRAM_PLL_EN, DATA_PRCM_SDRAM_PLL_EN, dump_prcm_sdram_pll_en, "PRCM_SDRAM_PLL_EN", 0},
	{VA_PRCM_SDRAM_PLL_FAC, DATA_PRCM_SDRAM_PLL_FAC, dump_prcm_sdram_pll_fac, "PRCM_SDRAM_PLL_FAC", 0},
	{VA_PRCM_SDRAM_PLL_TUNE0, DATA_PRCM_SDRAM_PLL_TUNE0, dump_prcm_sdram_pll_tune0, "PRCM_SDRAM_PLL_TUNE0", 0},
	{VA_PRCM_SDRAM_PLL_TEN, DATA_PRCM_SDRAM_PLL_TEN, dump_prcm_sdram_pll_ten, "PRCM_SDRAM_PLL_TEN", 0},
	{VA_PRCM_SDRAM_PLL_STAT, DATA_PRCM_SDRAM_PLL_STAT, dump_prcm_sdram_pll_stat, "PRCM_SDRAM_PLL_STAT", 0},
	{VA_PRCM_SDRAM_PLL_MOD, DATA_PRCM_SDRAM_PLL_MOD, dump_prcm_sdram_pll_mod, "PRCM_SDRAM_PLL_MOD", 0},
	{VA_PRCM_SDRAM_PLL_NFRAC, DATA_PRCM_SDRAM_PLL_NFRAC, dump_prcm_sdram_pll_nfrac, "PRCM_SDRAM_PLL_NFRAC", 0},
	{VA_PRCM_SDRAM_PLL_TUNE1, DATA_PRCM_SDRAM_PLL_TUNE1, dump_prcm_sdram_pll_tune1, "PRCM_SDRAM_PLL_TUNE1", 0},
	{VA_PRCM_SDRAM_PLL_TUNE2, DATA_PRCM_SDRAM_PLL_TUNE2, dump_prcm_sdram_pll_tune2, "PRCM_SDRAM_PLL_TUNE2", 0},
	{VA_PRCM_PERH0_PLL_EN, DATA_PRCM_PERH0_PLL_EN, dump_prcm_perh0_pll_en, "PRCM_PERH0_PLL_EN", 0},
	{VA_PRCM_PERH0_PLL_FAC, DATA_PRCM_PERH0_PLL_FAC, dump_prcm_perh0_pll_fac, "PRCM_PERH0_PLL_FAC", 0},
	{VA_PRCM_PERH0_PLL_TUNE0, DATA_PRCM_PERH0_PLL_TUNE0, dump_prcm_perh0_pll_tune0, "PRCM_PERH0_PLL_TUNE0", 0},
	{VA_PRCM_PERH0_PLL_TEN, DATA_PRCM_PERH0_PLL_TEN, dump_prcm_perh0_pll_ten, "PRCM_PERH0_PLL_TEN", 0},
	{VA_PRCM_PERH0_PLL_STAT, DATA_PRCM_PERH0_PLL_STAT, dump_prcm_perh0_pll_stat, "PRCM_PERH0_PLL_STAT", 0},
	{VA_PRCM_PERH0_PLL_MOD, DATA_PRCM_PERH0_PLL_MOD, dump_prcm_perh0_pll_mod, "PRCM_PERH0_PLL_MOD", 0},
	{VA_PRCM_PERH0_PLL_NFRAC, DATA_PRCM_PERH0_PLL_NFRAC, dump_prcm_perh0_pll_nfrac, "PRCM_PERH0_PLL_NFRAC", 0},
	{VA_PRCM_AUDIO_PLL_EN, DATA_PRCM_AUDIO_PLL_EN, dump_prcm_audio_pll_en, "PRCM_AUDIO_PLL_EN", 0},
	{VA_PRCM_AUDIO_PLL_FAC, DATA_PRCM_AUDIO_PLL_FAC, dump_prcm_audio_pll_fac, "PRCM_AUDIO_PLL_FAC", 0},
	{VA_PRCM_AUDIO_PLL_TUNE0, DATA_PRCM_AUDIO_PLL_TUNE0, dump_prcm_audio_pll_tune0, "PRCM_AUDIO_PLL_TUNE0", 0},
	{VA_PRCM_AUDIO_PLL_TEN, DATA_PRCM_AUDIO_PLL_TEN, dump_prcm_audio_pll_ten, "PRCM_AUDIO_PLL_TEN", 0},
	{VA_PRCM_AUDIO_PLL_STAT, DATA_PRCM_AUDIO_PLL_STAT, dump_prcm_audio_pll_stat, "PRCM_AUDIO_PLL_STAT", 0},
	{VA_PRCM_AUDIO_PLL_MOD, DATA_PRCM_AUDIO_PLL_MOD, dump_prcm_audio_pll_mod, "PRCM_AUDIO_PLL_MOD", 0},
	{VA_PRCM_AUDIO_PLL_NFRAC, DATA_PRCM_AUDIO_PLL_NFRAC, dump_prcm_audio_pll_nfrac, "PRCM_AUDIO_PLL_NFRAC", 0},
	{VA_PRCM_VC_PLL_EN, DATA_PRCM_VC_PLL_EN, dump_prcm_vc_pll_en, "PRCM_VC_PLL_EN", 0},
	{VA_PRCM_VC_PLL_FAC, DATA_PRCM_VC_PLL_FAC, dump_prcm_vc_pll_fac, "PRCM_VC_PLL_FAC", 0},
	{VA_PRCM_VC_PLL_TUNE0, DATA_PRCM_VC_PLL_TUNE0, dump_prcm_vc_pll_tune0, "PRCM_VC_PLL_TUNE0", 0},
	{VA_PRCM_VC_PLL_TEN, DATA_PRCM_VC_PLL_TEN, dump_prcm_vc_pll_ten, "PRCM_VC_PLL_TEN", 0},
	{VA_PRCM_VC_PLL_STAT, DATA_PRCM_VC_PLL_STAT, dump_prcm_vc_pll_stat, "PRCM_VC_PLL_STAT", 0},
	{VA_PRCM_VC_PLL_MOD, DATA_PRCM_VC_PLL_MOD, dump_prcm_vc_pll_mod, "PRCM_VC_PLL_MOD", 0},
	{VA_PRCM_VC_PLL_NFRAC, DATA_PRCM_VC_PLL_NFRAC, dump_prcm_vc_pll_nfrac, "PRCM_VC_PLL_NFRAC", 0},
	{VA_PRCM_PERH1_PLL_EN, DATA_PRCM_PERH1_PLL_EN, dump_prcm_perh1_pll_en, "PRCM_PERH1_PLL_EN", 0},
	{VA_PRCM_PERH1_PLL_FAC, DATA_PRCM_PERH1_PLL_FAC, dump_prcm_perh1_pll_fac, "PRCM_PERH1_PLL_FAC", 0},
	{VA_PRCM_PERH1_PLL_TUNE0, DATA_PRCM_PERH1_PLL_TUNE0, dump_prcm_perh1_pll_tune0, "PRCM_PERH1_PLL_TUNE0", 0},
	{VA_PRCM_PERH1_PLL_TEN, DATA_PRCM_PERH1_PLL_TEN, dump_prcm_perh1_pll_ten, "PRCM_PERH1_PLL_TEN", 0},
	{VA_PRCM_PERH1_PLL_STAT, DATA_PRCM_PERH1_PLL_STAT, dump_prcm_perh1_pll_stat, "PRCM_PERH1_PLL_STAT", 0},
	{VA_PRCM_PERH1_PLL_MOD, DATA_PRCM_PERH1_PLL_MOD, dump_prcm_perh1_pll_mod, "PRCM_PERH1_PLL_MOD", 0},
	{VA_PRCM_PERH1_PLL_NFRAC, DATA_PRCM_PERH1_PLL_NFRAC, dump_prcm_perh1_pll_nfrac, "PRCM_PERH1_PLL_NFRAC", 0},
	{VA_PRCM_PERH1_PLL_TUNE1, DATA_PRCM_PERH1_PLL_TUNE1, dump_prcm_perh1_pll_tune1, "PRCM_PERH1_PLL_TUNE1", 0},
	{VA_PRCM_DISP_PLL_EN, DATA_PRCM_DISP_PLL_EN, dump_prcm_disp_pll_en, "PRCM_DISP_PLL_EN", 0},
	{VA_PRCM_DISP_PLL_FAC, DATA_PRCM_DISP_PLL_FAC, dump_prcm_disp_pll_fac, "PRCM_DISP_PLL_FAC", 0},
	{VA_PRCM_DISP_PLL_TUNE0, DATA_PRCM_DISP_PLL_TUNE0, dump_prcm_disp_pll_tune0, "PRCM_DISP_PLL_TUNE0", 0},
	{VA_PRCM_DISP_PLL_TEN, DATA_PRCM_DISP_PLL_TEN, dump_prcm_disp_pll_ten, "PRCM_DISP_PLL_TEN", 0},
	{VA_PRCM_DISP_PLL_STAT, DATA_PRCM_DISP_PLL_STAT, dump_prcm_disp_pll_stat, "PRCM_DISP_PLL_STAT", 0},
	{VA_PRCM_DISP_PLL_MOD, DATA_PRCM_DISP_PLL_MOD, dump_prcm_disp_pll_mod, "PRCM_DISP_PLL_MOD", 0},
	{VA_PRCM_DISP_PLL_NFRAC, DATA_PRCM_DISP_PLL_NFRAC, dump_prcm_disp_pll_nfrac, "PRCM_DISP_PLL_NFRAC", 0},
	{VA_PRCM_DISP_PLL_TUNE1, DATA_PRCM_DISP_PLL_TUNE1, dump_prcm_disp_pll_tune1, "PRCM_DISP_PLL_TUNE1", 0},
	{VA_PRCM_AX_PLL_EN, DATA_PRCM_AX_PLL_EN, dump_prcm_ax_pll_en, "PRCM_AX_PLL_EN", 0},
	{VA_PRCM_AX_PLL_FAC, DATA_PRCM_AX_PLL_FAC, dump_prcm_ax_pll_fac, "PRCM_AX_PLL_FAC", 0},
	{VA_PRCM_AX_PLL_TUNE0, DATA_PRCM_AX_PLL_TUNE0, dump_prcm_ax_pll_tune0, "PRCM_AX_PLL_TUNE0", 0},
	{VA_PRCM_AX_PLL_TEN, DATA_PRCM_AX_PLL_TEN, dump_prcm_ax_pll_ten, "PRCM_AX_PLL_TEN", 0},
	{VA_PRCM_AX_PLL_STAT, DATA_PRCM_AX_PLL_STAT, dump_prcm_ax_pll_stat, "PRCM_AX_PLL_STAT", 0},
	{VA_PRCM_AX_PLL_MOD, DATA_PRCM_AX_PLL_MOD, dump_prcm_ax_pll_mod, "PRCM_AX_PLL_MOD", 0},
	{VA_PRCM_AX_PLL_NFRAC, DATA_PRCM_AX_PLL_NFRAC, dump_prcm_ax_pll_nfrac, "PRCM_AX_PLL_NFRAC", 0},
	{VA_PRCM_AX_PLL_TUNE2, DATA_PRCM_AX_PLL_TUNE2, dump_prcm_ax_pll_tune2, "PRCM_AX_PLL_TUNE2", 0},
	{VA_PRCM_PERH2_PLL_EN, DATA_PRCM_PERH2_PLL_EN, dump_prcm_perh2_pll_en, "PRCM_PERH2_PLL_EN", 0},
	{VA_PRCM_PERH2_PLL_FAC, DATA_PRCM_PERH2_PLL_FAC, dump_prcm_perh2_pll_fac, "PRCM_PERH2_PLL_FAC", 0},
	{VA_PRCM_PERH2_PLL_TUNE0, DATA_PRCM_PERH2_PLL_TUNE0, dump_prcm_perh2_pll_tune0, "PRCM_PERH2_PLL_TUNE0", 0},
	{VA_PRCM_PERH2_PLL_TEN, DATA_PRCM_PERH2_PLL_TEN, dump_prcm_perh2_pll_ten, "PRCM_PERH2_PLL_TEN", 0},
	{VA_PRCM_PERH2_PLL_STAT, DATA_PRCM_PERH2_PLL_STAT, dump_prcm_perh2_pll_stat, "PRCM_PERH2_PLL_STAT", 0},
	{VA_PRCM_PERH2_PLL_MOD, DATA_PRCM_PERH2_PLL_MOD, dump_prcm_perh2_pll_mod, "PRCM_PERH2_PLL_MOD", 0},
	{VA_PRCM_PERH2_PLL_NFRAC, DATA_PRCM_PERH2_PLL_NFRAC, dump_prcm_perh2_pll_nfrac, "PRCM_PERH2_PLL_NFRAC", 0},
	{VA_PRCM_PERH2_PLL_TUNE1, DATA_PRCM_PERH2_PLL_TUNE1, dump_prcm_perh2_pll_tune1, "PRCM_PERH2_PLL_TUNE1", 0},
	{VA_PRCM_PLL_TBLK, DATA_PRCM_PLL_TBLK, dump_prcm_pll_tblk, "PRCM_PLL_TBLK", 0},
	{VA_PRCM_CPU_AXI_CLK_CTRL, DATA_PRCM_CPU_AXI_CLK_CTRL, dump_prcm_cpu_axi_clk_ctrl, "PRCM_CPU_AXI_CLK_CTRL", 0},
	{VA_PRCM_AHB_APB_CLK_CTRL, DATA_PRCM_AHB_APB_CLK_CTRL, dump_prcm_ahb_apb_clk_ctrl, "PRCM_AHB_APB_CLK_CTRL", 0},
	{VA_PRCM_MAXI_CLK_CTRL, DATA_PRCM_MAXI_CLK_CTRL, dump_prcm_maxi_clk_ctrl, "PRCM_MAXI_CLK_CTRL", 0},
	{VA_PRCM_SDRAM_CLK_CTRL, DATA_PRCM_SDRAM_CLK_CTRL, dump_prcm_sdram_clk_ctrl, "PRCM_SDRAM_CLK_CTRL", 0},
	{VA_PRCM_SDRAM_RST, DATA_PRCM_SDRAM_RST, dump_prcm_sdram_rst, "PRCM_SDRAM_RST", 0},
	{VA_PRCM_SDRAM_BM_CLK_CTRL, DATA_PRCM_SDRAM_BM_CLK_CTRL, dump_prcm_sdram_bm_clk_ctrl, "PRCM_SDRAM_BM_CLK_CTRL", 0},
	{VA_PRCM_SDC0_CLK_CTRL, DATA_PRCM_SDC0_CLK_CTRL, dump_prcm_sdc0_clk_ctrl, "PRCM_SDC0_CLK_CTRL", 0},
	{VA_PRCM_SDC1_CLK_CTRL, DATA_PRCM_SDC1_CLK_CTRL, dump_prcm_sdc1_clk_ctrl, "PRCM_SDC1_CLK_CTRL", 0},
	{VA_PRCM_SDC2_CLK_CTRL, DATA_PRCM_SDC2_CLK_CTRL, dump_prcm_sdc2_clk_ctrl, "PRCM_SDC2_CLK_CTRL", 0},
	{VA_PRCM_SPI0_CLK_CTRL, DATA_PRCM_SPI0_CLK_CTRL, dump_prcm_spi0_clk_ctrl, "PRCM_SPI0_CLK_CTRL", 0},
	{VA_PRCM_SPI1_CLK_CTRL, DATA_PRCM_SPI1_CLK_CTRL, dump_prcm_spi1_clk_ctrl, "PRCM_SPI1_CLK_CTRL", 0},
	{VA_PRCM_SPI2_CLK_CTRL, DATA_PRCM_SPI2_CLK_CTRL, dump_prcm_spi2_clk_ctrl, "PRCM_SPI2_CLK_CTRL", 0},
	{VA_PRCM_VC_CLK_CTRL, DATA_PRCM_VC_CLK_CTRL, dump_prcm_vc_clk_ctrl, "PRCM_VC_CLK_CTRL", 0},
	{VA_PRCM_VDC_CLK_CTRL, DATA_PRCM_VDC_CLK_CTRL, dump_prcm_vdc_clk_ctrl, "PRCM_VDC_CLK_CTRL", 0},
	{VA_PRCM_VISS_CLK_CTRL, DATA_PRCM_VISS_CLK_CTRL, dump_prcm_viss_clk_ctrl, "PRCM_VISS_CLK_CTRL", 0},
	{VA_PRCM_DPU_SCLK0_CLK_CTRL, DATA_PRCM_DPU_SCLK0_CLK_CTRL, dump_prcm_dpu_sclk0_clk_ctrl, "PRCM_DPU_SCLK0_CLK_CTRL", 0},
	{VA_PRCM_DPU_SCLK1_CLK_CTRL, DATA_PRCM_DPU_SCLK1_CLK_CTRL, dump_prcm_dpu_sclk1_clk_ctrl, "PRCM_DPU_SCLK1_CLK_CTRL", 0},
	{VA_PRCM_DPU_SCLK2_CLK_CTRL, DATA_PRCM_DPU_SCLK2_CLK_CTRL, dump_prcm_dpu_sclk2_clk_ctrl, "PRCM_DPU_SCLK2_CLK_CTRL", 0},
	{VA_PRCM_DOSS_CLK_CTRL, DATA_PRCM_DOSS_CLK_CTRL, dump_prcm_doss_clk_ctrl, "PRCM_DOSS_CLK_CTRL", 0},
	{VA_PRCM_AX_CLK_CTRL, DATA_PRCM_AX_CLK_CTRL, dump_prcm_ax_clk_ctrl, "PRCM_AX_CLK_CTRL", 0},
	{VA_PRCM_I2S0_CLK_CTRL, DATA_PRCM_I2S0_CLK_CTRL, dump_prcm_i2s0_clk_ctrl, "PRCM_I2S0_CLK_CTRL", 0},
	{VA_PRCM_I2S1_CLK_CTRL, DATA_PRCM_I2S1_CLK_CTRL, dump_prcm_i2s1_clk_ctrl, "PRCM_I2S1_CLK_CTRL", 0},
	{VA_PRCM_I2C0_CLK_CTRL, DATA_PRCM_I2C0_CLK_CTRL, dump_prcm_i2c0_clk_ctrl, "PRCM_I2C0_CLK_CTRL", 0},
	{VA_PRCM_I2C1_CLK_CTRL, DATA_PRCM_I2C1_CLK_CTRL, dump_prcm_i2c1_clk_ctrl, "PRCM_I2C1_CLK_CTRL", 0},
	{VA_PRCM_I2C2_CLK_CTRL, DATA_PRCM_I2C2_CLK_CTRL, dump_prcm_i2c2_clk_ctrl, "PRCM_I2C2_CLK_CTRL", 0},
	{VA_PRCM_I2C3_CLK_CTRL, DATA_PRCM_I2C3_CLK_CTRL, dump_prcm_i2c3_clk_ctrl, "PRCM_I2C3_CLK_CTRL", 0},
	{VA_PRCM_UART0_CLK_CTRL, DATA_PRCM_UART0_CLK_CTRL, dump_prcm_uart0_clk_ctrl, "PRCM_UART0_CLK_CTRL", 0},
	{VA_PRCM_UART1_CLK_CTRL, DATA_PRCM_UART1_CLK_CTRL, dump_prcm_uart1_clk_ctrl, "PRCM_UART1_CLK_CTRL", 0},
	{VA_PRCM_UART2_CLK_CTRL, DATA_PRCM_UART2_CLK_CTRL, dump_prcm_uart2_clk_ctrl, "PRCM_UART2_CLK_CTRL", 0},
	{VA_PRCM_UART3_CLK_CTRL, DATA_PRCM_UART3_CLK_CTRL, dump_prcm_uart3_clk_ctrl, "PRCM_UART3_CLK_CTRL", 0},
	{VA_PRCM_USB_PHY_RST, DATA_PRCM_USB_PHY_RST, dump_prcm_usb_phy_rst, "PRCM_USB_PHY_RST", 0},
	{VA_PRCM_GPADC_CLK_CTRL, DATA_PRCM_GPADC_CLK_CTRL, dump_prcm_gpadc_clk_ctrl, "PRCM_GPADC_CLK_CTRL", 0},
	{VA_PRCM_IR_CLK_CTRL, DATA_PRCM_IR_CLK_CTRL, dump_prcm_ir_clk_ctrl, "PRCM_IR_CLK_CTRL", 0},
	{VA_PRCM_GMAC_CLK_CTRL, DATA_PRCM_GMAC_CLK_CTRL, dump_prcm_gmac_clk_ctrl, "PRCM_GMAC_CLK_CTRL", 0},
	{VA_PRCM_MBIST_CLK_CTRL, DATA_PRCM_MBIST_CLK_CTRL, dump_prcm_mbist_clk_ctrl, "PRCM_MBIST_CLK_CTRL", 0},
	{VA_PRCM_AES_CLK_CTRL, DATA_PRCM_AES_CLK_CTRL, dump_prcm_aes_clk_ctrl, "PRCM_AES_CLK_CTRL", 0},
};

void dump_prcm(u32 mode)
{
	u32 cnt = sizeof(prcm_info) / sizeof(prcm_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					prcm_info[i].name,
					prcm_info[i].addr,
					prcm_info[i].reset);
		def_strcat(buffer, sz_temp);

		prcm_info[i].dump(prcm_info[i].addr, prcm_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_prcm_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ver_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ver_l\n", reg.bits.ver_l);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ver_h\n", reg.bits.ver_h);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:comp\n", reg.bits.comp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ahb_gat0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ahb_gat0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dma\n", reg.bits.dma);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ptimer\n", reg.bits.ptimer);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:aes\n", reg.bits.aes);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdram\n", reg.bits.sdram);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdc0\n", reg.bits.sdc0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdc1\n", reg.bits.sdc1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdc2\n", reg.bits.sdc2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:spi0\n", reg.bits.spi0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:spi1\n", reg.bits.spi1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:spi2\n", reg.bits.spi2);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ahb_gat1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ahb_gat1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:viss\n", reg.bits.viss);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dpu\n", reg.bits.dpu);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:doss\n", reg.bits.doss);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vdc\n", reg.bits.vdc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vc\n", reg.bits.vc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ax\n", reg.bits.ax);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:usb\n", reg.bits.usb);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gmac\n", reg.bits.gmac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ahb_rst0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ahb_rst0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dma\n", reg.bits.dma);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ptimer\n", reg.bits.ptimer);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:aes\n", reg.bits.aes);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdram\n", reg.bits.sdram);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdc0\n", reg.bits.sdc0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdc1\n", reg.bits.sdc1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdc2\n", reg.bits.sdc2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:spi0\n", reg.bits.spi0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:spi1\n", reg.bits.spi1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:spi2\n", reg.bits.spi2);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ahb_rst1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ahb_rst1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:viss\n", reg.bits.viss);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dpu\n", reg.bits.dpu);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:doss\n", reg.bits.doss);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vdc\n", reg.bits.vdc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vc\n", reg.bits.vc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ax\n", reg.bits.ax);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:usb\n", reg.bits.usb);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gmac\n", reg.bits.gmac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_apb_gat0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_apb_gat0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:i2c0\n", reg.bits.i2c0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:i2c1\n", reg.bits.i2c1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:i2c2\n", reg.bits.i2c2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:i2c3\n", reg.bits.i2c3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:uart0\n", reg.bits.uart0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:uart1\n", reg.bits.uart1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:uart2\n", reg.bits.uart2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:uart3\n", reg.bits.uart3);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_apb_rst0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_apb_rst0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:i2c0\n", reg.bits.i2c0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:i2c1\n", reg.bits.i2c1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:i2c2\n", reg.bits.i2c2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:i2c3\n", reg.bits.i2c3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:uart0\n", reg.bits.uart0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:uart1\n", reg.bits.uart1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:uart2\n", reg.bits.uart2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:uart3\n", reg.bits.uart3);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_apb_gat1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_apb_gat1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:i2s0\n", reg.bits.i2s0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:i2s1\n", reg.bits.i2s1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpadc\n", reg.bits.gpadc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir\n", reg.bits.ir);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpio\n", reg.bits.gpio);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_apb_rst1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_apb_rst1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:i2s0\n", reg.bits.i2s0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:i2s1\n", reg.bits.i2s1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpadc\n", reg.bits.gpadc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir\n", reg.bits.ir);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpio\n", reg.bits.gpio);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_axi_gat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_axi_gat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dma\n", reg.bits.dma);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vc\n", reg.bits.vc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vdc\n", reg.bits.vdc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:viss0\n", reg.bits.viss0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dpu\n", reg.bits.dpu);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:doss\n", reg.bits.doss);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ax\n", reg.bits.ax);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gmac\n", reg.bits.gmac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_hfeosc_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_hfeosc_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:xtal_start\n", reg.bits.xtal_start);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune\n", reg.bits.tune);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel_dbg\n", reg.bits.src_sel_dbg);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_mbias_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_mbias_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tst_en0\n", reg.bits.tst_en0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tst_en1\n", reg.bits.tst_en1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tst_en2\n", reg.bits.tst_en2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tst_en3\n", reg.bits.tst_en3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2_0\n", reg.bits.tune2_0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_pll_dbg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_pll_dbg_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock_en\n", reg.bits.dlock_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lock_fac_det_en\n", reg.bits.lock_fac_det_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune11\n", reg.bits.tune11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune12\n", reg.bits.tune12);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_pll_tune2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_pll_tune2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:step\n", reg.bits.step);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune11\n", reg.bits.tune11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune12\n", reg.bits.tune12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune13\n", reg.bits.tune13);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten2\n", reg.bits.ten2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten3\n", reg.bits.ten3);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_tune1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_tune1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:period\n", reg.bits.period);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:amplitude\n", reg.bits.amplitude);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_pll_tune2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_pll_tune2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:step\n", reg.bits.step);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh0_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh0_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh0_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh0_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh0_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh0_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh0_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh0_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten3\n", reg.bits.ten3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten2\n", reg.bits.ten2);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh0_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh0_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh0_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh0_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh0_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh0_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_audio_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_audio_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div_endiv17\n", reg.bits.div_endiv17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div_endiv7\n", reg.bits.div_endiv7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_audio_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_audio_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_audio_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_audio_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune11\n", reg.bits.tune11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune12\n", reg.bits.tune12);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_audio_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_audio_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_audio_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_audio_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_audio_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_audio_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_audio_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_audio_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vc_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vc_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vc_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vc_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vc_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vc_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vc_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vc_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vc_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vc_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vc_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vc_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vc_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vc_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh1_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh1_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh1_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh1_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh1_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh1_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune11\n", reg.bits.tune11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune12\n", reg.bits.tune12);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh1_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh1_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh1_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh1_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh1_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh1_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh1_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh1_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh1_pll_tune1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh1_pll_tune1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:period\n", reg.bits.period);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:amplitude\n", reg.bits.amplitude);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_disp_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_disp_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_disp_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_disp_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_disp_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_disp_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune11\n", reg.bits.tune11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune12\n", reg.bits.tune12);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_disp_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_disp_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_disp_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_disp_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_disp_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_disp_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_disp_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_disp_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_disp_pll_tune1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_disp_pll_tune1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:period\n", reg.bits.period);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:amplitude\n", reg.bits.amplitude);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune11\n", reg.bits.tune11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune12\n", reg.bits.tune12);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_pll_tune2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_pll_tune2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:step\n", reg.bits.step);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh2_pll_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh2_pll_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enp\n", reg.bits.enp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enm\n", reg.bits.enm);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:oen\n", reg.bits.oen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh2_pll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh2_pll_fac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:post_div\n", reg.bits.post_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh2_pll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh2_pll_tune0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune8\n", reg.bits.tune8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune9\n", reg.bits.tune9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune10\n", reg.bits.tune10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune11\n", reg.bits.tune11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune12\n", reg.bits.tune12);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh2_pll_ten(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh2_pll_ten_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ten0\n", reg.bits.ten0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ten1\n", reg.bits.ten1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh2_pll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh2_pll_stat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:dlock\n", reg.bits.dlock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:asteady\n", reg.bits.asteady);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:alock\n", reg.bits.alock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh2_pll_mod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh2_pll_mod_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh2_pll_nfrac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh2_pll_nfrac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:nfrac\n", reg.bits.nfrac);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_perh2_pll_tune1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_perh2_pll_tune1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:period\n", reg.bits.period);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:amplitude\n", reg.bits.amplitude);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_pll_tblk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_pll_tblk_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div_endio\n", reg.bits.div_endio);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sel0\n", reg.bits.sel0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sel1\n", reg.bits.sel1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_cpu_axi_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_cpu_axi_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cpu_src_sel\n", reg.bits.cpu_src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpu_src_div\n", reg.bits.cpu_src_div);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ahb_apb_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ahb_apb_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ahb_div\n", reg.bits.ahb_div);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_maxi_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_maxi_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_rst(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_rst_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:sdramc\n", reg.bits.sdramc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pub\n", reg.bits.pub);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:x4clk_rst\n", reg.bits.x4clk_rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdram_bm_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdram_bm_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdc0_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdc0_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div1\n", reg.bits.div1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdc1_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdc1_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div1\n", reg.bits.div1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_sdc2_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_sdc2_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div1\n", reg.bits.div1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_spi0_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_spi0_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div1\n", reg.bits.div1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_spi1_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_spi1_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div1\n", reg.bits.div1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_spi2_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_spi2_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div1\n", reg.bits.div1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vc_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vc_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_vdc_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_vdc_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_viss_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_viss_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:sclk0_en\n", reg.bits.sclk0_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sclk1_en\n", reg.bits.sclk1_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sclk2_en\n", reg.bits.sclk2_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_dpu_sclk0_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_dpu_sclk0_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_dpu_sclk1_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_dpu_sclk1_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_dpu_sclk2_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_dpu_sclk2_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_doss_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_doss_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:hfeosc_en\n", reg.bits.hfeosc_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sclk0_en\n", reg.bits.sclk0_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sclk0_src\n", reg.bits.sclk0_src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sclk0_div\n", reg.bits.sclk0_div);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ax_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ax_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_i2s0_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_i2s0_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_i2s1_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_i2s1_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_i2c0_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_i2c0_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_i2c1_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_i2c1_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_i2c2_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_i2c2_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_i2c3_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_i2c3_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_uart0_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_uart0_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_uart1_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_uart1_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_uart2_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_uart2_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_uart3_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_uart3_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_usb_phy_rst(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_usb_phy_rst_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_gpadc_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_gpadc_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_ir_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_ir_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_gmac_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_gmac_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:core_src_sel\n", reg.bits.core_src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:core_div\n", reg.bits.core_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:core_en\n", reg.bits.core_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sclk_src_sel\n", reg.bits.sclk_src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sclk1_en\n", reg.bits.sclk1_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sclk0_en\n", reg.bits.sclk0_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_mbist_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_mbist_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_prcm_aes_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_prcm_aes_clk_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___PRCM___DUMP__C___ */
