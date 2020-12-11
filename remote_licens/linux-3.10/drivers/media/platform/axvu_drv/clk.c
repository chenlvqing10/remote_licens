#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/io.h>
#include "utils.h"
#include "ax_reg.h"

static int ax_powered_on = 0;
#define AX_CLK_RATE 360
#define ADPLL_ON 1




static unsigned int Re_Reg_RAW(unsigned int reg)
{
	unsigned int value = readl((void *)reg);
	return value;
}

static void Wr_Reg_RAW(unsigned int value, unsigned int reg)
{
	writel(value, (void *)reg);
}
#if 0
static void ax_prcm_enable(void)
{
	/* ENM */
	unsigned int rdval = Re_Reg_RAW(AX_PLL_ENB);
	Wr_Reg_RAW(rdval | (1<<1), AX_PLL_ENB);
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	udelay(100);

	/* ENP */
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	Wr_Reg_RAW(rdval | (1<<0), AX_PLL_ENB);
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	udelay(10);

	/* ENP */
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	Wr_Reg_RAW(rdval | (1<<2), AX_PLL_ENB);
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	udelay(10);
}

static void ax_prcm_disable(void)
{
	/* ENP */
	unsigned int rdval = Re_Reg_RAW(AX_PLL_ENB);
	Wr_Reg_RAW(rdval | (0<<2), AX_PLL_ENB);
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	udelay(10);

	/* ENP */
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	Wr_Reg_RAW(rdval | (0<<0), AX_PLL_ENB);
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	udelay(10);

	/* ENM */
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	Wr_Reg_RAW(rdval | (0<<1), AX_PLL_ENB);
	rdval = Re_Reg_RAW(AX_PLL_ENB);
	udelay(100);
}

static int get_prcm_clk_status(void)
{
	u32 rdsat = Re_Reg_RAW(AX_PLL_STAT);
	return ((rdsat & 0x7) == 0x7);
}
#endif

/*
 * -----------------------------------------------------------------------------
 * wait AD-pll lock
 * -----------------------------------------------------------------------------
 */
static void _wait_for_ax_adpll_lock(u32 is_cvu, unsigned int base_reg)
{
	u32 adpll_status_addr;
	u32 reg_val;

	adpll_status_addr = is_cvu ? (CVU_ADPLL_STAT) : (ENU_ADPLL_STAT);
	do {
		reg_val = Re_Reg_RAW(base_reg + adpll_status_addr);
	} while ((reg_val & 0x80008000) != 0x80008000);
}

static void _ax_clk_sel(u32 is_cvu, u32 sel, unsigned int base_reg)
{
	u32 clk_ctrl_addr;
	u32 reg_val;

	clk_ctrl_addr = is_cvu ? (CVU_ADPLL_CTRL) : (ENU_ADPLL_CTRL);

	reg_val = Re_Reg_RAW(base_reg + clk_ctrl_addr);
	reg_val &= 0xfffffffe;
	reg_val |= sel;

	Wr_Reg_RAW(reg_val, base_reg + clk_ctrl_addr);
}

/*
 * -----------------------------------------------------------------------------
 * set AD-pll freq
 * -----------------------------------------------------------------------------
 */
int set_adpll_freq_vu(u32 is_cvu, u32 freq, unsigned int base_reg)
{
	u32 clk_ctrl_addr;
	u32 adpll_fac_addr;
	u32 n, post_div, enable;
	u32 reg_val;

	clk_ctrl_addr = is_cvu ? (CVU_ADPLL_CTRL) : (ENU_ADPLL_CTRL);
	adpll_fac_addr = is_cvu ? (CVU_ADPLL_FAC) : (ENU_ADPLL_FAC);

	/* calculate pll fac */
	if ((freq >= (408)) && (freq <= (1008))) {
		n = freq / 24;
		post_div = 0;
	} else if ((freq > (102)) && (freq < (204))) {
		n = (freq * 4) / 24;
		post_div = 3;
	} else if ((freq >= (204)) && (freq < (408))) {
		n = (freq * 2) / 24;
		post_div = 1;
	} else
		return -1;

	/* factor */
	reg_val = Re_Reg_RAW(base_reg + adpll_fac_addr);
	reg_val &= 0xef0fff00;
	reg_val |= ((1 << 31) | (post_div << 20) | n);
	Wr_Reg_RAW(reg_val, base_reg + adpll_fac_addr);

	_wait_for_ax_adpll_lock(is_cvu, base_reg);

	/* adpll gating enable */
	enable = 1;
	reg_val = Re_Reg_RAW(base_reg + clk_ctrl_addr);
	reg_val |= (enable << 31);
	Wr_Reg_RAW(reg_val, base_reg + clk_ctrl_addr);

	return 0;
}
#if 0
static void set_ax_adpll_enable(u32 is_cvu, u32 enable, unsigned int base_reg)
{
	u32 adpll_fac_addr;
	u32 reg_val;

	adpll_fac_addr = is_cvu ? (CVU_ADPLL_FAC) : (ENU_ADPLL_FAC);

	/* factor */
	reg_val = Re_Reg_RAW(base_reg + adpll_fac_addr);
	reg_val &= 0x7fffffff;
	reg_val |= (enable << 31);
	Wr_Reg_RAW(reg_val, base_reg + adpll_fac_addr);
}

