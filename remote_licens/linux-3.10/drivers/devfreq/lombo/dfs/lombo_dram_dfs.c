/*
 * dram调频代码
 * 目前dram调频只支持软件调频，调频的方式是改变PRCM中SDRAM的module clock div，PLL值不变
 * V0.1  20181122  hbq Initial version
 */

#include "lombo_dram_dfs.h"

#define DATA_NUM 32

reg_prcm_t         *prcm = (reg_prcm_t *)VBASE_PRCM;
reg_dramc_t       *dramc = (reg_dramc_t *)VBASE_DRAMC;
reg_drampub_t   *drampub = (reg_drampub_t *)VBASE_DRAMPUB;
reg_mem_ctrl_t *mem_ctrl = (reg_mem_ctrl_t *)VBASE_MEM_CTRL;
reg_gtimer_t     *gtimer = (reg_gtimer_t *)VBASE_GTIMER;

/* 用于存放training中破坏的数据 */
u32 training_data_save[64];

/* use gtimer1 */
void udelay_dfs(u32 us)
{
	u32 store_cnt = 24 * us;/* 24MHz clk */

	if (us == 0)
		return;

	/* clear pending */
	if (READREG32(&gtimer->gtmr1_int_pending))
		WRITEREG32(&gtimer->gtmr1_clr_int_pending, 1);

	/* disable gtimer1 */
	WRITEREG32(&gtimer->gtmr1_ctrl, 0);

	/* counter */
	WRITEREG32(&gtimer->gtmr1_store_val, store_cnt);

	/* HFCOSC CLK, DIV=1 */
	WRITEREG32(&gtimer->gtmr1_clk_ctrl, 0);

	/* down count mode, manual mode, and enable */
	WRITEREG32(&gtimer->gtmr1_ctrl, 1);

	while (READREG32(&gtimer->gtmr1_int_pending) == 0)
		;

	WRITEREG32(&gtimer->gtmr1_clr_int_pending, 1);
}

/*
 * func: void mr_wr_dfs(u32 mr, u32 mr_val, u32 rnk_no)
 * 参数: mr: MR寄存器，如DDR3_MR0,DDR3_MR1,DDR2_EMR等
 * mr_val: 要往MR寄存器写入的值
 * rnk_no: 控制要操作的rank，如RNKO0,RNKO1,RNKN01等
 * 说明：Mode Register Writes
 */
void mr_wr_dfs(u32 mr, u32 mr_val, u32 rnk_no)
{
	u32 reg_val;

	/* check if mr_wr_dfs_busy */
	do {
		reg_val = READREG32(VA_DRAMC_MRSTAT) & 0x1;
	} while (reg_val == 1);

	/* select mr and rankn */
	reg_val = READREG32(VA_DRAMC_MRCTRL0) & ~(0xF<<4) & ~(0xF<<12);
	reg_val = reg_val | (rnk_no<<4) | (mr<<12);
	WRITEREG32(VA_DRAMC_MRCTRL0, reg_val);

	/* set the mr register value */
	WRITEREG32(VA_DRAMC_MRCTRL1, mr_val);

	/* triggers a mode register write op and wait finished */
	reg_val = READREG32(VA_DRAMC_MRCTRL0) | (1u<<31);
	WRITEREG32(VA_DRAMC_MRCTRL0, reg_val);

	do {
		reg_val = READREG32(VA_DRAMC_MRCTRL0) & (1u<<31);
	} while (reg_val);

	/* update the initial register */
	/* PUB initial */
	switch (mr) {
	case 0:
		/* PUB */
		WRITEREG32(VA_DRAMPUB_MR0, mr_val);
		/* MC */
		WRITEREG32(VA_DRAMC_SWCTL, 0);/* Enable quasi-dynamic register programming outside reset */

		reg_val = (READREG32(VA_DRAMC_INIT3) & ~(0xFFFFu<<16)) | (mr_val<<16);
		WRITEREG32(VA_DRAMC_INIT3, reg_val);

		WRITEREG32(VA_DRAMC_SWCTL, 1);/* set back to 1 */

		do {
			reg_val = READREG32(VA_DRAMC_SWSTAT) & 0x1;/* Register programming done */
		} while (reg_val != 1);
		break;
	case 1:
		/* PUB */
		WRITEREG32(VA_DRAMPUB_MR1, mr_val);
		/* MC */
		WRITEREG32(VA_DRAMC_SWCTL, 0);/* Enable quasi-dynamic register programming outside reset */

		reg_val = (READREG32(VA_DRAMC_INIT3) & ~(0xFFFF<<0)) | (mr_val<<0);
		WRITEREG32(VA_DRAMC_INIT3, reg_val);

		WRITEREG32(VA_DRAMC_SWCTL, 1);/* set back to 1 */

		do {
			reg_val = READREG32(VA_DRAMC_SWSTAT) & 0x1;/* Register programming done */
		} while (reg_val != 1);
		break;
	case 2:
		/* PUB */
		WRITEREG32(VA_DRAMPUB_MR2, mr_val);
		/* MC */
		WRITEREG32(VA_DRAMC_SWCTL, 0);/* Enable quasi-dynamic register programming outside reset */

		reg_val = (READREG32(VA_DRAMC_INIT4) & ~(0xFFFFu<<16)) | (mr_val<<16);
		WRITEREG32(VA_DRAMC_INIT4, reg_val);

		WRITEREG32(VA_DRAMC_SWCTL, 1);/* set back to 1 */

		do {
			reg_val = READREG32(VA_DRAMC_SWSTAT) & 0x1;/* Register programming done */
		} while (reg_val != 1);
		break;
	case 3:
		/* PUB */
		WRITEREG32(VA_DRAMPUB_MR3, mr_val);
		/* MC */
		WRITEREG32(VA_DRAMC_SWCTL, 0);/* Enable quasi-dynamic register programming outside reset */

		reg_val = (READREG32(VA_DRAMC_INIT4) & ~(0xFFFF<<0)) | (mr_val<<0);
		WRITEREG32(VA_DRAMC_INIT4, reg_val);

		WRITEREG32(VA_DRAMC_SWCTL, 1);/* set back to 1 */

		do {
			reg_val = READREG32(VA_DRAMC_SWSTAT) & 0x1;/* Register programming done */
		} while (reg_val != 1);
		break;
	default:
		ERR_FLG();
		break;
	}
}

