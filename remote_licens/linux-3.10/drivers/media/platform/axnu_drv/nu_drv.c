#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/errno.h>    /* error codes */
#include <linux/vmalloc.h>
#include <linux/init.h>     /* module_init/module_exit */
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <mach/hardware.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/sem.h>
#include <linux/time.h>
#include <linux/reset.h>
#include <mach/csp.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <mach/debug.h>
#include <linux/sched.h>
#include <linux/thermal.h>
#include "../../../misc/lombo/memctrl/memctrl.h"
#include "nu_drv.h"
#include "clk.h"
#include "utils.h"

extern int gax_power_on_flg;
extern int gax_prcm_clk_en;

static struct clk *nu_clk;
/*static struct clk *parent_pll;*/
static struct clk *nu_ahb_gate;
static struct clk *nu_ahb_reset;
static struct clk *nu_axi_gate;

/* Timeout counter: 3s */
#define WAIT_TIMEOUT (HZ / 5)  /* 1s <--> HZ */
#define WAIT_TIMEOUT_MS 3000 /* ms */

#define NU_INTTERUPT_MODE
#define NU_PARENT_CLK_RATE 360
#define NU_CLK_RATE 360

#define DEVDRV_NAME_ENU	"axnu"
#define MAJOR_NN 256
#define MINOR_NN 100
#define IRQ_ENU 104

struct asoc_nu_dev {
	struct device *dev;
	void __iomem *base;
	struct clk *clk;
	int irq;
	unsigned int setting; /* not used temporarily */
	struct regulator *power; /* not used temporarily */
	unsigned int freq;
};

static struct asoc_nu_dev *gNU;

static DEFINE_MUTEX(nu_mutex);
static struct completion nu_complete;
static wait_queue_head_t  nu_wait;

static int nu_status = 0;
static int nu_irq_registered = 0;
static int nu_open_count = 0;
static int nu_pre_hdl = 0;

static int enu_cmd_done_flag[64];
static void *pAbuf[_MAX_HANDLES_];

static int log_en = 0;

typedef struct {
	enu_stat_info_t stat_info;
	enu_clt_t clt_info;
	enu_status_out_t stat_out;
	/* stat_info_t vu_info; */
	unsigned int nu_count;
	/* unsigned int vu_count; */
	unsigned int nu_status;

	//unsigned int vu_status;
}nu_info_t;

static unsigned int special_freq;
static unsigned int setting_freq;
static unsigned int running_freq_tab[3];
static unsigned int band_width_limit[3];

static void clr_cmdq_flg(void)
{
	int i;
	for (i = 0; i < 64; i++) {
		enu_cmd_done_flag[i] = 0;
	}
}

static void get_cmdq_flgs(enu_status_out_t *stat_out)
{
	int i;
	stat_out->err_no = nu_status;
	stat_out->mCmdIntL32 = 0;
	stat_out->mCmdIntH32 = 0;
	for (i = 0; i < 32; i++)
		stat_out->mCmdIntL32 |= (enu_cmd_done_flag[i]<<i);

	for (i = 0; i < 32; i++)
		stat_out->mCmdIntH32 |= (enu_cmd_done_flag[i+32]<<i);

	ax_err(log_en, "cmdq_pflgs %x, %x\n", stat_out->mCmdIntL32,
			stat_out->mCmdIntH32);
}

static void print_regs(void)
{
	u32 rval = 0;

	rval = Re_Reg((unsigned int)gNU->base + ENU_CTL);
	ax_err(1, "ENU_CTL %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_STAT0);
	ax_err(1, "ENU_STAT0 %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_STAT1);
	ax_err(1, "ENU_STAT1 %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN);
	ax_err(1, "ENU_CMDQ_INT_EN %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD);
	ax_err(1, "ENU_CMDQ_INT_PD %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_CLR);
	ax_err(1, "ENU_CMDQ_INT_CLR %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_ADDR);
	ax_err(1, "ENU_CMDQ_ADDR %x\n", rval);

	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN0);
	ax_err(1, "ENU_CMDQ_INT_EN0 %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN1);
	ax_err(1, "ENU_CMDQ_INT_EN1 %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD0);
	ax_err(1, "ENU_CMDQ_INT_PD0 %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD1);
	ax_err(1, "ENU_CMDQ_INT_PD1 %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_PS_L32);
	ax_err(1, "ENU_CMDQ_PS_L32 %x\n", rval);
	rval = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_PS_H32);
	ax_err(1, "ENU_CMDQ_PS_H32 %x\n", rval);
}

/*
	NU / CMDQ independent interupt
*/

static int query_status(void)
{
	int ret_value = nu_status;
	return ret_value;
}

static void nu_reset(void)
{
	u32 rval = 0;
	u32 wait_cnt = 1000;
	Wr_Reg((unsigned int)gNU->base + ENU_RST_STAT, 0x00000100);
	while (wait_cnt & (rval == 0)) {
		rval = Re_Reg((unsigned int)gNU->base + ENU_RST_STAT);
		wait_cnt--;
	}
	Wr_Reg((unsigned int)gNU->base + ENU_RST_STAT, 0x00000000);
	Wr_Reg((unsigned int)gNU->base + ENU_CTL, 0x80000000);
	udelay(1);
	Wr_Reg((unsigned int)gNU->base + ENU_CTL, 0x00000000);
}

static void nu_stop(void)
{
	if (gax_prcm_clk_en) {
		int cnt = 10000;
		while (ENU_BUSY == query_status()) {
			/* 已暂停或结束 */

			ax_err(1, "nu_status %d\n", nu_status);
			mdelay(1);
			cnt--;
			if (cnt == 0) {
				ax_err(1, "nu_status err %d\n", nu_status);
				break;
			}
		}
		nu_status = ENU_STOPED;
	}
}

static void set_stat_enable(void)
{
	/* ax_err(0,"%d,%x\n",__LINE__,
		(unsigned int)gNU->base + ENU_PERF_CNT); */
	Wr_Reg((unsigned int)gNU->base + ENU_PERF_CNT, 0x80000000);
	Wr_Reg((unsigned int)gNU->base + ENU_BW_STAT_EN, 0x80000000);
	Wr_Reg((unsigned int)gNU->base + ENU_BW_R_STAT, 0x00000000);
	Wr_Reg((unsigned int)gNU->base + ENU_BW_W_STAT, 0x00000000);
}

static void updata_nu_cfg(enu_clt_t *clt_info)
{
	unsigned int enu_isr_en = (1<<0)|(0<<4)|(0<<5)|(0<<6)|(0<<7)|(2000<<16);
	unsigned int enu_ctl = 1 | (1 << 4) | ((clt_info->mCmdNum - 1) << 8)
		| (clt_info->mFstCmdLen << 16);
	unsigned int enu_clr_status = 1 | (1 << 4) | (1 << 5) | (1 << 6)
		| (1 << 7);
	/* print_regs(); */

	/* ax_err(0, "mCmdNum %d, mFstCmdLen %d, %x\n", clt_info->mCmdNum,
		clt_info->mFstCmdLen, enu_ctl); */

	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_CLR, enu_clr_status);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN, enu_isr_en);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_CLR0, 0xffffffff);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_CLR1, 0xffffffff);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN0,
		clt_info->mCmdIntL32);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN1,
		clt_info->mCmdIntH32);

	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_PS_L32,
		clt_info->mCmdPAUSL32);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_PS_H32,
		clt_info->mCmdPAUSH32);

	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_ADDR, clt_info->mCmdAddr);
	Wr_Reg((unsigned int)gNU->base + ENU_CTL, enu_ctl);
	/* print_regs(); */
}

