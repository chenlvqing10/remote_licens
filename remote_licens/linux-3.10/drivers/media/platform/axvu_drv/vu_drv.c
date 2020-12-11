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
#include <linux/uaccess.h>
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
#include "clk.h"
#include "utils.h"
#include "vu_drv.h"
#include "vu_cfg.h"


#define VU_PARENT_CLK_RATE 360
#define VU_CLK_RATE 360
/* Timeout counter: 3s */
#define  VU_WAIT_TIMEOUT (HZ/5) /*1s <--> HZ*/
#define  VU_WAIT_TIMEOUT_MS  3000 /*ms*/


#define VU_INTTERUPT_MODE

#define DEVDRV_NAME_CVU		"axvu"


extern int gax_power_on_flg;
extern int gax_prcm_clk_en;

static DEFINE_MUTEX(vu_mutex);
static struct completion vu_complete;
static wait_queue_head_t  vu_wait;

static int vu_status = 0;
static int vu_irq_registered = 0;
static int vu_open_count = 0;
static int vu_log_en = 1;
static int vu_pre_hdl = 0;

static void *pAbuf[_MAX_HANDLES_];

static struct clk *vu_clk;
//static struct clk *parent_pll;
static struct clk *vu_ahb_gate;
static struct clk *vu_ahb_reset;
static struct clk *vu_axi_gate;

typedef struct {
	cvu_stat_info_t stat_info;
	rsz_cfg_t rsz_cfg;
	pym_cfg_t pym_cfg;
	vu_regs_t vu_regs;
	unsigned int vu_count;
	unsigned int vu_status;

}vu_info_t;

struct asoc_vu_dev {
	struct device *dev;
	void __iomem *base;
	struct clk *clk;
	int irq;
	unsigned int setting; /* not used temporarily */
	struct regulator *power; /* not used temporarily */
	unsigned int freq;
};

static struct asoc_vu_dev *gVU;

static unsigned int Re_Reg(unsigned int reg)
{
	unsigned int value = readl((void *)reg);
	return value;
}

static void Wr_Reg(unsigned int reg, unsigned int value)
{
	writel(value, (void *)reg);
}

void init_vu_regs(vu_regs_t *vu_regs)
{
	memset(vu_regs, 0, sizeof(vu_regs_t));
	vu_regs->vu_ctl.reg_addr = CVU_CTL;
	vu_regs->irq_en.reg_addr = CVU_INT_EN;
	vu_regs->input_fmt.reg_addr = CVU_DIN_CFG0;
	vu_regs->input_stride.reg_addr = CVU_DIN_CFG1;
	vu_regs->input_size.reg_addr = CVU_DIN_CFG2;

	vu_regs->input_y_ddr.reg_addr = CVU_DIN_YADDR;
	vu_regs->input_uv_ddr.reg_addr = CVU_DIN_UVADDR;
	vu_regs->output_fmt.reg_addr = CVU_DOUT_CFG0;
	vu_regs->normal_output_size.reg_addr = CVU_DOUT_CFG1;
	vu_regs->output_pym0_size.reg_addr = CVU_DOUT_PYM0_CFG;
	vu_regs->output_pym1_size.reg_addr = CVU_DOUT_PYM1_CFG;
	vu_regs->output_pym2_size.reg_addr = CVU_DOUT_PYM2_CFG;
	vu_regs->output_pym3_size.reg_addr = CVU_DOUT_PYM3_CFG;
	vu_regs->output_pym4_size.reg_addr = CVU_DOUT_PYM4_CFG;
	vu_regs->output_pym5_size.reg_addr = CVU_DOUT_PYM5_CFG;
	vu_regs->output_pym6_size.reg_addr = CVU_DOUT_PYM6_CFG;
	vu_regs->output_pym7_size.reg_addr = CVU_DOUT_PYM7_CFG;

	vu_regs->normal_output_addr.reg_addr = CVU_DOUT_ADDR;
	vu_regs->offset_set.reg_addr = CVU_RSZ_CFG0;
	vu_regs->crop_size.reg_addr = CVU_RSZ_CFG1;
	vu_regs->normal_sclaex.reg_addr = CVU_RSZ_CFG2;
	vu_regs->normal_sclaey.reg_addr = CVU_RSZ_CFG3;

	vu_regs->pym0_scalex.reg_addr = CVU_RSZ_PYM0_CFG0;
	vu_regs->pym0_scaley.reg_addr = CVU_RSZ_PYM0_CFG1;
	vu_regs->pym1_scalex.reg_addr = CVU_RSZ_PYM1_CFG0;
	vu_regs->pym1_scaley.reg_addr = CVU_RSZ_PYM1_CFG1;
	vu_regs->pym2_scalex.reg_addr = CVU_RSZ_PYM2_CFG0;
	vu_regs->pym2_scaley.reg_addr = CVU_RSZ_PYM2_CFG1;
	vu_regs->pym3_scalex.reg_addr = CVU_RSZ_PYM3_CFG0;
	vu_regs->pym3_scaley.reg_addr = CVU_RSZ_PYM3_CFG1;
	vu_regs->pym4_scalex.reg_addr = CVU_RSZ_PYM4_CFG0;
	vu_regs->pym4_scaley.reg_addr = CVU_RSZ_PYM4_CFG1;
	vu_regs->pym5_scalex.reg_addr = CVU_RSZ_PYM5_CFG0;
	vu_regs->pym5_scaley.reg_addr = CVU_RSZ_PYM5_CFG1;
	vu_regs->pym6_scalex.reg_addr = CVU_RSZ_PYM6_CFG0;
	vu_regs->pym6_scaley.reg_addr = CVU_RSZ_PYM6_CFG1;
	vu_regs->pym7_scalex.reg_addr = CVU_RSZ_PYM7_CFG0;
	vu_regs->pym7_scaley.reg_addr = CVU_RSZ_PYM7_CFG1;
	vu_regs->pym_addr.reg_addr = CVU_RSZ_ADDR;
}

