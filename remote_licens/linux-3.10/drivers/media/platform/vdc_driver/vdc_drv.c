#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/errno.h>    /* error codes */
#include <linux/vmalloc.h>
#include <linux/slab.h>     /* kmalloc/kfree */
#include <linux/init.h>     /* module_init/module_exit */
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/sem.h>
#include <linux/time.h>
#include <linux/reset.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <mach/csp.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <mach/debug.h>
#include "vdc_core.h"
#include "vdc_drv.h"


#undef  MOD_NAME
#define MOD_NAME		"N7_VDC"

#define DEVDRV_NAME_VDC		"vdc"
#define MAX_INSTANCE_NUM	8
#define CLK_NAME_DEVCLK		"dev_clk"
#define CLK_NAME_DISPLAYPLL	"display_pll"
#define VDC_CLK			"vdc_module_clk"
#define VDC_AHB_GATE		"vdc_ahb_gate"
#define VDC_AHB_RESET		"vdc_ahb_reset"
#define VDC_AXI_GATE		"vdc_axi_gate"
#define VDC_CLK_RATE		300000000

#define DECODE_ERROR_CNT	3
/*
   static void __iomem *ahb_bus_clock_gating_reg;
   static void __iomem *ahb_bus_reset_reg;
   static void __iomem *axi_buf_clock_gating_reg;
   static void __iomem *vdc_pll_reg;
   static void __iomem *vdc_pll_fac;
   static void __iomem *vdc_pll_tune0;
   static void __iomem *tmp_tmp;
*/


static int vdc_cur_slot_id = -1;

/*jpeg slice pattern, can't free if frame is not completed*/
static int vdc_slice_slot_id = -1;

/*0:normal, -1:error;*/
static int vdc_last_status;

static int vdc_irq_registered;
static int vdc_open_count;

/*vdc shoule be occupied when jpeg slice pattern that
	may needs some times when decoded*/
static int vdc_occupied;

/*for multi process*/
static struct mutex m_mutex;

/* vdc can accept a new RUN order if finish */
static int vdc_idle_flag = 1;

static int vdc_clk_isenable;

static int vdc_is_suspend = -1;

static int decode_error_cnt = DECODE_ERROR_CNT;

static DECLARE_WAIT_QUEUE_HEAD(waitqueue);

typedef struct {
	unsigned int regs[MAX_VDC_REG_NUM];
} vdc_user_data_t;

struct asoc_vdc_dev {
	struct device *dev;
	void __iomem *base;
	struct clk *clk;
	int irq;
	unsigned int setting;
	struct regulator *power;
};

static struct asoc_vdc_dev *GVDC;
static struct clk *vdc_clk;
static struct clk *g_clk_null;
static struct clk *parent_pll;
static struct clk *vdc_ahb_gate;
static struct clk *vdc_ahb_reset;
static struct clk *vdc_axi_gate;
static struct platform_device *gvdcdev;

static void vdc_write(struct asoc_vdc_dev *vdc, u32 val, u32 reg)
{
	void *write_add = (void *)(vdc->base + reg);
	writel_relaxed(val, write_add);
}

static u32 vdc_read(struct asoc_vdc_dev *vdc, u32 reg)
{
	unsigned int data = 0;

	data = readl_relaxed((void *)(vdc->base + reg));
	return data;
}

static const struct of_device_id vdc_of_match[] = {
	{.compatible = "lombo,n7-vdc",},
	{}
};

MODULE_DEVICE_TABLE(of, vdc_of_match);

typedef enum {
	SLOT_IDLE,
	SLOT_OCCUPIED,
	SLOT_COMPLETED
} slot_state_e;

typedef struct slot_s {

	struct completion isr_complete;
	slot_state_e state;
	unsigned long clientregptr;
	unsigned int vdc_status;
	unsigned int slice_mode;
	int pid;
	vdc_user_data_t user_data;

} slot_t;

static slot_t slot[MAX_INSTANCE_NUM];
static void vdc_drv_flushreg_again(int id);

static int slot_reset(int i)
{
	init_completion(&slot[i].isr_complete);
	slot[i].state = SLOT_IDLE;
	slot[i].clientregptr = 0;
	slot[i].slice_mode = 0;
	slot[i].pid = -1;
	return 0;
}

static int slot_get(void)
{
	int i;

	for (i = 0; i < MAX_INSTANCE_NUM; i++) {
		if (slot[i].state == SLOT_IDLE) {
			init_completion(&slot[i].isr_complete);
			slot[i].state = SLOT_OCCUPIED;
			slot[i].pid = task_tgid_vnr(current);
			return i;
		}
	}
	if (i == MAX_INSTANCE_NUM) {
		PRT_ERR("vdc : no idle slot, max %d slots\n", MAX_INSTANCE_NUM);
		return -1;
	}
	return -1;
}