static void clr_paused_flg(enu_pause_flg_t *pas_flg)
{
	/* ax_err(log_en,"clr_paused_flg %x, %x\n", pas_flg->mCmdPAUSL32,
		pas_flg->mCmdPAUSH32); */
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_PS_L32, pas_flg->mCmdPAUSL32);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_PS_H32, pas_flg->mCmdPAUSH32);
}

#if 0
static void print_stat_info(void)
{
	//ax_err(1, "reg %x\n", (unsigned int)gNU->base + ENU_PERF_CNT);
	unsigned int mtiks = Re_Reg((unsigned int)gNU->base + ENU_PERF_CNT)
		& 0x7FFFFFFF;
	unsigned int mrbw = Re_Reg((unsigned int)gNU->base + ENU_BW_R_STAT);
	unsigned int  mwbw = Re_Reg((unsigned int)gNU->base + ENU_BW_W_STAT);

	ax_err(1, "stat_info %d, %d, %d\n", (unsigned int)mtiks, mrbw, mwbw);
}
#endif

static void get_stat_info(enu_stat_info_t *nu_info)
{
	nu_info->mstat.mtiks = Re_Reg((unsigned int)gNU->base + ENU_PERF_CNT)
		& 0x7FFFFFFF;
	nu_info->mstat.mrbw = Re_Reg((unsigned int)gNU->base + ENU_BW_R_STAT);
	nu_info->mstat.mwbw = Re_Reg((unsigned int)gNU->base + ENU_BW_W_STAT);
	nu_info->mboxes = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_BOX) >> 16;
	nu_info->status = nu_status;
	/* ax_err(log_en,"stat_info %d, %d, %d\n",
		(unsigned int)nu_info->mstat.mtiks,
		(nu_info->mstat.mrbw+nu_info->mstat.mwbw), nu_info->mboxes); */
}
#if 0
static void print_status(void)
{
	int nu_val;
	int nu_pd0;
	int nu_pd1;
	nu_pd0 = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD0);
	nu_pd1 = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD1);
	nu_val = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD);
	ax_err(1, "nu status %x,%x,%x\n", nu_val, nu_pd0, nu_pd1);
}
#endif
/* enable int */
static inline void enable_nu_irq(void)
{
}

static inline int disable_nu_irq(void)
{
	int nu_val;
	int nu_pd0;
	int nu_pd1;
	int valid_flg = 0;
	int i = 0;

	nu_pd0 = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD0);
	if (nu_pd0) {
		for (i = 0; i < 32; i++) {
			if ((nu_pd0 & (1 << i)) == (1 << i))
				enu_cmd_done_flag[i] |= 1;
		}
		Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_CLR0, nu_pd0);
		valid_flg |= 1;
		nu_status = ENU_PAUSED;
	}

	nu_pd1 = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD1);
	if (nu_pd1) {
		for (i = 0; i < 32; i++) {
			if ((nu_pd1 & (1 << i)) == (1 << i))
				enu_cmd_done_flag[i+32] |= 1;
		}
		Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_CLR1, nu_pd1);
		valid_flg |= (1<<8);
		nu_status = ENU_PAUSED;
	}

	nu_val = Re_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_PD);
	/* ax_err(1,"nu_ISR nu_val %x,%x,%x\n",nu_val,nu_pd0,nu_pd1); */
	if (nu_val) {
		Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_CLR, nu_val);
		valid_flg |= (1<<16);
		nu_status = ENU_STOPED;
	}

	/* Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN,0x00000000);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN0,0x00000000);
	Wr_Reg((unsigned int)gNU->base + ENU_CMDQ_INT_EN1,0x00000000); */

	return valid_flg;
}


