/* gpio_dump.c */

#ifndef ___GPIO___DUMP__C___
#define ___GPIO___DUMP__C___

reg_info_t gpio_info[] = {
	{VA_GPIO_GPIOC_VER, DATA_GPIO_GPIOC_VER, dump_gpio_gpioc_ver, "GPIO_GPIOC_VER", 0},
	{VA_GPIO_GPIOA_FUNC_R0, DATA_GPIO_GPIOA_FUNC_R0, dump_gpio_gpioa_func_r0, "GPIO_GPIOA_FUNC_R0", 0},
	{VA_GPIO_GPIOA_FUNC_R1, DATA_GPIO_GPIOA_FUNC_R1, dump_gpio_gpioa_func_r1, "GPIO_GPIOA_FUNC_R1", 0},
	{VA_GPIO_GPIOA_PUD_R0, DATA_GPIO_GPIOA_PUD_R0, dump_gpio_gpioa_pud_r0, "GPIO_GPIOA_PUD_R0", 0},
	{VA_GPIO_GPIOA_PUD_R1, DATA_GPIO_GPIOA_PUD_R1, dump_gpio_gpioa_pud_r1, "GPIO_GPIOA_PUD_R1", 0},
	{VA_GPIO_GPIOA_DRV_R0, DATA_GPIO_GPIOA_DRV_R0, dump_gpio_gpioa_drv_r0, "GPIO_GPIOA_DRV_R0", 0},
	{VA_GPIO_GPIOA_DRV_R1, DATA_GPIO_GPIOA_DRV_R1, dump_gpio_gpioa_drv_r1, "GPIO_GPIOA_DRV_R1", 0},
	{VA_GPIO_GPIOA_DATA, DATA_GPIO_GPIOA_DATA, dump_gpio_gpioa_data, "GPIO_GPIOA_DATA", 0},
	{VA_GPIO_GPIOA_PR_R, DATA_GPIO_GPIOA_PR_R, dump_gpio_gpioa_pr_r, "GPIO_GPIOA_PR_R", 0},
	{VA_GPIO_GPIOB_FUNC_R0, DATA_GPIO_GPIOB_FUNC_R0, dump_gpio_gpiob_func_r0, "GPIO_GPIOB_FUNC_R0", 0},
	{VA_GPIO_GPIOB_FUNC_R1, DATA_GPIO_GPIOB_FUNC_R1, dump_gpio_gpiob_func_r1, "GPIO_GPIOB_FUNC_R1", 0},
	{VA_GPIO_GPIOB_FUNC_R2, DATA_GPIO_GPIOB_FUNC_R2, dump_gpio_gpiob_func_r2, "GPIO_GPIOB_FUNC_R2", 0},
	{VA_GPIO_GPIOB_FUNC_R3, DATA_GPIO_GPIOB_FUNC_R3, dump_gpio_gpiob_func_r3, "GPIO_GPIOB_FUNC_R3", 0},
	{VA_GPIO_GPIOB_PUD_R0, DATA_GPIO_GPIOB_PUD_R0, dump_gpio_gpiob_pud_r0, "GPIO_GPIOB_PUD_R0", 0},
	{VA_GPIO_GPIOB_PUD_R1, DATA_GPIO_GPIOB_PUD_R1, dump_gpio_gpiob_pud_r1, "GPIO_GPIOB_PUD_R1", 0},
	{VA_GPIO_GPIOB_PUD_R2, DATA_GPIO_GPIOB_PUD_R2, dump_gpio_gpiob_pud_r2, "GPIO_GPIOB_PUD_R2", 0},
	{VA_GPIO_GPIOB_PUD_R3, DATA_GPIO_GPIOB_PUD_R3, dump_gpio_gpiob_pud_r3, "GPIO_GPIOB_PUD_R3", 0},
	{VA_GPIO_GPIOB_DRV_R0, DATA_GPIO_GPIOB_DRV_R0, dump_gpio_gpiob_drv_r0, "GPIO_GPIOB_DRV_R0", 0},
	{VA_GPIO_GPIOB_DRV_R1, DATA_GPIO_GPIOB_DRV_R1, dump_gpio_gpiob_drv_r1, "GPIO_GPIOB_DRV_R1", 0},
	{VA_GPIO_GPIOB_DRV_R2, DATA_GPIO_GPIOB_DRV_R2, dump_gpio_gpiob_drv_r2, "GPIO_GPIOB_DRV_R2", 0},
	{VA_GPIO_GPIOB_DRV_R3, DATA_GPIO_GPIOB_DRV_R3, dump_gpio_gpiob_drv_r3, "GPIO_GPIOB_DRV_R3", 0},
	{VA_GPIO_GPIOB_DATA, DATA_GPIO_GPIOB_DATA, dump_gpio_gpiob_data, "GPIO_GPIOB_DATA", 0},
	{VA_GPIO_GPIOB_PR_R, DATA_GPIO_GPIOB_PR_R, dump_gpio_gpiob_pr_r, "GPIO_GPIOB_PR_R", 0},
	{VA_GPIO_GPIOC_FUNC_R0, DATA_GPIO_GPIOC_FUNC_R0, dump_gpio_gpioc_func_r0, "GPIO_GPIOC_FUNC_R0", 0},
	{VA_GPIO_GPIOC_FUNC_R1, DATA_GPIO_GPIOC_FUNC_R1, dump_gpio_gpioc_func_r1, "GPIO_GPIOC_FUNC_R1", 0},
	{VA_GPIO_GPIOC_FUNC_R2, DATA_GPIO_GPIOC_FUNC_R2, dump_gpio_gpioc_func_r2, "GPIO_GPIOC_FUNC_R2", 0},
	{VA_GPIO_GPIOC_PUD_R0, DATA_GPIO_GPIOC_PUD_R0, dump_gpio_gpioc_pud_r0, "GPIO_GPIOC_PUD_R0", 0},
	{VA_GPIO_GPIOC_PUD_R1, DATA_GPIO_GPIOC_PUD_R1, dump_gpio_gpioc_pud_r1, "GPIO_GPIOC_PUD_R1", 0},
	{VA_GPIO_GPIOC_PUD_R2, DATA_GPIO_GPIOC_PUD_R2, dump_gpio_gpioc_pud_r2, "GPIO_GPIOC_PUD_R2", 0},
	{VA_GPIO_GPIOC_DRV_R0, DATA_GPIO_GPIOC_DRV_R0, dump_gpio_gpioc_drv_r0, "GPIO_GPIOC_DRV_R0", 0},
	{VA_GPIO_GPIOC_DRV_R1, DATA_GPIO_GPIOC_DRV_R1, dump_gpio_gpioc_drv_r1, "GPIO_GPIOC_DRV_R1", 0},
	{VA_GPIO_GPIOC_DRV_R2, DATA_GPIO_GPIOC_DRV_R2, dump_gpio_gpioc_drv_r2, "GPIO_GPIOC_DRV_R2", 0},
	{VA_GPIO_GPIOC_DATA, DATA_GPIO_GPIOC_DATA, dump_gpio_gpioc_data, "GPIO_GPIOC_DATA", 0},
	{VA_GPIO_GPIOC_PR_R, DATA_GPIO_GPIOC_PR_R, dump_gpio_gpioc_pr_r, "GPIO_GPIOC_PR_R", 0},
	{VA_GPIO_GPIOD_FUNC_R0, DATA_GPIO_GPIOD_FUNC_R0, dump_gpio_gpiod_func_r0, "GPIO_GPIOD_FUNC_R0", 0},
	{VA_GPIO_GPIOD_PUD_R0, DATA_GPIO_GPIOD_PUD_R0, dump_gpio_gpiod_pud_r0, "GPIO_GPIOD_PUD_R0", 0},
	{VA_GPIO_GPIOD_DRV_R0, DATA_GPIO_GPIOD_DRV_R0, dump_gpio_gpiod_drv_r0, "GPIO_GPIOD_DRV_R0", 0},
	{VA_GPIO_GPIOD_DATA, DATA_GPIO_GPIOD_DATA, dump_gpio_gpiod_data, "GPIO_GPIOD_DATA", 0},
	{VA_GPIO_GPIOD_PR_R, DATA_GPIO_GPIOD_PR_R, dump_gpio_gpiod_pr_r, "GPIO_GPIOD_PR_R", 0},
	{VA_GPIO_GPIOE_FUNC_R0, DATA_GPIO_GPIOE_FUNC_R0, dump_gpio_gpioe_func_r0, "GPIO_GPIOE_FUNC_R0", 0},
	{VA_GPIO_GPIOE_FUNC_R1, DATA_GPIO_GPIOE_FUNC_R1, dump_gpio_gpioe_func_r1, "GPIO_GPIOE_FUNC_R1", 0},
	{VA_GPIO_GPIOE_PUD_R0, DATA_GPIO_GPIOE_PUD_R0, dump_gpio_gpioe_pud_r0, "GPIO_GPIOE_PUD_R0", 0},
	{VA_GPIO_GPIOE_PUD_R1, DATA_GPIO_GPIOE_PUD_R1, dump_gpio_gpioe_pud_r1, "GPIO_GPIOE_PUD_R1", 0},
	{VA_GPIO_GPIOE_DRV_R0, DATA_GPIO_GPIOE_DRV_R0, dump_gpio_gpioe_drv_r0, "GPIO_GPIOE_DRV_R0", 0},
	{VA_GPIO_GPIOE_DRV_R1, DATA_GPIO_GPIOE_DRV_R1, dump_gpio_gpioe_drv_r1, "GPIO_GPIOE_DRV_R1", 0},
	{VA_GPIO_GPIOE_DATA, DATA_GPIO_GPIOE_DATA, dump_gpio_gpioe_data, "GPIO_GPIOE_DATA", 0},
	{VA_GPIO_GPIOE_PR_R, DATA_GPIO_GPIOE_PR_R, dump_gpio_gpioe_pr_r, "GPIO_GPIOE_PR_R", 0},
	{VA_GPIO_GPIOF_FUNC_R0, DATA_GPIO_GPIOF_FUNC_R0, dump_gpio_gpiof_func_r0, "GPIO_GPIOF_FUNC_R0", 0},
	{VA_GPIO_GPIOF_FUNC_R1, DATA_GPIO_GPIOF_FUNC_R1, dump_gpio_gpiof_func_r1, "GPIO_GPIOF_FUNC_R1", 0},
	{VA_GPIO_GPIOF_PUD_R0, DATA_GPIO_GPIOF_PUD_R0, dump_gpio_gpiof_pud_r0, "GPIO_GPIOF_PUD_R0", 0},
	{VA_GPIO_GPIOF_PUD_R1, DATA_GPIO_GPIOF_PUD_R1, dump_gpio_gpiof_pud_r1, "GPIO_GPIOF_PUD_R1", 0},
	{VA_GPIO_GPIOF_DRV_R0, DATA_GPIO_GPIOF_DRV_R0, dump_gpio_gpiof_drv_r0, "GPIO_GPIOF_DRV_R0", 0},
	{VA_GPIO_GPIOF_DRV_R1, DATA_GPIO_GPIOF_DRV_R1, dump_gpio_gpiof_drv_r1, "GPIO_GPIOF_DRV_R1", 0},
	{VA_GPIO_GPIOF_DATA, DATA_GPIO_GPIOF_DATA, dump_gpio_gpiof_data, "GPIO_GPIOF_DATA", 0},
	{VA_GPIO_GPIOF_PR_R, DATA_GPIO_GPIOF_PR_R, dump_gpio_gpiof_pr_r, "GPIO_GPIOF_PR_R", 0},
	{VA_GPIO_GPIOG_FUNC_R0, DATA_GPIO_GPIOG_FUNC_R0, dump_gpio_gpiog_func_r0, "GPIO_GPIOG_FUNC_R0", 0},
	{VA_GPIO_GPIOG_FUNC_R1, DATA_GPIO_GPIOG_FUNC_R1, dump_gpio_gpiog_func_r1, "GPIO_GPIOG_FUNC_R1", 0},
	{VA_GPIO_GPIOG_PUD_R0, DATA_GPIO_GPIOG_PUD_R0, dump_gpio_gpiog_pud_r0, "GPIO_GPIOG_PUD_R0", 0},
	{VA_GPIO_GPIOG_PUD_R1, DATA_GPIO_GPIOG_PUD_R1, dump_gpio_gpiog_pud_r1, "GPIO_GPIOG_PUD_R1", 0},
	{VA_GPIO_GPIOG_DRV_R0, DATA_GPIO_GPIOG_DRV_R0, dump_gpio_gpiog_drv_r0, "GPIO_GPIOG_DRV_R0", 0},
	{VA_GPIO_GPIOG_DRV_R1, DATA_GPIO_GPIOG_DRV_R1, dump_gpio_gpiog_drv_r1, "GPIO_GPIOG_DRV_R1", 0},
	{VA_GPIO_GPIOG_DATA, DATA_GPIO_GPIOG_DATA, dump_gpio_gpiog_data, "GPIO_GPIOG_DATA", 0},
	{VA_GPIO_GPIOG_PR_R, DATA_GPIO_GPIOG_PR_R, dump_gpio_gpiog_pr_r, "GPIO_GPIOG_PR_R", 0},
	{VA_GPIO_GPIOJ_FUNC_R0, DATA_GPIO_GPIOJ_FUNC_R0, dump_gpio_gpioj_func_r0, "GPIO_GPIOJ_FUNC_R0", 0},
	{VA_GPIO_GPIOJ_PUD_R0, DATA_GPIO_GPIOJ_PUD_R0, dump_gpio_gpioj_pud_r0, "GPIO_GPIOJ_PUD_R0", 0},
	{VA_GPIO_GPIOJ_DRV_R0, DATA_GPIO_GPIOJ_DRV_R0, dump_gpio_gpioj_drv_r0, "GPIO_GPIOJ_DRV_R0", 0},
	{VA_GPIO_GPIOJ_DATA, DATA_GPIO_GPIOJ_DATA, dump_gpio_gpioj_data, "GPIO_GPIOJ_DATA", 0},
	{VA_GPIO_GPIOJ_PR_R, DATA_GPIO_GPIOJ_PR_R, dump_gpio_gpioj_pr_r, "GPIO_GPIOJ_PR_R", 0},
	{VA_GPIO_GPIOB_INT_TRI_R0, DATA_GPIO_GPIOB_INT_TRI_R0, dump_gpio_gpiob_int_tri_r0, "GPIO_GPIOB_INT_TRI_R0", 0},
	{VA_GPIO_GPIOB_INT_TRI_R1, DATA_GPIO_GPIOB_INT_TRI_R1, dump_gpio_gpiob_int_tri_r1, "GPIO_GPIOB_INT_TRI_R1", 0},
	{VA_GPIO_GPIOB_INT_TRI_R2, DATA_GPIO_GPIOB_INT_TRI_R2, dump_gpio_gpiob_int_tri_r2, "GPIO_GPIOB_INT_TRI_R2", 0},
	{VA_GPIO_GPIOB_INT_TRI_R3, DATA_GPIO_GPIOB_INT_TRI_R3, dump_gpio_gpiob_int_tri_r3, "GPIO_GPIOB_INT_TRI_R3", 0},
	{VA_GPIO_GPIOB_INT_MASK, DATA_GPIO_GPIOB_INT_MASK, dump_gpio_gpiob_int_mask, "GPIO_GPIOB_INT_MASK", 0},
	{VA_GPIO_GPIOB_INT_PEND, DATA_GPIO_GPIOB_INT_PEND, dump_gpio_gpiob_int_pend, "GPIO_GPIOB_INT_PEND", 0},
	{VA_GPIO_GPIOB_INT_SAMP_CTRL, DATA_GPIO_GPIOB_INT_SAMP_CTRL, dump_gpio_gpiob_int_samp_ctrl, "GPIO_GPIOB_INT_SAMP_CTRL", 0},
	{VA_GPIO_GPIOE_INT_TRI_R0, DATA_GPIO_GPIOE_INT_TRI_R0, dump_gpio_gpioe_int_tri_r0, "GPIO_GPIOE_INT_TRI_R0", 0},
	{VA_GPIO_GPIOE_INT_TRI_R1, DATA_GPIO_GPIOE_INT_TRI_R1, dump_gpio_gpioe_int_tri_r1, "GPIO_GPIOE_INT_TRI_R1", 0},
	{VA_GPIO_GPIOE_INT_MASK, DATA_GPIO_GPIOE_INT_MASK, dump_gpio_gpioe_int_mask, "GPIO_GPIOE_INT_MASK", 0},
	{VA_GPIO_GPIOE_INT_PEND, DATA_GPIO_GPIOE_INT_PEND, dump_gpio_gpioe_int_pend, "GPIO_GPIOE_INT_PEND", 0},
	{VA_GPIO_GPIOE_INT_SAMP_CTRL, DATA_GPIO_GPIOE_INT_SAMP_CTRL, dump_gpio_gpioe_int_samp_ctrl, "GPIO_GPIOE_INT_SAMP_CTRL", 0},
	{VA_GPIO_GPIOF_INT_TRI_R0, DATA_GPIO_GPIOF_INT_TRI_R0, dump_gpio_gpiof_int_tri_r0, "GPIO_GPIOF_INT_TRI_R0", 0},
	{VA_GPIO_GPIOF_INT_TRI_R1, DATA_GPIO_GPIOF_INT_TRI_R1, dump_gpio_gpiof_int_tri_r1, "GPIO_GPIOF_INT_TRI_R1", 0},
	{VA_GPIO_GPIOF_INT_MASK, DATA_GPIO_GPIOF_INT_MASK, dump_gpio_gpiof_int_mask, "GPIO_GPIOF_INT_MASK", 0},
	{VA_GPIO_GPIOF_INT_PEND, DATA_GPIO_GPIOF_INT_PEND, dump_gpio_gpiof_int_pend, "GPIO_GPIOF_INT_PEND", 0},
	{VA_GPIO_GPIOF_INT_SAMP_CTRL, DATA_GPIO_GPIOF_INT_SAMP_CTRL, dump_gpio_gpiof_int_samp_ctrl, "GPIO_GPIOF_INT_SAMP_CTRL", 0},
	{VA_GPIO_GPIOG_INT_TRI_R0, DATA_GPIO_GPIOG_INT_TRI_R0, dump_gpio_gpiog_int_tri_r0, "GPIO_GPIOG_INT_TRI_R0", 0},
	{VA_GPIO_GPIOG_INT_TRI_R1, DATA_GPIO_GPIOG_INT_TRI_R1, dump_gpio_gpiog_int_tri_r1, "GPIO_GPIOG_INT_TRI_R1", 0},
	{VA_GPIO_GPIOG_INT_MASK, DATA_GPIO_GPIOG_INT_MASK, dump_gpio_gpiog_int_mask, "GPIO_GPIOG_INT_MASK", 0},
	{VA_GPIO_GPIOG_INT_PEND, DATA_GPIO_GPIOG_INT_PEND, dump_gpio_gpiog_int_pend, "GPIO_GPIOG_INT_PEND", 0},
	{VA_GPIO_GPIOG_INT_SAMP_CTRL, DATA_GPIO_GPIOG_INT_SAMP_CTRL, dump_gpio_gpiog_int_samp_ctrl, "GPIO_GPIOG_INT_SAMP_CTRL", 0},
};

