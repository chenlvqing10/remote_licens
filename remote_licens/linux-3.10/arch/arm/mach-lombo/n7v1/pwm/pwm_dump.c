/* pwm_dump.c */

#ifndef ___PWM___DUMP__C___
#define ___PWM___DUMP__C___

reg_info_t pwm_info[] = {
	{VA_PWM_VER, DATA_PWM_VER, dump_pwm_ver, "PWM_VER", 0},
	{VA_PWM_PWM_CE, DATA_PWM_PWM_CE, dump_pwm_pwm_ce, "PWM_PWM_CE", 0},
	{VA_PWM_PWM_PE, DATA_PWM_PWM_PE, dump_pwm_pwm_pe, "PWM_PWM_PE", 0},
	{VA_PWM_PWM_INT_EN, DATA_PWM_PWM_INT_EN, dump_pwm_pwm_int_en, "PWM_PWM_INT_EN", 0},
	{VA_PWM_PWM_CLR_INT_PENDING, DATA_PWM_PWM_CLR_INT_PENDING, dump_pwm_pwm_clr_int_pending, "PWM_PWM_CLR_INT_PENDING", 0},
	{VA_PWM_PWM_INT_PENDING, DATA_PWM_PWM_INT_PENDING, dump_pwm_pwm_int_pending, "PWM_PWM_INT_PENDING", 0},
	{VA_PWM_PWM0_CLK_CTRL, DATA_PWM_PWM0_CLK_CTRL, dump_pwm_pwm0_clk_ctrl, "PWM_PWM0_CLK_CTRL", 0},
	{VA_PWM_PWM1_CLK_CTRL, DATA_PWM_PWM1_CLK_CTRL, dump_pwm_pwm1_clk_ctrl, "PWM_PWM1_CLK_CTRL", 0},
	{VA_PWM_PWM2_CLK_CTRL, DATA_PWM_PWM2_CLK_CTRL, dump_pwm_pwm2_clk_ctrl, "PWM_PWM2_CLK_CTRL", 0},
	{VA_PWM_PWM3_CLK_CTRL, DATA_PWM_PWM3_CLK_CTRL, dump_pwm_pwm3_clk_ctrl, "PWM_PWM3_CLK_CTRL", 0},
	{VA_PWM_PWM4_CLK_CTRL, DATA_PWM_PWM4_CLK_CTRL, dump_pwm_pwm4_clk_ctrl, "PWM_PWM4_CLK_CTRL", 0},
	{VA_PWM_PWM5_CLK_CTRL, DATA_PWM_PWM5_CLK_CTRL, dump_pwm_pwm5_clk_ctrl, "PWM_PWM5_CLK_CTRL", 0},
	{VA_PWM_PWM6_CLK_CTRL, DATA_PWM_PWM6_CLK_CTRL, dump_pwm_pwm6_clk_ctrl, "PWM_PWM6_CLK_CTRL", 0},
	{VA_PWM_PWM7_CLK_CTRL, DATA_PWM_PWM7_CLK_CTRL, dump_pwm_pwm7_clk_ctrl, "PWM_PWM7_CLK_CTRL", 0},
	{VA_PWM_PWM8_CLK_CTRL, DATA_PWM_PWM8_CLK_CTRL, dump_pwm_pwm8_clk_ctrl, "PWM_PWM8_CLK_CTRL", 0},
	{VA_PWM_PWM9_CLK_CTRL, DATA_PWM_PWM9_CLK_CTRL, dump_pwm_pwm9_clk_ctrl, "PWM_PWM9_CLK_CTRL", 0},
	{VA_PWM_PWM0_CNT_STORE, DATA_PWM_PWM0_CNT_STORE, dump_pwm_pwm0_cnt_store, "PWM_PWM0_CNT_STORE", 0},
	{VA_PWM_PWM1_CNT_STORE, DATA_PWM_PWM1_CNT_STORE, dump_pwm_pwm1_cnt_store, "PWM_PWM1_CNT_STORE", 0},
	{VA_PWM_PWM2_CNT_STORE, DATA_PWM_PWM2_CNT_STORE, dump_pwm_pwm2_cnt_store, "PWM_PWM2_CNT_STORE", 0},
	{VA_PWM_PWM3_CNT_STORE, DATA_PWM_PWM3_CNT_STORE, dump_pwm_pwm3_cnt_store, "PWM_PWM3_CNT_STORE", 0},
	{VA_PWM_PWM4_CNT_STORE, DATA_PWM_PWM4_CNT_STORE, dump_pwm_pwm4_cnt_store, "PWM_PWM4_CNT_STORE", 0},
	{VA_PWM_PWM5_CNT_STORE, DATA_PWM_PWM5_CNT_STORE, dump_pwm_pwm5_cnt_store, "PWM_PWM5_CNT_STORE", 0},
	{VA_PWM_PWM6_CNT_STORE, DATA_PWM_PWM6_CNT_STORE, dump_pwm_pwm6_cnt_store, "PWM_PWM6_CNT_STORE", 0},
	{VA_PWM_PWM7_CNT_STORE, DATA_PWM_PWM7_CNT_STORE, dump_pwm_pwm7_cnt_store, "PWM_PWM7_CNT_STORE", 0},
	{VA_PWM_PWM8_CNT_STORE, DATA_PWM_PWM8_CNT_STORE, dump_pwm_pwm8_cnt_store, "PWM_PWM8_CNT_STORE", 0},
	{VA_PWM_PWM9_CNT_STORE, DATA_PWM_PWM9_CNT_STORE, dump_pwm_pwm9_cnt_store, "PWM_PWM9_CNT_STORE", 0},
	{VA_PWM_PWM0_CNT_CMP, DATA_PWM_PWM0_CNT_CMP, dump_pwm_pwm0_cnt_cmp, "PWM_PWM0_CNT_CMP", 0},
	{VA_PWM_PWM1_CNT_CMP, DATA_PWM_PWM1_CNT_CMP, dump_pwm_pwm1_cnt_cmp, "PWM_PWM1_CNT_CMP", 0},
	{VA_PWM_PWM2_CNT_CMP, DATA_PWM_PWM2_CNT_CMP, dump_pwm_pwm2_cnt_cmp, "PWM_PWM2_CNT_CMP", 0},
	{VA_PWM_PWM3_CNT_CMP, DATA_PWM_PWM3_CNT_CMP, dump_pwm_pwm3_cnt_cmp, "PWM_PWM3_CNT_CMP", 0},
	{VA_PWM_PWM4_CNT_CMP, DATA_PWM_PWM4_CNT_CMP, dump_pwm_pwm4_cnt_cmp, "PWM_PWM4_CNT_CMP", 0},
	{VA_PWM_PWM5_CNT_CMP, DATA_PWM_PWM5_CNT_CMP, dump_pwm_pwm5_cnt_cmp, "PWM_PWM5_CNT_CMP", 0},
	{VA_PWM_PWM6_CNT_CMP, DATA_PWM_PWM6_CNT_CMP, dump_pwm_pwm6_cnt_cmp, "PWM_PWM6_CNT_CMP", 0},
	{VA_PWM_PWM7_CNT_CMP, DATA_PWM_PWM7_CNT_CMP, dump_pwm_pwm7_cnt_cmp, "PWM_PWM7_CNT_CMP", 0},
	{VA_PWM_PWM8_CNT_CMP, DATA_PWM_PWM8_CNT_CMP, dump_pwm_pwm8_cnt_cmp, "PWM_PWM8_CNT_CMP", 0},
	{VA_PWM_PWM9_CNT_CMP, DATA_PWM_PWM9_CNT_CMP, dump_pwm_pwm9_cnt_cmp, "PWM_PWM9_CNT_CMP", 0},
	{VA_PWM_PWM0_CTRL, DATA_PWM_PWM0_CTRL, dump_pwm_pwm0_ctrl, "PWM_PWM0_CTRL", 0},
	{VA_PWM_PWM1_CTRL, DATA_PWM_PWM1_CTRL, dump_pwm_pwm1_ctrl, "PWM_PWM1_CTRL", 0},
	{VA_PWM_PWM2_CTRL, DATA_PWM_PWM2_CTRL, dump_pwm_pwm2_ctrl, "PWM_PWM2_CTRL", 0},
	{VA_PWM_PWM3_CTRL, DATA_PWM_PWM3_CTRL, dump_pwm_pwm3_ctrl, "PWM_PWM3_CTRL", 0},
	{VA_PWM_PWM4_CTRL, DATA_PWM_PWM4_CTRL, dump_pwm_pwm4_ctrl, "PWM_PWM4_CTRL", 0},
	{VA_PWM_PWM5_CTRL, DATA_PWM_PWM5_CTRL, dump_pwm_pwm5_ctrl, "PWM_PWM5_CTRL", 0},
	{VA_PWM_PWM6_CTRL, DATA_PWM_PWM6_CTRL, dump_pwm_pwm6_ctrl, "PWM_PWM6_CTRL", 0},
	{VA_PWM_PWM7_CTRL, DATA_PWM_PWM7_CTRL, dump_pwm_pwm7_ctrl, "PWM_PWM7_CTRL", 0},
	{VA_PWM_PWM8_CTRL, DATA_PWM_PWM8_CTRL, dump_pwm_pwm8_ctrl, "PWM_PWM8_CTRL", 0},
	{VA_PWM_PWM9_CTRL, DATA_PWM_PWM9_CTRL, dump_pwm_pwm9_ctrl, "PWM_PWM9_CTRL", 0},
	{VA_PWM_PWM0_CNT_CAP, DATA_PWM_PWM0_CNT_CAP, dump_pwm_pwm0_cnt_cap, "PWM_PWM0_CNT_CAP", 0},
	{VA_PWM_PWM1_CNT_CAP, DATA_PWM_PWM1_CNT_CAP, dump_pwm_pwm1_cnt_cap, "PWM_PWM1_CNT_CAP", 0},
	{VA_PWM_PWM2_CNT_CAP, DATA_PWM_PWM2_CNT_CAP, dump_pwm_pwm2_cnt_cap, "PWM_PWM2_CNT_CAP", 0},
	{VA_PWM_PWM3_CNT_CAP, DATA_PWM_PWM3_CNT_CAP, dump_pwm_pwm3_cnt_cap, "PWM_PWM3_CNT_CAP", 0},
	{VA_PWM_PWM4_CNT_CAP, DATA_PWM_PWM4_CNT_CAP, dump_pwm_pwm4_cnt_cap, "PWM_PWM4_CNT_CAP", 0},
	{VA_PWM_PWM5_CNT_CAP, DATA_PWM_PWM5_CNT_CAP, dump_pwm_pwm5_cnt_cap, "PWM_PWM5_CNT_CAP", 0},
	{VA_PWM_PWM6_CNT_CAP, DATA_PWM_PWM6_CNT_CAP, dump_pwm_pwm6_cnt_cap, "PWM_PWM6_CNT_CAP", 0},
	{VA_PWM_PWM7_CNT_CAP, DATA_PWM_PWM7_CNT_CAP, dump_pwm_pwm7_cnt_cap, "PWM_PWM7_CNT_CAP", 0},
	{VA_PWM_PWM8_CNT_CAP, DATA_PWM_PWM8_CNT_CAP, dump_pwm_pwm8_cnt_cap, "PWM_PWM8_CNT_CAP", 0},
	{VA_PWM_PWM9_CNT_CAP, DATA_PWM_PWM9_CNT_CAP, dump_pwm_pwm9_cnt_cap, "PWM_PWM9_CNT_CAP", 0},
	{VA_PWM_PWM0_IN_STS, DATA_PWM_PWM0_IN_STS, dump_pwm_pwm0_in_sts, "PWM_PWM0_IN_STS", 0},
	{VA_PWM_PWM1_IN_STS, DATA_PWM_PWM1_IN_STS, dump_pwm_pwm1_in_sts, "PWM_PWM1_IN_STS", 0},
	{VA_PWM_PWM2_IN_STS, DATA_PWM_PWM2_IN_STS, dump_pwm_pwm2_in_sts, "PWM_PWM2_IN_STS", 0},
	{VA_PWM_PWM3_IN_STS, DATA_PWM_PWM3_IN_STS, dump_pwm_pwm3_in_sts, "PWM_PWM3_IN_STS", 0},
	{VA_PWM_PWM4_IN_STS, DATA_PWM_PWM4_IN_STS, dump_pwm_pwm4_in_sts, "PWM_PWM4_IN_STS", 0},
	{VA_PWM_PWM5_IN_STS, DATA_PWM_PWM5_IN_STS, dump_pwm_pwm5_in_sts, "PWM_PWM5_IN_STS", 0},
	{VA_PWM_PWM6_IN_STS, DATA_PWM_PWM6_IN_STS, dump_pwm_pwm6_in_sts, "PWM_PWM6_IN_STS", 0},
	{VA_PWM_PWM7_IN_STS, DATA_PWM_PWM7_IN_STS, dump_pwm_pwm7_in_sts, "PWM_PWM7_IN_STS", 0},
	{VA_PWM_PWM8_IN_STS, DATA_PWM_PWM8_IN_STS, dump_pwm_pwm8_in_sts, "PWM_PWM8_IN_STS", 0},
	{VA_PWM_PWM9_IN_STS, DATA_PWM_PWM9_IN_STS, dump_pwm_pwm9_in_sts, "PWM_PWM9_IN_STS", 0},
	{VA_PWM_PWM0_DLY_CNT, DATA_PWM_PWM0_DLY_CNT, dump_pwm_pwm0_dly_cnt, "PWM_PWM0_DLY_CNT", 0},
	{VA_PWM_PWM1_DLY_CNT, DATA_PWM_PWM1_DLY_CNT, dump_pwm_pwm1_dly_cnt, "PWM_PWM1_DLY_CNT", 0},
	{VA_PWM_PWM2_DLY_CNT, DATA_PWM_PWM2_DLY_CNT, dump_pwm_pwm2_dly_cnt, "PWM_PWM2_DLY_CNT", 0},
	{VA_PWM_PWM3_DLY_CNT, DATA_PWM_PWM3_DLY_CNT, dump_pwm_pwm3_dly_cnt, "PWM_PWM3_DLY_CNT", 0},
	{VA_PWM_PWM4_DLY_CNT, DATA_PWM_PWM4_DLY_CNT, dump_pwm_pwm4_dly_cnt, "PWM_PWM4_DLY_CNT", 0},
	{VA_PWM_PWM5_DLY_CNT, DATA_PWM_PWM5_DLY_CNT, dump_pwm_pwm5_dly_cnt, "PWM_PWM5_DLY_CNT", 0},
	{VA_PWM_PWM6_DLY_CNT, DATA_PWM_PWM6_DLY_CNT, dump_pwm_pwm6_dly_cnt, "PWM_PWM6_DLY_CNT", 0},
	{VA_PWM_PWM7_DLY_CNT, DATA_PWM_PWM7_DLY_CNT, dump_pwm_pwm7_dly_cnt, "PWM_PWM7_DLY_CNT", 0},
	{VA_PWM_PWM8_DLY_CNT, DATA_PWM_PWM8_DLY_CNT, dump_pwm_pwm8_dly_cnt, "PWM_PWM8_DLY_CNT", 0},
	{VA_PWM_PWM9_DLY_CNT, DATA_PWM_PWM9_DLY_CNT, dump_pwm_pwm9_dly_cnt, "PWM_PWM9_DLY_CNT", 0},
	{VA_PWM_PWM0_PULSE_CNT, DATA_PWM_PWM0_PULSE_CNT, dump_pwm_pwm0_pulse_cnt, "PWM_PWM0_PULSE_CNT", 0},
	{VA_PWM_PWM1_PULSE_CNT, DATA_PWM_PWM1_PULSE_CNT, dump_pwm_pwm1_pulse_cnt, "PWM_PWM1_PULSE_CNT", 0},
	{VA_PWM_PWM2_PULSE_CNT, DATA_PWM_PWM2_PULSE_CNT, dump_pwm_pwm2_pulse_cnt, "PWM_PWM2_PULSE_CNT", 0},
	{VA_PWM_PWM3_PULSE_CNT, DATA_PWM_PWM3_PULSE_CNT, dump_pwm_pwm3_pulse_cnt, "PWM_PWM3_PULSE_CNT", 0},
	{VA_PWM_PWM4_PULSE_CNT, DATA_PWM_PWM4_PULSE_CNT, dump_pwm_pwm4_pulse_cnt, "PWM_PWM4_PULSE_CNT", 0},
	{VA_PWM_PWM5_PULSE_CNT, DATA_PWM_PWM5_PULSE_CNT, dump_pwm_pwm5_pulse_cnt, "PWM_PWM5_PULSE_CNT", 0},
	{VA_PWM_PWM6_PULSE_CNT, DATA_PWM_PWM6_PULSE_CNT, dump_pwm_pwm6_pulse_cnt, "PWM_PWM6_PULSE_CNT", 0},
	{VA_PWM_PWM7_PULSE_CNT, DATA_PWM_PWM7_PULSE_CNT, dump_pwm_pwm7_pulse_cnt, "PWM_PWM7_PULSE_CNT", 0},
	{VA_PWM_PWM8_PULSE_CNT, DATA_PWM_PWM8_PULSE_CNT, dump_pwm_pwm8_pulse_cnt, "PWM_PWM8_PULSE_CNT", 0},
	{VA_PWM_PWM9_PULSE_CNT, DATA_PWM_PWM9_PULSE_CNT, dump_pwm_pwm9_pulse_cnt, "PWM_PWM9_PULSE_CNT", 0},
};

