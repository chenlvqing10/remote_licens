#ifndef NNAAPI_H
#define NNAAPI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "./libnn-v2/img_type.h"

typedef enum
{
	FACE_FRONT,
	FACE_LEFT_30,
	FACE_LEFT_60,
	FACE_LEFT_90,
	FACE_RIGHT_30,
	FACE_RIGHT_60,
	FACE_RIGHT_90,
	FACE_ROTATE_LEFT_30,
	FACE_ROTATE_RIGHT_30,
	FACE_UP_30,
	FACE_DOWN_30,
} FACE_ORIENT_TYPE;


typedef struct
{
	int male;

	int wear_glasses;
	int wear_hat;

	int left_eye_cover;
	int right_eye_cover;
	int mouse_cover;
	/*0 - no mask
	  1 - mask,
	*/
	int mask;
} nna_face_attr_t;

/**
 * The information of a face from NN face detection.
 */
typedef struct nna_face {
	/**
	* Bounds of the face [left, top, right, bottom]
	* The left and top range is [0,2048)
	* The width and height cannot be 0 or negative.
	* This is supported by both hardware and software face detection.
	*/
	int rect[4];

	/**
	* The confidence level of the face. The range is 1 to 100. 100 is the
	* highest confidence. This is supported by both hardware and software
	* face detection.
	*/
	int score;

	/**
	* An unique id per face while the face is visible to the tracker. If
	* the face leaves the field-of-view and comes back, it will get a new
	* id. If the value is 0, id is not supported.
	*/
	int id;

	/**
	* The coordinates of key points of face as [left eye, right eye, nose, left mouth, right mouth]
	* The range is 0 to 2048, negative is not supported.
	*/
	float kpts[10];
	/*
	the rect after smooth
	*/
	int rect_smooth[4];
	/**
	* The flag of tracked face
	* 1 is tracked, otherwise untracked.
	*/
	int btracked;

	/**
	* The score of quality
	* The range is 0 to 100
	*/
	int quality_score;

	/*
	face orient type
	*/
	int orient;
	/*
	face top black
	lumia_top/lumia_down
	*/
	float top_black;
	/*
	face half exposure
	lumia_left/lumia_right
	*/
	float half_face;

	/*
	face key points drift
	*/
	float kpts_drift;
	/**
	* The reserved pointer
	*/
	/*head pose, yaw, pitch, roll*/
	float head_pose[3];

	/*face attributes*/
	nna_face_attr_t attr;


	/* yolo score */
	int yolo_score;
	int yolo_sequence_score;

	unsigned long rsvd;
} nna_face_t;


/**
* The information of memory handle
*/
typedef struct nna_mem_handle {
	unsigned long handle_t;
	int size;
	int used;
	unsigned long phyaddr;
	unsigned long viraddr;

	unsigned long uv_phyaddr;
	unsigned long uv_viraddr;

} nna_mem_handle_t;


/**
 * The information of faces from NN face detection.
 */
typedef struct nna_faces {
	/**
	* Face Rects pointer
	*/
	nna_face_t *face_rects;

	/**
	* Face Rects numbers
	*/
	int num;
} nna_faces_t;


/**
 * The information of cb info
 */
typedef struct nna_facecb_info {
	/**
	* Face Rects pointer
	*/
	nna_faces_t rects;
	/**
	* current timestamp
	*/
	int timestamp;
	/**
	* return buffer handle
	*/
	void *retbuf;
} nna_facecb_info_t;


typedef enum nna_image_type_t
{
	NNA_VIDEO,
	NNA_VIDEO_EMPTY,
	NNA_IMAGE,
}nna_image_type;

/**
 * The information of feature
 */
typedef struct nna_feature_map {
	/**
	* NN Feature format
	*/
	img_fmt fmt;
	int img_type;

	/**
	* NN Feature size
	*/
	//int batch_num;
	int width;
	int height;
	unsigned long stride1;
	unsigned long stride2;
	int channal;
	int timestamp;
	nna_mem_handle_t buf_hdl;
	int rsvd;
} nna_feature_map_t;

// typedef enum MODE{
// 	NORMAL,
// 	REGISTERING,
// }MODE;

/**
 * The information of face roi input buffer
 */
typedef struct nna_face_rib {
	/**
	* NN Feature map
	*/
	nna_feature_map_t ftmap;

	/**
	* NN face rois
	*/
	nna_face_t rois;
	unsigned long rsvd;
} nna_face_rib_t;


/**
 * The information of face feature
 */
typedef struct nna_face_feature {
	/**
	* NN Register Features and its norm
	*/
	signed char *features;
	float *faces_kpts;
	int features_num;
	float *features_norm;

	/**
	* NN Feature and its size
	*/
	signed char *feature_ref;
	float *face_kpts;
	int feature_size;
	int feature_cmp_size;
	int feature_part;
	/**
	* NN Register Feature and ref score and the max index
	*/
	float *score;
	int max_id;
} nna_face_feature_t;