void dump_gpio(u32 mode)
{
	u32 cnt = sizeof(gpio_info) / sizeof(gpio_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					gpio_info[i].name,
					gpio_info[i].addr,
					gpio_info[i].reset);
		def_strcat(buffer, sz_temp);

		gpio_info[i].dump(gpio_info[i].addr, gpio_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_gpio_gpioc_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_ver_t reg;

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

u32 dump_gpio_gpioa_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioa_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioa0\n", reg.bits.gpioa0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa1\n", reg.bits.gpioa1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa2\n", reg.bits.gpioa2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa3\n", reg.bits.gpioa3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa4\n", reg.bits.gpioa4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa5\n", reg.bits.gpioa5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa6\n", reg.bits.gpioa6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa7\n", reg.bits.gpioa7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioa_func_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioa_func_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioa8\n", reg.bits.gpioa8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa9\n", reg.bits.gpioa9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa10\n", reg.bits.gpioa10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa11\n", reg.bits.gpioa11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa12\n", reg.bits.gpioa12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa13\n", reg.bits.gpioa13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa14\n", reg.bits.gpioa14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa15\n", reg.bits.gpioa15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioa_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioa_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioa0\n", reg.bits.gpioa0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa1\n", reg.bits.gpioa1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa2\n", reg.bits.gpioa2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa3\n", reg.bits.gpioa3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa4\n", reg.bits.gpioa4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa5\n", reg.bits.gpioa5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa6\n", reg.bits.gpioa6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa7\n", reg.bits.gpioa7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioa_pud_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioa_pud_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioa8\n", reg.bits.gpioa8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa9\n", reg.bits.gpioa9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa10\n", reg.bits.gpioa10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa11\n", reg.bits.gpioa11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa12\n", reg.bits.gpioa12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa13\n", reg.bits.gpioa13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa14\n", reg.bits.gpioa14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa15\n", reg.bits.gpioa15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioa_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioa_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioa0\n", reg.bits.gpioa0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa1\n", reg.bits.gpioa1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa2\n", reg.bits.gpioa2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa3\n", reg.bits.gpioa3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa4\n", reg.bits.gpioa4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa5\n", reg.bits.gpioa5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa6\n", reg.bits.gpioa6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa7\n", reg.bits.gpioa7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioa_drv_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioa_drv_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioa8\n", reg.bits.gpioa8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa9\n", reg.bits.gpioa9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa10\n", reg.bits.gpioa10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa11\n", reg.bits.gpioa11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa12\n", reg.bits.gpioa12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa13\n", reg.bits.gpioa13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa14\n", reg.bits.gpioa14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioa15\n", reg.bits.gpioa15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioa_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioa_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioa_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioa_pr_r_t reg;

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

u32 dump_gpio_gpiob_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob2\n", reg.bits.gpiob2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob3\n", reg.bits.gpiob3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob4\n", reg.bits.gpiob4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob5\n", reg.bits.gpiob5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob6\n", reg.bits.gpiob6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob7\n", reg.bits.gpiob7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_func_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_func_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob10\n", reg.bits.gpiob10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob11\n", reg.bits.gpiob11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob12\n", reg.bits.gpiob12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob13\n", reg.bits.gpiob13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob14\n", reg.bits.gpiob14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob15\n", reg.bits.gpiob15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_func_r2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_func_r2_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob18\n", reg.bits.gpiob18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob19\n", reg.bits.gpiob19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob20\n", reg.bits.gpiob20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob21\n", reg.bits.gpiob21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob22\n", reg.bits.gpiob22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob23\n", reg.bits.gpiob23);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_func_r3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_func_r3_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob24\n", reg.bits.gpiob24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob25\n", reg.bits.gpiob25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob26\n", reg.bits.gpiob26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob27\n", reg.bits.gpiob27);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob2\n", reg.bits.gpiob2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob3\n", reg.bits.gpiob3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob4\n", reg.bits.gpiob4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob5\n", reg.bits.gpiob5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob6\n", reg.bits.gpiob6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob7\n", reg.bits.gpiob7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_pud_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_pud_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob10\n", reg.bits.gpiob10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob11\n", reg.bits.gpiob11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob12\n", reg.bits.gpiob12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob13\n", reg.bits.gpiob13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob14\n", reg.bits.gpiob14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob15\n", reg.bits.gpiob15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_pud_r2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_pud_r2_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob18\n", reg.bits.gpiob18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob19\n", reg.bits.gpiob19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob20\n", reg.bits.gpiob20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob21\n", reg.bits.gpiob21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob22\n", reg.bits.gpiob22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob23\n", reg.bits.gpiob23);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_pud_r3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_pud_r3_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob24\n", reg.bits.gpiob24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob25\n", reg.bits.gpiob25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob26\n", reg.bits.gpiob26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob27\n", reg.bits.gpiob27);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob2\n", reg.bits.gpiob2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob3\n", reg.bits.gpiob3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob4\n", reg.bits.gpiob4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob5\n", reg.bits.gpiob5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob6\n", reg.bits.gpiob6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob7\n", reg.bits.gpiob7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_drv_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_drv_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob10\n", reg.bits.gpiob10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob11\n", reg.bits.gpiob11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob12\n", reg.bits.gpiob12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob13\n", reg.bits.gpiob13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob14\n", reg.bits.gpiob14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob15\n", reg.bits.gpiob15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_drv_r2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_drv_r2_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob18\n", reg.bits.gpiob18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob19\n", reg.bits.gpiob19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob20\n", reg.bits.gpiob20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob21\n", reg.bits.gpiob21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob22\n", reg.bits.gpiob22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob23\n", reg.bits.gpiob23);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_drv_r3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_drv_r3_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob24\n", reg.bits.gpiob24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob25\n", reg.bits.gpiob25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob26\n", reg.bits.gpiob26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob27\n", reg.bits.gpiob27);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_pr_r_t reg;

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

