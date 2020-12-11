/* doss_dump.h */

#ifndef ___DOSS___DUMP__H___
#define ___DOSS___DUMP__H___

#ifndef TMP_STR_LEN
#define TMP_STR_LEN  256
#endif

#ifndef REG_INFO_DEF
#define REG_INFO_DEF

typedef u32 (*pfn_dump)(u32 addr, u32 data, u32 mode, char *buffer);

typedef struct tag_reg_info {
	u32      addr;           /*address         */
	u32      reset;          /*reset value     */
	pfn_dump dump;           /*reg dump func   */
	char     name[28];       /*reg name        */
	u32      res;            /*0               */
} reg_info_t;

#endif /* REG_INFO_DEF */

u32 dump_doss_ver(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_do0_ftr(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_do1_ftr(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_reset(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_route(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_tcon0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_tcon1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_tcon0_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_tcon1_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_lvds0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_lvds1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_lvds0_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_lvds1_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_mipi_dsi0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_mipi_dsi1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_mipi_dsi0_ref_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_mipi_dsi1_ref_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_mipi_dsi0_cfg_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_mipi_dsi1_cfg_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_tve0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_tve1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_tve0_clk(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_doss_tve1_clk(u32 addr, u32 data, u32 mode, char *outbuf);

#endif /* ___DOSS___DUMP__H___ */
