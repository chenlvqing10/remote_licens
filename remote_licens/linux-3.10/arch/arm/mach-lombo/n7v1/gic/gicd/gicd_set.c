/* gicd_set.c */

#ifndef ___GICD__SET___C___
#define ___GICD__SET___C___

#ifdef DEF_SET_GICD_CTLR
void set_gicd_ctlr(u32 reg_addr,
		u32 enablegrp0,
		u32 enablegrp1,
		u32 m_or_r)
{
	reg_gicd_ctlr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enablegrp0 = enablegrp0;
	reg.bits.enablegrp1 = enablegrp1;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_CTLR_ENABLEGRP0
fun_set_mod_reg_bit(gicd, ctlr, enablegrp0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CTLR_ENABLEGRP1
fun_set_mod_reg_bit(gicd, ctlr, enablegrp1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_TYPER
void set_gicd_typer(u32 reg_addr,
			u32 itlinesnumber,
			u32 cpunumber,
			u32 securityextn,
			u32 lspi,
			u32 m_or_r)
{
	reg_gicd_typer_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.itlinesnumber = itlinesnumber;
	reg.bits.cpunumber = cpunumber;
	reg.bits.securityextn = securityextn;
	reg.bits.lspi = lspi;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_TYPER_ITLINESNUMBER
fun_set_mod_reg_bit(gicd, typer, itlinesnumber, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_TYPER_CPUNUMBER
fun_set_mod_reg_bit(gicd, typer, cpunumber, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_TYPER_SECURITYEXTN
fun_set_mod_reg_bit(gicd, typer, securityextn, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_TYPER_LSPI
fun_set_mod_reg_bit(gicd, typer, lspi, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IIDR
void set_gicd_iidr(u32 reg_addr,
		u32 implementor,
		u32 revision,
		u32 variant,
		u32 productid,
		u32 m_or_r)
{
	reg_gicd_iidr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.implementor = implementor;
	reg.bits.revision = revision;
	reg.bits.variant = variant;
	reg.bits.productid = productid;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IIDR_IMPLEMENTOR
fun_set_mod_reg_bit(gicd, iidr, implementor, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IIDR_REVISION
fun_set_mod_reg_bit(gicd, iidr, revision, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IIDR_VARIANT
fun_set_mod_reg_bit(gicd, iidr, variant, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IIDR_PRODUCTID
fun_set_mod_reg_bit(gicd, iidr, productid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0
void set_gicd_igroupr0(u32 reg_addr,
			u32 grp_status_id0,
			u32 grp_status_id1,
			u32 grp_status_id2,
			u32 grp_status_id3,
			u32 grp_status_id4,
			u32 grp_status_id5,
			u32 grp_status_id6,
			u32 grp_status_id7,
			u32 grp_status_id8,
			u32 grp_status_id9,
			u32 grp_status_id10,
			u32 grp_status_id11,
			u32 grp_status_id12,
			u32 grp_status_id13,
			u32 grp_status_id14,
			u32 grp_status_id15,
			u32 grp_status_id16,
			u32 grp_status_id17,
			u32 grp_status_id18,
			u32 grp_status_id19,
			u32 grp_status_id20,
			u32 grp_status_id21,
			u32 grp_status_id22,
			u32 grp_status_id23,
			u32 grp_status_id24,
			u32 grp_status_id25,
			u32 grp_status_id26,
			u32 grp_status_id27,
			u32 grp_status_id28,
			u32 grp_status_id29,
			u32 grp_status_id30,
			u32 grp_status_id31,
			u32 m_or_r)
{
	reg_gicd_igroupr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.grp_status_id0 = grp_status_id0;
	reg.bits.grp_status_id1 = grp_status_id1;
	reg.bits.grp_status_id2 = grp_status_id2;
	reg.bits.grp_status_id3 = grp_status_id3;
	reg.bits.grp_status_id4 = grp_status_id4;
	reg.bits.grp_status_id5 = grp_status_id5;
	reg.bits.grp_status_id6 = grp_status_id6;
	reg.bits.grp_status_id7 = grp_status_id7;
	reg.bits.grp_status_id8 = grp_status_id8;
	reg.bits.grp_status_id9 = grp_status_id9;
	reg.bits.grp_status_id10 = grp_status_id10;
	reg.bits.grp_status_id11 = grp_status_id11;
	reg.bits.grp_status_id12 = grp_status_id12;
	reg.bits.grp_status_id13 = grp_status_id13;
	reg.bits.grp_status_id14 = grp_status_id14;
	reg.bits.grp_status_id15 = grp_status_id15;
	reg.bits.grp_status_id16 = grp_status_id16;
	reg.bits.grp_status_id17 = grp_status_id17;
	reg.bits.grp_status_id18 = grp_status_id18;
	reg.bits.grp_status_id19 = grp_status_id19;
	reg.bits.grp_status_id20 = grp_status_id20;
	reg.bits.grp_status_id21 = grp_status_id21;
	reg.bits.grp_status_id22 = grp_status_id22;
	reg.bits.grp_status_id23 = grp_status_id23;
	reg.bits.grp_status_id24 = grp_status_id24;
	reg.bits.grp_status_id25 = grp_status_id25;
	reg.bits.grp_status_id26 = grp_status_id26;
	reg.bits.grp_status_id27 = grp_status_id27;
	reg.bits.grp_status_id28 = grp_status_id28;
	reg.bits.grp_status_id29 = grp_status_id29;
	reg.bits.grp_status_id30 = grp_status_id30;
	reg.bits.grp_status_id31 = grp_status_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID0
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID1
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID2
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID3
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID4
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID5
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID6
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID7
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID8
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID9
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID10
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID11
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID12
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID13
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID14
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID15
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID16
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID17
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID18
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID19
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID20
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID21
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID22
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID23
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID24
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID25
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID26
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID27
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID28
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID29
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID30
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR0_GRP_STATUS_ID31
fun_set_mod_reg_bit(gicd, igroupr0, grp_status_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1
void set_gicd_igroupr1(u32 reg_addr,
			u32 grp_status_id32,
			u32 grp_status_id33,
			u32 grp_status_id34,
			u32 grp_status_id35,
			u32 grp_status_id36,
			u32 grp_status_id37,
			u32 grp_status_id38,
			u32 grp_status_id39,
			u32 grp_status_id40,
			u32 grp_status_id41,
			u32 grp_status_id42,
			u32 grp_status_id43,
			u32 grp_status_id44,
			u32 grp_status_id45,
			u32 grp_status_id46,
			u32 grp_status_id47,
			u32 grp_status_id48,
			u32 grp_status_id49,
			u32 grp_status_id50,
			u32 grp_status_id51,
			u32 grp_status_id52,
			u32 grp_status_id53,
			u32 grp_status_id54,
			u32 grp_status_id55,
			u32 grp_status_id56,
			u32 grp_status_id57,
			u32 grp_status_id58,
			u32 grp_status_id59,
			u32 grp_status_id60,
			u32 grp_status_id61,
			u32 grp_status_id62,
			u32 grp_status_id63,
			u32 m_or_r)
{
	reg_gicd_igroupr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.grp_status_id32 = grp_status_id32;
	reg.bits.grp_status_id33 = grp_status_id33;
	reg.bits.grp_status_id34 = grp_status_id34;
	reg.bits.grp_status_id35 = grp_status_id35;
	reg.bits.grp_status_id36 = grp_status_id36;
	reg.bits.grp_status_id37 = grp_status_id37;
	reg.bits.grp_status_id38 = grp_status_id38;
	reg.bits.grp_status_id39 = grp_status_id39;
	reg.bits.grp_status_id40 = grp_status_id40;
	reg.bits.grp_status_id41 = grp_status_id41;
	reg.bits.grp_status_id42 = grp_status_id42;
	reg.bits.grp_status_id43 = grp_status_id43;
	reg.bits.grp_status_id44 = grp_status_id44;
	reg.bits.grp_status_id45 = grp_status_id45;
	reg.bits.grp_status_id46 = grp_status_id46;
	reg.bits.grp_status_id47 = grp_status_id47;
	reg.bits.grp_status_id48 = grp_status_id48;
	reg.bits.grp_status_id49 = grp_status_id49;
	reg.bits.grp_status_id50 = grp_status_id50;
	reg.bits.grp_status_id51 = grp_status_id51;
	reg.bits.grp_status_id52 = grp_status_id52;
	reg.bits.grp_status_id53 = grp_status_id53;
	reg.bits.grp_status_id54 = grp_status_id54;
	reg.bits.grp_status_id55 = grp_status_id55;
	reg.bits.grp_status_id56 = grp_status_id56;
	reg.bits.grp_status_id57 = grp_status_id57;
	reg.bits.grp_status_id58 = grp_status_id58;
	reg.bits.grp_status_id59 = grp_status_id59;
	reg.bits.grp_status_id60 = grp_status_id60;
	reg.bits.grp_status_id61 = grp_status_id61;
	reg.bits.grp_status_id62 = grp_status_id62;
	reg.bits.grp_status_id63 = grp_status_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID32
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID33
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID34
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID35
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID36
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID37
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID38
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID39
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID40
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID41
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID42
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID43
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID44
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID45
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID46
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID47
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID48
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID49
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID50
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID51
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID52
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID53
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID54
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID55
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID56
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID57
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID58
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID59
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID60
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID61
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID62
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR1_GRP_STATUS_ID63
fun_set_mod_reg_bit(gicd, igroupr1, grp_status_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2
void set_gicd_igroupr2(u32 reg_addr,
			u32 grp_status_id64,
			u32 grp_status_id65,
			u32 grp_status_id66,
			u32 grp_status_id67,
			u32 grp_status_id68,
			u32 grp_status_id69,
			u32 grp_status_id70,
			u32 grp_status_id71,
			u32 grp_status_id72,
			u32 grp_status_id73,
			u32 grp_status_id74,
			u32 grp_status_id75,
			u32 grp_status_id76,
			u32 grp_status_id77,
			u32 grp_status_id78,
			u32 grp_status_id79,
			u32 grp_status_id80,
			u32 grp_status_id81,
			u32 grp_status_id82,
			u32 grp_status_id83,
			u32 grp_status_id84,
			u32 grp_status_id85,
			u32 grp_status_id86,
			u32 grp_status_id87,
			u32 grp_status_id88,
			u32 grp_status_id89,
			u32 grp_status_id90,
			u32 grp_status_id91,
			u32 grp_status_id92,
			u32 grp_status_id93,
			u32 grp_status_id94,
			u32 grp_status_id95,
			u32 m_or_r)
{
	reg_gicd_igroupr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.grp_status_id64 = grp_status_id64;
	reg.bits.grp_status_id65 = grp_status_id65;
	reg.bits.grp_status_id66 = grp_status_id66;
	reg.bits.grp_status_id67 = grp_status_id67;
	reg.bits.grp_status_id68 = grp_status_id68;
	reg.bits.grp_status_id69 = grp_status_id69;
	reg.bits.grp_status_id70 = grp_status_id70;
	reg.bits.grp_status_id71 = grp_status_id71;
	reg.bits.grp_status_id72 = grp_status_id72;
	reg.bits.grp_status_id73 = grp_status_id73;
	reg.bits.grp_status_id74 = grp_status_id74;
	reg.bits.grp_status_id75 = grp_status_id75;
	reg.bits.grp_status_id76 = grp_status_id76;
	reg.bits.grp_status_id77 = grp_status_id77;
	reg.bits.grp_status_id78 = grp_status_id78;
	reg.bits.grp_status_id79 = grp_status_id79;
	reg.bits.grp_status_id80 = grp_status_id80;
	reg.bits.grp_status_id81 = grp_status_id81;
	reg.bits.grp_status_id82 = grp_status_id82;
	reg.bits.grp_status_id83 = grp_status_id83;
	reg.bits.grp_status_id84 = grp_status_id84;
	reg.bits.grp_status_id85 = grp_status_id85;
	reg.bits.grp_status_id86 = grp_status_id86;
	reg.bits.grp_status_id87 = grp_status_id87;
	reg.bits.grp_status_id88 = grp_status_id88;
	reg.bits.grp_status_id89 = grp_status_id89;
	reg.bits.grp_status_id90 = grp_status_id90;
	reg.bits.grp_status_id91 = grp_status_id91;
	reg.bits.grp_status_id92 = grp_status_id92;
	reg.bits.grp_status_id93 = grp_status_id93;
	reg.bits.grp_status_id94 = grp_status_id94;
	reg.bits.grp_status_id95 = grp_status_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID64
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID65
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID66
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID67
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID68
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID69
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID70
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID71
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID72
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID73
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID74
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID75
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID76
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID77
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID78
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID79
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID80
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID81
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID82
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID83
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID84
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID85
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID86
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID87
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID88
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID89
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID90
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID91
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID92
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID93
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID94
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR2_GRP_STATUS_ID95
fun_set_mod_reg_bit(gicd, igroupr2, grp_status_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3
void set_gicd_igroupr3(u32 reg_addr,
			u32 grp_status_id96,
			u32 grp_status_id97,
			u32 grp_status_id98,
			u32 grp_status_id99,
			u32 grp_status_id100,
			u32 grp_status_id101,
			u32 grp_status_id102,
			u32 grp_status_id103,
			u32 grp_status_id104,
			u32 grp_status_id105,
			u32 grp_status_id106,
			u32 grp_status_id107,
			u32 grp_status_id108,
			u32 grp_status_id109,
			u32 grp_status_id110,
			u32 grp_status_id111,
			u32 grp_status_id112,
			u32 grp_status_id113,
			u32 grp_status_id114,
			u32 grp_status_id115,
			u32 grp_status_id116,
			u32 grp_status_id117,
			u32 grp_status_id118,
			u32 grp_status_id119,
			u32 grp_status_id120,
			u32 grp_status_id121,
			u32 grp_status_id122,
			u32 grp_status_id123,
			u32 grp_status_id124,
			u32 grp_status_id125,
			u32 grp_status_id126,
			u32 grp_status_id127,
			u32 m_or_r)
{
	reg_gicd_igroupr3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.grp_status_id96 = grp_status_id96;
	reg.bits.grp_status_id97 = grp_status_id97;
	reg.bits.grp_status_id98 = grp_status_id98;
	reg.bits.grp_status_id99 = grp_status_id99;
	reg.bits.grp_status_id100 = grp_status_id100;
	reg.bits.grp_status_id101 = grp_status_id101;
	reg.bits.grp_status_id102 = grp_status_id102;
	reg.bits.grp_status_id103 = grp_status_id103;
	reg.bits.grp_status_id104 = grp_status_id104;
	reg.bits.grp_status_id105 = grp_status_id105;
	reg.bits.grp_status_id106 = grp_status_id106;
	reg.bits.grp_status_id107 = grp_status_id107;
	reg.bits.grp_status_id108 = grp_status_id108;
	reg.bits.grp_status_id109 = grp_status_id109;
	reg.bits.grp_status_id110 = grp_status_id110;
	reg.bits.grp_status_id111 = grp_status_id111;
	reg.bits.grp_status_id112 = grp_status_id112;
	reg.bits.grp_status_id113 = grp_status_id113;
	reg.bits.grp_status_id114 = grp_status_id114;
	reg.bits.grp_status_id115 = grp_status_id115;
	reg.bits.grp_status_id116 = grp_status_id116;
	reg.bits.grp_status_id117 = grp_status_id117;
	reg.bits.grp_status_id118 = grp_status_id118;
	reg.bits.grp_status_id119 = grp_status_id119;
	reg.bits.grp_status_id120 = grp_status_id120;
	reg.bits.grp_status_id121 = grp_status_id121;
	reg.bits.grp_status_id122 = grp_status_id122;
	reg.bits.grp_status_id123 = grp_status_id123;
	reg.bits.grp_status_id124 = grp_status_id124;
	reg.bits.grp_status_id125 = grp_status_id125;
	reg.bits.grp_status_id126 = grp_status_id126;
	reg.bits.grp_status_id127 = grp_status_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID96
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID97
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID98
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID99
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID100
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID101
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID102
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID103
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID104
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID105
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID106
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID107
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID108
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID109
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID110
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID111
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID112
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID113
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID114
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID115
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID116
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID117
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID118
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID119
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID120
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID121
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID122
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID123
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID124
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID125
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID126
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR3_GRP_STATUS_ID127
fun_set_mod_reg_bit(gicd, igroupr3, grp_status_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4
void set_gicd_igroupr4(u32 reg_addr,
			u32 grp_status_id128,
			u32 grp_status_id129,
			u32 grp_status_id130,
			u32 grp_status_id131,
			u32 grp_status_id132,
			u32 grp_status_id133,
			u32 grp_status_id134,
			u32 grp_status_id135,
			u32 grp_status_id136,
			u32 grp_status_id137,
			u32 grp_status_id138,
			u32 grp_status_id139,
			u32 grp_status_id140,
			u32 grp_status_id141,
			u32 grp_status_id142,
			u32 grp_status_id143,
			u32 grp_status_id144,
			u32 grp_status_id145,
			u32 grp_status_id146,
			u32 grp_status_id147,
			u32 grp_status_id148,
			u32 grp_status_id149,
			u32 grp_status_id150,
			u32 grp_status_id151,
			u32 grp_status_id152,
			u32 grp_status_id153,
			u32 grp_status_id154,
			u32 grp_status_id155,
			u32 grp_status_id156,
			u32 grp_status_id157,
			u32 grp_status_id158,
			u32 grp_status_id159,
			u32 m_or_r)
{
	reg_gicd_igroupr4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.grp_status_id128 = grp_status_id128;
	reg.bits.grp_status_id129 = grp_status_id129;
	reg.bits.grp_status_id130 = grp_status_id130;
	reg.bits.grp_status_id131 = grp_status_id131;
	reg.bits.grp_status_id132 = grp_status_id132;
	reg.bits.grp_status_id133 = grp_status_id133;
	reg.bits.grp_status_id134 = grp_status_id134;
	reg.bits.grp_status_id135 = grp_status_id135;
	reg.bits.grp_status_id136 = grp_status_id136;
	reg.bits.grp_status_id137 = grp_status_id137;
	reg.bits.grp_status_id138 = grp_status_id138;
	reg.bits.grp_status_id139 = grp_status_id139;
	reg.bits.grp_status_id140 = grp_status_id140;
	reg.bits.grp_status_id141 = grp_status_id141;
	reg.bits.grp_status_id142 = grp_status_id142;
	reg.bits.grp_status_id143 = grp_status_id143;
	reg.bits.grp_status_id144 = grp_status_id144;
	reg.bits.grp_status_id145 = grp_status_id145;
	reg.bits.grp_status_id146 = grp_status_id146;
	reg.bits.grp_status_id147 = grp_status_id147;
	reg.bits.grp_status_id148 = grp_status_id148;
	reg.bits.grp_status_id149 = grp_status_id149;
	reg.bits.grp_status_id150 = grp_status_id150;
	reg.bits.grp_status_id151 = grp_status_id151;
	reg.bits.grp_status_id152 = grp_status_id152;
	reg.bits.grp_status_id153 = grp_status_id153;
	reg.bits.grp_status_id154 = grp_status_id154;
	reg.bits.grp_status_id155 = grp_status_id155;
	reg.bits.grp_status_id156 = grp_status_id156;
	reg.bits.grp_status_id157 = grp_status_id157;
	reg.bits.grp_status_id158 = grp_status_id158;
	reg.bits.grp_status_id159 = grp_status_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID128
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID129
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID130
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID131
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID132
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID133
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID134
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID135
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID136
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID137
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID138
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID139
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID140
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID141
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID142
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID143
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID144
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID145
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID146
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID147
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID148
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID149
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID150
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID151
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID152
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID153
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID154
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID155
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID156
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID157
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID158
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR4_GRP_STATUS_ID159
fun_set_mod_reg_bit(gicd, igroupr4, grp_status_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5
void set_gicd_igroupr5(u32 reg_addr,
			u32 grp_status_id160,
			u32 grp_status_id161,
			u32 grp_status_id162,
			u32 grp_status_id163,
			u32 grp_status_id164,
			u32 grp_status_id165,
			u32 grp_status_id166,
			u32 grp_status_id167,
			u32 grp_status_id168,
			u32 grp_status_id169,
			u32 grp_status_id170,
			u32 grp_status_id171,
			u32 grp_status_id172,
			u32 grp_status_id173,
			u32 grp_status_id174,
			u32 grp_status_id175,
			u32 grp_status_id176,
			u32 grp_status_id177,
			u32 grp_status_id178,
			u32 grp_status_id179,
			u32 grp_status_id180,
			u32 grp_status_id181,
			u32 grp_status_id182,
			u32 grp_status_id183,
			u32 grp_status_id184,
			u32 grp_status_id185,
			u32 grp_status_id186,
			u32 grp_status_id187,
			u32 grp_status_id188,
			u32 grp_status_id189,
			u32 grp_status_id190,
			u32 grp_status_id191,
			u32 m_or_r)
{
	reg_gicd_igroupr5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.grp_status_id160 = grp_status_id160;
	reg.bits.grp_status_id161 = grp_status_id161;
	reg.bits.grp_status_id162 = grp_status_id162;
	reg.bits.grp_status_id163 = grp_status_id163;
	reg.bits.grp_status_id164 = grp_status_id164;
	reg.bits.grp_status_id165 = grp_status_id165;
	reg.bits.grp_status_id166 = grp_status_id166;
	reg.bits.grp_status_id167 = grp_status_id167;
	reg.bits.grp_status_id168 = grp_status_id168;
	reg.bits.grp_status_id169 = grp_status_id169;
	reg.bits.grp_status_id170 = grp_status_id170;
	reg.bits.grp_status_id171 = grp_status_id171;
	reg.bits.grp_status_id172 = grp_status_id172;
	reg.bits.grp_status_id173 = grp_status_id173;
	reg.bits.grp_status_id174 = grp_status_id174;
	reg.bits.grp_status_id175 = grp_status_id175;
	reg.bits.grp_status_id176 = grp_status_id176;
	reg.bits.grp_status_id177 = grp_status_id177;
	reg.bits.grp_status_id178 = grp_status_id178;
	reg.bits.grp_status_id179 = grp_status_id179;
	reg.bits.grp_status_id180 = grp_status_id180;
	reg.bits.grp_status_id181 = grp_status_id181;
	reg.bits.grp_status_id182 = grp_status_id182;
	reg.bits.grp_status_id183 = grp_status_id183;
	reg.bits.grp_status_id184 = grp_status_id184;
	reg.bits.grp_status_id185 = grp_status_id185;
	reg.bits.grp_status_id186 = grp_status_id186;
	reg.bits.grp_status_id187 = grp_status_id187;
	reg.bits.grp_status_id188 = grp_status_id188;
	reg.bits.grp_status_id189 = grp_status_id189;
	reg.bits.grp_status_id190 = grp_status_id190;
	reg.bits.grp_status_id191 = grp_status_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID160
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID161
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID162
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID163
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID164
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID165
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID166
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID167
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID168
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID169
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID170
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID171
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID172
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID173
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID174
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID175
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID176
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID177
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID178
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID179
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID180
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID181
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID182
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID183
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID184
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID185
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID186
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID187
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID188
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID189
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID190
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR5_GRP_STATUS_ID191
fun_set_mod_reg_bit(gicd, igroupr5, grp_status_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6
void set_gicd_igroupr6(u32 reg_addr,
			u32 grp_status_id192,
			u32 grp_status_id193,
			u32 grp_status_id194,
			u32 grp_status_id195,
			u32 grp_status_id196,
			u32 grp_status_id197,
			u32 grp_status_id198,
			u32 grp_status_id199,
			u32 grp_status_id200,
			u32 grp_status_id201,
			u32 grp_status_id202,
			u32 grp_status_id203,
			u32 grp_status_id204,
			u32 grp_status_id205,
			u32 grp_status_id206,
			u32 grp_status_id207,
			u32 grp_status_id208,
			u32 grp_status_id209,
			u32 grp_status_id210,
			u32 grp_status_id211,
			u32 grp_status_id212,
			u32 grp_status_id213,
			u32 grp_status_id214,
			u32 grp_status_id215,
			u32 grp_status_id216,
			u32 grp_status_id217,
			u32 grp_status_id218,
			u32 grp_status_id219,
			u32 grp_status_id220,
			u32 grp_status_id221,
			u32 grp_status_id222,
			u32 grp_status_id223,
			u32 m_or_r)
{
	reg_gicd_igroupr6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.grp_status_id192 = grp_status_id192;
	reg.bits.grp_status_id193 = grp_status_id193;
	reg.bits.grp_status_id194 = grp_status_id194;
	reg.bits.grp_status_id195 = grp_status_id195;
	reg.bits.grp_status_id196 = grp_status_id196;
	reg.bits.grp_status_id197 = grp_status_id197;
	reg.bits.grp_status_id198 = grp_status_id198;
	reg.bits.grp_status_id199 = grp_status_id199;
	reg.bits.grp_status_id200 = grp_status_id200;
	reg.bits.grp_status_id201 = grp_status_id201;
	reg.bits.grp_status_id202 = grp_status_id202;
	reg.bits.grp_status_id203 = grp_status_id203;
	reg.bits.grp_status_id204 = grp_status_id204;
	reg.bits.grp_status_id205 = grp_status_id205;
	reg.bits.grp_status_id206 = grp_status_id206;
	reg.bits.grp_status_id207 = grp_status_id207;
	reg.bits.grp_status_id208 = grp_status_id208;
	reg.bits.grp_status_id209 = grp_status_id209;
	reg.bits.grp_status_id210 = grp_status_id210;
	reg.bits.grp_status_id211 = grp_status_id211;
	reg.bits.grp_status_id212 = grp_status_id212;
	reg.bits.grp_status_id213 = grp_status_id213;
	reg.bits.grp_status_id214 = grp_status_id214;
	reg.bits.grp_status_id215 = grp_status_id215;
	reg.bits.grp_status_id216 = grp_status_id216;
	reg.bits.grp_status_id217 = grp_status_id217;
	reg.bits.grp_status_id218 = grp_status_id218;
	reg.bits.grp_status_id219 = grp_status_id219;
	reg.bits.grp_status_id220 = grp_status_id220;
	reg.bits.grp_status_id221 = grp_status_id221;
	reg.bits.grp_status_id222 = grp_status_id222;
	reg.bits.grp_status_id223 = grp_status_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID192
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID193
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID194
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID195
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID196
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID197
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID198
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID199
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID200
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID201
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID202
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID203
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID204
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID205
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID206
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID207
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID208
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID209
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID210
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID211
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID212
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID213
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID214
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID215
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID216
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID217
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID218
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID219
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID220
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID221
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID222
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IGROUPR6_GRP_STATUS_ID223
fun_set_mod_reg_bit(gicd, igroupr6, grp_status_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0
void set_gicd_isenabler0(u32 reg_addr,
			u32 set_en_id0,
			u32 set_en_id1,
			u32 set_en_id2,
			u32 set_en_id3,
			u32 set_en_id4,
			u32 set_en_id5,
			u32 set_en_id6,
			u32 set_en_id7,
			u32 set_en_id8,
			u32 set_en_id9,
			u32 set_en_id10,
			u32 set_en_id11,
			u32 set_en_id12,
			u32 set_en_id13,
			u32 set_en_id14,
			u32 set_en_id15,
			u32 set_en_id16,
			u32 set_en_id17,
			u32 set_en_id18,
			u32 set_en_id19,
			u32 set_en_id20,
			u32 set_en_id21,
			u32 set_en_id22,
			u32 set_en_id23,
			u32 set_en_id24,
			u32 set_en_id25,
			u32 set_en_id26,
			u32 set_en_id27,
			u32 set_en_id28,
			u32 set_en_id29,
			u32 set_en_id30,
			u32 set_en_id31,
			u32 m_or_r)
{
	reg_gicd_isenabler0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_en_id0 = set_en_id0;
	reg.bits.set_en_id1 = set_en_id1;
	reg.bits.set_en_id2 = set_en_id2;
	reg.bits.set_en_id3 = set_en_id3;
	reg.bits.set_en_id4 = set_en_id4;
	reg.bits.set_en_id5 = set_en_id5;
	reg.bits.set_en_id6 = set_en_id6;
	reg.bits.set_en_id7 = set_en_id7;
	reg.bits.set_en_id8 = set_en_id8;
	reg.bits.set_en_id9 = set_en_id9;
	reg.bits.set_en_id10 = set_en_id10;
	reg.bits.set_en_id11 = set_en_id11;
	reg.bits.set_en_id12 = set_en_id12;
	reg.bits.set_en_id13 = set_en_id13;
	reg.bits.set_en_id14 = set_en_id14;
	reg.bits.set_en_id15 = set_en_id15;
	reg.bits.set_en_id16 = set_en_id16;
	reg.bits.set_en_id17 = set_en_id17;
	reg.bits.set_en_id18 = set_en_id18;
	reg.bits.set_en_id19 = set_en_id19;
	reg.bits.set_en_id20 = set_en_id20;
	reg.bits.set_en_id21 = set_en_id21;
	reg.bits.set_en_id22 = set_en_id22;
	reg.bits.set_en_id23 = set_en_id23;
	reg.bits.set_en_id24 = set_en_id24;
	reg.bits.set_en_id25 = set_en_id25;
	reg.bits.set_en_id26 = set_en_id26;
	reg.bits.set_en_id27 = set_en_id27;
	reg.bits.set_en_id28 = set_en_id28;
	reg.bits.set_en_id29 = set_en_id29;
	reg.bits.set_en_id30 = set_en_id30;
	reg.bits.set_en_id31 = set_en_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID0
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID1
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID2
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID3
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID4
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID5
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID6
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID7
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID8
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID9
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID10
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID11
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID12
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID13
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID14
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID15
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID16
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID17
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID18
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID19
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID20
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID21
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID22
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID23
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID24
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID25
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID26
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID27
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID28
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID29
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID30
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER0_SET_EN_ID31
fun_set_mod_reg_bit(gicd, isenabler0, set_en_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1
void set_gicd_isenabler1(u32 reg_addr,
			u32 set_en_id32,
			u32 set_en_id33,
			u32 set_en_id34,
			u32 set_en_id35,
			u32 set_en_id36,
			u32 set_en_id37,
			u32 set_en_id38,
			u32 set_en_id39,
			u32 set_en_id40,
			u32 set_en_id41,
			u32 set_en_id42,
			u32 set_en_id43,
			u32 set_en_id44,
			u32 set_en_id45,
			u32 set_en_id46,
			u32 set_en_id47,
			u32 set_en_id48,
			u32 set_en_id49,
			u32 set_en_id50,
			u32 set_en_id51,
			u32 set_en_id52,
			u32 set_en_id53,
			u32 set_en_id54,
			u32 set_en_id55,
			u32 set_en_id56,
			u32 set_en_id57,
			u32 set_en_id58,
			u32 set_en_id59,
			u32 set_en_id60,
			u32 set_en_id61,
			u32 set_en_id62,
			u32 set_en_id63,
			u32 m_or_r)
{
	reg_gicd_isenabler1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_en_id32 = set_en_id32;
	reg.bits.set_en_id33 = set_en_id33;
	reg.bits.set_en_id34 = set_en_id34;
	reg.bits.set_en_id35 = set_en_id35;
	reg.bits.set_en_id36 = set_en_id36;
	reg.bits.set_en_id37 = set_en_id37;
	reg.bits.set_en_id38 = set_en_id38;
	reg.bits.set_en_id39 = set_en_id39;
	reg.bits.set_en_id40 = set_en_id40;
	reg.bits.set_en_id41 = set_en_id41;
	reg.bits.set_en_id42 = set_en_id42;
	reg.bits.set_en_id43 = set_en_id43;
	reg.bits.set_en_id44 = set_en_id44;
	reg.bits.set_en_id45 = set_en_id45;
	reg.bits.set_en_id46 = set_en_id46;
	reg.bits.set_en_id47 = set_en_id47;
	reg.bits.set_en_id48 = set_en_id48;
	reg.bits.set_en_id49 = set_en_id49;
	reg.bits.set_en_id50 = set_en_id50;
	reg.bits.set_en_id51 = set_en_id51;
	reg.bits.set_en_id52 = set_en_id52;
	reg.bits.set_en_id53 = set_en_id53;
	reg.bits.set_en_id54 = set_en_id54;
	reg.bits.set_en_id55 = set_en_id55;
	reg.bits.set_en_id56 = set_en_id56;
	reg.bits.set_en_id57 = set_en_id57;
	reg.bits.set_en_id58 = set_en_id58;
	reg.bits.set_en_id59 = set_en_id59;
	reg.bits.set_en_id60 = set_en_id60;
	reg.bits.set_en_id61 = set_en_id61;
	reg.bits.set_en_id62 = set_en_id62;
	reg.bits.set_en_id63 = set_en_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID32
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID33
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID34
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID35
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID36
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID37
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID38
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID39
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID40
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID41
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID42
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID43
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID44
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID45
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID46
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID47
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID48
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID49
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID50
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID51
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID52
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID53
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID54
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID55
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID56
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID57
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID58
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID59
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID60
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID61
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID62
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER1_SET_EN_ID63
fun_set_mod_reg_bit(gicd, isenabler1, set_en_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2
void set_gicd_isenabler2(u32 reg_addr,
			u32 set_en_id64,
			u32 set_en_id65,
			u32 set_en_id66,
			u32 set_en_id67,
			u32 set_en_id68,
			u32 set_en_id69,
			u32 set_en_id70,
			u32 set_en_id71,
			u32 set_en_id72,
			u32 set_en_id73,
			u32 set_en_id74,
			u32 set_en_id75,
			u32 set_en_id76,
			u32 set_en_id77,
			u32 set_en_id78,
			u32 set_en_id79,
			u32 set_en_id80,
			u32 set_en_id81,
			u32 set_en_id82,
			u32 set_en_id83,
			u32 set_en_id84,
			u32 set_en_id85,
			u32 set_en_id86,
			u32 set_en_id87,
			u32 set_en_id88,
			u32 set_en_id89,
			u32 set_en_id90,
			u32 set_en_id91,
			u32 set_en_id92,
			u32 set_en_id93,
			u32 set_en_id94,
			u32 set_en_id95,
			u32 m_or_r)
{
	reg_gicd_isenabler2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_en_id64 = set_en_id64;
	reg.bits.set_en_id65 = set_en_id65;
	reg.bits.set_en_id66 = set_en_id66;
	reg.bits.set_en_id67 = set_en_id67;
	reg.bits.set_en_id68 = set_en_id68;
	reg.bits.set_en_id69 = set_en_id69;
	reg.bits.set_en_id70 = set_en_id70;
	reg.bits.set_en_id71 = set_en_id71;
	reg.bits.set_en_id72 = set_en_id72;
	reg.bits.set_en_id73 = set_en_id73;
	reg.bits.set_en_id74 = set_en_id74;
	reg.bits.set_en_id75 = set_en_id75;
	reg.bits.set_en_id76 = set_en_id76;
	reg.bits.set_en_id77 = set_en_id77;
	reg.bits.set_en_id78 = set_en_id78;
	reg.bits.set_en_id79 = set_en_id79;
	reg.bits.set_en_id80 = set_en_id80;
	reg.bits.set_en_id81 = set_en_id81;
	reg.bits.set_en_id82 = set_en_id82;
	reg.bits.set_en_id83 = set_en_id83;
	reg.bits.set_en_id84 = set_en_id84;
	reg.bits.set_en_id85 = set_en_id85;
	reg.bits.set_en_id86 = set_en_id86;
	reg.bits.set_en_id87 = set_en_id87;
	reg.bits.set_en_id88 = set_en_id88;
	reg.bits.set_en_id89 = set_en_id89;
	reg.bits.set_en_id90 = set_en_id90;
	reg.bits.set_en_id91 = set_en_id91;
	reg.bits.set_en_id92 = set_en_id92;
	reg.bits.set_en_id93 = set_en_id93;
	reg.bits.set_en_id94 = set_en_id94;
	reg.bits.set_en_id95 = set_en_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID64
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID65
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID66
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID67
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID68
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID69
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID70
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID71
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID72
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID73
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID74
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID75
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID76
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID77
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID78
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID79
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID80
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID81
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID82
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID83
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID84
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID85
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID86
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID87
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID88
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID89
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID90
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID91
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID92
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID93
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID94
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER2_SET_EN_ID95
fun_set_mod_reg_bit(gicd, isenabler2, set_en_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3
void set_gicd_isenabler3(u32 reg_addr,
			u32 set_en_id96,
			u32 set_en_id97,
			u32 set_en_id98,
			u32 set_en_id99,
			u32 set_en_id100,
			u32 set_en_id101,
			u32 set_en_id102,
			u32 set_en_id103,
			u32 set_en_id104,
			u32 set_en_id105,
			u32 set_en_id106,
			u32 set_en_id107,
			u32 set_en_id108,
			u32 set_en_id109,
			u32 set_en_id110,
			u32 set_en_id111,
			u32 set_en_id112,
			u32 set_en_id113,
			u32 set_en_id114,
			u32 set_en_id115,
			u32 set_en_id116,
			u32 set_en_id117,
			u32 set_en_id118,
			u32 set_en_id119,
			u32 set_en_id120,
			u32 set_en_id121,
			u32 set_en_id122,
			u32 set_en_id123,
			u32 set_en_id124,
			u32 set_en_id125,
			u32 set_en_id126,
			u32 set_en_id127,
			u32 m_or_r)
{
	reg_gicd_isenabler3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_en_id96 = set_en_id96;
	reg.bits.set_en_id97 = set_en_id97;
	reg.bits.set_en_id98 = set_en_id98;
	reg.bits.set_en_id99 = set_en_id99;
	reg.bits.set_en_id100 = set_en_id100;
	reg.bits.set_en_id101 = set_en_id101;
	reg.bits.set_en_id102 = set_en_id102;
	reg.bits.set_en_id103 = set_en_id103;
	reg.bits.set_en_id104 = set_en_id104;
	reg.bits.set_en_id105 = set_en_id105;
	reg.bits.set_en_id106 = set_en_id106;
	reg.bits.set_en_id107 = set_en_id107;
	reg.bits.set_en_id108 = set_en_id108;
	reg.bits.set_en_id109 = set_en_id109;
	reg.bits.set_en_id110 = set_en_id110;
	reg.bits.set_en_id111 = set_en_id111;
	reg.bits.set_en_id112 = set_en_id112;
	reg.bits.set_en_id113 = set_en_id113;
	reg.bits.set_en_id114 = set_en_id114;
	reg.bits.set_en_id115 = set_en_id115;
	reg.bits.set_en_id116 = set_en_id116;
	reg.bits.set_en_id117 = set_en_id117;
	reg.bits.set_en_id118 = set_en_id118;
	reg.bits.set_en_id119 = set_en_id119;
	reg.bits.set_en_id120 = set_en_id120;
	reg.bits.set_en_id121 = set_en_id121;
	reg.bits.set_en_id122 = set_en_id122;
	reg.bits.set_en_id123 = set_en_id123;
	reg.bits.set_en_id124 = set_en_id124;
	reg.bits.set_en_id125 = set_en_id125;
	reg.bits.set_en_id126 = set_en_id126;
	reg.bits.set_en_id127 = set_en_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID96
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID97
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID98
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID99
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID100
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID101
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID102
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID103
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID104
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID105
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID106
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID107
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID108
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID109
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID110
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID111
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID112
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID113
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID114
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID115
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID116
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID117
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID118
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID119
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID120
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID121
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID122
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID123
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID124
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID125
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID126
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER3_SET_EN_ID127
fun_set_mod_reg_bit(gicd, isenabler3, set_en_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4
void set_gicd_isenabler4(u32 reg_addr,
			u32 set_en_id128,
			u32 set_en_id129,
			u32 set_en_id130,
			u32 set_en_id131,
			u32 set_en_id132,
			u32 set_en_id133,
			u32 set_en_id134,
			u32 set_en_id135,
			u32 set_en_id136,
			u32 set_en_id137,
			u32 set_en_id138,
			u32 set_en_id139,
			u32 set_en_id140,
			u32 set_en_id141,
			u32 set_en_id142,
			u32 set_en_id143,
			u32 set_en_id144,
			u32 set_en_id145,
			u32 set_en_id146,
			u32 set_en_id147,
			u32 set_en_id148,
			u32 set_en_id149,
			u32 set_en_id150,
			u32 set_en_id151,
			u32 set_en_id152,
			u32 set_en_id153,
			u32 set_en_id154,
			u32 set_en_id155,
			u32 set_en_id156,
			u32 set_en_id157,
			u32 set_en_id158,
			u32 set_en_id159,
			u32 m_or_r)
{
	reg_gicd_isenabler4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_en_id128 = set_en_id128;
	reg.bits.set_en_id129 = set_en_id129;
	reg.bits.set_en_id130 = set_en_id130;
	reg.bits.set_en_id131 = set_en_id131;
	reg.bits.set_en_id132 = set_en_id132;
	reg.bits.set_en_id133 = set_en_id133;
	reg.bits.set_en_id134 = set_en_id134;
	reg.bits.set_en_id135 = set_en_id135;
	reg.bits.set_en_id136 = set_en_id136;
	reg.bits.set_en_id137 = set_en_id137;
	reg.bits.set_en_id138 = set_en_id138;
	reg.bits.set_en_id139 = set_en_id139;
	reg.bits.set_en_id140 = set_en_id140;
	reg.bits.set_en_id141 = set_en_id141;
	reg.bits.set_en_id142 = set_en_id142;
	reg.bits.set_en_id143 = set_en_id143;
	reg.bits.set_en_id144 = set_en_id144;
	reg.bits.set_en_id145 = set_en_id145;
	reg.bits.set_en_id146 = set_en_id146;
	reg.bits.set_en_id147 = set_en_id147;
	reg.bits.set_en_id148 = set_en_id148;
	reg.bits.set_en_id149 = set_en_id149;
	reg.bits.set_en_id150 = set_en_id150;
	reg.bits.set_en_id151 = set_en_id151;
	reg.bits.set_en_id152 = set_en_id152;
	reg.bits.set_en_id153 = set_en_id153;
	reg.bits.set_en_id154 = set_en_id154;
	reg.bits.set_en_id155 = set_en_id155;
	reg.bits.set_en_id156 = set_en_id156;
	reg.bits.set_en_id157 = set_en_id157;
	reg.bits.set_en_id158 = set_en_id158;
	reg.bits.set_en_id159 = set_en_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID128
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID129
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID130
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID131
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID132
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID133
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID134
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID135
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID136
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID137
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID138
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID139
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID140
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID141
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID142
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID143
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID144
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID145
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID146
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID147
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID148
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID149
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID150
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID151
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID152
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID153
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID154
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID155
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID156
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID157
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID158
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER4_SET_EN_ID159
fun_set_mod_reg_bit(gicd, isenabler4, set_en_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5
void set_gicd_isenabler5(u32 reg_addr,
			u32 set_en_id160,
			u32 set_en_id161,
			u32 set_en_id162,
			u32 set_en_id163,
			u32 set_en_id164,
			u32 set_en_id165,
			u32 set_en_id166,
			u32 set_en_id167,
			u32 set_en_id168,
			u32 set_en_id169,
			u32 set_en_id170,
			u32 set_en_id171,
			u32 set_en_id172,
			u32 set_en_id173,
			u32 set_en_id174,
			u32 set_en_id175,
			u32 set_en_id176,
			u32 set_en_id177,
			u32 set_en_id178,
			u32 set_en_id179,
			u32 set_en_id180,
			u32 set_en_id181,
			u32 set_en_id182,
			u32 set_en_id183,
			u32 set_en_id184,
			u32 set_en_id185,
			u32 set_en_id186,
			u32 set_en_id187,
			u32 set_en_id188,
			u32 set_en_id189,
			u32 set_en_id190,
			u32 set_en_id191,
			u32 m_or_r)
{
	reg_gicd_isenabler5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_en_id160 = set_en_id160;
	reg.bits.set_en_id161 = set_en_id161;
	reg.bits.set_en_id162 = set_en_id162;
	reg.bits.set_en_id163 = set_en_id163;
	reg.bits.set_en_id164 = set_en_id164;
	reg.bits.set_en_id165 = set_en_id165;
	reg.bits.set_en_id166 = set_en_id166;
	reg.bits.set_en_id167 = set_en_id167;
	reg.bits.set_en_id168 = set_en_id168;
	reg.bits.set_en_id169 = set_en_id169;
	reg.bits.set_en_id170 = set_en_id170;
	reg.bits.set_en_id171 = set_en_id171;
	reg.bits.set_en_id172 = set_en_id172;
	reg.bits.set_en_id173 = set_en_id173;
	reg.bits.set_en_id174 = set_en_id174;
	reg.bits.set_en_id175 = set_en_id175;
	reg.bits.set_en_id176 = set_en_id176;
	reg.bits.set_en_id177 = set_en_id177;
	reg.bits.set_en_id178 = set_en_id178;
	reg.bits.set_en_id179 = set_en_id179;
	reg.bits.set_en_id180 = set_en_id180;
	reg.bits.set_en_id181 = set_en_id181;
	reg.bits.set_en_id182 = set_en_id182;
	reg.bits.set_en_id183 = set_en_id183;
	reg.bits.set_en_id184 = set_en_id184;
	reg.bits.set_en_id185 = set_en_id185;
	reg.bits.set_en_id186 = set_en_id186;
	reg.bits.set_en_id187 = set_en_id187;
	reg.bits.set_en_id188 = set_en_id188;
	reg.bits.set_en_id189 = set_en_id189;
	reg.bits.set_en_id190 = set_en_id190;
	reg.bits.set_en_id191 = set_en_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID160
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID161
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID162
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID163
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID164
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID165
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID166
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID167
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID168
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID169
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID170
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID171
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID172
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID173
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID174
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID175
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID176
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID177
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID178
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID179
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID180
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID181
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID182
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID183
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID184
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID185
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID186
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID187
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID188
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID189
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID190
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER5_SET_EN_ID191
fun_set_mod_reg_bit(gicd, isenabler5, set_en_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6
void set_gicd_isenabler6(u32 reg_addr,
			u32 set_en_id192,
			u32 set_en_id193,
			u32 set_en_id194,
			u32 set_en_id195,
			u32 set_en_id196,
			u32 set_en_id197,
			u32 set_en_id198,
			u32 set_en_id199,
			u32 set_en_id200,
			u32 set_en_id201,
			u32 set_en_id202,
			u32 set_en_id203,
			u32 set_en_id204,
			u32 set_en_id205,
			u32 set_en_id206,
			u32 set_en_id207,
			u32 set_en_id208,
			u32 set_en_id209,
			u32 set_en_id210,
			u32 set_en_id211,
			u32 set_en_id212,
			u32 set_en_id213,
			u32 set_en_id214,
			u32 set_en_id215,
			u32 set_en_id216,
			u32 set_en_id217,
			u32 set_en_id218,
			u32 set_en_id219,
			u32 set_en_id220,
			u32 set_en_id221,
			u32 set_en_id222,
			u32 set_en_id223,
			u32 m_or_r)
{
	reg_gicd_isenabler6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_en_id192 = set_en_id192;
	reg.bits.set_en_id193 = set_en_id193;
	reg.bits.set_en_id194 = set_en_id194;
	reg.bits.set_en_id195 = set_en_id195;
	reg.bits.set_en_id196 = set_en_id196;
	reg.bits.set_en_id197 = set_en_id197;
	reg.bits.set_en_id198 = set_en_id198;
	reg.bits.set_en_id199 = set_en_id199;
	reg.bits.set_en_id200 = set_en_id200;
	reg.bits.set_en_id201 = set_en_id201;
	reg.bits.set_en_id202 = set_en_id202;
	reg.bits.set_en_id203 = set_en_id203;
	reg.bits.set_en_id204 = set_en_id204;
	reg.bits.set_en_id205 = set_en_id205;
	reg.bits.set_en_id206 = set_en_id206;
	reg.bits.set_en_id207 = set_en_id207;
	reg.bits.set_en_id208 = set_en_id208;
	reg.bits.set_en_id209 = set_en_id209;
	reg.bits.set_en_id210 = set_en_id210;
	reg.bits.set_en_id211 = set_en_id211;
	reg.bits.set_en_id212 = set_en_id212;
	reg.bits.set_en_id213 = set_en_id213;
	reg.bits.set_en_id214 = set_en_id214;
	reg.bits.set_en_id215 = set_en_id215;
	reg.bits.set_en_id216 = set_en_id216;
	reg.bits.set_en_id217 = set_en_id217;
	reg.bits.set_en_id218 = set_en_id218;
	reg.bits.set_en_id219 = set_en_id219;
	reg.bits.set_en_id220 = set_en_id220;
	reg.bits.set_en_id221 = set_en_id221;
	reg.bits.set_en_id222 = set_en_id222;
	reg.bits.set_en_id223 = set_en_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID192
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID193
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID194
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID195
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID196
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID197
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID198
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID199
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID200
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID201
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID202
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID203
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID204
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID205
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID206
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID207
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID208
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID209
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID210
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID211
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID212
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID213
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID214
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID215
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID216
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID217
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID218
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID219
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID220
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID221
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID222
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISENABLER6_SET_EN_ID223
fun_set_mod_reg_bit(gicd, isenabler6, set_en_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0
void set_gicd_icenabler0(u32 reg_addr,
			u32 clr_en_id0,
			u32 clr_en_id1,
			u32 clr_en_id2,
			u32 clr_en_id3,
			u32 clr_en_id4,
			u32 clr_en_id5,
			u32 clr_en_id6,
			u32 clr_en_id7,
			u32 clr_en_id8,
			u32 clr_en_id9,
			u32 clr_en_id10,
			u32 clr_en_id11,
			u32 clr_en_id12,
			u32 clr_en_id13,
			u32 clr_en_id14,
			u32 clr_en_id15,
			u32 clr_en_id16,
			u32 clr_en_id17,
			u32 clr_en_id18,
			u32 clr_en_id19,
			u32 clr_en_id20,
			u32 clr_en_id21,
			u32 clr_en_id22,
			u32 clr_en_id23,
			u32 clr_en_id24,
			u32 clr_en_id25,
			u32 clr_en_id26,
			u32 clr_en_id27,
			u32 clr_en_id28,
			u32 clr_en_id29,
			u32 clr_en_id30,
			u32 clr_en_id31,
			u32 m_or_r)
{
	reg_gicd_icenabler0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_en_id0 = clr_en_id0;
	reg.bits.clr_en_id1 = clr_en_id1;
	reg.bits.clr_en_id2 = clr_en_id2;
	reg.bits.clr_en_id3 = clr_en_id3;
	reg.bits.clr_en_id4 = clr_en_id4;
	reg.bits.clr_en_id5 = clr_en_id5;
	reg.bits.clr_en_id6 = clr_en_id6;
	reg.bits.clr_en_id7 = clr_en_id7;
	reg.bits.clr_en_id8 = clr_en_id8;
	reg.bits.clr_en_id9 = clr_en_id9;
	reg.bits.clr_en_id10 = clr_en_id10;
	reg.bits.clr_en_id11 = clr_en_id11;
	reg.bits.clr_en_id12 = clr_en_id12;
	reg.bits.clr_en_id13 = clr_en_id13;
	reg.bits.clr_en_id14 = clr_en_id14;
	reg.bits.clr_en_id15 = clr_en_id15;
	reg.bits.clr_en_id16 = clr_en_id16;
	reg.bits.clr_en_id17 = clr_en_id17;
	reg.bits.clr_en_id18 = clr_en_id18;
	reg.bits.clr_en_id19 = clr_en_id19;
	reg.bits.clr_en_id20 = clr_en_id20;
	reg.bits.clr_en_id21 = clr_en_id21;
	reg.bits.clr_en_id22 = clr_en_id22;
	reg.bits.clr_en_id23 = clr_en_id23;
	reg.bits.clr_en_id24 = clr_en_id24;
	reg.bits.clr_en_id25 = clr_en_id25;
	reg.bits.clr_en_id26 = clr_en_id26;
	reg.bits.clr_en_id27 = clr_en_id27;
	reg.bits.clr_en_id28 = clr_en_id28;
	reg.bits.clr_en_id29 = clr_en_id29;
	reg.bits.clr_en_id30 = clr_en_id30;
	reg.bits.clr_en_id31 = clr_en_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID0
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID1
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID2
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID3
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID4
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID5
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID6
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID7
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID8
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID9
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID10
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID11
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID12
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID13
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID14
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID15
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID16
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID17
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID18
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID19
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID20
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID21
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID22
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID23
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID24
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID25
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID26
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID27
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID28
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID29
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID30
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER0_CLR_EN_ID31
fun_set_mod_reg_bit(gicd, icenabler0, clr_en_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1
void set_gicd_icenabler1(u32 reg_addr,
			u32 clr_en_id32,
			u32 clr_en_id33,
			u32 clr_en_id34,
			u32 clr_en_id35,
			u32 clr_en_id36,
			u32 clr_en_id37,
			u32 clr_en_id38,
			u32 clr_en_id39,
			u32 clr_en_id40,
			u32 clr_en_id41,
			u32 clr_en_id42,
			u32 clr_en_id43,
			u32 clr_en_id44,
			u32 clr_en_id45,
			u32 clr_en_id46,
			u32 clr_en_id47,
			u32 clr_en_id48,
			u32 clr_en_id49,
			u32 clr_en_id50,
			u32 clr_en_id51,
			u32 clr_en_id52,
			u32 clr_en_id53,
			u32 clr_en_id54,
			u32 clr_en_id55,
			u32 clr_en_id56,
			u32 clr_en_id57,
			u32 clr_en_id58,
			u32 clr_en_id59,
			u32 clr_en_id60,
			u32 clr_en_id61,
			u32 clr_en_id62,
			u32 clr_en_id63,
			u32 m_or_r)
{
	reg_gicd_icenabler1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_en_id32 = clr_en_id32;
	reg.bits.clr_en_id33 = clr_en_id33;
	reg.bits.clr_en_id34 = clr_en_id34;
	reg.bits.clr_en_id35 = clr_en_id35;
	reg.bits.clr_en_id36 = clr_en_id36;
	reg.bits.clr_en_id37 = clr_en_id37;
	reg.bits.clr_en_id38 = clr_en_id38;
	reg.bits.clr_en_id39 = clr_en_id39;
	reg.bits.clr_en_id40 = clr_en_id40;
	reg.bits.clr_en_id41 = clr_en_id41;
	reg.bits.clr_en_id42 = clr_en_id42;
	reg.bits.clr_en_id43 = clr_en_id43;
	reg.bits.clr_en_id44 = clr_en_id44;
	reg.bits.clr_en_id45 = clr_en_id45;
	reg.bits.clr_en_id46 = clr_en_id46;
	reg.bits.clr_en_id47 = clr_en_id47;
	reg.bits.clr_en_id48 = clr_en_id48;
	reg.bits.clr_en_id49 = clr_en_id49;
	reg.bits.clr_en_id50 = clr_en_id50;
	reg.bits.clr_en_id51 = clr_en_id51;
	reg.bits.clr_en_id52 = clr_en_id52;
	reg.bits.clr_en_id53 = clr_en_id53;
	reg.bits.clr_en_id54 = clr_en_id54;
	reg.bits.clr_en_id55 = clr_en_id55;
	reg.bits.clr_en_id56 = clr_en_id56;
	reg.bits.clr_en_id57 = clr_en_id57;
	reg.bits.clr_en_id58 = clr_en_id58;
	reg.bits.clr_en_id59 = clr_en_id59;
	reg.bits.clr_en_id60 = clr_en_id60;
	reg.bits.clr_en_id61 = clr_en_id61;
	reg.bits.clr_en_id62 = clr_en_id62;
	reg.bits.clr_en_id63 = clr_en_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID32
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID33
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID34
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID35
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID36
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID37
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID38
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID39
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID40
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID41
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID42
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID43
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID44
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID45
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID46
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID47
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID48
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID49
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID50
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID51
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID52
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID53
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID54
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID55
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID56
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID57
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID58
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID59
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID60
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID61
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID62
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER1_CLR_EN_ID63
fun_set_mod_reg_bit(gicd, icenabler1, clr_en_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2
void set_gicd_icenabler2(u32 reg_addr,
			u32 clr_en_id64,
			u32 clr_en_id65,
			u32 clr_en_id66,
			u32 clr_en_id67,
			u32 clr_en_id68,
			u32 clr_en_id69,
			u32 clr_en_id70,
			u32 clr_en_id71,
			u32 clr_en_id72,
			u32 clr_en_id73,
			u32 clr_en_id74,
			u32 clr_en_id75,
			u32 clr_en_id76,
			u32 clr_en_id77,
			u32 clr_en_id78,
			u32 clr_en_id79,
			u32 clr_en_id80,
			u32 clr_en_id81,
			u32 clr_en_id82,
			u32 clr_en_id83,
			u32 clr_en_id84,
			u32 clr_en_id85,
			u32 clr_en_id86,
			u32 clr_en_id87,
			u32 clr_en_id88,
			u32 clr_en_id89,
			u32 clr_en_id90,
			u32 clr_en_id91,
			u32 clr_en_id92,
			u32 clr_en_id93,
			u32 clr_en_id94,
			u32 clr_en_id95,
			u32 m_or_r)
{
	reg_gicd_icenabler2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_en_id64 = clr_en_id64;
	reg.bits.clr_en_id65 = clr_en_id65;
	reg.bits.clr_en_id66 = clr_en_id66;
	reg.bits.clr_en_id67 = clr_en_id67;
	reg.bits.clr_en_id68 = clr_en_id68;
	reg.bits.clr_en_id69 = clr_en_id69;
	reg.bits.clr_en_id70 = clr_en_id70;
	reg.bits.clr_en_id71 = clr_en_id71;
	reg.bits.clr_en_id72 = clr_en_id72;
	reg.bits.clr_en_id73 = clr_en_id73;
	reg.bits.clr_en_id74 = clr_en_id74;
	reg.bits.clr_en_id75 = clr_en_id75;
	reg.bits.clr_en_id76 = clr_en_id76;
	reg.bits.clr_en_id77 = clr_en_id77;
	reg.bits.clr_en_id78 = clr_en_id78;
	reg.bits.clr_en_id79 = clr_en_id79;
	reg.bits.clr_en_id80 = clr_en_id80;
	reg.bits.clr_en_id81 = clr_en_id81;
	reg.bits.clr_en_id82 = clr_en_id82;
	reg.bits.clr_en_id83 = clr_en_id83;
	reg.bits.clr_en_id84 = clr_en_id84;
	reg.bits.clr_en_id85 = clr_en_id85;
	reg.bits.clr_en_id86 = clr_en_id86;
	reg.bits.clr_en_id87 = clr_en_id87;
	reg.bits.clr_en_id88 = clr_en_id88;
	reg.bits.clr_en_id89 = clr_en_id89;
	reg.bits.clr_en_id90 = clr_en_id90;
	reg.bits.clr_en_id91 = clr_en_id91;
	reg.bits.clr_en_id92 = clr_en_id92;
	reg.bits.clr_en_id93 = clr_en_id93;
	reg.bits.clr_en_id94 = clr_en_id94;
	reg.bits.clr_en_id95 = clr_en_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID64
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID65
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID66
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID67
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID68
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID69
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID70
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID71
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID72
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID73
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID74
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID75
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID76
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID77
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID78
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID79
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID80
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID81
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID82
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID83
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID84
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID85
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID86
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID87
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID88
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID89
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID90
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID91
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID92
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID93
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID94
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER2_CLR_EN_ID95
fun_set_mod_reg_bit(gicd, icenabler2, clr_en_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3
void set_gicd_icenabler3(u32 reg_addr,
			u32 clr_en_id96,
			u32 clr_en_id97,
			u32 clr_en_id98,
			u32 clr_en_id99,
			u32 clr_en_id100,
			u32 clr_en_id101,
			u32 clr_en_id102,
			u32 clr_en_id103,
			u32 clr_en_id104,
			u32 clr_en_id105,
			u32 clr_en_id106,
			u32 clr_en_id107,
			u32 clr_en_id108,
			u32 clr_en_id109,
			u32 clr_en_id110,
			u32 clr_en_id111,
			u32 clr_en_id112,
			u32 clr_en_id113,
			u32 clr_en_id114,
			u32 clr_en_id115,
			u32 clr_en_id116,
			u32 clr_en_id117,
			u32 clr_en_id118,
			u32 clr_en_id119,
			u32 clr_en_id120,
			u32 clr_en_id121,
			u32 clr_en_id122,
			u32 clr_en_id123,
			u32 clr_en_id124,
			u32 clr_en_id125,
			u32 clr_en_id126,
			u32 clr_en_id127,
			u32 m_or_r)
{
	reg_gicd_icenabler3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_en_id96 = clr_en_id96;
	reg.bits.clr_en_id97 = clr_en_id97;
	reg.bits.clr_en_id98 = clr_en_id98;
	reg.bits.clr_en_id99 = clr_en_id99;
	reg.bits.clr_en_id100 = clr_en_id100;
	reg.bits.clr_en_id101 = clr_en_id101;
	reg.bits.clr_en_id102 = clr_en_id102;
	reg.bits.clr_en_id103 = clr_en_id103;
	reg.bits.clr_en_id104 = clr_en_id104;
	reg.bits.clr_en_id105 = clr_en_id105;
	reg.bits.clr_en_id106 = clr_en_id106;
	reg.bits.clr_en_id107 = clr_en_id107;
	reg.bits.clr_en_id108 = clr_en_id108;
	reg.bits.clr_en_id109 = clr_en_id109;
	reg.bits.clr_en_id110 = clr_en_id110;
	reg.bits.clr_en_id111 = clr_en_id111;
	reg.bits.clr_en_id112 = clr_en_id112;
	reg.bits.clr_en_id113 = clr_en_id113;
	reg.bits.clr_en_id114 = clr_en_id114;
	reg.bits.clr_en_id115 = clr_en_id115;
	reg.bits.clr_en_id116 = clr_en_id116;
	reg.bits.clr_en_id117 = clr_en_id117;
	reg.bits.clr_en_id118 = clr_en_id118;
	reg.bits.clr_en_id119 = clr_en_id119;
	reg.bits.clr_en_id120 = clr_en_id120;
	reg.bits.clr_en_id121 = clr_en_id121;
	reg.bits.clr_en_id122 = clr_en_id122;
	reg.bits.clr_en_id123 = clr_en_id123;
	reg.bits.clr_en_id124 = clr_en_id124;
	reg.bits.clr_en_id125 = clr_en_id125;
	reg.bits.clr_en_id126 = clr_en_id126;
	reg.bits.clr_en_id127 = clr_en_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID96
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID97
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID98
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID99
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID100
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID101
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID102
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID103
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID104
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID105
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID106
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID107
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID108
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID109
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID110
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID111
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID112
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID113
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID114
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID115
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID116
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID117
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID118
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID119
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID120
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID121
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID122
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID123
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID124
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID125
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID126
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER3_CLR_EN_ID127
fun_set_mod_reg_bit(gicd, icenabler3, clr_en_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4
void set_gicd_icenabler4(u32 reg_addr,
			u32 clr_en_id128,
			u32 clr_en_id129,
			u32 clr_en_id130,
			u32 clr_en_id131,
			u32 clr_en_id132,
			u32 clr_en_id133,
			u32 clr_en_id134,
			u32 clr_en_id135,
			u32 clr_en_id136,
			u32 clr_en_id137,
			u32 clr_en_id138,
			u32 clr_en_id139,
			u32 clr_en_id140,
			u32 clr_en_id141,
			u32 clr_en_id142,
			u32 clr_en_id143,
			u32 clr_en_id144,
			u32 clr_en_id145,
			u32 clr_en_id146,
			u32 clr_en_id147,
			u32 clr_en_id148,
			u32 clr_en_id149,
			u32 clr_en_id150,
			u32 clr_en_id151,
			u32 clr_en_id152,
			u32 clr_en_id153,
			u32 clr_en_id154,
			u32 clr_en_id155,
			u32 clr_en_id156,
			u32 clr_en_id157,
			u32 clr_en_id158,
			u32 clr_en_id159,
			u32 m_or_r)
{
	reg_gicd_icenabler4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_en_id128 = clr_en_id128;
	reg.bits.clr_en_id129 = clr_en_id129;
	reg.bits.clr_en_id130 = clr_en_id130;
	reg.bits.clr_en_id131 = clr_en_id131;
	reg.bits.clr_en_id132 = clr_en_id132;
	reg.bits.clr_en_id133 = clr_en_id133;
	reg.bits.clr_en_id134 = clr_en_id134;
	reg.bits.clr_en_id135 = clr_en_id135;
	reg.bits.clr_en_id136 = clr_en_id136;
	reg.bits.clr_en_id137 = clr_en_id137;
	reg.bits.clr_en_id138 = clr_en_id138;
	reg.bits.clr_en_id139 = clr_en_id139;
	reg.bits.clr_en_id140 = clr_en_id140;
	reg.bits.clr_en_id141 = clr_en_id141;
	reg.bits.clr_en_id142 = clr_en_id142;
	reg.bits.clr_en_id143 = clr_en_id143;
	reg.bits.clr_en_id144 = clr_en_id144;
	reg.bits.clr_en_id145 = clr_en_id145;
	reg.bits.clr_en_id146 = clr_en_id146;
	reg.bits.clr_en_id147 = clr_en_id147;
	reg.bits.clr_en_id148 = clr_en_id148;
	reg.bits.clr_en_id149 = clr_en_id149;
	reg.bits.clr_en_id150 = clr_en_id150;
	reg.bits.clr_en_id151 = clr_en_id151;
	reg.bits.clr_en_id152 = clr_en_id152;
	reg.bits.clr_en_id153 = clr_en_id153;
	reg.bits.clr_en_id154 = clr_en_id154;
	reg.bits.clr_en_id155 = clr_en_id155;
	reg.bits.clr_en_id156 = clr_en_id156;
	reg.bits.clr_en_id157 = clr_en_id157;
	reg.bits.clr_en_id158 = clr_en_id158;
	reg.bits.clr_en_id159 = clr_en_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID128
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID129
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID130
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID131
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID132
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID133
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID134
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID135
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID136
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID137
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID138
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID139
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID140
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID141
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID142
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID143
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID144
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID145
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID146
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID147
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID148
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID149
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID150
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID151
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID152
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID153
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID154
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID155
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID156
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID157
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID158
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER4_CLR_EN_ID159
fun_set_mod_reg_bit(gicd, icenabler4, clr_en_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5
void set_gicd_icenabler5(u32 reg_addr,
			u32 clr_en_id160,
			u32 clr_en_id161,
			u32 clr_en_id162,
			u32 clr_en_id163,
			u32 clr_en_id164,
			u32 clr_en_id165,
			u32 clr_en_id166,
			u32 clr_en_id167,
			u32 clr_en_id168,
			u32 clr_en_id169,
			u32 clr_en_id170,
			u32 clr_en_id171,
			u32 clr_en_id172,
			u32 clr_en_id173,
			u32 clr_en_id174,
			u32 clr_en_id175,
			u32 clr_en_id176,
			u32 clr_en_id177,
			u32 clr_en_id178,
			u32 clr_en_id179,
			u32 clr_en_id180,
			u32 clr_en_id181,
			u32 clr_en_id182,
			u32 clr_en_id183,
			u32 clr_en_id184,
			u32 clr_en_id185,
			u32 clr_en_id186,
			u32 clr_en_id187,
			u32 clr_en_id188,
			u32 clr_en_id189,
			u32 clr_en_id190,
			u32 clr_en_id191,
			u32 m_or_r)
{
	reg_gicd_icenabler5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_en_id160 = clr_en_id160;
	reg.bits.clr_en_id161 = clr_en_id161;
	reg.bits.clr_en_id162 = clr_en_id162;
	reg.bits.clr_en_id163 = clr_en_id163;
	reg.bits.clr_en_id164 = clr_en_id164;
	reg.bits.clr_en_id165 = clr_en_id165;
	reg.bits.clr_en_id166 = clr_en_id166;
	reg.bits.clr_en_id167 = clr_en_id167;
	reg.bits.clr_en_id168 = clr_en_id168;
	reg.bits.clr_en_id169 = clr_en_id169;
	reg.bits.clr_en_id170 = clr_en_id170;
	reg.bits.clr_en_id171 = clr_en_id171;
	reg.bits.clr_en_id172 = clr_en_id172;
	reg.bits.clr_en_id173 = clr_en_id173;
	reg.bits.clr_en_id174 = clr_en_id174;
	reg.bits.clr_en_id175 = clr_en_id175;
	reg.bits.clr_en_id176 = clr_en_id176;
	reg.bits.clr_en_id177 = clr_en_id177;
	reg.bits.clr_en_id178 = clr_en_id178;
	reg.bits.clr_en_id179 = clr_en_id179;
	reg.bits.clr_en_id180 = clr_en_id180;
	reg.bits.clr_en_id181 = clr_en_id181;
	reg.bits.clr_en_id182 = clr_en_id182;
	reg.bits.clr_en_id183 = clr_en_id183;
	reg.bits.clr_en_id184 = clr_en_id184;
	reg.bits.clr_en_id185 = clr_en_id185;
	reg.bits.clr_en_id186 = clr_en_id186;
	reg.bits.clr_en_id187 = clr_en_id187;
	reg.bits.clr_en_id188 = clr_en_id188;
	reg.bits.clr_en_id189 = clr_en_id189;
	reg.bits.clr_en_id190 = clr_en_id190;
	reg.bits.clr_en_id191 = clr_en_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID160
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID161
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID162
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID163
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID164
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID165
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID166
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID167
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID168
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID169
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID170
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID171
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID172
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID173
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID174
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID175
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID176
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID177
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID178
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID179
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID180
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID181
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID182
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID183
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID184
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID185
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID186
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID187
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID188
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID189
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID190
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER5_CLR_EN_ID191
fun_set_mod_reg_bit(gicd, icenabler5, clr_en_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6
void set_gicd_icenabler6(u32 reg_addr,
			u32 clr_en_id192,
			u32 clr_en_id193,
			u32 clr_en_id194,
			u32 clr_en_id195,
			u32 clr_en_id196,
			u32 clr_en_id197,
			u32 clr_en_id198,
			u32 clr_en_id199,
			u32 clr_en_id200,
			u32 clr_en_id201,
			u32 clr_en_id202,
			u32 clr_en_id203,
			u32 clr_en_id204,
			u32 clr_en_id205,
			u32 clr_en_id206,
			u32 clr_en_id207,
			u32 clr_en_id208,
			u32 clr_en_id209,
			u32 clr_en_id210,
			u32 clr_en_id211,
			u32 clr_en_id212,
			u32 clr_en_id213,
			u32 clr_en_id214,
			u32 clr_en_id215,
			u32 clr_en_id216,
			u32 clr_en_id217,
			u32 clr_en_id218,
			u32 clr_en_id219,
			u32 clr_en_id220,
			u32 clr_en_id221,
			u32 clr_en_id222,
			u32 clr_en_id223,
			u32 m_or_r)
{
	reg_gicd_icenabler6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_en_id192 = clr_en_id192;
	reg.bits.clr_en_id193 = clr_en_id193;
	reg.bits.clr_en_id194 = clr_en_id194;
	reg.bits.clr_en_id195 = clr_en_id195;
	reg.bits.clr_en_id196 = clr_en_id196;
	reg.bits.clr_en_id197 = clr_en_id197;
	reg.bits.clr_en_id198 = clr_en_id198;
	reg.bits.clr_en_id199 = clr_en_id199;
	reg.bits.clr_en_id200 = clr_en_id200;
	reg.bits.clr_en_id201 = clr_en_id201;
	reg.bits.clr_en_id202 = clr_en_id202;
	reg.bits.clr_en_id203 = clr_en_id203;
	reg.bits.clr_en_id204 = clr_en_id204;
	reg.bits.clr_en_id205 = clr_en_id205;
	reg.bits.clr_en_id206 = clr_en_id206;
	reg.bits.clr_en_id207 = clr_en_id207;
	reg.bits.clr_en_id208 = clr_en_id208;
	reg.bits.clr_en_id209 = clr_en_id209;
	reg.bits.clr_en_id210 = clr_en_id210;
	reg.bits.clr_en_id211 = clr_en_id211;
	reg.bits.clr_en_id212 = clr_en_id212;
	reg.bits.clr_en_id213 = clr_en_id213;
	reg.bits.clr_en_id214 = clr_en_id214;
	reg.bits.clr_en_id215 = clr_en_id215;
	reg.bits.clr_en_id216 = clr_en_id216;
	reg.bits.clr_en_id217 = clr_en_id217;
	reg.bits.clr_en_id218 = clr_en_id218;
	reg.bits.clr_en_id219 = clr_en_id219;
	reg.bits.clr_en_id220 = clr_en_id220;
	reg.bits.clr_en_id221 = clr_en_id221;
	reg.bits.clr_en_id222 = clr_en_id222;
	reg.bits.clr_en_id223 = clr_en_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID192
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID193
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID194
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID195
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID196
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID197
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID198
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID199
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID200
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID201
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID202
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID203
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID204
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID205
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID206
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID207
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID208
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID209
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID210
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID211
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID212
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID213
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID214
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID215
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID216
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID217
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID218
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID219
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID220
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID221
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID222
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICENABLER6_CLR_EN_ID223
fun_set_mod_reg_bit(gicd, icenabler6, clr_en_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0
void set_gicd_ispendr0(u32 reg_addr,
			u32 set_pend_id0,
			u32 set_pend_id1,
			u32 set_pend_id2,
			u32 set_pend_id3,
			u32 set_pend_id4,
			u32 set_pend_id5,
			u32 set_pend_id6,
			u32 set_pend_id7,
			u32 set_pend_id8,
			u32 set_pend_id9,
			u32 set_pend_id10,
			u32 set_pend_id11,
			u32 set_pend_id12,
			u32 set_pend_id13,
			u32 set_pend_id14,
			u32 set_pend_id15,
			u32 set_pend_id16,
			u32 set_pend_id17,
			u32 set_pend_id18,
			u32 set_pend_id19,
			u32 set_pend_id20,
			u32 set_pend_id21,
			u32 set_pend_id22,
			u32 set_pend_id23,
			u32 set_pend_id24,
			u32 set_pend_id25,
			u32 set_pend_id26,
			u32 set_pend_id27,
			u32 set_pend_id28,
			u32 set_pend_id29,
			u32 set_pend_id30,
			u32 set_pend_id31,
			u32 m_or_r)
{
	reg_gicd_ispendr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id0 = set_pend_id0;
	reg.bits.set_pend_id1 = set_pend_id1;
	reg.bits.set_pend_id2 = set_pend_id2;
	reg.bits.set_pend_id3 = set_pend_id3;
	reg.bits.set_pend_id4 = set_pend_id4;
	reg.bits.set_pend_id5 = set_pend_id5;
	reg.bits.set_pend_id6 = set_pend_id6;
	reg.bits.set_pend_id7 = set_pend_id7;
	reg.bits.set_pend_id8 = set_pend_id8;
	reg.bits.set_pend_id9 = set_pend_id9;
	reg.bits.set_pend_id10 = set_pend_id10;
	reg.bits.set_pend_id11 = set_pend_id11;
	reg.bits.set_pend_id12 = set_pend_id12;
	reg.bits.set_pend_id13 = set_pend_id13;
	reg.bits.set_pend_id14 = set_pend_id14;
	reg.bits.set_pend_id15 = set_pend_id15;
	reg.bits.set_pend_id16 = set_pend_id16;
	reg.bits.set_pend_id17 = set_pend_id17;
	reg.bits.set_pend_id18 = set_pend_id18;
	reg.bits.set_pend_id19 = set_pend_id19;
	reg.bits.set_pend_id20 = set_pend_id20;
	reg.bits.set_pend_id21 = set_pend_id21;
	reg.bits.set_pend_id22 = set_pend_id22;
	reg.bits.set_pend_id23 = set_pend_id23;
	reg.bits.set_pend_id24 = set_pend_id24;
	reg.bits.set_pend_id25 = set_pend_id25;
	reg.bits.set_pend_id26 = set_pend_id26;
	reg.bits.set_pend_id27 = set_pend_id27;
	reg.bits.set_pend_id28 = set_pend_id28;
	reg.bits.set_pend_id29 = set_pend_id29;
	reg.bits.set_pend_id30 = set_pend_id30;
	reg.bits.set_pend_id31 = set_pend_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID0
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID1
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID2
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID3
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID4
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID5
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID6
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID7
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID8
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID9
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID10
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID11
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID12
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID13
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID14
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID15
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID16
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID17
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID18
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID19
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID20
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID21
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID22
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID23
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID24
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID25
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID26
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID27
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID28
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID29
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID30
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR0_SET_PEND_ID31
fun_set_mod_reg_bit(gicd, ispendr0, set_pend_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1
void set_gicd_ispendr1(u32 reg_addr,
			u32 set_pend_id32,
			u32 set_pend_id33,
			u32 set_pend_id34,
			u32 set_pend_id35,
			u32 set_pend_id36,
			u32 set_pend_id37,
			u32 set_pend_id38,
			u32 set_pend_id39,
			u32 set_pend_id40,
			u32 set_pend_id41,
			u32 set_pend_id42,
			u32 set_pend_id43,
			u32 set_pend_id44,
			u32 set_pend_id45,
			u32 set_pend_id46,
			u32 set_pend_id47,
			u32 set_pend_id48,
			u32 set_pend_id49,
			u32 set_pend_id50,
			u32 set_pend_id51,
			u32 set_pend_id52,
			u32 set_pend_id53,
			u32 set_pend_id54,
			u32 set_pend_id55,
			u32 set_pend_id56,
			u32 set_pend_id57,
			u32 set_pend_id58,
			u32 set_pend_id59,
			u32 set_pend_id60,
			u32 set_pend_id61,
			u32 set_pend_id62,
			u32 set_pend_id63,
			u32 m_or_r)
{
	reg_gicd_ispendr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id32 = set_pend_id32;
	reg.bits.set_pend_id33 = set_pend_id33;
	reg.bits.set_pend_id34 = set_pend_id34;
	reg.bits.set_pend_id35 = set_pend_id35;
	reg.bits.set_pend_id36 = set_pend_id36;
	reg.bits.set_pend_id37 = set_pend_id37;
	reg.bits.set_pend_id38 = set_pend_id38;
	reg.bits.set_pend_id39 = set_pend_id39;
	reg.bits.set_pend_id40 = set_pend_id40;
	reg.bits.set_pend_id41 = set_pend_id41;
	reg.bits.set_pend_id42 = set_pend_id42;
	reg.bits.set_pend_id43 = set_pend_id43;
	reg.bits.set_pend_id44 = set_pend_id44;
	reg.bits.set_pend_id45 = set_pend_id45;
	reg.bits.set_pend_id46 = set_pend_id46;
	reg.bits.set_pend_id47 = set_pend_id47;
	reg.bits.set_pend_id48 = set_pend_id48;
	reg.bits.set_pend_id49 = set_pend_id49;
	reg.bits.set_pend_id50 = set_pend_id50;
	reg.bits.set_pend_id51 = set_pend_id51;
	reg.bits.set_pend_id52 = set_pend_id52;
	reg.bits.set_pend_id53 = set_pend_id53;
	reg.bits.set_pend_id54 = set_pend_id54;
	reg.bits.set_pend_id55 = set_pend_id55;
	reg.bits.set_pend_id56 = set_pend_id56;
	reg.bits.set_pend_id57 = set_pend_id57;
	reg.bits.set_pend_id58 = set_pend_id58;
	reg.bits.set_pend_id59 = set_pend_id59;
	reg.bits.set_pend_id60 = set_pend_id60;
	reg.bits.set_pend_id61 = set_pend_id61;
	reg.bits.set_pend_id62 = set_pend_id62;
	reg.bits.set_pend_id63 = set_pend_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID32
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID33
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID34
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID35
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID36
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID37
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID38
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID39
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID40
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID41
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID42
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID43
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID44
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID45
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID46
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID47
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID48
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID49
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID50
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID51
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID52
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID53
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID54
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID55
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID56
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID57
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID58
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID59
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID60
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID61
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID62
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR1_SET_PEND_ID63
fun_set_mod_reg_bit(gicd, ispendr1, set_pend_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2
void set_gicd_ispendr2(u32 reg_addr,
			u32 set_pend_id64,
			u32 set_pend_id65,
			u32 set_pend_id66,
			u32 set_pend_id67,
			u32 set_pend_id68,
			u32 set_pend_id69,
			u32 set_pend_id70,
			u32 set_pend_id71,
			u32 set_pend_id72,
			u32 set_pend_id73,
			u32 set_pend_id74,
			u32 set_pend_id75,
			u32 set_pend_id76,
			u32 set_pend_id77,
			u32 set_pend_id78,
			u32 set_pend_id79,
			u32 set_pend_id80,
			u32 set_pend_id81,
			u32 set_pend_id82,
			u32 set_pend_id83,
			u32 set_pend_id84,
			u32 set_pend_id85,
			u32 set_pend_id86,
			u32 set_pend_id87,
			u32 set_pend_id88,
			u32 set_pend_id89,
			u32 set_pend_id90,
			u32 set_pend_id91,
			u32 set_pend_id92,
			u32 set_pend_id93,
			u32 set_pend_id94,
			u32 set_pend_id95,
			u32 m_or_r)
{
	reg_gicd_ispendr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id64 = set_pend_id64;
	reg.bits.set_pend_id65 = set_pend_id65;
	reg.bits.set_pend_id66 = set_pend_id66;
	reg.bits.set_pend_id67 = set_pend_id67;
	reg.bits.set_pend_id68 = set_pend_id68;
	reg.bits.set_pend_id69 = set_pend_id69;
	reg.bits.set_pend_id70 = set_pend_id70;
	reg.bits.set_pend_id71 = set_pend_id71;
	reg.bits.set_pend_id72 = set_pend_id72;
	reg.bits.set_pend_id73 = set_pend_id73;
	reg.bits.set_pend_id74 = set_pend_id74;
	reg.bits.set_pend_id75 = set_pend_id75;
	reg.bits.set_pend_id76 = set_pend_id76;
	reg.bits.set_pend_id77 = set_pend_id77;
	reg.bits.set_pend_id78 = set_pend_id78;
	reg.bits.set_pend_id79 = set_pend_id79;
	reg.bits.set_pend_id80 = set_pend_id80;
	reg.bits.set_pend_id81 = set_pend_id81;
	reg.bits.set_pend_id82 = set_pend_id82;
	reg.bits.set_pend_id83 = set_pend_id83;
	reg.bits.set_pend_id84 = set_pend_id84;
	reg.bits.set_pend_id85 = set_pend_id85;
	reg.bits.set_pend_id86 = set_pend_id86;
	reg.bits.set_pend_id87 = set_pend_id87;
	reg.bits.set_pend_id88 = set_pend_id88;
	reg.bits.set_pend_id89 = set_pend_id89;
	reg.bits.set_pend_id90 = set_pend_id90;
	reg.bits.set_pend_id91 = set_pend_id91;
	reg.bits.set_pend_id92 = set_pend_id92;
	reg.bits.set_pend_id93 = set_pend_id93;
	reg.bits.set_pend_id94 = set_pend_id94;
	reg.bits.set_pend_id95 = set_pend_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID64
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID65
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID66
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID67
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID68
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID69
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID70
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID71
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID72
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID73
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID74
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID75
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID76
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID77
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID78
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID79
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID80
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID81
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID82
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID83
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID84
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID85
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID86
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID87
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID88
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID89
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID90
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID91
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID92
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID93
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID94
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR2_SET_PEND_ID95
fun_set_mod_reg_bit(gicd, ispendr2, set_pend_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3
void set_gicd_ispendr3(u32 reg_addr,
			u32 set_pend_id96,
			u32 set_pend_id97,
			u32 set_pend_id98,
			u32 set_pend_id99,
			u32 set_pend_id100,
			u32 set_pend_id101,
			u32 set_pend_id102,
			u32 set_pend_id103,
			u32 set_pend_id104,
			u32 set_pend_id105,
			u32 set_pend_id106,
			u32 set_pend_id107,
			u32 set_pend_id108,
			u32 set_pend_id109,
			u32 set_pend_id110,
			u32 set_pend_id111,
			u32 set_pend_id112,
			u32 set_pend_id113,
			u32 set_pend_id114,
			u32 set_pend_id115,
			u32 set_pend_id116,
			u32 set_pend_id117,
			u32 set_pend_id118,
			u32 set_pend_id119,
			u32 set_pend_id120,
			u32 set_pend_id121,
			u32 set_pend_id122,
			u32 set_pend_id123,
			u32 set_pend_id124,
			u32 set_pend_id125,
			u32 set_pend_id126,
			u32 set_pend_id127,
			u32 m_or_r)
{
	reg_gicd_ispendr3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id96 = set_pend_id96;
	reg.bits.set_pend_id97 = set_pend_id97;
	reg.bits.set_pend_id98 = set_pend_id98;
	reg.bits.set_pend_id99 = set_pend_id99;
	reg.bits.set_pend_id100 = set_pend_id100;
	reg.bits.set_pend_id101 = set_pend_id101;
	reg.bits.set_pend_id102 = set_pend_id102;
	reg.bits.set_pend_id103 = set_pend_id103;
	reg.bits.set_pend_id104 = set_pend_id104;
	reg.bits.set_pend_id105 = set_pend_id105;
	reg.bits.set_pend_id106 = set_pend_id106;
	reg.bits.set_pend_id107 = set_pend_id107;
	reg.bits.set_pend_id108 = set_pend_id108;
	reg.bits.set_pend_id109 = set_pend_id109;
	reg.bits.set_pend_id110 = set_pend_id110;
	reg.bits.set_pend_id111 = set_pend_id111;
	reg.bits.set_pend_id112 = set_pend_id112;
	reg.bits.set_pend_id113 = set_pend_id113;
	reg.bits.set_pend_id114 = set_pend_id114;
	reg.bits.set_pend_id115 = set_pend_id115;
	reg.bits.set_pend_id116 = set_pend_id116;
	reg.bits.set_pend_id117 = set_pend_id117;
	reg.bits.set_pend_id118 = set_pend_id118;
	reg.bits.set_pend_id119 = set_pend_id119;
	reg.bits.set_pend_id120 = set_pend_id120;
	reg.bits.set_pend_id121 = set_pend_id121;
	reg.bits.set_pend_id122 = set_pend_id122;
	reg.bits.set_pend_id123 = set_pend_id123;
	reg.bits.set_pend_id124 = set_pend_id124;
	reg.bits.set_pend_id125 = set_pend_id125;
	reg.bits.set_pend_id126 = set_pend_id126;
	reg.bits.set_pend_id127 = set_pend_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID96
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID97
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID98
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID99
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID100
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID101
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID102
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID103
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID104
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID105
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID106
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID107
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID108
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID109
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID110
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID111
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID112
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID113
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID114
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID115
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID116
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID117
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID118
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID119
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID120
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID121
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID122
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID123
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID124
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID125
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID126
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR3_SET_PEND_ID127
fun_set_mod_reg_bit(gicd, ispendr3, set_pend_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4
void set_gicd_ispendr4(u32 reg_addr,
			u32 set_pend_id128,
			u32 set_pend_id129,
			u32 set_pend_id130,
			u32 set_pend_id131,
			u32 set_pend_id132,
			u32 set_pend_id133,
			u32 set_pend_id134,
			u32 set_pend_id135,
			u32 set_pend_id136,
			u32 set_pend_id137,
			u32 set_pend_id138,
			u32 set_pend_id139,
			u32 set_pend_id140,
			u32 set_pend_id141,
			u32 set_pend_id142,
			u32 set_pend_id143,
			u32 set_pend_id144,
			u32 set_pend_id145,
			u32 set_pend_id146,
			u32 set_pend_id147,
			u32 set_pend_id148,
			u32 set_pend_id149,
			u32 set_pend_id150,
			u32 set_pend_id151,
			u32 set_pend_id152,
			u32 set_pend_id153,
			u32 set_pend_id154,
			u32 set_pend_id155,
			u32 set_pend_id156,
			u32 set_pend_id157,
			u32 set_pend_id158,
			u32 set_pend_id159,
			u32 m_or_r)
{
	reg_gicd_ispendr4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id128 = set_pend_id128;
	reg.bits.set_pend_id129 = set_pend_id129;
	reg.bits.set_pend_id130 = set_pend_id130;
	reg.bits.set_pend_id131 = set_pend_id131;
	reg.bits.set_pend_id132 = set_pend_id132;
	reg.bits.set_pend_id133 = set_pend_id133;
	reg.bits.set_pend_id134 = set_pend_id134;
	reg.bits.set_pend_id135 = set_pend_id135;
	reg.bits.set_pend_id136 = set_pend_id136;
	reg.bits.set_pend_id137 = set_pend_id137;
	reg.bits.set_pend_id138 = set_pend_id138;
	reg.bits.set_pend_id139 = set_pend_id139;
	reg.bits.set_pend_id140 = set_pend_id140;
	reg.bits.set_pend_id141 = set_pend_id141;
	reg.bits.set_pend_id142 = set_pend_id142;
	reg.bits.set_pend_id143 = set_pend_id143;
	reg.bits.set_pend_id144 = set_pend_id144;
	reg.bits.set_pend_id145 = set_pend_id145;
	reg.bits.set_pend_id146 = set_pend_id146;
	reg.bits.set_pend_id147 = set_pend_id147;
	reg.bits.set_pend_id148 = set_pend_id148;
	reg.bits.set_pend_id149 = set_pend_id149;
	reg.bits.set_pend_id150 = set_pend_id150;
	reg.bits.set_pend_id151 = set_pend_id151;
	reg.bits.set_pend_id152 = set_pend_id152;
	reg.bits.set_pend_id153 = set_pend_id153;
	reg.bits.set_pend_id154 = set_pend_id154;
	reg.bits.set_pend_id155 = set_pend_id155;
	reg.bits.set_pend_id156 = set_pend_id156;
	reg.bits.set_pend_id157 = set_pend_id157;
	reg.bits.set_pend_id158 = set_pend_id158;
	reg.bits.set_pend_id159 = set_pend_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID128
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID129
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID130
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID131
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID132
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID133
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID134
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID135
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID136
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID137
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID138
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID139
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID140
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID141
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID142
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID143
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID144
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID145
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID146
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID147
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID148
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID149
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID150
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID151
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID152
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID153
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID154
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID155
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID156
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID157
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID158
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR4_SET_PEND_ID159
fun_set_mod_reg_bit(gicd, ispendr4, set_pend_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5
void set_gicd_ispendr5(u32 reg_addr,
			u32 set_pend_id160,
			u32 set_pend_id161,
			u32 set_pend_id162,
			u32 set_pend_id163,
			u32 set_pend_id164,
			u32 set_pend_id165,
			u32 set_pend_id166,
			u32 set_pend_id167,
			u32 set_pend_id168,
			u32 set_pend_id169,
			u32 set_pend_id170,
			u32 set_pend_id171,
			u32 set_pend_id172,
			u32 set_pend_id173,
			u32 set_pend_id174,
			u32 set_pend_id175,
			u32 set_pend_id176,
			u32 set_pend_id177,
			u32 set_pend_id178,
			u32 set_pend_id179,
			u32 set_pend_id180,
			u32 set_pend_id181,
			u32 set_pend_id182,
			u32 set_pend_id183,
			u32 set_pend_id184,
			u32 set_pend_id185,
			u32 set_pend_id186,
			u32 set_pend_id187,
			u32 set_pend_id188,
			u32 set_pend_id189,
			u32 set_pend_id190,
			u32 set_pend_id191,
			u32 m_or_r)
{
	reg_gicd_ispendr5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id160 = set_pend_id160;
	reg.bits.set_pend_id161 = set_pend_id161;
	reg.bits.set_pend_id162 = set_pend_id162;
	reg.bits.set_pend_id163 = set_pend_id163;
	reg.bits.set_pend_id164 = set_pend_id164;
	reg.bits.set_pend_id165 = set_pend_id165;
	reg.bits.set_pend_id166 = set_pend_id166;
	reg.bits.set_pend_id167 = set_pend_id167;
	reg.bits.set_pend_id168 = set_pend_id168;
	reg.bits.set_pend_id169 = set_pend_id169;
	reg.bits.set_pend_id170 = set_pend_id170;
	reg.bits.set_pend_id171 = set_pend_id171;
	reg.bits.set_pend_id172 = set_pend_id172;
	reg.bits.set_pend_id173 = set_pend_id173;
	reg.bits.set_pend_id174 = set_pend_id174;
	reg.bits.set_pend_id175 = set_pend_id175;
	reg.bits.set_pend_id176 = set_pend_id176;
	reg.bits.set_pend_id177 = set_pend_id177;
	reg.bits.set_pend_id178 = set_pend_id178;
	reg.bits.set_pend_id179 = set_pend_id179;
	reg.bits.set_pend_id180 = set_pend_id180;
	reg.bits.set_pend_id181 = set_pend_id181;
	reg.bits.set_pend_id182 = set_pend_id182;
	reg.bits.set_pend_id183 = set_pend_id183;
	reg.bits.set_pend_id184 = set_pend_id184;
	reg.bits.set_pend_id185 = set_pend_id185;
	reg.bits.set_pend_id186 = set_pend_id186;
	reg.bits.set_pend_id187 = set_pend_id187;
	reg.bits.set_pend_id188 = set_pend_id188;
	reg.bits.set_pend_id189 = set_pend_id189;
	reg.bits.set_pend_id190 = set_pend_id190;
	reg.bits.set_pend_id191 = set_pend_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID160
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID161
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID162
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID163
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID164
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID165
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID166
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID167
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID168
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID169
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID170
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID171
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID172
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID173
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID174
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID175
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID176
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID177
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID178
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID179
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID180
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID181
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID182
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID183
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID184
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID185
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID186
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID187
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID188
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID189
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID190
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR5_SET_PEND_ID191
fun_set_mod_reg_bit(gicd, ispendr5, set_pend_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6
void set_gicd_ispendr6(u32 reg_addr,
			u32 set_pend_id192,
			u32 set_pend_id193,
			u32 set_pend_id194,
			u32 set_pend_id195,
			u32 set_pend_id196,
			u32 set_pend_id197,
			u32 set_pend_id198,
			u32 set_pend_id199,
			u32 set_pend_id200,
			u32 set_pend_id201,
			u32 set_pend_id202,
			u32 set_pend_id203,
			u32 set_pend_id204,
			u32 set_pend_id205,
			u32 set_pend_id206,
			u32 set_pend_id207,
			u32 set_pend_id208,
			u32 set_pend_id209,
			u32 set_pend_id210,
			u32 set_pend_id211,
			u32 set_pend_id212,
			u32 set_pend_id213,
			u32 set_pend_id214,
			u32 set_pend_id215,
			u32 set_pend_id216,
			u32 set_pend_id217,
			u32 set_pend_id218,
			u32 set_pend_id219,
			u32 set_pend_id220,
			u32 set_pend_id221,
			u32 set_pend_id222,
			u32 set_pend_id223,
			u32 m_or_r)
{
	reg_gicd_ispendr6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id192 = set_pend_id192;
	reg.bits.set_pend_id193 = set_pend_id193;
	reg.bits.set_pend_id194 = set_pend_id194;
	reg.bits.set_pend_id195 = set_pend_id195;
	reg.bits.set_pend_id196 = set_pend_id196;
	reg.bits.set_pend_id197 = set_pend_id197;
	reg.bits.set_pend_id198 = set_pend_id198;
	reg.bits.set_pend_id199 = set_pend_id199;
	reg.bits.set_pend_id200 = set_pend_id200;
	reg.bits.set_pend_id201 = set_pend_id201;
	reg.bits.set_pend_id202 = set_pend_id202;
	reg.bits.set_pend_id203 = set_pend_id203;
	reg.bits.set_pend_id204 = set_pend_id204;
	reg.bits.set_pend_id205 = set_pend_id205;
	reg.bits.set_pend_id206 = set_pend_id206;
	reg.bits.set_pend_id207 = set_pend_id207;
	reg.bits.set_pend_id208 = set_pend_id208;
	reg.bits.set_pend_id209 = set_pend_id209;
	reg.bits.set_pend_id210 = set_pend_id210;
	reg.bits.set_pend_id211 = set_pend_id211;
	reg.bits.set_pend_id212 = set_pend_id212;
	reg.bits.set_pend_id213 = set_pend_id213;
	reg.bits.set_pend_id214 = set_pend_id214;
	reg.bits.set_pend_id215 = set_pend_id215;
	reg.bits.set_pend_id216 = set_pend_id216;
	reg.bits.set_pend_id217 = set_pend_id217;
	reg.bits.set_pend_id218 = set_pend_id218;
	reg.bits.set_pend_id219 = set_pend_id219;
	reg.bits.set_pend_id220 = set_pend_id220;
	reg.bits.set_pend_id221 = set_pend_id221;
	reg.bits.set_pend_id222 = set_pend_id222;
	reg.bits.set_pend_id223 = set_pend_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID192
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID193
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID194
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID195
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID196
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID197
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID198
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID199
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID200
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID201
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID202
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID203
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID204
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID205
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID206
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID207
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID208
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID209
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID210
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID211
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID212
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID213
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID214
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID215
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID216
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID217
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID218
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID219
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID220
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID221
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID222
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISPENDR6_SET_PEND_ID223
fun_set_mod_reg_bit(gicd, ispendr6, set_pend_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0
void set_gicd_icpendr0(u32 reg_addr,
			u32 clr_pend_id0,
			u32 clr_pend_id1,
			u32 clr_pend_id2,
			u32 clr_pend_id3,
			u32 clr_pend_id4,
			u32 clr_pend_id5,
			u32 clr_pend_id6,
			u32 clr_pend_id7,
			u32 clr_pend_id8,
			u32 clr_pend_id9,
			u32 clr_pend_id10,
			u32 clr_pend_id11,
			u32 clr_pend_id12,
			u32 clr_pend_id13,
			u32 clr_pend_id14,
			u32 clr_pend_id15,
			u32 clr_pend_id16,
			u32 clr_pend_id17,
			u32 clr_pend_id18,
			u32 clr_pend_id19,
			u32 clr_pend_id20,
			u32 clr_pend_id21,
			u32 clr_pend_id22,
			u32 clr_pend_id23,
			u32 clr_pend_id24,
			u32 clr_pend_id25,
			u32 clr_pend_id26,
			u32 clr_pend_id27,
			u32 clr_pend_id28,
			u32 clr_pend_id29,
			u32 clr_pend_id30,
			u32 clr_pend_id31,
			u32 m_or_r)
{
	reg_gicd_icpendr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id0 = clr_pend_id0;
	reg.bits.clr_pend_id1 = clr_pend_id1;
	reg.bits.clr_pend_id2 = clr_pend_id2;
	reg.bits.clr_pend_id3 = clr_pend_id3;
	reg.bits.clr_pend_id4 = clr_pend_id4;
	reg.bits.clr_pend_id5 = clr_pend_id5;
	reg.bits.clr_pend_id6 = clr_pend_id6;
	reg.bits.clr_pend_id7 = clr_pend_id7;
	reg.bits.clr_pend_id8 = clr_pend_id8;
	reg.bits.clr_pend_id9 = clr_pend_id9;
	reg.bits.clr_pend_id10 = clr_pend_id10;
	reg.bits.clr_pend_id11 = clr_pend_id11;
	reg.bits.clr_pend_id12 = clr_pend_id12;
	reg.bits.clr_pend_id13 = clr_pend_id13;
	reg.bits.clr_pend_id14 = clr_pend_id14;
	reg.bits.clr_pend_id15 = clr_pend_id15;
	reg.bits.clr_pend_id16 = clr_pend_id16;
	reg.bits.clr_pend_id17 = clr_pend_id17;
	reg.bits.clr_pend_id18 = clr_pend_id18;
	reg.bits.clr_pend_id19 = clr_pend_id19;
	reg.bits.clr_pend_id20 = clr_pend_id20;
	reg.bits.clr_pend_id21 = clr_pend_id21;
	reg.bits.clr_pend_id22 = clr_pend_id22;
	reg.bits.clr_pend_id23 = clr_pend_id23;
	reg.bits.clr_pend_id24 = clr_pend_id24;
	reg.bits.clr_pend_id25 = clr_pend_id25;
	reg.bits.clr_pend_id26 = clr_pend_id26;
	reg.bits.clr_pend_id27 = clr_pend_id27;
	reg.bits.clr_pend_id28 = clr_pend_id28;
	reg.bits.clr_pend_id29 = clr_pend_id29;
	reg.bits.clr_pend_id30 = clr_pend_id30;
	reg.bits.clr_pend_id31 = clr_pend_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID0
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID1
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID2
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID3
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID4
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID5
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID6
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID7
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID8
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID9
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID10
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID11
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID12
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID13
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID14
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID15
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID16
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID17
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID18
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID19
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID20
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID21
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID22
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID23
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID24
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID25
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID26
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID27
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID28
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID29
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID30
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR0_CLR_PEND_ID31
fun_set_mod_reg_bit(gicd, icpendr0, clr_pend_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1
void set_gicd_icpendr1(u32 reg_addr,
			u32 clr_pend_id32,
			u32 clr_pend_id33,
			u32 clr_pend_id34,
			u32 clr_pend_id35,
			u32 clr_pend_id36,
			u32 clr_pend_id37,
			u32 clr_pend_id38,
			u32 clr_pend_id39,
			u32 clr_pend_id40,
			u32 clr_pend_id41,
			u32 clr_pend_id42,
			u32 clr_pend_id43,
			u32 clr_pend_id44,
			u32 clr_pend_id45,
			u32 clr_pend_id46,
			u32 clr_pend_id47,
			u32 clr_pend_id48,
			u32 clr_pend_id49,
			u32 clr_pend_id50,
			u32 clr_pend_id51,
			u32 clr_pend_id52,
			u32 clr_pend_id53,
			u32 clr_pend_id54,
			u32 clr_pend_id55,
			u32 clr_pend_id56,
			u32 clr_pend_id57,
			u32 clr_pend_id58,
			u32 clr_pend_id59,
			u32 clr_pend_id60,
			u32 clr_pend_id61,
			u32 clr_pend_id62,
			u32 clr_pend_id63,
			u32 m_or_r)
{
	reg_gicd_icpendr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id32 = clr_pend_id32;
	reg.bits.clr_pend_id33 = clr_pend_id33;
	reg.bits.clr_pend_id34 = clr_pend_id34;
	reg.bits.clr_pend_id35 = clr_pend_id35;
	reg.bits.clr_pend_id36 = clr_pend_id36;
	reg.bits.clr_pend_id37 = clr_pend_id37;
	reg.bits.clr_pend_id38 = clr_pend_id38;
	reg.bits.clr_pend_id39 = clr_pend_id39;
	reg.bits.clr_pend_id40 = clr_pend_id40;
	reg.bits.clr_pend_id41 = clr_pend_id41;
	reg.bits.clr_pend_id42 = clr_pend_id42;
	reg.bits.clr_pend_id43 = clr_pend_id43;
	reg.bits.clr_pend_id44 = clr_pend_id44;
	reg.bits.clr_pend_id45 = clr_pend_id45;
	reg.bits.clr_pend_id46 = clr_pend_id46;
	reg.bits.clr_pend_id47 = clr_pend_id47;
	reg.bits.clr_pend_id48 = clr_pend_id48;
	reg.bits.clr_pend_id49 = clr_pend_id49;
	reg.bits.clr_pend_id50 = clr_pend_id50;
	reg.bits.clr_pend_id51 = clr_pend_id51;
	reg.bits.clr_pend_id52 = clr_pend_id52;
	reg.bits.clr_pend_id53 = clr_pend_id53;
	reg.bits.clr_pend_id54 = clr_pend_id54;
	reg.bits.clr_pend_id55 = clr_pend_id55;
	reg.bits.clr_pend_id56 = clr_pend_id56;
	reg.bits.clr_pend_id57 = clr_pend_id57;
	reg.bits.clr_pend_id58 = clr_pend_id58;
	reg.bits.clr_pend_id59 = clr_pend_id59;
	reg.bits.clr_pend_id60 = clr_pend_id60;
	reg.bits.clr_pend_id61 = clr_pend_id61;
	reg.bits.clr_pend_id62 = clr_pend_id62;
	reg.bits.clr_pend_id63 = clr_pend_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID32
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID33
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID34
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID35
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID36
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID37
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID38
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID39
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID40
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID41
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID42
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID43
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID44
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID45
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID46
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID47
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID48
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID49
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID50
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID51
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID52
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID53
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID54
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID55
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID56
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID57
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID58
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID59
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID60
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID61
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID62
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR1_CLR_PEND_ID63
fun_set_mod_reg_bit(gicd, icpendr1, clr_pend_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2
void set_gicd_icpendr2(u32 reg_addr,
			u32 clr_pend_id64,
			u32 clr_pend_id65,
			u32 clr_pend_id66,
			u32 clr_pend_id67,
			u32 clr_pend_id68,
			u32 clr_pend_id69,
			u32 clr_pend_id70,
			u32 clr_pend_id71,
			u32 clr_pend_id72,
			u32 clr_pend_id73,
			u32 clr_pend_id74,
			u32 clr_pend_id75,
			u32 clr_pend_id76,
			u32 clr_pend_id77,
			u32 clr_pend_id78,
			u32 clr_pend_id79,
			u32 clr_pend_id80,
			u32 clr_pend_id81,
			u32 clr_pend_id82,
			u32 clr_pend_id83,
			u32 clr_pend_id84,
			u32 clr_pend_id85,
			u32 clr_pend_id86,
			u32 clr_pend_id87,
			u32 clr_pend_id88,
			u32 clr_pend_id89,
			u32 clr_pend_id90,
			u32 clr_pend_id91,
			u32 clr_pend_id92,
			u32 clr_pend_id93,
			u32 clr_pend_id94,
			u32 clr_pend_id95,
			u32 m_or_r)
{
	reg_gicd_icpendr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id64 = clr_pend_id64;
	reg.bits.clr_pend_id65 = clr_pend_id65;
	reg.bits.clr_pend_id66 = clr_pend_id66;
	reg.bits.clr_pend_id67 = clr_pend_id67;
	reg.bits.clr_pend_id68 = clr_pend_id68;
	reg.bits.clr_pend_id69 = clr_pend_id69;
	reg.bits.clr_pend_id70 = clr_pend_id70;
	reg.bits.clr_pend_id71 = clr_pend_id71;
	reg.bits.clr_pend_id72 = clr_pend_id72;
	reg.bits.clr_pend_id73 = clr_pend_id73;
	reg.bits.clr_pend_id74 = clr_pend_id74;
	reg.bits.clr_pend_id75 = clr_pend_id75;
	reg.bits.clr_pend_id76 = clr_pend_id76;
	reg.bits.clr_pend_id77 = clr_pend_id77;
	reg.bits.clr_pend_id78 = clr_pend_id78;
	reg.bits.clr_pend_id79 = clr_pend_id79;
	reg.bits.clr_pend_id80 = clr_pend_id80;
	reg.bits.clr_pend_id81 = clr_pend_id81;
	reg.bits.clr_pend_id82 = clr_pend_id82;
	reg.bits.clr_pend_id83 = clr_pend_id83;
	reg.bits.clr_pend_id84 = clr_pend_id84;
	reg.bits.clr_pend_id85 = clr_pend_id85;
	reg.bits.clr_pend_id86 = clr_pend_id86;
	reg.bits.clr_pend_id87 = clr_pend_id87;
	reg.bits.clr_pend_id88 = clr_pend_id88;
	reg.bits.clr_pend_id89 = clr_pend_id89;
	reg.bits.clr_pend_id90 = clr_pend_id90;
	reg.bits.clr_pend_id91 = clr_pend_id91;
	reg.bits.clr_pend_id92 = clr_pend_id92;
	reg.bits.clr_pend_id93 = clr_pend_id93;
	reg.bits.clr_pend_id94 = clr_pend_id94;
	reg.bits.clr_pend_id95 = clr_pend_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID64
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID65
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID66
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID67
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID68
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID69
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID70
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID71
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID72
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID73
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID74
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID75
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID76
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID77
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID78
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID79
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID80
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID81
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID82
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID83
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID84
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID85
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID86
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID87
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID88
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID89
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID90
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID91
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID92
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID93
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID94
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR2_CLR_PEND_ID95
fun_set_mod_reg_bit(gicd, icpendr2, clr_pend_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3
void set_gicd_icpendr3(u32 reg_addr,
			u32 clr_pend_id96,
			u32 clr_pend_id97,
			u32 clr_pend_id98,
			u32 clr_pend_id99,
			u32 clr_pend_id100,
			u32 clr_pend_id101,
			u32 clr_pend_id102,
			u32 clr_pend_id103,
			u32 clr_pend_id104,
			u32 clr_pend_id105,
			u32 clr_pend_id106,
			u32 clr_pend_id107,
			u32 clr_pend_id108,
			u32 clr_pend_id109,
			u32 clr_pend_id110,
			u32 clr_pend_id111,
			u32 clr_pend_id112,
			u32 clr_pend_id113,
			u32 clr_pend_id114,
			u32 clr_pend_id115,
			u32 clr_pend_id116,
			u32 clr_pend_id117,
			u32 clr_pend_id118,
			u32 clr_pend_id119,
			u32 clr_pend_id120,
			u32 clr_pend_id121,
			u32 clr_pend_id122,
			u32 clr_pend_id123,
			u32 clr_pend_id124,
			u32 clr_pend_id125,
			u32 clr_pend_id126,
			u32 clr_pend_id127,
			u32 m_or_r)
{
	reg_gicd_icpendr3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id96 = clr_pend_id96;
	reg.bits.clr_pend_id97 = clr_pend_id97;
	reg.bits.clr_pend_id98 = clr_pend_id98;
	reg.bits.clr_pend_id99 = clr_pend_id99;
	reg.bits.clr_pend_id100 = clr_pend_id100;
	reg.bits.clr_pend_id101 = clr_pend_id101;
	reg.bits.clr_pend_id102 = clr_pend_id102;
	reg.bits.clr_pend_id103 = clr_pend_id103;
	reg.bits.clr_pend_id104 = clr_pend_id104;
	reg.bits.clr_pend_id105 = clr_pend_id105;
	reg.bits.clr_pend_id106 = clr_pend_id106;
	reg.bits.clr_pend_id107 = clr_pend_id107;
	reg.bits.clr_pend_id108 = clr_pend_id108;
	reg.bits.clr_pend_id109 = clr_pend_id109;
	reg.bits.clr_pend_id110 = clr_pend_id110;
	reg.bits.clr_pend_id111 = clr_pend_id111;
	reg.bits.clr_pend_id112 = clr_pend_id112;
	reg.bits.clr_pend_id113 = clr_pend_id113;
	reg.bits.clr_pend_id114 = clr_pend_id114;
	reg.bits.clr_pend_id115 = clr_pend_id115;
	reg.bits.clr_pend_id116 = clr_pend_id116;
	reg.bits.clr_pend_id117 = clr_pend_id117;
	reg.bits.clr_pend_id118 = clr_pend_id118;
	reg.bits.clr_pend_id119 = clr_pend_id119;
	reg.bits.clr_pend_id120 = clr_pend_id120;
	reg.bits.clr_pend_id121 = clr_pend_id121;
	reg.bits.clr_pend_id122 = clr_pend_id122;
	reg.bits.clr_pend_id123 = clr_pend_id123;
	reg.bits.clr_pend_id124 = clr_pend_id124;
	reg.bits.clr_pend_id125 = clr_pend_id125;
	reg.bits.clr_pend_id126 = clr_pend_id126;
	reg.bits.clr_pend_id127 = clr_pend_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID96
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID97
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID98
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID99
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID100
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID101
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID102
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID103
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID104
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID105
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID106
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID107
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID108
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID109
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID110
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID111
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID112
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID113
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID114
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID115
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID116
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID117
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID118
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID119
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID120
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID121
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID122
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID123
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID124
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID125
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID126
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR3_CLR_PEND_ID127
fun_set_mod_reg_bit(gicd, icpendr3, clr_pend_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4
void set_gicd_icpendr4(u32 reg_addr,
			u32 clr_pend_id128,
			u32 clr_pend_id129,
			u32 clr_pend_id130,
			u32 clr_pend_id131,
			u32 clr_pend_id132,
			u32 clr_pend_id133,
			u32 clr_pend_id134,
			u32 clr_pend_id135,
			u32 clr_pend_id136,
			u32 clr_pend_id137,
			u32 clr_pend_id138,
			u32 clr_pend_id139,
			u32 clr_pend_id140,
			u32 clr_pend_id141,
			u32 clr_pend_id142,
			u32 clr_pend_id143,
			u32 clr_pend_id144,
			u32 clr_pend_id145,
			u32 clr_pend_id146,
			u32 clr_pend_id147,
			u32 clr_pend_id148,
			u32 clr_pend_id149,
			u32 clr_pend_id150,
			u32 clr_pend_id151,
			u32 clr_pend_id152,
			u32 clr_pend_id153,
			u32 clr_pend_id154,
			u32 clr_pend_id155,
			u32 clr_pend_id156,
			u32 clr_pend_id157,
			u32 clr_pend_id158,
			u32 clr_pend_id159,
			u32 m_or_r)
{
	reg_gicd_icpendr4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id128 = clr_pend_id128;
	reg.bits.clr_pend_id129 = clr_pend_id129;
	reg.bits.clr_pend_id130 = clr_pend_id130;
	reg.bits.clr_pend_id131 = clr_pend_id131;
	reg.bits.clr_pend_id132 = clr_pend_id132;
	reg.bits.clr_pend_id133 = clr_pend_id133;
	reg.bits.clr_pend_id134 = clr_pend_id134;
	reg.bits.clr_pend_id135 = clr_pend_id135;
	reg.bits.clr_pend_id136 = clr_pend_id136;
	reg.bits.clr_pend_id137 = clr_pend_id137;
	reg.bits.clr_pend_id138 = clr_pend_id138;
	reg.bits.clr_pend_id139 = clr_pend_id139;
	reg.bits.clr_pend_id140 = clr_pend_id140;
	reg.bits.clr_pend_id141 = clr_pend_id141;
	reg.bits.clr_pend_id142 = clr_pend_id142;
	reg.bits.clr_pend_id143 = clr_pend_id143;
	reg.bits.clr_pend_id144 = clr_pend_id144;
	reg.bits.clr_pend_id145 = clr_pend_id145;
	reg.bits.clr_pend_id146 = clr_pend_id146;
	reg.bits.clr_pend_id147 = clr_pend_id147;
	reg.bits.clr_pend_id148 = clr_pend_id148;
	reg.bits.clr_pend_id149 = clr_pend_id149;
	reg.bits.clr_pend_id150 = clr_pend_id150;
	reg.bits.clr_pend_id151 = clr_pend_id151;
	reg.bits.clr_pend_id152 = clr_pend_id152;
	reg.bits.clr_pend_id153 = clr_pend_id153;
	reg.bits.clr_pend_id154 = clr_pend_id154;
	reg.bits.clr_pend_id155 = clr_pend_id155;
	reg.bits.clr_pend_id156 = clr_pend_id156;
	reg.bits.clr_pend_id157 = clr_pend_id157;
	reg.bits.clr_pend_id158 = clr_pend_id158;
	reg.bits.clr_pend_id159 = clr_pend_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID128
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID129
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID130
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID131
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID132
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID133
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID134
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID135
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID136
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID137
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID138
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID139
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID140
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID141
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID142
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID143
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID144
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID145
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID146
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID147
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID148
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID149
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID150
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID151
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID152
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID153
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID154
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID155
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID156
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID157
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID158
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR4_CLR_PEND_ID159
fun_set_mod_reg_bit(gicd, icpendr4, clr_pend_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5
void set_gicd_icpendr5(u32 reg_addr,
			u32 clr_pend_id160,
			u32 clr_pend_id161,
			u32 clr_pend_id162,
			u32 clr_pend_id163,
			u32 clr_pend_id164,
			u32 clr_pend_id165,
			u32 clr_pend_id166,
			u32 clr_pend_id167,
			u32 clr_pend_id168,
			u32 clr_pend_id169,
			u32 clr_pend_id170,
			u32 clr_pend_id171,
			u32 clr_pend_id172,
			u32 clr_pend_id173,
			u32 clr_pend_id174,
			u32 clr_pend_id175,
			u32 clr_pend_id176,
			u32 clr_pend_id177,
			u32 clr_pend_id178,
			u32 clr_pend_id179,
			u32 clr_pend_id180,
			u32 clr_pend_id181,
			u32 clr_pend_id182,
			u32 clr_pend_id183,
			u32 clr_pend_id184,
			u32 clr_pend_id185,
			u32 clr_pend_id186,
			u32 clr_pend_id187,
			u32 clr_pend_id188,
			u32 clr_pend_id189,
			u32 clr_pend_id190,
			u32 clr_pend_id191,
			u32 m_or_r)
{
	reg_gicd_icpendr5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id160 = clr_pend_id160;
	reg.bits.clr_pend_id161 = clr_pend_id161;
	reg.bits.clr_pend_id162 = clr_pend_id162;
	reg.bits.clr_pend_id163 = clr_pend_id163;
	reg.bits.clr_pend_id164 = clr_pend_id164;
	reg.bits.clr_pend_id165 = clr_pend_id165;
	reg.bits.clr_pend_id166 = clr_pend_id166;
	reg.bits.clr_pend_id167 = clr_pend_id167;
	reg.bits.clr_pend_id168 = clr_pend_id168;
	reg.bits.clr_pend_id169 = clr_pend_id169;
	reg.bits.clr_pend_id170 = clr_pend_id170;
	reg.bits.clr_pend_id171 = clr_pend_id171;
	reg.bits.clr_pend_id172 = clr_pend_id172;
	reg.bits.clr_pend_id173 = clr_pend_id173;
	reg.bits.clr_pend_id174 = clr_pend_id174;
	reg.bits.clr_pend_id175 = clr_pend_id175;
	reg.bits.clr_pend_id176 = clr_pend_id176;
	reg.bits.clr_pend_id177 = clr_pend_id177;
	reg.bits.clr_pend_id178 = clr_pend_id178;
	reg.bits.clr_pend_id179 = clr_pend_id179;
	reg.bits.clr_pend_id180 = clr_pend_id180;
	reg.bits.clr_pend_id181 = clr_pend_id181;
	reg.bits.clr_pend_id182 = clr_pend_id182;
	reg.bits.clr_pend_id183 = clr_pend_id183;
	reg.bits.clr_pend_id184 = clr_pend_id184;
	reg.bits.clr_pend_id185 = clr_pend_id185;
	reg.bits.clr_pend_id186 = clr_pend_id186;
	reg.bits.clr_pend_id187 = clr_pend_id187;
	reg.bits.clr_pend_id188 = clr_pend_id188;
	reg.bits.clr_pend_id189 = clr_pend_id189;
	reg.bits.clr_pend_id190 = clr_pend_id190;
	reg.bits.clr_pend_id191 = clr_pend_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID160
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID161
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID162
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID163
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID164
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID165
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID166
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID167
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID168
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID169
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID170
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID171
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID172
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID173
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID174
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID175
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID176
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID177
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID178
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID179
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID180
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID181
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID182
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID183
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID184
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID185
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID186
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID187
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID188
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID189
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID190
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR5_CLR_PEND_ID191
fun_set_mod_reg_bit(gicd, icpendr5, clr_pend_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6
void set_gicd_icpendr6(u32 reg_addr,
			u32 clr_pend_id192,
			u32 clr_pend_id193,
			u32 clr_pend_id194,
			u32 clr_pend_id195,
			u32 clr_pend_id196,
			u32 clr_pend_id197,
			u32 clr_pend_id198,
			u32 clr_pend_id199,
			u32 clr_pend_id200,
			u32 clr_pend_id201,
			u32 clr_pend_id202,
			u32 clr_pend_id203,
			u32 clr_pend_id204,
			u32 clr_pend_id205,
			u32 clr_pend_id206,
			u32 clr_pend_id207,
			u32 clr_pend_id208,
			u32 clr_pend_id209,
			u32 clr_pend_id210,
			u32 clr_pend_id211,
			u32 clr_pend_id212,
			u32 clr_pend_id213,
			u32 clr_pend_id214,
			u32 clr_pend_id215,
			u32 clr_pend_id216,
			u32 clr_pend_id217,
			u32 clr_pend_id218,
			u32 clr_pend_id219,
			u32 clr_pend_id220,
			u32 clr_pend_id221,
			u32 clr_pend_id222,
			u32 clr_pend_id223,
			u32 m_or_r)
{
	reg_gicd_icpendr6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id192 = clr_pend_id192;
	reg.bits.clr_pend_id193 = clr_pend_id193;
	reg.bits.clr_pend_id194 = clr_pend_id194;
	reg.bits.clr_pend_id195 = clr_pend_id195;
	reg.bits.clr_pend_id196 = clr_pend_id196;
	reg.bits.clr_pend_id197 = clr_pend_id197;
	reg.bits.clr_pend_id198 = clr_pend_id198;
	reg.bits.clr_pend_id199 = clr_pend_id199;
	reg.bits.clr_pend_id200 = clr_pend_id200;
	reg.bits.clr_pend_id201 = clr_pend_id201;
	reg.bits.clr_pend_id202 = clr_pend_id202;
	reg.bits.clr_pend_id203 = clr_pend_id203;
	reg.bits.clr_pend_id204 = clr_pend_id204;
	reg.bits.clr_pend_id205 = clr_pend_id205;
	reg.bits.clr_pend_id206 = clr_pend_id206;
	reg.bits.clr_pend_id207 = clr_pend_id207;
	reg.bits.clr_pend_id208 = clr_pend_id208;
	reg.bits.clr_pend_id209 = clr_pend_id209;
	reg.bits.clr_pend_id210 = clr_pend_id210;
	reg.bits.clr_pend_id211 = clr_pend_id211;
	reg.bits.clr_pend_id212 = clr_pend_id212;
	reg.bits.clr_pend_id213 = clr_pend_id213;
	reg.bits.clr_pend_id214 = clr_pend_id214;
	reg.bits.clr_pend_id215 = clr_pend_id215;
	reg.bits.clr_pend_id216 = clr_pend_id216;
	reg.bits.clr_pend_id217 = clr_pend_id217;
	reg.bits.clr_pend_id218 = clr_pend_id218;
	reg.bits.clr_pend_id219 = clr_pend_id219;
	reg.bits.clr_pend_id220 = clr_pend_id220;
	reg.bits.clr_pend_id221 = clr_pend_id221;
	reg.bits.clr_pend_id222 = clr_pend_id222;
	reg.bits.clr_pend_id223 = clr_pend_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID192
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID193
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID194
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID195
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID196
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID197
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID198
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID199
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID200
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID201
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID202
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID203
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID204
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID205
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID206
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID207
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID208
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID209
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID210
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID211
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID212
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID213
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID214
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID215
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID216
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID217
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID218
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID219
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID220
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID221
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID222
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICPENDR6_CLR_PEND_ID223
fun_set_mod_reg_bit(gicd, icpendr6, clr_pend_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0
void set_gicd_isactiver0(u32 reg_addr,
			u32 set_active_id0,
			u32 set_active_id1,
			u32 set_active_id2,
			u32 set_active_id3,
			u32 set_active_id4,
			u32 set_active_id5,
			u32 set_active_id6,
			u32 set_active_id7,
			u32 set_active_id8,
			u32 set_active_id9,
			u32 set_active_id10,
			u32 set_active_id11,
			u32 set_active_id12,
			u32 set_active_id13,
			u32 set_active_id14,
			u32 set_active_id15,
			u32 set_active_id16,
			u32 set_active_id17,
			u32 set_active_id18,
			u32 set_active_id19,
			u32 set_active_id20,
			u32 set_active_id21,
			u32 set_active_id22,
			u32 set_active_id23,
			u32 set_active_id24,
			u32 set_active_id25,
			u32 set_active_id26,
			u32 set_active_id27,
			u32 set_active_id28,
			u32 set_active_id29,
			u32 set_active_id30,
			u32 set_active_id31,
			u32 m_or_r)
{
	reg_gicd_isactiver0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_active_id0 = set_active_id0;
	reg.bits.set_active_id1 = set_active_id1;
	reg.bits.set_active_id2 = set_active_id2;
	reg.bits.set_active_id3 = set_active_id3;
	reg.bits.set_active_id4 = set_active_id4;
	reg.bits.set_active_id5 = set_active_id5;
	reg.bits.set_active_id6 = set_active_id6;
	reg.bits.set_active_id7 = set_active_id7;
	reg.bits.set_active_id8 = set_active_id8;
	reg.bits.set_active_id9 = set_active_id9;
	reg.bits.set_active_id10 = set_active_id10;
	reg.bits.set_active_id11 = set_active_id11;
	reg.bits.set_active_id12 = set_active_id12;
	reg.bits.set_active_id13 = set_active_id13;
	reg.bits.set_active_id14 = set_active_id14;
	reg.bits.set_active_id15 = set_active_id15;
	reg.bits.set_active_id16 = set_active_id16;
	reg.bits.set_active_id17 = set_active_id17;
	reg.bits.set_active_id18 = set_active_id18;
	reg.bits.set_active_id19 = set_active_id19;
	reg.bits.set_active_id20 = set_active_id20;
	reg.bits.set_active_id21 = set_active_id21;
	reg.bits.set_active_id22 = set_active_id22;
	reg.bits.set_active_id23 = set_active_id23;
	reg.bits.set_active_id24 = set_active_id24;
	reg.bits.set_active_id25 = set_active_id25;
	reg.bits.set_active_id26 = set_active_id26;
	reg.bits.set_active_id27 = set_active_id27;
	reg.bits.set_active_id28 = set_active_id28;
	reg.bits.set_active_id29 = set_active_id29;
	reg.bits.set_active_id30 = set_active_id30;
	reg.bits.set_active_id31 = set_active_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID0
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID1
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID2
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID3
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID4
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID5
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID6
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID7
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID8
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID9
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID10
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID11
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID12
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID13
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID14
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID15
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID16
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID17
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID18
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID19
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID20
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID21
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID22
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID23
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID24
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID25
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID26
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID27
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID28
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID29
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID30
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER0_SET_ACTIVE_ID31
fun_set_mod_reg_bit(gicd, isactiver0, set_active_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1
void set_gicd_isactiver1(u32 reg_addr,
			u32 set_active_id32,
			u32 set_active_id33,
			u32 set_active_id34,
			u32 set_active_id35,
			u32 set_active_id36,
			u32 set_active_id37,
			u32 set_active_id38,
			u32 set_active_id39,
			u32 set_active_id40,
			u32 set_active_id41,
			u32 set_active_id42,
			u32 set_active_id43,
			u32 set_active_id44,
			u32 set_active_id45,
			u32 set_active_id46,
			u32 set_active_id47,
			u32 set_active_id48,
			u32 set_active_id49,
			u32 set_active_id50,
			u32 set_active_id51,
			u32 set_active_id52,
			u32 set_active_id53,
			u32 set_active_id54,
			u32 set_active_id55,
			u32 set_active_id56,
			u32 set_active_id57,
			u32 set_active_id58,
			u32 set_active_id59,
			u32 set_active_id60,
			u32 set_active_id61,
			u32 set_active_id62,
			u32 set_active_id63,
			u32 m_or_r)
{
	reg_gicd_isactiver1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_active_id32 = set_active_id32;
	reg.bits.set_active_id33 = set_active_id33;
	reg.bits.set_active_id34 = set_active_id34;
	reg.bits.set_active_id35 = set_active_id35;
	reg.bits.set_active_id36 = set_active_id36;
	reg.bits.set_active_id37 = set_active_id37;
	reg.bits.set_active_id38 = set_active_id38;
	reg.bits.set_active_id39 = set_active_id39;
	reg.bits.set_active_id40 = set_active_id40;
	reg.bits.set_active_id41 = set_active_id41;
	reg.bits.set_active_id42 = set_active_id42;
	reg.bits.set_active_id43 = set_active_id43;
	reg.bits.set_active_id44 = set_active_id44;
	reg.bits.set_active_id45 = set_active_id45;
	reg.bits.set_active_id46 = set_active_id46;
	reg.bits.set_active_id47 = set_active_id47;
	reg.bits.set_active_id48 = set_active_id48;
	reg.bits.set_active_id49 = set_active_id49;
	reg.bits.set_active_id50 = set_active_id50;
	reg.bits.set_active_id51 = set_active_id51;
	reg.bits.set_active_id52 = set_active_id52;
	reg.bits.set_active_id53 = set_active_id53;
	reg.bits.set_active_id54 = set_active_id54;
	reg.bits.set_active_id55 = set_active_id55;
	reg.bits.set_active_id56 = set_active_id56;
	reg.bits.set_active_id57 = set_active_id57;
	reg.bits.set_active_id58 = set_active_id58;
	reg.bits.set_active_id59 = set_active_id59;
	reg.bits.set_active_id60 = set_active_id60;
	reg.bits.set_active_id61 = set_active_id61;
	reg.bits.set_active_id62 = set_active_id62;
	reg.bits.set_active_id63 = set_active_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID32
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID33
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID34
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID35
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID36
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID37
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID38
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID39
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID40
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID41
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID42
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID43
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID44
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID45
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID46
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID47
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID48
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID49
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID50
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID51
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID52
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID53
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID54
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID55
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID56
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID57
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID58
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID59
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID60
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID61
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID62
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER1_SET_ACTIVE_ID63
fun_set_mod_reg_bit(gicd, isactiver1, set_active_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2
void set_gicd_isactiver2(u32 reg_addr,
			u32 set_active_id64,
			u32 set_active_id65,
			u32 set_active_id66,
			u32 set_active_id67,
			u32 set_active_id68,
			u32 set_active_id69,
			u32 set_active_id70,
			u32 set_active_id71,
			u32 set_active_id72,
			u32 set_active_id73,
			u32 set_active_id74,
			u32 set_active_id75,
			u32 set_active_id76,
			u32 set_active_id77,
			u32 set_active_id78,
			u32 set_active_id79,
			u32 set_active_id80,
			u32 set_active_id81,
			u32 set_active_id82,
			u32 set_active_id83,
			u32 set_active_id84,
			u32 set_active_id85,
			u32 set_active_id86,
			u32 set_active_id87,
			u32 set_active_id88,
			u32 set_active_id89,
			u32 set_active_id90,
			u32 set_active_id91,
			u32 set_active_id92,
			u32 set_active_id93,
			u32 set_active_id94,
			u32 set_active_id95,
			u32 m_or_r)
{
	reg_gicd_isactiver2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_active_id64 = set_active_id64;
	reg.bits.set_active_id65 = set_active_id65;
	reg.bits.set_active_id66 = set_active_id66;
	reg.bits.set_active_id67 = set_active_id67;
	reg.bits.set_active_id68 = set_active_id68;
	reg.bits.set_active_id69 = set_active_id69;
	reg.bits.set_active_id70 = set_active_id70;
	reg.bits.set_active_id71 = set_active_id71;
	reg.bits.set_active_id72 = set_active_id72;
	reg.bits.set_active_id73 = set_active_id73;
	reg.bits.set_active_id74 = set_active_id74;
	reg.bits.set_active_id75 = set_active_id75;
	reg.bits.set_active_id76 = set_active_id76;
	reg.bits.set_active_id77 = set_active_id77;
	reg.bits.set_active_id78 = set_active_id78;
	reg.bits.set_active_id79 = set_active_id79;
	reg.bits.set_active_id80 = set_active_id80;
	reg.bits.set_active_id81 = set_active_id81;
	reg.bits.set_active_id82 = set_active_id82;
	reg.bits.set_active_id83 = set_active_id83;
	reg.bits.set_active_id84 = set_active_id84;
	reg.bits.set_active_id85 = set_active_id85;
	reg.bits.set_active_id86 = set_active_id86;
	reg.bits.set_active_id87 = set_active_id87;
	reg.bits.set_active_id88 = set_active_id88;
	reg.bits.set_active_id89 = set_active_id89;
	reg.bits.set_active_id90 = set_active_id90;
	reg.bits.set_active_id91 = set_active_id91;
	reg.bits.set_active_id92 = set_active_id92;
	reg.bits.set_active_id93 = set_active_id93;
	reg.bits.set_active_id94 = set_active_id94;
	reg.bits.set_active_id95 = set_active_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID64
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID65
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID66
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID67
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID68
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID69
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID70
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID71
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID72
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID73
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID74
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID75
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID76
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID77
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID78
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID79
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID80
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID81
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID82
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID83
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID84
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID85
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID86
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID87
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID88
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID89
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID90
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID91
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID92
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID93
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID94
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER2_SET_ACTIVE_ID95
fun_set_mod_reg_bit(gicd, isactiver2, set_active_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3
void set_gicd_isactiver3(u32 reg_addr,
			u32 set_active_id96,
			u32 set_active_id97,
			u32 set_active_id98,
			u32 set_active_id99,
			u32 set_active_id100,
			u32 set_active_id101,
			u32 set_active_id102,
			u32 set_active_id103,
			u32 set_active_id104,
			u32 set_active_id105,
			u32 set_active_id106,
			u32 set_active_id107,
			u32 set_active_id108,
			u32 set_active_id109,
			u32 set_active_id110,
			u32 set_active_id111,
			u32 set_active_id112,
			u32 set_active_id113,
			u32 set_active_id114,
			u32 set_active_id115,
			u32 set_active_id116,
			u32 set_active_id117,
			u32 set_active_id118,
			u32 set_active_id119,
			u32 set_active_id120,
			u32 set_active_id121,
			u32 set_active_id122,
			u32 set_active_id123,
			u32 set_active_id124,
			u32 set_active_id125,
			u32 set_active_id126,
			u32 set_active_id127,
			u32 m_or_r)
{
	reg_gicd_isactiver3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_active_id96 = set_active_id96;
	reg.bits.set_active_id97 = set_active_id97;
	reg.bits.set_active_id98 = set_active_id98;
	reg.bits.set_active_id99 = set_active_id99;
	reg.bits.set_active_id100 = set_active_id100;
	reg.bits.set_active_id101 = set_active_id101;
	reg.bits.set_active_id102 = set_active_id102;
	reg.bits.set_active_id103 = set_active_id103;
	reg.bits.set_active_id104 = set_active_id104;
	reg.bits.set_active_id105 = set_active_id105;
	reg.bits.set_active_id106 = set_active_id106;
	reg.bits.set_active_id107 = set_active_id107;
	reg.bits.set_active_id108 = set_active_id108;
	reg.bits.set_active_id109 = set_active_id109;
	reg.bits.set_active_id110 = set_active_id110;
	reg.bits.set_active_id111 = set_active_id111;
	reg.bits.set_active_id112 = set_active_id112;
	reg.bits.set_active_id113 = set_active_id113;
	reg.bits.set_active_id114 = set_active_id114;
	reg.bits.set_active_id115 = set_active_id115;
	reg.bits.set_active_id116 = set_active_id116;
	reg.bits.set_active_id117 = set_active_id117;
	reg.bits.set_active_id118 = set_active_id118;
	reg.bits.set_active_id119 = set_active_id119;
	reg.bits.set_active_id120 = set_active_id120;
	reg.bits.set_active_id121 = set_active_id121;
	reg.bits.set_active_id122 = set_active_id122;
	reg.bits.set_active_id123 = set_active_id123;
	reg.bits.set_active_id124 = set_active_id124;
	reg.bits.set_active_id125 = set_active_id125;
	reg.bits.set_active_id126 = set_active_id126;
	reg.bits.set_active_id127 = set_active_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID96
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID97
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID98
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID99
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID100
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID101
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID102
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID103
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID104
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID105
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID106
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID107
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID108
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID109
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID110
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID111
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID112
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID113
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID114
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID115
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID116
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID117
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID118
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID119
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID120
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID121
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID122
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID123
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID124
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID125
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID126
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER3_SET_ACTIVE_ID127
fun_set_mod_reg_bit(gicd, isactiver3, set_active_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4
void set_gicd_isactiver4(u32 reg_addr,
			u32 set_active_id128,
			u32 set_active_id129,
			u32 set_active_id130,
			u32 set_active_id131,
			u32 set_active_id132,
			u32 set_active_id133,
			u32 set_active_id134,
			u32 set_active_id135,
			u32 set_active_id136,
			u32 set_active_id137,
			u32 set_active_id138,
			u32 set_active_id139,
			u32 set_active_id140,
			u32 set_active_id141,
			u32 set_active_id142,
			u32 set_active_id143,
			u32 set_active_id144,
			u32 set_active_id145,
			u32 set_active_id146,
			u32 set_active_id147,
			u32 set_active_id148,
			u32 set_active_id149,
			u32 set_active_id150,
			u32 set_active_id151,
			u32 set_active_id152,
			u32 set_active_id153,
			u32 set_active_id154,
			u32 set_active_id155,
			u32 set_active_id156,
			u32 set_active_id157,
			u32 set_active_id158,
			u32 set_active_id159,
			u32 m_or_r)
{
	reg_gicd_isactiver4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_active_id128 = set_active_id128;
	reg.bits.set_active_id129 = set_active_id129;
	reg.bits.set_active_id130 = set_active_id130;
	reg.bits.set_active_id131 = set_active_id131;
	reg.bits.set_active_id132 = set_active_id132;
	reg.bits.set_active_id133 = set_active_id133;
	reg.bits.set_active_id134 = set_active_id134;
	reg.bits.set_active_id135 = set_active_id135;
	reg.bits.set_active_id136 = set_active_id136;
	reg.bits.set_active_id137 = set_active_id137;
	reg.bits.set_active_id138 = set_active_id138;
	reg.bits.set_active_id139 = set_active_id139;
	reg.bits.set_active_id140 = set_active_id140;
	reg.bits.set_active_id141 = set_active_id141;
	reg.bits.set_active_id142 = set_active_id142;
	reg.bits.set_active_id143 = set_active_id143;
	reg.bits.set_active_id144 = set_active_id144;
	reg.bits.set_active_id145 = set_active_id145;
	reg.bits.set_active_id146 = set_active_id146;
	reg.bits.set_active_id147 = set_active_id147;
	reg.bits.set_active_id148 = set_active_id148;
	reg.bits.set_active_id149 = set_active_id149;
	reg.bits.set_active_id150 = set_active_id150;
	reg.bits.set_active_id151 = set_active_id151;
	reg.bits.set_active_id152 = set_active_id152;
	reg.bits.set_active_id153 = set_active_id153;
	reg.bits.set_active_id154 = set_active_id154;
	reg.bits.set_active_id155 = set_active_id155;
	reg.bits.set_active_id156 = set_active_id156;
	reg.bits.set_active_id157 = set_active_id157;
	reg.bits.set_active_id158 = set_active_id158;
	reg.bits.set_active_id159 = set_active_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID128
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID129
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID130
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID131
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID132
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID133
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID134
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID135
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID136
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID137
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID138
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID139
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID140
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID141
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID142
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID143
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID144
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID145
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID146
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID147
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID148
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID149
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID150
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID151
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID152
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID153
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID154
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID155
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID156
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID157
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID158
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER4_SET_ACTIVE_ID159
fun_set_mod_reg_bit(gicd, isactiver4, set_active_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5
void set_gicd_isactiver5(u32 reg_addr,
			u32 set_active_id160,
			u32 set_active_id161,
			u32 set_active_id162,
			u32 set_active_id163,
			u32 set_active_id164,
			u32 set_active_id165,
			u32 set_active_id166,
			u32 set_active_id167,
			u32 set_active_id168,
			u32 set_active_id169,
			u32 set_active_id170,
			u32 set_active_id171,
			u32 set_active_id172,
			u32 set_active_id173,
			u32 set_active_id174,
			u32 set_active_id175,
			u32 set_active_id176,
			u32 set_active_id177,
			u32 set_active_id178,
			u32 set_active_id179,
			u32 set_active_id180,
			u32 set_active_id181,
			u32 set_active_id182,
			u32 set_active_id183,
			u32 set_active_id184,
			u32 set_active_id185,
			u32 set_active_id186,
			u32 set_active_id187,
			u32 set_active_id188,
			u32 set_active_id189,
			u32 set_active_id190,
			u32 set_active_id191,
			u32 m_or_r)
{
	reg_gicd_isactiver5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_active_id160 = set_active_id160;
	reg.bits.set_active_id161 = set_active_id161;
	reg.bits.set_active_id162 = set_active_id162;
	reg.bits.set_active_id163 = set_active_id163;
	reg.bits.set_active_id164 = set_active_id164;
	reg.bits.set_active_id165 = set_active_id165;
	reg.bits.set_active_id166 = set_active_id166;
	reg.bits.set_active_id167 = set_active_id167;
	reg.bits.set_active_id168 = set_active_id168;
	reg.bits.set_active_id169 = set_active_id169;
	reg.bits.set_active_id170 = set_active_id170;
	reg.bits.set_active_id171 = set_active_id171;
	reg.bits.set_active_id172 = set_active_id172;
	reg.bits.set_active_id173 = set_active_id173;
	reg.bits.set_active_id174 = set_active_id174;
	reg.bits.set_active_id175 = set_active_id175;
	reg.bits.set_active_id176 = set_active_id176;
	reg.bits.set_active_id177 = set_active_id177;
	reg.bits.set_active_id178 = set_active_id178;
	reg.bits.set_active_id179 = set_active_id179;
	reg.bits.set_active_id180 = set_active_id180;
	reg.bits.set_active_id181 = set_active_id181;
	reg.bits.set_active_id182 = set_active_id182;
	reg.bits.set_active_id183 = set_active_id183;
	reg.bits.set_active_id184 = set_active_id184;
	reg.bits.set_active_id185 = set_active_id185;
	reg.bits.set_active_id186 = set_active_id186;
	reg.bits.set_active_id187 = set_active_id187;
	reg.bits.set_active_id188 = set_active_id188;
	reg.bits.set_active_id189 = set_active_id189;
	reg.bits.set_active_id190 = set_active_id190;
	reg.bits.set_active_id191 = set_active_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID160
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID161
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID162
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID163
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID164
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID165
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID166
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID167
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID168
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID169
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID170
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID171
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID172
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID173
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID174
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID175
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID176
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID177
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID178
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID179
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID180
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID181
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID182
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID183
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID184
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID185
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID186
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID187
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID188
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID189
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID190
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER5_SET_ACTIVE_ID191
fun_set_mod_reg_bit(gicd, isactiver5, set_active_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6
void set_gicd_isactiver6(u32 reg_addr,
			u32 set_active_id192,
			u32 set_active_id193,
			u32 set_active_id194,
			u32 set_active_id195,
			u32 set_active_id196,
			u32 set_active_id197,
			u32 set_active_id198,
			u32 set_active_id199,
			u32 set_active_id200,
			u32 set_active_id201,
			u32 set_active_id202,
			u32 set_active_id203,
			u32 set_active_id204,
			u32 set_active_id205,
			u32 set_active_id206,
			u32 set_active_id207,
			u32 set_active_id208,
			u32 set_active_id209,
			u32 set_active_id210,
			u32 set_active_id211,
			u32 set_active_id212,
			u32 set_active_id213,
			u32 set_active_id214,
			u32 set_active_id215,
			u32 set_active_id216,
			u32 set_active_id217,
			u32 set_active_id218,
			u32 set_active_id219,
			u32 set_active_id220,
			u32 set_active_id221,
			u32 set_active_id222,
			u32 set_active_id223,
			u32 m_or_r)
{
	reg_gicd_isactiver6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_active_id192 = set_active_id192;
	reg.bits.set_active_id193 = set_active_id193;
	reg.bits.set_active_id194 = set_active_id194;
	reg.bits.set_active_id195 = set_active_id195;
	reg.bits.set_active_id196 = set_active_id196;
	reg.bits.set_active_id197 = set_active_id197;
	reg.bits.set_active_id198 = set_active_id198;
	reg.bits.set_active_id199 = set_active_id199;
	reg.bits.set_active_id200 = set_active_id200;
	reg.bits.set_active_id201 = set_active_id201;
	reg.bits.set_active_id202 = set_active_id202;
	reg.bits.set_active_id203 = set_active_id203;
	reg.bits.set_active_id204 = set_active_id204;
	reg.bits.set_active_id205 = set_active_id205;
	reg.bits.set_active_id206 = set_active_id206;
	reg.bits.set_active_id207 = set_active_id207;
	reg.bits.set_active_id208 = set_active_id208;
	reg.bits.set_active_id209 = set_active_id209;
	reg.bits.set_active_id210 = set_active_id210;
	reg.bits.set_active_id211 = set_active_id211;
	reg.bits.set_active_id212 = set_active_id212;
	reg.bits.set_active_id213 = set_active_id213;
	reg.bits.set_active_id214 = set_active_id214;
	reg.bits.set_active_id215 = set_active_id215;
	reg.bits.set_active_id216 = set_active_id216;
	reg.bits.set_active_id217 = set_active_id217;
	reg.bits.set_active_id218 = set_active_id218;
	reg.bits.set_active_id219 = set_active_id219;
	reg.bits.set_active_id220 = set_active_id220;
	reg.bits.set_active_id221 = set_active_id221;
	reg.bits.set_active_id222 = set_active_id222;
	reg.bits.set_active_id223 = set_active_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID192
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID193
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID194
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID195
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID196
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID197
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID198
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID199
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID200
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID201
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID202
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID203
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID204
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID205
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID206
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID207
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID208
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID209
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID210
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID211
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID212
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID213
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID214
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID215
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID216
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID217
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID218
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID219
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID220
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID221
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID222
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ISACTIVER6_SET_ACTIVE_ID223
fun_set_mod_reg_bit(gicd, isactiver6, set_active_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0
void set_gicd_icactiver0(u32 reg_addr,
			u32 clear_active_id0,
			u32 clear_active_id1,
			u32 clear_active_id2,
			u32 clear_active_id3,
			u32 clear_active_id4,
			u32 clear_active_id5,
			u32 clear_active_id6,
			u32 clear_active_id7,
			u32 clear_active_id8,
			u32 clear_active_id9,
			u32 clear_active_id10,
			u32 clear_active_id11,
			u32 clear_active_id12,
			u32 clear_active_id13,
			u32 clear_active_id14,
			u32 clear_active_id15,
			u32 clear_active_id16,
			u32 clear_active_id17,
			u32 clear_active_id18,
			u32 clear_active_id19,
			u32 clear_active_id20,
			u32 clear_active_id21,
			u32 clear_active_id22,
			u32 clear_active_id23,
			u32 clear_active_id24,
			u32 clear_active_id25,
			u32 clear_active_id26,
			u32 clear_active_id27,
			u32 clear_active_id28,
			u32 clear_active_id29,
			u32 clear_active_id30,
			u32 clear_active_id31,
			u32 m_or_r)
{
	reg_gicd_icactiver0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clear_active_id0 = clear_active_id0;
	reg.bits.clear_active_id1 = clear_active_id1;
	reg.bits.clear_active_id2 = clear_active_id2;
	reg.bits.clear_active_id3 = clear_active_id3;
	reg.bits.clear_active_id4 = clear_active_id4;
	reg.bits.clear_active_id5 = clear_active_id5;
	reg.bits.clear_active_id6 = clear_active_id6;
	reg.bits.clear_active_id7 = clear_active_id7;
	reg.bits.clear_active_id8 = clear_active_id8;
	reg.bits.clear_active_id9 = clear_active_id9;
	reg.bits.clear_active_id10 = clear_active_id10;
	reg.bits.clear_active_id11 = clear_active_id11;
	reg.bits.clear_active_id12 = clear_active_id12;
	reg.bits.clear_active_id13 = clear_active_id13;
	reg.bits.clear_active_id14 = clear_active_id14;
	reg.bits.clear_active_id15 = clear_active_id15;
	reg.bits.clear_active_id16 = clear_active_id16;
	reg.bits.clear_active_id17 = clear_active_id17;
	reg.bits.clear_active_id18 = clear_active_id18;
	reg.bits.clear_active_id19 = clear_active_id19;
	reg.bits.clear_active_id20 = clear_active_id20;
	reg.bits.clear_active_id21 = clear_active_id21;
	reg.bits.clear_active_id22 = clear_active_id22;
	reg.bits.clear_active_id23 = clear_active_id23;
	reg.bits.clear_active_id24 = clear_active_id24;
	reg.bits.clear_active_id25 = clear_active_id25;
	reg.bits.clear_active_id26 = clear_active_id26;
	reg.bits.clear_active_id27 = clear_active_id27;
	reg.bits.clear_active_id28 = clear_active_id28;
	reg.bits.clear_active_id29 = clear_active_id29;
	reg.bits.clear_active_id30 = clear_active_id30;
	reg.bits.clear_active_id31 = clear_active_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID0
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID1
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID2
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID3
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID4
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID5
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID6
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID7
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID8
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID9
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID10
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID11
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID12
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID13
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID14
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID15
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID16
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID17
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID18
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID19
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID20
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID21
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID22
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID23
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID24
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID25
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID26
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID27
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID28
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID29
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID30
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER0_CLEAR_ACTIVE_ID31
fun_set_mod_reg_bit(gicd, icactiver0, clear_active_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1
void set_gicd_icactiver1(u32 reg_addr,
			u32 clear_active_id32,
			u32 clear_active_id33,
			u32 clear_active_id34,
			u32 clear_active_id35,
			u32 clear_active_id36,
			u32 clear_active_id37,
			u32 clear_active_id38,
			u32 clear_active_id39,
			u32 clear_active_id40,
			u32 clear_active_id41,
			u32 clear_active_id42,
			u32 clear_active_id43,
			u32 clear_active_id44,
			u32 clear_active_id45,
			u32 clear_active_id46,
			u32 clear_active_id47,
			u32 clear_active_id48,
			u32 clear_active_id49,
			u32 clear_active_id50,
			u32 clear_active_id51,
			u32 clear_active_id52,
			u32 clear_active_id53,
			u32 clear_active_id54,
			u32 clear_active_id55,
			u32 clear_active_id56,
			u32 clear_active_id57,
			u32 clear_active_id58,
			u32 clear_active_id59,
			u32 clear_active_id60,
			u32 clear_active_id61,
			u32 clear_active_id62,
			u32 clear_active_id63,
			u32 m_or_r)
{
	reg_gicd_icactiver1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clear_active_id32 = clear_active_id32;
	reg.bits.clear_active_id33 = clear_active_id33;
	reg.bits.clear_active_id34 = clear_active_id34;
	reg.bits.clear_active_id35 = clear_active_id35;
	reg.bits.clear_active_id36 = clear_active_id36;
	reg.bits.clear_active_id37 = clear_active_id37;
	reg.bits.clear_active_id38 = clear_active_id38;
	reg.bits.clear_active_id39 = clear_active_id39;
	reg.bits.clear_active_id40 = clear_active_id40;
	reg.bits.clear_active_id41 = clear_active_id41;
	reg.bits.clear_active_id42 = clear_active_id42;
	reg.bits.clear_active_id43 = clear_active_id43;
	reg.bits.clear_active_id44 = clear_active_id44;
	reg.bits.clear_active_id45 = clear_active_id45;
	reg.bits.clear_active_id46 = clear_active_id46;
	reg.bits.clear_active_id47 = clear_active_id47;
	reg.bits.clear_active_id48 = clear_active_id48;
	reg.bits.clear_active_id49 = clear_active_id49;
	reg.bits.clear_active_id50 = clear_active_id50;
	reg.bits.clear_active_id51 = clear_active_id51;
	reg.bits.clear_active_id52 = clear_active_id52;
	reg.bits.clear_active_id53 = clear_active_id53;
	reg.bits.clear_active_id54 = clear_active_id54;
	reg.bits.clear_active_id55 = clear_active_id55;
	reg.bits.clear_active_id56 = clear_active_id56;
	reg.bits.clear_active_id57 = clear_active_id57;
	reg.bits.clear_active_id58 = clear_active_id58;
	reg.bits.clear_active_id59 = clear_active_id59;
	reg.bits.clear_active_id60 = clear_active_id60;
	reg.bits.clear_active_id61 = clear_active_id61;
	reg.bits.clear_active_id62 = clear_active_id62;
	reg.bits.clear_active_id63 = clear_active_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID32
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID33
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID34
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID35
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID36
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID37
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID38
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID39
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID40
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID41
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID42
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID43
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID44
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID45
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID46
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID47
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID48
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID49
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID50
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID51
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID52
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID53
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID54
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID55
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID56
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID57
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID58
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID59
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID60
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID61
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID62
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER1_CLEAR_ACTIVE_ID63
fun_set_mod_reg_bit(gicd, icactiver1, clear_active_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2
void set_gicd_icactiver2(u32 reg_addr,
			u32 clear_active_id64,
			u32 clear_active_id65,
			u32 clear_active_id66,
			u32 clear_active_id67,
			u32 clear_active_id68,
			u32 clear_active_id69,
			u32 clear_active_id70,
			u32 clear_active_id71,
			u32 clear_active_id72,
			u32 clear_active_id73,
			u32 clear_active_id74,
			u32 clear_active_id75,
			u32 clear_active_id76,
			u32 clear_active_id77,
			u32 clear_active_id78,
			u32 clear_active_id79,
			u32 clear_active_id80,
			u32 clear_active_id81,
			u32 clear_active_id82,
			u32 clear_active_id83,
			u32 clear_active_id84,
			u32 clear_active_id85,
			u32 clear_active_id86,
			u32 clear_active_id87,
			u32 clear_active_id88,
			u32 clear_active_id89,
			u32 clear_active_id90,
			u32 clear_active_id91,
			u32 clear_active_id92,
			u32 clear_active_id93,
			u32 clear_active_id94,
			u32 clear_active_id95,
			u32 m_or_r)
{
	reg_gicd_icactiver2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clear_active_id64 = clear_active_id64;
	reg.bits.clear_active_id65 = clear_active_id65;
	reg.bits.clear_active_id66 = clear_active_id66;
	reg.bits.clear_active_id67 = clear_active_id67;
	reg.bits.clear_active_id68 = clear_active_id68;
	reg.bits.clear_active_id69 = clear_active_id69;
	reg.bits.clear_active_id70 = clear_active_id70;
	reg.bits.clear_active_id71 = clear_active_id71;
	reg.bits.clear_active_id72 = clear_active_id72;
	reg.bits.clear_active_id73 = clear_active_id73;
	reg.bits.clear_active_id74 = clear_active_id74;
	reg.bits.clear_active_id75 = clear_active_id75;
	reg.bits.clear_active_id76 = clear_active_id76;
	reg.bits.clear_active_id77 = clear_active_id77;
	reg.bits.clear_active_id78 = clear_active_id78;
	reg.bits.clear_active_id79 = clear_active_id79;
	reg.bits.clear_active_id80 = clear_active_id80;
	reg.bits.clear_active_id81 = clear_active_id81;
	reg.bits.clear_active_id82 = clear_active_id82;
	reg.bits.clear_active_id83 = clear_active_id83;
	reg.bits.clear_active_id84 = clear_active_id84;
	reg.bits.clear_active_id85 = clear_active_id85;
	reg.bits.clear_active_id86 = clear_active_id86;
	reg.bits.clear_active_id87 = clear_active_id87;
	reg.bits.clear_active_id88 = clear_active_id88;
	reg.bits.clear_active_id89 = clear_active_id89;
	reg.bits.clear_active_id90 = clear_active_id90;
	reg.bits.clear_active_id91 = clear_active_id91;
	reg.bits.clear_active_id92 = clear_active_id92;
	reg.bits.clear_active_id93 = clear_active_id93;
	reg.bits.clear_active_id94 = clear_active_id94;
	reg.bits.clear_active_id95 = clear_active_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID64
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID65
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID66
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID67
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID68
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID69
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID70
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID71
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID72
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID73
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID74
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID75
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID76
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID77
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID78
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID79
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID80
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID81
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID82
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID83
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID84
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID85
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID86
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID87
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID88
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID89
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID90
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID91
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID92
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID93
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID94
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER2_CLEAR_ACTIVE_ID95
fun_set_mod_reg_bit(gicd, icactiver2, clear_active_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3
void set_gicd_icactiver3(u32 reg_addr,
			u32 clear_active_id96,
			u32 clear_active_id97,
			u32 clear_active_id98,
			u32 clear_active_id99,
			u32 clear_active_id100,
			u32 clear_active_id101,
			u32 clear_active_id102,
			u32 clear_active_id103,
			u32 clear_active_id104,
			u32 clear_active_id105,
			u32 clear_active_id106,
			u32 clear_active_id107,
			u32 clear_active_id108,
			u32 clear_active_id109,
			u32 clear_active_id110,
			u32 clear_active_id111,
			u32 clear_active_id112,
			u32 clear_active_id113,
			u32 clear_active_id114,
			u32 clear_active_id115,
			u32 clear_active_id116,
			u32 clear_active_id117,
			u32 clear_active_id118,
			u32 clear_active_id119,
			u32 clear_active_id120,
			u32 clear_active_id121,
			u32 clear_active_id122,
			u32 clear_active_id123,
			u32 clear_active_id124,
			u32 clear_active_id125,
			u32 clear_active_id126,
			u32 clear_active_id127,
			u32 m_or_r)
{
	reg_gicd_icactiver3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clear_active_id96 = clear_active_id96;
	reg.bits.clear_active_id97 = clear_active_id97;
	reg.bits.clear_active_id98 = clear_active_id98;
	reg.bits.clear_active_id99 = clear_active_id99;
	reg.bits.clear_active_id100 = clear_active_id100;
	reg.bits.clear_active_id101 = clear_active_id101;
	reg.bits.clear_active_id102 = clear_active_id102;
	reg.bits.clear_active_id103 = clear_active_id103;
	reg.bits.clear_active_id104 = clear_active_id104;
	reg.bits.clear_active_id105 = clear_active_id105;
	reg.bits.clear_active_id106 = clear_active_id106;
	reg.bits.clear_active_id107 = clear_active_id107;
	reg.bits.clear_active_id108 = clear_active_id108;
	reg.bits.clear_active_id109 = clear_active_id109;
	reg.bits.clear_active_id110 = clear_active_id110;
	reg.bits.clear_active_id111 = clear_active_id111;
	reg.bits.clear_active_id112 = clear_active_id112;
	reg.bits.clear_active_id113 = clear_active_id113;
	reg.bits.clear_active_id114 = clear_active_id114;
	reg.bits.clear_active_id115 = clear_active_id115;
	reg.bits.clear_active_id116 = clear_active_id116;
	reg.bits.clear_active_id117 = clear_active_id117;
	reg.bits.clear_active_id118 = clear_active_id118;
	reg.bits.clear_active_id119 = clear_active_id119;
	reg.bits.clear_active_id120 = clear_active_id120;
	reg.bits.clear_active_id121 = clear_active_id121;
	reg.bits.clear_active_id122 = clear_active_id122;
	reg.bits.clear_active_id123 = clear_active_id123;
	reg.bits.clear_active_id124 = clear_active_id124;
	reg.bits.clear_active_id125 = clear_active_id125;
	reg.bits.clear_active_id126 = clear_active_id126;
	reg.bits.clear_active_id127 = clear_active_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID96
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID97
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID98
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID99
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID100
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID101
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID102
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID103
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID104
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID105
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID106
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID107
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID108
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID109
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID110
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID111
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID112
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID113
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID114
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID115
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID116
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID117
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID118
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID119
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID120
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID121
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID122
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID123
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID124
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID125
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID126
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER3_CLEAR_ACTIVE_ID127
fun_set_mod_reg_bit(gicd, icactiver3, clear_active_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4
void set_gicd_icactiver4(u32 reg_addr,
			u32 clear_active_id128,
			u32 clear_active_id129,
			u32 clear_active_id130,
			u32 clear_active_id131,
			u32 clear_active_id132,
			u32 clear_active_id133,
			u32 clear_active_id134,
			u32 clear_active_id135,
			u32 clear_active_id136,
			u32 clear_active_id137,
			u32 clear_active_id138,
			u32 clear_active_id139,
			u32 clear_active_id140,
			u32 clear_active_id141,
			u32 clear_active_id142,
			u32 clear_active_id143,
			u32 clear_active_id144,
			u32 clear_active_id145,
			u32 clear_active_id146,
			u32 clear_active_id147,
			u32 clear_active_id148,
			u32 clear_active_id149,
			u32 clear_active_id150,
			u32 clear_active_id151,
			u32 clear_active_id152,
			u32 clear_active_id153,
			u32 clear_active_id154,
			u32 clear_active_id155,
			u32 clear_active_id156,
			u32 clear_active_id157,
			u32 clear_active_id158,
			u32 clear_active_id159,
			u32 m_or_r)
{
	reg_gicd_icactiver4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clear_active_id128 = clear_active_id128;
	reg.bits.clear_active_id129 = clear_active_id129;
	reg.bits.clear_active_id130 = clear_active_id130;
	reg.bits.clear_active_id131 = clear_active_id131;
	reg.bits.clear_active_id132 = clear_active_id132;
	reg.bits.clear_active_id133 = clear_active_id133;
	reg.bits.clear_active_id134 = clear_active_id134;
	reg.bits.clear_active_id135 = clear_active_id135;
	reg.bits.clear_active_id136 = clear_active_id136;
	reg.bits.clear_active_id137 = clear_active_id137;
	reg.bits.clear_active_id138 = clear_active_id138;
	reg.bits.clear_active_id139 = clear_active_id139;
	reg.bits.clear_active_id140 = clear_active_id140;
	reg.bits.clear_active_id141 = clear_active_id141;
	reg.bits.clear_active_id142 = clear_active_id142;
	reg.bits.clear_active_id143 = clear_active_id143;
	reg.bits.clear_active_id144 = clear_active_id144;
	reg.bits.clear_active_id145 = clear_active_id145;
	reg.bits.clear_active_id146 = clear_active_id146;
	reg.bits.clear_active_id147 = clear_active_id147;
	reg.bits.clear_active_id148 = clear_active_id148;
	reg.bits.clear_active_id149 = clear_active_id149;
	reg.bits.clear_active_id150 = clear_active_id150;
	reg.bits.clear_active_id151 = clear_active_id151;
	reg.bits.clear_active_id152 = clear_active_id152;
	reg.bits.clear_active_id153 = clear_active_id153;
	reg.bits.clear_active_id154 = clear_active_id154;
	reg.bits.clear_active_id155 = clear_active_id155;
	reg.bits.clear_active_id156 = clear_active_id156;
	reg.bits.clear_active_id157 = clear_active_id157;
	reg.bits.clear_active_id158 = clear_active_id158;
	reg.bits.clear_active_id159 = clear_active_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID128
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID129
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID130
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID131
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID132
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID133
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID134
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID135
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID136
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID137
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID138
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID139
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID140
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID141
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID142
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID143
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID144
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID145
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID146
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID147
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID148
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID149
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID150
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID151
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID152
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID153
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID154
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID155
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID156
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID157
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID158
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER4_CLEAR_ACTIVE_ID159
fun_set_mod_reg_bit(gicd, icactiver4, clear_active_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5
void set_gicd_icactiver5(u32 reg_addr,
			u32 clear_active_id160,
			u32 clear_active_id161,
			u32 clear_active_id162,
			u32 clear_active_id163,
			u32 clear_active_id164,
			u32 clear_active_id165,
			u32 clear_active_id166,
			u32 clear_active_id167,
			u32 clear_active_id168,
			u32 clear_active_id169,
			u32 clear_active_id170,
			u32 clear_active_id171,
			u32 clear_active_id172,
			u32 clear_active_id173,
			u32 clear_active_id174,
			u32 clear_active_id175,
			u32 clear_active_id176,
			u32 clear_active_id177,
			u32 clear_active_id178,
			u32 clear_active_id179,
			u32 clear_active_id180,
			u32 clear_active_id181,
			u32 clear_active_id182,
			u32 clear_active_id183,
			u32 clear_active_id184,
			u32 clear_active_id185,
			u32 clear_active_id186,
			u32 clear_active_id187,
			u32 clear_active_id188,
			u32 clear_active_id189,
			u32 clear_active_id190,
			u32 clear_active_id191,
			u32 m_or_r)
{
	reg_gicd_icactiver5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clear_active_id160 = clear_active_id160;
	reg.bits.clear_active_id161 = clear_active_id161;
	reg.bits.clear_active_id162 = clear_active_id162;
	reg.bits.clear_active_id163 = clear_active_id163;
	reg.bits.clear_active_id164 = clear_active_id164;
	reg.bits.clear_active_id165 = clear_active_id165;
	reg.bits.clear_active_id166 = clear_active_id166;
	reg.bits.clear_active_id167 = clear_active_id167;
	reg.bits.clear_active_id168 = clear_active_id168;
	reg.bits.clear_active_id169 = clear_active_id169;
	reg.bits.clear_active_id170 = clear_active_id170;
	reg.bits.clear_active_id171 = clear_active_id171;
	reg.bits.clear_active_id172 = clear_active_id172;
	reg.bits.clear_active_id173 = clear_active_id173;
	reg.bits.clear_active_id174 = clear_active_id174;
	reg.bits.clear_active_id175 = clear_active_id175;
	reg.bits.clear_active_id176 = clear_active_id176;
	reg.bits.clear_active_id177 = clear_active_id177;
	reg.bits.clear_active_id178 = clear_active_id178;
	reg.bits.clear_active_id179 = clear_active_id179;
	reg.bits.clear_active_id180 = clear_active_id180;
	reg.bits.clear_active_id181 = clear_active_id181;
	reg.bits.clear_active_id182 = clear_active_id182;
	reg.bits.clear_active_id183 = clear_active_id183;
	reg.bits.clear_active_id184 = clear_active_id184;
	reg.bits.clear_active_id185 = clear_active_id185;
	reg.bits.clear_active_id186 = clear_active_id186;
	reg.bits.clear_active_id187 = clear_active_id187;
	reg.bits.clear_active_id188 = clear_active_id188;
	reg.bits.clear_active_id189 = clear_active_id189;
	reg.bits.clear_active_id190 = clear_active_id190;
	reg.bits.clear_active_id191 = clear_active_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID160
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID161
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID162
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID163
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID164
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID165
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID166
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID167
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID168
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID169
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID170
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID171
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID172
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID173
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID174
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID175
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID176
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID177
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID178
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID179
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID180
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID181
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID182
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID183
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID184
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID185
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID186
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID187
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID188
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID189
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID190
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER5_CLEAR_ACTIVE_ID191
fun_set_mod_reg_bit(gicd, icactiver5, clear_active_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6
void set_gicd_icactiver6(u32 reg_addr,
			u32 clear_active_id192,
			u32 clear_active_id193,
			u32 clear_active_id194,
			u32 clear_active_id195,
			u32 clear_active_id196,
			u32 clear_active_id197,
			u32 clear_active_id198,
			u32 clear_active_id199,
			u32 clear_active_id200,
			u32 clear_active_id201,
			u32 clear_active_id202,
			u32 clear_active_id203,
			u32 clear_active_id204,
			u32 clear_active_id205,
			u32 clear_active_id206,
			u32 clear_active_id207,
			u32 clear_active_id208,
			u32 clear_active_id209,
			u32 clear_active_id210,
			u32 clear_active_id211,
			u32 clear_active_id212,
			u32 clear_active_id213,
			u32 clear_active_id214,
			u32 clear_active_id215,
			u32 clear_active_id216,
			u32 clear_active_id217,
			u32 clear_active_id218,
			u32 clear_active_id219,
			u32 clear_active_id220,
			u32 clear_active_id221,
			u32 clear_active_id222,
			u32 clear_active_id223,
			u32 m_or_r)
{
	reg_gicd_icactiver6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clear_active_id192 = clear_active_id192;
	reg.bits.clear_active_id193 = clear_active_id193;
	reg.bits.clear_active_id194 = clear_active_id194;
	reg.bits.clear_active_id195 = clear_active_id195;
	reg.bits.clear_active_id196 = clear_active_id196;
	reg.bits.clear_active_id197 = clear_active_id197;
	reg.bits.clear_active_id198 = clear_active_id198;
	reg.bits.clear_active_id199 = clear_active_id199;
	reg.bits.clear_active_id200 = clear_active_id200;
	reg.bits.clear_active_id201 = clear_active_id201;
	reg.bits.clear_active_id202 = clear_active_id202;
	reg.bits.clear_active_id203 = clear_active_id203;
	reg.bits.clear_active_id204 = clear_active_id204;
	reg.bits.clear_active_id205 = clear_active_id205;
	reg.bits.clear_active_id206 = clear_active_id206;
	reg.bits.clear_active_id207 = clear_active_id207;
	reg.bits.clear_active_id208 = clear_active_id208;
	reg.bits.clear_active_id209 = clear_active_id209;
	reg.bits.clear_active_id210 = clear_active_id210;
	reg.bits.clear_active_id211 = clear_active_id211;
	reg.bits.clear_active_id212 = clear_active_id212;
	reg.bits.clear_active_id213 = clear_active_id213;
	reg.bits.clear_active_id214 = clear_active_id214;
	reg.bits.clear_active_id215 = clear_active_id215;
	reg.bits.clear_active_id216 = clear_active_id216;
	reg.bits.clear_active_id217 = clear_active_id217;
	reg.bits.clear_active_id218 = clear_active_id218;
	reg.bits.clear_active_id219 = clear_active_id219;
	reg.bits.clear_active_id220 = clear_active_id220;
	reg.bits.clear_active_id221 = clear_active_id221;
	reg.bits.clear_active_id222 = clear_active_id222;
	reg.bits.clear_active_id223 = clear_active_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID192
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID193
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID194
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID195
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID196
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID197
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID198
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID199
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID200
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID201
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID202
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID203
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID204
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID205
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID206
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID207
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID208
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID209
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID210
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID211
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID212
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID213
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID214
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID215
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID216
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID217
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID218
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID219
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID220
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID221
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID222
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICACTIVER6_CLEAR_ACTIVE_ID223
fun_set_mod_reg_bit(gicd, icactiver6, clear_active_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR0
void set_gicd_ipriorityr0(u32 reg_addr,
			u32 ipriority_id0,
			u32 ipriority_id1,
			u32 ipriority_id2,
			u32 ipriority_id3,
			u32 m_or_r)
{
	reg_gicd_ipriorityr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id0 = ipriority_id0;
	reg.bits.ipriority_id1 = ipriority_id1;
	reg.bits.ipriority_id2 = ipriority_id2;
	reg.bits.ipriority_id3 = ipriority_id3;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR0_IPRIORITY_ID0
fun_set_mod_reg_bit(gicd, ipriorityr0, ipriority_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR0_IPRIORITY_ID1
fun_set_mod_reg_bit(gicd, ipriorityr0, ipriority_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR0_IPRIORITY_ID2
fun_set_mod_reg_bit(gicd, ipriorityr0, ipriority_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR0_IPRIORITY_ID3
fun_set_mod_reg_bit(gicd, ipriorityr0, ipriority_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR1
void set_gicd_ipriorityr1(u32 reg_addr,
			u32 ipriority_id4,
			u32 ipriority_id5,
			u32 ipriority_id6,
			u32 ipriority_id7,
			u32 m_or_r)
{
	reg_gicd_ipriorityr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id4 = ipriority_id4;
	reg.bits.ipriority_id5 = ipriority_id5;
	reg.bits.ipriority_id6 = ipriority_id6;
	reg.bits.ipriority_id7 = ipriority_id7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR1_IPRIORITY_ID4
fun_set_mod_reg_bit(gicd, ipriorityr1, ipriority_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR1_IPRIORITY_ID5
fun_set_mod_reg_bit(gicd, ipriorityr1, ipriority_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR1_IPRIORITY_ID6
fun_set_mod_reg_bit(gicd, ipriorityr1, ipriority_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR1_IPRIORITY_ID7
fun_set_mod_reg_bit(gicd, ipriorityr1, ipriority_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR2
void set_gicd_ipriorityr2(u32 reg_addr,
			u32 ipriority_id8,
			u32 ipriority_id9,
			u32 ipriority_id10,
			u32 ipriority_id11,
			u32 m_or_r)
{
	reg_gicd_ipriorityr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id8 = ipriority_id8;
	reg.bits.ipriority_id9 = ipriority_id9;
	reg.bits.ipriority_id10 = ipriority_id10;
	reg.bits.ipriority_id11 = ipriority_id11;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR2_IPRIORITY_ID8
fun_set_mod_reg_bit(gicd, ipriorityr2, ipriority_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR2_IPRIORITY_ID9
fun_set_mod_reg_bit(gicd, ipriorityr2, ipriority_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR2_IPRIORITY_ID10
fun_set_mod_reg_bit(gicd, ipriorityr2, ipriority_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR2_IPRIORITY_ID11
fun_set_mod_reg_bit(gicd, ipriorityr2, ipriority_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR3
void set_gicd_ipriorityr3(u32 reg_addr,
			u32 ipriority_id12,
			u32 ipriority_id13,
			u32 ipriority_id14,
			u32 ipriority_id15,
			u32 m_or_r)
{
	reg_gicd_ipriorityr3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id12 = ipriority_id12;
	reg.bits.ipriority_id13 = ipriority_id13;
	reg.bits.ipriority_id14 = ipriority_id14;
	reg.bits.ipriority_id15 = ipriority_id15;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR3_IPRIORITY_ID12
fun_set_mod_reg_bit(gicd, ipriorityr3, ipriority_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR3_IPRIORITY_ID13
fun_set_mod_reg_bit(gicd, ipriorityr3, ipriority_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR3_IPRIORITY_ID14
fun_set_mod_reg_bit(gicd, ipriorityr3, ipriority_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR3_IPRIORITY_ID15
fun_set_mod_reg_bit(gicd, ipriorityr3, ipriority_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR4
void set_gicd_ipriorityr4(u32 reg_addr,
			u32 ipriority_id16,
			u32 ipriority_id17,
			u32 ipriority_id18,
			u32 ipriority_id19,
			u32 m_or_r)
{
	reg_gicd_ipriorityr4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id16 = ipriority_id16;
	reg.bits.ipriority_id17 = ipriority_id17;
	reg.bits.ipriority_id18 = ipriority_id18;
	reg.bits.ipriority_id19 = ipriority_id19;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR4_IPRIORITY_ID16
fun_set_mod_reg_bit(gicd, ipriorityr4, ipriority_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR4_IPRIORITY_ID17
fun_set_mod_reg_bit(gicd, ipriorityr4, ipriority_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR4_IPRIORITY_ID18
fun_set_mod_reg_bit(gicd, ipriorityr4, ipriority_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR4_IPRIORITY_ID19
fun_set_mod_reg_bit(gicd, ipriorityr4, ipriority_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR5
void set_gicd_ipriorityr5(u32 reg_addr,
			u32 ipriority_id20,
			u32 ipriority_id21,
			u32 ipriority_id22,
			u32 ipriority_id23,
			u32 m_or_r)
{
	reg_gicd_ipriorityr5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id20 = ipriority_id20;
	reg.bits.ipriority_id21 = ipriority_id21;
	reg.bits.ipriority_id22 = ipriority_id22;
	reg.bits.ipriority_id23 = ipriority_id23;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR5_IPRIORITY_ID20
fun_set_mod_reg_bit(gicd, ipriorityr5, ipriority_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR5_IPRIORITY_ID21
fun_set_mod_reg_bit(gicd, ipriorityr5, ipriority_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR5_IPRIORITY_ID22
fun_set_mod_reg_bit(gicd, ipriorityr5, ipriority_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR5_IPRIORITY_ID23
fun_set_mod_reg_bit(gicd, ipriorityr5, ipriority_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR6
void set_gicd_ipriorityr6(u32 reg_addr,
			u32 ipriority_id24,
			u32 ipriority_id25,
			u32 ipriority_id26,
			u32 ipriority_id27,
			u32 m_or_r)
{
	reg_gicd_ipriorityr6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id24 = ipriority_id24;
	reg.bits.ipriority_id25 = ipriority_id25;
	reg.bits.ipriority_id26 = ipriority_id26;
	reg.bits.ipriority_id27 = ipriority_id27;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR6_IPRIORITY_ID24
fun_set_mod_reg_bit(gicd, ipriorityr6, ipriority_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR6_IPRIORITY_ID25
fun_set_mod_reg_bit(gicd, ipriorityr6, ipriority_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR6_IPRIORITY_ID26
fun_set_mod_reg_bit(gicd, ipriorityr6, ipriority_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR6_IPRIORITY_ID27
fun_set_mod_reg_bit(gicd, ipriorityr6, ipriority_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR7
void set_gicd_ipriorityr7(u32 reg_addr,
			u32 ipriority_id28,
			u32 ipriority_id29,
			u32 ipriority_id30,
			u32 ipriority_id31,
			u32 m_or_r)
{
	reg_gicd_ipriorityr7_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id28 = ipriority_id28;
	reg.bits.ipriority_id29 = ipriority_id29;
	reg.bits.ipriority_id30 = ipriority_id30;
	reg.bits.ipriority_id31 = ipriority_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR7_IPRIORITY_ID28
fun_set_mod_reg_bit(gicd, ipriorityr7, ipriority_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR7_IPRIORITY_ID29
fun_set_mod_reg_bit(gicd, ipriorityr7, ipriority_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR7_IPRIORITY_ID30
fun_set_mod_reg_bit(gicd, ipriorityr7, ipriority_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR7_IPRIORITY_ID31
fun_set_mod_reg_bit(gicd, ipriorityr7, ipriority_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR8
void set_gicd_ipriorityr8(u32 reg_addr,
			u32 ipriority_id32,
			u32 ipriority_id33,
			u32 ipriority_id34,
			u32 ipriority_id35,
			u32 m_or_r)
{
	reg_gicd_ipriorityr8_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id32 = ipriority_id32;
	reg.bits.ipriority_id33 = ipriority_id33;
	reg.bits.ipriority_id34 = ipriority_id34;
	reg.bits.ipriority_id35 = ipriority_id35;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR8_IPRIORITY_ID32
fun_set_mod_reg_bit(gicd, ipriorityr8, ipriority_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR8_IPRIORITY_ID33
fun_set_mod_reg_bit(gicd, ipriorityr8, ipriority_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR8_IPRIORITY_ID34
fun_set_mod_reg_bit(gicd, ipriorityr8, ipriority_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR8_IPRIORITY_ID35
fun_set_mod_reg_bit(gicd, ipriorityr8, ipriority_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR9
void set_gicd_ipriorityr9(u32 reg_addr,
			u32 ipriority_id36,
			u32 ipriority_id37,
			u32 ipriority_id38,
			u32 ipriority_id39,
			u32 m_or_r)
{
	reg_gicd_ipriorityr9_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id36 = ipriority_id36;
	reg.bits.ipriority_id37 = ipriority_id37;
	reg.bits.ipriority_id38 = ipriority_id38;
	reg.bits.ipriority_id39 = ipriority_id39;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR9_IPRIORITY_ID36
fun_set_mod_reg_bit(gicd, ipriorityr9, ipriority_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR9_IPRIORITY_ID37
fun_set_mod_reg_bit(gicd, ipriorityr9, ipriority_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR9_IPRIORITY_ID38
fun_set_mod_reg_bit(gicd, ipriorityr9, ipriority_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR9_IPRIORITY_ID39
fun_set_mod_reg_bit(gicd, ipriorityr9, ipriority_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR10
void set_gicd_ipriorityr10(u32 reg_addr,
			u32 ipriority_id40,
			u32 ipriority_id41,
			u32 ipriority_id42,
			u32 ipriority_id43,
			u32 m_or_r)
{
	reg_gicd_ipriorityr10_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id40 = ipriority_id40;
	reg.bits.ipriority_id41 = ipriority_id41;
	reg.bits.ipriority_id42 = ipriority_id42;
	reg.bits.ipriority_id43 = ipriority_id43;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR10_IPRIORITY_ID40
fun_set_mod_reg_bit(gicd, ipriorityr10, ipriority_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR10_IPRIORITY_ID41
fun_set_mod_reg_bit(gicd, ipriorityr10, ipriority_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR10_IPRIORITY_ID42
fun_set_mod_reg_bit(gicd, ipriorityr10, ipriority_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR10_IPRIORITY_ID43
fun_set_mod_reg_bit(gicd, ipriorityr10, ipriority_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR11
void set_gicd_ipriorityr11(u32 reg_addr,
			u32 ipriority_id44,
			u32 ipriority_id45,
			u32 ipriority_id46,
			u32 ipriority_id47,
			u32 m_or_r)
{
	reg_gicd_ipriorityr11_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id44 = ipriority_id44;
	reg.bits.ipriority_id45 = ipriority_id45;
	reg.bits.ipriority_id46 = ipriority_id46;
	reg.bits.ipriority_id47 = ipriority_id47;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR11_IPRIORITY_ID44
fun_set_mod_reg_bit(gicd, ipriorityr11, ipriority_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR11_IPRIORITY_ID45
fun_set_mod_reg_bit(gicd, ipriorityr11, ipriority_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR11_IPRIORITY_ID46
fun_set_mod_reg_bit(gicd, ipriorityr11, ipriority_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR11_IPRIORITY_ID47
fun_set_mod_reg_bit(gicd, ipriorityr11, ipriority_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR12
void set_gicd_ipriorityr12(u32 reg_addr,
			u32 ipriority_id48,
			u32 ipriority_id49,
			u32 ipriority_id50,
			u32 ipriority_id51,
			u32 m_or_r)
{
	reg_gicd_ipriorityr12_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id48 = ipriority_id48;
	reg.bits.ipriority_id49 = ipriority_id49;
	reg.bits.ipriority_id50 = ipriority_id50;
	reg.bits.ipriority_id51 = ipriority_id51;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR12_IPRIORITY_ID48
fun_set_mod_reg_bit(gicd, ipriorityr12, ipriority_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR12_IPRIORITY_ID49
fun_set_mod_reg_bit(gicd, ipriorityr12, ipriority_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR12_IPRIORITY_ID50
fun_set_mod_reg_bit(gicd, ipriorityr12, ipriority_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR12_IPRIORITY_ID51
fun_set_mod_reg_bit(gicd, ipriorityr12, ipriority_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR13
void set_gicd_ipriorityr13(u32 reg_addr,
			u32 ipriority_id52,
			u32 ipriority_id53,
			u32 ipriority_id54,
			u32 ipriority_id55,
			u32 m_or_r)
{
	reg_gicd_ipriorityr13_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id52 = ipriority_id52;
	reg.bits.ipriority_id53 = ipriority_id53;
	reg.bits.ipriority_id54 = ipriority_id54;
	reg.bits.ipriority_id55 = ipriority_id55;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR13_IPRIORITY_ID52
fun_set_mod_reg_bit(gicd, ipriorityr13, ipriority_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR13_IPRIORITY_ID53
fun_set_mod_reg_bit(gicd, ipriorityr13, ipriority_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR13_IPRIORITY_ID54
fun_set_mod_reg_bit(gicd, ipriorityr13, ipriority_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR13_IPRIORITY_ID55
fun_set_mod_reg_bit(gicd, ipriorityr13, ipriority_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR14
void set_gicd_ipriorityr14(u32 reg_addr,
			u32 ipriority_id56,
			u32 ipriority_id57,
			u32 ipriority_id58,
			u32 ipriority_id59,
			u32 m_or_r)
{
	reg_gicd_ipriorityr14_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id56 = ipriority_id56;
	reg.bits.ipriority_id57 = ipriority_id57;
	reg.bits.ipriority_id58 = ipriority_id58;
	reg.bits.ipriority_id59 = ipriority_id59;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR14_IPRIORITY_ID56
fun_set_mod_reg_bit(gicd, ipriorityr14, ipriority_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR14_IPRIORITY_ID57
fun_set_mod_reg_bit(gicd, ipriorityr14, ipriority_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR14_IPRIORITY_ID58
fun_set_mod_reg_bit(gicd, ipriorityr14, ipriority_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR14_IPRIORITY_ID59
fun_set_mod_reg_bit(gicd, ipriorityr14, ipriority_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR15
void set_gicd_ipriorityr15(u32 reg_addr,
			u32 ipriority_id60,
			u32 ipriority_id61,
			u32 ipriority_id62,
			u32 ipriority_id63,
			u32 m_or_r)
{
	reg_gicd_ipriorityr15_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id60 = ipriority_id60;
	reg.bits.ipriority_id61 = ipriority_id61;
	reg.bits.ipriority_id62 = ipriority_id62;
	reg.bits.ipriority_id63 = ipriority_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR15_IPRIORITY_ID60
fun_set_mod_reg_bit(gicd, ipriorityr15, ipriority_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR15_IPRIORITY_ID61
fun_set_mod_reg_bit(gicd, ipriorityr15, ipriority_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR15_IPRIORITY_ID62
fun_set_mod_reg_bit(gicd, ipriorityr15, ipriority_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR15_IPRIORITY_ID63
fun_set_mod_reg_bit(gicd, ipriorityr15, ipriority_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR16
void set_gicd_ipriorityr16(u32 reg_addr,
			u32 ipriority_id64,
			u32 ipriority_id65,
			u32 ipriority_id66,
			u32 ipriority_id67,
			u32 m_or_r)
{
	reg_gicd_ipriorityr16_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id64 = ipriority_id64;
	reg.bits.ipriority_id65 = ipriority_id65;
	reg.bits.ipriority_id66 = ipriority_id66;
	reg.bits.ipriority_id67 = ipriority_id67;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR16_IPRIORITY_ID64
fun_set_mod_reg_bit(gicd, ipriorityr16, ipriority_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR16_IPRIORITY_ID65
fun_set_mod_reg_bit(gicd, ipriorityr16, ipriority_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR16_IPRIORITY_ID66
fun_set_mod_reg_bit(gicd, ipriorityr16, ipriority_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR16_IPRIORITY_ID67
fun_set_mod_reg_bit(gicd, ipriorityr16, ipriority_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR17
void set_gicd_ipriorityr17(u32 reg_addr,
			u32 ipriority_id68,
			u32 ipriority_id69,
			u32 ipriority_id70,
			u32 ipriority_id71,
			u32 m_or_r)
{
	reg_gicd_ipriorityr17_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id68 = ipriority_id68;
	reg.bits.ipriority_id69 = ipriority_id69;
	reg.bits.ipriority_id70 = ipriority_id70;
	reg.bits.ipriority_id71 = ipriority_id71;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR17_IPRIORITY_ID68
fun_set_mod_reg_bit(gicd, ipriorityr17, ipriority_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR17_IPRIORITY_ID69
fun_set_mod_reg_bit(gicd, ipriorityr17, ipriority_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR17_IPRIORITY_ID70
fun_set_mod_reg_bit(gicd, ipriorityr17, ipriority_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR17_IPRIORITY_ID71
fun_set_mod_reg_bit(gicd, ipriorityr17, ipriority_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR18
void set_gicd_ipriorityr18(u32 reg_addr,
			u32 ipriority_id72,
			u32 ipriority_id73,
			u32 ipriority_id74,
			u32 ipriority_id75,
			u32 m_or_r)
{
	reg_gicd_ipriorityr18_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id72 = ipriority_id72;
	reg.bits.ipriority_id73 = ipriority_id73;
	reg.bits.ipriority_id74 = ipriority_id74;
	reg.bits.ipriority_id75 = ipriority_id75;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR18_IPRIORITY_ID72
fun_set_mod_reg_bit(gicd, ipriorityr18, ipriority_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR18_IPRIORITY_ID73
fun_set_mod_reg_bit(gicd, ipriorityr18, ipriority_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR18_IPRIORITY_ID74
fun_set_mod_reg_bit(gicd, ipriorityr18, ipriority_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR18_IPRIORITY_ID75
fun_set_mod_reg_bit(gicd, ipriorityr18, ipriority_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR19
void set_gicd_ipriorityr19(u32 reg_addr,
			u32 ipriority_id76,
			u32 ipriority_id77,
			u32 ipriority_id78,
			u32 ipriority_id79,
			u32 m_or_r)
{
	reg_gicd_ipriorityr19_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id76 = ipriority_id76;
	reg.bits.ipriority_id77 = ipriority_id77;
	reg.bits.ipriority_id78 = ipriority_id78;
	reg.bits.ipriority_id79 = ipriority_id79;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR19_IPRIORITY_ID76
fun_set_mod_reg_bit(gicd, ipriorityr19, ipriority_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR19_IPRIORITY_ID77
fun_set_mod_reg_bit(gicd, ipriorityr19, ipriority_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR19_IPRIORITY_ID78
fun_set_mod_reg_bit(gicd, ipriorityr19, ipriority_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR19_IPRIORITY_ID79
fun_set_mod_reg_bit(gicd, ipriorityr19, ipriority_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR20
void set_gicd_ipriorityr20(u32 reg_addr,
			u32 ipriority_id80,
			u32 ipriority_id81,
			u32 ipriority_id82,
			u32 ipriority_id83,
			u32 m_or_r)
{
	reg_gicd_ipriorityr20_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id80 = ipriority_id80;
	reg.bits.ipriority_id81 = ipriority_id81;
	reg.bits.ipriority_id82 = ipriority_id82;
	reg.bits.ipriority_id83 = ipriority_id83;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR20_IPRIORITY_ID80
fun_set_mod_reg_bit(gicd, ipriorityr20, ipriority_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR20_IPRIORITY_ID81
fun_set_mod_reg_bit(gicd, ipriorityr20, ipriority_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR20_IPRIORITY_ID82
fun_set_mod_reg_bit(gicd, ipriorityr20, ipriority_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR20_IPRIORITY_ID83
fun_set_mod_reg_bit(gicd, ipriorityr20, ipriority_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR21
void set_gicd_ipriorityr21(u32 reg_addr,
			u32 ipriority_id84,
			u32 ipriority_id85,
			u32 ipriority_id86,
			u32 ipriority_id87,
			u32 m_or_r)
{
	reg_gicd_ipriorityr21_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id84 = ipriority_id84;
	reg.bits.ipriority_id85 = ipriority_id85;
	reg.bits.ipriority_id86 = ipriority_id86;
	reg.bits.ipriority_id87 = ipriority_id87;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR21_IPRIORITY_ID84
fun_set_mod_reg_bit(gicd, ipriorityr21, ipriority_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR21_IPRIORITY_ID85
fun_set_mod_reg_bit(gicd, ipriorityr21, ipriority_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR21_IPRIORITY_ID86
fun_set_mod_reg_bit(gicd, ipriorityr21, ipriority_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR21_IPRIORITY_ID87
fun_set_mod_reg_bit(gicd, ipriorityr21, ipriority_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR22
void set_gicd_ipriorityr22(u32 reg_addr,
			u32 ipriority_id88,
			u32 ipriority_id89,
			u32 ipriority_id90,
			u32 ipriority_id91,
			u32 m_or_r)
{
	reg_gicd_ipriorityr22_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id88 = ipriority_id88;
	reg.bits.ipriority_id89 = ipriority_id89;
	reg.bits.ipriority_id90 = ipriority_id90;
	reg.bits.ipriority_id91 = ipriority_id91;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR22_IPRIORITY_ID88
fun_set_mod_reg_bit(gicd, ipriorityr22, ipriority_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR22_IPRIORITY_ID89
fun_set_mod_reg_bit(gicd, ipriorityr22, ipriority_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR22_IPRIORITY_ID90
fun_set_mod_reg_bit(gicd, ipriorityr22, ipriority_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR22_IPRIORITY_ID91
fun_set_mod_reg_bit(gicd, ipriorityr22, ipriority_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR23
void set_gicd_ipriorityr23(u32 reg_addr,
			u32 ipriority_id92,
			u32 ipriority_id93,
			u32 ipriority_id94,
			u32 ipriority_id95,
			u32 m_or_r)
{
	reg_gicd_ipriorityr23_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id92 = ipriority_id92;
	reg.bits.ipriority_id93 = ipriority_id93;
	reg.bits.ipriority_id94 = ipriority_id94;
	reg.bits.ipriority_id95 = ipriority_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR23_IPRIORITY_ID92
fun_set_mod_reg_bit(gicd, ipriorityr23, ipriority_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR23_IPRIORITY_ID93
fun_set_mod_reg_bit(gicd, ipriorityr23, ipriority_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR23_IPRIORITY_ID94
fun_set_mod_reg_bit(gicd, ipriorityr23, ipriority_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR23_IPRIORITY_ID95
fun_set_mod_reg_bit(gicd, ipriorityr23, ipriority_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR24
void set_gicd_ipriorityr24(u32 reg_addr,
			u32 ipriority_id96,
			u32 ipriority_id97,
			u32 ipriority_id98,
			u32 ipriority_id99,
			u32 m_or_r)
{
	reg_gicd_ipriorityr24_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id96 = ipriority_id96;
	reg.bits.ipriority_id97 = ipriority_id97;
	reg.bits.ipriority_id98 = ipriority_id98;
	reg.bits.ipriority_id99 = ipriority_id99;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR24_IPRIORITY_ID96
fun_set_mod_reg_bit(gicd, ipriorityr24, ipriority_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR24_IPRIORITY_ID97
fun_set_mod_reg_bit(gicd, ipriorityr24, ipriority_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR24_IPRIORITY_ID98
fun_set_mod_reg_bit(gicd, ipriorityr24, ipriority_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR24_IPRIORITY_ID99
fun_set_mod_reg_bit(gicd, ipriorityr24, ipriority_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR25
void set_gicd_ipriorityr25(u32 reg_addr,
			u32 ipriority_id100,
			u32 ipriority_id101,
			u32 ipriority_id102,
			u32 ipriority_id103,
			u32 m_or_r)
{
	reg_gicd_ipriorityr25_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id100 = ipriority_id100;
	reg.bits.ipriority_id101 = ipriority_id101;
	reg.bits.ipriority_id102 = ipriority_id102;
	reg.bits.ipriority_id103 = ipriority_id103;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR25_IPRIORITY_ID100
fun_set_mod_reg_bit(gicd, ipriorityr25, ipriority_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR25_IPRIORITY_ID101
fun_set_mod_reg_bit(gicd, ipriorityr25, ipriority_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR25_IPRIORITY_ID102
fun_set_mod_reg_bit(gicd, ipriorityr25, ipriority_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR25_IPRIORITY_ID103
fun_set_mod_reg_bit(gicd, ipriorityr25, ipriority_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR26
void set_gicd_ipriorityr26(u32 reg_addr,
			u32 ipriority_id104,
			u32 ipriority_id105,
			u32 ipriority_id106,
			u32 ipriority_id107,
			u32 m_or_r)
{
	reg_gicd_ipriorityr26_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id104 = ipriority_id104;
	reg.bits.ipriority_id105 = ipriority_id105;
	reg.bits.ipriority_id106 = ipriority_id106;
	reg.bits.ipriority_id107 = ipriority_id107;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR26_IPRIORITY_ID104
fun_set_mod_reg_bit(gicd, ipriorityr26, ipriority_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR26_IPRIORITY_ID105
fun_set_mod_reg_bit(gicd, ipriorityr26, ipriority_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR26_IPRIORITY_ID106
fun_set_mod_reg_bit(gicd, ipriorityr26, ipriority_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR26_IPRIORITY_ID107
fun_set_mod_reg_bit(gicd, ipriorityr26, ipriority_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR27
void set_gicd_ipriorityr27(u32 reg_addr,
			u32 ipriority_id108,
			u32 ipriority_id109,
			u32 ipriority_id110,
			u32 ipriority_id111,
			u32 m_or_r)
{
	reg_gicd_ipriorityr27_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id108 = ipriority_id108;
	reg.bits.ipriority_id109 = ipriority_id109;
	reg.bits.ipriority_id110 = ipriority_id110;
	reg.bits.ipriority_id111 = ipriority_id111;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR27_IPRIORITY_ID108
fun_set_mod_reg_bit(gicd, ipriorityr27, ipriority_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR27_IPRIORITY_ID109
fun_set_mod_reg_bit(gicd, ipriorityr27, ipriority_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR27_IPRIORITY_ID110
fun_set_mod_reg_bit(gicd, ipriorityr27, ipriority_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR27_IPRIORITY_ID111
fun_set_mod_reg_bit(gicd, ipriorityr27, ipriority_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR28
void set_gicd_ipriorityr28(u32 reg_addr,
			u32 ipriority_id112,
			u32 ipriority_id113,
			u32 ipriority_id114,
			u32 ipriority_id115,
			u32 m_or_r)
{
	reg_gicd_ipriorityr28_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id112 = ipriority_id112;
	reg.bits.ipriority_id113 = ipriority_id113;
	reg.bits.ipriority_id114 = ipriority_id114;
	reg.bits.ipriority_id115 = ipriority_id115;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR28_IPRIORITY_ID112
fun_set_mod_reg_bit(gicd, ipriorityr28, ipriority_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR28_IPRIORITY_ID113
fun_set_mod_reg_bit(gicd, ipriorityr28, ipriority_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR28_IPRIORITY_ID114
fun_set_mod_reg_bit(gicd, ipriorityr28, ipriority_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR28_IPRIORITY_ID115
fun_set_mod_reg_bit(gicd, ipriorityr28, ipriority_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR29
void set_gicd_ipriorityr29(u32 reg_addr,
			u32 ipriority_id116,
			u32 ipriority_id117,
			u32 ipriority_id118,
			u32 ipriority_id119,
			u32 m_or_r)
{
	reg_gicd_ipriorityr29_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id116 = ipriority_id116;
	reg.bits.ipriority_id117 = ipriority_id117;
	reg.bits.ipriority_id118 = ipriority_id118;
	reg.bits.ipriority_id119 = ipriority_id119;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR29_IPRIORITY_ID116
fun_set_mod_reg_bit(gicd, ipriorityr29, ipriority_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR29_IPRIORITY_ID117
fun_set_mod_reg_bit(gicd, ipriorityr29, ipriority_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR29_IPRIORITY_ID118
fun_set_mod_reg_bit(gicd, ipriorityr29, ipriority_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR29_IPRIORITY_ID119
fun_set_mod_reg_bit(gicd, ipriorityr29, ipriority_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR30
void set_gicd_ipriorityr30(u32 reg_addr,
			u32 ipriority_id120,
			u32 ipriority_id121,
			u32 ipriority_id122,
			u32 ipriority_id123,
			u32 m_or_r)
{
	reg_gicd_ipriorityr30_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id120 = ipriority_id120;
	reg.bits.ipriority_id121 = ipriority_id121;
	reg.bits.ipriority_id122 = ipriority_id122;
	reg.bits.ipriority_id123 = ipriority_id123;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR30_IPRIORITY_ID120
fun_set_mod_reg_bit(gicd, ipriorityr30, ipriority_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR30_IPRIORITY_ID121
fun_set_mod_reg_bit(gicd, ipriorityr30, ipriority_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR30_IPRIORITY_ID122
fun_set_mod_reg_bit(gicd, ipriorityr30, ipriority_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR30_IPRIORITY_ID123
fun_set_mod_reg_bit(gicd, ipriorityr30, ipriority_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR31
void set_gicd_ipriorityr31(u32 reg_addr,
			u32 ipriority_id124,
			u32 ipriority_id125,
			u32 ipriority_id126,
			u32 ipriority_id127,
			u32 m_or_r)
{
	reg_gicd_ipriorityr31_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id124 = ipriority_id124;
	reg.bits.ipriority_id125 = ipriority_id125;
	reg.bits.ipriority_id126 = ipriority_id126;
	reg.bits.ipriority_id127 = ipriority_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR31_IPRIORITY_ID124
fun_set_mod_reg_bit(gicd, ipriorityr31, ipriority_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR31_IPRIORITY_ID125
fun_set_mod_reg_bit(gicd, ipriorityr31, ipriority_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR31_IPRIORITY_ID126
fun_set_mod_reg_bit(gicd, ipriorityr31, ipriority_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR31_IPRIORITY_ID127
fun_set_mod_reg_bit(gicd, ipriorityr31, ipriority_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR32
void set_gicd_ipriorityr32(u32 reg_addr,
			u32 ipriority_id128,
			u32 ipriority_id129,
			u32 ipriority_id130,
			u32 ipriority_id131,
			u32 m_or_r)
{
	reg_gicd_ipriorityr32_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id128 = ipriority_id128;
	reg.bits.ipriority_id129 = ipriority_id129;
	reg.bits.ipriority_id130 = ipriority_id130;
	reg.bits.ipriority_id131 = ipriority_id131;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR32_IPRIORITY_ID128
fun_set_mod_reg_bit(gicd, ipriorityr32, ipriority_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR32_IPRIORITY_ID129
fun_set_mod_reg_bit(gicd, ipriorityr32, ipriority_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR32_IPRIORITY_ID130
fun_set_mod_reg_bit(gicd, ipriorityr32, ipriority_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR32_IPRIORITY_ID131
fun_set_mod_reg_bit(gicd, ipriorityr32, ipriority_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR33
void set_gicd_ipriorityr33(u32 reg_addr,
			u32 ipriority_id132,
			u32 ipriority_id133,
			u32 ipriority_id134,
			u32 ipriority_id135,
			u32 m_or_r)
{
	reg_gicd_ipriorityr33_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id132 = ipriority_id132;
	reg.bits.ipriority_id133 = ipriority_id133;
	reg.bits.ipriority_id134 = ipriority_id134;
	reg.bits.ipriority_id135 = ipriority_id135;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR33_IPRIORITY_ID132
fun_set_mod_reg_bit(gicd, ipriorityr33, ipriority_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR33_IPRIORITY_ID133
fun_set_mod_reg_bit(gicd, ipriorityr33, ipriority_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR33_IPRIORITY_ID134
fun_set_mod_reg_bit(gicd, ipriorityr33, ipriority_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR33_IPRIORITY_ID135
fun_set_mod_reg_bit(gicd, ipriorityr33, ipriority_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR34
void set_gicd_ipriorityr34(u32 reg_addr,
			u32 ipriority_id136,
			u32 ipriority_id137,
			u32 ipriority_id138,
			u32 ipriority_id139,
			u32 m_or_r)
{
	reg_gicd_ipriorityr34_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id136 = ipriority_id136;
	reg.bits.ipriority_id137 = ipriority_id137;
	reg.bits.ipriority_id138 = ipriority_id138;
	reg.bits.ipriority_id139 = ipriority_id139;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR34_IPRIORITY_ID136
fun_set_mod_reg_bit(gicd, ipriorityr34, ipriority_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR34_IPRIORITY_ID137
fun_set_mod_reg_bit(gicd, ipriorityr34, ipriority_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR34_IPRIORITY_ID138
fun_set_mod_reg_bit(gicd, ipriorityr34, ipriority_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR34_IPRIORITY_ID139
fun_set_mod_reg_bit(gicd, ipriorityr34, ipriority_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR35
void set_gicd_ipriorityr35(u32 reg_addr,
			u32 ipriority_id140,
			u32 ipriority_id141,
			u32 ipriority_id142,
			u32 ipriority_id143,
			u32 m_or_r)
{
	reg_gicd_ipriorityr35_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id140 = ipriority_id140;
	reg.bits.ipriority_id141 = ipriority_id141;
	reg.bits.ipriority_id142 = ipriority_id142;
	reg.bits.ipriority_id143 = ipriority_id143;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR35_IPRIORITY_ID140
fun_set_mod_reg_bit(gicd, ipriorityr35, ipriority_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR35_IPRIORITY_ID141
fun_set_mod_reg_bit(gicd, ipriorityr35, ipriority_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR35_IPRIORITY_ID142
fun_set_mod_reg_bit(gicd, ipriorityr35, ipriority_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR35_IPRIORITY_ID143
fun_set_mod_reg_bit(gicd, ipriorityr35, ipriority_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR36
void set_gicd_ipriorityr36(u32 reg_addr,
			u32 ipriority_id144,
			u32 ipriority_id145,
			u32 ipriority_id146,
			u32 ipriority_id147,
			u32 m_or_r)
{
	reg_gicd_ipriorityr36_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id144 = ipriority_id144;
	reg.bits.ipriority_id145 = ipriority_id145;
	reg.bits.ipriority_id146 = ipriority_id146;
	reg.bits.ipriority_id147 = ipriority_id147;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR36_IPRIORITY_ID144
fun_set_mod_reg_bit(gicd, ipriorityr36, ipriority_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR36_IPRIORITY_ID145
fun_set_mod_reg_bit(gicd, ipriorityr36, ipriority_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR36_IPRIORITY_ID146
fun_set_mod_reg_bit(gicd, ipriorityr36, ipriority_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR36_IPRIORITY_ID147
fun_set_mod_reg_bit(gicd, ipriorityr36, ipriority_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR37
void set_gicd_ipriorityr37(u32 reg_addr,
			u32 ipriority_id148,
			u32 ipriority_id149,
			u32 ipriority_id150,
			u32 ipriority_id151,
			u32 m_or_r)
{
	reg_gicd_ipriorityr37_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id148 = ipriority_id148;
	reg.bits.ipriority_id149 = ipriority_id149;
	reg.bits.ipriority_id150 = ipriority_id150;
	reg.bits.ipriority_id151 = ipriority_id151;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR37_IPRIORITY_ID148
fun_set_mod_reg_bit(gicd, ipriorityr37, ipriority_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR37_IPRIORITY_ID149
fun_set_mod_reg_bit(gicd, ipriorityr37, ipriority_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR37_IPRIORITY_ID150
fun_set_mod_reg_bit(gicd, ipriorityr37, ipriority_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR37_IPRIORITY_ID151
fun_set_mod_reg_bit(gicd, ipriorityr37, ipriority_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR38
void set_gicd_ipriorityr38(u32 reg_addr,
			u32 ipriority_id152,
			u32 ipriority_id153,
			u32 ipriority_id154,
			u32 ipriority_id155,
			u32 m_or_r)
{
	reg_gicd_ipriorityr38_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id152 = ipriority_id152;
	reg.bits.ipriority_id153 = ipriority_id153;
	reg.bits.ipriority_id154 = ipriority_id154;
	reg.bits.ipriority_id155 = ipriority_id155;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR38_IPRIORITY_ID152
fun_set_mod_reg_bit(gicd, ipriorityr38, ipriority_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR38_IPRIORITY_ID153
fun_set_mod_reg_bit(gicd, ipriorityr38, ipriority_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR38_IPRIORITY_ID154
fun_set_mod_reg_bit(gicd, ipriorityr38, ipriority_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR38_IPRIORITY_ID155
fun_set_mod_reg_bit(gicd, ipriorityr38, ipriority_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR39
void set_gicd_ipriorityr39(u32 reg_addr,
			u32 ipriority_id156,
			u32 ipriority_id157,
			u32 ipriority_id158,
			u32 ipriority_id159,
			u32 m_or_r)
{
	reg_gicd_ipriorityr39_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id156 = ipriority_id156;
	reg.bits.ipriority_id157 = ipriority_id157;
	reg.bits.ipriority_id158 = ipriority_id158;
	reg.bits.ipriority_id159 = ipriority_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR39_IPRIORITY_ID156
fun_set_mod_reg_bit(gicd, ipriorityr39, ipriority_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR39_IPRIORITY_ID157
fun_set_mod_reg_bit(gicd, ipriorityr39, ipriority_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR39_IPRIORITY_ID158
fun_set_mod_reg_bit(gicd, ipriorityr39, ipriority_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR39_IPRIORITY_ID159
fun_set_mod_reg_bit(gicd, ipriorityr39, ipriority_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR40
void set_gicd_ipriorityr40(u32 reg_addr,
			u32 ipriority_id160,
			u32 ipriority_id161,
			u32 ipriority_id162,
			u32 ipriority_id163,
			u32 m_or_r)
{
	reg_gicd_ipriorityr40_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id160 = ipriority_id160;
	reg.bits.ipriority_id161 = ipriority_id161;
	reg.bits.ipriority_id162 = ipriority_id162;
	reg.bits.ipriority_id163 = ipriority_id163;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR40_IPRIORITY_ID160
fun_set_mod_reg_bit(gicd, ipriorityr40, ipriority_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR40_IPRIORITY_ID161
fun_set_mod_reg_bit(gicd, ipriorityr40, ipriority_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR40_IPRIORITY_ID162
fun_set_mod_reg_bit(gicd, ipriorityr40, ipriority_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR40_IPRIORITY_ID163
fun_set_mod_reg_bit(gicd, ipriorityr40, ipriority_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR41
void set_gicd_ipriorityr41(u32 reg_addr,
			u32 ipriority_id164,
			u32 ipriority_id165,
			u32 ipriority_id166,
			u32 ipriority_id167,
			u32 m_or_r)
{
	reg_gicd_ipriorityr41_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id164 = ipriority_id164;
	reg.bits.ipriority_id165 = ipriority_id165;
	reg.bits.ipriority_id166 = ipriority_id166;
	reg.bits.ipriority_id167 = ipriority_id167;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR41_IPRIORITY_ID164
fun_set_mod_reg_bit(gicd, ipriorityr41, ipriority_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR41_IPRIORITY_ID165
fun_set_mod_reg_bit(gicd, ipriorityr41, ipriority_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR41_IPRIORITY_ID166
fun_set_mod_reg_bit(gicd, ipriorityr41, ipriority_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR41_IPRIORITY_ID167
fun_set_mod_reg_bit(gicd, ipriorityr41, ipriority_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR42
void set_gicd_ipriorityr42(u32 reg_addr,
			u32 ipriority_id168,
			u32 ipriority_id169,
			u32 ipriority_id170,
			u32 ipriority_id171,
			u32 m_or_r)
{
	reg_gicd_ipriorityr42_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id168 = ipriority_id168;
	reg.bits.ipriority_id169 = ipriority_id169;
	reg.bits.ipriority_id170 = ipriority_id170;
	reg.bits.ipriority_id171 = ipriority_id171;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR42_IPRIORITY_ID168
fun_set_mod_reg_bit(gicd, ipriorityr42, ipriority_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR42_IPRIORITY_ID169
fun_set_mod_reg_bit(gicd, ipriorityr42, ipriority_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR42_IPRIORITY_ID170
fun_set_mod_reg_bit(gicd, ipriorityr42, ipriority_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR42_IPRIORITY_ID171
fun_set_mod_reg_bit(gicd, ipriorityr42, ipriority_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR43
void set_gicd_ipriorityr43(u32 reg_addr,
			u32 ipriority_id172,
			u32 ipriority_id173,
			u32 ipriority_id174,
			u32 ipriority_id175,
			u32 m_or_r)
{
	reg_gicd_ipriorityr43_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id172 = ipriority_id172;
	reg.bits.ipriority_id173 = ipriority_id173;
	reg.bits.ipriority_id174 = ipriority_id174;
	reg.bits.ipriority_id175 = ipriority_id175;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR43_IPRIORITY_ID172
fun_set_mod_reg_bit(gicd, ipriorityr43, ipriority_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR43_IPRIORITY_ID173
fun_set_mod_reg_bit(gicd, ipriorityr43, ipriority_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR43_IPRIORITY_ID174
fun_set_mod_reg_bit(gicd, ipriorityr43, ipriority_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR43_IPRIORITY_ID175
fun_set_mod_reg_bit(gicd, ipriorityr43, ipriority_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR44
void set_gicd_ipriorityr44(u32 reg_addr,
			u32 ipriority_id176,
			u32 ipriority_id177,
			u32 ipriority_id178,
			u32 ipriority_id179,
			u32 m_or_r)
{
	reg_gicd_ipriorityr44_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id176 = ipriority_id176;
	reg.bits.ipriority_id177 = ipriority_id177;
	reg.bits.ipriority_id178 = ipriority_id178;
	reg.bits.ipriority_id179 = ipriority_id179;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR44_IPRIORITY_ID176
fun_set_mod_reg_bit(gicd, ipriorityr44, ipriority_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR44_IPRIORITY_ID177
fun_set_mod_reg_bit(gicd, ipriorityr44, ipriority_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR44_IPRIORITY_ID178
fun_set_mod_reg_bit(gicd, ipriorityr44, ipriority_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR44_IPRIORITY_ID179
fun_set_mod_reg_bit(gicd, ipriorityr44, ipriority_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR45
void set_gicd_ipriorityr45(u32 reg_addr,
			u32 ipriority_id180,
			u32 ipriority_id181,
			u32 ipriority_id182,
			u32 ipriority_id183,
			u32 m_or_r)
{
	reg_gicd_ipriorityr45_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id180 = ipriority_id180;
	reg.bits.ipriority_id181 = ipriority_id181;
	reg.bits.ipriority_id182 = ipriority_id182;
	reg.bits.ipriority_id183 = ipriority_id183;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR45_IPRIORITY_ID180
fun_set_mod_reg_bit(gicd, ipriorityr45, ipriority_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR45_IPRIORITY_ID181
fun_set_mod_reg_bit(gicd, ipriorityr45, ipriority_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR45_IPRIORITY_ID182
fun_set_mod_reg_bit(gicd, ipriorityr45, ipriority_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR45_IPRIORITY_ID183
fun_set_mod_reg_bit(gicd, ipriorityr45, ipriority_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR46
void set_gicd_ipriorityr46(u32 reg_addr,
			u32 ipriority_id184,
			u32 ipriority_id185,
			u32 ipriority_id186,
			u32 ipriority_id187,
			u32 m_or_r)
{
	reg_gicd_ipriorityr46_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id184 = ipriority_id184;
	reg.bits.ipriority_id185 = ipriority_id185;
	reg.bits.ipriority_id186 = ipriority_id186;
	reg.bits.ipriority_id187 = ipriority_id187;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR46_IPRIORITY_ID184
fun_set_mod_reg_bit(gicd, ipriorityr46, ipriority_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR46_IPRIORITY_ID185
fun_set_mod_reg_bit(gicd, ipriorityr46, ipriority_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR46_IPRIORITY_ID186
fun_set_mod_reg_bit(gicd, ipriorityr46, ipriority_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR46_IPRIORITY_ID187
fun_set_mod_reg_bit(gicd, ipriorityr46, ipriority_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR47
void set_gicd_ipriorityr47(u32 reg_addr,
			u32 ipriority_id188,
			u32 ipriority_id189,
			u32 ipriority_id190,
			u32 ipriority_id191,
			u32 m_or_r)
{
	reg_gicd_ipriorityr47_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id188 = ipriority_id188;
	reg.bits.ipriority_id189 = ipriority_id189;
	reg.bits.ipriority_id190 = ipriority_id190;
	reg.bits.ipriority_id191 = ipriority_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR47_IPRIORITY_ID188
fun_set_mod_reg_bit(gicd, ipriorityr47, ipriority_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR47_IPRIORITY_ID189
fun_set_mod_reg_bit(gicd, ipriorityr47, ipriority_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR47_IPRIORITY_ID190
fun_set_mod_reg_bit(gicd, ipriorityr47, ipriority_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR47_IPRIORITY_ID191
fun_set_mod_reg_bit(gicd, ipriorityr47, ipriority_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR48
void set_gicd_ipriorityr48(u32 reg_addr,
			u32 ipriority_id192,
			u32 ipriority_id193,
			u32 ipriority_id194,
			u32 ipriority_id195,
			u32 m_or_r)
{
	reg_gicd_ipriorityr48_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id192 = ipriority_id192;
	reg.bits.ipriority_id193 = ipriority_id193;
	reg.bits.ipriority_id194 = ipriority_id194;
	reg.bits.ipriority_id195 = ipriority_id195;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR48_IPRIORITY_ID192
fun_set_mod_reg_bit(gicd, ipriorityr48, ipriority_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR48_IPRIORITY_ID193
fun_set_mod_reg_bit(gicd, ipriorityr48, ipriority_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR48_IPRIORITY_ID194
fun_set_mod_reg_bit(gicd, ipriorityr48, ipriority_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR48_IPRIORITY_ID195
fun_set_mod_reg_bit(gicd, ipriorityr48, ipriority_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR49
void set_gicd_ipriorityr49(u32 reg_addr,
			u32 ipriority_id196,
			u32 ipriority_id197,
			u32 ipriority_id198,
			u32 ipriority_id199,
			u32 m_or_r)
{
	reg_gicd_ipriorityr49_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id196 = ipriority_id196;
	reg.bits.ipriority_id197 = ipriority_id197;
	reg.bits.ipriority_id198 = ipriority_id198;
	reg.bits.ipriority_id199 = ipriority_id199;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR49_IPRIORITY_ID196
fun_set_mod_reg_bit(gicd, ipriorityr49, ipriority_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR49_IPRIORITY_ID197
fun_set_mod_reg_bit(gicd, ipriorityr49, ipriority_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR49_IPRIORITY_ID198
fun_set_mod_reg_bit(gicd, ipriorityr49, ipriority_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR49_IPRIORITY_ID199
fun_set_mod_reg_bit(gicd, ipriorityr49, ipriority_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR50
void set_gicd_ipriorityr50(u32 reg_addr,
			u32 ipriority_id200,
			u32 ipriority_id201,
			u32 ipriority_id202,
			u32 ipriority_id203,
			u32 m_or_r)
{
	reg_gicd_ipriorityr50_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id200 = ipriority_id200;
	reg.bits.ipriority_id201 = ipriority_id201;
	reg.bits.ipriority_id202 = ipriority_id202;
	reg.bits.ipriority_id203 = ipriority_id203;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR50_IPRIORITY_ID200
fun_set_mod_reg_bit(gicd, ipriorityr50, ipriority_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR50_IPRIORITY_ID201
fun_set_mod_reg_bit(gicd, ipriorityr50, ipriority_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR50_IPRIORITY_ID202
fun_set_mod_reg_bit(gicd, ipriorityr50, ipriority_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR50_IPRIORITY_ID203
fun_set_mod_reg_bit(gicd, ipriorityr50, ipriority_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR51
void set_gicd_ipriorityr51(u32 reg_addr,
			u32 ipriority_id204,
			u32 ipriority_id205,
			u32 ipriority_id206,
			u32 ipriority_id207,
			u32 m_or_r)
{
	reg_gicd_ipriorityr51_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id204 = ipriority_id204;
	reg.bits.ipriority_id205 = ipriority_id205;
	reg.bits.ipriority_id206 = ipriority_id206;
	reg.bits.ipriority_id207 = ipriority_id207;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR51_IPRIORITY_ID204
fun_set_mod_reg_bit(gicd, ipriorityr51, ipriority_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR51_IPRIORITY_ID205
fun_set_mod_reg_bit(gicd, ipriorityr51, ipriority_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR51_IPRIORITY_ID206
fun_set_mod_reg_bit(gicd, ipriorityr51, ipriority_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR51_IPRIORITY_ID207
fun_set_mod_reg_bit(gicd, ipriorityr51, ipriority_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR52
void set_gicd_ipriorityr52(u32 reg_addr,
			u32 ipriority_id208,
			u32 ipriority_id209,
			u32 ipriority_id210,
			u32 ipriority_id211,
			u32 m_or_r)
{
	reg_gicd_ipriorityr52_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id208 = ipriority_id208;
	reg.bits.ipriority_id209 = ipriority_id209;
	reg.bits.ipriority_id210 = ipriority_id210;
	reg.bits.ipriority_id211 = ipriority_id211;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR52_IPRIORITY_ID208
fun_set_mod_reg_bit(gicd, ipriorityr52, ipriority_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR52_IPRIORITY_ID209
fun_set_mod_reg_bit(gicd, ipriorityr52, ipriority_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR52_IPRIORITY_ID210
fun_set_mod_reg_bit(gicd, ipriorityr52, ipriority_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR52_IPRIORITY_ID211
fun_set_mod_reg_bit(gicd, ipriorityr52, ipriority_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR53
void set_gicd_ipriorityr53(u32 reg_addr,
			u32 ipriority_id212,
			u32 ipriority_id213,
			u32 ipriority_id214,
			u32 ipriority_id215,
			u32 m_or_r)
{
	reg_gicd_ipriorityr53_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id212 = ipriority_id212;
	reg.bits.ipriority_id213 = ipriority_id213;
	reg.bits.ipriority_id214 = ipriority_id214;
	reg.bits.ipriority_id215 = ipriority_id215;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR53_IPRIORITY_ID212
fun_set_mod_reg_bit(gicd, ipriorityr53, ipriority_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR53_IPRIORITY_ID213
fun_set_mod_reg_bit(gicd, ipriorityr53, ipriority_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR53_IPRIORITY_ID214
fun_set_mod_reg_bit(gicd, ipriorityr53, ipriority_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR53_IPRIORITY_ID215
fun_set_mod_reg_bit(gicd, ipriorityr53, ipriority_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR54
void set_gicd_ipriorityr54(u32 reg_addr,
			u32 ipriority_id216,
			u32 ipriority_id217,
			u32 ipriority_id218,
			u32 ipriority_id219,
			u32 m_or_r)
{
	reg_gicd_ipriorityr54_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id216 = ipriority_id216;
	reg.bits.ipriority_id217 = ipriority_id217;
	reg.bits.ipriority_id218 = ipriority_id218;
	reg.bits.ipriority_id219 = ipriority_id219;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR54_IPRIORITY_ID216
fun_set_mod_reg_bit(gicd, ipriorityr54, ipriority_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR54_IPRIORITY_ID217
fun_set_mod_reg_bit(gicd, ipriorityr54, ipriority_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR54_IPRIORITY_ID218
fun_set_mod_reg_bit(gicd, ipriorityr54, ipriority_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR54_IPRIORITY_ID219
fun_set_mod_reg_bit(gicd, ipriorityr54, ipriority_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR55
void set_gicd_ipriorityr55(u32 reg_addr,
			u32 ipriority_id220,
			u32 ipriority_id221,
			u32 ipriority_id222,
			u32 ipriority_id223,
			u32 m_or_r)
{
	reg_gicd_ipriorityr55_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id220 = ipriority_id220;
	reg.bits.ipriority_id221 = ipriority_id221;
	reg.bits.ipriority_id222 = ipriority_id222;
	reg.bits.ipriority_id223 = ipriority_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_IPRIORITYR55_IPRIORITY_ID220
fun_set_mod_reg_bit(gicd, ipriorityr55, ipriority_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR55_IPRIORITY_ID221
fun_set_mod_reg_bit(gicd, ipriorityr55, ipriority_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR55_IPRIORITY_ID222
fun_set_mod_reg_bit(gicd, ipriorityr55, ipriority_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_IPRIORITYR55_IPRIORITY_ID223
fun_set_mod_reg_bit(gicd, ipriorityr55, ipriority_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR0
void set_gicd_itargetsr0(u32 reg_addr,
			u32 ipriority_id0,
			u32 ipriority_id1,
			u32 ipriority_id2,
			u32 ipriority_id3,
			u32 m_or_r)
{
	reg_gicd_itargetsr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id0 = ipriority_id0;
	reg.bits.ipriority_id1 = ipriority_id1;
	reg.bits.ipriority_id2 = ipriority_id2;
	reg.bits.ipriority_id3 = ipriority_id3;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR0_IPRIORITY_ID0
fun_set_mod_reg_bit(gicd, itargetsr0, ipriority_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR0_IPRIORITY_ID1
fun_set_mod_reg_bit(gicd, itargetsr0, ipriority_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR0_IPRIORITY_ID2
fun_set_mod_reg_bit(gicd, itargetsr0, ipriority_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR0_IPRIORITY_ID3
fun_set_mod_reg_bit(gicd, itargetsr0, ipriority_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR1
void set_gicd_itargetsr1(u32 reg_addr,
			u32 ipriority_id4,
			u32 ipriority_id5,
			u32 ipriority_id6,
			u32 ipriority_id7,
			u32 m_or_r)
{
	reg_gicd_itargetsr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id4 = ipriority_id4;
	reg.bits.ipriority_id5 = ipriority_id5;
	reg.bits.ipriority_id6 = ipriority_id6;
	reg.bits.ipriority_id7 = ipriority_id7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR1_IPRIORITY_ID4
fun_set_mod_reg_bit(gicd, itargetsr1, ipriority_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR1_IPRIORITY_ID5
fun_set_mod_reg_bit(gicd, itargetsr1, ipriority_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR1_IPRIORITY_ID6
fun_set_mod_reg_bit(gicd, itargetsr1, ipriority_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR1_IPRIORITY_ID7
fun_set_mod_reg_bit(gicd, itargetsr1, ipriority_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR2
void set_gicd_itargetsr2(u32 reg_addr,
			u32 ipriority_id8,
			u32 ipriority_id9,
			u32 ipriority_id10,
			u32 ipriority_id11,
			u32 m_or_r)
{
	reg_gicd_itargetsr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id8 = ipriority_id8;
	reg.bits.ipriority_id9 = ipriority_id9;
	reg.bits.ipriority_id10 = ipriority_id10;
	reg.bits.ipriority_id11 = ipriority_id11;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR2_IPRIORITY_ID8
fun_set_mod_reg_bit(gicd, itargetsr2, ipriority_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR2_IPRIORITY_ID9
fun_set_mod_reg_bit(gicd, itargetsr2, ipriority_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR2_IPRIORITY_ID10
fun_set_mod_reg_bit(gicd, itargetsr2, ipriority_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR2_IPRIORITY_ID11
fun_set_mod_reg_bit(gicd, itargetsr2, ipriority_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR3
void set_gicd_itargetsr3(u32 reg_addr,
			u32 ipriority_id12,
			u32 ipriority_id13,
			u32 ipriority_id14,
			u32 ipriority_id15,
			u32 m_or_r)
{
	reg_gicd_itargetsr3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id12 = ipriority_id12;
	reg.bits.ipriority_id13 = ipriority_id13;
	reg.bits.ipriority_id14 = ipriority_id14;
	reg.bits.ipriority_id15 = ipriority_id15;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR3_IPRIORITY_ID12
fun_set_mod_reg_bit(gicd, itargetsr3, ipriority_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR3_IPRIORITY_ID13
fun_set_mod_reg_bit(gicd, itargetsr3, ipriority_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR3_IPRIORITY_ID14
fun_set_mod_reg_bit(gicd, itargetsr3, ipriority_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR3_IPRIORITY_ID15
fun_set_mod_reg_bit(gicd, itargetsr3, ipriority_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR4
void set_gicd_itargetsr4(u32 reg_addr,
			u32 ipriority_id16,
			u32 ipriority_id17,
			u32 ipriority_id18,
			u32 ipriority_id19,
			u32 m_or_r)
{
	reg_gicd_itargetsr4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id16 = ipriority_id16;
	reg.bits.ipriority_id17 = ipriority_id17;
	reg.bits.ipriority_id18 = ipriority_id18;
	reg.bits.ipriority_id19 = ipriority_id19;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR4_IPRIORITY_ID16
fun_set_mod_reg_bit(gicd, itargetsr4, ipriority_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR4_IPRIORITY_ID17
fun_set_mod_reg_bit(gicd, itargetsr4, ipriority_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR4_IPRIORITY_ID18
fun_set_mod_reg_bit(gicd, itargetsr4, ipriority_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR4_IPRIORITY_ID19
fun_set_mod_reg_bit(gicd, itargetsr4, ipriority_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR5
void set_gicd_itargetsr5(u32 reg_addr,
			u32 ipriority_id20,
			u32 ipriority_id21,
			u32 ipriority_id22,
			u32 ipriority_id23,
			u32 m_or_r)
{
	reg_gicd_itargetsr5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id20 = ipriority_id20;
	reg.bits.ipriority_id21 = ipriority_id21;
	reg.bits.ipriority_id22 = ipriority_id22;
	reg.bits.ipriority_id23 = ipriority_id23;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR5_IPRIORITY_ID20
fun_set_mod_reg_bit(gicd, itargetsr5, ipriority_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR5_IPRIORITY_ID21
fun_set_mod_reg_bit(gicd, itargetsr5, ipriority_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR5_IPRIORITY_ID22
fun_set_mod_reg_bit(gicd, itargetsr5, ipriority_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR5_IPRIORITY_ID23
fun_set_mod_reg_bit(gicd, itargetsr5, ipriority_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR6
void set_gicd_itargetsr6(u32 reg_addr,
			u32 ipriority_id24,
			u32 ipriority_id25,
			u32 ipriority_id26,
			u32 ipriority_id27,
			u32 m_or_r)
{
	reg_gicd_itargetsr6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id24 = ipriority_id24;
	reg.bits.ipriority_id25 = ipriority_id25;
	reg.bits.ipriority_id26 = ipriority_id26;
	reg.bits.ipriority_id27 = ipriority_id27;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR6_IPRIORITY_ID24
fun_set_mod_reg_bit(gicd, itargetsr6, ipriority_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR6_IPRIORITY_ID25
fun_set_mod_reg_bit(gicd, itargetsr6, ipriority_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR6_IPRIORITY_ID26
fun_set_mod_reg_bit(gicd, itargetsr6, ipriority_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR6_IPRIORITY_ID27
fun_set_mod_reg_bit(gicd, itargetsr6, ipriority_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR7
void set_gicd_itargetsr7(u32 reg_addr,
			u32 ipriority_id28,
			u32 ipriority_id29,
			u32 ipriority_id30,
			u32 ipriority_id31,
			u32 m_or_r)
{
	reg_gicd_itargetsr7_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id28 = ipriority_id28;
	reg.bits.ipriority_id29 = ipriority_id29;
	reg.bits.ipriority_id30 = ipriority_id30;
	reg.bits.ipriority_id31 = ipriority_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR7_IPRIORITY_ID28
fun_set_mod_reg_bit(gicd, itargetsr7, ipriority_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR7_IPRIORITY_ID29
fun_set_mod_reg_bit(gicd, itargetsr7, ipriority_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR7_IPRIORITY_ID30
fun_set_mod_reg_bit(gicd, itargetsr7, ipriority_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR7_IPRIORITY_ID31
fun_set_mod_reg_bit(gicd, itargetsr7, ipriority_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR8
void set_gicd_itargetsr8(u32 reg_addr,
			u32 ipriority_id32,
			u32 ipriority_id33,
			u32 ipriority_id34,
			u32 ipriority_id35,
			u32 m_or_r)
{
	reg_gicd_itargetsr8_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id32 = ipriority_id32;
	reg.bits.ipriority_id33 = ipriority_id33;
	reg.bits.ipriority_id34 = ipriority_id34;
	reg.bits.ipriority_id35 = ipriority_id35;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR8_IPRIORITY_ID32
fun_set_mod_reg_bit(gicd, itargetsr8, ipriority_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR8_IPRIORITY_ID33
fun_set_mod_reg_bit(gicd, itargetsr8, ipriority_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR8_IPRIORITY_ID34
fun_set_mod_reg_bit(gicd, itargetsr8, ipriority_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR8_IPRIORITY_ID35
fun_set_mod_reg_bit(gicd, itargetsr8, ipriority_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR9
void set_gicd_itargetsr9(u32 reg_addr,
			u32 ipriority_id36,
			u32 ipriority_id37,
			u32 ipriority_id38,
			u32 ipriority_id39,
			u32 m_or_r)
{
	reg_gicd_itargetsr9_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id36 = ipriority_id36;
	reg.bits.ipriority_id37 = ipriority_id37;
	reg.bits.ipriority_id38 = ipriority_id38;
	reg.bits.ipriority_id39 = ipriority_id39;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR9_IPRIORITY_ID36
fun_set_mod_reg_bit(gicd, itargetsr9, ipriority_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR9_IPRIORITY_ID37
fun_set_mod_reg_bit(gicd, itargetsr9, ipriority_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR9_IPRIORITY_ID38
fun_set_mod_reg_bit(gicd, itargetsr9, ipriority_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR9_IPRIORITY_ID39
fun_set_mod_reg_bit(gicd, itargetsr9, ipriority_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR10
void set_gicd_itargetsr10(u32 reg_addr,
			u32 ipriority_id40,
			u32 ipriority_id41,
			u32 ipriority_id42,
			u32 ipriority_id43,
			u32 m_or_r)
{
	reg_gicd_itargetsr10_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id40 = ipriority_id40;
	reg.bits.ipriority_id41 = ipriority_id41;
	reg.bits.ipriority_id42 = ipriority_id42;
	reg.bits.ipriority_id43 = ipriority_id43;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR10_IPRIORITY_ID40
fun_set_mod_reg_bit(gicd, itargetsr10, ipriority_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR10_IPRIORITY_ID41
fun_set_mod_reg_bit(gicd, itargetsr10, ipriority_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR10_IPRIORITY_ID42
fun_set_mod_reg_bit(gicd, itargetsr10, ipriority_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR10_IPRIORITY_ID43
fun_set_mod_reg_bit(gicd, itargetsr10, ipriority_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR11
void set_gicd_itargetsr11(u32 reg_addr,
			u32 ipriority_id44,
			u32 ipriority_id45,
			u32 ipriority_id46,
			u32 ipriority_id47,
			u32 m_or_r)
{
	reg_gicd_itargetsr11_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id44 = ipriority_id44;
	reg.bits.ipriority_id45 = ipriority_id45;
	reg.bits.ipriority_id46 = ipriority_id46;
	reg.bits.ipriority_id47 = ipriority_id47;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR11_IPRIORITY_ID44
fun_set_mod_reg_bit(gicd, itargetsr11, ipriority_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR11_IPRIORITY_ID45
fun_set_mod_reg_bit(gicd, itargetsr11, ipriority_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR11_IPRIORITY_ID46
fun_set_mod_reg_bit(gicd, itargetsr11, ipriority_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR11_IPRIORITY_ID47
fun_set_mod_reg_bit(gicd, itargetsr11, ipriority_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR12
void set_gicd_itargetsr12(u32 reg_addr,
			u32 ipriority_id48,
			u32 ipriority_id49,
			u32 ipriority_id50,
			u32 ipriority_id51,
			u32 m_or_r)
{
	reg_gicd_itargetsr12_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id48 = ipriority_id48;
	reg.bits.ipriority_id49 = ipriority_id49;
	reg.bits.ipriority_id50 = ipriority_id50;
	reg.bits.ipriority_id51 = ipriority_id51;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR12_IPRIORITY_ID48
fun_set_mod_reg_bit(gicd, itargetsr12, ipriority_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR12_IPRIORITY_ID49
fun_set_mod_reg_bit(gicd, itargetsr12, ipriority_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR12_IPRIORITY_ID50
fun_set_mod_reg_bit(gicd, itargetsr12, ipriority_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR12_IPRIORITY_ID51
fun_set_mod_reg_bit(gicd, itargetsr12, ipriority_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR13
void set_gicd_itargetsr13(u32 reg_addr,
			u32 ipriority_id52,
			u32 ipriority_id53,
			u32 ipriority_id54,
			u32 ipriority_id55,
			u32 m_or_r)
{
	reg_gicd_itargetsr13_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id52 = ipriority_id52;
	reg.bits.ipriority_id53 = ipriority_id53;
	reg.bits.ipriority_id54 = ipriority_id54;
	reg.bits.ipriority_id55 = ipriority_id55;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR13_IPRIORITY_ID52
fun_set_mod_reg_bit(gicd, itargetsr13, ipriority_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR13_IPRIORITY_ID53
fun_set_mod_reg_bit(gicd, itargetsr13, ipriority_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR13_IPRIORITY_ID54
fun_set_mod_reg_bit(gicd, itargetsr13, ipriority_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR13_IPRIORITY_ID55
fun_set_mod_reg_bit(gicd, itargetsr13, ipriority_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR14
void set_gicd_itargetsr14(u32 reg_addr,
			u32 ipriority_id56,
			u32 ipriority_id57,
			u32 ipriority_id58,
			u32 ipriority_id59,
			u32 m_or_r)
{
	reg_gicd_itargetsr14_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id56 = ipriority_id56;
	reg.bits.ipriority_id57 = ipriority_id57;
	reg.bits.ipriority_id58 = ipriority_id58;
	reg.bits.ipriority_id59 = ipriority_id59;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR14_IPRIORITY_ID56
fun_set_mod_reg_bit(gicd, itargetsr14, ipriority_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR14_IPRIORITY_ID57
fun_set_mod_reg_bit(gicd, itargetsr14, ipriority_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR14_IPRIORITY_ID58
fun_set_mod_reg_bit(gicd, itargetsr14, ipriority_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR14_IPRIORITY_ID59
fun_set_mod_reg_bit(gicd, itargetsr14, ipriority_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR15
void set_gicd_itargetsr15(u32 reg_addr,
			u32 ipriority_id60,
			u32 ipriority_id61,
			u32 ipriority_id62,
			u32 ipriority_id63,
			u32 m_or_r)
{
	reg_gicd_itargetsr15_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id60 = ipriority_id60;
	reg.bits.ipriority_id61 = ipriority_id61;
	reg.bits.ipriority_id62 = ipriority_id62;
	reg.bits.ipriority_id63 = ipriority_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR15_IPRIORITY_ID60
fun_set_mod_reg_bit(gicd, itargetsr15, ipriority_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR15_IPRIORITY_ID61
fun_set_mod_reg_bit(gicd, itargetsr15, ipriority_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR15_IPRIORITY_ID62
fun_set_mod_reg_bit(gicd, itargetsr15, ipriority_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR15_IPRIORITY_ID63
fun_set_mod_reg_bit(gicd, itargetsr15, ipriority_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR16
void set_gicd_itargetsr16(u32 reg_addr,
			u32 ipriority_id64,
			u32 ipriority_id65,
			u32 ipriority_id66,
			u32 ipriority_id67,
			u32 m_or_r)
{
	reg_gicd_itargetsr16_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id64 = ipriority_id64;
	reg.bits.ipriority_id65 = ipriority_id65;
	reg.bits.ipriority_id66 = ipriority_id66;
	reg.bits.ipriority_id67 = ipriority_id67;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR16_IPRIORITY_ID64
fun_set_mod_reg_bit(gicd, itargetsr16, ipriority_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR16_IPRIORITY_ID65
fun_set_mod_reg_bit(gicd, itargetsr16, ipriority_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR16_IPRIORITY_ID66
fun_set_mod_reg_bit(gicd, itargetsr16, ipriority_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR16_IPRIORITY_ID67
fun_set_mod_reg_bit(gicd, itargetsr16, ipriority_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR17
void set_gicd_itargetsr17(u32 reg_addr,
			u32 ipriority_id68,
			u32 ipriority_id69,
			u32 ipriority_id70,
			u32 ipriority_id71,
			u32 m_or_r)
{
	reg_gicd_itargetsr17_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id68 = ipriority_id68;
	reg.bits.ipriority_id69 = ipriority_id69;
	reg.bits.ipriority_id70 = ipriority_id70;
	reg.bits.ipriority_id71 = ipriority_id71;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR17_IPRIORITY_ID68
fun_set_mod_reg_bit(gicd, itargetsr17, ipriority_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR17_IPRIORITY_ID69
fun_set_mod_reg_bit(gicd, itargetsr17, ipriority_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR17_IPRIORITY_ID70
fun_set_mod_reg_bit(gicd, itargetsr17, ipriority_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR17_IPRIORITY_ID71
fun_set_mod_reg_bit(gicd, itargetsr17, ipriority_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR18
void set_gicd_itargetsr18(u32 reg_addr,
			u32 ipriority_id72,
			u32 ipriority_id73,
			u32 ipriority_id74,
			u32 ipriority_id75,
			u32 m_or_r)
{
	reg_gicd_itargetsr18_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id72 = ipriority_id72;
	reg.bits.ipriority_id73 = ipriority_id73;
	reg.bits.ipriority_id74 = ipriority_id74;
	reg.bits.ipriority_id75 = ipriority_id75;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR18_IPRIORITY_ID72
fun_set_mod_reg_bit(gicd, itargetsr18, ipriority_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR18_IPRIORITY_ID73
fun_set_mod_reg_bit(gicd, itargetsr18, ipriority_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR18_IPRIORITY_ID74
fun_set_mod_reg_bit(gicd, itargetsr18, ipriority_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR18_IPRIORITY_ID75
fun_set_mod_reg_bit(gicd, itargetsr18, ipriority_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR19
void set_gicd_itargetsr19(u32 reg_addr,
			u32 ipriority_id76,
			u32 ipriority_id77,
			u32 ipriority_id78,
			u32 ipriority_id79,
			u32 m_or_r)
{
	reg_gicd_itargetsr19_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id76 = ipriority_id76;
	reg.bits.ipriority_id77 = ipriority_id77;
	reg.bits.ipriority_id78 = ipriority_id78;
	reg.bits.ipriority_id79 = ipriority_id79;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR19_IPRIORITY_ID76
fun_set_mod_reg_bit(gicd, itargetsr19, ipriority_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR19_IPRIORITY_ID77
fun_set_mod_reg_bit(gicd, itargetsr19, ipriority_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR19_IPRIORITY_ID78
fun_set_mod_reg_bit(gicd, itargetsr19, ipriority_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR19_IPRIORITY_ID79
fun_set_mod_reg_bit(gicd, itargetsr19, ipriority_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR20
void set_gicd_itargetsr20(u32 reg_addr,
			u32 ipriority_id80,
			u32 ipriority_id81,
			u32 ipriority_id82,
			u32 ipriority_id83,
			u32 m_or_r)
{
	reg_gicd_itargetsr20_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id80 = ipriority_id80;
	reg.bits.ipriority_id81 = ipriority_id81;
	reg.bits.ipriority_id82 = ipriority_id82;
	reg.bits.ipriority_id83 = ipriority_id83;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR20_IPRIORITY_ID80
fun_set_mod_reg_bit(gicd, itargetsr20, ipriority_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR20_IPRIORITY_ID81
fun_set_mod_reg_bit(gicd, itargetsr20, ipriority_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR20_IPRIORITY_ID82
fun_set_mod_reg_bit(gicd, itargetsr20, ipriority_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR20_IPRIORITY_ID83
fun_set_mod_reg_bit(gicd, itargetsr20, ipriority_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR21
void set_gicd_itargetsr21(u32 reg_addr,
			u32 ipriority_id84,
			u32 ipriority_id85,
			u32 ipriority_id86,
			u32 ipriority_id87,
			u32 m_or_r)
{
	reg_gicd_itargetsr21_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id84 = ipriority_id84;
	reg.bits.ipriority_id85 = ipriority_id85;
	reg.bits.ipriority_id86 = ipriority_id86;
	reg.bits.ipriority_id87 = ipriority_id87;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR21_IPRIORITY_ID84
fun_set_mod_reg_bit(gicd, itargetsr21, ipriority_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR21_IPRIORITY_ID85
fun_set_mod_reg_bit(gicd, itargetsr21, ipriority_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR21_IPRIORITY_ID86
fun_set_mod_reg_bit(gicd, itargetsr21, ipriority_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR21_IPRIORITY_ID87
fun_set_mod_reg_bit(gicd, itargetsr21, ipriority_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR22
void set_gicd_itargetsr22(u32 reg_addr,
			u32 ipriority_id88,
			u32 ipriority_id89,
			u32 ipriority_id90,
			u32 ipriority_id91,
			u32 m_or_r)
{
	reg_gicd_itargetsr22_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id88 = ipriority_id88;
	reg.bits.ipriority_id89 = ipriority_id89;
	reg.bits.ipriority_id90 = ipriority_id90;
	reg.bits.ipriority_id91 = ipriority_id91;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR22_IPRIORITY_ID88
fun_set_mod_reg_bit(gicd, itargetsr22, ipriority_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR22_IPRIORITY_ID89
fun_set_mod_reg_bit(gicd, itargetsr22, ipriority_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR22_IPRIORITY_ID90
fun_set_mod_reg_bit(gicd, itargetsr22, ipriority_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR22_IPRIORITY_ID91
fun_set_mod_reg_bit(gicd, itargetsr22, ipriority_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR23
void set_gicd_itargetsr23(u32 reg_addr,
			u32 ipriority_id92,
			u32 ipriority_id93,
			u32 ipriority_id94,
			u32 ipriority_id95,
			u32 m_or_r)
{
	reg_gicd_itargetsr23_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id92 = ipriority_id92;
	reg.bits.ipriority_id93 = ipriority_id93;
	reg.bits.ipriority_id94 = ipriority_id94;
	reg.bits.ipriority_id95 = ipriority_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR23_IPRIORITY_ID92
fun_set_mod_reg_bit(gicd, itargetsr23, ipriority_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR23_IPRIORITY_ID93
fun_set_mod_reg_bit(gicd, itargetsr23, ipriority_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR23_IPRIORITY_ID94
fun_set_mod_reg_bit(gicd, itargetsr23, ipriority_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR23_IPRIORITY_ID95
fun_set_mod_reg_bit(gicd, itargetsr23, ipriority_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR24
void set_gicd_itargetsr24(u32 reg_addr,
			u32 ipriority_id96,
			u32 ipriority_id97,
			u32 ipriority_id98,
			u32 ipriority_id99,
			u32 m_or_r)
{
	reg_gicd_itargetsr24_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id96 = ipriority_id96;
	reg.bits.ipriority_id97 = ipriority_id97;
	reg.bits.ipriority_id98 = ipriority_id98;
	reg.bits.ipriority_id99 = ipriority_id99;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR24_IPRIORITY_ID96
fun_set_mod_reg_bit(gicd, itargetsr24, ipriority_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR24_IPRIORITY_ID97
fun_set_mod_reg_bit(gicd, itargetsr24, ipriority_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR24_IPRIORITY_ID98
fun_set_mod_reg_bit(gicd, itargetsr24, ipriority_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR24_IPRIORITY_ID99
fun_set_mod_reg_bit(gicd, itargetsr24, ipriority_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR25
void set_gicd_itargetsr25(u32 reg_addr,
			u32 ipriority_id100,
			u32 ipriority_id101,
			u32 ipriority_id102,
			u32 ipriority_id103,
			u32 m_or_r)
{
	reg_gicd_itargetsr25_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id100 = ipriority_id100;
	reg.bits.ipriority_id101 = ipriority_id101;
	reg.bits.ipriority_id102 = ipriority_id102;
	reg.bits.ipriority_id103 = ipriority_id103;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR25_IPRIORITY_ID100
fun_set_mod_reg_bit(gicd, itargetsr25, ipriority_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR25_IPRIORITY_ID101
fun_set_mod_reg_bit(gicd, itargetsr25, ipriority_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR25_IPRIORITY_ID102
fun_set_mod_reg_bit(gicd, itargetsr25, ipriority_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR25_IPRIORITY_ID103
fun_set_mod_reg_bit(gicd, itargetsr25, ipriority_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR26
void set_gicd_itargetsr26(u32 reg_addr,
			u32 ipriority_id104,
			u32 ipriority_id105,
			u32 ipriority_id106,
			u32 ipriority_id107,
			u32 m_or_r)
{
	reg_gicd_itargetsr26_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id104 = ipriority_id104;
	reg.bits.ipriority_id105 = ipriority_id105;
	reg.bits.ipriority_id106 = ipriority_id106;
	reg.bits.ipriority_id107 = ipriority_id107;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR26_IPRIORITY_ID104
fun_set_mod_reg_bit(gicd, itargetsr26, ipriority_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR26_IPRIORITY_ID105
fun_set_mod_reg_bit(gicd, itargetsr26, ipriority_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR26_IPRIORITY_ID106
fun_set_mod_reg_bit(gicd, itargetsr26, ipriority_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR26_IPRIORITY_ID107
fun_set_mod_reg_bit(gicd, itargetsr26, ipriority_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR27
void set_gicd_itargetsr27(u32 reg_addr,
			u32 ipriority_id108,
			u32 ipriority_id109,
			u32 ipriority_id110,
			u32 ipriority_id111,
			u32 m_or_r)
{
	reg_gicd_itargetsr27_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id108 = ipriority_id108;
	reg.bits.ipriority_id109 = ipriority_id109;
	reg.bits.ipriority_id110 = ipriority_id110;
	reg.bits.ipriority_id111 = ipriority_id111;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR27_IPRIORITY_ID108
fun_set_mod_reg_bit(gicd, itargetsr27, ipriority_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR27_IPRIORITY_ID109
fun_set_mod_reg_bit(gicd, itargetsr27, ipriority_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR27_IPRIORITY_ID110
fun_set_mod_reg_bit(gicd, itargetsr27, ipriority_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR27_IPRIORITY_ID111
fun_set_mod_reg_bit(gicd, itargetsr27, ipriority_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR28
void set_gicd_itargetsr28(u32 reg_addr,
			u32 ipriority_id112,
			u32 ipriority_id113,
			u32 ipriority_id114,
			u32 ipriority_id115,
			u32 m_or_r)
{
	reg_gicd_itargetsr28_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id112 = ipriority_id112;
	reg.bits.ipriority_id113 = ipriority_id113;
	reg.bits.ipriority_id114 = ipriority_id114;
	reg.bits.ipriority_id115 = ipriority_id115;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR28_IPRIORITY_ID112
fun_set_mod_reg_bit(gicd, itargetsr28, ipriority_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR28_IPRIORITY_ID113
fun_set_mod_reg_bit(gicd, itargetsr28, ipriority_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR28_IPRIORITY_ID114
fun_set_mod_reg_bit(gicd, itargetsr28, ipriority_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR28_IPRIORITY_ID115
fun_set_mod_reg_bit(gicd, itargetsr28, ipriority_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR29
void set_gicd_itargetsr29(u32 reg_addr,
			u32 ipriority_id116,
			u32 ipriority_id117,
			u32 ipriority_id118,
			u32 ipriority_id119,
			u32 m_or_r)
{
	reg_gicd_itargetsr29_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id116 = ipriority_id116;
	reg.bits.ipriority_id117 = ipriority_id117;
	reg.bits.ipriority_id118 = ipriority_id118;
	reg.bits.ipriority_id119 = ipriority_id119;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR29_IPRIORITY_ID116
fun_set_mod_reg_bit(gicd, itargetsr29, ipriority_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR29_IPRIORITY_ID117
fun_set_mod_reg_bit(gicd, itargetsr29, ipriority_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR29_IPRIORITY_ID118
fun_set_mod_reg_bit(gicd, itargetsr29, ipriority_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR29_IPRIORITY_ID119
fun_set_mod_reg_bit(gicd, itargetsr29, ipriority_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR30
void set_gicd_itargetsr30(u32 reg_addr,
			u32 ipriority_id120,
			u32 ipriority_id121,
			u32 ipriority_id122,
			u32 ipriority_id123,
			u32 m_or_r)
{
	reg_gicd_itargetsr30_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id120 = ipriority_id120;
	reg.bits.ipriority_id121 = ipriority_id121;
	reg.bits.ipriority_id122 = ipriority_id122;
	reg.bits.ipriority_id123 = ipriority_id123;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR30_IPRIORITY_ID120
fun_set_mod_reg_bit(gicd, itargetsr30, ipriority_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR30_IPRIORITY_ID121
fun_set_mod_reg_bit(gicd, itargetsr30, ipriority_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR30_IPRIORITY_ID122
fun_set_mod_reg_bit(gicd, itargetsr30, ipriority_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR30_IPRIORITY_ID123
fun_set_mod_reg_bit(gicd, itargetsr30, ipriority_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR31
void set_gicd_itargetsr31(u32 reg_addr,
			u32 ipriority_id124,
			u32 ipriority_id125,
			u32 ipriority_id126,
			u32 ipriority_id127,
			u32 m_or_r)
{
	reg_gicd_itargetsr31_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id124 = ipriority_id124;
	reg.bits.ipriority_id125 = ipriority_id125;
	reg.bits.ipriority_id126 = ipriority_id126;
	reg.bits.ipriority_id127 = ipriority_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR31_IPRIORITY_ID124
fun_set_mod_reg_bit(gicd, itargetsr31, ipriority_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR31_IPRIORITY_ID125
fun_set_mod_reg_bit(gicd, itargetsr31, ipriority_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR31_IPRIORITY_ID126
fun_set_mod_reg_bit(gicd, itargetsr31, ipriority_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR31_IPRIORITY_ID127
fun_set_mod_reg_bit(gicd, itargetsr31, ipriority_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR32
void set_gicd_itargetsr32(u32 reg_addr,
			u32 ipriority_id128,
			u32 ipriority_id129,
			u32 ipriority_id130,
			u32 ipriority_id131,
			u32 m_or_r)
{
	reg_gicd_itargetsr32_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id128 = ipriority_id128;
	reg.bits.ipriority_id129 = ipriority_id129;
	reg.bits.ipriority_id130 = ipriority_id130;
	reg.bits.ipriority_id131 = ipriority_id131;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR32_IPRIORITY_ID128
fun_set_mod_reg_bit(gicd, itargetsr32, ipriority_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR32_IPRIORITY_ID129
fun_set_mod_reg_bit(gicd, itargetsr32, ipriority_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR32_IPRIORITY_ID130
fun_set_mod_reg_bit(gicd, itargetsr32, ipriority_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR32_IPRIORITY_ID131
fun_set_mod_reg_bit(gicd, itargetsr32, ipriority_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR33
void set_gicd_itargetsr33(u32 reg_addr,
			u32 ipriority_id132,
			u32 ipriority_id133,
			u32 ipriority_id134,
			u32 ipriority_id135,
			u32 m_or_r)
{
	reg_gicd_itargetsr33_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id132 = ipriority_id132;
	reg.bits.ipriority_id133 = ipriority_id133;
	reg.bits.ipriority_id134 = ipriority_id134;
	reg.bits.ipriority_id135 = ipriority_id135;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR33_IPRIORITY_ID132
fun_set_mod_reg_bit(gicd, itargetsr33, ipriority_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR33_IPRIORITY_ID133
fun_set_mod_reg_bit(gicd, itargetsr33, ipriority_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR33_IPRIORITY_ID134
fun_set_mod_reg_bit(gicd, itargetsr33, ipriority_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR33_IPRIORITY_ID135
fun_set_mod_reg_bit(gicd, itargetsr33, ipriority_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR34
void set_gicd_itargetsr34(u32 reg_addr,
			u32 ipriority_id136,
			u32 ipriority_id137,
			u32 ipriority_id138,
			u32 ipriority_id139,
			u32 m_or_r)
{
	reg_gicd_itargetsr34_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id136 = ipriority_id136;
	reg.bits.ipriority_id137 = ipriority_id137;
	reg.bits.ipriority_id138 = ipriority_id138;
	reg.bits.ipriority_id139 = ipriority_id139;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR34_IPRIORITY_ID136
fun_set_mod_reg_bit(gicd, itargetsr34, ipriority_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR34_IPRIORITY_ID137
fun_set_mod_reg_bit(gicd, itargetsr34, ipriority_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR34_IPRIORITY_ID138
fun_set_mod_reg_bit(gicd, itargetsr34, ipriority_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR34_IPRIORITY_ID139
fun_set_mod_reg_bit(gicd, itargetsr34, ipriority_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR35
void set_gicd_itargetsr35(u32 reg_addr,
			u32 ipriority_id140,
			u32 ipriority_id141,
			u32 ipriority_id142,
			u32 ipriority_id143,
			u32 m_or_r)
{
	reg_gicd_itargetsr35_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id140 = ipriority_id140;
	reg.bits.ipriority_id141 = ipriority_id141;
	reg.bits.ipriority_id142 = ipriority_id142;
	reg.bits.ipriority_id143 = ipriority_id143;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR35_IPRIORITY_ID140
fun_set_mod_reg_bit(gicd, itargetsr35, ipriority_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR35_IPRIORITY_ID141
fun_set_mod_reg_bit(gicd, itargetsr35, ipriority_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR35_IPRIORITY_ID142
fun_set_mod_reg_bit(gicd, itargetsr35, ipriority_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR35_IPRIORITY_ID143
fun_set_mod_reg_bit(gicd, itargetsr35, ipriority_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR36
void set_gicd_itargetsr36(u32 reg_addr,
			u32 ipriority_id144,
			u32 ipriority_id145,
			u32 ipriority_id146,
			u32 ipriority_id147,
			u32 m_or_r)
{
	reg_gicd_itargetsr36_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id144 = ipriority_id144;
	reg.bits.ipriority_id145 = ipriority_id145;
	reg.bits.ipriority_id146 = ipriority_id146;
	reg.bits.ipriority_id147 = ipriority_id147;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR36_IPRIORITY_ID144
fun_set_mod_reg_bit(gicd, itargetsr36, ipriority_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR36_IPRIORITY_ID145
fun_set_mod_reg_bit(gicd, itargetsr36, ipriority_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR36_IPRIORITY_ID146
fun_set_mod_reg_bit(gicd, itargetsr36, ipriority_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR36_IPRIORITY_ID147
fun_set_mod_reg_bit(gicd, itargetsr36, ipriority_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR37
void set_gicd_itargetsr37(u32 reg_addr,
			u32 ipriority_id148,
			u32 ipriority_id149,
			u32 ipriority_id150,
			u32 ipriority_id151,
			u32 m_or_r)
{
	reg_gicd_itargetsr37_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id148 = ipriority_id148;
	reg.bits.ipriority_id149 = ipriority_id149;
	reg.bits.ipriority_id150 = ipriority_id150;
	reg.bits.ipriority_id151 = ipriority_id151;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR37_IPRIORITY_ID148
fun_set_mod_reg_bit(gicd, itargetsr37, ipriority_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR37_IPRIORITY_ID149
fun_set_mod_reg_bit(gicd, itargetsr37, ipriority_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR37_IPRIORITY_ID150
fun_set_mod_reg_bit(gicd, itargetsr37, ipriority_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR37_IPRIORITY_ID151
fun_set_mod_reg_bit(gicd, itargetsr37, ipriority_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR38
void set_gicd_itargetsr38(u32 reg_addr,
			u32 ipriority_id152,
			u32 ipriority_id153,
			u32 ipriority_id154,
			u32 ipriority_id155,
			u32 m_or_r)
{
	reg_gicd_itargetsr38_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id152 = ipriority_id152;
	reg.bits.ipriority_id153 = ipriority_id153;
	reg.bits.ipriority_id154 = ipriority_id154;
	reg.bits.ipriority_id155 = ipriority_id155;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR38_IPRIORITY_ID152
fun_set_mod_reg_bit(gicd, itargetsr38, ipriority_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR38_IPRIORITY_ID153
fun_set_mod_reg_bit(gicd, itargetsr38, ipriority_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR38_IPRIORITY_ID154
fun_set_mod_reg_bit(gicd, itargetsr38, ipriority_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR38_IPRIORITY_ID155
fun_set_mod_reg_bit(gicd, itargetsr38, ipriority_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR39
void set_gicd_itargetsr39(u32 reg_addr,
			u32 ipriority_id156,
			u32 ipriority_id157,
			u32 ipriority_id158,
			u32 ipriority_id159,
			u32 m_or_r)
{
	reg_gicd_itargetsr39_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id156 = ipriority_id156;
	reg.bits.ipriority_id157 = ipriority_id157;
	reg.bits.ipriority_id158 = ipriority_id158;
	reg.bits.ipriority_id159 = ipriority_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR39_IPRIORITY_ID156
fun_set_mod_reg_bit(gicd, itargetsr39, ipriority_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR39_IPRIORITY_ID157
fun_set_mod_reg_bit(gicd, itargetsr39, ipriority_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR39_IPRIORITY_ID158
fun_set_mod_reg_bit(gicd, itargetsr39, ipriority_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR39_IPRIORITY_ID159
fun_set_mod_reg_bit(gicd, itargetsr39, ipriority_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR40
void set_gicd_itargetsr40(u32 reg_addr,
			u32 ipriority_id160,
			u32 ipriority_id161,
			u32 ipriority_id162,
			u32 ipriority_id163,
			u32 m_or_r)
{
	reg_gicd_itargetsr40_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id160 = ipriority_id160;
	reg.bits.ipriority_id161 = ipriority_id161;
	reg.bits.ipriority_id162 = ipriority_id162;
	reg.bits.ipriority_id163 = ipriority_id163;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR40_IPRIORITY_ID160
fun_set_mod_reg_bit(gicd, itargetsr40, ipriority_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR40_IPRIORITY_ID161
fun_set_mod_reg_bit(gicd, itargetsr40, ipriority_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR40_IPRIORITY_ID162
fun_set_mod_reg_bit(gicd, itargetsr40, ipriority_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR40_IPRIORITY_ID163
fun_set_mod_reg_bit(gicd, itargetsr40, ipriority_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR41
void set_gicd_itargetsr41(u32 reg_addr,
			u32 ipriority_id164,
			u32 ipriority_id165,
			u32 ipriority_id166,
			u32 ipriority_id167,
			u32 m_or_r)
{
	reg_gicd_itargetsr41_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id164 = ipriority_id164;
	reg.bits.ipriority_id165 = ipriority_id165;
	reg.bits.ipriority_id166 = ipriority_id166;
	reg.bits.ipriority_id167 = ipriority_id167;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR41_IPRIORITY_ID164
fun_set_mod_reg_bit(gicd, itargetsr41, ipriority_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR41_IPRIORITY_ID165
fun_set_mod_reg_bit(gicd, itargetsr41, ipriority_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR41_IPRIORITY_ID166
fun_set_mod_reg_bit(gicd, itargetsr41, ipriority_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR41_IPRIORITY_ID167
fun_set_mod_reg_bit(gicd, itargetsr41, ipriority_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR42
void set_gicd_itargetsr42(u32 reg_addr,
			u32 ipriority_id168,
			u32 ipriority_id169,
			u32 ipriority_id170,
			u32 ipriority_id171,
			u32 m_or_r)
{
	reg_gicd_itargetsr42_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id168 = ipriority_id168;
	reg.bits.ipriority_id169 = ipriority_id169;
	reg.bits.ipriority_id170 = ipriority_id170;
	reg.bits.ipriority_id171 = ipriority_id171;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR42_IPRIORITY_ID168
fun_set_mod_reg_bit(gicd, itargetsr42, ipriority_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR42_IPRIORITY_ID169
fun_set_mod_reg_bit(gicd, itargetsr42, ipriority_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR42_IPRIORITY_ID170
fun_set_mod_reg_bit(gicd, itargetsr42, ipriority_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR42_IPRIORITY_ID171
fun_set_mod_reg_bit(gicd, itargetsr42, ipriority_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR43
void set_gicd_itargetsr43(u32 reg_addr,
			u32 ipriority_id172,
			u32 ipriority_id173,
			u32 ipriority_id174,
			u32 ipriority_id175,
			u32 m_or_r)
{
	reg_gicd_itargetsr43_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id172 = ipriority_id172;
	reg.bits.ipriority_id173 = ipriority_id173;
	reg.bits.ipriority_id174 = ipriority_id174;
	reg.bits.ipriority_id175 = ipriority_id175;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR43_IPRIORITY_ID172
fun_set_mod_reg_bit(gicd, itargetsr43, ipriority_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR43_IPRIORITY_ID173
fun_set_mod_reg_bit(gicd, itargetsr43, ipriority_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR43_IPRIORITY_ID174
fun_set_mod_reg_bit(gicd, itargetsr43, ipriority_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR43_IPRIORITY_ID175
fun_set_mod_reg_bit(gicd, itargetsr43, ipriority_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR44
void set_gicd_itargetsr44(u32 reg_addr,
			u32 ipriority_id176,
			u32 ipriority_id177,
			u32 ipriority_id178,
			u32 ipriority_id179,
			u32 m_or_r)
{
	reg_gicd_itargetsr44_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id176 = ipriority_id176;
	reg.bits.ipriority_id177 = ipriority_id177;
	reg.bits.ipriority_id178 = ipriority_id178;
	reg.bits.ipriority_id179 = ipriority_id179;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR44_IPRIORITY_ID176
fun_set_mod_reg_bit(gicd, itargetsr44, ipriority_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR44_IPRIORITY_ID177
fun_set_mod_reg_bit(gicd, itargetsr44, ipriority_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR44_IPRIORITY_ID178
fun_set_mod_reg_bit(gicd, itargetsr44, ipriority_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR44_IPRIORITY_ID179
fun_set_mod_reg_bit(gicd, itargetsr44, ipriority_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR45
void set_gicd_itargetsr45(u32 reg_addr,
			u32 ipriority_id180,
			u32 ipriority_id181,
			u32 ipriority_id182,
			u32 ipriority_id183,
			u32 m_or_r)
{
	reg_gicd_itargetsr45_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id180 = ipriority_id180;
	reg.bits.ipriority_id181 = ipriority_id181;
	reg.bits.ipriority_id182 = ipriority_id182;
	reg.bits.ipriority_id183 = ipriority_id183;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR45_IPRIORITY_ID180
fun_set_mod_reg_bit(gicd, itargetsr45, ipriority_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR45_IPRIORITY_ID181
fun_set_mod_reg_bit(gicd, itargetsr45, ipriority_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR45_IPRIORITY_ID182
fun_set_mod_reg_bit(gicd, itargetsr45, ipriority_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR45_IPRIORITY_ID183
fun_set_mod_reg_bit(gicd, itargetsr45, ipriority_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR46
void set_gicd_itargetsr46(u32 reg_addr,
			u32 ipriority_id184,
			u32 ipriority_id185,
			u32 ipriority_id186,
			u32 ipriority_id187,
			u32 m_or_r)
{
	reg_gicd_itargetsr46_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id184 = ipriority_id184;
	reg.bits.ipriority_id185 = ipriority_id185;
	reg.bits.ipriority_id186 = ipriority_id186;
	reg.bits.ipriority_id187 = ipriority_id187;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR46_IPRIORITY_ID184
fun_set_mod_reg_bit(gicd, itargetsr46, ipriority_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR46_IPRIORITY_ID185
fun_set_mod_reg_bit(gicd, itargetsr46, ipriority_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR46_IPRIORITY_ID186
fun_set_mod_reg_bit(gicd, itargetsr46, ipriority_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR46_IPRIORITY_ID187
fun_set_mod_reg_bit(gicd, itargetsr46, ipriority_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR47
void set_gicd_itargetsr47(u32 reg_addr,
			u32 ipriority_id188,
			u32 ipriority_id189,
			u32 ipriority_id190,
			u32 ipriority_id191,
			u32 m_or_r)
{
	reg_gicd_itargetsr47_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id188 = ipriority_id188;
	reg.bits.ipriority_id189 = ipriority_id189;
	reg.bits.ipriority_id190 = ipriority_id190;
	reg.bits.ipriority_id191 = ipriority_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR47_IPRIORITY_ID188
fun_set_mod_reg_bit(gicd, itargetsr47, ipriority_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR47_IPRIORITY_ID189
fun_set_mod_reg_bit(gicd, itargetsr47, ipriority_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR47_IPRIORITY_ID190
fun_set_mod_reg_bit(gicd, itargetsr47, ipriority_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR47_IPRIORITY_ID191
fun_set_mod_reg_bit(gicd, itargetsr47, ipriority_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR48
void set_gicd_itargetsr48(u32 reg_addr,
			u32 ipriority_id192,
			u32 ipriority_id193,
			u32 ipriority_id194,
			u32 ipriority_id195,
			u32 m_or_r)
{
	reg_gicd_itargetsr48_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id192 = ipriority_id192;
	reg.bits.ipriority_id193 = ipriority_id193;
	reg.bits.ipriority_id194 = ipriority_id194;
	reg.bits.ipriority_id195 = ipriority_id195;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR48_IPRIORITY_ID192
fun_set_mod_reg_bit(gicd, itargetsr48, ipriority_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR48_IPRIORITY_ID193
fun_set_mod_reg_bit(gicd, itargetsr48, ipriority_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR48_IPRIORITY_ID194
fun_set_mod_reg_bit(gicd, itargetsr48, ipriority_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR48_IPRIORITY_ID195
fun_set_mod_reg_bit(gicd, itargetsr48, ipriority_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR49
void set_gicd_itargetsr49(u32 reg_addr,
			u32 ipriority_id196,
			u32 ipriority_id197,
			u32 ipriority_id198,
			u32 ipriority_id199,
			u32 m_or_r)
{
	reg_gicd_itargetsr49_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id196 = ipriority_id196;
	reg.bits.ipriority_id197 = ipriority_id197;
	reg.bits.ipriority_id198 = ipriority_id198;
	reg.bits.ipriority_id199 = ipriority_id199;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR49_IPRIORITY_ID196
fun_set_mod_reg_bit(gicd, itargetsr49, ipriority_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR49_IPRIORITY_ID197
fun_set_mod_reg_bit(gicd, itargetsr49, ipriority_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR49_IPRIORITY_ID198
fun_set_mod_reg_bit(gicd, itargetsr49, ipriority_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR49_IPRIORITY_ID199
fun_set_mod_reg_bit(gicd, itargetsr49, ipriority_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR50
void set_gicd_itargetsr50(u32 reg_addr,
			u32 ipriority_id200,
			u32 ipriority_id201,
			u32 ipriority_id202,
			u32 ipriority_id203,
			u32 m_or_r)
{
	reg_gicd_itargetsr50_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id200 = ipriority_id200;
	reg.bits.ipriority_id201 = ipriority_id201;
	reg.bits.ipriority_id202 = ipriority_id202;
	reg.bits.ipriority_id203 = ipriority_id203;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR50_IPRIORITY_ID200
fun_set_mod_reg_bit(gicd, itargetsr50, ipriority_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR50_IPRIORITY_ID201
fun_set_mod_reg_bit(gicd, itargetsr50, ipriority_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR50_IPRIORITY_ID202
fun_set_mod_reg_bit(gicd, itargetsr50, ipriority_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR50_IPRIORITY_ID203
fun_set_mod_reg_bit(gicd, itargetsr50, ipriority_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR51
void set_gicd_itargetsr51(u32 reg_addr,
			u32 ipriority_id204,
			u32 ipriority_id205,
			u32 ipriority_id206,
			u32 ipriority_id207,
			u32 m_or_r)
{
	reg_gicd_itargetsr51_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id204 = ipriority_id204;
	reg.bits.ipriority_id205 = ipriority_id205;
	reg.bits.ipriority_id206 = ipriority_id206;
	reg.bits.ipriority_id207 = ipriority_id207;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR51_IPRIORITY_ID204
fun_set_mod_reg_bit(gicd, itargetsr51, ipriority_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR51_IPRIORITY_ID205
fun_set_mod_reg_bit(gicd, itargetsr51, ipriority_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR51_IPRIORITY_ID206
fun_set_mod_reg_bit(gicd, itargetsr51, ipriority_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR51_IPRIORITY_ID207
fun_set_mod_reg_bit(gicd, itargetsr51, ipriority_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR52
void set_gicd_itargetsr52(u32 reg_addr,
			u32 ipriority_id208,
			u32 ipriority_id209,
			u32 ipriority_id210,
			u32 ipriority_id211,
			u32 m_or_r)
{
	reg_gicd_itargetsr52_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id208 = ipriority_id208;
	reg.bits.ipriority_id209 = ipriority_id209;
	reg.bits.ipriority_id210 = ipriority_id210;
	reg.bits.ipriority_id211 = ipriority_id211;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR52_IPRIORITY_ID208
fun_set_mod_reg_bit(gicd, itargetsr52, ipriority_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR52_IPRIORITY_ID209
fun_set_mod_reg_bit(gicd, itargetsr52, ipriority_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR52_IPRIORITY_ID210
fun_set_mod_reg_bit(gicd, itargetsr52, ipriority_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR52_IPRIORITY_ID211
fun_set_mod_reg_bit(gicd, itargetsr52, ipriority_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR53
void set_gicd_itargetsr53(u32 reg_addr,
			u32 ipriority_id212,
			u32 ipriority_id213,
			u32 ipriority_id214,
			u32 ipriority_id215,
			u32 m_or_r)
{
	reg_gicd_itargetsr53_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id212 = ipriority_id212;
	reg.bits.ipriority_id213 = ipriority_id213;
	reg.bits.ipriority_id214 = ipriority_id214;
	reg.bits.ipriority_id215 = ipriority_id215;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR53_IPRIORITY_ID212
fun_set_mod_reg_bit(gicd, itargetsr53, ipriority_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR53_IPRIORITY_ID213
fun_set_mod_reg_bit(gicd, itargetsr53, ipriority_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR53_IPRIORITY_ID214
fun_set_mod_reg_bit(gicd, itargetsr53, ipriority_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR53_IPRIORITY_ID215
fun_set_mod_reg_bit(gicd, itargetsr53, ipriority_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR54
void set_gicd_itargetsr54(u32 reg_addr,
			u32 ipriority_id216,
			u32 ipriority_id217,
			u32 ipriority_id218,
			u32 ipriority_id219,
			u32 m_or_r)
{
	reg_gicd_itargetsr54_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id216 = ipriority_id216;
	reg.bits.ipriority_id217 = ipriority_id217;
	reg.bits.ipriority_id218 = ipriority_id218;
	reg.bits.ipriority_id219 = ipriority_id219;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR54_IPRIORITY_ID216
fun_set_mod_reg_bit(gicd, itargetsr54, ipriority_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR54_IPRIORITY_ID217
fun_set_mod_reg_bit(gicd, itargetsr54, ipriority_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR54_IPRIORITY_ID218
fun_set_mod_reg_bit(gicd, itargetsr54, ipriority_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR54_IPRIORITY_ID219
fun_set_mod_reg_bit(gicd, itargetsr54, ipriority_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR55
void set_gicd_itargetsr55(u32 reg_addr,
			u32 ipriority_id220,
			u32 ipriority_id221,
			u32 ipriority_id222,
			u32 ipriority_id223,
			u32 m_or_r)
{
	reg_gicd_itargetsr55_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.ipriority_id220 = ipriority_id220;
	reg.bits.ipriority_id221 = ipriority_id221;
	reg.bits.ipriority_id222 = ipriority_id222;
	reg.bits.ipriority_id223 = ipriority_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ITARGETSR55_IPRIORITY_ID220
fun_set_mod_reg_bit(gicd, itargetsr55, ipriority_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR55_IPRIORITY_ID221
fun_set_mod_reg_bit(gicd, itargetsr55, ipriority_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR55_IPRIORITY_ID222
fun_set_mod_reg_bit(gicd, itargetsr55, ipriority_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ITARGETSR55_IPRIORITY_ID223
fun_set_mod_reg_bit(gicd, itargetsr55, ipriority_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0
void set_gicd_icfgr0(u32 reg_addr,
			u32 cfg_id0,
			u32 cfg_id1,
			u32 cfg_id2,
			u32 cfg_id3,
			u32 cfg_id4,
			u32 cfg_id5,
			u32 cfg_id6,
			u32 cfg_id7,
			u32 cfg_id8,
			u32 cfg_id9,
			u32 cfg_id10,
			u32 cfg_id11,
			u32 cfg_id12,
			u32 cfg_id13,
			u32 cfg_id14,
			u32 cfg_id15,
			u32 m_or_r)
{
	reg_gicd_icfgr0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id0 = cfg_id0;
	reg.bits.cfg_id1 = cfg_id1;
	reg.bits.cfg_id2 = cfg_id2;
	reg.bits.cfg_id3 = cfg_id3;
	reg.bits.cfg_id4 = cfg_id4;
	reg.bits.cfg_id5 = cfg_id5;
	reg.bits.cfg_id6 = cfg_id6;
	reg.bits.cfg_id7 = cfg_id7;
	reg.bits.cfg_id8 = cfg_id8;
	reg.bits.cfg_id9 = cfg_id9;
	reg.bits.cfg_id10 = cfg_id10;
	reg.bits.cfg_id11 = cfg_id11;
	reg.bits.cfg_id12 = cfg_id12;
	reg.bits.cfg_id13 = cfg_id13;
	reg.bits.cfg_id14 = cfg_id14;
	reg.bits.cfg_id15 = cfg_id15;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID0
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID1
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID2
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID3
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID4
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID5
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID6
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID7
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID8
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID9
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID10
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID11
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID12
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID13
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID14
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR0_CFG_ID15
fun_set_mod_reg_bit(gicd, icfgr0, cfg_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1
void set_gicd_icfgr1(u32 reg_addr,
			u32 cfg_id16,
			u32 cfg_id17,
			u32 cfg_id18,
			u32 cfg_id19,
			u32 cfg_id20,
			u32 cfg_id21,
			u32 cfg_id22,
			u32 cfg_id23,
			u32 cfg_id24,
			u32 cfg_id25,
			u32 cfg_id26,
			u32 cfg_id27,
			u32 cfg_id28,
			u32 cfg_id29,
			u32 cfg_id30,
			u32 cfg_id31,
			u32 m_or_r)
{
	reg_gicd_icfgr1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id16 = cfg_id16;
	reg.bits.cfg_id17 = cfg_id17;
	reg.bits.cfg_id18 = cfg_id18;
	reg.bits.cfg_id19 = cfg_id19;
	reg.bits.cfg_id20 = cfg_id20;
	reg.bits.cfg_id21 = cfg_id21;
	reg.bits.cfg_id22 = cfg_id22;
	reg.bits.cfg_id23 = cfg_id23;
	reg.bits.cfg_id24 = cfg_id24;
	reg.bits.cfg_id25 = cfg_id25;
	reg.bits.cfg_id26 = cfg_id26;
	reg.bits.cfg_id27 = cfg_id27;
	reg.bits.cfg_id28 = cfg_id28;
	reg.bits.cfg_id29 = cfg_id29;
	reg.bits.cfg_id30 = cfg_id30;
	reg.bits.cfg_id31 = cfg_id31;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID16
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id16, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID17
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id17, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID18
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id18, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID19
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id19, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID20
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id20, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID21
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id21, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID22
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id22, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID23
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id23, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID24
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id24, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID25
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id25, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID26
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id26, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID27
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id27, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID28
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id28, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID29
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id29, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID30
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id30, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR1_CFG_ID31
fun_set_mod_reg_bit(gicd, icfgr1, cfg_id31, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2
void set_gicd_icfgr2(u32 reg_addr,
			u32 cfg_id32,
			u32 cfg_id33,
			u32 cfg_id34,
			u32 cfg_id35,
			u32 cfg_id36,
			u32 cfg_id37,
			u32 cfg_id38,
			u32 cfg_id39,
			u32 cfg_id40,
			u32 cfg_id41,
			u32 cfg_id42,
			u32 cfg_id43,
			u32 cfg_id44,
			u32 cfg_id45,
			u32 cfg_id46,
			u32 cfg_id47,
			u32 m_or_r)
{
	reg_gicd_icfgr2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id32 = cfg_id32;
	reg.bits.cfg_id33 = cfg_id33;
	reg.bits.cfg_id34 = cfg_id34;
	reg.bits.cfg_id35 = cfg_id35;
	reg.bits.cfg_id36 = cfg_id36;
	reg.bits.cfg_id37 = cfg_id37;
	reg.bits.cfg_id38 = cfg_id38;
	reg.bits.cfg_id39 = cfg_id39;
	reg.bits.cfg_id40 = cfg_id40;
	reg.bits.cfg_id41 = cfg_id41;
	reg.bits.cfg_id42 = cfg_id42;
	reg.bits.cfg_id43 = cfg_id43;
	reg.bits.cfg_id44 = cfg_id44;
	reg.bits.cfg_id45 = cfg_id45;
	reg.bits.cfg_id46 = cfg_id46;
	reg.bits.cfg_id47 = cfg_id47;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID32
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id32, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID33
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id33, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID34
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id34, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID35
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id35, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID36
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id36, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID37
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id37, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID38
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id38, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID39
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id39, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID40
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id40, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID41
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id41, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID42
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id42, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID43
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id43, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID44
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id44, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID45
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id45, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID46
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id46, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR2_CFG_ID47
fun_set_mod_reg_bit(gicd, icfgr2, cfg_id47, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3
void set_gicd_icfgr3(u32 reg_addr,
			u32 cfg_id48,
			u32 cfg_id49,
			u32 cfg_id50,
			u32 cfg_id51,
			u32 cfg_id52,
			u32 cfg_id53,
			u32 cfg_id54,
			u32 cfg_id55,
			u32 cfg_id56,
			u32 cfg_id57,
			u32 cfg_id58,
			u32 cfg_id59,
			u32 cfg_id60,
			u32 cfg_id61,
			u32 cfg_id62,
			u32 cfg_id63,
			u32 m_or_r)
{
	reg_gicd_icfgr3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id48 = cfg_id48;
	reg.bits.cfg_id49 = cfg_id49;
	reg.bits.cfg_id50 = cfg_id50;
	reg.bits.cfg_id51 = cfg_id51;
	reg.bits.cfg_id52 = cfg_id52;
	reg.bits.cfg_id53 = cfg_id53;
	reg.bits.cfg_id54 = cfg_id54;
	reg.bits.cfg_id55 = cfg_id55;
	reg.bits.cfg_id56 = cfg_id56;
	reg.bits.cfg_id57 = cfg_id57;
	reg.bits.cfg_id58 = cfg_id58;
	reg.bits.cfg_id59 = cfg_id59;
	reg.bits.cfg_id60 = cfg_id60;
	reg.bits.cfg_id61 = cfg_id61;
	reg.bits.cfg_id62 = cfg_id62;
	reg.bits.cfg_id63 = cfg_id63;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID48
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id48, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID49
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id49, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID50
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id50, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID51
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id51, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID52
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id52, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID53
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id53, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID54
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id54, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID55
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id55, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID56
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id56, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID57
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id57, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID58
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id58, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID59
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id59, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID60
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id60, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID61
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id61, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID62
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id62, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR3_CFG_ID63
fun_set_mod_reg_bit(gicd, icfgr3, cfg_id63, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4
void set_gicd_icfgr4(u32 reg_addr,
			u32 cfg_id64,
			u32 cfg_id65,
			u32 cfg_id66,
			u32 cfg_id67,
			u32 cfg_id68,
			u32 cfg_id69,
			u32 cfg_id70,
			u32 cfg_id71,
			u32 cfg_id72,
			u32 cfg_id73,
			u32 cfg_id74,
			u32 cfg_id75,
			u32 cfg_id76,
			u32 cfg_id77,
			u32 cfg_id78,
			u32 cfg_id79,
			u32 m_or_r)
{
	reg_gicd_icfgr4_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id64 = cfg_id64;
	reg.bits.cfg_id65 = cfg_id65;
	reg.bits.cfg_id66 = cfg_id66;
	reg.bits.cfg_id67 = cfg_id67;
	reg.bits.cfg_id68 = cfg_id68;
	reg.bits.cfg_id69 = cfg_id69;
	reg.bits.cfg_id70 = cfg_id70;
	reg.bits.cfg_id71 = cfg_id71;
	reg.bits.cfg_id72 = cfg_id72;
	reg.bits.cfg_id73 = cfg_id73;
	reg.bits.cfg_id74 = cfg_id74;
	reg.bits.cfg_id75 = cfg_id75;
	reg.bits.cfg_id76 = cfg_id76;
	reg.bits.cfg_id77 = cfg_id77;
	reg.bits.cfg_id78 = cfg_id78;
	reg.bits.cfg_id79 = cfg_id79;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID64
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id64, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID65
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id65, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID66
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id66, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID67
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id67, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID68
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id68, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID69
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id69, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID70
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id70, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID71
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id71, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID72
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id72, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID73
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id73, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID74
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id74, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID75
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id75, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID76
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id76, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID77
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id77, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID78
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id78, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR4_CFG_ID79
fun_set_mod_reg_bit(gicd, icfgr4, cfg_id79, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5
void set_gicd_icfgr5(u32 reg_addr,
			u32 cfg_id80,
			u32 cfg_id81,
			u32 cfg_id82,
			u32 cfg_id83,
			u32 cfg_id84,
			u32 cfg_id85,
			u32 cfg_id86,
			u32 cfg_id87,
			u32 cfg_id88,
			u32 cfg_id89,
			u32 cfg_id90,
			u32 cfg_id91,
			u32 cfg_id92,
			u32 cfg_id93,
			u32 cfg_id94,
			u32 cfg_id95,
			u32 m_or_r)
{
	reg_gicd_icfgr5_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id80 = cfg_id80;
	reg.bits.cfg_id81 = cfg_id81;
	reg.bits.cfg_id82 = cfg_id82;
	reg.bits.cfg_id83 = cfg_id83;
	reg.bits.cfg_id84 = cfg_id84;
	reg.bits.cfg_id85 = cfg_id85;
	reg.bits.cfg_id86 = cfg_id86;
	reg.bits.cfg_id87 = cfg_id87;
	reg.bits.cfg_id88 = cfg_id88;
	reg.bits.cfg_id89 = cfg_id89;
	reg.bits.cfg_id90 = cfg_id90;
	reg.bits.cfg_id91 = cfg_id91;
	reg.bits.cfg_id92 = cfg_id92;
	reg.bits.cfg_id93 = cfg_id93;
	reg.bits.cfg_id94 = cfg_id94;
	reg.bits.cfg_id95 = cfg_id95;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID80
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id80, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID81
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id81, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID82
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id82, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID83
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id83, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID84
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id84, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID85
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id85, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID86
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id86, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID87
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id87, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID88
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id88, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID89
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id89, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID90
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id90, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID91
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id91, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID92
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id92, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID93
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id93, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID94
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id94, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR5_CFG_ID95
fun_set_mod_reg_bit(gicd, icfgr5, cfg_id95, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6
void set_gicd_icfgr6(u32 reg_addr,
			u32 cfg_id96,
			u32 cfg_id97,
			u32 cfg_id98,
			u32 cfg_id99,
			u32 cfg_id100,
			u32 cfg_id101,
			u32 cfg_id102,
			u32 cfg_id103,
			u32 cfg_id104,
			u32 cfg_id105,
			u32 cfg_id106,
			u32 cfg_id107,
			u32 cfg_id108,
			u32 cfg_id109,
			u32 cfg_id110,
			u32 cfg_id111,
			u32 m_or_r)
{
	reg_gicd_icfgr6_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id96 = cfg_id96;
	reg.bits.cfg_id97 = cfg_id97;
	reg.bits.cfg_id98 = cfg_id98;
	reg.bits.cfg_id99 = cfg_id99;
	reg.bits.cfg_id100 = cfg_id100;
	reg.bits.cfg_id101 = cfg_id101;
	reg.bits.cfg_id102 = cfg_id102;
	reg.bits.cfg_id103 = cfg_id103;
	reg.bits.cfg_id104 = cfg_id104;
	reg.bits.cfg_id105 = cfg_id105;
	reg.bits.cfg_id106 = cfg_id106;
	reg.bits.cfg_id107 = cfg_id107;
	reg.bits.cfg_id108 = cfg_id108;
	reg.bits.cfg_id109 = cfg_id109;
	reg.bits.cfg_id110 = cfg_id110;
	reg.bits.cfg_id111 = cfg_id111;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID96
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id96, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID97
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id97, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID98
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id98, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID99
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id99, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID100
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id100, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID101
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id101, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID102
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id102, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID103
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id103, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID104
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id104, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID105
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id105, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID106
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id106, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID107
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id107, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID108
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id108, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID109
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id109, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID110
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id110, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR6_CFG_ID111
fun_set_mod_reg_bit(gicd, icfgr6, cfg_id111, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7
void set_gicd_icfgr7(u32 reg_addr,
			u32 cfg_id112,
			u32 cfg_id113,
			u32 cfg_id114,
			u32 cfg_id115,
			u32 cfg_id116,
			u32 cfg_id117,
			u32 cfg_id118,
			u32 cfg_id119,
			u32 cfg_id120,
			u32 cfg_id121,
			u32 cfg_id122,
			u32 cfg_id123,
			u32 cfg_id124,
			u32 cfg_id125,
			u32 cfg_id126,
			u32 cfg_id127,
			u32 m_or_r)
{
	reg_gicd_icfgr7_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id112 = cfg_id112;
	reg.bits.cfg_id113 = cfg_id113;
	reg.bits.cfg_id114 = cfg_id114;
	reg.bits.cfg_id115 = cfg_id115;
	reg.bits.cfg_id116 = cfg_id116;
	reg.bits.cfg_id117 = cfg_id117;
	reg.bits.cfg_id118 = cfg_id118;
	reg.bits.cfg_id119 = cfg_id119;
	reg.bits.cfg_id120 = cfg_id120;
	reg.bits.cfg_id121 = cfg_id121;
	reg.bits.cfg_id122 = cfg_id122;
	reg.bits.cfg_id123 = cfg_id123;
	reg.bits.cfg_id124 = cfg_id124;
	reg.bits.cfg_id125 = cfg_id125;
	reg.bits.cfg_id126 = cfg_id126;
	reg.bits.cfg_id127 = cfg_id127;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID112
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id112, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID113
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id113, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID114
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id114, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID115
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id115, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID116
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id116, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID117
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id117, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID118
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id118, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID119
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id119, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID120
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id120, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID121
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id121, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID122
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id122, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID123
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id123, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID124
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id124, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID125
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id125, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID126
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id126, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR7_CFG_ID127
fun_set_mod_reg_bit(gicd, icfgr7, cfg_id127, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8
void set_gicd_icfgr8(u32 reg_addr,
			u32 cfg_id128,
			u32 cfg_id129,
			u32 cfg_id130,
			u32 cfg_id131,
			u32 cfg_id132,
			u32 cfg_id133,
			u32 cfg_id134,
			u32 cfg_id135,
			u32 cfg_id136,
			u32 cfg_id137,
			u32 cfg_id138,
			u32 cfg_id139,
			u32 cfg_id140,
			u32 cfg_id141,
			u32 cfg_id142,
			u32 cfg_id143,
			u32 m_or_r)
{
	reg_gicd_icfgr8_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id128 = cfg_id128;
	reg.bits.cfg_id129 = cfg_id129;
	reg.bits.cfg_id130 = cfg_id130;
	reg.bits.cfg_id131 = cfg_id131;
	reg.bits.cfg_id132 = cfg_id132;
	reg.bits.cfg_id133 = cfg_id133;
	reg.bits.cfg_id134 = cfg_id134;
	reg.bits.cfg_id135 = cfg_id135;
	reg.bits.cfg_id136 = cfg_id136;
	reg.bits.cfg_id137 = cfg_id137;
	reg.bits.cfg_id138 = cfg_id138;
	reg.bits.cfg_id139 = cfg_id139;
	reg.bits.cfg_id140 = cfg_id140;
	reg.bits.cfg_id141 = cfg_id141;
	reg.bits.cfg_id142 = cfg_id142;
	reg.bits.cfg_id143 = cfg_id143;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID128
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id128, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID129
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id129, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID130
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id130, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID131
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id131, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID132
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id132, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID133
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id133, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID134
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id134, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID135
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id135, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID136
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id136, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID137
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id137, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID138
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id138, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID139
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id139, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID140
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id140, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID141
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id141, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID142
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id142, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR8_CFG_ID143
fun_set_mod_reg_bit(gicd, icfgr8, cfg_id143, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9
void set_gicd_icfgr9(u32 reg_addr,
			u32 cfg_id144,
			u32 cfg_id145,
			u32 cfg_id146,
			u32 cfg_id147,
			u32 cfg_id148,
			u32 cfg_id149,
			u32 cfg_id150,
			u32 cfg_id151,
			u32 cfg_id152,
			u32 cfg_id153,
			u32 cfg_id154,
			u32 cfg_id155,
			u32 cfg_id156,
			u32 cfg_id157,
			u32 cfg_id158,
			u32 cfg_id159,
			u32 m_or_r)
{
	reg_gicd_icfgr9_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id144 = cfg_id144;
	reg.bits.cfg_id145 = cfg_id145;
	reg.bits.cfg_id146 = cfg_id146;
	reg.bits.cfg_id147 = cfg_id147;
	reg.bits.cfg_id148 = cfg_id148;
	reg.bits.cfg_id149 = cfg_id149;
	reg.bits.cfg_id150 = cfg_id150;
	reg.bits.cfg_id151 = cfg_id151;
	reg.bits.cfg_id152 = cfg_id152;
	reg.bits.cfg_id153 = cfg_id153;
	reg.bits.cfg_id154 = cfg_id154;
	reg.bits.cfg_id155 = cfg_id155;
	reg.bits.cfg_id156 = cfg_id156;
	reg.bits.cfg_id157 = cfg_id157;
	reg.bits.cfg_id158 = cfg_id158;
	reg.bits.cfg_id159 = cfg_id159;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID144
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id144, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID145
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id145, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID146
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id146, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID147
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id147, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID148
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id148, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID149
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id149, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID150
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id150, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID151
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id151, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID152
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id152, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID153
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id153, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID154
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id154, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID155
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id155, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID156
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id156, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID157
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id157, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID158
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id158, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR9_CFG_ID159
fun_set_mod_reg_bit(gicd, icfgr9, cfg_id159, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10
void set_gicd_icfgr10(u32 reg_addr,
			u32 cfg_id160,
			u32 cfg_id161,
			u32 cfg_id162,
			u32 cfg_id163,
			u32 cfg_id164,
			u32 cfg_id165,
			u32 cfg_id166,
			u32 cfg_id167,
			u32 cfg_id168,
			u32 cfg_id169,
			u32 cfg_id170,
			u32 cfg_id171,
			u32 cfg_id172,
			u32 cfg_id173,
			u32 cfg_id174,
			u32 cfg_id175,
			u32 m_or_r)
{
	reg_gicd_icfgr10_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id160 = cfg_id160;
	reg.bits.cfg_id161 = cfg_id161;
	reg.bits.cfg_id162 = cfg_id162;
	reg.bits.cfg_id163 = cfg_id163;
	reg.bits.cfg_id164 = cfg_id164;
	reg.bits.cfg_id165 = cfg_id165;
	reg.bits.cfg_id166 = cfg_id166;
	reg.bits.cfg_id167 = cfg_id167;
	reg.bits.cfg_id168 = cfg_id168;
	reg.bits.cfg_id169 = cfg_id169;
	reg.bits.cfg_id170 = cfg_id170;
	reg.bits.cfg_id171 = cfg_id171;
	reg.bits.cfg_id172 = cfg_id172;
	reg.bits.cfg_id173 = cfg_id173;
	reg.bits.cfg_id174 = cfg_id174;
	reg.bits.cfg_id175 = cfg_id175;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID160
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id160, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID161
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id161, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID162
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id162, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID163
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id163, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID164
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id164, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID165
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id165, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID166
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id166, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID167
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id167, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID168
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id168, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID169
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id169, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID170
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id170, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID171
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id171, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID172
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id172, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID173
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id173, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID174
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id174, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR10_CFG_ID175
fun_set_mod_reg_bit(gicd, icfgr10, cfg_id175, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11
void set_gicd_icfgr11(u32 reg_addr,
			u32 cfg_id176,
			u32 cfg_id177,
			u32 cfg_id178,
			u32 cfg_id179,
			u32 cfg_id180,
			u32 cfg_id181,
			u32 cfg_id182,
			u32 cfg_id183,
			u32 cfg_id184,
			u32 cfg_id185,
			u32 cfg_id186,
			u32 cfg_id187,
			u32 cfg_id188,
			u32 cfg_id189,
			u32 cfg_id190,
			u32 cfg_id191,
			u32 m_or_r)
{
	reg_gicd_icfgr11_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id176 = cfg_id176;
	reg.bits.cfg_id177 = cfg_id177;
	reg.bits.cfg_id178 = cfg_id178;
	reg.bits.cfg_id179 = cfg_id179;
	reg.bits.cfg_id180 = cfg_id180;
	reg.bits.cfg_id181 = cfg_id181;
	reg.bits.cfg_id182 = cfg_id182;
	reg.bits.cfg_id183 = cfg_id183;
	reg.bits.cfg_id184 = cfg_id184;
	reg.bits.cfg_id185 = cfg_id185;
	reg.bits.cfg_id186 = cfg_id186;
	reg.bits.cfg_id187 = cfg_id187;
	reg.bits.cfg_id188 = cfg_id188;
	reg.bits.cfg_id189 = cfg_id189;
	reg.bits.cfg_id190 = cfg_id190;
	reg.bits.cfg_id191 = cfg_id191;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID176
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id176, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID177
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id177, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID178
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id178, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID179
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id179, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID180
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id180, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID181
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id181, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID182
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id182, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID183
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id183, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID184
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id184, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID185
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id185, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID186
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id186, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID187
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id187, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID188
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id188, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID189
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id189, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID190
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id190, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR11_CFG_ID191
fun_set_mod_reg_bit(gicd, icfgr11, cfg_id191, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12
void set_gicd_icfgr12(u32 reg_addr,
			u32 cfg_id192,
			u32 cfg_id193,
			u32 cfg_id194,
			u32 cfg_id195,
			u32 cfg_id196,
			u32 cfg_id197,
			u32 cfg_id198,
			u32 cfg_id199,
			u32 cfg_id200,
			u32 cfg_id201,
			u32 cfg_id202,
			u32 cfg_id203,
			u32 cfg_id204,
			u32 cfg_id205,
			u32 cfg_id206,
			u32 cfg_id207,
			u32 m_or_r)
{
	reg_gicd_icfgr12_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id192 = cfg_id192;
	reg.bits.cfg_id193 = cfg_id193;
	reg.bits.cfg_id194 = cfg_id194;
	reg.bits.cfg_id195 = cfg_id195;
	reg.bits.cfg_id196 = cfg_id196;
	reg.bits.cfg_id197 = cfg_id197;
	reg.bits.cfg_id198 = cfg_id198;
	reg.bits.cfg_id199 = cfg_id199;
	reg.bits.cfg_id200 = cfg_id200;
	reg.bits.cfg_id201 = cfg_id201;
	reg.bits.cfg_id202 = cfg_id202;
	reg.bits.cfg_id203 = cfg_id203;
	reg.bits.cfg_id204 = cfg_id204;
	reg.bits.cfg_id205 = cfg_id205;
	reg.bits.cfg_id206 = cfg_id206;
	reg.bits.cfg_id207 = cfg_id207;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID192
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id192, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID193
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id193, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID194
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id194, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID195
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id195, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID196
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id196, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID197
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id197, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID198
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id198, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID199
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id199, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID200
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id200, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID201
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id201, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID202
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id202, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID203
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id203, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID204
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id204, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID205
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id205, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID206
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id206, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR12_CFG_ID207
fun_set_mod_reg_bit(gicd, icfgr12, cfg_id207, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13
void set_gicd_icfgr13(u32 reg_addr,
			u32 cfg_id208,
			u32 cfg_id209,
			u32 cfg_id210,
			u32 cfg_id211,
			u32 cfg_id212,
			u32 cfg_id213,
			u32 cfg_id214,
			u32 cfg_id215,
			u32 cfg_id216,
			u32 cfg_id217,
			u32 cfg_id218,
			u32 cfg_id219,
			u32 cfg_id220,
			u32 cfg_id221,
			u32 cfg_id222,
			u32 cfg_id223,
			u32 m_or_r)
{
	reg_gicd_icfgr13_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.cfg_id208 = cfg_id208;
	reg.bits.cfg_id209 = cfg_id209;
	reg.bits.cfg_id210 = cfg_id210;
	reg.bits.cfg_id211 = cfg_id211;
	reg.bits.cfg_id212 = cfg_id212;
	reg.bits.cfg_id213 = cfg_id213;
	reg.bits.cfg_id214 = cfg_id214;
	reg.bits.cfg_id215 = cfg_id215;
	reg.bits.cfg_id216 = cfg_id216;
	reg.bits.cfg_id217 = cfg_id217;
	reg.bits.cfg_id218 = cfg_id218;
	reg.bits.cfg_id219 = cfg_id219;
	reg.bits.cfg_id220 = cfg_id220;
	reg.bits.cfg_id221 = cfg_id221;
	reg.bits.cfg_id222 = cfg_id222;
	reg.bits.cfg_id223 = cfg_id223;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID208
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id208, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID209
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id209, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID210
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id210, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID211
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id211, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID212
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id212, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID213
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id213, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID214
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id214, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID215
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id215, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID216
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id216, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID217
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id217, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID218
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id218, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID219
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id219, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID220
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id220, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID221
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id221, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID222
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id222, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_ICFGR13_CFG_ID223
fun_set_mod_reg_bit(gicd, icfgr13, cfg_id223, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SGIR
void set_gicd_sgir(u32 reg_addr,
		u32 sgiintid,
		u32 nsatt,
		u32 cputargetlist,
		u32 targetlistfilter,
		u32 m_or_r)
{
	reg_gicd_sgir_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.sgiintid = sgiintid;
	reg.bits.nsatt = nsatt;
	reg.bits.cputargetlist = cputargetlist;
	reg.bits.targetlistfilter = targetlistfilter;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_SGIR_SGIINTID
fun_set_mod_reg_bit(gicd, sgir, sgiintid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SGIR_NSATT
fun_set_mod_reg_bit(gicd, sgir, nsatt, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SGIR_CPUTARGETLIST
fun_set_mod_reg_bit(gicd, sgir, cputargetlist, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SGIR_TARGETLISTFILTER
fun_set_mod_reg_bit(gicd, sgir, targetlistfilter, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR0
void set_gicd_cpendsgir0(u32 reg_addr,
			u32 clr_pend_id0,
			u32 clr_pend_id1,
			u32 clr_pend_id2,
			u32 clr_pend_id3,
			u32 m_or_r)
{
	reg_gicd_cpendsgir0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id0 = clr_pend_id0;
	reg.bits.clr_pend_id1 = clr_pend_id1;
	reg.bits.clr_pend_id2 = clr_pend_id2;
	reg.bits.clr_pend_id3 = clr_pend_id3;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_CPENDSGIR0_CLR_PEND_ID0
fun_set_mod_reg_bit(gicd, cpendsgir0, clr_pend_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR0_CLR_PEND_ID1
fun_set_mod_reg_bit(gicd, cpendsgir0, clr_pend_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR0_CLR_PEND_ID2
fun_set_mod_reg_bit(gicd, cpendsgir0, clr_pend_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR0_CLR_PEND_ID3
fun_set_mod_reg_bit(gicd, cpendsgir0, clr_pend_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR1
void set_gicd_cpendsgir1(u32 reg_addr,
			u32 clr_pend_id4,
			u32 clr_pend_id5,
			u32 clr_pend_id6,
			u32 clr_pend_id7,
			u32 m_or_r)
{
	reg_gicd_cpendsgir1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id4 = clr_pend_id4;
	reg.bits.clr_pend_id5 = clr_pend_id5;
	reg.bits.clr_pend_id6 = clr_pend_id6;
	reg.bits.clr_pend_id7 = clr_pend_id7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_CPENDSGIR1_CLR_PEND_ID4
fun_set_mod_reg_bit(gicd, cpendsgir1, clr_pend_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR1_CLR_PEND_ID5
fun_set_mod_reg_bit(gicd, cpendsgir1, clr_pend_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR1_CLR_PEND_ID6
fun_set_mod_reg_bit(gicd, cpendsgir1, clr_pend_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR1_CLR_PEND_ID7
fun_set_mod_reg_bit(gicd, cpendsgir1, clr_pend_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR2
void set_gicd_cpendsgir2(u32 reg_addr,
			u32 clr_pend_id8,
			u32 clr_pend_id9,
			u32 clr_pend_id10,
			u32 clr_pend_id11,
			u32 m_or_r)
{
	reg_gicd_cpendsgir2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id8 = clr_pend_id8;
	reg.bits.clr_pend_id9 = clr_pend_id9;
	reg.bits.clr_pend_id10 = clr_pend_id10;
	reg.bits.clr_pend_id11 = clr_pend_id11;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_CPENDSGIR2_CLR_PEND_ID8
fun_set_mod_reg_bit(gicd, cpendsgir2, clr_pend_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR2_CLR_PEND_ID9
fun_set_mod_reg_bit(gicd, cpendsgir2, clr_pend_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR2_CLR_PEND_ID10
fun_set_mod_reg_bit(gicd, cpendsgir2, clr_pend_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR2_CLR_PEND_ID11
fun_set_mod_reg_bit(gicd, cpendsgir2, clr_pend_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR3
void set_gicd_cpendsgir3(u32 reg_addr,
			u32 clr_pend_id12,
			u32 clr_pend_id13,
			u32 clr_pend_id14,
			u32 clr_pend_id15,
			u32 m_or_r)
{
	reg_gicd_cpendsgir3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.clr_pend_id12 = clr_pend_id12;
	reg.bits.clr_pend_id13 = clr_pend_id13;
	reg.bits.clr_pend_id14 = clr_pend_id14;
	reg.bits.clr_pend_id15 = clr_pend_id15;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_CPENDSGIR3_CLR_PEND_ID12
fun_set_mod_reg_bit(gicd, cpendsgir3, clr_pend_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR3_CLR_PEND_ID13
fun_set_mod_reg_bit(gicd, cpendsgir3, clr_pend_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR3_CLR_PEND_ID14
fun_set_mod_reg_bit(gicd, cpendsgir3, clr_pend_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_CPENDSGIR3_CLR_PEND_ID15
fun_set_mod_reg_bit(gicd, cpendsgir3, clr_pend_id15, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR0
void set_gicd_spendsgir0(u32 reg_addr,
			u32 set_pend_id0,
			u32 set_pend_id1,
			u32 set_pend_id2,
			u32 set_pend_id3,
			u32 m_or_r)
{
	reg_gicd_spendsgir0_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id0 = set_pend_id0;
	reg.bits.set_pend_id1 = set_pend_id1;
	reg.bits.set_pend_id2 = set_pend_id2;
	reg.bits.set_pend_id3 = set_pend_id3;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_SPENDSGIR0_SET_PEND_ID0
fun_set_mod_reg_bit(gicd, spendsgir0, set_pend_id0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR0_SET_PEND_ID1
fun_set_mod_reg_bit(gicd, spendsgir0, set_pend_id1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR0_SET_PEND_ID2
fun_set_mod_reg_bit(gicd, spendsgir0, set_pend_id2, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR0_SET_PEND_ID3
fun_set_mod_reg_bit(gicd, spendsgir0, set_pend_id3, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR1
void set_gicd_spendsgir1(u32 reg_addr,
			u32 set_pend_id4,
			u32 set_pend_id5,
			u32 set_pend_id6,
			u32 set_pend_id7,
			u32 m_or_r)
{
	reg_gicd_spendsgir1_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id4 = set_pend_id4;
	reg.bits.set_pend_id5 = set_pend_id5;
	reg.bits.set_pend_id6 = set_pend_id6;
	reg.bits.set_pend_id7 = set_pend_id7;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_SPENDSGIR1_SET_PEND_ID4
fun_set_mod_reg_bit(gicd, spendsgir1, set_pend_id4, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR1_SET_PEND_ID5
fun_set_mod_reg_bit(gicd, spendsgir1, set_pend_id5, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR1_SET_PEND_ID6
fun_set_mod_reg_bit(gicd, spendsgir1, set_pend_id6, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR1_SET_PEND_ID7
fun_set_mod_reg_bit(gicd, spendsgir1, set_pend_id7, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR2
void set_gicd_spendsgir2(u32 reg_addr,
			u32 set_pend_id8,
			u32 set_pend_id9,
			u32 set_pend_id10,
			u32 set_pend_id11,
			u32 m_or_r)
{
	reg_gicd_spendsgir2_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id8 = set_pend_id8;
	reg.bits.set_pend_id9 = set_pend_id9;
	reg.bits.set_pend_id10 = set_pend_id10;
	reg.bits.set_pend_id11 = set_pend_id11;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_SPENDSGIR2_SET_PEND_ID8
fun_set_mod_reg_bit(gicd, spendsgir2, set_pend_id8, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR2_SET_PEND_ID9
fun_set_mod_reg_bit(gicd, spendsgir2, set_pend_id9, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR2_SET_PEND_ID10
fun_set_mod_reg_bit(gicd, spendsgir2, set_pend_id10, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR2_SET_PEND_ID11
fun_set_mod_reg_bit(gicd, spendsgir2, set_pend_id11, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR3
void set_gicd_spendsgir3(u32 reg_addr,
			u32 set_pend_id12,
			u32 set_pend_id13,
			u32 set_pend_id14,
			u32 set_pend_id15,
			u32 m_or_r)
{
	reg_gicd_spendsgir3_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.set_pend_id12 = set_pend_id12;
	reg.bits.set_pend_id13 = set_pend_id13;
	reg.bits.set_pend_id14 = set_pend_id14;
	reg.bits.set_pend_id15 = set_pend_id15;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICD_SPENDSGIR3_SET_PEND_ID12
fun_set_mod_reg_bit(gicd, spendsgir3, set_pend_id12, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR3_SET_PEND_ID13
fun_set_mod_reg_bit(gicd, spendsgir3, set_pend_id13, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR3_SET_PEND_ID14
fun_set_mod_reg_bit(gicd, spendsgir3, set_pend_id14, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICD_SPENDSGIR3_SET_PEND_ID15
fun_set_mod_reg_bit(gicd, spendsgir3, set_pend_id15, addr, val, m_or_r)
#endif

#endif /* ___GICD__SET___C___ */
