#ifndef _CLK_H_
#define _CLK_H_

int ax_clk_init(int is_cvu, unsigned int base_reg);
int ax_clk_deinit(void);
int get_adpll_freq(int is_cvu, unsigned int base_reg);
int set_adpll_freq(unsigned int is_cvu, unsigned int freq_mhz, int bwait,
		unsigned int base_reg);
int tune_adpll_freq(unsigned int is_cvu, unsigned int freq, int bwait, unsigned int base_reg);
void set_ax_internal_gating(unsigned int is_cvu, unsigned int enable,
		unsigned int base_reg);
int set_ax_adpll_total_free_run_nu(unsigned int is_cvu, unsigned int freq,
		unsigned int base_reg);
int get_power_on_status(void);

#endif