irqreturn_t nu_ISR(int irq, void *dev_id)
{
	int valid_flg = disable_nu_irq();

	if ((valid_flg)) {
		complete(&nu_complete);
		wake_up_interruptible(&nu_wait);
	}

	return IRQ_HANDLED;
}
#if 0
static void nu_clk_enable(void)
{
	int ret = 0;
	/* unsigned int regval; */
	gax_prcm_clk_en++;
	if (gax_prcm_clk_en > 1)
		return;
	ax_err(1, "nu_clk_enable a\n");
	/* enable clk */
	ret = clk_set_rate(parent_pll, NU_PARENT_CLK_RATE);
	/* pr_err("pn_debug %s:%d\n", __func__, __LINE__);
	asm volatile ("b .");
	ax_err(1,"nu_clk_enable b\n");
	while (1) {
		ax_err(1,"nu_clk_enable c\n");
		regval = readl_relaxed((void *)AX_PLL_STAT);
		if ((regval & 0x7) == 7)
			break;
	} */
	/* ax_err(1,"nu_clk_enable d\n"); */
	if (clk_set_parent(nu_clk, parent_pll))
		ax_err(1, "failed to set nu parent to display_pll\n");
	/* pr_err("pn_debug %s:%d\n", __func__, __LINE__);
	asm volatile ("b .");
	ax_err(1,"nu_clk_enable e\n"); */
	ret = clk_set_rate(nu_clk, NU_CLK_RATE);
	if (ret)
		ax_err(1, "set nu_clk rate err\n");
	/* pr_err("pn_debug %s:%d\n", __func__, __LINE__);
	asm volatile ("b .");
	ax_err(1,"nu_clk_enable f\n"); */
	clk_prepare_enable(nu_clk);
	/* pr_err("pn_debug %s:%d\n", __func__, __LINE__);
	asm volatile ("b ."); */
	ax_err(1, "nu_clk_enable h\n");
	/* nu_clk_isenable = 1; */
}

static void nu_clk_disable(void)
{
	gax_prcm_clk_en--;
	if (gax_prcm_clk_en == 0) {
		nu_stop();
		clk_disable_unprepare(nu_clk);
	}

	/* gax_prcm_clk_en = 0; */
}
#endif
/* ssize_t vce_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
	unsigned long arg) */
