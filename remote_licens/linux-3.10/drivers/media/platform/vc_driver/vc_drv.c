#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/errno.h> /* error codes */
#include <linux/vmalloc.h>
#include <linux/slab.h> /* kmalloc/kfree */
#include <linux/init.h> /* module_init/module_exit */
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
#include "vc_core.h"
#include "vc_drv.h"

#undef  MOD_NAME
#define MOD_NAME			"N7_VC"



#ifdef CONFIG_ARCH_LOMBO_N9V0
	#ifdef CONFIG_ARCH_LOMBO_N9V0_FPGA
		#define PRCM_CLK
	#else
		#define VC_N9V0_CLOCK_FREQUENCE
	#endif

	#define DISABLE_ALLCLOCKON
#endif


#define VC_DRV_MINOR			80
#define VC_QUERY_INTERVAL		5
#define MAX_INSTANCE_NUM		8
#define VC_TOP_CLOCK_CTRL		0xA0
#define VC_TOP_ADPLL_FAC		0xA4
#define VC_TOP_ADPLL_TUNE0		0xA8
#define VC_TOP_ADPLL_TUNE1		0xAC
#define VC_TOP_ADPLL_STAT		0xB0
#define VC_PREPROCESS_SRAM_PORT		55
#define VC_CABAC_SRAM_PORT		61
#define MAX_VC_REG_RETRY_TIME		5

#define DEVDRV_NAME_VC			"vc"
#define CLK_NAME_DEVCLK			"dev_clk"
#define CLK_NAME_DISPLAYPLL		"display_pll"
#define VC_CLK				"vc_module_clk"
#define VC_AHB_GATE			"vc_ahb_gate"
#define VC_AHB_RESET			"vc_ahb_reset"
#define VC_AXI_GATE			"vc_axi_gate"

#ifdef PRCM_CLK
#define PRCM_BASE_ADDR			0x0400a000
#define VC_PLL_STATUS_REG		0x000005b0
#define VC_PARENT_CLK_RATE		600000000
#define VC_CLK_RATE			300000000
#endif
#define VC_TOP_BASE			0x00000000
#define VC_PRE_BASE			0x00000100
#define VC_ENC_BASE			0x00000200
#define VC_DEC_BASE			0x00000400
#define VC_JPEG_BASE			0x00000500


static int vc_cur_slot_id = -1;
static int vc_slice_slot_id = -1; /* jpeg slice pattern */
static int vc_last_status = -1; /* 0:normal, -1:error; */
static int vc_irq_registered;
static int vc_open_count; /* vc handle num */

/* vc shoule be occupied when jpeg slice pattern that
	may needs some times when decoded */
static int vc_occupied;

static struct mutex m_mutex; /* for multi-instance process */
static DECLARE_WAIT_QUEUE_HEAD(waitqueue);
static int vc_idle_flag = 1; /* vc can accept a new RUN order if finish */
static int vc_clk_isenable;

typedef struct {
	unsigned int top_reg[MAX_VC_TOP_REG_NUM];
	unsigned int pre_reg[MAX_VC_PRE_REG_NUM];
	unsigned int enc_reg[MAX_VC_ENC_REG_NUM];
	unsigned int dec_reg[MAX_VC_DEC_REG_NUM];
	unsigned int jpeg_reg[MAX_VC_JPEG_REG_NUM];
	unsigned int scale_reg[MAX_SCALE_PARM];
	unsigned int cabac_table[MAX_VC_CABAC_TABLE];
} vc_user_data_t;

struct asoc_vc_dev {
	struct device *dev;
	void __iomem *base;
	struct clk *clk;
	int irq;
	unsigned int setting; /* not used temporarily */
	struct regulator *power;
};

static struct asoc_vc_dev *G_VC;
static struct clk *vc_clk;
static struct clk *g_clk_null;
static struct clk *parent_pll;
static struct clk *vc_ahb_gate;
static struct clk *vc_ahb_reset;
static struct clk *vc_axi_gate;
static struct platform_device *gvcdev;

static int vc_enc_stream_full_flag = -1;
static int vc_dec_stream_empty_flag = -1;
static int vc_is_suspend = -1;

static void vc_write(struct asoc_vc_dev *vc, u32 val, u32 offset, u32 reg)
{
	void *write_add = (void *)(vc->base + offset + reg);
	writel_relaxed(val, write_add);
}
static u32 vc_read(struct asoc_vc_dev *vc, u32 offset, u32 reg)
{
	unsigned int data;

	data = readl_relaxed((void *)(vc->base + offset + reg));
	return data;
}

struct ic_info {
	int ic_type;
};

/*
	static struct ic_info s3001_data = {
	.ic_type = IC_TYPE_3001,
	};
*/

static const struct of_device_id vc_of_match[] = {
	{.compatible = "lombo,n7-vc",},
	{.compatible = "lombo,n9-vc",},
	{}
};

MODULE_DEVICE_TABLE(of, vc_of_match);

typedef enum {
	SLOT_IDLE,
	SLOT_OCCUPIED,
	SLOT_COMPLETED
} slot_state_e;

typedef struct slot_s {
	struct completion isr_complete;
	slot_state_e state;
	unsigned long topptr;
	unsigned long encptr;
	unsigned long decptr;
	unsigned long jpegptr;
	unsigned int vc_status;
	unsigned int slice_mode;
	unsigned int vc_type; /* 1-h264_dec 2-jpeg_dec 3-h264_enc 4-jpeg_enc */
	int pid;
	vc_user_data_t user_data;
} slot_t;

static slot_t slot[MAX_INSTANCE_NUM];

static int slot_reset(int i)
{
	init_completion(&slot[i].isr_complete);
	slot[i].state = SLOT_IDLE;
	slot[i].slice_mode = 0;
	slot[i].pid = -1;
	slot[i].topptr = 0;
	slot[i].encptr = 0;
	slot[i].decptr = 0;
	slot[i].jpegptr = 0;
	slot[i].vc_type = 0;
	slot[i].vc_status = 0;
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
		PRT_ERR("vc : no idle slot\n");
		return -1;
	}
	return -1;
}

