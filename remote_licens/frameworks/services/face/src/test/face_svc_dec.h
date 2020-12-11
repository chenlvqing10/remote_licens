#ifndef __OMX_MEDIAPLAYER_H__
#define __OMX_MEDIAPLAYER_H__
#include "face_svc.h"
#include "face_svc_media.h"
#include "face_svc_conf.h"
#include "video_stream.h"


typedef enum face_svc_dec_win_layer {
	FACE_SVC_DEC_LAYER_TOP = 0,
	FACE_SVC_DEC_LAYER_BOTTOM,
	FACE_SVC_DEC_LAYER_NONE = 0x7fff
} face_svc_dec_win_layer_e;

typedef enum face_svc_dec_rotate {
	FACE_SVC_DEC_ROTATE_NONE	= 0,
	FACE_SVC_DEC_ROTATE_90		= 1,
	FACE_SVC_DEC_ROTATE_180	= 2,
	FACE_SVC_DEC_ROTATE_270	= 3,
} face_svc_dec_rotate_e;

typedef enum omx_disp_mode {
	/* Display in the window at the original size of the video,
	 * can't overflow the window */
	FACE_SVC_DEC_WINDOW_ORIGINAL,
	/* Scale to full screen by video ratio, the video show normal */
	FACE_SVC_DEC_WINDOW_FULL_SCREEN_VIDEO_RATIO,
	/* Scale to full screen by screen ratio, the video may be distorted */
	FACE_SVC_DEC_WINDOW_FULL_SCREEN_SCREEN_RATIO,
	/* Forced to display at 4:3 ratio, the video may be distorted */
	FACE_SVC_DEC_WINDOW_4R3MODE,
	/* Forced to display at 16:9 ratio, the video may be distorted */
	FACE_SVC_DEC_WINDOW_16R9MODE,
	/* Used to cut off the black side of the video */
	FACE_SVC_DEC_WINDOW_CUTEDGE,
	/* User defined mode */
	FACE_SVC_DEC_WINDOW_USERDEF,
} omx_disp_mode_e;

typedef enum face_svc_dec_error {
	FACE_SVC_DEC_ERR_UNKNOW = 0,
	FACE_SVC_DEC_ERR_IO,
	FACE_SVC_DEC_ERR_UNSUPPORTED,
	FACE_SVC_DEC_ERR_TIMEOUT,
	FACE_SVC_DEC_ERR_FUNC,
	FACE_SVC_DEC_ERR_VIDEO_DEC,
	FACE_SVC_DEC_ERR_AUDIO_DEC,
	FACE_SVC_DEC_ERR_DEMUXER,
} face_svc_dec_error_e;

typedef struct face_svc_dec_win {
	int   left;
	int   top;
	int   width;
	int   height;
} face_svc_dec_win_t;


typedef struct face_svc_dec_audio_stream {
	int samplerate;
	int channels;
	int bits;
	char *text;
} face_svc_dec_audio_stream_t;

typedef struct face_svc_dec_audio_info {
	int audio_stream_num;
	int cur_stream;
	int supported;
	face_svc_dec_audio_stream_t audio_stream[16];
} face_svc_dec_audio_info_t;

typedef struct mp_callback_ops {
	void (*on_completion)(void *handle);
	void (*on_error)(void *handle, int omx_err);
	void (*on_prepared)(void *handle);
	void (*on_seek_complete)(void *handle);
	void (*on_video_size_changed)(void *handle, int width, int height);
	void (*on_timed_text)(void *handle, char *text);
} mp_callback_ops_t;

void *face_svc_dec_create(mp_callback_ops_t *cb_ops);

void face_svc_dec_release(void *handle);

int face_svc_dec_set_data_source(void *handle, const char *url);

int face_svc_dec_prepare(void *handle);

int face_svc_dec_start(void *handle);

int face_svc_dec_pause(void *handle);

int face_svc_dec_stop(void *handle);

int face_svc_dec_reset(void *handle);

int face_svc_dec_seek_to(void *handle, long msec);

int face_svc_dec_get_state(void *handle);

/* return 0 if false, return 1 if true */
int face_svc_dec_is_playing(void *handle);

/* current position of meida stream, unit: ms */
long face_svc_dec_get_current_position(void *handle);

/* duration of meida stream, unit: ms */
long face_svc_dec_get_duration(void *handle);

/* setloop, 0: not loop 1: loop */
void face_svc_dec_set_loop(void *handle, int loop);

int face_svc_dec_set_playback_rate(void *handle, int rate);

int face_svc_dec_set_playback_volume(void *handle, int volume);

int face_svc_dec_get_playback_volume(void *handle);

int face_svc_dec_set_scaling_mode(void *handle, omx_disp_mode_e mode);

int face_svc_dec_set_window(void *handle, face_svc_dec_win_t *win);

int face_svc_dec_set_rotation(void *handle, face_svc_dec_rotate_e rot_mode);

int face_svc_dec_enable_rotation(void *handle, int enable);

int face_svc_dec_disable_display(void *handle);

/* 0: layer top, 1: layer bottom */
int face_svc_dec_set_win_layer(void *handle, face_svc_dec_win_layer_e layer);

int face_svc_dec_set_audio_track(void *handle, int index);
int face_svc_dec_get_audio_info(void *handle, face_svc_dec_audio_info_t *audio_info);

int face_svc_dec_set_cb(void *hdl, 
	enum face_camera_id camera_id,
	void *p, p_face_frame_cb f_cb);
int face_svc_dec_rgb_frame_cb(void *p, face_frame_t *frame);
int face_svc_dec_nir_frame_cb(void *p, face_frame_t *frame);
int face_svc_dec_player_is_ready_for_next(void *handle);


#endif /* __OMX_MEDIAPLAYER_H__ */
