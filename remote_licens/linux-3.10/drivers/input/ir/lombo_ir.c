/*
 * Lombo ir driver
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/sched.h>

#include <mach/csp.h>

/* IR interrupt type */
#define K_IR_TO			1
#define K_IR_F_ERR		2
#define K_IR_REPEAT		3
#define K_IR_FRAME		4
#define K_IR_START		5

/* IR protocol type */
#define	K_IR_PROTOCOL_NEC			0
#define	K_IR_PROTOCOL_SONY_SIRC			1
#define	K_IR_PROTOCOL_RC5			2
#define	K_IR_PROTOCOL_RC6			3

#define IR_SIRC_12		0
#define IR_SIRC_15		1
#define IR_SIRC_20		2
#define IR_SIRC_AUTO		3

#define	K_IR_FRAME_GAP_NEC_MS_CNT		110
#define	K_IR_FRAME_GAP_SIRC_MS_CNT		44
#define	K_IR_FRAME_GAP_RC5_MS_CNT		110
#define	K_IR_FRAME_GAP_RC6_MS_CNT		106

#define	K_IR_BIT_PHASE_NEC_US_CNT		560
#define	K_IR_BIT_PHASE_SIRC_US_CNT		600
#define	K_IR_BIT_PHASE_RC5_US_CNT		889
#define	K_IR_BIT_PHASE_RC6_US_CNT		444

#define	K_IR_EFHOSC_FREQ			24000000	/* 24M */
#define	K_IR_WORK_CLK_FREQ			100000		/* 100k */

/* control nec cmd value, 0-9 */
#define IR_CONTROl_CMD_0	0x19
#define IR_CONTROl_CMD_1	0x45
#define IR_CONTROl_CMD_2	0x46
#define IR_CONTROl_CMD_3	0x47
#define IR_CONTROl_CMD_4	0x44
#define IR_CONTROl_CMD_5	0x40
#define IR_CONTROl_CMD_6	0x43
#define IR_CONTROl_CMD_7	0x7
#define IR_CONTROl_CMD_8	0x15
#define IR_CONTROl_CMD_9	0x9

union tag_ir_frame {
	u32 val;

	/* nec */
	struct {
		u32 addr:8;
		u32 addr_invert:8;
		u32 cmd:8;
		u32 cmd_invert:8;
	} nec;

	struct {
		u32 addr:16;
		u32 cmd:8;
		u32 cmd_invert:8;
	} nec_ext;

	/* rc5 */
	struct {
		u32 cmd:6;
		u32 addr:5;
		u32 t:1;
		u32 s2:1;
		u32 s1:1;
		u32 rsvd:18;
	} rc5;

	/* rc6 */
	struct {
		u32 info:8;
		u32 ctrl:8;
		u32 tr:1;
		u32 mod_bit:3;
		u32 sb:1;
		u32 rsvd:11;
	} rc6;

	/* sirc_12 */
	struct {
		u32 frame_size:5;
		u32 rsvd:15;
		u32 cmd:7;
		u32 addr:5;
	} sirc;

	/* sirc_15 */
	struct {
		u32 frame_size:5;
		u32 rsvd:12;
		u32 cmd:7;
		u32 addr:8;

	} sirc15;

	/* sirc_20 */
	struct {
		u32 frame_size:5;
		u32 rsvd:7;
		u32 cmd:7;
		u32 addr:5;
		u32 ext:8;
	} sirc20;
};

/* rx ir data */
typedef union tag_ir_frame  ir_frame_t;

struct lombo_ir_dev {
	void __iomem		*base;
	struct clk		*ir_clk;
	struct clk		*clk_gate;
	struct clk		*clk_reset;
	int			irq;
	struct input_dev	*dev;
};

/* current process nec protocol */
static int CURRENT_PROTOCOL = K_IR_PROTOCOL_NEC;

/* set IR module enable */
static void csp_ir_set_en(bool en)
{
	reg_ir_ctl_t reg;

	reg.val = READREG32(VA_IR_CTL);
	reg.bits.ir_en = en ? 1 : 0;

	WRITEREG32(VA_IR_CTL, reg.val);
}