static long nu_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = ENU_OK;
	int bOK = 0;
	/* unsigned long flags; */

	/* if(0 >= gax_prcm_clk_en)
	{
		ax_err(1,"pls power on!\n");
	} */

	/* ax_err(log_en,"cmd %x, arg %x\n", (unsigned int)cmd,
		(unsigned int)arg); */

	switch (cmd) {
	case ENU_RUN: {
		void __user *from; /* = (void __user *)arg; */
		long time_rest = 0;
		nu_info_t *info;
		mutex_lock(&nu_mutex);
		from = (void __user *)arg;
		if (nu_status == ENU_PAUSED) {
			/* 暂停，需要获取cmdq中断状态 */
			ax_err(log_en, "ENU_PAUSED\n");
			mutex_unlock(&nu_mutex);
			return ENU_WAIT;
		}
		/* ax_err(0,"%d\n",__LINE__); */
		time_rest = wait_event_interruptible_timeout(nu_wait,
				((nu_status != ENU_BUSY)), WAIT_TIMEOUT);
		if (time_rest <= 0) {
			nu_reset();
			mutex_unlock(&nu_mutex);
			return ENU_ERR_TIMEOUT;
		} else {
			if (nu_status == ENU_PAUSED) {
				/* 暂停，需要获取cmdq中断状态 */
				ax_err(log_en, "ENU_PAUSED\n");
				mutex_unlock(&nu_mutex);
				return ENU_WAIT;
			}
			/* ax_err(0,"%d\n",__LINE__);
			already done,clear all & save irqs
			把上一个驱动保存结果 */
			if (nu_pre_hdl >= 1 && nu_pre_hdl < _MAX_HANDLES_) {
				/* new add */
				nu_info_t *pinfo =
					(nu_info_t *) pAbuf[nu_pre_hdl - 1];
				if (pinfo != NULL) {
					get_stat_info(&pinfo->stat_info);
					pinfo->nu_status = ENU_STOPED;
				}
			}
		}
		/* ax_err(0,"%d\n",__LINE__); */
		info = (nu_info_t *) filp->private_data;
		bOK = copy_from_user(&info->clt_info, from, sizeof(enu_clt_t));
		clr_cmdq_flg();
		init_completion(&nu_complete);
		/* ax_err(0, "%d\n", __LINE__);
		print_status();
		print_stat_info();
		ax_err(0, "%d\n", __LINE__); */
		set_stat_enable();
		nu_status = ENU_BUSY;
		nu_pre_hdl = info->nu_count;
		info->nu_status = nu_status;
		/* ax_err(0,"%d\n", __LINE__); */
		updata_nu_cfg(&info->clt_info);
		/* ax_err(0, "nu_status %d\n", (unsigned int)nu_status); */
		mutex_unlock(&nu_mutex);
		break;
	}
	case ENU_SET_FREQ: {
		mutex_lock(&nu_mutex);

		nu_stop();
		ax_err(log_en, "set freq %d\n", (unsigned int)arg);

			//set_adpll_freq(0, (unsigned int)arg, 0, (unsigned int)gNU->base);
			gNU->freq = set_ax_adpll_total_free_run_nu(0, (unsigned int)arg,
														(unsigned int)gNU->base);
			setting_freq = gNU->freq;
		mutex_unlock(&nu_mutex);
		break;
	}

	case ENU_SET_NORMAL_FREQ: {
		ax_err(1, "%d\n", __LINE__);
		mutex_lock(&nu_mutex);
		nu_stop();
		ax_err(1, "set freq %d\n", (unsigned int)arg);
		gNU->freq = set_adpll_freq(0, (unsigned int)arg, 1, (unsigned int)gNU->base);
		mutex_unlock(&nu_mutex);
		break;
	}

	case ENU_SET_REG: {
		void __user *from = (void __user *)arg;
		ax_reg_t reg_flg;
		bOK = copy_from_user(&reg_flg, from, sizeof(ax_reg_t));
		Wr_Reg((unsigned int)gNU->base + reg_flg.reg_addr,
				reg_flg.reg_val);
		ax_err(log_en, "reg %x,%x\n",
			reg_flg.reg_addr, reg_flg.reg_val);
		break;
	}
	case ENU_GET_REG: {
		void __user *from = (void __user *)arg;
		ax_reg_t reg_flg;
		bOK = copy_from_user(&reg_flg, from, sizeof(ax_reg_t));
		reg_flg.reg_val =
			Re_Reg((unsigned int)gNU->base + reg_flg.reg_addr);
		ret = reg_flg.reg_val;
		ax_err(log_en, "reg %x,%x\n",
			reg_flg.reg_addr, reg_flg.reg_val);
		break;
	}
	case ENU_GET_NORMAL_FREQ:
	case ENU_GET_FREQ: {
		void __user *to = (void __user *)arg;
		int tfreq = 0;
		mutex_lock(&nu_mutex);
		tfreq = gNU->freq;//get_adpll_freq(0,(unsigned int)gNU->base);
		bOK = copy_to_user(to, (void *)&tfreq, sizeof(unsigned int));
		ax_err(log_en, "get freq %d\n", tfreq);
		mutex_unlock(&nu_mutex);
		break;
	}
	case ENU_GET_VER: {
		void __user *to = (void __user *)arg;
		unsigned int ver_flg =
			Re_Reg((unsigned int)gNU->base + AX_TOP_VER);
		bOK = copy_to_user(to, (void *)&ver_flg, sizeof(unsigned int));
		ax_err(log_en, "version %x, %d\n",
			(unsigned int)gNU->base + AX_TOP_VER,
			(unsigned int)ver_flg);
		break;
	}
	case ENU_LOG: {
		log_en = (unsigned int)arg;
		ax_err(1, "log_en %d\n", (unsigned int)log_en);
		break;
	}
	case ENU_STATUS: {
		void __user *to = (void __user *)arg;
		nu_info_t *pinfo = (nu_info_t *) filp->private_data;
		enu_status_out_t stat_out;
		get_cmdq_flgs(&stat_out);
		if (nu_pre_hdl != pinfo->nu_count)
			stat_out.err_no = pinfo->nu_status;
		bOK = copy_to_user(to, (void *)&stat_out,
				sizeof(enu_status_out_t));
		break;
	}
	case ENU_WAIT_CONTINUE: {
		void __user *from = (void __user *)arg;
		enu_pause_flg_t pas_flg;
		/* long time_rest = 0; */
		nu_info_t *info;

		mutex_lock(&nu_mutex);
		info = (nu_info_t *) filp->private_data;
		if (nu_pre_hdl != info->nu_count) {
			ax_err(1, "nu_pre_hdl err %d,%d\n",
				(unsigned int)nu_pre_hdl,
				info->nu_count);
			mutex_unlock(&nu_mutex);
			return ENU_ERR_CONFLICT;
		}

		bOK = copy_from_user(&pas_flg, from, sizeof(enu_pause_flg_t));
		clr_paused_flg(&pas_flg);
		ax_err(0, "clr_paused_flg ok\n");
		nu_status = ENU_BUSY;
		init_completion(&nu_complete);
		info->nu_status = nu_status;

		/*
		time_rest = wait_event_interruptible_timeout(nu_wait,
			((nu_status != ENU_BUSY)), WAIT_TIMEOUT);
		if(time_rest <= 0)
		{
			nu_reset();
			mutex_unlock(&nu_mutex);
			return ENU_ERR_TIMEOUT;
		}
		else
		{
			if(nu_status == ENU_PAUSED){
				//暂停，需要获取cmdq中断状态
				ax_err(log_en,"ENU_PAUSED\n");
				mutex_unlock(&nu_mutex);
				return ENU_WAIT;
			}
			//already done,clear all & save irqs
			//把上一个驱动保存结果
			if(nu_pre_hdl >= 1 && nu_pre_hdl < _MAX_HANDLES_)
			{
				nu_info_t *pinfo =
					(nu_info_t *)pAbuf[nu_pre_hdl - 1];
				if(pinfo != NULL)
				{
					get_stat_info(&pinfo->stat_info);
					pinfo->nu_status = ENU_STOPED;
				}
			}
		}
		*/
		mutex_unlock(&nu_mutex);
		break;
	}
	case ENU_QUERY_FINISH: {
		unsigned long timeout;
		int left_time;
		unsigned long expire;
		int enu_value = 0;
		void __user *to;
		nu_info_t *info;
		/* ax_err(0, "ENU_QUERY_FINISH %d\n",nu_status); */

		mutex_lock(&nu_mutex);
		to = (void __user *)arg;
		info = (nu_info_t *) filp->private_data;
		if (info->nu_status == ENU_STOPED) {
			ax_err(1, "nu_status stop %d\n", nu_status);
			print_regs();
			bOK = copy_to_user(to, (void *)&info->stat_info,
					sizeof(enu_stat_info_t));
			mutex_unlock(&nu_mutex);
			return 0;
		}

		/* mutex_unlock(&nu_mutex); */
#ifdef NU_INTTERUPT_MODE
		enu_value = query_status();

		if ((enu_value != ENU_BUSY) && (enu_value != ENU_PAUSED)) {
			/* ax_err(1, "nu_status ok %d\n",nu_status);
			print_regs(); */
			get_stat_info(&info->stat_info);
			bOK = copy_to_user(to, (void *)&info->stat_info,
					sizeof(enu_stat_info_t));
			info->nu_status = ENU_STOPED;
			mutex_unlock(&nu_mutex);
			goto out;
		}

		timeout = msecs_to_jiffies(WAIT_TIMEOUT_MS) + 1;
		if (nu_irq_registered) {
			left_time = wait_for_completion_timeout(&nu_complete,
					timeout);
			if (unlikely(left_time == 0)) {
				int enu_status = query_status();
				/* ax_err(1, "nu timeout!\n"); */

				if (enu_status == ENU_STOPED) {
					ret = 0;
					get_stat_info(&info->stat_info);
					bOK = copy_to_user(to,
						(void *)&info->stat_info,
						sizeof(enu_stat_info_t));
					info->nu_status = ENU_STOPED;
					mutex_unlock(&nu_mutex);
					goto out;
				}

				info->nu_status = ENU_STOPED;
				nu_status = ENU_STOPED;
				ret = ENU_ERR_TIMEOUT;
				disable_nu_irq();
				nu_reset();
				mutex_unlock(&nu_mutex);
				goto out;
			} else {
				/* normal case */
				ret = 0;
				/* ax_err(1, "nu_status wait %d\n",nu_status);
				print_regs(); */
				get_stat_info(&info->stat_info);
				bOK = copy_to_user(to, (void *)&info->stat_info,
						sizeof(enu_stat_info_t));
				if (nu_status == ENU_STOPED)
					info->nu_status = ENU_STOPED;
				else
					info->nu_status = ENU_PAUSED;
				mutex_unlock(&nu_mutex);
				goto out;
			}
		}

		expire = timeout + jiffies;
		do {
			enu_value = query_status();

			if (enu_value != ENU_BUSY) {
				get_stat_info(&info->stat_info);
				bOK = copy_to_user(to, (void *)&info->stat_info,
						sizeof(enu_stat_info_t));
				if (enu_value == ENU_STOPED)
					info->nu_status = ENU_STOPED;
				else
					info->nu_status = ENU_PAUSED;
				mutex_unlock(&nu_mutex);
				goto out;
			}

			if (time_after(jiffies, expire)) {
				info->nu_status = ENU_STOPED;
				nu_status = ENU_STOPED;
				ret = ENU_ERR_TIMEOUT;
				disable_nu_irq();
				nu_reset();
				mutex_unlock(&nu_mutex);
				goto out;
			}
		} while (1);

#else
		enu_value = 0;
		{
			int time_counts = 0xfffffff;
			do {
				enu_value = query_status();
				if (time_counts-- < 0) {
					info->nu_status = ENU_STOPED;
					ret = ENU_ERR_TIMEOUT;
					disable_nu_irq();
					nu_reset();
					mutex_unlock(&nu_mutex);
					goto out;
				}
			} while (enu_value == ENU_BUSY);

			get_stat_info(&info->stat_info);
			bOK = copy_to_user(to, (void *)&info->stat_info,
					sizeof(enu_stat_info_t));
			if (nu_status == ENU_STOPED)
				info->nu_status = ENU_STOPED;
			else
				info->nu_status = ENU_PAUSED;
			mutex_unlock(&nu_mutex);
		}
#endif
	}
	break;

	default:
		return -EIO;
	}

	/* printk(KERN_ERR"vce_dev out\n"); */