static void set_ax_adpll_n_div(u32 is_cvu, u32 n, u32 post_div,
		unsigned int base_reg)
{
	u32 adpll_fac_addr;
	u32 reg_val;

	adpll_fac_addr = is_cvu ? (CVU_ADPLL_FAC) : (ENU_ADPLL_FAC);

	/* factor */
	reg_val = Re_Reg_RAW(base_reg + adpll_fac_addr);
	reg_val &= 0xff0fff00;
	reg_val |= ((post_div << 20) | n);
	Wr_Reg_RAW(reg_val, base_reg + adpll_fac_addr);
}
#endif

void set_ax_internal_gating_vu(u32 is_cvu, u32 enable, unsigned int base_reg)
{
	u32 clk_ctrl_addr;
	u32 reg_val;

	clk_ctrl_addr = is_cvu ? (CVU_ADPLL_CTRL) : (ENU_ADPLL_CTRL);

	/* adpll gating enable */
	reg_val = Re_Reg_RAW(base_reg + clk_ctrl_addr);
	reg_val &= 0x7fffffff;
	reg_val |= (enable << 31);
	Wr_Reg_RAW(reg_val, base_reg + clk_ctrl_addr);
}
#if 0
static void ax_power_on(void)
{
	/* ahb bus enable gating */
	if (ax_powered_on == 0) {
		u32 rdval = Re_Reg_RAW(AHB_GAT1);
		Wr_Reg_RAW(rdval | (1<<13), AHB_GAT1);
		rdval = Re_Reg_RAW(AHB_GAT1);

		/* AHB BUS Reset/ax Reset */
		rdval = Re_Reg_RAW(AHB_RST1);
		Wr_Reg_RAW((rdval & 0xffffdfff), AHB_RST1);
		udelay(1);
		Wr_Reg_RAW((rdval | (1<<13)), AHB_RST1);

		/* AXI BUS CLOCK GATING */
		rdval = Re_Reg_RAW(AXI_GAT);
		Wr_Reg_RAW((rdval | (1<<12)), AXI_GAT);
		rdval = Re_Reg_RAW(AXI_GAT);
	}
	ax_powered_on++;
}

static void ax_power_off(void)
{
	ax_powered_on--;
	if (ax_powered_on == 0) {
		/* ahb bus enable gating */
		u32 rdval = Re_Reg_RAW(AHB_GAT1);
		Wr_Reg_RAW(rdval & 0xffffdfff, AHB_GAT1);
		rdval = Re_Reg_RAW(AHB_GAT1);

		/* AHB BUS Reset/ax Reset */
		rdval = Re_Reg_RAW(AHB_RST1);
		Wr_Reg_RAW((rdval & 0xffffdfff), AHB_RST1);

		/* AXI BUS CLOCK GATING */
		rdval = Re_Reg_RAW(AXI_GAT);
		Wr_Reg_RAW((rdval & 0xffff7fff), AXI_GAT);
		rdval = Re_Reg_RAW(AXI_GAT);
	}
}
#endif