static int slot_complete(int i, unsigned int vc_status)
{
	if (slot[i].state != SLOT_OCCUPIED) {
		PRT_ERR("vc : slot is idle, staus error\n");
		return -1;
	}
	slot[i].vc_status = vc_status;
	slot[i].state = SLOT_COMPLETED;
	return 0;
}

static void vc_waitforidle(void)
{
	int ctr = 0, v_en = 0, v_de = 0;

	v_en = vc_read(G_VC, VC_TOP_BASE, 0xC);
	v_de = vc_read(G_VC, VC_TOP_BASE, 0x10);

	if (((v_en & 0x1) == 0x1) || (((v_de & 0x1) == 0x1))) {
		PRT_INFO("vc : vc is on air\n");
		do {
			usleep_range(VC_QUERY_INTERVAL, 2*VC_QUERY_INTERVAL);
			ctr++;
			if (ctr == 100) {
				PRT_ERR("vc : vc always busy\n");
				break;
			}
		} while ((((vc_read(G_VC, VC_TOP_BASE, 0xC)) & 0x1) == 0x1)
		|| ((((vc_read(G_VC, VC_TOP_BASE, 0x10)) & 0x1) == 0x1)));
	}
}


/*============== funcs:clock, power, reset, irq =============*/
static void vc_do_reset(void)
{
	return;
}

static void vc_reset(void)
{
	usleep_range(5*1000, 6*1000);
	vc_do_reset();
}

#ifdef PRCM_CLK
static void vc_clk_enable(void)
{
	int ret = 0;

	if (vc_clk_isenable != 0)
		return;
/*
	res = pm_runtime_get_sync(&gvcdev->dev);
	if (res < 0) {
		PRT_ERR("vc_clk_enable: pm_runtime_get_sync failed\n");
		return;
	}
*/
	/* enable clk */
	ret = clk_set_rate(parent_pll, VC_PARENT_CLK_RATE);


	if (clk_set_parent(vc_clk, parent_pll))
		PRT_ERR("failed to set vc parent to display_pll\n");

	ret = clk_set_rate(vc_clk, VC_CLK_RATE);
	if (ret)
		PRT_ERR("set vc_clk rate err\n");
	clk_prepare_enable(vc_clk);

	vc_clk_isenable = 1;
}

static void vc_clk_disable(void)
{
	if (vc_clk_isenable == 0)
		return;
	vc_waitforidle();
	clk_disable_unprepare(vc_clk);

	vc_clk_isenable = 0;
}
#else
static void vc_clk_enable(void)
{
	unsigned int regval;
	int retry_cnt = 10, times_retry = 0;

	if (vc_clk_isenable != 0)
		return;

	regval = 0x5 << 28 | 0x7 << 24 | 0x3 << 20 | 0x7 << 16 |
			0x4 << 13 | 0x1 << 12 | 0x1 << 8 | 0x2 << 4;
	vc_write(G_VC, regval, VC_TOP_BASE, VC_TOP_ADPLL_TUNE1);

#ifdef VC_N9V0_CLOCK_FREQUENCE
	regval = 0x19;
	vc_write(G_VC, regval, VC_TOP_BASE, VC_TOP_ADPLL_FAC);
	usleep_range(5*1000, 6*1000);

	regval = 0x1ul << 31 | 0x19;
	vc_write(G_VC, regval, VC_TOP_BASE, VC_TOP_ADPLL_FAC);
#else
	regval = 0x1 << 8 | 0x19;
	vc_write(G_VC, regval, VC_TOP_BASE, VC_TOP_ADPLL_FAC);
	usleep_range(5*1000, 6*1000);

	regval = 0x1ul << 31 | 0x1 << 8 | 0x19;
	vc_write(G_VC, regval, VC_TOP_BASE, VC_TOP_ADPLL_FAC);
#endif

query_lock_status:
	usleep_range(5*1000, 6*1000);
	while (1) {
		regval = vc_read(G_VC, VC_TOP_BASE, VC_TOP_ADPLL_STAT);
		if ((regval & 0x80008000) == 0x80008000)
			break;
		else {
			usleep_range(5*1000, 6*1000);
			retry_cnt--;
			if (retry_cnt <= 0 && times_retry == 0) {
				regval = 0;
				vc_write(G_VC, regval, VC_TOP_BASE,
					VC_TOP_ADPLL_FAC);
				regval = 0x1 << 8 | 0x11;
				vc_write(G_VC, regval, VC_TOP_BASE,
					VC_TOP_ADPLL_FAC);
				usleep_range(5*1000, 6*1000);
				regval = 0x1ul << 31 | 0x1 << 8 | 0x11;
				vc_write(G_VC, regval, VC_TOP_BASE,
					VC_TOP_ADPLL_FAC);
				usleep_range(5*1000, 6*1000);
				regval = 0x1 << 8 | 0x19;
				vc_write(G_VC, regval, VC_TOP_BASE,
					VC_TOP_ADPLL_FAC);
				usleep_range(5*1000, 6*1000);
				regval = 0x1ul << 31 | 0x1 << 8 | 0x19;
				vc_write(G_VC, regval, VC_TOP_BASE,
					VC_TOP_ADPLL_FAC);
				usleep_range(5*1000, 6*1000);
				retry_cnt = 10;
				times_retry = 1;
				PRT_WARN("read vc pll reg try again!");
				goto query_lock_status;
			} else if (retry_cnt <= 0 && times_retry == 1) {
				retry_cnt = 10;
				PRT_ERR("read vc pll reg stat fail\n");
			}
		}
	}

	regval = 0x1ul << 31 | 0x1;
	vc_write(G_VC, regval, VC_TOP_BASE, VC_TOP_CLOCK_CTRL);

	vc_clk_isenable = 1;
}

static void vc_clk_disable(void)
{
	if (vc_clk_isenable == 0)
		return;
	vc_waitforidle();

	vc_write(G_VC, 0x0, VC_TOP_BASE, VC_TOP_ADPLL_FAC);

	vc_clk_isenable = 0;
}
#endif
/* enable int */
static inline void enable_vc_irq(void)
{
	unsigned int v, c;

	c = MAX_VC_REG_RETRY_TIME;
	v = vc_read(G_VC, VC_TOP_BASE, 0x4);
	v |= (1<<8);
	vc_write(G_VC, v, VC_TOP_BASE, 0x4);

	while (((vc_read(G_VC, VC_TOP_BASE, 0x4)) & (0x1<<8)) == 0) {
		PRT_ERR("vc : can not enable irq\n");
		vc_write(G_VC, v, VC_TOP_BASE, 0x4);
		c--;
		if (c <= 0)
			break;
	}
}