u32 dump_gpio_gpioc_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc0\n", reg.bits.gpioc0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc1\n", reg.bits.gpioc1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc2\n", reg.bits.gpioc2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc3\n", reg.bits.gpioc3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc4\n", reg.bits.gpioc4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc5\n", reg.bits.gpioc5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc6\n", reg.bits.gpioc6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc7\n", reg.bits.gpioc7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_func_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_func_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc8\n", reg.bits.gpioc8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc9\n", reg.bits.gpioc9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc10\n", reg.bits.gpioc10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc11\n", reg.bits.gpioc11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc12\n", reg.bits.gpioc12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc13\n", reg.bits.gpioc13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc14\n", reg.bits.gpioc14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc15\n", reg.bits.gpioc15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_func_r2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_func_r2_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc16\n", reg.bits.gpioc16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc17\n", reg.bits.gpioc17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc18\n", reg.bits.gpioc18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc19\n", reg.bits.gpioc19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc20\n", reg.bits.gpioc20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc21\n", reg.bits.gpioc21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc22\n", reg.bits.gpioc22);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc0\n", reg.bits.gpioc0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc1\n", reg.bits.gpioc1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc2\n", reg.bits.gpioc2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc3\n", reg.bits.gpioc3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc4\n", reg.bits.gpioc4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc5\n", reg.bits.gpioc5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc6\n", reg.bits.gpioc6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc7\n", reg.bits.gpioc7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_pud_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_pud_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc8\n", reg.bits.gpioc8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc9\n", reg.bits.gpioc9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc10\n", reg.bits.gpioc10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc11\n", reg.bits.gpioc11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc12\n", reg.bits.gpioc12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc13\n", reg.bits.gpioc13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc14\n", reg.bits.gpioc14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc15\n", reg.bits.gpioc15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_pud_r2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_pud_r2_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc16\n", reg.bits.gpioc16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc17\n", reg.bits.gpioc17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc18\n", reg.bits.gpioc18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc19\n", reg.bits.gpioc19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc20\n", reg.bits.gpioc20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc21\n", reg.bits.gpioc21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc22\n", reg.bits.gpioc22);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc0\n", reg.bits.gpioc0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc1\n", reg.bits.gpioc1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc2\n", reg.bits.gpioc2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc3\n", reg.bits.gpioc3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc4\n", reg.bits.gpioc4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc5\n", reg.bits.gpioc5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc6\n", reg.bits.gpioc6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc7\n", reg.bits.gpioc7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_drv_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_drv_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc8\n", reg.bits.gpioc8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc9\n", reg.bits.gpioc9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc10\n", reg.bits.gpioc10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc11\n", reg.bits.gpioc11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc12\n", reg.bits.gpioc12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc13\n", reg.bits.gpioc13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc14\n", reg.bits.gpioc14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc15\n", reg.bits.gpioc15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_drv_r2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_drv_r2_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioc16\n", reg.bits.gpioc16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc17\n", reg.bits.gpioc17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc18\n", reg.bits.gpioc18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc19\n", reg.bits.gpioc19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc20\n", reg.bits.gpioc20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc21\n", reg.bits.gpioc21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioc22\n", reg.bits.gpioc22);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioc_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioc_pr_r_t reg;

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

