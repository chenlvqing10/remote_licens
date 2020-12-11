#include <linux/time.h>
#include <linux/delay.h>
#include "utils.h"
#include "ax_reg.h"

static int ax_powered_on = 0;
#define AX_CLK_RATE 360
#define ADPLL_ON 1
#if 0
static void ax_prcm_enable(void){
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

static void ax_prcm_disable(void){
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

static int get_prcm_clk_status(void){
	u32 rdsat = Re_Reg_RAW(AX_PLL_STAT);
	return ((rdsat & 0x7) == 0x7);
}
#endif
/*
 * -----------------------------------------------------------------------------
 * wait AD-pll lock
 * -----------------------------------------------------------------------------
 */
static bool _wait_for_ax_adpll_lock(u32 is_cvu, unsigned int base_reg)
{
	u32 adpll_status_addr;
	u32 reg_val;
	int cnt = 0;

	adpll_status_addr = is_cvu ? (CVU_ADPLL_STAT) : (ENU_ADPLL_STAT);
	do {
		reg_val = Re_Reg_RAW(base_reg + adpll_status_addr);
		mdelay(1);
		ax_err(0, "wait %x,%x\n",
			base_reg + adpll_status_addr, reg_val);
	} while ((reg_val & 0x80008000) != 0x80008000 && ++cnt < 50);

	if (cnt >= 5000) {
		ax_err(1, "wait %x,%x timeout\n",
			base_reg + adpll_status_addr, reg_val);
			return 0;
	}
	return 1;
}

static void _ax_clk_sel(u32 is_cvu, u32 sel, unsigned int base_reg)
{
	u32 clk_ctrl_addr;
	u32 reg_val;

	clk_ctrl_addr = is_cvu ? (CVU_ADPLL_CTRL) : (ENU_ADPLL_CTRL);

	reg_val = Re_Reg_RAW(base_reg + clk_ctrl_addr);
	reg_val &= 0xfffffffe;
	reg_val |= sel;
	ax_err(1, "_ax_clk_sel %x,%x\n", base_reg + clk_ctrl_addr, reg_val);
	Wr_Reg_RAW(reg_val, base_reg + clk_ctrl_addr);
}

/*
 * -----------------------------------------------------------------------------
 * set AD-pll freq
 * -----------------------------------------------------------------------------
 */
int set_adpll_freq(u32 is_cvu, u32 freq, int bwait, unsigned int base_reg)
{
	u32 clk_ctrl_addr;
	u32 adpll_fac_addr;
	u32 tune0_addr;
	u32 tune1_addr;
	u32 n, post_div, enable;
	u32 reg_val;

	if (freq > 1000)
		freq = freq / 1000000;
	ax_err(1, "set_adpll_freq is %d\n", freq);

	clk_ctrl_addr = is_cvu ? (CVU_ADPLL_CTRL) : (ENU_ADPLL_CTRL);
	adpll_fac_addr = is_cvu ? (CVU_ADPLL_FAC) : (ENU_ADPLL_FAC);
	tune0_addr = is_cvu ? (CVU_ADPLL_TUNE0) : (ENU_ADPLL_TUNE0);
	tune1_addr = is_cvu ? (CVU_ADPLL_TUNE1) : (ENU_ADPLL_TUNE1);

	/* disable gating first */
	reg_val = Re_Reg_RAW(base_reg + clk_ctrl_addr);
	reg_val &= 0x7fffffff;  // disable
	Wr_Reg_RAW( reg_val, base_reg + clk_ctrl_addr);

	/* disable pll */
	reg_val = Re_Reg_RAW(base_reg + adpll_fac_addr);
	reg_val &= 0x7fffffff;
	Wr_Reg_RAW(reg_val, base_reg + adpll_fac_addr);


	// reset to normal mode
	Wr_Reg_RAW(0x139F0000, base_reg + tune0_addr);
	Wr_Reg_RAW(0x57379120, base_reg + tune1_addr);

	/* calculate pll fac */
	if ((freq >= (408)) && (freq <= (1200))) {
		n = freq / 24;
		post_div = 0;
	} else if ((freq > (102)) && (freq < (204))) {
		n = (freq * 4) / 24;
		post_div = 3;
	} else if ((freq >= (204)) && (freq < (408))) {
		n = (freq * 2) / 24;
		post_div = 1;
	} else
		return 0;

	/* factor */
	reg_val = Re_Reg_RAW(base_reg + adpll_fac_addr);
	reg_val &= 0x7f0fff00;
	reg_val |= ((1 << 31) | (post_div << 20) | n);
	ax_err(1, "set_adpll_freq %x,%x\n",
		base_reg + adpll_fac_addr, reg_val);

	Wr_Reg_RAW(reg_val, base_reg + adpll_fac_addr);

	if (bwait){
		if(!_wait_for_ax_adpll_lock(is_cvu, base_reg))
			return 0;
	}
	/* adpll gating enable */
	enable = 1;
	reg_val = Re_Reg_RAW(base_reg + clk_ctrl_addr);
	reg_val |= (enable << 31);
	ax_err(1, "set_adpll_freq clk_ctrl_addr %x,%x\n",
		base_reg + clk_ctrl_addr, reg_val);
	Wr_Reg_RAW(reg_val, base_reg + clk_ctrl_addr);

	return freq;
}

int tune_adpll_freq(u32 is_cvu, u32 freq, int bwait, unsigned int base_reg)
{
	u32 clk_ctrl_addr;
	u32 adpll_fac_addr;
	u32 tune0_addr;
	u32 tune1_addr;
	u32 n, post_div, enable;
	u32 reg_val;

	if (freq > 1000)
		freq = freq / 1000000;
	ax_err(1, "set_adpll_freq is %d\n", freq);

	clk_ctrl_addr = is_cvu ? (CVU_ADPLL_CTRL) : (ENU_ADPLL_CTRL);
	adpll_fac_addr = is_cvu ? (CVU_ADPLL_FAC) : (ENU_ADPLL_FAC);
	tune0_addr = is_cvu ? (CVU_ADPLL_TUNE0) : (ENU_ADPLL_TUNE0);
	tune1_addr = is_cvu ? (CVU_ADPLL_TUNE1) : (ENU_ADPLL_TUNE1);

	// reset to normal mode
	Wr_Reg_RAW(0x139F0000, base_reg + tune0_addr);
	Wr_Reg_RAW(0x57379120, base_reg + tune1_addr);

	/* calculate pll fac */
	if ((freq >= (408)) && (freq <= (1200))) {
		n = freq / 24;
		post_div = 0;
	} else if ((freq > (102)) && (freq < (204))) {
		n = (freq * 4) / 24;
		post_div = 3;
	} else if ((freq >= (204)) && (freq < (408))) {
		n = (freq * 2) / 24;
		post_div = 1;
	} else
		return 0;

	/* factor */
	reg_val = Re_Reg_RAW(base_reg + adpll_fac_addr);
	reg_val &= 0x7f0fff00;
	reg_val |= ((1 << 31) | (post_div << 20) | n);
	ax_err(1, "set_adpll_freq %x,%x\n",
		base_reg + adpll_fac_addr, reg_val);

	Wr_Reg_RAW(reg_val, base_reg + adpll_fac_addr);

	if (bwait){
		if(!_wait_for_ax_adpll_lock(is_cvu, base_reg))
			return 0;
	}
	
	return freq;
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

void set_ax_internal_gating(u32 is_cvu, u32 enable, unsigned int base_reg)
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
static void ax_power_on(void){
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

static void ax_power_off(void){
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

int get_adpll_freq(int is_cvu, unsigned int base_reg)
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

#define ADPLL_FREE_RUN_FREQ_CONF_NUM (132)
static double adpll_freq_table[ADPLL_FREE_RUN_FREQ_CONF_NUM][3] =
{
/*
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
*/
			{0, 198, 100}, {0, 427, 110}, {0, 658, 120}, {0, 890, 130}, 
			{0, 1123, 140}, {0, 1357, 150}, {0, 1593, 160}, {0, 1830, 170}, 
			{0, 2068, 180}, {0, 2307, 190}, {0, 2548, 200}, {0, 2790, 210}, 
			{0, 3033, 220}, {0, 3278, 230}, {0, 3524, 240}, {0, 3771, 250}, 
			{0, 4020, 260}, {0, 4271, 270}, {0, 4522, 280}, {0, 4775, 290}, 
			{0, 5030, 300}, {0, 5286, 310}, {0, 5543, 320}, {0, 5802, 330}, 
			{0, 6063, 340}, {0, 6325, 350}, {0, 6588, 360}, {0, 6853, 370}, 
			{0, 7120, 380}, {0, 7388, 390}, {0, 7658, 400}, {0, 7930, 410}, 
			{0, 8203, 420}, {0, 8478, 430}, {0, 8754, 440}, {0, 9032, 450},
			{0, 9312, 460}, {0, 9594, 470}, {0, 9877, 480}, {0, 10162, 490}, 
			{0, 10449, 500}, {0, 10737, 510}, {0, 11028, 520}, {0, 11320, 530}, 
			{0, 11614, 540}, {0, 11910, 550}, {0, 12207, 560}, {0, 12507, 570}, 
			{0, 12808, 580}, {0, 13112, 590}, {0, 13417, 600}, {0, 13725, 610}, 
			{0, 14034, 620}, {0, 14345, 630}, {0, 14658, 640}, {0, 14974, 650}, 
			{0, 15291, 660}, {0, 15610, 670}, {0, 15932, 680}, {0, 16255, 690}, 
			{0, 16581, 700}, {0, 16908, 710}, {0, 17238, 720}, {0, 17570, 730}, 
			{0, 17904, 740}, {0, 18240, 750}, {0, 18579, 760}, {0, 18919, 770}, 
			{0, 19262, 780}, {0, 19607, 790}, {0, 19954, 800}, {0, 20303, 810}, 
			{0, 20654, 820}, {0, 21008, 830}, {0, 21364, 840}, {0, 21722, 850}, 
			{0, 22082, 860}, {0, 22445, 870}, {0, 22809, 880}, {0, 23176, 890}, 
			{0, 23546, 900}, {0, 23917, 910}, {0, 24291, 920}, {0, 24666, 930}, 
			{0, 25044, 940}, {0, 25424, 950}, {0, 25807, 960}, {0, 26191, 970}, 
			{0, 26578, 980}, {0, 26966, 990}, {0, 27357, 1000},{0, 27750, 1010},
			{0, 28145, 1020},{0, 28542, 1030},{0, 28941, 1040},{0, 29341, 1050},
			{0, 29744, 1060},{0, 30149, 1070},{0, 30555, 1080},{0, 30963, 1090},
			{0, 31373, 1100},{0, 31785, 1110},{0, 32199, 1120},{0, 32614, 1130},
};
int set_ax_adpll_total_free_run_nu_init(u32 is_cvu, u32 freq, u32 base_reg)
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
			real_freq_mhz = (slowfreqerr < fastfreqerr) ? adpll_freq_table[i-1][2] : adpll_freq_table[i][2];
			dco_range = (u32)((slowfreqerr < fastfreqerr) ? adpll_freq_table[i-1][0] : adpll_freq_table[i][0]);
			config_free_run_clock_freq = (u32)((slowfreqerr < fastfreqerr) ? adpll_freq_table[i-1][1] : adpll_freq_table[i][1]);
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

	/*set to max pll clock*/
	Wr_Reg_RAW(0x03007FFF, base_reg + tune0_addr);

	Wr_Reg_RAW(0x80000000, base_reg + adpll_fac_addr);

	// tune0
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

int set_ax_adpll_total_free_run_nu(u32 is_cvu, u32 freq, u32 base_reg)
{
	u32 clk_ctrl_addr, adpll_fac_addr, tune0_addr, tune1_addr;
	double real_freq_mhz, slowfreqerr, fastfreqerr, freq_mhz;
	u32 i;
	u32 dco_range, config_free_run_clock_freq;

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
	Wr_Reg_RAW(0x03000000 | (dco_range << 16) |
		(config_free_run_clock_freq), base_reg + tune0_addr);

	return (int)real_freq_mhz;
}


int ax_clk_init(u32 is_cvu, unsigned int base_reg)
{
	/* ax_power_on(); */

#if PRCM_ON /* prcm clk */
	/* enable ax pll */
	u32 rdval = 0;

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

		u32 fac = AX_CLK_RATE / 240;
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
		_ax_clk_sel(0, 0, base_reg);
	}
#endif

#if ADPLL_ON
	//u32 fac = AX_CLK_RATE / 24000000;
	//set_ax_adpll_n_div(is_cvu, fac, 0, base_reg);
	int freq = set_ax_adpll_total_free_run_nu_init(is_cvu,
					AX_CLK_RATE, base_reg);
	//set_ax_adpll_enable(is_cvu, 1, base_reg);
	//_wait_for_ax_adpll_lock(is_cvu, base_reg);
	_ax_clk_sel(is_cvu, 1, base_reg);


	/*rdval = Re_Reg_RAW(base_reg + ENU_ADPLL_TUNE1);
	ax_err(1,"ENU_ADPLL_TUNE1 %x\n", rdval);
	rdval = rdval & 0x0000;
	rdval = rdval | 0x77779120;
	Wr_Reg_RAW(rdval, base_reg + ENU_ADPLL_TUNE1);
	rdval = Re_Reg_RAW(base_reg + ENU_ADPLL_TUNE1);
	ax_err(1,"-----ENU_ADPLL_TUNE1 %x\n", rdval);*/


#endif

	return freq;
}

void ax_clk_deinit(void){
#if PRCM_ON
	ax_prcm_disable();
#endif
	/* ax_power_off(); */
}

int get_power_on_status(void){
	return ax_powered_on;
}