static inline void disable_vc_irq(void)
{
	unsigned int v, c;

	c = MAX_VC_REG_RETRY_TIME;
	v = vc_read(G_VC, VC_TOP_BASE, 0x4);
	v &= ~(1<<8);
	vc_write(G_VC, v, VC_TOP_BASE, 0x4);

	while ((vc_read(G_VC, VC_TOP_BASE, 0x4)) & (0x1<<8) && c-- > 0) {
		PRT_ERR("vc : can not disable irq\n");
		vc_write(G_VC, v, VC_TOP_BASE, 0x4);
	}
}

static void enable_vc_all_clock_on(void)
{
#ifndef DISABLE_ALLCLOCKON
	unsigned int v;

	v = vc_read(G_VC, VC_TOP_BASE, 0x4);
	v |= (1<<30);
	vc_write(G_VC, v, VC_TOP_BASE, 0x4);
#endif
}

static void disable_vc_all_clock_on(void)
{
#ifndef DISABLE_ALLCLOCKON
	unsigned int v;

	v = vc_read(G_VC, VC_TOP_BASE, 0x4);
	v &= ~(1<<30);
	vc_write(G_VC, v, VC_TOP_BASE, 0x4);
#endif
}

static void vc_drv_updatereg(int id)
{
	int i;

	if (slot[id].vc_type == ENC_H264) {
		for (i = 0; i < MAX_VC_ENC_REG_NUM; i++) {
			slot[id].user_data.enc_reg[i] =
				(unsigned int)(vc_read(G_VC, VC_ENC_BASE, i*4));
			/* PRT_ERR("enc end : %d -- %x\n"
				,i,slot[id].user_data.enc_reg[i]); */
		}
	}
	if (slot[id].vc_type == DEC_H264) {
		for (i = 0; i < MAX_VC_DEC_REG_NUM; i++)
			slot[id].user_data.dec_reg[i] =
				(unsigned int)(vc_read(G_VC, VC_DEC_BASE, i*4));
	}
	if ((slot[id].vc_type == DEC_JPEG) || (slot[id].vc_type == ENC_JPEG)
	|| (slot[id].vc_type == DEC_MJPEG) || (slot[id].vc_type == ENC_MJPEG)) {
		for (i = 0; i < MAX_VC_JPEG_REG_NUM; i++)
			slot[id].user_data.jpeg_reg[i] =
			(unsigned int)(vc_read(G_VC, VC_JPEG_BASE, i*4));
	}
}

/*
	This function is VC ISR.
*/
irqreturn_t VC_ISR(int irq, void *dev_id)
{
	unsigned int s;

	vc_enc_stream_full_flag = 0;
	vc_dec_stream_empty_flag = 0;

	if ((slot[vc_cur_slot_id].vc_type == DEC_H264)
		|| (slot[vc_cur_slot_id].vc_type == DEC_JPEG) ||
			(slot[vc_cur_slot_id].vc_type == DEC_MJPEG)) {
		s = vc_read(G_VC, VC_TOP_BASE, 0x10);
		if (s == 0x201)
			vc_dec_stream_empty_flag = 1;
		else
			vc_write(G_VC, s, VC_TOP_BASE, 0x10);
		disable_vc_irq();

		if (slot[vc_cur_slot_id].vc_type == DEC_H264)
			disable_vc_all_clock_on();

		slot_complete(vc_cur_slot_id, s);
	} else {
		s = vc_read(G_VC, VC_TOP_BASE, 0x0C);
		if (s == 0x101)
			vc_enc_stream_full_flag = 1;
		else
			vc_write(G_VC, s, VC_TOP_BASE, 0x0C);
		disable_vc_irq();

		if (slot[vc_cur_slot_id].vc_type == ENC_H264)
			disable_vc_all_clock_on();

		slot_complete(vc_cur_slot_id, s);
	}

	vc_drv_updatereg(vc_cur_slot_id);
	/* when bistream empty or jpeg slice ready, and not frame ready. */
	if ((slot[vc_cur_slot_id].vc_type == DEC_H264)
		|| (slot[vc_cur_slot_id].vc_type == DEC_JPEG) ||
			(slot[vc_cur_slot_id].vc_type == DEC_MJPEG)) {
		if (((s & (0x1<<9)) || (s & (0x1<<4)))
			&& !(s & (0x1<<8)) && !(s & (0x1<<5))) {
			slot[vc_cur_slot_id].slice_mode = 1;
			vc_slice_slot_id = vc_cur_slot_id;
			slot[vc_slice_slot_id].state = SLOT_OCCUPIED;
		} else {
			slot[vc_cur_slot_id].slice_mode = 0;
			vc_occupied = 0;
		}
	} else {
		/* PRT_ERR("s : %x\n", s); */
		if ((s & (0x1<<8))) {
			slot[vc_cur_slot_id].slice_mode = 1;
			vc_slice_slot_id = vc_cur_slot_id;
			slot[vc_slice_slot_id].state = SLOT_OCCUPIED;
		} else {
			slot[vc_cur_slot_id].slice_mode = 0;
			vc_occupied = 0;
		}
	}

	vc_idle_flag = 1;

	if ((slot[vc_cur_slot_id].vc_type == DEC_H264)
		|| (slot[vc_cur_slot_id].vc_type == DEC_JPEG)
		|| (slot[vc_cur_slot_id].vc_type == DEC_MJPEG)) {
		if (slot[vc_cur_slot_id].vc_status & 0x8FFFF100)
			vc_last_status = -1;
	}
	complete(&(slot[vc_cur_slot_id].isr_complete));
	wake_up_interruptible(&waitqueue); /* wake up */
	/* PRT_ERR("VC_ISR End\n"); */
	return IRQ_HANDLED;
}