static int slot_complete(int i, unsigned int vdc_status)
{
	if (slot[i].state != SLOT_OCCUPIED) {
		PRT_ERR("vdc : slot is idle, staus error\n");
		return -1;
	}

	slot[i].vdc_status = vdc_status;
	slot[i].state = SLOT_COMPLETED;

	return 0;
}

static int vdc_query_interval(void)
{
	int r = 5;

	return r;
}

static void vdc_waitforidle(void)
{
	int ctr = 0;

	int v = vdc_read(GVDC, 0x4);
	if ((v & 0x1) == 0x1) {
		PRT_INFO("vdc : vdc is on air\n");
		do {
			msleep(vdc_query_interval());
			ctr++;
			if (ctr == 100) {
				PRT_ERR("vdc : vdc always busy\n");
				break;
			}
		} while (((vdc_read(GVDC, 0x4)) & 0x1) == 0x1);
	}
}




/*============== funcs:clock, power, reset, irq =============*/
static void vdc_do_reset(void)
{
	clk_disable(vdc_ahb_reset);
	usleep_range(1*1000, 1*1000);
	clk_enable(vdc_ahb_reset);
	usleep_range(1*1000, 1*1000);
	return;
}

static void vdc_reset(void)
{
	vdc_do_reset();
}

static void vdc_reset_and_sleep(int cnt)
{
	unsigned int time;

	if (cnt <= 0)
		time = 5 * 1000;
	else if (cnt == 1)
		time = 10 * 1000;
	else if (cnt == 2)
		time = 1 * 1000;
	else
		time = 5 * 1000;

	clk_disable(vdc_ahb_reset);
	usleep_range(time, time + 1);
	clk_enable(vdc_ahb_reset);
	usleep_range(time, time + 1);
	return;
}

static void vdc_clk_enable(void)
{
	int ret = 0;

	if (vdc_clk_isenable != 0)
		return;

	/*power on*/
	PRT_DBG("dev add %p,vdc_clk %p\n", &gvdcdev->dev, vdc_clk);
/*
	res = pm_runtime_get_sync(&gvdcdev->dev);
	if (res < 0) {
		printk("vdc_clk_enable: pm_runtime_get_sync failed\n");
		return;
	}
*/
	/*enable clk*/
	PRT_DBG("set vdc clock to displaypll\n");
	if (clk_set_parent(vdc_clk, parent_pll))
		PRT_ERR("failed to set vdc parent to display_pll\n");

	ret = clk_set_rate(vdc_clk, VDC_CLK_RATE);
	if (ret)
		PRT_ERR("set vdc_clk rate err\n");

	clk_prepare_enable(vdc_clk);
	vdc_clk_isenable = 1;
}

static void vdc_clk_disable(void)
{
	struct clk *null_clk = NULL;

	PRT_DBG("vdc : vdc_clk_disable, In\n");
	if (vdc_clk_isenable == 0)
		return;

	vdc_waitforidle();
	/* set parent to null */
	null_clk = clk_get(NULL, "null_clk");
	clk_set_parent(vdc_clk, null_clk);
	clk_put(null_clk);
	/*disable clk*/
	clk_disable_unprepare(vdc_clk);

	vdc_clk_isenable = 0;
	/*power off*/
/*
	res = pm_runtime_put_sync(&gvdcdev->dev);
	if (res < 0) {
		printk("vdc_clk_disable: pm_runtime_put_sync failed\n");
		return;
	}
*/
	PRT_DBG("vdc : vdc_clk_disable, Out\n");
}

#define MAX_VDC_REG_RETRY_TIME  5
/* enable int */
static inline void enable_vdc_irq(void)
{
}

static inline void disable_vdc_irq(void)
{
	unsigned int v;
	int c;

	c = MAX_VDC_REG_RETRY_TIME;
	v = vdc_read(GVDC, 0x4);
	v &= ~(1<<8);
	vdc_write(GVDC, v, 0x4);

	while ((vdc_read(GVDC, 0x4)) & (0x1<<8) && c-- > 0) {
		PRT_ERR("vdc : can not disable irq\n");
		vdc_write(GVDC, v, 0x4);
	}
}
/*=====================================================*/

static void vdc_drv_updatereg(int id)
{
	int i;
	/* we never handle reg0, reg1 */
	for (i = 1; i < MAX_VDC_REG_NUM; i++)
		slot[id].user_data.regs[i] =
			(unsigned int)(vdc_read(GVDC, i*4));
}