/*
 * void phy_ddl_training(void)
 * 功能：  Digital Delay Line training
 * 参数：  NULL
 * 返回值：NULL
 */
void phy_ddl_training(void)
{
	u32 reg_val;
	reg_drampub_pir_t pir;

	/* reg_val = (1 << 0) | (1 << 5) | (1 << 8) | (1 << 10)| (0x0 << 12); */
	reg_val = (1 << 0) | (1 << 5) | (0 << 7) | (1 << 8) | (1 << 10) | (0x0 << 12);
	WRITEREG32(VA_DRAMPUB_PIR, reg_val);

	asm volatile ("dsb");

	do {
		reg_val = READREG32(VA_DRAMPUB_PIR) & 0x1;
	} while (reg_val == 1);

	/* min 10 cfg cycles */
#ifdef SIMULATION
	idelay(8);
#else
	udelay_dfs(1);
#endif

	do {
		reg_val = READREG32(VA_DRAMPUB_PGSR0) & 0x1;
	} while (reg_val != 1);


	reg_val = READREG32(VA_DRAMPUB_PGSR0) & 0x7FFF0000;
	if (reg_val != 0) {
		dramfreq_printk("errs(PGSR0:%x) happen !\n", reg_val);
		ERR_FLG();
	}
}

void ddr_port_disable_dfs(void)
{
	u32 i, port_val;
	/* gating/rst must de-asserted before port_en */
	port_val = 0;

	WRITEREG32(&mem_ctrl->mem_pi_gat, (READREG32(&mem_ctrl->mem_pi_gat) & 0xffff0000) | port_val);
	/* 3001项目中，当XPI端口FIFO不为0的时候，不能够直接复位 */
	/* WRITEREG32(&mem_ctrl->mem_pi_rst, (READREG32(&mem_ctrl->mem_pi_rst)&0xffff0000)|port_val); */
	asm volatile ("dsb");

	for (i = 0; i < PORT_NUM; i++)
		WRITEREG32(MC_PCTRL_(i), 0);
}

/*
 * void ddr_dll_off(ddr_dfs_para_t * sdram_init_t)
 * 功能：  SDRAM DLL OFF
 * 参数：
 * 返回值：NULL
 * 说明：  关DLL(Digital Delay Line)
 */
void ddr_dll_off_dfs(ddr_dfs_para_t *sdram_init_t)
{
	u32 reg_val;
	u32 rnkn_cfg;

	rnkn_cfg = sdram_init_t->rankn;

	if (sdram_init_t->ddr_type == DDR3) {
		reg_val = (READREG32(VA_DRAMPUB_MR1) & ~(1<<0)) | (1<<0);
		mr_wr_dfs(DDR3_MR1, reg_val, rnkn_cfg);
	} else {
		reg_val = (READREG32(VA_DRAMPUB_MR1) & ~(1<<0)) | (1<<0);
		mr_wr_dfs(DDR2_EMR1, reg_val, rnkn_cfg);
	}
}

/*
 * func: void sw_SR(u32 enable)
 * 参数: enable ->  1: enable  0: disable(exit)
 * 说明：是否使能software self-refresh
 */
