#ifndef __FACE_SVC_LOG__
#define __FACE_SVC_LOG__


void face_log_init(void);
void face_log_set_log_full_path(const char *full_path);
void face_svc_log(const char *format, ...);



#endif
