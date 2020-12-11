#ifndef __VDECODER_H__
#define __VDECODER_H__

#include <sys/types.h>
#include <stdint.h>
#include <lombo_decplugin.h>


void *vdecoder_open(av_codec_type_e id, vdec_init_param_t *param, int *is_vdc);
int vdecoder_close(void *handle);
int vdecoder_get_info(void *handle, vdec_info_t *info);
int vdecoder_decode_frame(void *handle, vdec_packet_t *pkt);
int vdecoder_queue_buf(void *handle, vdec_frame_t *frame);
int vdecoder_dequeue_buf(void *handle, vdec_frame_t *frame, int timeout_ms);
int vdecoder_ctrl(void *handle, vdec_cmd_e cmd, void *param);
int vdecoder_clear(void *handle);

#endif /* __VDECODER_H__ */