void sw_sr_dfs(u32 enable)
{
	u32 reg_val;

	reg_val = (READREG32(VA_DRAMC_PWRCTL) & ~(1<<5)) | (enable<<5);
	WRITEREG32(VA_DRAMC_PWRCTL, reg_val);

	asm volatile ("dsb");

	if (enable) {
		/* wait for go into self refresh */
		do {
			reg_val = READREG32(VA_DRAMC_STAT) & 0x3;
		} while (reg_val != 3);

		/* check SR type */
		do {
			reg_val = READREG32(VA_DRAMC_STAT) & 0x30;
		} while (reg_val != 0x20);/* wait for 17cycles */
	} else {
		/* wait for exit self refresh */
		do {
			reg_val = READREG32(VA_DRAMC_STAT) & 0x3;
		} while (reg_val != 1);
	}
}

u32 cal_time2cycle_dfs(u32 clk, u32 time_ns)
{
#if 0
	float cycle;

	cycle = ((float)(time_ns * clk / 1000.0f)) / 1.0f;
	if (cycle > (u32)cycle)
		cycle++;

	return (u32)cycle;
#else
	u32 cycle;

	cycle = time_ns * clk / 1000;
	cycle++;

	return cycle;
#endif
}

u32 timing_div_fix_dfs(u32 timing)
{
	return timing = timing / 2 + timing % 2;
}