/* set IR interrupt enable */
static void csp_ir_set_int_en(int t, bool en)
{
	reg_ir_int_en_t reg;

	reg.val = READREG32(VA_IR_INT_EN);
	switch (t) {
	case K_IR_TO:
		/* IR frame timeout interrupt enable */
		reg.bits.ir_to = en ? 1 : 0;
		break;
	case K_IR_F_ERR:
		/* IR frame error interrupt enable */
		reg.bits.ir_f_err = en ? 1 : 0;
		break;
	case K_IR_REPEAT:
		/* IR repeat interrupt enable */
		reg.bits.ir_repeat = en ? 1 : 0;
		break;
	case K_IR_FRAME:
		/* IR frame interrupt enable */
		reg.bits.ir_frame = en ? 1 : 0;
		break;
	case K_IR_START:
		/* IR start interrupt enable */
		reg.bits.ir_start = en ? 1 : 0;
		break;
	default:
		pr_err("Unknown interrupt type: %d\n", t);
	}

	WRITEREG32(VA_IR_INT_EN, reg.val);
}

/* get IR interrupt pending */
static u32 csp_ir_get_int_pending(int t)
{
	reg_ir_int_pd_t reg;
	u32 val;

	reg.val = READREG32(VA_IR_INT_PD);
	switch (t) {
	case K_IR_TO:
		val = reg.bits.ir_to;
		break;
	case K_IR_F_ERR:
		val = reg.bits.ir_f_err;
		break;
	case K_IR_REPEAT:
		val = reg.bits.ir_repeat;
		break;
	case K_IR_FRAME:
		val = reg.bits.ir_frame;
		break;
	case K_IR_START:
		val = reg.bits.ir_start;
		break;
	default:
		pr_err("Unknown interrupt type: %d\n", t);
		val = 0;
	}

	return val;
}

/* clear IR interrupt */
static void csp_ir_int_clr(int t)
{
	reg_ir_int_clr_t reg;

	switch (t) {
	case K_IR_TO:
		reg.bits.ir_to = 1;
		break;
	case K_IR_F_ERR:
		reg.bits.ir_f_err = 1;
		break;
	case K_IR_REPEAT:
		reg.bits.ir_repeat = 1;
		break;
	case K_IR_FRAME:
		reg.bits.ir_frame = 1;
		break;
	case K_IR_START:
		reg.bits.ir_start = 1;
		break;
	default:
		pr_err("Unknown clear int type: %d\n", t);
	}

	WRITEREG32(VA_IR_INT_CLR, reg.val);
}

/* set IR protocol */
static void csp_ir_set_protocol(int p)
{
	reg_ir_ctl_t reg;

	reg.val = READREG32(VA_IR_CTL);
	switch (p) {
	case K_IR_PROTOCOL_NEC:
		reg.bits.protocol_sel = 0;
		break;
	case K_IR_PROTOCOL_SONY_SIRC:
		reg.bits.protocol_sel = 1;
		break;
	case K_IR_PROTOCOL_RC5:
		reg.bits.protocol_sel = 2;
		break;
	case K_IR_PROTOCOL_RC6:
		reg.bits.protocol_sel = 3;
		break;
	default:
		pr_err("Unknown protocol type: %d\n", p);
	}

	WRITEREG32(VA_IR_CTL, reg.val);
}

/* SIRC protocol select */
static void csp_ir_set_sirc_ext(u32 ext)
{
	reg_ir_ctl_t reg;

	reg.val = READREG32(VA_IR_CTL);
	reg.bits.sirc_ext = ext;

	WRITEREG32(VA_IR_CTL, reg.val);
}

/* set IR clock diver */
static void csp_ir_set_clk_div(u32 div)
{
	reg_ir_ctl_t reg;

	reg.val = READREG32(VA_IR_CTL);
	reg.bits.clk_div = div - 1;

	WRITEREG32(VA_IR_CTL, reg.val);
}

/* set IR timeout threshold */
static void csp_ir_set_timeout_th(u32 th)
{
	reg_ir_timeout_th_t reg;

	reg.val = READREG32(VA_IR_TIMEOUT_TH);
	reg.bits.thd = th;

	WRITEREG32(VA_IR_TIMEOUT_TH, reg.val);
}