static void vc_drv_writereg(unsigned int regno, int offset, unsigned int value)
{
	/* unsigned int value_recover; */
	vc_write(G_VC, value, offset, regno*4);
}

static void flush_dec_h264(int id)
{
	int ii = 0;

	for (ii = 0; ii < MAX_VC_TOP_REG_NUM; ii++) {
		if (ii == 4)
			;
		else
		vc_drv_writereg(ii, VC_TOP_BASE,
			slot[id].user_data.top_reg[ii]);
	}
	for (ii = 0; ii < MAX_VC_DEC_REG_NUM; ii++)
		vc_drv_writereg(ii, VC_DEC_BASE,
			slot[id].user_data.dec_reg[ii]);
	if (vc_dec_stream_empty_flag != 1) {
		for (ii = 0; ii < MAX_VC_H264_DEC_CABAC_TABLE; ii++)
			vc_drv_writereg(VC_CABAC_SRAM_PORT, VC_DEC_BASE,
				slot[id].user_data.cabac_table[ii]);
	}
}

static void flush_dec_jpeg(int id)
{
	int ii = 0;

	for (ii = 0; ii < MAX_VC_TOP_REG_NUM; ii++) {
		if (ii == 4)
			;
		else
		vc_drv_writereg(ii, VC_TOP_BASE,
			slot[id].user_data.top_reg[ii]);
	}
	for (ii = 0; ii < MAX_VC_JPEG_REG_NUM; ii++)
		vc_drv_writereg(ii, VC_JPEG_BASE,
			slot[id].user_data.jpeg_reg[ii]);
	if (vc_dec_stream_empty_flag != 1) {
		for (ii = 0; ii < MAX_VC_JPEG_DEC_CABAC_TABLE; ii++)
			vc_drv_writereg(VC_CABAC_SRAM_PORT, VC_JPEG_BASE,
				slot[id].user_data.cabac_table[ii]);
	}
}

static void flush_enc_h264(int id)
{
	int ii = 0;

	for (ii = 0; ii < MAX_VC_TOP_REG_NUM; ii++) {
		if (ii == 3)
			;
		else
			vc_drv_writereg(ii, VC_TOP_BASE,
				slot[id].user_data.top_reg[ii]);
		/* PRT_ERR("TOP REG : %d --- : %x\n"
			,ii, slot[id].user_data.top_reg[ii]); */
	}
	for (ii = 0; ii < MAX_VC_PRE_REG_NUM; ii++) {
		vc_drv_writereg(ii, VC_PRE_BASE,
			slot[id].user_data.pre_reg[ii]);
		/* PRT_ERR("PRE REG : %d --- : %x\n"
			,ii, slot[id].user_data.pre_reg[ii]); */
	}
	if (vc_enc_stream_full_flag != 1) {
		for (ii = 0; ii < MAX_SCALE_PARM; ii++) {
			vc_drv_writereg(VC_PREPROCESS_SRAM_PORT, VC_PRE_BASE,
				slot[id].user_data.scale_reg[ii]);
		}
	}
	for (ii = 0; ii < MAX_VC_ENC_REG_NUM; ii++) {
		vc_drv_writereg(ii, VC_ENC_BASE,
			slot[id].user_data.enc_reg[ii]);
		/* PRT_ERR("ENC REG : %d ---: %x\n"
			,ii, slot[id].user_data.enc_reg[ii]); */
	}
	if (vc_enc_stream_full_flag != 1) {
		for (ii = 0; ii < MAX_VC_H264_ENC_CABAC_TABLE; ii++) {
			vc_drv_writereg(VC_CABAC_SRAM_PORT, VC_ENC_BASE,
				slot[id].user_data.cabac_table[ii]);
		}
	}
}

static void flush_enc_jpeg(int id)
{
	int ii = 0;

	for (ii = 0; ii < MAX_VC_TOP_REG_NUM; ii++) {
		if (ii == 3)
			;
		else
			vc_drv_writereg(ii, VC_TOP_BASE,
				slot[id].user_data.top_reg[ii]);
	}
	for (ii = 0; ii < MAX_VC_PRE_REG_NUM; ii++)
		vc_drv_writereg(ii, VC_PRE_BASE,
			slot[id].user_data.pre_reg[ii]);
	if (vc_enc_stream_full_flag != 1) {
		for (ii = 0; ii < MAX_SCALE_PARM; ii++)
			vc_drv_writereg(VC_PREPROCESS_SRAM_PORT, VC_PRE_BASE,
				slot[id].user_data.scale_reg[ii]);
	}
	for (ii = 0; ii < MAX_VC_JPEG_REG_NUM; ii++)
		vc_drv_writereg(ii, VC_JPEG_BASE,
			slot[id].user_data.jpeg_reg[ii]);
	if (vc_enc_stream_full_flag != 1) {
		for (ii = 0; ii < MAX_VC_JPEG_ENC_CABAC_TABLE; ii++)
			vc_drv_writereg(VC_CABAC_SRAM_PORT, VC_JPEG_BASE,
				slot[id].user_data.cabac_table[ii]);
	}
}

static void vc_drv_flushreg(int id)
{
	if (slot[id].vc_type == DEC_H264)
		flush_dec_h264(id);
	else if ((slot[id].vc_type == DEC_JPEG) ||
			(slot[id].vc_type == DEC_MJPEG))
		flush_dec_jpeg(id);
	else if (slot[id].vc_type == ENC_H264)
		flush_enc_h264(id);
	else if ((slot[id].vc_type == ENC_JPEG) ||
			(slot[id].vc_type == ENC_MJPEG))
		flush_enc_jpeg(id);
	else
		PRT_ERR("--- error vc type ---\n");
}

