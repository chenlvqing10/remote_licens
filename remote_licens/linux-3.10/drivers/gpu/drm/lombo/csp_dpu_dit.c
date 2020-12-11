
#include <linux/io.h>
#include <linux/delay.h>

#include "csp_dpu_dit_i.h"
#include "csp_dpu_dit_reg.h"

#define cpuwr32(reg, val) writel((val), (void __iomem *)(reg))
#define cpurd32(reg)      readl((void __iomem *)(reg))

#define CSP_DIT_VERSION_MAJOR 1
#define CSP_DIT_VERSION_MINOR 0

static __regs_dit_t *dit_reg;
static u32 reg_size;

s32 csp_dit_init(void)
{
	return 0;
}

s32 csp_dit_exit(void)
{
	return 0;
}

s32 csp_dit_set_register_base(uintptr_t addr, u32 size)
{
	dit_reg = (__regs_dit_t *)(addr);
	reg_size = size;

	return 0;
}

u32 csp_dit_get_ver(u32 *major, u32 *minor)
{
	if (major)
		*major = CSP_DIT_VERSION_MAJOR;
	if (minor)
		*minor = CSP_DIT_VERSION_MINOR;
	return 0;
}

s32 csp_dit_enable(void)
{
	reg_dit_ctl_t tmpreg;
	tmpreg.val = 0;
	tmpreg.bits.dit_en = 1;
	cpuwr32(&(dit_reg->ctl), tmpreg.val);

	return 0;
}

s32 csp_dit_disable(void)
{
	reg_dit_ctl_t tmpreg;
	tmpreg.val = 0;
	tmpreg.bits.dit_en = 0;
	cpuwr32(&(dit_reg->ctl), tmpreg.val);

	return 0;
}

s32 csp_dit_reset(void)
{
	reg_dit_ctl_t tmpreg;

	tmpreg.val = cpurd32(&(dit_reg->ctl));
	tmpreg.bits.rst = 1;
	cpuwr32(&(dit_reg->ctl), tmpreg.val);
	udelay(3);
	tmpreg.bits.rst = 0;
	cpuwr32(&(dit_reg->ctl), tmpreg.val);

	return 0;
}

/*
  Set field1 polarity, TOP_FIELD/BOTTOM_FIELD
*/
s32 csp_dit_set_field_pol(__dit_field_pol_t pol)
{
	reg_dit_cfg_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->cfg));
	tmpreg.bits.field_pol = pol;
	cpuwr32(&(dit_reg->cfg), tmpreg.val);

	return 0;
}

/*
  UV combine data can be output as inversed sequence
  Not use for Y data
*/
s32 csp_dit_set_pixseq(u32 is_inversed)
{
	reg_dit_cfg_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->cfg));
	tmpreg.bits.pixseq = is_inversed;
	cpuwr32(&(dit_reg->cfg), tmpreg.val);

	return 0;
}

/*
  Set to 0 for Y/U/V single planar data
  Set to 1 for UV combine data
*/
s32 csp_dit_set_mode(u32 dit_mode)
{
	reg_dit_cfg_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->cfg));
	tmpreg.bits.ditmode = dit_mode;
	cpuwr32(&(dit_reg->cfg), tmpreg.val);

	return 0;
}

/* ======irq and status========================== */
s32 csp_dit_enable_irq_src(__dit_irq_src_t irq_trig)
{
	reg_dit_wbintctl_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->wbintctl));
	tmpreg.bits.wbfin_en = irq_trig & DIT_IRQ_WB_FINISH;
	tmpreg.bits.wbovfl_en = irq_trig & DIT_IRQ_WB_OVERFLOW;
	tmpreg.bits.wbtmout_en = irq_trig & DIT_IRQ_WB_TIMEOUT;
	cpuwr32(&(dit_reg->wbintctl), tmpreg.val);

	return 0;
}

s32 csp_dit_disable_irq_src(__dit_irq_src_t irq_trig)
{
	reg_dit_wbintctl_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->wbintctl));
	tmpreg.bits.wbfin_en = !(irq_trig & DIT_IRQ_WB_FINISH);
	tmpreg.bits.wbovfl_en = !(irq_trig & DIT_IRQ_WB_OVERFLOW);
	tmpreg.bits.wbtmout_en = !(irq_trig & DIT_IRQ_WB_TIMEOUT);
	cpuwr32(&(dit_reg->wbintctl), tmpreg.val);

	return 0;
}

s32 csp_dit_get_irq_status(void)
{
	reg_dit_wbintsta_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->wbintsta));

	return tmpreg.val;
}

s32 csp_dit_clear_irq_status(__dit_irq_src_t irq_trig)
{
	reg_dit_wbintclr_t tmpreg;
	tmpreg.val = irq_trig;
	cpuwr32(&(dit_reg->wbintclr), tmpreg.val);

	return 0;
}

/* ======irq and status========================== */

/*
	size and buffer
*/
s32 csp_dit_set_input_addr(unsigned long long addr[4])
{
	cpuwr32(&(dit_reg->inaddr0), (unsigned int)addr[0]);
	cpuwr32(&(dit_reg->inaddr1), (unsigned int)addr[1]);
	cpuwr32(&(dit_reg->inaddr2), (unsigned int)addr[2]);
	cpuwr32(&(dit_reg->inaddr3), (unsigned int)addr[3]);

	return 0;
}

s32 csp_dit_set_output_addr(unsigned long long addr)
{
	cpuwr32(&(dit_reg->outaddr), (unsigned int)addr);

	return 0;
}

s32 csp_dit_set_insize(u32 inw, u32 inh)
{
	reg_dit_insize_t tmpreg;
	tmpreg.bits.w = inw - 1;
	tmpreg.bits.h = inh - 1;

	cpuwr32(&(dit_reg->insize), tmpreg.val);

	return 0;
}

