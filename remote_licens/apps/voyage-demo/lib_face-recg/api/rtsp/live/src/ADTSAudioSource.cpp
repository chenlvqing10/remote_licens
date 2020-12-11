/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2019 Live Networks, Inc.  All rights reserved.
// A source object for AAC audio files in ADTS format
// Implementation

#include "InputFile.hh"
#include <GroupsockHelper.hh>
#include "ADTSAudioSource.hh"
#include "live_buffer.h"
#include <execinfo.h>

ADTSAudioSource::ADTSAudioSource(UsageEnvironment &env) :
		FramedSource(env), m_pToken(0)
{

}

ADTSAudioSource::~ADTSAudioSource(void)
{
	envir().taskScheduler().unscheduleDelayedTask(m_pToken);
}

void ADTSAudioSource::doGetNextFrame()
{
//	printf("%s %d\n", __func__, __LINE__);
	/* 根据 fps，计算等待时间 */
	double delay = 1000.0 / (FRAME_PER_SEC * 4);  /* ms */
	int to_delay = delay * 1000;  /* us */

	m_pToken = envir().taskScheduler().scheduleDelayedTask(to_delay, getNextFrame,
		this);
}

void ADTSAudioSource::getNextFrame(void * ptr)
{
//	printf("%s %d\n", __func__, __LINE__);
	((ADTSAudioSource *)ptr)->GetFrameData();
}

void ADTSAudioSource::GetFrameData()
{
	fFrameSize = 0;

	gettimeofday(&fPresentationTime, 0);


	get_data_from_buf(fTo, &fFrameSize, STREAM_TYPE_AUDIO);

	if (fFrameSize > fMaxSize) {
		fNumTruncatedBytes = fFrameSize - fMaxSize;
		fFrameSize = fMaxSize;
	} else {
		fNumTruncatedBytes = 0;
	}
//	printf("%s %d fFrameSize %d\n", __func__, __LINE__, fFrameSize);
	afterGetting(this);
}