u32 dump_gpio_gpiod_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiod_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiod0\n", reg.bits.gpiod0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod1\n", reg.bits.gpiod1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod2\n", reg.bits.gpiod2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod3\n", reg.bits.gpiod3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod4\n", reg.bits.gpiod4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod5\n", reg.bits.gpiod5);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiod_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiod_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiod0\n", reg.bits.gpiod0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod1\n", reg.bits.gpiod1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod2\n", reg.bits.gpiod2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod3\n", reg.bits.gpiod3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod4\n", reg.bits.gpiod4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod5\n", reg.bits.gpiod5);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiod_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiod_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiod0\n", reg.bits.gpiod0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod1\n", reg.bits.gpiod1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod2\n", reg.bits.gpiod2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod3\n", reg.bits.gpiod3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod4\n", reg.bits.gpiod4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiod5\n", reg.bits.gpiod5);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiod_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiod_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiod_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiod_pr_r_t reg;

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

u32 dump_gpio_gpioe_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioe0\n", reg.bits.gpioe0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe1\n", reg.bits.gpioe1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe2\n", reg.bits.gpioe2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe3\n", reg.bits.gpioe3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe4\n", reg.bits.gpioe4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe5\n", reg.bits.gpioe5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe6\n", reg.bits.gpioe6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe7\n", reg.bits.gpioe7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_func_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_func_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioe8\n", reg.bits.gpioe8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe9\n", reg.bits.gpioe9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe10\n", reg.bits.gpioe10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe11\n", reg.bits.gpioe11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe12\n", reg.bits.gpioe12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe13\n", reg.bits.gpioe13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe14\n", reg.bits.gpioe14);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioe0\n", reg.bits.gpioe0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe1\n", reg.bits.gpioe1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe2\n", reg.bits.gpioe2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe3\n", reg.bits.gpioe3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe4\n", reg.bits.gpioe4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe5\n", reg.bits.gpioe5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe6\n", reg.bits.gpioe6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe7\n", reg.bits.gpioe7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_pud_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_pud_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioe8\n", reg.bits.gpioe8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe9\n", reg.bits.gpioe9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe10\n", reg.bits.gpioe10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe11\n", reg.bits.gpioe11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe12\n", reg.bits.gpioe12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe13\n", reg.bits.gpioe13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe14\n", reg.bits.gpioe14);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioe0\n", reg.bits.gpioe0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe1\n", reg.bits.gpioe1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe2\n", reg.bits.gpioe2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe3\n", reg.bits.gpioe3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe4\n", reg.bits.gpioe4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe5\n", reg.bits.gpioe5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe6\n", reg.bits.gpioe6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe7\n", reg.bits.gpioe7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_drv_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_drv_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioe8\n", reg.bits.gpioe8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe9\n", reg.bits.gpioe9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe10\n", reg.bits.gpioe10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe11\n", reg.bits.gpioe11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe12\n", reg.bits.gpioe12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe13\n", reg.bits.gpioe13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe14\n", reg.bits.gpioe14);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_pr_r_t reg;

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