/* set IR noise threshold  */
static void csp_ir_set_noise_th(u32 th)
{
	reg_ir_noise_th_t reg;

	reg.val = READREG32(VA_IR_NOISE_TH);
	reg.bits.thd = th;

	WRITEREG32(VA_IR_NOISE_TH, reg.val);
}

/* get IR received data */
static u32 csp_ir_get_data(void)
{
	reg_ir_data_t reg;

	/* this value store the received byte of the IR frame */
	reg.val = READREG32(VA_IR_DATA);
	return reg.bits.data;
}

/* print ir register info for debug */
static void dump_ir_reg(void)
{
	reg_ir_ctl_t ctl_reg;
	reg_ir_timeout_th_t to_th_reg;
	reg_ir_noise_th_t n_th_reg;
	reg_ir_int_en_t en_reg;

	ctl_reg.val = READREG32(VA_IR_CTL);
	pr_info("VA_IR_CTL ir_en:%d, protocol_sel:%d, sirc_ext:%d, clk_div:%d\n",
		ctl_reg.bits.ir_en, ctl_reg.bits.protocol_sel,
		ctl_reg.bits.sirc_ext, ctl_reg.bits.clk_div);

	to_th_reg.val = READREG32(VA_IR_TIMEOUT_TH);
	pr_info("VA_IR_TIMEOUT_TH thd:%d\n", to_th_reg.bits.thd);

	n_th_reg.val = READREG32(VA_IR_NOISE_TH);
	pr_info("VA_IR_NOISE_TH thd:%d\n", n_th_reg.bits.thd);

	en_reg.val = READREG32(VA_IR_INT_EN);
	pr_info("VA_IR_INT_EN ir_to:%d, ir_f_err:%d, ir_repeat:%d, ir_frame:%d, ir_start:%d\n",
		en_reg.bits.ir_to, en_reg.bits.ir_f_err, en_reg.bits.ir_repeat,
		en_reg.bits.ir_frame, en_reg.bits.ir_start);
}

/* report control key envet */
static void report_ir_event(struct input_dev *dev, int cmd)
{
	u32 code;

	switch (cmd) {
	case IR_CONTROl_CMD_0:
		code = BTN_0;
		pr_info("IR control press -> BTN_0\n");
		break;
	case IR_CONTROl_CMD_1:
		code = BTN_1;
		pr_info("IR control press -> BTN_1\n");
		break;
	case IR_CONTROl_CMD_2:
		code = BTN_2;
		pr_info("IR control press -> BTN_2\n");
		break;
	case IR_CONTROl_CMD_3:
		code = BTN_3;
		pr_info("IR control press -> BTN_3\n");
		break;
	case IR_CONTROl_CMD_4:
		code = BTN_4;
		pr_info("IR control press -> BTN_4\n");
		break;
	case IR_CONTROl_CMD_5:
		code = BTN_5;
		pr_info("IR control press -> BTN_5\n");
		break;
	case IR_CONTROl_CMD_6:
		code = BTN_6;
		pr_info("IR control press -> BTN_6\n");
		break;
	case IR_CONTROl_CMD_7:
		code = BTN_7;
		pr_info("IR control press -> BTN_7\n");
		break;
	case IR_CONTROl_CMD_8:
		code = BTN_8;
		pr_info("IR control press -> BTN_8\n");
		break;
	case IR_CONTROl_CMD_9:
		code = BTN_9;
		pr_info("IR control press -> BTN_9\n");
		break;
	default:
		code = KEY_UNKNOWN;
		pr_info("IR control press -> undefine\n");
	}

	input_report_key(dev, code, cmd);
	input_sync(dev);
}

