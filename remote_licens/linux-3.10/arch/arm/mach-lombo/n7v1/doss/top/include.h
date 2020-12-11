/* include.h */

#define DUMP_DOSS

#define VA_DOSS		VA_REG

/* open the definations in xxx_cfg.h if you need the function */
#include "doss_get_cfg.h"
#include "doss_set_cfg.h"

#include "doss.h"
#include "doss_get.h"
#include "doss_set.h"

#ifdef DUMP_DOSS
#include "doss_dump.h"
#endif
