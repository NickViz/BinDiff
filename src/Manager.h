////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
//
// Copyright (c) 2010-2015 VorontSOFT
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Functions.h"

const size_t MAX_THREADS = 16;
const size_t MAX_UNITS = 128;
const size_t HEVENT = 0;
const size_t HMUTEX = 1;


struct SThreadState
{
	SThreadState() : units_(0), sequence_(0), free_(false), ready_(false), error_(false)
	{ 
		events_[HEVENT] = 0; events_[HMUTEX] = 0; 
	}

	char	data_[MAX_UNITS][BLOCK_SIZE];	// Data to compute in the thread. More than one 4k block.
	size_t	units_;							// Amount of blocks. Can be less than MAX_UNITS at the end of the file.
	CHash	hashes_[MAX_UNITS];

	size_t	sequence_;						// Global sequence number. To order outbuf later.

	HANDLE	events_[2];						// Exit event and mutex handles.
	volatile bool free_;					// Flag that thread is available for work.
	volatile bool ready_;					// Flag that thread has finished his work.
	volatile bool error_;					// Flag that thread encountered an error.

	HANDLE	thread_;						// Thread handle
	DWORD	threadID_;						// Thread ID

	void HashComputeProc();					// Class aware thread proc
};


class CManager
{
public:
	CManager(size_t nMaxThreads = 0);
	~CManager(void);

	HANDLE GetMutex() const { return states_[0].events_[HMUTEX]; }

	size_t GetFreeThread();
	size_t GetCompletedThread(size_t sequence);

	size_t& units() { return states_[workThread_].units_; }
	size_t& sequence() { return states_[workThread_].sequence_; }
	void* block(size_t unit) { return states_[workThread_].data_ + unit; }

	const CHash& hash(size_t unit) const { return states_[workThread_].hashes_[unit]; }

	void startThread() { states_[workThread_].free_ = false; states_[workThread_].ready_ = false; }
	void freeThread()  { states_[workThread_].free_ = true;  states_[workThread_].ready_ = false; }

protected:
	SThreadState	states_[MAX_THREADS];
	size_t			threads_;

	size_t			workThread_;

private:
static void __cdecl managerThreadProc(void* parg);	// Thread proc
};
