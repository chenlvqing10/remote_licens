/* cpu_ctrl_dump.c */

#ifndef ___CPU_CTRL___DUMP__C___
#define ___CPU_CTRL___DUMP__C___

reg_info_t cpu_ctrl_info[] = {
	{VA_CPU_CTRL_RST_CTRL, DATA_CPU_CTRL_RST_CTRL, dump_cpu_ctrl_rst_ctrl, "CPU_CTRL_RST_CTRL", 0},
	{VA_CPU_CTRL_CTRL_R, DATA_CPU_CTRL_CTRL_R, dump_cpu_ctrl_ctrl_r, "CPU_CTRL_CTRL_R", 0},
	{VA_CPU_CTRL_CFG_R, DATA_CPU_CTRL_CFG_R, dump_cpu_ctrl_cfg_r, "CPU_CTRL_CFG_R", 0},
	{VA_CPU_CTRL_STATUS_R, DATA_CPU_CTRL_STATUS_R, dump_cpu_ctrl_status_r, "CPU_CTRL_STATUS_R", 0},
	{VA_CPU_CTRL_GENER_REG, DATA_CPU_CTRL_GENER_REG, dump_cpu_ctrl_gener_reg, "CPU_CTRL_GENER_REG", 0},
	{VA_CPU_CTRL_CPU_CLK_SEL, DATA_CPU_CTRL_CPU_CLK_SEL, dump_cpu_ctrl_cpu_clk_sel, "CPU_CTRL_CPU_CLK_SEL", 0},
	{VA_CPU_CTRL_ADPLL_FAC, DATA_CPU_CTRL_ADPLL_FAC, dump_cpu_ctrl_adpll_fac, "CPU_CTRL_ADPLL_FAC", 0},
	{VA_CPU_CTRL_ADPLL_TUNE0, DATA_CPU_CTRL_ADPLL_TUNE0, dump_cpu_ctrl_adpll_tune0, "CPU_CTRL_ADPLL_TUNE0", 0},
	{VA_CPU_CTRL_ADPLL_TUNE1, DATA_CPU_CTRL_ADPLL_TUNE1, dump_cpu_ctrl_adpll_tune1, "CPU_CTRL_ADPLL_TUNE1", 0},
	{VA_CPU_CTRL_ADPLL_STAT, DATA_CPU_CTRL_ADPLL_STAT, dump_cpu_ctrl_adpll_stat, "CPU_CTRL_ADPLL_STAT", 0},
	{VA_CPU_CTRL_CACHE_CFG0, DATA_CPU_CTRL_CACHE_CFG0, dump_cpu_ctrl_cache_cfg0, "CPU_CTRL_CACHE_CFG0", 0},
	{VA_CPU_CTRL_CACHE_CFG1, DATA_CPU_CTRL_CACHE_CFG1, dump_cpu_ctrl_cache_cfg1, "CPU_CTRL_CACHE_CFG1", 0},
	{VA_CPU_CTRL_DBG_REG0, DATA_CPU_CTRL_DBG_REG0, dump_cpu_ctrl_dbg_reg0, "CPU_CTRL_DBG_REG0", 0},
	{VA_CPU_CTRL_VER, DATA_CPU_CTRL_VER, dump_cpu_ctrl_ver, "CPU_CTRL_VER", 0},
	{VA_CPU_CTRL_C_RST_CTRL, DATA_CPU_CTRL_C_RST_CTRL, dump_cpu_ctrl_c_rst_ctrl, "CPU_CTRL_C_RST_CTRL", 0},
};

