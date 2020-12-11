#ifndef __VU_DRV__
#define __VU_DRV__

#include "ax_reg.h"
#include "utils.h"
/* CVU STATUS */
enum {
	CVU_IDLE,
	CVU_BUSY,
	CVU_PAUSED,
	CVU_STOPED,
	CVU_ERR
};

/* CVU ERROR STATUS */
enum {
	CVU_OK = 0,
	CVU_WAIT  = -1,
	CVU_ERR_UNKOWN = -2,
	CVU_ERR_BUSY = -100,
	CVU_ERR_CFG = -101,
	CVU_ERR_TIMEOUT = -203
};



typedef struct {
	int x;
	int y;
	int w;
	int h;
} nv_rect_t;

typedef struct {
	int iw;
	int ih;
	int istride;
	int fmt_in;
	unsigned int phyaddrin;
	unsigned int phyaddrin_uv;
	nv_rect_t rect;
	int scale_x;
	int scale_y;
	int fmt_out;
	int ow;
	int oh;
	int oc;
	unsigned int phyaddrout;
} rsz_cfg_t;

typedef struct {
	int iw;
	int ih;
	int istride;
	int fmt_in;
	unsigned int phyaddrin;
	unsigned int phyaddrin_uv;
	int pym_layer;
	int fmt_out;
	int ow[8];
	int oh[8];
	unsigned int phyaddrout;
} pym_cfg_t;

typedef struct {
	dgb_perf_cnt_t mstat;
} cvu_stat_info_t;

#define CVU_DRV_IOC_MAGIC_NUMBER 'v'
#define CVU_LOG _IOW(CVU_DRV_IOC_MAGIC_NUMBER, 0x0, unsigned int)

#define CVU_SET_FREQ _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0x3, unsigned int)
#define CVU_GET_FREQ _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0x4, unsigned int)

#define CVU_RSZ_RUN _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0x5, rsz_cfg_t)
#define CVU_PYM_RUN _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0x6, pym_cfg_t)
#define CVU_BTNM_RUN _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0x7, rsz_cfg_t)
#define CVU_STATUS _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0x8, unsigned int)
#define CVU_QUERY_FINISH _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0x9, cvu_stat_info_t)

#define CVU_GET_VER _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0xa, unsigned int)
#define CVU_SET_REG _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0xb, ax_reg_t)
#define CVU_GET_REG _IOWR(CVU_DRV_IOC_MAGIC_NUMBER, 0xc, ax_reg_t)

#endif