out:
	return ret;
}

int nu_open(struct inode *inode, struct file *filp)
{
	nu_info_t *info = NULL;
	ax_err(1, "nu_open 20200414\n");

	mutex_lock(&nu_mutex);
	nu_open_count++;
	filp->private_data = NULL;

	if (nu_open_count > _MAX_HANDLES_) {
		ax_err(1, "nu_open_count error %d\n", nu_open_count);
		nu_open_count--;
		mutex_unlock(&nu_mutex);
		return -1;
	}
	ax_err(1, "nu_open malloc %d\n", nu_open_count);
	info = (nu_info_t *)ax_malloc(sizeof(nu_info_t));
	if (info == NULL) {
		ax_err(1, "ax_malloc fail\n");
		nu_open_count--;
		mutex_unlock(&nu_mutex);
		return -1;
	}
	ax_err(1, "ax_open %d\n", nu_open_count);
	/* init completion var */
	if (nu_open_count == 1) {
		/* ax_clk_init((unsigned int)gNU->base);
		pr_err("pn_debug %s:%d\n", __func__, __LINE__);
		nu_clk_enable(); */
		//set_adpll_freq(0, NU_CLK_RATE, 0, (unsigned int)gNU->base);
		/* pr_err("pn_debug %s:%d\n", __func__, __LINE__);
		asm volatile ("b ."); */
		ax_err(1, "nu_clk_enable out\n");
		init_completion(&nu_complete);
		/* clk_prepare_enable(nu_ahb_reset); */
	}
	ax_err(1, "init_completion out\n");
	pAbuf[nu_open_count - 1] = (void *)info;
	/* 当前vce info序列号，从1开始 */
	info->nu_count = nu_open_count;
	info->nu_status = ENU_IDLE;
	filp->private_data = (void *)info;
	mutex_unlock(&nu_mutex);

	return 0;
}

