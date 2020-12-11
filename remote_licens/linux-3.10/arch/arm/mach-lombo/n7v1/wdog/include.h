/* include.h */

#define DUMP_WDOG

#define VA_WDOG		VA_REG

/* open the definations in xxx_cfg.h if you need the function */
#include "wdog_get_cfg.h"
#include "wdog_set_cfg.h"

#include "wdog.h"
#include "wdog_get.h"
#include "wdog_set.h"

#ifdef DUMP_WDOG
#include "wdog_dump.h"
#endif
