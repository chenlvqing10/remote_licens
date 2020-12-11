/* vic_dump.c */

#ifndef ___VIC___DUMP__C___
#define ___VIC___DUMP__C___

reg_info_t vic_info[] = {
	{VA_VIC_VER, DATA_VIC_VER, dump_vic_ver, "VIC_VER", 0},
	{VA_VIC_FTR, DATA_VIC_FTR, dump_vic_ftr, "VIC_FTR", 0},
	{VA_VIC_VIC_CTRL, DATA_VIC_VIC_CTRL, dump_vic_vic_ctrl, "VIC_VIC_CTRL", 0},
	{VA_VIC_VIC_CFG, DATA_VIC_VIC_CFG, dump_vic_vic_cfg, "VIC_VIC_CFG", 0},
	{VA_VIC_IO_CFG, DATA_VIC_IO_CFG, dump_vic_io_cfg, "VIC_IO_CFG", 0},
	{VA_VIC_IF_CFG, DATA_VIC_IF_CFG, dump_vic_if_cfg, "VIC_IF_CFG", 0},
	{VA_VIC_DMA_MULTI_CH, DATA_VIC_DMA_MULTI_CH, dump_vic_dma_multi_ch, "VIC_DMA_MULTI_CH", 0},
	{VA_VIC_DMA_MULTI_ID, DATA_VIC_DMA_MULTI_ID, dump_vic_dma_multi_id, "VIC_DMA_MULTI_ID", 0},
	{VA_VIC_HFB_OFS, DATA_VIC_HFB_OFS, dump_vic_hfb_ofs, "VIC_HFB_OFS", 0},
	{VA_VIC_HACT_WIDTH, DATA_VIC_HACT_WIDTH, dump_vic_hact_width, "VIC_HACT_WIDTH", 0},
	{VA_VIC_F1_VFB_OFS, DATA_VIC_F1_VFB_OFS, dump_vic_f1_vfb_ofs, "VIC_F1_VFB_OFS", 0},
	{VA_VIC_F1_VACT_LINE, DATA_VIC_F1_VACT_LINE, dump_vic_f1_vact_line, "VIC_F1_VACT_LINE", 0},
	{VA_VIC_F2_VFB_OFS, DATA_VIC_F2_VFB_OFS, dump_vic_f2_vfb_ofs, "VIC_F2_VFB_OFS", 0},
	{VA_VIC_F2_VACT_LINE, DATA_VIC_F2_VACT_LINE, dump_vic_f2_vact_line, "VIC_F2_VACT_LINE", 0},
	{VA_VIC_DMA0_OFS, DATA_VIC_DMA0_OFS, dump_vic_dma0_ofs, "VIC_DMA0_OFS", 0},
	{VA_VIC_DMA1_OFS, DATA_VIC_DMA1_OFS, dump_vic_dma1_ofs, "VIC_DMA1_OFS", 0},
	{VA_VIC_DMA2_OFS, DATA_VIC_DMA2_OFS, dump_vic_dma2_ofs, "VIC_DMA2_OFS", 0},
	{VA_VIC_DMA3_OFS, DATA_VIC_DMA3_OFS, dump_vic_dma3_ofs, "VIC_DMA3_OFS", 0},
	{VA_VIC_DMA0_SIZE, DATA_VIC_DMA0_SIZE, dump_vic_dma0_size, "VIC_DMA0_SIZE", 0},
	{VA_VIC_DMA1_SIZE, DATA_VIC_DMA1_SIZE, dump_vic_dma1_size, "VIC_DMA1_SIZE", 0},
	{VA_VIC_DMA2_SIZE, DATA_VIC_DMA2_SIZE, dump_vic_dma2_size, "VIC_DMA2_SIZE", 0},
	{VA_VIC_DMA3_SIZE, DATA_VIC_DMA3_SIZE, dump_vic_dma3_size, "VIC_DMA3_SIZE", 0},
	{VA_VIC_DMA0_MODE, DATA_VIC_DMA0_MODE, dump_vic_dma0_mode, "VIC_DMA0_MODE", 0},
	{VA_VIC_DMA1_MODE, DATA_VIC_DMA1_MODE, dump_vic_dma1_mode, "VIC_DMA1_MODE", 0},
	{VA_VIC_DMA2_MODE, DATA_VIC_DMA2_MODE, dump_vic_dma2_mode, "VIC_DMA2_MODE", 0},
	{VA_VIC_DMA3_MODE, DATA_VIC_DMA3_MODE, dump_vic_dma3_mode, "VIC_DMA3_MODE", 0},
	{VA_VIC_DMA0_CTRL, DATA_VIC_DMA0_CTRL, dump_vic_dma0_ctrl, "VIC_DMA0_CTRL", 0},
	{VA_VIC_DMA1_CTRL, DATA_VIC_DMA1_CTRL, dump_vic_dma1_ctrl, "VIC_DMA1_CTRL", 0},
	{VA_VIC_DMA2_CTRL, DATA_VIC_DMA2_CTRL, dump_vic_dma2_ctrl, "VIC_DMA2_CTRL", 0},
	{VA_VIC_DMA3_CTRL, DATA_VIC_DMA3_CTRL, dump_vic_dma3_ctrl, "VIC_DMA3_CTRL", 0},
	{VA_VIC_DMA0_UPDATE, DATA_VIC_DMA0_UPDATE, dump_vic_dma0_update, "VIC_DMA0_UPDATE", 0},
	{VA_VIC_DMA1_UPDATE, DATA_VIC_DMA1_UPDATE, dump_vic_dma1_update, "VIC_DMA1_UPDATE", 0},
	{VA_VIC_DMA2_UPDATE, DATA_VIC_DMA2_UPDATE, dump_vic_dma2_update, "VIC_DMA2_UPDATE", 0},
	{VA_VIC_DMA3_UPDATE, DATA_VIC_DMA3_UPDATE, dump_vic_dma3_update, "VIC_DMA3_UPDATE", 0},
	{VA_VIC_DMA0_ADDR0, DATA_VIC_DMA0_ADDR0, dump_vic_dma0_addr0, "VIC_DMA0_ADDR0", 0},
	{VA_VIC_DMA1_ADDR0, DATA_VIC_DMA1_ADDR0, dump_vic_dma1_addr0, "VIC_DMA1_ADDR0", 0},
	{VA_VIC_DMA2_ADDR0, DATA_VIC_DMA2_ADDR0, dump_vic_dma2_addr0, "VIC_DMA2_ADDR0", 0},
	{VA_VIC_DMA3_ADDR0, DATA_VIC_DMA3_ADDR0, dump_vic_dma3_addr0, "VIC_DMA3_ADDR0", 0},
	{VA_VIC_DMA0_ADDR1, DATA_VIC_DMA0_ADDR1, dump_vic_dma0_addr1, "VIC_DMA0_ADDR1", 0},
	{VA_VIC_DMA1_ADDR1, DATA_VIC_DMA1_ADDR1, dump_vic_dma1_addr1, "VIC_DMA1_ADDR1", 0},
	{VA_VIC_DMA2_ADDR1, DATA_VIC_DMA2_ADDR1, dump_vic_dma2_addr1, "VIC_DMA2_ADDR1", 0},
	{VA_VIC_DMA3_ADDR1, DATA_VIC_DMA3_ADDR1, dump_vic_dma3_addr1, "VIC_DMA3_ADDR1", 0},
	{VA_VIC_DMA0_ADDR2, DATA_VIC_DMA0_ADDR2, dump_vic_dma0_addr2, "VIC_DMA0_ADDR2", 0},
	{VA_VIC_DMA1_ADDR2, DATA_VIC_DMA1_ADDR2, dump_vic_dma1_addr2, "VIC_DMA1_ADDR2", 0},
	{VA_VIC_DMA2_ADDR2, DATA_VIC_DMA2_ADDR2, dump_vic_dma2_addr2, "VIC_DMA2_ADDR2", 0},
	{VA_VIC_DMA3_ADDR2, DATA_VIC_DMA3_ADDR2, dump_vic_dma3_addr2, "VIC_DMA3_ADDR2", 0},
	{VA_VIC_DMA0_LS, DATA_VIC_DMA0_LS, dump_vic_dma0_ls, "VIC_DMA0_LS", 0},
	{VA_VIC_DMA1_LS, DATA_VIC_DMA1_LS, dump_vic_dma1_ls, "VIC_DMA1_LS", 0},
	{VA_VIC_DMA2_LS, DATA_VIC_DMA2_LS, dump_vic_dma2_ls, "VIC_DMA2_LS", 0},
	{VA_VIC_DMA3_LS, DATA_VIC_DMA3_LS, dump_vic_dma3_ls, "VIC_DMA3_LS", 0},
	{VA_VIC_DMA0_BC, DATA_VIC_DMA0_BC, dump_vic_dma0_bc, "VIC_DMA0_BC", 0},
	{VA_VIC_DMA1_BC, DATA_VIC_DMA1_BC, dump_vic_dma1_bc, "VIC_DMA1_BC", 0},
	{VA_VIC_DMA2_BC, DATA_VIC_DMA2_BC, dump_vic_dma2_bc, "VIC_DMA2_BC", 0},
	{VA_VIC_DMA3_BC, DATA_VIC_DMA3_BC, dump_vic_dma3_bc, "VIC_DMA3_BC", 0},
	{VA_VIC_DMA0_INT_EN, DATA_VIC_DMA0_INT_EN, dump_vic_dma0_int_en, "VIC_DMA0_INT_EN", 0},
	{VA_VIC_DMA1_INT_EN, DATA_VIC_DMA1_INT_EN, dump_vic_dma1_int_en, "VIC_DMA1_INT_EN", 0},
	{VA_VIC_DMA2_INT_EN, DATA_VIC_DMA2_INT_EN, dump_vic_dma2_int_en, "VIC_DMA2_INT_EN", 0},
	{VA_VIC_DMA3_INT_EN, DATA_VIC_DMA3_INT_EN, dump_vic_dma3_int_en, "VIC_DMA3_INT_EN", 0},
	{VA_VIC_DMA0_INT_PEND, DATA_VIC_DMA0_INT_PEND, dump_vic_dma0_int_pend, "VIC_DMA0_INT_PEND", 0},
	{VA_VIC_DMA1_INT_PEND, DATA_VIC_DMA1_INT_PEND, dump_vic_dma1_int_pend, "VIC_DMA1_INT_PEND", 0},
	{VA_VIC_DMA2_INT_PEND, DATA_VIC_DMA2_INT_PEND, dump_vic_dma2_int_pend, "VIC_DMA2_INT_PEND", 0},
	{VA_VIC_DMA3_INT_PEND, DATA_VIC_DMA3_INT_PEND, dump_vic_dma3_int_pend, "VIC_DMA3_INT_PEND", 0},
	{VA_VIC_DMA0_INT_CLR, DATA_VIC_DMA0_INT_CLR, dump_vic_dma0_int_clr, "VIC_DMA0_INT_CLR", 0},
	{VA_VIC_DMA1_INT_CLR, DATA_VIC_DMA1_INT_CLR, dump_vic_dma1_int_clr, "VIC_DMA1_INT_CLR", 0},
	{VA_VIC_DMA2_INT_CLR, DATA_VIC_DMA2_INT_CLR, dump_vic_dma2_int_clr, "VIC_DMA2_INT_CLR", 0},
	{VA_VIC_DMA3_INT_CLR, DATA_VIC_DMA3_INT_CLR, dump_vic_dma3_int_clr, "VIC_DMA3_INT_CLR", 0},
	{VA_VIC_DMA0_LINE_CNT, DATA_VIC_DMA0_LINE_CNT, dump_vic_dma0_line_cnt, "VIC_DMA0_LINE_CNT", 0},
	{VA_VIC_DMA1_LINE_CNT, DATA_VIC_DMA1_LINE_CNT, dump_vic_dma1_line_cnt, "VIC_DMA1_LINE_CNT", 0},
	{VA_VIC_DMA2_LINE_CNT, DATA_VIC_DMA2_LINE_CNT, dump_vic_dma2_line_cnt, "VIC_DMA2_LINE_CNT", 0},
	{VA_VIC_DMA3_LINE_CNT, DATA_VIC_DMA3_LINE_CNT, dump_vic_dma3_line_cnt, "VIC_DMA3_LINE_CNT", 0},
};