void set_ddr_timing_dfs(ddr_dfs_para_t *ddr_dfs_para)
{
	u8 twr2pre, t_faw, tras_max = 0, tras_min = 0;
	u8 t_xp = 0, t_rtp = 0, t_rc = 0;
	u8 twl = 0, trl = 0, trd2wr = 0, twr2rd = 0;
	u16 t_mrd = 0, t_mod = 0;
	u8 t_rcd = 0, t_ccd = 0, t_rrd = 0, t_rp = 0;
	u8 t_cksrx = 0, tcksre = 0, t_ckesr = 0, t_cke = 0;
	u8 txs_dll_x32 = 0, txs_x32 = 0;
	u8 tcl = 0, tcwl = 0, t_wr, t_wtr = 0; /* CL CWL WR */
	u16 t_rfc = 0;
	u32 t_refi;
	u8 tctrl_delay, tphy_wrdata, tphy_wrlat, trddata_en;
	u16 mr0 = 0, mr1 = 0, mr2 = 0, mr3 = 0;
	u32 clk = ddr_dfs_para->freq;
	u8 type = ddr_dfs_para->ddr_type;
	u32 reg_val;

	reg_dramc_dramtmg0_t dramtmg0;
	reg_dramc_dramtmg1_t dramtmg1;
	reg_dramc_dramtmg2_t dramtmg2;
	reg_dramc_dramtmg3_t dramtmg3;
	reg_dramc_dramtmg4_t dramtmg4;
	reg_dramc_dramtmg5_t dramtmg5;
	reg_dramc_dramtmg8_t dramtmg8;
	reg_dramc_dfitmg0_t  dfitmg0;
	reg_dramc_swctl_t    swctl;
	reg_drampub_dtpr0_t  dtpr0;
	reg_drampub_dtpr1_t  dtpr1;
	reg_drampub_dtpr2_t  dtpr2;

	switch (type) {
	case DDR2:
		if (clk <= 400)
			tcl = 6;
		else
			tcl = 7;
		tcwl = tcl-1; /* CWL = WL = RL-1 */
		break;
	case DDR3:
		if (clk <= 400) {
			tcl = 6;
			tcwl = 5;
		} else if (clk <= 533) {
			tcl = 8;
			tcwl = 6;
		} else if (clk <= 666) {
			tcl = 10;
			tcwl = 7;
		} else if (clk <= 800) {
			tcl = 11;
			tcwl = 8;
		} else {
			tcl = 11;
			tcwl = 10;
		}
		break;
	default:
		ERR_FLG();
	}

	t_wr = cal_time2cycle_dfs(clk, 15);

	switch (type) {
	case DDR2:
		if (t_wr < 2)
			t_wr = 2;
		break;
	case DDR3:
		if (t_wr < 5)
			t_wr = 5;
		if (t_wr == 9)
			t_wr = 10;
		if (t_wr == 11)
			t_wr = 12;
		if ((t_wr >= 13))
			t_wr = 14;
		break;
	}

	t_faw = cal_time2cycle_dfs(clk, 50);
	t_refi = cal_time2cycle_dfs(clk, 7800); /* expecially for ddr3 */
	trl = tcl;

	switch (type) {
	case DDR2:
		t_rfc = cal_time2cycle_dfs(clk, 327);
		tras_max = cal_time2cycle_dfs(clk, 70000) / 1024;
		tras_min = cal_time2cycle_dfs(clk, 45);
		t_xp = 2;

		t_rtp = cal_time2cycle_dfs(clk, 8);
		if (t_rtp < 2)
			t_rtp = 2;
		t_rtp += 2; /* tAL + BL / 2 + max(tRTP, 2) - 2 // rd2pre */

		t_rc = cal_time2cycle_dfs(clk, 65);
		twl = trl - 1;
		trd2wr = trl + 4 + 2 - twl + 1; /* RL + BL / 2 + 2 - WL (+ 1 if extended) */

		t_wtr = cal_time2cycle_dfs(clk, 10);
		if (t_wtr < 2)
			t_wtr = 2;

		twr2rd = twl + 4 + t_wtr;
		t_mrd = 2;
		t_rcd = cal_time2cycle_dfs(clk, 15);
		t_ccd = 2;
		t_rrd = cal_time2cycle_dfs(clk, 10);
		t_rp = cal_time2cycle_dfs(clk, 20);
		t_cksrx = 2;
		tcksre = 2;
		t_cke = 3;
		t_ckesr = t_cke;
		txs_dll_x32 = 7; /* 200 / 32 + 1; */

		txs_x32 = cal_time2cycle_dfs(clk, 338) / 32 + 1;
		if (txs_x32 < txs_dll_x32)
			txs_x32 = txs_dll_x32;

		break;
	case DDR3:
		t_rfc = cal_time2cycle_dfs(clk, 350);
		tras_max = 9 * t_refi / 1024;
		tras_min = cal_time2cycle_dfs(clk, 38);
		t_rc = cal_time2cycle_dfs(clk, 53);

		t_xp = cal_time2cycle_dfs(clk, 8);
		if (t_xp < 3)
			t_xp = 3;

		t_rtp = cal_time2cycle_dfs(clk, 8);
		if (t_rtp < 4)
			t_rtp = 4;

		twl = tcwl;
		trd2wr = trl + 4 + 2 - twl + 1; /* RL + BL / 2 + 2 - WL (+ 1 if extended) */

		t_wtr = cal_time2cycle_dfs(clk, 8);
		if (t_wtr < 4)
			t_wtr = 4;

		twr2rd = tcwl + 4 + t_wtr; /* CWL + BL / 2 + tWTR */
		t_mrd = 4;

		t_mod = cal_time2cycle_dfs(clk, 15);
		if (t_mod < 12)
			t_mod = 12;

		t_rcd = cal_time2cycle_dfs(clk, 15);
		t_ccd = 4;

		t_rrd = cal_time2cycle_dfs(clk, 10);
		if (t_rrd < 4)
			t_rrd = 4;

		t_rp = cal_time2cycle_dfs(clk, 15);

		t_cksrx = cal_time2cycle_dfs(clk, 10);
		if (t_cksrx < 5)
			t_cksrx = 5;

		tcksre = cal_time2cycle_dfs(clk, 10);
		if (tcksre < 5)
			tcksre = 5;

		t_cke = cal_time2cycle_dfs(clk, 8);
		if (t_cke < 3)
			t_cke = 3;

		t_ckesr = t_cke + 1;
		txs_dll_x32 = 16; /* 516 / 32 */
		txs_x32 = cal_time2cycle_dfs(clk, 360) / 32 + 1;
		break;
	default:
		break;
	}

	twr2pre = twl + 4 + t_wr; /* WL + BL / 2 + tWR */

	tctrl_delay = 3;
	tphy_wrdata = 1;

	if (twl > 2) {
		if (twl % 2)
			tphy_wrlat = (twl-3) / 2;
		else
			tphy_wrlat = (twl-4) / 2;
	} else {
		tphy_wrlat = 0;
	}

	if (trl > 2) {
		if (trl % 2)
			trddata_en = (trl-3) / 2;
		else
			trddata_en = (trl-4) / 2;
	} else {
		trddata_en = 0;
	}

	switch (type) {
	case DDR2:
		mr0 = (3) | ((t_wr-1) << 9) | (tcl << 4);
		mr1 = 0x40;
		mr2 = 0x0;
		mr3 = 0x0;
		break;
	case DDR3:
		if (t_wr == 16)
			mr0 = 0;
		else if (t_wr < 10)
			mr0 = (t_wr-4) << 9;
		else if (t_wr < 16)
			mr0 = (t_wr / 2) << 9;

		if ((tcl < 12) && (tcl > 4))
			mr0 |= ((tcl - 4) << 4);
		else if (tcl > 11)
			mr0 |= ((1 << 2) | ((tcl - 12) << 4));

		mr1 = 0x40;
		mr2 = ((tcwl-5) << 3) | (2 << 9);

		if (clk <= 408) {
			mr1 = 0x00;
			mr2 = ((tcwl-5) << 3) | (0 << 9);
		}

		mr3 = 0x0;
		break;
	default:
		break;
	}

	ddr_dfs_para->dram_mr0 = mr0;
	ddr_dfs_para->dram_mr1 = mr1;
	ddr_dfs_para->dram_mr2 = mr2;
	ddr_dfs_para->dram_mr3 = mr3;

	dtpr0.val = (t_rc << 26) | (t_rrd << 22) | (tras_min << 16) | (t_rcd << 12) | (t_rp << 8) | (t_wtr << 4) | (t_rtp << 0);
	WRITEREG32(&drampub->dtpr0, dtpr0.val);

	dtpr1.val = (6 << 26) | (0x28 << 20) | (t_rfc << 11) | (t_faw << 5) | (t_mod << 2) | (t_mrd << 0);
	WRITEREG32(&drampub->dtpr1, dtpr1.val);

	dtpr2.val = (0x200 << 19) | (t_cke << 15) | (t_xp << 10) | (txs_x32 * 32 << 0);
	WRITEREG32(&drampub->dtpr2, dtpr2.val);

	ddr_dfs_para->t_rfc = t_rfc;
	ddr_dfs_para->t_refi = t_refi;

	swctl.val = 0;
	WRITEREG32(&dramc->swctl, swctl.val);

	tras_min = timing_div_fix_dfs(tras_min);
	tras_max = timing_div_fix_dfs(tras_max);
	t_faw = timing_div_fix_dfs(t_faw);
	twr2pre = timing_div_fix_dfs(twr2pre);
	dramtmg0.val = (twr2pre << 24) | (t_faw << 16) | (tras_max << 8) | (tras_min << 0);
	WRITEREG32(&dramc->dramtmg0, dramtmg0.val);

	t_xp = timing_div_fix_dfs(t_xp);
	t_rtp = timing_div_fix_dfs(t_rtp); /* rd2pre */
	t_rc = timing_div_fix_dfs(t_rc);
	dramtmg1.val = (t_xp << 16) | (t_rtp << 8) | (t_rc << 0);
	WRITEREG32(&dramc->dramtmg1, dramtmg1.val);

	trd2wr = timing_div_fix_dfs(trd2wr);
	twr2rd = timing_div_fix_dfs(twr2rd);
	dramtmg2.val = (trd2wr << 8) | (twr2rd << 0);
	WRITEREG32(&dramc->dramtmg2, dramtmg2.val);

	t_mrd = timing_div_fix_dfs(t_mrd);
	t_mod = timing_div_fix_dfs(t_mod);
	dramtmg3.val = (t_mrd << 12) | (t_mod << 0);
	WRITEREG32(&dramc->dramtmg3, dramtmg3.val);

	t_rcd = timing_div_fix_dfs(t_rcd);
	t_ccd = timing_div_fix_dfs(t_ccd);
	t_rrd = timing_div_fix_dfs(t_rrd);
	t_rp = timing_div_fix_dfs(t_rp);
	dramtmg4.val = (t_rcd << 24) | (t_ccd << 16) | (t_rrd << 8) | (t_rp << 0);
	WRITEREG32(&dramc->dramtmg4, dramtmg4.val);

	t_cksrx = timing_div_fix_dfs(t_cksrx);
	tcksre = timing_div_fix_dfs(tcksre);
	t_ckesr = timing_div_fix_dfs(t_ckesr);
	t_cke = timing_div_fix_dfs(t_cke);
	dramtmg5.val = (t_cksrx << 24) | (tcksre << 16) | (t_ckesr << 8) | (t_cke << 0);
	WRITEREG32(&dramc->dramtmg5, dramtmg5.val);

	txs_dll_x32 = timing_div_fix_dfs(txs_dll_x32);
	txs_x32 = timing_div_fix_dfs(txs_x32);
	dramtmg8.val = (txs_dll_x32 << 8) | (txs_x32 << 0);
	WRITEREG32(&dramc->dramtmg8, dramtmg8.val);

	dfitmg0.val = (tctrl_delay << 24) | (trddata_en << 16) | (tphy_wrdata << 8) | (tphy_wrlat << 0);
	WRITEREG32(&dramc->dfitmg0, dfitmg0.val);

	swctl.val = 1;
	WRITEREG32(&dramc->swctl, swctl.val);

	do {
		reg_val = READREG32(&dramc->swstat) & 0x1;/* Register programming done */
	} while (reg_val != 1);

}