/*
	vu / CMDQ independent interupt
*/
static int query_status(void)
{
	int ret_value = vu_status;
	return ret_value;
}

static void vu_reset(void)
{
	u32 rval = 0;
	u32 wait_cnt = 1000;
	Wr_Reg((unsigned int)gVU->base + CVU_RST_STAT, 0x00000100);
	while (wait_cnt & (rval == 0)) {
		rval = Re_Reg((unsigned int)gVU->base + CVU_RST_STAT);
		wait_cnt--;
	}
	Wr_Reg((unsigned int)gVU->base + CVU_RST_STAT, 0x00000000);
	Wr_Reg((unsigned int)gVU->base + CVU_RST, 0x80000000);
	udelay(1);
	Wr_Reg((unsigned int)gVU->base + CVU_RST, 0x00000000);
}

static void vu_stop(void)
{
	if (gax_prcm_clk_en) {
		int cnt = 1000;
		while (CVU_BUSY == query_status()) {
			/* 已暂停或结束？ */

			ax_err(1, "vu_status %d\n", vu_status);
			mdelay(1);
			cnt--;
			if (cnt == 0) {
				ax_err(1, "vu_status err %d\n", vu_status);
				break;
			}
		}
		vu_status = CVU_STOPED;
	}
}

void set_stat_enable(void)
{
	ax_err(0, "gVU->base %x\n", (unsigned int)gVU->base+CVU_PERF_CNT);
	Wr_Reg((unsigned int)gVU->base + CVU_PERF_CNT, 0x80000000);
	ax_err(0, "gVU->base %x\n", (unsigned int)gVU->base+CVU_BW_STAT_EN);
	Wr_Reg((unsigned int)gVU->base + CVU_BW_STAT_EN, 0x80000000);
	ax_err(0, "gVU->base %x\n", (unsigned int)gVU->base+CVU_BW_R_STAT);
	Wr_Reg((unsigned int)gVU->base + CVU_BW_R_STAT, 0x00000000);
	ax_err(0, "gVU->base %x\n", (unsigned int)gVU->base+CVU_BW_W_STAT);
	Wr_Reg((unsigned int)gVU->base + CVU_BW_W_STAT, 0x00000000);
}


