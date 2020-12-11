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
// C++ header

#ifndef _ADTS_AUDIO__SOURCE_HH
#define _ADTS_AUDIO__SOURCE_HH

#ifndef _FRAMED_SOURCE_HH
#include "FramedSource.hh"
#endif

#define FRAME_PER_SEC 30

class ADTSAudioSource: public FramedSource {
public:
	ADTSAudioSource(UsageEnvironment &env);
	~ADTSAudioSource(void);

	static void getNextFrame(void *ptr);
	void GetFrameData();
private:
	// redefined virtual functions:
	virtual void doGetNextFrame();

	void *m_pToken;

};

#endif
