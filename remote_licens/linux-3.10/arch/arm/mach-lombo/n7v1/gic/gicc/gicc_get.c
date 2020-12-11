/* gicc_get.c */

#ifndef ___GICC__GET___C___
#define ___GICC__GET___C___

#ifdef DEF_GET_GICC_CTLR_ENABLEGRP0
fun_get_mod_reg_bit(gicc, ctlr, enablegrp0, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_ENABLEGRP1
fun_get_mod_reg_bit(gicc, ctlr, enablegrp1, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_ACKCTL
fun_get_mod_reg_bit(gicc, ctlr, ackctl, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_FIQEN
fun_get_mod_reg_bit(gicc, ctlr, fiqen, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_CBPR
fun_get_mod_reg_bit(gicc, ctlr, cbpr, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_FIQBYPDISGRP0
fun_get_mod_reg_bit(gicc, ctlr, fiqbypdisgrp0, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_IRQBYPDISGRP0
fun_get_mod_reg_bit(gicc, ctlr, irqbypdisgrp0, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_FIQBYPDISGRP1
fun_get_mod_reg_bit(gicc, ctlr, fiqbypdisgrp1, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_IRQBYPDISGRP1
fun_get_mod_reg_bit(gicc, ctlr, irqbypdisgrp1, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_EOIMODES
fun_get_mod_reg_bit(gicc, ctlr, eoimodes, addr)
#endif

#ifdef DEF_GET_GICC_CTLR_EOIMODENS
fun_get_mod_reg_bit(gicc, ctlr, eoimodens, addr)
#endif

#ifdef DEF_GET_GICC_PMR_PRIORITY
fun_get_mod_reg_bit(gicc, pmr, priority, addr)
#endif

#ifdef DEF_GET_GICC_BPR_BINARYPOINT
fun_get_mod_reg_bit(gicc, bpr, binarypoint, addr)
#endif

#ifdef DEF_GET_GICC_IAR_INTERRUPTID
fun_get_mod_reg_bit(gicc, iar, interruptid, addr)
#endif

#ifdef DEF_GET_GICC_IAR_CPUID
fun_get_mod_reg_bit(gicc, iar, cpuid, addr)
#endif

#ifdef DEF_GET_GICC_EOIR_EOIINTID
fun_get_mod_reg_bit(gicc, eoir, eoiintid, addr)
#endif

#ifdef DEF_GET_GICC_EOIR_CPUID
fun_get_mod_reg_bit(gicc, eoir, cpuid, addr)
#endif

#ifdef DEF_GET_GICC_RPR_PRIORITY
fun_get_mod_reg_bit(gicc, rpr, priority, addr)
#endif

#ifdef DEF_GET_GICC_HPPIR_PENDINTID
fun_get_mod_reg_bit(gicc, hppir, pendintid, addr)
#endif

#ifdef DEF_GET_GICC_HPPIR_CPUID
fun_get_mod_reg_bit(gicc, hppir, cpuid, addr)
#endif

#ifdef DEF_GET_GICC_ABPR_BINARYPOINT
fun_get_mod_reg_bit(gicc, abpr, binarypoint, addr)
#endif

#ifdef DEF_GET_GICC_AIAR_INTERRUPTID
fun_get_mod_reg_bit(gicc, aiar, interruptid, addr)
#endif

#ifdef DEF_GET_GICC_AIAR_CPUID
fun_get_mod_reg_bit(gicc, aiar, cpuid, addr)
#endif

#ifdef DEF_GET_GICC_AEOIR_EOIINTID
fun_get_mod_reg_bit(gicc, aeoir, eoiintid, addr)
#endif

#ifdef DEF_GET_GICC_AEOIR_CPUID
fun_get_mod_reg_bit(gicc, aeoir, cpuid, addr)
#endif

#ifdef DEF_GET_GICC_AHPPIR_PENDINTID
fun_get_mod_reg_bit(gicc, ahppir, pendintid, addr)
#endif

#ifdef DEF_GET_GICC_AHPPIR_CPUID
fun_get_mod_reg_bit(gicc, ahppir, cpuid, addr)
#endif

#ifdef DEF_GET_GICC_DIR_PENDINTID
fun_get_mod_reg_bit(gicc, dir, pendintid, addr)
#endif

#ifdef DEF_GET_GICC_DIR_CPUID
fun_get_mod_reg_bit(gicc, dir, cpuid, addr)
#endif

#endif /* ___GICC__GET___C___ */
