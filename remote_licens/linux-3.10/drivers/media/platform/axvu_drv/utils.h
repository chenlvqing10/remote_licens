#ifndef _UTILS_H_
#define _UTILS_H_

#define ADPLL_ON 1
#define PRCM_ON 0

/* Multi-handles */
#define _MAX_HANDLES_ 32

#define ax_err(n, x, arg...) \
	do { \
		if (n) \
			printk("[NV_INFO]"x, ##arg); \
	} while (0)

typedef struct {
	unsigned int mtiks;
	unsigned int mrbw;
	unsigned int mwbw;
} dgb_perf_cnt_t;

typedef struct {
	unsigned int reg_addr;
	unsigned int reg_val;
} ax_reg_t;

typedef struct {
	ax_reg_t vu_ctl;
	ax_reg_t irq_en;
	ax_reg_t input_fmt;
	ax_reg_t input_stride;

	ax_reg_t input_size;
	ax_reg_t input_y_ddr;
	ax_reg_t input_uv_ddr;
	ax_reg_t output_fmt;
	ax_reg_t normal_output_size;
	ax_reg_t output_pym0_size;
	ax_reg_t output_pym1_size;
	ax_reg_t output_pym2_size;
	ax_reg_t output_pym3_size;
	ax_reg_t output_pym4_size;
	ax_reg_t output_pym5_size;
	ax_reg_t output_pym6_size;
	ax_reg_t output_pym7_size;

	ax_reg_t normal_output_addr;
	ax_reg_t offset_set;
	ax_reg_t crop_size;
	ax_reg_t normal_sclaex;
	ax_reg_t normal_sclaey;

	ax_reg_t pym0_scalex;
	ax_reg_t pym0_scaley;
	ax_reg_t pym1_scalex;
	ax_reg_t pym1_scaley;
	ax_reg_t pym2_scalex;
	ax_reg_t pym2_scaley;
	ax_reg_t pym3_scalex;
	ax_reg_t pym3_scaley;
	ax_reg_t pym4_scalex;
	ax_reg_t pym4_scaley;
	ax_reg_t pym5_scalex;
	ax_reg_t pym5_scaley;
	ax_reg_t pym6_scalex;
	ax_reg_t pym6_scaley;
	ax_reg_t pym7_scalex;
	ax_reg_t pym7_scaley;
	ax_reg_t pym_addr;
} vu_regs_t;

void *vu_malloc(int size);
void vu_free(void *ptr);

#endif
