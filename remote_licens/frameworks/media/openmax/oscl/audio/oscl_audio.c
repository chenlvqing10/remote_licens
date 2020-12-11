#include <alsa/asoundlib.h>
#include "oscl.h"
#include "oscl_audio.h"

int oscl_set_play_volume(int volume)
{
	int ret = 0;
	snd_mixer_t *handle = NULL;
	snd_mixer_elem_t *elem = NULL;
	snd_mixer_selem_id_t *sid;
	snd_mixer_selem_id_alloca(&sid);

	ret = snd_mixer_open(&handle, 0);
	if (ret < 0) {
		OSCL_LOGE("snd_mixer_open err %d\n", ret);
		return ret;
	}
	ret = snd_mixer_attach(handle, "default");
	if (ret < 0) {
		OSCL_LOGE("snd_mixer_attach err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_selem_register(handle, NULL, NULL);
	if (ret < 0) {
		OSCL_LOGE("snd_mixer_selem_register err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_load(handle);
	if (ret < 0) {
		OSCL_LOGE("snd_mixer_load err %d\n", ret);
		goto EXIT;
	}
	snd_mixer_selem_id_set_name(sid, "Soft");
	snd_mixer_selem_id_set_index(sid, 0);
	elem = snd_mixer_find_selem(handle, sid);
	if (!elem) {
		OSCL_LOGE("snd_mixer_find_selem err\n");
		goto EXIT;
	}
	ret = snd_mixer_selem_set_playback_volume(elem,
			SND_MIXER_SCHN_FRONT_LEFT, volume);
	if (ret < 0) {
		OSCL_LOGE("set left channel volume err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_selem_set_playback_volume(elem,
			SND_MIXER_SCHN_FRONT_RIGHT, volume);
	if (ret < 0) {
		OSCL_LOGE("set right channel volume err %d\n", ret);
		goto EXIT;
	}
EXIT:
	if (handle) {
		snd_mixer_close(handle);
		handle = NULL;
	}
	return ret;
}

int oscl_set_record_volume(int volume)
{
	int ret = 0;
	snd_mixer_t *handle = NULL;
	snd_mixer_elem_t *elem = NULL;
	snd_mixer_selem_id_t *sid;
	snd_mixer_selem_id_alloca(&sid);

	ret = snd_mixer_open(&handle, 0);
	if (ret < 0) {
		OSCL_LOGE("snd_mixer_open err %d\n", ret);
		return ret;
	}
	ret = snd_mixer_attach(handle, "default");
	if (ret < 0) {
		OSCL_LOGE("snd_mixer_attach err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_selem_register(handle, NULL, NULL);
	if (ret < 0) {
		OSCL_LOGE("snd_mixer_selem_register err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_load(handle);
	if (ret < 0) {
		OSCL_LOGE("snd_mixer_load err %d\n", ret);
		goto EXIT;
	}
	snd_mixer_selem_id_set_name(sid, "Soft");
	snd_mixer_selem_id_set_index(sid, 0);
	elem = snd_mixer_find_selem(handle, sid);
	if (!elem) {
		OSCL_LOGE("snd_mixer_find_selem\n");
		goto EXIT;
	}
	ret = snd_mixer_selem_set_capture_volume(elem,
			SND_MIXER_SCHN_FRONT_LEFT, volume);
	if (ret < 0) {
		OSCL_LOGE("set left channel volume err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_selem_set_capture_volume(elem,
			SND_MIXER_SCHN_FRONT_RIGHT, volume);
	if (ret < 0) {
		OSCL_LOGE("set right channel volume err %d\n", ret);
		goto EXIT;
	}
EXIT:
	if (handle) {
		snd_mixer_close(handle);
		handle = NULL;
	}
	return ret;
}