/* decode the frame by protocol */
static void decode_ir_frame(struct lombo_ir_dev *ir_dev,
	ir_frame_t frame, int protocol)
{
	if (protocol == K_IR_PROTOCOL_NEC) {
		/* nec*/
		if (((frame.nec.cmd_invert + frame.nec.cmd) != 0xff) ||
			((frame.nec.addr_invert + frame.nec.addr) != 0xff))
			pr_info("error !!!! nec-frame-invalid\n");
		else {
			pr_info("ir-nec-frame: addr %x, cmd %x\n",
				frame.nec.addr, frame.nec.cmd);
			report_ir_event(ir_dev->dev, frame.nec.cmd);
		}
	} else if (protocol == K_IR_PROTOCOL_SONY_SIRC) {
		/* sony-sirc */
		if (frame.sirc.frame_size == 12)
			pr_info("ir-sirc-frame: frame_size %d,addr %x,cmd %x\n",
				frame.sirc.frame_size, frame.sirc.addr,
				frame.sirc.cmd);
		else if (frame.sirc.frame_size == 15)
			pr_info("ir-sirc-frame: frame_size %d,addr %x,cmd %x\n",
				frame.sirc.frame_size, frame.sirc15.addr,
				frame.sirc15.cmd);
		else if (frame.sirc.frame_size == 20)
			pr_info("ir-sirc-frame: frame_size %d,addr %x,cmd %x\n",
				frame.sirc.frame_size, frame.sirc20.addr,
				frame.sirc20.cmd);
		else
			pr_info("ir-sirc-frame: ?????????");

	} else if (protocol == K_IR_PROTOCOL_RC5) {
		/* rc5 */
		pr_info("ir-rc5-frame: s1 %d, s2 %d, t %d, addr %x, cmd %x\n",
			frame.rc5.s1, frame.rc5.s2, frame.rc5.t,
			frame.rc5.addr, frame.rc5.cmd);
	} else if (protocol == K_IR_PROTOCOL_RC6) {
		/* rc6 */
		pr_info("ir-rc6-frame: s%d, mode%d, tr %d, ctrl %x, info %x\n",
			frame.rc6.sb, frame.rc6.mod_bit, frame.rc6.tr,
			frame.rc6.ctrl, frame.rc6.info);
	}
}

/* initialize IR register */
static void csp_ir_init(void)
{
	u32 src_freq, ir_clk_div, ir_work_freq, protocol;

	u32 frame_gap_ms_cnt;
	u32 frame_timeout_ms_cnt;
	u32 frame_timeout_reg_val;
	u32 bif_phase_us_cnt;
	u32 noise_th_reg_val;

	protocol = CURRENT_PROTOCOL;

	pr_info("--------- csp_ir_init ---------\n");
	/* dump_ir_reg(); */

	/* protocol select */
	csp_ir_set_protocol(protocol);
	if (protocol == K_IR_PROTOCOL_SONY_SIRC)
		csp_ir_set_sirc_ext(IR_SIRC_AUTO);

	/* ir driver cfg */
	src_freq = K_IR_EFHOSC_FREQ;
	ir_work_freq = K_IR_WORK_CLK_FREQ;

	ir_clk_div = src_freq / ir_work_freq;
	csp_ir_set_clk_div(ir_clk_div);

	/* ir timeout threshold */
	if (protocol == K_IR_PROTOCOL_NEC)
		frame_gap_ms_cnt = K_IR_FRAME_GAP_NEC_MS_CNT;
	else if (protocol == K_IR_PROTOCOL_SONY_SIRC)
		frame_gap_ms_cnt = K_IR_FRAME_GAP_SIRC_MS_CNT;
	else if (protocol == K_IR_PROTOCOL_RC5)
		frame_gap_ms_cnt = K_IR_FRAME_GAP_RC5_MS_CNT;
	else if (protocol == K_IR_PROTOCOL_RC6)
		frame_gap_ms_cnt = K_IR_FRAME_GAP_RC6_MS_CNT;

	/* frame-timne-out = 2/3*frame-gap */
	frame_timeout_ms_cnt = frame_gap_ms_cnt * 2 / 3;

	/* (val+1) * 128 * 1/freq = timeout-ms-cnt/1000 */
	frame_timeout_reg_val = (frame_timeout_ms_cnt*ir_work_freq
		/ 1000 / 128) - 1;
	csp_ir_set_timeout_th(frame_timeout_reg_val);

	/* ir noise-threshold */
	if (protocol == K_IR_PROTOCOL_NEC)
		bif_phase_us_cnt = K_IR_BIT_PHASE_NEC_US_CNT;	/* 560us */
	else if (protocol == K_IR_PROTOCOL_SONY_SIRC)
		bif_phase_us_cnt = K_IR_BIT_PHASE_SIRC_US_CNT;	/* 600us */
	else if (protocol == K_IR_PROTOCOL_RC5)
		bif_phase_us_cnt = K_IR_BIT_PHASE_RC5_US_CNT;	/* 889us */
	else if (protocol == K_IR_PROTOCOL_RC6)
		bif_phase_us_cnt = K_IR_BIT_PHASE_RC6_US_CNT;	/* 444us */

	/* noise-th = 1/10 bit-phase */
	noise_th_reg_val = ((bif_phase_us_cnt * ir_work_freq
		/ 1000000 / 10) + 1);
	csp_ir_set_noise_th(noise_th_reg_val);

	/* set enable */
	csp_ir_set_en(true);
	csp_ir_set_int_en(K_IR_TO, true);
	csp_ir_set_int_en(K_IR_F_ERR, true);
	csp_ir_set_int_en(K_IR_REPEAT, true);
	csp_ir_set_int_en(K_IR_FRAME, true);
	csp_ir_set_int_en(K_IR_START, true);

	dump_ir_reg();
}

