#ifndef _JSON_H_
#define _JSON_H_
#ifdef __cplusplus
extern "C" {
#endif 

void testjson(void);



void *face_svc_json_get();
void face_svc_json_put(void *handle);
void face_svc_json_reset(void *handle);
void face_svc_json_start(void *handle);
void face_svc_json_end(void *handle);
void face_svc_json_char(void *handle, const char *key, char value);
void face_svc_json_int(void *handle, const char *key, int value);
void face_svc_json_uint(void *handle, const char *key, unsigned int value);
void face_svc_json_uint64(void *handle, const char *key, unsigned long long value);

void face_svc_json_double(void *handle, const char *key, int value);
void face_svc_json_string(void *handle, const char *key, const char *str);
void face_svc_json_array_char(
	void *handle, const char *key, char arr[], unsigned int sz);
void face_svc_json_array_int(
	void *handle, const char *key, int arr[], unsigned int sz);
void face_svc_json_array_float(
	void *handle, const char *key, float arr[], unsigned int sz);
void face_svc_json_array_double(
	void *handle, const char *key, double arr[], unsigned int sz);
const char *face_svc_json_get_string(void *handle);
unsigned int face_svc_json_get_size(void *handle);

#ifdef __cplusplus
};
#endif
#endif /*_JSON_H_*/

