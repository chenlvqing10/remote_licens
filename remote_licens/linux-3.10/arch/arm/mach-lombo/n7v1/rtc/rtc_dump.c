/* rtc_dump.c */

#ifndef ___RTC___DUMP__C___
#define ___RTC___DUMP__C___

reg_info_t rtc_info[] = {
	{VA_RTC_VER, DATA_RTC_VER, dump_rtc_ver, "RTC_VER", 0},
	{VA_RTC_RTC_CLK_CTRL, DATA_RTC_RTC_CLK_CTRL, dump_rtc_rtc_clk_ctrl, "RTC_RTC_CLK_CTRL", 0},
	{VA_RTC_RTC_LD_EN, DATA_RTC_RTC_LD_EN, dump_rtc_rtc_ld_en, "RTC_RTC_LD_EN", 0},
	{VA_RTC_RTC_LD_INT_EN, DATA_RTC_RTC_LD_INT_EN, dump_rtc_rtc_ld_int_en, "RTC_RTC_LD_INT_EN", 0},
	{VA_RTC_RTC_LD_INT_CLR, DATA_RTC_RTC_LD_INT_CLR, dump_rtc_rtc_ld_int_clr, "RTC_RTC_LD_INT_CLR", 0},
	{VA_RTC_RTC_LD_INT_PENDING, DATA_RTC_RTC_LD_INT_PENDING, dump_rtc_rtc_ld_int_pending, "RTC_RTC_LD_INT_PENDING", 0},
	{VA_RTC_RTC_YMD, DATA_RTC_RTC_YMD, dump_rtc_rtc_ymd, "RTC_RTC_YMD", 0},
	{VA_RTC_RTC_WEEK, DATA_RTC_RTC_WEEK, dump_rtc_rtc_week, "RTC_RTC_WEEK", 0},
	{VA_RTC_RTC_HMS, DATA_RTC_RTC_HMS, dump_rtc_rtc_hms, "RTC_RTC_HMS", 0},
	{VA_RTC_RTC_ALARM_INT_EN, DATA_RTC_RTC_ALARM_INT_EN, dump_rtc_rtc_alarm_int_en, "RTC_RTC_ALARM_INT_EN", 0},
	{VA_RTC_RTC_ALARM_INT_CLR, DATA_RTC_RTC_ALARM_INT_CLR, dump_rtc_rtc_alarm_int_clr, "RTC_RTC_ALARM_INT_CLR", 0},
	{VA_RTC_RTC_ALARM_INT_PENDING, DATA_RTC_RTC_ALARM_INT_PENDING, dump_rtc_rtc_alarm_int_pending, "RTC_RTC_ALARM_INT_PENDING", 0},
	{VA_RTC_RTC_ALARM_MATCH_EN, DATA_RTC_RTC_ALARM_MATCH_EN, dump_rtc_rtc_alarm_match_en, "RTC_RTC_ALARM_MATCH_EN", 0},
	{VA_RTC_RTC_ALARM_DAY_MATCH, DATA_RTC_RTC_ALARM_DAY_MATCH, dump_rtc_rtc_alarm_day_match, "RTC_RTC_ALARM_DAY_MATCH", 0},
	{VA_RTC_RTC_ALARM_WEEK_MATCH, DATA_RTC_RTC_ALARM_WEEK_MATCH, dump_rtc_rtc_alarm_week_match, "RTC_RTC_ALARM_WEEK_MATCH", 0},
	{VA_RTC_RTC_ALARM_HMS_MATCH, DATA_RTC_RTC_ALARM_HMS_MATCH, dump_rtc_rtc_alarm_hms_match, "RTC_RTC_ALARM_HMS_MATCH", 0},
	{VA_RTC_RTC_WAKEUP_EN, DATA_RTC_RTC_WAKEUP_EN, dump_rtc_rtc_wakeup_en, "RTC_RTC_WAKEUP_EN", 0},
	{VA_RTC_RTC_LFEOSC_FANOUT_CFG, DATA_RTC_RTC_LFEOSC_FANOUT_CFG, dump_rtc_rtc_lfeosc_fanout_cfg, "RTC_RTC_LFEOSC_FANOUT_CFG", 0},
	{VA_RTC_RTC_SU_EN, DATA_RTC_RTC_SU_EN, dump_rtc_rtc_su_en, "RTC_RTC_SU_EN", 0},
	{VA_RTC_RTC_CORE, DATA_RTC_RTC_CORE, dump_rtc_rtc_core, "RTC_RTC_CORE", 0},
	{VA_RTC_RTC_TOP, DATA_RTC_RTC_TOP, dump_rtc_rtc_top, "RTC_RTC_TOP", 0},
	{VA_RTC_RTC_HFEOSC, DATA_RTC_RTC_HFEOSC, dump_rtc_rtc_hfeosc, "RTC_RTC_HFEOSC", 0},
	{VA_RTC_RTC_LFEOSC, DATA_RTC_RTC_LFEOSC, dump_rtc_rtc_lfeosc, "RTC_RTC_LFEOSC", 0},
	{VA_RTC_RTC_ALIVE_REG0, DATA_RTC_RTC_ALIVE_REG0, dump_rtc_rtc_alive_reg0, "RTC_RTC_ALIVE_REG0", 0},
	{VA_RTC_RTC_ALIVE_REG1, DATA_RTC_RTC_ALIVE_REG1, dump_rtc_rtc_alive_reg1, "RTC_RTC_ALIVE_REG1", 0},
	{VA_RTC_RTC_ALIVE_REG2, DATA_RTC_RTC_ALIVE_REG2, dump_rtc_rtc_alive_reg2, "RTC_RTC_ALIVE_REG2", 0},
	{VA_RTC_RTC_ALIVE_REG3, DATA_RTC_RTC_ALIVE_REG3, dump_rtc_rtc_alive_reg3, "RTC_RTC_ALIVE_REG3", 0},
	{VA_RTC_PM_PE, DATA_RTC_PM_PE, dump_rtc_pm_pe, "RTC_PM_PE", 0},
	{VA_RTC_PM_CFG, DATA_RTC_PM_CFG, dump_rtc_pm_cfg, "RTC_PM_CFG", 0},
	{VA_RTC_PM_INT_EN, DATA_RTC_PM_INT_EN, dump_rtc_pm_int_en, "RTC_PM_INT_EN", 0},
	{VA_RTC_PM_PEND_CLR, DATA_RTC_PM_PEND_CLR, dump_rtc_pm_pend_clr, "RTC_PM_PEND_CLR", 0},
	{VA_RTC_PM_PEND, DATA_RTC_PM_PEND, dump_rtc_pm_pend, "RTC_PM_PEND", 0},
	{VA_RTC_PM_PE1, DATA_RTC_PM_PE1, dump_rtc_pm_pe1, "RTC_PM_PE1", 0},
	{VA_RTC_PM_STAT, DATA_RTC_PM_STAT, dump_rtc_pm_stat, "RTC_PM_STAT", 0},
	{VA_RTC_PM_PKT, DATA_RTC_PM_PKT, dump_rtc_pm_pkt, "RTC_PM_PKT", 0},
	{VA_RTC_PM_PE2, DATA_RTC_PM_PE2, dump_rtc_pm_pe2, "RTC_PM_PE2", 0},
	{VA_RTC_LOCK_CTRL, DATA_RTC_LOCK_CTRL, dump_rtc_lock_ctrl, "RTC_LOCK_CTRL", 0},
	{VA_RTC_DEEP_SLP_FLAG, DATA_RTC_DEEP_SLP_FLAG, dump_rtc_deep_slp_flag, "RTC_DEEP_SLP_FLAG", 0},
	{VA_RTC_DEEP_SLP_EXIT, DATA_RTC_DEEP_SLP_EXIT, dump_rtc_deep_slp_exit, "RTC_DEEP_SLP_EXIT", 0},
	{VA_RTC_HOT_RST_FLAG, DATA_RTC_HOT_RST_FLAG, dump_rtc_hot_rst_flag, "RTC_HOT_RST_FLAG", 0},
	{VA_RTC_HOT_RST_EXIT, DATA_RTC_HOT_RST_EXIT, dump_rtc_hot_rst_exit, "RTC_HOT_RST_EXIT", 0},
	{VA_RTC_PAD_HOLD, DATA_RTC_PAD_HOLD, dump_rtc_pad_hold, "RTC_PAD_HOLD", 0},
	{VA_RTC_SYS_STAT, DATA_RTC_SYS_STAT, dump_rtc_sys_stat, "RTC_SYS_STAT", 0},
	{VA_RTC_AX_CLAMP, DATA_RTC_AX_CLAMP, dump_rtc_ax_clamp, "RTC_AX_CLAMP", 0},
	{VA_RTC_SIO_FUNC_R0, DATA_RTC_SIO_FUNC_R0, dump_rtc_sio_func_r0, "RTC_SIO_FUNC_R0", 0},
	{VA_RTC_SIO_PUD_R0, DATA_RTC_SIO_PUD_R0, dump_rtc_sio_pud_r0, "RTC_SIO_PUD_R0", 0},
	{VA_RTC_SIO_DRV_R0, DATA_RTC_SIO_DRV_R0, dump_rtc_sio_drv_r0, "RTC_SIO_DRV_R0", 0},
	{VA_RTC_SIO_DATA_R0, DATA_RTC_SIO_DATA_R0, dump_rtc_sio_data_r0, "RTC_SIO_DATA_R0", 0},
	{VA_RTC_SIO_PR_R, DATA_RTC_SIO_PR_R, dump_rtc_sio_pr_r, "RTC_SIO_PR_R", 0},
	{VA_RTC_SIO_INT_TRI_R0, DATA_RTC_SIO_INT_TRI_R0, dump_rtc_sio_int_tri_r0, "RTC_SIO_INT_TRI_R0", 0},
	{VA_RTC_SIO_INT_MASK0, DATA_RTC_SIO_INT_MASK0, dump_rtc_sio_int_mask0, "RTC_SIO_INT_MASK0", 0},
	{VA_RTC_SIO_INT_PEND0, DATA_RTC_SIO_INT_PEND0, dump_rtc_sio_int_pend0, "RTC_SIO_INT_PEND0", 0},
	{VA_RTC_SIO_INT_SAMP_CTRL0, DATA_RTC_SIO_INT_SAMP_CTRL0, dump_rtc_sio_int_samp_ctrl0, "RTC_SIO_INT_SAMP_CTRL0", 0},
};