void set_refresh_timing_dfs(ddr_dfs_para_t *ddr_dfs_para)
{
	reg_dramc_rfshtmg_t  rfshtmg;
	reg_dramc_rfshctl0_t rfshctl0;
	reg_dramc_rfshctl3_t rfshctl3;

	rfshtmg.val = (timing_div_fix_dfs(ddr_dfs_para->t_rfc) << 0) | ((timing_div_fix_dfs(ddr_dfs_para->t_refi) / 32 + 1) << 16);		/* tRFC(min) */
	WRITEREG32(&dramc->rfshtmg, rfshtmg.val);

	rfshctl0.val = (READREG32(&dramc->rfshctl0) & (~(0x1f << 4))) | (3 << 4);
	WRITEREG32(&dramc->rfshctl0, rfshctl0.val);

	rfshctl3.val = READREG32(&dramc->rfshctl3) & (~(1 << 1));
	WRITEREG32(&dramc->rfshctl3, rfshctl3.val);

	rfshctl3.val = READREG32(&dramc->rfshctl3) | (1 << 1); /* toggle 0->1 */
	WRITEREG32(&dramc->rfshctl3, rfshctl3.val);
}

void update_mrs_chg_freq_dfs(ddr_dfs_para_t *sdram_init_t)
{
	u32 reg_val;
	u32 rnkn_cfg;

	rnkn_cfg = sdram_init_t->rankn;

	if (sdram_init_t->ddr_type == DDR3) {
		mr_wr_dfs(DDR3_MR0, sdram_init_t->dram_mr0, rnkn_cfg);
		mr_wr_dfs(DDR3_MR1, sdram_init_t->dram_mr1, rnkn_cfg);
		mr_wr_dfs(DDR3_MR2, sdram_init_t->dram_mr2, rnkn_cfg);
		/* DLL reset */
		reg_val = (READREG32(VA_DRAMPUB_MR0) & ~(0x1<<8)) | (1<<8);
		mr_wr_dfs(DDR3_MR0, reg_val, rnkn_cfg);/* WR */
	} else {
		mr_wr_dfs(DDR2_MR, sdram_init_t->dram_mr0, rnkn_cfg);
		mr_wr_dfs(DDR2_EMR1, sdram_init_t->dram_mr1, rnkn_cfg);
	}
}

