/* vic_get.h */

#ifndef ___VIC__GET___H___
#define ___VIC__GET___H___

#ifdef DEF_GET_VIC_VER_VER_L
def_get_mod_reg_bit(vic, ver, ver_l, addr);
#endif

#ifdef DEF_GET_VIC_VER_VER_H
def_get_mod_reg_bit(vic, ver, ver_h, addr);
#endif

#ifdef DEF_GET_VIC_VER_COMP
def_get_mod_reg_bit(vic, ver, comp, addr);
#endif

#ifdef DEF_GET_VIC_FTR_HAS_YC_INTL
def_get_mod_reg_bit(vic, ftr, has_yc_intl, addr);
#endif

#ifdef DEF_GET_VIC_FTR_HAS_YC_SP
def_get_mod_reg_bit(vic, ftr, has_yc_sp, addr);
#endif

#ifdef DEF_GET_VIC_FTR_HAS_MULTI_CH
def_get_mod_reg_bit(vic, ftr, has_multi_ch, addr);
#endif

#ifdef DEF_GET_VIC_FTR_HAS_DMA0
def_get_mod_reg_bit(vic, ftr, has_dma0, addr);
#endif

#ifdef DEF_GET_VIC_FTR_HAS_DMA1
def_get_mod_reg_bit(vic, ftr, has_dma1, addr);
#endif

#ifdef DEF_GET_VIC_FTR_HAS_DMA2
def_get_mod_reg_bit(vic, ftr, has_dma2, addr);
#endif

#ifdef DEF_GET_VIC_FTR_HAS_DMA3
def_get_mod_reg_bit(vic, ftr, has_dma3, addr);
#endif

#ifdef DEF_GET_VIC_VIC_CTRL_VIC_EN
def_get_mod_reg_bit(vic, vic_ctrl, vic_en, addr);
#endif

#ifdef DEF_GET_VIC_VIC_CFG_RAW_MSB
def_get_mod_reg_bit(vic, vic_cfg, raw_msb, addr);
#endif

#ifdef DEF_GET_VIC_VIC_CFG_DATA_8BIT
def_get_mod_reg_bit(vic, vic_cfg, data_8bit, addr);
#endif

#ifdef DEF_GET_VIC_VIC_CFG_DVP_PATH
def_get_mod_reg_bit(vic, vic_cfg, dvp_path, addr);
#endif

#ifdef DEF_GET_VIC_VIC_CFG_BUSY
def_get_mod_reg_bit(vic, vic_cfg, busy, addr);
#endif

#ifdef DEF_GET_VIC_IO_CFG_PCLK_INV
def_get_mod_reg_bit(vic, io_cfg, pclk_inv, addr);
#endif

#ifdef DEF_GET_VIC_IO_CFG_FIELD_INV
def_get_mod_reg_bit(vic, io_cfg, field_inv, addr);
#endif

#ifdef DEF_GET_VIC_IO_CFG_HS_INV
def_get_mod_reg_bit(vic, io_cfg, hs_inv, addr);
#endif

#ifdef DEF_GET_VIC_IO_CFG_VS_INV
def_get_mod_reg_bit(vic, io_cfg, vs_inv, addr);
#endif

#ifdef DEF_GET_VIC_IO_CFG_PCLK_PHASE
def_get_mod_reg_bit(vic, io_cfg, pclk_phase, addr);
#endif

#ifdef DEF_GET_VIC_IF_CFG_DATA_PTCL
def_get_mod_reg_bit(vic, if_cfg, data_ptcl, addr);
#endif

#ifdef DEF_GET_VIC_IF_CFG_YC_SEQ
def_get_mod_reg_bit(vic, if_cfg, yc_seq, addr);
#endif

#ifdef DEF_GET_VIC_IF_CFG_YC_SYNC
def_get_mod_reg_bit(vic, if_cfg, yc_sync, addr);
#endif

#ifdef DEF_GET_VIC_IF_CFG_SYNC_PTCL
def_get_mod_reg_bit(vic, if_cfg, sync_ptcl, addr);
#endif

#ifdef DEF_GET_VIC_IF_CFG_FIELD_DET
def_get_mod_reg_bit(vic, if_cfg, field_det, addr);
#endif

#ifdef DEF_GET_VIC_IF_CFG_V_MODE
def_get_mod_reg_bit(vic, if_cfg, v_mode, addr);
#endif

#ifdef DEF_GET_VIC_DMA_MULTI_CH_NUM
def_get_mod_reg_bit(vic, dma_multi_ch, num, addr);
#endif

#ifdef DEF_GET_VIC_DMA_MULTI_CH_EN
def_get_mod_reg_bit(vic, dma_multi_ch, en, addr);
#endif