#define ADPLL_FREE_RUN_FREQ_CONF_NUM (132)
static double adpll_freq_table[ADPLL_FREE_RUN_FREQ_CONF_NUM][3] =
{
	{0,0,95.2},        {0,1024,127.6},    {0,2048,183.2},    {0,3072,218.8},      {31,0,262},           {0,4096,266},         {31,1024,296.8},     {0,5120,301.6},
	{31,2048,341.2},   {0,6144,344.4},    {31,3072,375.6},   {0,7168,379.6},      {63,0,416},           {31,4096,416.4},      {0,8192,420},        {31,5120,450},
	{63,1024,452.4},   {0,9216,454.8},    {31,6144,488.4},   {63,2048,491.6},     {0,10240,492},        {31,7168,520.8},      {63,3072,523.6},     {0,11264,524.8},
	{31,8192,557.2},   {0,12288,560},     {63,4096,560.4},   {31,9216,589.2},     {63,5120,591.6},      {0,13312,592.4},      {31,10240,623.6},    {63,6144,625.6},
	{0,14336,625.6},   {31,11264,653.2},  {63,7168,656.4},   {0,15360,657.2},     {31,12288,686},       {127,0,686},          {0,16384,689.2},     {63,8192,689.6},
	{31,12800,700.8},  {31,13312,715.6},  {63,9216,719.2},   {0,17408,719.2},     {0,17749,729.2},      {0,18090,739.2},      {31,14336,746.4},    {127,2048,746.4},
	{0,18432,749.2},   {0, 18773, 758.6}, {0, 19114, 768.0}, {0,19456,777.6},     {0, 19797, 787.6},    {0, 20138, 797.6},    {0,20480,806.4},     {63,12288,808},
	{127,5120,831.6},  {31,17408,832.4},  {0,21504,834},     {63,13312,835.2},    {31,18432,859.2},     {127,6144,860},       {0,22528,861.6},     {63,14336,864.4},
	{127,7168,886},    {31,19456,886.4},  {0,23552,888.4},   {63,15360,890.8},    {31,20480,913.2},     {127,8192,914},       {0,24576,914.8},     {63,16384,918.4},
	{31,21504,938.4},  {127,9216,939.2},  {0,25600,941.2},   {63,17408,944},      {31,22528,963.6},     {127,10240,966},      {0,26624,966},       {63,18432,969.6},
	{31,23552,989.2},  {127,11264,990.4}, {0,27648,990.8},   {63,19456,994},      {31,24576,1013.6},    {0,28672,1014.4},     {127,12288,1015.6},  {63,20480,1018.4},
	{31,25600,1037.2}, {0,29696,1038.4},  {127,13312,1039.6},{63,21504,1042.8},   {31,26624,1060.8},    {0,30720,1062},       {127,14336,1064},    {63,22528,1065.6},
	{31,27648,1084},   {0,31744,1085.6},  {127,15360,1087.2},{63,23552,1088.8},   {31,28672,1106},      {0,32767,1107.6},     {127,16384,1111.2},  {63,24576,1111.6},
	{31,29696,1128.8}, {63,25600,1133.2}, {127,17408,1133.6},{31,30720,1150.4},   {63,26624,1154.8},    {127,18432,1156},     {31,31744,1172.8},   {63,27648,1176.8},
	{127,19456,1177.6},{31,32767,1193.2}, {63,28672,1197.2}, {127,20480,1198.8},  {63,29696,1218},      {127,21504,1220},     {63,30720,1238.4},   {127,22528,1240.4},
	{63,31744,1259.2}, {127,23552,1261.6},{63,32767,1278.8}, {127,24576,1281.88}, {127,25600,1301.28},  {127,26624,1320.68},  {127,27648,1340},    {127,28672,1358},
	{127,29696,1377.2},{127,30720,1395.6},{127,31744,1413.6},{127,32767,1431.6},
};

