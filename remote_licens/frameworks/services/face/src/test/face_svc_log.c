#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <stdarg.h>

#define name_len 256
typedef struct log_ctl{
	int c;
	char name[name_len];
	FILE *f;
	pthread_mutex_t wlock;
}log_ctl_t;




const char *conf = "[formats]\n"
"simple = \"%m%n\" \n"
"[rules]\n"
"my_cat.*      $myoutput, \"\";simple\n";

const char *conf_path = "/data/face_nn_self_test_zlog.conf";
log_ctl_t ctrl = {0};
__thread unsigned char buffer[1024];


int wlock()
{
	return pthread_mutex_lock(&ctrl.wlock);
}

int wunlock(void)
{
	return pthread_mutex_unlock(&ctrl.wlock);
}

void face_log_init(void)
{
	ctrl.c = 1;
	ctrl.f = NULL;
	snprintf(ctrl.name, name_len, "./default.log");
	pthread_mutex_init(&ctrl.wlock, NULL);
}



void face_log_set_log_full_path(const char *full_path)
{
	if (0 == wlock()){
		ctrl.c = 1;
		snprintf(ctrl.name, name_len, "%s", full_path);
		if (1 == ctrl.c){
			if (NULL != ctrl.f){
				fclose(ctrl.f);
			}
			ctrl.f = fopen(ctrl.name, "w+");
			ctrl.c = 0;
		}
		wunlock();
	}

}

void face_svc_log(const char *format, ...)
{
	unsigned int sz = 0;
	unsigned int writed = 0;

	va_list args;

	va_start(args, format);
	sz = vsnprintf(buffer, 1024, format, args);
	va_end(args);
	buffer[sz] = '\n';
	buffer[sz+1] = '\0';



	if (NULL != ctrl.f){
		if (0 == wlock()){
			writed = fwrite(buffer, 1,sz + 1, ctrl.f);
			wunlock();
		}
	}
}



