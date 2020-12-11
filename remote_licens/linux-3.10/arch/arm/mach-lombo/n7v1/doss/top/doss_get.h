/* doss_get.h */

#ifndef ___DOSS__GET___H___
#define ___DOSS__GET___H___

#ifdef DEF_GET_DOSS_VER_VER_L
def_get_mod_reg_bit(doss, ver, ver_l, addr);
#endif

#ifdef DEF_GET_DOSS_VER_VER_H
def_get_mod_reg_bit(doss, ver, ver_h, addr);
#endif

#ifdef DEF_GET_DOSS_VER_COMP
def_get_mod_reg_bit(doss, ver, comp, addr);
#endif

#ifdef DEF_GET_DOSS_DO0_FTR_RGB
def_get_mod_reg_bit(doss, do0_ftr, rgb, addr);
#endif

#ifdef DEF_GET_DOSS_DO0_FTR_LVDS
def_get_mod_reg_bit(doss, do0_ftr, lvds, addr);
#endif

#ifdef DEF_GET_DOSS_DO0_FTR_DSI
def_get_mod_reg_bit(doss, do0_ftr, dsi, addr);
#endif

#ifdef DEF_GET_DOSS_DO0_FTR_EDP
def_get_mod_reg_bit(doss, do0_ftr, edp, addr);
#endif

#ifdef DEF_GET_DOSS_DO0_FTR_TVE
def_get_mod_reg_bit(doss, do0_ftr, tve, addr);
#endif

#ifdef DEF_GET_DOSS_DO0_FTR_HDMI
def_get_mod_reg_bit(doss, do0_ftr, hdmi, addr);
#endif

#ifdef DEF_GET_DOSS_DO0_FTR_TCON
def_get_mod_reg_bit(doss, do0_ftr, tcon, addr);
#endif

#ifdef DEF_GET_DOSS_DO1_FTR_RGB
def_get_mod_reg_bit(doss, do1_ftr, rgb, addr);
#endif

#ifdef DEF_GET_DOSS_DO1_FTR_LVDS
def_get_mod_reg_bit(doss, do1_ftr, lvds, addr);
#endif

#ifdef DEF_GET_DOSS_DO1_FTR_DSI
def_get_mod_reg_bit(doss, do1_ftr, dsi, addr);
#endif

#ifdef DEF_GET_DOSS_DO1_FTR_EDP
def_get_mod_reg_bit(doss, do1_ftr, edp, addr);
#endif

#ifdef DEF_GET_DOSS_DO1_FTR_TVE
def_get_mod_reg_bit(doss, do1_ftr, tve, addr);
#endif

#ifdef DEF_GET_DOSS_DO1_FTR_HDMI
def_get_mod_reg_bit(doss, do1_ftr, hdmi, addr);
#endif

#ifdef DEF_GET_DOSS_DO1_FTR_TCON
def_get_mod_reg_bit(doss, do1_ftr, tcon, addr);
#endif

#ifdef DEF_GET_DOSS_RESET_RST
def_get_mod_reg_bit(doss, reset, rst, addr);
#endif

#ifdef DEF_GET_DOSS_ROUTE_SRC_DO0
def_get_mod_reg_bit(doss, route, src_do0, addr);
#endif

#ifdef DEF_GET_DOSS_ROUTE_SRC_DO1
def_get_mod_reg_bit(doss, route, src_do1, addr);
#endif

#ifdef DEF_GET_DOSS_TCON0_CTRL_SLAVE
def_get_mod_reg_bit(doss, tcon0_ctrl, slave, addr);
#endif

#ifdef DEF_GET_DOSS_TCON0_CTRL_GATE
def_get_mod_reg_bit(doss, tcon0_ctrl, gate, addr);
#endif

#ifdef DEF_GET_DOSS_TCON0_CTRL_RST
def_get_mod_reg_bit(doss, tcon0_ctrl, rst, addr);
#endif

#ifdef DEF_GET_DOSS_TCON1_CTRL_SLAVE
def_get_mod_reg_bit(doss, tcon1_ctrl, slave, addr);
#endif

#ifdef DEF_GET_DOSS_TCON1_CTRL_GATE
def_get_mod_reg_bit(doss, tcon1_ctrl, gate, addr);
#endif

#ifdef DEF_GET_DOSS_TCON1_CTRL_RST
def_get_mod_reg_bit(doss, tcon1_ctrl, rst, addr);
#endif

#ifdef DEF_GET_DOSS_TCON0_CLK_DIV0
def_get_mod_reg_bit(doss, tcon0_clk, div0, addr);
#endif

#ifdef DEF_GET_DOSS_TCON0_CLK_DIV1
def_get_mod_reg_bit(doss, tcon0_clk, div1, addr);
#endif

