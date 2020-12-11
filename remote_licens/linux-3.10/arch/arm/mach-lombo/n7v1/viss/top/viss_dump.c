/* viss_dump.c */

#ifndef ___VISS___DUMP__C___
#define ___VISS___DUMP__C___

reg_info_t viss_info[] = {
	{VA_VISS_VER, DATA_VISS_VER, dump_viss_ver, "VISS_VER", 0},
	{VA_VISS_FTR, DATA_VISS_FTR, dump_viss_ftr, "VISS_FTR", 0},
	{VA_VISS_VIC_CFG, DATA_VISS_VIC_CFG, dump_viss_vic_cfg, "VISS_VIC_CFG", 0},
	{VA_VISS_MCSI_CFG, DATA_VISS_MCSI_CFG, dump_viss_mcsi_cfg, "VISS_MCSI_CFG", 0},
	{VA_VISS_TVD_CFG, DATA_VISS_TVD_CFG, dump_viss_tvd_cfg, "VISS_TVD_CFG", 0},
	{VA_VISS_ISP_CFG, DATA_VISS_ISP_CFG, dump_viss_isp_cfg, "VISS_ISP_CFG", 0},
	{VA_VISS_ISP_LITE_CFG, DATA_VISS_ISP_LITE_CFG, dump_viss_isp_lite_cfg, "VISS_ISP_LITE_CFG", 0},
	{VA_VISS_ISP_PIX_SEL, DATA_VISS_ISP_PIX_SEL, dump_viss_isp_pix_sel, "VISS_ISP_PIX_SEL", 0},
	{VA_VISS_PLL_MUX_CTRL, DATA_VISS_PLL_MUX_CTRL, dump_viss_pll_mux_ctrl, "VISS_PLL_MUX_CTRL", 0},
	{VA_VISS_ADPLL_FAC, DATA_VISS_ADPLL_FAC, dump_viss_adpll_fac, "VISS_ADPLL_FAC", 0},
	{VA_VISS_ADPLL_TUNE0, DATA_VISS_ADPLL_TUNE0, dump_viss_adpll_tune0, "VISS_ADPLL_TUNE0", 0},
	{VA_VISS_ADPLL_TUNE1, DATA_VISS_ADPLL_TUNE1, dump_viss_adpll_tune1, "VISS_ADPLL_TUNE1", 0},
	{VA_VISS_ADPLL_STAT, DATA_VISS_ADPLL_STAT, dump_viss_adpll_stat, "VISS_ADPLL_STAT", 0},
	{VA_VISS_MBIST_CFG, DATA_VISS_MBIST_CFG, dump_viss_mbist_cfg, "VISS_MBIST_CFG", 0},
	{VA_VISS_AXI_RD_QOS_LVL, DATA_VISS_AXI_RD_QOS_LVL, dump_viss_axi_rd_qos_lvl, "VISS_AXI_RD_QOS_LVL", 0},
	{VA_VISS_AXI_WT_QOS_LVL, DATA_VISS_AXI_WT_QOS_LVL, dump_viss_axi_wt_qos_lvl, "VISS_AXI_WT_QOS_LVL", 0},
};

