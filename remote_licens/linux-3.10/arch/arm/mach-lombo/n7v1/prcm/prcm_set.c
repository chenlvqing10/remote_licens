/* prcm_set.c */

#ifndef ___PRCM__SET___C___
#define ___PRCM__SET___C___

#ifdef DEF_SET_PRCM_VER
void set_prcm_ver(u32 reg_addr,
		u32 ver_l,
		u32 ver_h,
		u32 comp,
		u32 m_or_r)
{
	reg_prcm_ver_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ver_l = ver_l;
	reg.bits.ver_h = ver_h;
	reg.bits.comp = comp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VER_VER_L
fun_set_mod_reg_bit(prcm, ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VER_VER_H
fun_set_mod_reg_bit(prcm, ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VER_COMP
fun_set_mod_reg_bit(prcm, ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0
void set_prcm_ahb_gat0(u32 reg_addr,
			u32 dma,
			u32 ptimer,
			u32 aes,
			u32 sdram,
			u32 sdc0,
			u32 sdc1,
			u32 sdc2,
			u32 spi0,
			u32 spi1,
			u32 spi2,
			u32 m_or_r)
{
	reg_prcm_ahb_gat0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dma = dma;
	reg.bits.ptimer = ptimer;
	reg.bits.aes = aes;
	reg.bits.sdram = sdram;
	reg.bits.sdc0 = sdc0;
	reg.bits.sdc1 = sdc1;
	reg.bits.sdc2 = sdc2;
	reg.bits.spi0 = spi0;
	reg.bits.spi1 = spi1;
	reg.bits.spi2 = spi2;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_DMA
fun_set_mod_reg_bit(prcm, ahb_gat0, dma, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_PTIMER
fun_set_mod_reg_bit(prcm, ahb_gat0, ptimer, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_AES
fun_set_mod_reg_bit(prcm, ahb_gat0, aes, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_SDRAM
fun_set_mod_reg_bit(prcm, ahb_gat0, sdram, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_SDC0
fun_set_mod_reg_bit(prcm, ahb_gat0, sdc0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_SDC1
fun_set_mod_reg_bit(prcm, ahb_gat0, sdc1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_SDC2
fun_set_mod_reg_bit(prcm, ahb_gat0, sdc2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_SPI0
fun_set_mod_reg_bit(prcm, ahb_gat0, spi0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_SPI1
fun_set_mod_reg_bit(prcm, ahb_gat0, spi1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT0_SPI2
fun_set_mod_reg_bit(prcm, ahb_gat0, spi2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1
void set_prcm_ahb_gat1(u32 reg_addr,
			u32 viss,
			u32 dpu,
			u32 doss,
			u32 vdc,
			u32 vc,
			u32 ax,
			u32 usb,
			u32 gmac,
			u32 m_or_r)
{
	reg_prcm_ahb_gat1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.viss = viss;
	reg.bits.dpu = dpu;
	reg.bits.doss = doss;
	reg.bits.vdc = vdc;
	reg.bits.vc = vc;
	reg.bits.ax = ax;
	reg.bits.usb = usb;
	reg.bits.gmac = gmac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1_VISS
fun_set_mod_reg_bit(prcm, ahb_gat1, viss, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1_DPU
fun_set_mod_reg_bit(prcm, ahb_gat1, dpu, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1_DOSS
fun_set_mod_reg_bit(prcm, ahb_gat1, doss, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1_VDC
fun_set_mod_reg_bit(prcm, ahb_gat1, vdc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1_VC
fun_set_mod_reg_bit(prcm, ahb_gat1, vc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1_AX
fun_set_mod_reg_bit(prcm, ahb_gat1, ax, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1_USB
fun_set_mod_reg_bit(prcm, ahb_gat1, usb, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_GAT1_GMAC
fun_set_mod_reg_bit(prcm, ahb_gat1, gmac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0
void set_prcm_ahb_rst0(u32 reg_addr,
			u32 dma,
			u32 ptimer,
			u32 aes,
			u32 sdram,
			u32 sdc0,
			u32 sdc1,
			u32 sdc2,
			u32 spi0,
			u32 spi1,
			u32 spi2,
			u32 m_or_r)
{
	reg_prcm_ahb_rst0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dma = dma;
	reg.bits.ptimer = ptimer;
	reg.bits.aes = aes;
	reg.bits.sdram = sdram;
	reg.bits.sdc0 = sdc0;
	reg.bits.sdc1 = sdc1;
	reg.bits.sdc2 = sdc2;
	reg.bits.spi0 = spi0;
	reg.bits.spi1 = spi1;
	reg.bits.spi2 = spi2;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_DMA
fun_set_mod_reg_bit(prcm, ahb_rst0, dma, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_PTIMER
fun_set_mod_reg_bit(prcm, ahb_rst0, ptimer, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_AES
fun_set_mod_reg_bit(prcm, ahb_rst0, aes, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_SDRAM
fun_set_mod_reg_bit(prcm, ahb_rst0, sdram, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_SDC0
fun_set_mod_reg_bit(prcm, ahb_rst0, sdc0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_SDC1
fun_set_mod_reg_bit(prcm, ahb_rst0, sdc1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_SDC2
fun_set_mod_reg_bit(prcm, ahb_rst0, sdc2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_SPI0
fun_set_mod_reg_bit(prcm, ahb_rst0, spi0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_SPI1
fun_set_mod_reg_bit(prcm, ahb_rst0, spi1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST0_SPI2
fun_set_mod_reg_bit(prcm, ahb_rst0, spi2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST1
void set_prcm_ahb_rst1(u32 reg_addr,
			u32 viss,
			u32 dpu,
			u32 doss,
			u32 vdc,
			u32 vc,
			u32 ax,
			u32 usb,
			u32 gmac,
			u32 m_or_r)
{
	reg_prcm_ahb_rst1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.viss = viss;
	reg.bits.dpu = dpu;
	reg.bits.doss = doss;
	reg.bits.vdc = vdc;
	reg.bits.vc = vc;
	reg.bits.ax = ax;
	reg.bits.usb = usb;
	reg.bits.gmac = gmac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AHB_RST1_VISS
fun_set_mod_reg_bit(prcm, ahb_rst1, viss, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST1_DPU
fun_set_mod_reg_bit(prcm, ahb_rst1, dpu, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST1_DOSS
fun_set_mod_reg_bit(prcm, ahb_rst1, doss, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST1_VDC
fun_set_mod_reg_bit(prcm, ahb_rst1, vdc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST1_VC
fun_set_mod_reg_bit(prcm, ahb_rst1, vc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST1_AX
fun_set_mod_reg_bit(prcm, ahb_rst1, ax, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST1_USB
fun_set_mod_reg_bit(prcm, ahb_rst1, usb, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_RST1_GMAC
fun_set_mod_reg_bit(prcm, ahb_rst1, gmac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT0
void set_prcm_apb_gat0(u32 reg_addr,
			u32 i2c0,
			u32 i2c1,
			u32 i2c2,
			u32 i2c3,
			u32 uart0,
			u32 uart1,
			u32 uart2,
			u32 uart3,
			u32 m_or_r)
{
	reg_prcm_apb_gat0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.i2c0 = i2c0;
	reg.bits.i2c1 = i2c1;
	reg.bits.i2c2 = i2c2;
	reg.bits.i2c3 = i2c3;
	reg.bits.uart0 = uart0;
	reg.bits.uart1 = uart1;
	reg.bits.uart2 = uart2;
	reg.bits.uart3 = uart3;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_APB_GAT0_I2C0
fun_set_mod_reg_bit(prcm, apb_gat0, i2c0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT0_I2C1
fun_set_mod_reg_bit(prcm, apb_gat0, i2c1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT0_I2C2
fun_set_mod_reg_bit(prcm, apb_gat0, i2c2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT0_I2C3
fun_set_mod_reg_bit(prcm, apb_gat0, i2c3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT0_UART0
fun_set_mod_reg_bit(prcm, apb_gat0, uart0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT0_UART1
fun_set_mod_reg_bit(prcm, apb_gat0, uart1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT0_UART2
fun_set_mod_reg_bit(prcm, apb_gat0, uart2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT0_UART3
fun_set_mod_reg_bit(prcm, apb_gat0, uart3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST0
void set_prcm_apb_rst0(u32 reg_addr,
			u32 i2c0,
			u32 i2c1,
			u32 i2c2,
			u32 i2c3,
			u32 uart0,
			u32 uart1,
			u32 uart2,
			u32 uart3,
			u32 m_or_r)
{
	reg_prcm_apb_rst0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.i2c0 = i2c0;
	reg.bits.i2c1 = i2c1;
	reg.bits.i2c2 = i2c2;
	reg.bits.i2c3 = i2c3;
	reg.bits.uart0 = uart0;
	reg.bits.uart1 = uart1;
	reg.bits.uart2 = uart2;
	reg.bits.uart3 = uart3;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_APB_RST0_I2C0
fun_set_mod_reg_bit(prcm, apb_rst0, i2c0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST0_I2C1
fun_set_mod_reg_bit(prcm, apb_rst0, i2c1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST0_I2C2
fun_set_mod_reg_bit(prcm, apb_rst0, i2c2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST0_I2C3
fun_set_mod_reg_bit(prcm, apb_rst0, i2c3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST0_UART0
fun_set_mod_reg_bit(prcm, apb_rst0, uart0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST0_UART1
fun_set_mod_reg_bit(prcm, apb_rst0, uart1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST0_UART2
fun_set_mod_reg_bit(prcm, apb_rst0, uart2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST0_UART3
fun_set_mod_reg_bit(prcm, apb_rst0, uart3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT1
void set_prcm_apb_gat1(u32 reg_addr,
			u32 i2s0,
			u32 i2s1,
			u32 gpadc,
			u32 ir,
			u32 gpio,
			u32 m_or_r)
{
	reg_prcm_apb_gat1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.i2s0 = i2s0;
	reg.bits.i2s1 = i2s1;
	reg.bits.gpadc = gpadc;
	reg.bits.ir = ir;
	reg.bits.gpio = gpio;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_APB_GAT1_I2S0
fun_set_mod_reg_bit(prcm, apb_gat1, i2s0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT1_I2S1
fun_set_mod_reg_bit(prcm, apb_gat1, i2s1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT1_GPADC
fun_set_mod_reg_bit(prcm, apb_gat1, gpadc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT1_IR
fun_set_mod_reg_bit(prcm, apb_gat1, ir, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_GAT1_GPIO
fun_set_mod_reg_bit(prcm, apb_gat1, gpio, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST1
void set_prcm_apb_rst1(u32 reg_addr,
			u32 i2s0,
			u32 i2s1,
			u32 gpadc,
			u32 ir,
			u32 gpio,
			u32 m_or_r)
{
	reg_prcm_apb_rst1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.i2s0 = i2s0;
	reg.bits.i2s1 = i2s1;
	reg.bits.gpadc = gpadc;
	reg.bits.ir = ir;
	reg.bits.gpio = gpio;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_APB_RST1_I2S0
fun_set_mod_reg_bit(prcm, apb_rst1, i2s0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST1_I2S1
fun_set_mod_reg_bit(prcm, apb_rst1, i2s1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST1_GPADC
fun_set_mod_reg_bit(prcm, apb_rst1, gpadc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST1_IR
fun_set_mod_reg_bit(prcm, apb_rst1, ir, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_APB_RST1_GPIO
fun_set_mod_reg_bit(prcm, apb_rst1, gpio, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AXI_GAT
void set_prcm_axi_gat(u32 reg_addr,
			u32 dma,
			u32 vc,
			u32 vdc,
			u32 viss0,
			u32 dpu,
			u32 doss,
			u32 ax,
			u32 gmac,
			u32 m_or_r)
{
	reg_prcm_axi_gat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dma = dma;
	reg.bits.vc = vc;
	reg.bits.vdc = vdc;
	reg.bits.viss0 = viss0;
	reg.bits.dpu = dpu;
	reg.bits.doss = doss;
	reg.bits.ax = ax;
	reg.bits.gmac = gmac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AXI_GAT_DMA
fun_set_mod_reg_bit(prcm, axi_gat, dma, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AXI_GAT_VC
fun_set_mod_reg_bit(prcm, axi_gat, vc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AXI_GAT_VDC
fun_set_mod_reg_bit(prcm, axi_gat, vdc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AXI_GAT_VISS0
fun_set_mod_reg_bit(prcm, axi_gat, viss0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AXI_GAT_DPU
fun_set_mod_reg_bit(prcm, axi_gat, dpu, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AXI_GAT_DOSS
fun_set_mod_reg_bit(prcm, axi_gat, doss, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AXI_GAT_AX
fun_set_mod_reg_bit(prcm, axi_gat, ax, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AXI_GAT_GMAC
fun_set_mod_reg_bit(prcm, axi_gat, gmac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_HFEOSC_CTRL
void set_prcm_hfeosc_ctrl(u32 reg_addr,
			u32 en,
			u32 src_sel,
			u32 xtal_start,
			u32 tune,
			u32 src_sel_dbg,
			u32 m_or_r)
{
	reg_prcm_hfeosc_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.src_sel = src_sel;
	reg.bits.xtal_start = xtal_start;
	reg.bits.tune = tune;
	reg.bits.src_sel_dbg = src_sel_dbg;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_HFEOSC_CTRL_EN
fun_set_mod_reg_bit(prcm, hfeosc_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_HFEOSC_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, hfeosc_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_HFEOSC_CTRL_XTAL_START
fun_set_mod_reg_bit(prcm, hfeosc_ctrl, xtal_start, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_HFEOSC_CTRL_TUNE
fun_set_mod_reg_bit(prcm, hfeosc_ctrl, tune, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_HFEOSC_CTRL_SRC_SEL_DBG
fun_set_mod_reg_bit(prcm, hfeosc_ctrl, src_sel_dbg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL
void set_prcm_mbias_ctrl(u32 reg_addr,
			u32 en,
			u32 tst_en0,
			u32 tst_en1,
			u32 tst_en2,
			u32 tst_en3,
			u32 tune2_0,
			u32 tune3,
			u32 tune4,
			u32 m_or_r)
{
	reg_prcm_mbias_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.tst_en0 = tst_en0;
	reg.bits.tst_en1 = tst_en1;
	reg.bits.tst_en2 = tst_en2;
	reg.bits.tst_en3 = tst_en3;
	reg.bits.tune2_0 = tune2_0;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL_EN
fun_set_mod_reg_bit(prcm, mbias_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL_TST_EN0
fun_set_mod_reg_bit(prcm, mbias_ctrl, tst_en0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL_TST_EN1
fun_set_mod_reg_bit(prcm, mbias_ctrl, tst_en1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL_TST_EN2
fun_set_mod_reg_bit(prcm, mbias_ctrl, tst_en2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL_TST_EN3
fun_set_mod_reg_bit(prcm, mbias_ctrl, tst_en3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL_TUNE2_0
fun_set_mod_reg_bit(prcm, mbias_ctrl, tune2_0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL_TUNE3
fun_set_mod_reg_bit(prcm, mbias_ctrl, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIAS_CTRL_TUNE4
fun_set_mod_reg_bit(prcm, mbias_ctrl, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PLL_DBG
void set_prcm_pll_dbg(u32 reg_addr,
			u32 dlock_en,
			u32 lock_fac_det_en,
			u32 m_or_r)
{
	reg_prcm_pll_dbg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock_en = dlock_en;
	reg.bits.lock_fac_det_en = lock_fac_det_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PLL_DBG_DLOCK_EN
fun_set_mod_reg_bit(prcm, pll_dbg, dlock_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PLL_DBG_LOCK_FAC_DET_EN
fun_set_mod_reg_bit(prcm, pll_dbg, lock_fac_det_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_EN
void set_prcm_cpu_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 m_or_r)
{
	reg_prcm_cpu_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, cpu_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, cpu_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, cpu_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_EN_REN
fun_set_mod_reg_bit(prcm, cpu_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_FAC
void set_prcm_cpu_pll_fac(u32 reg_addr,
			u32 pre_div,
			u32 post_div,
			u32 n,
			u32 m_or_r)
{
	reg_prcm_cpu_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, cpu_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, cpu_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_FAC_N
fun_set_mod_reg_bit(prcm, cpu_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0
void set_prcm_cpu_pll_tune0(u32 reg_addr,
				u32 tune1,
				u32 tune2,
				u32 tune3,
				u32 tune4,
				u32 tune5,
				u32 tune6,
				u32 tune7,
				u32 tune8,
				u32 tune9,
				u32 tune10,
				u32 tune11,
				u32 tune12,
				u32 m_or_r)
{
	reg_prcm_cpu_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;
	reg.bits.tune11 = tune11;
	reg.bits.tune12 = tune12;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE2
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE11
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE0_TUNE12
fun_set_mod_reg_bit(prcm, cpu_pll_tune0, tune12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TEN
void set_prcm_cpu_pll_ten(u32 reg_addr,
			u32 ten0,
			u32 ten1,
			u32 m_or_r)
{
	reg_prcm_cpu_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, cpu_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, cpu_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_STAT
void set_prcm_cpu_pll_stat(u32 reg_addr,
			u32 dlock,
			u32 asteady,
			u32 alock,
			u32 m_or_r)
{
	reg_prcm_cpu_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, cpu_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, cpu_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, cpu_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_MOD
void set_prcm_cpu_pll_mod(u32 reg_addr,
			u32 mod,
			u32 m_or_r)
{
	reg_prcm_cpu_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, cpu_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_NFRAC
void set_prcm_cpu_pll_nfrac(u32 reg_addr,
				u32 nfrac,
				u32 m_or_r)
{
	reg_prcm_cpu_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, cpu_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE2
void set_prcm_cpu_pll_tune2(u32 reg_addr,
				u32 en,
				u32 step,
				u32 m_or_r)
{
	reg_prcm_cpu_pll_tune2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.step = step;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE2_EN
fun_set_mod_reg_bit(prcm, cpu_pll_tune2, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_PLL_TUNE2_STEP
fun_set_mod_reg_bit(prcm, cpu_pll_tune2, step, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_EN
void set_prcm_sdram_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 m_or_r)
{
	reg_prcm_sdram_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, sdram_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, sdram_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, sdram_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_EN_REN
fun_set_mod_reg_bit(prcm, sdram_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_FAC
void set_prcm_sdram_pll_fac(u32 reg_addr,
				u32 pre_div,
				u32 post_div,
				u32 n,
				u32 m_or_r)
{
	reg_prcm_sdram_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, sdram_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, sdram_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_FAC_N
fun_set_mod_reg_bit(prcm, sdram_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0
void set_prcm_sdram_pll_tune0(u32 reg_addr,
				u32 tune1,
				u32 tune3,
				u32 tune4,
				u32 tune5,
				u32 tune6,
				u32 tune7,
				u32 tune8,
				u32 tune9,
				u32 tune10,
				u32 tune11,
				u32 tune12,
				u32 tune13,
				u32 m_or_r)
{
	reg_prcm_sdram_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;
	reg.bits.tune11 = tune11;
	reg.bits.tune12 = tune12;
	reg.bits.tune13 = tune13;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE11
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE12
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE0_TUNE13
fun_set_mod_reg_bit(prcm, sdram_pll_tune0, tune13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TEN
void set_prcm_sdram_pll_ten(u32 reg_addr,
				u32 ten0,
				u32 ten1,
				u32 ten2,
				u32 ten3,
				u32 m_or_r)
{
	reg_prcm_sdram_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;
	reg.bits.ten2 = ten2;
	reg.bits.ten3 = ten3;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, sdram_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, sdram_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TEN_TEN2
fun_set_mod_reg_bit(prcm, sdram_pll_ten, ten2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TEN_TEN3
fun_set_mod_reg_bit(prcm, sdram_pll_ten, ten3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_STAT
void set_prcm_sdram_pll_stat(u32 reg_addr,
				u32 dlock,
				u32 asteady,
				u32 alock,
				u32 m_or_r)
{
	reg_prcm_sdram_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, sdram_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, sdram_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, sdram_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_MOD
void set_prcm_sdram_pll_mod(u32 reg_addr,
				u32 mod,
				u32 m_or_r)
{
	reg_prcm_sdram_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, sdram_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_NFRAC
void set_prcm_sdram_pll_nfrac(u32 reg_addr,
				u32 nfrac,
				u32 m_or_r)
{
	reg_prcm_sdram_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, sdram_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE1
void set_prcm_sdram_pll_tune1(u32 reg_addr,
				u32 period,
				u32 amplitude,
				u32 m_or_r)
{
	reg_prcm_sdram_pll_tune1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.period = period;
	reg.bits.amplitude = amplitude;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE1_PERIOD
fun_set_mod_reg_bit(prcm, sdram_pll_tune1, period, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE1_AMPLITUDE
fun_set_mod_reg_bit(prcm, sdram_pll_tune1, amplitude, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE2
void set_prcm_sdram_pll_tune2(u32 reg_addr,
				u32 en,
				u32 step,
				u32 m_or_r)
{
	reg_prcm_sdram_pll_tune2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.step = step;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE2_EN
fun_set_mod_reg_bit(prcm, sdram_pll_tune2, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_PLL_TUNE2_STEP
fun_set_mod_reg_bit(prcm, sdram_pll_tune2, step, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_EN
void set_prcm_perh0_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 m_or_r)
{
	reg_prcm_perh0_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, perh0_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, perh0_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, perh0_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_EN_REN
fun_set_mod_reg_bit(prcm, perh0_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_FAC
void set_prcm_perh0_pll_fac(u32 reg_addr,
				u32 pre_div,
				u32 post_div,
				u32 n,
				u32 m_or_r)
{
	reg_prcm_perh0_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, perh0_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, perh0_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_FAC_N
fun_set_mod_reg_bit(prcm, perh0_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0
void set_prcm_perh0_pll_tune0(u32 reg_addr,
				u32 tune1,
				u32 tune2,
				u32 tune3,
				u32 tune4,
				u32 tune5,
				u32 tune6,
				u32 tune7,
				u32 tune8,
				u32 tune9,
				u32 tune10,
				u32 m_or_r)
{
	reg_prcm_perh0_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE2
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, perh0_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TEN
void set_prcm_perh0_pll_ten(u32 reg_addr,
				u32 ten0,
				u32 ten1,
				u32 ten3,
				u32 ten2,
				u32 m_or_r)
{
	reg_prcm_perh0_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;
	reg.bits.ten3 = ten3;
	reg.bits.ten2 = ten2;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, perh0_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, perh0_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TEN_TEN3
fun_set_mod_reg_bit(prcm, perh0_pll_ten, ten3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_TEN_TEN2
fun_set_mod_reg_bit(prcm, perh0_pll_ten, ten2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_STAT
void set_prcm_perh0_pll_stat(u32 reg_addr,
				u32 dlock,
				u32 asteady,
				u32 alock,
				u32 m_or_r)
{
	reg_prcm_perh0_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, perh0_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, perh0_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, perh0_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_MOD
void set_prcm_perh0_pll_mod(u32 reg_addr,
				u32 mod,
				u32 m_or_r)
{
	reg_prcm_perh0_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, perh0_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_NFRAC
void set_prcm_perh0_pll_nfrac(u32 reg_addr,
				u32 nfrac,
				u32 m_or_r)
{
	reg_prcm_perh0_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH0_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, perh0_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_EN
void set_prcm_audio_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 div_endiv17,
			u32 div_endiv7,
			u32 m_or_r)
{
	reg_prcm_audio_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;
	reg.bits.div_endiv17 = div_endiv17;
	reg.bits.div_endiv7 = div_endiv7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, audio_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, audio_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, audio_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_EN_REN
fun_set_mod_reg_bit(prcm, audio_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_EN_DIV_ENDIV17
fun_set_mod_reg_bit(prcm, audio_pll_en, div_endiv17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_EN_DIV_ENDIV7
fun_set_mod_reg_bit(prcm, audio_pll_en, div_endiv7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_FAC
void set_prcm_audio_pll_fac(u32 reg_addr,
				u32 pre_div,
				u32 post_div,
				u32 n,
				u32 m_or_r)
{
	reg_prcm_audio_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, audio_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, audio_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_FAC_N
fun_set_mod_reg_bit(prcm, audio_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0
void set_prcm_audio_pll_tune0(u32 reg_addr,
				u32 tune1,
				u32 tune2,
				u32 tune3,
				u32 tune4,
				u32 tune5,
				u32 tune6,
				u32 tune7,
				u32 tune8,
				u32 tune9,
				u32 tune10,
				u32 tune11,
				u32 tune12,
				u32 m_or_r)
{
	reg_prcm_audio_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;
	reg.bits.tune11 = tune11;
	reg.bits.tune12 = tune12;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE2
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE11
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TUNE0_TUNE12
fun_set_mod_reg_bit(prcm, audio_pll_tune0, tune12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TEN
void set_prcm_audio_pll_ten(u32 reg_addr,
				u32 ten0,
				u32 ten1,
				u32 m_or_r)
{
	reg_prcm_audio_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, audio_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, audio_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_STAT
void set_prcm_audio_pll_stat(u32 reg_addr,
				u32 dlock,
				u32 asteady,
				u32 alock,
				u32 m_or_r)
{
	reg_prcm_audio_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, audio_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, audio_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, audio_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_MOD
void set_prcm_audio_pll_mod(u32 reg_addr,
				u32 mod,
				u32 m_or_r)
{
	reg_prcm_audio_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, audio_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_NFRAC
void set_prcm_audio_pll_nfrac(u32 reg_addr,
				u32 nfrac,
				u32 m_or_r)
{
	reg_prcm_audio_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AUDIO_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, audio_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_EN
void set_prcm_vc_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 m_or_r)
{
	reg_prcm_vc_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VC_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, vc_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, vc_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, vc_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_EN_REN
fun_set_mod_reg_bit(prcm, vc_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_FAC
void set_prcm_vc_pll_fac(u32 reg_addr,
			u32 pre_div,
			u32 post_div,
			u32 n,
			u32 m_or_r)
{
	reg_prcm_vc_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VC_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, vc_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, vc_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_FAC_N
fun_set_mod_reg_bit(prcm, vc_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0
void set_prcm_vc_pll_tune0(u32 reg_addr,
			u32 tune1,
			u32 tune2,
			u32 tune3,
			u32 tune4,
			u32 tune5,
			u32 tune6,
			u32 tune7,
			u32 tune8,
			u32 tune9,
			u32 tune10,
			u32 m_or_r)
{
	reg_prcm_vc_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE2
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, vc_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TEN
void set_prcm_vc_pll_ten(u32 reg_addr,
			u32 ten0,
			u32 ten1,
			u32 m_or_r)
{
	reg_prcm_vc_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, vc_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, vc_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_STAT
void set_prcm_vc_pll_stat(u32 reg_addr,
			u32 dlock,
			u32 asteady,
			u32 alock,
			u32 m_or_r)
{
	reg_prcm_vc_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VC_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, vc_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, vc_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, vc_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_MOD
void set_prcm_vc_pll_mod(u32 reg_addr,
			u32 mod,
			u32 m_or_r)
{
	reg_prcm_vc_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VC_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, vc_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_PLL_NFRAC
void set_prcm_vc_pll_nfrac(u32 reg_addr,
			u32 nfrac,
			u32 m_or_r)
{
	reg_prcm_vc_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VC_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, vc_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_EN
void set_prcm_perh1_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 m_or_r)
{
	reg_prcm_perh1_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, perh1_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, perh1_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, perh1_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_EN_REN
fun_set_mod_reg_bit(prcm, perh1_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_FAC
void set_prcm_perh1_pll_fac(u32 reg_addr,
				u32 pre_div,
				u32 post_div,
				u32 n,
				u32 m_or_r)
{
	reg_prcm_perh1_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, perh1_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, perh1_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_FAC_N
fun_set_mod_reg_bit(prcm, perh1_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0
void set_prcm_perh1_pll_tune0(u32 reg_addr,
				u32 tune1,
				u32 tune2,
				u32 tune3,
				u32 tune4,
				u32 tune5,
				u32 tune6,
				u32 tune7,
				u32 tune8,
				u32 tune9,
				u32 tune10,
				u32 tune11,
				u32 tune12,
				u32 m_or_r)
{
	reg_prcm_perh1_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;
	reg.bits.tune11 = tune11;
	reg.bits.tune12 = tune12;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE2
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE11
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE0_TUNE12
fun_set_mod_reg_bit(prcm, perh1_pll_tune0, tune12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TEN
void set_prcm_perh1_pll_ten(u32 reg_addr,
				u32 ten0,
				u32 ten1,
				u32 m_or_r)
{
	reg_prcm_perh1_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, perh1_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, perh1_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_STAT
void set_prcm_perh1_pll_stat(u32 reg_addr,
				u32 dlock,
				u32 asteady,
				u32 alock,
				u32 m_or_r)
{
	reg_prcm_perh1_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, perh1_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, perh1_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, perh1_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_MOD
void set_prcm_perh1_pll_mod(u32 reg_addr,
				u32 mod,
				u32 m_or_r)
{
	reg_prcm_perh1_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, perh1_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_NFRAC
void set_prcm_perh1_pll_nfrac(u32 reg_addr,
				u32 nfrac,
				u32 m_or_r)
{
	reg_prcm_perh1_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, perh1_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE1
void set_prcm_perh1_pll_tune1(u32 reg_addr,
				u32 period,
				u32 amplitude,
				u32 m_or_r)
{
	reg_prcm_perh1_pll_tune1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.period = period;
	reg.bits.amplitude = amplitude;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE1_PERIOD
fun_set_mod_reg_bit(prcm, perh1_pll_tune1, period, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH1_PLL_TUNE1_AMPLITUDE
fun_set_mod_reg_bit(prcm, perh1_pll_tune1, amplitude, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_EN
void set_prcm_disp_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 m_or_r)
{
	reg_prcm_disp_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, disp_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, disp_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, disp_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_EN_REN
fun_set_mod_reg_bit(prcm, disp_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_FAC
void set_prcm_disp_pll_fac(u32 reg_addr,
			u32 pre_div,
			u32 post_div,
			u32 n,
			u32 m_or_r)
{
	reg_prcm_disp_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, disp_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, disp_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_FAC_N
fun_set_mod_reg_bit(prcm, disp_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0
void set_prcm_disp_pll_tune0(u32 reg_addr,
				u32 tune1,
				u32 tune2,
				u32 tune3,
				u32 tune4,
				u32 tune5,
				u32 tune6,
				u32 tune7,
				u32 tune8,
				u32 tune9,
				u32 tune10,
				u32 tune11,
				u32 tune12,
				u32 m_or_r)
{
	reg_prcm_disp_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;
	reg.bits.tune11 = tune11;
	reg.bits.tune12 = tune12;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE2
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE11
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE0_TUNE12
fun_set_mod_reg_bit(prcm, disp_pll_tune0, tune12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TEN
void set_prcm_disp_pll_ten(u32 reg_addr,
			u32 ten0,
			u32 ten1,
			u32 m_or_r)
{
	reg_prcm_disp_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, disp_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, disp_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_STAT
void set_prcm_disp_pll_stat(u32 reg_addr,
				u32 dlock,
				u32 asteady,
				u32 alock,
				u32 m_or_r)
{
	reg_prcm_disp_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, disp_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, disp_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, disp_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_MOD
void set_prcm_disp_pll_mod(u32 reg_addr,
			u32 mod,
			u32 m_or_r)
{
	reg_prcm_disp_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, disp_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_NFRAC
void set_prcm_disp_pll_nfrac(u32 reg_addr,
				u32 nfrac,
				u32 m_or_r)
{
	reg_prcm_disp_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, disp_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE1
void set_prcm_disp_pll_tune1(u32 reg_addr,
				u32 period,
				u32 amplitude,
				u32 m_or_r)
{
	reg_prcm_disp_pll_tune1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.period = period;
	reg.bits.amplitude = amplitude;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE1_PERIOD
fun_set_mod_reg_bit(prcm, disp_pll_tune1, period, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DISP_PLL_TUNE1_AMPLITUDE
fun_set_mod_reg_bit(prcm, disp_pll_tune1, amplitude, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_EN
void set_prcm_ax_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 m_or_r)
{
	reg_prcm_ax_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, ax_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, ax_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, ax_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_EN_REN
fun_set_mod_reg_bit(prcm, ax_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_FAC
void set_prcm_ax_pll_fac(u32 reg_addr,
			u32 pre_div,
			u32 post_div,
			u32 n,
			u32 m_or_r)
{
	reg_prcm_ax_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, ax_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, ax_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_FAC_N
fun_set_mod_reg_bit(prcm, ax_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0
void set_prcm_ax_pll_tune0(u32 reg_addr,
			u32 tune1,
			u32 tune2,
			u32 tune3,
			u32 tune4,
			u32 tune5,
			u32 tune6,
			u32 tune7,
			u32 tune8,
			u32 tune9,
			u32 tune10,
			u32 tune11,
			u32 tune12,
			u32 m_or_r)
{
	reg_prcm_ax_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;
	reg.bits.tune11 = tune11;
	reg.bits.tune12 = tune12;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE2
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE11
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE0_TUNE12
fun_set_mod_reg_bit(prcm, ax_pll_tune0, tune12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TEN
void set_prcm_ax_pll_ten(u32 reg_addr,
			u32 ten0,
			u32 ten1,
			u32 m_or_r)
{
	reg_prcm_ax_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, ax_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, ax_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_STAT
void set_prcm_ax_pll_stat(u32 reg_addr,
			u32 dlock,
			u32 asteady,
			u32 alock,
			u32 m_or_r)
{
	reg_prcm_ax_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, ax_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, ax_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, ax_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_MOD
void set_prcm_ax_pll_mod(u32 reg_addr,
			u32 mod,
			u32 m_or_r)
{
	reg_prcm_ax_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, ax_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_NFRAC
void set_prcm_ax_pll_nfrac(u32 reg_addr,
			u32 nfrac,
			u32 m_or_r)
{
	reg_prcm_ax_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, ax_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE2
void set_prcm_ax_pll_tune2(u32 reg_addr,
			u32 en,
			u32 step,
			u32 m_or_r)
{
	reg_prcm_ax_pll_tune2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.step = step;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE2_EN
fun_set_mod_reg_bit(prcm, ax_pll_tune2, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_PLL_TUNE2_STEP
fun_set_mod_reg_bit(prcm, ax_pll_tune2, step, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_EN
void set_prcm_perh2_pll_en(u32 reg_addr,
			u32 enp,
			u32 enm,
			u32 oen,
			u32 ren,
			u32 m_or_r)
{
	reg_prcm_perh2_pll_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enp = enp;
	reg.bits.enm = enm;
	reg.bits.oen = oen;
	reg.bits.ren = ren;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_EN_ENP
fun_set_mod_reg_bit(prcm, perh2_pll_en, enp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_EN_ENM
fun_set_mod_reg_bit(prcm, perh2_pll_en, enm, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_EN_OEN
fun_set_mod_reg_bit(prcm, perh2_pll_en, oen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_EN_REN
fun_set_mod_reg_bit(prcm, perh2_pll_en, ren, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_FAC
void set_prcm_perh2_pll_fac(u32 reg_addr,
				u32 pre_div,
				u32 post_div,
				u32 n,
				u32 m_or_r)
{
	reg_prcm_perh2_pll_fac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pre_div = pre_div;
	reg.bits.post_div = post_div;
	reg.bits.n = n;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_FAC_PRE_DIV
fun_set_mod_reg_bit(prcm, perh2_pll_fac, pre_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_FAC_POST_DIV
fun_set_mod_reg_bit(prcm, perh2_pll_fac, post_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_FAC_N
fun_set_mod_reg_bit(prcm, perh2_pll_fac, n, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0
void set_prcm_perh2_pll_tune0(u32 reg_addr,
				u32 tune1,
				u32 tune2,
				u32 tune3,
				u32 tune4,
				u32 tune5,
				u32 tune6,
				u32 tune7,
				u32 tune8,
				u32 tune9,
				u32 tune10,
				u32 tune11,
				u32 tune12,
				u32 m_or_r)
{
	reg_prcm_perh2_pll_tune0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.tune1 = tune1;
	reg.bits.tune2 = tune2;
	reg.bits.tune3 = tune3;
	reg.bits.tune4 = tune4;
	reg.bits.tune5 = tune5;
	reg.bits.tune6 = tune6;
	reg.bits.tune7 = tune7;
	reg.bits.tune8 = tune8;
	reg.bits.tune9 = tune9;
	reg.bits.tune10 = tune10;
	reg.bits.tune11 = tune11;
	reg.bits.tune12 = tune12;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE1
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE2
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE3
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE4
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE5
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE6
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE7
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE8
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE9
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE10
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE11
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE0_TUNE12
fun_set_mod_reg_bit(prcm, perh2_pll_tune0, tune12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TEN
void set_prcm_perh2_pll_ten(u32 reg_addr,
				u32 ten0,
				u32 ten1,
				u32 m_or_r)
{
	reg_prcm_perh2_pll_ten_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ten0 = ten0;
	reg.bits.ten1 = ten1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TEN_TEN0
fun_set_mod_reg_bit(prcm, perh2_pll_ten, ten0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TEN_TEN1
fun_set_mod_reg_bit(prcm, perh2_pll_ten, ten1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_STAT
void set_prcm_perh2_pll_stat(u32 reg_addr,
				u32 dlock,
				u32 asteady,
				u32 alock,
				u32 m_or_r)
{
	reg_prcm_perh2_pll_stat_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.dlock = dlock;
	reg.bits.asteady = asteady;
	reg.bits.alock = alock;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_STAT_DLOCK
fun_set_mod_reg_bit(prcm, perh2_pll_stat, dlock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_STAT_ASTEADY
fun_set_mod_reg_bit(prcm, perh2_pll_stat, asteady, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_STAT_ALOCK
fun_set_mod_reg_bit(prcm, perh2_pll_stat, alock, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_MOD
void set_prcm_perh2_pll_mod(u32 reg_addr,
				u32 mod,
				u32 m_or_r)
{
	reg_prcm_perh2_pll_mod_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.mod = mod;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_MOD_MOD
fun_set_mod_reg_bit(prcm, perh2_pll_mod, mod, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_NFRAC
void set_prcm_perh2_pll_nfrac(u32 reg_addr,
				u32 nfrac,
				u32 m_or_r)
{
	reg_prcm_perh2_pll_nfrac_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.nfrac = nfrac;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_NFRAC_NFRAC
fun_set_mod_reg_bit(prcm, perh2_pll_nfrac, nfrac, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE1
void set_prcm_perh2_pll_tune1(u32 reg_addr,
				u32 period,
				u32 amplitude,
				u32 m_or_r)
{
	reg_prcm_perh2_pll_tune1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.period = period;
	reg.bits.amplitude = amplitude;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE1_PERIOD
fun_set_mod_reg_bit(prcm, perh2_pll_tune1, period, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PERH2_PLL_TUNE1_AMPLITUDE
fun_set_mod_reg_bit(prcm, perh2_pll_tune1, amplitude, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PLL_TBLK
void set_prcm_pll_tblk(u32 reg_addr,
			u32 en,
			u32 div_endio,
			u32 sel0,
			u32 sel1,
			u32 div0,
			u32 m_or_r)
{
	reg_prcm_pll_tblk_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;
	reg.bits.div_endio = div_endio;
	reg.bits.sel0 = sel0;
	reg.bits.sel1 = sel1;
	reg.bits.div0 = div0;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_PLL_TBLK_EN
fun_set_mod_reg_bit(prcm, pll_tblk, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PLL_TBLK_DIV_ENDIO
fun_set_mod_reg_bit(prcm, pll_tblk, div_endio, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PLL_TBLK_SEL0
fun_set_mod_reg_bit(prcm, pll_tblk, sel0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PLL_TBLK_SEL1
fun_set_mod_reg_bit(prcm, pll_tblk, sel1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_PLL_TBLK_DIV0
fun_set_mod_reg_bit(prcm, pll_tblk, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_AXI_CLK_CTRL
void set_prcm_cpu_axi_clk_ctrl(u32 reg_addr,
				u32 cpu_src_sel,
				u32 cpu_src_div,
				u32 m_or_r)
{
	reg_prcm_cpu_axi_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cpu_src_sel = cpu_src_sel;
	reg.bits.cpu_src_div = cpu_src_div;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_CPU_AXI_CLK_CTRL_CPU_SRC_SEL
fun_set_mod_reg_bit(prcm, cpu_axi_clk_ctrl, cpu_src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_CPU_AXI_CLK_CTRL_CPU_SRC_DIV
fun_set_mod_reg_bit(prcm, cpu_axi_clk_ctrl, cpu_src_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_APB_CLK_CTRL
void set_prcm_ahb_apb_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 ahb_div,
				u32 m_or_r)
{
	reg_prcm_ahb_apb_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.ahb_div = ahb_div;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AHB_APB_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, ahb_apb_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AHB_APB_CLK_CTRL_AHB_DIV
fun_set_mod_reg_bit(prcm, ahb_apb_clk_ctrl, ahb_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MAXI_CLK_CTRL
void set_prcm_maxi_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_maxi_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_MAXI_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, maxi_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MAXI_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, maxi_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MAXI_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, maxi_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_CLK_CTRL
void set_prcm_sdram_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_sdram_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, sdram_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, sdram_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, sdram_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_RST
void set_prcm_sdram_rst(u32 reg_addr,
			u32 sdramc,
			u32 pub,
			u32 x4clk_rst,
			u32 m_or_r)
{
	reg_prcm_sdram_rst_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sdramc = sdramc;
	reg.bits.pub = pub;
	reg.bits.x4clk_rst = x4clk_rst;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_RST_SDRAMC
fun_set_mod_reg_bit(prcm, sdram_rst, sdramc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_RST_PUB
fun_set_mod_reg_bit(prcm, sdram_rst, pub, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_RST_X4CLK_RST
fun_set_mod_reg_bit(prcm, sdram_rst, x4clk_rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_BM_CLK_CTRL
void set_prcm_sdram_bm_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_sdram_bm_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDRAM_BM_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, sdram_bm_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDRAM_BM_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, sdram_bm_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC0_CLK_CTRL
void set_prcm_sdc0_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div0,
				u32 div1,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_sdc0_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div0 = div0;
	reg.bits.div1 = div1;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDC0_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, sdc0_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC0_CLK_CTRL_DIV0
fun_set_mod_reg_bit(prcm, sdc0_clk_ctrl, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC0_CLK_CTRL_DIV1
fun_set_mod_reg_bit(prcm, sdc0_clk_ctrl, div1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC0_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, sdc0_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC1_CLK_CTRL
void set_prcm_sdc1_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div0,
				u32 div1,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_sdc1_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div0 = div0;
	reg.bits.div1 = div1;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDC1_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, sdc1_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC1_CLK_CTRL_DIV0
fun_set_mod_reg_bit(prcm, sdc1_clk_ctrl, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC1_CLK_CTRL_DIV1
fun_set_mod_reg_bit(prcm, sdc1_clk_ctrl, div1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC1_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, sdc1_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC2_CLK_CTRL
void set_prcm_sdc2_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div0,
				u32 div1,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_sdc2_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div0 = div0;
	reg.bits.div1 = div1;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SDC2_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, sdc2_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC2_CLK_CTRL_DIV0
fun_set_mod_reg_bit(prcm, sdc2_clk_ctrl, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC2_CLK_CTRL_DIV1
fun_set_mod_reg_bit(prcm, sdc2_clk_ctrl, div1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SDC2_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, sdc2_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI0_CLK_CTRL
void set_prcm_spi0_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div0,
				u32 div1,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_spi0_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div0 = div0;
	reg.bits.div1 = div1;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SPI0_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, spi0_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI0_CLK_CTRL_DIV0
fun_set_mod_reg_bit(prcm, spi0_clk_ctrl, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI0_CLK_CTRL_DIV1
fun_set_mod_reg_bit(prcm, spi0_clk_ctrl, div1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI0_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, spi0_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI1_CLK_CTRL
void set_prcm_spi1_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div0,
				u32 div1,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_spi1_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div0 = div0;
	reg.bits.div1 = div1;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SPI1_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, spi1_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI1_CLK_CTRL_DIV0
fun_set_mod_reg_bit(prcm, spi1_clk_ctrl, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI1_CLK_CTRL_DIV1
fun_set_mod_reg_bit(prcm, spi1_clk_ctrl, div1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI1_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, spi1_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI2_CLK_CTRL
void set_prcm_spi2_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div0,
				u32 div1,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_spi2_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div0 = div0;
	reg.bits.div1 = div1;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_SPI2_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, spi2_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI2_CLK_CTRL_DIV0
fun_set_mod_reg_bit(prcm, spi2_clk_ctrl, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI2_CLK_CTRL_DIV1
fun_set_mod_reg_bit(prcm, spi2_clk_ctrl, div1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_SPI2_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, spi2_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_CLK_CTRL
void set_prcm_vc_clk_ctrl(u32 reg_addr,
			u32 src_sel,
			u32 div0,
			u32 en,
			u32 m_or_r)
{
	reg_prcm_vc_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div0 = div0;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VC_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, vc_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_CLK_CTRL_DIV0
fun_set_mod_reg_bit(prcm, vc_clk_ctrl, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VC_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, vc_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VDC_CLK_CTRL
void set_prcm_vdc_clk_ctrl(u32 reg_addr,
			u32 src_sel,
			u32 div0,
			u32 en,
			u32 m_or_r)
{
	reg_prcm_vdc_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div0 = div0;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VDC_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, vdc_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VDC_CLK_CTRL_DIV0
fun_set_mod_reg_bit(prcm, vdc_clk_ctrl, div0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VDC_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, vdc_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VISS_CLK_CTRL
void set_prcm_viss_clk_ctrl(u32 reg_addr,
				u32 sclk0_en,
				u32 sclk1_en,
				u32 sclk2_en,
				u32 m_or_r)
{
	reg_prcm_viss_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sclk0_en = sclk0_en;
	reg.bits.sclk1_en = sclk1_en;
	reg.bits.sclk2_en = sclk2_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_VISS_CLK_CTRL_SCLK0_EN
fun_set_mod_reg_bit(prcm, viss_clk_ctrl, sclk0_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VISS_CLK_CTRL_SCLK1_EN
fun_set_mod_reg_bit(prcm, viss_clk_ctrl, sclk1_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_VISS_CLK_CTRL_SCLK2_EN
fun_set_mod_reg_bit(prcm, viss_clk_ctrl, sclk2_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK0_CLK_CTRL
void set_prcm_dpu_sclk0_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_dpu_sclk0_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK0_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, dpu_sclk0_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK0_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, dpu_sclk0_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK0_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, dpu_sclk0_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK1_CLK_CTRL
void set_prcm_dpu_sclk1_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_dpu_sclk1_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK1_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, dpu_sclk1_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK1_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, dpu_sclk1_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK1_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, dpu_sclk1_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK2_CLK_CTRL
void set_prcm_dpu_sclk2_clk_ctrl(u32 reg_addr,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_dpu_sclk2_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DPU_SCLK2_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, dpu_sclk2_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DOSS_CLK_CTRL
void set_prcm_doss_clk_ctrl(u32 reg_addr,
				u32 hfeosc_en,
				u32 sclk0_en,
				u32 sclk0_src,
				u32 sclk0_div,
				u32 m_or_r)
{
	reg_prcm_doss_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.hfeosc_en = hfeosc_en;
	reg.bits.sclk0_en = sclk0_en;
	reg.bits.sclk0_src = sclk0_src;
	reg.bits.sclk0_div = sclk0_div;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_DOSS_CLK_CTRL_HFEOSC_EN
fun_set_mod_reg_bit(prcm, doss_clk_ctrl, hfeosc_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DOSS_CLK_CTRL_SCLK0_EN
fun_set_mod_reg_bit(prcm, doss_clk_ctrl, sclk0_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DOSS_CLK_CTRL_SCLK0_SRC
fun_set_mod_reg_bit(prcm, doss_clk_ctrl, sclk0_src, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_DOSS_CLK_CTRL_SCLK0_DIV
fun_set_mod_reg_bit(prcm, doss_clk_ctrl, sclk0_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_CLK_CTRL
void set_prcm_ax_clk_ctrl(u32 reg_addr,
			u32 src_sel,
			u32 div,
			u32 en,
			u32 m_or_r)
{
	reg_prcm_ax_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AX_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, ax_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, ax_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AX_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, ax_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2S0_CLK_CTRL
void set_prcm_i2s0_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_i2s0_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_I2S0_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, i2s0_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2S0_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, i2s0_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2S0_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, i2s0_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2S1_CLK_CTRL
void set_prcm_i2s1_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_i2s1_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_I2S1_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, i2s1_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2S1_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, i2s1_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2S1_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, i2s1_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C0_CLK_CTRL
void set_prcm_i2c0_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_i2c0_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_I2C0_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, i2c0_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C0_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, i2c0_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C0_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, i2c0_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C1_CLK_CTRL
void set_prcm_i2c1_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_i2c1_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_I2C1_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, i2c1_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C1_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, i2c1_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C1_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, i2c1_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C2_CLK_CTRL
void set_prcm_i2c2_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_i2c2_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_I2C2_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, i2c2_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C2_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, i2c2_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C2_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, i2c2_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C3_CLK_CTRL
void set_prcm_i2c3_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_i2c3_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_I2C3_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, i2c3_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C3_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, i2c3_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_I2C3_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, i2c3_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART0_CLK_CTRL
void set_prcm_uart0_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_uart0_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_UART0_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, uart0_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART0_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, uart0_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART0_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, uart0_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART1_CLK_CTRL
void set_prcm_uart1_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_uart1_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_UART1_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, uart1_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART1_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, uart1_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART1_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, uart1_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART2_CLK_CTRL
void set_prcm_uart2_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_uart2_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_UART2_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, uart2_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART2_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, uart2_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART2_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, uart2_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART3_CLK_CTRL
void set_prcm_uart3_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_uart3_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_UART3_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, uart3_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART3_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, uart3_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_UART3_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, uart3_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_USB_PHY_RST
void set_prcm_usb_phy_rst(u32 reg_addr,
			u32 rst,
			u32 m_or_r)
{
	reg_prcm_usb_phy_rst_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.rst = rst;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_USB_PHY_RST_RST
fun_set_mod_reg_bit(prcm, usb_phy_rst, rst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GPADC_CLK_CTRL
void set_prcm_gpadc_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_gpadc_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_GPADC_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, gpadc_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GPADC_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, gpadc_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GPADC_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, gpadc_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_IR_CLK_CTRL
void set_prcm_ir_clk_ctrl(u32 reg_addr,
			u32 en,
			u32 m_or_r)
{
	reg_prcm_ir_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_IR_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, ir_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GMAC_CLK_CTRL
void set_prcm_gmac_clk_ctrl(u32 reg_addr,
				u32 core_src_sel,
				u32 core_div,
				u32 core_en,
				u32 sclk_src_sel,
				u32 sclk1_en,
				u32 sclk0_en,
				u32 m_or_r)
{
	reg_prcm_gmac_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.core_src_sel = core_src_sel;
	reg.bits.core_div = core_div;
	reg.bits.core_en = core_en;
	reg.bits.sclk_src_sel = sclk_src_sel;
	reg.bits.sclk1_en = sclk1_en;
	reg.bits.sclk0_en = sclk0_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_GMAC_CLK_CTRL_CORE_SRC_SEL
fun_set_mod_reg_bit(prcm, gmac_clk_ctrl, core_src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GMAC_CLK_CTRL_CORE_DIV
fun_set_mod_reg_bit(prcm, gmac_clk_ctrl, core_div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GMAC_CLK_CTRL_CORE_EN
fun_set_mod_reg_bit(prcm, gmac_clk_ctrl, core_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GMAC_CLK_CTRL_SCLK_SRC_SEL
fun_set_mod_reg_bit(prcm, gmac_clk_ctrl, sclk_src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GMAC_CLK_CTRL_SCLK1_EN
fun_set_mod_reg_bit(prcm, gmac_clk_ctrl, sclk1_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_GMAC_CLK_CTRL_SCLK0_EN
fun_set_mod_reg_bit(prcm, gmac_clk_ctrl, sclk0_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIST_CLK_CTRL
void set_prcm_mbist_clk_ctrl(u32 reg_addr,
				u32 src_sel,
				u32 div,
				u32 en,
				u32 m_or_r)
{
	reg_prcm_mbist_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.src_sel = src_sel;
	reg.bits.div = div;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_MBIST_CLK_CTRL_SRC_SEL
fun_set_mod_reg_bit(prcm, mbist_clk_ctrl, src_sel, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIST_CLK_CTRL_DIV
fun_set_mod_reg_bit(prcm, mbist_clk_ctrl, div, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_MBIST_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, mbist_clk_ctrl, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_PRCM_AES_CLK_CTRL
void set_prcm_aes_clk_ctrl(u32 reg_addr,
			u32 en,
			u32 m_or_r)
{
	reg_prcm_aes_clk_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_PRCM_AES_CLK_CTRL_EN
fun_set_mod_reg_bit(prcm, aes_clk_ctrl, en, addr, val, m_or_r)
#endif

#endif /* ___PRCM__SET___C___ */