/* IR interrupt handle function */
static irqreturn_t lombo_ir_irq(int irq, void *data)
{
	ir_frame_t frame;
	struct lombo_ir_dev *ir_dev;

	ir_dev = data;

	/* timeout interrupt */
	if (csp_ir_get_int_pending(K_IR_TO)) {
		pr_info("<K_IR_TO>\n");
		csp_ir_int_clr(K_IR_TO);
	}

	/* frame error interrupt */
	if (csp_ir_get_int_pending(K_IR_F_ERR)) {
		pr_info("<K_IR_F_ERR>\n");
		csp_ir_int_clr(K_IR_F_ERR);
	}

	/* repeat interrupt */
	if (csp_ir_get_int_pending(K_IR_REPEAT)) {
		pr_info("<K_IR_REPEAT>\n");
		csp_ir_int_clr(K_IR_REPEAT);
	}

	/* start interrupt */
	if (csp_ir_get_int_pending(K_IR_START)) {
		pr_info("<K_IR_START>\n");
		csp_ir_int_clr(K_IR_START);
	}

	/* frame interrupt */
	if (csp_ir_get_int_pending(K_IR_FRAME)) {
		int i = 0;
		pr_info("<K_IR_FRAME>\n");
		csp_ir_int_clr(K_IR_FRAME);

		/* handle the ir frame after clear the pending */
		while (i < 10) {
			if (csp_ir_get_int_pending(K_IR_FRAME)) {
				csp_ir_int_clr(K_IR_FRAME);
				i++;
				pr_info("try to clear pending: %d\n", i);
			} else
				break;
		}

		frame.val = csp_ir_get_data();
		decode_ir_frame(ir_dev, frame, CURRENT_PROTOCOL);
	}

	return 0;
}

