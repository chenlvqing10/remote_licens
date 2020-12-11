
#include "lombo_dram_printk.h"
#include "types.h"

#define VA_DRAMC		0xF0000000
#define VA_DRAMPUB		0xF0000000
#define VA_MEM_CTRL		0xF0000000

#include "dramc.h"
#include "drampub.h"
#include "mem_ctrl.h"
#include "prcm.h"
#include "gtimer.h"

#define DATA_NUM 32
#define CLK_DIV

#define SDRAM_BASE		0xC0000000

#define VBASE_DRAMC		(BASE_DRAMC + VA_DRAMC)
#define VBASE_DRAMPUB		(BASE_DRAMPUB + VA_DRAMPUB)
#define VBASE_MEM_CTRL		(BASE_MEM_CTRL + VA_MEM_CTRL)
#define VBASE_PRCM		(BASE_PRCM + 0xF0000000)
#define VBASE_GTIMER		(BASE_GTIMER + 0xF0000000)

#define PORT_NUM		(16)
#define MC_PCTRL_(n)		(BASE_DRAMC + VA_DRAMC + 0x490 + 0xb0 * (n))
#define DDR2			(2)
#define DDR3			(3)
#define DDR3_MR0		0
#define DDR3_MR1		1
#define DDR3_MR2		2
#define DDR3_MR3		3
#define DDR2_MR			0
#define DDR2_EMR1		1
#define DDR2_EMR2		2
#define DDR2_EMR3		3
/* MR_RANK number */
#define RNKO0			1
#define	RNKO1			2
#define	RNKN01			3

#define READREG32(reg)		(*(volatile u32 *)(reg))
#define WRITEREG32(reg, val)	(*(volatile u32 *)(reg)) = (val)

#define ERR_FLG(...)		do {} while (0)

typedef struct ddr_dfs_para {
	u32 version;
	u32 ddr_type;	/* ddr type */
	u32 freq;	/* ddr clk */
	u32 ddr3_clk;
	u32 ddr2_clk;
	u32 size;
	u32 col_num;
	u32 bank_num;
	u32 row_num;
	u32 ddrc_dw;
	u32 rankn;
	u32 ssc_en;
	u32 mod2t;
	u32 dram_mr0;
	u32 dram_mr1;
	u32 dram_mr2;
	u32 dram_mr3;
	u32 t_refi;
	u32 t_rfc;
	u32 odt_en;
	u32 ca_drv;
	u32 dx01_drv;
	u32 dx23_drv;
	u32 read_dq_dly;
	u32 read_dqs_dly;
	u32 write_dq_dly;
	u32 write_dqs_dly;
	u32 ca_dly;
	u32 ck_dly;
	u32 obtain_para;  /* bit0 置1代表执行obtain功能，并且不打印obtain过程中的error信息 */
} ddr_dfs_para_t;

void udelay_dfs(u32 us);
void mr_wr_dfs(u32 mr, u32 mr_val, u32 rnk_no);
void phy_ddl_training(void);
void ddr_port_disable_dfs(void);
void ddr_dll_off_dfs(ddr_dfs_para_t *sdram_init_t);
void sw_sr_dfs(u32 enable);
u32  cal_time2cycle_dfs(u32 clk, u32 time_ns);
u32  timing_div_fix_dfs(u32 timing);
void set_ddr_timing_dfs(ddr_dfs_para_t *ddr_dfs_para);
void set_refresh_timing_dfs(ddr_dfs_para_t *ddr_dfs_para);
void update_mrs_chg_freq_dfs(ddr_dfs_para_t *sdram_init_t);
void ddr_port_enable_dfs(void);
u32  ddr_dfs(u32 div, ddr_dfs_para_t *ddr_dfs_para);