static void vdc_drv_showreg(int id)
{
	if (id >= 0) {
		PRT_INFO("vdc : (showReg-1) 0x%08x 0x%08x 0x%08x 0x%08x\n",
			slot[id].user_data.regs[1], slot[id].user_data.regs[2],
			slot[id].user_data.regs[3], slot[id].user_data.regs[4]);
	} else {
		int i = 0;

		PRT_ERR("vdc :(showReg-2)[%d] 0x%08x 0x%08x 0x%08x 0x%08x\n", i,
			(unsigned int)(vdc_read(GVDC, (i+0)*4)),
			(unsigned int)(vdc_read(GVDC, (i+1)*4)),
			(unsigned int)(vdc_read(GVDC, (i+2)*4)),
			(unsigned int)(vdc_read(GVDC, (i+3)*4)));

	}
}

/**
 * This function is VDC ISR.
 */
irqreturn_t VDC_ISR(int irq, void *dev_id)
{
	unsigned int s;

	disable_vdc_irq();
	slot_complete(vdc_cur_slot_id, vdc_read(GVDC, 0x4));
	vdc_last_status = 0;

	/*when bistream empty or jpeg slice ready, and not frame ready.*/
	s = vdc_read(GVDC, 0x4);

	if (((s & (0x1<<17)) || (s & (0x1<<14))) && !(s & (0x1<<12))) {
		slot[vdc_cur_slot_id].slice_mode = 1;
		vdc_slice_slot_id = vdc_cur_slot_id;
		slot[vdc_slice_slot_id].state = SLOT_OCCUPIED;
	} else {
		slot[vdc_cur_slot_id].slice_mode = 0;
		vdc_occupied = 0;
	}

	slot[vdc_cur_slot_id].vdc_status = vdc_read(GVDC, 0x4);
	vdc_idle_flag = 1;

	if (slot[vdc_cur_slot_id].vdc_status & 0x1da000) {
		/*when meet some error.*/
		vdc_drv_showreg(-1);
		vdc_last_status = -1;
		decode_error_cnt--;
		PRT_ERR("decode_error_cnt : %d\n", decode_error_cnt);
		if (decode_error_cnt <= 0)
			vdc_drv_updatereg(vdc_cur_slot_id);
	} else {
		vdc_drv_updatereg(vdc_cur_slot_id);
	}

	complete(&(slot[vdc_cur_slot_id].isr_complete));
	wake_up_interruptible(&waitqueue); /*wake up*/

	return IRQ_HANDLED;
}

static void vdc_drv_writereg(unsigned int regno, unsigned int value)
{
	/*unsigned int value_recover;*/
	vdc_write(GVDC, value, regno*4);
}

static void vdc_drv_flushreg(int id, void __user *v)
{
	int i, rt;

	rt = copy_from_user(&(slot[id].user_data.regs[0]),
				v, sizeof(vdc_user_data_t));
	if (rt != 0)
		PRT_ERR("vdc : Warning: copy_from_user failed, rt=%d\n", rt);

	/* we never handle reg0, reg1 */
	for (i = 2; i < MAX_VDC_REG_NUM; i++) {
		/* printk("vdc : %d --- %x\n",i,slot[id].user_data.regs[i]); */
		vdc_drv_writereg(i, slot[id].user_data.regs[i]);
	}

	for (i = 2; i < MAX_VDC_REG_NUM; i++) {
		;/* unsigned int value = 0; */
		/* value = vdc_read(GVDC, 4*i); */
		/* PRT_ERR("vdc : %d --- %x\n", i, value); */
	}
}

static void vdc_drv_flushreg_again(int id)
{
	int i;
	/* we never handle reg0, reg1 */
	for (i = 2; i < MAX_VDC_REG_NUM; i++) {
		/* printk("vdc : %d --- %x\n",i,slot[id].user_data.regs[i]); */
		vdc_drv_writereg(i, slot[id].user_data.regs[i]);
	}

	for (i = 2; i < MAX_VDC_REG_NUM; i++) {
		;/* unsigned int value = 0; */
		/* value = vdc_read(GVDC, 4*i); */
		/* PRT_ERR("vdc : %d --- %x\n", i, value); */
	}
}

