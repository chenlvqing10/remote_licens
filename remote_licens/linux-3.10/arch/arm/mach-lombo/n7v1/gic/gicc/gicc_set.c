/* gicc_set.c */

#ifndef ___GICC__SET___C___
#define ___GICC__SET___C___

#ifdef DEF_SET_GICC_CTLR
void set_gicc_ctlr(u32 reg_addr,
		u32 enablegrp0,
		u32 enablegrp1,
		u32 ackctl,
		u32 fiqen,
		u32 cbpr,
		u32 fiqbypdisgrp0,
		u32 irqbypdisgrp0,
		u32 fiqbypdisgrp1,
		u32 irqbypdisgrp1,
		u32 eoimodes,
		u32 eoimodens,
		u32 m_or_r)
{
	reg_gicc_ctlr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.enablegrp0 = enablegrp0;
	reg.bits.enablegrp1 = enablegrp1;
	reg.bits.ackctl = ackctl;
	reg.bits.fiqen = fiqen;
	reg.bits.cbpr = cbpr;
	reg.bits.fiqbypdisgrp0 = fiqbypdisgrp0;
	reg.bits.irqbypdisgrp0 = irqbypdisgrp0;
	reg.bits.fiqbypdisgrp1 = fiqbypdisgrp1;
	reg.bits.irqbypdisgrp1 = irqbypdisgrp1;
	reg.bits.eoimodes = eoimodes;
	reg.bits.eoimodens = eoimodens;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_CTLR_ENABLEGRP0
fun_set_mod_reg_bit(gicc, ctlr, enablegrp0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_ENABLEGRP1
fun_set_mod_reg_bit(gicc, ctlr, enablegrp1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_ACKCTL
fun_set_mod_reg_bit(gicc, ctlr, ackctl, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_FIQEN
fun_set_mod_reg_bit(gicc, ctlr, fiqen, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_CBPR
fun_set_mod_reg_bit(gicc, ctlr, cbpr, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_FIQBYPDISGRP0
fun_set_mod_reg_bit(gicc, ctlr, fiqbypdisgrp0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_IRQBYPDISGRP0
fun_set_mod_reg_bit(gicc, ctlr, irqbypdisgrp0, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_FIQBYPDISGRP1
fun_set_mod_reg_bit(gicc, ctlr, fiqbypdisgrp1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_IRQBYPDISGRP1
fun_set_mod_reg_bit(gicc, ctlr, irqbypdisgrp1, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_EOIMODES
fun_set_mod_reg_bit(gicc, ctlr, eoimodes, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_CTLR_EOIMODENS
fun_set_mod_reg_bit(gicc, ctlr, eoimodens, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_PMR
void set_gicc_pmr(u32 reg_addr,
		u32 priority,
		u32 m_or_r)
{
	reg_gicc_pmr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.priority = priority;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_PMR_PRIORITY
fun_set_mod_reg_bit(gicc, pmr, priority, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_BPR
void set_gicc_bpr(u32 reg_addr,
		u32 binarypoint,
		u32 m_or_r)
{
	reg_gicc_bpr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.binarypoint = binarypoint;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_BPR_BINARYPOINT
fun_set_mod_reg_bit(gicc, bpr, binarypoint, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_IAR
void set_gicc_iar(u32 reg_addr,
		u32 interruptid,
		u32 cpuid,
		u32 m_or_r)
{
	reg_gicc_iar_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.interruptid = interruptid;
	reg.bits.cpuid = cpuid;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_IAR_INTERRUPTID
fun_set_mod_reg_bit(gicc, iar, interruptid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_IAR_CPUID
fun_set_mod_reg_bit(gicc, iar, cpuid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_EOIR
void set_gicc_eoir(u32 reg_addr,
		u32 eoiintid,
		u32 cpuid,
		u32 m_or_r)
{
	reg_gicc_eoir_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.eoiintid = eoiintid;
	reg.bits.cpuid = cpuid;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_EOIR_EOIINTID
fun_set_mod_reg_bit(gicc, eoir, eoiintid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_EOIR_CPUID
fun_set_mod_reg_bit(gicc, eoir, cpuid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_RPR
void set_gicc_rpr(u32 reg_addr,
		u32 priority,
		u32 m_or_r)
{
	reg_gicc_rpr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.priority = priority;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_RPR_PRIORITY
fun_set_mod_reg_bit(gicc, rpr, priority, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_HPPIR
void set_gicc_hppir(u32 reg_addr,
			u32 pendintid,
			u32 cpuid,
			u32 m_or_r)
{
	reg_gicc_hppir_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pendintid = pendintid;
	reg.bits.cpuid = cpuid;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_HPPIR_PENDINTID
fun_set_mod_reg_bit(gicc, hppir, pendintid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_HPPIR_CPUID
fun_set_mod_reg_bit(gicc, hppir, cpuid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_ABPR
void set_gicc_abpr(u32 reg_addr,
		u32 binarypoint,
		u32 m_or_r)
{
	reg_gicc_abpr_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.binarypoint = binarypoint;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_ABPR_BINARYPOINT
fun_set_mod_reg_bit(gicc, abpr, binarypoint, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_AIAR
void set_gicc_aiar(u32 reg_addr,
		u32 interruptid,
		u32 cpuid,
		u32 m_or_r)
{
	reg_gicc_aiar_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.interruptid = interruptid;
	reg.bits.cpuid = cpuid;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_AIAR_INTERRUPTID
fun_set_mod_reg_bit(gicc, aiar, interruptid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_AIAR_CPUID
fun_set_mod_reg_bit(gicc, aiar, cpuid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_AEOIR
void set_gicc_aeoir(u32 reg_addr,
			u32 eoiintid,
			u32 cpuid,
			u32 m_or_r)
{
	reg_gicc_aeoir_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.eoiintid = eoiintid;
	reg.bits.cpuid = cpuid;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_AEOIR_EOIINTID
fun_set_mod_reg_bit(gicc, aeoir, eoiintid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_AEOIR_CPUID
fun_set_mod_reg_bit(gicc, aeoir, cpuid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_AHPPIR
void set_gicc_ahppir(u32 reg_addr,
			u32 pendintid,
			u32 cpuid,
			u32 m_or_r)
{
	reg_gicc_ahppir_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pendintid = pendintid;
	reg.bits.cpuid = cpuid;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_AHPPIR_PENDINTID
fun_set_mod_reg_bit(gicc, ahppir, pendintid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_AHPPIR_CPUID
fun_set_mod_reg_bit(gicc, ahppir, cpuid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_DIR
void set_gicc_dir(u32 reg_addr,
		u32 pendintid,
		u32 cpuid,
		u32 m_or_r)
{
	reg_gicc_dir_t reg;

	if (m_or_r == 0)
		reg.val = 0;
	else
		reg.val = READREG32(reg_addr);

	reg.bits.pendintid = pendintid;
	reg.bits.cpuid = cpuid;

	WRITEREG32(reg_addr, reg.val);
}
#endif

#ifdef DEF_SET_GICC_DIR_PENDINTID
fun_set_mod_reg_bit(gicc, dir, pendintid, addr, val, m_or_r)
#endif

#ifdef DEF_SET_GICC_DIR_CPUID
fun_set_mod_reg_bit(gicc, dir, cpuid, addr, val, m_or_r)
#endif

#endif /* ___GICC__SET___C___ */
