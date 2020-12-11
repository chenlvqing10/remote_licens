#ifndef _CLK_H_
#define _CLK_H_

int ax_clk_init_vu(int is_cvu, unsigned int base_reg);
int ax_clk_deinit_vu(void);
int get_adpll_freq_vu(int is_cvu, unsigned int base_reg);
int set_adpll_freq_vu(unsigned int is_cvu, unsigned int freq_mhz,
		unsigned int base_reg);
void set_ax_internal_gating_vu(unsigned int is_cvu, unsigned int enable,
		unsigned int base_reg);
int get_power_on_status_vu(void);
int set_ax_adpll_total_free_run_vu(unsigned int is_cvu, unsigned int freq,
		unsigned int base_reg);

#endif