vdc_status_t vdc_query_status(unsigned int vdc_status)
{
	if (vdc_status & (0x1<<12))
		return VDC_STATUS3;
	else if (vdc_status & (0x1<<18))
		return VDC_STATUS8;
	else if (vdc_status & (0x1<<16))
		return VDC_STATUS9;
	else if (vdc_status & (0x1<<17))
		return VDC_STATUS4;
	else if (vdc_status & (0x1<<15))
		return VDC_STATUS_UNKNOWN_ERROR;
	else if (vdc_status & (0x1<<14))
		return VDC_STATUS6;
	else if (vdc_status & (0x1<<13))
		return VDC_STATUS10;
	else {
		PRT_ERR("vdc: status unknow error, status: 0x%x\n", vdc_status);
		return VDC_STATUS_UNKNOWN_ERROR;
	}
}


static long vdc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int left_time, id, s, slot_id, jpeg_on_going;

	switch (cmd) {
	case VDC_RUN:
	/* return 0; */

	if (arg != 0) {

		jpeg_on_going = 0;

		mutex_lock(&m_mutex);

		decode_error_cnt = DECODE_ERROR_CNT;

		/*continue if completed, interrupted or timeout*/
		left_time = wait_event_interruptible_timeout(
				waitqueue, vdc_idle_flag == 1,
					msecs_to_jiffies(5*1000) + 1);
		if (unlikely(left_time == 0))  {
			PRT_ERR("vdc :wait_event_interruptible_timeout 5s\n");
			vdc_reset();
			mutex_unlock(&m_mutex);
			return VDC_STATUS_UNKNOWN_ERROR;
		}

		if (vdc_last_status == -1)
			vdc_reset();

		if (vdc_occupied == 1) {
			/* FIXME: Arg should not change that
				next slice comes in slice pattern */
			if (arg != slot[vdc_slice_slot_id].clientregptr) {
				PRT_ERR("vdc : VDC_RUN --- UNKNOWN_ERROR\n");
				mutex_unlock(&m_mutex);
				return VDC_STATUS_UNKNOWN_ERROR;
			} else {
				jpeg_on_going = 1;
			}
		}

		if (jpeg_on_going) {
			id = vdc_slice_slot_id;
		} else {
		/*Get a slot. It needs several slots
			if multitasks run VDC, that can't VDC_QUERY in time*/
			id = slot_get();
			if (id < 0) {
				mutex_unlock(&m_mutex);
				return VDC_STATUS_UNKNOWN_ERROR;
			}
		}

		/*backup data*/
		vdc_cur_slot_id = id;
		slot[id].clientregptr = arg;
		vdc_drv_flushreg(id, (void __user *)arg);
/*
{
	unsigned int value;
	ahb_bus_clock_gating_reg = ioremap_nocache(0x0400a000 + 0x204, 0x4);
	value = readl_relaxed(ahb_bus_clock_gating_reg);
	PRT_ERR(" ahb_bus_clock_gating_reg : %x\n", value);

	ahb_bus_reset_reg = ioremap_nocache(0x0400a000 + 0x224, 0x4);
	value = readl_relaxed(ahb_bus_reset_reg);
	PRT_ERR(" ahb_bus_reset_reg : %x\n", value);

	axi_buf_clock_gating_reg = ioremap_nocache(0x0400a000 + 0x340, 0x4);
	value = readl_relaxed(axi_buf_clock_gating_reg);
	PRT_ERR(" axi_buf_clock_gating_reg1 : %x\n", value);

	vdc_pll_fac = ioremap_nocache(0x0400a000 + 0x5a4, 0x4);
	value = readl_relaxed(vdc_pll_fac);
	PRT_ERR(" vdc_pll_fac1 : %x\n", value);

	vdc_pll_tune0 = ioremap_nocache(0x0400a000 + 0x5a8, 0x4);
	value = readl_relaxed(vdc_pll_tune0);
	PRT_ERR(" vdc_pll_tune0 : %x\n", value);

	vdc_pll_reg = ioremap_nocache(0x0400a000 + 0x09c4, 0x4);
	value = readl_relaxed(vdc_pll_reg);
	PRT_ERR(" vdc_pll_reg1 : %x\n", value);

	tmp_tmp = ioremap_nocache(0x01051000 + 0x84, 0x4);
	value = readl_relaxed(tmp_tmp);
	PRT_ERR(" tmp0x01051000_0x84 : %x\n", value);

	tmp_tmp = ioremap_nocache(0x01051000 + 0x88, 0x4);
	value = readl_relaxed(tmp_tmp);
	PRT_ERR(" tmp0x01051000_0x88 : %x\n", value);

	tmp_tmp = ioremap_nocache(0x01051000 + 0x100, 0x4);
	value = readl_relaxed(tmp_tmp);
	PRT_ERR(" tmp0x01051000_0x100 : %x\n", value);
}
*/
		vdc_occupied = 1;
		vdc_idle_flag = 0;

		s = 0;
		s |= 1;

		enable_vdc_irq();
		vdc_write(GVDC, s, 0x4);
		mutex_unlock(&m_mutex);

		return id;

	} else {
		PRT_ERR("Fail to run, arg: %x\n", (unsigned int)arg);
		return VDC_STATUS_UNKNOWN_ERROR;
	}

	break;

	case VDC_QUERY:
		slot_id = (int)arg;

		if (slot_id != vdc_cur_slot_id) {
			PRT_ERR("query error, slot_id: %d - cur_slot : %d\n",
				slot_id, vdc_cur_slot_id);
		}

		if (vdc_irq_registered) {
			vdc_status_t s;
			int rt;

			mutex_lock(&m_mutex);

			/*check interrupted*/
			left_time = wait_for_completion_timeout(
					&(slot[slot_id].isr_complete),
						msecs_to_jiffies(5*1000) + 1);

			if (unlikely(left_time == 0)) {
				PRT_ERR("QUERY: wait timeout : %d\n", slot_id);
				PRT_ERR("cur_slot_id %d\n", vdc_cur_slot_id);
				vdc_reset();
				s = VDC_STATUS_DEAD;
			} else {
				/* normal case */
		while (1) {

			if ((vdc_last_status == -1) && (decode_error_cnt > 0)) {
				vdc_reset_and_sleep(DECODE_ERROR_CNT -
					decode_error_cnt);
				slot_id = vdc_cur_slot_id;
				init_completion(&slot[slot_id].isr_complete);
				slot[slot_id].state = SLOT_OCCUPIED;

				vdc_drv_flushreg_again(vdc_cur_slot_id);
				vdc_occupied = 1;
				vdc_idle_flag = 0;
				s = 0;
				s |= 1;
				PRT_ERR(" -- run again\n");
				vdc_write(GVDC, s, 0x4);

				left_time = wait_for_completion_timeout(
					&(slot[slot_id].isr_complete),
						msecs_to_jiffies(5*1000) + 1);

				if (unlikely(left_time == 0)) {
					PRT_ERR("QUERY: wait timeout1 : %d\n",
						slot_id);
					PRT_ERR("cur_slot_id1 %d\n",
						vdc_cur_slot_id);
					vdc_reset();
					s = VDC_STATUS_DEAD;
					goto query_end;
				}
			} else
				break;
		}

				rt = copy_to_user(
				(void __user *)slot[slot_id].clientregptr,
					&(slot[slot_id].user_data.regs[0]),
						sizeof(vdc_user_data_t));
				if (rt != 0)
					PRT_ERR("copy failed, rt=%d\n", rt);

				s = vdc_query_status(slot[slot_id].vdc_status);
			}
query_end:
			/* free slot */
			if ((slot[vdc_cur_slot_id].slice_mode == 0)
					||  (s == VDC_STATUS_DEAD)) {
				slot_reset(slot_id);
			}

			mutex_unlock(&m_mutex);

			return s;
		} else {
			vdc_status_t s1;
			int rt;
			unsigned int s = vdc_read(GVDC, 0x4);
			int slot_id = vdc_cur_slot_id;

			while (s != 0x1100) {
				usleep_range(5000, 10000);
				PRT_INFO("wait vdc dec end -- s : %x\n", s);
				s = vdc_read(GVDC, 0x4);
			}
			disable_vdc_irq();
			slot_complete(vdc_cur_slot_id, s);
			vdc_drv_updatereg(vdc_cur_slot_id);

			if (((s & (0x1<<17)) || (s & (0x1<<14)))
					&& !(s & (0x1<<12))) {
				slot[vdc_cur_slot_id].slice_mode = 1;
				vdc_slice_slot_id = vdc_cur_slot_id;
				slot[vdc_slice_slot_id].state = SLOT_OCCUPIED;
			} else {
				slot[vdc_cur_slot_id].slice_mode = 0;
				vdc_occupied = 0;
			}

			slot[vdc_cur_slot_id].vdc_status = s;
			vdc_idle_flag = 1;
			complete(&(slot[vdc_cur_slot_id].isr_complete));

			rt = copy_to_user(
				(void __user *)slot[slot_id].clientregptr,
					&(slot[slot_id].user_data.regs[0]),
						sizeof(vdc_user_data_t));
				if (rt != 0)
					PRT_ERR("copy failed 2, rt=%d\n", rt);

				s1 = vdc_query_status(slot[slot_id].vdc_status);
				PRT_INFO("vdc : s1 : %x\n", s1);
				if ((slot[vdc_cur_slot_id].slice_mode == 0)
					||  (s == VDC_STATUS_DEAD)) {
					slot_reset(slot_id);
				}
				return s1;
		}
		break;
	default:
		PRT_ERR("vdc : no such cmd 0x%x\n", cmd);
		return -EIO;
	}
	/* vdc_reset_in_playing(); */
	return 0;
}