vc_status_t vc_query_status(int vc_type, unsigned int vc_status)
{
	if ((vc_type == DEC_H264) ||
			(vc_type == DEC_JPEG) || (vc_type == DEC_MJPEG)) {
		switch (vc_status) {
		case (0x1 << 4):
			return VC_DEC_STATUS_SLICE_FININSH_H264;
		case (0x3 << 4):
			return VC_DEC_STATUS_FININSH_H264;
		case (0x1 << 6):
			return VC_DEC_STATUS_FININSH_JPEG;
		case (0x3 << 6):
			return VC_DEC_STATUS_MBLINE_FININSH_JPEG;
		case (0x1 << 8):
			PRT_ERR("vc dec status error\n");
			return VC_DEC_STATUS_ERROR;
		case ((0x1 << 9) | 1):
			return VC_DEC_STATUS_EMPTY;
		case (0x1 << 31):
			PRT_ERR("vc dec status time out\n");
			return VC_DEC_STATUS_TIMEOUT;
		default:
			PRT_ERR("vc dec status unkown error\n");
			return VC_DEC_STATUS_ERROR;
		}
	} else {
		switch (vc_status) {
		case (0x1 << 4):
			return VC_ENC_STATUS_FINISH_H264;
		case (0x1 << 5):
			return VC_ENC_STATUS_FINISH_JPEG;
		case (0x1 << 6):
			return VC_ENC_STATUS_PREVIEW_FINISH;
		case ((0x1 << 8) | 1):
			return VC_ENC_STATUS_STREAM_FULL;
		case (0x1 << 31):
			PRT_ERR("vc enc status time out\n");
			return VC_ENC_STATUS_TIMEOUT;
		default:
			PRT_ERR("vc enc status unkown error\n");
			return VC_ENC_STATUS_ERROR;
		}
	}
}

/*
void print_reg()
{
	int i = 0;

	for (i = 0; i < 60; i++)
		slot[vc_cur_slot_id].user_data.top_reg[i] =
		(unsigned int)(vc_read(G_VC, VC_TOP_BASE, i*4));

	PRT_ERR( "TOP_REG\n");
	for (i=0; i<60; i+=4) {
		PRT_ERR("Offset_%08x:	0x%08x	0x%08x
			0x%08x	0x%08x\n",
			i*4,
			slot[vc_cur_slot_id].user_data.top_reg[i + 0],
			slot[vc_cur_slot_id].user_data.top_reg[i + 1],
			slot[vc_cur_slot_id].user_data.top_reg[i + 2],
			slot[vc_cur_slot_id].user_data.top_reg[i + 3]);
	}

	for (i = 0; i < 60; i++)
		slot[vc_cur_slot_id].user_data.jpeg_reg[i] =
		(unsigned int)(vc_read(G_VC, VC_JPEG_BASE, i*4));

	PRT_ERR("JPEG_REG\n");
	for (i=0; i<60; i+=4)
	{
		PRT_ERR("Offset_%08x:	0x%08x	0x%08x
			0x%08x	0x%08x\n",
			i*4,
			slot[vc_cur_slot_id].user_data.jpeg_reg[i + 0],
			slot[vc_cur_slot_id].user_data.jpeg_reg[i + 1],
			slot[vc_cur_slot_id].user_data.jpeg_reg[i + 2],
			slot[vc_cur_slot_id].user_data.jpeg_reg[i + 3]);
	}
}

void adjust_pll_tmp()
{

	unsigned int rdval, rate;

	rdval = READREG32(VA_PRCM_VC_PLL_EN);
	PRT_ERR("VA_PRCM_VC_PLL_EN : 0x%x\n", rdval);

	rdval = READREG32(VA_PRCM_VC_PLL_FAC);
	PRT_ERR("VA_PRCM_VC_PLL_FAC : 0x%x\n", rdval);

	rdval = READREG32(VA_PRCM_VC_PLL_TUNE0);
	PRT_ERR("VA_PRCM_VC_PLL_TUNE0 : 0x%x\n", rdval);

	rdval = READREG32(VA_PRCM_VC_CLK_CTRL);
	PRT_ERR("VA_PRCM_VC_CLK_CTRL : 0x%x\n", rdval);

	rate = clk_get_rate(vc_clk);
	PRT_ERR("rate : %d\n", rate);

}
*/

static void vc_soft_reset(void)
{
	unsigned int temp;

	temp = vc_read(G_VC, VC_TOP_BASE, 0x14);
	temp = temp | 1;
	vc_write(G_VC, temp, VC_TOP_BASE, 0x14);

	temp = vc_read(G_VC, VC_TOP_BASE, 0x14);
	temp = temp & (0xfffffffe);
	vc_write(G_VC, temp, VC_TOP_BASE, 0x14);

	temp = vc_read(G_VC, VC_TOP_BASE, 0x14);
	temp = temp | 2;
	vc_write(G_VC, temp, VC_TOP_BASE, 0x14);

	temp = vc_read(G_VC, VC_TOP_BASE, 0x14);
	temp = temp & (0xfffffffd);
	vc_write(G_VC, temp, VC_TOP_BASE, 0x14);

	temp = vc_read(G_VC, VC_TOP_BASE, 0x14);
	temp = temp | 4;
	vc_write(G_VC, temp, VC_TOP_BASE, 0x14);

	temp = vc_read(G_VC, VC_TOP_BASE, 0x14);
	temp = temp & (0xfffffffb);
	vc_write(G_VC, temp, VC_TOP_BASE, 0x14);
}