static int lombo_ir_probe(struct platform_device *pdev)
{
	int ret, err;
	struct lombo_ir_dev *ir_dev;
	struct resource *res;

	pr_info("------------ lombo_ir_probe ------------\n");

	ir_dev = devm_kzalloc(&pdev->dev,
		sizeof(struct lombo_ir_dev), GFP_KERNEL);
	if (!ir_dev) {
		dev_err(&pdev->dev, "devm_kzalloc lombo_ir_dev error\n");
		return -ENOMEM;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ir_dev->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ir_dev->base)) {
		dev_err(&pdev->dev, "err ioremap invalid\n");
		err = PTR_ERR(ir_dev->base);
		goto out;
	}

	/* clk resource */
	ir_dev->ir_clk = devm_clk_get(&pdev->dev, "ir_clk");
	ir_dev->clk_gate = devm_clk_get(&pdev->dev, "apb_ir_gate");
	ir_dev->clk_reset = devm_clk_get(&pdev->dev, "apb_ir_reset");
	if (IS_ERR(ir_dev->ir_clk) || IS_ERR(ir_dev->clk_gate) ||
		IS_ERR(ir_dev->clk_reset)) {
		dev_err(&pdev->dev, "get ir clock resource failed\n");
		err = -EINVAL;
		goto out;
	}

	ret = clk_prepare_enable(ir_dev->ir_clk);
	if (ret) {
		dev_err(&pdev->dev, "enable ir clk failed\n");
		err = ret;
		goto out;
	}

	ret = clk_prepare_enable(ir_dev->clk_reset);
	if (ret) {
		dev_err(&pdev->dev, "enable ir clk reset failed\n");
		err = ret;
		goto out1;
	}

	ret = clk_prepare_enable(ir_dev->clk_gate);
	if (ret) {
		dev_err(&pdev->dev, "enable ir clk gate failed\n");
		err = ret;
		goto out2;
	}

	/* get irq */
	ir_dev->irq = platform_get_irq(pdev, 0);
	if (ir_dev->irq < 0) {
		dev_err(&pdev->dev, "platform_get_irq error: missing IRQ\n");
		err = -EINVAL;
		goto out3;
	}

	/* init ir */
	csp_ir_init();

	/* alloc input device */
	ir_dev->dev = input_allocate_device();
	if (!ir_dev->dev) {
		dev_err(&pdev->dev, "input allocate device error\n");
		err = -ENOMEM;
		goto out3;
	}

	/* set input device event type and key type */
	ir_dev->dev->name = "lombo-ir";

	/* input event type, support key event */
	ir_dev->dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY);

	/* key code, 0 -9 */
	ir_dev->dev->keybit[BIT_WORD(BTN_0)] |= BIT_MASK(BTN_0);
	set_bit(BTN_0, ir_dev->dev->keybit);
	set_bit(BTN_1, ir_dev->dev->keybit);
	set_bit(BTN_2, ir_dev->dev->keybit);
	set_bit(BTN_3, ir_dev->dev->keybit);
	set_bit(BTN_4, ir_dev->dev->keybit);
	set_bit(BTN_5, ir_dev->dev->keybit);
	set_bit(BTN_6, ir_dev->dev->keybit);
	set_bit(BTN_7, ir_dev->dev->keybit);
	set_bit(BTN_8, ir_dev->dev->keybit);
	set_bit(BTN_9, ir_dev->dev->keybit);

	/* register device to input subsystem */
	ret = input_register_device(ir_dev->dev);
	if (ret) {
		dev_err(&pdev->dev, "input device register error\n");
		err = ret;
		goto out4;
	}

	ret = devm_request_irq(&pdev->dev, ir_dev->irq, lombo_ir_irq,
				IRQF_SHARED, "lombo-ir", ir_dev);
	if (ret) {
		dev_err(&pdev->dev, "devm_request_irq error\n");
		err = ret;
		goto out5;
	}

	platform_set_drvdata(pdev, ir_dev);
	dev_info(&pdev->dev, "lombo_ir_probe end with %d\n", ret);
	return 0;

out5:
	input_unregister_device(ir_dev->dev);
out4:
	input_free_device(ir_dev->dev);
out3:
	clk_disable_unprepare(ir_dev->clk_gate);
out2:
	clk_disable_unprepare(ir_dev->clk_reset);
out1:
	clk_disable_unprepare(ir_dev->ir_clk);
out:
	devm_kfree(&pdev->dev, ir_dev);
	return err;

}

static int lombo_ir_remove(struct platform_device *pdev)
{
	struct lombo_ir_dev *ir_dev;

	ir_dev = platform_get_drvdata(pdev);
	input_unregister_device(ir_dev->dev);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lombo_ir_dt_match[] = {
	{ .compatible = "lombo,n7-ir" },
	{},
};
MODULE_DEVICE_TABLE(of, lombo_ir_dt_match);
#endif

static struct platform_driver lombo_ir_driver = {
	.probe		= lombo_ir_probe,
	.remove		= lombo_ir_remove,
	.driver		= {
		.name	= "lombo-ir",
		.owner	= THIS_MODULE,
		#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(lombo_ir_dt_match),
		#endif
	},
};
module_platform_driver(lombo_ir_driver);

MODULE_DESCRIPTION("LomboTech ir driver");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_LICENSE("GPL");

