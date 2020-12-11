
#ifndef __LOMBO__VISS_CSP___H
#define __LOMBO__VISS_CSP___H

#ifdef CONFIG_ARCH_LOMBO_N7V0
#include "n7v0/viss-const.h"
#include "n7v0/viss_top/viss_top_csp.h"
#include "n7v0/vic/vic_csp.h"
#include "n7v0/mipi_csi/mipi_csi_csp.h"
#include "n7v0/isp/isp_csp.h"
#elif defined CONFIG_ARCH_LOMBO_N7V1
#include "n7v1/viss-const.h"
#include "n7v1/viss_top/viss_top_csp.h"
#include "n7v1/vic/vic_csp.h"
#include "n7v1/mipi_csi/mipi_csi_csp.h"
#include "n7v1/isp/isp_csp.h"
#elif defined CONFIG_ARCH_LOMBO_N9V0
#include "n9v0/viss-const.h"
#include "n9v0/viss_top/viss_top_csp.h"
#include "n9v0/vic/vic_csp.h"
#include "n9v0/mipi_csi/mipi_csi_csp.h"
#include "n9v0/isp/isp_csp.h"
#else
#error "No supported platform!"
#endif

#endif /* __LOMBO__VISS_CSP___H */
