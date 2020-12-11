
    
#ifndef __FRAME_QUEUE_H__
#define __FRAME_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include "systype.h"






typedef enum 
{
    FRAME_TYPE_I_FRAME = 0,
    FRAME_TYPE_P_FRAME,
    FRAME_TYPE_B_FRAME,
}frame_type_e;



typedef	struct
{
	byte			byType;				
	frame_type_e	eSubType;
	dword 			dwTargetFrameRate;	
	dword 			dwFrameCounter;		
	dword 			dwChnId;			
	dword			dwLowerTimeStamp; 	
	dword			dwUpperTimeStamp; 	
	dword			dwDataLen; 			
	dword			dwWidth;            
	dword			dwHeight;            
	byte	*		pbyData;			
	byte 			byData[0];			
  	
}frame_dsc_s;


typedef	struct
{
	
}FRAME_QUEUE_INFO_S;


typedef enum
{
    FRAME_QUEUE_WAIT = 0,                     
    FRAME_QUEUE_NO_WAIT,
}frame_queue_wait_e;

sdword frame_queue_init();

sdword frame_queue_get_node_frame(dword dwFrameQueueNum, frame_dsc_s * pstFrameDsc, frame_queue_wait_e eFrameQueueWait);


sdword frame_queue_push_node(dword dwFrameQueueNum, frame_dsc_s * pstFrameDsc, dword dwTimeOut);

sbool frame_queue_is_empty(dword dwFrameQueueNum);

sdword frame_queue_flush(dword dwFrameQueueNum);


#ifdef __cplusplus
}
#endif 

#endif



