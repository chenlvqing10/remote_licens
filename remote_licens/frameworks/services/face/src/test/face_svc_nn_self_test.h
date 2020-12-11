

typedef struct Eevnt {
	int event_id;
	int event_type;
	char *name;
	int values_sz;
	unsigned char values[0];
	
}Event_t;
#define EventBegin(name) { \
					Event_t  e_;\
					memset(&e_,0, sizeof(Event_t);\
					e_.name = name;\
					unsigned char vals[1024];\
					int event_idx = 0;
#define EventPutBytes(ptr, sz)  \
					memcpy(&vals[(event_idx], (sz));\
					event_idx += (sz);
					
#define 
#define EenvtEnd() }
