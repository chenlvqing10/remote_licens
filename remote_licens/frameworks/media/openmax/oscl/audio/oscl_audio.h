#ifndef __OSCL_AUDIO_H__
#define __OSCL_AUDIO_H__

typedef enum _oscl_pcm_stream {
	/** Playback stream */
	OSCL_PCM_STREAM_PLAYBACK = 0,
	/** Capture stream */
	OSCL_PCM_STREAM_CAPTURE,
	OSCL_PCM_STREAM_LAST = OSCL_PCM_STREAM_CAPTURE
} oscl_pcm_stream_t;

/* playback volume, range: 0 ~ 100 */
int oscl_set_play_volume(int volume);

/* record volume, range: 0 ~ 100 */
int oscl_set_record_volume(int volume);

#endif /* __OSCL_AUDIO_H__ */