static void get_stat_info(cvu_stat_info_t *nu_info)
{
	nu_info->mstat.mtiks = Re_Reg((unsigned int)gVU->base + CVU_PERF_CNT)
		& 0x7FFFFFFF;
	nu_info->mstat.mrbw = Re_Reg((unsigned int)gVU->base + CVU_BW_R_STAT);
	nu_info->mstat.mwbw = Re_Reg((unsigned int)gVU->base + CVU_BW_W_STAT);
}


static inline int disable_vu_irq(void)
{
	int nu_val;
	int valid_flg = 0;

	nu_val = Re_Reg((unsigned int)gVU->base + CVU_INT_PD);
	/* ax_err(1,"isq %x\n",(unsigned int)nu_val); */
	if (nu_val) {
		Wr_Reg((unsigned int)gVU->base + CVU_INT_CLR, nu_val);
		valid_flg |= (1<<16);
		vu_status = CVU_STOPED;
	}
	return valid_flg;
}


irqreturn_t vu_ISR(int irq, void *dev_id)
{
	int valid_flg = disable_vu_irq();
	if ((valid_flg)) {
		complete(&vu_complete);
		wake_up_interruptible(&vu_wait);
	}
	ax_err(0, "irq cvu_value %d\n", vu_status);
	return IRQ_HANDLED;
}
#if 0
static void vu_clk_enable(void)
{
	int ret = 0;
	//unsigned int regval;

	gax_prcm_clk_en++;
	if (gax_prcm_clk_en > 1)
		return;

	/* enable clk */
	ret = clk_set_rate(parent_pll, VU_PARENT_CLK_RATE);

	/*while (1) {
		regval = readl_relaxed((void *)(AX_PLL_STAT));
		if ((regval & 0x7) == 7)
			break;
	}*/

	if (clk_set_parent(vu_clk, parent_pll))
		PRT_ERR("failed to set nu parent to display_pll\n");

	ret = clk_set_rate(vu_clk, VU_CLK_RATE);
	if (ret)
		PRT_ERR("set nu_clk rate err\n");
	clk_prepare_enable(vu_clk);

	/* nu_clk_isenable = 1; */
}

static void vu_clk_disable(void)
{
	gax_prcm_clk_en--;
	if (gax_prcm_clk_en == 0) {
		vu_stop();
		clk_disable_unprepare(vu_clk);
		gax_prcm_clk_en = 0;
	}

}
#endif