void set_phy_pll_para_dfs(ddr_dfs_para_t *ddr_dfs_para)
{
	reg_drampub_pllcr_t pllcr;

	pllcr.val = READREG32(&drampub->pllcr);
	if ((ddr_dfs_para->freq / 2) < 275)
		pllcr.bits.frqsel = 1;/* ref_clk: 166-300MHz */
	else
		pllcr.bits.frqsel = 0;/* ref_clk: 250-400MHz */
	WRITEREG32(&drampub->pllcr, pllcr.val);

#ifndef SIMULATION
	udelay_dfs(15);/* >= 2tREFI */
#endif
}

void ddr_port_enable_dfs(void)
{
	u32 i, port_val;

	/* gating / rst must de-asserted before port_en */
	port_val = (1 << PORT_NUM) - 1;
	WRITEREG32(&mem_ctrl->mem_pi_gat, (READREG32(&mem_ctrl->mem_pi_gat) & 0xffff0000) | port_val);
	WRITEREG32(&mem_ctrl->mem_pi_rst, (READREG32(&mem_ctrl->mem_pi_rst) & 0xffff0000) | port_val);

	asm volatile ("dsb");

	for (i = 0; i < PORT_NUM; i++)
		WRITEREG32(MC_PCTRL_(i), 1);
}

