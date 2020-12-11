#include "net_wired.h"
#include "net_wifi.h"
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#define LOG_TAG "web_biz_api"
#include <log/log.h>

typedef struct web_wifi{
	wifi_event_cb cb;
	int req;
	int expected;
	int resp;
	sem_t sem;
}web_wifi_t;
static web_wifi_t wifi = {0};



#define wifi_req_start(expect) web_wifi_init();\
								wifi.req = 0xffff; \
								wifi.resp = 0xffff;\
								wifi.expected = (expect)

#define wifi_req_cancel() wifi.req = 0;

#define wifi_signal_req_done(event)   do {if ((wifi.req == 0xffff) && \
										(wifi.expected == event || wifi.expected == 0)) \
										{ \
											wifi.req = 0;\
											sem_post(&wifi.sem);\
										}\
										wifi.resp = event;\
										}while (0)
#define wifi_wait_req_done(res)  do {struct timespec tm= {0}; \
								struct timeval tt = {0}; \
								gettimeofday(&tt, NULL);\
								tm.tv_sec = tt.tv_sec + 50;\
								do{\
								res = sem_timedwait(&wifi.sem, &tm);\
									}while (res == EAGAIN);\
								if (res != 0)\
								ALOGW("wait wifi_sem error:%s", \
								strerror(errno));}while(0)



static void web_wifi_init(void);

static void web_wifi_event_cb(int event, int type)
{

	ALOGD("event=%d type=%d", event, type);
	wifi_signal_req_done(event);
}

static void web_wifi_init(void)
{
	if (!wifi.cb){
		ALOGD("web_wifi_init");
		wifi.cb = web_wifi_event_cb;
		sem_init(&wifi.sem, 0, 0);
		net_wifi_register_event_cb(wifi.cb);
	}
}

int web_set_wifi_state(int enable)
{
	wifi_config_para_t para = {0};
	int ret = 0;
	int result = 0;
	


	net_wifi_get_para(&para);

	if (para.enable_wifi == enable){
		result = 0;
		ALOGD("__func__ state is the same, do nothing");
	}else{
		memset(&para, 0, sizeof(para));
		para.enable_wifi = enable;
		
		wifi_req_start(0);
		ALOGD("send request to net");
		result = net_wifi_set_config_para(&para);
		ALOGD("send request to net %s", (result == 0)?"Ok":"error");
		ret |= result;
		if (0 != result){
			wifi_req_cancel();
		}else{
			wifi_wait_req_done(result);
			ret |= result;
		}
		
		
		
		if (ret){
			
		}else{
			result = -1;
			switch(wifi.resp){
			case wifi_closed:
			case wifi_close_successful:
				if (!enable){
					result = 0;
				}
				break;
			case wifi_open_successful:
			case wifi_scan_results:
				if (enable){
					result = 0;
				}
			break;
			default:
				break;
			}
			ALOGD("wifi enable=%d %s", 
					enable,
					(!result)?"ok":"failed");
		}
	}

	
	ret |= result;
	return ret;
}



SCAN_INFO * web_scan_wifi(void)
{
	int ret = 0;
	int result = 0;
	SCAN_INFO * scan_res = 0;

	wifi_req_start(0);
	ALOGD("send request to net");
	result = net_wifi_start_scan();
	ALOGD("send request to net %s", (result == 0)?"Ok":"error");
	ret |= result;
	if (0 != result){
		wifi_req_cancel();
	}else{
		wifi_wait_req_done(result);
	}

	
	ret |= result;
	if (ret){
		
	}else{
		result = -1;
		switch(wifi.resp){
		case wifi_scan_results:
			scan_res = net_wifi_get_scan_results();
			result = 0;
			break;
		break;
		default:
			break;
		}
		ALOGD("scan wifi %s", 
				(!result)?"ok":"failed");
	}
	
	ret |= result;
	return scan_res;
}


int web_connect_wifi(char *p_ssid, char *p_pwd, int security_type, int *failed_reason)
{
	int ret = 0;
	int result = 0;

	

	wifi_req_start(wifi_connect);
	ALOGD("send request to net");
	result = net_wifi_start_connect(p_ssid, p_pwd, security_type);
	ALOGD("send request to net %s", (result == 0)?"Ok":"error");
	if (0 != result){
		wifi_req_cancel();
	}else{
		wifi_wait_req_done(result);
	}

	ret |= result;

	if (ret){
		*failed_reason = ret;
	}else{
		result = -1;
		switch(wifi.resp){
		case wifi_connect:
			result = 0;
		if (failed_reason){
			*failed_reason = wifi.resp;
		}
			break;
		break;
		default:
			if (failed_reason){
				*failed_reason = wifi.resp;
			}
			break;
		}
		ALOGD("%s %s", 
				__func__,
				(!result)?"ok":"failed");
	}



	
	ret |= result;
	return ret;

}

