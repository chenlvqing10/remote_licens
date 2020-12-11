#ifndef _VDC_CORE_H_
#define _VDC_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum VDC_REG_NO {
	VDC_REG0 = 0,
	VDC_REG1,  VDC_REG2,  VDC_REG3,  VDC_REG4,  VDC_REG5,  VDC_REG6,
	VDC_REG7,  VDC_REG8,  VDC_REG9,  VDC_REG10, VDC_REG11, VDC_REG12,
	VDC_REG13, VDC_REG14, VDC_REG15, VDC_REG16, VDC_REG17, VDC_REG18,
	VDC_REG19, VDC_REG20, VDC_REG21, VDC_REG22, VDC_REG23, VDC_REG24,
	VDC_REG25, VDC_REG26, VDC_REG27, VDC_REG28, VDC_REG29, VDC_REG30,
	VDC_REG31, VDC_REG32, VDC_REG33, VDC_REG34, VDC_REG35, VDC_REG36,
	VDC_REG37, VDC_REG38, VDC_REG39, VDC_REG40, VDC_REG41, VDC_REG42,
	VDC_REG43, VDC_REG44, VDC_REG45, VDC_REG46, VDC_REG47, VDC_REG48,
	VDC_REG49, VDC_REG50, VDC_REG51, VDC_REG52, VDC_REG53, VDC_REG54,
	VDC_REG55, VDC_REG56, VDC_REG57, VDC_REG58, VDC_REG59, VDC_REG60,
	VDC_REG61, VDC_REG62, VDC_REG63, VDC_REG64, VDC_REG65, VDC_REG66,
	VDC_REG67, VDC_REG68, VDC_REG69, VDC_REG70, VDC_REG71, VDC_REG72,
	VDC_REG73, VDC_REG74, VDC_REG75, VDC_REG76, VDC_REG77, VDC_REG78,
	VDC_REG79, VDC_REG80, VDC_REG81, VDC_REG82, VDC_REG83, VDC_REG84,
	VDC_REG85, VDC_REG86, VDC_REG87, VDC_REG88, VDC_REG89, VDC_REG90,
	VDC_REG91, VDC_REG92, VDC_REG93, VDC_REG94, VDC_REG95, VDC_REG96,
	VDC_REG97, VDC_REG98, VDC_REG99, VDC_REG_MAX
} vdc_regno_t;


#define MAX_VDC_REG_NUM         (VDC_REG_MAX+1)


typedef enum VDC_STATUS {
	VDC_STATUS_IDLE                 = 0x1,
	VDC_STATUS1,
	VDC_STATUS2,
	VDC_STATUS3,
	VDC_STATUS4                     = 0x100,
	VDC_STATUS5,
	VDC_STATUS6,
	VDC_STATUS7,
	VDC_STATUS8                     = -1,
	VDC_STATUS9                     = -2,
	VDC_STATUS10                    = -3,
	VDC_STATUS_DEAD                 = -4,
	VDC_STATUS_UNKNOWN_ERROR        = -0x100
} vdc_status_t;


typedef struct vdc_handle {
	/* 读寄存器*/
	unsigned int (*readreg)(struct vdc_handle *, vdc_regno_t);
	/* 写寄存器, 状态寄存器(reg1)由驱动统一管理, 不能写, 返回-1; */
	int (*writereg)(struct vdc_handle *,
			vdc_regno_t, const unsigned int);
	/* 启动解码, 返回-1，表示VDC状态错误，不能启动; */
	int (*run)(struct vdc_handle *);
	/* 查询VDC状态，不阻塞版本 */
	int (*query)(struct vdc_handle *, vdc_status_t *);
	/* 查询VDC状态, 阻塞版本, 直到VDC_STATUS_DEAD或者VDC中断产生,
		返回值见VDC_Status_t */
	int (*query_timeout)(struct vdc_handle *, vdc_status_t *);
	/* 将状态转为idle */
	int (*reset)(struct vdc_handle *);
} vdc_handle_t;


vdc_handle_t *vdc_gethandle(void);

void vdc_freehandle(vdc_handle_t *);

void vdc_enable_log(void);

void vdc_disable_log(void);

void vdc_dump_info(void);

void vdc_hw_setmulti(unsigned int value);

#ifdef __cplusplus
}
#endif

#endif