#ifdef DEF_GET_DOSS_TCON0_CLK_GATE
def_get_mod_reg_bit(doss, tcon0_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_TCON1_CLK_DIV0
def_get_mod_reg_bit(doss, tcon1_clk, div0, addr);
#endif

#ifdef DEF_GET_DOSS_TCON1_CLK_DIV1
def_get_mod_reg_bit(doss, tcon1_clk, div1, addr);
#endif

#ifdef DEF_GET_DOSS_TCON1_CLK_GATE
def_get_mod_reg_bit(doss, tcon1_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_LVDS0_CTRL_SLAVE
def_get_mod_reg_bit(doss, lvds0_ctrl, slave, addr);
#endif

#ifdef DEF_GET_DOSS_LVDS0_CTRL_GATE
def_get_mod_reg_bit(doss, lvds0_ctrl, gate, addr);
#endif

#ifdef DEF_GET_DOSS_LVDS0_CTRL_RST
def_get_mod_reg_bit(doss, lvds0_ctrl, rst, addr);
#endif

#ifdef DEF_GET_DOSS_LVDS1_CTRL_SLAVE
def_get_mod_reg_bit(doss, lvds1_ctrl, slave, addr);
#endif

#ifdef DEF_GET_DOSS_LVDS1_CTRL_GATE
def_get_mod_reg_bit(doss, lvds1_ctrl, gate, addr);
#endif

#ifdef DEF_GET_DOSS_LVDS1_CTRL_RST
def_get_mod_reg_bit(doss, lvds1_ctrl, rst, addr);
#endif

#ifdef DEF_GET_DOSS_LVDS0_CLK_GATE
def_get_mod_reg_bit(doss, lvds0_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_LVDS1_CLK_GATE
def_get_mod_reg_bit(doss, lvds1_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI0_CTRL_SLAVE
def_get_mod_reg_bit(doss, mipi_dsi0_ctrl, slave, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI0_CTRL_GATE
def_get_mod_reg_bit(doss, mipi_dsi0_ctrl, gate, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI0_CTRL_RST
def_get_mod_reg_bit(doss, mipi_dsi0_ctrl, rst, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI1_CTRL_SLAVE
def_get_mod_reg_bit(doss, mipi_dsi1_ctrl, slave, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI1_CTRL_GATE
def_get_mod_reg_bit(doss, mipi_dsi1_ctrl, gate, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI1_CTRL_RST
def_get_mod_reg_bit(doss, mipi_dsi1_ctrl, rst, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI0_REF_CLK_DIV0
def_get_mod_reg_bit(doss, mipi_dsi0_ref_clk, div0, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI0_REF_CLK_GATE
def_get_mod_reg_bit(doss, mipi_dsi0_ref_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI1_REF_CLK_DIV0
def_get_mod_reg_bit(doss, mipi_dsi1_ref_clk, div0, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI1_REF_CLK_GATE
def_get_mod_reg_bit(doss, mipi_dsi1_ref_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI0_CFG_CLK_GATE
def_get_mod_reg_bit(doss, mipi_dsi0_cfg_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_MIPI_DSI1_CFG_CLK_GATE
def_get_mod_reg_bit(doss, mipi_dsi1_cfg_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_TVE0_CTRL_SLAVE
def_get_mod_reg_bit(doss, tve0_ctrl, slave, addr);
#endif

#ifdef DEF_GET_DOSS_TVE0_CTRL_GATE
def_get_mod_reg_bit(doss, tve0_ctrl, gate, addr);
#endif

#ifdef DEF_GET_DOSS_TVE0_CTRL_RST
def_get_mod_reg_bit(doss, tve0_ctrl, rst, addr);
#endif

#ifdef DEF_GET_DOSS_TVE1_CTRL_SLAVE
def_get_mod_reg_bit(doss, tve1_ctrl, slave, addr);
#endif

#ifdef DEF_GET_DOSS_TVE1_CTRL_GATE
def_get_mod_reg_bit(doss, tve1_ctrl, gate, addr);
#endif

#ifdef DEF_GET_DOSS_TVE1_CTRL_RST
def_get_mod_reg_bit(doss, tve1_ctrl, rst, addr);
#endif

#ifdef DEF_GET_DOSS_TVE0_CLK_DIV0
def_get_mod_reg_bit(doss, tve0_clk, div0, addr);
#endif

#ifdef DEF_GET_DOSS_TVE0_CLK_GATE
def_get_mod_reg_bit(doss, tve0_clk, gate, addr);
#endif

#ifdef DEF_GET_DOSS_TVE1_CLK_DIV0
def_get_mod_reg_bit(doss, tve1_clk, div0, addr);
#endif

#ifdef DEF_GET_DOSS_TVE1_CLK_GATE
def_get_mod_reg_bit(doss, tve1_clk, gate, addr);
#endif

#endif /* ___DOSS__GET___H___ */