void dump_pwm(u32 mode)
{
	u32 cnt = sizeof(pwm_info) / sizeof(pwm_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					pwm_info[i].name,
					pwm_info[i].addr,
					pwm_info[i].reset);
		def_strcat(buffer, sz_temp);

		pwm_info[i].dump(pwm_info[i].addr, pwm_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_pwm_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_ver_t reg;

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

u32 dump_pwm_pwm_ce(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm_ce_t reg;

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

	def_sprintf(sz_temp, "0x%X:ce0\n", reg.bits.ce0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce1\n", reg.bits.ce1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce2\n", reg.bits.ce2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce3\n", reg.bits.ce3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce4\n", reg.bits.ce4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce5\n", reg.bits.ce5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce6\n", reg.bits.ce6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce7\n", reg.bits.ce7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce8\n", reg.bits.ce8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ce9\n", reg.bits.ce9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm_pe(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm_pe_t reg;

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

	def_sprintf(sz_temp, "0x%X:pe0\n", reg.bits.pe0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe1\n", reg.bits.pe1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe2\n", reg.bits.pe2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe3\n", reg.bits.pe3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe4\n", reg.bits.pe4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe5\n", reg.bits.pe5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe6\n", reg.bits.pe6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe7\n", reg.bits.pe7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe8\n", reg.bits.pe8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pe9\n", reg.bits.pe9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm_int_en_t reg;

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

	def_sprintf(sz_temp, "0x%X:en0\n", reg.bits.en0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en1\n", reg.bits.en1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en2\n", reg.bits.en2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en3\n", reg.bits.en3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en4\n", reg.bits.en4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en5\n", reg.bits.en5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en6\n", reg.bits.en6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en7\n", reg.bits.en7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en8\n", reg.bits.en8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:en9\n", reg.bits.en9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm_clr_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm_clr_int_pending_t reg;

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

	def_sprintf(sz_temp, "0x%X:clr0\n", reg.bits.clr0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr1\n", reg.bits.clr1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr2\n", reg.bits.clr2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr3\n", reg.bits.clr3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr4\n", reg.bits.clr4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr5\n", reg.bits.clr5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr6\n", reg.bits.clr6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr7\n", reg.bits.clr7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr8\n", reg.bits.clr8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr9\n", reg.bits.clr9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm_int_pending_t reg;

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

	def_sprintf(sz_temp, "0x%X:pending0\n", reg.bits.pending0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending1\n", reg.bits.pending1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending2\n", reg.bits.pending2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending3\n", reg.bits.pending3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending4\n", reg.bits.pending4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending5\n", reg.bits.pending5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending6\n", reg.bits.pending6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending7\n", reg.bits.pending7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending8\n", reg.bits.pending8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:pending9\n", reg.bits.pending9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm0_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm0_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm1_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm1_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm2_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm2_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm3_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm3_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm4_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm4_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm5_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm5_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm6_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm6_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm7_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm7_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm8_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm8_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm9_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm9_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_sel\n", reg.bits.src_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gat\n", reg.bits.gat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:bypass\n", reg.bits.bypass);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm0_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm0_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm1_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm1_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm2_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm2_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm3_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm3_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm4_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm4_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm5_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm5_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm6_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm6_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm7_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm7_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm8_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm8_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm9_cnt_store(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm9_cnt_store_t reg;

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

	def_sprintf(sz_temp, "0x%X:store\n", reg.bits.store);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm0_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm0_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm1_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm1_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm2_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm2_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm3_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm3_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm4_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm4_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm5_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm5_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm6_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm6_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm7_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm7_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm8_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm8_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm9_cnt_cmp(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm9_cnt_cmp_t reg;

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

	def_sprintf(sz_temp, "0x%X:cmp\n", reg.bits.cmp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm0_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm1_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm2_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm2_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm3_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm3_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm4_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm4_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm5_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm5_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm6_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm6_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm7_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm7_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm8_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm8_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm9_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm9_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:dt\n", reg.bits.dt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dt_en\n", reg.bits.dt_en);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mod\n", reg.bits.mod);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_oe\n", reg.bits.p_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_pol\n", reg.bits.p_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_oe\n", reg.bits.n_oe);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:n_pol\n", reg.bits.n_pol);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ren\n", reg.bits.ren);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fen\n", reg.bits.fen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_rev\n", reg.bits.p_rev);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:p_ie\n", reg.bits.p_ie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm0_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm0_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm1_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm1_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm2_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm2_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm3_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm3_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm4_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm4_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm5_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm5_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm6_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm6_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm7_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm7_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm8_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm8_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm9_cnt_cap(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm9_cnt_cap_t reg;

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

	def_sprintf(sz_temp, "0x%X:r_cap\n", reg.bits.r_cap);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:f_cap\n", reg.bits.f_cap);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm0_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm0_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm1_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm1_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm2_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm2_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm3_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm3_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm4_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm4_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm5_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm5_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm6_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm6_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm7_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm7_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm8_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm8_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm9_in_sts(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm9_in_sts_t reg;

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

	def_sprintf(sz_temp, "0x%X:rflag\n", reg.bits.rflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:orflag\n", reg.bits.orflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fflag\n", reg.bits.fflag);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:offlag\n", reg.bits.offlag);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm0_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm0_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm1_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm1_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm2_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm2_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm3_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm3_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm4_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm4_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm5_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm5_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm6_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm6_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm7_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm7_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm8_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm8_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm9_dly_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm9_dly_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:delay\n", reg.bits.delay);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm0_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm0_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm1_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm1_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm2_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm2_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm3_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm3_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm4_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm4_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm5_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm5_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm6_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm6_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm7_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm7_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm8_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm8_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_pwm_pwm9_pulse_cnt(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_pwm_pwm9_pulse_cnt_t reg;

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

	def_sprintf(sz_temp, "0x%X:pcnt\n", reg.bits.pcnt);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___PWM___DUMP__C___ */
