/* viss_dump.h */

#ifndef ___VISS___DUMP__H___
#define ___VISS___DUMP__H___

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

u32 dump_viss_ver(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_ftr(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_vic_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_mcsi_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_tvd_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_isp_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_isp_lite_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_isp_pix_sel(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_pll_mux_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_adpll_fac(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_adpll_tune0(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_adpll_tune1(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_adpll_stat(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_mbist_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_axi_rd_qos_lvl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_viss_axi_wt_qos_lvl(u32 addr, u32 data, u32 mode, char *outbuf);

#endif /* ___VISS___DUMP__H___ */
