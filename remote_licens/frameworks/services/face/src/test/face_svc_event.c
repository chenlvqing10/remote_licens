#include "face_svc_json.h"

#define LOG_TAG "face_nst"
#include <log/log.h>
#include "face_svc_log.h"



#define EventStart()  void *handle = face_svc_json_get();\
						face_svc_json_reset(handle);\
						face_svc_json_start(handle)
#define EventEnd()    face_svc_json_end(handle);\
						/*ALOGI("%s", face_svc_json_get_string(handle))*/face_svc_log("%s", face_svc_json_get_string(handle))

#define ToChar(key, value)   face_svc_json_char(handle, key, value)
#define ToInt(key, value)   face_svc_json_int(handle, key, value)
#define ToUint(key, value)   face_svc_json_uint(handle, key, value)
#define ToUint64(key, value)   face_svc_json_uint64(handle, key, value)

#define ToFloat(key, value)   face_svc_json_double(handle, key, value)
#define ToString(key, value)  face_svc_json_string(handle, key, value)

#define ToArrayChar(key, values, size) \
					face_svc_json_array_char(handle,key,values,size)
#define ToArrayInt(key, values, size) \
						face_svc_json_array_int(handle,key,values,size)
#define ToArrayFloat(key, values, size) \
						face_svc_json_array_float(handle,key,values,size)





void face_event_face_det_send(
	unsigned long long timestamp,
	float *head_pose_ptr,
	unsigned int head_pose_size,
	int score)
{
	EventStart();
	ToString("event", "face_det");
	ToUint64("timestamp", timestamp);
	ToArrayFloat("head_pose",head_pose_ptr,head_pose_size);
	ToInt("score", score);
	EventEnd();
}

void face_event_face_recg_send(
	unsigned long long timestamp,
	char *values_ptr,
	unsigned int values_size,
	float *norm_ptr,
	unsigned int norm_size,
	float score,
	unsigned int max_id)
{
	EventStart();
	ToString("event", "face_recg");
	ToUint64("timestamp", timestamp);
	ToArrayChar("values",values_ptr,values_size);
	ToArrayFloat("norm",norm_ptr,norm_size);
	ToFloat("score", score);
	ToUint("max_id", max_id);
	EventEnd();
}







