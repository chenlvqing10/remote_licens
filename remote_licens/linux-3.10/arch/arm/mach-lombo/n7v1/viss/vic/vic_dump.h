/* vic_dump.h */

#ifndef ___VIC___DUMP__H___
#define ___VIC___DUMP__H___

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

u32 dump_vic_ver(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_ftr(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_vic_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_vic_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_io_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_if_cfg(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma_multi_ch(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma_multi_id(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_hfb_ofs(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_hact_width(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_f1_vfb_ofs(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_f1_vact_line(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_f2_vfb_ofs(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_f2_vact_line(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_ofs(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_ofs(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_ofs(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_ofs(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_size(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_size(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_size(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_size(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_mode(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_mode(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_mode(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_mode(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_ctrl(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_update(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_update(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_update(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_update(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_addr0(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_addr0(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_addr0(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_addr0(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_addr1(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_addr1(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_addr1(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_addr1(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_addr2(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_addr2(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_addr2(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_addr2(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_ls(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_ls(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_ls(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_ls(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_bc(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_bc(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_bc(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_bc(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_int_en(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_int_en(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_int_en(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_int_en(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_int_pend(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_int_pend(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_int_pend(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_int_pend(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_int_clr(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_int_clr(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_int_clr(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_int_clr(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma0_line_cnt(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma1_line_cnt(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma2_line_cnt(u32 addr, u32 data, u32 mode, char *outbuf);

u32 dump_vic_dma3_line_cnt(u32 addr, u32 data, u32 mode, char *outbuf);

#endif /* ___VIC___DUMP__H___ */