int nu_release(struct inode *inode, struct file *filp)
{
	nu_info_t *info = (nu_info_t *) filp->private_data;

	if (info == NULL)
		return 0;

	mutex_lock(&nu_mutex);
	nu_open_count--;
	ax_err(1, "nu_release %d\n", nu_open_count);
	/* print_regs(); */
	nu_stop();
	if (nu_open_count >= 0) {
		if (info) {
			ax_free(info);
			info = filp->private_data = NULL;
		}

		pAbuf[nu_open_count] = NULL;
	} else if (nu_open_count < 0) {
		nu_open_count = 0;
	}

	if (nu_open_count == 0) {
		nu_pre_hdl = 0;
		/* disable_nu_irq();
		ax_clk_deinit(); */
	}

	mutex_unlock(&nu_mutex);

	return 0;
}

int nu_suspend(struct platform_device *dev, pm_message_t state)
{
	mutex_lock(&nu_mutex);
	ax_err(1, "nu_suspend %d\n", nu_open_count);
	nu_stop();
	if (gax_power_on_flg == 1) {

		clk_disable_unprepare(nu_ahb_gate);
		clk_disable_unprepare(nu_ahb_reset);
		clk_disable_unprepare(nu_axi_gate);
		set_ax_adpll_total_free_run_nu(0, 0, (unsigned int)gNU->base);
		gax_power_on_flg--;
	} else if (gax_power_on_flg > 1) {
		gax_power_on_flg--;
		ax_err(1, "nu gax_power_on_flg %d\n", gax_power_on_flg);
	} else {
		ax_err(1, "should not happend nu gax_power_on_flg %d\n",
							gax_power_on_flg);
	}

	mutex_unlock(&nu_mutex);

	return 0;
}

int nu_resume(struct platform_device *dev)
{
	mutex_lock(&nu_mutex);
	ax_err(1, "nu_resume\n");
	if (gax_power_on_flg == 0) {

		clk_prepare_enable(nu_ahb_gate);
		clk_prepare_enable(nu_ahb_reset);
		clk_prepare_enable(nu_axi_gate);
		gax_power_on_flg++;
	} else if (gax_power_on_flg == 1) {
		gax_power_on_flg++;
		ax_err(1, "nu gax_power_on_flg %d\n", gax_power_on_flg);
	} else {
		ax_err(1, "should not happend nu gax_power_on_flg %d\n",
							gax_power_on_flg);
	}
	ax_clk_init(0, (unsigned int)gNU->base);
	set_ax_internal_gating(0, 1, (unsigned int)gNU->base);
	set_ax_adpll_total_free_run_nu(0, gNU->freq, (unsigned int)gNU->base);
	mutex_unlock(&nu_mutex);
	return 0;
}

static struct of_device_id lombo_nu_match[] = {
	{ .compatible = "lombo,n7-axnu", },
	{ },
};
MODULE_DEVICE_TABLE(of, lombo_nu_match);

#define NU_CLK "ax_module_clk"
#define NU_AHB_GATE "ax_ahb_gate"
#define NU_AHB_RESET "ax_ahb_reset"
#define NU_AXI_GATE "ax_axi_gate"

static int nufreq_get_max_state(struct thermal_cooling_device *cdev, unsigned long *state)
{		
	if (!state)
		return -EINVAL;

	*state = 2;

	return 0;
}

static int nufreq_get_cur_state(struct thermal_cooling_device *cdev, unsigned long *state)
{	
	if (!state)
		return -EINVAL;

	unsigned int curr_freq = gNU->freq;
	if (curr_freq > running_freq_tab[1])
		*state = 0;
	else if (curr_freq > running_freq_tab[2])
		*state = 1;
	else
		*state = 2;

	return 0;
}

static int nufreq_set_cur_state(struct thermal_cooling_device *cdev, unsigned long state)
{
	if (state >= sizeof(running_freq_tab) / sizeof(unsigned int))
		return -EINVAL;

	unsigned int freq = running_freq_tab[state];
	unsigned int bw_limit = band_width_limit[state];
	if(state == 0 && setting_freq > freq) {
		freq = setting_freq;		
	}
	mutex_lock(&nu_mutex);
	if (!special_freq) {
		gNU->freq = set_ax_adpll_total_free_run_nu(0, freq, (unsigned int)gNU->base);
	}
	else {
		gNU->freq = tune_adpll_freq(0, freq, 1, (unsigned int)gNU->base);
	}
	lombo_memctrl_wr_bw_limit(2, 30000, bw_limit);
	lombo_memctrl_rd_bw_limit(2, 30000, bw_limit);
	mutex_unlock(&nu_mutex);
	
	return 0;
}

static struct thermal_cooling_device_ops const nufreq_cooling_ops = {
	.get_max_state = nufreq_get_max_state,
	.get_cur_state = nufreq_get_cur_state,
	.set_cur_state = nufreq_set_cur_state,
};

#define READREG32(reg) *(volatile unsigned int *)(reg)
#define READREG16(reg) *(volatile unsigned short *)(reg)
#define WRITEREG32(reg, val) *(volatile unsigned int *)(reg) = (val)
#define read_reg_bits(reg, pos, width) (READREG32(reg) >> pos) & ((1U << width) -1)
#define write_reg_bits(reg, pos, width, val) WRITEREG32(reg, ((READREG32(reg) & (u32)(~(((1U << width) -1) << pos))) | (u32)((val & ((1U << width) - 1)) << pos)))

