/* gpadc_dump.c */

#ifndef ___GPADC___DUMP__C___
#define ___GPADC___DUMP__C___

reg_info_t gpadc_info[] = {
	{VA_GPADC_VER, DATA_GPADC_VER, dump_gpadc_ver, "GPADC_VER", 0},
	{VA_GPADC_GPAEN, DATA_GPADC_GPAEN, dump_gpadc_gpaen, "GPADC_GPAEN", 0},
	{VA_GPADC_GPAC, DATA_GPADC_GPAC, dump_gpadc_gpac, "GPADC_GPAC", 0},
	{VA_GPADC_GPAIE, DATA_GPADC_GPAIE, dump_gpadc_gpaie, "GPADC_GPAIE", 0},
	{VA_GPADC_GPAIS, DATA_GPADC_GPAIS, dump_gpadc_gpais, "GPADC_GPAIS", 0},
	{VA_GPADC_GPAIC, DATA_GPADC_GPAIC, dump_gpadc_gpaic, "GPADC_GPAIC", 0},
	{VA_GPADC_CDAT, DATA_GPADC_CDAT, dump_gpadc_cdat, "GPADC_CDAT", 0},
	{VA_GPADC_AIN0DAT, DATA_GPADC_AIN0DAT, dump_gpadc_ain0dat, "GPADC_AIN0DAT", 0},
	{VA_GPADC_AUX0HDAT, DATA_GPADC_AUX0HDAT, dump_gpadc_aux0hdat, "GPADC_AUX0HDAT", 0},
	{VA_GPADC_AUX0TDAT, DATA_GPADC_AUX0TDAT, dump_gpadc_aux0tdat, "GPADC_AUX0TDAT", 0},
	{VA_GPADC_AIN1DAT, DATA_GPADC_AIN1DAT, dump_gpadc_ain1dat, "GPADC_AIN1DAT", 0},
	{VA_GPADC_AUX1HDAT, DATA_GPADC_AUX1HDAT, dump_gpadc_aux1hdat, "GPADC_AUX1HDAT", 0},
	{VA_GPADC_AUX1TDAT, DATA_GPADC_AUX1TDAT, dump_gpadc_aux1tdat, "GPADC_AUX1TDAT", 0},
	{VA_GPADC_GSC, DATA_GPADC_GSC, dump_gpadc_gsc, "GPADC_GSC", 0},
	{VA_GPADC_S0CDAT, DATA_GPADC_S0CDAT, dump_gpadc_s0cdat, "GPADC_S0CDAT", 0},
	{VA_GPADC_S1CDAT, DATA_GPADC_S1CDAT, dump_gpadc_s1cdat, "GPADC_S1CDAT", 0},
	{VA_GPADC_S2CDAT, DATA_GPADC_S2CDAT, dump_gpadc_s2cdat, "GPADC_S2CDAT", 0},
	{VA_GPADC_S0DAT, DATA_GPADC_S0DAT, dump_gpadc_s0dat, "GPADC_S0DAT", 0},
	{VA_GPADC_S0UTDAT, DATA_GPADC_S0UTDAT, dump_gpadc_s0utdat, "GPADC_S0UTDAT", 0},
	{VA_GPADC_S0DTDAT, DATA_GPADC_S0DTDAT, dump_gpadc_s0dtdat, "GPADC_S0DTDAT", 0},
	{VA_GPADC_S1DAT, DATA_GPADC_S1DAT, dump_gpadc_s1dat, "GPADC_S1DAT", 0},
	{VA_GPADC_S1UTDAT, DATA_GPADC_S1UTDAT, dump_gpadc_s1utdat, "GPADC_S1UTDAT", 0},
	{VA_GPADC_S1DTDAT, DATA_GPADC_S1DTDAT, dump_gpadc_s1dtdat, "GPADC_S1DTDAT", 0},
	{VA_GPADC_S2DAT, DATA_GPADC_S2DAT, dump_gpadc_s2dat, "GPADC_S2DAT", 0},
	{VA_GPADC_S2UTDAT, DATA_GPADC_S2UTDAT, dump_gpadc_s2utdat, "GPADC_S2UTDAT", 0},
	{VA_GPADC_S2DTDAT, DATA_GPADC_S2DTDAT, dump_gpadc_s2dtdat, "GPADC_S2DTDAT", 0},
};