s32 csp_dit_set_outsize(u32 outw, u32 outh)
{
	reg_dit_outsize_t tmpreg;
	tmpreg.bits.w = outw - 1;
	tmpreg.bits.h = outh - 1;

	cpuwr32(&(dit_reg->outsize), tmpreg.val);

	return 0;
}

s32 csp_dit_set_input_linestride(u32 lstr)
{
	reg_dit_lstr_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->lstr));
	tmpreg.bits.ilstr = lstr;

	cpuwr32(&(dit_reg->lstr), tmpreg.val);

	return 0;
}

s32 csp_dit_set_output_linestride(u32 lstr)
{
	reg_dit_lstr_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->lstr));
	tmpreg.bits.olstr = lstr;

	cpuwr32(&(dit_reg->lstr), tmpreg.val);

	return 0;
}

s32 csp_dit_wb_start(void)
{
	reg_dit_wbctl_t tmpreg;
	tmpreg.bits.start = 1;
	cpuwr32(&(dit_reg->wbctl), tmpreg.val);

	return 0;
}

s32 csp_dit_wb_stop(void)
{
	reg_dit_wbctl_t tmpreg;
	tmpreg.bits.start = 0;
	cpuwr32(&(dit_reg->wbctl), tmpreg.val);

	return 0;
}

s32 csp_dit_set_wb_timer_thr(u32 timeout_len)
{
	reg_dit_wbtmr_t tmpreg;
	tmpreg.bits.wbthr = (timeout_len > 0xFFFF) ? 0xFFFF : timeout_len;
	cpuwr32(&(dit_reg->wbtmr), tmpreg.val);

	return 0;
}

s32 csp_dit_get_wb_timer_thr(void)
{
	s32 timer_thr;
	reg_dit_wbtmr_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->wbtmr));
	timer_thr = tmpreg.bits.wbthr;
	return timer_thr;
}

s32 csp_dit_get_wb_timer_status(void)
{
	s32 timer_val;
	reg_dit_wbtmr_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->wbtmr));
	timer_val = tmpreg.bits.wbtmr;
	return timer_val;
}

s32 csp_dit_check_wb_finish(void)
{
	reg_dit_wbintsta_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->wbintsta));

	return tmpreg.val & DIT_IRQ_WB_FINISH;
}

s32 csp_dit_check_wb_timeout(void)
{
	reg_dit_wbintsta_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->wbintsta));

	return tmpreg.val & DIT_IRQ_WB_TIMEOUT;
}

s32 csp_dit_check_wb_overflow(void)
{
	reg_dit_wbintsta_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->wbintsta));

	return tmpreg.val & DIT_IRQ_WB_OVERFLOW;
}

s32 csp_dit_enable_still_avg(void)
{
	reg_dit_sthr_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->sthr));
	tmpreg.bits.stavg = 1;
	cpuwr32(&(dit_reg->sthr), tmpreg.val);
	return 0;
}

s32 csp_dit_disable_still_avg(void)
{
	reg_dit_sthr_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->sthr));
	tmpreg.bits.stavg = 0;
	cpuwr32(&(dit_reg->sthr), tmpreg.val);
	return 0;
}

s32 csp_dit_set_still_threshold(u32 still_thr)
{
	reg_dit_sthr_t tmpreg;
	tmpreg.val = cpurd32(&(dit_reg->sthr));
	tmpreg.bits.sthr0 = still_thr;
	cpuwr32(&(dit_reg->sthr), tmpreg.val);
	return 0;
}

s32 csp_dit_set_interpolation_threshold(u32 thr0, u32 thr1, u32 thr2, u32 thr3)
{
	reg_dit_ithr_t tmpreg;
	tmpreg.bits.thr0 = thr0;
	tmpreg.bits.thr1 = thr1;
	tmpreg.bits.thr2 = thr2;
	tmpreg.bits.thr3 = thr3;
	cpuwr32(&(dit_reg->ithr), tmpreg.val);
	return 0;
}

void csp_dit_cal_field_addr_from_frame_addr(
	unsigned long long frameaddr[3], unsigned long long pfields[4],
	u32 lstr, __dit_field_seq_t fdseq, __dit_field_pol_t F0_pol)
{
	if (fdseq == DIT_TFF_SEQ) {
		if (F0_pol == DIT_FIRST_FIELD) {
			/* use frame 0/1 */
			pfields[0] = frameaddr[0];
			pfields[1] = frameaddr[0] + lstr;
			pfields[2] = frameaddr[1];
			pfields[3] = frameaddr[1] + lstr;
		} else if (F0_pol == DIT_SECOND_FIELD) {
			/* use frame 0_bot/1/2_top */
			pfields[0] = frameaddr[0] + lstr;
			pfields[1] = frameaddr[1];
			pfields[2] = frameaddr[1] + lstr;
			pfields[3] = frameaddr[0]; /* can use 0 or 2 */
		}
	} else if (fdseq == DIT_BFF_SEQ) {
		if (F0_pol == DIT_FIRST_FIELD) {
			/* first field for BFF */
			/* use frame 0/1 */
			pfields[0] = frameaddr[1];
			pfields[1] = frameaddr[1] + lstr;
			pfields[2] = frameaddr[2];
			pfields[3] = frameaddr[2] + lstr;
		} else if (F0_pol == DIT_SECOND_FIELD) {
			/* use frame 0_bot/1/2_top */
			pfields[0] = frameaddr[0] + lstr;
			pfields[1] = frameaddr[1];
			pfields[2] = frameaddr[1] + lstr;
			pfields[3] = frameaddr[2];	/* can use 0 or 2 */
		}
	}
}