void dump_cpu_ctrl(u32 mode)
{
	u32 cnt = sizeof(cpu_ctrl_info) / sizeof(cpu_ctrl_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					cpu_ctrl_info[i].name,
					cpu_ctrl_info[i].addr,
					cpu_ctrl_info[i].reset);
		def_strcat(buffer, sz_temp);

		cpu_ctrl_info[i].dump(cpu_ctrl_info[i].addr, cpu_ctrl_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_cpu_ctrl_rst_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_rst_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:ncorereset0\n", reg.bits.ncorereset0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ncorereset1\n", reg.bits.ncorereset1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ncoreporeset0\n", reg.bits.ncoreporeset0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ncoreporeset1\n", reg.bits.ncoreporeset1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ndbgreset0\n", reg.bits.ndbgreset0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ndbgreset1\n", reg.bits.ndbgreset1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:l1rstdisable0\n", reg.bits.l1rstdisable0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:l1rstdisable1\n", reg.bits.l1rstdisable1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_ctrl_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_ctrl_r_t reg;

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

	def_sprintf(sz_temp, "0x%X:eventi\n", reg.bits.eventi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfgsdisable\n", reg.bits.cfgsdisable);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cp15sdisable0\n", reg.bits.cp15sdisable0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cp15sdisable1\n", reg.bits.cp15sdisable1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpu1_gat\n", reg.bits.cpu1_gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:acinactm\n", reg.bits.acinactm);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_cfg_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_cfg_r_t reg;

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

	def_sprintf(sz_temp, "0x%X:broadcastinner\n", reg.bits.broadcastinner);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:broadcastouter\n", reg.bits.broadcastouter);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:broadcastcachemaint\n", reg.bits.broadcastcachemaint);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sysbardisable\n", reg.bits.sysbardisable);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_status_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_status_r_t reg;

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

	def_sprintf(sz_temp, "0x%X:smpnamp0\n", reg.bits.smpnamp0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:smpnamp1\n", reg.bits.smpnamp1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:standbywfe0\n", reg.bits.standbywfe0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:standbywfe1\n", reg.bits.standbywfe1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:standbywfi0\n", reg.bits.standbywfi0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:standbywfi1\n", reg.bits.standbywfi1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:standbywfil2\n", reg.bits.standbywfil2);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_gener_reg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_gener_reg_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpr\n", reg.bits.gpr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_cpu_clk_sel(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_cpu_clk_sel_t reg;

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

	def_sprintf(sz_temp, "0x%X:src\n", reg.bits.src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_div\n", reg.bits.src_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dbg_en\n", reg.bits.dbg_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_adpll_fac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_adpll_fac_t reg;

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

	def_sprintf(sz_temp, "0x%X:pre_div\n", reg.bits.pre_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n\n", reg.bits.n);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_adpll_tune0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_adpll_tune0_t reg;

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

u32 dump_cpu_ctrl_adpll_tune1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_adpll_tune1_t reg;

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

u32 dump_cpu_ctrl_adpll_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_adpll_stat_t reg;

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

u32 dump_cpu_ctrl_cache_cfg0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_cache_cfg0_t reg;

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

	def_sprintf(sz_temp, "0x%X:parameter\n", reg.bits.parameter);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_cache_cfg1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_cache_cfg1_t reg;

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

	def_sprintf(sz_temp, "0x%X:parameter\n", reg.bits.parameter);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_dbg_reg0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_dbg_reg0_t reg;

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

	def_sprintf(sz_temp, "0x%X:dbgpwrdup0\n", reg.bits.dbgpwrdup0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dbgpwrdup1\n", reg.bits.dbgpwrdup1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dbgpwrupreq0\n", reg.bits.dbgpwrupreq0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dbgpwrupreq1\n", reg.bits.dbgpwrupreq1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dbgnopwrdwn0\n", reg.bits.dbgnopwrdwn0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dbgnopwrdwn1\n", reg.bits.dbgnopwrdwn1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_cpu_ctrl_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_ver_t reg;

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

u32 dump_cpu_ctrl_c_rst_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_cpu_ctrl_c_rst_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:cluster_rst\n", reg.bits.cluster_rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:soc_dbg_rst\n", reg.bits.soc_dbg_rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dap_rst\n", reg.bits.dap_rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:jtag_rst\n", reg.bits.jtag_rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gic_rst\n", reg.bits.gic_rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:l2_rst\n", reg.bits.l2_rst);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:l2rstdisable\n", reg.bits.l2rstdisable);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___CPU_CTRL___DUMP__C___ */