#ifdef DEF_GET_VIC_DMA_MULTI_ID_CH0_ID
def_get_mod_reg_bit(vic, dma_multi_id, ch0_id, addr);
#endif

#ifdef DEF_GET_VIC_DMA_MULTI_ID_CH1_ID
def_get_mod_reg_bit(vic, dma_multi_id, ch1_id, addr);
#endif

#ifdef DEF_GET_VIC_DMA_MULTI_ID_CH2_ID
def_get_mod_reg_bit(vic, dma_multi_id, ch2_id, addr);
#endif

#ifdef DEF_GET_VIC_DMA_MULTI_ID_CH3_ID
def_get_mod_reg_bit(vic, dma_multi_id, ch3_id, addr);
#endif

#ifdef DEF_GET_VIC_HFB_OFS_HOFFSET
def_get_mod_reg_bit(vic, hfb_ofs, hoffset, addr);
#endif

#ifdef DEF_GET_VIC_HACT_WIDTH_HAW
def_get_mod_reg_bit(vic, hact_width, haw, addr);
#endif

#ifdef DEF_GET_VIC_F1_VFB_OFS_F1_VOFFSET
def_get_mod_reg_bit(vic, f1_vfb_ofs, f1_voffset, addr);
#endif

#ifdef DEF_GET_VIC_F1_VACT_LINE_F1_VACT_LINE
def_get_mod_reg_bit(vic, f1_vact_line, f1_vact_line, addr);
#endif

#ifdef DEF_GET_VIC_F2_VFB_OFS_F2_VOFFSET
def_get_mod_reg_bit(vic, f2_vfb_ofs, f2_voffset, addr);
#endif