int set_ax_adpll_total_free_run_vu_init(u32 is_cvu, u32 freq, u32 base_reg)
{
	u32 clk_ctrl_addr, adpll_fac_addr, tune0_addr, tune1_addr;
	//u32 n, post_div, pre_div, enable;
	//u32 reg_val;
	double real_freq_mhz, slowfreqerr, fastfreqerr, freq_mhz;
	u32 i;
	u32 dco_range, config_free_run_clock_freq;

	//clk_ctrl_addr = is_cvu ? (AX_BASE_ADDR + 0xb4) : (AX_BASE_ADDR + 0xa0);
	//adpll_fac_addr = is_cvu ? (AX_BASE_ADDR + 0xb8) : (AX_BASE_ADDR + 0xa4);

	clk_ctrl_addr = is_cvu ? (CVU_ADPLL_CTRL) : (ENU_ADPLL_CTRL);
	adpll_fac_addr = is_cvu ? (CVU_ADPLL_FAC) : (ENU_ADPLL_FAC);

	tune0_addr = is_cvu ? (CVU_ADPLL_TUNE0) : (ENU_ADPLL_TUNE0);
	tune1_addr = is_cvu ? (CVU_ADPLL_TUNE1) : (ENU_ADPLL_TUNE1);

	freq_mhz = freq;

	for(i = 0; i < ADPLL_FREE_RUN_FREQ_CONF_NUM; i++)
	{
		if (freq_mhz > adpll_freq_table[i][2])
			continue;
		else if (freq_mhz == adpll_freq_table[i][2])
		{
			real_freq_mhz = adpll_freq_table[i][2];
			dco_range = (u32)adpll_freq_table[i][0];
			config_free_run_clock_freq = (u32)adpll_freq_table[i][1];
			break;
		}
		else
		{
			slowfreqerr = freq_mhz - adpll_freq_table[i-1][2];
			fastfreqerr = adpll_freq_table[i][2] - freq_mhz;
			real_freq_mhz = (slowfreqerr < fastfreqerr) ?
							adpll_freq_table[i-1][2] : adpll_freq_table[i][2];
			dco_range = (u32)((slowfreqerr < fastfreqerr) ?
							adpll_freq_table[i-1][0] : adpll_freq_table[i][0]);
			config_free_run_clock_freq = (u32)((slowfreqerr < fastfreqerr) ?
							adpll_freq_table[i-1][1] : adpll_freq_table[i][1]);
			break;
		}
	}

	if (i == ADPLL_FREE_RUN_FREQ_CONF_NUM)
	{
		//ERR_FLG();
		//printk("invalid freq !\n");
		return -1;
	}

	// disable pll first
	Wr_Reg_RAW(0, base_reg + adpll_fac_addr);

	// tune1
	Wr_Reg_RAW(0x57379120, base_reg + tune1_addr);

	// tune0
	Wr_Reg_RAW(0x03000000, base_reg + tune0_addr);

	// tune0
	Wr_Reg_RAW(0x03007FFF, base_reg + tune0_addr);

	Wr_Reg_RAW(0x80000000, base_reg + adpll_fac_addr);

	/* tune0*/
	Wr_Reg_RAW(0x03000000 | (dco_range << 16) | (config_free_run_clock_freq), base_reg + tune0_addr);
	mdelay(10);

	// enable
	Wr_Reg_RAW(0x80000001, base_reg + clk_ctrl_addr);

	// adpll gating enable
	//enable = 1;
	//reg_val = read_reg32(clk_ctrl_addr);
	//reg_val |= (enable << 31);
	//IO_WR(clk_ctrl_addr, reg_val);

	//printk("%s: dco_range = %d, config_free = %d.\n", __FUNCTION__, dco_range, config_free_run_clock_freq);
	return (int)real_freq_mhz;
}


int set_ax_adpll_total_free_run_vu(u32 is_cvu, u32 freq, u32 base_reg)
{
	u32 clk_ctrl_addr, adpll_fac_addr, tune0_addr, tune1_addr;
	/*u32 n, post_div, pre_div, enable;
	u32 reg_val;
	*/
	double real_freq_mhz, slowfreqerr, fastfreqerr, freq_mhz;
	u32 i;
	u32 dco_range, config_free_run_clock_freq;

	/* clk_ctrl_addr = is_cvu ?
	(AX_BASE_ADDR + 0xb4) : (AX_BASE_ADDR + 0xa0);
	adpll_fac_addr = is_cvu ?
	(AX_BASE_ADDR + 0xb8) : (AX_BASE_ADDR + 0xa4);
	*/

	clk_ctrl_addr = is_cvu ? (CVU_ADPLL_CTRL) : (ENU_ADPLL_CTRL);
	adpll_fac_addr = is_cvu ? (CVU_ADPLL_FAC) : (ENU_ADPLL_FAC);

	tune0_addr = is_cvu ? (CVU_ADPLL_TUNE0) : (ENU_ADPLL_TUNE0);
	tune1_addr = is_cvu ? (CVU_ADPLL_TUNE1) : (ENU_ADPLL_TUNE1);

	freq_mhz = freq;

	for (i = 0; i < ADPLL_FREE_RUN_FREQ_CONF_NUM; i++) {
		if (freq_mhz > adpll_freq_table[i][2])
			continue;
		else if (freq_mhz == adpll_freq_table[i][2]) {
			real_freq_mhz = adpll_freq_table[i][2];
			dco_range = (u32)adpll_freq_table[i][0];
			config_free_run_clock_freq =
					(u32)adpll_freq_table[i][1];
			break;
		} else {
			slowfreqerr = freq_mhz - adpll_freq_table[i-1][2];
			fastfreqerr = adpll_freq_table[i][2] - freq_mhz;
			real_freq_mhz = (slowfreqerr < fastfreqerr) ?
					adpll_freq_table[i-1][2] :
					adpll_freq_table[i][2];
			dco_range = (u32)((slowfreqerr < fastfreqerr) ?
					adpll_freq_table[i-1][0] :
					adpll_freq_table[i][0]);
			config_free_run_clock_freq =
					(u32)((slowfreqerr < fastfreqerr) ?
					adpll_freq_table[i-1][1] :
					adpll_freq_table[i][1]);
			break;
		}
	}

	if (i == ADPLL_FREE_RUN_FREQ_CONF_NUM) {
		printk("invalid freq !\n");
		return -1;
	}

#ifndef PRCM_ON
	Wr_Reg_RAW(0x00000001, base_reg + clk_ctrl_addr);

	Wr_Reg_RAW(0x03000000 | (dco_range << 16) |
		(config_free_run_clock_freq), base_reg + tune0_addr);
	mdelay(10);


	Wr_Reg_RAW(0x80000001, base_reg + clk_ctrl_addr);
#endif

	return (int)real_freq_mhz;
}


