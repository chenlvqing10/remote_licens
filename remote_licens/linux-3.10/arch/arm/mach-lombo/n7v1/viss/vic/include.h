/* include.h */

#define DUMP_VIC

#define VA_VIC		VA_REG

/* open the definations in xxx_cfg.h if you need the function */
#include "vic_get_cfg.h"
#include "vic_set_cfg.h"

#include "vic.h"
#include "vic_get.h"
#include "vic_set.h"

#ifdef DUMP_VIC
#include "vic_dump.h"
#endif