/**
 * The information of Capability
 */
typedef enum nna_cap {
	NNA_FACEDET_CAP = 0x1,
	NNA_FACEREC_CAP = 0x2,
	NNA_FACEQTY_CAP = 0x4,
	NNA_FACETCK_CAP = 0x8,
	NNA_FACEDEPTH_CAP = 0x10,
} nna_cap_t;

/**
 * The information of req memory and cap
 */
typedef struct nna_reqcap_mem {
	unsigned long nna_cap;
	int szmem;
} nna_reqcap_mem_t;



/**
 * The information of facedet cfg
 */
typedef struct nna_face_cfg {
	int fdet_en;
	int ftracking_en;
	int frec_en;
	int fmask_rec_en;
	int fquality_en;
	int headpose_en;
	int faceattr_en;
	int facemask_en;
	int max_det_faces;
	int max_track_frames;
	int visual_track_thresh;
	int use_fast_track;
	int det_mode; /*normal 0, ipc 1*/
	int max_track_faces;
	int fdet_max_size;
	int fdet_min_size;
	int frame_width;
	int frame_height;
	int color_en;
	int yolo_en;
} nna_facedet_cfg_t;


typedef enum nna_status {
	NNA_STATUS_UNINIT = -1,
	NNA_STATUS_INITED,
	NNA_STATUS_ERR = 0x10,
	NNA_STATUS_BUSY,
	NNA_STATUS_IDLE,
	NNA_STATUS_STOPPED,
} nna_status;

typedef enum CB_CMD {
	CB_FACEDET_DONE,	/* nna_facecb_info_t */
	CB_DQBUF_DONE,		/* nna_feature_map_t */
	CB_FACEREC_DONE,	/* nna_face_feature_t */
	CB_FACETRK_DONE,	/* nna_facecb_info_t */
} CB_CMD;

//OMX_ERRORTYPE (*NNA_CB)(OMX_U32 hComponent, OMX_U32 pCmd, OMX_U32 pArg);
typedef enum RUN_MODE {
	NORMAL_MODE,
	BATCH_REGISTER_MODE,
	RUN_MODE_TOTAL,
} RUN_MODE;

typedef enum NNA_CMD {
	NNA_REQ_CAP = 0xa000, /* nna_cap_t */
	NNA_REQ_MEM, /* nna_reqmem_t */
	NNA_SET_USRMEM, /* nna_mem_handle */
	NNA_SET_INIT, /* none */
	NNA_SET_STOP, /* none */
	NNA_GET_STATUS, /* OMX_S32 */
	NNA_GET_DETSTATUS, /* OMX_S32 */
	NNA_GET_RECSTATUS, /* OMX_S32 */
	NNA_GET_DET_QUEUE,
	NNA_GET_DET_TRKMUTEX,

	NNA_SET_CALLBACK, /* nna_CB */

	NNA_SET_FACE_CFG, /* nna_facedet_cfg_t */
	NNA_SET_FACEDET_RUN_MODE, /* NORMAL_MODE,BATCH_REGISTER_MODE*/

	NNA_SET_FACEDET_RUN, /* nna_feature_map_t */
	NNA_SET_FACEREC_RUN, /* nna_face_rib_t */
	NNA_SET_FACETRK_RUN, /* nna_face_rib_t */

	NNA_SET_FACEDET_DATA,
	NNA_SET_FACEREC_DATA,
	NNA_SET_FACETRK_DATA,

	NNA_SET_FACEDET_STOP, /* nna_feature_map_t */
	NNA_SET_FACEREC_STOP, /* nna_face_rib_t */
	NNA_SET_FACEQTY_STOP,

	NNA_SET_FACEDET_TERMINATE, /* nna_feature_map_t */
	NNA_SET_FACEREC_TERMINATE, /* nna_face_rib_t */
	NNA_SET_FACEQTY_TERMINATE,


	NNA_SET_TRK_ENABLE,

	//NNA_SETJIT_FACEMT_RUN = 0xa100, /* nna_face_feature_t */
} NNA_CMD;


typedef enum ERR {
	NULL_NNA = -31,
	NULL_REG,
	NULL_DET,
	REQ_MEM_ERR,
	NOT_READY,
	NO_ERR = 0,
} CMD_RESULT;

typedef struct nna_fn_s{
	int (*nna_init)(void *hdl);
	int (*nna_cmd)(void *hdl, unsigned int cmd, void *param);
	int (*nna_deinit)(void *hdl);
	int (*nna_facematch)(void *hdl, nna_face_feature_t *fetscore);
}nna_fn_t;

void *lomboax_open(void *param);
int lomboax_close(void *hdl);

typedef int (*nna_event_cb)(void *comp, void *data, unsigned int event, void *event_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