static int pdef_rd(unsigned int rd_value)
{
    int ret = 0;
    unsigned short MEM_LAYOUT_VAL = 0, MEM_LAYOUT_VAL_MASK = 0x01e0 , MEM_LAYOUT_VAL_SHIFT = 5; 
    unsigned int pgm_stat = 0, rdy = 0;
    unsigned int MOD = 19, si = 0, pt = 0;
	
	if (!rd_value)
		return -1;

	unsigned int addr_base = 0x01203000 + 0xF0000000;

    pgm_stat = read_reg_bits((addr_base + 0x18), 0, 1);
    write_reg_bits((addr_base + 0x18), 0, 1, pgm_stat);

    MEM_LAYOUT_VAL = READREG16((addr_base + 0x800 + 0x14)) & MEM_LAYOUT_VAL_MASK;

    rdy = read_reg_bits((addr_base + 0x10), 31, 1);
    write_reg_bits((addr_base + 0x10), 31, 1, rdy);
    
    MEM_LAYOUT_VAL = MEM_LAYOUT_VAL >> MEM_LAYOUT_VAL_SHIFT; 

    write_reg_bits((addr_base + 0x30), 16, 5, MEM_LAYOUT_VAL);

    si = rd_value + MOD;

    switch(si)
    {
        case 0: pt = 0; break; 
        case 1: pt = 2; break; 
        case 2: pt = 3; break; 
        case 3: pt = 6; break; 
        case 4: pt = 7; break; 
        case 5: pt = 4; break; 
        case 6: pt = 5; break; 

        /* expand to 4-bit value */
		
        case 7:  pt = 10; break;
        case 8:  pt = 13; break;
        case 9:  pt = 15; break;
        case 10: pt = 8; break;
        case 11: pt = 9; break;
        case 12: pt = 1; break;
        case 13: pt = 11; break;
        case 14: pt = 12; break;
        case 15: pt = 14; break;

        default: pt = 0xFFFF0000; break; 
    }

    if(MEM_LAYOUT_VAL == pt) 
        ret = 0;
    else
        ret = -1;

    return ret;
}

static struct thermal_cooling_device * nufreq_cooling_register(struct device_node *np)
{
	char dev_name[THERMAL_NAME_LENGTH];
	snprintf(dev_name, sizeof(dev_name), "thermal-nufreq");

	struct thermal_cooling_device * cool_dev = thermal_of_cooling_device_register(np, dev_name, NULL, &nufreq_cooling_ops);
	if (IS_ERR(cool_dev)) {
		printk("register cooling device error: %s\n", dev_name);
		return cool_dev;
	}
	
	special_freq = 0;
	if (pdef_rd(0X49911528) == 0 || pdef_rd(0X6C401028) == 0) {
		special_freq = 1;
	}
	if (special_freq == 0) {
		running_freq_tab[0] = 720;
		running_freq_tab[1] = 540;
		running_freq_tab[2] = 360;
	}
	else{
		running_freq_tab[0] = 816;
		running_freq_tab[1] = 612;
		running_freq_tab[2] = 408;
	}
	
	setting_freq = gNU->freq;

	band_width_limit[0] = 480000;//4.8GB/s
	band_width_limit[1] = 240000;
	band_width_limit[2] = 120000;

	return cool_dev;
}