static int vdc_open(struct inode *inode, struct file *file)
{
	mutex_lock(&m_mutex);
	vdc_open_count++;
	if (vdc_open_count > 1) {
		PRT_ERR("vdc drv already open\n");
		mutex_unlock(&m_mutex);
		return 0;
	}

	clk_prepare_enable(vdc_ahb_gate);
	clk_prepare_enable(vdc_ahb_reset);
	clk_prepare_enable(vdc_axi_gate);
	vdc_clk_enable();
	disable_vdc_irq();

	/* enable_irq(GVDC->irq); */
	vdc_idle_flag = 1;
	vdc_is_suspend = 0;
	mutex_unlock(&m_mutex);

	return 0;
}

static int vdc_release(struct inode *inode, struct file *file)
{
	int i;

	mutex_lock(&m_mutex);
	vdc_open_count--;
	if (vdc_open_count > 0) {
		PRT_ERR("vdc : vdc_release: count:%d\n", vdc_open_count);
		PRT_ERR("vdc : vdc_release: pid(%d)\n", task_tgid_vnr(current));
		vdc_waitforidle();
		goto VDC_REL;
	} else if (vdc_open_count < 0) {
		PRT_ERR("vdc : vdc_release: module is closed before opened\n");
		vdc_open_count = 0;
	}

	if (vdc_open_count == 0) {
		PRT_DBG("vdc : vdc_release: disable vdc irq\n");
		disable_vdc_irq();
		PRT_DBG("vdc : vdc_release: disable IRQ_VDC\n");
		/* disable_irq(GVDC->irq); */
		PRT_DBG("vdc : vdc_release: disable vdc irq ok!\n");
	}

	vdc_clk_disable();

	clk_disable_unprepare(vdc_axi_gate);
	clk_disable_unprepare(vdc_ahb_gate);
	clk_disable_unprepare(vdc_ahb_reset);
	vdc_is_suspend = -1;

VDC_REL:
	for (i = 0; i < MAX_INSTANCE_NUM; i++) {
		if (slot[i].pid == task_tgid_vnr(current)) {
			PRT_ERR("vdc : vdc slot is leak by pid, reset it\n");
			PRT_ERR("pid: %d\n", task_tgid_vnr(current));
			if (slot[i].slice_mode == 1 && vdc_occupied == 1)
				vdc_occupied = 0;
			slot_reset(i);
		}
	}
	mutex_unlock(&m_mutex);
	return 0;
}