u32 dump_gpio_gpiof_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiof0\n", reg.bits.gpiof0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof1\n", reg.bits.gpiof1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof2\n", reg.bits.gpiof2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof3\n", reg.bits.gpiof3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof4\n", reg.bits.gpiof4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof5\n", reg.bits.gpiof5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof6\n", reg.bits.gpiof6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof7\n", reg.bits.gpiof7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_func_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_func_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiof8\n", reg.bits.gpiof8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof9\n", reg.bits.gpiof9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiof0\n", reg.bits.gpiof0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof1\n", reg.bits.gpiof1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof2\n", reg.bits.gpiof2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof3\n", reg.bits.gpiof3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof4\n", reg.bits.gpiof4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof5\n", reg.bits.gpiof5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof6\n", reg.bits.gpiof6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof7\n", reg.bits.gpiof7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_pud_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_pud_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiof8\n", reg.bits.gpiof8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof9\n", reg.bits.gpiof9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiof0\n", reg.bits.gpiof0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof1\n", reg.bits.gpiof1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof2\n", reg.bits.gpiof2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof3\n", reg.bits.gpiof3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof4\n", reg.bits.gpiof4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof5\n", reg.bits.gpiof5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof6\n", reg.bits.gpiof6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof7\n", reg.bits.gpiof7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_drv_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_drv_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiof8\n", reg.bits.gpiof8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof9\n", reg.bits.gpiof9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_pr_r_t reg;

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