/* div由上层传入，div=1为按booster阶段的clock，div=2及booster频点除2，div为其他无效 */
u32 ddr_dfs(u32 div, ddr_dfs_para_t *ddr_dfs_para)
{
#ifndef CLK_DIV
	reg_prcm_sdram_pll_en_t sdram_pll_enable;
	reg_prcm_sdram_pll_fac_t sdram_pll_factor;
#if (!defined SIMULATION) && (!defined FPGA)
	reg_prcm_sdram_pll_stat_t sdram_pll_status;
#endif
#endif
	u32 i;
	u32 reg_val;
	reg_drampub_dtcr_t dtcr;
	reg_drampub_pir_t pir;

	dramfreq_printk("div=%d freq=%d rankn=%d ddr_type=%d size=%x\n",
				div, ddr_dfs_para->freq, ddr_dfs_para->rankn,
				ddr_dfs_para->ddr_type, ddr_dfs_para->size);

	if (div == 1)
		ddr_dfs_para->freq = ddr_dfs_para->freq;
	else if (div == 2)
		ddr_dfs_para->freq = ddr_dfs_para->freq / 2;
	else
		asm volatile ("b .");

	/* data save */
	for (i = 0; i < DATA_NUM; i++)
		training_data_save[i] = READREG32(SDRAM_BASE + i * 0x4);

	for (i = 0; i < DATA_NUM; i++)
		training_data_save[i + DATA_NUM] = READREG32(SDRAM_BASE + ddr_dfs_para->size / 2 + i * 0x4);

	asm volatile ("isb");
	asm volatile ("dsb");

	/* step1 禁止SDRAM的访问,port disable */
	ddr_port_disable_dfs();

	/* step 2 Poll PSTAT.rd_port_busy_n=0 and PSTAT.wr_port_busy_n=0  wait idle */
	do {
		reg_val = READREG32(VA_DRAMC_PSTAT);
	} while (reg_val);

	/* step 3 Write 0 to SBRCTL.scrub_en. Disable SBR */
	/* reg_val = READREG32(MC_SBRCTL) & ~(1<<0) | (0<<0); */
	/* WRITEREG32(MC_SBRCTL,reg_val); */

#if 0
	/* step 4 Poll SBRSTAT.scrub_busy=0 */
	do {
		reg_val = READREG32(MC_SBRSTAT) & 0x1;
	} while (reg_val);
#endif

	/* step 5 LPDDR2/3/4 only */

	/* step 6 DBG1.dis_hif = 1 */
	reg_val = (READREG32(VA_DRAMC_DBG1) & ~(1<<1)) | (1<<1);
	WRITEREG32(VA_DRAMC_DBG1, reg_val);

	/* step 7 Poll DBGCAM.dbg_wr_q_empty and DBGCAM.dbg_rd_q_empty */
	do {
		reg_val = READREG32(VA_DRAMC_DBGCAM) & (3<<25);
	} while (reg_val != 0x06000000);

	/* step 8 for DDR4 only */

	/* step 9 Set DFILPCFG0.dfi_lp_en_sr = 0 */
	reg_val = (READREG32(VA_DRAMC_DFILPCFG0) & ~(1<<8)) | (0<<8);
	WRITEREG32(VA_DRAMC_DFILPCFG0, reg_val);

	do {
		reg_val = READREG32(VA_DRAMC_DFISTAT) & 0x2;/* dfi_lp_ack */
	} while (reg_val);

	/* step 10/11/12 put SDRAM into sw self-refresh and ack */
	/* DLL-OFF */
	ddr_dll_off_dfs(ddr_dfs_para);

	sw_sr_dfs(1);

	/* step 13 is required if gen2 multiPHY is used */

	/* step 14 Change the clock frequency to the desired value with no glitches */
	/* glitch是毛刺的意思，在调频过程不可出现毛刺，包括数字后除频 */
#ifdef CLK_DIV
	/* set sdram clk ,div=1 除2，div=2 除4*/
	WRITEREG32(&prcm->sdram_clk_ctrl, READREG32(&prcm->sdram_clk_ctrl) & (~(1U<<31)));
	WRITEREG32(&prcm->sdram_clk_ctrl, (1 << 0) | (div << 4) | (1u << 31));
#else
	WRITEREG32(&prcm->sdram_pll_en, 0);
	WRITEREG32(&prcm->sdram_pll_mod, 3);
	WRITEREG32(&prcm->sdram_pll_tune0, 0x1059E18);

	reg_val = (ddr_dfs_para->freq / 24) << 8; /* ddr_para->freq / 24 */
	WRITEREG32(&prcm->sdram_pll_fac, reg_val);
	WRITEREG32(&prcm->sdram_pll_tune2, 640<<16);

	/* eanble pll */
	sdram_pll_enable.val = READREG32(&prcm->sdram_pll_en);
	/* if ((sdram_pll_enable.val & 0x7) != 0x7) { */
		/* enable ENM */
		sdram_pll_enable.val = READREG32(&prcm->sdram_pll_en) | (1 << 1);
		WRITEREG32(&prcm->sdram_pll_en, sdram_pll_enable.val);
#ifndef SIMULATION
		udelay_dfs(100);
#endif
		/* enable ENP */
		sdram_pll_enable.val = READREG32(&prcm->sdram_pll_en) | (1 << 0);
		WRITEREG32(&prcm->sdram_pll_en, sdram_pll_enable.val);
#ifndef SIMULATION
		udelay_dfs(10);
#endif
		/* enable OGAT */
		sdram_pll_enable.val = READREG32(&prcm->sdram_pll_en) | (1 << 2);
		WRITEREG32(&prcm->sdram_pll_en, sdram_pll_enable.val);
		udelay_dfs(1000);
	/* } */

	/* wait for pll locked */
#if (!defined SIMULATION) && (!defined FPGA)
	do {
		sdram_pll_status.val = READREG32(&prcm->sdram_pll_stat) & 0x7;
	} while (sdram_pll_status.val != 0x7); /* locked &steady */
#endif
#endif /* CLK_DIV */
	set_phy_pll_para_dfs(ddr_dfs_para);

	/* Un-initialized PLL */
	WRITEREG32(&drampub->pllcr, READREG32(&drampub->pllcr) | (0x3<<29) | (1<<10));
	udelay_dfs(10);

	WRITEREG32(&drampub->pllcr, READREG32(&drampub->pllcr) & (~(0x1<<29)));
	udelay_dfs(10);

	WRITEREG32(&drampub->pllcr, READREG32(&drampub->pllcr) & (~(0x1<<30)));
	udelay_dfs(10);

	WRITEREG32(&drampub->pllcr, READREG32(&drampub->pllcr) & (~(0x1<<10)));
	udelay_dfs(10);

	WRITEREG32(&drampub->pir, 0x11);

	/* wait initial finished */
	do {
		pir.val = READREG32(&drampub->pir);
	} while (pir.bits.init == 1);

	while (((READREG32(&drampub->pgsr0) >> 31) & 0x1) != 1)
		;

	/* step 15 Update any registers which required to chg for the new frequency */

	set_ddr_timing_dfs(ddr_dfs_para);

	set_refresh_timing_dfs(ddr_dfs_para);

#if 0
	if (ddr_dfs_para->freq <= 408) {
		/* set soc odt disable */
		reg_val = read_reg(VA_DRAMPUB_DX0GCR)&(~(3<<9));
		write_reg(VA_DRAMPUB_DX0GCR, reg_val);
		reg_val = read_reg(VA_DRAMPUB_DX1GCR)&(~(3<<9));
		write_reg(VA_DRAMPUB_DX1GCR, reg_val);
		reg_val = read_reg(VA_DRAMPUB_DX2GCR)&(~(3<<9));
		write_reg(VA_DRAMPUB_DX2GCR, reg_val);
		reg_val = read_reg(VA_DRAMPUB_DX3GCR)&(~(3<<9));
		write_reg(VA_DRAMPUB_DX3GCR, reg_val);
		reg_val = read_reg(VA_DRAMPUB_DXCCR)&(~(1<<0));
		write_reg(VA_DRAMPUB_DXCCR, reg_val);
	} else {
		/* set soc odt dynamic on */
		reg_val = read_reg(VA_DRAMPUB_DX0GCR)|(3<<9);
		write_reg(VA_DRAMPUB_DX0GCR, reg_val);
		reg_val = read_reg(VA_DRAMPUB_DX1GCR)|(3<<9);
		write_reg(VA_DRAMPUB_DX1GCR, reg_val);
		reg_val = read_reg(VA_DRAMPUB_DX2GCR)|(3<<9);
		write_reg(VA_DRAMPUB_DX2GCR, reg_val);
		reg_val = read_reg(VA_DRAMPUB_DX3GCR)|(3<<9);
		write_reg(VA_DRAMPUB_DX3GCR, reg_val);
		reg_val = read_reg(VA_DRAMPUB_DXCCR)|(1<<0);
		write_reg(VA_DRAMPUB_DXCCR, reg_val);
	}
#endif

	/* step 16 If required, trigger the initialization in the PHY.step15已经做了 */

	/* step 17 Exit the self-refresh state */
	sw_sr_dfs(0);

	/* step 18 Reset DFILPCFG0.dfi_lp_en_sr = 1 */
	reg_val = (READREG32(VA_DRAMC_DFILPCFG0) & ~(1<<8)) | (1<<8);
	WRITEREG32(VA_DRAMC_DFILPCFG0, reg_val);

	/* step 19 re-program timing register in MRS */
	update_mrs_chg_freq_dfs(ddr_dfs_para);

	dtcr.val = (READREG32(&drampub->dtcr) & (~(0x1ff << 23)))
				| (ddr_dfs_para->rankn << 24)
				| (1 << 23)
				| (ddr_dfs_para->rankn << 28);
	WRITEREG32(&drampub->dtcr, dtcr.val);

	phy_ddl_training();

	/* step 20 Enable HIF commands by setting DBG1.dis_hif=0 */
	reg_val = (READREG32(VA_DRAMC_DBG1) & ~(1<<1)) | (0<<1);
	WRITEREG32(VA_DRAMC_DBG1, reg_val);

	/* step 21 Reset DERATEEN.derate_enable = 1	(LPDDR2/3/4 only) */

	/* step 22 Write 1 to PCTRL_n.port_en */
	ddr_port_enable_dfs();

	/* data re-write */
	for (i = 0; i < DATA_NUM; i++)
		WRITEREG32(SDRAM_BASE + i * 0x4, training_data_save[i]);

	for (i = 0; i < DATA_NUM; i++)
		WRITEREG32(SDRAM_BASE + ddr_dfs_para->size / 2 + i * 0x4,
				training_data_save[i + DATA_NUM]);

	asm volatile ("dsb");

	return 0;
}