static int asoc_nu_probe(struct platform_device *pdev)
{
	struct asoc_nu_dev *nu;
	struct resource *iores;
	int ret = 0;

	ax_err(1, "asoc_nu_probe start\n");
	nu = devm_kzalloc(&pdev->dev, sizeof(*nu), GFP_KERNEL);
	if (!nu) {
		ret = -ENOMEM;
		goto devm_kzalloc_err;
	}

	iores = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!iores) {
		ret = -EINVAL;
		goto platform_get_resource_err;
	}

	nu->base = devm_ioremap_resource(&pdev->dev, iores);
	if (IS_ERR(nu->base)) {
		ret = PTR_ERR(nu->base);
		goto devm_ioremap_resource_err;
	}

	nu->irq = platform_get_irq(pdev, 0);
	if (nu->irq < 0) {
		ret = nu->irq;
		goto get_irq_err;
	}

	ax_err(1, "resource: iomem: %pR mapping to %p, irq %d, %d\n",
	       iores, nu->base, nu->irq, gax_power_on_flg);
	/* ret = devm_request_threaded_irq(vc_dev->dev, irq_vc, NULL,
					vc_irq_handler, IRQF_ONESHOT,
					dev_name(vc_dev->dev), vc_dev); */
	ret = request_irq(nu->irq, (void *)nu_ISR, 0, DEVDRV_NAME_ENU, 0);
	if (ret) {
		ax_err(1, "nu: register nu irq failed!\n");
		nu_irq_registered = 0;
		ret = -1;
		goto request_irq_err;
	} else {
		nu_irq_registered = 1;
	}
	gNU = nu;
	if (gax_power_on_flg == 0) {
		ax_err(1, "nu --------------- register gax_power_on_flg!\n");
		/* pm_runtime_enable(&pdev->dev); */
		nu_ahb_gate = devm_clk_get(&pdev->dev, NU_AHB_GATE);
		if (IS_ERR(nu_ahb_gate)) {
			ax_err(1, "get nu_ ahb gate err\n");
			ret = PTR_ERR(nu_ahb_gate);
			goto devm_clk_get_nu_ahb_gate_err;
		}

		nu_ahb_reset = devm_clk_get(&pdev->dev, NU_AHB_RESET);
		if (IS_ERR(nu_ahb_reset)) {
			ax_err(1, "get nu_ahb_reset err\n");
			ret = PTR_ERR(nu_ahb_reset);
			goto devm_clk_get_nu_ahb_reset_err;
		}

		nu_axi_gate = devm_clk_get(&pdev->dev, NU_AXI_GATE);
		if (IS_ERR(nu_axi_gate)) {
			ax_err(1, "get nu_axi_gate err\n");
			ret = PTR_ERR(nu_axi_gate);
			goto devm_clk_get_nu_axi_gate_err;
		}

		clk_prepare_enable(nu_ahb_gate);
		clk_prepare_enable(nu_ahb_reset);
		clk_prepare_enable(nu_axi_gate);

		gNU->freq = ax_clk_init(0, (unsigned int)gNU->base);
		set_ax_internal_gating(0, 1, (unsigned int)gNU->base);

		/* get the vc clk, set parent clk & clk rate */
		/* nu_clk = devm_clk_get(&pdev->dev, NU_CLK);
		if (IS_ERR(nu_clk)) {
			ax_err(1,"failed to get nu_clk!\n");
			ret = PTR_ERR(nu_clk);
			goto devm_clk_get_nu_clk_err;
		} */
		/* get display_pll struct */
		/* parent_pll = clk_get(NULL, "ax_pll");
		if (IS_ERR(parent_pll)) {
			ax_err(1,"nu : clk_get parent_pll failed\n");
			ret = PTR_ERR(parent_pll);
			goto clk_get_parent_pll_err;
		} */


	}
	gax_power_on_flg++;
	/* enable_irq(nu->irq); */

	nufreq_cooling_register(pdev->dev.of_node);

	ax_err(1, "dev add %p,  nu_clk %p\n", &pdev->dev, nu_clk);

	init_waitqueue_head(&nu_wait);
	mutex_init(&nu_mutex);

	gax_prcm_clk_en = 0;
	return 0;

//clk_get_parent_pll_err:
//	devm_clk_put(&pdev->dev, nu_clk);
//devm_clk_get_nu_clk_err:
	devm_clk_put(&pdev->dev, nu_axi_gate);
devm_clk_get_nu_axi_gate_err:
	devm_clk_put(&pdev->dev, nu_ahb_reset);
devm_clk_get_nu_ahb_reset_err:
	devm_clk_put(&pdev->dev, nu_ahb_gate);
devm_clk_get_nu_ahb_gate_err:
	free_irq(nu->irq, 0);
request_irq_err:
get_irq_err:
platform_get_resource_err:
devm_ioremap_resource_err:
	devm_kfree(&pdev->dev, nu);
devm_kzalloc_err:
	return ret;
}

static int nu_remove(struct platform_device *pdev)
{
	gax_power_on_flg--;
	if (gax_power_on_flg == 0) {
		/* clk_put(parent_pll); */
		/* devm_clk_put(&pdev->dev, nu_clk); */
		devm_clk_put(&pdev->dev, nu_axi_gate);
		devm_clk_put(&pdev->dev, nu_ahb_reset);
		devm_clk_put(&pdev->dev, nu_ahb_gate);
	}

	if (nu_irq_registered == 1) {
		/* disable_irq(gNU->irq); */
		/* nu_clk_disable(); */
		free_irq(gNU->irq, 0);
	}

	if (gNU != NULL) {
		devm_kfree(&pdev->dev, gNU);
		gNU = NULL;
	}

	return 0;
}

static const  struct file_operations enu_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = nu_ioctl,
	.open = nu_open,
	.release = nu_release,
};


/* static struct platform_device enu_platform_device =
{
    .name   = DEVDRV_NAME_ENU,
    .id = -1,
}; */

static struct platform_driver enu_platform_driver = {
	.driver = {
		.name = DEVDRV_NAME_ENU,
		.owner = THIS_MODULE,
		.of_match_table = lombo_nu_match,
	},
	.suspend = nu_suspend,
	.resume = nu_resume,
	.probe = asoc_nu_probe,
	.remove = nu_remove,
};


static struct miscdevice enu_miscdevice = {
	.minor = 104,
	.name = DEVDRV_NAME_ENU,
	.fops = &enu_fops,
};

static int enu_init(void)
{
	int ret;

	/* 自动insmod，注册设备 */
	ret = misc_register(&enu_miscdevice);
	if (ret) {
		ax_err(1, "register misc_register error\n");
		goto err0;
	}
	ax_err(1, "enu_init v0414\n");

	/* ret = platform_device_register(&enu_platform_device);
	if (ret)
	{
		ax_err(1,"register enu platform device error\n");
	    goto err1;
	} */

	ret = platform_driver_register(&enu_platform_driver);
	if (ret) {
		ax_err(1, "register enu platform driver error\n");
		goto err1;
	}

	return 0;

/* err2:
	platform_device_unregister(&enu_platform_device); */
err1:
	misc_deregister(&enu_miscdevice);
err0:
	return ret;
}

static void enu_exit(void)
{
	ax_err(1, "drv exit\n");
	misc_deregister(&enu_miscdevice);
	/* platform_device_unregister(&enu_platform_device); */
	platform_driver_unregister(&enu_platform_driver);
}

module_init(enu_init);
module_exit(enu_exit);
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("AXNU kernel module");
MODULE_LICENSE("GPL");