static long vc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int left_time, id, s, slot_id, continus_last_slot, ret;
	struct reg_p *reg;

	switch (cmd) {
	case VC_S_TOPREG_PARAMS:
		reg = (struct reg_p *)arg;
		mutex_lock(&m_mutex);
		continus_last_slot = 0;
		if (vc_occupied == 1) {
			if ((unsigned long)reg->register_p !=
					slot[vc_slice_slot_id].topptr) {
				PRT_ERR("vc : VC_RUN --- UNKNOWN_ERROR\n");
				mutex_unlock(&m_mutex);
				return VC_STATUS_UNKOWNERROR;
			} else {
				continus_last_slot = 1;
			}
		}

		if (continus_last_slot)
			id = vc_slice_slot_id;
		else {
			id = slot_get();
			if (id < 0) {
				PRT_ERR("error:can not get slot\n");
				mutex_unlock(&m_mutex);
				return VC_STATUS_UNKOWNERROR;
			}
		}

		vc_cur_slot_id = id;
		slot[vc_cur_slot_id].topptr = (unsigned long)reg->register_p;

		ret = copy_from_user(slot[vc_cur_slot_id].user_data.top_reg,
			(void __user *)reg->register_p, reg->size);

		slot[vc_cur_slot_id].user_data.top_reg[1] &= ~(1<<8);

		mutex_unlock(&m_mutex);
		return vc_cur_slot_id;

	case VC_S_PREREG_PARAMS:
		reg = (struct reg_p *)arg;
		mutex_lock(&m_mutex);
		ret = copy_from_user(slot[vc_cur_slot_id].user_data.pre_reg,
			(void __user *)reg->register_p, reg->size);
		mutex_unlock(&m_mutex);
		break;

	case VC_S_DECREG_PARAMS:
		reg = (struct reg_p *)arg;
		mutex_lock(&m_mutex);
		slot[vc_cur_slot_id].decptr = (unsigned long)reg->register_p;
		ret = copy_from_user(slot[vc_cur_slot_id].user_data.dec_reg,
			(void __user *)reg->register_p, reg->size);
		mutex_unlock(&m_mutex);
		break;

	case VC_S_ENCREG_PARAMS:
		reg = (struct reg_p *)arg;
		mutex_lock(&m_mutex);
		slot[vc_cur_slot_id].encptr = (unsigned long)reg->register_p;
		ret = copy_from_user(slot[vc_cur_slot_id].user_data.enc_reg,
			(void __user *)reg->register_p, reg->size);
		mutex_unlock(&m_mutex);
		break;

	case VC_S_JPEGREG_PARAMS:
		reg = (struct reg_p *)arg;
		mutex_lock(&m_mutex);
		slot[vc_cur_slot_id].jpegptr = (unsigned long)reg->register_p;
		ret = copy_from_user(slot[vc_cur_slot_id].user_data.jpeg_reg,
			(void __user *)reg->register_p, reg->size);
		mutex_unlock(&m_mutex);
		break;

	case VC_S_ENC_DEC_TABLE:
		reg = (struct reg_p *)arg;
		mutex_lock(&m_mutex);
		ret = copy_from_user(slot[vc_cur_slot_id].user_data.cabac_table,
			(void __user *)reg->register_p, reg->size);
		mutex_unlock(&m_mutex);
		break;

	case VC_S_SCALE_PARAMS:
		reg = (struct reg_p *)arg;
		mutex_lock(&m_mutex);
		ret = copy_from_user(slot[vc_cur_slot_id].user_data.scale_reg,
			(void __user *)reg->register_p, reg->size);
		mutex_unlock(&m_mutex);
		break;

	case VC_RUN:
		mutex_lock(&m_mutex);
		left_time = wait_event_interruptible_timeout(
				waitqueue, vc_idle_flag == 1,
					msecs_to_jiffies(5*1000) + 1);
		if (unlikely(left_time == 0))  {
			PRT_ERR("vc :wait_event_interruptible_timeout 5s\n");
				vc_reset();
			mutex_unlock(&m_mutex);
			return VC_STATUS_UNKOWNERROR;
		}

		if (vc_last_status == -1)
			vc_reset();

		slot[vc_cur_slot_id].vc_type = *((int *)arg);

		vc_drv_flushreg(vc_cur_slot_id);

		vc_occupied = 1;
		vc_idle_flag = 0;

		s = 0;
		if (vc_enc_stream_full_flag == 1)
			s |= 0x101;
		else if (vc_dec_stream_empty_flag == 1)
			s |= 0x201;
		else
			s |= 1;

		if ((vc_enc_stream_full_flag != 1) &&
				(vc_dec_stream_empty_flag != 1))
			enable_vc_irq();

		if ((slot[vc_cur_slot_id].vc_type == DEC_H264)
			|| (slot[vc_cur_slot_id].vc_type == DEC_JPEG)
			|| (slot[vc_cur_slot_id].vc_type == DEC_MJPEG)) {
				if (slot[vc_cur_slot_id].vc_type == DEC_H264)
					enable_vc_all_clock_on();
			vc_write(G_VC, s, VC_TOP_BASE, 0x10);
		} else {
			if (slot[vc_cur_slot_id].vc_type == ENC_H264)
				enable_vc_all_clock_on();
			vc_write(G_VC, s, VC_TOP_BASE, 0xC);
		}

		if ((vc_enc_stream_full_flag == 1) ||
				(vc_dec_stream_empty_flag == 1))
			enable_vc_irq();

		mutex_unlock(&m_mutex);
		return 0;

	case VC_QUERY:
		slot_id = (int)arg;
		if (slot_id != vc_cur_slot_id) {
			PRT_INFO("1-slot_id: %d\n", slot_id);
			PRT_INFO("1-cur_slot: %d\n", vc_cur_slot_id);
		}
		if (vc_irq_registered) {
			vc_status_t s;
			mutex_lock(&m_mutex);
			/* check interrupted */
			left_time = wait_for_completion_timeout(
					&(slot[slot_id].isr_complete),
						msecs_to_jiffies(5*1000) + 1);
			if (unlikely(left_time == 0))  {
				PRT_ERR("QUERY: wait timeout,\n");
				PRT_ERR("slot_id: %d\n", slot_id);
				PRT_ERR("cur:%d\n", vc_cur_slot_id);
				vc_reset();
				s = VC_STATUS_DEAD;
			} else {
				/* normal case */
				int rt = -1;
				if (slot[slot_id].vc_type == DEC_H264)
					rt = copy_to_user(
					(void __user *)slot[slot_id].decptr,
					&(slot[slot_id].user_data.dec_reg),
					MAX_VC_DEC_REG_NUM * 4);
				else if (slot[slot_id].vc_type == ENC_H264)
					rt = copy_to_user(
					(void __user *)slot[slot_id].encptr,
					&(slot[slot_id].user_data.enc_reg[0]),
					MAX_VC_ENC_REG_NUM * 4);
				else if ((slot[slot_id].vc_type == ENC_JPEG)
					|| (slot[slot_id].vc_type == DEC_JPEG)
					|| (slot[slot_id].vc_type == DEC_MJPEG)
					|| (slot[slot_id].vc_type == ENC_MJPEG))
					rt = copy_to_user(
					(void __user *)slot[slot_id].jpegptr,
					&(slot[slot_id].user_data.jpeg_reg[0]),
					MAX_VC_JPEG_REG_NUM * 4);
				else
					PRT_ERR("vc : vc type error\n");
				if (rt != 0)
					PRT_ERR("vc : copy_to_user error\n");
				s = vc_query_status(slot[slot_id].vc_type,
					slot[slot_id].vc_status);
			}

			if ((vc_enc_stream_full_flag != 1) &&
					(vc_dec_stream_empty_flag != 1))
				vc_soft_reset();

			/* free slot */
			if ((slot[vc_cur_slot_id].slice_mode == 0)
					||  (s == VC_STATUS_DEAD)) {
				slot_reset(slot_id);
			}
			mutex_unlock(&m_mutex);
			return s;
		} else {
			PRT_ERR("vc : should not be here\n");
			return -1;
		}
		break;
	default:
		PRT_ERR("vc : no such cmd 0x%x\n", cmd);
		return -EIO;
	}
	/* vc_reset_in_playing(); */
	return 0;
}