void dump_rtc(u32 mode)
{
	u32 cnt = sizeof(rtc_info) / sizeof(rtc_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					rtc_info[i].name,
					rtc_info[i].addr,
					rtc_info[i].reset);
		def_strcat(buffer, sz_temp);

		rtc_info[i].dump(rtc_info[i].addr, rtc_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_rtc_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_ver_t reg;

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

u32 dump_rtc_rtc_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_clk_ctrl_t reg;

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
	def_sprintf(sz_temp, "0x%X:stat\n", reg.bits.stat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rcosc_div\n", reg.bits.rcosc_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_field\n", reg.bits.key_field);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_ld_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_ld_en_t reg;

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

	return 0;
}

u32 dump_rtc_rtc_ld_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_ld_int_en_t reg;

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

	return 0;
}

u32 dump_rtc_rtc_ld_int_clr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_ld_int_clr_t reg;

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

u32 dump_rtc_rtc_ld_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_ld_int_pending_t reg;

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

u32 dump_rtc_rtc_ymd(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_ymd_t reg;

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

	def_sprintf(sz_temp, "0x%X:day\n", reg.bits.day);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:month\n", reg.bits.month);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:year\n", reg.bits.year);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_week(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_week_t reg;

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

	def_sprintf(sz_temp, "0x%X:week\n", reg.bits.week);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_hms(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_hms_t reg;

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

	def_sprintf(sz_temp, "0x%X:second\n", reg.bits.second);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:minute\n", reg.bits.minute);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hour\n", reg.bits.hour);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_alarm_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alarm_int_en_t reg;

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

	return 0;
}

