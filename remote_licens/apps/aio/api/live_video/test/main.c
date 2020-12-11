#include "live_server.h"
#include "live_video.h"
#include <unistd.h>

#define LOG_TAG "live_video"
#include <log/log.h>

#include "live_video_priv.h"

int live_video_set_para(int channel, 
								const live_video_para_t *para);
int live_video_get_para(int channel,
								live_video_para_t *para);

static void test_pause_resume(void)
{
	const int delay = 1000*50;
	live_video_logd("11111");
	usleep(delay);

	live_video_pause(0);
	live_video_pause(1);
	usleep(delay);
	live_video_logd("2222");

	live_video_resume(0);
	live_video_resume(1);
	usleep(delay);
	live_video_logd("3333");

	live_video_pause(0);
	live_video_resume(1);
	usleep(delay);
	live_video_logd("44444");

	live_video_pause(1);
	live_video_resume(0);
}

static void test_para(void)
{
	live_video_para_t para;
	static int cnt = 0;
	sleep(1);
	if (cnt%2 == 0){
		live_video_get_para(0, &para);
		para.width = 640;
		para.height = 480;
		live_video_set_para(0, &para);

		live_video_get_para(1, &para);
		para.width = 1280;
		para.height = 720;
		live_video_set_para(1, &para);
		
	}else{
		live_video_get_para(0, &para);
		para.width = 1280;
		para.height = 720;
		live_video_set_para(0, &para);

		live_video_get_para(1, &para);
		para.width = 640;
		para.height = 480;
		live_video_set_para(1, &para);
		

	}
	

	cnt++;
}
int main(int argc, const char **argv)
{
  live_video_init(0);
	//rtsp_server_init();
	sleep(2);
	while (1){ 
		//test_para();
		test_pause_resume();
	}
	return 0;
}
