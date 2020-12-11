/* include.h */

#define DUMP_RTC

#define VA_RTC		VA_REG

/* open the definations in xxx_cfg.h if you need the function */
#include "rtc_get_cfg.h"
#include "rtc_set_cfg.h"

#include "rtc.h"
#include "rtc_get.h"
#include "rtc_set.h"

#ifdef DUMP_RTC
#include "rtc_dump.h"
#endif
