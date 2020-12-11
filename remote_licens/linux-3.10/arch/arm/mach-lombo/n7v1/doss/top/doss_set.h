/* doss_set.h */

#ifndef ___DOSS__SET___H___
#define ___DOSS__SET___H___

#ifdef DEF_SET_DOSS_VER
void set_doss_ver(u32 reg_addr,
		u32 ver_l,
		u32 ver_h,
		u32 comp,
		u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_VER_VER_L
def_set_mod_reg_bit(doss, ver, ver_l, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_VER_VER_H
def_set_mod_reg_bit(doss, ver, ver_h, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_VER_COMP
def_set_mod_reg_bit(doss, ver, comp, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO0_FTR
void set_doss_do0_ftr(u32 reg_addr,
			u32 rgb,
			u32 lvds,
			u32 dsi,
			u32 edp,
			u32 tve,
			u32 hdmi,
			u32 tcon,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO0_FTR_RGB
def_set_mod_reg_bit(doss, do0_ftr, rgb, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO0_FTR_LVDS
def_set_mod_reg_bit(doss, do0_ftr, lvds, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO0_FTR_DSI
def_set_mod_reg_bit(doss, do0_ftr, dsi, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO0_FTR_EDP
def_set_mod_reg_bit(doss, do0_ftr, edp, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO0_FTR_TVE
def_set_mod_reg_bit(doss, do0_ftr, tve, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO0_FTR_HDMI
def_set_mod_reg_bit(doss, do0_ftr, hdmi, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO0_FTR_TCON
def_set_mod_reg_bit(doss, do0_ftr, tcon, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO1_FTR
void set_doss_do1_ftr(u32 reg_addr,
			u32 rgb,
			u32 lvds,
			u32 dsi,
			u32 edp,
			u32 tve,
			u32 hdmi,
			u32 tcon,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO1_FTR_RGB
def_set_mod_reg_bit(doss, do1_ftr, rgb, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO1_FTR_LVDS
def_set_mod_reg_bit(doss, do1_ftr, lvds, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO1_FTR_DSI
def_set_mod_reg_bit(doss, do1_ftr, dsi, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO1_FTR_EDP
def_set_mod_reg_bit(doss, do1_ftr, edp, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO1_FTR_TVE
def_set_mod_reg_bit(doss, do1_ftr, tve, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO1_FTR_HDMI
def_set_mod_reg_bit(doss, do1_ftr, hdmi, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_DO1_FTR_TCON
def_set_mod_reg_bit(doss, do1_ftr, tcon, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_RESET
void set_doss_reset(u32 reg_addr,
			u32 rst,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_RESET_RST
def_set_mod_reg_bit(doss, reset, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_ROUTE
void set_doss_route(u32 reg_addr,
			u32 src_do0,
			u32 src_do1,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_ROUTE_SRC_DO0
def_set_mod_reg_bit(doss, route, src_do0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_ROUTE_SRC_DO1
def_set_mod_reg_bit(doss, route, src_do1, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON0_CTRL
void set_doss_tcon0_ctrl(u32 reg_addr,
			u32 slave,
			u32 gate,
			u32 rst,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON0_CTRL_SLAVE
def_set_mod_reg_bit(doss, tcon0_ctrl, slave, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON0_CTRL_GATE
def_set_mod_reg_bit(doss, tcon0_ctrl, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON0_CTRL_RST
def_set_mod_reg_bit(doss, tcon0_ctrl, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON1_CTRL
void set_doss_tcon1_ctrl(u32 reg_addr,
			u32 slave,
			u32 gate,
			u32 rst,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON1_CTRL_SLAVE
def_set_mod_reg_bit(doss, tcon1_ctrl, slave, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON1_CTRL_GATE
def_set_mod_reg_bit(doss, tcon1_ctrl, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON1_CTRL_RST
def_set_mod_reg_bit(doss, tcon1_ctrl, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON0_CLK
void set_doss_tcon0_clk(u32 reg_addr,
			u32 div0,
			u32 div1,
			u32 gate,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON0_CLK_DIV0
def_set_mod_reg_bit(doss, tcon0_clk, div0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON0_CLK_DIV1
def_set_mod_reg_bit(doss, tcon0_clk, div1, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON0_CLK_GATE
def_set_mod_reg_bit(doss, tcon0_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON1_CLK
void set_doss_tcon1_clk(u32 reg_addr,
			u32 div0,
			u32 div1,
			u32 gate,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON1_CLK_DIV0
def_set_mod_reg_bit(doss, tcon1_clk, div0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON1_CLK_DIV1
def_set_mod_reg_bit(doss, tcon1_clk, div1, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TCON1_CLK_GATE
def_set_mod_reg_bit(doss, tcon1_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS0_CTRL
void set_doss_lvds0_ctrl(u32 reg_addr,
			u32 slave,
			u32 gate,
			u32 rst,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS0_CTRL_SLAVE
def_set_mod_reg_bit(doss, lvds0_ctrl, slave, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS0_CTRL_GATE
def_set_mod_reg_bit(doss, lvds0_ctrl, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS0_CTRL_RST
def_set_mod_reg_bit(doss, lvds0_ctrl, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS1_CTRL
void set_doss_lvds1_ctrl(u32 reg_addr,
			u32 slave,
			u32 gate,
			u32 rst,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS1_CTRL_SLAVE
def_set_mod_reg_bit(doss, lvds1_ctrl, slave, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS1_CTRL_GATE
def_set_mod_reg_bit(doss, lvds1_ctrl, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS1_CTRL_RST
def_set_mod_reg_bit(doss, lvds1_ctrl, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS0_CLK
void set_doss_lvds0_clk(u32 reg_addr,
			u32 gate,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS0_CLK_GATE
def_set_mod_reg_bit(doss, lvds0_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS1_CLK
void set_doss_lvds1_clk(u32 reg_addr,
			u32 gate,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_LVDS1_CLK_GATE
def_set_mod_reg_bit(doss, lvds1_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_CTRL
void set_doss_mipi_dsi0_ctrl(u32 reg_addr,
				u32 slave,
				u32 gate,
				u32 rst,
				u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_CTRL_SLAVE
def_set_mod_reg_bit(doss, mipi_dsi0_ctrl, slave, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_CTRL_GATE
def_set_mod_reg_bit(doss, mipi_dsi0_ctrl, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_CTRL_RST
def_set_mod_reg_bit(doss, mipi_dsi0_ctrl, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_CTRL
void set_doss_mipi_dsi1_ctrl(u32 reg_addr,
				u32 slave,
				u32 gate,
				u32 rst,
				u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_CTRL_SLAVE
def_set_mod_reg_bit(doss, mipi_dsi1_ctrl, slave, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_CTRL_GATE
def_set_mod_reg_bit(doss, mipi_dsi1_ctrl, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_CTRL_RST
def_set_mod_reg_bit(doss, mipi_dsi1_ctrl, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_REF_CLK
void set_doss_mipi_dsi0_ref_clk(u32 reg_addr,
				u32 div0,
				u32 gate,
				u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_REF_CLK_DIV0
def_set_mod_reg_bit(doss, mipi_dsi0_ref_clk, div0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_REF_CLK_GATE
def_set_mod_reg_bit(doss, mipi_dsi0_ref_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_REF_CLK
void set_doss_mipi_dsi1_ref_clk(u32 reg_addr,
				u32 div0,
				u32 gate,
				u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_REF_CLK_DIV0
def_set_mod_reg_bit(doss, mipi_dsi1_ref_clk, div0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_REF_CLK_GATE
def_set_mod_reg_bit(doss, mipi_dsi1_ref_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_CFG_CLK
void set_doss_mipi_dsi0_cfg_clk(u32 reg_addr,
				u32 gate,
				u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI0_CFG_CLK_GATE
def_set_mod_reg_bit(doss, mipi_dsi0_cfg_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_CFG_CLK
void set_doss_mipi_dsi1_cfg_clk(u32 reg_addr,
				u32 gate,
				u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_MIPI_DSI1_CFG_CLK_GATE
def_set_mod_reg_bit(doss, mipi_dsi1_cfg_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE0_CTRL
void set_doss_tve0_ctrl(u32 reg_addr,
			u32 slave,
			u32 gate,
			u32 rst,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE0_CTRL_SLAVE
def_set_mod_reg_bit(doss, tve0_ctrl, slave, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE0_CTRL_GATE
def_set_mod_reg_bit(doss, tve0_ctrl, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE0_CTRL_RST
def_set_mod_reg_bit(doss, tve0_ctrl, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE1_CTRL
void set_doss_tve1_ctrl(u32 reg_addr,
			u32 slave,
			u32 gate,
			u32 rst,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE1_CTRL_SLAVE
def_set_mod_reg_bit(doss, tve1_ctrl, slave, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE1_CTRL_GATE
def_set_mod_reg_bit(doss, tve1_ctrl, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE1_CTRL_RST
def_set_mod_reg_bit(doss, tve1_ctrl, rst, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE0_CLK
void set_doss_tve0_clk(u32 reg_addr,
			u32 div0,
			u32 gate,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE0_CLK_DIV0
def_set_mod_reg_bit(doss, tve0_clk, div0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE0_CLK_GATE
def_set_mod_reg_bit(doss, tve0_clk, gate, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE1_CLK
void set_doss_tve1_clk(u32 reg_addr,
			u32 div0,
			u32 gate,
			u32 m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE1_CLK_DIV0
def_set_mod_reg_bit(doss, tve1_clk, div0, addr, val, m_or_r);
#endif

#ifdef DEF_SET_DOSS_TVE1_CLK_GATE
def_set_mod_reg_bit(doss, tve1_clk, gate, addr, val, m_or_r);
#endif

#endif /* ___DOSS__SET___H___ */