u32 dump_rtc_rtc_alarm_int_clr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alarm_int_clr_t reg;

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

u32 dump_rtc_rtc_alarm_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alarm_int_pending_t reg;

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

u32 dump_rtc_rtc_alarm_match_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alarm_match_en_t reg;

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

	def_sprintf(sz_temp, "0x%X:sec_alarm_match_en\n", reg.bits.sec_alarm_match_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:min_alarm_match_en\n", reg.bits.min_alarm_match_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hour_alarm_match_en\n", reg.bits.hour_alarm_match_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:week_alarm_match_en\n", reg.bits.week_alarm_match_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:day_alarm_match_en\n", reg.bits.day_alarm_match_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_alarm_day_match(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alarm_day_match_t reg;

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

	def_sprintf(sz_temp, "0x%X:day_match\n", reg.bits.day_match);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_alarm_week_match(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alarm_week_match_t reg;

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

	def_sprintf(sz_temp, "0x%X:week_match\n", reg.bits.week_match);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_alarm_hms_match(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alarm_hms_match_t reg;

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

	def_sprintf(sz_temp, "0x%X:second_match\n", reg.bits.second_match);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:minute_match\n", reg.bits.minute_match);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hour_match\n", reg.bits.hour_match);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_wakeup_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_wakeup_en_t reg;

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

	def_sprintf(sz_temp, "0x%X:wakeup_en\n", reg.bits.wakeup_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_lfeosc_fanout_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_lfeosc_fanout_cfg_t reg;

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

	def_sprintf(sz_temp, "0x%X:func\n", reg.bits.func);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lfeosc_fanout_en\n", reg.bits.lfeosc_fanout_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_su_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_su_en_t reg;

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
	def_sprintf(sz_temp, "0x%X:rate\n", reg.bits.rate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_core(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_core_t reg;

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

	def_sprintf(sz_temp, "0x%X:osc1_en\n", reg.bits.osc1_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:vref_sel\n", reg.bits.vref_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ldo_sel\n", reg.bits.ldo_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div_sel\n", reg.bits.div_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div_en\n", reg.bits.div_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tsd\n", reg.bits.tsd);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tsa\n", reg.bits.tsa);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bg1_en\n", reg.bits.bg1_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bg0_en\n", reg.bits.bg0_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_top(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_top_t reg;

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

	def_sprintf(sz_temp, "0x%X:eno\n", reg.bits.eno);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ad\n", reg.bits.ad);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:seld\n", reg.bits.seld);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_hfeosc(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_hfeosc_t reg;

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
	def_sprintf(sz_temp, "0x%X:capen\n", reg.bits.capen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:smtsel\n", reg.bits.smtsel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:smten\n", reg.bits.smten);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lpfen\n", reg.bits.lpfen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:capsel_i\n", reg.bits.capsel_i);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:capsel_o\n", reg.bits.capsel_o);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gmsel\n", reg.bits.gmsel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rsel\n", reg.bits.rsel);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_lfeosc(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_lfeosc_t reg;

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
	def_sprintf(sz_temp, "0x%X:capen\n", reg.bits.capen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:smtsel\n", reg.bits.smtsel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:smten\n", reg.bits.smten);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lpfen\n", reg.bits.lpfen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:capsel_i\n", reg.bits.capsel_i);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:capsel_o\n", reg.bits.capsel_o);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gmsel\n", reg.bits.gmsel);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_alive_reg0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alive_reg0_t reg;

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

	def_sprintf(sz_temp, "0x%X:reg\n", reg.bits.reg);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_alive_reg1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alive_reg1_t reg;

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

	def_sprintf(sz_temp, "0x%X:reg\n", reg.bits.reg);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_alive_reg2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alive_reg2_t reg;

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

	def_sprintf(sz_temp, "0x%X:reg\n", reg.bits.reg);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_rtc_alive_reg3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_rtc_alive_reg3_t reg;

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

	def_sprintf(sz_temp, "0x%X:reg\n", reg.bits.reg);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_pe(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_pe_t reg;

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
	def_sprintf(sz_temp, "0x%X:ddr_en\n", reg.bits.ddr_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_filed\n", reg.bits.key_filed);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_cfg(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_cfg_t reg;

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

	def_sprintf(sz_temp, "0x%X:g_int_trig\n", reg.bits.g_int_trig);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pir_wake_trig\n", reg.bits.pir_wake_trig);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ring_key_trig\n", reg.bits.ring_key_trig);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gs_wake_detect_en\n", reg.bits.gs_wake_detect_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pir_wake_detect_en\n", reg.bits.pir_wake_detect_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ring_key_wake_det_en\n", reg.bits.ring_key_wake_det_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_slong_en\n", reg.bits.key_slong_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_filed\n", reg.bits.key_filed);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_int_en_t reg;

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

	def_sprintf(sz_temp, "0x%X:pwr_en\n", reg.bits.pwr_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_press\n", reg.bits.key_press);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_short\n", reg.bits.key_short);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_long\n", reg.bits.key_long);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_release\n", reg.bits.key_release);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gs_int\n", reg.bits.gs_int);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pir_wake\n", reg.bits.pir_wake);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ring_key\n", reg.bits.ring_key);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_pend_clr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_pend_clr_t reg;

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

	def_sprintf(sz_temp, "0x%X:pw_con\n", reg.bits.pw_con);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pw_discon\n", reg.bits.pw_discon);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_press\n", reg.bits.key_press);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_short\n", reg.bits.key_short);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_long\n", reg.bits.key_long);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_release\n", reg.bits.key_release);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gs_int\n", reg.bits.gs_int);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pir_wake\n", reg.bits.pir_wake);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ring_key\n", reg.bits.ring_key);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_pend_t reg;

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

	def_sprintf(sz_temp, "0x%X:pw_con\n", reg.bits.pw_con);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pw_discon\n", reg.bits.pw_discon);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_press\n", reg.bits.key_press);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_short\n", reg.bits.key_short);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_long\n", reg.bits.key_long);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_release\n", reg.bits.key_release);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gs_int\n", reg.bits.gs_int);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pir_wake\n", reg.bits.pir_wake);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ring_key\n", reg.bits.ring_key);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_pe1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_pe1_t reg;

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
	def_sprintf(sz_temp, "0x%X:pol\n", reg.bits.pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tp\n", reg.bits.tp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ap\n", reg.bits.ap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_stat_t reg;

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

	def_sprintf(sz_temp, "0x%X:pwr_con\n", reg.bits.pwr_con);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_pkt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_pkt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pwr_up\n", reg.bits.pwr_up);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_long\n", reg.bits.key_long);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_slong\n", reg.bits.key_slong);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pm_pe2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pm_pe2_t reg;

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

	def_sprintf(sz_temp, "0x%X:swi\n", reg.bits.swi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:key_filed\n", reg.bits.key_filed);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_lock_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_lock_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:deep_slp_exit\n", reg.bits.deep_slp_exit);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:deep_slp_flag\n", reg.bits.deep_slp_flag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hot_rst_exit\n", reg.bits.hot_rst_exit);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hot_rst_flag\n", reg.bits.hot_rst_flag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pad_hold\n", reg.bits.pad_hold);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sys_stat\n", reg.bits.sys_stat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:keyfield\n", reg.bits.keyfield);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_deep_slp_flag(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_deep_slp_flag_t reg;

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

	def_sprintf(sz_temp, "0x%X:flag\n", reg.bits.flag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_deep_slp_exit(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_deep_slp_exit_t reg;

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

	def_sprintf(sz_temp, "0x%X:exit_addr\n", reg.bits.exit_addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_hot_rst_flag(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_hot_rst_flag_t reg;

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

	def_sprintf(sz_temp, "0x%X:flag\n", reg.bits.flag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_hot_rst_exit(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_hot_rst_exit_t reg;

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

	def_sprintf(sz_temp, "0x%X:exit_addr\n", reg.bits.exit_addr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_pad_hold(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_pad_hold_t reg;

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

	def_sprintf(sz_temp, "0x%X:ddr_pad\n", reg.bits.ddr_pad);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sys_stat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sys_stat_t reg;

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

	def_sprintf(sz_temp, "0x%X:stat\n", reg.bits.stat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_ax_clamp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_ax_clamp_t reg;

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

	def_sprintf(sz_temp, "0x%X:ax\n", reg.bits.ax);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:sio0\n", reg.bits.sio0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio1\n", reg.bits.sio1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio2\n", reg.bits.sio2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio3\n", reg.bits.sio3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio4\n", reg.bits.sio4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio5\n", reg.bits.sio5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio6\n", reg.bits.sio6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio7\n", reg.bits.sio7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:sio0\n", reg.bits.sio0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio1\n", reg.bits.sio1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio2\n", reg.bits.sio2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio3\n", reg.bits.sio3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio4\n", reg.bits.sio4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio5\n", reg.bits.sio5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio6\n", reg.bits.sio6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio7\n", reg.bits.sio7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:sio0\n", reg.bits.sio0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio1\n", reg.bits.sio1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio2\n", reg.bits.sio2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio3\n", reg.bits.sio3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio4\n", reg.bits.sio4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio5\n", reg.bits.sio5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio6\n", reg.bits.sio6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio7\n", reg.bits.sio7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_data_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_data_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:sio_data\n", reg.bits.sio_data);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_pr_r_t reg;

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

	def_sprintf(sz_temp, "0x%X:pull_cfg\n", reg.bits.pull_cfg);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_int_tri_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_int_tri_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:sio0\n", reg.bits.sio0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio1\n", reg.bits.sio1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio2\n", reg.bits.sio2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio3\n", reg.bits.sio3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio4\n", reg.bits.sio4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio5\n", reg.bits.sio5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio6\n", reg.bits.sio6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sio7\n", reg.bits.sio7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_int_mask0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_int_mask0_t reg;

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

	def_sprintf(sz_temp, "0x%X:sio_int_mask\n", reg.bits.sio_int_mask);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_int_pend0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_int_pend0_t reg;

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

	def_sprintf(sz_temp, "0x%X:sio_int_pend\n", reg.bits.sio_int_pend);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_rtc_sio_int_samp_ctrl0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_rtc_sio_int_samp_ctrl0_t reg;

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

	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en\n", reg.bits.en);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___RTC___DUMP__C___ */