/*
 * guarantee that frame has been decoded before low power
 */
#ifdef CONFIG_PM
static int vdc_suspend(struct device *_dev)
{
	if (vdc_is_suspend == 0) {
		/* store the clk parent */

		if (vdc_clk_isenable == 1)
			vdc_waitforidle();

		g_clk_null = clk_get(NULL, "null_clk");

		/* set the module's clk to null_clk */
		clk_set_parent(vdc_clk, g_clk_null);

		if (vdc_clk_isenable == 1) {
			clk_disable_unprepare(vdc_clk);
			vdc_clk_isenable = 0;
		}

		clk_disable_unprepare(vdc_axi_gate);
		clk_disable_unprepare(vdc_ahb_gate);
		clk_disable_unprepare(vdc_ahb_reset);

		vdc_is_suspend = 1;
	} else
		PRT_INFO("vdc not need call vdc_suspend:%d!\n", vdc_is_suspend);

	/* Do other things about your module's suspend */
	return 0;
}

static int vdc_resume(struct device *_dev)
{
	if (vdc_is_suspend == 1) {
		clk_prepare_enable(vdc_ahb_gate);
		clk_prepare_enable(vdc_ahb_reset);
		clk_prepare_enable(vdc_axi_gate);

		vdc_clk_enable();
		clk_put(g_clk_null);
		vdc_is_suspend = 0;
	} else
		PRT_INFO("vdc not need call vdc_resume: %d!\n", vdc_is_suspend);

	return 0;
}

static const struct dev_pm_ops vdc_pmops = {
	.suspend = vdc_suspend,
	.resume = vdc_resume,
};

#endif

static const struct file_operations vdc_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = vdc_ioctl,
	.compat_ioctl   = vdc_ioctl,/* compat_vdc_ioctl */
	.open = vdc_open,
	.release = vdc_release,
};

/*#define ENABLE_COOLING*/



