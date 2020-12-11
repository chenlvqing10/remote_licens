/* wdog_dump.c */

#ifndef ___WDOG___DUMP__C___
#define ___WDOG___DUMP__C___

reg_info_t wdog_info[] = {
	{VA_WDOG_WDOG_VER, DATA_WDOG_WDOG_VER, dump_wdog_wdog_ver, "WDOG_WDOG_VER", 0},
	{VA_WDOG_WDOG_CLK_CTRL, DATA_WDOG_WDOG_CLK_CTRL, dump_wdog_wdog_clk_ctrl, "WDOG_WDOG_CLK_CTRL", 0},
	{VA_WDOG_WDOG_CTRL, DATA_WDOG_WDOG_CTRL, dump_wdog_wdog_ctrl, "WDOG_WDOG_CTRL", 0},
	{VA_WDOG_WDOG_TMRPERIOD, DATA_WDOG_WDOG_TMRPERIOD, dump_wdog_wdog_tmrperiod, "WDOG_WDOG_TMRPERIOD", 0},
	{VA_WDOG_WDOG_CNT_CUR_VAL, DATA_WDOG_WDOG_CNT_CUR_VAL, dump_wdog_wdog_cnt_cur_val, "WDOG_WDOG_CNT_CUR_VAL", 0},
	{VA_WDOG_WDOG_RESTART, DATA_WDOG_WDOG_RESTART, dump_wdog_wdog_restart, "WDOG_WDOG_RESTART", 0},
	{VA_WDOG_WDOG_INT_EN, DATA_WDOG_WDOG_INT_EN, dump_wdog_wdog_int_en, "WDOG_WDOG_INT_EN", 0},
	{VA_WDOG_WDOG_CLR_INT_PENDING, DATA_WDOG_WDOG_CLR_INT_PENDING, dump_wdog_wdog_clr_int_pending, "WDOG_WDOG_CLR_INT_PENDING", 0},
	{VA_WDOG_WDOG_INT_PENDING, DATA_WDOG_WDOG_INT_PENDING, dump_wdog_wdog_int_pending, "WDOG_WDOG_INT_PENDING", 0},
};

void dump_wdog(u32 mode)
{
	u32 cnt = sizeof(wdog_info) / sizeof(wdog_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					wdog_info[i].name,
					wdog_info[i].addr,
					wdog_info[i].reset);
		def_strcat(buffer, sz_temp);

		wdog_info[i].dump(wdog_info[i].addr, wdog_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_wdog_wdog_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_ver_t reg;

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

u32 dump_wdog_wdog_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:sel\n", reg.bits.sel);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_wdog_wdog_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rmod\n", reg.bits.rmod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rpl\n", reg.bits.rpl);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:su\n", reg.bits.su);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_wdog_wdog_tmrperiod(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_tmrperiod_t reg;

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

	def_sprintf(sz_temp, "0x%X:tmrperiod\n", reg.bits.tmrperiod);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_wdog_wdog_cnt_cur_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_cnt_cur_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:cnt_cur_val\n", reg.bits.cnt_cur_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_wdog_wdog_restart(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_restart_t reg;

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

	def_sprintf(sz_temp, "0x%X:restart\n", reg.bits.restart);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_wdog_wdog_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_int_en_t reg;

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

	def_sprintf(sz_temp, "0x%X:enable\n", reg.bits.enable);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_wdog_wdog_clr_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_clr_int_pending_t reg;

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

	def_sprintf(sz_temp, "0x%X:clr\n", reg.bits.clr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_wdog_wdog_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_wdog_wdog_int_pending_t reg;

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

	def_sprintf(sz_temp, "0x%X:pending\n", reg.bits.pending);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___WDOG___DUMP__C___ */
