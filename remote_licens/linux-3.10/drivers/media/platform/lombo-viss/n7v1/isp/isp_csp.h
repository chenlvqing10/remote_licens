#ifndef __ISP_CSP_H__
#define __ISP_CSP_H__

#include <linux/types.h>

u32 isp_reg_read(void *base, u32 reg);
void isp_reg_write(void *base, u32 reg, u32 val);
void isp_reg_clr(void *base, u32 reg, u32 clr_bits);
void isp_reg_set(void *base, u32 reg, u32 set_bits);
void isp_reg_clr_set(void *base, u32 reg,
		u32 clr_bits, u32 set_bits);
void csp_isp_cfg_clk_en(void *base);
void csp_isp_data_path_ctrl(void *base);

void csp_isp_enable(void *base, u32 val);
void csp_isp_set_ctrl(void *base);
void csp_isp_acq_cfg(void *base, u32 val);
void csp_isp_set_hor_input_offset(void *base, u32 val);
void csp_isp_set_ver_input_offset(void *base, u32 val);
void csp_isp_set_hor_input_size(void *base, u32 val);
void csp_isp_set_ver_input_size(void *base, u32 val);
void csp_isp_set_hor_offset(void *base, u32 val);
void csp_isp_set_ver_offset(void *base, u32 val);
void csp_isp_set_hor_size(void *base, u32 val);
void csp_isp_set_ver_size(void *base, u32 val);

u32 csp_isp_get_isp_int_status(void *base);
u32 csp_isp_check_data_loss(void *base);
u32 csp_isp_check_off(void *base);
void csp_isp_en_data_loss(void *base, u32 val);
void csp_isp_disable_isp_int(void *base);
#if 0
void csp_isp_clr_isp_int(void *base);
#else
void csp_isp_clr_isp_int(void *base, u32 imask);
#endif

void csp_isp_ippu_ctrl(void *base);
u32 csp_isp_ippu_is_enabled(void *base);
void csp_isp_enable_ippu(void *base, u32 val);
void csp_isp_set_tnr_input_add(void *base, u32 y_add,
		u32 cb_add, u32 cr_add);
void csp_isp_set_tnr_output_add(void *base, u32 y_add,
		u32 cb_add, u32 cr_add);

void csp_isp_dma_config_update(void *base);
void csp_isp_dma_main_path_enable(void *base, u32 val);
void csp_isp_dma_sub_path_enable(void *base, u32 val);
void csp_isp_dma_ctrl(void *base, u32 fmt);
void csp_isp_set_dma_main_path_add(void *base, u32 y_add,
		u32 cb_add, u32 cr_add);
void csp_isp_set_dma_sub_path_add(void *base, u32 y_add,
		u32 cb_add, u32 cr_add);
void csp_isp_set_dma_main_path_y_size(void *base, u32 y_size,
		u32 cb_size, u32 cr_size);
void csp_isp_set_dma_sub_path_y_size(void *base, u32 y_size,
		u32 cb_size, u32 cr_size);
void csp_isp_set_main_path_scale(void *base, u32 inw, u32 inh,
			u32 outw, u32 outh);
void csp_isp_set_sub_path_scale(void *base, u32 inw, u32 inh,
			u32 outw, u32 outh);

void csp_isp_set_dma_y_pic_start_add(void *base, u32 val);
void csp_isp_set_dma_y_pic_width(void *base, u32 val);
void csp_isp_set_dma_y_pic_len(void *base, u32 val);
void csp_isp_set_dma_y_pic_size(void *base, u32 val);

u32 csp_isp_get_dma_int_status(void *base);
u32 csp_isp_check_main_path_int(void *base);
u32 csp_isp_check_sub_path_int(void *base);
void csp_isp_clr_dma_int(void *base, u32 imask);
void csp_isp_disable_dma_int(void *base);
u32 csp_isp_get_dma_status(void *base);
void csp_isp_clr_dma_status(void *base, u32 status);

void csp_isp_set_anti_shake_hor_offset(void *base, u32 val);
void csp_isp_set_anti_shake_ver_offset(void *base, u32 val);
void csp_isp_set_anti_shake_hor_size(void *base, u32 val);
void csp_isp_set_anti_shake_ver_size(void *base, u32 val);
void csp_isp_set_anti_shake_address(void *base, u32 val);
void csp_isp_set_bgm_stat_add(void *base, u32 val);
void csp_isp_dump_regs(void *base, u32 *val);
void csp_isp_restore_regs(void *base, u32 *val);
void csp_isp_dump_stat(void *base, u32 *val);
void csp_isp_dma_start(void *base);
void csp_isp_dma_start_ctrl(void *base, u32 val);

#endif /* __ISP_CSP_H__ */