long vu_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = CVU_OK;
	int bOK = 0;
	/* unsigned long flags;

	if(0 >= gax_prcm_clk_en)
	{
		ax_err(1,"pls power on!\n");
	} */

	ax_err(0, "cmd %x, arg %x\n", (unsigned int)cmd, (unsigned int)arg);

	switch (cmd) {
	case CVU_RSZ_RUN: {
		void __user *from = (void __user *)arg;
		long time_rest = 0;
		vu_info_t *info;
		mutex_lock(&vu_mutex);

		time_rest = wait_event_interruptible_timeout(vu_wait,
				((vu_status != CVU_BUSY)), VU_WAIT_TIMEOUT);
		if (time_rest <= 0) {
			vu_reset();
			mutex_unlock(&vu_mutex);
			return CVU_ERR_TIMEOUT;
		} else {
			/* already done,clear all & save irqs
			把上一个驱动保存结果 */
			if (vu_pre_hdl >= 1 && vu_pre_hdl < _MAX_HANDLES_) {
				vu_info_t *pinfo =
					(vu_info_t *)pAbuf[vu_pre_hdl - 1];
				if (pinfo != NULL) {
					get_stat_info(&pinfo->stat_info);
					pinfo->vu_status = CVU_STOPED;
				}
			}
		}
		info = (vu_info_t *) filp->private_data;
		bOK = copy_from_user(&info->rsz_cfg, from, sizeof(rsz_cfg_t));

		init_completion(&vu_complete);
		set_stat_enable();
		set_rsz_cfg(info->rsz_cfg, (unsigned int)gVU->base);
		vu_status = CVU_BUSY;

		vu_pre_hdl = info->vu_count;
		info->vu_status = vu_status;
		mutex_unlock(&vu_mutex);
		break;
	}
	case CVU_PYM_RUN: {
		void __user *from = (void __user *)arg;
		long time_rest = 0;
		vu_info_t *info;
		mutex_lock(&vu_mutex);

		time_rest = wait_event_interruptible_timeout(vu_wait,
				((vu_status != CVU_BUSY)), VU_WAIT_TIMEOUT);
		if (time_rest <= 0) {
			vu_reset();
			mutex_unlock(&vu_mutex);
			return CVU_ERR_TIMEOUT;
		} else {

			/* already done,clear all & save irqs
			把上一个驱动保存结果 */
			if (vu_pre_hdl >= 1 && vu_pre_hdl < _MAX_HANDLES_) {
				vu_info_t *pinfo =
					(vu_info_t *)pAbuf[vu_pre_hdl - 1];
				if (pinfo != NULL) {
					get_stat_info(&pinfo->stat_info);
					pinfo->vu_status = CVU_STOPED;
				}
			}
		}
		info = (vu_info_t *) filp->private_data;
		bOK = copy_from_user(&info->pym_cfg, from, sizeof(pym_cfg_t));

		init_completion(&vu_complete);
		set_stat_enable();
		set_pym_cfg(info->pym_cfg, (unsigned int)gVU->base);
		vu_status = CVU_BUSY;

		vu_pre_hdl = info->vu_count;
		info->vu_status = vu_status;
		mutex_unlock(&vu_mutex);
		break;
	}
	case CVU_BTNM_RUN: {
		ax_err(vu_log_en, "Fixed later...\n");
		break;
	}
	case CVU_SET_FREQ: {
		mutex_lock(&vu_mutex);

		vu_stop();
		ax_err(vu_log_en, "freq %d\n", (unsigned int)arg);

			//set_adpll_freq_vu(1, (unsigned int)arg, (unsigned int)gVU->base);
			gVU->freq = set_ax_adpll_total_free_run_vu(1,
											(unsigned int)arg, (unsigned int)gVU->base);

		mutex_unlock(&vu_mutex);
		break;
	}
	case CVU_SET_REG: {
		void __user *from = (void __user *)arg;
		ax_reg_t reg_flg;
		bOK = copy_from_user(&reg_flg, from, sizeof(ax_reg_t));
		Wr_Reg((unsigned int)gVU->base + reg_flg.reg_addr,
				reg_flg.reg_val);
		ax_err(vu_log_en, "reg %x,%x\n",
				reg_flg.reg_addr, reg_flg.reg_val);
		break;
	}
	case CVU_GET_REG: {
		void __user *from = (void __user *)arg;
		ax_reg_t reg_flg;
		bOK = copy_from_user(&reg_flg, from, sizeof(ax_reg_t));
		reg_flg.reg_val =
			Re_Reg((unsigned int)gVU->base + reg_flg.reg_addr);
		ret = reg_flg.reg_val;
		ax_err(vu_log_en, "reg %x,%x\n",
				reg_flg.reg_addr, reg_flg.reg_val);
		break;
	}
	case CVU_GET_FREQ: {
		void __user *to = (void __user *)arg;
		int tfreq = 0;
		mutex_lock(&vu_mutex);

		/* ret = get_adpll_freq_vu(1, (unsigned int)gVU->base); */
		tfreq = gVU->freq;//get_adpll_freq_vu(0,(unsigned int)gVU->base);
		bOK = copy_to_user(to, (void *)&tfreq, sizeof(unsigned int));
		ax_err(vu_log_en, "get freq %d\n", tfreq);

		mutex_unlock(&vu_mutex);
		break;
	}
	case CVU_GET_VER: {
		unsigned int ver_flg =
			Re_Reg((unsigned int)gVU->base + AX_TOP_VER);
		ax_err(vu_log_en, "version %x, %d\n",
			(unsigned int)gVU->base + AX_TOP_VER,
			(unsigned int)ver_flg);
		break;
	}
	case CVU_LOG: {
		vu_log_en = (unsigned int)arg;
		ax_err(1, "log_en %d\n", (unsigned int)vu_log_en);
		break;
	}
	case CVU_STATUS: {
		vu_info_t *pinfo = (vu_info_t *) filp->private_data;
		ret = pinfo->vu_status;
		break;
	}
	case CVU_QUERY_FINISH: {
		unsigned long timeout;
		int left_time;
		unsigned long expire;
		vu_info_t *info;
		int cvu_value;
		void __user *to;
		mutex_lock(&vu_mutex);
		to = (void __user *)arg;
		info = (vu_info_t *) filp->private_data;
		if (info->vu_status == CVU_STOPED) {
			bOK = copy_to_user(to, &info->stat_info,
					sizeof(cvu_stat_info_t));
			clear_rsz_cfg((unsigned int)gVU->base);
			clear_pym_cfg((unsigned int)gVU->base);
			mutex_unlock(&vu_mutex);
			return 0;
		}


#ifdef VU_INTTERUPT_MODE
		cvu_value = query_status();
		ax_err(0, "cvu_value %d\n", cvu_value);
		if (cvu_value != CVU_BUSY) {
			get_stat_info(&info->stat_info);
			bOK = copy_to_user(to, &info->stat_info,
					sizeof(cvu_stat_info_t));
			info->vu_status = CVU_STOPED;
			clear_rsz_cfg((unsigned int)gVU->base);
			clear_pym_cfg((unsigned int)gVU->base);
			mutex_unlock(&vu_mutex);
			goto out;
		}

		timeout = msecs_to_jiffies(VU_WAIT_TIMEOUT_MS) + 1;
		if (vu_irq_registered) {
			left_time = wait_for_completion_timeout(&vu_complete,
					timeout);
			if (unlikely(left_time == 0)) {
				int cvu_status = query_status();
				ax_err(1, "vu timeout!\n");

				if (cvu_status == CVU_STOPED) {
					ret = 0;
					get_stat_info(&info->stat_info);
					bOK = copy_to_user(to, &info->stat_info,
						sizeof(cvu_stat_info_t));
					info->vu_status = CVU_STOPED;
					clear_rsz_cfg((unsigned int)gVU->base);
					clear_pym_cfg((unsigned int)gVU->base);
					mutex_unlock(&vu_mutex);
					goto out;
				}

				info->vu_status = CVU_STOPED;
				vu_status = CVU_STOPED;
				ret = CVU_ERR_TIMEOUT;
				disable_vu_irq();
				vu_reset();
				clear_rsz_cfg((unsigned int)gVU->base);
				clear_pym_cfg((unsigned int)gVU->base);
				mutex_unlock(&vu_mutex);
				goto out;
			} else {
				/* normal case */
				ret = 0;
				ax_err(0, "vu time ok!\n");
				get_stat_info(&info->stat_info);
				bOK = copy_to_user(to, &info->stat_info,
						sizeof(cvu_stat_info_t));
				info->vu_status = CVU_STOPED;
				vu_status = CVU_STOPED;
				clear_rsz_cfg((unsigned int)gVU->base);
				clear_pym_cfg((unsigned int)gVU->base);
				mutex_unlock(&vu_mutex);
				goto out;
			}
		}

		expire = timeout + jiffies;
		do {
			cvu_value = query_status();

			if (cvu_value != CVU_BUSY) {
				get_stat_info(&info->stat_info);
				bOK = copy_to_user(to, &info->stat_info,
						sizeof(cvu_stat_info_t));
				info->vu_status = CVU_STOPED;
				clear_rsz_cfg((unsigned int)gVU->base);
				clear_pym_cfg((unsigned int)gVU->base);
				mutex_unlock(&vu_mutex);
				goto out;
			}

			if (time_after(jiffies, expire)) {
				info->vu_status = CVU_STOPED;
				vu_status = CVU_STOPED;
				ret = CVU_ERR_TIMEOUT;
				disable_vu_irq();
				vu_reset();
				clear_rsz_cfg((unsigned int)gVU->base);
				clear_pym_cfg((unsigned int)gVU->base);
				mutex_unlock(&vu_mutex);
				goto out;
			}
		} while (1);

#else
		cvu_value = 0;
		{
			int time_counts = 0xfffffff;
			do {
				cvu_value = query_status();
				if (time_counts-- < 0) {
					nfo->vu_status = CVU_STOPED;
					ret = CVU_ERR_TIMEOUT;
					disable_vu_irq();
					vu_reset();
					mutex_unlock(&vu_mutex);
					goto out;
				}
			} while (cvu_value == CVU_BUSY);

			get_stat_info(&info->stat_info);
			bOK = copy_to_user(to, &info->stat_info,
					sizeof(cvu_stat_info_t));
			info->vu_status = CVU_STOPED;
			mutex_unlock(&vu_mutex);
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

int vu_open(struct inode *inode, struct file *filp)
{
	vu_info_t *info = NULL;
	/* int ret = 0; */
	mutex_lock(&vu_mutex);
	vu_open_count++;
	filp->private_data = NULL;
	ax_err(1, "======vu_open_count error %d\n", vu_open_count);
	if (vu_open_count > _MAX_HANDLES_) {
		ax_err(1, "vu_open_count error %d\n", vu_open_count);
		vu_open_count--;
		mutex_unlock(&vu_mutex);
		return -1;
	}

	info = (vu_info_t *)vu_malloc(sizeof(vu_info_t));
	if (info == NULL) {
		ax_err(1, "ax_malloc fail\n");
		vu_open_count--;
		mutex_unlock(&vu_mutex);
		return -1;
	}
	init_vu_regs(&info->vu_regs);
	ax_err(1, "ax_open %d\n", vu_open_count);
	/* init completion var */
	if (vu_open_count == 1) {
		/* ax_clk_init_vu((unsigned int)gVU->base); */
		//set_adpll_freq_vu(1, VU_CLK_RATE, (unsigned int)gVU->base);
		/* vu_clk_enable(); */
		init_completion(&vu_complete);
	}
	pAbuf[vu_open_count - 1] = (void *)info;
	/* 当前vce info序列号，从1开始 */
	info->vu_count = vu_open_count;
	info->vu_status = CVU_IDLE;
	filp->private_data = (void *)info;
	mutex_unlock(&vu_mutex);

	return 0;
}

int vu_release(struct inode *inode, struct file *filp)
{
	vu_info_t *info = (vu_info_t *) filp->private_data;

	if (info == NULL)
		return 0;

	mutex_lock(&vu_mutex);
	vu_open_count--;
	ax_err(1, "vu_release %d\n", vu_open_count);

	vu_stop();
	if (vu_open_count >= 0) {
		if (info) {
			vu_free(info);
			info = filp->private_data = NULL;
		}

		pAbuf[vu_open_count] = NULL;
	} else if (vu_open_count < 0) {
		vu_open_count = 0;
	}


	if (vu_open_count == 0) {
		vu_pre_hdl = 0;
		/* ax_clk_deinit_vu();
		vu_clk_disable(); */
	}

	mutex_unlock(&vu_mutex);

	return 0;
}




int vu_suspend(struct platform_device *dev, pm_message_t state)
{
	mutex_lock(&vu_mutex);
	ax_err(1, "vu_suspend %d\n", vu_open_count);
	vu_stop();
	if (gax_power_on_flg == 1) {

		clk_disable_unprepare(vu_ahb_gate);
		clk_disable_unprepare(vu_ahb_reset);
		clk_disable_unprepare(vu_axi_gate);
		set_ax_adpll_total_free_run_vu(1, 0, (unsigned int)gVU->base);
		gax_power_on_flg--;
	} else if (gax_power_on_flg > 1) {
		gax_power_on_flg--;
		ax_err(1, "vu gax_power_on_flg %d\n", gax_power_on_flg);
	} else {
		ax_err(1, "should not happend vu gax_power_on_flg %d\n",
							gax_power_on_flg);
	}

	mutex_unlock(&vu_mutex);

	return 0;
}

int vu_resume(struct platform_device *dev)
{
	mutex_lock(&vu_mutex);
	ax_err(1, "vu_resume\n");
	if (gax_power_on_flg == 0) {

		clk_prepare_enable(vu_ahb_gate);
		clk_prepare_enable(vu_ahb_reset);
		clk_prepare_enable(vu_axi_gate);
		gax_power_on_flg++;
	} else if (gax_power_on_flg == 1) {
		gax_power_on_flg++;
		ax_err(1, "vu gax_power_on_flg %d\n", gax_power_on_flg);
	} else {
		ax_err(1, "should not happend vu gax_power_on_flg %d\n",
							gax_power_on_flg);
	}

	ax_clk_init_vu(1, (unsigned int)gVU->base);
	set_ax_internal_gating_vu(1, 1, (unsigned int)gVU->base);
	set_ax_adpll_total_free_run_vu(1, gVU->freq, (unsigned int)gVU->base);
	mutex_unlock(&vu_mutex);
	return 0;
}

#define VU_CLK				"ax_module_clk"
#define VU_AHB_GATE			"ax_ahb_gate"
#define VU_AHB_RESET		"ax_ahb_reset"
#define VU_AXI_GATE			"ax_axi_gate"

static int asoc_vu_probe(struct platform_device *pdev)
{
	struct asoc_vu_dev *vu;
	//struct resource *iores;
	int ret = 0;

	ax_err(1, "asoc_vu_probe start-------------------\n");
	vu = devm_kzalloc(&pdev->dev, sizeof(*vu), GFP_KERNEL);
	if (!vu) {
		ret = -ENOMEM;
		goto devm_kzalloc_err;
	}

	/* iores = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!iores) {
		ret = -EINVAL;
		goto platform_get_resource_err;
	} */

	vu->base = (void *)0xf1700000;
	/*vu->base = devm_ioremap_resource(&pdev->dev, iores);
	if (IS_ERR(vu->base)) {
		ret = PTR_ERR(vu->base);
		goto devm_ioremap_resource_err;
	}*/

	vu->irq = platform_get_irq(pdev, 0);
	if (vu->irq < 0) {
		ret = vu->irq;
		goto get_irq_err;
	}

	ax_err(1, "vu resource: mapping to %p, irq %d, %d\n",
	        vu->base, vu->irq, gax_power_on_flg);

	ret = request_irq(vu->irq, (void *)vu_ISR, 0, DEVDRV_NAME_CVU, 0);
	if (ret) {
		ax_err(1, "vu : register vu irq failed!\n");
		vu_irq_registered = 0;
		ret = -1;
		goto request_irq_err;
	} else {
		vu_irq_registered = 1;
	}
	gVU = vu;
	if (gax_power_on_flg == 0) {
		/* pm_runtime_enable(&pdev->dev); */
		vu_ahb_gate = devm_clk_get(&pdev->dev, VU_AHB_GATE);
		if (IS_ERR(vu_ahb_gate)) {
			ax_err(1, "get vu_ ahb gate err\n");
			ret = PTR_ERR(vu_ahb_gate);
			goto devm_clk_get_vu_ahb_gate_err;
		}

		vu_ahb_reset = devm_clk_get(&pdev->dev, VU_AHB_RESET);
		if (IS_ERR(vu_ahb_reset)) {
			ax_err(1, "get vu_ahb_reset err\n");
			ret = PTR_ERR(vu_ahb_reset);
			goto devm_clk_get_vu_ahb_reset_err;
		}

		vu_axi_gate = devm_clk_get(&pdev->dev, VU_AXI_GATE);
		if (IS_ERR(vu_axi_gate)) {
			ax_err(1, "get vu_axi_gate err\n");
			ret = PTR_ERR(vu_axi_gate);
			goto devm_clk_get_vu_axi_gate_err;
		}

		clk_prepare_enable(vu_ahb_gate);
		clk_prepare_enable(vu_ahb_reset);
		clk_prepare_enable(vu_axi_gate);
	}

	gVU->freq = ax_clk_init_vu(1, (unsigned int)gVU->base);
	set_ax_internal_gating_vu(1, 1, (unsigned int)gVU->base);

	/* get the vc clk, set parent clk & clk rate */
	/* vu_clk = devm_clk_get(&pdev->dev, VU_CLK);
	if (IS_ERR(vu_clk)) {
		ax_err(1,"failed to get vu_clk!\n");
		ret = PTR_ERR(vu_clk);
		goto devm_clk_get_vu_clk_err;
	} */
	/* get display_pll struct */
	/* parent_pll = clk_get(NULL, "ax_pll");
	if (IS_ERR(parent_pll)) {
		ax_err(1,"vu : clk_get parent_pll failed\n");
		ret = PTR_ERR(parent_pll);
		goto clk_get_parent_pll_err;
	} */

	gax_power_on_flg++;
	/* enable_irq(vu->irq); */

	/* set_stat_enable(); */

	ax_err(1, "dev add %p,  vu_clk %p\n", &pdev->dev, vu_clk);
	init_waitqueue_head(&vu_wait);
	mutex_init(&vu_mutex);
	/* vu_clk_isenable = 0; */
	return 0;

//clk_get_parent_pll_err:
//	devm_clk_put(&pdev->dev, vu_clk);
//devm_clk_get_vu_clk_err:
	devm_clk_put(&pdev->dev, vu_axi_gate);
devm_clk_get_vu_axi_gate_err:
	devm_clk_put(&pdev->dev, vu_ahb_reset);
devm_clk_get_vu_ahb_reset_err:
	devm_clk_put(&pdev->dev, vu_ahb_gate);
devm_clk_get_vu_ahb_gate_err:
	free_irq(vu->irq, 0);
request_irq_err:
get_irq_err:
//platform_get_resource_err:
//devm_ioremap_resource_err:
	devm_kfree(&pdev->dev, vu);
devm_kzalloc_err:
	return ret;
}

static int vu_remove(struct platform_device *pdev)
{
	gax_power_on_flg--;
	if (gax_power_on_flg == 0) {
		/* clk_put(parent_pll);
		devm_clk_put(&pdev->dev, vu_clk); */
		devm_clk_put(&pdev->dev, vu_axi_gate);
		devm_clk_put(&pdev->dev, vu_ahb_reset);
		devm_clk_put(&pdev->dev, vu_ahb_gate);
	}

	if (vu_irq_registered == 1)
		free_irq(gVU->irq, 0);
	if (gVU != NULL) {
		devm_kfree(&pdev->dev, gVU);
		gVU = NULL;
	}
	return 0;
}

static const struct file_operations cvu_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = vu_ioctl,
	.open = vu_open,
	.release = vu_release,
};

#if 0
static struct platform_device cvu_platform_device = {
	.name   = DEVDRV_NAME_CVU,
	.id = -1,
};
#endif

static struct of_device_id lombo_vu_match[] = {
	{ .compatible = "lombo,n7-axvu", },
	{ },
};
MODULE_DEVICE_TABLE(of, lombo_vu_match);

static struct platform_driver cvu_platform_driver = {
	.driver = {
		.name = DEVDRV_NAME_CVU,
		.owner = THIS_MODULE,
		.of_match_table = lombo_vu_match,
	},
	.suspend = vu_suspend,
	.resume = vu_resume,
	.probe = asoc_vu_probe,
	.remove = vu_remove,
};


static struct miscdevice cvu_miscdevice = {
	.minor = 105,
	.name = DEVDRV_NAME_CVU,
	.fops = &cvu_fops,
};


static int cvu_init(void)
{
	int ret;

	/* 自动insmod，注册设备 */
	ret = misc_register(&cvu_miscdevice);
	if (ret) {
		ax_err(1, "register misc_register error\n");
		goto err0;
	}
	ax_err(1, "cvu_init\n");

	/*ret = platform_device_register(&cvu_platform_device);
	if (ret)
	{
		ax_err(1,"register cvu platform device error\n");
	    goto err1;
	}*/

	ret = platform_driver_register(&cvu_platform_driver);
	if (ret) {
		ax_err(1, "register cvu platform driver error\n");
		goto err1;
	}

	/* init_waitqueue_head(&vu_wait); */
	return 0;

/* err2:
	platform_device_unregister(&cvu_platform_device); */

err1:
	misc_deregister(&cvu_miscdevice);

err0:
	return ret;
}

static void cvu_exit(void)
{
	ax_err(1, "drv exit\n");
	misc_deregister(&cvu_miscdevice);
	/* platform_device_unregister(&cvu_platform_device); */
	platform_driver_unregister(&cvu_platform_driver);
}

module_init(cvu_init);
module_exit(cvu_exit);
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("AXVU kernel module");
MODULE_LICENSE("GPL");
