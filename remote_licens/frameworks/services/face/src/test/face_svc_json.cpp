#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#include <thread>
#include <unistd.h>



using namespace rapidjson;


typedef Writer<StringBuffer> Writer_t ;


typedef struct WriterHandle{
	StringBuffer *stream;
	Writer_t *writer;
}WriterHandle_t;

__thread WriterHandle_t *hdl = 0;

#ifdef __cplusplus
extern "C" {
#endif 

void *face_svc_json_get()
{
	void *handle = 0;
	if (0 == hdl){
		hdl = (WriterHandle_t *)malloc(sizeof(WriterHandle_t));
		hdl->stream = new StringBuffer;
		hdl->writer = new Writer_t(*(hdl->stream));
		//printf("new hdl=%p\n", (void*)hdl);
	}
	else{
		//printf("old hdl=%p\n", (void*)hdl);
	}
	handle = (void *)hdl;
	return handle;
}

void face_svc_json_put(void *handle)
{
	
}

void face_svc_json_reset(void *handle)
{
	WriterHandle_t & hdl = *(WriterHandle_t *)handle;
	hdl.stream->Clear();
	hdl.writer->Reset(*(hdl.stream));
}

void face_svc_json_start(void *handle)
{
	WriterHandle_t & hdl = *(WriterHandle_t *)handle;
	hdl.writer->StartObject();
}

void face_svc_json_end(void *handle)
{
	WriterHandle_t & hdl = *(WriterHandle_t *)handle;
	hdl.writer->EndObject();
}

void face_svc_json_char(void *handle, const char *key, char value)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);
	writer.Key(key);
	writer.Int(value);	
}

void face_svc_json_int(void *handle, const char *key, int value)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);
	writer.Key(key);
	writer.Int(value);	
}


void face_svc_json_uint(void *handle, const char *key, unsigned int value)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);
	writer.Key(key);
	writer.Uint(value);	
}

void face_svc_json_uint64(void *handle, const char *key, unsigned long long value)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);
	writer.Key(key);
	writer.Uint64(value);	
}


void face_svc_json_double(void *handle, const char *key, int value)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);
	writer.Key(key);
	writer.Double(value);	
}

void face_svc_json_string(void *handle, const char *key, const char *str)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);

	writer.Key(key);
	writer.String(str);	
}

void face_svc_json_array_char(
	void *handle, const char *key, char arr[], unsigned int sz)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);

	unsigned int i = 0;
	writer.Key(key);	
	writer.StartArray();
	for (i = 0;i < sz; i++)
	{
		writer.Int(arr[i]);
	}
	writer.EndArray();
}


void face_svc_json_array_int(
	void *handle, const char *key, int arr[], unsigned int sz)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);

	unsigned int i = 0;
	writer.Key(key);	
	writer.StartArray();
	for (i = 0;i < sz; i++)
	{
		writer.Int(arr[i]);
	}
	writer.EndArray();
}

void face_svc_json_array_float(
	void *handle, const char *key, float arr[], unsigned int sz)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);

	unsigned int i = 0;
	writer.Key(key);	
	writer.StartArray();
	for (i = 0;i < sz; i++)
	{
		writer.Double(arr[i]);
	}
	writer.EndArray();
}


void face_svc_json_array_double(
	void *handle, const char *key, double arr[], unsigned int sz)
{
	Writer_t & writer = *(((WriterHandle_t *)handle)->writer);

	unsigned int i = 0;
	writer.Key(key);	
	writer.StartArray();
	for (i = 0;i < sz; i++)
	{
		writer.Double(arr[i]);
	}
	writer.EndArray();
}

const char *face_svc_json_get_string(void *handle)
{
	return  ((WriterHandle_t *)handle)->stream->GetString();
}


unsigned int face_svc_json_get_size(void *handle)
{
	return  ((WriterHandle_t *)handle)->stream->GetSize();
}

void test_api(int id)
{
	printf("*******%s-%d****\n", 
		__func__,
		(int)id);
	int array_int[3] = {1,2,3};
	double array_double[3] = {1.1, 2.2, 3.3};
	void *handle = face_svc_json_get();
	face_svc_json_reset(handle);
	
	face_svc_json_start(handle);
	face_svc_json_int(handle, "tid", (int)id);
	face_svc_json_double(handle, "double", 3.14);
	face_svc_json_int(handle, "int", 100);
	face_svc_json_string(handle, "string", "hello");
	face_svc_json_array_int(handle, "array_int", array_int, 3);
	face_svc_json_array_double(handle, "array_double", array_double, 3);
	face_svc_json_end(handle);
	printf("%s !!! %d\n", face_svc_json_get_string(handle), 
		face_svc_json_get_size(handle));
	


	

	face_svc_json_reset(handle);
	face_svc_json_start(handle);
	face_svc_json_string(handle, "after_reset", "hello");
	face_svc_json_end(handle);
	printf("%s !!! %d\n", face_svc_json_get_string(handle), 
		face_svc_json_get_size(handle));
}

void test_smp(void)
{
	test_api(20);
	test_api(20);

	std::thread t1(test_api, 1);
	std::thread t2(test_api, 2);
	std::thread t3(test_api, 3);
	t1.join();
	t2.join();
	t3.join();
}

void testjson(void)
{

	StringBuffer Stream;
	Writer<StringBuffer> writer(Stream);

	writer.StartObject();
	writer.Key("time");
	writer.Int(30);
	writer.Key("recg_event");
	writer.StartObject();
	writer.Key("f1");
	writer.StartArray();
	writer.Int(10);
	writer.Int(11);
	writer.Int(12);
	writer.EndArray();
	writer.Key("f2");
	writer.Double(3.14);
	writer.EndObject();
	writer.EndObject();

	printf("json:%s  !!! %d\n", 
		Stream.GetString(),
		Stream.GetSize());

	test_smp();

	//sleep(1000);
}
#ifdef __cplusplus
};
#endif


