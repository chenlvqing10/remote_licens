#ifndef __NU_DRV__
#define __NU_DRV__

#include "ax_reg.h"
#include "utils.h"

/* ENU STATUS */
enum {
	ENU_IDLE,
	ENU_BUSY,
	ENU_PAUSED,
	ENU_STOPED,
	ENU_ERR
};

/* ENU ERROR STATUS */
enum {
	ENU_OK = 0,
	ENU_WAIT  = -1,
	ENU_ERR_UNKOWN = -2,
	ENU_ERR_BUSY = -100,
	ENU_ERR_CFG = -101,
	ENU_ERR_CONFLICT = -103,
	ENU_ERR_TIMEOUT = -203
};

typedef struct {
	dgb_perf_cnt_t mstat;
	unsigned int mboxes;
	int status;
} enu_stat_info_t;

/* ax control */
typedef struct {
	unsigned int mFstCmdLen;
	unsigned int mCmdNum;
	unsigned int mCmdIntL32;
	unsigned int mCmdIntH32;
	unsigned int mCmdPAUSL32;
	unsigned int mCmdPAUSH32;
	unsigned int mISREn;
	unsigned int mCmdAddr;
} enu_clt_t;

typedef struct {
	int err_no;
	unsigned int mCmdIntL32;
	unsigned int mCmdIntH32;
} enu_status_out_t;

typedef struct {
	unsigned int mCmdPAUSL32;
	unsigned int mCmdPAUSH32;
} enu_pause_flg_t;

#define ENU_DRV_IOC_MAGIC_NUMBER 'n'
#define ENU_LOG _IOW(ENU_DRV_IOC_MAGIC_NUMBER, 0x0, unsigned int)
#define ENU_SET_FREQ _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0x1, unsigned int)
#define ENU_GET_FREQ _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0x2, unsigned int)
#define ENU_SET_NORMAL_FREQ _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0x3, unsigned int)
#define ENU_GET_NORMAL_FREQ _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0x4, unsigned int)


#define ENU_RUN _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0x5, enu_clt_t)
#define ENU_STATUS _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0x6, enu_status_out_t)
#define ENU_WAIT_CONTINUE _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0x7, enu_pause_flg_t)
#define ENU_QUERY_FINISH _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0x8, enu_stat_info_t)

#define ENU_GET_VER _IOR(ENU_DRV_IOC_MAGIC_NUMBER, 0x9, unsigned int)
#define ENU_SET_REG _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0xa, ax_reg_t)
#define ENU_GET_REG _IOWR(ENU_DRV_IOC_MAGIC_NUMBER, 0xb, ax_reg_t)

#endif
