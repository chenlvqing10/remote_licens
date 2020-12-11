/* include.h */

#define DUMP_CPU_CTRL

#define VA_CPU_CTRL		VA_REG

/* open the definations in xxx_cfg.h if you need the function */
#include "cpu_ctrl_get_cfg.h"
#include "cpu_ctrl_set_cfg.h"

#include "cpu_ctrl.h"
#include "cpu_ctrl_get.h"
#include "cpu_ctrl_set.h"

#ifdef DUMP_CPU_CTRL
#include "cpu_ctrl_dump.h"
#endif