static int asoc_vdc_probe(struct platform_device *pdev)
{
	struct asoc_vdc_dev *vdc;
	struct resource *iores;
	int ret = 0;

	PRT_INFO("vdc prob\n");
	dev_info(&pdev->dev, " ---  Probe vdc device\n");

	vdc = devm_kzalloc(&pdev->dev, sizeof(*vdc), GFP_KERNEL);
	if (!vdc) {
		ret = -ENOMEM;
		goto devm_kzalloc_err;
	}

	iores = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!iores) {
		ret = -EINVAL;
		goto platform_get_resource_err;
	}

	vdc->base = devm_ioremap_resource(&pdev->dev, iores);
	if (IS_ERR(vdc->base)) {
		ret = PTR_ERR(vdc->base);
		goto devm_ioremap_resource_err;
	}

	vdc->irq = platform_get_irq(pdev, 0);
	if (vdc->irq < 0) {
		ret = vdc->irq;
		goto get_irq_err;
	}

	dev_info(&pdev->dev, "resource: iomem: %pR mapping to %p, irq %d\n",
		iores, vdc->base, vdc->irq);

	ret = request_irq(vdc->irq, (void *)VDC_ISR, 0, "vdc_isr", 0);
	if (ret) {
		PRT_ERR("vdc : register vdc irq failed!\n");
		vdc_irq_registered = 0;
		ret = -1;
		goto request_irq_err;
	} else {
		vdc_irq_registered = 1;
	}

/*
{
	unsigned int value;
	ahb_bus_clock_gating_reg = ioremap_nocache(0x0400a000 + 0x204, 0x4);
	value = readl_relaxed(ahb_bus_clock_gating_reg);
	PRT_ERR(" ahb_bus_clock_gating_reg : %x\n", value);
	value = value | (1<<11);
	writel_relaxed(value, ahb_bus_clock_gating_reg);
	value = readl_relaxed(ahb_bus_clock_gating_reg);
	PRT_ERR(" ahb_bus_clock_gating_reg2 : %x\n", value);


	ahb_bus_reset_reg = ioremap_nocache(0x0400a000 + 0x224, 0x4);
	value = readl_relaxed(ahb_bus_reset_reg);
	PRT_ERR(" ahb_bus_reset_reg : %x\n", value);
	value = value & 0xfffff7ff;
	writel_relaxed(value, ahb_bus_reset_reg);
	usleep_range(1000, 1000);
	value = readl_relaxed(ahb_bus_reset_reg);
	PRT_ERR(" ahb_bus_reset_reg2 : %x\n", value);
	value = value | (1<<11);
	writel_relaxed(value, ahb_bus_reset_reg);
	usleep_range(1000, 1000);
	value = readl_relaxed(ahb_bus_reset_reg);
	PRT_ERR(" ahb_bus_reset_reg3 : %x\n", value);

	axi_buf_clock_gating_reg = ioremap_nocache(0x0400a000 + 0x340, 0x4);
	value = readl_relaxed(axi_buf_clock_gating_reg);
	PRT_ERR(" axi_buf_clock_gating_reg1 : %x\n", value);
	value = value | (1<<2);
	writel_relaxed(value, axi_buf_clock_gating_reg);
	value = readl_relaxed(axi_buf_clock_gating_reg);
	PRT_ERR(" axi_buf_clock_gating_reg2 : %x\n", value);


	vdc_pll_fac = ioremap_nocache(0x0400a000 + 0x5a4, 0x4);
	value = readl_relaxed(vdc_pll_fac);
	PRT_ERR(" vdc_pll_fac1 : %x\n", value);

	vdc_pll_tune0 = ioremap_nocache(0x0400a000 + 0x5a8, 0x4);
	value = readl_relaxed(vdc_pll_tune0);
	PRT_ERR(" vdc_pll_tune0 : %x\n", value);

	vdc_pll_reg = ioremap_nocache(0x0400a000 + 0x09c4, 0x4);
	value = readl_relaxed(vdc_pll_reg);
	PRT_ERR(" vdc_pll_reg1 : %x\n", value);
	value = value | (1<<31) | 1;
	writel_relaxed(value, vdc_pll_reg);
	value = readl_relaxed(vdc_pll_reg);
	PRT_ERR(" vdc_pll_reg2 : %x\n", value);
}
*/

	vdc_ahb_gate = devm_clk_get(&pdev->dev, VDC_AHB_GATE);
	if (IS_ERR(vdc_ahb_gate)) {
		PRT_ERR("get vdc ahb gate err\n");
		ret = PTR_ERR(vdc_ahb_gate);
		goto devm_clk_get_vdc_ahb_gate_err;
	}

	vdc_ahb_reset = devm_clk_get(&pdev->dev, VDC_AHB_RESET);
	if (IS_ERR(vdc_ahb_reset)) {
		PRT_ERR("get vdc_ahb_reset err\n");
		ret = PTR_ERR(vdc_ahb_reset);
		goto devm_clk_get_vdc_ahb_reset_err;
	}

	vdc_axi_gate = devm_clk_get(&pdev->dev, VDC_AXI_GATE);
	if (IS_ERR(vdc_axi_gate)) {
		PRT_ERR("get vdc_axi_gate err\n");
		ret = PTR_ERR(vdc_axi_gate);
		goto devm_clk_get_vdc_axi_gate_err;
	}

	/* get the vc clk, set parent clk & clk rate */
	vdc_clk = devm_clk_get(&pdev->dev, VDC_CLK);
	if (IS_ERR(vdc_clk)) {
		PRT_ERR("failed to get vdc_clk!\n");
		ret = PTR_ERR(vdc_clk);
		goto devm_clk_get_vdc_clk_err;
	}

	/*get display_pll struct*/
	parent_pll = clk_get(NULL, "vc_pll");
	if (IS_ERR(parent_pll)) {
		PRT_ERR("vdc : clk_get parent_pll failed\n");
		ret = PTR_ERR(parent_pll);
		goto clk_get_parent_pll_err;
	}

	/* disable_irq(vdc->irq); */
	GVDC = vdc;
	gvdcdev = pdev;
	PRT_INFO("dev add %p, vdc_clk %p\n", &pdev->dev, vdc_clk);
	vdc_clk_isenable = 0;
	vdc_is_suspend = -1;
	return 0;

