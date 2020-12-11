#include "tts_ext.h"
#include "lombo_system_sound.h"

#define LOG_TAG		"tts"
#include <log/log.h>

int main(int argc, char *argv[])
{
	char word[] = {0xef, 0xbb, 0xbf, 0xe6, 0xbd, 0x98, 0xe4, 0xbc, 0x9f, 0xe9, 0x9b, 0x84};/*潘伟雄*/
	//char word[] = {0xef, 0xbb, 0xbf, 0xe6, 0xbd, 0x98, 0xe4, 0xbc, 0x9f, 0xe9, 0x9b, 0x84,
	//				0xe6, 0x89, 0x93, 0xe5, 0x8d, 0xa1, 0xe6, 0x88, 0x90, 0xe5, 0x8a, 0x9f};/*潘伟雄打卡成功*/

	char *buf;
	int out_size, ret;
	void *hdl;
	FILE *fp = NULL;
	int count = 0;
	greet_param_t greet_param;

	ALOGD("tts test start");

	hdl = tts_init();
	if (!hdl) {
		ALOGD("tts init failed");
		return -1;
	}

	buf = malloc(1024 * 150);
	if (!buf) {
		ALOGE("malloc failed");
		return -1;
	}

	greet_param.pos = (argc >= 3 ? atoi(argv[1]): GREETING_END);
	greet_param.index = (argc >= 3 ? atoi(argv[2]): GREET_WORK);
	ALOGD("greet_param.pos: %d", greet_param.pos);
	ALOGD("greet_param.pos: %d", greet_param.index);
	tts_getWavData(hdl, word, buf, &out_size, 1024 * 150, &greet_param);
	ALOGD("tts_getWavData out size:%d", out_size);

	fp = fopen("/data/test_tts.wav", "ab");
	count = fwrite(buf, 1, out_size, fp);
	fclose(fp);
	ALOGD(" write size:%d", count);

	ret = lombo_system_play_wav(buf, out_size);
	if (ret) {
		ALOGE("lombo system play wav failed");
		return -1;
	}

	ALOGD("tts test done");
	free(buf);

	return 0;
}

