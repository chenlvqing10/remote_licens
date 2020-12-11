#ifndef __DEMUXER_PLUGIN_H__
#define __DEMUXER_PLUGIN_H__
#include <stdlib.h>
#include <stdint.h>

#include "av_media_type.h"

#define DMX_MAX_VIDEO_STREAM	4
#define DMX_MAX_AUDIO_STREAM	32
#define DMX_MAX_SUB_STREAM	32
typedef enum dmx_error_type {
	DMX_ERRNO_UNDEFINED = -1,
	DMX_ERRNO_NONE = 0,
	DMX_ERRNO_NOBUF = 1, /* Run out of buffer */
} dmx_error_type_e;

typedef enum dmx_cmd {
	DMX_CMD_INVALID = 0,
	DMX_CMD_BLOCKING, /* blocking state, 1: blocking, 0: no blocking */
} dmx_cmd_e;
typedef enum dmx_seek_dir {
	DMX_SEEKDIR_ANY = 0,
	DMX_SEEKDIR_FORWARD,
	DMX_SEEKDIR_BACKWARD,
} dmx_seek_dir_e;

typedef struct dmx_packet {
	uint8_t *vir_addr;
	unsigned long phy_addr;
	size_t data_size; /* data size in buffer */
	av_media_type_e media_type;
	int flags; /* A combination of AV_PACKET_FLAG values */
	int stream_index; /* index in all stream */
	int64_t pts; /* timestamp in us */
	int64_t dts;
	int64_t duration; /* duration in us */
	void *reserve;
} dmx_packet_t;

typedef struct dmx_ainfo {
	av_codec_type_e codec_id;

	/* The average bitrate (in bits per second) */
	int bit_rate;

	int sample_rate; /* samples per second */
	int channels; /* number of audio channels */
	uint64_t channel_layout;
	int bits_per_coded_sample; /* bits per sample from the demuxer */

	/* Audio frame size, if known. Required by some formats to be static. */
	size_t frame_size;

	/* The number of bytes per coded audio frame, required by some formats */
	int block_align;

	void *extradata;
	size_t extradata_size;
	int stream_index; /* index in all stream */
	int64_t start_time; /* start time in us */
	int64_t duration; /* total duration in us */
	char *text;
	void *reserve;
} dmx_ainfo_t;

typedef struct dmx_vinfo {
	av_codec_type_e codec_id;
	int width;
	int height;
	void *extradata;
	size_t extradata_size;
	int stream_index; /* index in all stream */
	int64_t start_time; /* start time in us */
	int64_t duration; /* total duration in us */
	char *text;
	void *reserve;
} dmx_vinfo_t;

typedef struct dmx_subinfo {
	av_codec_type_e codec_id;
	void *extradata;
	size_t extradata_size;
	int stream_index; /* index in all stream */
	int64_t start_time; /* start time in us */
	int64_t duration; /* total duration in us */
	char *text;
	void *reserve;
} dmx_subinfo_t;

typedef struct dmx_mediainfo {
	int64_t duration; /* total duration in us */
	int seek_enable; /* if seekable flag, 0 unseekable, 1 seekable */
	av_media_content_e content;
	int64_t first_audio_time; /* first audio frame timestamp in us */
	int astream_num; /* total audio stream number */
	int vstream_num; /* total video stream number */
	int substream_num; /* total subtitle stream number */
	dmx_ainfo_t ainfo[DMX_MAX_AUDIO_STREAM]; /* audio stream infomation */
	dmx_vinfo_t vinfo[DMX_MAX_VIDEO_STREAM]; /* video stream infomation */
	dmx_subinfo_t subinfo[DMX_MAX_SUB_STREAM]; /* subtitle stream infomation */
	int default_astream_idx; /* default audio stream index */
	int default_vstream_idx; /* default video stream index */
	int default_substream_idx; /* default subtitle stream index */
	void *reserve;
} dmx_mediainfo_t;

typedef struct dmx_buffer_info {
	size_t video_buf_size; /* video stream total buffer size */
	int video_buf_align; /* video stream buffer align */
	size_t audio_buf_size; /* audio stream total buffer size */
	int audio_buf_align; /* audio stream buffer align */
	size_t sub_buf_size; /* subtitle stream total buffer size */
	int sub_buf_align; /* subtitle stream buffer align */
	void *reserve;
} dmx_buffer_info_t;

typedef struct dmx_plugin {
	/* Read the format header and initialize demuxer */
	void *(*read_header)(void *io_ctx,
		dmx_buffer_info_t *bufinfo,
		dmx_mediainfo_t *mediainfo);

	/* get one packet from demuxer. */
	int (*read_packet)(void *handle, dmx_packet_t *pkt);

	/* return packet to demuxer */
	int (*return_packet)(void *handle, dmx_packet_t *pkt);

	/* Close the stream and deinit demuxer */
	int (*read_close)(void *handle);

	/* Seek to a given timestamp in us */
	int (*read_seek)(void *handle, int64_t timestamp, dmx_seek_dir_e dir);

	/* reserve */
	int (*read_probe)(void *io_ctx);

	/* set parameter or send command to the demuxer */
	int (*read_ctrl)(void *handle, dmx_cmd_e cmd, void *arg);

	void *reserve;
} dmx_plugin_t;

typedef dmx_plugin_t *(*get_plugin_func)(void);

#endif /* __DEMUXER_PLUGIN_H__ */