int get_adpll_freq_vu(int is_cvu, unsigned int base_reg)
{
	u32 adpll_fac_addr;
	u32 reg_val;
	u32 post_div = 0;
	u32 n = 0;
	u32 mfreq = 0;

	adpll_fac_addr = is_cvu ? (CVU_ADPLL_FAC) : (ENU_ADPLL_FAC);

	reg_val = Re_Reg_RAW(base_reg + adpll_fac_addr);
	post_div = ((reg_val >> 20)&0xf) + 0;
	n = (reg_val&0xff);
	mfreq = n * 24 / (post_div + 1);

	return mfreq;
}

int ax_clk_init_vu(u32 is_cvu, unsigned int base_reg)
{
	/* enable ax pll */
	//u32 rdval = 0;

	/* ax_power_on(); */

#if PRCM_ON /* prcm clk */
	ax_powered_on = 1;
	if (ax_powered_on == 1) {
#ifdef __LINEAR_FREQ__
		rdval = Re_Reg_RAW(AX_PLL_MODE);
		Wr_Reg_RAW((rdval | (3<<0)), AX_PLL_MODE); /* linear adjust */
		rdval = Re_Reg_RAW(AX_PLL_MODE);

		rdval = Re_Reg_RAW(AX_PLL_TUN2);
		/* Integer adjust */
		Wr_Reg_RAW((rdval | (3495<<16)), AX_PLL_TUN2);
		rdval = Re_Reg_RAW(AX_PLL_TUN2);
#else
		rdval = Re_Reg_RAW(AX_PLL_MODE);
		Wr_Reg_RAW((rdval | (0<<0)), AX_PLL_MODE); /* Integer adjust */
		rdval = Re_Reg_RAW(AX_PLL_MODE);
#endif

		/* u32 fac = AX_CLK_RATE / 240; */
		u32 fac = AX_CLK_RATE / 24;
		u32 pre = 0;
		u32 post = 0;

		rdval = Re_Reg_RAW(AX_PLL_FAC);
		Wr_Reg_RAW((rdval | (pre<<0) | (post<<4) | (fac<<8)),
			AX_PLL_FAC); /* Integer adjust */
		rdval = Re_Reg_RAW(AX_PLL_FAC);

		ax_prcm_enable();
		while (!get_prcm_clk_status())
			;

		rdval = Re_Reg_RAW(AX_PLL_CTL);
		Wr_Reg_RAW((rdval | (1<<0)), AX_PLL_CTL); /* Integer adjust */
		rdval = Re_Reg_RAW(AX_PLL_CTL);

		rdval = Re_Reg_RAW(AX_PLL_CTL);
		Wr_Reg_RAW((rdval | (0<<4)), AX_PLL_CTL); /* Integer adjust */
		rdval = Re_Reg_RAW(AX_PLL_CTL);

		rdval = Re_Reg_RAW(AX_PLL_CTL);
		Wr_Reg_RAW((rdval | (1<<31)), AX_PLL_CTL); /* Integer adjust */
		rdval = Re_Reg_RAW(AX_PLL_CTL);
		_ax_clk_sel(is_cvu, 0, base_reg);
	}
#endif

#if ADPLL_ON
	//u32 fac = AX_CLK_RATE / 24000000;
	//set_ax_adpll_n_div(is_cvu, fac, 0, base_reg);
	int freq = set_ax_adpll_total_free_run_vu_init(is_cvu,
					AX_CLK_RATE, base_reg);
	//set_ax_adpll_enable(is_cvu, 1, base_reg);
	//_wait_for_ax_adpll_lock(is_cvu, base_reg);
	_ax_clk_sel(is_cvu, 1, base_reg);
#endif

	return freq;
}

void ax_clk_deinit_vu(void)
{
#if PRCM_ON
	ax_prcm_disable();
#endif
	/* ax_power_off(); */
}

int get_power_on_status_vu(void)
{
	return ax_powered_on;
}