#ifdef DEF_GET_VIC_F2_VACT_LINE_F2_VACT_LINE
def_get_mod_reg_bit(vic, f2_vact_line, f2_vact_line, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_OFS_XS
def_get_mod_reg_bit(vic, dma0_ofs, xs, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_OFS_YS
def_get_mod_reg_bit(vic, dma0_ofs, ys, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_OFS_XS
def_get_mod_reg_bit(vic, dma1_ofs, xs, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_OFS_YS
def_get_mod_reg_bit(vic, dma1_ofs, ys, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_OFS_XS
def_get_mod_reg_bit(vic, dma2_ofs, xs, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_OFS_YS
def_get_mod_reg_bit(vic, dma2_ofs, ys, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_OFS_XS
def_get_mod_reg_bit(vic, dma3_ofs, xs, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_OFS_YS
def_get_mod_reg_bit(vic, dma3_ofs, ys, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_SIZE_X
def_get_mod_reg_bit(vic, dma0_size, x, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_SIZE_Y
def_get_mod_reg_bit(vic, dma0_size, y, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_SIZE_X
def_get_mod_reg_bit(vic, dma1_size, x, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_SIZE_Y
def_get_mod_reg_bit(vic, dma1_size, y, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_SIZE_X
def_get_mod_reg_bit(vic, dma2_size, x, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_SIZE_Y
def_get_mod_reg_bit(vic, dma2_size, y, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_SIZE_X
def_get_mod_reg_bit(vic, dma3_size, x, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_SIZE_Y
def_get_mod_reg_bit(vic, dma3_size, y, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_MODE_OUT_FMT
def_get_mod_reg_bit(vic, dma0_mode, out_fmt, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_MODE_BURST
def_get_mod_reg_bit(vic, dma0_mode, burst, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_MODE_FILED_STRG
def_get_mod_reg_bit(vic, dma0_mode, filed_strg, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_MODE_FCI
def_get_mod_reg_bit(vic, dma0_mode, fci, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_MODE_VBI
def_get_mod_reg_bit(vic, dma0_mode, vbi, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_MODE_LCTI
def_get_mod_reg_bit(vic, dma0_mode, lcti, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_MODE_UD
def_get_mod_reg_bit(vic, dma0_mode, ud, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_MODE_CAP
def_get_mod_reg_bit(vic, dma0_mode, cap, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_MODE_OUT_FMT
def_get_mod_reg_bit(vic, dma1_mode, out_fmt, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_MODE_BURST
def_get_mod_reg_bit(vic, dma1_mode, burst, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_MODE_FILED_STRG
def_get_mod_reg_bit(vic, dma1_mode, filed_strg, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_MODE_FCI
def_get_mod_reg_bit(vic, dma1_mode, fci, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_MODE_VBI
def_get_mod_reg_bit(vic, dma1_mode, vbi, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_MODE_LCTI
def_get_mod_reg_bit(vic, dma1_mode, lcti, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_MODE_UD
def_get_mod_reg_bit(vic, dma1_mode, ud, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_MODE_CAP
def_get_mod_reg_bit(vic, dma1_mode, cap, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_MODE_OUT_FMT
def_get_mod_reg_bit(vic, dma2_mode, out_fmt, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_MODE_BURST
def_get_mod_reg_bit(vic, dma2_mode, burst, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_MODE_FILED_STRG
def_get_mod_reg_bit(vic, dma2_mode, filed_strg, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_MODE_FCI
def_get_mod_reg_bit(vic, dma2_mode, fci, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_MODE_VBI
def_get_mod_reg_bit(vic, dma2_mode, vbi, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_MODE_LCTI
def_get_mod_reg_bit(vic, dma2_mode, lcti, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_MODE_UD
def_get_mod_reg_bit(vic, dma2_mode, ud, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_MODE_CAP
def_get_mod_reg_bit(vic, dma2_mode, cap, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_MODE_OUT_FMT
def_get_mod_reg_bit(vic, dma3_mode, out_fmt, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_MODE_BURST
def_get_mod_reg_bit(vic, dma3_mode, burst, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_MODE_FILED_STRG
def_get_mod_reg_bit(vic, dma3_mode, filed_strg, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_MODE_FCI
def_get_mod_reg_bit(vic, dma3_mode, fci, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_MODE_VBI
def_get_mod_reg_bit(vic, dma3_mode, vbi, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_MODE_LCTI
def_get_mod_reg_bit(vic, dma3_mode, lcti, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_MODE_UD
def_get_mod_reg_bit(vic, dma3_mode, ud, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_MODE_CAP
def_get_mod_reg_bit(vic, dma3_mode, cap, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_CTRL_START
def_get_mod_reg_bit(vic, dma0_ctrl, start, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_CTRL_START
def_get_mod_reg_bit(vic, dma1_ctrl, start, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_CTRL_START
def_get_mod_reg_bit(vic, dma2_ctrl, start, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_CTRL_START
def_get_mod_reg_bit(vic, dma3_ctrl, start, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_UPDATE_UD_MAN
def_get_mod_reg_bit(vic, dma0_update, ud_man, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_UPDATE_UD_MAN
def_get_mod_reg_bit(vic, dma1_update, ud_man, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_UPDATE_UD_MAN
def_get_mod_reg_bit(vic, dma2_update, ud_man, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_UPDATE_UD_MAN
def_get_mod_reg_bit(vic, dma3_update, ud_man, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_ADDR0_ADDR
def_get_mod_reg_bit(vic, dma0_addr0, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_ADDR0_ADDR
def_get_mod_reg_bit(vic, dma1_addr0, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_ADDR0_ADDR
def_get_mod_reg_bit(vic, dma2_addr0, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_ADDR0_ADDR
def_get_mod_reg_bit(vic, dma3_addr0, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_ADDR1_ADDR
def_get_mod_reg_bit(vic, dma0_addr1, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_ADDR1_ADDR
def_get_mod_reg_bit(vic, dma1_addr1, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_ADDR1_ADDR
def_get_mod_reg_bit(vic, dma2_addr1, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_ADDR1_ADDR
def_get_mod_reg_bit(vic, dma3_addr1, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_ADDR2_ADDR
def_get_mod_reg_bit(vic, dma0_addr2, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_ADDR2_ADDR
def_get_mod_reg_bit(vic, dma1_addr2, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_ADDR2_ADDR
def_get_mod_reg_bit(vic, dma2_addr2, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_ADDR2_ADDR
def_get_mod_reg_bit(vic, dma3_addr2, addr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_LS_Y
def_get_mod_reg_bit(vic, dma0_ls, y, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_LS_C
def_get_mod_reg_bit(vic, dma0_ls, c, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_LS_Y
def_get_mod_reg_bit(vic, dma1_ls, y, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_LS_C
def_get_mod_reg_bit(vic, dma1_ls, c, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_LS_Y
def_get_mod_reg_bit(vic, dma2_ls, y, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_LS_C
def_get_mod_reg_bit(vic, dma2_ls, c, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_LS_Y
def_get_mod_reg_bit(vic, dma3_ls, y, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_LS_C
def_get_mod_reg_bit(vic, dma3_ls, c, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_BC_BC
def_get_mod_reg_bit(vic, dma0_bc, bc, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_BC_BC
def_get_mod_reg_bit(vic, dma1_bc, bc, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_BC_BC
def_get_mod_reg_bit(vic, dma2_bc, bc, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_BC_BC
def_get_mod_reg_bit(vic, dma3_bc, bc, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_EN_FCI_EN
def_get_mod_reg_bit(vic, dma0_int_en, fci_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_EN_VBI_EN
def_get_mod_reg_bit(vic, dma0_int_en, vbi_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_EN_LCTI_EN
def_get_mod_reg_bit(vic, dma0_int_en, lcti_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_EN_P0_OVF_EN
def_get_mod_reg_bit(vic, dma0_int_en, p0_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_EN_P1_OVF_EN
def_get_mod_reg_bit(vic, dma0_int_en, p1_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_EN_P2_OVF_EN
def_get_mod_reg_bit(vic, dma0_int_en, p2_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_EN_FCI_EN
def_get_mod_reg_bit(vic, dma1_int_en, fci_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_EN_VBI_EN
def_get_mod_reg_bit(vic, dma1_int_en, vbi_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_EN_LCTI_EN
def_get_mod_reg_bit(vic, dma1_int_en, lcti_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_EN_P0_OVF_EN
def_get_mod_reg_bit(vic, dma1_int_en, p0_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_EN_P1_OVF_EN
def_get_mod_reg_bit(vic, dma1_int_en, p1_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_EN_P2_OVF_EN
def_get_mod_reg_bit(vic, dma1_int_en, p2_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_EN_FCI_EN
def_get_mod_reg_bit(vic, dma2_int_en, fci_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_EN_VBI_EN
def_get_mod_reg_bit(vic, dma2_int_en, vbi_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_EN_LCTI_EN
def_get_mod_reg_bit(vic, dma2_int_en, lcti_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_EN_P0_OVF_EN
def_get_mod_reg_bit(vic, dma2_int_en, p0_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_EN_P1_OVF_EN
def_get_mod_reg_bit(vic, dma2_int_en, p1_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_EN_P2_OVF_EN
def_get_mod_reg_bit(vic, dma2_int_en, p2_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_EN_FCI_EN
def_get_mod_reg_bit(vic, dma3_int_en, fci_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_EN_VBI_EN
def_get_mod_reg_bit(vic, dma3_int_en, vbi_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_EN_LCTI_EN
def_get_mod_reg_bit(vic, dma3_int_en, lcti_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_EN_P0_OVF_EN
def_get_mod_reg_bit(vic, dma3_int_en, p0_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_EN_P1_OVF_EN
def_get_mod_reg_bit(vic, dma3_int_en, p1_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_EN_P2_OVF_EN
def_get_mod_reg_bit(vic, dma3_int_en, p2_ovf_en, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_PEND_FCI_PEND
def_get_mod_reg_bit(vic, dma0_int_pend, fci_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_PEND_VBI_PEND
def_get_mod_reg_bit(vic, dma0_int_pend, vbi_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_PEND_LCTI_PEND
def_get_mod_reg_bit(vic, dma0_int_pend, lcti_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_PEND_P0_OVF_PEND
def_get_mod_reg_bit(vic, dma0_int_pend, p0_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_PEND_P1_OVF_PEND
def_get_mod_reg_bit(vic, dma0_int_pend, p1_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_PEND_P2_OVF_PEND
def_get_mod_reg_bit(vic, dma0_int_pend, p2_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_PEND_FCI_PEND
def_get_mod_reg_bit(vic, dma1_int_pend, fci_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_PEND_VBI_PEND
def_get_mod_reg_bit(vic, dma1_int_pend, vbi_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_PEND_LCTI_PEND
def_get_mod_reg_bit(vic, dma1_int_pend, lcti_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_PEND_P0_OVF_PEND
def_get_mod_reg_bit(vic, dma1_int_pend, p0_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_PEND_P1_OVF_PEND
def_get_mod_reg_bit(vic, dma1_int_pend, p1_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_PEND_P2_OVF_PEND
def_get_mod_reg_bit(vic, dma1_int_pend, p2_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_PEND_FCI_PEND
def_get_mod_reg_bit(vic, dma2_int_pend, fci_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_PEND_VBI_PEND
def_get_mod_reg_bit(vic, dma2_int_pend, vbi_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_PEND_LCTI_PEND
def_get_mod_reg_bit(vic, dma2_int_pend, lcti_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_PEND_P0_OVF_PEND
def_get_mod_reg_bit(vic, dma2_int_pend, p0_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_PEND_P1_OVF_PEND
def_get_mod_reg_bit(vic, dma2_int_pend, p1_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_PEND_P2_OVF_PEND
def_get_mod_reg_bit(vic, dma2_int_pend, p2_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_PEND_FCI_PEND
def_get_mod_reg_bit(vic, dma3_int_pend, fci_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_PEND_VBI_PEND
def_get_mod_reg_bit(vic, dma3_int_pend, vbi_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_PEND_LCTI_PEND
def_get_mod_reg_bit(vic, dma3_int_pend, lcti_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_PEND_P0_OVF_PEND
def_get_mod_reg_bit(vic, dma3_int_pend, p0_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_PEND_P1_OVF_PEND
def_get_mod_reg_bit(vic, dma3_int_pend, p1_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_PEND_P2_OVF_PEND
def_get_mod_reg_bit(vic, dma3_int_pend, p2_ovf_pend, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_CLR_FCI_CLR
def_get_mod_reg_bit(vic, dma0_int_clr, fci_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_CLR_VBI_CLR
def_get_mod_reg_bit(vic, dma0_int_clr, vbi_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_CLR_LCTI_CLR
def_get_mod_reg_bit(vic, dma0_int_clr, lcti_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_CLR_P0_OVF_CLR
def_get_mod_reg_bit(vic, dma0_int_clr, p0_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_CLR_P1_OVF_CLR
def_get_mod_reg_bit(vic, dma0_int_clr, p1_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_INT_CLR_P2_OVF_CLR
def_get_mod_reg_bit(vic, dma0_int_clr, p2_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_CLR_FCI_CLR
def_get_mod_reg_bit(vic, dma1_int_clr, fci_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_CLR_VBI_CLR
def_get_mod_reg_bit(vic, dma1_int_clr, vbi_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_CLR_LCTI_CLR
def_get_mod_reg_bit(vic, dma1_int_clr, lcti_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_CLR_P0_OVF_CLR
def_get_mod_reg_bit(vic, dma1_int_clr, p0_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_CLR_P1_OVF_CLR
def_get_mod_reg_bit(vic, dma1_int_clr, p1_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_INT_CLR_P2_OVF_CLR
def_get_mod_reg_bit(vic, dma1_int_clr, p2_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_CLR_FCI_CLR
def_get_mod_reg_bit(vic, dma2_int_clr, fci_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_CLR_VBI_CLR
def_get_mod_reg_bit(vic, dma2_int_clr, vbi_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_CLR_LCTI_CLR
def_get_mod_reg_bit(vic, dma2_int_clr, lcti_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_CLR_P0_OVF_CLR
def_get_mod_reg_bit(vic, dma2_int_clr, p0_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_CLR_P1_OVF_CLR
def_get_mod_reg_bit(vic, dma2_int_clr, p1_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_INT_CLR_P2_OVF_CLR
def_get_mod_reg_bit(vic, dma2_int_clr, p2_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_CLR_FCI_CLR
def_get_mod_reg_bit(vic, dma3_int_clr, fci_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_CLR_VBI_CLR
def_get_mod_reg_bit(vic, dma3_int_clr, vbi_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_CLR_LCTI_CLR
def_get_mod_reg_bit(vic, dma3_int_clr, lcti_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_CLR_P0_OVF_CLR
def_get_mod_reg_bit(vic, dma3_int_clr, p0_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_CLR_P1_OVF_CLR
def_get_mod_reg_bit(vic, dma3_int_clr, p1_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_INT_CLR_P2_OVF_CLR
def_get_mod_reg_bit(vic, dma3_int_clr, p2_ovf_clr, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_LINE_CNT_LS_TRIG
def_get_mod_reg_bit(vic, dma0_line_cnt, ls_trig, addr);
#endif

#ifdef DEF_GET_VIC_DMA0_LINE_CNT_LS_CRNT
def_get_mod_reg_bit(vic, dma0_line_cnt, ls_crnt, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_LINE_CNT_LS_TRIG
def_get_mod_reg_bit(vic, dma1_line_cnt, ls_trig, addr);
#endif

#ifdef DEF_GET_VIC_DMA1_LINE_CNT_LS_CRNT
def_get_mod_reg_bit(vic, dma1_line_cnt, ls_crnt, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_LINE_CNT_LS_TRIG
def_get_mod_reg_bit(vic, dma2_line_cnt, ls_trig, addr);
#endif

#ifdef DEF_GET_VIC_DMA2_LINE_CNT_LS_CRNT
def_get_mod_reg_bit(vic, dma2_line_cnt, ls_crnt, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_LINE_CNT_LS_TRIG
def_get_mod_reg_bit(vic, dma3_line_cnt, ls_trig, addr);
#endif

#ifdef DEF_GET_VIC_DMA3_LINE_CNT_LS_CRNT
def_get_mod_reg_bit(vic, dma3_line_cnt, ls_crnt, addr);
#endif

#endif /* ___VIC__GET___H___ */