u32 dump_gpio_gpiog_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiog0\n", reg.bits.gpiog0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog1\n", reg.bits.gpiog1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog2\n", reg.bits.gpiog2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog3\n", reg.bits.gpiog3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog4\n", reg.bits.gpiog4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog5\n", reg.bits.gpiog5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog6\n", reg.bits.gpiog6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog7\n", reg.bits.gpiog7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_func_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_func_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiog8\n", reg.bits.gpiog8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog9\n", reg.bits.gpiog9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiog0\n", reg.bits.gpiog0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog1\n", reg.bits.gpiog1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog2\n", reg.bits.gpiog2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog3\n", reg.bits.gpiog3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog4\n", reg.bits.gpiog4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog5\n", reg.bits.gpiog5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog6\n", reg.bits.gpiog6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog7\n", reg.bits.gpiog7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_pud_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_pud_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiog8\n", reg.bits.gpiog8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog9\n", reg.bits.gpiog9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiog0\n", reg.bits.gpiog0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog1\n", reg.bits.gpiog1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog2\n", reg.bits.gpiog2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog3\n", reg.bits.gpiog3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog4\n", reg.bits.gpiog4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog5\n", reg.bits.gpiog5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog6\n", reg.bits.gpiog6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog7\n", reg.bits.gpiog7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_drv_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_drv_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiog8\n", reg.bits.gpiog8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog9\n", reg.bits.gpiog9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_pr_r_t reg;

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

