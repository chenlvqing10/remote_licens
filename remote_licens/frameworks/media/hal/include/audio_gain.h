
#define VOL_SCALE_SHIFT		16
#define VOL_SCALE_MASK          ((1 << VOL_SCALE_SHIFT) - 1)

#define PRESET_RESOLUTION	100
#define PRESET_MIN_DB		-24.0
#define ZERO_DB                  0.0
#define MAX_DB_UPPER_LIMIT      6.0

void *audio_gain_open(unsigned int channel, unsigned int bit_per_sample);

/* volume : 0 - 100 */
void audio_gain_set_volume(void *gain, unsigned int volume);

void audio_gain_close(void *gain);

int audio_gain_apply(void *gain, void *src, void *dst, int frames);