clk_get_parent_pll_err:
	devm_clk_put(&pdev->dev, vdc_clk);
devm_clk_get_vdc_clk_err:
	devm_clk_put(&pdev->dev, vdc_axi_gate);
devm_clk_get_vdc_axi_gate_err:
	devm_clk_put(&pdev->dev, vdc_ahb_reset);
devm_clk_get_vdc_ahb_reset_err:
	devm_clk_put(&pdev->dev, vdc_ahb_gate);
devm_clk_get_vdc_ahb_gate_err:
	free_irq(vdc->irq, 0);
request_irq_err:
get_irq_err:
platform_get_resource_err:
devm_ioremap_resource_err:
	devm_kfree(&pdev->dev, vdc);
devm_kzalloc_err:
	return ret;
}

static int vdc_remove(struct platform_device *pdev)
{
	pm_runtime_disable(&pdev->dev);
	vdc_clk_disable();
	clk_put(vdc_clk);
	clk_put(parent_pll);
	return 0;
}



static struct platform_driver vdc_platform_driver = {
	.driver = {
		.name = DEVDRV_NAME_VDC,
		.owner = THIS_MODULE,
		.of_match_table = vdc_of_match,
		.pm = &vdc_pmops,
	},
	.probe = asoc_vdc_probe,
	.remove = vdc_remove,
};

static struct miscdevice vdc_miscdevice = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVDRV_NAME_VDC,
	.fops = &vdc_fops,
};

static int vdc_init(void)
{
	void __iomem *vdc_support = NULL;
	unsigned int value = 0;
	int ret;

	PRT_INFO("#### insmod vdc driver!\n");

	vdc_support = ioremap_nocache(0x01203818, 0x4);
	value = readl_relaxed(vdc_support);
	PRT_INFO(" vdc_support : %x\n", value);

	if (value & 0x10000000)
		return -1;


	/* start insmod£¬register device.*/
	ret = misc_register(&vdc_miscdevice);
	if (ret) {
		PRT_ERR("register vdc misc device failed!\n");
		goto err0;
	}

	ret = platform_driver_register(&vdc_platform_driver);
	if (ret) {
		PRT_ERR("register gpu platform driver4pm error!\n");
		goto err1;
	}

	mutex_init(&m_mutex);

	return 0;

err1:

	free_irq(GVDC->irq, 0);

	misc_deregister(&vdc_miscdevice);

err0:
	return ret;
}

static void vdc_exit(void)
{
	if (vdc_irq_registered)
		free_irq(GVDC->irq, 0);

	misc_deregister(&vdc_miscdevice);
	platform_driver_unregister(&vdc_platform_driver);

	PRT_INFO("vdc module unloaded\n");
}


module_init(vdc_init);
module_exit(vdc_exit);

MODULE_AUTHOR("Lombo Semi Inc");
MODULE_DESCRIPTION("VDC kernel module");
MODULE_LICENSE("GPL");
