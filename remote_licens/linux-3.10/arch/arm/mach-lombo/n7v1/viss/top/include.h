/* include.h */

#define DUMP_VISS

#define VA_VISS		VA_REG

/* open the definations in xxx_cfg.h if you need the function */
#include "viss_get_cfg.h"
#include "viss_set_cfg.h"

#include "viss.h"
#include "viss_get.h"
#include "viss_set.h"

#ifdef DUMP_VISS
#include "viss_dump.h"
#endif
