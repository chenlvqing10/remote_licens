#include "H264VideoSource.hh"
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#endif

#include "live_buffer.h"

H264VideoSource::H264VideoSource(UsageEnvironment & env) : 
FramedSource(env),
m_pToken(0)
{

}

H264VideoSource::~H264VideoSource(void)
{
	envir().taskScheduler().unscheduleDelayedTask(m_pToken);

	printf("[MEDIA SERVER] rtsp connection closed\n");
}

void H264VideoSource::doGetNextFrame()
{
	/* 根据 fps，计算等待时间 */
	double delay = 1000.0 / (FRAME_PER_SEC * 2);  /* ms */
	int to_delay = delay * 1000;  /* us */
 
	m_pToken = envir().taskScheduler().scheduleDelayedTask(to_delay, getNextFrame,
		this);
}

unsigned int H264VideoSource::maxFrameSize() const
{
	return 1024*600;
}

void H264VideoSource::getNextFrame(void * ptr)
{
	((H264VideoSource *)ptr)->GetFrameData();
}

void H264VideoSource::GetFrameData()
{
	fFrameSize = 0;

	gettimeofday(&fPresentationTime, 0);

	/* fill frame data */
	/* memcpy(fTo,m_pFrameBuffer,fFrameSize); */

	get_data_from_buf(fTo, &fFrameSize, STREAM_TYPE_VIDEO);

	if (fFrameSize > fMaxSize) {
		fNumTruncatedBytes = fFrameSize - fMaxSize;
		fFrameSize = fMaxSize;
	} else {
		fNumTruncatedBytes = 0;
	}

	afterGetting(this);
}