u32 dump_gpio_gpioj_func_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioj_func_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioj0\n", reg.bits.gpioj0);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioj_pud_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioj_pud_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioj0\n", reg.bits.gpioj0);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioj_drv_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioj_drv_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioj0\n", reg.bits.gpioj0);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioj_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioj_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:dat\n", reg.bits.dat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioj_pr_r(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioj_pr_r_t reg;

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

u32 dump_gpio_gpiob_int_tri_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_int_tri_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob2\n", reg.bits.gpiob2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob3\n", reg.bits.gpiob3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob4\n", reg.bits.gpiob4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob5\n", reg.bits.gpiob5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob6\n", reg.bits.gpiob6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob7\n", reg.bits.gpiob7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_int_tri_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_int_tri_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob10\n", reg.bits.gpiob10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob11\n", reg.bits.gpiob11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob12\n", reg.bits.gpiob12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob13\n", reg.bits.gpiob13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob14\n", reg.bits.gpiob14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob15\n", reg.bits.gpiob15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_int_tri_r2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_int_tri_r2_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob18\n", reg.bits.gpiob18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob19\n", reg.bits.gpiob19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob20\n", reg.bits.gpiob20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob21\n", reg.bits.gpiob21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob22\n", reg.bits.gpiob22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob23\n", reg.bits.gpiob23);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_int_tri_r3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_int_tri_r3_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiob24\n", reg.bits.gpiob24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob25\n", reg.bits.gpiob25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob26\n", reg.bits.gpiob26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiob27\n", reg.bits.gpiob27);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_int_mask(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_int_mask_t reg;

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

	def_sprintf(sz_temp, "0x%X:mask_en\n", reg.bits.mask_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiob_int_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_int_pend_t reg;

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

u32 dump_gpio_gpiob_int_samp_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiob_int_samp_ctrl_t reg;

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

u32 dump_gpio_gpioe_int_tri_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_int_tri_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioe0\n", reg.bits.gpioe0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe1\n", reg.bits.gpioe1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe2\n", reg.bits.gpioe2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe3\n", reg.bits.gpioe3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe4\n", reg.bits.gpioe4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe5\n", reg.bits.gpioe5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe6\n", reg.bits.gpioe6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe7\n", reg.bits.gpioe7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_int_tri_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_int_tri_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpioe8\n", reg.bits.gpioe8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe9\n", reg.bits.gpioe9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe10\n", reg.bits.gpioe10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe11\n", reg.bits.gpioe11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe12\n", reg.bits.gpioe12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe13\n", reg.bits.gpioe13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpioe14\n", reg.bits.gpioe14);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_int_mask(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_int_mask_t reg;

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

	def_sprintf(sz_temp, "0x%X:mask_en\n", reg.bits.mask_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpioe_int_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_int_pend_t reg;

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

u32 dump_gpio_gpioe_int_samp_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpioe_int_samp_ctrl_t reg;

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

u32 dump_gpio_gpiof_int_tri_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_int_tri_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiof0\n", reg.bits.gpiof0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof1\n", reg.bits.gpiof1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof2\n", reg.bits.gpiof2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof3\n", reg.bits.gpiof3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof4\n", reg.bits.gpiof4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof5\n", reg.bits.gpiof5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof6\n", reg.bits.gpiof6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof7\n", reg.bits.gpiof7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_int_tri_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_int_tri_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiof8\n", reg.bits.gpiof8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiof9\n", reg.bits.gpiof9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_int_mask(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_int_mask_t reg;

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

	def_sprintf(sz_temp, "0x%X:mask_en\n", reg.bits.mask_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiof_int_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_int_pend_t reg;

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

u32 dump_gpio_gpiof_int_samp_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiof_int_samp_ctrl_t reg;

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

u32 dump_gpio_gpiog_int_tri_r0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_int_tri_r0_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiog0\n", reg.bits.gpiog0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog1\n", reg.bits.gpiog1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog2\n", reg.bits.gpiog2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog3\n", reg.bits.gpiog3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog4\n", reg.bits.gpiog4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog5\n", reg.bits.gpiog5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog6\n", reg.bits.gpiog6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog7\n", reg.bits.gpiog7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_int_tri_r1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_int_tri_r1_t reg;

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

	def_sprintf(sz_temp, "0x%X:gpiog8\n", reg.bits.gpiog8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gpiog9\n", reg.bits.gpiog9);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_int_mask(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_int_mask_t reg;

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

	def_sprintf(sz_temp, "0x%X:mask_en\n", reg.bits.mask_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpio_gpiog_int_pend(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_int_pend_t reg;

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

u32 dump_gpio_gpiog_int_samp_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpio_gpiog_int_samp_ctrl_t reg;

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

#endif /* ___GPIO___DUMP__C___ */