static int vc_open(struct inode *inode, struct file *file)
{
	mutex_lock(&m_mutex);
	vc_open_count++;
	if (vc_open_count > 1) {
		PRT_WARN("vc drv already open\n");
		mutex_unlock(&m_mutex);
		return 0;
	}

	clk_prepare_enable(vc_ahb_gate);

	/* reset the vc module */
/*
	p_addr_tmp = ioremap(VA_PRCM_AHB_RST1, 0x4);
	reset_v = READREG32(p_addr_tmp);
	reset_v &= (~(1 << 12));
	WRITEREG32(p_addr_tmp, reset_v);
	PRT_ERR("--- %s -- %d --\n", __FILE__, __LINE__);
	usleep_range(5*1000, 6*1000);
	reset_v |= 1 << 12;
	WRITEREG32(p_addr_tmp, reset_v);
	PRT_ERR("--- %s -- %d --\n", __FILE__, __LINE__);
*/
/*
	clk_prepare(vc_ahb_reset);
	clk_enable(vc_ahb_reset);
	clk_disable(vc_ahb_reset);
	clk_enable(vc_ahb_reset);
*/
	clk_prepare_enable(vc_ahb_reset);

	/* enable axi gate */
	clk_prepare_enable(vc_axi_gate);

	vc_clk_enable();
	disable_vc_irq();
	enable_irq(G_VC->irq);

	vc_idle_flag = 1;
	vc_is_suspend = 0;

	mutex_unlock(&m_mutex);
	return 0;
}

static int vc_release(struct inode *inode, struct file *file)
{
	int i;

	mutex_lock(&m_mutex);
	vc_open_count--;
	if (vc_open_count > 0) {
		/* VC_DBG("vc : vc_release: count:%d pid(%d)\n",
			vc_open_count, task_tgid_vnr(current)); */
		vc_waitforidle();
		goto VC_REL;
	} else if (vc_open_count < 0) {
		PRT_ERR("vc_release: module is closed\n");
		vc_open_count = 0;
	}
	if (vc_open_count == 0) {
		disable_vc_irq();
		disable_irq(G_VC->irq);
	}

	vc_clk_disable();

	/* disable axi && ahb gate */
	clk_disable_unprepare(vc_axi_gate);
	clk_disable_unprepare(vc_ahb_gate);
	clk_disable_unprepare(vc_ahb_reset);
	vc_is_suspend = -1;

	/* PRT_ERR("vc : vc_clk_disable end\n"); */
VC_REL:
	for (i = 0; i < MAX_INSTANCE_NUM; i++) {
		if (slot[i].pid == task_tgid_vnr(current)) {
			PRT_ERR("vc : vc slot is leak by pid(%d), reset it\n",
				 task_tgid_vnr(current));
			if (slot[i].slice_mode == 1 && vc_occupied == 1)
				vc_occupied = 0;
			slot_reset(i);
		}
	}

	mutex_unlock(&m_mutex);
	return 0;
}

#ifdef CONFIG_PM
static int vc_suspend(struct device *_dev)
{
	if (vc_is_suspend == 0) {
		/* store the clk parent */
		g_clk_null = clk_get(NULL, "null_clk");

		/* set the module's clk to null_clk */
		clk_set_parent(vc_clk, g_clk_null);

		vc_clk_disable();

		clk_disable_unprepare(vc_axi_gate);
		clk_disable_unprepare(vc_ahb_gate);
		clk_disable_unprepare(vc_ahb_reset);
		vc_is_suspend = 1;
	} else
		PRT_INFO("vc not need call vdc_suspend: %d!\n", vc_is_suspend);

	/* Do other things about your module's suspend */
	return 0;
}

static int vc_resume(struct device *_dev)
{
	if (vc_is_suspend == 1) {
		clk_prepare_enable(vc_ahb_gate);
		clk_prepare_enable(vc_ahb_reset);
		clk_prepare_enable(vc_axi_gate);

		vc_clk_enable();
		clk_put(g_clk_null);
		vc_is_suspend = 0;
	} else
		PRT_INFO("vc not need call vc_resume: %d!\n", vc_is_suspend);

	return 0;
}

static const struct dev_pm_ops vc_pmops = {
	.suspend = vc_suspend,
	.resume = vc_resume,
};

#endif

static const struct file_operations vc_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = vc_ioctl,
	.compat_ioctl = vc_ioctl,/* compat_vc_ioctl */
	.open = vc_open,
	.release = vc_release,
};

/* #define ENABLE_COOLING */