void dump_vic(u32 mode)
{
	u32 cnt = sizeof(vic_info) / sizeof(vic_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					vic_info[i].name,
					vic_info[i].addr,
					vic_info[i].reset);
		def_strcat(buffer, sz_temp);

		vic_info[i].dump(vic_info[i].addr, vic_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_vic_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_ver_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ver_l\n", reg.bits.ver_l);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ver_h\n", reg.bits.ver_h);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:comp\n", reg.bits.comp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_ftr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_ftr_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:has_yc_intl\n", reg.bits.has_yc_intl);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_yc_sp\n", reg.bits.has_yc_sp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_multi_ch\n", reg.bits.has_multi_ch);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_dma0\n", reg.bits.has_dma0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_dma1\n", reg.bits.has_dma1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_dma2\n", reg.bits.has_dma2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_dma3\n", reg.bits.has_dma3);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_vic_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_vic_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:vic_en\n", reg.bits.vic_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_vic_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_vic_cfg_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:raw_msb\n", reg.bits.raw_msb);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:data_8bit\n", reg.bits.data_8bit);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dvp_path\n", reg.bits.dvp_path);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:busy\n", reg.bits.busy);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_io_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_io_cfg_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:pclk_inv\n", reg.bits.pclk_inv);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:field_inv\n", reg.bits.field_inv);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hs_inv\n", reg.bits.hs_inv);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vs_inv\n", reg.bits.vs_inv);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pclk_phase\n", reg.bits.pclk_phase);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_if_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_if_cfg_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:data_ptcl\n", reg.bits.data_ptcl);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:yc_seq\n", reg.bits.yc_seq);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:yc_sync\n", reg.bits.yc_sync);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sync_ptcl\n", reg.bits.sync_ptcl);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:field_det\n", reg.bits.field_det);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:v_mode\n", reg.bits.v_mode);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma_multi_ch(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma_multi_ch_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:num\n", reg.bits.num);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma_multi_id(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma_multi_id_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ch0_id\n", reg.bits.ch0_id);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ch1_id\n", reg.bits.ch1_id);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ch2_id\n", reg.bits.ch2_id);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ch3_id\n", reg.bits.ch3_id);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_hfb_ofs(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_hfb_ofs_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:hoffset\n", reg.bits.hoffset);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_hact_width(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_hact_width_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:haw\n", reg.bits.haw);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_f1_vfb_ofs(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_f1_vfb_ofs_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:f1_voffset\n", reg.bits.f1_voffset);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_f1_vact_line(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_f1_vact_line_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:f1_vact_line\n", reg.bits.f1_vact_line);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_f2_vfb_ofs(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_f2_vfb_ofs_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:f2_voffset\n", reg.bits.f2_voffset);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_f2_vact_line(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_f2_vact_line_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:f2_vact_line\n", reg.bits.f2_vact_line);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_ofs(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_ofs_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:xs\n", reg.bits.xs);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ys\n", reg.bits.ys);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_ofs(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_ofs_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:xs\n", reg.bits.xs);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ys\n", reg.bits.ys);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_ofs(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_ofs_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:xs\n", reg.bits.xs);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ys\n", reg.bits.ys);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_ofs(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_ofs_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:xs\n", reg.bits.xs);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ys\n", reg.bits.ys);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_size(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_size_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:x\n", reg.bits.x);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:y\n", reg.bits.y);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_size(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_size_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:x\n", reg.bits.x);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:y\n", reg.bits.y);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_size(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_size_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:x\n", reg.bits.x);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:y\n", reg.bits.y);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_size(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_size_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:x\n", reg.bits.x);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:y\n", reg.bits.y);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_mode(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_mode_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:out_fmt\n", reg.bits.out_fmt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:burst\n", reg.bits.burst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:filed_strg\n", reg.bits.filed_strg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fci\n", reg.bits.fci);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi\n", reg.bits.vbi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti\n", reg.bits.lcti);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ud\n", reg.bits.ud);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cap\n", reg.bits.cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_mode(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_mode_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:out_fmt\n", reg.bits.out_fmt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:burst\n", reg.bits.burst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:filed_strg\n", reg.bits.filed_strg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fci\n", reg.bits.fci);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi\n", reg.bits.vbi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti\n", reg.bits.lcti);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ud\n", reg.bits.ud);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cap\n", reg.bits.cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_mode(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_mode_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:out_fmt\n", reg.bits.out_fmt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:burst\n", reg.bits.burst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:filed_strg\n", reg.bits.filed_strg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fci\n", reg.bits.fci);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi\n", reg.bits.vbi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti\n", reg.bits.lcti);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ud\n", reg.bits.ud);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cap\n", reg.bits.cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_mode(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_mode_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:out_fmt\n", reg.bits.out_fmt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:burst\n", reg.bits.burst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:filed_strg\n", reg.bits.filed_strg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fci\n", reg.bits.fci);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi\n", reg.bits.vbi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti\n", reg.bits.lcti);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ud\n", reg.bits.ud);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cap\n", reg.bits.cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:start\n", reg.bits.start);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:start\n", reg.bits.start);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:start\n", reg.bits.start);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_ctrl_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:start\n", reg.bits.start);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_update(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_update_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ud_man\n", reg.bits.ud_man);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_update(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_update_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ud_man\n", reg.bits.ud_man);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_update(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_update_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ud_man\n", reg.bits.ud_man);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_update(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_update_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ud_man\n", reg.bits.ud_man);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_addr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_addr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_addr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_addr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_addr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_addr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_addr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_addr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_addr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_addr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_addr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_addr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_addr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_addr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_addr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_addr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_addr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_addr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_addr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_addr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_addr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_addr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_addr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_addr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:addr\n", reg.bits.addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_ls(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_ls_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:y\n", reg.bits.y);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:c\n", reg.bits.c);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_ls(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_ls_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:y\n", reg.bits.y);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:c\n", reg.bits.c);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_ls(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_ls_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:y\n", reg.bits.y);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:c\n", reg.bits.c);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_ls(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_ls_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:y\n", reg.bits.y);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:c\n", reg.bits.c);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_bc(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_bc_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:bc\n", reg.bits.bc);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_bc(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_bc_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:bc\n", reg.bits.bc);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_bc(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_bc_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:bc\n", reg.bits.bc);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_bc(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_bc_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:bc\n", reg.bits.bc);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_int_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_en\n", reg.bits.fci_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_en\n", reg.bits.vbi_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_en\n", reg.bits.lcti_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_en\n", reg.bits.p0_ovf_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_en\n", reg.bits.p1_ovf_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_en\n", reg.bits.p2_ovf_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_int_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_en\n", reg.bits.fci_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_en\n", reg.bits.vbi_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_en\n", reg.bits.lcti_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_en\n", reg.bits.p0_ovf_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_en\n", reg.bits.p1_ovf_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_en\n", reg.bits.p2_ovf_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_int_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_en\n", reg.bits.fci_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_en\n", reg.bits.vbi_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_en\n", reg.bits.lcti_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_en\n", reg.bits.p0_ovf_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_en\n", reg.bits.p1_ovf_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_en\n", reg.bits.p2_ovf_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_int_en_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_en\n", reg.bits.fci_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_en\n", reg.bits.vbi_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_en\n", reg.bits.lcti_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_en\n", reg.bits.p0_ovf_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_en\n", reg.bits.p1_ovf_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_en\n", reg.bits.p2_ovf_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_int_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_int_pend_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_pend\n", reg.bits.fci_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_pend\n", reg.bits.vbi_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_pend\n", reg.bits.lcti_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_pend\n", reg.bits.p0_ovf_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_pend\n", reg.bits.p1_ovf_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_pend\n", reg.bits.p2_ovf_pend);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_int_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_int_pend_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_pend\n", reg.bits.fci_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_pend\n", reg.bits.vbi_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_pend\n", reg.bits.lcti_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_pend\n", reg.bits.p0_ovf_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_pend\n", reg.bits.p1_ovf_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_pend\n", reg.bits.p2_ovf_pend);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_int_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_int_pend_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_pend\n", reg.bits.fci_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_pend\n", reg.bits.vbi_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_pend\n", reg.bits.lcti_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_pend\n", reg.bits.p0_ovf_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_pend\n", reg.bits.p1_ovf_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_pend\n", reg.bits.p2_ovf_pend);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_int_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_int_pend_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_pend\n", reg.bits.fci_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_pend\n", reg.bits.vbi_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_pend\n", reg.bits.lcti_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_pend\n", reg.bits.p0_ovf_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_pend\n", reg.bits.p1_ovf_pend);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_pend\n", reg.bits.p2_ovf_pend);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_int_clr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_int_clr_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_clr\n", reg.bits.fci_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_clr\n", reg.bits.vbi_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_clr\n", reg.bits.lcti_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_clr\n", reg.bits.p0_ovf_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_clr\n", reg.bits.p1_ovf_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_clr\n", reg.bits.p2_ovf_clr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_int_clr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_int_clr_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_clr\n", reg.bits.fci_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_clr\n", reg.bits.vbi_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_clr\n", reg.bits.lcti_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_clr\n", reg.bits.p0_ovf_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_clr\n", reg.bits.p1_ovf_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_clr\n", reg.bits.p2_ovf_clr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_int_clr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_int_clr_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_clr\n", reg.bits.fci_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_clr\n", reg.bits.vbi_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_clr\n", reg.bits.lcti_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_clr\n", reg.bits.p0_ovf_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_clr\n", reg.bits.p1_ovf_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_clr\n", reg.bits.p2_ovf_clr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_int_clr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_int_clr_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:fci_clr\n", reg.bits.fci_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vbi_clr\n", reg.bits.vbi_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lcti_clr\n", reg.bits.lcti_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p0_ovf_clr\n", reg.bits.p0_ovf_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p1_ovf_clr\n", reg.bits.p1_ovf_clr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p2_ovf_clr\n", reg.bits.p2_ovf_clr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma0_line_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma0_line_cnt_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ls_trig\n", reg.bits.ls_trig);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ls_crnt\n", reg.bits.ls_crnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma1_line_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma1_line_cnt_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ls_trig\n", reg.bits.ls_trig);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ls_crnt\n", reg.bits.ls_crnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma2_line_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma2_line_cnt_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ls_trig\n", reg.bits.ls_trig);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ls_crnt\n", reg.bits.ls_crnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_vic_dma3_line_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_vic_dma3_line_cnt_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ls_trig\n", reg.bits.ls_trig);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ls_crnt\n", reg.bits.ls_crnt);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___VIC___DUMP__C___ */