void dump_gpadc(u32 mode)
{
	u32 cnt = sizeof(gpadc_info) / sizeof(gpadc_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					gpadc_info[i].name,
					gpadc_info[i].addr,
					gpadc_info[i].reset);
		def_strcat(buffer, sz_temp);

		gpadc_info[i].dump(gpadc_info[i].addr, gpadc_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_gpadc_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_ver_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
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

u32 dump_gpadc_gpaen(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_gpaen_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:gen\n", reg.bits.gen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cen\n", reg.bits.cen);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_gpac(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_gpac_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:auxin0e\n", reg.bits.auxin0e);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:auxin1e\n", reg.bits.auxin1e);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sdae\n", reg.bits.sdae);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src\n", reg.bits.src);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fss\n", reg.bits.fss);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cce\n", reg.bits.cce);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dive\n", reg.bits.dive);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:opc\n", reg.bits.opc);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_gpaie(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_gpaie_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:aux0ie\n", reg.bits.aux0ie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:aux1ie\n", reg.bits.aux1ie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0die\n", reg.bits.s0die);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0uie\n", reg.bits.s0uie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1die\n", reg.bits.s1die);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1uie\n", reg.bits.s1uie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2die\n", reg.bits.s2die);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2uie\n", reg.bits.s2uie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ain0datie\n", reg.bits.ain0datie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ain1datie\n", reg.bits.ain1datie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0datie\n", reg.bits.s0datie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1datie\n", reg.bits.s1datie);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2datie\n", reg.bits.s2datie);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_gpais(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_gpais_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:aux0is\n", reg.bits.aux0is);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:aux1is\n", reg.bits.aux1is);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0dis\n", reg.bits.s0dis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0uis\n", reg.bits.s0uis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1dis\n", reg.bits.s1dis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1uis\n", reg.bits.s1uis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2dis\n", reg.bits.s2dis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2uis\n", reg.bits.s2uis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ain0datis\n", reg.bits.ain0datis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ain1datis\n", reg.bits.ain1datis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0datis\n", reg.bits.s0datis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1datis\n", reg.bits.s1datis);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2datis\n", reg.bits.s2datis);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_gpaic(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_gpaic_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:auxin0pc\n", reg.bits.auxin0pc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:auxin1pc\n", reg.bits.auxin1pc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0dpc\n", reg.bits.s0dpc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0upc\n", reg.bits.s0upc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1dpc\n", reg.bits.s1dpc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1upc\n", reg.bits.s1upc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2dpc\n", reg.bits.s2dpc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2upc\n", reg.bits.s2upc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ain0datpc\n", reg.bits.ain0datpc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ain1datpc\n", reg.bits.ain1datpc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s0datpc\n", reg.bits.s0datpc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s1datpc\n", reg.bits.s1datpc);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:s2datpc\n", reg.bits.s2datpc);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_cdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_cdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
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

u32 dump_gpadc_ain0dat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_ain0dat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
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

u32 dump_gpadc_aux0hdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_aux0hdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:auxin0shdat\n", reg.bits.auxin0shdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_aux0tdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_aux0tdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:auxin0tdat\n", reg.bits.auxin0tdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_ain1dat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_ain1dat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
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

u32 dump_gpadc_aux1hdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_aux1hdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:auxin1hdat\n", reg.bits.auxin1hdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_aux1tdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_aux1tdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:auxin1tdat\n", reg.bits.auxin1tdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_gsc(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_gsc_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:sen\n", reg.bits.sen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ssdaen\n", reg.bits.ssdaen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ssr\n", reg.bits.ssr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s0cdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s0cdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s0cdat\n", reg.bits.s0cdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s1cdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s1cdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s1cdat\n", reg.bits.s1cdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s2cdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s2cdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s2cdat\n", reg.bits.s2cdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s0dat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s0dat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s0adcdat\n", reg.bits.s0adcdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s0utdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s0utdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s0datut\n", reg.bits.s0datut);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s0dtdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s0dtdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s0datdt\n", reg.bits.s0datdt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s1dat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s1dat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s1adcdat\n", reg.bits.s1adcdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s1utdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s1utdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s1datut\n", reg.bits.s1datut);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s1dtdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s1dtdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s1datdt\n", reg.bits.s1datdt);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s2dat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s2dat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s2adcdat\n", reg.bits.s2adcdat);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s2utdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s2utdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s2datut\n", reg.bits.s2datut);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gpadc_s2dtdat(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gpadc_s2dtdat_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:s2datdt\n", reg.bits.s2datdt);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___GPADC___DUMP__C___ */