static int asoc_vc_probe(struct platform_device *pdev)
{
	struct asoc_vc_dev *vc;
	struct resource *iores;
	int ret = 0;

	PRT_INFO("asoc_vc_probe start\n");
	dev_info(&pdev->dev, "Probe vc device\n");
/*
	id = of_match_device(vc_of_match, &pdev->dev);
	if (id != NULL) {
		struct ic_info *info = (struct ic_info *)id->data;
		if (info != NULL) {
			ic_type = info->ic_type;
			PRT_ERR("info ic_type 0x%x\n", ic_type);
		} else {
			PRT_ERR("info is null\n");
		}
	}
*/
	vc = devm_kzalloc(&pdev->dev, sizeof(*vc), GFP_KERNEL);
	if (!vc) {
		ret = -ENOMEM;
		goto devm_kzalloc_err;
	}

	iores = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!iores) {
		ret = -EINVAL;
		goto platform_get_resource_err;
	}

	vc->base = devm_ioremap_resource(&pdev->dev, iores);
	if (IS_ERR(vc->base)) {
		ret = PTR_ERR(vc->base);
		goto devm_ioremap_resource_err;
	}

	vc->irq = platform_get_irq(pdev, 0);
	if (vc->irq < 0) {
		ret = vc->irq;
		goto get_irq_err;
	}

	dev_info(&pdev->dev, "resource: iomem: %pR mapping to %p, irq %d\n",
		iores, vc->base, vc->irq);

	ret = request_irq(vc->irq, (void *)VC_ISR, 0, "vc_isr", 0);
	if (ret) {
		PRT_ERR("vc : register vc irq failed!\n");
		vc_irq_registered = 0;
		ret = -1;
		goto request_irq_err;
	} else {
		vc_irq_registered = 1;
	}
	/* pm_runtime_enable(&pdev->dev); */
	vc_ahb_gate = devm_clk_get(&pdev->dev, VC_AHB_GATE);
	if (IS_ERR(vc_ahb_gate)) {
		PRT_ERR("get vc ahb gate err\n");
		ret = PTR_ERR(vc_ahb_gate);
		goto devm_clk_get_vc_ahb_gate_err;
	}

	vc_ahb_reset = devm_clk_get(&pdev->dev, VC_AHB_RESET);
	if (IS_ERR(vc_ahb_reset)) {
		PRT_ERR("get vc_ahb_reset err\n");
		ret = PTR_ERR(vc_ahb_reset);
		goto devm_clk_get_vc_ahb_reset_err;
	}

	vc_axi_gate = devm_clk_get(&pdev->dev, VC_AXI_GATE);
	if (IS_ERR(vc_axi_gate)) {
		PRT_ERR("get vc_axi_gate err\n");
		ret = PTR_ERR(vc_axi_gate);
		goto devm_clk_get_vc_axi_gate_err;
	}

	/* get the vc clk, set parent clk & clk rate */
	vc_clk = devm_clk_get(&pdev->dev, VC_CLK);
	if (IS_ERR(vc_clk)) {
		PRT_ERR("failed to get vc_clk!\n");
		ret = PTR_ERR(vc_clk);
		goto devm_clk_get_vc_clk_err;
	}
	/* get display_pll struct */
	parent_pll = clk_get(NULL, "vc_pll");
	if (IS_ERR(parent_pll)) {
		PRT_ERR("vc : clk_get parent_pll failed\n");
		ret = PTR_ERR(parent_pll);
		goto clk_get_parent_pll_err;
	}
	disable_irq(vc->irq);
	G_VC = vc;
	gvcdev = pdev;
	PRT_INFO("dev add %p,  vc_clk %p\n", &pdev->dev, vc_clk);

	vc_last_status = 0;
	vc_clk_isenable = 0;
	vc_is_suspend = -1;
	return 0;

clk_get_parent_pll_err:
	devm_clk_put(&pdev->dev, vc_clk);
devm_clk_get_vc_clk_err:
	devm_clk_put(&pdev->dev, vc_axi_gate);
devm_clk_get_vc_axi_gate_err:
	devm_clk_put(&pdev->dev, vc_ahb_reset);
devm_clk_get_vc_ahb_reset_err:
	devm_clk_put(&pdev->dev, vc_ahb_gate);
devm_clk_get_vc_ahb_gate_err:
	free_irq(vc->irq, 0);
request_irq_err:
get_irq_err:
platform_get_resource_err:
devm_ioremap_resource_err:
	devm_kfree(&pdev->dev, vc);
devm_kzalloc_err:
	return ret;
}

static int vc_remove(struct platform_device *pdev)
{
	clk_put(parent_pll);
	devm_clk_put(&pdev->dev, vc_clk);
	devm_clk_put(&pdev->dev, vc_axi_gate);
	devm_clk_put(&pdev->dev, vc_ahb_reset);
	devm_clk_put(&pdev->dev, vc_ahb_gate);

	if (vc_irq_registered == 1)
		free_irq(G_VC->irq, 0);
	if (G_VC != NULL) {
		devm_kfree(&pdev->dev, G_VC);
		G_VC = NULL;
	}
	return 0;
}



static struct platform_driver vc_platform_driver = {
	.driver = {
		.name = DEVDRV_NAME_VC,
		.owner = THIS_MODULE,
		.of_match_table = vc_of_match,
		.pm = &vc_pmops,
	},
	.probe = asoc_vc_probe,
	.remove = vc_remove,
};

static struct miscdevice vc_miscdevice = {
	.minor = VC_DRV_MINOR,
	.name = DEVDRV_NAME_VC,
	.fops = &vc_fops,
};

static int vc_init(void)
{
	int ret;

	PRT_INFO("#### insmod vc driver!\n");
	/* start insmod£¬register device.*/
	ret = misc_register(&vc_miscdevice);
	if (ret) {
		PRT_ERR("register vc misc device failed!\n");
		goto err0;
	}
	PRT_INFO("#### misc_register!\n");
	ret = platform_driver_register(&vc_platform_driver);
	if (ret) {
		PRT_ERR("register gpu platform driver4pm error!\n");
		goto err1;
	}
	PRT_INFO("#### platform_driver_register!\n");
	mutex_init(&m_mutex);
	return 0;
err1:
	free_irq(G_VC->irq, 0);
	misc_deregister(&vc_miscdevice);
err0:
	return ret;
}

static void vc_exit(void)
{
	PRT_INFO("vc module unloaded start\n");
	if (vc_irq_registered)
		free_irq(G_VC->irq, 0);
	misc_deregister(&vc_miscdevice);
	platform_driver_unregister(&vc_platform_driver);
	PRT_INFO("vc module unloaded\n");
}

module_init(vc_init);
module_exit(vc_exit);
MODULE_AUTHOR("lombo@tech");
MODULE_DESCRIPTION("VC kernel module");
MODULE_LICENSE("GPL");
