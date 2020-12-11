/* vic_set.c */

#ifndef ___VIC__SET___C___
#define ___VIC__SET___C___

#ifdef DEF_SET_VIC_VER
void set_vic_ver(u32 reg_addr,
		u32 ver_l,
		u32 ver_h,
		u32 comp,
		u32 m_or_r)
{
	reg_vic_ver_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ver_l = ver_l;
	reg.bits.ver_h = ver_h;
	reg.bits.comp = comp;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_VER_VER_L
fun_set_mod_reg_bit(vic, ver, ver_l, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_VER_VER_H
fun_set_mod_reg_bit(vic, ver, ver_h, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_VER_COMP
fun_set_mod_reg_bit(vic, ver, comp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_FTR
void set_vic_ftr(u32 reg_addr,
		u32 has_yc_intl,
		u32 has_yc_sp,
		u32 has_multi_ch,
		u32 has_dma0,
		u32 has_dma1,
		u32 has_dma2,
		u32 has_dma3,
		u32 m_or_r)
{
	reg_vic_ftr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.has_yc_intl = has_yc_intl;
	reg.bits.has_yc_sp = has_yc_sp;
	reg.bits.has_multi_ch = has_multi_ch;
	reg.bits.has_dma0 = has_dma0;
	reg.bits.has_dma1 = has_dma1;
	reg.bits.has_dma2 = has_dma2;
	reg.bits.has_dma3 = has_dma3;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_FTR_HAS_YC_INTL
fun_set_mod_reg_bit(vic, ftr, has_yc_intl, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_FTR_HAS_YC_SP
fun_set_mod_reg_bit(vic, ftr, has_yc_sp, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_FTR_HAS_MULTI_CH
fun_set_mod_reg_bit(vic, ftr, has_multi_ch, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_FTR_HAS_DMA0
fun_set_mod_reg_bit(vic, ftr, has_dma0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_FTR_HAS_DMA1
fun_set_mod_reg_bit(vic, ftr, has_dma1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_FTR_HAS_DMA2
fun_set_mod_reg_bit(vic, ftr, has_dma2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_FTR_HAS_DMA3
fun_set_mod_reg_bit(vic, ftr, has_dma3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_VIC_CTRL
void set_vic_vic_ctrl(u32 reg_addr,
			u32 vic_en,
			u32 m_or_r)
{
	reg_vic_vic_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.vic_en = vic_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_VIC_CTRL_VIC_EN
fun_set_mod_reg_bit(vic, vic_ctrl, vic_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_VIC_CFG
void set_vic_vic_cfg(u32 reg_addr,
			u32 raw_msb,
			u32 data_8bit,
			u32 dvp_path,
			u32 busy,
			u32 m_or_r)
{
	reg_vic_vic_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.raw_msb = raw_msb;
	reg.bits.data_8bit = data_8bit;
	reg.bits.dvp_path = dvp_path;
	reg.bits.busy = busy;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_VIC_CFG_RAW_MSB
fun_set_mod_reg_bit(vic, vic_cfg, raw_msb, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_VIC_CFG_DATA_8BIT
fun_set_mod_reg_bit(vic, vic_cfg, data_8bit, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_VIC_CFG_DVP_PATH
fun_set_mod_reg_bit(vic, vic_cfg, dvp_path, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_VIC_CFG_BUSY
fun_set_mod_reg_bit(vic, vic_cfg, busy, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IO_CFG
void set_vic_io_cfg(u32 reg_addr,
			u32 pclk_inv,
			u32 field_inv,
			u32 hs_inv,
			u32 vs_inv,
			u32 pclk_phase,
			u32 m_or_r)
{
	reg_vic_io_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pclk_inv = pclk_inv;
	reg.bits.field_inv = field_inv;
	reg.bits.hs_inv = hs_inv;
	reg.bits.vs_inv = vs_inv;
	reg.bits.pclk_phase = pclk_phase;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_IO_CFG_PCLK_INV
fun_set_mod_reg_bit(vic, io_cfg, pclk_inv, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IO_CFG_FIELD_INV
fun_set_mod_reg_bit(vic, io_cfg, field_inv, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IO_CFG_HS_INV
fun_set_mod_reg_bit(vic, io_cfg, hs_inv, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IO_CFG_VS_INV
fun_set_mod_reg_bit(vic, io_cfg, vs_inv, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IO_CFG_PCLK_PHASE
fun_set_mod_reg_bit(vic, io_cfg, pclk_phase, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IF_CFG
void set_vic_if_cfg(u32 reg_addr,
			u32 data_ptcl,
			u32 yc_seq,
			u32 yc_sync,
			u32 sync_ptcl,
			u32 field_det,
			u32 v_mode,
			u32 m_or_r)
{
	reg_vic_if_cfg_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.data_ptcl = data_ptcl;
	reg.bits.yc_seq = yc_seq;
	reg.bits.yc_sync = yc_sync;
	reg.bits.sync_ptcl = sync_ptcl;
	reg.bits.field_det = field_det;
	reg.bits.v_mode = v_mode;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_IF_CFG_DATA_PTCL
fun_set_mod_reg_bit(vic, if_cfg, data_ptcl, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IF_CFG_YC_SEQ
fun_set_mod_reg_bit(vic, if_cfg, yc_seq, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IF_CFG_YC_SYNC
fun_set_mod_reg_bit(vic, if_cfg, yc_sync, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IF_CFG_SYNC_PTCL
fun_set_mod_reg_bit(vic, if_cfg, sync_ptcl, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IF_CFG_FIELD_DET
fun_set_mod_reg_bit(vic, if_cfg, field_det, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_IF_CFG_V_MODE
fun_set_mod_reg_bit(vic, if_cfg, v_mode, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA_MULTI_CH
void set_vic_dma_multi_ch(u32 reg_addr,
			u32 num,
			u32 en,
			u32 m_or_r)
{
	reg_vic_dma_multi_ch_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.num = num;
	reg.bits.en = en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA_MULTI_CH_NUM
fun_set_mod_reg_bit(vic, dma_multi_ch, num, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA_MULTI_CH_EN
fun_set_mod_reg_bit(vic, dma_multi_ch, en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA_MULTI_ID
void set_vic_dma_multi_id(u32 reg_addr,
			u32 ch0_id,
			u32 ch1_id,
			u32 ch2_id,
			u32 ch3_id,
			u32 m_or_r)
{
	reg_vic_dma_multi_id_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ch0_id = ch0_id;
	reg.bits.ch1_id = ch1_id;
	reg.bits.ch2_id = ch2_id;
	reg.bits.ch3_id = ch3_id;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA_MULTI_ID_CH0_ID
fun_set_mod_reg_bit(vic, dma_multi_id, ch0_id, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA_MULTI_ID_CH1_ID
fun_set_mod_reg_bit(vic, dma_multi_id, ch1_id, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA_MULTI_ID_CH2_ID
fun_set_mod_reg_bit(vic, dma_multi_id, ch2_id, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA_MULTI_ID_CH3_ID
fun_set_mod_reg_bit(vic, dma_multi_id, ch3_id, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_HFB_OFS
void set_vic_hfb_ofs(u32 reg_addr,
			u32 hoffset,
			u32 m_or_r)
{
	reg_vic_hfb_ofs_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.hoffset = hoffset;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_HFB_OFS_HOFFSET
fun_set_mod_reg_bit(vic, hfb_ofs, hoffset, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_HACT_WIDTH
void set_vic_hact_width(u32 reg_addr,
			u32 haw,
			u32 m_or_r)
{
	reg_vic_hact_width_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.haw = haw;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_HACT_WIDTH_HAW
fun_set_mod_reg_bit(vic, hact_width, haw, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_F1_VFB_OFS
void set_vic_f1_vfb_ofs(u32 reg_addr,
			u32 f1_voffset,
			u32 m_or_r)
{
	reg_vic_f1_vfb_ofs_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.f1_voffset = f1_voffset;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_F1_VFB_OFS_F1_VOFFSET
fun_set_mod_reg_bit(vic, f1_vfb_ofs, f1_voffset, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_F1_VACT_LINE
void set_vic_f1_vact_line(u32 reg_addr,
			u32 f1_vact_line,
			u32 m_or_r)
{
	reg_vic_f1_vact_line_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.f1_vact_line = f1_vact_line;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_F1_VACT_LINE_F1_VACT_LINE
fun_set_mod_reg_bit(vic, f1_vact_line, f1_vact_line, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_F2_VFB_OFS
void set_vic_f2_vfb_ofs(u32 reg_addr,
			u32 f2_voffset,
			u32 m_or_r)
{
	reg_vic_f2_vfb_ofs_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.f2_voffset = f2_voffset;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_F2_VFB_OFS_F2_VOFFSET
fun_set_mod_reg_bit(vic, f2_vfb_ofs, f2_voffset, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_F2_VACT_LINE
void set_vic_f2_vact_line(u32 reg_addr,
			u32 f2_vact_line,
			u32 m_or_r)
{
	reg_vic_f2_vact_line_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.f2_vact_line = f2_vact_line;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_F2_VACT_LINE_F2_VACT_LINE
fun_set_mod_reg_bit(vic, f2_vact_line, f2_vact_line, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_OFS
void set_vic_dma0_ofs(u32 reg_addr,
			u32 xs,
			u32 ys,
			u32 m_or_r)
{
	reg_vic_dma0_ofs_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.xs = xs;
	reg.bits.ys = ys;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_OFS_XS
fun_set_mod_reg_bit(vic, dma0_ofs, xs, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_OFS_YS
fun_set_mod_reg_bit(vic, dma0_ofs, ys, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_OFS
void set_vic_dma1_ofs(u32 reg_addr,
			u32 xs,
			u32 ys,
			u32 m_or_r)
{
	reg_vic_dma1_ofs_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.xs = xs;
	reg.bits.ys = ys;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_OFS_XS
fun_set_mod_reg_bit(vic, dma1_ofs, xs, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_OFS_YS
fun_set_mod_reg_bit(vic, dma1_ofs, ys, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_OFS
void set_vic_dma2_ofs(u32 reg_addr,
			u32 xs,
			u32 ys,
			u32 m_or_r)
{
	reg_vic_dma2_ofs_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.xs = xs;
	reg.bits.ys = ys;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_OFS_XS
fun_set_mod_reg_bit(vic, dma2_ofs, xs, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_OFS_YS
fun_set_mod_reg_bit(vic, dma2_ofs, ys, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_OFS
void set_vic_dma3_ofs(u32 reg_addr,
			u32 xs,
			u32 ys,
			u32 m_or_r)
{
	reg_vic_dma3_ofs_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.xs = xs;
	reg.bits.ys = ys;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_OFS_XS
fun_set_mod_reg_bit(vic, dma3_ofs, xs, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_OFS_YS
fun_set_mod_reg_bit(vic, dma3_ofs, ys, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_SIZE
void set_vic_dma0_size(u32 reg_addr,
			u32 x,
			u32 y,
			u32 m_or_r)
{
	reg_vic_dma0_size_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.x = x;
	reg.bits.y = y;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_SIZE_X
fun_set_mod_reg_bit(vic, dma0_size, x, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_SIZE_Y
fun_set_mod_reg_bit(vic, dma0_size, y, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_SIZE
void set_vic_dma1_size(u32 reg_addr,
			u32 x,
			u32 y,
			u32 m_or_r)
{
	reg_vic_dma1_size_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.x = x;
	reg.bits.y = y;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_SIZE_X
fun_set_mod_reg_bit(vic, dma1_size, x, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_SIZE_Y
fun_set_mod_reg_bit(vic, dma1_size, y, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_SIZE
void set_vic_dma2_size(u32 reg_addr,
			u32 x,
			u32 y,
			u32 m_or_r)
{
	reg_vic_dma2_size_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.x = x;
	reg.bits.y = y;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_SIZE_X
fun_set_mod_reg_bit(vic, dma2_size, x, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_SIZE_Y
fun_set_mod_reg_bit(vic, dma2_size, y, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_SIZE
void set_vic_dma3_size(u32 reg_addr,
			u32 x,
			u32 y,
			u32 m_or_r)
{
	reg_vic_dma3_size_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.x = x;
	reg.bits.y = y;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_SIZE_X
fun_set_mod_reg_bit(vic, dma3_size, x, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_SIZE_Y
fun_set_mod_reg_bit(vic, dma3_size, y, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_MODE
void set_vic_dma0_mode(u32 reg_addr,
			u32 out_fmt,
			u32 burst,
			u32 filed_strg,
			u32 fci,
			u32 vbi,
			u32 lcti,
			u32 ud,
			u32 cap,
			u32 m_or_r)
{
	reg_vic_dma0_mode_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.out_fmt = out_fmt;
	reg.bits.burst = burst;
	reg.bits.filed_strg = filed_strg;
	reg.bits.fci = fci;
	reg.bits.vbi = vbi;
	reg.bits.lcti = lcti;
	reg.bits.ud = ud;
	reg.bits.cap = cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_MODE_OUT_FMT
fun_set_mod_reg_bit(vic, dma0_mode, out_fmt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_MODE_BURST
fun_set_mod_reg_bit(vic, dma0_mode, burst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_MODE_FILED_STRG
fun_set_mod_reg_bit(vic, dma0_mode, filed_strg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_MODE_FCI
fun_set_mod_reg_bit(vic, dma0_mode, fci, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_MODE_VBI
fun_set_mod_reg_bit(vic, dma0_mode, vbi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_MODE_LCTI
fun_set_mod_reg_bit(vic, dma0_mode, lcti, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_MODE_UD
fun_set_mod_reg_bit(vic, dma0_mode, ud, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_MODE_CAP
fun_set_mod_reg_bit(vic, dma0_mode, cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_MODE
void set_vic_dma1_mode(u32 reg_addr,
			u32 out_fmt,
			u32 burst,
			u32 filed_strg,
			u32 fci,
			u32 vbi,
			u32 lcti,
			u32 ud,
			u32 cap,
			u32 m_or_r)
{
	reg_vic_dma1_mode_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.out_fmt = out_fmt;
	reg.bits.burst = burst;
	reg.bits.filed_strg = filed_strg;
	reg.bits.fci = fci;
	reg.bits.vbi = vbi;
	reg.bits.lcti = lcti;
	reg.bits.ud = ud;
	reg.bits.cap = cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_MODE_OUT_FMT
fun_set_mod_reg_bit(vic, dma1_mode, out_fmt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_MODE_BURST
fun_set_mod_reg_bit(vic, dma1_mode, burst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_MODE_FILED_STRG
fun_set_mod_reg_bit(vic, dma1_mode, filed_strg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_MODE_FCI
fun_set_mod_reg_bit(vic, dma1_mode, fci, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_MODE_VBI
fun_set_mod_reg_bit(vic, dma1_mode, vbi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_MODE_LCTI
fun_set_mod_reg_bit(vic, dma1_mode, lcti, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_MODE_UD
fun_set_mod_reg_bit(vic, dma1_mode, ud, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_MODE_CAP
fun_set_mod_reg_bit(vic, dma1_mode, cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_MODE
void set_vic_dma2_mode(u32 reg_addr,
			u32 out_fmt,
			u32 burst,
			u32 filed_strg,
			u32 fci,
			u32 vbi,
			u32 lcti,
			u32 ud,
			u32 cap,
			u32 m_or_r)
{
	reg_vic_dma2_mode_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.out_fmt = out_fmt;
	reg.bits.burst = burst;
	reg.bits.filed_strg = filed_strg;
	reg.bits.fci = fci;
	reg.bits.vbi = vbi;
	reg.bits.lcti = lcti;
	reg.bits.ud = ud;
	reg.bits.cap = cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_MODE_OUT_FMT
fun_set_mod_reg_bit(vic, dma2_mode, out_fmt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_MODE_BURST
fun_set_mod_reg_bit(vic, dma2_mode, burst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_MODE_FILED_STRG
fun_set_mod_reg_bit(vic, dma2_mode, filed_strg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_MODE_FCI
fun_set_mod_reg_bit(vic, dma2_mode, fci, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_MODE_VBI
fun_set_mod_reg_bit(vic, dma2_mode, vbi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_MODE_LCTI
fun_set_mod_reg_bit(vic, dma2_mode, lcti, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_MODE_UD
fun_set_mod_reg_bit(vic, dma2_mode, ud, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_MODE_CAP
fun_set_mod_reg_bit(vic, dma2_mode, cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_MODE
void set_vic_dma3_mode(u32 reg_addr,
			u32 out_fmt,
			u32 burst,
			u32 filed_strg,
			u32 fci,
			u32 vbi,
			u32 lcti,
			u32 ud,
			u32 cap,
			u32 m_or_r)
{
	reg_vic_dma3_mode_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.out_fmt = out_fmt;
	reg.bits.burst = burst;
	reg.bits.filed_strg = filed_strg;
	reg.bits.fci = fci;
	reg.bits.vbi = vbi;
	reg.bits.lcti = lcti;
	reg.bits.ud = ud;
	reg.bits.cap = cap;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_MODE_OUT_FMT
fun_set_mod_reg_bit(vic, dma3_mode, out_fmt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_MODE_BURST
fun_set_mod_reg_bit(vic, dma3_mode, burst, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_MODE_FILED_STRG
fun_set_mod_reg_bit(vic, dma3_mode, filed_strg, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_MODE_FCI
fun_set_mod_reg_bit(vic, dma3_mode, fci, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_MODE_VBI
fun_set_mod_reg_bit(vic, dma3_mode, vbi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_MODE_LCTI
fun_set_mod_reg_bit(vic, dma3_mode, lcti, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_MODE_UD
fun_set_mod_reg_bit(vic, dma3_mode, ud, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_MODE_CAP
fun_set_mod_reg_bit(vic, dma3_mode, cap, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_CTRL
void set_vic_dma0_ctrl(u32 reg_addr,
			u32 start,
			u32 m_or_r)
{
	reg_vic_dma0_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.start = start;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_CTRL_START
fun_set_mod_reg_bit(vic, dma0_ctrl, start, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_CTRL
void set_vic_dma1_ctrl(u32 reg_addr,
			u32 start,
			u32 m_or_r)
{
	reg_vic_dma1_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.start = start;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_CTRL_START
fun_set_mod_reg_bit(vic, dma1_ctrl, start, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_CTRL
void set_vic_dma2_ctrl(u32 reg_addr,
			u32 start,
			u32 m_or_r)
{
	reg_vic_dma2_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.start = start;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_CTRL_START
fun_set_mod_reg_bit(vic, dma2_ctrl, start, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_CTRL
void set_vic_dma3_ctrl(u32 reg_addr,
			u32 start,
			u32 m_or_r)
{
	reg_vic_dma3_ctrl_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.start = start;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_CTRL_START
fun_set_mod_reg_bit(vic, dma3_ctrl, start, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_UPDATE
void set_vic_dma0_update(u32 reg_addr,
			u32 ud_man,
			u32 m_or_r)
{
	reg_vic_dma0_update_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ud_man = ud_man;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_UPDATE_UD_MAN
fun_set_mod_reg_bit(vic, dma0_update, ud_man, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_UPDATE
void set_vic_dma1_update(u32 reg_addr,
			u32 ud_man,
			u32 m_or_r)
{
	reg_vic_dma1_update_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ud_man = ud_man;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_UPDATE_UD_MAN
fun_set_mod_reg_bit(vic, dma1_update, ud_man, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_UPDATE
void set_vic_dma2_update(u32 reg_addr,
			u32 ud_man,
			u32 m_or_r)
{
	reg_vic_dma2_update_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ud_man = ud_man;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_UPDATE_UD_MAN
fun_set_mod_reg_bit(vic, dma2_update, ud_man, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_UPDATE
void set_vic_dma3_update(u32 reg_addr,
			u32 ud_man,
			u32 m_or_r)
{
	reg_vic_dma3_update_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ud_man = ud_man;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_UPDATE_UD_MAN
fun_set_mod_reg_bit(vic, dma3_update, ud_man, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_ADDR0
void set_vic_dma0_addr0(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma0_addr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_ADDR0_ADDR
fun_set_mod_reg_bit(vic, dma0_addr0, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_ADDR0
void set_vic_dma1_addr0(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma1_addr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_ADDR0_ADDR
fun_set_mod_reg_bit(vic, dma1_addr0, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_ADDR0
void set_vic_dma2_addr0(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma2_addr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_ADDR0_ADDR
fun_set_mod_reg_bit(vic, dma2_addr0, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_ADDR0
void set_vic_dma3_addr0(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma3_addr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_ADDR0_ADDR
fun_set_mod_reg_bit(vic, dma3_addr0, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_ADDR1
void set_vic_dma0_addr1(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma0_addr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_ADDR1_ADDR
fun_set_mod_reg_bit(vic, dma0_addr1, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_ADDR1
void set_vic_dma1_addr1(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma1_addr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_ADDR1_ADDR
fun_set_mod_reg_bit(vic, dma1_addr1, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_ADDR1
void set_vic_dma2_addr1(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma2_addr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_ADDR1_ADDR
fun_set_mod_reg_bit(vic, dma2_addr1, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_ADDR1
void set_vic_dma3_addr1(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma3_addr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_ADDR1_ADDR
fun_set_mod_reg_bit(vic, dma3_addr1, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_ADDR2
void set_vic_dma0_addr2(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma0_addr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_ADDR2_ADDR
fun_set_mod_reg_bit(vic, dma0_addr2, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_ADDR2
void set_vic_dma1_addr2(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma1_addr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_ADDR2_ADDR
fun_set_mod_reg_bit(vic, dma1_addr2, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_ADDR2
void set_vic_dma2_addr2(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma2_addr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_ADDR2_ADDR
fun_set_mod_reg_bit(vic, dma2_addr2, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_ADDR2
void set_vic_dma3_addr2(u32 reg_addr,
			u32 addr,
			u32 m_or_r)
{
	reg_vic_dma3_addr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.addr = addr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_ADDR2_ADDR
fun_set_mod_reg_bit(vic, dma3_addr2, addr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_LS
void set_vic_dma0_ls(u32 reg_addr,
			u32 y,
			u32 c,
			u32 m_or_r)
{
	reg_vic_dma0_ls_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.y = y;
	reg.bits.c = c;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_LS_Y
fun_set_mod_reg_bit(vic, dma0_ls, y, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_LS_C
fun_set_mod_reg_bit(vic, dma0_ls, c, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_LS
void set_vic_dma1_ls(u32 reg_addr,
			u32 y,
			u32 c,
			u32 m_or_r)
{
	reg_vic_dma1_ls_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.y = y;
	reg.bits.c = c;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_LS_Y
fun_set_mod_reg_bit(vic, dma1_ls, y, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_LS_C
fun_set_mod_reg_bit(vic, dma1_ls, c, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_LS
void set_vic_dma2_ls(u32 reg_addr,
			u32 y,
			u32 c,
			u32 m_or_r)
{
	reg_vic_dma2_ls_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.y = y;
	reg.bits.c = c;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_LS_Y
fun_set_mod_reg_bit(vic, dma2_ls, y, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_LS_C
fun_set_mod_reg_bit(vic, dma2_ls, c, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_LS
void set_vic_dma3_ls(u32 reg_addr,
			u32 y,
			u32 c,
			u32 m_or_r)
{
	reg_vic_dma3_ls_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.y = y;
	reg.bits.c = c;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_LS_Y
fun_set_mod_reg_bit(vic, dma3_ls, y, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_LS_C
fun_set_mod_reg_bit(vic, dma3_ls, c, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_BC
void set_vic_dma0_bc(u32 reg_addr,
			u32 bc,
			u32 m_or_r)
{
	reg_vic_dma0_bc_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.bc = bc;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_BC_BC
fun_set_mod_reg_bit(vic, dma0_bc, bc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_BC
void set_vic_dma1_bc(u32 reg_addr,
			u32 bc,
			u32 m_or_r)
{
	reg_vic_dma1_bc_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.bc = bc;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_BC_BC
fun_set_mod_reg_bit(vic, dma1_bc, bc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_BC
void set_vic_dma2_bc(u32 reg_addr,
			u32 bc,
			u32 m_or_r)
{
	reg_vic_dma2_bc_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.bc = bc;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_BC_BC
fun_set_mod_reg_bit(vic, dma2_bc, bc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_BC
void set_vic_dma3_bc(u32 reg_addr,
			u32 bc,
			u32 m_or_r)
{
	reg_vic_dma3_bc_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.bc = bc;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_BC_BC
fun_set_mod_reg_bit(vic, dma3_bc, bc, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_EN
void set_vic_dma0_int_en(u32 reg_addr,
			u32 fci_en,
			u32 vbi_en,
			u32 lcti_en,
			u32 p0_ovf_en,
			u32 p1_ovf_en,
			u32 p2_ovf_en,
			u32 m_or_r)
{
	reg_vic_dma0_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_en = fci_en;
	reg.bits.vbi_en = vbi_en;
	reg.bits.lcti_en = lcti_en;
	reg.bits.p0_ovf_en = p0_ovf_en;
	reg.bits.p1_ovf_en = p1_ovf_en;
	reg.bits.p2_ovf_en = p2_ovf_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_INT_EN_FCI_EN
fun_set_mod_reg_bit(vic, dma0_int_en, fci_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_EN_VBI_EN
fun_set_mod_reg_bit(vic, dma0_int_en, vbi_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_EN_LCTI_EN
fun_set_mod_reg_bit(vic, dma0_int_en, lcti_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_EN_P0_OVF_EN
fun_set_mod_reg_bit(vic, dma0_int_en, p0_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_EN_P1_OVF_EN
fun_set_mod_reg_bit(vic, dma0_int_en, p1_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_EN_P2_OVF_EN
fun_set_mod_reg_bit(vic, dma0_int_en, p2_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_EN
void set_vic_dma1_int_en(u32 reg_addr,
			u32 fci_en,
			u32 vbi_en,
			u32 lcti_en,
			u32 p0_ovf_en,
			u32 p1_ovf_en,
			u32 p2_ovf_en,
			u32 m_or_r)
{
	reg_vic_dma1_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_en = fci_en;
	reg.bits.vbi_en = vbi_en;
	reg.bits.lcti_en = lcti_en;
	reg.bits.p0_ovf_en = p0_ovf_en;
	reg.bits.p1_ovf_en = p1_ovf_en;
	reg.bits.p2_ovf_en = p2_ovf_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_INT_EN_FCI_EN
fun_set_mod_reg_bit(vic, dma1_int_en, fci_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_EN_VBI_EN
fun_set_mod_reg_bit(vic, dma1_int_en, vbi_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_EN_LCTI_EN
fun_set_mod_reg_bit(vic, dma1_int_en, lcti_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_EN_P0_OVF_EN
fun_set_mod_reg_bit(vic, dma1_int_en, p0_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_EN_P1_OVF_EN
fun_set_mod_reg_bit(vic, dma1_int_en, p1_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_EN_P2_OVF_EN
fun_set_mod_reg_bit(vic, dma1_int_en, p2_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_EN
void set_vic_dma2_int_en(u32 reg_addr,
			u32 fci_en,
			u32 vbi_en,
			u32 lcti_en,
			u32 p0_ovf_en,
			u32 p1_ovf_en,
			u32 p2_ovf_en,
			u32 m_or_r)
{
	reg_vic_dma2_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_en = fci_en;
	reg.bits.vbi_en = vbi_en;
	reg.bits.lcti_en = lcti_en;
	reg.bits.p0_ovf_en = p0_ovf_en;
	reg.bits.p1_ovf_en = p1_ovf_en;
	reg.bits.p2_ovf_en = p2_ovf_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_INT_EN_FCI_EN
fun_set_mod_reg_bit(vic, dma2_int_en, fci_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_EN_VBI_EN
fun_set_mod_reg_bit(vic, dma2_int_en, vbi_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_EN_LCTI_EN
fun_set_mod_reg_bit(vic, dma2_int_en, lcti_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_EN_P0_OVF_EN
fun_set_mod_reg_bit(vic, dma2_int_en, p0_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_EN_P1_OVF_EN
fun_set_mod_reg_bit(vic, dma2_int_en, p1_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_EN_P2_OVF_EN
fun_set_mod_reg_bit(vic, dma2_int_en, p2_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_EN
void set_vic_dma3_int_en(u32 reg_addr,
			u32 fci_en,
			u32 vbi_en,
			u32 lcti_en,
			u32 p0_ovf_en,
			u32 p1_ovf_en,
			u32 p2_ovf_en,
			u32 m_or_r)
{
	reg_vic_dma3_int_en_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_en = fci_en;
	reg.bits.vbi_en = vbi_en;
	reg.bits.lcti_en = lcti_en;
	reg.bits.p0_ovf_en = p0_ovf_en;
	reg.bits.p1_ovf_en = p1_ovf_en;
	reg.bits.p2_ovf_en = p2_ovf_en;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_INT_EN_FCI_EN
fun_set_mod_reg_bit(vic, dma3_int_en, fci_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_EN_VBI_EN
fun_set_mod_reg_bit(vic, dma3_int_en, vbi_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_EN_LCTI_EN
fun_set_mod_reg_bit(vic, dma3_int_en, lcti_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_EN_P0_OVF_EN
fun_set_mod_reg_bit(vic, dma3_int_en, p0_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_EN_P1_OVF_EN
fun_set_mod_reg_bit(vic, dma3_int_en, p1_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_EN_P2_OVF_EN
fun_set_mod_reg_bit(vic, dma3_int_en, p2_ovf_en, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_PEND
void set_vic_dma0_int_pend(u32 reg_addr,
			u32 fci_pend,
			u32 vbi_pend,
			u32 lcti_pend,
			u32 p0_ovf_pend,
			u32 p1_ovf_pend,
			u32 p2_ovf_pend,
			u32 m_or_r)
{
	reg_vic_dma0_int_pend_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_pend = fci_pend;
	reg.bits.vbi_pend = vbi_pend;
	reg.bits.lcti_pend = lcti_pend;
	reg.bits.p0_ovf_pend = p0_ovf_pend;
	reg.bits.p1_ovf_pend = p1_ovf_pend;
	reg.bits.p2_ovf_pend = p2_ovf_pend;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_INT_PEND_FCI_PEND
fun_set_mod_reg_bit(vic, dma0_int_pend, fci_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_PEND_VBI_PEND
fun_set_mod_reg_bit(vic, dma0_int_pend, vbi_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_PEND_LCTI_PEND
fun_set_mod_reg_bit(vic, dma0_int_pend, lcti_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_PEND_P0_OVF_PEND
fun_set_mod_reg_bit(vic, dma0_int_pend, p0_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_PEND_P1_OVF_PEND
fun_set_mod_reg_bit(vic, dma0_int_pend, p1_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_PEND_P2_OVF_PEND
fun_set_mod_reg_bit(vic, dma0_int_pend, p2_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_PEND
void set_vic_dma1_int_pend(u32 reg_addr,
			u32 fci_pend,
			u32 vbi_pend,
			u32 lcti_pend,
			u32 p0_ovf_pend,
			u32 p1_ovf_pend,
			u32 p2_ovf_pend,
			u32 m_or_r)
{
	reg_vic_dma1_int_pend_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_pend = fci_pend;
	reg.bits.vbi_pend = vbi_pend;
	reg.bits.lcti_pend = lcti_pend;
	reg.bits.p0_ovf_pend = p0_ovf_pend;
	reg.bits.p1_ovf_pend = p1_ovf_pend;
	reg.bits.p2_ovf_pend = p2_ovf_pend;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_INT_PEND_FCI_PEND
fun_set_mod_reg_bit(vic, dma1_int_pend, fci_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_PEND_VBI_PEND
fun_set_mod_reg_bit(vic, dma1_int_pend, vbi_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_PEND_LCTI_PEND
fun_set_mod_reg_bit(vic, dma1_int_pend, lcti_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_PEND_P0_OVF_PEND
fun_set_mod_reg_bit(vic, dma1_int_pend, p0_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_PEND_P1_OVF_PEND
fun_set_mod_reg_bit(vic, dma1_int_pend, p1_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_PEND_P2_OVF_PEND
fun_set_mod_reg_bit(vic, dma1_int_pend, p2_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_PEND
void set_vic_dma2_int_pend(u32 reg_addr,
			u32 fci_pend,
			u32 vbi_pend,
			u32 lcti_pend,
			u32 p0_ovf_pend,
			u32 p1_ovf_pend,
			u32 p2_ovf_pend,
			u32 m_or_r)
{
	reg_vic_dma2_int_pend_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_pend = fci_pend;
	reg.bits.vbi_pend = vbi_pend;
	reg.bits.lcti_pend = lcti_pend;
	reg.bits.p0_ovf_pend = p0_ovf_pend;
	reg.bits.p1_ovf_pend = p1_ovf_pend;
	reg.bits.p2_ovf_pend = p2_ovf_pend;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_INT_PEND_FCI_PEND
fun_set_mod_reg_bit(vic, dma2_int_pend, fci_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_PEND_VBI_PEND
fun_set_mod_reg_bit(vic, dma2_int_pend, vbi_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_PEND_LCTI_PEND
fun_set_mod_reg_bit(vic, dma2_int_pend, lcti_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_PEND_P0_OVF_PEND
fun_set_mod_reg_bit(vic, dma2_int_pend, p0_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_PEND_P1_OVF_PEND
fun_set_mod_reg_bit(vic, dma2_int_pend, p1_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_PEND_P2_OVF_PEND
fun_set_mod_reg_bit(vic, dma2_int_pend, p2_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_PEND
void set_vic_dma3_int_pend(u32 reg_addr,
			u32 fci_pend,
			u32 vbi_pend,
			u32 lcti_pend,
			u32 p0_ovf_pend,
			u32 p1_ovf_pend,
			u32 p2_ovf_pend,
			u32 m_or_r)
{
	reg_vic_dma3_int_pend_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_pend = fci_pend;
	reg.bits.vbi_pend = vbi_pend;
	reg.bits.lcti_pend = lcti_pend;
	reg.bits.p0_ovf_pend = p0_ovf_pend;
	reg.bits.p1_ovf_pend = p1_ovf_pend;
	reg.bits.p2_ovf_pend = p2_ovf_pend;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_INT_PEND_FCI_PEND
fun_set_mod_reg_bit(vic, dma3_int_pend, fci_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_PEND_VBI_PEND
fun_set_mod_reg_bit(vic, dma3_int_pend, vbi_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_PEND_LCTI_PEND
fun_set_mod_reg_bit(vic, dma3_int_pend, lcti_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_PEND_P0_OVF_PEND
fun_set_mod_reg_bit(vic, dma3_int_pend, p0_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_PEND_P1_OVF_PEND
fun_set_mod_reg_bit(vic, dma3_int_pend, p1_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_PEND_P2_OVF_PEND
fun_set_mod_reg_bit(vic, dma3_int_pend, p2_ovf_pend, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_CLR
void set_vic_dma0_int_clr(u32 reg_addr,
			u32 fci_clr,
			u32 vbi_clr,
			u32 lcti_clr,
			u32 p0_ovf_clr,
			u32 p1_ovf_clr,
			u32 p2_ovf_clr,
			u32 m_or_r)
{
	reg_vic_dma0_int_clr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_clr = fci_clr;
	reg.bits.vbi_clr = vbi_clr;
	reg.bits.lcti_clr = lcti_clr;
	reg.bits.p0_ovf_clr = p0_ovf_clr;
	reg.bits.p1_ovf_clr = p1_ovf_clr;
	reg.bits.p2_ovf_clr = p2_ovf_clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_INT_CLR_FCI_CLR
fun_set_mod_reg_bit(vic, dma0_int_clr, fci_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_CLR_VBI_CLR
fun_set_mod_reg_bit(vic, dma0_int_clr, vbi_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_CLR_LCTI_CLR
fun_set_mod_reg_bit(vic, dma0_int_clr, lcti_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_CLR_P0_OVF_CLR
fun_set_mod_reg_bit(vic, dma0_int_clr, p0_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_CLR_P1_OVF_CLR
fun_set_mod_reg_bit(vic, dma0_int_clr, p1_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_INT_CLR_P2_OVF_CLR
fun_set_mod_reg_bit(vic, dma0_int_clr, p2_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_CLR
void set_vic_dma1_int_clr(u32 reg_addr,
			u32 fci_clr,
			u32 vbi_clr,
			u32 lcti_clr,
			u32 p0_ovf_clr,
			u32 p1_ovf_clr,
			u32 p2_ovf_clr,
			u32 m_or_r)
{
	reg_vic_dma1_int_clr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_clr = fci_clr;
	reg.bits.vbi_clr = vbi_clr;
	reg.bits.lcti_clr = lcti_clr;
	reg.bits.p0_ovf_clr = p0_ovf_clr;
	reg.bits.p1_ovf_clr = p1_ovf_clr;
	reg.bits.p2_ovf_clr = p2_ovf_clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_INT_CLR_FCI_CLR
fun_set_mod_reg_bit(vic, dma1_int_clr, fci_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_CLR_VBI_CLR
fun_set_mod_reg_bit(vic, dma1_int_clr, vbi_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_CLR_LCTI_CLR
fun_set_mod_reg_bit(vic, dma1_int_clr, lcti_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_CLR_P0_OVF_CLR
fun_set_mod_reg_bit(vic, dma1_int_clr, p0_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_CLR_P1_OVF_CLR
fun_set_mod_reg_bit(vic, dma1_int_clr, p1_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_INT_CLR_P2_OVF_CLR
fun_set_mod_reg_bit(vic, dma1_int_clr, p2_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_CLR
void set_vic_dma2_int_clr(u32 reg_addr,
			u32 fci_clr,
			u32 vbi_clr,
			u32 lcti_clr,
			u32 p0_ovf_clr,
			u32 p1_ovf_clr,
			u32 p2_ovf_clr,
			u32 m_or_r)
{
	reg_vic_dma2_int_clr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_clr = fci_clr;
	reg.bits.vbi_clr = vbi_clr;
	reg.bits.lcti_clr = lcti_clr;
	reg.bits.p0_ovf_clr = p0_ovf_clr;
	reg.bits.p1_ovf_clr = p1_ovf_clr;
	reg.bits.p2_ovf_clr = p2_ovf_clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_INT_CLR_FCI_CLR
fun_set_mod_reg_bit(vic, dma2_int_clr, fci_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_CLR_VBI_CLR
fun_set_mod_reg_bit(vic, dma2_int_clr, vbi_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_CLR_LCTI_CLR
fun_set_mod_reg_bit(vic, dma2_int_clr, lcti_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_CLR_P0_OVF_CLR
fun_set_mod_reg_bit(vic, dma2_int_clr, p0_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_CLR_P1_OVF_CLR
fun_set_mod_reg_bit(vic, dma2_int_clr, p1_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_INT_CLR_P2_OVF_CLR
fun_set_mod_reg_bit(vic, dma2_int_clr, p2_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_CLR
void set_vic_dma3_int_clr(u32 reg_addr,
			u32 fci_clr,
			u32 vbi_clr,
			u32 lcti_clr,
			u32 p0_ovf_clr,
			u32 p1_ovf_clr,
			u32 p2_ovf_clr,
			u32 m_or_r)
{
	reg_vic_dma3_int_clr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.fci_clr = fci_clr;
	reg.bits.vbi_clr = vbi_clr;
	reg.bits.lcti_clr = lcti_clr;
	reg.bits.p0_ovf_clr = p0_ovf_clr;
	reg.bits.p1_ovf_clr = p1_ovf_clr;
	reg.bits.p2_ovf_clr = p2_ovf_clr;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_INT_CLR_FCI_CLR
fun_set_mod_reg_bit(vic, dma3_int_clr, fci_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_CLR_VBI_CLR
fun_set_mod_reg_bit(vic, dma3_int_clr, vbi_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_CLR_LCTI_CLR
fun_set_mod_reg_bit(vic, dma3_int_clr, lcti_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_CLR_P0_OVF_CLR
fun_set_mod_reg_bit(vic, dma3_int_clr, p0_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_CLR_P1_OVF_CLR
fun_set_mod_reg_bit(vic, dma3_int_clr, p1_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_INT_CLR_P2_OVF_CLR
fun_set_mod_reg_bit(vic, dma3_int_clr, p2_ovf_clr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_LINE_CNT
void set_vic_dma0_line_cnt(u32 reg_addr,
			u32 ls_trig,
			u32 ls_crnt,
			u32 m_or_r)
{
	reg_vic_dma0_line_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ls_trig = ls_trig;
	reg.bits.ls_crnt = ls_crnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA0_LINE_CNT_LS_TRIG
fun_set_mod_reg_bit(vic, dma0_line_cnt, ls_trig, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA0_LINE_CNT_LS_CRNT
fun_set_mod_reg_bit(vic, dma0_line_cnt, ls_crnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_LINE_CNT
void set_vic_dma1_line_cnt(u32 reg_addr,
			u32 ls_trig,
			u32 ls_crnt,
			u32 m_or_r)
{
	reg_vic_dma1_line_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ls_trig = ls_trig;
	reg.bits.ls_crnt = ls_crnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA1_LINE_CNT_LS_TRIG
fun_set_mod_reg_bit(vic, dma1_line_cnt, ls_trig, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA1_LINE_CNT_LS_CRNT
fun_set_mod_reg_bit(vic, dma1_line_cnt, ls_crnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_LINE_CNT
void set_vic_dma2_line_cnt(u32 reg_addr,
			u32 ls_trig,
			u32 ls_crnt,
			u32 m_or_r)
{
	reg_vic_dma2_line_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ls_trig = ls_trig;
	reg.bits.ls_crnt = ls_crnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA2_LINE_CNT_LS_TRIG
fun_set_mod_reg_bit(vic, dma2_line_cnt, ls_trig, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA2_LINE_CNT_LS_CRNT
fun_set_mod_reg_bit(vic, dma2_line_cnt, ls_crnt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_LINE_CNT
void set_vic_dma3_line_cnt(u32 reg_addr,
			u32 ls_trig,
			u32 ls_crnt,
			u32 m_or_r)
{
	reg_vic_dma3_line_cnt_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ls_trig = ls_trig;
	reg.bits.ls_crnt = ls_crnt;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_VIC_DMA3_LINE_CNT_LS_TRIG
fun_set_mod_reg_bit(vic, dma3_line_cnt, ls_trig, addr, val, m_or_r)
#endif

#ifdef DEF_SET_VIC_DMA3_LINE_CNT_LS_CRNT
fun_set_mod_reg_bit(vic, dma3_line_cnt, ls_crnt, addr, val, m_or_r)
#endif

#endif /* ___VIC__SET___C___ */
