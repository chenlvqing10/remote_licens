#ifndef __OMX_MEDIAPLAYER_H__
#define __OMX_MEDIAPLAYER_H__

#include <stdint.h>
#include <media_common.h>

typedef enum omxmp_win_layer {
	OMXMP_LAYER_TOP = 0,
	OMXMP_LAYER_BOTTOM,
	OMXMP_LAYER_NONE = 0x7fff
} omxmp_win_layer_e;

typedef enum omxmp_rotate {
	OMXMP_ROTATE_NONE	= 0,
	OMXMP_ROTATE_90		= 1,
	OMXMP_ROTATE_180	= 2,
	OMXMP_ROTATE_270	= 3,
} omxmp_rotate_e;

typedef enum omx_disp_mode {
	/* Display in the window at the original size of the video,
	 * can't overflow the window */
	OMXMP_WINDOW_ORIGINAL,
	/* Scale to full screen by video ratio, the video show normal */
	OMXMP_WINDOW_FULL_SCREEN_VIDEO_RATIO,
	/* Scale to full screen by screen ratio, the video may be distorted */
	OMXMP_WINDOW_FULL_SCREEN_SCREEN_RATIO,
	/* Forced to display at 4:3 ratio, the video may be distorted */
	OMXMP_WINDOW_4R3MODE,
	/* Forced to display at 16:9 ratio, the video may be distorted */
	OMXMP_WINDOW_16R9MODE,
	/* Used to cut off the black side of the video */
	OMXMP_WINDOW_CUTEDGE,
	/* User defined mode */
	OMXMP_WINDOW_USERDEF,
} omx_disp_mode_e;

typedef enum omxmp_scale_mode {
	OMXMP_SCALE_MODE_ONLINE = 0,
	OMXMP_SCALE_MODE_OFFLINE,
} omxmp_scale_mode_e;

typedef enum omxmp_error {
	OMXMP_ERR_UNKNOW = 0,
	OMXMP_ERR_IO,
	OMXMP_ERR_UNSUPPORTED,
	OMXMP_ERR_TIMEOUT,
	OMXMP_ERR_FUNC,
	OMXMP_ERR_VIDEO_DEC,
	OMXMP_ERR_AUDIO_DEC,
	OMXMP_ERR_DEMUXER,
	OMXMP_ERR_SUB_FILE_IS_NOT_EXIST,
	OMXMP_ERR_SUB_FILE_NUM_BIGGER,
	OMXMP_ERR_SUB_DEC_NOT_SUPPORTED,
	OMXMP_ERR_SUB_FILE_IS_EXIST
} omxmp_error_e;

typedef struct omxmp_win {
	int   left;
	int   top;
	int   width;
	int   height;
} omxmp_win_t;

typedef struct omxmp_audio_info {
	int audio_stream_num;
	int cur_stream;
	int supported;
	audio_stream_info_t stream_info[16];
} omxmp_audio_info_t;

typedef struct omxmp_sub_stream {
	int stream_idx;
	int status;
	char *name;
} omxmp_sub_stream_t;

typedef struct omxmp_sub_info {
	int stream_num;
	omxmp_sub_stream_t sub_stream[64];
} omxmp_sub_info_t;

typedef struct omxmp_io_interface {
	int (*open)(const char *path, int flag);
	long (*read)(int fd, void *buf, unsigned long size);
	int64_t (*lseek)(int fd, int64_t offset, int whence);
	int (*close)(int fd);
} omxmp_io_interface_t;

typedef struct mp_callback_ops {
	void (*on_completion)(void *handle);
	void (*on_error)(void *handle, int omx_err);
	void (*on_prepared)(void *handle);
	void (*on_seek_complete)(void *handle);
	void (*on_video_size_changed)(void *handle, int width, int height);
	void (*on_timed_text)(void *handle, char *text);
	int (*on_get_sub_frame)(void *handle, frame_t *frame);
} mp_callback_ops_t;

void *omxmp_create(mp_callback_ops_t *cb_ops);

void omxmp_release(void *handle);

int omxmp_set_data_source(void *handle, const char *url);

int omxmp_prepare(void *handle);

int omxmp_start(void *handle);

int omxmp_pause(void *handle);

int omxmp_stop(void *handle);

int omxmp_reset(void *handle);

int omxmp_seek_to(void *handle, long msec);

int omxmp_get_state(void *handle);

/* return 0 if false, return 1 if true */
int omxmp_is_playing(void *handle);

/* current position of meida stream, unit: ms */
long omxmp_get_current_position(void *handle);

/* duration of meida stream, unit: ms */
long omxmp_get_duration(void *handle);

/* setloop, 0: not loop 1: loop */
void omxmp_set_loop(void *handle, int loop);

int omxmp_set_playback_rate(void *handle, int rate);

int omxmp_set_playback_volume(void *handle, int volume);

int omxmp_get_playback_volume(void *handle);

int omxmp_set_scaling_mode(void *handle, omx_disp_mode_e mode);

/**
 * set scale mode before omxmp_prepare
 */
int omxmp_set_scale_mode(void *handle, omxmp_scale_mode_e mode);

int omxmp_set_window(void *handle, omxmp_win_t *win);

int omxmp_set_rotation(void *handle, omxmp_rotate_e rot_mode);

int omxmp_enable_rotation(void *handle, int enable);

int omxmp_disable_display(void *handle);

/* 0: layer top, 1: layer bottom */
int omxmp_set_win_layer(void *handle, omxmp_win_layer_e layer);

int omxmp_set_audio_track(void *handle, int index);
int omxmp_get_audio_info(void *handle, omxmp_audio_info_t *audio_info);

/**
 * set stream parameters, must call this function before omxmp_prepare.
 *
 * @param player handle created by omxmp_create.
 * @param stream_info stream infomation.
 *
 * @return 0 on successful, otherwise error code..
 */
int omxmp_set_stream(void *handle, avstream_info_t *stream_info);

/**
 * fill stream data, must call this function after omxmp_set_stream.
 *
 * @param player handle created by omxmp_create.
 * @param data input stream data buffer.
 * @param size input stream data size.
 * @param type stream type, see stream_type_e, only need for muxer type OMXMP_MUXER_RAW.
 * @param timestamp buffer timestamp, only need for muxer type OMXMP_MUXER_RAW.
 *
 * @return 0 on successful, otherwise error code..
 */
int omxmp_fill_stream(void *handle, avstream_t *stream);

int omxmp_subtitle_enable(void *handle, int index);

int omxmp_subtitle_add_external_source(void *handle, const char *url);

int omxmp_subtitle_get_info(void *handle, omxmp_sub_info_t *sub_info);

int omxmp_subtitle_return_frame(frame_t *frame);

int omxmp_subtitle_disable(void *handle, int index);

/**
 * set io interface after omxmp_set_data_source and before omxmp_prepare
 */
int omxmp_set_io_interface(void *handle, omxmp_io_interface_t *intf);


#endif /* __OMX_MEDIAPLAYER_H__ */
