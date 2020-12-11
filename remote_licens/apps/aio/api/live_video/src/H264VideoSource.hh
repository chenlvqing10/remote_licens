#ifndef _H264VideoSource_H
#define _H264VideoSource_H

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "FramedSource.hh"

/* TODO */
#define FRAME_PER_SEC 30

class H264VideoSource : public FramedSource
{
public:
	H264VideoSource(UsageEnvironment & env, int channel = 0);
	~H264VideoSource(void);
public:
	virtual void doGetNextFrame();
	virtual unsigned int maxFrameSize() const;
	static void getNextFrame(void * ptr);
	void GetFrameData();
private:
	void *m_pToken;
	int channel_;
};

#endif /* _H264VideoSource_H */
