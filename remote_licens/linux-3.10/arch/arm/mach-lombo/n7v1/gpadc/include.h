/* include.h */

#define DUMP_GPADC

#define VA_GPADC		VA_REG

/* open the definations in xxx_cfg.h if you need the function */
#include "gpadc_get_cfg.h"
#include "gpadc_set_cfg.h"

#include "gpadc.h"
#include "gpadc_get.h"
#include "gpadc_set.h"

#ifdef DUMP_GPADC
#include "gpadc_dump.h"
#endif
