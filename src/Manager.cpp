////////////////////////////////////////////////////////////////////////////////
// Author: Nikolai Vorontsov
// Manager.cpp : Multithreading manager
//
// Copyright (c) 2010-2020 VorontSOFT
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

#include "Manager.h"

CManager::CManager(size_t nMaxThreads) : threads_(nMaxThreads)
{
	if (threads_ == 0)	// Calculate amount automatically
	{
		SYSTEM_INFO SystemInfo;				// Get amount of working threads
		GetSystemInfo(&SystemInfo);
		threads_ = SystemInfo.dwNumberOfProcessors;
	}
	if (threads_ > MAX_THREADS)
		threads_ = MAX_THREADS;

	HANDLE mutex = CreateMutex(0, TRUE, 0);
	HANDLE event = CreateEvent(0, TRUE, FALSE, 0);

	if (mutex == 0  ||  event == 0)
		throw 0;

	for (size_t i = 0; i < threads_; i++)
	{
		states_[i].events_[HEVENT] = event;
		states_[i].events_[HMUTEX] = mutex;
		states_[i].thread_ = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)managerThreadProc, 
											states_ + i, 0, &states_[i].threadID_);
	}
	ReleaseMutex(mutex);					// Allow threads to run
}


CManager::~CManager(void)
{
	SetEvent(states_[0].events_[HEVENT]);	// Signal all threads to quit

	for (unsigned i = 0; i < 100; i++)		// 100 * 100ms = 10 second timeout
		for (size_t j = 0; j < threads_; j++)
			if (!states_[j].free_)			// Not released thread yet
			{
				Sleep(100);
				break;
			}
	
	CloseHandle(states_[0].events_[HMUTEX]);
	CloseHandle(states_[0].events_[HEVENT]);
}


size_t CManager::GetFreeThread()
{
	for (size_t i = 0; i < threads_; i++)
		if (states_[i].free_)
			return (workThread_ = i);
	return -1;
}


size_t CManager::GetCompletedThread(size_t sequence)
{
	for (size_t i = 0; i < threads_; i++)
		if (!states_[i].free_  &&  states_[i].ready_  &&  
			states_[i].sequence_ == sequence)
			return (workThread_ = i);
	return -1;
}


void __cdecl CManager::managerThreadProc(void* parg)
{
	if (parg) ((SThreadState*)parg)->HashComputeProc();
}


void SThreadState::HashComputeProc()
{
	free_  = true;
	ready_ = false;
	error_ = false;

	bool bRun = true;
	while (bRun)
	{
		DWORD dwRet = WaitForMultipleObjects(_countof(events_), events_, FALSE, INFINITE);
		switch (dwRet)
		{
		case WAIT_OBJECT_0:			// Exit event
			free_  = true;
			ready_ = false;
			bRun = false;
			break;

		case WAIT_OBJECT_0 + 1:		// Mutex released
			if (free_  ||  ready_)	// No data yet available or already proceessed
			{
				ReleaseMutex(events_[HMUTEX]);
				Sleep(0);
				continue;
			}

			ReleaseMutex(events_[HMUTEX]);
			for (size_t i = 0; i < units_; i++)	// Processing hashes
				hashes_[i] = ComputeHash(data_[i], BLOCK_SIZE);

			ready_ = true;
			continue;

		default:
			_tprintf(_T("Error! WaitForMultipleObjects failed. Error %d.\n"), GetLastError());
			free_  = true;
			ready_ = false;
			error_ = true;
			bRun = false;
			break;
		} // switch()
	} // while (bRun)

	ReleaseMutex(events_[HMUTEX]);
} // HashComputeProc()