void dump_viss(u32 mode)
{
	u32 cnt = sizeof(viss_info) / sizeof(viss_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					viss_info[i].name,
					viss_info[i].addr,
					viss_info[i].reset);
		def_strcat(buffer, sz_temp);

		viss_info[i].dump(viss_info[i].addr, viss_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_viss_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_ver_t reg;

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

u32 dump_viss_ftr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_ftr_t reg;

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

	def_sprintf(sz_temp, "0x%X:has_vic\n", reg.bits.has_vic);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_mcsi\n", reg.bits.has_mcsi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_tvd\n", reg.bits.has_tvd);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_isp\n", reg.bits.has_isp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:has_isp_lite\n", reg.bits.has_isp_lite);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_vic_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_vic_cfg_t reg;

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

	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hclk_gat\n", reg.bits.hclk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_gat\n", reg.bits.clk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_src\n", reg.bits.clk_src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mclk_gat\n", reg.bits.mclk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mclk_src\n", reg.bits.mclk_src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_dir\n", reg.bits.clk_dir);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mclk_dir\n", reg.bits.mclk_dir);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_mcsi_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_mcsi_cfg_t reg;

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

	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hclk_gat\n", reg.bits.hclk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_gat\n", reg.bits.clk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_src\n", reg.bits.clk_src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mclk_gat\n", reg.bits.mclk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mclk_src\n", reg.bits.mclk_src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfgclk_gat\n", reg.bits.cfgclk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_dir\n", reg.bits.clk_dir);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mclk_dir\n", reg.bits.mclk_dir);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_tvd_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_tvd_cfg_t reg;

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

	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hclk_gat\n", reg.bits.hclk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_gat\n", reg.bits.clk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_src\n", reg.bits.clk_src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_dir\n", reg.bits.clk_dir);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_isp_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_isp_cfg_t reg;

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

	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hclk_gat\n", reg.bits.hclk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_gat\n", reg.bits.clk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_src\n", reg.bits.clk_src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_dir\n", reg.bits.clk_dir);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_isp_lite_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_isp_lite_cfg_t reg;

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

	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hclk_gat\n", reg.bits.hclk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_gat\n", reg.bits.clk_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_src\n", reg.bits.clk_src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_dir\n", reg.bits.clk_dir);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_isp_pix_sel(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_isp_pix_sel_t reg;

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

	def_sprintf(sz_temp, "0x%X:isp_pix_sel\n", reg.bits.isp_pix_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ispl_pix_sel\n", reg.bits.ispl_pix_sel);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_pll_mux_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_pll_mux_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:pllmux_clk_sel\n", reg.bits.pllmux_clk_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:adpll_pre_div\n", reg.bits.adpll_pre_div);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_adpll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_adpll_fac_t reg;

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

	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_adpll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_adpll_tune0_t reg;

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

	def_sprintf(sz_temp, "0x%X:tune0\n", reg.bits.tune0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:auto_adrange\n", reg.bits.auto_adrange);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_adpll_tune1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_adpll_tune1_t reg;

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

	def_sprintf(sz_temp, "0x%X:tune0\n", reg.bits.tune0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune1\n", reg.bits.tune1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune2\n", reg.bits.tune2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune3\n", reg.bits.tune3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:slewrate\n", reg.bits.slewrate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune4\n", reg.bits.tune4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune5\n", reg.bits.tune5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune6\n", reg.bits.tune6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tune7\n", reg.bits.tune7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_adpll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_adpll_stat_t reg;

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

	def_sprintf(sz_temp, "0x%X:ferr\n", reg.bits.ferr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:flock\n", reg.bits.flock);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:perr\n", reg.bits.perr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:plock\n", reg.bits.plock);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_mbist_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_mbist_cfg_t reg;

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

	def_sprintf(sz_temp, "0x%X:vic_cfg\n", reg.bits.vic_cfg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mcsi_cfg\n", reg.bits.mcsi_cfg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tvd_cfg\n", reg.bits.tvd_cfg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:isp_cfg\n", reg.bits.isp_cfg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:isp_lite_cfg\n", reg.bits.isp_lite_cfg);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vic_sta\n", reg.bits.vic_sta);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mcsi_sta\n", reg.bits.mcsi_sta);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tvd_sta\n", reg.bits.tvd_sta);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:isp_sta\n", reg.bits.isp_sta);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:isp_lite_sta\n", reg.bits.isp_lite_sta);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_axi_rd_qos_lvl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_axi_rd_qos_lvl_t reg;

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

	def_sprintf(sz_temp, "0x%X:vic\n", reg.bits.vic);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mcsi\n", reg.bits.mcsi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:isp\n", reg.bits.isp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:isp_lite\n", reg.bits.isp_lite);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tvd_wb\n", reg.bits.tvd_wb);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tvd_3d\n", reg.bits.tvd_3d);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_viss_axi_wt_qos_lvl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_viss_axi_wt_qos_lvl_t reg;

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

	def_sprintf(sz_temp, "0x%X:vic\n", reg.bits.vic);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mcsi\n", reg.bits.mcsi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:isp\n", reg.bits.isp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:isp_lite\n", reg.bits.isp_lite);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tvd_wb\n", reg.bits.tvd_wb);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tvd_3d\n", reg.bits.tvd_3d);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___VISS___DUMP__C___ */
